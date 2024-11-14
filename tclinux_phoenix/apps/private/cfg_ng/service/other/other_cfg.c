

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
#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_cli.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include "other_mgr.h"
#include "other_cfg.h"
#include <crypt.h>
#include <unistd.h>
#include <sys/stat.h>
#include "utility.h"
#include <linux/version.h>
#include <ctype.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include <netinet/in.h>
#include "blapi_perform.h"
#include "blapi_traffic.h"
#include <syslog.h>
#include <flash_layout/tc_partition.h>


typedef struct IpMask_s
{
	char ip_v4[16];	
	char ip_netmask_v4[16];
	char mask_dec[8];
}IpMask_t, *IpMask_p;

struct portBind_map
{
	char *bindif;
	char *realif;
	int lanBindStatus;
};

char LanIP[32] = {0};

#ifdef TCSUPPORT_IGMP_SNOOPING
void svc_other_operateIgmpSnooping(void)
{
	char  tmp[32], nodeName[32];
	char cmdbuf[100] = {0}; 
#ifdef TCSUPPORT_MULTICAST_SPEED
	char cmdbuftemp[100] = {0};
#endif

#ifdef TCSUPPORT_SNOOPING_SEPERATION
	char igmpProxyActive[32] = {0},  mldProxyActive[32] = {0};

	memset(nodeName, 0, sizeof(nodeName));  	
	strncpy(nodeName, IGMPPROXY_ENTRY_NODE, sizeof(nodeName)-1);
	cfg_get_object_attr(nodeName, "Active", igmpProxyActive, sizeof(igmpProxyActive));
	
	memset(nodeName, 0, sizeof(nodeName));  	
	strncpy(nodeName, MLDPROXY_ENTRY_NODE, sizeof(nodeName)-1);
	cfg_get_object_attr(nodeName, "Active", mldProxyActive, sizeof(mldProxyActive));
#endif

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, LAN_IGMPSNOOP_NODE, sizeof(nodeName)-1);
	/*enable or disable igmp snooping*/
	if(cfg_get_object_attr(nodeName, "igmpsnoopEnable", tmp, sizeof(tmp)) > 0)
	{
		if(strcmp(tmp,"Yes") == 0
#ifdef TCSUPPORT_SNOOPING_SEPERATION	
			 || (strcmp(tmp, "No") == 0 && strcmp(igmpProxyActive, "Yes") == 0)
#endif
			 )
		{	
#ifdef TCSUPPORT_SNOOPING_SEPERATION
			snprintf(cmdbuf, sizeof(cmdbuf), "%s", "brctl igmpsnoop br0 on");
#else
			strncpy(cmdbuf, "brctl snoop br0 on", sizeof(cmdbuf)-1);			
#endif
#ifdef TCSUPPORT_MULTICAST_SPEED
#if defined(TCSUPPORT_CT_SIMCARD_SEPARATION) && defined(TCSUPPORT_CT_2PORTS) 
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat -V 1", sizeof(cmdbuftemp)-1);
			system(cmdbuftemp);
			
			memset(cmdbuftemp,0,sizeof(cmdbuftemp));
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat  -@ 1", sizeof(cmdbuftemp)-1);
			system(cmdbuftemp);
			
			memset(cmdbuftemp,0,sizeof(cmdbuftemp));
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat  -!", sizeof(cmdbuftemp)-1);
#else
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat -V 1", sizeof(cmdbuftemp)-1);
#endif
#endif

		}
		else
		{
#ifdef TCSUPPORT_SNOOPING_SEPERATION
						snprintf(cmdbuf, sizeof(cmdbuf), "%s", "brctl igmpsnoop br0 off");
#else
#if defined(TCSUPPORT_CT_SIMCARD_SEPARATION) && defined(TCSUPPORT_CT_2PORTS) 
			strncpy(cmdbuf, "brctl snoop br0 on", sizeof(cmdbuf)-1);	
#else
			strncpy(cmdbuf, "brctl snoop br0 off", sizeof(cmdbuf)-1);	
#endif
#endif
#ifdef TCSUPPORT_MULTICAST_SPEED
#if defined(TCSUPPORT_CT_SIMCARD_SEPARATION) && defined(TCSUPPORT_CT_2PORTS) 
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat -V 1", sizeof(cmdbuftemp)-1);
			system(cmdbuftemp);
			
			memset(cmdbuftemp,0,sizeof(cmdbuftemp));
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat  -@ 0", sizeof(cmdbuftemp)-1);
			system(cmdbuftemp);
			
			memset(cmdbuftemp,0,sizeof(cmdbuftemp));
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat  -!", sizeof(cmdbuftemp)-1);
#else
			strncpy(cmdbuftemp, "/userfs/bin/hw_nat -V 1", sizeof(cmdbuftemp)-1);
#endif
#endif

		}
	}
	system(cmdbuf);
	
#ifdef TCSUPPORT_SNOOPING_SEPERATION
		bzero(tmp, sizeof(tmp));
		cfg_get_object_attr(nodeName, "mldsnoopEnable", tmp, sizeof(tmp));
		if ( 0 == strcmp(tmp,"Yes") || (strcmp(tmp, "No") == 0 && strcmp(mldProxyActive, "Yes") == 0) )
			snprintf(cmdbuf, sizeof(cmdbuf), "%s", "brctl mldsnoop br0 on");
		else
			snprintf(cmdbuf, sizeof(cmdbuf), "%s", "brctl mldsnoop br0 off");
		system(cmdbuf);
#endif
#ifdef TCSUPPORT_MULTICAST_SPEED
	system(cmdbuftemp);
#endif
	/*set age time*/
	memset(cmdbuf,0,sizeof(cmdbuf));
	if(cfg_get_object_attr(nodeName, "igmpsnoopAge", tmp, sizeof(tmp)) > 0)
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "brctl snoop br0 %s",tmp);
	}
	system_escape(cmdbuf);
	/*set quick leave function*/
	memset(cmdbuf,0,sizeof(cmdbuf));
	if(cfg_get_object_attr(nodeName, "quickleaveEnable", tmp, sizeof(tmp)) > 0)
	{
		if(strcmp(tmp,"Yes") == 0)
		{	
			strncpy(cmdbuf, "brctl quickleave br0 on", sizeof(cmdbuf)-1);
		}
		else
		{
			strncpy(cmdbuf, "brctl quickleave br0 off", sizeof(cmdbuf)-1);
		}
	}
	else{
		strncpy(cmdbuf, "brctl quickleave br0 on", sizeof(cmdbuf)-1);
	}
	system(cmdbuf);
	/*set debug function*/
	memset(cmdbuf,0,sizeof(cmdbuf));
	if(cfg_get_object_attr(nodeName, "igmpdbgEnable", tmp, sizeof(tmp)) > 0)
	{
		if(strcmp(tmp,"Yes") == 0)
		{	
			strncpy(cmdbuf, "brctl snoopdbg br0 on", sizeof(cmdbuf)-1); 			
		}
		else
		{
			strncpy(cmdbuf, "brctl snoopdbg br0 off", sizeof(cmdbuf)-1);					
		}
	}
	system(cmdbuf);
	return;
}
#endif


#if defined(TCSUPPORT_CT_BRIDGEARP_NOFWD_LAN)
int svc_other_addBridgeArpRules ()
{
	char bufCmd[128] = {0};
	char nodeName[64] = {0};
	char active[8] = {0};
	char lanIP[20] = {0};
	char netmask[20] = {0};
	char mask_dec[4] = {0};
	char isp[4] = {0};
	int pvc_index = 0;
	int entry_index = 0;
#ifdef ALIAS_IP
	int lan_index = 0;
	char aliasIPActive[LAN_ALIAS_NUM][8] = {0};
	char aliasLanIP[LAN_ALIAS_NUM][20] = {0};
	char aliasnetmask[LAN_ALIAS_NUM][20] = {0};
	char aliasmask_dec[LAN_ALIAS_NUM][4] = {0};
#endif

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, LAN_ENTRY_NODE, sizeof(nodeName)-1);
	if ( cfg_get_object_attr(nodeName,LAN_IP,lanIP,sizeof(lanIP) ) < 0 )
	{
		return -1;
	}
	if ( cfg_get_object_attr(nodeName,LAN_MASK,netmask,sizeof(netmask) ) < 0 )
	{
		return -1;
	}

	if ( 0 == strlen(netmask) )
		return -1;
	if ( !svc_other_check_mask_format(netmask,mask_dec) )
		return -1;

#ifdef ALIAS_IP
	for (lan_index = 0; lan_index < LAN_ALIAS_NUM; lan_index++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), LANALIAS_ENTRY_NODE, lan_index+1);

		if ( cfg_get_object_attr(nodeName, "Active", aliasIPActive[lan_index], 8) < 0 )
			break;

		if ( 0 == strcmp(aliasIPActive[lan_index],"Yes") )
		{
			if ( cfg_get_object_attr(nodeName, LAN_IP, aliasLanIP[lan_index], 20) < 0 )
				break;

			if ( cfg_get_object_attr(nodeName, LAN_MASK, aliasnetmask[lan_index], 20) < 0 )
				break;

			if ( 0 == strlen(aliasnetmask[lan_index]) ) 
				break;

			if ( !svc_other_check_mask_format(aliasnetmask[lan_index], aliasmask_dec[lan_index]) )
				break;
		}
	}
#endif

	system("ebtables -t filter -F bridgeArp_chain");
	system("ebtables -t filter -Z bridgeArp_chain");
	for(pvc_index = 0; pvc_index < PVC_NUM; pvc_index++)
	{
		for(entry_index = 0; entry_index < MAX_SMUX_NUM; entry_index++)
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName,sizeof(nodeName),WAN_PVC_ENTRY_NODE,pvc_index+1,entry_index+1);
			memset(active, 0, sizeof(active));
			memset(isp, 0, sizeof(isp));

			if ( cfg_get_object_attr(nodeName,NODE_ACTIVE,active,sizeof(active) ) < 0
				|| 0 != strcmp(active, "Yes"))
				continue;

			if ( cfg_get_object_attr(nodeName,WAN_ISP,isp,sizeof(isp)) < 0
				|| WAN_ENCAP_BRIDGE_INT != atoi(isp) ) /* only bridge mode need this rule */
				continue;

			/* when bridge,rule should apply to wan interface of nasx */
			memset(bufCmd, 0, sizeof(bufCmd));
			snprintf(bufCmd, sizeof(bufCmd), "ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-src %s/%s -i nas%d_%d -j DROP",
							lanIP, mask_dec, pvc_index, entry_index);
			system(bufCmd);

			memset(bufCmd, 0, sizeof(bufCmd));
			snprintf(bufCmd, sizeof(bufCmd), "ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-dst %s/%s -i nas%d_%d -j DROP",
							lanIP, mask_dec, pvc_index, entry_index);
			system(bufCmd);

#ifdef ALIAS_IP
			for ( lan_index = 0; lan_index < LAN_ALIAS_NUM; lan_index++ )
			{
				if ( 0 == strcmp(aliasIPActive[lan_index], "Yes") )
				{
					memset(bufCmd, 0, sizeof(bufCmd));
					snprintf(bufCmd, sizeof(bufCmd), "ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-src %s/%s -i nas%d_%d -j DROP",
									aliasLanIP[lan_index], aliasmask_dec[lan_index], pvc_index, entry_index);
					system_escape(bufCmd);

					memset(bufCmd, 0, sizeof(bufCmd));
					snprintf(bufCmd, sizeof(bufCmd), "ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-dst %s/%s -i nas%d_%d -j DROP",
									aliasLanIP[lan_index], aliasmask_dec[lan_index], pvc_index, entry_index);
					system_escape(bufCmd);
				}
			}
