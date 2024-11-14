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
#include "cfg_cli.h"
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_msg.h"
#include "blapi_traffic.h"

#if defined(TCSUPPORT_MULTI_USER_ITF)
#define MAX_WLAN_MAPPING_COUNT 16
#define MAX_WLAN_2_4_COUNT 8
#else
#define MAX_WLAN_MAPPING_COUNT 8
#define MAX_WLAN_2_4_COUNT 4
#endif

static void update_guest_ssid_rule(void){
	char  wlan_node_name[64] = {0}, rule[8] = {0}, strindex[10] = {0};
	char cmd_path[256] = {0};
	int index = 0, entryidx = 0;
	
	system("iptables -t filter -F WLan_INPUTAccess && iptables -t filter -Z WLan_INPUTAccess");
	
	/* IPv6 */
	system("ip6tables -t filter -F WLan_INPUTAccess && ip6tables -t filter -Z WLan_INPUTAccess");
	
	for(index = 0; index < MAX_WLAN_MAPPING_COUNT; index++)
	{
		snprintf(wlan_node_name, sizeof(wlan_node_name), WLANMAPPING_ENTRY_N_NODE, index);
		if(cfg_get_object_attr(wlan_node_name, "AccessRule", rule, sizeof(rule)) > 0 && 0 == (strcmp(rule, "1")))
		{
			if ( cfg_get_object_attr(wlan_node_name, "entryidx", strindex, sizeof(strindex)) <= 0 )
				continue;
			entryidx = atoi(strindex);
			snprintf(cmd_path, sizeof(cmd_path), 
			"iptables -t filter -A WLan_INPUTAccess -p tcp -m mark --mark 0x%x/0xf8000000 -j DROP", ((entryidx + (MAX_WLAN_MAPPING_COUNT/2)+1) << 27));
			system(cmd_path);
			snprintf(cmd_path, sizeof(cmd_path), 
			"iptables -t filter -A WLan_INPUTAccess -p icmp -m mark --mark 0x%x/0xf8000000 -j DROP", ((entryidx + (MAX_WLAN_MAPPING_COUNT/2)+1) << 27));
			system(cmd_path);

			/* IPv6 */
			snprintf(cmd_path, sizeof(cmd_path), 
			"ip6tables -t filter -A WLan_INPUTAccess -p tcp -m mark --mark 0x%x/0xf8000000 -j DROP", ((entryidx + (MAX_WLAN_MAPPING_COUNT/2)+1) << 27));
			system(cmd_path);
			snprintf(cmd_path, sizeof(cmd_path), 
			"ip6tables -t filter -A WLan_INPUTAccess -p icmpv6 --icmpv6-type echo-request -m mark --mark 0x%x/0xf8000000 -j DROP", ((entryidx + (MAX_WLAN_MAPPING_COUNT/2)+1) << 27));
			system(cmd_path);
		}
	}
	
}

