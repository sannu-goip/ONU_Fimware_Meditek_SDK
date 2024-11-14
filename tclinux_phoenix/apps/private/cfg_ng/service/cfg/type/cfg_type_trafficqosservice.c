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
#include <stdlib.h>
#include <string.h> 
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "utility.h"
#include "modules/traffic_classify_global/traffic_classify_global.h"
#include "libapi_lib_flowmgr.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "cfg_type_transferservices.h"
#include "blapi_traffic.h"
#include "traffic/global_dnshost.h"

#define QUEUE_ACTION_SET (E_QUEUE_MAPPING_ACTION | E_TOS_REMARK_ACTION)
#define MIN(a, b) (((a)<(b))?(a):(b))

extern cfg_node_type_t cfg_type_trafficqosservice_pvc;
#define MAX_FLOW_NUM 8


int traffic_flow_rule_setup_port_only
(int flow_idx, int rule_idx, int i_dscpRemark
, int ip_match_type, int port_match_type
, int port_s, int port_e)
{
	ecn_traffic_classify_info_t flow_rule = {0};

	bzero(&flow_rule, sizeof(flow_rule));
	flow_rule.port = E_ETH_PORT_0;
	flow_rule.act_element.acl_mode = E_FORWARD;
	flow_rule.act_element.action_bit_map = QUEUE_ACTION_SET;
	flow_rule.act_element.remark_queue = flow_idx + 1;
	if ( i_dscpRemark > 0 )
		flow_rule.act_element.remark_tos = i_dscpRemark;
	
	/* field element flow configure. */
	flow_rule.fld_element.fld_bit_map = ECN_FLD_DEST_IP_MASK_PORT_RANGE_TYPE;
	flow_rule.fld_element.ipv4_ip_match_type = ip_match_type;
	flow_rule.fld_element.ipv4_port_match_type = port_match_type;
	flow_rule.fld_element.l4_ipv4_dst_port = port_s;
	flow_rule.fld_element.ipv4_dst_port_end = port_e;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl LAN flow failed[%d].\n"
		, rule_idx);
	flow_rule.fld_element.ipv4_ip_match_type = 0;
	flow_rule.fld_element.ipv4_port_match_type = 0;
	flow_rule.fld_element.l4_ipv4_dst_port = 0;
	flow_rule.fld_element.ipv4_dst_port_end = 0;
	
	flow_rule.fld_element.fld_bit_map = ECN_FLD_DEST_IPV6_MASK_PORT_RANGE_TYPE;
	flow_rule.fld_element.ipv6_ip_match_type = ip_match_type;
	flow_rule.fld_element.ipv6_port_match_type = port_match_type;
	flow_rule.fld_element.l4_ipv6_dst_port = port_s;
	flow_rule.fld_element.ipv6_dst_port_end = port_e;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl LAN flow failed[%d].\n"
		, rule_idx);
	flow_rule.fld_element.ipv6_ip_match_type = 0;
	flow_rule.fld_element.ipv6_port_match_type = 0;
	flow_rule.fld_element.l4_ipv6_dst_port = 0;
	flow_rule.fld_element.ipv6_dst_port_end = 0;
	
	/* configure it to WAN port */
	flow_rule.port = E_WAN_PORT;
	/* NO NEED DSCP for down*/
	flow_rule.act_element.action_bit_map = E_QUEUE_MAPPING_ACTION;
 	flow_rule.fld_element.fld_bit_map = ECN_FLD_SRC_IP_MASK_PORT_RANGE_TYPE;
	flow_rule.fld_element.ipv4_ip_match_type = ip_match_type;
	flow_rule.fld_element.ipv4_port_match_type = port_match_type;
	flow_rule.fld_element.l4_ipv4_src_port = port_s;
	flow_rule.fld_element.ipv4_src_port_end = port_e;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl WAN flow failed[%d].\n"
		, rule_idx); 
	flow_rule.fld_element.ipv4_ip_match_type = 0;
	flow_rule.fld_element.ipv4_port_match_type = 0;
	flow_rule.fld_element.l4_ipv4_src_port = 0;
	flow_rule.fld_element.ipv4_src_port_end = 0;

	flow_rule.fld_element.fld_bit_map = ECN_FLD_SRC_IPV6_MASK_PORT_RANGE_TYPE;
	flow_rule.fld_element.ipv6_ip_match_type = ip_match_type;
	flow_rule.fld_element.ipv6_port_match_type = port_match_type;
	flow_rule.fld_element.l4_ipv6_src_port = port_s;
	flow_rule.fld_element.ipv6_src_port_end = port_e;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl WAN flow failed[%d].\n"
		, rule_idx); 

	return 0;
}