#endif
		}
	}

	bzero(bufCmd, sizeof(bufCmd));
	snprintf(bufCmd, sizeof(bufCmd), "/usr/bin/ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-src %s/%s -i nas+ -j DROP",lanIP, "255.255.255.255");
	system_escape(bufCmd);

	bzero(bufCmd, sizeof(bufCmd));
	snprintf(bufCmd, sizeof(bufCmd), "/usr/bin/ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-dst %s/%s -i nas+ -j DROP",lanIP, "255.255.255.255");
	system_escape(bufCmd);

#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
	bzero(bufCmd, sizeof(bufCmd));
	snprintf(bufCmd, sizeof(bufCmd), "/usr/bin/ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-src %s/%s -i apcli+ -j DROP",
					lanIP, "255.255.255.255");
	system_escape(bufCmd);

	bzero(bufCmd, sizeof(bufCmd));
	snprintf(bufCmd, sizeof(bufCmd), "/usr/bin/ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-dst %s/%s -i apcli+ -j DROP",
					lanIP, "255.255.255.255");
	system_escape(bufCmd);

	bzero(bufCmd, sizeof(bufCmd));
	snprintf(bufCmd, sizeof(bufCmd), "/usr/bin/ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-src %s/%s -i apclii+ -j DROP",
					lanIP, "255.255.255.255");
	system_escape(bufCmd);

	bzero(bufCmd, sizeof(bufCmd));
	snprintf(bufCmd, sizeof(bufCmd), "/usr/bin/ebtables -t filter -A bridgeArp_chain -p arp --arp-ip-dst %s/%s -i apclii+ -j DROP",
					lanIP, "255.255.255.255");
	system_escape(bufCmd);
#endif

	return 0;
}
#endif

#if defined(TCSUPPORT_CMCC)
int svc_other_bsr_control(char* path)
{
	char  tmp[32]={0};

	int bsr_index = 0, rate = 0;
	char bsr_buf[10] = {0};
	char cmd[64] = {0};
	int bsr_lan = 0;
	int ret = -2;

	/* get boardcast storm rate, default rate is 3M */
	if(cfg_get_object_attr(path, "Rate", tmp, sizeof(tmp)) > 0){
		rate = atoi(tmp);
	}
	else {
		rate = 3;
	}
	
	/* set broadcast storm rate control  */
	for (bsr_index = 0; bsr_index < 4; bsr_index++) {
		snprintf(bsr_buf, sizeof(bsr_buf), "LAN%d", bsr_index);
		if(cfg_get_object_attr(path, bsr_buf, tmp, sizeof(tmp)) > 0){
			if (strcmp(tmp, "1") == 0) {

				bsr_lan |= (1 << bsr_index);
			}
		}
	}

	ret = blapi_traffic_set_broadcast_ratelimit(bsr_lan, rate);
	return 0;
}
#endif

#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
int svc_other_setBridgeMacLimit()
{
	char  tmp[32]={0};

	char bml_attr[16];
	char bml_cmd[64];
	int i = 0;

	if(cfg_get_object_attr(LAN_BRIDGEMACLIMIT_NODE, "macLimitTotal", tmp, sizeof(tmp)) > 0)
	{
		snprintf(bml_cmd, sizeof(bml_cmd), "%d\n", atoi(tmp));
		doValPut("/proc/br_fdb/mac_limit_total", bml_cmd);
	}

#ifdef TCSUPPORT_MULTI_SWITCH_EXT
	for(i = 1; i < 9;i++)
#else
	for(i = 1; i < 5;i++)
#endif
	{
		memset(bml_attr,0,sizeof(bml_attr));
		snprintf(bml_attr, sizeof(bml_attr), "macLimitPort%d", i);
		if(cfg_get_object_attr(LAN_BRIDGEMACLIMIT_NODE, bml_attr, tmp, sizeof(tmp)) > 0)
		{
                        /*eth0.1~eth0.4=>port:3~6*/
			snprintf(bml_cmd, sizeof(bml_cmd), "%d-%d\n", i+2, atoi(tmp));
			doValPut("/proc/br_fdb/mac_limit_by_port", bml_cmd);
		}
	}

	system("/userfs/bin/hw_nat -!");

	return 0;
}
#endif

int svc_other_handle_event_lan_write()
{
	FILE *fp 			= NULL;
	char path[128]		= {0};
	char lan_ip[16] 	= {0};
	char lan_mask[16] 	= {0};
	char buf[128]		= {0};
	int i = 0;			/* Looping var */
	int ret = -1;
#ifdef IPV6
	char ip_v6[39]		= {0};
	char prefix_v6[3]	= {0};	
#endif
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
	char tmp_node_str[40] = {0};
#endif
#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
	char cmdBuf[128]= {0};
#endif
	int res = 0;
#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
	char ap_mode[16] = {0};
#endif
	int is_route_mode = 1;
#if defined(TCSUPPORT_NP_CMCC)
	char ap_bst[12] = {0};
	int i_ap_bst = 0;
#endif

	strncpy(path, LAN_ENTRY_NODE, sizeof(path)-1);

	if(cfg_query_object(path, NULL, NULL) > 0){
		fp = fopen(LAN_PATH,"w");
		if(fp == NULL){
			return -1;
		}
		else{
			if (cfg_get_object_attr(path,LAN_IP,lan_ip,sizeof(lan_ip)) < 0)
			{
				fclose(fp);
				return -1;
			}

			if (cfg_get_object_attr(path,LAN_MASK,lan_mask,sizeof(lan_mask)) < 0)
			{
				fclose(fp);
				return -1;
			}

			/*Buf: /sbin/ifconfig br0 10.10.10.10 netmask 255.255.255.0 up*/
			snprintf(buf, sizeof(buf), "%s %s %s netmask %s up\n",
				LAN_IFCONFG_CMD, LAN_IF, lan_ip,
				lan_mask);
			fputs_escape(buf, fp);

#if defined(TCSUPPORT_CT)
			ret = blapi_traffic_set_lan_info(lan_ip, lan_mask);
#endif
			
	#if defined(TCSUPPORT_ANDLINK) || defined(TCSUPPORT_CT_UBUS)
			system_escape("/usr/bin/ebtables -t broute -F ALINK_BROUTE");
			system_escape("/usr/bin/ebtables -t broute -Z ALINK_BROUTE");
#if defined(TCSUPPORT_CT_UBUS)
			cfg_obj_get_object_attr(WAN_COMMON_NODE, "WorkMode", 0, ap_mode, sizeof(ap_mode));
			if ( 0 != strcmp(ap_mode, "router") )
#else
			cfg_obj_get_object_attr(APWANINFO_COMMON_NODE, "FixedAPMode", 0, ap_mode, sizeof(ap_mode));
			if ( 0 != strcmp(ap_mode, "Route") )
#endif
			{
				snprintf(cmdBuf, sizeof(cmdBuf)
								, "/usr/bin/ebtables -t broute -A ALINK_BROUTE"
								" -p IPv4 --ip-proto TCP"
								" --ip-dst %s"
								" --ip-dport 80 -j redirect", lan_ip);
				system_escape(cmdBuf);
			}
	#endif
	
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
			memset(buf,0,sizeof(buf));
			snprintf(buf, sizeof(buf), "%s %s %s %s\n",
				LAN_ACCESSLIMIT_CMD, LAN_IF, lan_ip,
				lan_mask);
			fputs_escape(buf, fp);
#endif
			
	#ifdef IPV6
#if defined(TCSUPPORT_NP_CMCC)
			if ( cfg_get_object_attr(GLOBALSTATE_NODE, "ap_bridge_st", ap_bst, sizeof(ap_bst)) > 0 )
				i_ap_bst = atoi(ap_bst);
			/* ap_bridge_st ==> 1 already bridge   2 already route. */
			if ( 1 == i_ap_bst )
				is_route_mode = 0;
#endif
			if ( is_route_mode )
			{
				if (cfg_get_object_attr(path,LAN_IPV6,ip_v6,sizeof(ip_v6)) < 0)
				{
					fclose(fp);
					return -1;
				}

				if (cfg_get_object_attr(path,LAN_PREFIXV6,prefix_v6,sizeof(prefix_v6)) < 0)
				{
					fclose(fp);
					return -1;
				}
			
				if(strlen(ip_v6) != 0 && strlen(prefix_v6) != 0){	
					/*Buf: /sbin/ifconfig br0 inet6 ip_v6/prefix_v6*/
					snprintf(buf, sizeof(buf), "%s %s add %s/%s\n",
					LAN_IFCONFG_CMD, LAN_IF, ip_v6,
					prefix_v6);
					fputs_escape(buf, fp);
				}
			}
	#endif
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
			snprintf(prefix_v6, sizeof(prefix_v6), "%d", MAX_PREFIX_MASK_LEN);
			for(i = START_NEW_ADD_lAN_IPV6; i<= MAX_NEW_ADD_lAN_IPV6; i++){
				snprintf(tmp_node_str, sizeof(tmp_node_str), "%s_%d", LAN_IPV6, i);
				if (cfg_get_object_attr(path,tmp_node_str,ip_v6,sizeof(ip_v6)) < 0)
					continue;
				if(strlen(ip_v6) != 0 && strlen(prefix_v6) != 0){	
					/*Buf: /sbin/ifconfig br0 inet6 ip_v6/prefix_v6*/
					snprintf(buf, sizeof(buf), "%s %s add %s/%s\n",
					LAN_IFCONFG_CMD, LAN_IF, ip_v6,
					prefix_v6);
					fputs_escape(buf, fp);
				}
			}
#endif

			fclose(fp);
		}
	}else{
		unlink(LAN_PATH);
	}	
	res = chmod(LAN_PATH,777);
	if(res){
		/*do nothing,just for compile*/
	}

	return 0;
}

void getLanIpMask( IpMask_p IpMask)
{
	char netmask_v4[16] = {0};
	struct in_addr ip_v4_addr;
	struct in_addr netmask_v4_addr;

	memset(IpMask->ip_v4, 0, sizeof(IpMask->ip_v4));
	memset(netmask_v4, 0, sizeof(netmask_v4));
	if(cfg_get_object_attr(LAN_ENTRY0_NODE, "IP", IpMask->ip_v4, sizeof(IpMask->ip_v4)) > 0  && cfg_get_object_attr(LAN_ENTRY0_NODE, "netmask", netmask_v4, sizeof(netmask_v4)) > 0)
	{
		if(IpMask->ip_v4[0] != '\0' && netmask_v4[0] != '\0')
		{
			if ( !check_mask_format(netmask_v4, IpMask->mask_dec, sizeof(IpMask->mask_dec)) )
			{
			    	snprintf(IpMask->mask_dec, sizeof(IpMask->mask_dec), "%d", 24);
			}
			inet_aton(IpMask->ip_v4, &ip_v4_addr);
			inet_aton(netmask_v4, &netmask_v4_addr);
			ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);
			snprintf(IpMask->ip_netmask_v4, sizeof(IpMask->ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));
		}
		else
		{
			snprintf(IpMask->ip_v4, sizeof(IpMask->ip_v4), "192.168.1.1");
			snprintf(IpMask->mask_dec, sizeof(IpMask->mask_dec), "%d", 24);
			snprintf(IpMask->ip_netmask_v4, sizeof(IpMask->ip_netmask_v4), "192.168.1.0");
		}
	}
}

