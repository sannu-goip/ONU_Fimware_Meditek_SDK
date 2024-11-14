
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
#include <stdlib.h>
#include <sys/stat.h>

#include <svchost_api.h> 
#include <cfg_cli.h>
#include "utility.h"
#include <syslog.h>
#include "other_mgr.h"
#include "other_cfg.h"
#include "other_snmp_cfg.h"
#include "other_cfg_v6.h"
#include "dhcpd_boot_execute.h"
#include "dhcp_relay_boot_execute.h"
#include "parental_boot_execute.h"
#include "parental_mac_boot_execute.h"
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
#include "other_cfg_transferservices.h"
#endif

#include "libapi_lib_switchmgr.h"
#include <ecnt_event_global/ecnt_event_global.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_dbus.h>

#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
#include <arpa/inet.h>
#include <netdb.h>

#define CTCOM_STOPPING_FROM_CWMP "1"
#define CTCOM_STOPPING_FROM_PING "2"
#define IPPINGDIAGNOSTIC_DNSURL "/tmp/ippingdiag.conf"
#define CTCOM_PING_ROUTE_PATH "/tmp/cwmp/ct_ping_route"
#define BRIDGE_MODE	"3"
#define CTCOM_PING_PID_PATH "/tmp/cwmp/ct_ping%d.pid"
#define CTCOM_MAX_IPPINGDIAGNOSTIC_NUM 3
#endif
int svc_other_handle_event_lan_update(char* path)
{
	svc_other_handle_event_lan_write();
	svc_other_handle_event_lan_execute(path);

	return 0;
}

#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
int svc_other_handle_event_samba_update(int flag, char* path)
{
	svc_other_handle_event_samba_write(flag, path);
	svc_other_handle_event_samba_execute(path);		
		
	return 0;
}
#endif

int svc_other_handle_event_arprule_change()
{
	return svc_other_addBridgeArpRules();
}

int svc_other_handle_event_snmpd_update(char* path)
{
	svc_other_handle_event_snmpd_write(path);
	return svc_other_handle_event_snmpd_execute(path);
}

	
int svc_other_handle_event_upnpd_update(char* path)
{
	svc_other_handle_event_upnpd_write(path);
	return svc_other_handle_event_upnpd_execute(path);
}


int svc_other_handle_event_radvd_update(char *path)
{
	svc_other_handle_event_radvd_write(path);
	return svc_other_handle_event_radvd_execute(path);
}

int svc_other_handle_event_dhcp6s_update(char *path)
{
	svc_other_handle_event_dhcp6s_write(path);
	return svc_other_handle_event_dhcp6s_execute(path);
}
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)

void cfg_stopCwmpPing(int entryindex)
{
	FILE *fp = NULL;
	char cmdbuf[128] = {0};
	int ping_pid = 0;
	char *line = NULL;
	int len = 0, res = 0, res1 = 0;

	/*operation route information*/
	memset(cmdbuf,0,sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "%s%d",CTCOM_PING_ROUTE_PATH, entryindex);
	fp = fopen(cmdbuf, "r");
	if(fp)
	{
		/*delete the route which add at the beginning of the ping */
		res = getline(&line, &len, fp);
		if ( line )
		{
			system(line);
			free(line);
			line = NULL;
		}

		/*delete the file /tmp/cwmp/ct_ping_route*/
		fclose(fp);
		snprintf(cmdbuf, sizeof(cmdbuf), "%s%d",CTCOM_PING_ROUTE_PATH, entryindex);
		unlink(cmdbuf);
	}

	/*operate ping application*/			
	snprintf(cmdbuf, sizeof(cmdbuf),CTCOM_PING_PID_PATH,entryindex);
	fp = fopen(cmdbuf, "r");
	if(fp)
	{
		res1 = getline(&line, &len, fp);
		if ( line )
		{
			ping_pid = atoi(line);
			free(line);
		}

		fclose(fp);
		/*delete the file/tmp/cwmp/ct_ping_xxx.pid*/
		snprintf(cmdbuf, sizeof(cmdbuf), CTCOM_PING_PID_PATH, entryindex);
		unlink(cmdbuf);
		
		/*kill the ping process*/
		if(ping_pid != 0)
		{ 
			/*if pid != 0, that's mean old ping diagnostic process already exist*/
			memset(cmdbuf,0,sizeof(cmdbuf));
			/*remove old ping diagnostic process*/
			snprintf(cmdbuf, sizeof(cmdbuf), "/bin/kill -9 %d", ping_pid);
			system(cmdbuf);
		}
	}

	return;
}

void cfg_stopAllPingOperation()
{
	int i;

	for(i = 0; i < 3; i++)
	{
		cfg_stopCwmpPing(i);
	}

	return;
}

