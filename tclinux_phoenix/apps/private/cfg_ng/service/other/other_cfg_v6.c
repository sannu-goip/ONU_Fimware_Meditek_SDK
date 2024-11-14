

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
#include "other_mgr.h"
#include "other_cfg_v6.h"
#include <unistd.h>
#include <sys/stat.h>
#include <net/if.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include "utility.h"
#include "cfg_msg.h"


static int get_run_pd_index();
static int getRaWanPDInterface(char *path);
static int create_br0_v6addr(char *pd, int pd_len, char *newV6addr);
static int create_br0_pool_ipv6addr(char *nodeName, char *ipv6_addr);
static int lan_ipv6_prefix_gen(char * result_prefix, char *entry);
static int lan_ipv6_addr_gen(char * ipv6_addr_str, char * attr, char * prefix_str, char *path);
static int ipv6AddressTo01(char *Dest, char * Src);
static int ipv6AddressToFF(char *Dest, char *Src);

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
int getSkbMark(char *path)
{
	int skb_mark = -1;
	int IFIdx = 0;
	char wanIfIndex[32] = {0};
	char nodeName[32] = {0};
	char attrName[32] = {0};
	
	if( NULL != strstr(path, "radvd") )
		snprintf(attrName, sizeof(attrName), "DelegatedWanConnection");
	else
		snprintf(attrName, sizeof(attrName), "DNSWANConnection");
	
	if ( cfg_get_object_attr(path, attrName, wanIfIndex, sizeof(wanIfIndex)) > 0 && 0 != wanIfIndex[0] )
	{
		memset(nodeName, 0, sizeof(nodeName));

		IFIdx = atoi(wanIfIndex);
		skb_mark = (unsigned int)IFIdx << 24;
	}
	else if( strstr(path, "common") )
	{
		/* should use PDRUNIFIdx */
		IFIdx = getRaWanPDInterface(path);
		if ( IFIdx >= 0 )
			skb_mark = (unsigned int)IFIdx << 24;
	}

	return skb_mark;
}
#endif
int svc_other_handle_event_radvd_write(char *path)
{
	char ip_v6[40]={0},
		prefix_v6len[5]={0},enable_flag[5] = {0},
		preffered_lifetime[16]={0},valid_lifetime[16]={0},mode[5] = {0};	
	char dhcpv6_wan_pd_ptime[16] = {0}, dhcpv6_wan_pd_vtime[16] = {0};
#if defined(TCSUPPORT_CT_E8GUI)
	char managed_flag[5] = {0}, other_flag[5] = {0},max_raInterval[8] = {0},min_raInterval[8] = {0};
	char state_other[5] = {0}, state_managed[5] = {0};
#endif
	char state_auto[5] = {0};	
	char *tmp = NULL;
	char val[64]= {0};
	char buf[512]={0};
	int len = 0;
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)				
	char pd_index_v6[5] = {0};
	char new_prefix[45] = {0};	
	char nodeName[64] = {0};
	char status6[8] = {0};
	char temp_str[45] = {0};
	char *temp2_p = NULL;
	int pd_index = 0;
#endif
	int res = 0;
	char rdnss[90] = {0}, dns1_v6[40]={0}, dns2_v6[40]={0};
	int dnsType = 0;
	char configFile[64] = {0};
	int entry_idx = 0;
#if defined(TCSUPPORT_CT_VRWAN)
	char orgpd[128] = {0}, vrpd[128] = {0};
	int plen = 0;
#endif
#if defined(TCSUPPORT_NPTv6)
	char nptindex[16] = {0};
	int wanmask = 0;
#endif

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)	
	char nodeName_radvd[64] = {0};
	char nodeName_dhcp6s[64] = {0};
#endif

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)

	if(cfg_get_object_attr(path, "Type", val, sizeof(val)) > 0){			
		if(strstr(val, "VR") || strstr(val, "OTHER"))
			return 0;
	}

	if( NULL != strstr(path, "common") )
	{
		snprintf(configFile, sizeof(configFile), IPV6_CONFPATH);
	}
	else
	{
		sscanf(path, RADVD_ENTRY_NODE, &entry_idx);
		if( entry_idx < 0 )
			return -1;
		
		snprintf(configFile, sizeof(configFile), RADVD_VR_CONFPATH, entry_idx-1);
	}
#else
	snprintf(configFile, sizeof(configFile), IPV6_CONFPATH);
#endif

	if(cfg_query_object(path, NULL, NULL) > 0){	
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "PrefixIPv6", val, sizeof(val)) > 0){			
			strncpy(ip_v6, val, sizeof(ip_v6)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "Prefixv6Len", val, sizeof(val)) > 0){			
			strncpy(prefix_v6len, val, sizeof(prefix_v6len)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "Enable", val, sizeof(val)) > 0){			
			strncpy(enable_flag, val, sizeof(enable_flag)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "Mode", val, sizeof(val)) > 0){			
			strncpy(mode, val, sizeof(mode)-1);
		}

#if defined(TCSUPPORT_CT_E8GUI)
		/*set M/O flag*/
		strncpy(state_managed,"off", sizeof(state_managed)-1);
		strncpy(state_other, "on", sizeof(state_other)-1);
		strncpy(state_auto, "on", sizeof(state_auto)-1);
		strncpy(min_raInterval, "200", sizeof(min_raInterval)-1);
		strncpy(max_raInterval, "600", sizeof(max_raInterval)-1);

#if defined(TCSUPPORT_NPTv6)
		cfg_get_object_attr(RADVD_ENTRY_NODE, "npt_wan_idx", nptindex, sizeof(nptindex));
		if( 0 != nptindex[0] )
		{
			sscanf(nptindex, "%x", &wanmask);
			if( 0 != wanmask )
				cfg_set_object_attr(RADVD_ENTRY_NODE, "ManagedEnable", "0");
		}
#endif
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "ManagedEnable", val, sizeof(val)) > 0){			
			strncpy(managed_flag, val, sizeof(managed_flag)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "OtherEnable", val, sizeof(val)) > 0){			
			strncpy(other_flag, val, sizeof(other_flag)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "MinRAInterval", val, sizeof(val)) > 0){			
			strncpy(min_raInterval, val, sizeof(min_raInterval)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "MaxRAInterval", val, sizeof(val)) > 0){			
			strncpy(max_raInterval, val, sizeof(max_raInterval)-1);
		}
		
		if('1' == other_flag[0])
		{
			strncpy(state_other,"on", sizeof(state_other)-1);
		}
		else
		{
			strncpy(state_other,"off", sizeof(state_other)-1);
		}

		if('1' == managed_flag[0])
		{
			strncpy(state_managed,"on", sizeof(state_managed)-1);
			strncpy(state_auto, "off", sizeof(state_auto)-1);
		}
		else
		{
			strncpy(state_managed,"off", sizeof(state_managed)-1);
			strncpy(state_auto, "on", sizeof(state_auto)-1);
		}