void updateIpRouteRuleTable(void)
{	
	int i = 0;
	int pvcIndex = 0;
	int entryIndex = 0;
	int pvcName[64] = {0};
	int pvcEntryName[64] = {0};
	char attrName[32] = {0};
	char Active[32] = {0};
	char ipVer[32] = {0};
	char ISP[32] = {0};
	IpMask_t IpMask;
	char cmd[256] = {0};
	char buf[16] = {0};

	struct portBind_map portBindData[] = 
	{
	    {"LAN1",        "eth0.1",   0},
	    {"LAN2",        "eth0.2",   0},
	    {"LAN3",        "eth0.3",   0},
	    {"LAN4",        "eth0.4",   0},

	    {"SSID1",       "ra0",       0},
	    {"SSID2",       "ra1",       0},
	    {"SSID3",       "ra2",       0},
	    {"SSID4",       "ra3",       0},
	    {"SSID5",       "ra4",       0},
	    {"SSID6",       "ra5",       0},
	    {"SSID7",       "ra6",       0},
	    {"SSID8",       "ra7",       0},

	    {"SSIDAC1",     "rai0",     0},
	    {"SSIDAC2",     "rai1",     0},
	    {"SSIDAC3",     "rai2",     0},
	    {"SSIDAC4",     "rai3",     0},
	    {"SSIDAC5",     "rai4",     0},
	    {"SSIDAC6",     "rai5",     0},
	    {"SSIDAC7",     "rai6",     0},
	    {"SSIDAC8",     "rai7",     0}
	};

	memset(&IpMask, 0, sizeof(IpMask));
	getLanIpMask( &IpMask);
	
	for(pvcIndex = 0; pvcIndex < MAX_WAN_PVC_NUMBER; pvcIndex++)
	{
		memset(pvcName, 0, sizeof(pvcName));
		snprintf(pvcName, sizeof(pvcName), WAN_PVC_NODE, pvcIndex+1);
		if (cfg_obj_query_object(pvcName, NULL, NULL) > 0)
		{
			for(entryIndex = 0; entryIndex < MAX_WAN_ENTRY_NUMBER; entryIndex++)
			{
				memset(pvcEntryName, 0, sizeof(pvcEntryName));
				snprintf(pvcEntryName, sizeof(pvcEntryName), WAN_PVC_ENTRY_NODE, pvcIndex+1, entryIndex+1);
				if (cfg_obj_query_object(pvcEntryName, NULL, NULL) > 0)
				{
					memset(Active, 0, sizeof(Active));
					memset(ipVer, 0, sizeof(ipVer));
					memset(ISP, 0, sizeof(ISP));
					cfg_get_object_attr(pvcEntryName, "Active", Active, sizeof(Active));
					cfg_get_object_attr(pvcEntryName, "IPVERSION", ipVer, sizeof(ipVer));
					cfg_get_object_attr(pvcEntryName, "ISP", ISP, sizeof(ISP));
						
					if( !strcmp(Active, "Yes")  && strstr(ipVer, "IPv4") && strcmp(ISP, "3"))
					{
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s/%s via %s dev br0 table %d", IpMask.ip_netmask_v4, IpMask.mask_dec, IpMask.ip_v4, 100 + pvcIndex*MAX_WAN_PVC_NUMBER + entryIndex );	
						system_escape(cmd);

						for(i = 0; i < sizeof(portBindData)/sizeof(portBindData[0]); i++)
						{
							if( 0 == portBindData[i].lanBindStatus)
							{
								if(cfg_get_object_attr(pvcEntryName, portBindData[i].bindif, buf, sizeof(buf)) > 0 &&  !strcmp(buf, "Yes"))
								{
									portBindData[i].lanBindStatus = 1;
								}
							}
						}
					}
				}
			}
		}		
	}

	for(i = 0; i < sizeof(portBindData)/sizeof(portBindData[0]); i++)
	{
		if( 1 == portBindData[i].lanBindStatus)
		{
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s/%s via %s dev br0 table %s", IpMask.ip_netmask_v4, IpMask.mask_dec, IpMask.ip_v4, portBindData[i].realif);	
			system_escape(cmd);
		}
	}

	return 0;
}

#if defined(TCSUPPORT_CLMP_NG)
char old_lan_ip[32] = {0};
char old_lan_mask[32] = {0};

static void restart_upnpd(void)
{
	char new_lan_ip[32] = {0};
	char new_lan_mask[32] = {0};
	char upnp_state[16] = {0};

	cfg_obj_get_object_attr(UPNPD_ENTRY_NODE, "Active", 0, upnp_state, sizeof(upnp_state));
	cfg_obj_get_object_attr(LAN_ENTRY_NODE, "IP", 0, new_lan_ip, sizeof(new_lan_ip));
	cfg_obj_get_object_attr(LAN_ENTRY_NODE, "netmask", 0, new_lan_mask, sizeof(new_lan_mask));
	if(0 != strcmp(new_lan_ip, old_lan_ip) || 0 != strcmp(new_lan_mask, old_lan_mask))
	{
		strncpy(old_lan_ip, new_lan_ip, sizeof(old_lan_ip) - 1);
		strncpy(old_lan_mask, new_lan_mask, sizeof(old_lan_mask) - 1);
		if(0 == strcmp(upnp_state, "Yes"))
		{
			system("killall -9 upnpd");
			sleep(3);
			system("/userfs/bin/upnpd &");
		}
	}
}
#endif


int svc_other_handle_event_lan_execute(char* path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	
	char  tmp[32];

#if defined(TCSUPPORT_CMCC)
	cfg_node_type_t* node = NULL;
	
	node = cfg_type_query(path);
	
	if ( node && strcmp(node->name, "BSR") == 0 ) {

		return svc_other_bsr_control(path);
	}
#endif

	system(LAN_PATH);
#ifdef TCSUPPORT_DMS
	/*Restart DMS for IP may be changed*/
	cfg_commit_object(DMS_NODE);
#endif

#ifdef CT_COM_DEVICEREG
	cfg_commit_object(DEVICEACCOUNT_ENTRY_NODE);
#endif

#if defined(TCSUPPORT_IGMP_PROXY)
	/*restart IGMP proxy when LAN interface change*/
	if(cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "Active", tmp, sizeof(tmp)) > 0){
		if(strcmp(tmp,"Yes") == 0){
			cfg_commit_object(IGMPPROXY_NODE);
		}
	}
#endif

#ifdef TCSUPPORT_IGMP_SNOOPING	
	svc_other_operateIgmpSnooping();
#endif
#if defined(TCSUPPORT_CT_BRIDGEARP_NOFWD_LAN)
	cfg_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_ARP_RULE_CHG, (void*)&param, sizeof(param));
#endif
	if(cfg_get_object_attr(LAN_DHCP_NODE, "type", tmp, sizeof(tmp)) > 0){
		if(!strcmp(tmp,"0")){/*0:disable*/
			kill_process("/var/run/udhcpd.pid");
			kill_process("/var/log/dhcrelay.pid");
		}
#if defined(TCSUPPORT_CT_UBUS)
		else if ( !strcmp(tmp,"1") )
		{
			/* restart dnsmasq */
			cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1");
		}
#endif

	}else{
		return -1;
	}

#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
	svc_other_setBridgeMacLimit();
#endif

#if defined(TCSUPPORT_CT_UBUS)
	/* restart dnsfilter when LAN interface changed */
	cfg_commit_object(DNSFILTER_NODE);
#endif

	memset(tmp, 0, sizeof(tmp));
	if(cfg_get_object_attr(LAN_ENTRY0_NODE, "IP", tmp, sizeof(tmp)) > 0  &&  strcmp(tmp, LanIP))
	{
		updateIpRouteRuleTable();
		strncpy(LanIP, tmp, sizeof(LanIP)-1);
	}
	
#if defined(TCSUPPORT_CLMP_NG) 
	restart_upnpd();	
#endif

	
	return 0;
}

int svc_other_handle_event_lan_boot(char* path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	
	FILE *startupSh=NULL;

	memset(LanIP, 0, sizeof(LanIP));
	if(cfg_get_object_attr(LAN_ENTRY0_NODE, "IP", LanIP, sizeof(LanIP)) < 0)
	{
		strncpy(LanIP, "192.168.1.1", sizeof(LanIP)-1);
	}

	startupSh=fopen("/etc/lanconfig.sh","r");
	if(startupSh){
		fclose(startupSh);
		system("chmod 755 /etc/lanconfig.sh");
		system("/etc/lanconfig.sh");
	}
	else{
		system("/bin/ifconfig br0 192.168.1.1");
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
		system("/usr/bin/accesslimitcmd laninfo br0 192.168.1.1 255.255.255.0");
#endif
	}
#ifdef TCSUPPORT_IGMP_SNOOPING	
	svc_other_operateIgmpSnooping();
#endif
#if defined(TCSUPPORT_CT_BRIDGEARP_NOFWD_LAN)
	system("ebtables -N bridgeArp_chain");
	system("ebtables -P bridgeArp_chain RETURN");
	system("ebtables -I FORWARD 1 -j bridgeArp_chain");
	system("ebtables -I INPUT 1 -j bridgeArp_chain");
	cfg_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_ARP_RULE_CHG, (void*)&param, sizeof(param));
#endif

	svc_other_check_route("br0"); /*todo function check_route with only one param is not found, except two parameters.*/
#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
	svc_other_setBridgeMacLimit();
#endif

#if defined(TCSUPPORT_CLMP_NG)
	cfg_obj_get_object_attr(LAN_ENTRY_NODE, "IP", 0, old_lan_ip, sizeof(old_lan_ip));
	cfg_obj_get_object_attr(LAN_ENTRY_NODE, "netmask", 0, old_lan_mask, sizeof(old_lan_mask));
#endif

#if defined(TCSUPPORT_CMCC)
	return svc_other_bsr_control(LAN_BSR_NODE);
#endif

	return 0;
}


#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
#if defined(TCSUPPORT_CT_JOYME4)
#define USB_NODE_F "" \
"[global]\n" \
"netbios name = MyGroup\n" \
"server string = Samba Server\n" \
"workgroup = SambaSvr\n" \
"security = user\n" \
"%s" \
"guest account = %s\n" \
"log file = /var/log.samba\n" \
"socket options = TCP_NODELAY SO_RCVBUF=262144 SO_SNDBUF=131072\n" \
"passdb backend = smbpasswd\n" \
"encrypt passwords = yes\n" \
"use sendfile = yes\n" \
"use spne go = no\n" \
"client use spnego = no\n" \
"disable spoolss = yes\n" \
"smb passwd file = /etc/samba/smbpasswd\n" \
"host msdfs = no\n" \
"strict allocate = No\n" \
"os level = 20\n" \
"log level = 1\n" \
"max log size = 50\n" \
"max protocol = NT1\n" \
"null passwords = yes\n" \
"mangling method = hash\n" \
"dos charset = ASCII\n" \
"unix charset = UTF8\n" \
"display charset = UTF8\n" \
"fstype = Samba\n" \
"map to guest = %s\n" \
"\n"

