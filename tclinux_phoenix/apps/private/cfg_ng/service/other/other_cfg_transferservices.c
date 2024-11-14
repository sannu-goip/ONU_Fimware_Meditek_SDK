

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
#include <fcntl.h>
#include <netinet/in.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_cli.h>
#include "other_cfg_transferservices.h"
#include "type/cfg_type_transferservices.h"
#include "modules/traffic_process_global_def.h"
#include "utility.h"
#include "traffic/global_dnshost.h"

int svc_other_handle_event_trafficdetail_write()
{
	char nodeName[64]={0};
	int idx_entry = 0;
	FILE *fp = NULL;
	detail_file_info_t *p_fileInfo = NULL;
	detail_file_node_info_t *p_currNode = NULL;
	char detail_counts[12] = {0};
	char httpcode[30]={0};	
	char mobilenode[64] = {0};
	char website[64] = {0};

	fp = fopen(TRAFFIC_DETAIL_PATH, "wb");
	if( !fp )
		goto traffic_detail_write_err;

	p_fileInfo = (detail_file_info_t *)malloc(sizeof(detail_file_info_t));
	if ( !p_fileInfo )
		goto traffic_detail_write_err;

	bzero(p_fileInfo, sizeof(detail_file_info_t));
	bzero(nodeName, sizeof(nodeName));
	p_fileInfo->node_count = 0;
	sendTodnshost(BIT_DETAIL,DEL_ACT, NULL);
	
	memset(mobilenode, 0, sizeof(mobilenode));			
	strcpy(mobilenode, MOBILE_ENTRY_NODE);	
	cfg_obj_get_object_attr(mobilenode, "website", 0, website, sizeof(website));
	cfg_obj_get_object_attr(mobilenode, "httpcode",0,httpcode,sizeof(httpcode));
	
	for ( idx_entry = 0; idx_entry < MAX_FLOWNUM_RULE; idx_entry ++ )
	{
		snprintf(nodeName, sizeof(nodeName), TRAFFICDETAILPROCESS_ENTRY_NODE, idx_entry+1);
		p_currNode = &p_fileInfo->node[idx_entry];

		if ( 0 > cfg_obj_get_object_attr(nodeName, "methodList", 0, p_currNode->methodList, sizeof(p_currNode->methodList))
			|| 0 == p_currNode->methodList[0] )
			continue;

		if ( 0 > cfg_obj_get_object_attr(nodeName, "direction", 0, p_currNode->direction, sizeof(p_currNode->direction))
			|| 0 == p_currNode->direction[0] )
			continue;

		if ( 0 > cfg_obj_get_object_attr(nodeName, DETAIL_BUNDLE_NAME, 0, p_currNode->bundle_name, sizeof(p_currNode->bundle_name))
			|| 0 == p_currNode->bundle_name[0] )
			continue;

		cfg_obj_get_object_attr(nodeName, "remoteAddrress", 0, p_currNode->remoteAddress, sizeof(p_currNode->remoteAddress));
		sendTodnshost(BIT_DETAIL,ADD_ACT, p_currNode->remoteAddress);
		cfg_obj_get_object_attr(nodeName, "remotePort", 0, p_currNode->remotePort, sizeof(p_currNode->remotePort));
		cfg_obj_get_object_attr(nodeName, "hostMAC", 0, p_currNode->hostMAC, sizeof(p_currNode->hostMAC));
		cfg_obj_get_object_attr(nodeName, "headerList", 0, p_currNode->headerList, sizeof(p_currNode->headerList));
		/* it means ALL when statuscodeList is empty */
		cfg_obj_get_object_attr(nodeName, "statuscodeList", 0, p_currNode->statuscodeList, sizeof(p_currNode->statuscodeList));
		
		if(strstr(website,p_currNode->remoteAddress) != NULL)
		{											
			if(strlen(httpcode) != 0)	
			{					
				strcat(p_currNode->statuscodeList,httpcode);				
			}		
		}
		
		p_currNode->ruleinst = idx_entry;
		p_fileInfo->node_count ++;
	}

	/* update cnt */
	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TRAFFICDETAILPROCESS_COMMON_NODE);

	BuffSet_Format(detail_counts, "%d", p_fileInfo->node_count);
	cfg_set_object_attr(nodeName, TRAFFIC_DETAIL_RULENUM, detail_counts);
	if ( p_fileInfo->node_count <= 0 )
		cfg_set_object_attr(nodeName, DETAIL_BUNDLE_NAME, "");

	fwrite(p_fileInfo, sizeof(detail_file_info_t), 1, fp);

	if ( fp )
		fclose(fp);
	if ( p_fileInfo )
		free(p_fileInfo);

	return SUCCESS;

traffic_detail_write_err:
	if ( fp )
		fclose(fp);
	/*
	if ( p_fileInfo )
		free(p_fileInfo);
	*/
	return FAIL;
}


int svc_other_handle_event_trafficdetail_boot()
{
	system("killall -9 trafficdetail");
	system("/userfs/bin/trafficdetail &");
	return 0;
}


