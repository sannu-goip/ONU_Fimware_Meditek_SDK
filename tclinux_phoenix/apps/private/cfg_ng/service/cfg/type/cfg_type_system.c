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
#include <cfg_cli.h> 
#include <sys/msg.h> 
#include "cfg_types.h" 
#include "cfg_romfile.h" 
#include <dirent.h>
#include <unistd.h>
#include <flash_layout/tc_partition.h>
#include <mxml.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#ifndef TCSUPPORT_UPGRADE_WDOGREBOOT
#include <linux/reboot.h>
#include <sys/syscall.h>
#endif
#include <pthread.h>
#include "cfg_msg.h" 
#include "utility.h"
#include <svchost_evt.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>

#ifdef WITHVOIP
#define VOIP        1 /*use WITHVOIP define to decide using voip function or not. shnwind 20100203*/
#else
#define VOIP        0
#endif


#if defined(TCSUPPORT_CRJO)
#include <stdbool.h>
#endif
#include "blapi_system.h"
#include "blapi_perform.h"

#define REBOOT_CMD				"/userfs/bin/mtd -r write %s romfile"
#define SKBMGR_CMD				"echo %s > /proc/net/skbmgr_hot_list_len"
#define SKBMGR_PATH				"/proc/net/skbmgr_hot_list_len"
#define DEVICEPARASTATIC_PATH	"/etc/deviceParaStatic.conf"
#define UPGRADE_CHECK_FILE 		"/tmp/upgst_phone"
#define UPLOAD_ROMFILE_PATH		"/var/tmp/up_romfile.cfg"
#define TCLINUX_PATH 			"/var/tmp/tclinux.bin"
#define MULTI_TO_NORMAL_TEMP 	"/tmp/parse_temp"
#define UPGRADE_FW_STATUS_FILE	"/tmp/upgrade_firmware"
#define MXML_DESCEND			1	/* Descend when finding/walking */


#define PRODUCTLINEPARAMAGIC 	0x12344321

#define MOBILE_ENTRY "mobile_Entry"
#define MOBILE_OSRIGHTENTRY "mobile_OSRight"

int TR69_NodeIdx = -1;
char TR69_VLANMODE[12] = {0}, TR69_VLANID[12] = {0}, TR69_PBIT[12] = {0};
int currEmptyPVC_index = -1, restore_TR69NodeIdx = -1;
#define LONGRESET_TR69_KEY1 	"VLANMode"
#if defined(TCSUPPORT_CT_E8B_ADSL)
#define LONGRESET_TR69_KEY2 	"VPI"
#define LONGRESET_TR69_KEY3 	"VCI"
#else
#define LONGRESET_TR69_KEY2 	"VLANID"
#define LONGRESET_TR69_KEY3 	"DOT1P"
#endif

int TR69_NodeIdxATM = -1, TR69_NodeIdxPTM = -1;
#define BOOT2DEFSETTR69			4
int SimCardResetFail = 0;
#define OP_ATTRIBUTE  			(1<<4)/*operate attribute */
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
#define OP_LONG_RESETBAK 		(1<<5)
#else
#define OP_LONG_RESETBAK 		OP_ATTRIBUTE
#endif
#define OP_AT_END 				(1<<0)	/*flag to show at the end of key parameter list*/
#define OP_NODE  				(1<<1)
#define OP_NODE_LEVEL_ALL  		(1<<2)/*all node information include sub node */
#define OP_NODE_LEVEL_SINGLE  	(1<<3)/*single node information*/
mxml_node_t *g_fsDefaultRomfileTree  = NULL;/*filesystem default romfile tree*/
#define OP_CWMP_ONLY  			(1<<7) /*key will be reserved via TR69 factory default.*/
#define OP_ATTRIBUTE_IGNORE_LIST	(1<<8) /* only support for OP_NODE_LEVEL_SINGLE.*/
#if defined(TCSUPPORT_CT_PON_JS)
#define OP_JSCWMP_ONLY 			OP_CWMP_ONLY
#define OP_JSAccount_ONLY 		OP_CWMP_ONLY
#else
#define OP_JSCWMP_ONLY 			OP_ATTRIBUTE
#define OP_JSAccount_ONLY 		OP_NODE
#endif
#define MAX_PVC_NUM				8
#define MAX_SKBMGR_LEN			512
#define NO_OPERATION  			0
#define NO_HEADER				1
#define HTML_HEADER				2
#if defined(TCSUPPORT_CT_PHONEAPP)
#define PHONE_NO_HEADER			3
#endif
#define CTCDBUS_NO_HEADER		4
#define NP_NO_HEADER			5
#if defined(TCSUPPORT_CT_JOYME4_JC)
#define USB_NO_HEADER			8
#endif
#define MAXGET_PROFILE_SIZE 	128
#define INFORM_ERR_BINERR		"1"
#define INFORM_ERR_DLDERR		"2"
#define INFORM_RESULT_FAIL 		"1"
#define INFORM_ERR_SAMEVERSION	"4"
#define UG_ROMFILE				1
#define UG_TCLINUX				4
#define UG_TCLINUX_ALLINONE		5
#define TRX_MAGIC2				0x32524448	/* "for tclinux" */
#define CHECKSUM_TEMP_BUF_SIZE 	4096
#define ROMFILE_SIZE 			0x10000
#define CKSUM_LENGTH 			4
#if defined(TCSUPPORT_CT_DUAL_IMAGE)
#if defined(TCSUPPORT_CT_PON) && defined(TCSUPPORT_RESERVEAREA_EXTEND)
#define TCLINUX_SIZE 			0x7B0000
#else
#define TCLINUX_SIZE 			0x780000
#endif
#else
#if defined(TCSUPPORT_CUC_DUAL_IMAGE)
#define TCLINUX_SIZE 			0x700000
#endif
#endif

typedef struct _Key_Parameters_Info_Enhance
{
	int flag;					  /*flag to judge if node operation or attribute operation*/	
	char *node;		/*node name-----for example:Cwmp_Entry*/
	char *nodenew;	/*node name-----for example:Cwmp_Entry*/
	char *attr;		/*attribute name----for example:acsUrl*/	
	int	(*funp)(struct _Key_Parameters_Info_Enhance* parainfo,mxml_node_t* oldtree,mxml_node_t* newtree);     /* handle function poiter*/
}Key_Parameters_Info_Enhance,*Key_Parameters_Info_Enhance_Ptr;

int add_node_if = 0;

mxml_node_t **gTreePtr = NULL;

typedef struct _ignore_list_
{
	char **attrlist;
	int nodelen;
}ignore_list, *pignore_list_ptr;


typedef enum __syslog{
	e_Reboot = 0,
	e_UpgradeFail,
	e_UpgradeSuccess,
	e_Factory,
}syslog_E;


void deRegBeforeReboot(void)
{
    int i = 0;
    char cmd[64] = {0};
    char tmp[32] = {0};
	int VOIPLineNum = 1;    
    char nodeName[64] = {0};
    
    
    memset(nodeName,0,sizeof(nodeName));	
    
    for(i=0 ; i < VOIPLineNum ; i++)
	{
        snprintf(nodeName, sizeof(nodeName), VOIPBASIC_ENTRY_NODE, i);
		cfg_obj_get_object_attr(nodeName, "Enable", 0, tmp, sizeof(tmp)); 
        if(tmp[0] != '\0')
		{
            memset(cmd, 0, sizeof(cmd));			
            if(strcmp(tmp, "Yes") == 0)
			{
                snprintf(cmd, sizeof(cmd), "/userfs/bin/sipclient -d %d", i);
                printf("Before reboot, deregister [cmd:%s]\n", cmd);
                system(cmd);
            }
        }
    }

    return;
}

static void _sys_delay_reboot(void)
{
    int delayRebootSec = 0;
    char cmd[64] = {0};
    char tmp[32] = {0};
    
    char nodeName[64] = {0};
    
    
    memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, VOIPBASIC_COMMON_NODE, sizeof(nodeName)-1);
	
#if defined(TCSUPPORT_ECN_SIP)
    deRegBeforeReboot();
#endif
	
	cfg_obj_get_object_attr(nodeName, "DelayRebootTime", 0, tmp, sizeof(tmp)); 
    if(tmp[0] != '\0')
	{
        delayRebootSec = strtoul(tmp, 0, 16);
    }
	else
        delayRebootSec = 3;
    
    if (delayRebootSec)
        snprintf(cmd, sizeof(cmd), "reboot -d %d &", delayRebootSec);
    else
        snprintf(cmd, sizeof(cmd), "reboot");

    printf("reboot [cmd:%s]\n", cmd);
    system(cmd);
}

#ifdef TCSUPPORT_BACKUPROMFILE
int sys_update_backup_romfile(char *romfilePath) 
{
	char cmd[128];

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), UPDATE_ROMFILE_CMD,romfilePath);	
	system(cmd);
	usleep(100000);

	snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD,(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	snprintf(cmd, sizeof(cmd), TC_FLASH_WRITE_CMD,romfilePath,(unsigned long)BACKUPROMFILE_RA_SIZE,(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(cmd);
	usleep(100000);

	return 0;
}
#endif

int backupTR69WanIndex(void)
{
	int valid_if_num = 0;
	int idx = 0;
	int pvc_index = 0;
	int entry_index = 0;
	int if_index = 0;
	int valid_if[MAX_WAN_IF_INDEX] = {0};
	char nodeName[64] = {0};
	char serviceList[64] = {0};

	/* important: save current to mtd*/
	cfg_evt_write_romfile_to_flash();

	valid_if_num = get_all_wan_index(valid_if, MAX_WAN_IF_INDEX);
	for( idx = 0; idx < valid_if_num; idx ++)
	{
		if_index = valid_if[idx];
		if ( if_index > MAX_WAN_IF_INDEX )
			return -1;
		
		pvc_index = if_index / MAX_SMUX_NUM + 1;
		entry_index = if_index % MAX_SMUX_NUM + 1;

		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);

		cfg_obj_get_object_attr(nodeName, "ServiceList", 0, serviceList, sizeof(serviceList)); 
		if( serviceList[0] != '\0' && NULL != strstr(serviceList, "TR069"))
		{
			TR69_NodeIdx = if_index;
#if (defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CT_E8B_ADSL)) || defined(TCSUPPORT_CUC)
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, pvc_index);
			memset(TR69_VLANMODE, 0, sizeof(TR69_VLANMODE));
			memset(TR69_VLANID, 0, sizeof(TR69_VLANID));
			memset(TR69_PBIT, 0, sizeof(TR69_PBIT));
#if defined(TCSUPPORT_CT_E8B_ADSL)
			if(cfg_obj_get_object_attr(nodeName, LONGRESET_TR69_KEY2, 0, TR69_VLANID, sizeof(TR69_VLANID)) < 0)
				memset(TR69_VLANID, 0, sizeof(TR69_VLANID));
			if(cfg_obj_get_object_attr(nodeName, LONGRESET_TR69_KEY3, 0, TR69_PBIT, sizeof(TR69_PBIT)) < 0)
				memset(TR69_PBIT, 0, sizeof(TR69_PBIT));
#else
			if(cfg_obj_get_object_attr(nodeName, LONGRESET_TR69_KEY1, 0, TR69_VLANMODE, sizeof(TR69_VLANMODE)) < 0)
				memset(TR69_VLANMODE, 0, sizeof(TR69_VLANMODE));
			if(cfg_obj_get_object_attr(nodeName, LONGRESET_TR69_KEY2, 0, TR69_VLANID, sizeof(TR69_VLANID)) < 0)
				memset(TR69_VLANID, 0, sizeof(TR69_VLANID));
			if(cfg_obj_get_object_attr(nodeName, LONGRESET_TR69_KEY3, 0, TR69_PBIT, sizeof(TR69_PBIT)) < 0)
				memset(TR69_PBIT, 0, sizeof(TR69_PBIT));
#endif
#endif
			break;
		}
	}
	return 0;
}

void simCardReset(int resetType)
{
	int ret = 1;
	if(resetType == BOOT2DEFSET)
	{
		printf("\r\nshort reset sim card rom");
		ret = system("/userfs/bin/simCard romreset 2");

	}
#if defined(TCSUPPORT_CT_LONG_RESETBTN)	
	else if(resetType == LONGRESET_BOOT2DEFSET)
	{
		printf("\r\nlong reset sim card rom");
		ret = system("/userfs/bin/simCard romreset 0");
	}
#endif
	else if(resetType == BOOT2DEFSETTR69)
	{
		printf("\r\nTR69 reset sim card rom");
		ret = system("/userfs/bin/simCard romreset 0");
	}
	if(ret != 0)
	{
		printf("\r\nreset sim card fail, will signal infrom");
		SimCardResetFail = 1;
	}
}

int restoreApiRight1(int mode)
{
#define MOBILE_RIGHT1PATH "/usr/osgi"
#define MOBILE_RIGHT1_DEF_PATH "/usr/osgi/def_api_right1"

	/* remove current api right1 */
	system("rm -f "MOBILE_RIGHT1PATH"/*_right1");

	/* restore api right1 */
	system("cp -f "MOBILE_RIGHT1_DEF_PATH"/* "MOBILE_RIGHT1PATH"/");

	return 0;
}

int pluginc_stop_it(char *pid_path)
{
	char pidval[40] = {0};
	int pid_i = 0;
	FILE *fp = NULL;

	if ( !pid_path )
		return -1;

	fp = fopen(pid_path,"r");
	if ( fp )
	{		
		fgets(pidval, sizeof(pidval), fp);
		fclose(fp);
	}
	else
		return -2;

	pid_i = atoi(pidval);
	if ( pid_i )
		kill(pid_i, SIGTERM);

	return 0;
}

/* restore plugin b/c to factory default. */
int restorePluginBC(int mode)
{
#define RESTORE_C_PIDS			"/usr/osgi/plugin-c/%s/pid"  /* pid for plugin c */
#define RESTORE_C_PATHS			"/usr/osgi/plugin-c" 		/* path for plugin c */
#define RESTORE_B_PATHS			"/usr/osgi/plugin-b" 		/* path for plugin b */
#define RESTORE_DEF_PATHS		"/usr/osgi/factory" 		/* path for factory */
#define RESTORE_COPY_FILE		"cp -f "RESTORE_DEF_PATHS"/%s /tmp/"
#define RESTORE_INSTALL_FILE	"/usr/osgi/plugin_install.sh %d %s %s %s"
#define BUNDLE_TYPE 0
#define PLUGIN_C_TYPE 5


	/* restore plugin C: 1->kill all plugins */
	DIR *dir;
	struct dirent *dirptr;
	int sizeLen = 0;
	char buffer[256] = {0}, appfolder[128] = {0};
	char version[64] = {0};

	dir = opendir(RESTORE_C_PATHS);
	if (NULL != dir)
	{
		while ((dirptr = readdir(dir)) != NULL)
		{
			if ((strcmp(dirptr->d_name, ".") == 0)
				|| (strcmp(dirptr->d_name, "..") == 0))
				continue;
			if (DT_DIR != dirptr->d_type) 
				continue;
			snprintf(buffer, sizeof(buffer) - 1, RESTORE_C_PIDS, dirptr->d_name);	
			pluginc_stop_it(buffer);
		}
		closedir(dir);
	}

	/* restore plugin B/C: 2->restore factory */
	system("rm -rf "RESTORE_C_PATHS"/*");
	system("rm -rf "RESTORE_B_PATHS"/*");

	dir = opendir(RESTORE_DEF_PATHS);
	if ( NULL != dir )
	{
		while ( (dirptr = readdir(dir)) != NULL )
		{
			if ((strcmp(dirptr->d_name, ".") == 0)
				|| (strcmp(dirptr->d_name, "..") == 0))
				continue;
			if ( DT_DIR == dirptr->d_type )
				continue;

			sizeLen = strlen(dirptr->d_name);
			if ( sizeLen < 5 ) /* *.jar, *.cpk */
				continue;

			bzero(appfolder, sizeof(appfolder));
			strncpy(appfolder, dirptr->d_name, (sizeLen - 4));

			if ( 0 == strcmp(dirptr->d_name + (sizeLen - 4), ".cpk") )
			{
				/* create cpk folder */
				snprintf(buffer, sizeof(buffer) - 1, "mkdir %s/%s",
							RESTORE_C_PATHS, appfolder);
				system(buffer);

				/* copy to tmp install folder */
				snprintf(buffer, sizeof(buffer) - 1, RESTORE_COPY_FILE,
					dirptr->d_name);
				system(buffer);

				/* install it */
				snprintf(buffer, sizeof(buffer) - 1,
					RESTORE_INSTALL_FILE,
					PLUGIN_C_TYPE,  appfolder,  "1.0.0", "Yes");
				system(buffer);
			}
			else if ( 0 == strcmp(dirptr->d_name + (sizeLen - 4), ".jar") )
			{
				/* create cpk folder */
				snprintf(buffer, sizeof(buffer) - 1, "mkdir %s/%s",
							RESTORE_B_PATHS, appfolder);
				system(buffer);

				/* copy to tmp install folder */
				snprintf(buffer, sizeof(buffer) - 1, RESTORE_COPY_FILE,
					dirptr->d_name);
				system(buffer);

				/* get version */
				snprintf(buffer, sizeof(buffer) - 1,
							RESTORE_DEF_PATHS"/%s.info", appfolder);
				snprintf(version, sizeof(version) - 1,
							"%s", "1.0.0");
				memset(version, 0, sizeof(version));
				get_profile_str_new( "version=", version, sizeof(version)
					, buffer);

				/* install it */
				snprintf(buffer, sizeof(buffer) - 1,
					RESTORE_INSTALL_FILE,
					BUNDLE_TYPE,  appfolder,  version, "Yes");
				system(buffer);
			}
		}
		closedir(dir);
	}


	return 0;
}

int restoreOSGIRight(int mode)
{
	switch ( mode )
	{
		case BOOT2DEFSETTR69: /* factory default with tr069 */
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
		case LONGRESET_BOOT2DEFSET: /* factory default with long reset button */
#endif
			restoreApiRight1(mode);
			restorePluginBC(mode);
			return 0;
		default:
			break;
	}

	return 0;
}

mxml_node_t *keypara_load_romfile(char *pathname)
{
	FILE *fp=NULL;
	mxml_node_t *tree=NULL;
	int ret = -1, res = 0;

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	int flag = -1;
	/*Load romfile  file*/
	flag = check_checksum(pathname,LOAD_ROMFILE_FILE_NAME, GZ_TMP_PATH);
	if(flag <0)
	{
		printf(ROMFILE_CHECKSUM_ERR_MSG);
		unlink(LOAD_ROMFILE_FILE_NAME);
		return NULL;
	}
	
	fp = fopen(LOAD_ROMFILE_FILE_NAME, "r");
#else
	fp = fopen(pathname, "r");
#endif

	if(fp == NULL)
	{
		printf("\r\nkeypara_load_romfile:open file %s fail!",pathname);
		return NULL;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_IGNORE_CALLBACK);	
	fclose(fp);
	
	ret = cfg_load_from_xml(tree);
	if(ret < 0)
	{	
		if (tree)
			res = mxmlRelease(tree);
	}
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	unlink(LOAD_ROMFILE_FILE_NAME);
#endif
	return tree;
}