static int commit_one_node(int e_idx, int guestflag)
{
	int  ra_idx = 0, mapping_idx = 0, idx = 0;
	int i_accessRule = 0, i_port = 0;
	char wlan_node_name[64] = {0}, *v6_pos = NULL;
	char upstream[32] = {0}, dwstream[32] = {0};
	char cmd_path[256] = {0}, entry_idx[24] = {0};
	char accessRule[32] = {0}, i_ra_itf[32] = {0}, ip_str[128] = {0};
	static char allowedIPPort[1024] = {0};
	char *ipport_p = NULL, *savePtr = NULL;
	
	snprintf(wlan_node_name, sizeof(wlan_node_name), WLANMAPPING_ENTRY_N_NODE, e_idx );
	if ( cfg_get_object_attr(wlan_node_name, "entryidx", entry_idx, sizeof(entry_idx)) <= 0 )
		return -1;
	mapping_idx = atoi(entry_idx);

	/* speed limit base SSID -- upstream */
	if ( cfg_get_object_attr(wlan_node_name, "USBandwidth", upstream, sizeof(upstream)) > 0 )
	{
		blapi_traffic_set_wifi_interface_ratelimit(UP_DIR, mapping_idx, atoi(upstream));
	}
	/* speed limit base SSID -- downstream */
	if ( cfg_get_object_attr(wlan_node_name, "DSBandwidth", dwstream, sizeof(dwstream)) > 0 )
	{
		blapi_traffic_set_wifi_interface_ratelimit(DOWN_DIR, mapping_idx, atoi(dwstream));
	}

	snprintf(cmd_path, sizeof(cmd_path),"iptables -t filter -F WLan_AccessRule%d && iptables -t filter -Z WLan_AccessRule%d", mapping_idx, mapping_idx);
	system(cmd_path);
		
	snprintf(cmd_path, sizeof(cmd_path),"ebtables -t filter -F WLan_AccessRule%d && ebtables -t filter -Z WLan_AccessRule%d", mapping_idx, mapping_idx);
	system(cmd_path);

	/* IPv6 */
	snprintf(cmd_path, sizeof(cmd_path),"ip6tables -t filter -F WLan_AccessRule%d && ip6tables -t filter -Z WLan_AccessRule%d", mapping_idx, mapping_idx);
	system(cmd_path);


	/* AccessRule base SSID */
	if ( cfg_get_object_attr(wlan_node_name, "AccessRule", accessRule, sizeof(accessRule)) > 0 )
	{
		i_accessRule = atoi(accessRule);
		if ( mapping_idx < MAX_WLAN_2_4_COUNT )
		{
			ra_idx = mapping_idx;
			snprintf(i_ra_itf, sizeof(i_ra_itf), "ra%d", ra_idx);
		}
		else
		{
			ra_idx = mapping_idx - MAX_WLAN_2_4_COUNT;
			snprintf(i_ra_itf, sizeof(i_ra_itf), "rai%d", ra_idx);
		}
	
		snprintf(cmd_path, sizeof(cmd_path), 
			"/usr/bin/portbindcmd portisolation 0 %d 1", mapping_idx);
		system(cmd_path);
		
		if ( 0 != i_accessRule )
		{
			snprintf(cmd_path, sizeof(cmd_path), 
				"/usr/bin/portbindcmd portisolation 1 %d 1", mapping_idx);
			system(cmd_path);
			
			wifimgr_lib_set_WIFI_CMD(i_ra_itf, -1, sizeof(cmd_path), "NoForwardingBTNBSSID=1", cmd_path, 0);
			system(cmd_path);

			if ( 1 == i_accessRule && 1 == guestflag){
				update_guest_ssid_rule();
			}
			else if ( 2 == i_accessRule )
			{
				snprintf(cmd_path, sizeof(cmd_path), 
					"iptables -t filter -A WLan_AccessRule%d -i %s -j DROP", mapping_idx, i_ra_itf);
				system(cmd_path);
				/* IPv6 */
				snprintf(cmd_path, sizeof(cmd_path), 
					"ip6tables -t filter -A WLan_AccessRule%d -i %s -j DROP", mapping_idx, i_ra_itf);
				system(cmd_path);
				
				snprintf(cmd_path, sizeof(cmd_path),
						"ebtables -t filter -A WLan_AccessRule%d -p arp -j ACCEPT", mapping_idx);
				system(cmd_path);
				/* IPv6 NS packet. */
				snprintf(cmd_path, sizeof(cmd_path),
						"ebtables -t filter -A WLan_AccessRule%d -p IPv6 --ip6-proto 58 --ip6-icmpv6type 135 -j ACCEPT", mapping_idx);
				system(cmd_path);

				snprintf(cmd_path, sizeof(cmd_path), 
					"ebtables -t filter -A WLan_AccessRule%d -p IPv4 --ip-proto 17 --ip-dport 67 -j ACCEPT"
					, mapping_idx);
				system(cmd_path);
				/* IPv6 */
				snprintf(cmd_path, sizeof(cmd_path), 
					"ebtables -t filter -A WLan_AccessRule%d -p IPv6 --ip6-proto 17 --ip6-dport 547 -j ACCEPT"
					, mapping_idx);
				system(cmd_path);
				
				if ( cfg_get_object_attr(wlan_node_name, "AllowedIPPort", allowedIPPort, sizeof(allowedIPPort)) > 0 )
				{
					ipport_p = strtok_r(allowedIPPort, ",", &savePtr);
					while ( ipport_p )
					{
						if ( (v6_pos = strstr(ipport_p, "]")) ) /* IPv6 */	
						{
							*(v6_pos + 1) = 0;
							snprintf(ip_str, sizeof(ip_str), "%s", ipport_p);
							i_port = atoi(v6_pos + 2);
						}
						else
							sscanf(ipport_p, "%[^:]:%d", ip_str, &i_port);

						tcdbg_printf("\n ip_str=[%s],i_port=[%d] \n", ip_str, i_port);

						if ( 0 != ip_str[0] && 0 != i_port )
						{
							/* IPv6 */	
							if ( strstr(ip_str, "]") )
							{
								/* replace [] with space. */
								for ( idx = 0; idx < strlen(ip_str); idx ++ )
								{
									if ( '[' == ip_str[idx] || ']' == ip_str[idx] )
										ip_str[idx] = ' ';
								}

								snprintf(cmd_path, sizeof(cmd_path), 
									"ebtables -t filter -A WLan_AccessRule%d -p IPv6 --ip6-proto 6 --ip6-dst %s --ip6-dport %d -j ACCEPT"
									, mapping_idx, ip_str, i_port);
								system(cmd_path);
								snprintf(cmd_path, sizeof(cmd_path), 
									"ebtables -t filter -A WLan_AccessRule%d -p IPv6 --ip6-proto 17 --ip6-dst %s --ip6-dport %d -j ACCEPT"
									, mapping_idx, ip_str, i_port);
								system(cmd_path);
							}
							else
							{
								snprintf(cmd_path, sizeof(cmd_path), 
									"ebtables -t filter -A WLan_AccessRule%d -p IPv4 --ip-proto 6 --ip-dst %s --ip-dport %d -j ACCEPT"
									, mapping_idx, ip_str, i_port);
								system(cmd_path);
								snprintf(cmd_path, sizeof(cmd_path), 
									"ebtables -t filter -A WLan_AccessRule%d -p IPv4 --ip-proto 17 --ip-dst %s --ip-dport %d -j ACCEPT"
									, mapping_idx, ip_str, i_port);
								system(cmd_path);
							}
						}
						ipport_p = strtok_r(NULL, ",", &savePtr);
					}
					snprintf(cmd_path, sizeof(cmd_path), 
							"ebtables -t filter -A WLan_AccessRule%d  -j DROP", mapping_idx);
					system(cmd_path);
				}
			}
		}
		else
		{
			wifimgr_lib_set_WIFI_CMD(i_ra_itf, -1, sizeof(cmd_path), "NoForwardingBTNBSSID=0", cmd_path, 0);
			system(cmd_path);
		}

	}
	return 0;
}

