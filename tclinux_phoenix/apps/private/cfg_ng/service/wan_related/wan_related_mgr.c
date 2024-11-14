
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
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <pthread.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_cli.h> 
#include "utility.h" 
#include "../cfg/cfg_msg.h"
#include "wan_related_mgr.h"
#include "wan_related_cfg.h"
#include "wan_related_qos_cfg.h"
#include "wan_related_maintenance.h"
#include "ping_diagnostic.h"
#include "wan_related_conf.h"
#include "cfg_msg.h"
#include "wan_related_vpn_cfg.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#if defined(TCSUPPORT_VXLAN)
#include "wan_related_vxlan_cfg.h"
#endif
#include "blapi_system.h"

#define NODENAME 32
#define FIREWALL_LEVELCMD "/usr/bin/firewallcmd level %s"
#define FIREWALL_ON "/usr/bin/firewallcmd switch on"
#define FIREWALL_OFF "/usr/bin/firewallcmd switch off"

#if defined(TCSUPPORT_CMCCV2)
#define FIREWALL_DOS_ON "/usr/bin/firewallcmd DoSSwitch on"
#define FIREWALL_DOS_OFF "/usr/bin/firewallcmd DoSSwitch off"
#define FIREWALL_PORTSCAN_ON "/usr/bin/firewallcmd PortScanSwitch on"
#define FIREWALL_PORTSCAN_OFF "/usr/bin/firewallcmd PortScanSwitch off"
#endif

#define PPPOE_EMULATOR_PPPNAME "ppp199"
#define IPFILTER_VECTOR (1<<3)
#if defined(TCSUPPORT_CT_NEWGUI)
#define FIREWALL_DoSProtectCMD "/usr/bin/firewallcmd DoSSwitch %s"
#endif

#if defined(TCSUPPORT_IPV6_FIREWALL)
#define FW6_CHAIN_NAME "INPUT_FW"
#define FW6_SPI_NAME "INPUT_SPI"

#define FIREWALL_IPV6_ON_HIGH 		"/usr/bin/firewallcmd ipv6 on -C INPUT_FW -L high"
#define FIREWALL_IPV6_ON_MEDIUM	"/usr/bin/firewallcmd ipv6 on -C INPUT_FW -L medium"
#define FIREWALL_IPV6_ON_LOW		"/usr/bin/firewallcmd ipv6 on -C INPUT_FW -L low"
#define FIREWALL_IPV6_OFF			"/usr/bin/firewallcmd ipv6 off -C INPUT_FW"

#define FIREWALL_IPV6SPI_ON		"/usr/bin/firewallcmd ipv6spi on"		
#define FIREWALL_IPV6SPI_OFF		"/usr/bin/firewallcmd ipv6spi off"		
#endif

#ifdef CT_COM_DEVICEREG
/* prev flag for device registration*/
int prevDevRegFlag = 1;
#endif

#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE_NUM        (32)
#else
#define MAX_STATIC_ROUTE_NUM        (16)
#endif

#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE6_NUM       (32)
#else
#define MAX_STATIC_ROUTE6_NUM       (16) 
#endif

#ifdef TCSUPPORT_VPN
extern int svc_wan_related_ipsec_update(void);
#endif

int svc_wan_related_qos_update(wan_related_cfg_update_t* param)
{	
	return svc_wan_related_qos_execute(param);
}
#if defined(TCSUPPORT_CT_JOYME4)
int svc_wan_related_downlinkqos_update(wan_related_cfg_update_t* param)
{	
	return svc_wan_related_downlinkqos_execute(param);
}
#endif
#if defined(TCSUPPORT_STUN)
int svc_wan_related_stun_update()
{	
	char cmd[256] = {0},serverAddr[64] = {0},enable[8] = {0},ifip[32] = {0};
	char port[16] = {0},serverPort[16] = {0},min[16] = {0},max[16] = {0};
	
	memset(enable,0,sizeof(enable));
	if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "Enable", 0, enable, sizeof(enable)) > 0)
	{
		if (enable[0] == '1') 
		{
			system("/usr/bin/killall -9 stun-client");
			memset(serverAddr,0,sizeof(serverAddr));
			if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "ServerAddr", 0, serverAddr, sizeof(serverAddr)) <= 0 
				|| serverAddr[0] == '\0' )
			{
				tcdbg_printf("%s:%d No Valid Server Address\n",__FUNCTION__,__LINE__);
				return FAIL;
			}
			
			memset(port,0,sizeof(port));
			if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "Port", 0, port, sizeof(port)) <= 0 
				|| port[0] == '\0' )
			{
				tcdbg_printf("%s:%d No Valid Port\n",__FUNCTION__,__LINE__);
				return FAIL;
			}
			
			memset(serverPort,0,sizeof(serverPort));
			if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "ServerPort", 0, serverPort, sizeof(serverPort)) <= 0 
				|| serverPort[0] == '\0' )
			{
				tcdbg_printf("%s:%d No Valid Port\n",__FUNCTION__,__LINE__);
				return FAIL;
			}
			
			memset(min,0,sizeof(min));
			if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "Min", 0, min, sizeof(min)) <= 0 
				|| min[0] == '\0' )
			{
				tcdbg_printf("%s:%d No Keep Alive Min\n",__FUNCTION__,__LINE__);
				return FAIL;
			}
			
			memset(max,0,sizeof(max));
			if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "Max", 0, max, sizeof(max)) <= 0 
				|| max[0] == '\0' )
			{
				tcdbg_printf("%s:%d No Keep Alive Max\n",__FUNCTION__,__LINE__);
				return FAIL;
			}
			
			memset(ifip,0,sizeof(ifip));
			if(cfg_obj_get_object_attr(STUN_ENTRY_NODE, "IFIP", 0, ifip, sizeof(ifip)) <= 0 
				|| ifip[0] == '\0' || !check_ip_format(ifip))
			{
				tcdbg_printf("%s:%d No Valid Interface IP\n",__FUNCTION__,__LINE__);
				return FAIL;
			}
			
			snprintf(cmd,sizeof(cmd),"/userfs/bin/stun-client %s -v -p %s -i %s -min %s -max %s -sp %s &",
				serverAddr,port,ifip,min,max,serverPort);
			system(cmd);
		}
		else
		{
			system("/usr/bin/killall -9 stun-client");
		}
	}
	return SUCCESS;
}

int svc_wan_related_stun_boot()
{
	return svc_wan_related_stun_update();
}
#endif

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CMCCV2)
static int rule_flag = 0;

int clear_ipv6_firewall_rule( void )
{
	char ipv6_cmd[128] = {0};

	if( rule_flag )
	{
		snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -D FORWARD -j FIREWALL_CHAIN");
		system(ipv6_cmd);

		memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
		snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -F FIREWALL_CHAIN");
		system(ipv6_cmd);
		
		memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
		snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -X FIREWALL_CHAIN");
		system(ipv6_cmd);

		memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
		snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -P FORWARD ACCEPT");
		system(ipv6_cmd);
		
		rule_flag = 0;
	}
	
	return 0;
}
int set_ipv6_firewall_rule( char* level )
{
	char ipv6_cmd[128] = {0};

	if ( 0 == strcmp(level, "high") )
	{
		if(0 == rule_flag)
		{			
			memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
			snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -P FORWARD DROP");
			system(ipv6_cmd);
			
			memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
			snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -t filter -N FIREWALL_CHAIN");
			system(ipv6_cmd);
			
			memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
			snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -A FIREWALL_CHAIN -i br0 -m state --state NEW,RELATED,ESTABLISHED -j ACCEPT");
			system(ipv6_cmd);
			
			memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
			snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -A FIREWALL_CHAIN -o br0 -m state --state RELATED,ESTABLISHED -j ACCEPT");
			system(ipv6_cmd);
			
			memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
			snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/usr/bin/ip6tables -t filter -A FORWARD -j FIREWALL_CHAIN");
			system(ipv6_cmd);

			rule_flag = 1;
		}
	}
	else
	{		
		clear_ipv6_firewall_rule();
	}
	return 0;
}
#endif