#endif

		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "PreferredLifetime", val, sizeof(val)) > 0){			
			strncpy(preffered_lifetime, val, sizeof(preffered_lifetime)-1);
		}
		memset(val, 0, sizeof(val));
		if(cfg_get_object_attr(path, "ValidLifetime", val, sizeof(val)) > 0){			
			strncpy(valid_lifetime, val, sizeof(valid_lifetime)-1);
		}
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)	
		/*get rdnss info*/
		memset(rdnss, 0, sizeof(rdnss));
		memset(nodeName_radvd, 0, sizeof(nodeName_radvd));
		memset(nodeName_dhcp6s, 0, sizeof(nodeName_dhcp6s));

		snprintf(nodeName_radvd, sizeof(nodeName), RADVD_ENTRY_NODE, entry_idx);
		snprintf(nodeName_dhcp6s, sizeof(nodeName), DHCP6S_ENTRY_NODE, entry_idx);

		cfg_set_object_attr(nodeName_radvd, "RDNSS", "");
		if(cfg_query_object(nodeName_dhcp6s, NULL, NULL) > 0)
		{
			memset(val, 0, sizeof(val));
			if(cfg_get_object_attr(nodeName_dhcp6s, DHCP6S_DNSTYPE, val, sizeof(val)) > 0)
			{
				dnsType = atoi(val);
				if(1 == dnsType)
				{
					memset(val, 0, sizeof(val));
					memset(dns1_v6, 0, sizeof(dns1_v6));
					if ( cfg_get_object_attr(nodeName_dhcp6s, DNS1_SERVER, val, sizeof(val)) > 0 )
					{
						strncpy(dns1_v6, val, sizeof(dns1_v6)-1);
					}
					memset(val, 0, sizeof(val));
					memset(dns2_v6, 0, sizeof(dns2_v6));
					if ( cfg_get_object_attr(nodeName_dhcp6s, DNS2_SERVER, val, sizeof(val)) > 0 )
					{
						strncpy(dns2_v6, val, sizeof(dns2_v6)-1);
					}

					memset(rdnss, 0, sizeof(rdnss));
					snprintf(rdnss, sizeof(rdnss)-1, "%s %s", dns1_v6, dns2_v6);
					cfg_set_object_attr(nodeName_radvd, "RDNSS", rdnss);
				}
			}
		}	
#else
		/*get rdnss info*/
		memset(rdnss, 0, sizeof(rdnss));
		cfg_set_object_attr(RADVD_ENTRY_NODE, "RDNSS", "");
		if(cfg_query_object(DHCP6S_ENTRY_NODE, NULL, NULL) > 0)
		{
			memset(val, 0, sizeof(val));
			if(cfg_get_object_attr(DHCP6S_ENTRY_NODE, DHCP6S_DNSTYPE, val, sizeof(val)) > 0)
			{
				dnsType = atoi(val);
				if(1 == dnsType)
				{
					memset(val, 0, sizeof(val));
					memset(dns1_v6, 0, sizeof(dns1_v6));
					if ( cfg_get_object_attr(DHCP6S_ENTRY_NODE, DNS1_SERVER, val, sizeof(val)) > 0 )
					{
						strncpy(dns1_v6, val, sizeof(dns1_v6)-1);
					}
					memset(val, 0, sizeof(val));
					memset(dns2_v6, 0, sizeof(dns2_v6));
					if ( cfg_get_object_attr(DHCP6S_ENTRY_NODE, DNS2_SERVER, val, sizeof(val)) > 0 )
					{
						strncpy(dns2_v6, val, sizeof(dns2_v6)-1);
					}

					memset(rdnss, 0, sizeof(rdnss));
					snprintf(rdnss, sizeof(rdnss)-1, "%s %s", dns1_v6, dns2_v6);
					cfg_set_object_attr(RADVD_ENTRY_NODE, "RDNSS", rdnss);
				}
			}
		}	
#endif				
#if defined(TCSUPPORT_CT_E8GUI)
		snprintf(buf, sizeof(buf), "interface br0  \
		\n{\n\tAdvSendAdvert on; \
		\n\tAdvManagedFlag %s; \
		\n\tAdvOtherConfigFlag %s; \
		\n\tMinRtrAdvInterval %s; \
		\n\tMaxRtrAdvInterval %s; ",state_managed,state_other,min_raInterval,max_raInterval);
#endif
		 /*when mode == 1  mannual mode is enabled, prefix information is added into configuration
		     when mode == 0 auto mode is enabled	*/		
		if(mode[0] == '1')
		{
			len = strlen(buf);
			snprintf(buf+len, sizeof(buf)-len, "\n\tprefix %s/%s\n\t{ \
			\n\tAdvOnLink on; \
			\n\tAdvAutonomous %s; \
			\n\tAdvRouterAddr off; \
			\n\tAdvValidLifetime %s; \
			\n\t AdvPreferredLifetime %s; \
			\n\t};",
				ip_v6, prefix_v6len,state_auto,
				valid_lifetime,preffered_lifetime);
		}