static int cfg_type_wlanmapping_func_commit(char* path)
{
	int e_idx = 0;
	char *pos = NULL;

#if defined(TCSUPPORT_CUC)
	return 0;
#endif
	if ( NULL != (pos = strstr(path, "entry")) )
	{
		e_idx = atoi(pos + strlen("entry."));
		commit_one_node(e_idx, 1);
#if defined(TCSUPPORT_CT_UBUS)
		update_guest_ssid_rule();
#endif
	}
	else
	{
		for ( e_idx = 0; e_idx < MAX_WLAN_MAPPING_COUNT; e_idx ++ )
		{
			commit_one_node(e_idx + 1, 0);
		}
		update_guest_ssid_rule();
	}

	return 0;
}

static cfg_node_ops_t cfg_type_wlanmapping_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_wlanmapping_common= { 
	 .name = "Common", 
	 .flag = 1,
	 .parent = &cfg_type_wlanmapping, 
	 .ops = &cfg_type_wlanmapping_common_ops, 
}; 

static cfg_node_ops_t cfg_type_wlanmapping_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlanmapping_func_commit 
}; 

static cfg_node_type_t cfg_type_wlanmapping_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_WLAN_MAPPING_COUNT,
	 .parent = &cfg_type_wlanmapping, 
	 .ops = &cfg_type_wlanmapping_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_wlanmapping_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_wlanmapping_func_commit 
};

static cfg_node_type_t* cfg_type_wlanmapping_child[] = { 
	 &cfg_type_wlanmapping_entry,
	 &cfg_type_wlanmapping_common,
	 NULL 
};

cfg_node_type_t cfg_type_wlanmapping = { 
	 .name = "WLanMapping",
	 .flag = 1 ,
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_wlanmapping_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_wlanmapping_child, 
	 .ops = &cfg_type_wlanmapping_ops, 
};