int svc_other_handle_event_ctdiagnostic_update(char* path)
{
	char nodeName[64];
	char nodeNameTemp[64];
	char tmp[32] = {0}, isp[4] = {0},active[8] = {0},pingflag[4] = {0};
	char *temp = NULL;
	int interfaceindex = 0;
	char StopFlag[10] = {0};
	char DiagState[16] = {0};
	char InterfaceStr[16] = {0};
	char DevStr[32] = "None",GatewayStr[32] = "None",HostStr[32] = {0};
	char cmdbuf[200]  = {0};
	char errMessage[32] = {0};
	int blocksize = 0,timeout = 0,interval = 0,pingnum = 0,pingnumleave = 0, dscp = 0;
	int pingid = -1;
	FILE *fp = NULL;
	char wanNode[64] = {0};
	char def_route_index_v4[10] = {0};
	int i_def_route_index_v4 = -1;
	struct sockaddr_in cvtaddr = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL;
	int dns_wtime = 5, dns_tmp_time = -1;
	
	/*first,judge if total switch is on or off*/
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, CTDIAGNOSTIC_COMMON_NODE, sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, "Active", 0, active, sizeof(active)) > 0)
	{
		if(0 == strcmp(active,"No"))
		{
			/*if switch == "No" ,stop all related ping info(include ping application and route info) if exist*/
			cfg_stopAllPingOperation();
			return 0;
		}
	}
	else
	{
		return 0;
	}

	/*get current ipping index*/
	if(get_entry_number_cfg2(path,"entry.",&pingid) != 0)
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName)-1);
		if(cfg_obj_get_object_attr(nodeName, "ippdiag_id", 0, tmp, sizeof(tmp)) > 0)
		{
			pingid = atoi(tmp) + 1;
		}
		else
		{
			return -1;
		}
	}
	
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), CTDIAGNOSTIC_ENTRY_NODE, pingid);
	pingid -= 1;
	if(cfg_obj_get_object_attr(nodeName, "DiagnosticsState", 0, DiagState, sizeof(DiagState)) < 0)
	{
		strncpy(errMessage, "None", sizeof(errMessage)-1);
		goto errHandle;
	}

	memset(pingflag, 0, sizeof(pingflag));
	if(cfg_obj_get_object_attr(nodeName, "StopPingFlag", 0, pingflag, sizeof(pingflag)) > 0)
	{
		if(!strcmp(pingflag,CTCOM_STOPPING_FROM_CWMP) || !strcmp(pingflag,CTCOM_STOPPING_FROM_PING))
		{
			cfg_set_object_attr(nodeName,"StopPingFlag","0");
			strncpy(errMessage, DiagState, sizeof(errMessage)-1);
			cfg_stopCwmpPing(pingid);
			goto errHandle;
		}
	}

	if(strlen(DiagState) != 0 && strcmp(DiagState,"None") && strcmp(DiagState,"Complete"))
	{
		if(cfg_obj_get_object_attr(nodeName, "Stop", 0, StopFlag, sizeof(StopFlag)) > 0)
		{
			if(strcmp(StopFlag,"1") == 0)
			{
				cfg_stopCwmpPing(pingid);
				strncpy(errMessage, "None", sizeof(errMessage)-1);
				goto errHandle;
			}	
		}
		/*judge if the ping is exist,if exist,just return*/
		/*why add this judgement??	to settle this case: active is yes,and requested one entry and recomit active to yes,using "ps",it will show two ping aplications*/
		memset(cmdbuf,0,sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), CTCOM_PING_PID_PATH, pingid);
		fp = fopen(cmdbuf, "r");
		if(fp)
		{
			fclose(fp);
			fp = NULL;
			return 0;
		}
		/*NumberOfRepetitions*/
		if(cfg_obj_get_object_attr(nodeName, "NumberOfRepetitions", 0, tmp, sizeof(tmp)) < 0)
		{
			tcdbg_printf("\r\nctcom_ippingdiagnotic_execute:fetch NumberOfRepetitions error!");
		}
		else
		{
			pingnum = atoi(tmp);
			if(cfg_obj_get_object_attr(nodeName, "PingNum", 0, tmp, sizeof(tmp)) < 0)
			{
				tcdbg_printf("\r\nctcom_ippingdiagnotic_execute:fetch PingNum error!");
			}
			else
			{
				pingnumleave = atoi(tmp);
			}
		}

		if(pingnum != 0 && pingnum <= pingnumleave)
		{
			return 0;
		}

		/*operation interface*/
		if(cfg_obj_get_object_attr(nodeName, "Interface", 0, InterfaceStr, sizeof(InterfaceStr)) < 0)
		{
			strncpy(errMessage, "Error_Internal", sizeof(errMessage)-1);
			goto errHandle;
		}

		if((temp = strstr(InterfaceStr,"smux")) != NULL)/*ping is wan operation*/
		{
			temp += strlen("smux");
			interfaceindex = atoi(temp);
			
			if(get_waninfo_by_index(interfaceindex, "Active", active, sizeof(active)) == 0 &&
				get_waninfo_by_index(interfaceindex, "ISP", isp, sizeof(isp)) == 0 &&
				get_waninfo_by_index(interfaceindex, "IFName", DevStr, sizeof(DevStr)) == 0)
			{
				/*pvc is active,and not bridge mode */
				if(!strcmp(active, "Yes") && strcmp(isp, BRIDGE_MODE))
				{
					memset(nodeNameTemp,0,sizeof(nodeNameTemp));
					snprintf(nodeNameTemp, sizeof(nodeNameTemp), WANINFO_ENTRY_NODE, (interfaceindex + 1));
					if(cfg_obj_get_object_attr(nodeNameTemp, "GateWay", 0, GatewayStr, sizeof(GatewayStr)) < 0)
					{
						strncpy(errMessage, "Error_Internal", sizeof(errMessage)-1);
						goto errHandle;
					}
					if(strlen(DevStr) == 0 || strlen(GatewayStr) == 0)
					{
						strncpy(DevStr, "None", sizeof(DevStr)-1);
						strncpy(GatewayStr, "None", sizeof(GatewayStr)-1);
					}
				}
			}
		}
		else if(strlen(InterfaceStr) == 0/* ||strstr(InterfaceStr,LAN_IF)*/)/*no need to add default route*/
		{
			/* should get the info from default route internet wan.*/
			memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
			memset(wanNode, 0, sizeof(wanNode));
			strncpy(wanNode, WANINFO_COMMON_NODE, sizeof(wanNode)-1);
			/* get default route for ipv4*/
			if ( cfg_obj_get_object_attr(wanNode, "DefRouteIndexv4", 0, def_route_index_v4, sizeof(def_route_index_v4)) > 0
				&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A") )
			{
				i_def_route_index_v4 = atoi(def_route_index_v4);
			}
			
			if ( i_def_route_index_v4 >= 0 )
			{
				interfaceindex = i_def_route_index_v4 + 1;
				if(get_waninfo_by_index(interfaceindex, "Active", active, sizeof(active)) == 0 &&
					get_waninfo_by_index(interfaceindex, "ISP", isp, sizeof(isp)) == 0 &&
					get_waninfo_by_index(interfaceindex, "IFName", DevStr, sizeof(DevStr)) == 0)
				{
					/*pvc is active,and not bridge mode */
					if(!strcmp(active, "Yes") && strcmp(isp, BRIDGE_MODE))
					{
						memset(nodeNameTemp,0,sizeof(nodeNameTemp));
						snprintf(nodeNameTemp, sizeof(nodeNameTemp), WANINFO_ENTRY_NODE, (interfaceindex + 1));
						if(cfg_obj_get_object_attr(nodeNameTemp, "GateWay", 0, GatewayStr, sizeof(GatewayStr)) < 0)
						{
							strncpy(errMessage, "Error_Internal", sizeof(errMessage)-1);
							goto errHandle;
						}
						if(strlen(DevStr) == 0 || strlen(GatewayStr) == 0)
						{
							strncpy(DevStr, "None", sizeof(DevStr)-1);
							strncpy(GatewayStr, "None", sizeof(GatewayStr)-1);
						}
					}
				}
			}
		}

		/*operate host*/
		if(cfg_obj_get_object_attr(nodeName, "Host", 0, HostStr, sizeof(HostStr)) < 0)
		{
			strncpy(errMessage, "Error_CannotResolveHostName", sizeof(errMessage)-1);
			goto errHandle;
		}

		/*gethostbyname will be excuted by ping app */
		if (strlen(HostStr) == 0/* || (retval = gethostbyname(HostStr)) == NULL*/)
		{
			strncpy(errMessage, "Error_CannotResolveHostName", sizeof(errMessage)-1);
			goto errHandle;
		}

		if ( NULL == strstr(InterfaceStr,"br0") )
		{
			if(!strcmp(DevStr,"None") || !strcmp(GatewayStr,"None"))
			{
				strncpy(errMessage, "Error_Internal", sizeof(errMessage));
				goto errHandle;
			}
		}

		/*DateBlockSize*/
		if(cfg_obj_get_object_attr(nodeName, "DataBlockSize", 0, tmp, sizeof(tmp)) < 0)
		{
			tcdbg_printf("\r\nctcom_ippingdiagnotic_execute:fetch DateBlockSize error!");
		}
		else
		{
			blocksize = atoi(tmp);
		}

		/*timeout*/
		if(cfg_obj_get_object_attr(nodeName, "Timeout", 0, tmp, sizeof(tmp)) < 0)
		{
			tcdbg_printf("\r\nctcom_ippingdiagnotic_execute:fetch Timeout error!");
		}
		else
		{
			timeout = atoi(tmp);
		}

		/*Interval*/
		if(cfg_obj_get_object_attr(nodeName, "Interval", 0, tmp, sizeof(tmp)) < 0)
		{
			tcdbg_printf("\r\nctcom_ippingdiagnotic_execute:fetch Interval error!");
		}
		else
		{
			interval = atoi(tmp);
		}

		/*DSCP*/
		if(cfg_obj_get_object_attr(nodeName, "DSCP", 0, tmp, sizeof(tmp)) < 0)
		{
			tcdbg_printf("\r\nctcom_ippingdiagnotic_execute:fetch DSCP error!");
		}
		else
		{
			dscp = atoi(tmp);
		}

		if( 0 != strcmp(GatewayStr,"None") && 0 != strcmp(DevStr,"None") )
		{
			if( 0 == inet_aton(HostStr, &cvtaddr.sin_addr) ) /*failed*/
			{
				if( cfg_obj_get_object_attr(nodeName, "DNS_WTime", 0, tmp, sizeof(tmp)) > 0
					&& '\0' != tmp[0] )
				{
					dns_tmp_time = atoi(tmp);
					if ( dns_tmp_time > 0 )
						dns_wtime = dns_tmp_time;

					cfg_set_object_attr(nodeName, "DNS_WTime", "");
				}

				memset(cmdbuf, 0, sizeof(cmdbuf));
				snprintf(cmdbuf, sizeof(cmdbuf), "/bin/echo \"%s %s\" > %s", HostStr, DevStr, IPPINGDIAGNOSTIC_DNSURL);
				system_escape(cmdbuf);

				memset(&hints, 0 , sizeof(hints));
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_DGRAM; /*DNS is UDP packet*/
				hints.ai_protocol = IPPROTO_UDP;

				if ( tc_getaddrinfo(HostStr, NULL, &hints, &res, dns_wtime)
					|| AF_INET != res->ai_family )
				{
					unlink(IPPINGDIAGNOSTIC_DNSURL);
					strncpy(errMessage, "Error_CannotResolveHostName", sizeof(errMessage)-1);
					goto dnsErrHandle;
				}
				unlink(IPPINGDIAGNOSTIC_DNSURL);

				strncpy(HostStr, inet_ntoa(((struct sockaddr_in *) res->ai_addr)->sin_addr), sizeof(HostStr)-1);
			}
		}

		/*do ping operation*/
		memset(cmdbuf,0,sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf),"ping -q -f 1 -c %d -s %d -W %d -i %d -Q %d -p %d -d %s -g %s %s &",pingnum-pingnumleave,blocksize,timeout,interval,dscp,pingid,DevStr,GatewayStr,HostStr);
		system_escape(cmdbuf);

		/*if DiagnosticsState is not "Requested",but ping is ping ok,set and save the value and */
		if(strcmp(DiagState,"Requested"))
		{
			cfg_set_object_attr(nodeName,"DiagnosticsState","Requested");
			cfg_evt_write_romfile_to_flash();
		}
	}
	if(res)
		freeaddrinfo(res);
	return 0;	

errHandle:
	cfg_set_object_attr(nodeName,"DiagnosticsState",errMessage);
	cfg_evt_write_romfile_to_flash();
	return -1;
dnsErrHandle:
	cfg_set_object_attr(nodeName,"DiagnosticsState",errMessage);
	cfg_evt_write_romfile_to_flash();
	if(res)
		freeaddrinfo(res);
	return 0;

}
int svc_other_handle_event_ctdiagnostic_boot(void)
{
	int i,flag = 0;
	char nodeName[64]={0};
	char diagnostic[20] = {0},pingtotal[15] = {0},pingnum[15] = {0},Interfacebuf[32] = {0};
	char active[10] = {0};

	strncpy(nodeName, CTDIAGNOSTIC_COMMON_NODE, sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, "Active", 0, active, sizeof(active)) > 0)
	{
		if(0 == strcmp(active,"Yes"))
		{
			flag = 1;
		}
	}
	if(!flag)
	{
		return 0;
	}


	for(i = 1; i <= CTCOM_MAX_IPPINGDIAGNOSTIC_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), CTDIAGNOSTIC_ENTRY_NODE, i);
		if(cfg_obj_get_object_attr(nodeName, "DiagnosticsState", 0, diagnostic, sizeof(diagnostic)) > 0 &&
			cfg_obj_get_object_attr(nodeName, "NumberOfRepetitions", 0, pingtotal, sizeof(pingtotal)) > 0	&&
			cfg_obj_get_object_attr(nodeName, "Interface", 0, Interfacebuf, sizeof(Interfacebuf)) > 0 &&
			cfg_obj_get_object_attr(nodeName, "PingNum", 0, pingnum, sizeof(pingnum)) > 0)
		{
			/*only "None" mean not need to do ping operation*/
			if(strlen(diagnostic) != 0 && strcmp(diagnostic, "None") && (0 == atoi(pingtotal) ||(pingtotal - pingnum) > 0))
			{
				/*only handle ping lan pc or use default route*/
				if(strlen(Interfacebuf) == 0 || strstr(Interfacebuf, "br0"))
				{
					svc_other_handle_event_ctdiagnostic_update(nodeName);					
				}
			}
		}
	}
	return 0;
}
#endif