#define USB_FOLDER_F "" \
"[%s]\n" \
"path = %s\n" \
"browseable = Yes\n" \
"writable = Yes\n" \
"create mask = 0664\n" \
"directory mask = 0775\n" \
"\n"

int create_smb_conf(char *guestName, int anonymous)
{
	FILE *fp = NULL;
	char nodeName[64] = {0};
	char mnt_dir[32] = {0}, str_usbcnt[16] = {0};
	char *p_folder_name = NULL;
	int i = 0, usb_cnt = 0;

	if( cfg_obj_get_object_attr(USBMOUNT_COMMON_NODE, "number"
			, 0, str_usbcnt, sizeof(str_usbcnt)) > 0 )
		usb_cnt = atoi(str_usbcnt);

	fp = fopen(SAMBA_FILE_PATH, "w");
	if ( fp )
	{
		/* write global config. */
		fprintf(fp, USB_NODE_F
							, (1 == anonymous ? "" : "restrict anonymous = 2\n")
							, guestName, (1 == anonymous ? "bad user" : "/ /"));

		/* write usb folder. */
		for ( i = 0; i < usb_cnt; i++ )
		{
			bzero(nodeName, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), USBMOUNT_ENTRY_NODE, i + 1);		
			if( cfg_obj_get_object_attr(nodeName, "filepath", 0, mnt_dir, sizeof(mnt_dir)) <= 0)
			{
				/*tcdbg_printf("\r\nget filepath fail");*/
				continue;
			}

			p_folder_name = strrchr(mnt_dir, '/');
			if ( NULL == p_folder_name )
				continue;

			fprintf(fp, USB_FOLDER_F, p_folder_name + 1, mnt_dir);
		}

		fclose(fp);
	}
	else
		return -1;

	return 0;
}

#endif

int svc_other_handle_event_samba_write(int flag, char* path)
{
	int i = 0;
	char cmd[512] = {0};
	char userName[129] = {0};
	char Passwd[129] = {0};
	char nodeName[64] = {0};
	char Active[4] = {'\0'};
	FILE *samba_script=NULL,  *fppwd = NULL;
	char pwd_buf[129] = {0}; 
	char *cpwd = NULL;
	char guestName[129] = {0};
	int samba_entry_exist = 0;
	char smbdcmd[64] = {0}, nmbdcmd[64] = {0};
	char Right[4] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
#define USER3_NORMAL_PRIV3 3 == i ? ACCOUNT_NORMAL_USER_STRING :
#else
#define USER3_NORMAL_PRIV3 
#endif

	fppwd = fopen(WEBPASSWD_PATH, "w");
	if(fppwd == NULL)
	{
		printf("\r\nsamba_write:create file(%s) fail!", WEBPASSWD_PATH);
		return -1;
	}
	
	samba_script=fopen(SAMBA_EXE_PATH, "w");
	if(samba_script == NULL)
	{
		printf("\r\nsamba_write:create file(%s) fail!", SAMBA_EXE_PATH);
		fclose(fppwd);
		
		return -1;
	}

	/* rm /etc/samba/smbpasswd  */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "rm -rf /etc/samba/smbpasswd\n");
	fputs_escape(cmd, samba_script);
	
	for(i=1; i<=3; i++)
	{
		snprintf(nodeName,sizeof(nodeName),ACCOUNT_ENTRY_NODE,i);

		if ( cfg_get_object_attr(nodeName, "Active", Active, sizeof(Active)) <= 0 
			|| 0 != strcmp(Active,"Yes") )
		{
			continue;
		}

		if(cfg_get_object_attr(nodeName, "username", userName, sizeof(userName)) < 0){
			continue;
		}
		memset(Passwd, 0, sizeof(Passwd));
		if(cfg_get_object_attr(nodeName, "web_passwd", Passwd, sizeof(Passwd)) < 0){
			continue;
		}
		if(1 == i){			
			memset(guestName, 0, sizeof(guestName));			
			snprintf(guestName, sizeof(guestName), userName);			
			string_tolower(guestName);		
		}
		
		memset(pwd_buf, 0, sizeof(pwd_buf));
		cpwd = crypt(Passwd, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf),  USER3_NORMAL_PRIV3 ACCOUNT_DEFAULT_STRING, userName, cpwd);
		F_PUTS_NORMAL(pwd_buf, fppwd);
	}

#if defined(TCSUPPORT_CT_JOYME4)
	/* add telnet root account for su command. */
	if ( cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "Active", Active, sizeof(Active)) > 0
		&& 0 == strcmp(Active, "Yes") )
	{
		bzero(userName, sizeof(userName));
		if ( cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_username", userName, sizeof(userName)) <= 0
			|| 0 == userName[0] )
		{
			snprintf(userName, sizeof(userName), "%s", "telnetadmin_super");
			cfg_set_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_username", userName);
		}

		bzero(Passwd, sizeof(Passwd));
		if ( cfg_get_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_passwd", Passwd, sizeof(Passwd)) <= 0
			|| 0 == Passwd[0] )
		{
			snprintf(Passwd, sizeof(Passwd), "%s", "telnetadmin");
			cfg_set_object_attr(ACCOUNT_TELNETENTRY_NODE, "telnet_admin_passwd", Passwd);
		}
	
		cpwd = crypt(Passwd, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, userName, cpwd);
		F_PUTS_NORMAL(pwd_buf, fppwd);
	}
#else /* remove duplicated root account in /etc/passwd file */
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, ACCOUNT_ENTRY1_NODE, sizeof(nodeName) - 1);
	memset(userName, 0, sizeof(userName));
	if(cfg_get_object_attr(nodeName, "username", userName, sizeof(userName)) >= 0)
	{
		i = 0;
		while(userName[i]!='\0')
		{
			userName[i] = tolower(userName[i]);
			i++;
		}
	}
	memset(Passwd, 0, sizeof(Passwd));
	if(cfg_get_object_attr(nodeName, "web_passwd", Passwd, sizeof(Passwd)) >= 0)
	{
		memset(pwd_buf, 0, sizeof(pwd_buf));
		cpwd = crypt(Passwd, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, userName, cpwd);
		F_PUTS_NORMAL(pwd_buf, fppwd);
	}
#endif
	
	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SAMBA_COMMON_NODE, sizeof(nodeName)-1);
	memset(Passwd,0,sizeof(Passwd));
	if(cfg_get_object_attr(nodeName, "Anonymous", Passwd, sizeof(Passwd)) > 0 
		&& strcmp(Passwd,"TRUE") == 0){
#if defined(TCSUPPORT_CT_JOYME4)
		create_smb_conf(guestName, 1);
#else
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "%s %s %s %s %s %s\n", SAMBA_CMD, "MyGroup", "SambaSvr", guestName, "bad", "user");
		fputs(cmd, samba_script);
#if !defined(TCSUPPORT_CT_JOYME2)
		memset(cmd, 0, sizeof(cmd));	
		snprintf(cmd, sizeof(cmd), "%s %s %s %s %s %s\n", SAMBA_ADD_DIR_CMD, "Samba_Share", "/", "Yes", "Yes", "Yes");	
		fputs(cmd, samba_script);
#endif
#endif

	}
	else{
#if defined(TCSUPPORT_CT_JOYME4)
		create_smb_conf(guestName, 0);
#else
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "%s %s %s %s %s %s\n", SAMBA_CMD, "MyGroup", "SambaSvr", guestName, "/","/");
		fputs(cmd, samba_script);
#if !defined(TCSUPPORT_CT_JOYME2)
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "%s %s %s %s %s %s\n", SAMBA_ADD_DIR_CMD, "Samba_Share", "/", "Yes", "Yes", "No");	
		fputs(cmd, samba_script);
#endif
#endif
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	for(i = 1; i <= 6; i++)
	{
		snprintf(nodeName, sizeof(nodeName), SAMBA_ENTRY_NODE, i);
		if(cfg_get_object_attr(nodeName, "Active", Active, sizeof(Active)) < 0 
			|| (strcmp(Active,"Yes") != 0)){
			continue;
		}
		memset(userName, 0, sizeof(userName));
		if(cfg_get_object_attr(nodeName, "username", userName, sizeof(nodeName)) < 0){
			continue;
		}
		memset(Passwd, 0, sizeof(Passwd));
		if(cfg_get_object_attr(nodeName, "Passwd", Passwd, sizeof(Passwd)) < 0){
			continue;
		}
#if !defined(TCSUPPORT_CT_JOYME4)
		string_tolower(userName);
		string_tolower(Passwd);
#endif	
		memset(pwd_buf, 0, sizeof(pwd_buf));
		cpwd = crypt(Passwd, SLAT);
		snprintf(pwd_buf, sizeof(pwd_buf), ACCOUNT_DEFAULT_STRING, userName, cpwd);
		F_PUTS_NORMAL(pwd_buf, fppwd);
		
		samba_entry_exist = 1;
		
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "%s %s %s\n", SAMBA_PASSWD_CMD, userName, Passwd);
		fputs_escape(cmd, samba_script);
	}

#if defined(TCSUPPORT_CT_JOYME2)
    if(0 == samba_entry_exist
        || cfg_get_object_attr(SAMBA_COMMON_NODE, "Right", Right, sizeof(Right)) <= 0
        || strcmp(Right,"1") == 0) //only enable local use web[userbane/password]
    {		
    	memset(userName, 0, sizeof(userName));
    	memset(Passwd, 0, sizeof(Passwd));
    	memset(cmd, 0, sizeof(cmd));
    	if((cfg_get_object_attr(ACCOUNT_ENTRY2_NODE, "username", userName, sizeof(userName)) > 0)
    		&& (cfg_get_object_attr(ACCOUNT_ENTRY2_NODE, "web_passwd", Passwd, sizeof(Passwd)) > 0))
    	{
#if !defined(TCSUPPORT_CT_JOYME4)
    		string_tolower(userName);
    		string_tolower(Passwd);
#endif
    		snprintf(cmd, sizeof(cmd), "%s %s %s\n", SAMBA_PASSWD_CMD, userName, Passwd);
    	}	
    	fputs_escape(cmd, samba_script);	
	}
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	snprintf(smbdcmd, sizeof(smbdcmd), "taskset 0x8 %s", SAMBA_SMBD_CMD);
	snprintf(nmbdcmd, sizeof(nmbdcmd), "taskset 0x8 %s", SAMBA_NMBD_CMD);
#else
	snprintf(smbdcmd, sizeof(smbdcmd), SAMBA_SMBD_CMD);
	snprintf(nmbdcmd, sizeof(nmbdcmd), SAMBA_NMBD_CMD);
#endif

	fputs(nmbdcmd, samba_script);
	fputs(smbdcmd, samba_script);

	/* enlarge dirty ratio / dirty writeback centisecs to reduce flushing frequency*/
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "echo 30 > /proc/sys/vm/dirty_background_ratio\n");
	fputs(cmd, samba_script);

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "echo 60 > /proc/sys/vm/dirty_ratio\n");
	fputs(cmd, samba_script);

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "echo 1000 > /proc/sys/vm/dirty_writeback_centisecs\n");
	fputs(cmd, samba_script);

	fclose(samba_script);
	fclose(fppwd);
	
	return 0;
}