int svc_cfg_wlanmapping_boot(void)
{
	char node_name[64] = {0}, wlan_node_name[64] = {0};
	char entry_idx[24] = {0}, ssid_val[256] = {0}, bssid_num[16] = {0};
	int idx = 0, bssid_num_i = 0, mapping_pos = 0, ra_idx = 0;
	int last_index = 1;
	char buf[16] = {0};
	char cmd_buf[128] = {0};
	char *s_ra_if = NULL;

#if defined(TCSUPPORT_CUC)
	return 0;
#endif
	/* add for guest ssid rule chain */
	system("iptables -t filter -N WLan_INPUTAccess");
	system("iptables -t filter -A INPUT -j WLan_INPUTAccess");
	system("iptables -t filter -F WLan_INPUTAccess");
	system("iptables -t filter -Z WLan_INPUTAccess");
	
	system("iptables -t filter -N WLan_AccessRule");
	system("iptables -t filter -A FORWARD -i ra+ -o ppp+ -j WLan_AccessRule");
	system("iptables -t filter -A FORWARD -i ra+ -o nas+ -j WLan_AccessRule");
	system("iptables -t filter -F WLan_AccessRule && iptables -t filter -Z WLan_AccessRule");
	
	/* IPv6 */
	system("ip6tables -t filter -N WLan_INPUTAccess");
	system("ip6tables -t filter -A INPUT -j WLan_INPUTAccess");
	system("ip6tables -t filter -F WLan_INPUTAccess");
	system("ip6tables -t filter -Z WLan_INPUTAccess");
	
	system("ip6tables -t filter -N WLan_AccessRule");
	system("ip6tables -t filter -A FORWARD -i ra+ -o ppp+ -j WLan_AccessRule");
	system("ip6tables -t filter -A FORWARD -i ra+ -o nas+ -j WLan_AccessRule");
	system("ip6tables -t filter -F WLan_AccessRule && iptables -t filter -Z WLan_AccessRule");

	for ( idx = 0; idx < MAX_WLAN_MAPPING_COUNT; idx ++ )
	{
		snprintf(cmd_buf, sizeof(cmd_buf), "iptables -t filter -N WLan_AccessRule%d", idx);
		system(cmd_buf);
		/* IPv6 */
		snprintf(cmd_buf, sizeof(cmd_buf), "ip6tables -t filter -N WLan_AccessRule%d", idx);
		system(cmd_buf);

		if ( idx < MAX_WLAN_2_4_COUNT )
		{
			s_ra_if = "ra";
			ra_idx = idx;
		}
		else
		{
			s_ra_if = "rai";
			ra_idx = idx - MAX_WLAN_2_4_COUNT;
		}

		snprintf(cmd_buf, sizeof(cmd_buf)
			, "iptables -t filter -A WLan_AccessRule -i %s%d -j WLan_AccessRule%d"
			, s_ra_if, ra_idx, idx);
		system(cmd_buf);
		/* IPv6 */
		snprintf(cmd_buf, sizeof(cmd_buf)
			, "ip6tables -t filter -A WLan_AccessRule -i %s%d -j WLan_AccessRule%d"
			, s_ra_if, ra_idx, idx);
		system(cmd_buf);

		snprintf(cmd_buf, sizeof(cmd_buf),
			"iptables -t filter -F WLan_AccessRule%d && iptables -t filter -Z WLan_AccessRule%d"
			, idx, idx);
		system(cmd_buf);
		/* IPv6 */
		snprintf(cmd_buf, sizeof(cmd_buf),
			"ip6tables -t filter -F WLan_AccessRule%d && iptables -t filter -Z WLan_AccessRule%d"
			, idx, idx);
		system(cmd_buf);

	}

	system("ebtables -t filter -N WLan_AccessRule");
	system("ebtables -t filter -P WLan_AccessRule RETURN");
	system("ebtables -A INPUT -i ra+ -j WLan_AccessRule");
#if defined(TCSUPPORT_CT_UBUS)
	system("ebtables -t filter -A FORWARD -i ra+ -j WLan_AccessRule");
#endif
	system("ebtables -t filter -F WLan_AccessRule");
	system("ebtables -t filter -Z WLan_AccessRule");
	for ( idx = 0; idx < MAX_WLAN_MAPPING_COUNT; idx ++ )
	{
		snprintf(cmd_buf, sizeof(cmd_buf), "ebtables -t filter -N WLan_AccessRule%d", idx);
		system(cmd_buf);
		snprintf(cmd_buf, sizeof(cmd_buf), "ebtables -t filter -P WLan_AccessRule%d RETURN", idx);
		system(cmd_buf);
		snprintf(wlan_node_name, sizeof(wlan_node_name), WLANMAPPING_ENTRY_N_NODE, idx);
		if ( idx < MAX_WLAN_2_4_COUNT )
		{
			s_ra_if = "ra";
			ra_idx = idx;
		}
		else
		{
			s_ra_if = "rai";
			ra_idx = idx - MAX_WLAN_2_4_COUNT;
		}

		snprintf(cmd_buf, sizeof(cmd_buf)
			, "ebtables -t filter -A WLan_AccessRule -i %s%d -j WLan_AccessRule%d"
			, s_ra_if, ra_idx, idx);
		system(cmd_buf);

		snprintf(cmd_buf, sizeof(cmd_buf),
			"ebtables -t filter -F WLan_AccessRule%d && ebtables -t filter -Z WLan_AccessRule%d"
			, idx, idx);
		system(cmd_buf);
		
		commit_one_node(idx, 0);
	}
	/* update guest ssid rule only once*/		
	update_guest_ssid_rule();


	/* check first init */
	snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE, 1);

	/* need re-init. */
	if ( cfg_get_object_attr(node_name, "entryidx", entry_idx, sizeof(entry_idx)) <= 0 )
	{
		snprintf(wlan_node_name, sizeof(wlan_node_name), WLAN_COMMON_NODE);
		bzero(bssid_num, sizeof(bssid_num));
		if ( cfg_get_object_attr(wlan_node_name, "BssidNum", bssid_num, sizeof(bssid_num)) > 0 )
			bssid_num_i = atoi(bssid_num);
		last_index  = 1;
		for ( idx = 0; idx < bssid_num_i; idx ++ )
		{
			snprintf(wlan_node_name, sizeof(wlan_node_name), WLAN_ENTRY_N_NODE, idx + 1);
			if ( cfg_get_object_attr(wlan_node_name, "SSID", ssid_val, sizeof(ssid_val)) > 0 )
			{
				/* init mapping index */
#if defined(TCSUPPORT_CT_UBUS)
				snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE, idx + 1);
#else
				snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE, mapping_pos + 1);
#endif
				if ( cfg_query_object(node_name, NULL, NULL) < 0 )
					cfg_create_object(node_name);
				snprintf(entry_idx, sizeof(entry_idx), "%d", idx);
				cfg_set_object_attr(node_name, "entryidx", entry_idx);
				bzero(buf, sizeof(buf));
				snprintf(buf, sizeof(buf), "2.4G-%d", last_index);
				cfg_set_object_attr(node_name, "SSIDAlias", buf);
				last_index++;

#if !defined(TCSUPPORT_CT_UBUS)
				/* init reverse mapping index */
				snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE, idx + 1);
				if ( cfg_query_object(node_name, NULL, NULL) < 0 )
					cfg_create_object(node_name);
				snprintf(entry_idx, sizeof(entry_idx), "%d", mapping_pos);
				cfg_set_object_attr(node_name, "reverseidx", entry_idx);

				mapping_pos ++;
