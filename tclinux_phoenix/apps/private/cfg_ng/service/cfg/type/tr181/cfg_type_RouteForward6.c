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
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_tr181_global.h" 
#include "cfg_tr181_utility.h" 

static char* cfg_type_routeforward6_index[] = { 
	 "route6_Idx", 
	 NULL 
}; 

void syncRouteFwd6ByRouteNode(char *path){
	char tmpValue[8]={0};
	int routeIdx = -1;
	
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
	if(strcmp(tmpValue,"1"))
	{	
		/*not commit from tr181*/
		if(get_entry_number_cfg2(path, "entry.", &routeIdx) != 0){
			updateAllStaticRoute6();
		}else{				
			syncStaticRoute2RouteFwd6(routeIdx-1);
		}
	}
	return;
}

void delRouteFwd6ByRouteNode(char *path){
	char tmpValue[8]={0};
	char nodeRouteFwd[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char tmp[8] = {0};
	int index = -1;
	int num = -1;
	int ret = -1;
	char Num[8]={0};
	
	if(get_entry_number_cfg2(path, "entry.", &index) != 0)
		return;

	if(index < 1)
		return;
	
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
	
	if(strcmp(tmpValue,"1")){
		for(i = 0; i < TR181_ROUTE_IPV6_FORWARDING_NUM; i++){
			memset(nodeRouteFwd, 0, sizeof(nodeRouteFwd));
			snprintf(nodeRouteFwd, sizeof(nodeRouteFwd),TR181_ROUTE_IPV6_ENTRY_NODE, i + 1);
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(nodeRouteFwd, "StaticRouteIdx", 0,tmp,sizeof(tmp));
			if('\0' == tmp[0])
				continue;
			if(index -1== atoi(tmp)){
				num = getTR181NodeEntryNum(TR181_ROUTE_IPV6_FORWARDING_NODE,TR181_ROUTE_IPV6_FORWARDING_NUM);
				snprintf(Num, sizeof(Num), "%d", num - 1);
				cfg_type_default_func_delete(nodeRouteFwd);			
				cfg_obj_set_object_attr(TR181_ROUTE_IPV6_COMMON_NODE, "Num", 0, Num);
				break;
			}
		}
	}
	return;
}

int routeFwd6Sync2WanByTR181IPIf(int ipIfIdx, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	int ret = -1;
	int index = -1;
	char enable[8] = {0};

	if(ipIfIdx < 0)
	{
		tcdbg_printf("[%s:%d]ipIfIdx=%d < 0, error!\n",__FUNCTION__,__LINE__,ipIfIdx);
		return -1;
	}
	
	if(wanIf < 0)
	{
		tcdbg_printf("[%s:%d]wanIf=%d < 0, error!\n",__FUNCTION__,__LINE__,wanIf);
		return -1;
	}
	
	/*Find NatInterface Entry index by TR181IP Index*/
	index = getTR181NodeEntryIdxByIPIf(TR181_ROUTE_IPV6_FORWARDING_NODE, TR181_ROUTE_IPV6_FORWARDING_NUM, ipIfIdx);
	if(index < 0)
	{
		/*not find NatInerface*/		
		tcdbg_printf("[%s:%d]index=%d < 0, error!\n",__FUNCTION__,__LINE__,index);
		return -1;
	}
		
	snprintf(nodeName, sizeof(nodeName), TR181_ROUTE_IPV6_ENTRY_NODE, index + 1);
	cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable));
	ret = routeFwdSync2WanByWanIf(nodeName,wanIf,enable,1);

	return ret;
}

int checkRouteForward6(int ipIFIdx)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char strInterface[256] = {0};
	int i = 0;
	int idx = -1;

	for( i = 0; i < TR181_ROUTE_IPV6_FORWARDING_NUM; i++ )
	{
		/*get value*/
		memset(strInterface, 0, sizeof(strInterface));
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TR181_ROUTE_IPV6_ENTRY_NODE, i + 1);
		cfg_obj_get_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, strInterface, sizeof(strInterface));
		sscanf(strInterface+strlen(TR181_IP_INTERFACE), "%d", &idx);	
		
		if(idx == ipIFIdx)
		{
			/*find the IP.interface*/
			return 1;
		}
	}

	return 0;
}