#if defined(TCSUPPORT_IPV6_FIREWALL)
int clearIpv6FirewallChain(char* chainName)
{
	char cmd[128] = {0};

	if(chainName == NULL)
	{
		return -1;
	}
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -F %s",chainName);
	system_escape(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -X %s",chainName);
	system_escape(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -D INPUT -j %s",chainName);
	system_escape(cmd);

	return 0;
}

int createIpv6FirewallChain(char* chainName)
{
	char cmd[128] = {0};

	if(chainName == NULL || chainName[0] == '\0')
	{
		return -1;
	}

	clearIpv6FirewallChain(chainName);
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter -N %s",chainName);
	system_escape(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip6tables -t filter  -A INPUT -j %s\n", chainName);
	system_escape(cmd);

	return 0;
}

int  openIPV6FirewallByFwLevel(char* level)
{
	if(level == NULL || level[0] == '\0')
	{
		return -1;
	}
	
	createIpv6FirewallChain(FW6_CHAIN_NAME);
	createIpv6FirewallChain(FW6_SPI_NAME);
	
	system_escape(FIREWALL_IPV6SPI_ON);
	
	if(strcmp(level, "high") == 0)
	{
		system_escape(FIREWALL_IPV6_ON_HIGH);
	}
	else if(strcmp(level, "medium") == 0)
	{
		system_escape(FIREWALL_IPV6_ON_MEDIUM);
	}
	else if(strcmp(level, "low") == 0)
	{
		system_escape(FIREWALL_IPV6_ON_LOW);
	}

	return 0;
}

void closeIPV6Firewall(void)
{
	system_escape(FIREWALL_IPV6_OFF);
	system_escape(FIREWALL_IPV6SPI_OFF);
	clearIpv6FirewallChain(FW6_CHAIN_NAME);
	clearIpv6FirewallChain(FW6_SPI_NAME);
}
#endif

#if defined(TCSUPPORT_CMCCV2)
int svc_wan_related_fw_ipv6session_update(void)
{
	char enable[8] = {0};

	cfg_get_object_attr(FIREWALL_IPV6SESSION_NODE, "Enable", enable, sizeof(enable));
	if( !strcmp(enable, "Yes") )
	{
		set_ipv6_firewall_rule("high");
	}
	else
	{
		clear_ipv6_firewall_rule();
	}
	return SUCCESS;
}

int svc_wan_related_fw_portscan_update(void)
{
	char enable[8] = {0};
	char portscan_addr[48] = {0};
	static int portscan_flag = 0;
	
#define PORTSCAN_ADDR_FILE "/tmp/portscan_addr"
	
	unlink(PORTSCAN_ADDR_FILE);
	/* save portscan source address to tmp file */
	memset(portscan_addr, 0, sizeof(portscan_addr));
	if ( cfg_get_object_attr(FIREWALL_ENTRY_NODE, "portscan_addr", portscan_addr, sizeof(portscan_addr)) > 0 )
	{
		if ( -1 == write2file(portscan_addr, PORTSCAN_ADDR_FILE) )
		{
			tcdbg_printf("fail to write file %s\n", PORTSCAN_ADDR_FILE);
		}
	}
	cfg_get_object_attr(FIREWALL_PORTSCAN_NODE, "Enable", enable, sizeof(enable));
	if( !strcmp(enable, "Yes") )
	{
		if( portscan_flag == 0 )
		{
			portscan_flag = 1;
			system(FIREWALL_PORTSCAN_ON);
		}
	}
	else
	{
		if( portscan_flag )
		{
			portscan_flag = 0;
			system(FIREWALL_PORTSCAN_OFF);
		}
	}
	return SUCCESS;
}
	
int svc_wan_related_fw_dos_update(void)
{
	char enable[8] = {0};
	static int dos_flag = 0;

	cfg_get_object_attr(FIREWALL_DOSENTRY_NODE, "Enable", enable, sizeof(enable));
	if( !strcmp(enable, "Yes") )
	{
		if( dos_flag == 0 )
		{
			system(FIREWALL_DOS_ON);
			dos_flag = 1;
		}
	}
	else
	{
		if( dos_flag )
		{
			dos_flag = 0;
			system(FIREWALL_DOS_OFF);
		}
	}
	return SUCCESS;
}

int svc_wan_related_attack_update()
{
	svc_wan_related_fw_dos_update();
	svc_wan_related_fw_portscan_update();
	svc_wan_related_fw_ipv6session_update();
	return 0;
}

#endif

int svc_wan_related_firewall_update(void)
{
	char firewall_status[2]={0};
#if defined(TCSUPPORT_CT_NEWGUI)
	char DosAttackProtect_status[10]={0};
#endif
	char firewall_level[10] = {0};
	char firewallcmd[40] = {0};
	char nodeName[64] = {0};
	/*char ipv6_cmd[256] = {0};*/

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), FIREWALL_ENTRY_NODE);
	if(cfg_get_object_attr(nodeName, "firewall_level", firewall_level, sizeof(firewall_level)) < 0)
	{	
		strncpy(firewall_level, "medium", sizeof(firewall_level) - 1);
	}
	
	if(cfg_get_object_attr(nodeName, "firewall_status", firewall_status, sizeof(firewall_status)) < 0)
	{	
		return FAIL;
	}
	
#if defined(TCSUPPORT_CT_NEWGUI)
	if(cfg_get_object_attr(nodeName, "DosAttackProtect_status", DosAttackProtect_status, sizeof(DosAttackProtect_status)) < 0)
	{	
		return FAIL;
	}
#endif

	if(!strcmp(firewall_status,"1"))/*firewall active*/
	{
#if 0 /* move it  to function check_and_set_filter(). */
		memset(ipv6_cmd, 0, sizeof(ipv6_cmd));
		snprintf(ipv6_cmd, sizeof(ipv6_cmd), "/sbin/insmod /lib/modules/3.18.21/kernel/net/ipv6/netfilter/ip6table_filter.ko");
		system_escape(ipv6_cmd);
#endif
		
#if defined(TCSUPPORT_CT_JOYME2)
		set_ipv6_firewall_rule(firewall_level);
#endif
#if defined(TCSUPPORT_IPV6_FIREWALL)
		openIPV6FirewallByFwLevel(firewall_level);
#endif

		system(FIREWALL_OFF);
		snprintf(firewallcmd, sizeof(firewallcmd), FIREWALL_LEVELCMD, firewall_level);
		system(FIREWALL_ON);
		system_escape(firewallcmd);
#if defined(TCSUPPORT_CT_NEWGUI)	
		system(FIREWALL_OFF);
		snprintf(firewallcmd, sizeof(firewallcmd), FIREWALL_DoSProtectCMD, DosAttackProtect_status);
		system(FIREWALL_ON);
		system_escape(firewallcmd);
#endif
#if defined(TCSUPPORT_CFG_NG_UNION) && !defined(TCSUPPORT_CMCC)
		doValPut("/proc/tc3162/attackprotector", "S 1");
#endif
		openlog("TCSysLog alarm", 0, LOG_LOCAL2);
		syslog(LOG_ALERT, "FireWall ON\n");
		closelog();
	}
	else
	{	
#if defined(TCSUPPORT_CT_JOYME2)
		clear_ipv6_firewall_rule();
#endif
#if defined(TCSUPPORT_IPV6_FIREWALL)
		closeIPV6Firewall();
#endif

#if defined(TCSUPPORT_CFG_NG_UNION) && !defined(TCSUPPORT_CMCC)
		doValPut("/proc/tc3162/attackprotector", "S 0");
#endif
		system(FIREWALL_OFF);
		openlog("TCSysLog alarm", 0, LOG_LOCAL2);
		syslog(LOG_ALERT, "FireWall OFF\n");
		closelog();
	}
	return SUCCESS;
}

int svc_wan_related_firewall_boot(void)
{
	svc_wan_related_firewall_update();
#if defined(TCSUPPORT_CMCCV2)
	system("iptables -t filter -N INPUT_PORTSCAN 2>/dev/null");
	system("iptables -t filter -A INPUT -j INPUT_PORTSCAN 2>/dev/null");	//at the last ,because do drop at the end of this chain

	system("iptables -t filter -N SYNFLOOD 2>/dev/null");
	system("iptables -t filter -A INPUT -j SYNFLOOD 2>/dev/null");
	system("iptables -t filter -A FORWARD -j SYNFLOOD 2>/dev/null");
	svc_wan_related_attack_update();
#endif

	return SUCCESS;
}

int svc_wan_related_acl_update()
		{
	int ret = -1;
	if((ret = svc_wan_related_acl_write()) != SUCCESS){
		return ret;
		}
	if((ret = svc_wan_related_acl_execute()) != SUCCESS){
		return ret;
		}
		
	return SUCCESS;
}

void splitName(char *name, char buf[][32], const char* delim)
{
	char *pValue=NULL;
	char tmp[32]={0};
	int i=0;

	strncpy(tmp, name, sizeof(tmp) - 1);
	/*Get parent node name*/
	strtok(tmp,delim);
	strncpy(buf[0], tmp, 31);
	for(i=1; i < 4; i++)
	{
		pValue=strtok(NULL, delim);
		if(pValue)
		{
			strncpy(buf[i], pValue, 31);
		}
		else
		{
			break;
		}
	}
}





int svc_wan_handle_event_appfilter_update(wan_related_cfg_update_t* param)
{
	unsigned int new_filter_state=0;
	char appFilterState[5]={0};
	char nodePath[64] = {0};
	char newFilterState[32] = {0};

	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	svc_wan_related_appfilter_write();

	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;
	/*get the Activate value and see if it is ok*/
	if(cfg_get_object_attr(param->path, "Activate", appFilterState, sizeof(appFilterState)) < 0)
	{
		printf("Error--AppFilter get some errors when get activate value.\n");
	}
	/*if Activated, check filter state and run AppFilter.sh*/
	else if (!strcmp(appFilterState,"1"))
	{
		if(app_check_filter())/*filter on*/
		{
			new_filter_state=atoi(newFilterState) | APP_VECTOR;
		}
		else/*filter down*/
		{
			new_filter_state= atoi(newFilterState) & (~APP_VECTOR);
		}
		check_and_set_l7filter(new_filter_state);
		
		check_and_set_filter( new_filter_state );
		
		system(APP_FILTER_SH);			
	}
	/*we will stop app filter if the Activate value is not 1*/
	else
	{
		system(APP_STOP_SH);
		if(app_check_filter())/*filter on*/
		{
			new_filter_state=atoi(newFilterState) | APP_VECTOR;
		}
		else/*filter down*/
		{
			new_filter_state= atoi(newFilterState) & (~APP_VECTOR);
		}
		check_and_set_l7filter(new_filter_state);
		
		check_and_set_filter( new_filter_state );
	}
	return SUCCESS;
}

int url_check_filter(void)
{
	int filter_on=0;
	char nodeName[64] = {0};
	char urlfilter_status[5]={0};
	/*check url filter state*/

	/*flush the buffer*/
	memset(nodeName,0,sizeof(nodeName));

	/*let the node name = UrlFilter_Common*/
	strncpy(nodeName, URLFILTER_COMMON_NODE, sizeof(nodeName)-1);
	/*check the UrlFilter_Common Activate value*/
	if(cfg_get_object_attr(nodeName, "Activate", urlfilter_status, sizeof(urlfilter_status)) < 0)
	{
		printf("nodename %s Activate urlfilter_status %s\n", nodeName, urlfilter_status);
	}
	/*if UrlFilter_Common Activate value=1 then do follows*/
	else if(!strcmp(urlfilter_status,"1"))
	{
		/*flush the node name [1] field*/
		memset(nodeName,0,sizeof(nodeName));
		int urlIndex=0;
		/*check url rule activate one by one, if any one is activated, then load the filter module*/
		for(urlIndex=1;urlIndex<=MAX_URL_RULE; urlIndex++)
		{
			/*reset the state buffer*/
			strncpy(urlfilter_status, "", sizeof(urlfilter_status)-1);
			/*set node name[1]=Entry+number(0~15)*/
			snprintf(nodeName, sizeof(nodeName), URLFILTER_ENTRY_N_NODE, urlIndex);
			if(cfg_get_object_attr(nodeName, "Activate", urlfilter_status, sizeof(urlfilter_status)) < 0)
			{
				continue;
			}
			/*find a rule is activated, load the l7 filter module and break for loop*/
			else if(!strcmp(urlfilter_status,"1"))
			{
				filter_on=1;
				break;
			}
			else
			{
				/*nothing to do*/
			}
		}
	}
	return filter_on;
}

int svc_wan_handle_event_urlfilter_update(wan_related_cfg_update_t* param)
{
	unsigned int new_filter_state=0;
	char urlFilterState[5]={0};
	char nodeName[64] = {0};
#if defined(TCSUPPORT_CT_E8GUI)
	char list_type[5] = {0};
#endif
	char nodePath[64] = {0};
	char newFilterState[32] = {0};
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
	int entry_idx = 0;

	if( get_entry_number_cfg2(param->path, "entry.", &entry_idx) != 0)
		entry_idx = 0;

	initurlfilterduration(entry_idx);
	svc_wan_related_urlfilter_write(entry_idx);
#else	
	svc_wan_related_urlfilter_write();
#endif	
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, URLFILTER_COMMON_NODE, sizeof(nodeName)-1);

	if(cfg_get_object_attr(nodeName, "Activate", urlFilterState, sizeof(urlFilterState)) < 0)
	{
		printf("Error--UrlFilter get some errors when get activate value.\n");
	}
	else if (!strcmp(urlFilterState,"1")){
		if(url_check_filter())/*filter on*/
		{
			new_filter_state=atoi(newFilterState) | URL_VECTOR;
		}
		else/*filter down*/
		{
			new_filter_state=atoi(newFilterState) & (~URL_VECTOR);
		}
		check_and_set_l7filter(new_filter_state);
		check_and_set_filter(new_filter_state);
		system(URL_FILTER_SH);
	}
	else{		
#if defined(TCSUPPORT_CT_E8GUI)
		char  urlFilterpolicy[5] = {0};
		cfg_get_object_attr(nodeName, "Filter_Policy", urlFilterpolicy, sizeof(urlFilterpolicy));
		if(urlFilterpolicy[0] != '\0')
		{
			if(!strcmp(urlFilterpolicy, "1"))
			{
				url_filter_deletewhiterules(1);
			}
			else
			{
				system(URL_STOP_SH);
			}
		}
#else
		system(URL_STOP_SH);
#endif	
		if(url_check_filter())/*filter on*/
		{
			new_filter_state=atoi(newFilterState) | URL_VECTOR;
		}
		else/*filter down*/
		{
			new_filter_state=atoi(newFilterState) & (~URL_VECTOR);
		}
		check_and_set_l7filter(new_filter_state);
		check_and_set_filter(new_filter_state);
	}
#if defined(TCSUPPORT_CT_E8GUI)
	cfg_get_object_attr(nodeName, "Filter_Policy", list_type, sizeof(list_type));
	if(strcmp(urlFilterState,"1") || !strcmp(list_type, "1")) /*url_filter off or white list active*/
	{
		printf("\nurl_filter_execute url_filter_clear");
		system(URL_FILTER_BL_CLEAR);
	}
#endif

	cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1") ;

	return SUCCESS;
}