int operateAttributePara(Key_Parameters_Info_Ptr parainfo,mxml_node_t* oldtree,mxml_node_t* newtree)
{
	char nodeName[4][32];
	mxml_node_t *parentNode = NULL;
	mxml_node_t *newCurNode = NULL;
	mxml_node_t *curNode = NULL;
	int i;

	if(!(parainfo->flag & OP_ATTRIBUTE))
	{
		printf("\r\noperateAttributePara:the function only do attribute operation,you use the wrong function ,please check it!!!");
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	splitName(parainfo->node, nodeName, ".");
	
	newCurNode = newtree;
	curNode = oldtree;
	for(i = 1; i < 4; i++)
	{
		if(strlen(nodeName[i])==0)
			break;
		curNode = mxmlFindElement(curNode, curNode, nodeName[i],
			NULL, NULL, MXML_DESCEND);
		if(!curNode)
		{
			printf("\r\noperateAttributePara:the node(%s) is not in running file(means no change)",parainfo->node);
			return 0;
		}
		
		parentNode = newCurNode;
		newCurNode = mxmlFindElement(newCurNode, newCurNode, nodeName[i],
			NULL, NULL, MXML_DESCEND);
		if(!newCurNode)
		{
			newCurNode = mxmlNewElement(parentNode,nodeName[i]);
			if(newCurNode == NULL )
			{  
				printf("\r\noperateAttributePara:create a new node(%s) failed",parainfo->node);
				return -1;
			}
		}
	}
	
	mxmlElementSetAttr(newCurNode, parainfo->attr, mxmlElementGetAttr(curNode,parainfo->attr));
	return 0;
	
}

Key_Parameters_Info defaultPara[]=
{
	/*follow is node parameters*/
	/*follow is attribute parameters*/	
	{OP_ATTRIBUTE,"root.WLan.Entry0","SSID",operateAttributePara},/*wlan ssid*/
	{OP_ATTRIBUTE,"root.WLan.Entry0","WPAPSK",operateAttributePara},/*wlan key*/
	{OP_ATTRIBUTE,"root.WLan.Entry0","DefaultCfg",operateAttributePara},/*wlan auto ssid flag*/
	{OP_ATTRIBUTE,"root.Account.Entry1","web_passwd",operateAttributePara},/*useradmin web password*/
#if defined(TCSUPPORT_CY)
	{OP_ATTRIBUTE,"root.Account.TelnetEntry","Active",operateAttributePara},
#endif
	{OP_AT_END,NULL,NULL,NULL}
};

int defaultParaRestore()
{
#if !defined(TCSUPPORT_RESERVEAREA_EXTEND) /* we use parameters from proline para block*/
	mxml_node_t *flashDefaultTree  = NULL;
	char buf[128] = {0};
#endif
	mxml_node_t *defaultCurNode = NULL;
	char romfilepath[64]={0};

	/*Restore default parameter from flash default romfile*/
#if !defined(TCSUPPORT_RESERVEAREA_EXTEND)    /* we use parameters from proline para block*/
	/*Read  flash default romfile from reservearea block2 */
	snprintf(buf, sizeof(buf), TC_FLASH_READ_CMD, DR_FILE_NAME, 
	(unsigned long)DEFAULTROMFILE_RA_SIZE, 
	(unsigned long)DEFAULTROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(buf);
	usleep(100000);
	
	/*create a new tree from flash default romfile "reservearea block2".*/
	flashDefaultTree = keypara_load_romfile(DR_FILE_NAME);
	if(flashDefaultTree == NULL) 
	{
		printf("\r\ndefaultParaRestore:load flash default romfile %s error", DR_FILE_NAME);
		goto errorHandle;
	}
#endif

	/*create a new tree from filesystem default romfile, such as "/userfs/ctromfile.cfg".*/
	getDefaultRomfile(romfilepath,sizeof(romfilepath));
	g_fsDefaultRomfileTree = keypara_load_romfile(romfilepath);
	if(g_fsDefaultRomfileTree == NULL) 
	{
		printf("\r\ndefaultParaRestore:load filesystem default romfile %s error", DEF_ROMFILE_PATH);
		goto errorHandle;
	}

	defaultCurNode = g_fsDefaultRomfileTree;/*defaultCurNode is filesystem default romfile tree*/

#if defined(TCSUPPORT_RESERVEAREA_EXTEND)  /*we use parameters from proline para block*/
	handle_proline_paras_restore(defaultCurNode);
#else
	mxml_node_t *curNode = NULL;
	Key_Parameters_Info_Ptr pkeyPara = NULL;
	int retvalue = -1;
	curNode = flashDefaultTree;	/*curNode is flash default romfile tree*/
		
	for (pkeyPara = defaultPara ; (pkeyPara->flag & OP_AT_END) == 0; pkeyPara++ )
	{
		if(pkeyPara->funp != NULL)
		{
			/*Default Para Restore: restore default parameters from flash default romfile to filesystem default romfile*/
			retvalue = pkeyPara->funp(pkeyPara,curNode,defaultCurNode);			
			if(retvalue < 0)
			{
				goto errorHandle;
			}
		}
	}
	cfg_load_from_xml(defaultCurNode);
	if(flashDefaultTree)
		mxmlDelete(flashDefaultTree);
#endif

	return 0;

errorHandle:

	if(g_fsDefaultRomfileTree) {
		mxmlDelete(g_fsDefaultRomfileTree);
		g_fsDefaultRomfileTree = NULL;
	}

	return -1;
}

int operateNodePara(Key_Parameters_Info_Ptr parainfo,mxml_node_t* oldtree,mxml_node_t* newtree)
{
	char nodeName[4][32];
	mxml_node_t *parentNode = NULL;
	mxml_node_t *curNode = NULL;
	mxml_node_t *newCurNode = NULL;
	mxml_attr_t *attr = NULL;
	int i,j;
	int idx = 0, found_ignore = 0;
	ignore_list *pList = NULL;

	if(!(parainfo->flag & OP_NODE))
	{
		printf("\r\noperateNodePara:the function only do node operation,you use the wrong function ,please check it!!!");
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));
	splitName(parainfo->node, nodeName, ".");

	newCurNode = newtree;
	curNode = oldtree;
	for(i = 1; i < 4; i++)
	{
		if(strlen(nodeName[i]) == 0)
			break;
		curNode = mxmlFindElement(curNode, curNode, nodeName[i],
			NULL, NULL, MXML_DESCEND);
		if(!curNode)
		{
			printf("\r\noperateNodePara:the node(%s) is not in running file(means no change)",parainfo->node);
			return 0;/*must return 0,because curNode ==NULL not means error!!!*/
		}
		
		parentNode = newCurNode;
		newCurNode = mxmlFindElement(newCurNode, newCurNode, nodeName[i],
			NULL, NULL, MXML_DESCEND);
	
		if(parainfo->flag & OP_NODE_LEVEL_SINGLE)
		{
			/*because we set the all attributes of the node,so we need to create it if it's not exist!!! */
			if(!newCurNode)
			{
				newCurNode = mxmlNewElement(parentNode,nodeName[i]);
				if(newCurNode == NULL )
				{  
					printf("\r\noperateAttributePara:create a new node(%s) failed",parainfo->node);
					return -1;
				}
			}
		}	
	}

	if(parainfo->flag & OP_NODE_LEVEL_ALL)
	{
		/*First remove from running tree, then add to default tree
		because we replace the node(include sub node) with running romfile,so we need to delete it first!!*/
		if(newCurNode)
		{
			mxmlDelete(newCurNode);
		}
		mxmlRemove(curNode);
		mxmlAdd(parentNode, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, curNode);
	}
	else if(parainfo->flag & OP_NODE_LEVEL_SINGLE)
	{
		attr = curNode->value.element.attrs;
		for(j = 0; j < curNode->value.element.num_attrs; j++ )
		{
			if ( parainfo->flag & OP_ATTRIBUTE_IGNORE_LIST )
			{
				found_ignore = 0;
				pList = (ignore_list *)parainfo->attr;
				if ( pList )
				{
					for ( idx = 0; idx < pList->nodelen; idx ++ )
					{
						if ( 0 == strcmp(pList->attrlist[idx], attr->name) )
						{
							found_ignore = 1;
							break;
						}
					}
				}

				if ( found_ignore )
				{
					attr++;
					continue;
				}
			}
			mxmlElementSetAttr(newCurNode, attr->name, attr->value);
			attr++;
		}
	}
	else
	{
		printf("\r\noperateNodePara:not support this type!!");
	}

	return 0;
	
}

char *ignoreWLan_Common_attr[] = 
{
	  "APOn"
};

ignore_list ignoreWLanList = 
{
	  ignoreWLan_Common_attr
	, sizeof(ignoreWLan_Common_attr)/sizeof(char *)
};

Key_Parameters_Info keyPara[]=
{
	/*follow is node parameters*/
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VirServer",NULL,operateNodePara},/*VirServer information*/
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.Wan",NULL,operateNodePara},/*wan information*/
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.CtDiagnostic",NULL,operateNodePara},/*remote link information*/
#if defined(CT_COM_DEVICEREG)
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.deviceAccount",NULL,operateNodePara},/*remote link information*/
#endif
#if defined(TCSUPPORT_CT_PON_GD) || defined(TCSUPPORT_CT_PON_CZ_HN) || defined(TCSUPPORT_CT_PON_CZ_CQ)
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.Entry0",NULL,operateNodePara},/*admin information*/
#else
	{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_LONG_RESETBAK|OP_JSAccount_ONLY,
	"root.Account.Entry0",NULL,operateNodePara},/*admin information*/
#endif
	{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_ATTRIBUTE_IGNORE_LIST,
	"root.WLan.Common", (char *)&ignoreWLanList,operateNodePara}, /* wlan information */
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry1",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry2",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry3",NULL,operateNodePara},/*wlan information*/
#if defined(TCSUPPORT_MULTI_USER_ITF)
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry4",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry5",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry6",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan.Entry7",NULL,operateNodePara},/*wlan information*/
#endif
	{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_ATTRIBUTE_IGNORE_LIST,
	"root.WLan11ac.Common", (char *)&ignoreWLanList,operateNodePara}, /* wlan information */
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry1",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry2",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry3",NULL,operateNodePara},/*wlan information	*/
#if defined(TCSUPPORT_MULTI_USER_ITF)
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry4",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry5",NULL,operateNodePara},/*wlan information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry6",NULL,operateNodePara},/*wlan information	*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.WLan11ac.Entry7",NULL,operateNodePara},/*wlan information	*/
#endif
	/*follow is attribute parameters*/
#if defined(TCSUPPORT_CMCCV2)
	{OP_ATTRIBUTE,"root.WLan.Entry0","MaxStaNum",operateAttributePara},
	{OP_ATTRIBUTE,"root.WLan11ac.Entry0","MaxStaNum",operateAttributePara},
#endif
#ifdef TCSUPPORT_CWMP
#if defined(TCSUPPORT_CT_PON_GD) || defined(TCSUPPORT_CT_PON_CZ_HN) || defined(TCSUPPORT_CT_PON_CZ_CQ)
	{OP_ATTRIBUTE,"root.Cwmp.Entry","acsUrl",operateAttributePara},/*acs url*/
	{OP_ATTRIBUTE,"root.Cwmp.Entry","acsUserName",operateAttributePara},/*tr069 certification information*/
	{OP_ATTRIBUTE,"root.Cwmp.Entry","acsPassword",operateAttributePara},
	{OP_ATTRIBUTE,"root.Cwmp.Entry","conReqUserName",operateAttributePara},
	{OP_ATTRIBUTE,"root.Cwmp.Entry","conReqPassword",operateAttributePara},
#else
#if defined(TCSUPPORT_CUC)
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY|OP_CWMP_ONLY,"root.Cwmp.Entry","acsUrl",operateAttributePara},/*acs url*/
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.Cwmp.Entry","acsUserName",operateAttributePara},/*tr069 certification information*/
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.Cwmp.Entry","acsPassword",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY|OP_CWMP_ONLY,"root.Cwmp.Entry","conReqUserName",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY|OP_CWMP_ONLY,"root.Cwmp.Entry","conReqPassword",operateAttributePara},
#else
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY,"root.Cwmp.Entry","acsUrl",operateAttributePara},/*acs url*/
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY,"root.Cwmp.Entry","acsUserName",operateAttributePara},/*tr069 certification information*/
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY,"root.Cwmp.Entry","acsPassword",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY,"root.Cwmp.Entry","conReqUserName",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY,"root.Cwmp.Entry","conReqPassword",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_JSCWMP_ONLY,"root.Cwmp.Entry","prePassword",operateAttributePara},
#endif
#endif
#if !defined(TCSUPPORT_C1_CUC) && !defined(TCSUPPORT_CUC)
    {OP_ATTRIBUTE,"root.Cwmp.Entry","cwmpflags",operateAttributePara},
#endif
#endif
	{OP_ATTRIBUTE,"root.Accesslimit.Common","totalnum",operateAttributePara},
	{OP_ATTRIBUTE,"root.Accesslimit.Common","mode",operateAttributePara},
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CT_WAN_PTM)
#if !defined(TCSUPPORT_CUC)
	{OP_ATTRIBUTE,"root.Timezone.Entry","SERVER",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_2NTP)
#if !defined(TCSUPPORT_CUC)
	{OP_ATTRIBUTE,"root.Timezone.Entry","SERVER1",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_5NTP)
	{OP_ATTRIBUTE,"root.Timezone.Entry","SERVER2",operateAttributePara},
	{OP_ATTRIBUTE,"root.Timezone.Entry","SERVER3",operateAttributePara},
	{OP_ATTRIBUTE,"root.Timezone.Entry","SERVER4",operateAttributePara},
#endif
#endif
#if defined(TCSUPPORT_CT_NTPSERVERTYPE)
#if !defined(TCSUPPORT_CUC)
	{OP_ATTRIBUTE,"root.Timezone.Entry","NTPServerType",operateAttributePara},
#endif
#else
	{OP_ATTRIBUTE,"root.Timezone.Entry","AddRoute",operateAttributePara},
#endif
	{OP_ATTRIBUTE,"root.Timezone.Entry","SYNCTIME",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_PON)
#ifdef TCSUPPORT_EPON_OAM_CTC
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.EPON.LOIDAuth","LOID0",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.EPON.LOIDAuth","Password0",operateAttributePara},
#endif
#if defined(TCSUPPORT_CMCC)
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.GPON.ONU","Password",operateAttributePara},
#else
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.GPON.LOIDAuth","LOID",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.GPON.LOIDAuth","Password",operateAttributePara},
#endif
#if VOIP
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPBasic",NULL,operateNodePara},/*VirServer information*/	
	{OP_ATTRIBUTE,"root.VoIPAdvanced.Common","SIPDomain",operateAttributePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPAdvanced",NULL,operateNodePara},/*VirServer information*/
#if defined(TCSUPPORT_CMCCV2)
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPAdvanced.Entry0",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPAdvanced.Entry1",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPCodecs",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPMedia.Common",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPMedia.Entry0",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPMedia.Entry1",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPDigitMap.Entry",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPCallCtrl.Entry0",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPCallCtrl.Entry1",NULL,operateNodePara},
#endif
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VoIPH248",NULL,operateNodePara},
#endif
#endif
#ifdef IPV6
#if !defined(TCSUPPORT_CUC)
	{OP_ATTRIBUTE,"root.Radvd.Entry","AutoPrefix",operateAttributePara},
	{OP_ATTRIBUTE,"root.Radvd.Entry","DelegatedWanConnection",operateAttributePara},
	{OP_ATTRIBUTE,"root.Dhcp6s.Entry","DNSType",operateAttributePara},
	{OP_ATTRIBUTE,"root.Dhcp6s.Entry","DNSWANConnection",operateAttributePara},
#endif
#endif
#if !defined(TCSUPPORT_CT_PON)
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry22",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry23",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry24",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry25",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry26",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry27",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry28",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.QoS.Entry29",NULL,operateNodePara},
#endif
#if defined(TCSUPPORT_IGMP_PROXY)
#if !defined(TCSUPPORT_CT_CONGW_CH)
	{OP_ATTRIBUTE,"root.IGMPproxy.Entry","UpstreamIF",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_WAN_PTM)
	{OP_ATTRIBUTE,"root.IGMPproxyATM.Entry","UpstreamIF",operateAttributePara},
	{OP_ATTRIBUTE,"root.IGMPproxyPTM.Entry","UpstreamIF",operateAttributePara},
#endif
#endif
#if defined(TCSUPPORT_MLD_PROXY)
#if !defined(TCSUPPORT_CT_CONGW_CH)
	{OP_ATTRIBUTE,"root.MLDproxy.Entry","UpstreamIF",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_WAN_PTM)
	{OP_ATTRIBUTE,"root.MLDproxyATM.Entry","UpstreamIF",operateAttributePara},
	{OP_ATTRIBUTE,"root.MLDproxyPTM.Entry","UpstreamIF",operateAttributePara},