#if defined(TCSUPPORT_CT_E8GUI) && defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
		else if('0' == mode[0]){
			/*Get prefix delegation from Wan interface which is default route*/
			memset(ip_v6, 0, sizeof(ip_v6));
			memset(prefix_v6len, 0, sizeof(prefix_v6len));

			pd_index = getRaWanPDInterface(path);
#if defined(TCSUPPORT_CT_VRWAN)
			if( pd_index < 0 )
			{
				len = strlen(buf);
				snprintf(buf+len, sizeof(buf)-len, "\n\tprefix fd00::/64\n\t{ \
				\n\tAdvOnLink on; \
				\n\tAdvAutonomous %s; \
				\n\tAdvRouterAddr off; \
				\n\tAdvValidLifetime 7200; \
				\n\t AdvPreferredLifetime 3600; \
				\n\t};",state_auto);
			}
#endif
			if ( pd_index >= 0 )
			{
				snprintf(pd_index_v6, sizeof(pd_index_v6), "%d", pd_index);
						
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName),WANINFO_ENTRY_NODE, pd_index + 1);

				memset(status6, 0, sizeof(status6));
				cfg_get_object_attr(nodeName, "Status6", status6,sizeof(status6));

				memset(temp_str, 0, sizeof(temp_str));			
				cfg_get_object_attr(nodeName, "PD6", temp_str,sizeof(temp_str));
							
				cfg_get_object_attr(nodeName, "PD6ptime", dhcpv6_wan_pd_ptime,sizeof(dhcpv6_wan_pd_ptime));
				cfg_get_object_attr(nodeName, "PD6vtime", dhcpv6_wan_pd_vtime,sizeof(dhcpv6_wan_pd_vtime));
				
				/*generate prefix for lan DHCP*/
				if(lan_ipv6_prefix_gen(new_prefix, pd_index_v6) == 0){
					strncpy(temp_str, new_prefix, sizeof(temp_str)-1);
				}
				
				tmp = strstr(temp_str, "/");				
				if((0 != strcmp(temp_str, ""))
					&& (tmp != NULL)
					&& !strcmp(status6, "up")){ 				
					/*Parse prefix and prefix-len from PD6,eg:3000:456:AEF:DCA::/64*/
					temp2_p = tmp+1;
					*tmp = '\0';
					
					strncpy(ip_v6, temp_str, sizeof(ip_v6)-1);
					strncpy(prefix_v6len, temp2_p, sizeof(prefix_v6len)-1);
					
					if ( '\0' != dhcpv6_wan_pd_ptime[0]
							&& '\0' != dhcpv6_wan_pd_vtime[0] )
					{
						strncpy(preffered_lifetime, dhcpv6_wan_pd_ptime, sizeof(preffered_lifetime)-1);
						strncpy(valid_lifetime, dhcpv6_wan_pd_vtime, sizeof(valid_lifetime)-1);
					}
					else
					{
						strncpy(preffered_lifetime, "3600", sizeof(preffered_lifetime)-1);
						strncpy(valid_lifetime, "7200", sizeof(valid_lifetime)-1);
					}
					
					len = strlen(buf);
					snprintf(buf+len, sizeof(buf)-len, "\n\tprefix %s/%s\n\t{ \
					\n\tAdvOnLink on; \
					\n\tAdvAutonomous %s; \
					\n\tAdvRouterAddr off; \
					\n\tAdvValidLifetime %s; \
					\n\t AdvPreferredLifetime %s; \
					\n\t};",
						ip_v6, prefix_v6len,state_auto,
						valid_lifetime,preffered_lifetime);
					
				}
#if defined(TCSUPPORT_CT_VRWAN)
				else
				{
					len = strlen(buf);
					snprintf(buf+len, sizeof(buf)-len, "\n\tprefix fd00::/64\n\t{ \
					\n\tAdvOnLink on; \
					\n\tAdvAutonomous %s; \
					\n\tAdvRouterAddr off; \
					\n\tAdvValidLifetime 7200; \
					\n\t AdvPreferredLifetime 3600; \
					\n\t};",state_auto);
				}
#endif
			}
		}				
#endif

		/*write RDNSS to config file*/
		if(1 == dnsType)
		{
			if(rdnss[0] != 0)
			{
				len = strlen(buf);
				snprintf(buf+len,sizeof(buf)-len,"\n\tRDNSS %s\n\t{\
				\n\tAdvRDNSSLifetime 1200;\
				\n\t};\n",rdnss);
			}
		}
		
		len = strlen(buf);
		snprintf(buf+len, sizeof(buf)-len, "\n};\n");

		if(write2file(buf,configFile) == -1)
		{
			tcdbg_printf("fail to write file /etc/radvd.conf\n");
			return -1;
		}

	
	}
	else{
		unlink(configFile);
	}	
	res = chmod(configFile,777);
	if(res){
		/*do nothing,just for compile*/
	}

	return 0;
}


int svc_other_handle_event_radvd_execute(char *path)
{
	char enable[3] = {0};
	char mode[5] = {0}, ipv6_br0[50] = {0}, cmd[128] = {0};
	char pid[8]={0};
	char configFile[64] = {0}, pidPath[64] = {0};
	int skb_mark = -1;
	int entry_idx = 0;
	char val[64]= {0};

	if(cfg_get_object_attr(path, "Enable", enable, sizeof(enable)) < 0)
	{
		return -1;
	}

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	if(cfg_get_object_attr(path, "Type", val, sizeof(val)) > 0){			
		if(strstr(val, "VR") || strstr(val, "OTHER"))
			return 0;
	}

	if( NULL != strstr(path, "common") )
	{
		snprintf(configFile, sizeof(configFile), IPV6_CONFPATH);
		snprintf(pidPath, sizeof(pidPath), "/var/run/radvd.pid");
	}
	else
	{
		sscanf(path, RADVD_ENTRY_NODE, &entry_idx);
		if( entry_idx < 0 )
			return -1;
		
		snprintf(configFile, sizeof(configFile), RADVD_VR_CONFPATH, entry_idx-1);
		snprintf(pidPath, sizeof(pidPath), RADVD_VR_PIDPATH, entry_idx-1);
	}
	skb_mark = getSkbMark(path);
#else
	snprintf(configFile, sizeof(configFile), IPV6_CONFPATH);
	snprintf(pidPath, sizeof(pidPath), "/var/run/radvd.pid");
#endif

	if(cfg_get_object_attr(path, BR0_ADDR, ipv6_br0, sizeof(ipv6_br0)) > 0 && strcmp(ipv6_br0, "N/A")){
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 del %s", ipv6_br0);
		system_escape(cmd);
		cfg_set_object_attr(path, BR0_ADDR, "N/A");
	}	

	fileRead(pidPath, pid, sizeof(pid));
	if(strlen(pid) > 0){
		kill(atoi(pid), SIGTERM);
		unlink(pidPath);
	}

	if ( enable[0] == '1' )  
	{
		if(cfg_get_object_attr(path, "Mode", mode, sizeof(mode)) < 0){
			return -1;
		}
		/*Manual mode*/
		if(mode[0] == '1'){ 
			if(create_br0_pool_ipv6addr(path, ipv6_br0) == 0){
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 %s", ipv6_br0);
				system(cmd);
				/*save br0 address*/
				cfg_set_object_attr(path, BR0_ADDR, ipv6_br0);
			}
		}
		memset(cmd, 0, sizeof(cmd));
		
		if( 0 > skb_mark )
			skb_mark = 0;
		
		snprintf(cmd, sizeof(cmd), "/userfs/bin/radvd -C %s -M %d -p %s &", configFile, skb_mark, pidPath);

		system(cmd);
	}

	else if( '0' == enable[0] )
	{
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
		unlink(configFile);
#endif
		tcdbg_printf("radvd call closed by exec!\n");
	}

	return 0;
}