char pppoeEmudns[2][64] = {"",""};

int addEMUDNSConfig(FILE* fp, FILE* fpInfo)
{
	char  buf[256] = {0};

	if ( !fp || !fpInfo )
		return -1;

	if ( 0 != pppoeEmudns[0][0] )
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "server=%s@%s\n", pppoeEmudns[0], PPPOE_EMULATOR_PPPNAME); 		
		fwrite(buf, sizeof(char), strlen(buf), fp);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%s %s %s\n", pppoeEmudns[0], PPPOE_EMULATOR_PPPNAME, "PPPEMU");
		fwrite(buf, sizeof(char), strlen(buf), fpInfo);
	}

	if ( 0 != pppoeEmudns[1][0] )
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "server=%s@%s\n", pppoeEmudns[1], PPPOE_EMULATOR_PPPNAME); 		
		fwrite(buf, sizeof(char), strlen(buf), fp);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%s %s %s\n", pppoeEmudns[1], PPPOE_EMULATOR_PPPNAME, "PPPEMU");
		fwrite(buf, sizeof(char), strlen(buf), fpInfo);

	}

	return 0;
}

#if defined(TCSUPPORT_CT_FULL_ROUTE)
unsigned int get_default_pvc_DNSOrder(void){

	char nodeName[64] = {0};
	char default_routeV4[16]={0};
	char default_routeV6[16]={0};
	unsigned int defaut_route_id = 0xFFFFFFFF;
	int tempId = -1;

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, WANINFO_COMMON_NODE, sizeof(nodeName)-1);
	if(cfg_get_object_attr(nodeName, "DefRouteIndexv4", default_routeV4, sizeof(default_routeV4)) < 0)
	{
		goto checkv6;
	}
	if(strlen(default_routeV4) == 0 || !strncmp(default_routeV4, "N/A", 3)) 
	{
		goto checkv6;
	}
	defaut_route_id &= ~0xFFFF;
	defaut_route_id |= atoi(default_routeV4);

checkv6:
	if(cfg_get_object_attr(nodeName, "DefRouteIndexv6", default_routeV6, sizeof(default_routeV6)) < 0)
	{
		goto out;
	}
	if(strlen(default_routeV6) == 0 || !strncmp(default_routeV6, "N/A", 3)) 
	{
		goto out;
	}

	tempId = atoi(default_routeV6);
	defaut_route_id &= ~0xFFFF0000;
	defaut_route_id |= tempId << 16;
	
out:	
	return defaut_route_id;
}
#endif

int isOndemandWan(int pvc_index, int entry_index)
{
	char nodeName[64] = {0};
	char connection[32] = {0};
	char ispMode[12] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);

	if(cfg_get_object_attr(nodeName, "ISP", ispMode, sizeof(ispMode)) < 0
		|| 0 != strcmp(ispMode, "2") )
		return 0;
	if(cfg_get_object_attr(nodeName, "CONNECTION", connection, sizeof(connection)) < 0
		|| 0 != strcmp(connection, "Connect_on_Demand") )
		return 0;

	return 1;
}

void wan_cmd_excute(char *cmd)
{
	if (NULL == cmd) {
		printf("==>wan_cmd_excute(): cmd is null.\n");
		return;
	}

	if ('\0' == *cmd) {
		return;
	}
	system_escape(cmd);	

	return;
}

int addDnsRoute6forOtherWan(char *ifname, char *ifsvrapp, char *hisaddrV6)
{
	char cmdbuf[128] = {0};

	if ( NULL == ifname || NULL == ifsvrapp || NULL == hisaddrV6 )
		return -1;

	if ( NULL == strstr(ifsvrapp, "INTERNET") )
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/ip -6 route add %s dev %s", hisaddrV6, ifname);
		wan_cmd_excute(cmdbuf);
	}

	return 0;
}

void svc_wan_handle_event_dnsmask_restart(void)
{
	FILE* fp = NULL;
	FILE* fpInfo = NULL;
	char buf[160] = {0};
	char nodeName[64] = {0};
	char dns[20] = {0}, secdns[20] = {0}, ifname[16] = {0}, status[6] = {0}, serviceapp[64] = {0}, isp[3] = {0};
	#ifdef IPV6
	char dns6[40] = {0}, secdns6[40] = {0}, status6[6] = {0};
	#endif
	int i;
#if defined(TCSUPPORT_CT_FULL_ROUTE)
	unsigned int default_pvc = 0;
	unsigned short tempV4 = 0;
	unsigned short tempV6 = 0;
	int flag = 0;
	int fullroutesw = 0;
#endif
#if defined(TCSUPPORT_CT_PPP_ONDEMAND) || defined(TCSUPPORT_CT_ADV_DNSPROXY)
	char ppphisaddr[64] = {0};
	char ip_version[20] = {0};
	int ipVersion = 0;
#endif
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
	char query_delay[64] = {0};
#endif

	/* step1: down dnsmasq process */
	system("killall -9 dnsmasq");

	/* step2: write dnsmasq.conf and dnsInfo.conf */
	/* dnsmasq.conf format is:
	** strict-order
	** no-resolv
	** 168.95.1.1@nas0_1
	** 168.95.1.2@ppp1
	** 2001::fe@nas0_1 //for IPv6

	** dnsInfo.conf format is:
	** 168.95.1.1 nas0 Internet
	** 168.95.1.2 ppp1 TR69
	
	** get wan information from WanInfo node
	*/
	fp = fopen("/etc/dnsmasq.conf", "w");
	if(fp == NULL){
		printf("\n %s:open dnsmasq.conf failed", __func__);
		return;
	}

	fpInfo = fopen("/etc/dnsInfo.conf", "w");
	if(fpInfo == NULL){
		fclose(fp);
		printf("\n %s: open dnsInfo.conf failed", __func__);
		return;
	}
	
	sprintf(buf, "no-resolv\n");
	fwrite(buf, sizeof(char), strlen(buf), fp);
#if defined(TCSUPPORT_CT_FULL_ROUTE)
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, WAN_COMMON_NODE, sizeof(nodeName)-1);

	if(cfg_get_object_attr(nodeName, "IPForwardModeEnable", status, sizeof(status)) < 0)
	{		
		fclose(fp);
		fclose(fpInfo);
		printf("\n %s:%d: get IPForwardModeEnable  failed", __func__,__LINE__);
		return;
	}

	if(0 == strncmp(status, "Yes", 3))
	{
		fullroutesw = 1;
	}
	
	if(fullroutesw == 1)
	{
		default_pvc = get_default_pvc_DNSOrder();
		tempV4 = (unsigned short)default_pvc;
		tempV6 = (unsigned short)(default_pvc >> 16);
	}
#endif
	
	for(i = 0; i < MAX_WAN_IF_INDEX; i++){
#if defined(TCSUPPORT_CT_FULL_ROUTE)
		if(fullroutesw == 1)
		{
			if(flag == 0)/*dns v4*/
			{
				if(tempV4 !=0xFFFF)
				{
					i = tempV4;
				}
				else
					flag = 1;/*v4 pass*/
				if(tempV4 == tempV6)
					flag = 1;
			}

			if(flag == 1){/*dns v6*/
				if(tempV6 !=0xFFFF)
				{
					i = tempV6;
				}
				else
				{
					flag = 2;/*v6 pass*/
					i = -1;
					continue;
				}
			}
		}
#endif
		memset(ifname, 0, sizeof(ifname));
		memset(serviceapp, 0, sizeof(serviceapp));
		/*Get interface name and check wan interface exited*/
		if(get_waninfo_by_index(i, "IFName", ifname, sizeof(ifname)) != SUCCESS ||
			get_waninfo_by_index(i, "ISP", isp, sizeof(isp)) != SUCCESS)
			continue;

		/*skip bridge mode interface*/
		if(!strcmp(isp, "3"))
		{
				continue;
		}
		
		if(get_waninfo_by_index(i, "ServiceList", serviceapp, sizeof(serviceapp)) != SUCCESS)
		{
			/*Use others as default service type*/
			strncpy(serviceapp, "OTHER", sizeof(serviceapp)-1);
		}
		
#if defined(TCSUPPORT_CT_PPP_ONDEMAND) || defined(TCSUPPORT_CT_ADV_DNSPROXY)
		if( SUCCESS == get_waninfo_by_index(i, "IPVERSION", ip_version, sizeof(ip_version)) )
		{
			if ( 0 == strcmp(ip_version, "IPv4") )
				ipVersion = 0;
			else if ( 0 == strcmp(ip_version, "IPv6") )
				ipVersion = 1;
			else if ( 0 == strcmp(ip_version, "IPv4/IPv6") )
				ipVersion = 2;
		}
#endif
		
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, i);
#if defined(TCSUPPORT_CT_FULL_ROUTE)
		if(fullroutesw == 1){
			if(flag == 2 && i == tempV4)/*next time,if it's default DNS no need to write again*/
			{
				goto v6;
			}

			if(flag == 1 && tempV4 != tempV6)/*v6 dns set , if v4==v6, we need write v4&v6 dns to file one time.*/
				goto v6;
		}