#endif
#endif
#if defined(TCSUPPORT_CT_VLAN_BIND)
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VlanBind",NULL,operateNodePara},
#endif
#if defined(TCSUPPORT_CT_PON)
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.LogicID_Entry",NULL,operateNodePara},
#endif
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
#if defined(CT_COM_DEVICEREG)
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.deviceAccount.Entry","userName",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.deviceAccount.Entry","userPasswordDEV",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.LogicID.Entry","Username",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.LogicID.Entry","Password",operateAttributePara},
#if defined(TCSUPPORT_CT_PON_JS)
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.LogicID.Entry","isRegistered",operateAttributePara},
#endif
#endif
#endif
#else
#if defined(CT_COM_DEVICEREG)
	{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.deviceAccount.Entry","userName",operateAttributePara},
	{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.deviceAccount.Entry","userPasswordDEV",operateAttributePara},
#endif
#if defined(TCSUPPORT_CT_PON)
	{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.LogicID.Entry","Username",operateAttributePara},
	{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.LogicID.Entry","Password",operateAttributePara},
#if defined(TCSUPPORT_CT_PON_JS)
	{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.LogicID.Entry","isRegistered",operateAttributePara},
#endif
#endif
#endif
#if defined(TCSUPPORT_CT_PHONEAPP)
#if defined(TCSUPPORT_CT_JOYME)
	{OP_NODE|OP_NODE_LEVEL_ALL, "root.VPN", NULL, operateNodePara},
	{OP_ATTRIBUTE, MOBILE_ENTRY, "root.InterfaceRight", operateAttributePara},
	{OP_ATTRIBUTE, MOBILE_OSRIGHTENTRY, "root.CPU", operateAttributePara},
	{OP_ATTRIBUTE, MOBILE_OSRIGHTENTRY, "root.RAM", operateAttributePara},
	{OP_ATTRIBUTE, MOBILE_OSRIGHTENTRY, "root.FLASH", operateAttributePara},
	{OP_ATTRIBUTE, MOBILE_OSRIGHTENTRY, "root.SOCKET", operateAttributePara},
	{OP_ATTRIBUTE, MOBILE_OSRIGHTENTRY, "root.PORT", operateAttributePara},
	{OP_ATTRIBUTE, MOBILE_OSRIGHTENTRY, "root.thread", operateAttributePara},
#else
	{OP_ATTRIBUTE|OP_LONG_RESETBAK,"root.mobile.Entry","InterfaceRight",operateAttributePara},
#endif
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.mobile.Entry","MgtURL",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK|OP_CWMP_ONLY,"root.mobile.Entry","Port",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK,"root.mobile.Entry","Heartbeat",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK,"root.mobile.Entry","Ability",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK,"root.mobile.Entry","LocatePort",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK,"root.mobile.Entry","OperateSAddr",operateAttributePara},
	{OP_ATTRIBUTE|OP_LONG_RESETBAK,"root.mobile.Entry","OperateSPort",operateAttributePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.mobile.DnsRedirect",NULL,operateNodePara},
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	{OP_ATTRIBUTE,"root.mobile.Entry", "Ability",operateAttributePara},
	{OP_ATTRIBUTE,"root.mobile.Entry", "Heartbeat",operateAttributePara},
	{OP_ATTRIBUTE,"root.mobile.Entry", "MgtURL",operateAttributePara},
	{OP_ATTRIBUTE,"root.mobile.Entry", "Port",operateAttributePara},
	{OP_ATTRIBUTE,"root.mobile.Entry", "LocatePort",operateAttributePara},
	{OP_ATTRIBUTE,"root.mobile.Entry", "Appmodel",operateAttributePara},	
	{OP_ATTRIBUTE,"root.mobile.Entry", "BSSAddr",operateAttributePara},
	{OP_NODE|OP_NODE_LEVEL_ALL, "root.VPN", NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL, "root.VPNDomain", NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL, "root.VPNIPS", NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL, "root.VPNMAC", NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL, "root.Samba",NULL,operateNodePara},/*samba information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpCommon",NULL,operateNodePara},/*ftp information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry0",NULL,operateNodePara},/*ftp information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry1",NULL,operateNodePara},/*ftp information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry2",NULL,operateNodePara},/*ftp information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry3",NULL,operateNodePara},/*ftp information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry4",NULL,operateNodePara},/*ftp information*/
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry5",NULL,operateNodePara},/*ftp information*/
#endif

#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
	{OP_ATTRIBUTE,"XPON_LinkCfg","Mode",operateAttributePara},
#endif
	{OP_AT_END,NULL,NULL,NULL}
};

#if defined(TCSUPPORT_CWMP_NG)
void setCwmpFlagsEvent(mxml_node_t* newtree)
{
	cfg_node_type_t *curNode = NULL;
	cfg_node_type_t *cwmpNode = NULL;
	char nodeName[64] = {0};
	char attr[12] = {0}; 
	int idx = 0 ,attrNum = 0;
	char attrNameBuf[32] ={0};
	char attrBuf[600] ={0};

	strncpy(nodeName, CWMP_ENTRY_NODE, sizeof(nodeName)-1);
	if ( NULL == newtree )
		return;
	
	cwmpNode = cfg_type_query(nodeName);
	curNode = mxmlFindElement(newtree, newtree, "Cwmp", NULL, NULL, MXML_DESCEND);
	if ( cwmpNode &&  curNode)
	{
		curNode = mxmlFindElement(curNode, curNode, "Entry", NULL, NULL, MXML_DESCEND);
		if ( curNode )
		{
			bzero(attr, sizeof(attr));
			cfg_obj_get_object_attr(nodeName, "cwmpflagsLen", 0, attr, sizeof(attr));
			if( '\0' == attr[0])
				return;
			mxmlElementSetAttr(curNode, "cwmpflagsLen", attr);
			attrNum = atoi(attr);
			for(idx = 0;idx < attrNum;idx++)
			{
				bzero(attrNameBuf, sizeof(attrNameBuf));
				snprintf(attrNameBuf, sizeof(attrNameBuf), "cwmpflags%d", idx);
				cfg_obj_get_object_attr(nodeName, attrNameBuf, 0, attrBuf, sizeof(attrBuf));
				if( '\0' == attrBuf[0])
					continue;
				mxmlElementSetAttr(curNode, attrNameBuf, attrBuf);
			}						
		}	
	}
	
	return;
}
#endif

void setSimCardResetFailAlarmEvent(mxml_node_t* newtree)
{
	cfg_node_type_t *curNode = NULL;
	char nodeName[64] = {0};

	strncpy(nodeName, CWMP_ENTRY_NODE, sizeof(nodeName)-1);
	if ( NULL == newtree )
		return;

	curNode = cfg_type_query(nodeName);
	if ( curNode )
	{
		cfg_set_object_attr(nodeName, "simcardrstfail", "1");
	}
}

void setCwmpLongResetEvent(mxml_node_t* newtree)
{
	mxml_node_t *curNode = NULL;

	if ( NULL == newtree )
		return;

	curNode = mxmlFindElement(newtree, newtree, "Cwmp", NULL, NULL, MXML_DESCEND);
	if ( curNode )
	{
		curNode = mxmlFindElement(curNode, curNode, "Entry", NULL, NULL, MXML_DESCEND);
		if ( curNode )
			mxmlElementSetAttr(curNode, "longresetbtn", "1");
	}
}

int delTR69andChkBackupTR69Position(mxml_node_t* newtree)
{
#if 1
	mxml_node_t *curNode = NULL;
	mxml_node_t *curNode_pvc = NULL;
	mxml_node_t *curNode_entry = NULL;
	int pvc_idx = 0, entry_idx = 0;
	char WanNodePVC[32] = {0};
	char WanNodeEntry[32] = {0};
	char *attr1=NULL, *attr2 = NULL, *attr3 = NULL;
	int found_PVC = 0;
	int isSinglePVC = 1;
	int restore_PVC = -1, restore_Entry = -1;

	if ( NULL == newtree )
		return -1;

	/* we shouldn't delete Wan_Common node.*/
	if ( -1 != TR69_NodeIdx )
	{
		curNode = mxmlFindElement(newtree, newtree, "Wan", NULL, NULL, MXML_DESCEND);
		if( curNode )
		{
			for ( pvc_idx = 0; pvc_idx < MAX_PVC_NUM; pvc_idx++ )
			{
				snprintf(WanNodePVC, sizeof(WanNodePVC), "PVC%d", pvc_idx);
				curNode_pvc = mxmlFindElement(curNode, curNode, WanNodePVC, NULL, NULL, MXML_DESCEND);
				if( curNode_pvc )
				{
					found_PVC = 0;
					/* check PVC entry */
					attr1 = mxmlElementGetAttr(curNode_pvc, LONGRESET_TR69_KEY1);
					attr2 = mxmlElementGetAttr(curNode_pvc, LONGRESET_TR69_KEY2);
					attr3 = mxmlElementGetAttr(curNode_pvc, LONGRESET_TR69_KEY3);
					if (
#if defined(TCSUPPORT_CT_E8B_ADSL)
						attr2 && 0 == strcmp(attr2, TR69_VLANID)
						&& attr3 && 0 == strcmp(attr3, TR69_PBIT)
#else
						attr1 && 0 == strcmp(attr1, TR69_VLANMODE)
						&& attr2 && 0 == strcmp(attr2, TR69_VLANID)
						&& attr3 && 0 == strcmp(attr3, TR69_PBIT)
#endif
						&& -1 == restore_PVC )
					{
						restore_PVC = pvc_idx;
						found_PVC = 1;
					}

					isSinglePVC = 1;
					for ( entry_idx = 0; entry_idx < MAX_SMUX_NUM; entry_idx ++ )
					{
						snprintf(WanNodeEntry, sizeof(WanNodeEntry), "Entry%d", entry_idx);
						curNode_entry = mxmlFindElement(curNode_pvc, curNode_pvc, WanNodeEntry, NULL, NULL, MXML_DESCEND);
						if ( curNode_entry )
						{
							attr1 = mxmlElementGetAttr(curNode_entry, "ServiceList");
							if( attr1 && NULL != strstr(attr1, "TR069") )
							{
								mxmlDelete(curNode_entry); /* remove current TR69.*/
								if ( 1 == found_PVC && -1 == restore_Entry )
									restore_Entry = entry_idx;
							}
							else
								isSinglePVC = 0;
						}
						else
						{
							if ( 1 == found_PVC && -1 == restore_Entry )
								restore_Entry = entry_idx;
						}
					}

					if ( isSinglePVC && -1 == restore_PVC && -1 == restore_Entry )
						mxmlDelete(curNode_pvc);/* remove single PVC.*/
				}
				else
				{
					if ( -1 == currEmptyPVC_index )
					{
						/* backup empty pvc index*/
						currEmptyPVC_index = pvc_idx;
					}
				}
			}
		}

		if ( -1 == restore_PVC || -1 == restore_Entry )
		{
			if ( -1 == currEmptyPVC_index )
			{
				printf("\nlong press reset fatal error: there is no wan space to restore TR69 wan[%s]-[%s]-[%s] \n",
					TR69_VLANMODE, TR69_VLANID, TR69_PBIT);
				return -1;
			}
			else
				restore_TR69NodeIdx = currEmptyPVC_index * MAX_SMUX_NUM;
		}
		else
			restore_TR69NodeIdx = restore_PVC * MAX_SMUX_NUM + restore_Entry;
	}

#else
	cfg_node_type_t *curNode = NULL;
	int pvc_idx = 0, entry_idx = 0;
	char WanNodePVC[64] = {0};
	char WanNodeEntry[64] = {0};
	char attr1[12] = {0}; 
	char attr2[12] = {0}; 
	char attr3[12] = {0};
	int found_PVC = 0;
	int isSinglePVC = 1;
	int restore_PVC = -1, restore_Entry = -1;

	if ( NULL == newtree )
		return -1;

	/* we shouldn't delete Wan_Common node.*/
	if ( -1 != TR69_NodeIdx )
	{
		curNode = cfg_type_query(WAN_NODE);
		if( curNode )
		{
			for ( pvc_idx = 1; pvc_idx <= MAX_PVC_NUM; pvc_idx++ )
			{
				snprintf(WanNodePVC, sizeof(WanNodePVC), WAN_PVC_NODE, pvc_idx);
				if (cfg_query_object(WanNodePVC,NULL,NULL) > 0)
				{
					found_PVC = 0;
					/* check PVC entry*/
					cfg_obj_get_object_attr(WanNodePVC, LONGRESET_TR69_KEY1, 0, attr1, sizeof(attr1));
					cfg_obj_get_object_attr(WanNodePVC, LONGRESET_TR69_KEY2, 0, attr2, sizeof(attr2));
					cfg_obj_get_object_attr(WanNodePVC, LONGRESET_TR69_KEY3, 0, attr3, sizeof(attr3));
					if (
#if defined(TCSUPPORT_CT_E8B_ADSL)
						0 == strcmp(attr2, TR69_VLANID) && 0 == strcmp(attr3, TR69_PBIT)
#else
						0 == strcmp(attr1, TR69_VLANMODE) && 0 == strcmp(attr2, TR69_VLANID)
						&& 0 == strcmp(attr3, TR69_PBIT)
#endif
						&& -1 == restore_PVC )
					{
						restore_PVC = pvc_idx;
						found_PVC = 1;
					}

					isSinglePVC = 1;
					for ( entry_idx = 1; entry_idx <= MAX_SMUX_NUM; entry_idx ++ )
					{
						snprintf(WanNodeEntry, sizeof(WanNodeEntry), WAN_PVC_ENTRY_NODE, pvc_idx, entry_idx);
						if (cfg_query_object(WanNodeEntry,NULL,NULL) > 0)
						{
							cfg_obj_get_object_attr(WanNodeEntry, "ServiceList", 0, attr1, sizeof(attr1));
							if( NULL != strstr(attr1, "TR069") )
							{
								cfg_delete_object(WanNodeEntry); /* remove current TR69.*/
								if ( 1 == found_PVC && -1 == restore_Entry )
									restore_Entry = entry_idx;
							}
							else
								isSinglePVC = 0;
						}
						else
						{
							if ( 1 == found_PVC && -1 == restore_Entry )
								restore_Entry = entry_idx;
						}
					}

					if ( isSinglePVC && -1 == restore_PVC && -1 == restore_Entry )
						cfg_delete_object(WanNodePVC);/* remove single PVC.*/
				}
				else
				{
					if ( -1 == currEmptyPVC_index )
					{
						/* backup empty pvc index*/
						currEmptyPVC_index = pvc_idx;
					}
				}
			}
		}

		if ( -1 == restore_PVC || -1 == restore_Entry )
		{
			if ( -1 == currEmptyPVC_index )
			{
				printf("\nlong press reset fatal error: there is no wan space to restore TR69 wan[%s]-[%s]-[%s] \n",
					TR69_VLANMODE, TR69_VLANID, TR69_PBIT);
				return -1;
			}
			else
				restore_TR69NodeIdx = currEmptyPVC_index * MAX_SMUX_NUM;
		}
		else
			restore_TR69NodeIdx = restore_PVC * MAX_SMUX_NUM + restore_Entry;
	}
#endif

	return 0;
}

int del_wan_node(mxml_node_t* newtree)
{
	cfg_node_type_t *curNode = NULL;
	int pvc_idx = 0;
	char WanNodePVC[64] = {0};

	if ( NULL == newtree )
		return -1;

	/* we shouldn't delete Wan_Common node.*/
	if ( -1 != TR69_NodeIdx )
	{
		curNode = cfg_type_query(WAN_NODE);
		if( curNode )
		{
			for ( pvc_idx = 1; pvc_idx <= MAX_PVC_NUM; pvc_idx++ )
			{
				snprintf(WanNodePVC, sizeof(WanNodePVC), WAN_PVC_NODE, pvc_idx);
				if (cfg_query_object(WanNodePVC,NULL,NULL) > 0)
					cfg_delete_object(WanNodePVC);
			}
		}
	}
	return 0;
}

int operateNodeParaEnhance(Key_Parameters_Info_Enhance_Ptr parainfo,mxml_node_t* oldtree,mxml_node_t* newtree)
{
	char nodeName[4][32];
	char nodeName_new[4][32];
	mxml_node_t *parentNode = NULL;
	mxml_node_t *curNode = NULL;
	mxml_node_t *newCurNode = NULL;
	mxml_attr_t *attr = NULL;
	int i,j;

	if(!(parainfo->flag & OP_NODE))
	{
		printf("\r\noperateNodeParaEnhance:the function only do node operation,you use the wrong function ,please check it!!!");
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));
	splitName(parainfo->node, nodeName, ".");
	memset(nodeName_new, 0, sizeof(nodeName_new));
	splitName(parainfo->nodenew, nodeName_new, ".");

	newCurNode = newtree;
	curNode = oldtree;
	for(i = 1; i <= 3; i++)
	{
		if(strlen(nodeName[i])==0)
			break;
		curNode = mxmlFindElement(curNode, curNode, nodeName[i],
			NULL, NULL, MXML_DESCEND);
		if(!curNode)
		{
			printf("\r\noperateNodeParaEnhance:the node(%s) is not in running file(means no change)",parainfo->node);
			return 0;/*must return 0,because curNode ==NULL not means error!!!*/
		}

		parentNode = newCurNode;
		newCurNode = mxmlFindElement(newCurNode, newCurNode, nodeName_new[i],
			NULL, NULL, MXML_DESCEND);
	
		if(parainfo->flag & OP_NODE_LEVEL_SINGLE)
		{
			/*because we set the all attributes of the node,so we need to create it if it's not exist!!! */
			if(!newCurNode)
			{
				newCurNode = mxmlNewElement(parentNode,nodeName_new[i]);
				if(newCurNode == NULL )
				{  
					printf("\r\noperateNodeParaEnhance:create a new node(%s) failed",parainfo->node);
					return -1;
				}
			}
		}	
	}

	if(parainfo->flag & OP_NODE_LEVEL_SINGLE)
	{
		attr = curNode->value.element.attrs;
		for(j = 0; j < curNode->value.element.num_attrs; j++ )
		{
			mxmlElementSetAttr(newCurNode, attr->name, attr->value);
			attr++;
		}
	}
	else
	{
		printf("\r\noperateNodeParaEnhance:not support this type!!");
	}

	return 0;
}

int resetTR69NodeAttribute(mxml_node_t* newtree)
{
	char nodeName[3][32] = {0};
	mxml_node_t *curNode = NULL;
	mxml_attr_t *attr = NULL;
	int i = 0;
	int pvc_index = 0;
	int entry_index = 0;
	char tr69newWanNodePVC[32] = {0};
	char tr69newWanNodePVCEntry[32] = {0};
	char tempval[32] = {0};

	if ( NULL == newtree )
		return -1;

	if ( -1 != TR69_NodeIdx
		&& -1 != restore_TR69NodeIdx )
	{
		pvc_index = restore_TR69NodeIdx / MAX_SMUX_NUM/* - 1*/;
		entry_index = restore_TR69NodeIdx % MAX_SMUX_NUM/* - 1*/;

		sprintf(tr69newWanNodePVCEntry, "Wan_PVC%d_Entry%d", pvc_index, entry_index);
	}
	else
		return -1;

	memset(nodeName, 0, sizeof(nodeName));
	splitName(tr69newWanNodePVCEntry, nodeName, "_");

	curNode = newtree;
	for(i = 0; i < 3; i++)
	{
		if( strlen(nodeName[i])==0 )
			break;
		curNode = mxmlFindElement(curNode, curNode, nodeName[i],
			NULL, NULL, MXML_DESCEND);
		if( !curNode )
			return 0;
	}

	attr = mxmlElementGetAttr(curNode, "NASName");
	if ( attr )
	{
		sprintf(tempval, "nas%d_%d", pvc_index, entry_index);
		mxmlElementSetAttr(curNode, "NASName", tempval);
	}

	attr = mxmlElementGetAttr(curNode, "IFName");
	if ( attr )
	{
		if ( NULL != strstr(attr, "ppp") )
			sprintf(tempval, "ppp%d", restore_TR69NodeIdx);
		else
			sprintf(tempval, "nas%d_%d", pvc_index, entry_index);
		mxmlElementSetAttr(curNode, "IFName", tempval);
	}

	attr = mxmlElementGetAttr(curNode, "PPPUNIT");
	if ( attr )
	{
		sprintf(tempval, "%d", restore_TR69NodeIdx);
		mxmlElementSetAttr(curNode, "PPPUNIT", tempval);
	}

	return 0;
}

int restoreTR69WanIndex(mxml_node_t* oldtree, mxml_node_t* newtree)
{
#if (defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CT_E8B_ADSL)) || defined(TCSUPPORT_CUC)
	Key_Parameters_Info_Enhance_Ptr pkeyPara = NULL;
	Key_Parameters_Info_Enhance keyPara_restore[]=
	{
		{OP_NODE|OP_NODE_LEVEL_SINGLE, "root.Wan.PVCx", "root.Wan.PVCx", NULL, operateNodeParaEnhance},
		{OP_NODE|OP_NODE_LEVEL_SINGLE, "root.Wan.PVCx.Entryx", "root.Wan.PVCx.Entryx", NULL, operateNodeParaEnhance},
		{OP_AT_END,NULL,NULL,NULL}
	};
#else
	Key_Parameters_Info_Ptr pkeyPara = NULL;
	Key_Parameters_Info keyPara_restore[]=
	{
		{OP_NODE|OP_NODE_LEVEL_SINGLE, "root.Wan.PVCx", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_SINGLE, "root.Wan.PVCx.Entryx", NULL, operateNodePara},
		{OP_AT_END,NULL,NULL,NULL}
	};
#endif
	char tr69WanNodePVC[64] = {0};
	char tr69WanNodePVCEntry[64] = {0};
#if (defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CT_E8B_ADSL)) || defined(TCSUPPORT_CUC)
	char tr69newWanNodePVC[64] = {0};
	char tr69newWanNodePVCEntry[64] = {0};
#endif
	int pvc_index = 0;
	int entry_index = 0;

	if ( NULL == oldtree || NULL == newtree || (-1 == TR69_NodeIdx))
		return -1;

#if (defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CT_E8B_ADSL)) || defined(TCSUPPORT_CUC)
	if ( -1 != TR69_NodeIdx
		&& -1 != restore_TR69NodeIdx )
	{
		pvc_index = TR69_NodeIdx / MAX_SMUX_NUM;
		entry_index = TR69_NodeIdx % MAX_SMUX_NUM;

		snprintf(tr69WanNodePVC, sizeof(tr69WanNodePVC), "root.Wan.PVC%d", pvc_index);
		keyPara_restore[0].node = tr69WanNodePVC;
		snprintf(tr69WanNodePVCEntry, sizeof(tr69WanNodePVCEntry), "root.Wan.PVC%d.Entry%d", pvc_index, entry_index);
		keyPara_restore[1].node = tr69WanNodePVCEntry;

		pvc_index = restore_TR69NodeIdx / MAX_SMUX_NUM/* - 1*/;
		entry_index = restore_TR69NodeIdx % MAX_SMUX_NUM/* - 1*/;

		snprintf(tr69newWanNodePVC, sizeof(tr69newWanNodePVC), "root.Wan.PVC%d", pvc_index);
		keyPara_restore[0].nodenew = tr69newWanNodePVC;
		snprintf(tr69newWanNodePVCEntry, sizeof(tr69newWanNodePVCEntry), "root.Wan.PVC%d.Entry%d", pvc_index, entry_index);
		keyPara_restore[1].nodenew = tr69newWanNodePVCEntry;
	}
	else
	{
		keyPara_restore[0].funp = NULL;
		keyPara_restore[1].funp = NULL;
	}

#else

	if ( -1 != TR69_NodeIdx )
	{
		pvc_index = TR69_NodeIdx / MAX_SMUX_NUM;
		entry_index = TR69_NodeIdx % MAX_SMUX_NUM;

		snprintf(tr69WanNodePVC, sizeof(tr69WanNodePVC), "root.Wan.PVC%d", pvc_index);
		keyPara_restore[0].node = tr69WanNodePVC;
		snprintf(tr69WanNodePVCEntry, sizeof(tr69WanNodePVCEntry), 
			"root.Wan.PVC%d.Entry%d", pvc_index, entry_index);
		keyPara_restore[1].node = tr69WanNodePVCEntry;
	}
	else
	{
		keyPara_restore[0].funp = NULL;
		keyPara_restore[1].funp = NULL;
	}
#endif

	for ( pkeyPara = keyPara_restore ; (pkeyPara->flag & OP_AT_END) == 0; pkeyPara++ )
	{
		if(pkeyPara->funp != NULL)
		{
			if ( pkeyPara->funp(pkeyPara, oldtree, newtree) < 0 )
				return -1;
		}
	}
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_CUC)
	/* must reset the specal attribute*/
	resetTR69NodeAttribute(newtree);
#endif

	return 0;
}

int keyParaRestore(int mode)
{
	mxml_node_t *defaultTree  = NULL;
	mxml_node_t *runningTree  = NULL;
	mxml_node_t *defaultCurNode = NULL;
	mxml_node_t *runningCurNode = NULL;
	Key_Parameters_Info_Ptr pkeyPara = NULL;
	char cmd[80] = {0};
	int retvalue = -1;
	char romfilepath[64]={0};
#if defined(TCSUPPORT_CT_PHONEAPP)
	Key_Parameters_Info WLanSch[] =
	{
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry","WLanStartTime",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry","WLanEndTime",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry","WLanScheduleEable",operateAttributePara},
		{OP_AT_END,NULL,NULL,NULL}
	};
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	Key_Parameters_Info cwmp_param[] =
	{
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY,"root.Samba",NULL,operateNodePara},/*samba information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpCommon",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpEntry0",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpEntry1",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpEntry2",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpEntry3",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpEntry4",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_SINGLE|OP_CWMP_ONLY,"root.Account.FtpEntry5",NULL,operateNodePara},/*ftp information*/
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.Timer", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.VPN", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.VPNDomain", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.VPNIPS", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.VPNMAC", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.SpeedTest", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.Account.TelnetEntry1", NULL, operateNodePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry", "Ability",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry", "Heartbeat",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry", "MgtURL",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry", "Port",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry", "LocatePort",operateAttributePara},
		{OP_ATTRIBUTE|OP_CWMP_ONLY,"root.mobile.Entry", "Appmodel",operateAttributePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.BeaconVSIE", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.ProbeRxVSIE", NULL, operateNodePara},
		{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.WiFiProbeInfo", NULL, operateNodePara},
		/*{OP_NODE|OP_NODE_LEVEL_ALL|OP_CWMP_ONLY, "root.LANHost2", NULL, operateNodePara},*/
		{OP_AT_END,NULL,NULL,NULL}
	};
#endif

	/*weather we need to free the running romfile tree in the memory for reduing memory??
	because after calling the function 'keyParaRestore()',cpe will reboot 
	
	create a new tree from "/userfs/romfile.cfg".*/
	if(g_fsDefaultRomfileTree != NULL) 
	{
		defaultTree = g_fsDefaultRomfileTree;/*The default romfile which has been restored by defaultParaRestore*/
	}
	else {
		getDefaultRomfile(romfilepath,sizeof(romfilepath));
		defaultTree = keypara_load_romfile(romfilepath);
		if(defaultTree == NULL)
		{
			printf("\r\nkeyParaRestore:load default romfile error");
			goto errorHandle;
		}
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)  /*we use parameters from proline para block*/
		else
		{
			handle_proline_paras_restore(defaultTree);
		}
#endif
	}
	
	/*create a new tree from "/mtd/mtdblock1".*/
	runningTree = keypara_load_romfile(RUNNING_ROMFILE_PATH);
	if(runningTree == NULL)
	{
		printf("\r\nkeyParaRestore:load running romfile error");
		goto errorHandle;
	}

	defaultCurNode = defaultTree;
	runningCurNode = runningTree;
	
	/*store key parameters*/

	for (pkeyPara = keyPara ; (pkeyPara->flag & OP_AT_END) == 0; pkeyPara++ )
	{
		if ( BOOT2DEFSETTR69 == mode )
		{
			if ( !(pkeyPara->flag & OP_CWMP_ONLY) )
				continue;
		}
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
		if ( LONGRESET_BOOT2DEFSET == mode )
		{
#if defined(TCSUPPORT_CUC)
			break;
#endif
			if ( !(pkeyPara->flag & OP_LONG_RESETBAK) )
				continue;
		}
#endif

		if(pkeyPara->funp != NULL)
		{
			retvalue = pkeyPara->funp(pkeyPara,runningCurNode,defaultCurNode);
			if(retvalue < 0)
			{
				goto errorHandle;
			}
		}
	}
#if defined(TCSUPPORT_CT_PHONEAPP)
	if ( BOOT2DEFSETTR69 == mode )
	{
		for (pkeyPara = WLanSch ; (pkeyPara->flag & OP_AT_END) == 0; pkeyPara++ )
		{
			if(pkeyPara->funp != NULL)
			{
				retvalue = pkeyPara->funp(pkeyPara,runningCurNode,defaultCurNode);
				if(retvalue < 0)
				{
					goto errorHandle;
				}
			}
		}
	}
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	if ( BOOT2DEFSETTR69 == mode )
	{
		for (pkeyPara = cwmp_param ; (pkeyPara->flag & OP_AT_END) == 0; pkeyPara++ )
		{
			if(pkeyPara->funp != NULL)
			{
				retvalue = pkeyPara->funp(pkeyPara,runningCurNode,defaultCurNode);
				if(retvalue < 0)
				{
					goto errorHandle;
				}
			}
		}
	}
#endif

#if defined(TCSUPPORT_CWMP_NG)
	if ( BOOT2DEFSETTR69 != mode && LONGRESET_BOOT2DEFSET != mode )
	{
		setCwmpFlagsEvent(defaultCurNode);
	}
#endif

#if defined(TCSUPPORT_CT_LONG_RESETBTN)
	if ( LONGRESET_BOOT2DEFSET == mode ) /* restore tr69 wan.*/
	{
		/* add long reset button cwmp inform event.*/
		setCwmpLongResetEvent(defaultCurNode);

		if ( -1 != TR69_NodeIdx
			)
		{
#if defined(TCSUPPORT_CT_PON) && !defined(TCSUPPORT_CUC)
			/* 1. delete tr69 wan node and check backup tr69 wan position  from new tree.*/
			delTR69andChkBackupTR69Position(defaultCurNode);
#else
			/* 1. delete wan node from new tree.*/
			if ( 0 != del_wan_node(defaultCurNode) )
				goto errorHandle;
#endif
			/* 2. restore tr69 wan.*/
			if ( 0 != restoreTR69WanIndex(runningCurNode, defaultCurNode) )
				goto errorHandle;
		}
	}
#endif

#if defined(TCSUPPORT_CUC)
	if ( BOOT2DEFSETTR69 == mode && ( -1 != TR69_NodeIdx))
	{
		/* 1. delete tr69 wan node and check backup tr69 wan position  from new tree.*/
		delTR69andChkBackupTR69Position(defaultCurNode);
			
		if ( 0 != restoreTR69WanIndex(runningCurNode, defaultCurNode) )
				goto errorHandle;
	}
#endif

	/*Create a romfile.cfg and store into "/mtd/mtdblock1"*/
	cfg_delete_node();

	cfg_load_from_xml(defaultTree);			
	if(cfg_save_object(CFG_OBJ_FILE_PATH) < 0)
		goto errorHandle;

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	retvalue = compute_checksum(CFG_OBJ_FILE_PATH,STORE_ROMFILE_FILE_NAME);
	if(retvalue < 0)
	{
		printf("\r\nkeyParaRestore:Fail to compute checksum when save to flash!\n");
		goto errorHandle;
	}
#endif
	/*free the default tree if need*/
	if((g_fsDefaultRomfileTree == NULL) && defaultTree)
		mxmlDelete(defaultTree);
	
	/*tree the runningTree tree*/
	if(runningTree)
		mxmlDelete(runningTree);
	
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	snprintf(cmd, sizeof(cmd), UPDATE_ROMFILE_CMD, STORE_ROMFILE_FILE_NAME);
	system(cmd);
	usleep(100000);

	snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD,
		(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	snprintf(cmd, sizeof(cmd), TC_FLASH_WRITE_CMD, STORE_ROMFILE_FILE_NAME, 
		(unsigned long)BACKUPROMFILE_RA_SIZE,(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(cmd);
#else
	#ifdef TCSUPPORT_BACKUPROMFILE
	snprintf(cmd, sizeof(cmd), UPDATE_ROMFILE_CMD, ROMFILE_PATH);
	system(cmd);
	usleep(100000);
	snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD, 
		(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	snprintf(cmd, sizeof(cmd), TC_FLASH_WRITE_CMD, ROMFILE_PATH,
		(unsigned long)BACKUPROMFILE_RA_SIZE,(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(cmd);
	#else
	snprintf(cmd, sizeof(cmd), UPDATE_ROMFILE_CMD, ROMFILE_PATH);	
	system(cmd);
	#endif
#endif

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	unlink(STORE_ROMFILE_FILE_NAME);
#endif
	
	return 0;
	
errorHandle:

	if((g_fsDefaultRomfileTree == NULL) && defaultTree) 
	{
		mxmlDelete(defaultTree);
	}
	if(runningTree)
		mxmlDelete(runningTree);
	return -1;
}

#if defined(TCSUPPORT_CUC)
/* check facotry reset mode. */
int cuc_check_factory_reset(void)
{
	char nodeName[64] = {0};
	char tempValue[24] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, "root.Sys.Entry", sizeof(nodeName)-1);

	cfg_obj_get_object_attr(nodeName, "FacResetMode", 0, tempValue, sizeof(tempValue));

	if ((tempValue != NULL) && (0 == strcmp(tempValue, "0")) )
	{
		return 0;
	}

	return 1;
}
#endif

int defaultParaRomfileUpdate()
{
#ifndef TCSUPPORT_BACKUPROMFILE
	char cmd[80] = {0};
#endif
	int retvalue = -1;

	if(g_fsDefaultRomfileTree == NULL)
		goto errorHandle;

	/*Create a romfile.cfg and store into "/mtd/mtdblock1"*/
	if(cfg_save_file(DEF_ROMFILE_PATH) < 0)
		goto errorHandle;

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	retvalue = compute_checksum(CFG_OBJ_FILE_PATH,STORE_ROMFILE_FILE_NAME);
	if(retvalue < 0)
	{
		printf("\r\ndefaultParaRestore:Fail to compute checksum when save to flash!\n");
		goto errorHandle;
	}
#endif
	
	//free the tree
	if(g_fsDefaultRomfileTree) {
		mxmlDelete(g_fsDefaultRomfileTree);
		g_fsDefaultRomfileTree = NULL;
	}
	
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	sys_update_backup_romfile(STORE_ROMFILE_FILE_NAME);
#else
	#ifdef TCSUPPORT_BACKUPROMFILE
	sys_update_backup_romfile(ROMFILE_PATH);
	#else
	snprintf(cmd, sizeof(cmd), UPDATE_ROMFILE_CMD, ROMFILE_PATH);	
	system(cmd);
	#endif
#endif

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	unlink(STORE_ROMFILE_FILE_NAME);
#endif

	return 0;
	
errorHandle:

	if(g_fsDefaultRomfileTree) {
		mxmlDelete(g_fsDefaultRomfileTree);
		g_fsDefaultRomfileTree = NULL;
	}

	return -1;
}

void restore2Defaulromile()
{
	char buf[256] = {0};
	int ret = -1;
#define TMP_DF_ROMFILE_NAME "/tmp/tc_defromfile_tims"
	/*
	* 1. read romfile form defaultromfile area
	* TC_FLASH_READ_CMD --> "/userfs/bin/mtd readflash %s %lu %lu %s"
	*/
	snprintf(buf, sizeof(buf) - 1, TC_FLASH_READ_CMD,
			TMP_DF_ROMFILE_NAME,
			DEFAULTROMFILE_RA_SIZE,
			DEFAULTROMFILE_RA_OFFSET,
			RESERVEAREA_NAME);

	system(buf);
	usleep(100000);

	/*
	* 2. checksum and parse tree.
	*/
	ret = check_checksum(TMP_DF_ROMFILE_NAME,LOAD_ROMFILE_FILE_NAME, GZ_TMP_PATH);
	if ( ret < 0 )
	{
		printf("\n invalid default romfile, do nothing!\n");
		return; /* romfile is bad, do nothing. */
	}
	unlink(LOAD_ROMFILE_FILE_NAME);
	/*
	* 3. write it to current running romfile.
	* UPDATE_ROMFILE_CMD --> "/userfs/bin/mtd write %s romfile"
	*/
	snprintf(buf, sizeof(buf) - 1,
			UPDATE_ROMFILE_CMD, TMP_DF_ROMFILE_NAME);	
	system(buf);
	usleep(100000);

	unlink(TMP_DF_ROMFILE_NAME);

	return;
}

#if defined(TCSUPPORT_CT_JOYME2)
/*
NOT SUPPORT OP_ATTRIBUTE now !!!
*/
Key_Parameters_Info dbusRestorePara[]=
{
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.Samba",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry0",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry1",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry2",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry3",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry4",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_SINGLE,"root.Account.FtpEntry5",NULL,operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.Timer",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VPN",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VPNDomain",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VPNIPS",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.VPNMAC",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.SpeedTest",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.Account.TelnetEntry1",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.BeaconVSIE",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.ProbeRxVSIE",NULL, operateNodePara},
	{OP_NODE|OP_NODE_LEVEL_ALL,"root.WiFiProbeInfo",NULL, operateNodePara},
	/*{OP_NODE|OP_NODE_LEVEL_ALL,"root.LANHost2",NULL, operateNodePara},*/
	{OP_AT_END,NULL,NULL,NULL}
};

static mxml_node_t *load_romfile(char *pathname)
{
	FILE *fp=NULL;
	mxml_node_t *tree=NULL;
	int ret = -1, res = 0;

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	int flag = -1;
	/*Load romfile  file*/
	flag = check_checksum(pathname,LOAD_ROMFILE_FILE_NAME, GZ_TMP_PATH);
	if(flag <0)
	{
		printf(ROMFILE_CHECKSUM_ERR_MSG);
		unlink(LOAD_ROMFILE_FILE_NAME);
		return NULL;
	}
	
	fp = fopen(LOAD_ROMFILE_FILE_NAME, "r");
#else
	fp = fopen(pathname, "r");
#endif

	if(fp == NULL)
	{
		printf("\r\nkeypara_load_romfile:open file %s fail!",pathname);
		return NULL;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_IGNORE_CALLBACK);	
	fclose(fp);

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	unlink(LOAD_ROMFILE_FILE_NAME);
#endif
	return tree;
}

int restore_LANHost2_Access(void)
{
	int idx = 0, internet = 0;
	char lanhost_path[32] = {0}, mac_addr[32] = {0}, lanhost2black_path[32] = {0};
	char internet_access[32] = {0};

	for ( idx = 0; idx < MAX_LANHOST2_ENTRY_NUM; idx++ )
	{
		snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_ENTRY_NODE, idx + 1);

		if ( cfg_obj_get_object_attr(lanhost_path, "InternetAccess", 0, internet_access, sizeof(internet_access)) <= 0 )
			continue;

		if ( cfg_obj_get_object_attr(lanhost_path, "MAC", 0, mac_addr, sizeof(mac_addr)) <= 0 )
			continue;
		internet = atoi(internet_access);
		if ( 2 != internet )
		{
			cfg_obj_set_object_attr(lanhost_path, "InternetAccess", 0, "2");
			if ( 0 == internet )
			{
				snprintf(lanhost2black_path, sizeof(lanhost2black_path), LANHOST2BLACK_ENTRY_NODE, idx + 1);
				cfg_obj_delete_object(lanhost2black_path);
			}
		}
	}

	return 0;
}

int restore_dbus_param(void)
{
	mxml_node_t *source_romfile = NULL, *dest_romfile = NULL;
	char romfilepath[128]={0}, cmd[256] = {0};
	Key_Parameters_Info_Ptr pkeyPara = NULL;
	int retvalue = -1;

	/* create a new tree from filesystem default romfile, such as "/userfs/ctromfile.cfg". */
	memset(romfilepath,0,sizeof(romfilepath));
	getDefaultRomfile(romfilepath,sizeof(romfilepath));
	source_romfile = load_romfile(romfilepath);
	if ( NULL == source_romfile ) 
	{
		tcdbg_printf("restore_dbus_param romfile %s error", romfilepath);
		goto errorHandle;
	}

	/* Load an empty node. */
	dest_romfile = mxmlLoadString(NULL, "<ROMFILE></ROMFILE>", MXML_IGNORE_CALLBACK);
	if ( NULL == dest_romfile )
	{
		tcdbg_printf("restore_dbus_param romfile %s error", RUNNING_ROMFILE_PATH);
		goto errorHandle;
	}

	/* restore lanhost2 internet access. */
	restore_LANHost2_Access();
	cfg_commit_object("root.LANHost2");

	for ( pkeyPara = dbusRestorePara; (pkeyPara->flag & OP_AT_END) == 0; pkeyPara++ )
	{
		if ( pkeyPara->funp )
		{
			/* do restore */
			cfg_obj_delete_object(pkeyPara->node);
			cfg_obj_create_object(pkeyPara->node);
			retvalue = pkeyPara->funp(pkeyPara, source_romfile, dest_romfile); 		
			if(retvalue < 0)
			{
				goto errorHandle;
			}

			if ( 0 != strcmp("root.VPN", pkeyPara->node) ) /* VPN node not support */
				cfg_commit_object(pkeyPara->node);
		}
	}

	cfg_load_from_xml(dest_romfile); 		
	/*tcdbg_printf("[%s] do restore done! \n", __FUNCTION__);*/
errorHandle:
	if ( source_romfile )
	{
		mxmlDelete(source_romfile);
		source_romfile = NULL;
	}

	if ( dest_romfile )
	{
		mxmlDelete(dest_romfile);
		dest_romfile = NULL;
	}

	return 0;
}


#endif


int write_restore_syslog(char *source , int len, char *buf)
{
	char log[128] = {0};
	
	if( !buf || !buf[0] )
		return -1;
	
	switch (atoi(buf))
	{
		case 1:				
			snprintf(source, len - 1, "%s", "long reset button");
			break;	
		case 2: 			
			snprintf(source, len - 1, "%s", "short reset button");
			break;
		case 3:
			snprintf(source, len - 1, "%s", "WEB");
			break;	
		case 4: 			
			snprintf(source, len - 1, "%s", "ITMS");
			break;	
		default:
			break;				
	}

	return 0;
}


int write_upgrade_syslog(char *source, int len, char *buf)
{
	char log[128] = {0};
	char result[16] = {0};

	if( !buf || !buf[0] )
		return -1;
		
	switch (atoi(buf))
	{
		case 1:				
			snprintf(source, len - 1, "%s", "ITMS");
			break;			
		case 2:
			snprintf(source, len - 1, "%s", "WEB");
			break;	
		case 6:
			snprintf(source, len - 1, "%s", "DBus");
			break;	
		default:
			break;				
	}
	
	return 0;
}

int write_upgrade_type(char *source, int len, char *buf)
{
	char log[128] = {0};
	char result[16] = {0};

	if( !buf || !buf[0] )
		return -1;
		
	switch (atoi(buf))
	{
		case 1:				
			snprintf(source, len - 1, "%s", "Romfile");
			break;			
		case 2:
			snprintf(source, len - 1, "%s", "Image");
			break;	
		default:
			break;				
	}
	
	return 0;
}

int write_upgrade_check_result(char *source, int len, char *buf)
{
	char log[128] = {0};
	char result[16] = {0};

	if( !buf || !buf[0] )
		return -1;
		
	switch (atoi(buf))
	{
		case 1:				
			snprintf(source, len - 1, "%s", "Romfile check failed.");
			break;			
		case 2:
			snprintf(source, len - 1, "%s", "Could not get image header.");
			break;
		case 3:
			snprintf(source, len - 1, "%s", "Bad trx header.");
			break;
		case 4:
			snprintf(source, len - 1, "%s", "crc32 check failed.");
			break;
		case 5:
			snprintf(source, len - 1, "%s", "Lseek to tclinuxbin header failed.");
			break;
		case 6:
			snprintf(source, len - 1, "%s", "Lseek to mac in tcboot failed.");
			break;
		case 7:
			snprintf(source, len - 1, "%s", "Change the mac in tcboot failed.");
			break;	
		default:
			break;				
	}
	
	return 0;
}


#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
#define MAXLINE 				2048				/* maximum line length */
#define	SYSLOG_START_STRING		"StartOfTCSysLog"	/* the string in flash to indentify the start of log file */
#define SYSLOG_END_STRING		"EndOfTCSysLog"		/* The string in flash to identify the end of log file */

/*_____________________________________________________________________________
**      function name: get_logfile_size
**      descriptions:
**           get log file size
**
**      parameters:
**
**      global:
**             None
**
**      return:
**            Success:        0
**            Otherwise:     -1
**____________________________________________________________________________
*/
unsigned long get_logfile_size(char *fileName)
{
	unsigned long fileSize = 0;
	FILE *fp = NULL;
	
	fp = fopen(fileName, "r");
	if(fp == NULL) {
		tcdbg_printf("\n==>Syslog Error, open file %s failed\n", fileName);
		return 0;
	}
	
	fseek(fp, 0, SEEK_END);		//seek to the file end
	fileSize = ftell(fp);		//get the file size
	fseek(fp, 0, SEEK_SET);		//seek back to the file beginning 

	fclose(fp);
	
	return fileSize;	
}


/*_____________________________________________________________________________
**      function name: write_logMsg_to_flash
**      descriptions:
**            write log file to flash every five minite, this time can be 
**			  config of course
**
**      parameters:
**
**      global:
**             None
**
**      return:
**            Success:        0
**            Otherwise:     -1

**____________________________________________________________________________
*/
int  write_logMsg_to_flash(int flag)
{
	char cmds[128] = {0};
	char tmpPath[32] = "/var/log/tmp_file"; 
	char currLogFilePath[24] = "/var/log/currLogFile";
	FILE *fp = NULL;
	FILE *curr_fp = NULL;
	FILE *reset_fp = NULL, *upgrade_fp = NULL;
	FILE *upgrade_tfp = NULL, *upgrade_cfp = NULL;
	char line[64] = {0};
	char source_upgrade[64] = {0}, source_reset[64] = {0};
	char source_type[64] = {0}, source_check[64] = {0};
	char buf[MAXLINE + 1] = {0};
	unsigned long fileSize = 0;
	time_t t;
	struct tm *local;
	char new_timestamp[24];
	char *tbuf = NULL;
	size_t len = 0;
	
	/* append SYSLOG_END_STRING to currLogFile */
	curr_fp = fopen(currLogFilePath, "r");
	if(curr_fp == NULL){
		printf("\n==>Syslog Error, write logMsg, open file %s failed \n", currLogFilePath);
		return -1;
	}
	
	fp = fopen(tmpPath, "w");
	if(fp == NULL) {
		printf("\n==>Syslog Error, write logMsg, read open file %s failed \n", tmpPath);
		fclose(curr_fp);
		return -1;
	}
	/* Read log messages from currLogFile, and write into tmpFile */
	while(fgets(buf, MAXLINE + 1, curr_fp)) {
			fputs(buf, fp);
	}
	t = time(NULL);
	local = localtime(&t);
	sprintf(new_timestamp, "%d-%02d-%02d %02d:%02d:%02d", local->tm_year+1900,
		local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

#if defined(TCSUPPORT_CT_JOYME4)
	if( upgrade_tfp = fopen("/opt/upt/apps/info/upgrade_type", "r") )
	{
		if( getline(&tbuf, &len, upgrade_tfp) != -1 )
		{
			memset(source_type, 0, sizeof(source_type));
			write_upgrade_type(source_type, sizeof(source_type), tbuf);
		}
		fclose(upgrade_tfp);
		unlink("/opt/upt/apps/info/upgrade_type");
		if( tbuf )
			free(tbuf);
	}
	tbuf = NULL;
	len = 0;
	if( upgrade_cfp = fopen("/opt/upt/apps/info/upgrade_check_image", "r") )
	{
		if( getline(&tbuf, &len, upgrade_cfp) != -1 )
		{
			memset(source_check, 0, sizeof(source_check));
			write_upgrade_check_result(source_check, sizeof(source_check), tbuf);
		}
		fclose(upgrade_cfp);
		unlink("/opt/upt/apps/info/upgrade_check_image");
		if( tbuf )
			free(tbuf);
	}
	if( upgrade_fp = fopen("/opt/upt/apps/info/upgrade_syslog", "r") )
	{
		if(fgets(line, sizeof(line), upgrade_fp ))
		{
			memset(source_upgrade, 0, sizeof(source_upgrade));
			write_upgrade_syslog(source_upgrade, sizeof(source_upgrade), line);
		}
		fclose(upgrade_fp);
	}
	else if( reset_fp = fopen("/opt/upt/apps/info/reset_syslog", "r") )
	{
		if( fgets(line, sizeof(line), reset_fp) )
		{
			memset(source_reset, 0, sizeof(source_reset));
			write_restore_syslog(source_reset, sizeof(source_reset), line);
		}
		fclose(reset_fp);
	}
#endif

	switch (flag)
	{
		case e_Reboot:
#if defined(TCSUPPORT_CT_JOYME4)
			snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104001 Device reboot from WEB\r\n", new_timestamp);
			AddAlarmNumber("104001");
#else
			snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104001 Device reboot\r\n", new_timestamp);
#endif
			break;			
		case e_UpgradeFail:
#if defined(TCSUPPORT_CT_JOYME4)
			if( 0 != source_check[0] )
			{
				snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104057 %s \r\n", new_timestamp, source_check);
				fputs(buf, fp);
			}
			
			if(	!strcmp(source_type, "Romfile") )
			{
					snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104054 %s upgrade fail from %s\r\n", new_timestamp, source_type, source_upgrade);
			}
			else
			{
				if( !strcmp(source_upgrade, "WEB") )
					snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104060 Software upgrade fail from %s\r\n", new_timestamp, source_upgrade);
				else
					snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104058 Software upgrade fail from %s\r\n", new_timestamp, source_upgrade);
			}
#else
			snprintf(buf, sizeof(buf), "%s [Alert] alarm:id:104058 Software upgrade fail\r\n", new_timestamp);
#endif
			break;	
#if defined(TCSUPPORT_CT_JOYME4)
		case e_UpgradeSuccess:
			snprintf(buf, sizeof(buf), "%s [Alert] Software upgrade success from %s\r\n", new_timestamp, source_upgrade);
			break;	
		case e_Factory:
			snprintf(buf, sizeof(buf), "%s [Alert] Restore factory from %s\r\n", new_timestamp, source_reset);
			break;	
#endif
		default:
			break;				
	}

	fputs(buf, fp);
	sprintf(buf, "%s\r\n", SYSLOG_END_STRING);
	fputs(buf, fp);
		
	fclose(fp);
	fclose(curr_fp);

	/* Recaculate file size */
	fileSize = get_logfile_size(tmpPath);

	
	/* Last, write syslog messages to flash */
	memset(cmds, 0, sizeof(cmds));
	snprintf(cmds,sizeof(cmds), TC_FLASH_WRITE_CMD, tmpPath, fileSize, SYSLOG_RA_OFFSET, RESERVEAREA_NAME);
#ifdef SYSLOG_DEBUG
	tcdbg_printf("\n===%s[%d] cmds=%s===\n", __FUNCTION__, __LINE__, cmds);	
#endif
	system(cmds);
	
	unlink(tmpPath);
	return 0;
}
#endif

int sys_boot_effect(char *value)
{
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	int flag = -1;
#endif
	char cmd[128];
	char val[64] = {0};
	int mode = atoi(value);
	int ret = -1;

#if defined(TCSUPPORT_CT_JOYME2)	
	if ( cfg_obj_get_object_attr(SYSTEM_ENTRY_NODE, "restore_from", 0, val, sizeof(val)) > 0 )
	{
		if ( 0 == strcmp("ui", val) ){
#if defined(TCSUPPORT_CT_DBUS)
			doValPut(RESETSOURCEPATH, "3");
#if defined(TCSUPPORT_CT_JOYME4)
			doValPut("/opt/upt/apps/info/reset_syslog", "3");
#endif
#endif
			doValPut("/tmp/restore_source", "3");
		}

		cfg_set_object_attr(SYSTEM_ENTRY_NODE, "restore_from", "");
	}
#endif

	memset(cmd, 0, sizeof(cmd));
	/*Reboot type*/
	switch(mode)
	{
		case BOOT2CURSET:/*Reboot with current settings*/
#if defined(TCSUPPORT_CT_JOYME2)
			doValPut("/tmp/reboot_source", "Telecomadmin");
#if defined(TCSUPPORT_CT_JOYME4)
			doValPut("/opt/upt/apps/info/reboot_syslog", "Telecomadmin");
#endif
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
			write_logMsg_to_flash(e_Reboot);
#endif
#if defined(TCSUPPORT_CATV_GD)
			cfg_set_object_attr(SYSTEM_ENTRY_NODE, "reboot_type", "0");
#endif
			cfg_save_object(CFG_OBJ_FILE_PATH);
			#ifdef TCSUPPORT_BACKUPROMFILE
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
			flag = compute_checksum(CFG_OBJ_FILE_PATH,STORE_ROMFILE_FILE_NAME);
			if(flag < 0)
			{
				printf("\r\nsys_boot:Fail to compute checksum!\n");
				return FAIL;
			}
			sys_update_backup_romfile(STORE_ROMFILE_FILE_NAME);
			unlink(STORE_ROMFILE_FILE_NAME);
			_sys_delay_reboot();
#else
			sys_update_backup_romfile(CFG_OBJ_FILE_PATH);
			_sys_delay_reboot();
#endif
			#else
			snprintf(cmd, sizeof(cmd), REBOOT_CMD, CFG_OBJ_FILE_PATH);
			system(cmd);
			#endif
			break;
		case BOOT2DEFSET:/*Reboot with default factory settings*/
#if defined(TCSUPPORT_CT_UBUS)
			system("/usr/bin/prolinecmd restore default");
			break;
#endif
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
		case LONGRESET_BOOT2DEFSET:
#endif
#if defined(TCSUPPORT_CT_JOYME)
			restoreOSGIRight(mode);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			write_logMsg_to_flash(e_Factory);
#endif
#if defined(TCSUPPORT_CFG_NG_UNION)
			system("/usr/bin/prolinecmd restore default");
			break;
#endif
#if defined(TCSUPPORT_KEYPARA_STORE)
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
			if ( LONGRESET_BOOT2DEFSET == mode )
			{
				backupTR69WanIndex();
#if defined(TCSUPPORT_CT_JOYME2)	
				system("dbus-send --system --print-reply --dest=com.ctc.saf1 /com/ctc/saf1 com.ctc.saf1.framework.Restore");
#endif
			}
#endif


			defaultParaRestore();	

			ret = keyParaRestore(mode);	

			if(ret < 0)/*Have not done romfile update yet, so do update*/
			{
				cfg_set_object_attr(GLOBALSTATE_NODE, "isbootset", "1");
				system("/usr/bin/prolinecmd restore default");
				break;
			}
		
			_sys_delay_reboot();
			

#else
			system("/usr/bin/prolinecmd restore default");
#endif
			cfg_set_object_attr(GLOBALSTATE_NODE, "isbootset", "1");
#ifdef TCSUPPORT_CUC
			doValPut(RESETSOURCEPATH, "3");
			sync();
#endif
			doValPut("/tmp/rebootflag", "1");
			break;
		case WARM_REBOOT:/*Reboot with current settings*/
			system("reboot");
			break;
		case BOOT2DEFSETTR69:/*Reboot with default factory settings for tr069*/
#if defined(TCSUPPORT_CT_JOYME)
			restoreOSGIRight(mode);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			write_logMsg_to_flash(e_Factory);
#endif
#if defined(TCSUPPORT_CFG_NG_UNION)
			system("/usr/bin/prolinecmd restore default");
			break;
#endif
#if defined(TCSUPPORT_KEYPARA_STORE)
#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_CUC)
			/* check facotry reset mode. */
			if ( 0 == cuc_check_factory_reset() )
			{
				snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD, (unsigned long)0, "romfile");
				system(cmd);
				usleep(100000);
				snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD,
					(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
				system(cmd);
				usleep(100000);

				_sys_delay_reboot();
				break;
			}
#endif
#endif
#if defined(TCSUPPORT_CUC)	
			backupTR69WanIndex();
#endif
			defaultParaRestore();	

			ret = keyParaRestore(mode);	

			if(ret < 0)/*Have not done romfile update yet, so do update*/
			{
				system("/usr/bin/prolinecmd restore default");
				break;
			}
			

			_sys_delay_reboot();		
#else
			system("/usr/bin/prolinecmd restore default");
#endif
			doValPut("/tmp/rebootflag", "1");
#ifdef TCSUPPORT_CUC
			doValPut(RESETSOURCEPATH, "4");
			sync();
#endif
			break;			
			case BOOT2DEFSETITMS:
			restore2Defaulromile();
			_sys_delay_reboot();
			break;
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
		case BOOT2PUREFACTORY:
			restorePluginBC(mode);
			system("/usr/bin/prolinecmd restore default");
			break;
#endif
#if defined(TCSUPPORT_CUC)
		case CUC_LONGRESET_BOOT2DEFSET:
			system("/usr/bin/prolinecmd restore default");
			break;
#endif
#if defined(TCSUPPORT_CT_JOYME2)
		case JOYME2_RESTORE_NOT_KEYPARAM:
			restore_dbus_param();
			cfg_write_romfile_to_flash();
			cfg_set_object_attr(SYSTEM_ENTRY_NODE, "reboot_type", "0");
			break;
#endif
#if defined(TCSUPPORT_CUC)	
		case DBUS_RESTORE_KEYPARAM:
			doValPut(RESETSOURCEPATH, "5");
			sync();
			system("/usr/bin/prolinecmd restore default");
			break;
#endif

		case 0:/*Default value or error case, Do nothing*/
		default:
			break;
	}
	return SUCCESS;
}

int sys_boot(char *value)
{
	cfg_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, value, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_BOOT, (void *)&param, sizeof(param));
	return 0;
}

int sys_skbmgr(char *value)
{
	static int skbmgr_flag = 0;

	/*
	Update skbmgr_hot_list_len value
	ext: echo 512 > /proc/net/skbmgr_hot_list_len
	*/
	if(	(skbmgr_flag == 0 || atoi(value) != skbmgr_flag) && atoi(value) <= MAX_SKBMGR_LEN)
	{
		skbmgr_flag = atoi(value);
		doValPut(SKBMGR_PATH, value);
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

int getDownloadstate()
{
	FILE *fp = NULL;
	char downstate[8] = {0};
	int res = 0;

	fp = fopen(UPGRADE_CHECK_FILE, "r");
	if ( !fp )
		return 2;

	if ( (res = fread(downstate, sizeof(char), sizeof(downstate), fp)) <= 0 )
	{
		fclose(fp);
		return 3;
	}
	fclose(fp);

	return atoi(downstate);
}

unsigned long choose_type(char *file_path, unsigned char *firmware_type)
{
	struct stat buf;

	if(stat(UPLOAD_ROMFILE_PATH,&buf) == 0)
	{
		*firmware_type = UG_ROMFILE;
		strncpy(file_path, UPLOAD_ROMFILE_PATH, 31);
	}else if(stat(TCLINUX_PATH,&buf) == 0)
	{
		*firmware_type = UG_TCLINUX;
		strncpy(file_path, TCLINUX_PATH, 31);
	}
	return buf.st_size;
}

int get_parameter_value(char *buf, char *paremeter, char *ret_val, int buf_size)
{
	char *ret, *ret2;
	int i;
   	ret = strstr(buf,paremeter);

	if(ret != NULL)
	{
		ret = ret +strlen(paremeter);
		/*filename will be xxxxx&xxxxx when use ie to upgrade fw.
			It will cause CPE get wrong file name. 
			2009.9.15. shnwind modify*/
		ret2 = strchr(ret,'=');
		
		if(ret2 == NULL)
		{
			ret2 = buf + buf_size;/*last byte*/
		}
		for(i=0; i < (ret2 - ret) ; i++)
		{
			if(*(ret2 - i) == '&')
			{
				ret2 -= i;
				break;
			}	
		}
		if((ret2 == NULL) || (ret2 == ret) || (ret2-ret > buf_size))
		{
			return -1;
		}
		memset(ret_val,0,buf_size);
		memcpy(ret_val,ret,ret2-ret);
		ret_val[ret2-ret] = 0;
	}
	else
	{
		return -1;
	}
	return 0;
}

int html_post_parser(char *file_path, unsigned long *data_start, unsigned long *data_stop, unsigned char *firmware_type)
{
	char value[256] = {0};
	char *tmp;
	int fd, content_len;
	struct stat buf_st;
	int res = 0;
#if defined(TCSUPPORT_NP_CMCC)
	char model_name[128] = {0};

	cfg_get_object_attr(DEVICEINFO_DEVPARASTATIC_NODE, "ModelName", model_name, sizeof(model_name));
	if ( 0 == model_name[0] )
		strcpy(model_name, "AN");
#endif
	
	if(stat(MULTI_TO_NORMAL_TEMP, &buf_st) != 0)
	{
		return -1;
	}

	content_len = buf_st.st_size;
	tmp = (char*)malloc(content_len+1);
	if(tmp == NULL)
	{
		return -1;
	}
	fd = open(MULTI_TO_NORMAL_TEMP,O_RDONLY);
	if(fd < 0)
	{
		free(tmp);
		return -1;
	}

	if((res = read(fd,tmp,content_len)) < 0)
	{
		free(tmp);
		close(fd);
		return -1;
	}
	tmp[content_len] = '\0';
	close(fd);
	/*get filename*/
	if(get_parameter_value(tmp,"bfilename=", value, (content_len+1)) == FAIL){
		free(tmp);
		return -1;
	}

	/*choose upgrade type by name*/
	if(strstr(value,"romfile") != NULL)
	{
		*firmware_type = UG_ROMFILE;
	}
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	else if( NULL != strstr(value,"tclinux")
		&& NULL == strstr(value,"tclinux_allinone") )
	{
		*firmware_type = UG_TCLINUX;
	}
	else if( NULL != strstr(value,"tclinux_allinone") )
	{
		*firmware_type = UG_TCLINUX_ALLINONE;
	}
#else
	else if(strstr(value,"tclinux") != NULL)
	{
		*firmware_type = UG_TCLINUX;
	}
#endif
#if defined(TCSUPPORT_NP_CMCC)
	else if( strstr(value, model_name) )
	{
		*firmware_type = UG_TCLINUX;
	}
#endif
	else
	{
		printf("not support firmware\n");
		free(tmp);
		return -1;
	}
	/*get data start*/
	if(get_parameter_value(tmp,"bdata_start=", value, (content_len+1)) == -1){
		free(tmp);
		return -1;
	}

	*data_start = strtol(value, NULL, 10);
	/*get data end*/
	if(get_parameter_value(tmp,"bdata_end=", value, (content_len+1)) == -1){
		free(tmp);
		return -1;
	}
	*data_stop = strtol(value, NULL, 10);

	free(tmp);

	return 0;
}

#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_DBUS)
unsigned long determine_type(char *file_path, unsigned char *firmware_type)
{
	struct stat buf;

	if(stat(file_path,&buf) == 0)
	{
		*firmware_type = UG_TCLINUX;
	}

	return buf.st_size;
}
#endif

#if defined(TCSUPPORT_ANDLINK)
unsigned long determine_type1(char *file_path, unsigned char *firmware_type)
{
	struct stat buf;

	if(stat("/tmp/upgradetemp_firm",&buf) == 0)
	{
		*firmware_type = UG_TCLINUX;
		strncpy(file_path, "/tmp/upgradetemp_firm", 31);
	}
	else if(stat("/tmp/upgradetemp_cfg",&buf) == 0)
	{
		*firmware_type = UG_ROMFILE;
		strncpy(file_path, "/tmp/upgradetemp_cfg", 31);
	}

	return buf.st_size;
}

#endif

static const unsigned long crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

unsigned long calculate_crc32(int imagefd, long len)
{
	long length=len;	
	int check_len=0, i=0;
	char flag=0;/*for stop loop*/
	unsigned char *buf;
	unsigned long crc;
	int res = 0;
	
      	crc = 0xFFFFFFFF;
	
	/*Because read 1 byte at a time will spent much time, 
		we read a bigger size at a time and use this space to 
		do checksum. */
   	buf = (unsigned char *)malloc(CHECKSUM_TEMP_BUF_SIZE);
	if(buf == NULL)
		return 0;
 	while(flag == 0){
		/*decide add length*/
		if(length <= CHECKSUM_TEMP_BUF_SIZE){
			check_len=length;
			flag = 1;	
		}else{
			check_len = CHECKSUM_TEMP_BUF_SIZE;
		}

		length -= check_len;
		res = read(imagefd, buf, check_len); 
		
    		for(i=0;i < check_len;i++)
    		{
    			crc = UPDC32(*(buf+i), crc);	
		}  
	}  	

    free(buf);
    return ((unsigned long)crc);
}

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
/*modify the return value 
before-------return trx_header_len if success
now---------return 0 if success
*/
int
image_check_tc(int imagefd, char firmware_type, char *file_path,char *mtd, unsigned  int length
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE) || defined(TCSUPPORT_CT_JOYME2)
					, unsigned int *tclinuxbin_len
#endif
#if defined(TCSUPPORT_CT_JOYME2)
					, int upg_type, unsigned int *saflag, unsigned int *saflen
#endif
					)
{
	char img_buf[sizeof(struct trx_header)];
#if defined(TCSUPPORT_CT_JOYME2)
	char oldversion[64] = {0};
#endif
	struct trx_header *trx = (struct trx_header *) img_buf;
	int mtd_size = 0;
	int fd, buflen;
	int ret = -1;
	int readmtd = 1;
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	int cal_crc = 0;
	int filecrc = 0;
	unsigned char fcrcbuf[4] = {0};
	int allinone_hdr = 0;
#endif
	unsigned char curmac[6] = {0};
	int addr = 0;
	int size = 0;
	char bootFlag = -1;

	if(firmware_type == UG_ROMFILE)
	{
		strcpy(mtd,"romfile");
	}
	else if(firmware_type == UG_TCLINUX)
	{
#if defined(TCSUPPORT_CT_UPG_PINGPONG)
		blapi_system_get_current_boot_flag(&bootFlag);

		if (bootFlag == 1)
		{
			strncpy(mtd, "tclinux", 31);
		}
		else
		{
			strncpy(mtd, "tclinux_slave", 31);
		}
#else
		strcpy(mtd,"tclinux");
#endif
	}
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	else if ( UG_TCLINUX_ALLINONE == firmware_type )
	{
		readmtd = 0; /* no need to read mtd info for allinone*/
		allinone_hdr = lseek(imagefd, 0, SEEK_CUR); /* backup allinone header*/
		if ( -1 == allinone_hdr )
		{
			printf("[%s]: backup allinone header failed\n", __FUNCTION__);
			return -1;
		}
	}
#endif
	else
	{
		printf("image_check_tc:not support this type\n");
#if defined(TCSUPPORT_CT_JOYME2)
		return -2;
#else
		return -1;
#endif

	}
	
	if ( readmtd )
	{
		ret = blapi_system_get_mtdsize(mtd, &mtd_size, NULL);
		if(ret < 0)
			return -1;
	}
	
	switch(firmware_type)
	{
		case UG_ROMFILE:
			/*Check the romfile image is vaildly or not.*/
			close(imagefd);/*first close,because in function 'check_romfile' will open the file*/
			ret = check_romfile(file_path);
			if(ret < 0)
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 1 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("image_check_tc:romfile check failed!\n");
				return -1;
			}
			break;
			
		case UG_TCLINUX:
			/*Check the tclinux is vaildly or not.*/
			buflen = read(imagefd, img_buf, sizeof(struct trx_header));
			/*max len check*/
			if(mtd_size < trx->len)
			{
				printf("Image too big for partition: %s\n", mtd);
#if defined(TCSUPPORT_CT_JOYME2)
				return -3;
#else
				return -1;
#endif
			}
			/*mix len check*/
			if (buflen < sizeof(struct trx_header)) 
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 2 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("Could not get image header, file too small (%d bytes)\n", buflen);
				return -1;
			}
			/*magic check*/
			if ((trx->magic != TRX_MAGIC2) || (trx->len < sizeof(struct trx_header))) {
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 3 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("filesystem: Bad trx header\n");
				return -1;
			}
			/*crc check*/
			if(trx->crc32 != calculate_crc32(imagefd, (trx->len-sizeof(struct trx_header))))
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 4 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("crc32 check fail\n");
				return -1;
			}
#if defined(TCSUPPORT_CT_JOYME2)
			cfg_get_object_attr("root.System.Entry", "UpgradeSWVersion", oldversion, sizeof(oldversion));
			if(0 == strcmp(oldversion, trx->version))
			{
				if ( CTCDBUS_NO_HEADER == upg_type )
					return -4;
			}

			if ( HTML_HEADER == upg_type )
			{
				/* save sw version when upgrade via webpage. */
				cfg_evt_t param;
				bzero(&param, sizeof(param));
				cfg_obj_send_event(EVT_CFG_INTERNAL,EVT_CFG_TCAPI_SAVE,(void *)&param, sizeof(param));
			}

			if(trx->saflag == 1)
			{
				*saflag = trx->saflag;
				*saflen = trx->saflen;
				buflen = read(imagefd, img_buf, sizeof(struct trx_header));
				*tclinuxbin_len = trx->len;
			}
#endif
			break;
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
		case UG_TCLINUX_ALLINONE:
			/* 1. move to tclinux area then check*/
			ret = blapi_system_get_tcboot_size(&size);
			
			if( -1L == lseek(imagefd, size + ROMFILE_SIZE, SEEK_CUR) )
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 5 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: lseek to tclinuxbin header failed\n", __FUNCTION__);
				return -1;
			}
			/* check the tclinux is vaildly or not.*/
			buflen = read(imagefd, img_buf, sizeof(struct trx_header));
			/*mix len check*/
			if (buflen < sizeof(struct trx_header)) 
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 2 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: could not get image header, file too small (%d bytes)\n", __FUNCTION__, buflen);
				return -1;
			}
			/*magic check*/
			if ((trx->magic != TRX_MAGIC2) || (trx->len < sizeof(struct trx_header)))
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 3 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: Bad trx header trx->magic=[%X], TRX_MAGIC2=[%X], trx->len=[%X] \n", __FUNCTION__, trx->magic, TRX_MAGIC2, trx->len);
				return -1;
			}
			/* 2.move to allinone header again to calculate crc*/
			if( -1L == lseek(imagefd, allinone_hdr, SEEK_SET) )
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 5 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: lseek to allinone header failed\n", __FUNCTION__);
				return -1;
			}
			/* check allinone crc*/
			cal_crc = calculate_crc32(imagefd, length - CKSUM_LENGTH);
			buflen = read(imagefd, fcrcbuf, CKSUM_LENGTH);
			filecrc |= fcrcbuf[0];
			filecrc |= fcrcbuf[1] << 8;
			filecrc |= fcrcbuf[2] << 16;
			filecrc |= fcrcbuf[3] << 24;
			if ( buflen != CKSUM_LENGTH || cal_crc != filecrc )
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 4 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: crc32 check fail cal_crc=[%X], filecrc=[%X].\n", 
					__FUNCTION__, cal_crc, filecrc);
				return -1;
			}
			*tclinuxbin_len = trx->len;
			/* all is done, modify the MAC address.*/

			ret = blapi_system_get_EtherAddr_offset(&addr);

			if( -1L == lseek(imagefd, allinone_hdr + addr, SEEK_SET) )
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 6 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: lseek to mac in tcboot failed.\n", __FUNCTION__);
				return -1;
			}
			if ( 0 != blapi_system_get_EtherAddr(curmac) )				
				return -1;
			if ( 6 != write(imagefd, curmac, 6) )
			{
#if defined(TCSUPPORT_CT_JOYME4)
				system("echo -n 7 > /opt/upt/apps/info/upgrade_check_image");
#endif
				printf("[%s]: change the mac in tcboot failed.\n", __FUNCTION__);
				return -1;
			}
			break;