int svc_other_handle_event_trafficmonitor_write()
{
	char nodeName[64]={0};
	int idx_pvc = 0, idx_entry = 0;
	FILE *fp = NULL;
	monitor_file_info_t *p_fileInfo = NULL;
	monitor_file_node_info_t *p_currNode = NULL;
	char *p_bundle_name = NULL, *p_addr = NULL;
	monitor_file_bundle_info_t *p_currMonitor = NULL;
	char timeout_buf[12] = {0}, addr_counts[12] = {0};
	char def_route_index_v4[10] = {0}, def_route_index_v6[10] = {0};
	int i_def_route_index_v4 = -1, i_def_route_index_v6 = -1, empty_pvc = 0;
	char buffer[12] = {0};

	fp = fopen(TRAFFIC_MONITOR_PATH, "wb");
	if( !fp )
		goto monitor_write_err;

	p_fileInfo = (monitor_file_info_t *)malloc(sizeof(monitor_file_info_t));
	if ( !p_fileInfo )
		goto monitor_write_err;

	bzero(p_fileInfo, sizeof(monitor_file_info_t));
	sendTodnshost(BIT_MONITOR, DEL_ACT,NULL);
	for ( idx_pvc = 0; idx_pvc < PVC_NUM; idx_pvc ++ )
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TRAFFICMONITOR_PVC_NODE, idx_pvc+1);
		empty_pvc = 0;
		p_currMonitor = &p_fileInfo->bundle[idx_pvc];
		p_bundle_name = p_currMonitor->bundle_name;

		if ( 0 > cfg_obj_get_object_attr(nodeName, MONITOR_BUNDLE_NAME, 0, p_bundle_name, MAX_DOMAIN_LEN)
			|| 0 == p_bundle_name[0] )
		{
			empty_pvc = 1;
			snprintf(p_bundle_name, MAX_DOMAIN_LEN, "%s", "bundle_name");
		}

		p_currMonitor->node_count = 0;
		if ( 0 == empty_pvc )
		{
			for ( idx_entry = 0; idx_entry < MAX_FLOWNUM_RULE; idx_entry ++ )
			{
				bzero(nodeName, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), TRAFFICMONITOR_PVC_ENTRY_NODE, idx_pvc+1, idx_entry+1);
				p_currNode = &p_currMonitor->node[idx_entry];
				p_addr = p_currNode->destAddr;

				if ( 0 > cfg_obj_get_object_attr(nodeName, MONITOR_DESTADDR, 0, p_addr, MAX_DOMAIN_LEN)
					|| 0 == p_addr[0] )
					continue;
				sendTodnshost(BIT_MONITOR, ADD_ACT, p_addr);
				p_currMonitor->node_count ++;
			}
		}

		/* update cnt */
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TRAFFICMONITOR_PVC_NODE, idx_pvc+1);
		snprintf(addr_counts, sizeof(addr_counts), "%d", p_currMonitor->node_count);
		cfg_set_object_attr(nodeName, MONITOR_ADDRESS_RULENUM, addr_counts);
#if !defined(TCSUPPORT_CT_DBUS)
		if ( p_currMonitor->node_count <= 0 )
			cfg_set_object_attr(nodeName, MONITOR_BUNDLE_NAME, "");
#endif
	}

	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TRAFFICMONITOR_COMMON_NODE);
	if ( 0 > cfg_obj_get_object_attr(nodeName, MONITOR_TIMEOUT, 0, timeout_buf, sizeof(timeout_buf))
		|| 0 == timeout_buf[0] )
		p_fileInfo->monitor_timeout = TRAFFIC_MONITOR_TIME_OUT;
	else
		p_fileInfo->monitor_timeout = atoi(timeout_buf);

	/* get default route wan info */
	bzero(def_route_index_v4, sizeof(def_route_index_v4));
	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WANINFO_COMMON_NODE);
	if ( 0 < cfg_obj_get_object_attr(nodeName, "DefRouteIndexv4", 0, def_route_index_v4, sizeof(def_route_index_v4))
		&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A") )
		i_def_route_index_v4 = atoi(def_route_index_v4);
	if ( 0 < cfg_obj_get_object_attr(nodeName, "DefRouteIndexv6", 0, def_route_index_v6, sizeof(def_route_index_v6))
		&& '\0' != def_route_index_v6[0] && 0 != strcmp(def_route_index_v6, "N/A") )
		i_def_route_index_v6 = atoi(def_route_index_v6);
	if ( i_def_route_index_v4 >= 0 )
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, i_def_route_index_v4+1);
		cfg_obj_get_object_attr(nodeName, "IP", 0, p_fileInfo->wan_ipv4, sizeof(p_fileInfo->wan_ipv4));

		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 
			i_def_route_index_v4 / MAX_SMUX_NUM + 1, 
			i_def_route_index_v4 % MAX_SMUX_NUM + 1);
		cfg_obj_get_object_attr(nodeName, WAN_IFNAME, 0, buffer, sizeof(buffer));

		if (strstr(buffer, "ppp") == NULL){
			strncpy(p_fileInfo->wan_name, "", sizeof(p_fileInfo->wan_name));
		}
		else {
			cfg_obj_get_object_attr(nodeName, "USERNAME", 0, p_fileInfo->wan_name, sizeof(p_fileInfo->wan_name));
		}
	}

	if ( i_def_route_index_v6 >= 0 )
	{
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, i_def_route_index_v6+1);
		cfg_obj_get_object_attr(nodeName, "IP6", 0, p_fileInfo->wan_ipv6, sizeof(p_fileInfo->wan_ipv6));

		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, 
			i_def_route_index_v6 / MAX_SMUX_NUM + 1, 
			i_def_route_index_v6 % MAX_SMUX_NUM + 1);
		cfg_obj_get_object_attr(nodeName, WAN_IFNAME, 0, buffer, sizeof(buffer));

		if (strstr(buffer, "ppp") == NULL){
			strncpy(p_fileInfo->wan_name6, "", sizeof(p_fileInfo->wan_name6));
		}
		else {
			cfg_obj_get_object_attr(nodeName, "USERNAME", 0, p_fileInfo->wan_name6, sizeof(p_fileInfo->wan_name6));
		}
	}

	fwrite(p_fileInfo, sizeof(monitor_file_info_t), 1, fp);

	if ( fp )
		fclose(fp);
	if ( p_fileInfo )
		free(p_fileInfo);

	return SUCCESS;