int svc_other_handle_event_radvd_boot(char *path)
{
	return svc_other_handle_event_radvd_execute(path);
}

void set_dnsv6_Relay_domain_name_server(char* buf, int size)
{
	char dnsRelayPrimary[64] = {0};
	char dnsRelaySecond[64] = {0};

	cfg_get_object_attr(DHCP6S_DNSRELAY_NODE, "Primary_DNS", dnsRelayPrimary, sizeof(dnsRelayPrimary));
	cfg_get_object_attr(DHCP6S_DNSRELAY_NODE, "Secondary_DNS", dnsRelaySecond, sizeof(dnsRelaySecond));

	if(dnsRelayPrimary[0] != '\0' && dnsRelaySecond[0] != '\0')
	{
		snprintf(buf, size, "option domain-name-servers %s  %s;\n", dnsRelayPrimary, dnsRelaySecond);
	}
	else
	{
		snprintf(buf, size, "option domain-name-servers %s;\n", dnsRelayPrimary);
	}

	return;
}

void set_domain_name_server(char* buf, int size, char mode[],int isvrentry, char* path)
{
	char tmp[128]= {0};
	char dns1_v6[64]={0};
	char dns2_v6[64]={0};
	char domain_name[68] = {0};
	char dns_type[8] = {0};
	char dns_source[8] = {0};
	int ifidx = 0;
	int dnsType = 0;
	char status6[8] = {0};
	char nodeName[64] = {0};	
	char defrouteidx[8] = {0};

	/*Manual mode*/ 
	if(mode[0] == '1')
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(path, DNS1_SERVER, tmp, sizeof(tmp)) > 0){	
			strncpy(dns1_v6, tmp, sizeof(dns1_v6)-1);
		}
		
		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(path, DNS2_SERVER, tmp, sizeof(tmp)) > 0){			
			strncpy(dns2_v6, tmp, sizeof(dns2_v6)-1);
		}
		if( 0 != strcmp(dns1_v6, "") || 0 != strcmp(dns2_v6,"") ){
			snprintf(buf, size, "option domain-name-servers %s  %s;\n", dns1_v6, dns2_v6);
		}
	}
	else
	{
		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(path, DHCP6S_DNSTYPE, tmp, sizeof(tmp)) > 0){
			strncpy(dns_type, tmp, sizeof(dns_type)-1);
		}
		else{
			strncpy(dns_type, "2", sizeof(dns_type)-1);
		}

		memset(tmp, 0, sizeof(tmp));
#if defined(TCSUPPORT_CT_VRWAN)	
		if( 1 == isvrentry ){
			if(cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv6", defrouteidx, sizeof(defrouteidx)) > 0){
				strncpy(dns_source, defrouteidx, sizeof(dns_source)-1);
			}
		}
		else
#endif
		{
			if(cfg_get_object_attr(path, DHCP6S_DNSSOURCE, tmp, sizeof(tmp)) > 0){
				strncpy(dns_source, tmp, sizeof(dns_source)-1);
			}
		}

		dnsType = atoi(dns_type);
		if ( 0 == dnsType )
		{
			ifidx = atoi(dns_source);
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, ifidx+1);
			if ( 0 > cfg_get_object_attr(nodeName, "Status6", status6, sizeof(status6)) )
				status6[0] = '\0';
			if ( 0 > cfg_get_object_attr(nodeName, "DNS6", dns1_v6, sizeof(dns1_v6)) )
				dns1_v6[0] = '\0';
			if ( 0 > cfg_get_object_attr(nodeName, "SecDNS6", dns2_v6, sizeof(dns2_v6)) )
				dns2_v6[0] = '\0';

			if ( 0 == strcmp(status6, "up") && '\0' != dns1_v6[0] ){
				snprintf(buf, size, "option domain-name-servers %s  %s;\n", dns1_v6, dns2_v6);
			}
			else{
				snprintf(buf, size, "option domain-name-servers %s;\n", DHCP6S_DEFAULT_DNS);
			}
		}
		else if ( 1 == dnsType )
		{
			memset(tmp, 0, sizeof(tmp));
			if ( cfg_get_object_attr(path, DNS1_SERVER, tmp, sizeof(tmp)) > 0 )
				strncpy(dns1_v6, tmp, sizeof(dns1_v6)-1);
			memset(tmp, 0, sizeof(tmp));
			if ( cfg_get_object_attr(path, DNS2_SERVER, tmp, sizeof(tmp)) > 0 )
				strncpy(dns2_v6, tmp, sizeof(dns2_v6)-1);
			if ( '\0' != dns1_v6[0] )
				snprintf(buf, size, "option domain-name-servers %s  %s;\n", dns1_v6, dns2_v6);
		}
		else
			snprintf(buf, size, "option domain-name-servers %s;\n", DHCP6S_DEFAULT_DNS);
	}

	return;
}