#endif
			
		default:
			/*Not supprot this type*/
			return -1;
	}

	return 0;
}/*end image_check_tc*/
#else
int
image_check_tc(int imagefd, char firmware_type, char *file_path,char *mtd, unsigned  int length)
{
	char img_buf[sizeof(struct trx_header)];
	struct trx_header *trx = (struct trx_header *) img_buf;
	int mtd_size = 0;
	int fd = 0, buflen = 0;
	mxml_node_t *tree=NULL;
	FILE *fp=NULL;
	struct stat fs;
	int romfile_length = 0, get_len = 0;
	int ret = -1;
	
	/*get trx header*/

	if(firmware_type != UG_ROMFILE){
		buflen = read(imagefd, img_buf, sizeof(struct trx_header));

		if (buflen < sizeof(struct trx_header)) 
		{
			printf("Could not get image header, file too small (%d bytes)\n", buflen);
			return -1;
		}
	}

	switch(firmware_type)
	{
		case UG_ROMFILE:
			/*get romfile for verify*/
			romfile_length = length;
			fp = fopen(UPLOAD_ROMFILE_PATH, "w");
			if(fp == NULL)
			{
				return -1;
			}
			while(romfile_length > 0)
			{
				if(romfile_length >= sizeof(struct trx_header))
				{
					get_len = sizeof(struct trx_header);
				}
				else
				{
					get_len = romfile_length;
				}
				romfile_length -= get_len;

				read(imagefd, img_buf, get_len);
				fwrite(img_buf,get_len,1,fp);
			}
			fclose(fp);
			/*Check the romfile image is vaildly or not.*/
			
			ret = cfg_load_object(UPLOAD_ROMFILE_PATH);
			strncpy(mtd, "romfile", 31);
			break;
		case UG_TCLINUX:
			/*Check the tclinux is vaildly or not.*/
			if ((trx->magic != TRX_MAGIC2) || (trx->len < sizeof(struct trx_header))) 
			{
				printf("filesystem: Bad trx header\n");
				return -1;
			}
			strncpy(mtd,"tclinux", 31);
			break;
		default:
			/*Not supprot this type*/
			return -1;
	}
	/*check crc32*/
	if(firmware_type != UG_ROMFILE)
	{
		/*ROMFILE does not have trx header*/
		if(trx->crc32 != calculate_crc32(imagefd, (trx->len-sizeof(struct trx_header))))
		{
			printf("crc32 check fail\n");
			return -1;
		}
	}

	ret = blapi_system_get_mtdsize(mtd, &mtd_size, NULL);
	if(ret < 0)
		return -1;

	if(firmware_type == UG_ROMFILE)
	{
		/*ROMFILE isn't include trx header, so we check the file size*/
		if(stat(UPLOAD_ROMFILE_PATH, &fs) == 0)
		{
			if(mtd_size < fs.st_size)
			{
				printf("Image too big for partition: romfile\n");
				return -1;
			}
		}

	}
	else
	{
		/*Check the image size*/
		if(mtd_size < trx->len) 
		{
			printf("Image too big for partition: %s\n", mtd);
			return -1;
		}
	}

	return trx->header_len;
}/*end image_check_tc*/
#endif