int svc_other_handle_event_samba_execute(char* path)
{
	char nodeName[64] = {0};
	char Right[4] = {0};
	int dir_res = 0;
	FILE *fp = NULL;
	int ret = 0;
	
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, SAMBA_COMMON_NODE, sizeof(nodeName)-1);

	if(cfg_get_object_attr(nodeName, "Right", Right, sizeof(Right)) <= 0){
		strncpy(Right, "1", sizeof(Right)-1);
	}

	/*kill samba related process firstly*/
	if (access(SAMBA_NMBD_PID_PATH, 0) == 0)
	{
		kill_process(SAMBA_NMBD_PID_PATH);
	}
	if (access(SAMBA_SMBD_PID_PATH, 0) == 0)
	{
		system(SAMBA_KILL_SMBD_CMD);
	}
	
	/*delete iptables*/
	//drop WAN
	system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j DROP");
	system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j DROP");
	
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j DROP");
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j DROP");
	
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j DROP");
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j DROP");
	
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j DROP");
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j DROP");
	//drop LAN
	system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i br+ -j DROP");
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i br+ -j DROP");
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i br+ -j DROP");
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i br+ -j DROP");
	//drop ALL
    system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -j DROP");
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -j DROP");
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -j DROP");
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -j DROP");
	//accept ALL
	system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -j ACCEPT");
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -j ACCEPT");
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -j ACCEPT");
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -j ACCEPT");
	//accept WAN
	system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j ACCEPT");
	system("iptables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j ACCEPT");
	
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j ACCEPT");
	system("iptables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j ACCEPT");
	
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j ACCEPT");
	system("ip6tables -t filter -D INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j ACCEPT");
	
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j ACCEPT");
	system("ip6tables -t filter -D INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j ACCEPT");
	
    if(strcmp(Right,"0") == 0) //close remote and local
    {
        system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -j DROP");
	    system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -j DROP");
	    
	    system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -j DROP");
	    system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -j DROP");
    }
	if(strcmp(Right,"1") == 0) //close remote, open local
	{
		system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j DROP");
		system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j DROP");
	    
		system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j DROP");
		system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j DROP");
	    
		system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j DROP");
		system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j DROP");
	    
		system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j DROP");
		system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j DROP");
	}
	if(strcmp(Right,"2") == 0) //open remote, close local
	{
		system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j ACCEPT");
		system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j ACCEPT");

		system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j ACCEPT");
		system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j ACCEPT");

		system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i nas+ -j ACCEPT");
		system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i ppp+ -j ACCEPT");
		
		system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i nas+ -j ACCEPT");
		system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i ppp+ -j ACCEPT");
	    
		system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i br+ -j DROP");
	    system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i br+ -j DROP");
	    
	    system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -i br+ -j DROP");
	    system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -i br+ -j DROP");
	    cfg_commit_object(FIREWALL_ENTRY_NODE); //let firewall rule at the last
	}
	if(strcmp(Right,"3") == 0) //open remote and local
	{
		system("iptables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -j ACCEPT");
	    system("iptables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -j ACCEPT");
	    
	    system("ip6tables -t filter -A INPUT -p TCP -m multiport --dport 139,389,445,901 -j ACCEPT");
	    system("ip6tables -t filter -A INPUT -p UDP -m multiport --dport 137,138 -j ACCEPT");
	    cfg_commit_object(FIREWALL_ENTRY_NODE); //let firewall rule at the last
	}

	
	if(strcmp(Right,"0") != 0)
	{
		fp = fopen( SAMBA_EXE_PATH, "r" );
		if( NULL != fp)
		{
			dir_res = chmod(SAMBA_EXE_PATH, 777);
			
			system(SAMBA_EXE_PATH);	
			
			ret = blapi_perform_samba_optimize(1);
			
			fclose(fp);
			unlink(SAMBA_EXE_PATH);
		}
	}

	return 0;
}

int svc_other_handle_event_samba_boot(char* path)
{
#ifdef TCSUPPORT_USBHOST	
	system("/usr/script/usb_insmod.sh");
#endif
	svc_other_handle_event_samba_write(SAMBA_SERVER_SAMBA_COMMIT_FLAG, path);
	return svc_other_handle_event_samba_execute(NULL);
}
#endif

 
void dowscd(char* path)
{
	 char value[4];
	 char lan_ip[32] = {0};
	 char wscdpid[128]={0};
	 char upnpd_stat[8]={0};

	 system("killall -9 wscd");
	 system("/sbin/route del -net 239.0.0.0 netmask 255.0.0.0 dev br0");
	 if(cfg_get_object_attr(UPNPD_ENTRY_NODE, NODE_ACTIVE, upnpd_stat, sizeof(upnpd_stat)) > 0)
	 {
		 if(strcmp(upnpd_stat, ACTIVE) != 0)
		 {
			 return;
		 }	 
	 }	 
	  
	if(cfg_get_object_attr(WLAN_ENTRY1_NODE, "WPSConfMode", value, sizeof(value)) > 0)
	{
		 if(atoi(value)!=0)
		 {
			system("/sbin/route add -net 239.0.0.0 netmask 255.0.0.0 dev br0");
			 if(cfg_get_object_attr(LAN_ENTRY0_NODE, LAN_IP, lan_ip, sizeof(lan_ip)) > 0)
			 {
				snprintf(wscdpid, sizeof(wscdpid), "/usr/bin/wscd -i ra0 -a %s -m 1 &",lan_ip);
				system(wscdpid);
			 }
		 }
	}

#if defined(TCSUPPORT_WLAN_AC)
	if(cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, "WPSConfMode", value, sizeof(value)) > 0)
	{ 
		 if(atoi(value)!=0)
		 {
		 	system("/sbin/route add -net 239.0.0.0 netmask 255.0.0.0 dev br0");
			if(cfg_get_object_attr(LAN_ENTRY0_NODE, LAN_IP, lan_ip, sizeof(lan_ip)) > 0)
			 {	 
				 memset(wscdpid, 0, sizeof(wscdpid));
				 snprintf(wscdpid, sizeof(wscdpid), "/usr/bin/wscd -i rai0 -a %s -m 1 &",lan_ip);
				 system(wscdpid);
			 }
		 }
	}
#endif

	 return;
}