int svc_other_handle_event_dhcp6s_write(char *path)
{
	char ip_v6[40]={0},dns1_v6[40]={0},dns2_v6[40]={0},
		prefix_v6len[5]={0},enable_flag[5] = {0},
			  preffered_lifetime[16]={0},valid_lifetime[16]={0},
			  mode[4]={0};
	char tmp[128]= {0};
	char buf[512]={0};int len = 0;
	char val[64]= {0};

	char ipv6_start[50]={0};
	char ipv6_end[50] = {0};
	char pd_index_v6[5] = {0}, temp[45] = {0}, status6[7] = {0};
	char dhcpv6_wan_pd_ptime[16] = {0}, dhcpv6_wan_pd_vtime[16] = {0};
	char nodeName[64];	
	char *p = NULL;
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
	char new_prefix[45] = {0};
#endif
	int pd_index = 0;
	char domain_name[68] = {0};
	int res = 0;
	char configFile[64] = {0};
	int entry_idx = 0;
	char dnsRelayType[8] = {0};
	int isvrentry = 0;
#if defined(TCSUPPORT_CT_VRWAN)
	char orgpd[128] = {0}, vrpd[128] = {0};
	int plen = 0;
#endif
	char orgpd6[128] = {0}, orgpd_length[8] = {0};

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	if(cfg_get_object_attr(path, "Type", val, sizeof(val)) > 0){			
		if(strstr(val, "VR") || strstr(val, "OTHER"))
			return 0;
	}

	if( NULL != strstr(path, "common") )
	{
		snprintf(configFile, sizeof(configFile), DHCP6S_CONFPATH);
	}
	else
	{
		sscanf(path, DHCP6S_ENTRY_NODE, &entry_idx);
		if( entry_idx < 0 )
			return -1;
		
#if defined(TCSUPPORT_CT_VRWAN)		
		if(cfg_get_object_attr(path, "Type", val, sizeof(val)) > 0){			
			if(strstr(val, "VR"))
				isvrentry = 1;
		}
#endif
		snprintf(configFile, sizeof(configFile), DHCP6S_VR_CONFPATH, entry_idx-1);
	}
#else
	snprintf(configFile, sizeof(configFile), DHCP6S_CONFPATH);
#endif

	if(cfg_query_object(path, NULL, NULL) > 0){
		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(path, DHCP6S_ENABLE_SIMBOL, tmp, sizeof(tmp)) > 0)
		{
			strncpy(enable_flag, tmp, sizeof(enable_flag)-1);
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(path, DHCP6S_MODE, tmp, sizeof(tmp)) > 0)
		{
			strncpy(mode, tmp, sizeof(mode)-1);
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(path, DHCP6S_DOMAIN, tmp, sizeof(tmp)) > 0)
		{
			strncpy(domain_name, tmp, sizeof(domain_name)-1);
		}

		cfg_get_object_attr(DHCP6S_DNSRELAY_NODE, "type", dnsRelayType, sizeof(dnsRelayType));
		if(atoi(dnsRelayType) == 1)
			{
			set_dnsv6_Relay_domain_name_server(buf, sizeof(buf));
			}
				else
			{
			set_domain_name_server(buf, sizeof(buf), mode, isvrentry, path);	
		}

		if ( '\0' != domain_name[0] )
		{
			len = strlen(buf);
			snprintf(buf+len, sizeof(buf)-len, "option domain-name \"%s\";\n", domain_name);
		}

		if(mode[0] == '0'){ /*Dhcp6s auto DNS and prefix mode*/
			/*Get prefix delegation from Wan interface which is default route*/

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
			pd_index = getRaWanPDInterface(path);
#else
			pd_index = get_run_pd_index();
#endif

			if ( pd_index >= 0 )
			{
				snprintf(pd_index_v6, sizeof(pd_index_v6), "%d", pd_index);
			
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, pd_index + 1);
				cfg_get_object_attr(nodeName, "Status6", status6, sizeof(status6));
				cfg_get_object_attr(nodeName, "PD6", temp, sizeof(temp));	
				cfg_get_object_attr(nodeName, "OrgPD6", orgpd6, sizeof(orgpd6));	
				cfg_get_object_attr(nodeName, "PD6ptime", dhcpv6_wan_pd_ptime, sizeof(dhcpv6_wan_pd_ptime));
				cfg_get_object_attr(nodeName, "PD6vtime", dhcpv6_wan_pd_vtime, sizeof(dhcpv6_wan_pd_vtime));

#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
				/*generate prefix for lan DHCP*/
				if(lan_ipv6_prefix_gen(new_prefix, pd_index_v6) == 0){
					strncpy(temp, new_prefix, sizeof(temp)-1);
				}
#endif
				
				if((0 != strcmp(temp, "")) && !strcmp(status6, "up")){
					/*Parse prefix and prefix-len from PD6,eg:3000:456:AEF:DCA::/64*/
					strtok(temp, "/");
					strncpy(ip_v6, temp, sizeof(ip_v6)-1);

					p = strtok(NULL, "/");
					if(p)
						strncpy(prefix_v6len, p, sizeof(prefix_v6len) - 1);
					sscanf(orgpd6, "%*[^/]/%s", orgpd_length);
					if ( '\0' != dhcpv6_wan_pd_ptime[0]
							&& '\0' != dhcpv6_wan_pd_vtime[0] )
					{
						strncpy(preffered_lifetime, dhcpv6_wan_pd_ptime, sizeof(preffered_lifetime)-1);
						strncpy(valid_lifetime, dhcpv6_wan_pd_vtime, sizeof(valid_lifetime)-1);
					}
					else
					{
						strncpy(preffered_lifetime, DHCP6S_DEF_PREFER_LIFE, sizeof(preffered_lifetime)-1);		
						strncpy(valid_lifetime, DHCP6S_DEF_VALID_LIFE, sizeof(valid_lifetime)-1);
					}
				}
			}
		}
		else if(mode[0] == '1'){ /*Dhcp6s manual mode*/
			memset(tmp, 0, sizeof(tmp));
			if(cfg_get_object_attr(path, PREFIX_SIMBOL, tmp, sizeof(tmp)) > 0){			
				strncpy(ip_v6, tmp, sizeof(ip_v6)-1);
			}
			memset(tmp, 0, sizeof(tmp));
			if(cfg_get_object_attr(path, PREFIX_LEN_SIMBOL, tmp, sizeof(tmp)) > 0){			
				strncpy(prefix_v6len, tmp, sizeof(prefix_v6len)-1);
				strncpy(orgpd_length, prefix_v6len, sizeof(orgpd_length)-1);
			}
			memset(tmp, 0, sizeof(tmp));
			if(cfg_get_object_attr(path, PREFFERED_LIFETIME_SIMBOL, tmp, sizeof(tmp)) > 0){			
				strncpy(preffered_lifetime, tmp, sizeof(preffered_lifetime)-1);
			}
			memset(tmp, 0, sizeof(tmp));
			if(cfg_get_object_attr(path, VALID_LIFETIME_SIMBOL, tmp, sizeof(tmp)) > 0){			
				strncpy(valid_lifetime, tmp, sizeof(valid_lifetime)-1);
			}
		}

		if(
				( 0 != strcmp(preffered_lifetime, "") ) &&	
				( 0 != strcmp(valid_lifetime, "") ) && 
				( 0 != strcmp(ip_v6, "") )
			)
		{
			/*setting the dhcp6s address pool range*/			
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
			/*generate start address*/			
			if(lan_ipv6_addr_gen(ipv6_start, "MinAddress", ip_v6, path) != 0)
#endif
			{
				ipv6AddressTo01(ipv6_start, ip_v6);
			}
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
			/*generate start address*/
			if(lan_ipv6_addr_gen(ipv6_end,"MaxAddress", ip_v6, path) != 0)
#endif
			{
				ipv6AddressToFF(ipv6_end, ip_v6);
			}
			
			len = strlen(buf);
			snprintf(buf+len, sizeof(buf)-len,"interface br0 {\n\t address-pool pool1 %s %s;\n};\n",
			preffered_lifetime, valid_lifetime);

			len = strlen(buf);
			snprintf(buf+len, sizeof(buf)-len,"\nhost ecnt {\n\t duid 11:11:11:11:11:11:11:11:11:11:11:11:11:11;\n\tprefix %s/%s %s %s;\n};\n",
			ip_v6, orgpd_length, preffered_lifetime, valid_lifetime);

			len = strlen(buf);
			snprintf(buf+len, sizeof(buf)-len, "\npool pool1 {\n\t range %s to %s;\n};\n",
			ipv6_start, ipv6_end);
		}
		else
		{
			len = strlen(buf);
			snprintf(buf+len, sizeof(buf)-len,"interface br0 { };\n");
		}
	
		
		if(write2file(buf,configFile) == -1)
		{
			 tcdbg_printf("fail to write file /etc/dhcp6s.conf\n");
			 return -1;
		}
	
	}
	else{
		unlink(configFile);
	}	
	res = chmod(configFile, 777);
	if(res){
		/*do nothing,just for compile*/
	}

	return 0;
}