#ifndef TCSUPPORT_UPGRADE_WDOGREBOOT
void start_reboot()
{
	/*function sleep is stopped by signal*/
#if 0
	sleep(2);
#else
	int n=2;
	do
	{
		n=sleep(n);
	}while(n>0);
#endif	
	fflush(stdout);
	fflush(stderr);
	syscall(SYS_reboot,LINUX_REBOOT_MAGIC1,LINUX_REBOOT_MAGIC2,LINUX_REBOOT_CMD_RESTART,NULL);
}
#endif

#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
int upgrade_done = 0;
#endif

#if defined(TCSUPPORT_IPV6)
int upgSysIPVersionHandle()
{
	char nodeName[64] = {0};
	char ipVersion[10] = {0};
	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, "root.Sys.Entry", sizeof(nodeName)-1);
	
	cfg_obj_get_object_attr(nodeName, "IPProtocolVersion", 0, ipVersion, sizeof(ipVersion));	
	if(ipVersion[0] != '\0')
	{
		if((0 == strcmp(ipVersion, "1")) || (0 == strcmp(ipVersion, "2")))
		{
			cfg_set_object_attr(nodeName, "IPProtocolVersion", "3");/* change to v4/v6*/
		}
	}

	cfg_type_default_func_commit("root.Sys");
	cfg_evt_write_romfile_to_flash();
	printf("\nSys IPVersion is changed to IPv4/IPv6! \n");
	
	return 0;
}
#endif