void ConfigRight(char *Right, int type)
{
	if(type == CONFIGHTTPRIGHT)
	{
		system("iptables -t filter -D INPUT -p TCP --dport 80 -i nas+ -j DROP 2>/dev/null");
		system("iptables -t filter -D INPUT -p TCP --dport 80 -i ppp+ -j DROP 2>/dev/null");
		system("iptables -t filter -D INPUT -p TCP --dport 80 -i  br+ -j DROP 2>/dev/null");
#if defined(TCSUPPORT_CT_JOYME2)
		if(strcmp(Right,"1")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 80 -i nas+ -j DROP");
			system("iptables -t filter -A INPUT -p TCP --dport 80 -i ppp+ -j DROP");
		}
		if(strcmp(Right,"2")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 80 -i  br+ -j DROP");
		}
#else
		if( strcmp(Right,"0") == 0 || strcmp(Right,"1") == 0 )
		{
			system("iptables -t filter -A INPUT -p TCP --dport 80 -i ! br+ -j DROP");
		}
		if( strcmp(Right,"0") == 0 || strcmp(Right,"2") == 0 )
		{
			system("iptables -t filter -A INPUT -p TCP --dport 80 -i  br+ -j DROP");
		}
#endif
	}
	else if(type == CONFIGFTPRIGHT)
	{
	    //drop ALL
	    system("iptables -t filter -D INPUT -p TCP --dport 21 -j DROP");
	    system("ip6tables -t filter -D INPUT -p TCP --dport 21 -j DROP");
	    //drop WAN
		system("iptables -t filter -D INPUT -p TCP --dport 21 -i nas+ -j DROP");
		system("iptables -t filter -D INPUT -p TCP --dport 21 -i ppp+ -j DROP");
		system("ip6tables -t filter -D INPUT -p TCP --dport 21 -i nas+ -j DROP");
		system("ip6tables -t filter -D INPUT -p TCP --dport 21 -i ppp+ -j DROP");
		//drop LAN
		system("iptables -t filter -D INPUT -p TCP --dport 21 -i  br+ -j DROP");
		system("ip6tables -t filter -D INPUT -p TCP --dport 21 -i  br+ -j DROP");
		//accept ALL
		system("iptables -t filter -D INPUT -p TCP --dport 21 -j ACCEPT");
		system("ip6tables -t filter -D INPUT -p TCP --dport 21 -j ACCEPT");
		//accept WAN
		system("iptables -t filter -D INPUT -p TCP --dport 21 -i nas+ -j ACCEPT");
		system("iptables -t filter -D INPUT -p TCP --dport 21 -i ppp+ -j ACCEPT");
		system("ip6tables -t filter -D INPUT -p TCP --dport 21 -i nas+ -j ACCEPT");
		system("ip6tables -t filter -D INPUT -p TCP --dport 21 -i ppp+ -j ACCEPT");
#if defined(TCSUPPORT_CT_JOYME2)
        if(strcmp(Right,"0")==0) //close local and remote
        {
            system("iptables -t filter -A INPUT -p TCP --dport 21 -j DROP");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -j DROP");
        }
		if(strcmp(Right,"1")==0) //close remote, open local
		{
			system("iptables -t filter -A INPUT -p TCP --dport 21 -i nas+ -j DROP");
			system("iptables -t filter -A INPUT -p TCP --dport 21 -i ppp+ -j DROP");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i nas+ -j DROP");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i ppp+ -j DROP");
		}
		if(strcmp(Right,"2")==0) //close local, open remote
		{
			system("iptables -t filter -A INPUT -p TCP --dport 21 -i nas+ -j ACCEPT");
			system("iptables -t filter -A INPUT -p TCP --dport 21 -i ppp+ -j ACCEPT");
            system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i nas+ -j ACCEPT");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i ppp+ -j ACCEPT");
            system("iptables -t filter -A INPUT -p TCP --dport 21 -i  br+ -j DROP");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i  br+ -j DROP");
            cfg_commit_object(FIREWALL_ENTRY_NODE);
		}
		if(strcmp(Right,"3")==0) //open local and remote
		{
		    system("iptables -t filter -A INPUT -p TCP --dport 21 -j ACCEPT");
            system("ip6tables -t filter -A INPUT -p TCP --dport 21 -j ACCEPT");
            cfg_commit_object(FIREWALL_ENTRY_NODE);
		}
#else	
		if(strcmp(Right,"0")==0 || strcmp(Right,"1")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 21 -i ! br+ -j DROP");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i ! br+ -j DROP");
		}
		if(strcmp(Right,"0")==0 || strcmp(Right,"2")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 21 -i  br+ -j DROP");
			system("ip6tables -t filter -A INPUT -p TCP --dport 21 -i  br+ -j DROP");
		}
#endif
	}
	else if(type == CONFIGTELNETRIGHT)
	{
		system("iptables -t filter -D INPUT -p TCP --dport 23 -i nas+ -j DROP 2>/dev/null");
		system("iptables -t filter -D INPUT -p TCP --dport 23 -i ppp+ -j DROP 2>/dev/null");
		system("iptables -t filter -D INPUT -p TCP --dport 23 -i  br+ -j DROP 2>/dev/null");
#if defined(TCSUPPORT_CT_JOYME2)
		if (strcmp(Right,"1")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 23 -i nas+ -j DROP");
			system("iptables -t filter -A INPUT -p TCP --dport 23 -i ppp+ -j DROP");
		}
		if (strcmp(Right,"2")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 23 -i  br+ -j DROP");
		}
#else
		if(strcmp(Right,"0")==0 || strcmp(Right,"1")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 23 -i ! br+ -j DROP");
		}
		if(strcmp(Right,"0")==0 || strcmp(Right,"2")==0)
		{
			system("iptables -t filter -A INPUT -p TCP --dport 23 -i  br+ -j DROP");
		}
#endif
	}

	return;
}

void writeTelnetAccountfile(void){
	char *pname[] = {"telnet_username","telnet_username","username"};
	char *ppaswd[] = {"telnet_passwd","telnet_passwd","web_passwd"};
	char *pnode[] = {ACCOUNT_TELNETENTRY_NODE, "root.account.telnetentry1","root.account.entry.1"};
	char *cpwd = NULL;
	FILE *fp = NULL;
	char tmpattr[64] = {0};
	char username[129] = {0};
	char pwd_buf[256] = {0}; 
	char passwd[256] = {0};
	char Active[4] = {0};
	char Right[4] = {0};
	int i;

	fp = fopen(CONSOLE_PATH, "w");
	if(fp==NULL)
	{
		return;
	}
#if defined(TCSUPPORT_CT_JOYME4)
	/*
	* JOYME4:  root.account.telnetentry and root.account.telnetentry1 is normal user
	* 		default is telnetadmin and telnetadmin1 (SPEC is not defined)
	*/
	for(i =0; i < 2; i++)
#else
	for(i =0; i < 3; i++)
#endif
	{
		memset(tmpattr, 0, sizeof(tmpattr));
		if(cfg_get_object_attr(pnode[i], "Active", tmpattr, sizeof(tmpattr)) < 0 || 0 != strcmp(tmpattr,"Yes"))
		{
#if defined(TCSUPPORT_CMCCV2)
			if ( 0 == i )
			{
				if ( cfg_get_object_attr(ACCOUNT_WANTELNETENTRY_NODE, "Active", tmpattr, sizeof(tmpattr)) < 0 || 0 != strcmp(tmpattr,"Yes") )
					continue;
			}
			else
#endif
			continue;
		}
		memset(username, 0, sizeof(username));
		if(cfg_get_object_attr(pnode[i], pname[i], username, sizeof(username)) < 0)
		{
			continue;
		}		
		memset(passwd, 0, sizeof(passwd));
		if(cfg_get_object_attr(pnode[i], ppaswd[i], passwd, sizeof(passwd)) < 0)
		{
			continue;
		}
		memset(pwd_buf, 0, sizeof(pwd_buf));
		cpwd = crypt(passwd, SLAT);
	#if defined(TCSUPPORT_CT_JOYME2)
		if(i == 2)
		{
			snprintf(pwd_buf,sizeof(pwd_buf), ACCOUNT_TELNET_ROOT_STRING, username, cpwd);
		}
		else 
		{
			snprintf(pwd_buf,sizeof(pwd_buf), ACCOUNT_TELNET_STRING, username, cpwd);
		}
	#else
		snprintf(pwd_buf,sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, username, cpwd);
	#endif
		F_PUTS_NORMAL(pwd_buf, fp);
	}
	fclose(fp);
	
	
#if defined(TCSUPPORT_CT_JOYME2)
	if(cfg_get_object_attr(pnode[0], "telnet_Right", tmpattr, sizeof(tmpattr)) < 0)
	{
		strncpy(Right, "1", sizeof(Right)-1);
	}
	else
	{
		snprintf(Right,sizeof(Right), "%s", tmpattr);
	}
	ConfigRight(Right,CONFIGTELNETRIGHT);
#endif
	return;
}