int traffic_flow_rule_setup_v4
(int flow_idx, int rule_idx, int i_dscpRemark
, int ip_match_type, int port_match_type
, struct in_addr *p_in_s_v4addr, int ip_mask, int port_s, int port_e)
{
	ecn_traffic_classify_info_t flow_rule = {0};
	int idx = 0;

	if ( !p_in_s_v4addr )
		return -1;

	bzero(&flow_rule, sizeof(flow_rule));
	flow_rule.port = E_ETH_PORT_0;
	flow_rule.act_element.acl_mode = E_FORWARD;
	flow_rule.act_element.action_bit_map = QUEUE_ACTION_SET;
	flow_rule.act_element.remark_queue = flow_idx + 1;
	if ( i_dscpRemark > 0 )
		flow_rule.act_element.remark_tos = i_dscpRemark;
	
	/* field element flow configure. */
	flow_rule.fld_element.fld_bit_map =
					ECN_FLD_DEST_IP_MASK_PORT_RANGE_TYPE;
	flow_rule.fld_element.ipv4_ip_match_type = ip_match_type;
	flow_rule.fld_element.ipv4_port_match_type = port_match_type;
	memcpy(&flow_rule.fld_element.ipv4_dst_ip
		 , p_in_s_v4addr
		 , sizeof(struct in_addr));
	for ( idx = 0; idx < ip_mask; idx++ )
		flow_rule.fld_element.ipv4_dst_ip_mask |= (0x80000000>>idx);
	flow_rule.fld_element.l4_ipv4_dst_port = port_s;
	flow_rule.fld_element.ipv4_dst_port_end = port_e;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl LAN flow failed[%d].\n"
		, rule_idx);
	
	/* configure it to WAN port */
	flow_rule.port = E_WAN_PORT;
	/* NO NEED DSCP for down*/
	flow_rule.act_element.action_bit_map = E_QUEUE_MAPPING_ACTION;
	flow_rule.fld_element.fld_bit_map = 
					ECN_FLD_SRC_IP_MASK_PORT_RANGE_TYPE;
	memcpy(&flow_rule.fld_element.ipv4_src_ip
		 , p_in_s_v4addr
		 , sizeof(struct in_addr));
	memcpy(&flow_rule.fld_element.ipv4_src_ip_mask
		 , &flow_rule.fld_element.ipv4_dst_ip_mask
		 , sizeof(flow_rule.fld_element.ipv4_dst_ip_mask));
	flow_rule.fld_element.l4_ipv4_src_port = port_s;
	flow_rule.fld_element.ipv4_src_port_end = port_e;

	memset(&flow_rule.fld_element.ipv4_dst_ip, 0, sizeof(struct in_addr));
	flow_rule.fld_element.ipv4_dst_ip_mask = 0;
	flow_rule.fld_element.l4_ipv4_dst_port = 0;
	flow_rule.fld_element.ipv4_dst_port_end = 0;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl WAN flow failed[%d].\n"
		, rule_idx);

	return 0;
}

int traffic_flow_rule_setup_v6
(int flow_idx, int rule_idx, int i_dscpRemark
, int ip_match_type, int port_match_type
, struct in6_addr *p_in_s_v6addr, int ip_mask, int port_s, int port_e)
{
	ecn_traffic_classify_info_t flow_rule = {0};
	unsigned short idx = 0;
	unsigned short ipv6_mask_idx = 0;

	if ( !p_in_s_v6addr )
		return -1;

	bzero(&flow_rule, sizeof(flow_rule));
	flow_rule.port = E_ETH_PORT_0;
	flow_rule.act_element.acl_mode = E_FORWARD;
	flow_rule.act_element.action_bit_map = QUEUE_ACTION_SET;
	flow_rule.act_element.remark_queue = flow_idx + 1;
	if ( i_dscpRemark > 0 )
		flow_rule.act_element.remark_tos = i_dscpRemark;
	
	/* field element flow configure. */
	flow_rule.fld_element.fld_bit_map =
					ECN_FLD_DEST_IPV6_MASK_PORT_RANGE_TYPE;
	flow_rule.fld_element.ipv6_ip_match_type = ip_match_type;
	flow_rule.fld_element.ipv6_port_match_type = port_match_type;
	memcpy(&flow_rule.fld_element.ipv6_daddr
		 , p_in_s_v6addr
		 , sizeof(struct in6_addr));
	for ( idx = 0; idx < ip_mask; idx++ )
	{
		ipv6_mask_idx = idx / 16;
		flow_rule.fld_element.ipv6_dst_ip_mask[ipv6_mask_idx] |= (0x8000>>(idx-ipv6_mask_idx*16));
	}
	flow_rule.fld_element.l4_ipv6_dst_port = port_s;
	flow_rule.fld_element.ipv6_dst_port_end = port_e;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl LAN flow failed[%d].\n"
		, rule_idx);
	
	/* configure it to WAN port */
	flow_rule.port = E_WAN_PORT;
	/* NO NEED DSCP for down*/
	flow_rule.act_element.action_bit_map = E_QUEUE_MAPPING_ACTION;
	flow_rule.fld_element.fld_bit_map = 
					ECN_FLD_SRC_IPV6_MASK_PORT_RANGE_TYPE;
	memcpy(&flow_rule.fld_element.ipv6_saddr
		 , p_in_s_v6addr
		 , sizeof(struct in6_addr));
	memcpy(&flow_rule.fld_element.ipv6_src_ip_mask
		 , &flow_rule.fld_element.ipv6_dst_ip_mask
		 , sizeof(flow_rule.fld_element.ipv6_dst_ip_mask));
	flow_rule.fld_element.l4_ipv6_src_port = port_s;
	flow_rule.fld_element.ipv6_src_port_end = port_e;

	memset(&flow_rule.fld_element.ipv6_daddr, 0, sizeof(struct in6_addr));
	memset(&flow_rule.fld_element.ipv6_dst_ip_mask, 0, sizeof(flow_rule.fld_element.ipv6_dst_ip_mask));
	flow_rule.fld_element.l4_ipv6_dst_port = 0;
	flow_rule.fld_element.ipv6_dst_port_end = 0;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_ioctl(TRAFFIC_CLASSIFY_ADD_ENTRY, &flow_rule) )
		printf("\ntraffic flowmgr_lib_ioctl WAN flow failed[%d].\n"
		, rule_idx);

	return 0;
}