#if defined(TCSUPPORT_SIMPLE_UPNP)
int write_upnp_conf(char *path)
{
	char tmp[64]={0};
	char Wan_if[16]={0}, default_route[16]={0}, active[8]={0}, autoconf[2]={0}, igd[8]={0};
	int i=0 , fd = -1;
	int defRouteIndex = -1;
	char serviceApp[SERVICE_LIST_LEN] = {0};
	char nodePath[64] = {0};
	
	unlink(UPNPD_PATH);
	snprintf(nodePath, sizeof(nodePath),UPNPD_ENTRY_NODE);

	/*query upnp attribute value*/
	if(cfg_get_object_attr(nodePath, "autoconf", autoconf, sizeof(autoconf)) < 0 || cfg_get_object_attr(path, NODE_ACTIVE, igd, sizeof(igd)) < 0)
	{
		return FAIL;
	}
	/*if deactive, remove configure file*/
	if(!strcmp(igd, DEACTIVE))
	{
		unlink(UPNPD_PATH);
		return SUCCESS;
	}
	else
	{/*If user actived upnpd service, the igd.conf must be set "enable"  strings not "Yes". */
		strncpy(igd, UPNPD_ACTIVE,sizeof(igd)-1);
	}

	snprintf(nodePath, sizeof(nodePath),WANINFO_COMMON_NODE);
	cfg_get_object_attr(nodePath, "DefRouteIndexv4", default_route, sizeof(default_route));
	if(strlen(default_route) != 0 && strncmp(default_route, "N/A", 3))
	{
		defRouteIndex = atoi(default_route);
		memset(active, 0, sizeof(active));
		memset(Wan_if, 0, sizeof(Wan_if));
		if(get_waninfo_by_index(defRouteIndex, "Active", active, sizeof(active)) == SUCCESS)
		{
			if(!strcmp(active, "Yes"))
			{
				get_waninfo_by_index(defRouteIndex, WAN_IFNAME, Wan_if, sizeof(Wan_if));
			}
		}
	}
	
	/*no pvc active default route, find the first actived Internet interface*/
	if((strlen(default_route) == 0) || (!strncmp(default_route, "N/A", 3)) || (strlen(Wan_if) == 0))
	{
		for(i = 0; i < MAX_WAN_IF_INDEX; i++)
		{
			memset(active, 0, sizeof(active));
			memset(Wan_if, 0, sizeof(Wan_if));
			if(get_waninfo_by_index(i, "Active", active, sizeof(active)) == SUCCESS && get_waninfo_by_index(i, "ServiceList", serviceApp, sizeof(serviceApp)) == SUCCESS)
			{
				if(!strcmp(active, "Yes") &&
#if defined(TCSUPPORT_CT_SERVICELIST_E8C) 
					(strstr(serviceApp, INTERNET)!=NULL)){
#else
					(!strcmp(serviceApp, INTERNET) || !strcmp(serviceApp, TR069_INTERNET))){
#endif
					get_waninfo_by_index(i, WAN_IFNAME, Wan_if, sizeof(Wan_if));
					break;
				}
			}
		}
	}
	/*write config file*/
	fd = open(UPNPD_PATH,O_WRONLY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
	if(fd != -1){
		snprintf(tmp, sizeof(tmp), "autoconf=1\n");
		write(fd,tmp,strlen(tmp));
		snprintf(tmp, sizeof(tmp), "igd=%s\n",igd);
		write(fd,tmp,strlen(tmp));
		snprintf(tmp, sizeof(tmp), "lanif=br0\n");
		write(fd,tmp,strlen(tmp));
		snprintf(tmp, sizeof(tmp), "wanif=%s\n",Wan_if);
		write(fd,tmp,strlen(tmp));
		snprintf(tmp, sizeof(tmp), "nat=1\n");
		write(fd,tmp,strlen(tmp));
		close(fd);
	}

	return SUCCESS;
}
#endif


int svc_other_handle_event_upnpd_write(char* path)
{
	int retval=0;
	
#if defined(TCSUPPORT_SIMPLE_UPNP)
	retval = write_upnp_conf(path);
#endif

	return retval;
}

int svc_other_handle_event_upnpd_boot(void)
{	
	char active[8] = {0};
#if defined(TCSUPPORT_SIMPLE_UPNP)
	FILE *startupSh=NULL;
	system("iptables -t nat -N UPNP_PRE");
	system("iptables -t nat -A PREROUTING -j UPNP_PRE");
	startupSh=fopen("/etc/igd/igd.conf","r");
	if(startupSh)
	{
		fclose(startupSh);
		system("/userfs/bin/upnpd &");
	}
#endif
#if defined(TCSUPPORT_CLMP_NG)
	cfg_obj_get_object_attr("root.upnpd.entry.1", "Active", 0, active, sizeof(active));
	if ( strcmp(active, "Yes") == 0 )
		system("/userfs/bin/upnpd &");
#endif
	return SUCCESS;
}

int svc_other_handle_event_upnpd_execute(char* path)
{
	char BlockFlag[8]= {0};
	char BlockIPstart[32]= {0};
	char BlockIPend[32]= {0};
	char BlockUpnpIP[32]= {0};
	char BlockUpnpPort[8]= {0};
	char BlockSpeed[8]= {0};
	char BlockSpeedBurst[8]= {0};
	char iptblcmd[256] = {0};
#if defined(TCSUPPORT_SIMPLE_UPNP)
	char upnpd_stat[8]={0};
	cfg_get_object_attr(path, NODE_ACTIVE, upnpd_stat, sizeof(upnpd_stat));
	kill_process(UPNPD_PID_PATH);
	if(strcmp(upnpd_stat, ACTIVE) == 0){
		system(UPNPD_CMD);
	}
	else
	{
		unlink(UPNPD_PATH);
	}
#endif
	
#if defined(TCSUPPORT_CLMP_NG)
	char active[8]= {0};
   
   cfg_obj_get_object_attr("root.upnpd.entry.1", "Active", 0, active, sizeof(active));
   system("killall -9 upnpd");
   if ( strcmp(active, "Yes") == 0 )
   {
	   system("/userfs/bin/upnpd &");
   }
	cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockFlag", 0, BlockFlag, sizeof(BlockFlag));
	if( strncmp(BlockFlag, "Yes", 3) == 0)
	{
		cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockIPStart", 0, BlockIPstart, sizeof(BlockIPstart));   
		cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockIPEnd", 0, BlockIPend, sizeof(BlockIPend));
		cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockUpnpIP", 0, BlockUpnpIP, sizeof(BlockUpnpIP));
		cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockUpnpPort", 0, BlockUpnpPort, sizeof(BlockUpnpPort));
		cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockSpeed", 0, BlockSpeed, sizeof(BlockSpeed));
		cfg_obj_get_object_attr("root.upnpd.entry.1", "BlockSpeedBurst", 0, BlockSpeedBurst, sizeof(BlockSpeedBurst));

		memset(iptblcmd, 0, sizeof(iptblcmd));
		snprintf(iptblcmd, sizeof(iptblcmd), 
		"iptables -A OUTPUT -p tcp -m iprange --src-range %s-%s -d %s --dport %s -m limit --limit=%s/s --limit-burst=%s -j ACCEPT",
		BlockIPstart, BlockIPend, BlockUpnpIP,BlockUpnpPort, BlockSpeed, BlockSpeedBurst);
		tcdbg_printf("Command is %s\n", iptblcmd);
		system(iptblcmd);
	}

#endif

#ifdef TCSUPPORT_WLAN
#ifdef WSC_AP_SUPPORT
		dowscd(path);
#endif
#endif

	/*reset firewall */
	sleep(1);
	cfg_commit_object(FIREWALL_NODE);   

	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2)
static unsigned int getRandNumber()
{
	unsigned int i = 0, j =0;

	srand((unsigned int)time(NULL));

	/* return a value in range 1 ~ 5 */
	return (unsigned int)(5.0 * rand() / (RAND_MAX + 1.0)) + 1;
}

static void getNowFormatTime(char *ptime, int len)
{
	int year, mon, day, hour, min, sec = 0;
	struct timespec curtime;
	struct tm nowTime;
	char now_time[20] = {0};

	clock_gettime(CLOCK_REALTIME, &curtime);
	localtime_r(&curtime.tv_sec, &nowTime);
	year = nowTime.tm_year + 1900;
	mon = nowTime.tm_mon + 1;
	day = nowTime.tm_mday;
	hour = nowTime.tm_hour;
	min = nowTime.tm_min;
	sec = nowTime.tm_sec;

	snprintf(now_time, sizeof(now_time), "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
	strncpy(ptime, now_time, len);
	return;
}

static int parse_IpAddr_Port(char *ip_buf, char *port_buf, char *ipaddr, char *port, int ip_len, int port_len)
{
	struct in_addr in_s_v4addr = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL;
	struct sockaddr_in *tmp_addr  = NULL;

	if ( NULL == ip_buf 
		|| NULL == port_buf 
		|| NULL == ipaddr 
		|| NULL == port )
		return -1;
	
	/* get ipaddress */
	if ( 1 != inet_pton(AF_INET, ip_buf, &in_s_v4addr) ) {
		bzero(&hints, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		/* parse domain */
		if( 0 != getaddrinfo(ip_buf, NULL, &hints, &res) ) {
			return -1;
		}
		if ( !res ) {
			return -1;
		}
		if( !res->ai_addr ) {
			freeaddrinfo(res);
			return -1;
		}
		tmp_addr = (struct sockaddr_in *)res->ai_addr;
		snprintf(ipaddr, ip_len, "%s", (char *)inet_ntoa(tmp_addr->sin_addr));
		freeaddrinfo(res);
	}
	else {
		/* ip */
		snprintf(ipaddr, ip_len, "%s", ip_buf);
	}

	/* get port */
	if ( 0 == strlen(port_buf) )
		strncpy(port, "80", port_len - 1);
	else
		strncpy(port, port_buf, port_len - 1);	

	return 0;
}

static int config_httpdownload_speedtest(int sync_conn_num)
{
	char nodeName[64] = {0}, cmdbuf[1200] = {0}, test_ip[1024] = {0}, test_port[8] = {0};
	char dest_ip[1024] = {0}, dest_port[8] = {0}, file_name[1024] = {0}, host[1024] = {0}, action[1024] = {0};
	char urltotal[4] = {0};
	int i = 0, j = 0, cnt = 0, conn_num = 0;

	tcdbg_printf("%s: enter <============\n", __FUNCTION__);
	if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "HttpTestUrlTotal", 0, urltotal, sizeof(urltotal)) <= 0 )
		conn_num = sync_conn_num;
	else {
		conn_num = atoi(urltotal);
		if ( 0 == conn_num || conn_num > 4 )
			conn_num = sync_conn_num;
		else if ( 3 == conn_num )
			conn_num = 2;
		else /* conn_num = 1,2,4 */
			conn_num = 4/conn_num;
	}

	/* set DestIP, DestPort, Host and Action */
	cnt = 0;
	for (i = 0; i < 4; i++) {
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), SPEEDTEST_ENTRY_NODE, i + 1);
		bzero(dest_ip, sizeof(dest_ip));
		bzero(dest_port, sizeof(dest_port));
		bzero(file_name, sizeof(file_name));
		if ( cfg_obj_get_object_attr(nodeName, "DestIP", 0, dest_ip, sizeof(dest_ip)) <= 0 
			|| cfg_obj_get_object_attr(nodeName, "DestPort", 0, dest_port, sizeof(dest_port)) < 0 
			|| cfg_obj_get_object_attr(nodeName, "FileName", 0, file_name, sizeof(file_name)) <= 0 ) {
			continue;
		}

		/* get test ip and port */
		bzero(test_ip, sizeof(test_ip));
		bzero(test_port, sizeof(test_port));
		if ( 0 != parse_IpAddr_Port(dest_ip, dest_port, test_ip, test_port, sizeof(test_ip), sizeof(test_port)) )
			return -1;
		bzero(host, sizeof(host));
		bzero(action, sizeof(action));
		snprintf(host, sizeof(host), "%s:%s", test_ip, test_port);
		snprintf(action, sizeof(action), "GET /%s %s %s", file_name, test_ip, test_port);

		for (j = 0; cnt < 8 && j < conn_num; j++) {
			/* Dest IP */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "destip=%s", test_ip);
			SPEED_TEST_TASK(cmdbuf);
			/* Dest Port */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "destport=%s", test_port);
			SPEED_TEST_TASK(cmdbuf);
			/* Host */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "host=%s", host);
			SPEED_TEST_TASK(cmdbuf);
			/* Action. */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "action=%s", action);
			SPEED_TEST_TASK(cmdbuf);
			
			cnt++;
			usleep( 50 * 1000 );
		}
	}

	if ( 0 == cnt )
		return -1;
	
	return 0;
}

static int config_ftpdownload_speedtest()
{
	char nodeName[64] = {0}, cmdbuf[1200] = {0}, test_ip[1024] = {0}, test_port[8] = {0};
	char linkmode[4] = {0}, ifname[8] = {0}, gateway[16] = {0}, netmask[16] = {0}, duration[16] = {0};
	char dest_ip[1024] = {0}, dest_port[8] = {0}, passive_mode[2] = {0}, file_name[1024] = {0};
	char ftp_user[1024] = {0}, ftp_passwd[1024] = {0}, ftp_port[8] = {0};

	tcdbg_printf("%s: enter <============\n", __FUNCTION__);
	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), SPEEDTEST_ENTRY_NODE, 4 + 1);
	
	/* set ftp user and pwd */
	bzero(ftp_user, sizeof(ftp_user));
	bzero(ftp_passwd, sizeof(ftp_passwd));
	cfg_obj_get_object_attr(nodeName, "UserName", 0, ftp_user, sizeof(ftp_user));
	cfg_obj_get_object_attr(nodeName, "Password", 0, ftp_passwd, sizeof(ftp_passwd));

	/* get PassiveMode, default value:0 - no passive mode */
	bzero(passive_mode, sizeof(passive_mode));
	if ( cfg_obj_get_object_attr(nodeName, "PassiveMode", 0, passive_mode, sizeof(passive_mode) ) <= 0 )
		strncpy(passive_mode, "0", sizeof(passive_mode) - 1);

	/* set DestIP, DestPort, Action */
	bzero(dest_ip, sizeof(dest_ip));
	bzero(dest_port, sizeof(dest_port));
	bzero(file_name, sizeof(file_name));
	if ( cfg_obj_get_object_attr(nodeName, "DestIP", 0, dest_ip, sizeof(dest_ip)) <= 0 
		|| cfg_obj_get_object_attr(nodeName, "DestPort", 0, dest_port, sizeof(dest_port)) < 0 
		|| cfg_obj_get_object_attr(nodeName, "FileName", 0, file_name, sizeof(file_name)) <= 0 )
		return -1;

	/* get test ip and port */
	bzero(test_ip, sizeof(test_ip));
	bzero(test_port, sizeof(test_port));
	/* use default port when dest port is not set */
	if ( 0 == dest_port[0] )
		strncpy(dest_port, "21", sizeof(dest_port) - 1);
	if ( 0 != parse_IpAddr_Port(dest_ip, dest_port, test_ip, test_port, sizeof(test_ip), sizeof(test_port)) )
		return -1;
	
	/* Dest IP */
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "destip=%s", test_ip);
	SPEED_TEST_TASK(cmdbuf);
	/* Dest Port */
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "destport=%d", test_port);
	SPEED_TEST_TASK(cmdbuf);
	/* wget cmd */
	bzero(cmdbuf, sizeof(cmdbuf));
	if ( 0 == strcmp("0", passive_mode) ) {
		if ( '\0' == ftp_user[0] ) {
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/wget -b ftp://%s/%s --no-passive-ftp"
					, test_ip, file_name);
		} 
		else if ( '\0' == ftp_passwd[0] ) {
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/wget -b ftp://%s/%s --ftp-user=%s --no-passive-ftp"
					, test_ip, file_name, ftp_user);
		}
		else {
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/wget -b ftp://%s/%s --ftp-user=%s --ftp-password=%s --no-passive-ftp"
					, test_ip, file_name, ftp_user, ftp_passwd);
		}
	}
	else {
		if ( '\0' == ftp_user[0] ) {
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/wget -b ftp://%s/%s"
					, test_ip, file_name);
		} 
		else if ( '\0' == ftp_passwd[0] ) {
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/wget -b ftp://%s/%s --ftp-user=%s"
					, test_ip, file_name, ftp_user);
		}
		else {
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/wget -b ftp://%s/%s --ftp-user=%s --ftp-password=%s"
					, test_ip, file_name, ftp_user, ftp_passwd);
		}
	}

	system(cmdbuf);
	return 0;
}