int getEmptyRouteFwd6Entry(int type, int staticRouteIdx, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char tmp[20] = {0};
	int tmp1 = -1;
	int tmp2 = -1;
	char strStaticFlag[20] = {0};
	int fisrtEmptyEntry = -1;
	int index = -1;
	int existFlag = 0;
	
	for( i = 0; i < TR181_ROUTE_IPV6_FORWARDING_NUM; i++)
	{		
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_ROUTE_IPV6_ENTRY_NODE, i + 1);
		
		memset(strStaticFlag, 0, sizeof(strStaticFlag));		
		if(cfg_obj_get_object_attr(nodeName, "StaticRoute", 0, strStaticFlag, sizeof(strStaticFlag)) < 0 
			|| strlen(strStaticFlag) == 0)
		{
			/*is not exist, find an empty entry*/
			if(fisrtEmptyEntry < 0)
			{
				fisrtEmptyEntry = i;
			}
			continue;
		}

		if(type == ROUTE_TYPE_STATIC && strcmp(strStaticFlag,"1") == 0)
		{
			/*for static route Entry*/
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(nodeName, "StaticRouteIdx", 0, tmp, sizeof(tmp));
			if(atoi(tmp) == staticRouteIdx)
			{
				/*already exist*/				
				existFlag = 1;
				break;
			}
		}
		else if(type == ROUTE_TYPE_WAN_SYNC && strcmp(strStaticFlag,"0") == 0)
		{
			/*route sync from wan_PVC*/		
			memset(tmp, 0, sizeof(tmp));		
			cfg_obj_get_object_attr(nodeName, "pvcIndex", 0, tmp, sizeof(tmp));
			if(wanIf == atoi(tmp))	{
				/*already exist*/
				existFlag = 1;
				break;
			}
		}
	}

	if( i != TR181_ROUTE_IPV6_FORWARDING_NUM )
	{ 	
		/*find exist entry*/
		index = i;
	}
	else if(fisrtEmptyEntry >= 0 )
	{
		/*find empty entry*/
		index = fisrtEmptyEntry;
	}
	else
	{
		/*not find empty entry*/
		return -1;
	}

	return index;
}