static void switchtoflowcount()
{
	char nodename[64]={0};
	char def_route_index_v4[10] = {0}, def_route_index_v6[10] = {0};
	int i_def_route_index_v4 = -1, i_def_route_index_v6 = -1;
	char wanname[20] = {0};
	char cmd[60] = {0};

	memset(nodename, 0,sizeof(nodename));
	
	snprintf(nodename, sizeof(nodename)-1, WANINFO_COMMON_NODE);
	if ( 0 < cfg_obj_get_object_attr(nodename, "DefRouteIndexv4", 0, def_route_index_v4, sizeof(def_route_index_v4))
		&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A") )
	{
		i_def_route_index_v4 = atoi(def_route_index_v4);
	}
	
	if ( 0 < cfg_obj_get_object_attr(nodename, "DefRouteIndexv6", 0, def_route_index_v6, sizeof(def_route_index_v6))
		&& '\0' != def_route_index_v6[0] && 0 != strcmp(def_route_index_v6, "N/A") )
	{
		i_def_route_index_v6 = atoi(def_route_index_v6);
	}
	
	if ( i_def_route_index_v4 >= 0 )
	{
		memset(nodename, 0,sizeof(nodename));
		snprintf(nodename,sizeof(nodename)-1,"root.wan.pvc.%d.entry.%d",i_def_route_index_v4/8+1,i_def_route_index_v4%8+1);
		cfg_obj_get_object_attr(nodename, "IFName", 0, wanname, sizeof(wanname));
		if(strlen(wanname) != 0)
		{
			blapi_traffic_set_default_wan_interface(wanname);
		}
	}

	if ( i_def_route_index_v6 >= 0 )
	{
		memset(nodename, 0,sizeof(nodename));
		snprintf(nodename,sizeof(nodename)-1,"root.wan.pvc.%d.entry.%d",i_def_route_index_v6/8+1,i_def_route_index_v6%8+1);
		cfg_obj_get_object_attr(nodename, "IFName", 0, wanname, sizeof(wanname));
		if(strlen(wanname) != 0)
		{
			blapi_traffic_set_default_wan_interface(wanname);
		}
	}
}

int traffic_qos_service_commit(int isBoot)
{
	char nodeName[64] = {0};
	int flow_idx = 0, rule_idx = 0, flow_num = 0;
	int i_upspeed = 0, i_downspeed = 0, i_dscpRemark = 0;
	unsigned char direction = 0, flowid = 0;
	unsigned int ratelimit = 0;
	char upSpeed[64] = {0}, downSpeed[64] = {0};
	char remoteAddress[256] ={0}, remotePort[64] = {0};
	char start_addr[80]= {0}, dscpRemark[16] = {0};
	char s_flow_class_num[32] = {0};
	int port_s = 0, port_e = 0;
	int ip_mask = 0, is_exist = 0;
	struct in_addr in_s_v4addr = {0};
	struct in6_addr in_s_v6addr = {0}; 
	ecn_traffic_classify_info_t flow_rule = {0};
	e_match_ip_type ip_match_type = E_MATCH_IP_IGNORE;
	e_match_port_type port_match_type = E_MATCH_PORT_IGNORE;
	int ip4_type = 0,ip6_type = 0;
	int index = 0;
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL, *p_res = NULL;
	int domain_type = 0;
	char *str = NULL ;

	if(Product_judge() == 1)
	{
		switchtoflowcount();
	}

	/* close traffic classify */
	system(DISABLE_TRAFFIC);

	/* clean all traffic flow LAN & WAN. */
	bzero(&flow_rule, sizeof(flow_rule));
	flow_rule.port = E_ETH_PORT_0;
	flow_rule.act_element.action_bit_map |= QUEUE_ACTION_SET;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_clean(&flow_rule) )
		printf("\ntraffic flowmgr_lib_clean LAN flow failed.\n"); 

	bzero(&flow_rule, sizeof(flow_rule));
	flow_rule.port = E_WAN_PORT;
	flow_rule.act_element.action_bit_map |= QUEUE_ACTION_SET;
	if ( ECNT_E_FLOW_SUCCESS != 
		flowmgr_lib_clean(&flow_rule) )
		printf("\ntraffic flowmgr_lib_clean WAN flow failed.\n"); 

	sendTodnshost(BIT_QOS, DEL_ACT, NULL);
	for ( flow_idx = 0; flow_idx < MAX_FLOW_NUM; flow_idx ++ )
	{
		/* clean flow rate limit first */
		if ( blapi_traffic_set_flowid_ratelimit(FLOWID_RATELIMIT_DIR_UP, flow_idx
		, 0) )
			printf("\ntraffic clean trqos_flow_ratelimit up failed.\n");
		if ( blapi_traffic_set_flowid_ratelimit(FLOWID_RATELIMIT_DIR_DOWN, flow_idx
		, 0) )
			printf("\ntraffic clean trqos_flow_ratelimit down failed.\n");

		bzero(nodeName, sizeof(nodeName));
		index = snprintf(nodeName, sizeof(nodeName), TRAFFICQOSSERVICE_PVC_NODE, flow_idx+1);

		if ( isBoot )
		{
			if ( cfg_query_object(nodeName, NULL, NULL) < 0 )
				cfg_create_object(nodeName);
			cfg_set_object_attr(nodeName, FLOW_UP_STATS_SPEED, "0");
			cfg_set_object_attr(nodeName, FLOW_DOWN_STATS_SPEED, "0");
		}

		i_upspeed = 0;
		i_downspeed = 0;
		if ( 0 < cfg_obj_get_object_attr(nodeName, FLOW_UP_SPEED, 0, upSpeed, sizeof(upSpeed))
			&& 0 != upSpeed[0] )
			i_upspeed = atoi(upSpeed);
		if ( 0 < cfg_obj_get_object_attr(nodeName, FLOW_DOWN_SPEED, 0, downSpeed, sizeof(downSpeed))
			&& 0 != downSpeed[0] )
			i_downspeed = atoi(downSpeed);

		/* Speed uint --> Kbps */
		if ( i_upspeed > 0 )
		{
			direction = FLOWID_RATELIMIT_DIR_UP;
			flowid = flow_idx;
			ratelimit = i_upspeed * 1024;

			if ( blapi_traffic_set_flowid_ratelimit(direction, flowid
				, ratelimit) )
				printf("\ntraffic set_trqos_flow_ratelimit up failed.\n");
		}

		if ( i_downspeed > 0 )
		{
			direction = FLOWID_RATELIMIT_DIR_DOWN;
			flowid = flow_idx;
			ratelimit = i_downspeed * 1024;

			if ( blapi_traffic_set_flowid_ratelimit(direction, flowid
				, ratelimit) )
				printf("\ntraffic set_trqos_flow_ratelimit down failed.\n");
		}

		i_dscpRemark = 0;
		if ( 0 < cfg_obj_get_object_attr(nodeName, FLOW_DSCP_REMARK, 0, dscpRemark, sizeof(dscpRemark))
			&& 0 != dscpRemark[0] )
		{
			i_dscpRemark = (atoi(dscpRemark) & 0x7f) + 1;
		}

		flow_num = 0;
		/* IP class */
		for ( rule_idx = 0; rule_idx < TRAFFIC_FLOW_RULE_NUM; rule_idx++ )
		{
			is_exist = 0;
			ip4_type = 0;
			ip6_type = 0;
			snprintf(nodeName+index, sizeof(nodeName)-index, ".Entry.%d", rule_idx+1);
			if ( 0 > cfg_obj_get_object_attr(nodeName, RULE_REMOTE_ADDRESS, 0, remoteAddress, sizeof(remoteAddress)) )
				remoteAddress[0] = 0;
			else
				is_exist = 1;

			if ( 0 > cfg_obj_get_object_attr(nodeName, RULE_REMOTE_PORT, 0, remotePort, sizeof(remotePort)) )
				remotePort[0] = 0;
			else
				is_exist = 1;

			if ( is_exist )
				flow_num ++;

			ip_match_type = E_MATCH_IP_ALL_TYPE;
			if ( 0 != remoteAddress[0] )
			{
				domain_type = 1;
				ip_mask = 0;
				sendTodnshost(BIT_QOS, ADD_ACT, remoteAddress);
				cfg_set_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, "");
				if(NULL != (str = strstr(remoteAddress, "://")))
				{
					strncpy(remoteAddress,str+strlen("://"),sizeof(remoteAddress)-1);
				}

				
				if ( NULL != strstr(remoteAddress, "/") ) /* IP range*/
				{
					/* IP mask range */
					sscanf(remoteAddress, "%[^/]/%d", start_addr, &ip_mask);
					if( NULL == strstr(remoteAddress, ":")  )
					{
						if ( 1 != inet_pton(AF_INET, start_addr, &in_s_v4addr) )
							continue;
						if(ip_mask > 32)
							continue;
						ip4_type = 1;
					}
					else
					{/* IPv6 */
						if ( 1 != inet_pton(AF_INET6, start_addr, &in_s_v6addr) )
							continue;
						if(ip_mask > 128)
							continue;
						ip6_type =1;
					}
					domain_type = 0;
					ip_match_type = E_MATCH_IP_MASK_TYPE;
				}

				if(domain_type == 1)
				{
					bzero(&hints, sizeof(hints));
					hints.ai_socktype = SOCK_STREAM;
					
					if(0 != getaddrinfo(remoteAddress, NULL, &hints, &res))
						continue;

					for ( p_res = res; p_res; p_res = p_res->ai_next )
					{
						if ( AF_INET != p_res->ai_family && AF_INET6 != p_res->ai_family )
							continue;
						
						if ( AF_INET == p_res->ai_family)
						{
							memcpy(&in_s_v4addr, &(((struct sockaddr_in *) p_res->ai_addr)->sin_addr),sizeof(struct in_addr));
							ip4_type = 1;
						}
						else if ( AF_INET6 == p_res->ai_family )
						{
							memcpy(&in_s_v6addr, &(((struct sockaddr_in6 *) p_res->ai_addr)->sin6_addr),sizeof(struct in6_addr));
							ip6_type = 1;
						}
					}
				}

				if(ip_mask == 0)
				{
					if( ip4_type == 1)
					{/* IPv4 */
						ip_mask = 32;
					}
					else if( ip6_type == 1)
					{/* IPv6 */
						ip_mask = 128;
					}
					ip_match_type = E_MATCH_IP_ONLY_TYPE;
				}
				if ( 0 == ip_mask )
					continue;
			
			}

			if ( 0 != remotePort[0] )
			{
				if ( NULL != strstr(remotePort, "-") ) /* PORT range */
				{
					sscanf(remotePort, "%d-%d", &port_s, &port_e);
					port_match_type = E_MATCH_PORT_RANGE_TYPE;
				}
				else if ( NULL != strstr(remotePort, "!") ) /* PORT NOR */
				{
					sscanf(remotePort, "%*[!]%d", &port_s);
					port_match_type = E_MATCH_EXCLUDE_PORT_TYPE;
				}
				else /* single PORT */
				{
					sscanf(remotePort, "%d", &port_s);
					if ( 0 == port_s )
						port_match_type = E_MATCH_PORT_ALL_TYPE;
					else
						port_match_type = E_MATCH_PORT_ONLY_TYPE;
				}
			}
			else
				continue;

			if ( 0 == remoteAddress[0] )/* set port rule for IPv4 & IPv6 */
			{
				traffic_flow_rule_setup_port_only
					(flow_idx, rule_idx, i_dscpRemark
					, ip_match_type, port_match_type
					, port_s, port_e);
			}
			else
			{
				if( ip4_type == 1 && ip6_type == 1)
				{
					traffic_flow_rule_setup_v4
					(flow_idx, rule_idx, i_dscpRemark
					, ip_match_type, port_match_type
					, &in_s_v4addr, ip_mask, port_s, port_e);
				}
				else if( ip4_type == 1 )
				{
					traffic_flow_rule_setup_v4
					(flow_idx, rule_idx, i_dscpRemark
					, ip_match_type, port_match_type
					, &in_s_v4addr, ip_mask, port_s, port_e);
					
				}
				else if(ip6_type == 1)
				{
					traffic_flow_rule_setup_v6
					(flow_idx, rule_idx, i_dscpRemark
					, ip_match_type, port_match_type
					, &in_s_v6addr, ip_mask, port_s, port_e);
				}
			}
		}		

		/* update rule num */
		BuffSet_Format(s_flow_class_num, "%d", flow_num);
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TRAFFICQOSSERVICE_PVC_NODE, flow_idx+1);
		cfg_set_object_attr(nodeName, FLOW_CLASSIFY_NUM, s_flow_class_num);
	}

	system(ENABLE_TRAFFIC);
	system("/userfs/bin/hw_nat	-!");

	return 0;

}