static int config_postman_speedtest(int sync_conn_num)
{
	char nodeName[64] = {0}, attrName[64] = {0}, cmdbuf[1200] = {0}, test_ip[1024] = {0}, test_port[8] = {0};
	char dest_ip[1024] = {0}, dest_port[8] = {0}, file_name[1024] = {0}, action[1200] = {0};
	char ticket[81] = {0}, speed_type[2] = {0}, urltotal[4] = {0};
	int i = 0, j = 0, cnt = 0, url_cnt = 0, conn_num = 0;

	tcdbg_printf("%s: enter <============\n", __FUNCTION__);
	bzero(ticket, sizeof(ticket));
	bzero(speed_type, sizeof(speed_type));
	if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "Ticket", 0, ticket, sizeof(ticket)) <= 0 
		|| cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "SpeedType", 0, speed_type, sizeof(speed_type)) <= 0 )
		return -1;
	
	/* Set Action */
	cnt = 0;
	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), SPEEDTEST_ENTRY_NODE, 5 + 1);
	if ( cfg_obj_get_object_attr(nodeName, "UrlTotal", 0, urltotal, sizeof(urltotal)) <= 0 )
		conn_num = sync_conn_num;
	else {
		conn_num = atoi(urltotal);
		if ( 0 == conn_num || conn_num > 4 )
			conn_num = sync_conn_num;
		else if ( 3 == conn_num )
			conn_num = 2;
		else /* conn_num = 1,2,4 */
			conn_num = 4/conn_num;
	}
	
	for ( i = 0; i < 4; i++ ) {
		/* DestIP & DestPort */
		bzero(dest_ip, sizeof(dest_ip));
		bzero(dest_port, sizeof(dest_port));
		bzero(attrName, sizeof(attrName));
		snprintf(attrName, sizeof(attrName), "DestIP%d", i + 1);
		if ( cfg_obj_get_object_attr(nodeName, attrName, 0, dest_ip, sizeof(dest_ip)) <= 0 )
			continue;
		snprintf(attrName, sizeof(attrName), "DestPort%d", i + 1);
		cfg_obj_get_object_attr(nodeName, attrName, 0, dest_port, sizeof(dest_port));
		
		/* get test ip and port */
		bzero(test_ip, sizeof(test_ip));
		bzero(test_port, sizeof(test_port));
		if ( 0 != parse_IpAddr_Port(dest_ip, dest_port, test_ip, test_port, sizeof(test_ip), sizeof(test_port)) )
			return -1;
			
		/* get filename */
		bzero(attrName, sizeof(attrName));
		bzero(file_name, sizeof(file_name));
		snprintf(attrName, sizeof(attrName), "FileName%d", i + 1);
		cfg_obj_get_object_attr(nodeName, attrName, 0, file_name, sizeof(file_name));
		
		for (j = 0; cnt < 8 && j < conn_num; j++) {
			/* Dest IP */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "destip=%s", test_ip);
			SPEED_TEST_TASK(cmdbuf);
			/* Dest Port */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "destport=%s", test_port);
			SPEED_TEST_TASK(cmdbuf);
			
			/* SpeedType */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "speedtype=%s", speed_type);
			SPEED_TEST_TASK(cmdbuf);

			/* Ticket */
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "ticket=%s", ticket);
			SPEED_TEST_TASK(cmdbuf);
			
			/* Action */
			bzero(action, sizeof(action));
			snprintf(action, sizeof(action), "POSTMAN /%s %s %s", (0 != strcmp(file_name, "N/A") && '\0' != file_name[0]) ? file_name : "100M", test_ip, test_port);
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "action=%s", action);
			SPEED_TEST_TASK(cmdbuf);
			cnt++;
			usleep( 50 * 1000 );
		}
	}

	if ( 0 == cnt )
		return -1;

	return 0;
}

void svc_other_handle_event_speedtest_start()
{
	int ret = -1, i = 0, test_duration = 0, buf_len = 0;
	unsigned int weight = 0;
	int error_state = 0, def_route_wan_idx = 0, isppp = 0;
	unsigned long dw_peak_rate = 0, dw_avg_rate = 0, up_peak_rate = 0, up_avg_rate = 0, packet_cnt = 0, error_no = 0;
	char buf[MAX_BUF_SIZE] = {0}, item[16] = {0};
	char value[MAX_LEN_ULONG + 1] = {0};
	char *pbuf = NULL, *pitem = NULL, *pstate = NULL;
	char event_data[256] = {0}, dbus_if[2] = {0}, packet[11] = {0}, result[257] = {0};
	char action_type[8] = {0}, duration_end[8] = {0};
	char nodeName[64] = {0}, def_route_wan[8] = {0}, cmdbuf[512] = {0};
	char linkmode[8] = {0}, ifname[8] = {0}, gateway[16] = {0}, netmask[16] = {0}, duration[16] = {0};
	char sync_conn_num[2] = {0}, cur_time[20] = {0};
	char obj_path[64] = {0}, itf_name[64] = {0};
	FILE *fp = NULL;
	size_t size = 0;
	ssize_t res = 0;
	speed_test_result_t test_result;

	tcdbg_printf("%s: enter <============\n", __FUNCTION__);
	/* reset all connection. */
	SPEED_TEST_TASK("reset");

	/* get default route wan ipv4 */
	bzero(def_route_wan, sizeof(def_route_wan));
	if ( cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", 0, def_route_wan, sizeof(def_route_wan)) < 0 ) {
		goto startHttpUploadDiagnostics_end;
	}
	def_route_wan_idx = atoi(def_route_wan);

	/* get default route wan gateway & netmask */
	bzero(nodeName, sizeof(nodeName));
	bzero(gateway, sizeof(gateway));
	bzero(netmask, sizeof(netmask));
	snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, def_route_wan_idx + 1);
	if ( cfg_obj_get_object_attr(nodeName, "GateWay", 0, gateway, sizeof(gateway)) <= 0 
		|| cfg_obj_get_object_attr(nodeName, "NetMask", 0, netmask, sizeof(netmask)) <= 0 ) {
		goto startHttpUploadDiagnostics_end;
	}

	/* get wan type and name */
	bzero(nodeName, sizeof(nodeName));
	bzero(linkmode, sizeof(linkmode));
	bzero(ifname, sizeof(ifname));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, def_route_wan_idx/8 + 1, def_route_wan_idx%8 + 1);	
	if ( cfg_obj_get_object_attr(nodeName, "LinkMode", 0, linkmode, sizeof(linkmode)) <= 0 
		|| cfg_obj_get_object_attr(nodeName, "IFName", 0, ifname, sizeof(ifname)) <= 0 ) {
		goto startHttpUploadDiagnostics_end;
	}

	/* Wan LinkMode. */
	if ( strstr(linkmode, "PPP") ) {
		isppp = 1;
		SPEED_TEST_TASK("ppp");
	}
	else {
		SPEED_TEST_TASK("ip");
	}

	/* Wan IFName. */
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "wandev=%s", ifname);
	SPEED_TEST_TASK(cmdbuf);

	/* GateWay & Mask. */
	if (!isppp) {
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "gateway=%s", gateway);
		SPEED_TEST_TASK(cmdbuf);
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "mask=%s", netmask);
		SPEED_TEST_TASK(cmdbuf);
	}

	/* stop download after duration */
	if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "DurationEnd", 0, duration_end, sizeof(duration_end)) > 0 
		&& 0 == strcmp(duration_end, "1") )
		SPEED_TEST_TASK("duration_end=1");

	/* Duration */
	bzero(cmdbuf, sizeof(cmdbuf));
	bzero(duration, sizeof(duration));
	if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "Duration", 0, duration, sizeof(duration)) <= 0 )
		strncpy(duration, "30", sizeof(duration) - 1);
	snprintf(cmdbuf, sizeof(cmdbuf), "duration=%s", duration);
	SPEED_TEST_TASK(cmdbuf);

	/* set connection paramters */
	bzero(sync_conn_num, sizeof(sync_conn_num));
	if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "SyncConnNum", 0, sync_conn_num, sizeof(sync_conn_num)) <= 0 )
		strncpy(sync_conn_num, "1", sizeof(sync_conn_num) - 1);

	/* get speedtest interface and action type => config speedtest data */
	bzero(dbus_if, sizeof(dbus_if));
	if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "DbusIF", 0, dbus_if, sizeof(dbus_if)) < 0 )
		strncpy(dbus_if, "0", sizeof(dbus_if) - 1);

	if (0 == strcmp(dbus_if, "0")) {
		/* HttpDownloadTest:check action type */
		bzero(action_type, sizeof(action_type));
		if ( cfg_obj_get_object_attr(SPEEDTEST_COMMON_NODE, "ActionType", 0, action_type, sizeof(action_type)) <= 0 ) {
			goto startHttpUploadDiagnostics_end;
		}

		if ( !strcasecmp(action_type, "http") )
			ret = config_httpdownload_speedtest(atoi(sync_conn_num));
		else if ( !strcasecmp(action_type, "ftp") )
			ret = config_ftpdownload_speedtest();
	}
	else if (0 == strcmp(dbus_if, "1")) {
		/* SpeedTestFF */
		ret = config_postman_speedtest(atoi(sync_conn_num));
	}

	if ( ret < 0 )
		goto startHttpUploadDiagnostics_end;

	/* set testing state */
	cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "TestingStatus", "true");
	bzero(cur_time, sizeof(cur_time));
	getNowFormatTime(cur_time, sizeof(cur_time) - 1);
	cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "BEGIN_TIME", cur_time);
	
	/* send event to dbus */
	bzero(obj_path, sizeof(obj_path));
	bzero(itf_name, sizeof(itf_name));
	strncpy(obj_path, IGD_PATH"/Diagnostics/HttpDownload", sizeof(obj_path) - 1);
	if ( 0 == strcmp( dbus_if, "1") ) {
		bzero(event_data, sizeof(event_data));
		strncpy(itf_name, IGD_NAME".SpeedTestFF", sizeof(itf_name) - 1);
		snprintf(event_data, sizeof(event_data), "%s;%s;TestingStatus:b:%s", obj_path, itf_name, "1");
		ret = ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, event_data, strlen(event_data) + 1);
	}
	else if ( 0 == strcmp( dbus_if, "0") )
	{
		strncpy(itf_name, IGD_NAME".HttpDownloadTest", sizeof(itf_name) - 1);
	}

#if 0
	/* wait until download complete in duration time */
	test_duration = atoi(duration);
	bzero(result, sizeof(result));
	while (test_duration--){
		usleep(1 * 1000 * 1000);
		bzero(buf, sizeof(buf));
		fileRead(SPEED_TEST_PATH, buf, MAX_BUF_SIZE);
		if ( 0 != strlen(buf) ) {
			/* get all session test len */
			bzero(packet, sizeof(packet));
			pbuf = strstr(buf, SPEEDTEST_PACKET_CNT);
			if ( pbuf ) {
				bzero(value, sizeof(value));
				strncpy(value, pbuf + strlen(SPEEDTEST_PACKET_CNT), MAX_LEN_ULONG);
				packet_cnt = strtoul(value, NULL, 10);
				snprintf(packet, sizeof(packet), "%lu", packet_cnt/1024);
			}
			else{
				strncpy(packet, "0", sizeof(packet) - 1);
			}
		}
		else{
			strncpy(packet, "0", sizeof(packet) - 1);
		}
		buf_len += snprintf(result + buf_len, sizeof(result) - buf_len, "%s,", packet);
	}
	result[strlen(result) - 1] = '\0';
