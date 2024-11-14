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

#ifndef __CFG_TYPE_TRANSFERSERVICES_H__
#define __CFG_TYPE_TRANSFERSERVICES_H__

#define BuffSet_Str(x, y) 					snprintf(x, sizeof(x), "%s", y)
#define BuffSet_Format(x, y, z) 			snprintf(x, sizeof(x), y, z)

#define TF_ALL_ADDR_PATH 					"/etc/TF_all_addr.sh"
#define TF_RMT_DOMAIN_PATH 					"/etc/TF_rmt_domain.sh"
#define TF_DST_DOMAIN_PATH 					"/etc/TF_dst_domain.sh"
#define TF_ALL_DOMAIN_PATH 					"/etc/TF_all_domain.sh"

#define INSMOD_IP6T_MODULES	"insmod /lib/modules/%s/kernel/net/ipv6/netfilter/%s"
#define RMMOD_IP6T_MODULES	"rmmod /lib/modules/%s/kernel/net/ipv6/netfilter/%s"
#define IP6T_NAT_KO						"ip6table_nat.ko"
#define IP6T_FILTER_KO					"ip6table_filter.ko"

#define TF_REDIRECT_CHAIN		"TF_REDIRECT_CHAIN"
#define TF_BLOCK_CHAIN			"TF_BLOCK_CHAIN"

#define MAX_FLOWNUM_RULE 					255
#define TRAFFIC_IP_ADDR_STR_MAX_LEN			128
#define TRAFFIC_FLOW_RULE_NUM				255
#define TRAFFIC_MONITOR_TIME_OUT			30
#define MAX_DOMAIN_LEN						256

#define FORWARD 							"TrafficForward"
#define RULE_REMOTE_DOMAIN_MODE 			"domainMode"

#define TRAFFIC_QOS_NODE_NAME 				"TrafficQoSService"
#define FLOW_UP_SPEED 						"upSpeed"
#define FLOW_DOWN_SPEED 					"downSpeed"
#define FLOW_DSCP_REMARK 					"remarkDSCP"
#define RULE_REMOTE_ADDRESS 				"remoteAddress"
#define RULE_REMOTE_PORT 					"remotePort"
#define RULE_REMOTE_DOMAIN_MODE 			"domainMode"
#define RULE_REMOTE_DOMAIN_ADDR 			"domainAddr"
#define FLOW_UP_STATS 						"UsStats"
#define FLOW_DOWN_STATS 					"DsStats"
#define FLOW_CLASSIFY_NUM 					"flowClassNUM"
#define FLOW_UP_STATS_SPEED 				"upStatsSpeed"
#define FLOW_DOWN_STATS_SPEED 				"downStatsSpeed"
#define TRAFFIC_CTRL_BITMAP 				"/proc/"TRAFFIC_CLASSIFY_PROC_DIR"/"TRAFFIC_CLASSIFY_PROC_DEBUG_FILE
#define DISABLE_TRAFFIC 					"echo set ctrl_bit_map 0 > "TRAFFIC_CTRL_BITMAP
#define ENABLE_TRAFFIC 						"echo set ctrl_bit_map 18 > "TRAFFIC_CTRL_BITMAP

#define TRAFFIC_DETAIL_NODE_NAME			"TrafficDetailProcess"
#define TRAFFIC_DETAIL_RULENUM				"remoteAddrCount"
#define DETAIL_BUNDLE_NAME					"BundleName"
#define MIRROR 								"TrafficMirror"
#define WAN_IFNAME							"IFName"
#define WAN_IF_NODE 						"WanInfo"
#define WAN_IF_COMMON						"Common"
#define SUB_NODE_NAME						"Entry"
#define MONITOR 							"TrafficMonitor"
#define MONITOR_BUNDLE_NAME					"BundleName"
#define MONITOR_DESTADDR					"destAddr"
#define MONITOR_TIMEOUT						"monitorTimeout"
#define MONITOR_ADDRESS_RULENUM				"destAddrCount"

int traffic_qos_service_rate_update(void);
int traffic_qos_service_domain_resolve(void);
int traffic_forward_domain_resolve(void);
#endif