int syncStaticRoute2RouteFwd6(int routeIdx)
{
	char routeNode[MAXLEN_NODE_NAME] = {0};	
	char routeV6Node[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	int routeForward6Idx = 0;
	int wanIf = -1;
	int ipIfIdx = -1;
	int etyIdx = -1;
	char sinterface[128] = {0};
	char destip[128] = {0};
	char len[16] = {0};
	char pvcIdx[8] = {0};
	int num = -1;
	
	memset(routeNode, 0, sizeof(routeNode));	
	snprintf(routeNode, sizeof(routeNode), ROUTE6_ENTRY_NODE, routeIdx + 1);
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeNode, "Active", 0, tmp, sizeof(tmp)) < 0)
	{
		/*Route_Entry not exist*/
		return -1;
	}
	
	routeForward6Idx = getEmptyRouteFwd6Entry(ROUTE_TYPE_STATIC, routeIdx, -1);
	if(routeForward6Idx < 0)
	{
		tcdbg_printf("[%s:%d]getEmptyRouteFwd4Entry fail.routeIdx=%d \n",__FUNCTION__,__LINE__,routeIdx);
		return -1;
	}


	memset(routeV6Node, 0, sizeof(routeV6Node));
	snprintf(routeV6Node, sizeof(routeV6Node), TR181_ROUTE_IPV6_ENTRY_NODE, routeForward6Idx + 1);
	if (cfg_obj_query_object(routeV6Node,NULL,NULL) < 0)
	{
		cfg_obj_create_object(routeV6Node);
	}

	if(cfg_obj_get_object_attr(routeNode, "Active", 0, tmp, sizeof(tmp)) >= 0 && strcmp(tmp, "Yes") == 0)
	{
		cfg_obj_set_object_attr(routeV6Node, "Enable", 0, "1");
	}
	else
	{
		cfg_obj_set_object_attr(routeV6Node, "Enable", 0, "0");
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeNode, "DST_IP", 0, destip, sizeof(destip)) > 0 && cfg_obj_get_object_attr(routeNode, "Prefix_len", 0, len, sizeof(len)) > 0)
	{
		snprintf(tmp, sizeof(tmp), "%s/%s", destip, len);
		cfg_obj_set_object_attr(routeV6Node, "DestIPPrefix", 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeNode, "Gateway", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(routeV6Node, "GatewayIPAddress", 0, tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeNode, "metric", 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(routeV6Node, "ForwardingMetric", 0, tmp);
	}
	

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeNode, "Device", 0, tmp, sizeof(tmp)) >= 0)
	{
		if(!strcmp(tmp,"br0")){
			
			ipIfIdx = findLanIPInterfaceEntry();
			snprintf(sinterface,sizeof(sinterface),TR181_IP_INTERFACE"%d",ipIfIdx + 1);
			snprintf(pvcIdx,sizeof(pvcIdx),"%d",-1);
		}else if(!strstr(tmp,"ppp")){
		    wanIf = get_wanindex_by_name(tmp);
			etyIdx = getTR181NodeEntryIdxByWanIf(TR181_IP_NODE,IP_INTERFACE_NUM,wanIf);
			snprintf(sinterface,sizeof(sinterface),TR181_IP_INTERFACE"%d",etyIdx + 1);
			snprintf(pvcIdx,sizeof(pvcIdx),"%d",wanIf);
		}else if(!strstr(tmp,"nas")){
		    wanIf = get_wanindex_by_name(tmp);
			etyIdx = getTR181NodeEntryIdxByWanIf(TR181_IP_NODE,IP_INTERFACE_NUM,wanIf);
			snprintf(sinterface,sizeof(sinterface),TR181_IP_INTERFACE"%d",etyIdx + 1);
			snprintf(pvcIdx,sizeof(pvcIdx),"%d",wanIf);
		}
		cfg_obj_set_object_attr(routeV6Node, TR181_INTERFACE_ATTR, 0, sinterface);
		cfg_obj_set_object_attr(routeV6Node, PVC_INDEX_ATTR, 0, pvcIdx);
	}

	/* update RouteForward6  Num*/
	num = getTR181NodeEntryNum(TR181_ROUTE_IPV6_FORWARDING_NODE,TR181_ROUTE_IPV4_FORWARDING_NUM);
	snprintf(tmp, sizeof(tmp), "%d", num);
	cfg_obj_set_object_attr(TR181_ROUTE_IPV6_COMMON_NODE, "Num", 0, tmp);

	
	/*for static route Entry*/
	cfg_obj_set_object_attr(routeV6Node, "StaticRoute", 0, "1");

	memset(tmp, 0, sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), "%d", routeIdx);
	cfg_obj_set_object_attr(routeV6Node, "StaticRouteIdx", 0, tmp);
		
	return routeForward6Idx;	
}