int getBackupMTD(char *mtdname)
{
	char cmd[400] = {0}, envbuf[64] = {0}, dbuscmd[256] = {0};
	char *buf = NULL, *p = NULL, tmpname[10] = {0};
	size_t len = 0;
	FILE *fp = NULL;
	int i = 0, mtdnum = -1;

	snprintf(envbuf, sizeof(envbuf), "export LD_LIBRARY_PATH=/usr/lib/glib-2.0/");
	snprintf(dbuscmd, sizeof(dbuscmd), "dbus-send --system --print-reply --dest="GDBUS_SAF" "GDBUS_SAF_PATH" "GDBUS_IFACE_PROPERTIES".Get string:\""GDBUS_SAF".framework\" string:\"BackupMTD\" > /tmp/backupmtd");
	snprintf(cmd, sizeof(cmd), "%s && %s", envbuf, dbuscmd);
	system(cmd);

	fp = fopen("/tmp/backupmtd", "r");
	if ( !fp )
		return -1;
	
	while ( getline(&buf, &len, fp) != -1 )
	{
		if ( 0 == i )
		{
			i++;
			continue;
		}
		p = strstr(buf, "byte ");
		if(p)
			mtdnum = atoi(p + strlen("byte "));
	}
	
	fclose(fp);
	unlink("/tmp/backupmtd");
	if(mtdnum < 0)
	{
		if(buf)
			free(buf);
		return -1;
	}
	
	snprintf(tmpname, sizeof(tmpname), "mtd%d", mtdnum);
	fp = fopen("/proc/mtd", "r");
	if ( !fp )
	{
		if(buf)
			free(buf);
		return -1;
	}
	
	while ( getline(&buf, &len, fp) != -1 )
	{
		if(strstr(buf, tmpname))
		{
			sscanf(buf, "%*[^\"]\"%[^\"]\"", mtdname);
			break;
		}
	}
	
	fclose(fp);
	if(buf)
		free(buf);
	return 0;
}