/*for start cc*/
int traffic_qos_service_rate_update(void)
{
	char statbuf[64] = {0};
	char statBuf_up[64] = {0}, statBuf_down[64] = {0};
	int flow_idx = 0, flow_num = 0;
	char nodeName[64] = {0};
	unsigned long long  l_UpStat = 0, l_DownStat = 0;
	unsigned long long speed_up = 0, speed_down = 0, diff_stat = 0;
	unsigned char direction = 0;
	static struct timespec t_pre_time[MAX_FLOW_NUM];
	static unsigned long long l_preUpStat[MAX_FLOW_NUM] = {0};
	static unsigned long long l_preDownStat[MAX_FLOW_NUM] = {0};
	struct timespec t_stat[MAX_FLOW_NUM];
	char s_flow_class_num[32] = {0};
	int diff_msec = 0;
	int i_upspeed = 0, i_downspeed = 0;
	char upSpeed[64] = {0}, downSpeed[64] = {0};
	char isboot[4] = {0};
	int flag = -1;

	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "trafficqosboot", 0, isboot, sizeof(isboot));

	if ( !atoi(isboot) )
	{
		bzero(t_pre_time, sizeof(t_pre_time));
		bzero(l_preUpStat, sizeof(l_preUpStat));
		bzero(l_preDownStat, sizeof(l_preDownStat));
		return 0;
	}

	for ( flow_idx = 0; flow_idx < MAX_FLOW_NUM; flow_idx ++ )
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TRAFFICQOSSERVICE_PVC_NODE, flow_idx+1);
	
		if ( 0 > cfg_obj_get_object_attr(nodeName, FLOW_CLASSIFY_NUM, 0, s_flow_class_num, sizeof(s_flow_class_num)) )
			BuffSet_Str(s_flow_class_num, "0");
		flow_num = atoi(s_flow_class_num);
		if ( flow_num <= 0 )
			continue;

		i_upspeed = 0;
		i_downspeed = 0;
		if ( 0 < cfg_obj_get_object_attr(nodeName, FLOW_UP_SPEED, 0, upSpeed, sizeof(upSpeed))
			&& 0 != upSpeed[0] )
			i_upspeed = atoi(upSpeed);
		if ( i_upspeed > 2048 )
			i_upspeed -= MIN(500, i_upspeed*0.05);
		if ( 0 < cfg_obj_get_object_attr(nodeName, FLOW_DOWN_SPEED, 0, downSpeed, sizeof(downSpeed))
			&& 0 != downSpeed[0] )
			i_downspeed = atoi(downSpeed);
		if ( i_downspeed > 2048 )
			i_downspeed -= MIN(500, i_downspeed*0.05);
		
		clock_gettime(CLOCK_MONOTONIC, &t_stat[flow_idx]);

		direction = UP_DIR;
		l_UpStat = 0;
		
		blapi_traffic_get_flowid_cnt(direction, flow_idx, &l_UpStat, &flag);
		
		BuffSet_Format(statBuf_up, "%llu", l_UpStat);
		cfg_set_object_attr(nodeName, FLOW_UP_STATS, statBuf_up);

		direction = DOWN_DIR;
		l_DownStat = 0;

		blapi_traffic_get_flowid_cnt(direction, flow_idx, &l_DownStat, &flag);

		BuffSet_Format(statBuf_down, "%llu", l_DownStat);
		cfg_set_object_attr(nodeName, FLOW_DOWN_STATS, statBuf_down);

		speed_up = speed_down = 0;
		diff_msec = (t_stat[flow_idx].tv_sec - t_pre_time[flow_idx].tv_sec)
					* 1000 
					+ (t_stat[flow_idx].tv_nsec - t_pre_time[flow_idx].tv_nsec )
					/ 1000000;
		
		if ( diff_msec > 0  )
		{
			diff_stat = (l_UpStat > l_preUpStat[flow_idx]) ?
					(l_UpStat - l_preUpStat[flow_idx]) : 0;
			speed_up = (diff_stat * 8 * 1000) / diff_msec / 1024;

			if(flag == 1)
			{
			if ( i_upspeed > 0 && speed_up > i_upspeed )
				speed_up = i_upspeed;
			}

			diff_stat = (l_DownStat > l_preDownStat[flow_idx]) ? 
				(l_DownStat - l_preDownStat[flow_idx]) : 0;
			speed_down = (diff_stat * 8 * 1000) / diff_msec / 1024;

			if(flag == 1)
			{
			if ( i_downspeed > 0 && speed_down > i_downspeed )
				speed_down = i_downspeed;
			}	
		}

		BuffSet_Format(statbuf, "%llu", speed_up);
		cfg_set_object_attr(nodeName, FLOW_UP_STATS_SPEED, statbuf);
		BuffSet_Format(statbuf, "%llu", speed_down);
		cfg_set_object_attr(nodeName, FLOW_DOWN_STATS_SPEED, statbuf);

		l_preUpStat[flow_idx] = l_UpStat;
		l_preDownStat[flow_idx] = l_DownStat;
		memcpy(&t_pre_time[flow_idx], &t_stat[flow_idx]
				, sizeof(struct timespec));

	}

	return 0;
}