int wanSync2Fwd6ByWanIf(int wanIf, char* enable, char *gateway)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[8] = {0};
	int num = 0;
	int index = -1;
	
	if(wanIf < 0)
	{
		tcdbg_printf("[%s:%d] wanIf=%d error\n", __FUNCTION__, __LINE__, wanIf);
		return -1;
	}
	
	/*need to create RouteForward6_Entry*/
	index = getEmptyRouteFwd6Entry(ROUTE_TYPE_WAN_SYNC, -1, wanIf);
	
	if(index >= 0)
	{
		/*check TR181Route Entry node*/
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(TR181_ROUTE_ENTRY_0_NODE, "Enable", 0, tmp, sizeof(tmp)) < 0)
		{
			if (cfg_obj_query_object(TR181_ROUTE_ENTRY_0_NODE,NULL,NULL) < 0)
			{
				cfg_obj_create_object(TR181_ROUTE_ENTRY_0_NODE);
			}
			cfg_obj_set_object_attr(TR181_ROUTE_ENTRY_0_NODE, "Enable", 0, "1");

			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(TR181_ROUTE_COMMON_NODE, "Num", 0, tmp, sizeof(tmp)) >= 0)
			{
				num = atoi(tmp) + 1;
				snprintf(tmp, sizeof(tmp), "%d", num);
				cfg_obj_set_object_attr(TR181_ROUTE_COMMON_NODE, "Num", 0, tmp);
			}
		}
		/* update RouteForward6 Num*/
		num = getTR181NodeEntryNum(TR181_ROUTE_IPV6_FORWARDING_NODE,TR181_ROUTE_IPV6_FORWARDING_NUM);
		
		/*check entry exists */
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TR181_ROUTE_IPV6_ENTRY_NODE, index + 1);
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "Enable", 0, tmp, sizeof(tmp)) < 0)
		{
			/*check entry not exists */
			num = num + 1;
		}
		
		snprintf(tmp, sizeof(tmp), "%d", num);
		cfg_obj_set_object_attr(TR181_ROUTE_IPV6_COMMON_NODE, "Num", 0, tmp);

		/*add new RouteForward6 Entry node*/
		if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
		{
			cfg_obj_create_object(nodeName);
		}
		cfg_obj_set_object_attr(nodeName, "Enable", 0, enable);
		
		/*not static route Entry*/
		cfg_obj_set_object_attr(nodeName, "StaticRoute", 0, "0");
	
		if(gateway && strlen(gateway))
			cfg_obj_set_object_attr(nodeName, "GatewayIPAddress", 0,gateway);
		else
			cfg_obj_set_object_attr(nodeName, "GatewayIPAddress", 0,"");
		memset(tmp, 0, sizeof(tmp));		
		snprintf(tmp, sizeof(tmp), "%d", wanIf);
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);	
	}

	return index;
}


int findEmptyStaticRoute6Entry(){	
	char nodeName[MAXLEN_NODE_NAME] = {0};		
	int tmp[32] = {0};
	int i = 0;
	
	for(i = 0; i < CFG_TYPE_MAX_ROUTE6_ENTRY_NUM; i++){
		memset(nodeName, 0, sizeof(nodeName));
		memset(tmp, 0, sizeof(tmp));
		snprintf(nodeName,sizeof(nodeName), ROUTE6_ENTRY_NODE, i+1);
		if(cfg_obj_get_object_attr(nodeName, "DST_IP", 0, tmp,sizeof(tmp)) < 0){
			/*Entry not exist, find Empty entry*/
			return i;
		}
	}

	return -1;
}


