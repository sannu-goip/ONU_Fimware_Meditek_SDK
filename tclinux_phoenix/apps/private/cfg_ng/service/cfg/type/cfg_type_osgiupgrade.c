/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/


#include <stdio.h> 
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_msg.h"




#define MAXGET_PROFILE_SIZE 128
#define LAN_DHCP_TYPE	"type"

#define UPGRADE_JVM "/usr/script/osgi_upg.sh"
#define JAVA_VM_STATE_FILE "/usr/osgi/jvmboot"
#define JAVA_FRAMEWORK_STATE_FILE "/usr/osgi/frameworkset"
#define CTGWLIBS_STATE_FILE "/usr/osgi/ctgwlibset"
#define MOBILE_STATE_FILE "/usr/osgi/mobileset"
#define CTGWLIBS_CHECK_FILE "/tmp/ctsgw_test"
#define MOBILE_CHECK_FILE "/tmp/mobile_test"
#define UPGRADE_CHECK_FILE "/tmp/upgst_phone"
#define TAR_CHECK_FILE "/tmp/tarerr"

#define INFORM_ERR_BINERR		 "1"
#define INFORM_ERR_DLDERR		 "2"
#define INFORM_ERR_SPACEFULL	 "3"
#define INFORM_ERR_SAMEVERSION	 "4"

#define INFORM_RESULT_OK 	"0"
#define INFORM_RESULT_FAIL "1"

#define EVENT_INFORM_PATH		"/tmp/eventInform"

typedef struct pkt_buf {
	unsigned int length;
	unsigned char payload[];
} pkt_buf_t;


static int jvmRunCheck = 0;

/* get framework flag */
int getFrameworkflag()
{
	FILE *fp = NULL;
	char frameflag[8] = {0};
	int res = 0;

	fp = fopen(JAVA_FRAMEWORK_STATE_FILE, "r");
	if ( !fp )
		return 0;

	if ( (res = fread(frameflag, sizeof(char), sizeof(frameflag), fp)) <= 0 )
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);

	if ( 1 == atoi(frameflag) )
		return 1;
	else
		return 0;
}


/* get version profile */
int getVersion(int mode, int bootflag, char *ver)
{
	char path[128] = {0}, attrVal[64] = {0};
	char attrName[32] = "version=";

	if ( !ver )
		return -1;

	switch ( mode )
	{
		case 1 : /* OSGI framework */
			snprintf(path, sizeof(path), "/usr/osgi/framework%d/ver.txt", bootflag);
			break;
		case 2: /* JAVA machine */
			snprintf(path, sizeof(path), "/usr/osgi/jvm%d/ver.txt", bootflag);
			break;
		case 5: /* Mobile manager */
			snprintf(path, sizeof(path), "/usr/osgi/mobileapps%d/ver.txt", bootflag);
			break;
		case 6: /* Gateway Libs */
			snprintf(path, sizeof(path), "/usr/osgi/ctgwlibs%d/ver.txt", bootflag);
			break;
		default:
			return -1;
	}

	get_profile_str_new( attrName, attrVal, sizeof(attrVal), path);
	strcpy(ver, attrVal);	

	return 0;
}

int getJVMBootflag()
{
	FILE *fp = NULL;
	char bootflag[8] = {0};
	int res = 0;

	fp = fopen(JAVA_VM_STATE_FILE, "r");
	if ( !fp )
		return 0;

	if ( (res = fread(bootflag, sizeof(char), sizeof(bootflag), fp)) <= 0 )
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);

	if ( 1 == atoi(bootflag) )
		return 1;
	else
		return 0;
}

int getMobilesetflag()
{
	FILE *fp = NULL;
	char mobileflag[8] = {0};
	int res = 0;

	fp = fopen(MOBILE_STATE_FILE, "r");
	if ( !fp )
		return 0;

	if ( (res = fread(mobileflag, sizeof(char), sizeof(mobileflag), fp)) <= 0 )
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);

	if ( 1 == atoi(mobileflag) )
		return 1;
	else
		return 0;
}