void AddFtpAccount(void)
{
	char *cpwd = NULL;
	char username[129] = {0};
	char pwd_buf[256] = {0}; 
	char passwd[256] = {0};
	char Active[4] = {0};
	FILE *fp = NULL;
	int i = 0;	
	char nodePath[64] = {0};
	int ftp_entry_exist = 0;
	char ftp_Right[4] = {0};
		
	fp = fopen(FTPPASSWD_PATH, "w");
	if(fp==NULL)
	{
		return;
	}
	for(i = 0; i < 6; i++)
	{
		memset(pwd_buf, 0, sizeof(pwd_buf));
		memset(username, 0, sizeof(username));
		memset(nodePath, 0, sizeof(nodePath));
#if defined(TCSUPPORT_CMCCV2)
		if(i==0)
			strncpy(nodePath, ACCOUNT_FTPENTRY_NODE ,sizeof(nodePath));
		else
#endif
		snprintf(nodePath, sizeof(nodePath), ACCOUNT_FTPENTRY_N_NODE, i);
		
		if(cfg_get_object_attr(nodePath, "ftp_username", username, sizeof(username)) < 0)
		{
			/*printf("\n[%s]:line=%d,username is deactive!!\n",__FUNCTION__,__LINE__);*/
			continue;
		}
		
		if(cfg_get_object_attr(nodePath, "ftp_passwd", passwd, sizeof(passwd)) < 0)
		{
			/*printf("\n[%s]:line=%d,passwd not fetch value!\n",__FUNCTION__,__LINE__);*/
			continue;
		}
			
		ftp_entry_exist = 1;
		
		cpwd = crypt(passwd, SLAT);
		snprintf(pwd_buf,sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, username, cpwd);
		F_PUTS_NORMAL(pwd_buf, fp);	
	}	

	/*Add ONU username and password to ftp*/
	memset(nodePath, 0, sizeof(nodePath));
#if defined(TCSUPPORT_CT_JOYME2)
	strncpy(nodePath, ACCOUNT_FTPCOMMON_NODE, sizeof(nodePath)-1);
#else
	strncpy(nodePath, ACCOUNT_FTPENTRY_NODE, sizeof(nodePath)-1);
#endif
	if (0 == ftp_entry_exist
	    || cfg_get_object_attr(nodePath, "ftp_Right", ftp_Right, sizeof(ftp_Right)) <= 0
        || strcmp(ftp_Right,"1") == 0) //only enable local use web[userbane/password])
	{
		memset(username, 0, sizeof(username));
		memset(passwd, 0, sizeof(passwd));
		memset(pwd_buf, 0, sizeof(pwd_buf));
		
		if(cfg_get_object_attr(ACCOUNT_ENTRY2_NODE, "username", username, sizeof(username)) < 0)
		{
			goto ERR;
		}
		
		if(cfg_get_object_attr(ACCOUNT_ENTRY2_NODE, "web_passwd", passwd, sizeof(passwd)) < 0)
		{
			goto ERR;
		}
		cpwd = crypt(passwd, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, username, cpwd);
		F_PUTS_NORMAL(pwd_buf, fp);	
	}

ERR:
	fclose(fp);
	
	return;
}

void ConfigAnonymous(char *Anonymous)
{
	FILE *servicesfp=NULL;
	FILE *servicesfptmp=NULL;
	char servicestr[128]={0};
	char servicestrtmp[128]={0};
	char *ptr = NULL;
	char *ptrbegin = NULL;
	char *ptrend = NULL;
	char tmp[50] = {0};
	int i = 0;
	int res = 0;

	servicesfp=fopen(FTPSERVER_FILE_PATH, "r");

	if(servicesfp)
	{
		servicesfptmp = fopen(FTPSERVER_FILE_PATH_TMP, "w");
		if(!servicesfptmp)
		{
			fclose(servicesfp);
			printf("\n[%s]:line=%d,parseftpport:open file(%s) error!\n",__FUNCTION__,__LINE__,FTPSERVER_FILE_PATH_TMP);
			return;
		}
		while(fgets(servicestr, 128, servicesfp))
		{
			if(servicestr[0] != '#' && strstr(servicestr,"ANONYMOUS_USER="))
			{
				ptr = servicestr;
				ptrbegin = strchr(servicestr,'\"');
				ptrend = strrchr(servicestr,'\"');
				if(!ptrbegin ||!ptrend ||ptrbegin == ptrend)
				{
					printf("\n[%s]:line=%d,parseftpport:parse or file error!\n",__FUNCTION__,__LINE__);
					fclose(servicesfp);
					fclose(servicesfptmp);
					return;
				}
				if(i == 0)
				{
					memset(servicestrtmp,0,sizeof(servicestrtmp));
					strncpy(servicestrtmp,ptr,ptrbegin-ptr);
					if(strcmp(Anonymous,"TRUE") == 0)
					{
						snprintf(tmp,sizeof(tmp),"\"%s\"\n","yes");
						strcat(servicestrtmp,tmp);
						fputs(servicestrtmp, servicesfptmp);
						i = 1;
						continue;
					}
					else
					{
						snprintf(tmp,sizeof(tmp),"\"%s\"\n","no");
						strcat(servicestrtmp,tmp);
						fputs(servicestrtmp, servicesfptmp);
						i = 1;
						continue;
					}
				}
			}
			fputs(servicestr, servicesfptmp);
		}
		fclose(servicesfptmp);
		fclose(servicesfp);
		/*renname*/
		unlink(FTPSERVER_FILE_PATH);
		res = rename(FTPSERVER_FILE_PATH_TMP, FTPSERVER_FILE_PATH);
		unlink(FTPSERVER_FILE_PATH_TMP);
		
	}
	return ;
}


int parseftpport(const char *port)
{
	FILE *servicesfp=NULL;
	FILE *servicesfptmp=NULL;
	char servicestr[128]={0};
	char servicestrtmp[128]={0};
	char *ptr = NULL;
	char *ptrbegin = NULL;
	char *ptrend = NULL;
	char porttmp[10] = {0};
	char tmp[50] = {0};
	char buf[100] = {0};
	int res = 0;

	servicesfp=fopen(FTPSERVER_FILE_PATH, "r");

	if(servicesfp)
	{
		servicesfptmp = fopen(FTPSERVER_FILE_PATH_TMP, "w");
		if(!servicesfptmp)
		{
			fclose(servicesfp);
			printf("\n[%s]:line=%d,parseftpport:open file(%s) error!",__FUNCTION__,__LINE__,FTPSERVER_FILE_PATH_TMP);
			return -1;
		}
		while(fgets(servicestr, 128, servicesfp))
		{
			if(servicestr[0] != '#' && strstr(servicestr,"PORT="))
			{
				ptr = servicestr;
				ptrbegin = strchr(servicestr,'\"');
				ptrend = strrchr(servicestr,'\"');
				if(!ptrbegin ||!ptrend ||ptrbegin == ptrend)
				{
					printf("\n[%s]:line=%d,parseftpport:parse or file error!\n",__FUNCTION__,__LINE__);
					fclose(servicesfp);
					fclose(servicesfptmp);
					return -1;
				}
				strncpy(servicestrtmp,ptr,ptrbegin-ptr);
				if((ptrend-ptrbegin - 1) >= 0 && (ptrend-ptrbegin - 1) <= 9)
					strncpy(porttmp,ptrbegin+1,ptrend-ptrbegin-1);
				snprintf(buf,sizeof(buf),"echo %s >/proc/sys/net/netfilter/nf_conntrack_ftp_port",port);
				system_escape(buf);
				if(!strcmp(porttmp,port))
				{
					fclose(servicesfptmp);
					fclose(servicesfp);
					unlink(FTPSERVER_FILE_PATH_TMP);
					return 0;
				}
				else
				{
					snprintf(tmp,sizeof(tmp),"\"%s\"\n",port);
					strcat(servicestrtmp,tmp);
					printf("\n[%s]:line=%d,the new string is %s\n",__FUNCTION__,__LINE__,servicestrtmp);
					fputs_escape(servicestrtmp, servicesfptmp);
					continue;		
				}			
			}
			fputs(servicestr, servicesfptmp);
		}
		fclose(servicesfptmp);
		fclose(servicesfp);
		unlink(FTPSERVER_FILE_PATH);
		res = rename(FTPSERVER_FILE_PATH_TMP, FTPSERVER_FILE_PATH);
		unlink(FTPSERVER_FILE_PATH_TMP);
		return 0;
	}
	else
	{

		fprintf(stderr, "parseftpport:open file(%s) error!",FTPSERVER_FILE_PATH);
		return -1;
	}
}