void getPartitionSize(char *s, char *img, char *mtdname, int *imgsize, int *optsize)
{
	char *token = NULL;
	char tmpbuf[32] = {0}, tmpvalue[256] = {0};

	strncpy(tmpvalue, s, sizeof(tmpvalue) - 1);
	token = strtok(tmpvalue, ",");
	while(token)
	{
		if(strstr(token, img))
		{
			sscanf(token, "%[^m]m%*[^ ]", tmpbuf);
			*imgsize = atoi(tmpbuf) * 1024 * 1024;
		}
		else if(strstr(token, mtdname))
		{
			sscanf(token, "%[^m]m%*[^ ]", tmpbuf);
			*optsize = atoi(tmpbuf) * 1024 * 1024;
		}
		token = strtok(NULL, ",");
	}
	
	return;
}
int upgarde_fw(int type)
{
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE) || !defined(TCSUPPORT_UPGRADE_WDOGREBOOT)
	pthread_t thread_id;
#endif
	char nodeName[64] = {0};
	char web_upgrade_flag[5] = {0};
#if defined(TCSUPPORT_CT_JOYME2)
	unsigned int saflag = 0;
	unsigned int saflen = 0;
	char mtdname[16] = {0}, mtdname2[16] = {0};
	int checkVal = -1, imgsize = 0, optsize = 0, i = 0;
	char bootflag[10] = {0};
	char envbuf[64] = {0}, dbuscmd[256] = {0};
#endif
#if defined(TCSUPPORT_CT_ZIPROMFILE)
	int ret = 0;
#endif
	int fd=0;
	char cmd_buf[300] = {0};
	char img[32] = {0};
	char file_path[64] = {0};
	unsigned int offset=0 , length=0;
	unsigned char firmware_type;
	unsigned long data_start, data_stop;
	int header_len=0;
#if defined(TCSUPPORT_CT)	
#ifdef CWMP	
	int n = 0;
	cwmp_msg_t message;
	#define CWMP_REBOOT  10
	memset(&message,0,sizeof(cwmp_msg_t));
	message.cwmptype = CWMP_REBOOT;
#endif
#endif
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE) || defined(TCSUPPORT_CT_JOYME2)
	unsigned int tclinux_len = 0;
#if defined(TCSUPPORT_CT_DUAL_IMAGE) || defined(TCSUPPORT_CUC_DUAL_IMAGE)
	int tclinux_slave_len = 0;
#endif
#endif
#if defined(TCSUPPORT_CT_JOYME)
	int eventInform = 0;
	char upgrade_ID[128] = {0};
	char newversion[64] = {0};
	char currversion[64] = {0};
	char nodeUPGName[64] = {0};
	char attrName[32] = "version=";
#endif
	int res = 0;
#if defined(TCSUPPORT_ANDLINK)
	char reboot[4] = {0};
#endif
	int size = 0, retcd = 0;

	memset(cmd_buf, 0, sizeof(cmd_buf));
	memset(img, 0, sizeof(img));
	memset(file_path, 0, sizeof(file_path));
	memset(nodeName, 0, sizeof(nodeName));
	memset(web_upgrade_flag, 0, sizeof(web_upgrade_flag));

#if defined(TCSUPPORT_CT_JOYME)
	if ( PHONE_NO_HEADER == type )
	{
		memset(nodeUPGName, 0, sizeof(nodeUPGName));
		strncpy(nodeUPGName, "root.OSGIUpgrade.Entry", sizeof(nodeUPGName)-1);

		memset(upgrade_ID, 0, sizeof(upgrade_ID));
		cfg_obj_get_object_attr(nodeUPGName, "upgrade_ID", 0, upgrade_ID, sizeof(upgrade_ID));
		memset(newversion, 0, sizeof(newversion));
		cfg_obj_get_object_attr(nodeUPGName, "fwver", 0, newversion, sizeof(newversion));
		memset(currversion, 0, sizeof(currversion));
		get_profile_str_new( attrName, currversion, sizeof(currversion)
			, DEVICEPARASTATIC_PATH);

		/* report error if download fail */
		if ( 0 != getDownloadstate() )
		{
			sendEventInform("1", currversion, INFORM_ERR_DLDERR, upgrade_ID);
			sleep(1);
			return -1;
		}
		else if ( 0 == strcmp(currversion, newversion) )
		{
			sendEventInform(INFORM_RESULT_FAIL, currversion
				, INFORM_ERR_SAMEVERSION, upgrade_ID);
			sleep(1);
			return -1;
		}
	}
#endif
	

	strncpy(nodeName, "root.WebCustom.Entry", sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, "web_upgrade", 0, web_upgrade_flag, sizeof(web_upgrade_flag)) < 0)
		snprintf(web_upgrade_flag, sizeof(web_upgrade_flag), "%d", 0);
	else
		cfg_set_object_attr(nodeName, "web_upgrade", "0");
	
	#ifdef DEBUG
	printf("sys_upgrade_firmware type:%d\r\n", type);
	#endif

	/*decide firmware type, length, start offset, and file path. */
	switch(type)
	{
		case NO_OPERATION:
			return 0;
		case NO_HEADER:
#if defined(TCSUPPORT_CT_JOYME4)
			system("echo -n 1 > /opt/upt/apps/info/upgrade_syslog");
#endif
			length = choose_type(file_path, &firmware_type);
			offset = 0;
			break;
		case HTML_HEADER:
#if defined(TCSUPPORT_CT_JOYME4)
			system("echo -n 2 > /opt/upt/apps/info/upgrade_syslog");
#endif
			strncpy(file_path,"/tmp/boa-temp", sizeof(file_path)-1);
			html_post_parser(file_path,&data_start,&data_stop,&firmware_type);
			unlink(MULTI_TO_NORMAL_TEMP);
			offset = data_start;
			length = data_stop - data_start + 1;
			break;
#if defined(TCSUPPORT_CT_PHONEAPP)
		case PHONE_NO_HEADER:
			strncpy(file_path,"/tmp/phone-temp", sizeof(file_path)-1);
			length = determine_type(file_path, &firmware_type);
			offset = 0;
#if defined(TCSUPPORT_CT_JOYME)
			eventInform = 1;
#endif
			break;			
#endif
#if defined(TCSUPPORT_CT_DBUS)
		case CTCDBUS_NO_HEADER:
#if defined(TCSUPPORT_CT_JOYME4)
			system("echo -n 6 > /opt/upt/apps/info/upgrade_syslog");
#endif
			snprintf(file_path, sizeof(file_path), "%s", "/tmp/ctc-upgradetemp");
			length = determine_type(file_path, &firmware_type);
			offset = 0;
			break;
#endif
#if defined(TCSUPPORT_NP_CMCC)
		case NP_NO_HEADER:
			length = determine_type1(file_path, &firmware_type);
			offset = 0;
			break;
#endif
#if defined(TCSUPPORT_CT_JOYME4_JC)
		case USB_NO_HEADER:
			if(cfg_obj_get_object_attr(SYSTEM_ENTRY_NODE, "usb_upgrade_fw", 0, file_path, sizeof(file_path)) < 0)
			{
				return -1;
			}
			tcdbg_printf("%s:%d file_path=%s\n",__FUNCTION__,__LINE__,file_path);
			length = determine_type(file_path, &firmware_type);
			offset = 0;
			break;
#endif
		default:
			unlink(file_path);
			return -1;
	}
#if defined(TCSUPPORT_CT_JOYME4)
	if( UG_ROMFILE == firmware_type )
		system("echo -n 1 > /opt/upt/apps/info/upgrade_type");
	else
		system("echo -n 2 > /opt/upt/apps/info/upgrade_type");
#endif
	ret = blapi_system_secure_upgrade(256, offset, file_path, firmware_type);

#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	if ( UG_TCLINUX_ALLINONE == firmware_type )
		fd = open(file_path, O_RDWR);
	else
#endif
	/*check the firmware.*/
	fd = open(file_path, O_RDONLY);
	if(fd < 0){
		unlink(file_path);
#if defined(TCSUPPORT_CT_JOYME)
		if (eventInform )
			sendEventInform("1", currversion, INFORM_ERR_BINERR, upgrade_ID);
#endif
		return -1;
	}
	else
	{
#if defined(TCSUPPORT_CT_ZIPROMFILE)
		if(strcmp(web_upgrade_flag, "1") == 0 && firmware_type == UG_ROMFILE){
			ret = update_UploadedRomfile(fd, offset, length, "/tmp/tmp_romfile.cfg");
			if(ret < 0)
			{
				close(fd);
				return -1;
			}
			else
			{
				close(fd);				
				unlink(file_path);
				
				retcd = compute_checksum("/tmp/tmp_romfile.cfg", file_path);

				unlink("/tmp/tmp_romfile.cfg");

				snprintf(cmd_buf, sizeof(cmd_buf), "cp -f %s %s", file_path, "/tmp/tmp_romfile.cfg");
				system(cmd_buf);

				fd = open(file_path, O_RDONLY);
				if(fd < 0)
				{
					unlink(file_path);
					return -1;
				}
			}
		}
		else
		{
			res = lseek(fd,offset,SEEK_CUR);
		}
		
#else
		lseek(fd,offset,SEEK_CUR);
#endif

#if defined(TCSUPPORT_PON_ROSTELECOM)
		add_node_if = 2 ;
#endif

		header_len = image_check_tc(fd,firmware_type,file_path,img,length
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE) || defined(TCSUPPORT_CT_JOYME2)
				, &tclinux_len
#endif
#if defined(TCSUPPORT_CT_JOYME2)
				, type, &saflag, &saflen
#endif
				);

#if defined(TCSUPPORT_CT_JOYME2)
		switch(header_len)
		{
			case -2:
				cfg_set_object_attr("root.System.Entry", "UpgradeStatus", "1");
				break;
			case -3:
				cfg_set_object_attr("root.System.Entry", "UpgradeStatus", "3");
				break;
			case -4:
				cfg_set_object_attr("root.System.Entry", "UpgradeStatus", "4");
				break;
			case 0:
				cfg_set_object_attr("root.System.Entry", "UpgradeStatus", "6");
				break;
			default:
				cfg_set_object_attr("root.System.Entry", "UpgradeStatus", "5");
				break;
		}
#endif

#if defined(TCSUPPORT_CT_JOYME2)
		if( header_len < 0)
#else
		if( header_len == -1)
#endif
		{
		if(UG_ROMFILE != firmware_type)
		{
			close(fd);
		}
			unlink(file_path);
#if defined(TCSUPPORT_CT_JOYME)
			if ( eventInform )
				sendEventInform("1", currversion, INFORM_ERR_BINERR, upgrade_ID);
#endif
			return -1;
		}
		close(fd);
		/*remove romfile tmp file*/
		unlink(UPLOAD_ROMFILE_PATH);
		
#if defined(TCSUPPORT_CT_ZIPROMFILE)
		if(strcmp(web_upgrade_flag, "1") == 0 && firmware_type == UG_ROMFILE)
		{
			unlink(file_path);

			snprintf(cmd_buf, sizeof(cmd_buf), "mv -f %s %s", "/tmp/tmp_romfile.cfg", file_path);
			system(cmd_buf);
		}
#endif

	}
	/*Decide write length and offset.*/
	switch(firmware_type)
	{
		case UG_TCLINUX:
			break;
		case UG_ROMFILE:
		default:
			break;
	}
#if defined(TCSUPPORT_CT_JOYME)
	if ( eventInform )
		sendEventInform("0", newversion, "", upgrade_ID);
#endif

#if defined(TCSUPPORT_FW_INTERNET_LED)
	doValPut("/proc/tc3162/led_internet", "0 8\n");
	printf("upgrade_fw or romfile\n");
#endif

	if(strcmp(web_upgrade_flag, "1") == 0 && (firmware_type == UG_TCLINUX
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	|| UG_TCLINUX_ALLINONE == firmware_type
#endif
	)){
		system("/usr/bin/killall -9 tftpd 2>/dev/null");
#ifndef TCSUPPORT_UPGRADE_WDOGREBOOT
		system("killall -9 tcwdog");
#endif
	}

#if defined(TCSUPPORT_CT)	
	printf("Please Waiting,CPE is erasing flash......\n");
#if defined(TCSUPPORT_CT_ZIPROMFILE)
	if(strcmp(web_upgrade_flag, "1") == 0 && firmware_type == UG_ROMFILE)
		snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s",file_path, length, 0 ,img);
	else
#endif
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	if ( UG_TCLINUX_ALLINONE == firmware_type )
	{
		/* write bootloader*/
		ret = blapi_system_get_tcboot_size(&size);
		
		snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s", 
		file_path, size, offset, "bootloader");
		system(cmd_buf);
		/* write romfile*/
		snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s", 
		file_path, ROMFILE_SIZE, offset + size, "romfile");
		system(cmd_buf);
		/* write tclinux*/
		snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s", 
		file_path, tclinux_len, offset + size + ROMFILE_SIZE, "tclinux");
#if defined(TCSUPPORT_CT_DUAL_IMAGE) || defined(TCSUPPORT_CUC_DUAL_IMAGE)
		if ( (tclinux_slave_len = length - size - ROMFILE_SIZE - TCLINUX_SIZE - CKSUM_LENGTH) > 0 )
		{
			system(cmd_buf);
			/* write tclinux*/
			snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s", 
			file_path, tclinux_slave_len, offset+ size + ROMFILE_SIZE + TCLINUX_SIZE, "tclinux_slave");
		}
#endif
	}
	else
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	if ( UG_TCLINUX == firmware_type)
	{
		if(saflag == 1)
		{
			checkVal = getBackupMTD(mtdname);
			getPartitionSize(TCSUPPORT_PARTITIONS_CMDLINE_STR, img, mtdname, &imgsize, &optsize);
			if(tclinux_len <= imgsize)
			{
				snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s", file_path, imgsize, offset, img);
				system(cmd_buf);
			}
			if(saflen <= optsize && checkVal >= 0)
			{
				snprintf(envbuf, sizeof(envbuf), "export LD_LIBRARY_PATH=/usr/lib/glib-2.0/");
				snprintf(dbuscmd, sizeof(dbuscmd), "dbus-send --system --print-reply --dest=com.ctc.saf1 /com/ctc/saf1 com.ctc.saf1.framework.Pause uint32:\"10\"");
				snprintf(cmd_buf, sizeof(cmd_buf), "%s && %s", envbuf, dbuscmd);
				system(cmd_buf);
				sleep(5);
				snprintf(cmd_buf, sizeof(cmd_buf), "mtd erase %s", mtdname);
				system(cmd_buf);
				snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s", file_path, optsize, offset+imgsize, mtdname);
				system(cmd_buf);
				sleep(5);
				snprintf(dbuscmd, sizeof(dbuscmd), "dbus-send --system --print-reply --dest=com.ctc.saf1 /com/ctc/saf1 com.ctc.saf1.framework.Switch");
				snprintf(cmd_buf, sizeof(cmd_buf), "%s && %s", envbuf, dbuscmd);
				system(cmd_buf);
				sleep(5);
				snprintf(dbuscmd, sizeof(dbuscmd), "dbus-send --system --print-reply --dest=com.ctc.saf1 /com/ctc/saf1 com.ctc.saf1.framework.Pause uint32:\"0\"");
				snprintf(cmd_buf, sizeof(cmd_buf), "%s && %s", envbuf, dbuscmd);
				system(cmd_buf);
				sleep(10);
			}
		}
		else
		{
			snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s",
			file_path, length, offset ,img);
		}
	}
#else
	snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s",
	file_path, length, offset ,img);
#endif
#else
	snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -rf write %s %d %d %s &",
	file_path, length, offset ,img);
#endif