int checkMobile()
{
	FILE *fp = NULL;
	char mobcontent[256] = {0};
	int res = 0;

	fp = fopen(MOBILE_CHECK_FILE, "r");
	if ( !fp )
		return -1;

	if ( (res = fread(mobcontent, sizeof(char), sizeof(mobcontent) - 1, fp)) <= 0 )
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);
	mobcontent[sizeof(mobcontent) - 1] = '\0';

	if ( NULL != strstr(mobcontent, "test ok") )
		return 0;

	return -1;
}

int checkFlashSpace()
{
	FILE *fp = NULL;
	char error[256] = {0};
	int res = 0;

	fp = fopen(TAR_CHECK_FILE, "r");
	if ( !fp )
		return -1;

	if ( (res = fread(error, sizeof(char), sizeof(error) - 1, fp)) <= 0 )
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);
	error[sizeof(error) - 1] = '\0';

	if ( NULL != strstr(error, "No space left on device") )
		return 1;

	return 0;
}


int setCTGWLibsflag(int flag)
{
	char libflag[8] = {0};

	snprintf(libflag, sizeof(libflag) - 1,
		"%d", flag);
	doValPut(CTGWLIBS_STATE_FILE, libflag);

	return 0;
}


int getCTGWLibsflag()
{
	FILE *fp = NULL;
	char libflag[8] = {0};
	int res = 0;

	fp = fopen(CTGWLIBS_STATE_FILE, "r");
	if ( !fp )
		return 0;

	if ( (res = fread(libflag, sizeof(char), sizeof(libflag), fp)) <= 0 )
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);

	if ( 1 == atoi(libflag) )
		return 1;
	else
		return 0;
}

int checkCTGWLib()
{
	FILE *fp = NULL;
	char libcontent[256] = {0};
	int res = 0;

	fp = fopen(CTGWLIBS_CHECK_FILE, "r");
	if ( !fp )
		return -1;

	if ( (res = fread(libcontent, sizeof(char), sizeof(libcontent) - 1, fp)) <= 0 )
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);
	libcontent[sizeof(libcontent) - 1] = '\0';
	if ( NULL != strstr(libcontent, "ctgw lib test ok") )
		return 0;

	return -1;
}

int setMobilesetflag(int flag)
{
	char mobileflag[8] = {0};

	snprintf(mobileflag, sizeof(mobileflag) - 1,
		"%d", flag);
	doValPut(MOBILE_STATE_FILE, mobileflag);

	return 0;
}

int svc_cfg_boot_osgiupgrade( void)
{
	char dhcpType[8] = {0};
	char pusview[12] = {0}, pushselect[12] = {0};
	int isDhcpActive = 0;
	char nodePath[64] = {0};
	char lan_nodeName[64] = {0};
	memset(lan_nodeName, 0, sizeof(lan_nodeName));
	snprintf(lan_nodeName, sizeof(lan_nodeName), LAN_DHCP_NODE);
	if(cfg_obj_get_object_attr(lan_nodeName, LAN_DHCP_TYPE, 0, dhcpType, sizeof(dhcpType)) > 0 && 0 == strcmp(dhcpType, "1"))
	{
		isDhcpActive = 1;
	}

	memset(nodePath, 0,  sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), OSGIUPGRADE_ENTRY_NODE);
	
	system("ebtables -N OSGIUPG_CHAIN");
	system("ebtables -P OSGIUPG_CHAIN RETURN");
	system("ebtables -I FORWARD -j OSGIUPG_CHAIN");
	system("ebtables -t filter -F OSGIUPG_CHAIN");
	system("ebtables -t filter -Z OSGIUPG_CHAIN");

	system("iptables -t nat -N OSGIUPG_CHAIN");
	system("iptables -t nat -A PREROUTING -j OSGIUPG_CHAIN");
	system("iptables -t nat -F OSGIUPG_CHAIN");
	system("iptables -t nat -Z OSGIUPG_CHAIN");

	memset(pusview, 0, sizeof(pusview));
	if(cfg_obj_get_object_attr(nodePath, "pushview", 0, pusview, sizeof(pusview)) > 0  && 0 == strcmp(pusview, "Yes"))
	{
		/* push webui mode */
		if(cfg_obj_get_object_attr(nodePath, "pushselected", 0, pushselect, sizeof(pushselect)) > 0  &&  0 == strcmp(pushselect, "unset"))  /* default set */
		{
			system("iptables -t nat -A OSGIUPG_CHAIN -i br+ -p tcp"
				" --dport 80 -j REDIRECT --to-ports 80");
			system("iptables -t nat -A OSGIUPG_CHAIN -p udp"
				" --dport 53  -j REDIRECT --to-ports 53");

			if ( isDhcpActive )
				system("ebtables -t filter -A OSGIUPG_CHAIN -p IPv4 "
						"--ip-proto udp --ip-dport 53 -j DROP");
		}
	}
	else
	{
		if(cfg_obj_get_object_attr(nodePath, "pushselected", 0, pushselect, sizeof(pushselect)) > 0  &&  0 == strcmp(pusview, "later"))
		{
			return 0; /* won't clear flags. */
		}	

		/* clear all flags */
		cfg_set_object_attr(nodePath, "cmd", "");
		cfg_set_object_attr(nodePath, "mode", "");
		cfg_set_object_attr( nodePath, "pushview", "");
		cfg_set_object_attr(nodePath, "pushselected", "");
		cfg_set_object_attr(nodePath, "pushviewstart", "");
		cfg_set_object_attr(nodePath, "ntpfail", "");
		cfg_set_object_attr(nodePath, "upgrade_ID", "");
		cfg_set_object_attr(nodePath, "fwver", "");
	}

	return 0;
}