int cfg_type_account_write(void)	
{
	int retval=0;
	int i=0, j = 0;
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
	char Right[4] = {0};
	char Anonymous[8] = {0};	
#endif
	char *cpwd = NULL;
	char pwd_buf[256] = {0}; 
	char username[129] = {0};
	FILE *fp = NULL;
	char tmpattr[64] = {0};
	char webnodePath[3][64];
	char telnetPath[64] = {0};
	char ftpPath[64] = {0};
	char consolePath[64] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
#define USER3_NORMAL_PRIV3 2 == i ? ACCOUNT_NORMAL_USER_STRING :
#else
#define USER3_NORMAL_PRIV3 
#endif

	memset(webnodePath, 0, sizeof(webnodePath));
	for(i=0; i<3; i++)
	{
		snprintf(webnodePath[i], sizeof(webnodePath[i]), ACCOUNT_ENTRY_NODE, i+1);
		if(cfg_query_object(webnodePath[i],NULL,NULL) < 0)
		{
			j++;
		}
	}
	if( 3 == j )
		goto telnet_config;
	
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
    memset(Right, 0, sizeof(Right));
	if(cfg_get_object_attr(webnodePath[1], "Right", tmpattr, sizeof(tmpattr)) <= 0)
	{
		strncpy(Right,"1", sizeof(Right)-1);
	}
	else
	{
		snprintf(Right, sizeof(Right), "%s", tmpattr);
	}
	
	ConfigRight(Right,CONFIGHTTPRIGHT);
#endif

	/*operate web login info */
	/*Create passwd file */
	fp = fopen(WEBPASSWD_PATH, "w");
	if(fp == NULL)
	{
		goto telnet_config;
	}

	for(i = 0; i < 3; i++ )
	{
		memset(pwd_buf, 0, sizeof(pwd_buf));
		memset(username, 0, sizeof(username));
		memset(tmpattr, 0, sizeof(tmpattr));
		if(cfg_get_object_attr(webnodePath[i], "Active", tmpattr, sizeof(tmpattr)) <= 0)
		{
			continue;
		}
		if(strcmp(tmpattr,"Yes"))
		{
			continue;
		}
		
		memset(tmpattr, 0, sizeof(tmpattr));
		if(cfg_get_object_attr(webnodePath[i], "username", tmpattr, sizeof(tmpattr)) <= 0)
		{
			continue;
		}
		snprintf(username,sizeof(username), "%s", tmpattr);
		
		memset(tmpattr, 0, sizeof(tmpattr));
		if(cfg_get_object_attr(webnodePath[i], "web_passwd", tmpattr, sizeof(tmpattr)) <= 0)
		{
			continue;
		}
		cpwd = crypt(tmpattr, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf), USER3_NORMAL_PRIV3  ACCOUNT_DEFAULT_STRING, username, cpwd);
		F_PUTS_NORMAL(pwd_buf, fp);
	}
		
#if defined(TCSUPPORT_CT_JOYME4)
	/* add telnet root account for su command. */
	if ( cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "Active", tmpattr, sizeof(tmpattr)) > 0
		&& 0 == strcmp(tmpattr, "Yes") )
	{
		bzero(username, sizeof(username));
		if ( cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_username", username, sizeof(username)) <= 0
			|| 0 == username[0] )
		{
			snprintf(username, sizeof(username), "%s", "telnetadmin_super");
			cfg_set_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_username", username);
		}

		bzero(tmpattr, sizeof(tmpattr));
		if ( cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_passwd", tmpattr, sizeof(tmpattr)) <= 0
			|| 0 == tmpattr[0] )
		{
			snprintf(tmpattr, sizeof(tmpattr), "%s", "telnetadmin");
			cfg_set_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_passwd", tmpattr);
		}

		cpwd = crypt(tmpattr, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, username, cpwd);
		F_PUTS_NORMAL(pwd_buf, fp);
	}
#endif

	fclose(fp);

telnet_config:
	/*operate telnet info */
	memset(pwd_buf, 0, sizeof(pwd_buf));
	memset(username, 0, sizeof(username));

	writeTelnetAccountfile();

ftp_config:
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
	AddFtpAccount();
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	strncpy(ftpPath, ACCOUNT_FTPCOMMON_NODE, sizeof(ftpPath)-1);
#else
	strncpy(ftpPath, ACCOUNT_FTPENTRY_NODE, sizeof(ftpPath)-1);
#endif

	if(cfg_query_object(ftpPath,NULL,NULL) < 0)
	{
		goto console_config;
	}
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
    memset(Right, 0, sizeof(Right));
	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(ftpPath, "ftp_Right", tmpattr, sizeof(tmpattr)) <= 0)
	{
		strncpy(Right,"1", sizeof(Right)-1);
	}
	else
	{
		snprintf(Right,sizeof(Right), "%s", tmpattr);
	}
	ConfigRight(Right,CONFIGFTPRIGHT);

	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(ftpPath, "ftp_Anonymous", tmpattr, sizeof(tmpattr)) <= 0)
	{
		strncpy(Anonymous,"FALSE", sizeof(Anonymous)-1);
	}
	else
	{
		snprintf(Anonymous,sizeof(Anonymous), "%s", tmpattr);
	}
	ConfigAnonymous(Anonymous);
#else
	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(ftpPath, "ftp_username", tmpattr, sizeof(tmpattr)) <= 0)
	{
		goto console_config;
	}
	snprintf(username, sizeof(username), "%s", tmpattr);

	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(ftpPath, "ftp_passwd", tmpattr, sizeof(tmpattr)) <= 0)
	{
		goto console_config;
	}
	cpwd = crypt(tmpattr, SLAT);
	snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING,username, cpwd);
	
	/*Create ftp file,store username and passwd*/
	if(write2file(pwd_buf, FTPPASSWD_PATH) == FAIL)
	{
		goto console_config;
	}
#endif
	/*modify ftp port*/
	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(ftpPath, "ftp_port", tmpattr, sizeof(tmpattr)) <= 0)
	{
		goto console_config;
	}
	if(parseftpport(tmpattr) < 0)
	{
		goto console_config;
	}
console_config:
	/*operate console info */
	memset(pwd_buf, 0, sizeof(pwd_buf));
	memset(username, 0, sizeof(username));
	strncpy(consolePath, ACCOUNT_CONSOLEENTRY_NODE, sizeof(consolePath)-1);
	if(cfg_query_object(consolePath,NULL,NULL) < 0)
	{
		return FAIL;
	}

	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(consolePath, "Active", tmpattr, sizeof(tmpattr)) <= 0)
	{
		return FAIL;
	}
	unlink(CONSOLE_PATH2);
	if( !strcmp(tmpattr, ACTIVE) )
	{
		memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(consolePath, "console_username", tmpattr, sizeof(tmpattr)) <= 0)
	{
		return FAIL;
	}
	snprintf(username, sizeof(username), "%s", tmpattr);

	memset(tmpattr, 0, sizeof(tmpattr));
	if(cfg_get_object_attr(consolePath, "console_passwd", tmpattr, sizeof(tmpattr)) <= 0)
	{
		return FAIL;
	}
	cpwd = crypt(tmpattr, SLAT);
	snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING,username, cpwd);

	/*Create usertty file,store username and passwd*/
	if(write2file(pwd_buf, CONSOLE_PATH2) == FAIL)
	{
		return FAIL;
	}
	}
	return retval;
}/* end account_write */