int svc_other_handle_event_dhcp6s_execute(char *path)
{
	char enable_value[8] = {0};
	char mode[5] = {0}, ipv6_br0[50] = {0}, cmd[128] = {0};
	char configFile[64] = {0}, pidPath[64] = {0};
	int skb_mark = -1;
	int entry_idx = 0;
	char val[64]= {0};

#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	if(cfg_get_object_attr(path, "Type", val, sizeof(val)) > 0){			
		if(strstr(val, "VR") || strstr(val, "OTHER"))
			return 0;
	}

	if( NULL != strstr(path, "common") )
	{
		snprintf(configFile, sizeof(configFile), DHCP6S_CONFPATH);
		snprintf(pidPath, sizeof(pidPath), "/var/run/dhcp6s.pid");	
	}
	else
	{
		sscanf(path, DHCP6S_ENTRY_NODE, &entry_idx);
		if( entry_idx < 0 )
		return -1;
		
		snprintf(configFile, sizeof(configFile), DHCP6S_VR_CONFPATH, entry_idx-1);
		snprintf(pidPath, sizeof(pidPath), DHCP6S_VR_PIDPATH, entry_idx-1);
	}
	skb_mark = getSkbMark(path);
#else
	snprintf(configFile, sizeof(configFile), DHCP6S_CONFPATH);
	snprintf(pidPath, sizeof(pidPath), "/var/run/dhcp6s.pid");
#endif

	if(cfg_get_object_attr(path, "Enable", enable_value, sizeof(enable_value)) < 0){
		return -1;
	}

	if(cfg_get_object_attr(path, BR0_ADDR, ipv6_br0, sizeof(ipv6_br0)) > 0 && strcmp(ipv6_br0, "N/A")){
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 del %s", ipv6_br0);
		system_escape(cmd);
		cfg_set_object_attr(path, BR0_ADDR, "N/A");
	}

	kill_process(pidPath);
	
	if (0 == strcmp(enable_value, "1")) 
	{
		if(cfg_get_object_attr(path, DHCP6S_MODE, mode, sizeof(mode)) < 0){
			return -1;
		}
		/*Manual mode*/
		if(mode[0] == '1'){
			if(create_br0_pool_ipv6addr(path, ipv6_br0) == 0){ 
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 %s", ipv6_br0);
				system(cmd);
				/*save br0 address*/
				cfg_set_object_attr(path, BR0_ADDR, ipv6_br0);
			}
			
		}
		memset(cmd, 0, sizeof(cmd));

		if( 0 > skb_mark )
			skb_mark = 0;
		
		snprintf(cmd, sizeof(cmd), "/userfs/bin/dhcp6s -c %s br0 -M %d -P %s &", configFile, skb_mark, pidPath);
		system(cmd);
	}

	else 
	{
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
		unlink(configFile);
#endif
		tcdbg_printf("dhcp6s call closed by exec!\n");
	}

	return 0;
}

int svc_other_handle_event_dhcp6s_boot(char *path)
{
	char value_en[3] = {0};
	char value_mode[3] = {0};
	char ipv6_br0[50] = {0}, cmd[80] = {0};

	if(cfg_get_object_attr(path, "Enable", value_en, sizeof(value_en)) < 0){
		tcdbg_printf("error:Dhcp6s function Get enable value fail!\n");
		return -1;
	}

	if( 0 != strcmp(value_en, "1") )
	{
		tcdbg_printf("Enable:Dhcp6s function NOT activated!\n");
		return 0;
	}

	
	/* judging the dhcp6s mode 1 for manual mode; 0 for auto mode
	   even dhcp6s is enabled, in auto mode  this function will 
	   NOT be started !
 	*/
	if(cfg_get_object_attr(path, "Mode", value_mode, sizeof(value_mode)) < 0){
		tcdbg_printf("error:Dhcp6s function Get enable value fail!\n");
		return -1;
	}

	if( 0 != strcmp(value_mode, "1") )
	{
		tcdbg_printf("Mode:Dhcp6s mode is not manual!\n");
		return 0;
	}


	if(create_br0_pool_ipv6addr(path, ipv6_br0) == 0){
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig br0 %s", ipv6_br0);
		system(cmd);
		/*save br0 address*/
		cfg_set_object_attr(path, "Br0Addr", ipv6_br0);
	}
	
	system("/userfs/bin/dhcp6s -c /etc/dhcp6s.conf br0 -P /var/run/dhcp6s.pid &");

	return 0;
}



static int get_run_pd_index()
{
	char pd_index[6] = {0};
	
	if( 0 > cfg_get_object_attr(WANINFO_COMMON_NODE, "PDRUNIFIdx", pd_index, sizeof(pd_index)) )
		return -1;
	if( 0 == strcmp(pd_index, "N/A") )
		return -2;

	return atoi(pd_index);
}