monitor_write_err:
	if ( fp )
		fclose(fp);
	/*
	if ( p_fileInfo )
		free(p_fileInfo);
	*/
	return FAIL;
}


int svc_other_handle_event_trafficmonitor_boot()
{
	system("killall -9 trafficmonitor");
	system("/userfs/bin/trafficmonitor &");
	return 0;
}


int svc_other_handle_event_trafficmirror_write()
{
	char nodeName[64]={0};	
	int idx = 0;
	mirror_file_info_t *pMirrFile = (mirror_file_info_t *)malloc(sizeof(mirror_file_info_t));
	FILE *fp = NULL;
	char num[8] = {0};
	mirror_file_node_info_t *pCurN = NULL;

	blapi_traffic_uninstall_mirror_traffic();

	fp = fopen(TRAFFIC_MIRROR_PATH, "wb");
	if ( !fp || !pMirrFile )
		goto trafficmirror_write_err;

	bzero(pMirrFile, sizeof(mirror_file_info_t));
	pMirrFile->node_count = 0;

	sendTodnshost(BIT_MIRROR, DEL_ACT, NULL);
	for ( idx = 1; idx <= MAX_FLOWNUM_RULE; idx ++ )
	{
		snprintf(nodeName, sizeof(nodeName), TRAFFICMIRROR_ENTRY_NODE, idx);
		pCurN = &(pMirrFile->node[pMirrFile->node_count]);
		bzero(pCurN, sizeof(mirror_file_node_info_t));
		if ( 0 > cfg_obj_get_object_attr(nodeName, "protocol", 0, pCurN->protocol, sizeof(pCurN->protocol))
			|| 0 == pCurN->protocol[0] )
			continue;
		if ( 0 > cfg_obj_get_object_attr(nodeName, "direction", 0, pCurN->direction, sizeof(pCurN->direction))
			|| 0 == pCurN->direction[0] )
			continue;
		if ( 0 > cfg_obj_get_object_attr(nodeName, "mirrorToIP", 0, pCurN->mirrorToIP, sizeof(pCurN->mirrorToIP))
			|| 0 == pCurN->mirrorToIP[0] )
			continue;
		if ( 0 > cfg_obj_get_object_attr(nodeName, "mirrorToPort", 0, pCurN->mirrorToPort, sizeof(pCurN->mirrorToPort))
			|| 0 == pCurN->mirrorToPort[0] )
			continue;
		memset(num, 0 ,sizeof(num));
		sscanf(nodeName, "root.trafficmirror.entry.%s", num);
		pCurN->entrynum = atoi(num) - 1;
		cfg_obj_get_object_attr(nodeName, "remoteAddr", 0, pCurN->remoteAddress, sizeof(pCurN->remoteAddress));
		sendTodnshost(BIT_MIRROR, ADD_ACT, pCurN->remoteAddress);
		cfg_obj_get_object_attr(nodeName, "remotePort", 0, pCurN->remotePort, sizeof(pCurN->remotePort));
		cfg_obj_get_object_attr(nodeName, "hostMAC", 0, pCurN->hostMAC, sizeof(pCurN->hostMAC));
		pMirrFile->node_count ++;
	}	

	fwrite(pMirrFile, sizeof(mirror_file_info_t), 1, fp);

	if ( fp )
		fclose(fp);
	if ( pMirrFile )
		free(pMirrFile);

	return 0;

trafficmirror_write_err:
	if ( fp )
		fclose(fp);
	if ( pMirrFile )
		free(pMirrFile);

	return -1;
}


int svc_other_handle_event_trafficmirror_boot()
{
	system("killall -9 trafficmirror");
	system("/userfs/bin/trafficmirror &");
	return 0;
}