#if defined(TCSUPPORT_CMCCV2)
#define TELNET_CMD "/usr/bin/utelnetd -p %d -i %s -l /bin/login -P %s -d"
#define WAN_TELNET_PID "/var/run/wan_telnet.pid"
#define LAN_TELNET_PID "/var/run/lan_telnet.pid"
#endif
int cfg_type_account_execute(char* path)
{
	char active[10] = {0};
	char portbuf[10] = {0};
	char cmdbuf[100] = {0};
	char pidbuf[64] = {0};
#if defined(TCSUPPORT_CMCCV2)
	char ifname[64] = {0};
#endif
	char pidallbuf[128] = {0};
	int portnum = 0;
#ifdef TCSUPPORT_SYSLOG       
	char ip[32] = {0};
	char times[4] = {0};
	char log[128] = {0};
#endif

	if(strstr(path,FTP_ENTRY))
	{
		system("killall -9 bftpd");
#if defined(TCSUPPORT_CT_JOYME2)		
		if(cfg_get_object_attr(ACCOUNT_FTPCOMMON_NODE, ACCOUNT_NODE_ACTIVE, active, sizeof(active)) <  0)
		{
			goto getattrerror;
		}
#else
		if(cfg_get_object_attr(ACCOUNT_FTPENTRY_NODE, ACCOUNT_NODE_ACTIVE, active, sizeof(active)) <  0)
		{
			goto getattrerror;
		}
#endif
		if(!strcmp(active,ACTIVE))
		{
			system("taskset 2 /userfs/bin/bftpd -d");
		}
	}
#if defined(TCSUPPORT_CMCCV2)
	else if(strstr(path,WANTELNET_ENTRY))
	{
		if(cfg_get_object_attr(path, ACCOUNT_NODE_ACTIVE, active, sizeof(active)) <= 0)
		{
			goto getattrerror;
		}

		kill_process(WAN_TELNET_PID);
		if(!strcmp(active,ACTIVE))
		{
			if(cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_port", portbuf, sizeof(portbuf)) > 0 
				&& cfg_get_object_attr(path, "IFName", ifname, sizeof(ifname)) > 0)
			{
				portnum = atoi(portbuf);
				if(portnum == 0)
				{
					strncpy(portbuf,TELNETDEFAULTPORT, sizeof(portbuf)-1);
					portnum = 23;
				}
				snprintf(cmdbuf, sizeof(cmdbuf), TELNET_CMD, portnum, ifname, WAN_TELNET_PID);
				system(cmdbuf);
			}
		}
	}
#endif
	else if(strstr(path,TELNET_ENTRY))
	{
		if (cfg_get_object_attr("root.deviceinfo", "norestart", active, sizeof(active)) >= 0) {
			if (!strcmp(active, "1")) {
				cfg_set_object_attr("root.deviceinfo", "norestart", "0");
				tcdbg_printf("\r\nno need restart telnet");
				goto getattrerror;
			}
		}
	
		if(cfg_get_object_attr(path, ACCOUNT_NODE_ACTIVE, active, sizeof(active)) <	0)
		{
			goto getattrerror;
		}

#if defined(TCSUPPORT_CMCCV2)
		kill_process(LAN_TELNET_PID);
#else
		system("killall -9 utelnetd");
#endif
		if(!strcmp(active,ACTIVE))
		{
			if(cfg_get_object_attr(path, "telnet_port", portbuf, sizeof(portbuf)) <  0)
			{
				goto getattrerror;
			}
			else
			{
				portnum = atoi(portbuf);
				if(portnum == 0)
				{
					strncpy(portbuf,TELNETDEFAULTPORT, sizeof(portbuf)-1);
					portnum = 23;
				}
#if defined(TCSUPPORT_CMCCV2)
				snprintf(cmdbuf, sizeof(cmdbuf), TELNET_CMD, portnum, "br0", LAN_TELNET_PID);
#else
				snprintf(cmdbuf, sizeof(cmdbuf), "taskset 2 /usr/bin/utelnetd -p %d -l /bin/login -d",portnum);
#endif
				system(cmdbuf);
			}
		}
	}
	else if(strstr(path,CONSOLE_ENTRY))
	{
		memset(pidbuf, 0, sizeof(pidbuf));
		fileRead(CONSOLE_PID_PATH, pidbuf, sizeof(pidbuf));
		if ( '\0' != pidbuf[0] )
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "pidof sh > %s", CONSOLE_PID_TEMP_PATH);
			system(cmdbuf);
			fileRead(CONSOLE_PID_TEMP_PATH, pidallbuf, sizeof(pidallbuf));
			unlink(CONSOLE_PID_TEMP_PATH);
			if ( NULL != strstr(pidallbuf, pidbuf) )
				kill_process(CONSOLE_PID_PATH);
		}
	}
	else
	{
	#ifdef TCSUPPORT_SYSLOG
		if(cfg_get_object_attr(path, "LoginTimes", times, sizeof(times)) < 0)
		{
			goto getattrerror;
		}		
		if(!strcmp(times,"3"))
		{
			if(cfg_get_object_attr(path, "LoginIp", ip, sizeof(ip)) <  0)
			{
				goto getattrerror;
			}	
#if defined(TCSUPPORT_C1_CUC)
			snprintf(log, sizeof(log), " ALARM LEV-3 ASTATUS-2 EVENTID-104032 WEB ip <%s> login failed 3 times!\n", ip);
#else
			snprintf(log, sizeof(log), "104032 WEB ip <%s> login failed 3 times!\n", ip);
#endif
			openlog("TCSysLog WEB", 0, LOG_LOCAL2);
			syslog(LOG_ALERT, log);
			closelog();
		}
	#else
		printf("\r\noperate web info,do nothing!");
	#endif
	}
	
	return SUCCESS;

getattrerror:	
	return FAIL;	
	
}/* end account_execute*/

int cfg_type_account_func_commit_core(char* path)
{
	cfg_type_account_write();
	return cfg_type_account_execute(path);
}