#endif
		cfg_get_object_attr(nodeName, "Status", status, sizeof(status));
		if(status[0] != '\0' && !strcmp(status, "up"))
		{
			memset(dns, 0, sizeof(dns));
			if(cfg_get_object_attr(nodeName, "DNS", dns, sizeof(dns)) < 0 && strlen(dns) != 0 && isValidDnsIp(dns))
			{
				memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
				snprintf(buf, sizeof(buf), "server=%s@%s%s\n", dns, ifname, ((2 == ipVersion) ? "$ds" : ""));
#else
				snprintf(buf, sizeof(buf), "server=%s@%s\n", dns, ifname);			
#endif
				fwrite(buf, sizeof(char), strlen(buf), fp);

				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "%s %s %s\n", dns, ifname, serviceapp);
				fwrite(buf, sizeof(char), strlen(buf), fpInfo);
			}
			memset(secdns, 0, sizeof(secdns));
			cfg_get_object_attr(nodeName, "SecDNS", secdns, sizeof(secdns));
			if(secdns[0] != '\0' && strlen(secdns) != 0 && isValidDnsIp(secdns))
			{
				memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
				snprintf(buf, sizeof(buf), "server=%s@%s%s\n", secdns, ifname, ((2 == ipVersion) ? "$ds" : ""));
#else
				snprintf(buf, sizeof(buf), "server=%s@%s\n", secdns, ifname);			
#endif
				fwrite(buf, sizeof(char), strlen(buf), fp);

				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "%s %s %s\n", secdns, ifname, serviceapp);
				fwrite(buf, sizeof(char), strlen(buf), fpInfo);
			}
		}
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
		else if ( 1 == isOndemandWan(i/MAX_SMUX_NUM, i%MAX_SMUX_NUM) )
		{
			cfg_get_object_attr(nodeName, "OndemandHisAddr", ppphisaddr, sizeof(ppphisaddr));
			if('\0' != ppphisaddr[0])
			{
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "server=%s@%s\n", ppphisaddr, ifname);			
				fwrite(buf, sizeof(char), strlen(buf), fp);

				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "%s %s %s\n", ppphisaddr, ifname, serviceapp);
				fwrite(buf, sizeof(char), strlen(buf), fpInfo);
			}
		}
#endif
		
#if defined(TCSUPPORT_CT_FULL_ROUTE)
	v6:
	if(fullroutesw == 1)
	{
		if(flag == 0)/*v4 dns set finish*/
			goto next;
		if(flag == 2 && i == tempV6)/*next time,if it's default DNS no need to write again*/
			goto next;
	}
#endif
		#ifdef IPV6
		cfg_get_object_attr(nodeName, "Status6", status6, sizeof(status6));
		if(status6[0] != '\0' && !strcmp(status6, "up")){
			memset(dns6, 0, sizeof(dns6));
			cfg_get_object_attr(nodeName, "DNS6", dns6, sizeof(dns6));
			if(dns6[0] != '\0' && strlen(dns6) != 0){
				memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
				snprintf(buf, sizeof(buf), "server=%s@%s%s\n", dns6, ifname, ((2 == ipVersion) ? "$ds" : ""));
#else
				snprintf(buf, sizeof(buf), "server=%s@%s\n", dns6, ifname);			
#endif
				fwrite(buf, sizeof(char), strlen(buf), fp);

				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "%s %s %s\n", dns6, ifname, serviceapp);
				fwrite(buf, sizeof(char), strlen(buf), fpInfo);
			}
			memset(secdns6, 0, sizeof(secdns6));
			cfg_get_object_attr(nodeName, "SecDNS6", secdns6, sizeof(secdns6));
			if(secdns6[0] != '\0' && strlen(secdns6) != 0){
				memset(buf, 0, sizeof(buf));
#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
				snprintf(buf, sizeof(buf), "server=%s@%s%s\n", secdns6, ifname, ((2 == ipVersion) ? "$ds" : ""));
#else
				snprintf(buf, sizeof(buf), "server=%s@%s\n", secdns6, ifname);			
#endif
				fwrite(buf, sizeof(char), strlen(buf), fp);

				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "%s %s %s\n", secdns6, ifname, serviceapp);
				fwrite(buf, sizeof(char), strlen(buf), fpInfo);
			}
		}
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
		else if ( ( 1 == ipVersion || 2 == ipVersion )
				&& 1 == isOndemandWan(i/MAX_SMUX_NUM, i%MAX_SMUX_NUM) )
		{
			snprintf(ppphisaddr, sizeof(ppphisaddr), "2014:1211::%d", i+6311);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "server=%s@%s\n", ppphisaddr, ifname); 		
			fwrite(buf, sizeof(char), strlen(buf), fp);

			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "%s %s %s\n", ppphisaddr, ifname, serviceapp);
			fwrite(buf, sizeof(char), strlen(buf), fpInfo);

			addDnsRoute6forOtherWan(ifname, serviceapp, ppphisaddr);
		}
#endif

		#endif
#if defined(TCSUPPORT_CT_FULL_ROUTE)
	next:
	if(fullroutesw == 1)
	{
			if(flag != 2)/*set default DNS first*/
			{
				if(++flag == 2)
					i = -1;
			}
	}
#endif
	}

#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
	addEMUDNSConfig(fp, fpInfo);
#endif

#if defined(TCSUPPORT_CT_ADV_DNSPROXY)
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, WAN_COMMON_NODE, sizeof(nodeName)-1);
	cfg_get_object_attr(nodeName, "DNSForwardDelay", query_delay, sizeof(query_delay));
	if( '\0' != query_delay[0] )
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "query-delay=%s\n", query_delay); 		
		fwrite(buf, sizeof(char), strlen(buf), fp);
	}
#endif

	fclose(fp);
	fclose(fpInfo);

	/* step3: start dnsmasq process */
	system("/userfs/bin/dnsmasq &");
	sleep(1);
	return;
}

int ipfilter_check_filter(void)
{
	int filter_on=0;
	filter_on = ipfilter_check_active();
	return filter_on;
}

int svc_wan_handle_event_ipmacfilter_update(wan_related_cfg_update_t* param)
{
	unsigned int new_filter_state=0;
	char nodePath[64] = {0};
	char newFilterState[32] = {0};

	memset(nodePath, 0, sizeof(nodePath));
	memset(newFilterState, 0, sizeof(newFilterState));
	/*check if iptable_filter module is loaded or not*/
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;
	if(ipfilter_check_filter())
	{
		/*filter on*/
		new_filter_state=atoi(newFilterState) | IPFILTER_VECTOR;
	}
	else
	{
		/*filter down*/
		new_filter_state=atoi(newFilterState) & (~IPFILTER_VECTOR);
	}

	check_and_set_filter(new_filter_state);
	
	/*set iptable settings*/		
	system(MACFILTER_SH);
	system(IPUPFILTER_SH);
	system(IPDOWNFILTER_SH);
	return SUCCESS;
}

int svc_wan_related_igmpproxy_update(void)
{
	int ret = -1;
	if((ret = svc_wan_related_igmpproxy_write()) != SUCCESS){
		return ret;
	}
	if((ret = svc_wan_related_igmpproxy_execute()) != SUCCESS){
		return ret;
	}

	return SUCCESS;
}

int svc_wan_related_mldproxy_update(void)
{
	int ret = -1;
	if((ret = svc_wan_related_mldproxy_execute()) != SUCCESS){
		return ret;
	}

	return SUCCESS;
}

static int svc_wan_related_ddns_boot(void)
{
	int ret = -1;
	if((ret = svc_wan_related_ddns_write()) != SUCCESS){
		return ret;
	}
	if((ret = svc_wan_related_ddns_execute()) != SUCCESS){
		return ret;
	}

	return SUCCESS;
}

static int svc_wan_related_ddns_update(void)
{
	int ret = -1;
	if((ret = svc_wan_related_ddns_write()) != SUCCESS){
		return ret;
	}
	if((ret = svc_wan_related_ddns_execute()) != SUCCESS){
		return ret;
	}

	return SUCCESS;
}