int cfg_type_osgiupgrade_execute(char* path)
{
	char dhcpType[8] = {0};
	char cmdbuf[256] = {0};
	char mode[12] = {0}, pusview[12] = {0};
	char pushselect[12] = {0}, later_time_buf[16] ={0};
	int isDhcpActive = 0, currJVMFlag = 0, upgJVMFlag = 0;
	int currFrameFlag = 0, upgFrameFlag = 0;
	int currGWLibFlag = 0, upgGWLibFlag = 0;
	int currMobileFlag = 0, upgMobileFlag = 0;
	struct timeval later_time = {0};
	char upgrade_ID[128] = {0};
	char version[64] = {0}, preversion[64] = {0};
	char lanPath[64] = {0};
	char nodePath[64] = {0};
	char globalPath[64] = {0};
	char ntpServer[16] = {0};

	snprintf(lanPath, sizeof(lanPath), LAN_DHCP_NODE);
	snprintf(globalPath, sizeof(globalPath), GLOBALSTATE_NODE);
	if(cfg_obj_get_object_attr(lanPath, LAN_DHCP_TYPE, 0, dhcpType, sizeof(dhcpType)) > 0 && 0 == strcmp(dhcpType, "1"))
	{
		isDhcpActive = 1;
	}

	snprintf(nodePath, sizeof(nodePath), OSGIUPGRADE_ENTRY_NODE);
	cfg_obj_get_object_attr(nodePath, "upgrade_ID", 0, upgrade_ID, sizeof(upgrade_ID));

	if(cfg_obj_get_object_attr(nodePath, "pushview", 0, pusview, sizeof(pusview)) > 0 && 0 == strcmp(pusview, "Yes"))
	{
		/* push webui mode */
		if(cfg_obj_get_object_attr(nodePath, "pushselected", 0, pushselect, sizeof(pushselect)) > 0 && 0 != pushselect[0])
		{
			system("ebtables -t filter -F OSGIUPG_CHAIN");
			system("ebtables -t filter -Z OSGIUPG_CHAIN");
			system("iptables -t nat -F OSGIUPG_CHAIN");
			system("iptables -t nat -Z OSGIUPG_CHAIN");

			unlink("/etc/hosts");
			system("/usr/bin/killall -9 dnsmasq");
			system("/userfs/bin/dnsmasq &");

			/* default set */
			if ( 0 == strcmp(pushselect, "unset") )
			{
				system("iptables -t nat -A OSGIUPG_CHAIN -i br+ -p tcp"
					" --dport 80 -j REDIRECT --to-ports 80");
				system("iptables -t nat -A OSGIUPG_CHAIN -p udp"
					" --dport 53 -j REDIRECT --to-ports 53");

				if ( isDhcpActive )
					system("ebtables -t filter -A OSGIUPG_CHAIN -p IPv4 "
							"--ip-proto udp --ip-dport 53 -j DROP");
			}
			/* do upgrade */
			else if ( 0 == strcmp(pushselect, "upgrade") )
			{
				;
			}
			/* do not upgrade  */
			else if ( 0 == strcmp(pushselect, "ignore") )
			{
				/* clear all flags */
				cfg_set_object_attr(nodePath, "cmd", "" );
				cfg_set_object_attr(nodePath, "pushview", "" );
				/* clear dns hook */
				unlink("/etc/hosts");
				system("/usr/bin/killall -9 dnsmasq");
				system("/userfs/bin/dnsmasq &");
			}
			/* upgrade later  */
			else if ( 0 == strcmp(pushselect, "later") )
			{
				/* clear all flags */
				cfg_set_object_attr(nodePath, "pushview", "" );
				cfg_obj_get_object_attr(globalPath, "ntp_server_ret", 0, ntpServer, sizeof(ntpServer));
				/* save current times if ntp succeed*/
				if ( 0 == atoi(ntpServer) )
				{
					gettimeofday(&later_time, NULL);
					snprintf(later_time_buf, sizeof(later_time_buf) - 1, "%lu", later_time.tv_sec);
					cfg_set_object_attr(nodePath, "pushviewstart", later_time_buf );
					cfg_set_object_attr(nodePath, "ntpfail", "" );
				}
				/* will cancel it */
				else
				{
					printf("\n>>> upgrade cancel, ntp not succeed.\n");
					/* clear all flags */
					cfg_set_object_attr(nodePath, "cmd", "" );
					cfg_set_object_attr(nodePath, "pushview", "" );
					cfg_set_object_attr(nodePath, "ntpfail", "Yes" );
				}

				/* clear dns hook */
				unlink("/etc/hosts");
				system("/usr/bin/killall -9 dnsmasq");
				system("/userfs/bin/dnsmasq &");
			}
		}
	}
	else
	{
		if(cfg_obj_get_object_attr(nodePath, "mode", 0, mode, sizeof(mode)) > 0 && 0 != mode[0])
		{
			/* OSGI framework */
			if ( 0 == strcmp(mode, "1") )
			{
				/* 1. check current ver. */
				currFrameFlag = getFrameworkflag();
				/* 2. extract pack */
				jvmRunCheck = 1;
				upgFrameFlag = (1 == currFrameFlag) ? 0 : 1;
				/* 3. report error if download fail */
				if ( 0 != getDownloadstate() )
				{
					getVersion(1, currFrameFlag, version);
					sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_DLDERR, upgrade_ID);
					system("reboot -d 3 &");
					return SUCCESS;
				}
				snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s framework framework%d", UPGRADE_JVM, upgFrameFlag);
				system(cmdbuf);
			}
			/* JAVA machine */
			else if ( 0 == strcmp(mode, "2") )
			{
				/* 1. check current ver. */
				currJVMFlag = getJVMBootflag();
				/* 2. extract pack */
				jvmRunCheck = 1;
				upgJVMFlag = (1 == currJVMFlag) ? 0 : 1;
				/* 3. report error if download fail */
				if ( 0 != getDownloadstate() )
				{
					getVersion(2, currFrameFlag, version);
					sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_DLDERR, upgrade_ID);
					system("reboot -d 3 &");
					return SUCCESS;
				}
				snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s jvm jvm%d", UPGRADE_JVM, upgJVMFlag);
				system(cmdbuf);
			}
			/* Mobile manager */
			else if ( 0 == strcmp(mode, "5") )
			{
				/* 1. check current ver. */
				currMobileFlag = getMobilesetflag();
				/* 2. extract pack */
				upgMobileFlag = (1 == currMobileFlag) ? 0 : 1;
				/* 3. report error if download fail */
				if ( 0 != getDownloadstate() )
				{
					getVersion(5, currFrameFlag, version);
					sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_DLDERR, upgrade_ID);
					system("reboot -d 3 &");
					return SUCCESS;
				}
				snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s mobilechk mobileapps%d", UPGRADE_JVM, upgMobileFlag);
				system(cmdbuf);

				/* 4. check mobile test */
				if (  0 != checkMobile() )
				{
					/* check failed, delete old and no need to restore it. */
					snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s remove mobileapps%d", UPGRADE_JVM, upgMobileFlag);
					system(cmdbuf);

					getVersion(5, currMobileFlag, version);
					/* space full */
					if ( 1 == checkFlashSpace() )
					{
						sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_SPACEFULL, upgrade_ID);
					}
					else
					{
						sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_BINERR, upgrade_ID);
					}
				}
				else
				{
					/* check whether the versions are same. */
					getVersion(5, currMobileFlag, preversion);
					getVersion(5, upgMobileFlag, version);
					if ( 0 == strcmp(preversion, version) )
					{
						sendEventInform(INFORM_RESULT_FAIL, preversion, INFORM_ERR_SAMEVERSION, upgrade_ID);
					}
					else
					{
						/* send inform first, because mobile-manager will stop*/
						sendEventInform(INFORM_RESULT_OK, version, "", upgrade_ID);
						sleep(1);
						/* check ok , we will do ln */
						snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s mobileln mobileapps%d", UPGRADE_JVM, upgMobileFlag);
						system(cmdbuf);
						setMobilesetflag(upgMobileFlag);
					}
				}
				system("reboot -d 3 &");
			}
			/* Gateway Libs */
			else if ( 0 == strcmp(mode, "6") )
			{
				/* 1. check current ver. */
				currGWLibFlag = getCTGWLibsflag();
				/* 2. extract pack */
				upgGWLibFlag = (1 == currGWLibFlag) ? 0 : 1;
				/* 3. report error if download fail */
				if ( 0 != getDownloadstate() )
				{
					getVersion(6, currFrameFlag, version);
					sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_DLDERR, upgrade_ID);
					system("reboot -d 3 &");
					return SUCCESS;
				}
				
				snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s ctgw ctgwlibs%d", UPGRADE_JVM, upgGWLibFlag);
				system(cmdbuf);
				/* 4. check ctsgw test */
				if (  0 != checkCTGWLib() )
				{
					/* check failed, delete old then restore it. */
					snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s remove ctgwlibs%d", UPGRADE_JVM, upgGWLibFlag);
					system(cmdbuf);

					snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s restorectgw ctgwlibs%d", UPGRADE_JVM, currGWLibFlag);
					system(cmdbuf);

					getVersion(6, currGWLibFlag, version);
					/* space full */
					if ( 1 == checkFlashSpace() )
					{
						sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_SPACEFULL, upgrade_ID);
					}
					else
					{
						sendEventInform(INFORM_RESULT_FAIL, version, INFORM_ERR_BINERR, upgrade_ID);
					}
				}
				else
				{
					/* check whether the versions are same. */
					getVersion(6, currGWLibFlag, preversion);
					getVersion(6, upgGWLibFlag, version);
					if ( 0 == strcmp(preversion, version) )
					{
						/* same ver, then restore it. */
						snprintf(cmdbuf, sizeof(cmdbuf) - 1,"%s restorectgw ctgwlibs%d", UPGRADE_JVM, currGWLibFlag);
						system(cmdbuf);
						sendEventInform(INFORM_RESULT_FAIL, preversion, INFORM_ERR_SAMEVERSION, upgrade_ID);
					}
					else
					{
						setCTGWLibsflag(upgGWLibFlag);
						getVersion(6, upgGWLibFlag, version);
						sendEventInform(INFORM_RESULT_OK, version, "", upgrade_ID);
					}
				}
				system("reboot -d 3 &");
			}
			else
			{
				return SUCCESS;
			}
				
		}
	}

	return SUCCESS;
}

int cfg_type_osgiupgrade_func_commit(char* path)
{
	return cfg_type_osgiupgrade_execute(path);
}

static cfg_node_ops_t cfg_type_osgiupgrade_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_osgiupgrade_func_commit 
}; 


static cfg_node_type_t cfg_type_osgiupgrade_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_osgiupgrade, 
	 .ops = &cfg_type_osgiupgrade_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_osgiupgrade_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_osgiupgrade_func_commit 
}; 


static cfg_node_type_t* cfg_type_osgiupgrade_child[] = { 
	 &cfg_type_osgiupgrade_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_osgiupgrade = { 
	 .name = "OSGIUpgrade", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_osgiupgrade_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_osgiupgrade_child, 
	 .ops = &cfg_type_osgiupgrade_ops, 
}; 