static int getRaWanPDInterface(char *path)
{
	char nodeName[64] = {0};
	char val_buf[8] = {0};
	char active[8] = {0};
	int pdIdx = 0;
	int pvc_idx = 0, entry_idx = 0;
	char attrName_index[64] = {0};
	char attrName[64] = {0};
	
	memset(val_buf, 0, sizeof(val_buf));
	if( NULL != strstr(path, "radvd") ){
		snprintf(attrName_index, sizeof(attrName_index), "DelegatedWanConnection");
		snprintf(attrName, sizeof(attrName), "AutoPrefix");
	}
	else{
		snprintf(attrName_index, sizeof(attrName_index), "DNSWANConnection");
		snprintf(attrName, sizeof(attrName), "DNSType");
	}
	
	if ( 0 > cfg_get_object_attr(path, attrName, val_buf, sizeof(val_buf)) || 2 == atoi(val_buf))
		return get_run_pd_index();

	if ( 0 == atoi(val_buf) )
	{
		memset(val_buf, 0, sizeof(val_buf));
		if ( 0 > cfg_get_object_attr(path, attrName_index, val_buf, sizeof(val_buf)) )
			return get_run_pd_index();
		else
		{
			if ( '\0' != val_buf[0] )
			{
				pdIdx = atoi(val_buf);
				pvc_idx = pdIdx / 8 + 1;
				entry_idx = pdIdx % 8 + 1;
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName,sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_idx, entry_idx);
				if ( 0 > cfg_get_object_attr(nodeName, "Active", active, sizeof(active)) )
				{
					cfg_set_object_attr(path, attrName_index, "");
					return get_run_pd_index();
				}
#if defined(TCSUPPORT_CT_VRWAN)
				else if( strcmp(active, "Yes") )
				{
					if( NULL != strstr(path, "Common") ){
					return get_run_pd_index();
					}else{
						return -1;
				}
			}
#endif			
			}
			else
				return get_run_pd_index();
		}
	}
	else
		return -1;

	return atoi(val_buf);

}

static int lan_ipv6_prefix_gen(char * result_prefix, char *entry)
{
	int ret = -1, fd = -1, result = -1;
	char nodeName[64] = {0};
	char tmp_str[64] = {0}, path[64] = {0};
	char prefix_str[64] = {0};
	char child_prefix_str[64] = {0};
	struct in6_addr prefix_addr;
	struct in6_addr child_prefix_addr;
	int prefix_len = 0, child_prefix_len = 0;
	int pvc_idx = 0, pvc_entry = 0,entry_idx = 0;
	int prfix_type = PREFIX_ORIGN_DFT;
	int tmp_val = 0;
	char * tmp_p = NULL;
	char * tmp2_p = NULL;
	int start_mask =0, end_mask = 0, mask_range = 0;
	int i,j,k, res = 0;
	unsigned char tmp_u8 = 0;
	
	if(result_prefix == NULL || entry == NULL || (strlen(entry)<=0)){
		goto end;
	}
	/*init*/
	entry_idx = atoi(entry);
	pvc_idx = entry_idx / 8;
	pvc_entry = entry_idx % 8;
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_idx, pvc_entry);

	/*get ipv6Prefix.ChildPrefixBits*/
	memset(tmp_str, 0, sizeof(tmp_str));
	if(cfg_get_object_attr(nodeName, "PDChildPrefixBits", tmp_str, sizeof(tmp_str)) < 0){
		goto end;
	}
	tmp_val = strlen(tmp_str);
	if(tmp_val <= 0 || tmp_val > MAX_PREFIX_ADDR_LEN){
		goto end;
	}
	strncpy(child_prefix_str, tmp_str,sizeof(child_prefix_str)-1);
	
	/*get ipv6Prefix.origin*/
	memset(tmp_str, 0, sizeof(tmp_str));
	if(cfg_get_object_attr(nodeName, "DHCPv6PD", tmp_str, sizeof(tmp_str)) > 0){
		if(strcmp(tmp_str,"Yes") == 0){
			prfix_type = PREFIX_ORIGN_DHCP;
			/*get prefix for dhcp*/
			memset(tmp_str, 0, sizeof(tmp_str));
			memset(nodeName, 0, sizeof(nodeName));
		  	snprintf(nodeName,sizeof(nodeName), WANINFO_ENTRY_NODE, entry_idx);
			if(cfg_get_object_attr( nodeName, "PD6", tmp_str, sizeof(tmp_str)) < 0){
				goto end;
			}
			tmp_val = strlen(tmp_str);
			if(tmp_val <= 0 || tmp_val > MAX_PREFIX_ADDR_LEN){
				goto end;
			}
			strncpy(prefix_str, tmp_str,sizeof(prefix_str)-1);
		}
	}

	if(prfix_type == PREFIX_ORIGN_DFT){
		memset(tmp_str, 0, sizeof(tmp_str));
		if(cfg_get_object_attr(nodeName, "PDOrigin", tmp_str, sizeof(tmp_str)) < 0){
			goto end;
		}
		if(strcmp(tmp_str,"RouterAdv") == 0){
			prfix_type = PREFIX_ORIGN_SLLA;
			/*get prefix for SLAAC*/
			/*get ifc name*/
			memset(tmp_str, 0, sizeof(tmp_str));
			if(cfg_get_object_attr(nodeName, "IFName", tmp_str, sizeof(tmp_str)) < 0){
				goto end;
			}
			/*check parent prefix*/ 
			snprintf(path,sizeof(path), "/proc/sys/net/ipv6/conf/%s/slaac_prefix", tmp_str);
			fd=open(path, O_RDONLY|O_NONBLOCK);
			if(fd == -1){
				goto end;
			}
			memset(tmp_str, 0, sizeof(tmp_str));
			ret = read(fd, tmp_str, MAX_PREFIX_ADDR_LEN);
			tmp_str[MAX_PREFIX_ADDR_LEN] = '\0';
			close(fd);
			if((ret <= 1) || (strlen(tmp_str)<=0)){
				goto end;
			}
			strncpy(prefix_str, tmp_str,sizeof(prefix_str)-1);
		}
		else if (strcmp(tmp_str,"Static") == 0){
			/*nothing*/
		}
	}

	if(prfix_type == PREFIX_ORIGN_DFT){
		goto end;
	}
	
	/*get child prefix len*/
	tmp_p = strstr(child_prefix_str,"/");
	if(tmp_p == NULL){
		goto end;
	}
	tmp2_p = tmp_p +1;
	child_prefix_len = atoi(tmp2_p);
	if(child_prefix_len <= 0 || child_prefix_len > 64){
		goto end;
	}

	*tmp_p = '\0';
	/*get child prefix ipv6 addr*/
	res = inet_pton(AF_INET6, child_prefix_str, (void *)&child_prefix_addr);

	/*get prefix len*/
	tmp_p = strstr(prefix_str,"/");
	if(tmp_p == NULL){
		goto end;
	}
	tmp2_p = tmp_p +1;
	prefix_len = atoi(tmp2_p);

	if(prefix_len <= 0 || prefix_len > 64){
		goto end;
	}

	*tmp_p = '\0';
	/*get prefix ipv6 addr*/
	res = inet_pton(AF_INET6, prefix_str, (void *)&prefix_addr);

	/*calculate new prefix based on childprefix and parentprefix*/
	start_mask = prefix_len;
	end_mask = child_prefix_len;
	mask_range = end_mask - start_mask;
	if(mask_range <= 0 ){
		if(mask_range == 0){
			result = 0;
		}
		goto end;
	}

	for(i=start_mask; i<end_mask; i++){
		j = i/8;
		k = i%8;
		tmp_u8 = (1<<k);
		tmp_u8 &= child_prefix_addr.s6_addr[j];
		prefix_addr.s6_addr[j] |= tmp_u8;
	}	

	prefix_len = child_prefix_len;	
	result = 0;