static int svc_wan_related_attach_update()
{
	system("/usr/bin/killall -9 dnsmasq");
	system("/userfs/bin/dnsmasq &");
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
int svc_wan_related_incomingfilter_update(void)
{
	char policy[2] = {0}, remote_ip[48] = {0}, protocol[2] = {0};
	char port[6] = {0}, itf_type[2] = {0};
	FILE *fp = NULL;
	char cmd_buf[256] = {0};
	char if_name[3][8] = {{0}, {0}, {0}}; /* [nas+/ppp+/br+] */
	char port_ip_detail[14 + 19 + 1]= {0}; /* [ --dport 65535][ -s 192.168.100.100] */
	char policy_detail[12 + 1] = {0}; /* [ -j ACCEPT\n] */
	char nodeName[64] = {0};
	int i = 0, j = 0;
	
	/* delete previous file(setting) */
	unlink(INCOMINGFILTER_SH);

	/*open incomingfilter.sh to write*/
	fp = fopen(INCOMINGFILTER_SH, "w+");

	if ( NULL == fp ) 
		return FAIL;
	
	/* flush previous settings of incomingfilter */
	fputs("iptables -F INCOMINGFILTER\n", fp);
	fputs("ip6tables -F INCOMINGFILTER\n", fp);
	/* set counters of incomingfilter as zero */
	fputs("iptables -Z INCOMINGFILTER\n", fp);
	fputs("ip6tables -Z INCOMINGFILTER\n", fp);

	for ( i = 0; i < MAX_INCOMFILTERRULES_NUM; i++ )
	{
		bzero(cmd_buf, sizeof(cmd_buf));
		bzero(nodeName, sizeof(nodeName));
		bzero(remote_ip, sizeof(remote_ip));
		bzero(policy, sizeof(policy));
		bzero(protocol, sizeof(protocol));
		bzero(port, sizeof(port));
		bzero(itf_type, sizeof(itf_type));

		snprintf(nodeName, sizeof(nodeName), INCOMINGFILTER_ENTRY_NODE, i + 1);
		if ( cfg_get_object_attr(nodeName, "remoteIP", remote_ip, sizeof(remote_ip)) < 0 )
			continue;

		if ( cfg_get_object_attr(nodeName, "policy", policy, sizeof(policy)) < 0 
			|| 0 == policy[0])
			continue;

		if ( cfg_get_object_attr(nodeName, "protocol", protocol, sizeof(protocol)) < 0 
			|| 0 == protocol[0])
			continue;

		if ( cfg_get_object_attr(nodeName, "port", port, sizeof(port)) < 0 
			|| 0 == port[0])
			continue;

		if ( cfg_get_object_attr(nodeName, "interface", itf_type, sizeof(itf_type)) < 0 
			|| 0 == itf_type[0])
			continue;
		
		if (strcmp(remote_ip, "")) /* "" means all ip */
			snprintf(port_ip_detail, sizeof(port_ip_detail), " --dport %s -s %s", port, remote_ip);
		else
			snprintf(port_ip_detail, sizeof(port_ip_detail), " --dport %s", port);

		if (POLICY_ACCEPT == atoi(policy))
			strncpy(policy_detail, " -j ACCEPT\n", sizeof(policy_detail) - 1);
		else
			strncpy(policy_detail, " -j DROP\n", sizeof(policy_detail) - 1);

		bzero(if_name, sizeof(if_name));
		if (0 == strcmp(itf_type, "0")) /* WAN */
		{
			strncpy(if_name[0], "nas+", sizeof(if_name[0]) - 1);
			strncpy(if_name[1], "ppp+", sizeof(if_name[1]) - 1);
		}
		else /* LAN */
			strncpy(if_name[0], "br+", sizeof(if_name[0]) - 1);

		for (j = 0; if_name[j][0] != 0; j++)
		{
			if ( !strcmp(protocol, "0") ) /* TCP */
			{
				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "iptables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p TCP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);

				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "ip6tables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p TCP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);	
			}
			else if ( !strcmp(protocol, "1") ) /* UDP */
			{
				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "iptables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p UDP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);

				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "ip6tables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p UDP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);
			}
			else /* TCP&UDP */
			{
				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "iptables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p TCP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);

				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "ip6tables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p TCP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);

				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "iptables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p UDP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);

				memset(cmd_buf, 0, sizeof(cmd_buf));
				strcpy(cmd_buf, "ip6tables -t filter -A INCOMINGFILTER -i ");
				strcat(cmd_buf, if_name[j]);
				strcat(cmd_buf, " -p UDP");
				strcat(cmd_buf, port_ip_detail);
				strcat(cmd_buf, policy_detail);
				fputs(cmd_buf, fp);
			}
		}
	}

	fclose(fp);
	if(FAIL == chmod(INCOMINGFILTER_SH ,777))
		return FAIL;
	system(INCOMINGFILTER_SH);
	return SUCCESS;
}

#if !defined(TCSUPPORT_CT_JOYME4) && !defined(TCSUPPORT_CT_UBUS)
#define MAX_DNSFILTER_NUM	100
int svc_wan_releated_dnsfilter_update(void)
#else
int svc_wan_releated_dnsfilter_update(wan_related_cfg_update_t* param)
#endif
{
	char nodeName[128] = {0};
	char common_enable[4] = {0};
	char entry_enable[4] = {0};
	int i = 0;
#if !defined(TCSUPPORT_CT_JOYME4) && !defined(TCSUPPORT_CT_UBUS)
	char hostname_old[128] = {0};
	char hostname_new[128] = {0};
#else
	int entry_idx = 0;

	if( get_entry_number_cfg2(param->path, "entry.", &entry_idx) != 0)
		entry_idx = 0;

	initdnsfilterduration(entry_idx);
#if defined(TCSUPPORT_CT_UBUS)
	svc_wan_related_dnsfilter_write();
#endif
#endif

	cfg_get_object_attr(DNSFILTER_COMON_NODE, "Enable", common_enable, sizeof(common_enable));
	for (i = 0; i < MAX_DNSFILTER_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), DNSFILTER_ENTRY_N_NODE, i);
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
		if (cfg_get_object_attr(nodeName, "Enable", entry_enable, sizeof(entry_enable)) < 0
			|| !strcmp(entry_enable, "0") || '\0' == entry_enable[0]
			|| !strcmp(common_enable, "0") || '\0' == common_enable[0])
		{	
			cfg_set_object_attr(nodeName, "BlockedTimes", "0");
		}
#else
		if (cfg_get_object_attr(nodeName, "HostName_new", hostname_new, sizeof(hostname_new)) > 0)
		{
			if (cfg_get_object_attr(nodeName, "HostName_old", hostname_old, sizeof(hostname_old)) < 0 
				|| !strcmp(common_enable, "0") || '\0' == common_enable[0]
				|| cfg_get_object_attr(nodeName, "Enable", entry_enable, sizeof(entry_enable)) < 0
				|| !strcmp(entry_enable, "0") || '\0' == entry_enable[0]
				|| strcmp(hostname_new, hostname_old) != 0)
			{
				cfg_set_object_attr(nodeName, "BlockedTimes", "0");
			}
			cfg_set_object_attr(nodeName, "HostName_old", hostname_new);
		}
#endif
	}
	
	system("/usr/bin/killall -9 dnsmasq");
	system("/userfs/bin/dnsmasq &");
	
	return SUCCESS;
}
#endif

#if defined(TCSUPPORT_CT_VPN_PPTP) || defined(TCSUPPORT_CT_L2TP_VPN)
int svc_wan_releated_vpn_boot(void)
{
#if defined(TCSUPPORT_CT_JOYME2)
	vpnlist_l2tp_write(NULL, 0);
#if defined(TCSUPPORT_CT_VPN_PPTP)
	vpnlist_pptp_write_for_boot();
#endif
#endif

	vpnlist_boot_execute();

	return 0;
}

int svc_wan_releated_vpn_update(wan_related_cfg_update_t* param)
{
	char *p_idx = NULL;
	int idx = -1;
	char *path = NULL;
	
	path = param->path;
	
	if(NULL == path)
	{
		tcdbg_printf("The path is NULL.\n");
		return -1;
	}
	
	if (NULL == (p_idx = strstr(path, "entry")))
	{
		tcdbg_printf("The path is invalid, and the path should include entry.\n");	
		return -1;
	}
	
	p_idx += strlen("entry.");
	idx = atoi(p_idx) - 1;

	vpnlist_write(path, idx);
	vpnlist_execute(path, idx);
	
#if defined(TCSUPPORT_CT_JOYME2)
	svc_wan_related_attach_update();
#endif
	
	return 0;
}

int svc_wan_related_vpn_ip_domain_update(wan_related_cfg_update_t* param)
{
#if defined(TCSUPPORT_CT_JOYME2)
	int idx = -1;
	char *p_idx = NULL;
	char *path = NULL;

	path = param->path;

	if (NULL == (p_idx = strstr(path, "entry")))
	{
		tcdbg_printf("The path is invalid, and the path should include entry.\n");	
		return -1;
	}
	
	p_idx += strlen("entry.");
	idx = atoi(p_idx) - 1;
	
	blapi_system_vpn_free_ip_hashmap(idx);
#endif
	
	svc_wan_related_attach_update();

	return 0;
}

int svc_wan_related_vpn_mac_update(wan_related_cfg_update_t* param)
{
	system("/usr/bin/killall -9 dnsmasq");
	system("/userfs/bin/dnsmasq &");

	return 0;
}

#endif
int wan_related_virServer_execute(char* path)
{
	char nodeName[128] 		= {0};
	char server_node[128] 	= {0};
	char cur_id_virSer[4]	= {0};
	char buf[64]			= {0};
	int entryIndex 			= 0;
	char virServ_file[32]	= {0};
	char if_name[32] 		= {0};
	char virServ_Active[10] = {0};
#if !defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CUC)
	char* ptmp_path			= NULL;
	int pvcIndex 			= 0;
	char cur_pvc[4]			= {0};
#endif
	int ret = -1;
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
	char def_route_wan_idx[4] = {0}, nat_enable[10] = {0};
	char active_ifname[32] = {0};
#endif

#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
	/* VirServer_EntryX */
	snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
	if (get_entry_number_cfg2(path, "entry.", &entryIndex) != 0){
		if (cfg_get_object_attr(nodeName, "virServ_id", cur_id_virSer, sizeof(cur_id_virSer)) > 0){
			snprintf(server_node, sizeof(server_node), VIRSERVER_ENTRY_NODE, atoi(cur_id_virSer) + 1);
		}	
	}
	else {
		snprintf(cur_id_virSer, sizeof(cur_id_virSer), "%d", entryIndex - 1);
		snprintf(server_node, sizeof(server_node), VIRSERVER_ENTRY_NODE, entryIndex);
	}
#else
	/*VirServer_EntryX_EntryX*/
	snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
	if(get_entry_number_cfg2(path, "entry.", &pvcIndex) != 0){
		if(cfg_get_object_attr(nodeName, "nat_pvc", cur_pvc, sizeof(cur_pvc)) > 0){
			snprintf(server_node, sizeof(server_node), VIRSERVER_ENTRY_NODE, atoi(cur_pvc)+1);
		}	
	}
	else{
		snprintf(cur_pvc, sizeof(cur_pvc), "%d", pvcIndex - 1);
	}
	if(NULL != strstr(path, "entry.")){
		ptmp_path = strstr(path, "entry.") + strlen("entry.");
		if(get_entry_number_cfg2(ptmp_path, "entry.", &entryIndex) != 0){
			if(cfg_get_object_attr(nodeName, "virServ_id", cur_id_virSer, sizeof(cur_id_virSer)) > 0){
				snprintf(server_node, sizeof(server_node), VIRSERVER_ENTRY_ENTRY_NODE, 
											atoi(cur_pvc) + 1, atoi(cur_id_virSer) + 1);
			}
		}
		else{
			snprintf(cur_id_virSer, sizeof(cur_id_virSer), "%d", entryIndex - 1);
			snprintf(server_node, sizeof(server_node), VIRSERVER_ENTRY_ENTRY_NODE, 
											atoi(cur_pvc) + 1, atoi(cur_id_virSer) + 1);
		}
	}else{
		if(cfg_get_object_attr(nodeName, "virServ_id", cur_id_virSer, sizeof(cur_id_virSer)) > 0){
			snprintf(server_node, sizeof(server_node), VIRSERVER_ENTRY_ENTRY_NODE, 
										atoi(cur_pvc) + 1, atoi(cur_id_virSer) + 1);
		}
	}
#endif
	
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
	/*Get default route wan interface name*/
	memset(def_route_wan_idx, 0, sizeof(def_route_wan_idx));
	if (cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", def_route_wan_idx, sizeof(def_route_wan_idx)) < 0) {
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));
	memset(nat_enable, 0, sizeof(nat_enable));
	memset(if_name, 0, sizeof(if_name));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, atoi(def_route_wan_idx) / 8 + 1, atoi(def_route_wan_idx) % 8 + 1);
	if ( cfg_get_object_attr(nodeName, "NATENABLE", nat_enable, sizeof(nat_enable)) <= 0 
		|| 0 == strcasecmp(nat_enable, "Disabled") ) {
		return -1;
	}
	if(cfg_get_object_attr(nodeName, "IFName", if_name, sizeof(if_name)) <= 0){
		return -1;
	}
#else
	/*Get interface name*/
	memset(nodeName, 0, sizeof(nodeName));
#if defined(TCSUPPORT_CUC)
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, atoi(cur_pvc)/8 + 1, atoi(cur_pvc)%8 + 1);
#else
	snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, atoi(cur_pvc)+1);
#endif
	if(cfg_get_object_attr(nodeName, "IFName", if_name, sizeof(if_name)) <= 0){
		return -1;
	}