#endif
	/* wait until download complete in duration time */
	usleep(atoi(duration) * 1000 * 1000);
	/* get speedtest receive total size, read file max times:3 */
	bzero(result, sizeof(result));
	ret = blapi_perf_get_speedtest_result_time(result, sizeof(result));
	if( ret == 0)
	{
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "Result", result);
	}	
	else
	{
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "Result", "0");
	}		

	bzero(cur_time, sizeof(cur_time));
	getNowFormatTime(cur_time, sizeof(cur_time) - 1);
	cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "END_TIME", cur_time);
	
	/* get speedtest result */
	if (-1 == blapi_perf_get_speedtest_result(&test_result)) {
		if ( 0 == strcmp(dbus_if, "0") )
			snprintf(event_data, sizeof(event_data), "%s;%s;Status:u:%s;Result:s:%s", obj_path, itf_name, error_state ? "1" : "0", result);
		else
			snprintf(event_data, sizeof(event_data), "%s;%s;TestingStatus:b:%s", obj_path, itf_name, "0");
		/* set testing state */
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "TestingStatus", "false");
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "Status", "1");
	} else {
		if ( 0 == strcmp(dbus_if, "0") )
			snprintf(event_data, sizeof(event_data), "%s;%s;Status:u:%s;Result:s:%s", obj_path, itf_name, "1", "0");
		else
			snprintf(event_data, sizeof(event_data), "%s;%s;TestingStatus:b:%s", obj_path, itf_name, "0");
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "TestingStatus", "false");	
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "Status", test_result.error_state);
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "DW_SPEED_MAX_RATE", test_result.dw_max_rate);
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "DW_SPEED_RATE", test_result.dw_rate);
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "UP_SPEED_RATE", test_result.up_rate);
		cfg_set_object_attr(SPEEDTEST_COMMON_NODE, "UP_SPEED_MAX_RATE", test_result.up_max_rate);
	}
	bzero(buf, sizeof(buf));
	
	ret = ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, event_data, strlen(event_data) + 1);
	
startHttpUploadDiagnostics_end:
	tcdbg_printf("%s: leave <============\n", __FUNCTION__);

	pthread_exit(0);
}

int creat_other_cfg_speedtest_pthread()
{
	pthread_t thread_id;

	tcdbg_printf("%s: enter <============\n", __FUNCTION__);
	if (pthread_create(&thread_id, NULL, (void *)svc_other_handle_event_speedtest_start, NULL) < 0)
	{
		printf("creat_other_cfg_speedtest_pthread: create thread fail \n");
		return -1;
	}
	tcdbg_printf("%s: leave <============\n", __FUNCTION__);

	return 0;
}
#endif
#if defined(TCSUPPORT_CT_JOYME4)
int svc_other_handle_event_bridgedata_update(void)
{
	int i = 0;
	char nodeName[64] = {0};
	char IPBuf[32] = {0}, PortBuf[8] = {0}, ProtocolBuf[8] = {0};
	unsigned int ip = 0, port = 0, protocol = 0;
	unsigned int ipv4_add[4] = {0};
	
	/* delete all hwnat info first */
	blapi_traffic_delete_all_black_ip_port_protocol();

	/* Add current info to hwnat */
	for( i = 0; i < MAX_BRIDGEDATAACL_NUM; i++ )
	{
		memset(nodeName, 0, sizeof(nodeName));
		memset(IPBuf, 0, sizeof(IPBuf));
		memset(PortBuf, 0, sizeof(PortBuf));
		memset(ProtocolBuf, 0, sizeof(ProtocolBuf));
		snprintf(nodeName, sizeof(nodeName), BRIDGEDATAACL_ENTRY_NODE, i + 1);
		if( cfg_obj_get_object_attr(nodeName, "IP", 0, IPBuf, sizeof(IPBuf)) <= 0 )
			continue;
		
		cfg_obj_get_object_attr(nodeName, "Port", 0, PortBuf, sizeof(PortBuf));
		cfg_obj_get_object_attr(nodeName, "Protocol", 0, ProtocolBuf, sizeof(ProtocolBuf));
		port = atoi(PortBuf);
		if( 0 == port )
			port = 65536;

		if( !strcmp(ProtocolBuf, "TCP") )
			protocol = 1;
		else if( !strcmp(ProtocolBuf, "UDP") )
			protocol = 2;
		else if( !strcmp(ProtocolBuf, "ICMP") )
			protocol = 3;
		else
			protocol = 4;

		sscanf(IPBuf, "%d.%d.%d.%d", &ipv4_add[0],  &ipv4_add[1],  &ipv4_add[2],  &ipv4_add[3]);
		ip = ipv4_add[0]<<24 | ipv4_add[1]<<16 | ipv4_add[2]<<8 | ipv4_add[3];

		blapi_traffic_add_black_ip_port_protocol(ip, port, protocol);
	}
	return 0;
}
#endif

#if defined(TCSUPPORT_CUC)
int svc_other_handle_event_speedtest_execute(char* path)
{
	tcdbg_printf("%s: enter path=[%s]<============\n", __FUNCTION__, path);
	
	system("killall -9 rms_speedtest");
	if ( strstr(path, "httpentry") )
		system("/userfs/bin/rms_speedtest httpentry &");
	else if ( strstr(path, "rmsentry") )
		system("/userfs/bin/rms_speedtest rmsentry &");
	else
		return -1;
	
	return 0;
}
#endif

#define SYSLOG_VALUE_LEN	8
#define SYSLOG_CONF_PATH	"/var/log/log.conf"

int syslog_write(void)
{
	int i = 0;
	cfg_node_type_t *node = NULL;
	char tmp[8] = {0};
	FILE *fp = NULL;
	char buf[32] = {0};

	enum syslog_param 
	{
		WRITE_LEVEL,
		WRITE_INTERVAL,
		UPDATE_INTERVAL,			
  		SYSLOG_PARAM_NUM,
	};
	
	char syslog_param[][32]=
	{
		{"WriteLevel"},
		{"WriteFlashInterval"},
		{"UpdateInterval"},					
		{""},
	};
	char param_value[SYSLOG_PARAM_NUM][SYSLOG_VALUE_LEN];
		
	node = cfg_type_query(SYSLOG_ENTRY_NODE);
	if(node != NULL)
	{
		for( i = 0; i < SYSLOG_PARAM_NUM; i++) 
		{
			memset(param_value[i], 0, SYSLOG_VALUE_LEN);
			cfg_obj_get_object_attr(SYSLOG_ENTRY_NODE, syslog_param[i], 0, tmp, sizeof(tmp));
			if(tmp[0] != '\0')
				strncpy(param_value[i], tmp, SYSLOG_VALUE_LEN);
			else
				printf("\n==>syslog_write error, get %s value failed\n", syslog_param[i]);
		}
	}
	else
		return FAIL;

	/* Write syslog config file "/var/log/syslog.conf"
  	* e.g.
  	* WriteLevel 3
	*/
	fp = fopen(SYSLOG_CONF_PATH, "w");
	if(fp != NULL) {
		for( i = 0; i < SYSLOG_PARAM_NUM; i++) 
		{
			snprintf(buf, sizeof(buf), "%s %s\n", syslog_param[i], param_value[i]);
			fputs_escape(buf, fp);
		}
		fclose(fp);
	}
	else
		return FAIL;

	return SUCCESS;
}

int svc_other_handle_event_syslog_boot(void)
{
	char buf[8]; 
	char nodeName[64] = {0};

	syslog_write();
	
	memset(buf, 0, sizeof(buf));
	memset(nodeName, 0, sizeof(nodeName));
	
	strncpy(nodeName, SYSLOG_ENTRY_NODE, sizeof(nodeName)-1);
	
	if (cfg_obj_get_object_attr(nodeName, "LogEnable", 0, buf, sizeof(buf)) < 0)
	{
		return -1;
	}
	if(!strncmp(buf,"Yes", sizeof(buf)))
	{
		system("/sbin/syslogd -m 0 &");
		system("/sbin/klogd &");

#if defined(TCSUPPORT_CT_JOYME4)
		sleep(2);

		if( access("/opt/upt/apps/info/reset_syslog", 0) 
			&& access("/opt/upt/apps/info/upgrade_syslog", 0) 
			&& access("/opt/upt/apps/info/reboot_syslog", 0) )
		{
			openlog("TCSysLog alarm", 0, LOG_LOCAL2);
			syslog(LOG_ALERT, "id:104001 Device reboot from Power\n");
			closelog();
		}
		else if( !access("/opt/upt/apps/info/reboot_syslog", 0) )
		{
			memset(buf, 0, sizeof(buf));
			fileRead("/opt/upt/apps/info/reboot_syslog", buf, MAX_BUF_SIZE);
			if(!strcmp(buf, "Terminal"))
			{
				openlog("TCSysLog alarm", 0, LOG_LOCAL2);
				syslog(LOG_ALERT, "id:104001 Device reboot from Terminal\n");
				closelog();
			}
			
		}

		openlog("TCSysLog alarm", 0, LOG_LOCAL2);
		syslog(LOG_INFO, "Device Boot\n");
		closelog();
		AddAlarmNumber("104001");
#endif
	}
#if defined(TCSUPPORT_CT_JOYME4)
	unlink("/opt/upt/apps/info/reboot_syslog");
	unlink("/opt/upt/apps/info/reset_syslog");
	unlink("/opt/upt/apps/info/upgrade_syslog");
#endif
	
	return 0;
}

int svc_other_handle_event_syslog_update(char *path){

	char buf[8] = {0};
	char nodeName[64] = {0};
	char cmd[128] = {0};
#if !defined(TCSUPPORT_CMCCV2)
	char clealogbuf[16] = {0};
#endif

	syslog_write();

	/* First, kill the syslogd process */
	system("killall -9 syslogd");
	system("killall -9 klogd");
	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYSLOG_ENTRY_NODE, sizeof(nodeName)-1);
		
#if !defined(TCSUPPORT_CMCCV2)
	if( cfg_obj_get_object_attr(nodeName, "clearLog", 0, clealogbuf, sizeof(clealogbuf)) > 0
		&& !strcmp(clealogbuf, "Yes") )
	{
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD,(unsigned long)SYSLOG_RA_OFFSET, RESERVEAREA_NAME);
		system(cmd);
	
		cfg_set_object_attr(nodeName, "clearLog", "");
	}
#endif
	
	if (cfg_obj_get_object_attr(nodeName, "LogEnable", 0, buf, sizeof(buf)) < 0)
	{
		return FAIL;
	}
	if(!strncmp(buf, "Yes", sizeof(buf)))/* If syslog is enable, restart the syslogd process */
	{
		unlink("/var/log/messages");
		unlink("/var/log/currLogFile");
#if defined(TCSUPPORT_CMCCV2)
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd, sizeof(cmd), TC_FLASH_ERASE_SECTOR_CMD,(unsigned long)SYSLOG_RA_OFFSET, RESERVEAREA_NAME);
		system(cmd);
#endif
		system("/sbin/syslogd -m 0 &");
		system("/sbin/klogd &");
	}
	else /*syslog disable*/
	{
		system("killall -9 syslogd");
		system("killall -9 klogd");	

		unlink("/var/log/messages");
		unlink("/var/log/currLogFile");	
	}
		
	return SUCCESS;
}