/*RouteForward6 -> Route6_Entry*/
int tr181Route6UpdateStaticRoute(int routeV6FIdx){

	char routeForward6Node[MAXLEN_NODE_NAME] = {0};	
	char routeNode[MAXLEN_NODE_NAME] = {0};
	char tr181ipNode[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	int etyIdx = -1;
	int ipIfIdx = -1;
	int wanIf = -1;
	char tmp[32] = {0};
	int routeIdx = -1;
	int pvcIdx = -1;
	char lanIdx[8] = {0};
	char len[8] = {0};
	char destip[128] = {0};
	char isp[8]={0};
	char *spit = NULL;
	
	snprintf(routeForward6Node, sizeof(routeForward6Node),TR181_ROUTE_IPV6_ENTRY_NODE, routeV6FIdx);
	if(cfg_query_object(routeForward6Node,NULL,NULL) <= 0)
	{		
		etyIdx = cfg_obj_create_object(routeForward6Node);
		if(etyIdx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,routeForward6Node);
			return -1;
		}
	}
	if(cfg_obj_get_object_attr(routeForward6Node, "StaticRouteIdx", 0, tmp,sizeof(tmp)) < 0 
			|| strlen(tmp) == 0){
		routeIdx = findEmptyStaticRoute6Entry();
		
	}else{
		routeIdx = atoi(tmp);
	}
	
	if(routeIdx < 0){
		return -1;
	}
	
	snprintf(tmp,sizeof(tmp), "%d", routeIdx);
	cfg_obj_set_object_attr(routeForward6Node, "StaticRouteIdx",0, tmp);
	cfg_obj_set_object_attr(routeForward6Node, "StaticRoute", 0, "1");

	/*RouteForward6 -> Route_Entry*/
	memset(routeNode, 0, sizeof(routeNode));
	snprintf(routeNode,sizeof(routeNode), ROUTE6_ENTRY_NODE, routeIdx + 1);	

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeNode, "DST_IP",0, tmp,sizeof(tmp)) >= 0)
	{/*First delete old route rule when change the route table*/
		cfg_tr181_delete_object(routeNode);
	}
	
	if(cfg_query_object(routeNode,NULL,NULL) <= 0)
	{		
		etyIdx = cfg_obj_create_object(routeNode);
		if(etyIdx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,routeNode);
			return -1;
		}
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeForward6Node, "Enable", 0, tmp,sizeof(tmp)) >= 0)
	{	
		if(!strcmp(tmp,"1"))
			cfg_obj_set_object_attr(routeNode, "Active", 0,"Yes");
		else
			cfg_obj_set_object_attr(routeNode, "Active", 0, "No");
	}

	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeForward6Node, "DestIPPrefix", 0, tmp,sizeof(tmp)) > 0){
		spit=strchr(tmp,'/');
		strncpy(destip,tmp,spit-tmp);
		strncpy(len,spit+1,sizeof(len)-1);
		cfg_obj_set_object_attr(routeNode, "DST_IP", 0,destip);
		cfg_obj_set_object_attr(routeNode, "Prefix_len", 0,len);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeForward6Node, "GatewayIPAddress", 0, tmp,sizeof(tmp))>= 0)
		cfg_obj_set_object_attr(routeNode, "Gateway", 0,tmp);
	
	/*check ip.interface,set Device*/		
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeForward6Node, TR181_INTERFACE_ATTR, 0, tmp,sizeof(tmp)) >= 0){
		ipIfIdx = getSuffixIntByInterface(tmp);
	}
	if(ipIfIdx > 0){
		memset(tr181ipNode, 0, sizeof(tr181ipNode));
		snprintf(tr181ipNode,sizeof(tr181ipNode),TR181_IP_ENTRY_NODE, ipIfIdx);
		memset(tmp, 0, sizeof(tmp));
		if( cfg_obj_get_object_attr(tr181ipNode, PVC_INDEX_ATTR, 0, tmp,sizeof(tmp)) < 0
				|| strlen(tmp) == 0){
			cfg_obj_set_object_attr(routeNode, "Device", 0, "");
		}else{
			wanIf = atoi(tmp);
			if(wanIf < 0){
				cfg_obj_get_object_attr(tr181ipNode, LAN_INDEX_ATTR, 0, lanIdx,sizeof(lanIdx));
				if(atoi(lanIdx) == 0)
					cfg_obj_set_object_attr(routeNode, "Device", 0, "br0");
				else
					cfg_obj_set_object_attr(routeNode, "Device", 0, "");
			}else{
				memset(wanNode, 0, sizeof(wanNode));	
				if(setWanNodeByWanIf(wanNode, wanIf) < 0){
					return -1;
				}
				cfg_obj_get_object_attr(wanNode, "ISP", 0, isp,sizeof(isp));
				memset(tmp, 0, sizeof(tmp));
				if(atoi(isp) == WAN_ENCAP_PPP_INT){
					snprintf(tmp, sizeof(tmp), "ppp%d", wanIf);
				}
				else{
					pvcIdx = wanIf/PVC_NUM;
					etyIdx = wanIf%PVC_NUM;
					snprintf(tmp, sizeof(tmp), "nas%d_%d", pvcIdx, etyIdx);
				}
				cfg_obj_set_object_attr(routeNode, "Device", 0,tmp);
			}
		}
	}
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(routeForward6Node, "ForwardingMetric", 0, tmp,sizeof(tmp)) < 0)
		snprintf(tmp,sizeof(tmp),"0");
	
	cfg_obj_set_object_attr(routeNode, "metric", 0, tmp);

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp,sizeof(tmp), "%d", routeIdx);	
	cfg_obj_set_object_attr(WEBCURSET_ENTRY_NODE, "route6_id", 0,tmp);
	cfg_tr181_commit_object(routeNode);
	return 0;
}