int svc_other_handle_event_account_boot(void)
{
	char active[10] = {0};
	char port[10] = {0};
	char tmp[150] = {0};
	int portnum = 0;
	char nodePath[64] = {0};
	
#if !defined(TCSUPPORT_CUC)
#if defined(TCSUPPORT_CT_PON)
	int disp_mask[9] = {0};
	char attr_mask[64] = {0};
#endif
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#define INTER_CONNDV3_TCP   ",32768"
#define INTER_CONNDV3_UDP   ",32767"
#else
#define INTER_CONNDV3_TCP   ""
#define INTER_CONNDV3_UDP   ""
#endif
#if defined(TCSUPPORT_CT_JOYME4_JC)
	char telnetEnable[4] = {0};
#endif

#if defined(TCSUPPORT_CFG_NG_UNION) || defined(TCSUPPORT_CT_JOYME4)
	cfg_obj_set_object_attr(WANINFO_COMMON_NODE, "NoGUIAccessLimit", 0, "1");
#endif

#if defined(TCSUPPORT_CT_JOYME4_JC)
	if ( cfg_get_object_attr(DEVICEINFO_DEVPARADYNAMIC_NODE, "telnetEnable", telnetEnable, sizeof(telnetEnable)) > 0 )
	{
		if ( 0 == strcmp(telnetEnable, "1") )
		{
			/* enable telnet by prolinecmd */
			cfg_obj_set_object_attr(ACCOUNT_TELNETENTRY_NODE, "Active", 0, "Yes");
		}
		else if ( 0 == strcmp(telnetEnable, "0") )
		{
			/* disable telnet by prolinecmd */
			cfg_obj_set_object_attr(ACCOUNT_TELNETENTRY_NODE, "Active", 0, "No");
		}
	}
#endif

	cfg_type_account_write( );

	/*add by brian for not allow to login in in the wan side*/
#if !defined(TCSUPPORT_CMCCV2)	
#if !defined(TCSUPPORT_CT_JOYME2)
	system("iptables -t filter -A INPUT -p TCP --dport 80 -i nas+ -j DROP");
    system("iptables -t filter -A INPUT -p TCP --dport 80 -i ppp+ -j DROP");
#endif
	system("ip6tables -t filter -A INPUT -p TCP --dport 80 -i ppp+ -j DROP");
	system("ip6tables -t filter -A INPUT -p TCP --dport 80 -i nas+ -j DROP");
	
	system("iptables -t filter -A INPUT -p TCP --dport 12345 -i nas+ -j DROP");
	system("iptables -t filter -A INPUT -p TCP --dport 12345 -i ppp+ -j DROP");
	system("ip6tables -t filter -A INPUT -p TCP --dport 12345 -i ppp+ -j DROP");
	system("ip6tables -t filter -A INPUT -p TCP --dport 12345 -i nas+ -j DROP");
	
	system("iptables -t filter -A INPUT -p TCP --dport 4011 -i nas+ -j DROP");
	system("iptables -t filter -A INPUT -p TCP --dport 4011 -i ppp+ -j DROP");
	system("ip6tables -t filter -A INPUT -p TCP --dport 4011 -i ppp+ -j DROP");
	system("ip6tables -t filter -A INPUT -p TCP --dport 4011 -i nas+ -j DROP");
#endif

#if defined(TCSUPPORT_CMCCV2)	
	char Right[4] = {0};
	snprintf(nodePath, sizeof(nodePath), ACCOUNT_ENTRY2_NODE);
	if(cfg_get_object_attr(nodePath, "Right", Right, sizeof(Right)) <= 0)
	{
		strncpy(Right,"1", sizeof(Right) - 1);
	}
	ConfigRight(Right,CONFIGHTTPRIGHT);
	
	memset(nodePath, 0, sizeof(nodePath));
	memset(Right, 0, sizeof(Right));
	snprintf(nodePath, sizeof(nodePath), ACCOUNT_FTPENTRY_NODE);
	if(cfg_get_object_attr(nodePath, "ftp_Right", Right, sizeof(Right)) <= 0)
	{
		strncpy(Right, "1", sizeof(Right) - 1 );
	}
	ConfigRight(Right,CONFIGFTPRIGHT);
	memset(nodePath, 0, sizeof(nodePath));
	memset(Right, 0, sizeof(Right));
	snprintf(nodePath, sizeof(nodePath), ACCOUNT_TELNETENTRY_NODE);
	if(cfg_get_object_attr(nodePath, "telnet_Right", Right, sizeof(Right)) <= 0)
	{
		strncpy(Right, "1", sizeof(Right) - 1 );
	}
	ConfigRight(Right,CONFIGTELNETRIGHT);
#endif
#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG)
	/* CT technical specification: LAND attack prevention and log. */
	system("iptables -t mangle -N LAND");
	system("iptables -t mangle -A PREROUTING -j LAND");
#endif

#if defined(TCSUPPORT_CT_PORTSLIMIT)
	system("iptables -t filter -N PORTS_LIMIT");
	system("iptables -t filter -A INPUT -j PORTS_LIMIT");
	system("iptables -t filter -F PORTS_LIMIT");
	system("iptables -t filter -Z PORTS_LIMIT");

	/*DROP wan ports packets for router mode */
	system("iptables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 53,3555,5555,49152,49153"INTER_CONNDV3_TCP" -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 53,3555,5555,49152,49153"INTER_CONNDV3_TCP" -i nas+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 53,3555,67,1900"INTER_CONNDV3_UDP" -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 53,3555,67,1900"INTER_CONNDV3_UDP" -i nas+ -j DROP");
#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
	system("iptables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 53,3555,5555,49152,49153 -i apcli+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 53,3555,5555,49152,49153 -i apclii+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 53,3555,67,1900 -i apcli+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 53,3555,67,1900 -i apclii+ -j DROP");
#endif
#ifdef TCSUPPORT_CT_JOYME	
	system("iptables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 3456,3457,8080 -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 3456,3457,8080 -i nas+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 3456,3457,8080 -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 3456,3457,8080 -i nas+ -j DROP");
#endif

#if defined(TCSUPPORT_MAP_WAPP)
	/* DROP wan 3517 port packets for ipv4 route mode */
	system("iptables -t filter -A PORTS_LIMIT -p TCP --dport 3517 -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p TCP --dport 3517 -i nas+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP --dport 3517 -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p UDP --dport 3517 -i nas+ -j DROP");
#endif

#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
	system("iptables -t filter -A PORTS_LIMIT -p TCP --dport 17998 -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p TCP --dport 17998 -i nas+ -j DROP");
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	/* DROP wan port packets for ipv4 port 8080 route mode  */
	system("iptables -t filter -A PORTS_LIMIT -p TCP --dport 8080 -i ppp+ -j DROP");
	system("iptables -t filter -A PORTS_LIMIT -p TCP --dport 8080 -i nas+ -j DROP");

	/* DROP wan port packets for ipv6 route mode */
	system("ip6tables -t filter -N PORTS_LIMIT");
	system("ip6tables -t filter -A INPUT -j PORTS_LIMIT");
	system("ip6tables -t filter -F PORTS_LIMIT");
	system("ip6tables -t filter -Z PORTS_LIMIT");

	system("ip6tables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 53,8080 -i ppp+ -j DROP");
	system("ip6tables -t filter -A PORTS_LIMIT -p TCP -m multiport --dport 53,8080 -i nas+ -j DROP");
	system("ip6tables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 53,8080 -i ppp+ -j DROP");
	system("ip6tables -t filter -A PORTS_LIMIT -p UDP -m multiport --dport 53,8080 -i nas+ -j DROP");
#endif

#if defined(TCSUPPORT_MAP_WAPP)
	/* DROP wan 3517 port packets for ipv6 route mode */
	system("ip6tables -t filter -A PORTS_LIMIT -p TCP --dport 3517 -i ppp+ -j DROP");
	system("ip6tables -t filter -A PORTS_LIMIT -p TCP --dport 3517 -i nas+ -j DROP");
	system("ip6tables -t filter -A PORTS_LIMIT -p UDP --dport 3517 -i ppp+ -j DROP");
	system("ip6tables -t filter -A PORTS_LIMIT -p UDP --dport 3517 -i nas+ -j DROP");
#endif

#if defined(TCSUPPORT_CT_JOYME4)
	/* CT technical specification: CPE will not forward ICMPv6 packet which is sent from LAN with wrong IPv6 prefix. */
	system("ip6tables -t filter -N LAN_DROP_ICMPV6");
	system("ip6tables -I FORWARD -j LAN_DROP_ICMPV6");
	system("ip6tables -A LAN_DROP_ICMPV6 -i br0 -p icmpv6 -j DROP");
#endif

	system("ebtables -t filter -N PORTS_LIMIT");
	system("ebtables -t filter -P PORTS_LIMIT RETURN");
	system("ebtables -A INPUT -j PORTS_LIMIT");
	system("ebtables -t filter -F PORTS_LIMIT");
	system("ebtables -t filter -Z PORTS_LIMIT");

	/* DROP wan ports packets for bridge mode */
#if !defined(TCSUPPORT_NP_CMCC)
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 80 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 6 --ip6-dport 80 -i nas+ -j DROP");
#endif
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 5555 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 49152 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 49153 -i nas+ -j DROP");

	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 53 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 3555 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 67 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 17 --ip6-dport 53 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 17 --ip6-dport 3555 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 17 --ip6-dport 547 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 1900 -i nas+ -j DROP");
#ifdef TCSUPPORT_CT_JOYME
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 3456 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 3457 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 8080 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 3456 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 3457 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 17 --ip-dport 8080 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 6 --ip6-dport 3456 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 6 --ip6-dport 3457 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 6 --ip6-dport 8080 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 17 --ip6-dport 3456 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 17 --ip6-dport 3457 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 17 --ip6-dport 8080 -i nas+ -j DROP");
#endif
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
	system("ebtables -t filter -A PORTS_LIMIT -p IPv4 --ip-proto 6 --ip-dport 17998 -i nas+ -j DROP");
	system("ebtables -t filter -A PORTS_LIMIT -p IPv6 --ip6-proto 6 --ip6-dport 17998 -i nas+ -j DROP");
#endif
	system("ebtables -t filter -N PORTS_LIMIT_OUT");
	system("ebtables -t filter -P PORTS_LIMIT_OUT RETURN");
	system("ebtables -A OUTPUT -j PORTS_LIMIT_OUT");
	system("ebtables -t filter -F PORTS_LIMIT_OUT");
	system("ebtables -t filter -Z PORTS_LIMIT_OUT");

	/* FORBIDDEN CPE RA packets to wan for bridge mode*/
	system("ebtables -t filter -A PORTS_LIMIT_OUT -o nas+ -p IPv6 --ip6-proto 58 --ip6-icmpv6type 134 -j DROP");
#endif

#if !defined(TCSUPPORT_CUC)
#if defined(TCSUPPORT_CT_PON)
	/* 1. get old display mask */
	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), ACCOUNT_ENTRY2_NODE);
	if(cfg_get_object_attr(nodePath, "display_mask", attr_mask, sizeof(attr_mask)) > 0  &&  0 != attr_mask[0])
	{
		if ( 9 == sscanf(attr_mask, "%02X %02X %02X %02X %02X %02X %02X %02X %02X",
			&disp_mask[0], &disp_mask[1], &disp_mask[2], &disp_mask[3], &disp_mask[4],
			&disp_mask[5], &disp_mask[6], &disp_mask[7], &disp_mask[8]) )
		{
			/* 2. add net-tr069.asp display mask */
			disp_mask[3] |= 0x10;
			disp_mask[0] |= 0x04;

			/* 3. save it */
			snprintf(attr_mask, sizeof(attr_mask), "%02X %02X %02X %02X %02X %02X %02X %02X %02X",
			disp_mask[0], disp_mask[1], disp_mask[2], disp_mask[3], disp_mask[4],
			disp_mask[5], disp_mask[6], disp_mask[7], disp_mask[8]);
			cfg_set_object_attr(nodePath, "display_mask", attr_mask);
		}
	}
#endif
#endif

	/*start up telnet application */
	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), ACCOUNT_TELNETENTRY_NODE);
	if(cfg_get_object_attr(nodePath, "Active", active, sizeof(active)) > 0)
	{
		if(!strcmp(active,ACTIVE))
		{
			if(cfg_get_object_attr(nodePath, "telnet_port", port, sizeof(port)) > 0)
			{
				portnum = atoi(port);
				if(portnum == 0)
				{
					strncpy(port,TELNETDEFAULTPORT,sizeof(port)-1);
					portnum = 23;
				}
				
#if defined(TCSUPPORT_CMCCV2)
				snprintf(tmp, sizeof(tmp), TELNET_CMD, portnum, "br0", LAN_TELNET_PID);
#else
				snprintf(tmp, sizeof(tmp), "taskset 2 /usr/bin/utelnetd -p %d -l /bin/login -d",portnum);
#endif
				system(tmp);
			}
			else
			{
				goto getattrerror;
			}	
		}
	}
	else
	{
		goto getattrerror;
	}

	/*start up tftp application */
	memset(nodePath, 0, sizeof(nodePath));
#if defined(TCSUPPORT_CT_JOYME2)
	strncpy(nodePath, ACCOUNT_FTPCOMMON_NODE, sizeof(nodePath)-1);
#else
	strncpy(nodePath, ACCOUNT_FTPENTRY_NODE, sizeof(nodePath)-1);
#endif
	if(cfg_get_object_attr(nodePath, "Active", active, sizeof(active)) > 0)
	{
		if(!strcmp(active,ACTIVE))
		{
			if(cfg_get_object_attr(nodePath, "ftp_port", port, sizeof(port)) > 0)
			{
				portnum = atoi(port);
				if(portnum == 0)
				{
					strncpy(port,FTPDEFAULTPORT, sizeof(port)-1);
				}
				if(parseftpport(port) < 0)
				{
					printf("\n [%s],line = %d, parse ftp port fail,return -1 ==================>\n",__FUNCTION__,__LINE__);
					return -1;
				}
				system("taskset 2 /userfs/bin/bftpd -d");
			}
			else
			{
				goto getattrerror;
			}
		}
	}
	else
	{
		goto getattrerror;
	}
	
	cfg_type_account_func_commit_core(ACCOUNT_NODE);
	return 0;
	
getattrerror:	
	cfg_type_account_func_commit_core(ACCOUNT_NODE);
	return -1;

}

static int svc_other_handle_event_process_update()
{
	int ret = 0;
	
	system("/usr/bin/killall -9 process_monitor");
	system("/userfs/bin/process_monitor &");
	ret = ecnt_event_send(ECNT_EVENT_DBUS,
			ECNT_EVENT_DBUS_RESOURCE,
			NULL,
			0);
	return 0;
}