#endif

	memset(virServ_Active, 0, sizeof(virServ_Active));
	if(cfg_get_object_attr(server_node, "Active", virServ_Active, sizeof(virServ_Active)) <= 0){
		strncpy(virServ_Active, "No", sizeof(virServ_Active));
	}
	if(cfg_obj_query_object(server_node, NULL, NULL) > 0){
		/*Delete virserver rule firstly before adding new one*/
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
		if ( cfg_get_object_attr(server_node, "ActiveIFName", active_ifname, sizeof(active_ifname)) > 0 )
		{
			snprintf(buf, sizeof(buf), "%s del %s %s", VIRSERV_SH, active_ifname, cur_id_virSer);
			system_escape(buf);
		}
#else
		snprintf(buf, sizeof(buf), "%s del %s %s", VIRSERV_SH, if_name, cur_id_virSer);
		system_escape(buf);
#endif
		/*Write virServ_file : /var/run/nas0_0/vserver0 */
		snprintf(virServ_file, sizeof(virServ_file), VIRSERV_PATH, if_name, cur_id_virSer);
		if((ret = cfg_save_attrs_to_file(server_node, virServ_file, NO_QMARKS)) < 0){
			return -1;
		}
		if (!strcmp(virServ_Active, "Yes")){
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
			cfg_set_object_attr(server_node, "ActiveIFName", if_name);
#endif
			snprintf(buf, sizeof(buf), "%s add %s %s", VIRSERV_SH, if_name, cur_id_virSer);
			system_escape(buf);
		}
	}
	else{
		snprintf(buf, sizeof(buf), "%s del %s %s", VIRSERV_SH, if_name, cur_id_virSer);
		system_escape(buf);
		/*Delete the configuration file*/
		snprintf(virServ_file, sizeof(virServ_file), VIRSERV_PATH, if_name, cur_id_virSer);
		unlink(virServ_file);
	}
	return 0;
}/* end virServer_execute*/

#if defined(TCSUPPORT_CT_JOYME2)
static int svc_wan_related_virserver_update(void)
{
	int i = 0;
	char nodeName[64] = {0};
	
	for(i = 0; i < MAX_VIRSERV_RULE; i++){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, i + 1);
		wan_related_virServer_execute(nodeName);
	}
	return 0;
}
#endif

static int svc_wan_related_virserver_entry_update(char *path)
{

	return wan_related_virServer_execute(path);
}

static int svc_wan_related_virserver_entry_delete(char *path)
{
	cfg_type_virServer_entry_entry_func_delete_core(path);
	cfg_type_default_func_delete(path);

	return 0;
}

static int svc_wan_related_virserver_boot(char *path)
{
	int i;
#if !defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CUC)
	int j;
#endif
	char nodeName[128]	= {0};

#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
	for(i = 0; i < MAX_VIRSERV_RULE; i++){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, i + 1);
		wan_related_virServer_execute(nodeName);
	}
#else
	for(i = 0; i < MAX_WAN_IF_INDEX; i++){
		for(j = 0; j < MAX_VIRSERV_RULE; j++){
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_ENTRY_NODE, i+1, j+1);
			wan_related_virServer_execute(nodeName);
		}
	}
#endif
	return 0;
}