int cfg_type_routeforward6_func_commit(char *path)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char destIP[128] = {0};
	int ipIfIdx = -1;
	int routeIdx = -1;
	int tmp[128] = {0};
	int oldWanIf = -1;
	int newWanIf = -1;
	int ret = -1;
	char enable[8] = {0};
	
	if(get_entry_number_cfg2(path, "entry.", &routeIdx) != 0)
	{
		return -1;
	}

	if(routeIdx < 1){
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName),TR181_ROUTE_IPV6_ENTRY_NODE, routeIdx);	
	
	ipIfIdx = getIPIfByTR181Node(TR181_ROUTE_IPV6_FORWARDING_NODE, routeIdx);
	newWanIf = getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_ROUTE_IPV6_FORWARDING_NODE, routeIdx);

	cfg_obj_get_object_attr(nodeName, "DestIPPrefix", 0,destIP,sizeof(destIP));

	if(newWanIf != oldWanIf)
	{
		memset(tmp, 0, sizeof(tmp));
		cfg_obj_get_object_attr(nodeName, "StaticRoute",0, tmp,sizeof(tmp));
		
		/*interface  changed, need clear old wan first*/
		if((oldWanIf >= 0 && !strcmp(tmp, "0")) 	/*Entry is for Route6 wan not static route*/
			|| (!strlen(tmp) && !strlen(destIP)))		/*new Entry just add for Wan route*/
		{
			/*Sync to wan*/
			if(oldWanIf >= 0){
				routeFwdSync2WanByWanIf(nodeName,oldWanIf,"0",1);
				commitWanByWanIf(oldWanIf);
			}
			/*clear other Entry with the same interface */
			clearOtherWanRouteByWanIf(TR181_ROUTE_IPV6_FORWARDING_NODE, TR181_ROUTE_IPV6_FORWARDING_NUM, routeIdx, newWanIf);
		}
	}

	if(newWanIf > 0){		
		setPvcIdxByTR181NodeEntry(TR181_ROUTE_IPV6_FORWARDING_NODE, routeIdx, newWanIf);
	}else{
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, "");
	}

	if( strlen(destIP) > 0 ){
		/*DestIPAddress is  set, thought as static route;*/
		tr181Route6UpdateStaticRoute(routeIdx);		
	}else if(newWanIf > 0){
		/*not static route, check for route wan or bridge*/	
		/*sync to new wan*/	
		cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, enable, sizeof(enable));
		routeFwdSync2WanByWanIf(nodeName,newWanIf,enable,1);
		cfg_obj_set_object_attr(nodeName, "StaticRoute",0, "0");
		commitWanByWanIf(newWanIf);
	}

	return 0;
}


static cfg_node_ops_t cfg_type_routeforward6_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_routeforward6_func_commit 
}; 


static cfg_node_type_t cfg_type_routeforward6_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_ROUTE_IPV6_FORWARDING_NUM, 
	 .parent = &cfg_type_routeforward6, 
	 .index = cfg_type_routeforward6_index, 
	 .ops = &cfg_type_routeforward6_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_routeforward6_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_routeforward6_func_commit 
}; 


static cfg_node_type_t cfg_type_routeforward6_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_routeforward6, 
	 .ops = &cfg_type_routeforward6_common_ops, 
}; 


static cfg_node_ops_t cfg_type_routeforward6_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_routeforward6_func_commit 
}; 


static cfg_node_type_t* cfg_type_routeforward6_child[] = { 
	 &cfg_type_routeforward6_common, 
	 &cfg_type_routeforward6_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_routeforward6 = { 
	 .name = "RouteForward6", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_routeforward6_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_routeforward6_child, 
	 .ops = &cfg_type_routeforward6_ops, 
}; 