int svc_other_mgr_external_handle_event(unsigned int event)
{
	int ret = 0;

	switch ( event )
	{
		case EVT_CFG_OTHER_SNMP_SIGNAL:
			ret = svc_send_signal_snmpd();
			break;
	}

	return ret;
}
#ifdef TCSUPPORT_CT_JOYME4
void Set_cpu_band_default()
{
    char cmd_buf[64] = {0};
    int i = 0, pidnum = 0, pid_t[128] = {0},bandcpu = 1;	
    char process[][32]= {"tr69","ctc_igd1_dbus","lan_host_mgr"};
	
    for(i =0;i<3;i++)
    {
      find_pid_by_name( process[i], pid_t, &pidnum);
      bandcpu = i%2+1; /*band to cpu0 or cpu1*/
      if(pidnum == 1 )
      {
        memset(cmd_buf, 0, 64);
        sprintf(cmd_buf, "taskset -p %d %d", bandcpu, pid_t[0]);
        system (cmd_buf);
      }
    }

    return;
}

#endif

void checkOtherSvcBoot(){
	int ret = 0;
	char lanBootSent[4] = {0};

	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "LanBootSent", 0, lanBootSent, sizeof(lanBootSent));

	if(atoi(lanBootSent) == 1){
		tcdbg_printf("checkOtherSvcBoot enter===> \n");
		svc_other_handle_event_lan_write();
		ret = svc_other_handle_event_lan_boot(LAN_ENTRY_NODE);
		ret = svc_other_handle_event_account_boot();
		ret = svc_other_handle_event_dhcpd_boot();
	}

	return;
}

int svc_other_mgr_handle_event(unsigned int event, void* param)
{
	int ret = 0;
	char path[64]={0};
	int flag = 0;
	other_evt_t* p_other_evt = NULL;
	int index = -1;
	
	p_other_evt = (other_evt_t*)param;

	if(p_other_evt && p_other_evt->buf[0] != '\0'){
		printf("svc_other_mgr_handle_event: event [%u] [%s]  \n", event, p_other_evt->buf);	
		strncpy(path, p_other_evt->buf, sizeof(path) - 1);
		flag = p_other_evt->flag;
	}		
	
	switch(event)
	{
		case EVT_CFG_OTHER_LAN_BOOT:
                        cfg_obj_set_object_attr(GLOBALSTATE_COMMON_NODE, "LanBootSent", 0, "2");
			svc_other_handle_event_lan_write();
			ret = svc_other_handle_event_lan_boot(path);
			break;
		case EVT_CFG_OTHER_LAN_UPDATE:
			ret = svc_other_handle_event_lan_update(path);
			break;
#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
		case EVT_CFG_OTHER_SAMBA_BOOT:
			svc_other_handle_event_samba_write(SAMBA_SERVER_SAMBA_COMMIT_FLAG, path);
			ret = svc_other_handle_event_samba_boot(path);
			break;
		case EVT_CFG_OTHER_SAMBA_UPDATE:
			ret = svc_other_handle_event_samba_update(flag, path);
			break;
#endif
		case EVT_CFG_OTHER_ACCOUNT_UPDATE:
			cfg_type_account_write();
			ret = cfg_type_account_execute(path);
			break;
		case EVT_CFG_OTHER_ACCOUNT_BOOT:
			ret = svc_other_handle_event_account_boot();
			break;

		case EVT_CFG_OTHER_ARP_RULE_CHG:
			ret = svc_other_handle_event_arprule_change();
			break;
		case EVT_CFG_OTHER_SNMPD_UPDATE:
			ret = svc_other_handle_event_snmpd_update(path);
			break;
		case EVT_CFG_OTHER_SNMPD_BOOT:
			svc_other_handle_event_snmpd_write(path);
			ret = svc_other_handle_event_snmpd_boot();
			break;
		case EVT_CFG_OTHER_UPNPD_UPDATE:
			ret = svc_other_handle_event_upnpd_update(path);
			break;
		case EVT_CFG_OTHER_UPNPD_BOOT:
			svc_other_handle_event_upnpd_write(path);
			ret = svc_other_handle_event_upnpd_boot();
			break;
		case EVT_CFG_OTHER_RADVD_BOOT:
			svc_other_handle_event_radvd_write(path);
			ret = svc_other_handle_event_radvd_boot(path);
			break;
		case EVT_CFG_OTHER_RADVD_UPDATE:
			ret = svc_other_handle_event_radvd_update(path);
			break;
		case EVT_CFG_OTHER_DHCP6S_BOOT:
			svc_other_handle_event_dhcp6s_write(path);
			ret = svc_other_handle_event_dhcp6s_boot(path);
			break;
		case EVT_CFG_OTHER_DHCP6S_UPDATE:
			ret = svc_other_handle_event_dhcp6s_update(path);
			break;
		case EVT_CFG_OTHER_DHCPD_BOOT:
			ret = svc_other_handle_event_dhcpd_boot();
			break;
		case EVT_CFG_OTHER_DHCPD_UPDATE:
			ret = svc_other_handle_event_dhcpd_update();
			break;
		case EVT_CFG_OTHER_DHCP_RELAY_BOOT:
			ret = svc_other_handle_event_dhcp_relay_boot();
			break;
		case EVT_CFG_OTHER_DHCP_RELAY_UPDATE:
			ret = svc_other_handle_event_dhcp_relay_update();
			break;
		case EVT_CFG_OTHER_PARENTAL_BOOT:
			ret = svc_other_handle_event_parental_boot();
			break;
		case EVT_CFG_OTHER_PARENTAL_UPDATE:
			ret = svc_other_handle_event_parental_update();
			break;
		case EVT_CFG_OTHER_PROCESS_UPDATE:
			ret = svc_other_handle_event_process_update();
			break;
		case EVT_CFG_OTHER_PARENTAL_MAC_BOOT:
			ret = svc_other_handle_event_parental_mac_boot();
			break;
		case EVT_CFG_OTHER_PARENTAL_MAC_UPDATE:
			ret = svc_other_handle_event_parental_mac_update();
			break;
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
		case EVT_CFG_OTHER_TRAFFICDETAIL_BOOT:
		case EVT_CFG_OTHER_TRAFFICDETAIL_UPDATE:
			svc_other_handle_event_trafficdetail_write();
			ret = svc_other_handle_event_trafficdetail_boot();
			break;
		case EVT_CFG_OTHER_TRAFFICMONITOR_BOOT:
		case EVT_CFG_OTHER_TRAFFICMONITOR_UPDATE:
			svc_other_handle_event_trafficmonitor_write();
			ret = svc_other_handle_event_trafficmonitor_boot();
			break;
		case EVT_CFG_OTHER_TRAFFICMIRROR_BOOT:
		case EVT_CFG_OTHER_TRAFFICMIRROR_UPDATE:
			svc_other_handle_event_trafficmirror_write();
			ret = svc_other_handle_event_trafficmirror_boot();
			break;
#endif
		case EVT_CFG_OTHER_LANHOST2_UPDATE:
			ret = svc_other_handle_event_lanhost2_config();
			break;
		case EVT_CFG_OTHER_LANHOST2_BOOT:
			init_lanhost2black_node();
			ret = svc_other_handle_event_lanhost2_config();
			system("/userfs/bin/lan_host_mgr &");
		#ifdef TCSUPPORT_CT_JOYME4
			Set_cpu_band_default();
		#endif
			break;
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CUC)
		case EVT_CFG_OTHER_SPEEDTEST_START:
#if defined(TCSUPPORT_CUC)
			ret = svc_other_handle_event_speedtest_execute(path);
#else
			creat_other_cfg_speedtest_pthread();
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		case EVT_CFG_OTHER_BRIDGEDATA_UPDATE:
			svc_other_handle_event_bridgedata_update();
			break;
#endif
			break;
#endif
		case EVT_CFG_OTHER_SYSLOG_BOOT:
			svc_other_handle_event_syslog_boot();
			break;
		case EVT_CFG_OTHER_SYSLOG_UPDATE:
			svc_other_handle_event_syslog_update(path);
			break;
#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
		case EVT_CFG_OTHER_PPPOE_EMULATOR_BOOT:
			ret = svc_other_handle_event_pppoeemulator_boot();
			break;
		case EVT_CFG_OTHER_PPPOE_EMULATOR_UPDATE:
			ret = svc_other_handle_event_pppoeemulator_update();
			break;
#endif
#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
		case EVT_CFG_OTHER_IPOE_EMULATOR_BOOT:
			ret = svc_other_handle_event_ipoe_emulator_boot();
			break;
		case EVT_CFG_OTHER_IPOE_EMULATOR_UPDATE:
			ret = svc_other_handle_event_ipoe_emulator_update();
			break;
#endif
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
		case EVT_CFG_OTHER_CTDIAGNOSTIC_BOOT:
			svc_other_handle_event_ctdiagnostic_boot();
			break;	
		case EVT_CFG_OTHER_CTDIAGNOSTIC_UPDATE:
			svc_other_handle_event_ctdiagnostic_update(path);
			break;	
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
		case EVT_CFG_OTHER_IPOEDIAG_UPDATE:
			sscanf(path, IPOEDIAG_ENTRY_NODE, &index);
			svc_other_handle_event_ipoe_update(index);
			break;		
#endif

		default:
			printf("svc_other_mgr_handle_event: path[%s]  event [%d]  \n",path,event);	
	}

	return ret;
}


int svc_other_start_lan(void)
{
	return 0;
}

int svc_other_stop_lan(void)
{
	return 0;
}