/*for start cc*/
int traffic_qos_service_domain_resolve(void)
{
	int flow_idx = 0, flow_num = 0, rule_idx = 0;
	char nodeName[64] = {0};
	char s_flow_class_num[32] = {0}, domain_mode[24] = {0}, domain_addr[50] = {0};
	char remoteAddress[256] = {0}, remotePort[64] = {0}, dscpRemark[16] = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL, *p_res = NULL;
	struct sockaddr_in *cvtaddr2 = NULL;
	struct sockaddr_in6 *cvtaddr2_v6 = NULL;
	e_match_ip_type ip_match_type = E_MATCH_IP_IGNORE;
	e_match_port_type port_match_type = E_MATCH_PORT_IGNORE;
	int port_s = 0, port_e = 0, i_dscpRemark = 0, ip_mask = 0;
	int isNew_domain = 0;
	char isboot[4] = {0};
	char attr[16] = {0};
	char parsedone[16] = {0};
	int parsedone_val[8] = {0};

	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "trafficqosboot", 0, isboot, sizeof(isboot));

	if ( !atoi(isboot) )
	{
		return 0;
	}

	for ( flow_idx = 0; flow_idx < MAX_FLOW_NUM; flow_idx ++ )
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TRAFFICQOSSERVICE_PVC_NODE, flow_idx+1);
	
		if ( 0 > cfg_obj_get_object_attr(nodeName, FLOW_CLASSIFY_NUM, 0, s_flow_class_num, sizeof(s_flow_class_num)) )
			BuffSet_Str(s_flow_class_num, "0");
		flow_num = atoi(s_flow_class_num);
		if ( flow_num <= 0 )
			continue;

		memset(attr, 0, sizeof(attr));
		memset(parsedone, 0, sizeof(parsedone));
		snprintf(attr, sizeof(attr), "parsedone%d", flow_idx+1);
		cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, attr, 0, parsedone, sizeof(parsedone));
		parsedone_val[flow_idx] = atoi(parsedone);
		if(parsedone_val[flow_idx] == flow_num){
			continue;
		}else{
			cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, attr, "0");
			parsedone_val[flow_idx] = 0;
		}

		i_dscpRemark = 0;
		if ( 0 < cfg_obj_get_object_attr(nodeName, FLOW_DSCP_REMARK, 0, dscpRemark, sizeof(dscpRemark))
			&& 0 != dscpRemark[0] )
		{
			i_dscpRemark = (atoi(dscpRemark) & 0x7f) + 1;
		}

		/* IP class */
		for ( rule_idx = 0; rule_idx < TRAFFIC_FLOW_RULE_NUM; rule_idx++ )
		{
			bzero(domain_mode, sizeof(domain_mode));
			bzero(remoteAddress, sizeof(remoteAddress));
			bzero(domain_addr, sizeof(domain_addr));

			snprintf(nodeName, sizeof(nodeName)
					, TRAFFICQOSSERVICE_PVC_ENTRY_NODE, flow_idx+1, rule_idx+1);
			if ( 0 > cfg_obj_get_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, 0, domain_mode, sizeof(domain_mode)) )
				continue;
			
			if ( 0 != strcmp(domain_mode, "Yes") ){
				parsedone_val[flow_idx]++;
				snprintf(parsedone, sizeof(parsedone), "%d", parsedone_val[flow_idx]);
				cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, attr, parsedone);
				continue;
			}

			if ( 0 > cfg_obj_get_object_attr(nodeName, RULE_REMOTE_ADDRESS, 0, remoteAddress, sizeof(remoteAddress))
				|| 0 == remoteAddress[0] )
				continue;

			if ( 0 > cfg_obj_get_object_attr(nodeName, RULE_REMOTE_PORT, 0, remotePort, sizeof(remotePort)) )
				remotePort[0] = 0;

			if ( 0 != remotePort[0] )
			{
				if ( NULL != strstr(remotePort, "-") ) /* PORT range */
				{
					sscanf(remotePort, "%d-%d", &port_s, &port_e);
					port_match_type = E_MATCH_PORT_RANGE_TYPE;
				}
				else if ( NULL != strstr(remotePort, "!") ) /* PORT NOR */
				{
					sscanf(remotePort, "%*[!]%d", &port_s);
					port_match_type = E_MATCH_EXCLUDE_PORT_TYPE;
				}
				else /* single PORT */
				{
					sscanf(remotePort, "%d", &port_s);
					if ( 0 == port_s )
						port_match_type = E_MATCH_PORT_ALL_TYPE;
					else
						port_match_type = E_MATCH_PORT_ONLY_TYPE;
				}
			}
			else
				continue;

			bzero(&hints, sizeof(hints));
			hints.ai_socktype = SOCK_STREAM;

			/* parse domain */
			if(0 != getaddrinfo(remoteAddress, NULL, &hints, &res))
				continue;

			for ( p_res = res; p_res; p_res = p_res->ai_next )
			{
				if ( p_res && AF_INET == p_res->ai_family )
				{
					cvtaddr2 = (struct sockaddr_in *) p_res->ai_addr;
					BuffSet_Format(domain_addr
						, "%s", inet_ntoa(cvtaddr2->sin_addr));

					ip_mask = 32;
					ip_match_type = E_MATCH_IP_ONLY_TYPE;

					traffic_flow_rule_setup_v4
					(flow_idx, rule_idx, i_dscpRemark
					, ip_match_type, port_match_type
					, &cvtaddr2->sin_addr, ip_mask, port_s, port_e);
				}
				else if ( p_res && AF_INET6 == p_res->ai_family )
				{
					cvtaddr2_v6 = (struct sockaddr_in6 *) p_res->ai_addr;
					inet_ntop(AF_INET6, &(cvtaddr2_v6->sin6_addr), domain_addr, INET6_ADDRSTRLEN);

					ip_mask = 128;
					ip_match_type = E_MATCH_IP_ONLY_TYPE;

					traffic_flow_rule_setup_v6
					(flow_idx, rule_idx, i_dscpRemark
					, ip_match_type, port_match_type
					, &cvtaddr2_v6->sin6_addr, ip_mask, port_s, port_e);
				}
			}

			cfg_set_object_attr(nodeName, RULE_REMOTE_DOMAIN_ADDR, domain_addr);
			cfg_set_object_attr(nodeName, RULE_REMOTE_DOMAIN_MODE, "");
			if ( res )
				freeaddrinfo(res);
			isNew_domain = 1;
			parsedone_val[flow_idx]++;
			snprintf(parsedone, sizeof(parsedone), "%d", parsedone_val[flow_idx]);
			cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, attr, parsedone);
		}
	}

	if ( isNew_domain )
		system("/userfs/bin/hw_nat	-!");

	return 0;
}