static int svc_wan_related_device_account_update(char *path)
{
	char nodeName[64]={0};
	char lan_nodeName[64] = {0}, dhcpType[8] = {0};
	int status=-1,result = -1;

	int retryTimes = -1, retryLimit = -1, no_landingpage = 0;
	char time_sec[16] = {0};
	struct timespec tcur;

	char tmp[8] = {0};
	int curflag = 0, isDhcpActive = 0;

	clock_gettime(CLOCK_MONOTONIC, &tcur);

	memset(nodeName, 0, sizeof(nodeName));	
	strncpy(nodeName, DEVICEACCOUNT_ENTRY_NODE, sizeof(nodeName)-1);

	memset(lan_nodeName, 0, sizeof(lan_nodeName));
	strncpy(lan_nodeName, LAN_DHCP_NODE, sizeof(lan_nodeName)-1);
	if ( cfg_obj_get_object_attr(lan_nodeName, "type", 0, dhcpType, sizeof(dhcpType)) > 0
		&& 0 == strcmp(dhcpType, "1") )
		isDhcpActive = 1;
	
	memset(tmp, 0, sizeof(tmp) );
	if(cfg_obj_get_object_attr(nodeName, DEVACCOUNT_STATUS, 0, tmp, sizeof(tmp)) < 0){
		printf("error:deviceAccount function Get status value fail!\n");
		return FAIL;
	}
	else
	{
		status = atoi(tmp);
	}

	memset(tmp, 0, sizeof(tmp) );
	if(cfg_obj_get_object_attr(nodeName, DEVACCOUNT_RESULT, 0, tmp, sizeof(tmp)) < 0){
		printf("error:deviceAccount function Get result value fail!\n");
		return FAIL;
	}
	else
	{
		printf(">> deviceaccount execute registerResult = [%s]\n", tmp);
		result = atoi(tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, DEVACCOUNT_RETRYTIMES, 0, tmp, sizeof(tmp)) < 0){
		printf("error:deviceAccount function Get retry times value fail!\n");
		return FAIL;
	}
	else
	{
		retryTimes = atoi(tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, DEVACCOUNT_RETRYLIMIT, 0, tmp, sizeof(tmp)) < 0){
		printf("error:deviceAccount function Get retry limit value fail!\n");
		return FAIL;
	}
	else
	{
		retryLimit = atoi(tmp);
	}

	memset(tmp, 0, sizeof(tmp) );
	if(cfg_obj_get_object_attr(nodeName, "isNOLandingPage", 0, tmp, sizeof(tmp)) < 0)
	  strncpy(tmp, "0", sizeof(tmp)-1);
	no_landingpage = atoi(tmp);
	
	curflag = (status == 99 || (status == 0 && result == 2) || (status == 0 && result == 99)	
	 || (status == 1) || (status == 2) || (status == 3) || (status == 4) || (status == 0 && result == 0));
	if( curflag && (1 != no_landingpage) )
	{
		cfg_set_object_attr(nodeName, DEVACCOUNT_WEBPAGELOCK, "1");
		system("iptables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		system("iptables -t nat -A PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		/* ipv6 */
		system("ip6tables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		system("ip6tables -t nat -A PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
	
		system("ebtables -t filter -D FORWARD -p IPv4 --ip-proto udp --ip-dport 53 -j DROP");
		/* ipv6 */
		system("ebtables -t filter -D FORWARD -p IPv6 --ip6-proto udp --ip6-dport 53 -j DROP");
		if ( isDhcpActive )
		{
			system("ebtables -t filter -A FORWARD -p IPv4 "
					"--ip-proto udp --ip-dport 53 -j DROP");
			/* ipv6 */
			system("ebtables -t filter -A FORWARD -p IPv6 "
					"--ip6-proto udp --ip6-dport 53 -j DROP");
		}
	}
	else
	{
		cfg_set_object_attr(nodeName, DEVACCOUNT_WEBPAGELOCK, "0");
		system("iptables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		/* ipv6 */
		system("ip6tables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
	
		system("ebtables -t filter -D FORWARD -p IPv4 --ip-proto udp --ip-dport 53 -j DROP");
		/* ipv6 */
		system("ebtables -t filter -D FORWARD -p IPv6 --ip6-proto udp --ip6-dport 53 -j DROP");
		cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "isLoidRegistering", "No");
	#ifdef CT_COM_DEVICEREG	
		unlink("/etc/hosts");
	#endif
	}
#ifdef CT_COM_DEVICEREG
	if(prevDevRegFlag != curflag)
		{
			 system("killall -9 dnsmasq");
			 system("/userfs/bin/dnsmasq &");
			 prevDevRegFlag = curflag;
		}
#endif
	if( retryTimes >= retryLimit )
	{
		snprintf(time_sec, sizeof(time_sec), "%d", (int)tcur.tv_sec);
		cfg_set_object_attr(nodeName, "recTime", time_sec);
	}
	cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "retryTimesCommit", "");
	return SUCCESS;
}

static int svc_wan_related_device_account_boot()
{
	char nodeName[64]={0};
	char tmpVar[4] = {0};
	int status = -1, result = -1, no_landingpage = 0;
#if defined(TCSUPPORT_CT_ADSL_BIND1)
	char v_value[128] = {0};
#endif
	int curflag = 0;

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, DEVICEACCOUNT_ENTRY_NODE, sizeof(nodeName)-1);
	
	cfg_set_object_attr(nodeName, "recTime", "-180");
#if !defined(TCSUPPORT_CT_JOYME4)
	cfg_set_object_attr(nodeName, "retryTimes", "0");
#endif

	memset(tmpVar, 0, sizeof(tmpVar) );
	if(cfg_get_object_attr(nodeName, "registerStatus", tmpVar, sizeof(tmpVar)) < 0){
		printf("error:deviceAccount function Get status value fail!\n");
		return -1;
	}
	else
	{
		status = atoi(tmpVar);
	}

	memset(tmpVar, 0, sizeof(tmpVar) );
	if(cfg_get_object_attr(nodeName, "registerResult", tmpVar, sizeof(tmpVar)) < 0){
		printf("error:deviceAccount function Get result value fail!\n");
		return -1;
	}
	else
	{
		result = atoi(tmpVar);
	}

	memset(tmpVar, 0, sizeof(tmpVar) );
	if ( 0 > cfg_get_object_attr(nodeName, "isNOLandingPage", tmpVar, sizeof(tmpVar)) )
	  strcpy(tmpVar, "0");
	no_landingpage = atoi(tmpVar);

	curflag = (status == 99 || (status == 0 && result == 2) || (status == 0 && result == 99)	
	 || (status == 1) || (status == 2) || (status == 3) || (status == 4) || (status == 0 && result == 0));
	if( curflag && (1 != no_landingpage) )
	{
		system("iptables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		system("iptables -t nat -A PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		/* ipv6 */
		system("ip6tables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		system("ip6tables -t nat -A PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
#ifdef CT_COM_DEVICEREG
		prevDevRegFlag = 1;
#endif
		printf("\n\n\ndevice registration webpage LOCKED!\n\n\n");
	}
	else
	{
		system("iptables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		/* ipv6 */
		system("ip6tables -t nat -D PREROUTING -i br+ -p tcp --dport 80 -j REDIRECT --to-ports 80");
		printf("\n\n\ndevice registration webpage UNLOCKED!\n\n\n");
	#ifdef CT_COM_DEVICEREG	
		unlink("/etc/hosts");
		prevDevRegFlag = 0;
	#endif
	}

#if defined(TCSUPPORT_CT_ADSL_BIND1)
	if ( 0 < cfg_get_object_attr(nodeName, "XBINDResult", v_value, sizeof(v_value))
		&& 0 != strcmp(v_value, "SUCC") )
	{
		cfg_set_object_attr(nodeName, "XBINDResult", "");
		cfg_set_object_attr(nodeName, "XBINDType", "");
	}
#endif

	return 0;
}

#if defined(TCSUPPORT_CT_JOYME4)
static void boot_wan_ntp_check_pthread(void)
{
	char ntp_first_boot[12] = {0}, wan_v4_up[12] = {0};
	wan_related_cfg_update_t param;

	/*
	* wan relate mgr event is busy when boot,
	* cant not do EVT_CFG_WAN_RELATED_TIMEZONE_UPDATE fastly.
	*/
	while ( 1 )
	{
		if ( cfg_get_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", ntp_first_boot, sizeof(ntp_first_boot)) > 0
			&& 1 == atoi(ntp_first_boot) )
		{
			/* check wan v4 up done. */
			if ( cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "WanStatus", wan_v4_up, sizeof(wan_v4_up)) > 0
					&& 1 == atoi(wan_v4_up) )
			{
				bzero(&param, sizeof(param));
				snprintf(param.path, sizeof(param.path), "%s", TIMEZONE_ENTRY_NODE);
				if ( SUCCESS == svc_wan_handle_event_timezone_update(&param) )
					printf("svc_wan_handle_event_timezone_update ok.\n");
				/* timezone_update maybe return FAIL before update flag, update it here. */
				cfg_set_object_attr(GLOBALSTATE_NODE, "ntp_first_boot", "2");
				break;
			}
		}
	
		sleep(1);
	}

	pthread_exit(0);
}

int creat_boot_wan_ntp_check_pthread(void)
{
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, (void *)boot_wan_ntp_check_pthread, NULL) < 0)
    {
        printf("creat_boot_wan_ntp_check_pthread: create thread fail \n");
        return -1;
    }

    return 0;
}
#endif

int svc_wan_related_mgr_handle_event(int event,wan_related_cfg_update_t* param)
{
	int ret = 0;

	switch(event)
	{
		case EVT_CFG_WAN_RELATED_FIREWALL_BOOT:
			ret = svc_wan_related_firewall_boot();
			break;
		case EVT_CFG_WAN_RELATED_FIREWALL_UPDATE:
			ret = svc_wan_related_firewall_update();
			break;
#if defined(TCSUPPORT_CMCCV2)
		case EVT_CFG_WAN_RELATED_ATTACK_UPDATE:
			ret = svc_wan_related_attack_update();
			break;
#endif
        #ifdef TCSUPPORT_VPN
        case EVT_CFG_WAN_RELATED_IPSEC_UPDATE:
            //printf("Trey EVT_CFG_WAN_RELATED_IPSEC_UPDATE\n");
            ret = svc_wan_related_ipsec_update();
            break;
        #endif
		case EVT_CFG_WAN_RELATED_QOS_UPDATE:
			if(param != NULL){
				ret = svc_wan_related_qos_update(param);				
			}
			break;
		case EVT_CFG_WAN_RELATED_QOS_BOOT:
			ret = svc_wan_related_qos_boot( param );
			break;
		case EVT_CFG_WAN_RELATED_PPPOESIMULATE:
			ret = svc_wan_related_pppoesimulate( );
			break;
		case EVT_CFG_WAN_RELATED_CUCPING:
			ret = svc_wan_related_cucping( );
			break;
		case EVT_CFG_WAN_RELATED_TIMEZONE_BOOT:
			ret = svc_wan_handle_event_timezone_boot();		
			break;
        case EVT_CFG_WAN_RELATED_TIMEZONE_UPDATE:
			if(param != NULL){
				ret = svc_wan_handle_event_timezone_update(param);		
			}
			break;
		case EVT_CFG_WAN_RELATED_URLFILTER_UPDATE:
			if(param != NULL){
				ret = svc_wan_handle_event_urlfilter_update(param);
			}
			break;	
		case EVT_CFG_WAN_RELATED_URLFILTER_BOOT:
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
			ret = initurlfilterduration(0);
			ret = initdnsfilterduration(0);
			svc_wan_related_urlfilter_write(0);
#else
			svc_wan_related_urlfilter_write();
#endif		
			ret = svc_wan_related_urlfilter_boot();
			break;	
		case EVT_CFG_WAN_RELATED_DNSMASKRESTART:
			svc_wan_handle_event_dnsmask_restart();
			break;
		case EVT_CFG_WAN_RELATED_IPMACFILTER_UPDATE:		
			if(param != NULL){
				ret = svc_wan_handle_event_ipmacfilter_update(param);
			}
			break;	
		case EVT_CFG_WAN_RELATED_IPMACFILTER_BOOT:		
			ret = svc_wan_related_ipmacfilter_boot();
			break;	
		case EVT_CFG_WAN_RELATED_ACL_BOOT:	
		case EVT_CFG_WAN_RELATED_ACL_UPDATE:		
			ret = svc_wan_related_acl_update();
			break;	
		case EVT_CFG_WAN_RELATED_IGMPPROXY_UPDATE:
			ret = svc_wan_related_igmpproxy_update();
			break;
		case EVT_CFG_WAN_RELATED_MLDPROXY_UPDATE:
			ret = svc_wan_related_mldproxy_update();
			break;
		case EVT_CFG_WAN_RELATED_DDNS_UPDATE:
			ret = svc_wan_related_ddns_update();
			break;
                case EVT_CFG_WAN_RELATED_DDNS_BOOT:
			ret = svc_wan_related_ddns_boot();
			break;
		case EVT_CFG_WAN_RELATED_DEVICE_ACCOUNT_UPDATE:
			ret = svc_wan_related_device_account_update(param->path);
			break;
		case EVT_CFG_WAN_RELATED_DEVICE_ACCOUNT_BOOT:
			ret = svc_wan_related_device_account_boot();
			break;
		case EVT_CFG_WAN_RELATED_ATTACH_UPDATE:
			ret = svc_wan_related_attach_update();
			break;
		case EVT_CFG_WAN_RELATED_VIRSERVER_ENTRY_UPDATE:
			ret = svc_wan_related_virserver_entry_update(param->path);
			break;
		case EVT_CFG_WAN_RELATED_VIRSERVER_ENTRY_DELETE:
			ret = svc_wan_related_virserver_entry_delete(param->path);
			break;
		case EVT_CFG_WAN_RELATED_VIRSERVER_BOOT:
			ret = svc_wan_related_virserver_boot(param->path);
			break;
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
		case EVT_CFG_WAN_RELATED_VIRSERVER_UPDATE:
			ret = svc_wan_related_virserver_update();
			break;
		case EVT_CFG_WAN_RELATED_INCOMINGFILTER_BOOT:
		case EVT_CFG_WAN_RELATED_INCOMINGFILTER_UPDATE:
			ret = svc_wan_related_incomingfilter_update();
			break;
		case EVT_CFG_WAN_RELATED_DNSFILTER_UPDATE:
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CT_UBUS)
			if(param != NULL){
				ret = svc_wan_releated_dnsfilter_update(param);
			}
#else
			ret = svc_wan_releated_dnsfilter_update();
#endif
			break;
#endif
#if defined(TCSUPPORT_CT_VPN_PPTP) || defined(TCSUPPORT_CT_L2TP_VPN)
		case EVT_CFG_WAN_RELATED_VPN_BOOT:
			ret = svc_wan_releated_vpn_boot();
			break;
			
		case EVT_CFG_WAN_RELATED_VPN_UPDATE:	
			ret = svc_wan_releated_vpn_update(param);
			break;

		case EVT_CFG_WAN_RELATED_VPN_IP_DOMAIN_UPDATE:	
			ret = svc_wan_related_vpn_ip_domain_update(param);
			break;

		case EVT_CFG_WAN_RELATED_VPN_MAC_UPDATE:	
			ret = svc_wan_related_vpn_mac_update(param);
			break;
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		case EVT_CFG_WAN_RELATED_DOWNLINKQOS_UPDATE:
			ret = svc_wan_related_downlinkqos_update(param);				
			break;
		case EVT_CFG_WAN_RELATED_DOWNLINKQOS_BOOT:
			ret = svc_wan_related_downlinkqos_boot( param );
			break;
#endif
#if defined(TCSUPPORT_VXLAN)
		case EVT_CFG_WAN_RELATED_VXLAN_BOOT:
			ret = svc_wan_related_start_all_vxlan();
			break;
		case EVT_CFG_WAN_RELATED_VXLAN_UPDATE:
			ret = svc_wan_related_vxlan_update(param->path);
			break;
		case EVT_CFG_WAN_RELATED_VXLAN_DELETE:
			ret = svc_wan_related_vxlan_delete(param->path);
			break;
		case EVT_CFG_WAN_RELATED_VXLAN_GET4:
			ret = svc_wan_related_vxlan_get4(param->path);
			break;
		case EVT_CFG_WAN_RELATED_VXLAN_LOST4:
			ret = svc_wan_related_vxlan_lost4(param->path);
			break;
#endif
#if defined(TCSUPPORT_STUN)
		case EVT_CFG_WAN_RELATED_STUN_UPDATE:
			ret = svc_wan_related_stun_update();
			break;
		case EVT_CFG_WAN_RELATED_STUN_BOOT:
			ret = svc_wan_related_stun_boot();
			break;
#endif
		
		default:
			printf("svc_wan_related_mgr_handle_event: event [%d]  \n",event);	
	}

	return ret;
}

int svc_wan_related_entry_delete(wan_related_cfg_update_t* param)
{
	char ifname[16] 	= {0};
	char* path 			= NULL;
	wan_related_conf_obj* obj = NULL;
	
	path = param->path;

	if((obj = svc_wan_related_find_object(path)) != NULL)
	{
		if(obj->cfg.linkmode == SVC_WAN_RELATED_ATTR_LINKMODE_PPP ||
			obj->cfg.isp == WAN_ENCAP_PPP_INT)
		{
			snprintf(ifname, sizeof(ifname), "ppp%d", obj->idx);
		}
		else
		{
			strncpy(ifname, obj->dev, sizeof(ifname)-1);
		}

		/*Delete dmz entry for this interface*/
		svc_wan_related_del_dmz_entry(obj->idx);
		
      	/*Delete virtual server entry for this interface*/
		svc_wan_related_del_virserver_entry(obj->idx);

		/* proxy will auto switch wan interface, not need to excute below */
#if !defined(TCSUPPORT_CT_JOYME4)
		/*delete related igmp & mld proxy info*/
		svc_wan_related_del_igmp_mld_proxy(ifname);
#endif
		
		/*attention: all interface conf need to delete before removing wan obj from list*/
		/*finally delete wan obj from list*/
		svc_wan_related_del_from_list(obj);
	}
	else
	{
		tcdbg_printf("wan obj %s is not found! \n", path);
	}

	return SUCCESS;
}