#if defined(TCSUPPORT_PON_ROSTELECOM)
	/*1.if type = romfile,and has new romfile, use new romfile to upgrade*/
	/*2.html_post_parser,get paras; compute checksum*/
	/*3.erase flash and delete file*/
	if( (UG_ROMFILE == firmware_type) && (add_node_if == 1) )
	{
		memset(file_path, 0, sizeof(file_path));
		memset(cmd_buf, 0, sizeof(cmd_buf));
		strncpy(file_path, FRAG_ROMFILE_PATH, sizeof(file_path)-1);
		
		html_post_parser(file_path,&data_start,&data_stop,&firmware_type);
		unlink(MULTI_TO_NORMAL_TEMP);
		offset = data_start;
		length = data_stop - data_start + 1;

		compute_checksum(file_path , "/tmp/boa-temp-ct");
		
		snprintf(cmd_buf, sizeof(cmd_buf), "/userfs/bin/mtd -f write %s %d %d %s",
			"/tmp/boa-temp-ct", length, 0 ,img);
	}
	add_node_if = 0 ;
#endif
		system(cmd_buf);
#if defined(TCSUPPORT_CT)
#if defined(TCSUPPORT_CT_UPG_PINGPONG)
		 blapi_system_set_boot_flag(img);
#endif
#if defined(TCSUPPORT_CT_JOYME2)
		cfg_get_object_attr("root.System.Entry", "needreboot", bootflag, sizeof(bootflag));
		if(0 == strcmp(bootflag, "0"))
		{
#if defined(TCSUPPORT_CT_UBUS)
			system("reboot -d 2 &");
#endif
			return 0;
		}	
#endif

#ifdef CWMP
	n = sendmegq(1,&message, 0);
	if(n < 0)
		printf("send message error\n");
#endif	
#endif
#if defined(TCSUPPORT_IPV6)
	if(NO_HEADER == type)
	{
		upgSysIPVersionHandle();
	}
#endif

	if(strcmp(web_upgrade_flag, "1") == 0)
	{
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
		char msg_value[32] = {0};
		snprintf(msg_value, sizeof(msg_value), "%d", firmware_type);
		cfg_set_object_attr("root.System.Entry", "upgrade_fw_type",msg_value);
#endif
#ifndef TCSUPPORT_UPGRADE_WDOGREBOOT
		if( pthread_create(&thread_id, NULL, (void *)start_reboot, NULL) != 0 )
		{
			system("reboot -d 2 &");
			tcdbg_printf("pthread_create start_reboot error!!\n");
		}
#endif
	}
	return 0;

}

int sys_evt_upgrade_firmware(char *value)
{
	cfg_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, value, sizeof(param)-1);
	cfg_obj_send_event(EVT_CFG_INTERNAL,EVT_CFG_UPGRADE_FIRMWARE,(void *)&param, sizeof(param));

	while (0 != strcmp(value,"0") && 0 != access(UPGRADE_FW_STATUS_FILE, F_OK) )
	{
		sleep(1);
	}
	return 0;
}

int sys_upgrade_firmware(char *value)
{
	int retval=0;
	char msg_value[32] = {0};
#if defined(TCSUPPORT_CT_JOYME2)
	char bootflag[10] = {0}, tmpBuf[10] = {0};
	int sec = 0;
#endif
#if defined(TCSUPPORT_ANDLINK)
	char reboot[4] = {0};
#endif

	retval=upgarde_fw(atoi(value));
	if(retval == 0)
	{
		/*set firmware upgrade status*/
		if ( NO_OPERATION == atoi(value) )
			cfg_set_object_attr("root.System.Entry", "upgrade_fw_status", "NONE");
		else
		{
			cfg_set_object_attr("root.System.Entry", "upgrade_fw_status", "SUCCESS");
#if defined(TCSUPPORT_CT_JOYME4)
			write_logMsg_to_flash(e_UpgradeSuccess);
#endif
#if defined(TCSUPPORT_CT_JOYME2)
			cfg_set_object_attr("root.System.Entry", "UpgradeStatus", "7");
#endif

		}
			
	}
	else
	{
		/*set firmware upgrade status*/
		cfg_set_object_attr("root.System.Entry", "upgrade_fw_status", "FAIL");
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
		write_logMsg_to_flash(e_UpgradeFail);
#endif
	}
	if(0 != strcmp(value,"0"))
	{
		write2file("1",UPGRADE_FW_STATUS_FILE);
	}
	/*reset the upgrade_fw flag*/
	snprintf(msg_value, sizeof(msg_value), "%d", NO_OPERATION);
	cfg_set_object_attr("root.System.Entry", "upgrade_fw", msg_value);
#if defined(TCSUPPORT_CT_JOYME2)
	cfg_get_object_attr("root.System.Entry", "needreboot", bootflag, sizeof(bootflag));
	if(0 == strcmp(bootflag, "0"))
	{
		cfg_set_object_attr("root.System.Entry", "needreboot", "");
		cfg_evt_write_romfile_to_flash_raw();
		return retval;
	}	
	
	cfg_evt_write_romfile_to_flash_raw();
	cfg_set_object_attr("root.Cwmp.Entry","FWCompleteFlag","0");
	while(1)
	{
		cfg_get_object_attr("root.Cwmp.Entry","FWCompleteFlag",tmpBuf, sizeof(tmpBuf));
		if('\0' != tmpBuf[0] && 1 == atoi(tmpBuf))
			break;
		sleep(1);
		sec++;
		if(sec > 4)
			break;
	}
	
#endif

#if !defined(TCSUPPORT_ANDLINK)
#if !defined(TCSUPPORT_UPGRADE_NO_REBOOT)

#ifdef PURE_BRIDGE
	/*Reboot after upload file wether the result is success or fail.*/
	if(retval == -1)
	{
		system("reboot -d 1 &");
	}
#endif
#ifdef WITHVOIP
	/*Reboot after upload file wether the result is success or fail.*/
	if(retval == -1)
	{
		system("reboot -d 1 &");
	}
#endif
#if defined(TCSUPPORT_CT_E8GUI)
#if defined(TCSUPPORT_CT_FW_UPGRADE_16M)
	if(retval == -1)
	{
		system("reboot -d 1 &");
	}
#endif
#endif

#endif
#endif

	return retval;
}

#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
int sys_start_up(char *value)
{
	cfg_node_obj_t *all_cfg_node=NULL;
	int res = 0;
#if defined(TCSUPPORT_CRJO)	
	bool nodeTable_flag = 0;
#endif		
	pthread_attr_t thread_attr;
	char cc_is_run[8] = {0};
	
	if(atoi(value) == 1)
	{
		all_cfg_node=cfg_obj_get_root();
		if(all_cfg_node)
		{
#if 0
			for(ptr = all_cfg_node; ptr->name!= NULL; ptr++)
			{
				node = mxmlFindElement(tree, tree, ptr->name,
			  		NULL, NULL,MXML_DESCEND);
				if((node !=NULL) && (ptr->cfg_boot!=NULL))
				{
					ptr->cfg_boot(tree);
				}
#if defined(TCSUPPORT_CRJO)				
				if(strcmp(ptr->name, "LogicID") == 0)
					break;	
#endif				
			}
#endif
			system("msg call 4097 boot");
		}
		cfg_obj_get_object_attr(GLOBALSTATE_PRESYSSTATE_NODE, "cc_is_run", 0, cc_is_run, sizeof(cc_is_run));
		if(0 == atoi(cc_is_run))
		{
			pthread_attr_init(&thread_attr);
			pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED); 
			cfg_set_object_attr(GLOBALSTATE_PRESYSSTATE_NODE, "cc_is_run", "1");
		}
	}

/*****************************************************************************************/
	else if(atoi(value) == 2)
	{		
		all_cfg_node=cfg_obj_get_root();
		if(all_cfg_node)
		{
		
#if 0
			for(ptr = all_cfg_node; ptr->name!= NULL; ptr++)
			{
				node = mxmlFindElement(tree, tree, ptr->name,
			  		NULL, NULL,MXML_DESCEND);
				if((strcmp(ptr->name, VOIPSYSPARAM) == 0) && (node !=NULL) && (ptr->cfg_boot != NULL))
				{
					ptr->cfg_boot(tree);
				}
			}
#endif			
			system("msg call 4097 boot");
		}	
	}
/******************************************************************************************/
#if defined(TCSUPPORT_CRJO)
		/*add by nelson 20170316*/
		else if(atoi(value) == 3)
		{
			all_cfg_node=cfg_obj_get_root();
			if(all_cfg_node)
			{
#if 0
				for(ptr = all_cfg_node; ptr->name!= NULL; ptr++)
				{
					if(strcmp(ptr->name, "LogicID") == 0)
					{
						nodeTable_flag = 1;
						ptr++;
					}
					if(nodeTable_flag)
					{
						node = mxmlFindElement(tree, tree, ptr->name,
						NULL, NULL,MXML_DESCEND);
						if((node !=NULL)
						&&(ptr->cfg_boot!=NULL))
						{
							ptr->cfg_boot(tree);
						}
					}
				}
#endif			
				system("msg call 4097 boot");
			}
		}
		/*add by nelson 20170316*/
	/******************************************************************************************/
#endif
	return 0;
}/*end getAllCfgTable*/
#else
int
sys_start_up(char *value)
{
	mxml_node_t *node=NULL;
	cfg_node_obj_t *all_cfg_node=NULL;
//	cfg_node_t *ptr=NULL;
	if(atoi(value))
	{
		all_cfg_node=cfg_obj_get_root();
		if(all_cfg_node)
		{
#if 0
			for(ptr = all_cfg_node; ptr->name!= NULL; ptr++)
			{
				node = mxmlFindElement(tree, tree, ptr->name,
			  		NULL, NULL,MXML_DESCEND);
				if((node !=NULL) &&(ptr->cfg_boot!=NULL))
				{
					ptr->cfg_boot(tree);
				}
			}
#endif			
			system("msg call 4097 boot");
		}
	}

	return 0;
}/*end getAllCfgTable*/


#endif

#ifdef TCSUPPORT_CT_PON
int sys_avaTest(char *value)
{
	char nodeName[64] = {0};
	char xponmode[10] = {0};
	int xpon_flag = 0;
	int ret = 0;
	
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, "root.XPON.LinkCfg", sizeof(nodeName)-1);
	cfg_obj_get_object_attr(nodeName, "LinkSta", 0, xponmode, sizeof(xponmode));
	if (xponmode[0] != '\0' && strcmp(xponmode, "2") == 0) 
	{
		xpon_flag = 2; /* 0, auto, 1, gpon, 2, epon */
	}

	cfg_obj_set_object_attr("root.system.entry", "avalanchTest", 0, "2");
	
	ret = blapi_perform_avaTest(atoi(value), xpon_flag);

	return 0;
}
#endif

typedef struct
web_node_s{
	char attr[32];
	char value[64];
	int (*action)(char* value);
}web_node_t;

web_node_t system_node[]=
{
	{"reboot_type","0", sys_boot},
	{"skbmgr_hot_len","512", sys_skbmgr},
	{"upgrade_fw","0", sys_evt_upgrade_firmware},
	{"start_up","0", sys_start_up},
#ifdef TCSUPPORT_CT_PON
	{"avalanchTest", "2", sys_avaTest},
#endif
	{"","", NULL}
};

int readRomfileList(char *nodeName, char *basePath)
{
    DIR *dir = NULL;
    struct dirent *ptr = NULL;
	char attrName[32] = {0};
	char attrValue[64] = {0};
	int i = 0;
#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND)
	proline_Para para;
#endif

    if ((dir=opendir(basePath)) == NULL)
    {
        return FAIL;
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    /*current dir OR parrent dir*/
            continue;
        else if(ptr->d_type == 8)/*file*/
		{    	
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
			if(NULL!=strstr(ptr->d_name,"ctromfile")
#else
			if(NULL!=strstr(ptr->d_name,"romfile")
#endif
        	 && NULL!=strstr(ptr->d_name,".cfg") && NULL==strstr(ptr->d_name,"_f.cfg"))
        	{
				i++;
				snprintf(attrName, sizeof(attrName), "romfile%d", i);
				cfg_set_object_attr(nodeName, attrName, ptr->d_name);
        	}
        }
        else if(ptr->d_type == 10)    	/*link file*/
            continue;
        else if(ptr->d_type == 4)    	/*dir*/
        	continue;
    }
	snprintf(attrValue, sizeof(attrValue), "%d", i);
	cfg_set_object_attr(nodeName, "romfileCount", attrValue);
#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND)
	memset(&para, 0, sizeof(para));
	tcgetProLinePara(&para,0);
	if((PRODUCTLINEPARAMAGIC == para.magic) && (para.flag & PL_CP_ROMFILE_SELECT_FLAG))
	{
		para.romfileselect[sizeof(para.romfileselect) - 1] = '\0';
		snprintf(attrValue, sizeof(attrValue), "%s", para.romfileselect);
		cfg_set_object_attr(nodeName, "romfileSelect", attrValue);
	}
#endif	
    closedir(dir);
    return SUCCESS;
}

int cfg_type_system_func_get(char* path,char* attr,char* val,int len)
{
  	char nodeName[64] = {0};

	memset(nodeName,0, sizeof(nodeName));

	strncpy(nodeName, "root.System.Entry", sizeof(nodeName)-1);

	if(strstr(attr,"romfile") != NULL){
#if defined(TCSUPPORT_CT_PON)
	readRomfileList(nodeName,"/userfs/");
#endif
	}

	return cfg_type_default_func_get(path,attr,val,len);
}
#if defined(TCSUPPORT_CT_JOYME2)
static void cfg_type_system_version_update(){
	char swversion[32] ={0};
	fileRead("/etc/fwver.conf", swversion, sizeof(swversion)-1);
	cfg_set_object_attr("root.System.Entry", "UpgradeSWVersion", swversion);	
}
#endif
int svc_cfg_boot_system(void)
{
	int i=0;
	char val[64] = {0};
#if defined(TCSUPPORT_CT_JOYME2)	
	char cmd[128] = {0};
	char crash_flag[4] = {0};
#endif
#ifndef PURE_BRIDGE
	/*backup up now romfile at boot*/
	if(cfg_save_object(CFG_OBJ_FILE_PATH) == -1)
	{
		printf(CREATE_ROMFILE_ERR_MSG);
		return -1;
	}
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	/*update system SWversion*/
	cfg_type_system_version_update();
#endif
	/*Init system node attribute-value*/
	for(i=0; strlen(system_node[i].attr)!=0; i++)
	{
		cfg_set_object_attr("root.System.Entry", system_node[i].attr, system_node[i].value);
	}

	/*init system upgrade status*/
	cfg_set_object_attr("root.System.Entry", "upgrade_fw_status", "NONE");

	
#if defined(TCSUPPORT_CT_JOYME2)
/*
-------------------------------------------------------------------
* 262144 = 0X40000 = DEFAULTROMFILE_RA_OFFSET
-------------------------------------------------------------------
*/	
	snprintf(cmd, sizeof(cmd), "/userfs/bin/mtd readflash /tmp/crash_info 4096 %u reservearea",DEFAULTROMFILE_RA_OFFSET);
	system(cmd);
	
	fileRead("/tmp/crash_info", crash_flag, 4);
	if ( 0 == strncmp(crash_flag, "eco", 3) )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/bin/cp /tmp/crash_info /opt/upt/apps/info/crash_info");
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "chmod 644 -R /opt/upt/apps/info/crash_info");
		system(cmd);
		
		system("echo ffff > /tmp/crash_info");
		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/userfs/bin/mtd writeflash /tmp/crash_info 4096 %u  reservearea",DEFAULTROMFILE_RA_OFFSET);
		system(cmd);
		unlink("/tmp/crash_info");
	}

	fileRead(RESETSOURCEPATH, val, sizeof(val));
	if ( atoi(val) > 0 )
	{
		cfg_set_object_attr(SYSTEM_ENTRY_NODE,  "RestoreStatus", val);
	}
	unlink(RESETSOURCEPATH);
#endif

	return 0;

}

int setSelectRomfile(char *filename)
{
	char syscmd[100]={0};

	memset(syscmd,0,sizeof(syscmd));
	snprintf(syscmd, sizeof(syscmd), "prolinecmd romfileselect set %s", filename);
	system(syscmd);
	
	memset(syscmd,0,sizeof(syscmd));
	if(strstr(filename,"epon")!=NULL)			
		snprintf(syscmd, sizeof(syscmd), "prolinecmd xponmode set %s","EPON");
	else
		snprintf(syscmd, sizeof(syscmd), "prolinecmd xponmode set %s","GPON");
	system(syscmd);
	
	memset(syscmd,0,sizeof(syscmd));
	snprintf(syscmd, sizeof(syscmd), "prolinecmd restore default");
	system(syscmd);	
	return 0;
}

int cfg_type_system_func_commit( char *name)
{
	int i=0;
	int retval=0;
	char attrValue[64] = {0};
	char oriValue[64] = {0};
#if defined(TCSUPPORT_CMCCV2)
	char devicereboot[8] = {0};
#endif

	/*Get the attribute-value*/
	for(i=0; strlen(system_node[i].attr)!=0; i++)
	{
		cfg_obj_get_object_attr(name, system_node[i].attr, 0, system_node[i].value, 64);
#ifdef DEBUG
		printf("attr:%s value:%s\r\n",system_node[i].attr, system_node[i].value);
#endif
	}

	/*Execute the action*/
	for(i=0; strlen(system_node[i].attr)!=0; i++)
	{
		if(system_node[i].action!=NULL)
		{
			retval +=system_node[i].action(system_node[i].value);
		}
	}

#if defined(TCSUPPORT_CT_PON)
	cfg_obj_get_object_attr(name, "newRomfileSelect", 0, attrValue, sizeof(attrValue));

	if( '\0' != attrValue[0] )
	{
		cfg_obj_get_object_attr(name, "romfileSelect", 0, oriValue, sizeof(oriValue));
		if(strcmp(attrValue,oriValue)!=0)
		{
			cfg_set_object_attr(name, "romfileSelect", attrValue);
			setSelectRomfile(attrValue);
		}
	}
#endif

	#ifdef DEBUG
	printf("system_execute retval:%d\r\n",retval);
	#endif

#if defined(TCSUPPORT_CMCCV2)
	cfg_obj_get_object_attr(name, "devicereboot", 0, devicereboot, sizeof(devicereboot));
	if(devicereboot[0] != '\0' && (strcmp(devicereboot,"1") == 0))
	{
		openlog("TCSysLog alarm", 0, LOG_LOCAL2);
		syslog(LOG_ALERT, "id:104001 Device reboot\n");
		closelog();
	}
#endif
	return retval;
}/*end system_execute*/

static cfg_node_ops_t cfg_type_system_entry_ops  = { 
	 .get = cfg_type_system_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_system_func_commit 
}; 


static cfg_node_type_t cfg_type_system_entry = { 
	 .name = "Entry", 
	 .flag =  1, 
	 .parent = &cfg_type_system, 
	 .ops = &cfg_type_system_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_system_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_system_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_system_func_commit 
}; 


static cfg_node_type_t* cfg_type_system_child[] = { 
	 &cfg_type_system_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_system = { 
	 .name = "System", 
	 .flag =  1, 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_system_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_system_child, 
	 .ops = &cfg_type_system_ops, 
}; 