int cfg_type_traffic_qos_service_func_execute()
{
	char isboot[4] = {0};
	char attr[16] = {0};
	int i;

	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "trafficqosboot", 0, isboot, sizeof(isboot));
	for(i = 1; i <= MAX_FLOW_NUM; i++)
	{
		memset(attr, 0, sizeof(attr));
		snprintf(attr, sizeof(attr), "parsedone%d", i);
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, attr, "0");
	}
	return traffic_qos_service_commit(0);
}


#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
int svc_cfg_boot_trafficqosservice(void)
{
	char attr[16] = {0};
	int i;

	cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "trafficqosboot", "1");
	for(i = 1; i <= MAX_FLOW_NUM; i++)
	{
		memset(attr, 0, sizeof(attr));
		snprintf(attr, sizeof(attr), "parsedone%d", i);
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, attr, "0");
	}
	return traffic_qos_service_commit(1);
}
#endif


static int cfg_type_trafficqosservice_func_commit(char* path)
{
	cfg_type_traffic_qos_service_func_execute();
	return 0;
}

static cfg_node_ops_t cfg_type_trafficqosservice_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_trafficqosservice_func_commit 
}; 


static cfg_node_type_t cfg_type_trafficqosservice_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_FLOWNUM_RULE, 
	 .parent = &cfg_type_trafficqosservice_pvc, 
	 .ops = &cfg_type_trafficqosservice_entry_ops, 
}; 