#endif
			}
		}
		bzero(buf, sizeof(buf));
		snprintf(buf, sizeof(buf), "%d", last_index);
		cfg_set_object_attr(WLAN_COMMON_NODE, "LastIndex", buf);
		snprintf(wlan_node_name, sizeof(wlan_node_name), WLAN11AC_COMMON_NODE);

		bzero(bssid_num, sizeof(bssid_num));
		bssid_num_i = 0;
		if ( cfg_get_object_attr(wlan_node_name, "BssidNum", bssid_num, sizeof(bssid_num)) > 0 )
			bssid_num_i = atoi(bssid_num);
		last_index = 1;
		for ( idx = 0; idx < bssid_num_i; idx ++ )
		{
			snprintf(wlan_node_name, sizeof(wlan_node_name), WLAN11AC_ENTRY_N_NODE, idx + 1);
			if ( cfg_get_object_attr(wlan_node_name, "SSID", ssid_val, sizeof(ssid_val)) > 0 )
			{
				/* init mapping index */
#if defined(TCSUPPORT_CT_UBUS)
				snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE, idx + MAX_WLAN_2_4_COUNT + 1);
#else
				snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE
												, mapping_pos + 1);
#endif
				if ( cfg_query_object(node_name, NULL, NULL) < 0 )
					cfg_create_object(node_name);
				snprintf(entry_idx, sizeof(entry_idx), "%d"
												, MAX_WLAN_2_4_COUNT + idx);
				cfg_set_object_attr(node_name, "entryidx", entry_idx);
				bzero(buf, sizeof(buf));
				snprintf(buf, sizeof(buf), "5G-%d", last_index);
				cfg_set_object_attr(node_name, "SSIDAlias", buf);
				last_index++;

#if !defined(TCSUPPORT_CT_UBUS)
				/* init reverse mapping index */
				snprintf(node_name, sizeof(node_name), WLANMAPPING_ENTRY_N_NODE
												, MAX_WLAN_2_4_COUNT + idx + 1);
				if ( cfg_query_object(node_name, NULL, NULL) < 0 )
					cfg_create_object(node_name);
				snprintf(entry_idx, sizeof(entry_idx), "%d", mapping_pos);
				cfg_set_object_attr(node_name, "reverseidx", entry_idx);

				mapping_pos ++;
#endif
			}
		}
		bzero(buf, sizeof(buf));
		snprintf(buf, sizeof(buf), "%d", last_index);
		cfg_set_object_attr(WLAN11AC_COMMON_NODE, "LastIndex", buf);
	}

	return 0;
}