end:
	if(result == 0){
		/*
		if(prefix_len > 64){
			prefix_len = 64;
		}
		*/
		memset(prefix_str, 0 , sizeof(prefix_str));
		memset(tmp_str, 0, sizeof(tmp_str));
		inet_ntop(AF_INET6,&prefix_addr.s6_addr, prefix_str, sizeof(prefix_str));
		snprintf(tmp_str,sizeof(tmp_str),"%s/%d", prefix_str, prefix_len);
		strncpy(result_prefix, tmp_str, 44);
		result_prefix[44] = '\0';
	}
	return result;
}



static int create_br0_v6addr(char *pd, int pd_len, char *newV6addr)
{
	int i = 0;
	unsigned char ifid[16] = {0};
	char tmp_str[64] = {0};
	struct in6_addr in_s_v6addr;

	if ( NULL == pd || NULL == newV6addr )
		return -1;

	if ( pd_len < 16 )
	{
		return -1;
	}

	get_br0_v6addr(ifid);
	if ( 1 != inet_pton(AF_INET6, pd, &in_s_v6addr) )
		return -1;


	for (i = 15; i >= pd_len / 8; i--)
		in_s_v6addr.s6_addr[i] = ifid[i];

	inet_ntop(AF_INET6, &in_s_v6addr.s6_addr, tmp_str, sizeof(tmp_str));
	
	strcpy(newV6addr, tmp_str);

	return 0;
}


static int create_br0_pool_ipv6addr(char *nodeName, char *ipv6_addr)
{
	char prefix[50] = {0}, prefix_len[5] = {0};
	char v6addr_str[64] = {0};
	
	if(cfg_get_object_attr(nodeName, "PrefixIPv6", prefix, sizeof(prefix)) < 0){
		return -1;
	}
	if(cfg_get_object_attr(nodeName, "Prefixv6Len", prefix_len, sizeof(prefix_len)) < 0){
		return -1;
	}
	if ( 0 != create_br0_v6addr(prefix, atoi(prefix_len), v6addr_str) )
	{
		return -1;
	}
	snprintf(ipv6_addr, 50, "%s/%s", v6addr_str, prefix_len);

	return 0;
}


static int lan_ipv6_addr_gen(char * ipv6_addr_str, char * attr, char * prefix_str, char *path)
{
	int ret = -1;
	struct in6_addr prefix_addr;
	struct in6_addr ipv6_addr;
	int ip6_len = sizeof(struct in6_addr);
	char tmp_str[64] = {0};
	char tmp_str2[64] = {0};
	char attr_str[64] = {0};
	char * tmp_p = NULL;
	int i = 0, res = 0;

	/*init*/
	if(ipv6_addr_str == NULL || attr == NULL || prefix_str == NULL){
		goto end;
	}
	tmp_p = strstr(prefix_str, "/");
	if(tmp_p != NULL){
		*tmp_p = '\0';
	}
	if(strlen(attr) == 0 || strlen(prefix_str) == 0){
		goto end;
	}
	memset(&prefix_addr, 0, ip6_len);
	memset(&ipv6_addr, 0, ip6_len);
	strncpy(ipv6_addr_str,"",50-1);

	/*get prefix ipv6 addr*/	
	res = inet_pton(AF_INET6, prefix_str, (void *)&prefix_addr);

	/*get min or max address*/	
	if(cfg_get_object_attr(path, attr, attr_str, sizeof(attr_str)) < 0){
		goto end;		
	}

	tmp_p = strstr(attr_str, "/");
	if(tmp_p != NULL){
		*tmp_p = '\0';
	}

	if(strlen(attr_str) <= 0){
		goto end;
	}
	memset(tmp_str2, 0, sizeof(tmp_str2));
	if(strstr(attr_str,"::") == NULL){
		strncpy(tmp_str2,"::",sizeof(tmp_str2)-1);
		strcat(tmp_str2, attr_str);
	}else{
		strncpy(tmp_str2,attr_str,sizeof(tmp_str2)-1);
	}

	memset(tmp_str, 0, sizeof(tmp_str));
	shrink_ipv6addr(tmp_str2, tmp_str, sizeof(tmp_str));
	res = inet_pton(AF_INET6, tmp_str, (void *)&ipv6_addr);

	for(i = 8; i<16; i++){
		prefix_addr.s6_addr[i] |= ipv6_addr.s6_addr[i];
	}
	inet_ntop(AF_INET6,&prefix_addr.s6_addr, tmp_str, sizeof(tmp_str));
	strncpy(ipv6_addr_str,tmp_str,49);
	ret = 0;
end:
	return ret;

}

static int ipv6AddressTo01(char *Dest, char * Src)
{
	strncpy(Dest,Src,strlen(Src));

	Dest++;	

	while(*Dest != '\0')
	{

		if( (Dest[-1] == ':') && ( *Dest == ':' ) )
		{
			break;
		}
		Dest++;
	}

	
	Dest ++;	
	*Dest = '1';
	Dest ++;
	*Dest = '\0';

	return 0;	
}

static int ipv6AddressToFF(char *Dest, char *Src)
{
	if( (*Src == ':') || (*Src == '\0') )
		return -1;

	strncpy( Dest, Src, strlen(Src) );
	while(*Dest++ );
	Dest--;

	*Dest = '1';Dest ++;  *Dest = '0';Dest ++;
	*Dest = '0';Dest ++;  *Dest = '0';Dest ++;  
	*Dest = '\0';
	Dest ++;

	return 0;
}