static cfg_node_ops_t cfg_type_trafficqosservice_pvc_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create,
	 .delete = cfg_type_default_func_delete,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_trafficqosservice_func_commit 
}; 


static cfg_node_type_t* cfg_type_trafficqosservice_pvc_child[] = { 
	 &cfg_type_trafficqosservice_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_trafficqosservice_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_MULTIPLE | MAX_FLOWNUM_RULE,  
	 .parent = &cfg_type_trafficqosservice, 
	 .nsubtype = sizeof(cfg_type_trafficqosservice_pvc_child) / sizeof(cfg_node_type_t*) - 1 ,
	 .subtype = cfg_type_trafficqosservice_pvc_child, 
	 .ops = &cfg_type_trafficqosservice_pvc_ops, 
}; 


static cfg_node_ops_t cfg_type_trafficqosservice_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_trafficqosservice_func_commit 
}; 


static cfg_node_type_t* cfg_type_trafficqosservice_child[] = { 
	 &cfg_type_trafficqosservice_pvc, 
	 NULL 
}; 


cfg_node_type_t cfg_type_trafficqosservice = { 
	 .name = "TrafficQoSService", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_trafficqosservice_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_trafficqosservice_child, 
	 .ops = &cfg_type_trafficqosservice_ops, 
}; 