static int svc_wan_related_update_object(wan_related_conf_obj* obj)
{
    wan_entry_cfg tmp;

    if (svc_wan_related_load_cfg(obj->path,&tmp) < 0)
    {
        tcdbg_printf("svc_wan_related_update_object: load cfg fail \n");
        return -1;
    }

    memcpy(&obj->cfg,&tmp,sizeof(wan_entry_cfg));
    return 0;
}

#if !defined(TCSUPPORT_CT_UBUS)
static void update_all_route4_entry(void)
{
	char nodeName[128] = {0};
	char Active[5] = {0};
	int i = 0;

	for(i = 0; i < MAX_STATIC_ROUTE_NUM; i++)
	{
		cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "route_id", itoa(i));
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ROUTE_ENTRY_NODE, i+1);
		memset(Active, 0, sizeof(Active));
		cfg_obj_get_object_attr(nodeName, "Active", 0, Active, sizeof(Active));

		if (strcmp(Active, "Yes") == 0)
			cfg_obj_commit_object("root.route");
	}
}
#endif
int restart_udpxy(char *ifname,char *state)
{
	int pvc_index = 0;
	int entry_index = 0;
	int if_index = 0;
	char node_name[128] = {0};
	char svc_list[64] = {0};
	char wan_mode[64] = {0};
	
	if((if_index = get_wanindex_by_name(ifname)) < 0){
		return FAIL;
	}
	
	pvc_index = if_index/PVC_NUM;
	entry_index = if_index%PVC_NUM;
	snprintf(node_name,sizeof(node_name),WAN_PVC_ENTRY_NODE,pvc_index+1,entry_index+1);

	if((cfg_get_object_attr(node_name,"ServiceList",svc_list,sizeof(svc_list)) >= 0 
		&& !strncmp(svc_list, "OTHER", sizeof(svc_list)-1)) 
		&&(cfg_get_object_attr(node_name,"WanMode",wan_mode,sizeof(wan_mode)) >= 0 &&
		!strncmp(wan_mode, "Route", sizeof(wan_mode)-1)))
	{
		cfg_set_object_attr(IPTV_ENTRY_NODE, "status", state);
		cfg_set_object_attr(IPTV_ENTRY_NODE, "ifname", ifname);
		cfg_commit_object(IPTV_ENTRY_NODE);
	}

	return SUCCESS;
}

int svc_wan_related_ipv4_up(wan_related_cfg_update_t* param)
{
	int if_index		= 0;
#if !defined(TCSUPPORT_NP_CMCC)
	int i 				= 0;
	char node_name[128] = {0};
#endif
	char ifname[32]		= {0};
	char* path = param->path;
	
#if defined(TCSUPPORT_NP_CMCC)
	int apRoute = 0;

	if ( 0 == strcmp(path, NP_AP_B_WAN_IF) )
	{
		apRoute = 0;
		strncpy(ifname, NP_AP_B_WAN_IF, sizeof(ifname) - 1);
	}
	else
	{
		apRoute = 1;
		/*Get wan interface cfg node index*/
		if(cfg_get_object_attr(path, "IFName", ifname, sizeof(ifname)) <= 0)
		{
			return FAIL;
		}
	}
	if_index = 0;
#else
#if defined(TCSUPPORT_CT_UBUS)
	int apRoute = 0;
	if ( 0 == strcmp(path, WAN_ITF_BR0_1) )
	{
		apRoute = 0;
		if_index = 0;
		strncpy(ifname, WAN_ITF_BR0_1, sizeof(ifname) - 1);
	}
	else
	{
		apRoute = 1;
#endif
	/*Get wan interface cfg node index*/
	if(cfg_get_object_attr(path, "IFName", ifname, sizeof(ifname)) <= 0){
		return FAIL;
	}
	if((if_index = get_wanindex_by_name(ifname)) < 0){
		return FAIL;
	}

	/* restart dmz based this wan interface */
	snprintf(node_name, sizeof(node_name), DMZ_ENTRY_NODE, if_index + 1);
	cfg_commit_object(node_name);
	
	/* restart virtual based this wan interface */
	for(i = 0; i < MAX_VIRSERV_RULE; i++){
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), VIRSERVER_ENTRY_ENTRY_NODE, if_index + 1, i + 1);
		cfg_commit_object(node_name);
	}
#if defined(TCSUPPORT_CT_UBUS)
	}
#endif
#endif
	
#if defined(TCSUPPORT_IGMP_PROXY)
#if !defined(TCSUPPORT_CT_CONGW_CH)
	/*restart igmpproxy*/
#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
	if ( apRoute )
#endif
	restart_igmpproxy(ifname);
#endif
#endif

#if defined(TCSUPPORT_CT_JOYME4)
	restart_udpxy(ifname,"up");
#endif

#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
	if ( apRoute )
#endif
	recheckPingDiagnostic(if_index);
#endif

#if !defined(TCSUPPORT_CMCCV2) && !defined(TCSUPPORT_NP_CMCC) && !defined(TCSUPPORT_CT_UBUS)
	restart_dhcprelay(if_index);
#endif

#if defined(TCSUPPORT_WLAN_8021X) 
	restart_rtdot1xd();
#endif
#if defined(TCSUPPORT_CMCCV2)
	restart_wantelnet();
#endif

	resyncNTPServer(ifname,if_index,IF_STATE_UP);

#if !defined(TCSUPPORT_CT_UBUS)
    #ifdef TCSUPPORT_VPN
    svc_wan_related_ipsec_update();
    #endif

	svc_wan_related_acl_update();
	update_all_route4_entry();
#if defined(TCSUPPORT_VXLAN)
	svc_wan_related_vxlan_binding_wanif(ifname, E_VXLAN_WAN_UP);
#endif
#endif
	return SUCCESS;
}


int svc_wan_related_ipv4_down(wan_related_cfg_update_t* param)
{
#if defined(TCSUPPORT_VXLAN)
	char ifname[32]		= {0};
#endif

#if defined(TCSUPPORT_CMCCV2)
	restart_wantelnet();
#endif

	svc_wan_related_acl_update();
#if defined(TCSUPPORT_VXLAN)
	/*Get wan interface cfg node index*/
	if ( cfg_get_object_attr(param->path, "IFName", ifname, sizeof(ifname)) <= 0 )
	{
		return FAIL;
	}
	svc_wan_related_vxlan_binding_wanif(ifname, E_VXLAN_WAN_DOWN);
#endif
	return SUCCESS;
}

static void update_all_route6_entry(void)
{
	char nodeName[128] = {0};
	char Active[5] = {0};
	int i = 0;

	for(i = 0; i < MAX_STATIC_ROUTE6_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), ROUTE6_ENTRY_NODE, i+1);
		memset(Active, 0, sizeof(Active));
		cfg_obj_get_object_attr(nodeName, "Active", 0, Active, sizeof(Active));

		if (strcmp(Active, "Yes") == 0)
			cfg_obj_commit_object(nodeName);
	}
}

int svc_wan_related_ipv6_up(wan_related_cfg_update_t* param)
{
	int if_index		= 0;
	char ifname[32]		= {0};
	char* path = param->path;

	/*Get wan interface cfg node index*/
	if(cfg_get_object_attr(path, "IFName", ifname, sizeof(ifname)) <= 0){
		return FAIL;
	}
	if((if_index = get_wanindex_by_name(ifname)) < 0){
		return FAIL;
	}

#ifdef TCSUPPORT_MLD_PROXY
#if !defined(TCSUPPORT_CT_CONGW_CH)
	/*restart mldproxy*/
	restart_mldproxy(ifname, if_index);
#endif
#endif
	update_all_route6_entry();

	return SUCCESS;
}


int svc_wan_related_get_wan_conf(char* path)
{
	wan_related_conf_obj* obj;
	int id[CFG_MAX_NODE_LEVEL];
	unsigned int pvc,entry;

	if (cfg_query_object(path,NULL,NULL) <= 0)
	{
        tcdbg_printf("svc_wan_related_entry_update: cfg query [%s] fail  \n",path);
        return -1;
	}

	if((obj = svc_wan_related_find_object(path)) == NULL)
	{
		if (cfg_path_to_oid_array(path,id) != 4)
        {
            tcdbg_printf("svc_wan_related_entry_update: parse [%s] fail \n",path);
            return -1;
        }

        pvc = (id[2] & CFG_OBJ_MASK )- 1;
        entry = (id[3] & CFG_OBJ_MASK) - 1;

        if (pvc >= PVC_NUM || entry >= MAX_SMUX_NUM )
        {
            tcdbg_printf("svc_wan_related_entry_update: [%s] pvc[%d] entry[%d] error \n",path,id[1],id[2]);
            return -1;
        }

        if ((obj = svc_wan_related_create_object(path)) == NULL)
        {
            tcdbg_printf("svc_wan_related_entry_update: [%s] create fail \n",path);
            return -1;
        }

        strncpy(obj->path,path, sizeof(obj->path) - 1);
        obj->idx = pvc * MAX_SMUX_NUM + entry;
        snprintf(obj->dev, sizeof(obj->dev), "nas%d_%d", pvc, entry);
	}

	if (svc_wan_related_update_object(obj) < 0)
	{
        tcdbg_printf("svc_wan_related_entry_update: [%s] update fail \n",path);
        return -1;
	}
	
	return 0;
}


void svc_wan_related_free_all_wan_conf(void)
{
	wan_related_conf_obj* tmp = NULL;
	wan_related_conf_obj* obj = NULL;

	obj = svc_wan_related_get_obj_list();
	while(obj != NULL)
	{
	    tmp = obj;
	    obj = obj->next;
	    svc_wan_related_free_object(tmp);
	}

	svc_wan_related_init_obj_list();
	return;
}



int svc_wan_related_mgr_start(void)
{
#if 0
	char node[32] = {0};
	strncpy(node, "root.Firewall", sizeof(node)-1);
	svc_wan_related_mgr_handle_event(WAN_RELATED_EVENT_FIREWALL_UPDATE,node);
	strncpy(node, "root.ACL", sizeof(node)-1);
	svc_wan_related_mgr_handle_event(WAN_RELATED_EVENT_ACL_UPDATE,node);
	strncpy(node, "root.Timezone", sizeof(node)-1);
	svc_wan_related_mgr_handle_event(WAN_RELATED_EVENT_TIMEZONE_UPDATE,node);
	strncpy(node, "root.AppFilter", sizeof(node)-1);
	svc_wan_related_mgr_handle_event(WAN_RELATED_EVENT_APPFILTER_UPDATE,node);
#endif	
	return 0;
}

int svc_wan_related_mgr_stop(void)
{

#if 0
	wan_entry_obj_t *tmp, *obj = svc_wan_entry_list;

	while(obj != NULL)
	{
		tmp = obj;
		obj = obj->next;
		svc_wan_stop_intf(tmp);
		svc_wan_free_object(tmp);
	}
	
	svc_wan_entry_list = NULL;
#endif
	return 0;
}

