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
#include <svchost_evt.h>
#include "cfg_tr181_global.h" 
#include "utility.h" 
#include "cfg_tr181_utility.h" 
#include "cfg_msg.h" 
#include <sys/msg.h>
#include <sys/ipc.h>
#include "time.h"


void syncWan2TR181V6Info(int wanIf,int ipIndex);
extern int initQosPolicer(void);

int getDbgLevel()
{
	char buf[64] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};

	/*Only do when dhcp enable. shnwind 20110428*/
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, TR181_COMMON_NODE, sizeof(nodeName) - 1);
	cfg_obj_get_object_attr(nodeName, "dbgflag", 0, buf, sizeof(buf));
	
	return atoi(buf);
}

int updateInterfaceNodeDbg()
{
	int mask = 0;

	mask = getDbgLevel();
	if(mask&INTERFACE_NODE_UPDATE_DBG)
		return 1;
	else
		return 0;
}

void
sendTR181MsgtoCwmp(int tr181Type)
{
	cwmp_msg_t message;
	long type = 1;	/*tr69 must be 1*/
	int msgFlag = IPC_NOWAIT;/*0*/
	int rv = 0, pidnum = 0, pid_t[128] = {0};

	rv = find_pid_by_name( "tr69", pid_t, &pidnum);
	if( 0 != rv || 1 != pidnum )
	{
		return;
	}
	
	memset(&message,0,sizeof(cwmp_msg_t));
	message.cwmptype = TR181_EVENT; 	
	snprintf(message.text.reserved,CWMP_MAX_MSG_LEN - 1,"%d" ,tr181Type);
	if(sendmegq(type, &message,msgFlag) < 0){								
		tcdbg_printf("[%s:%d] subType=%d\n",__FUNCTION__,__LINE__,tr181Type);
	}	
	
	return;
}

void cfg_tr181_commit_object(char* node)
{	

	cfg_obj_set_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, "1");
	
	cfg_commit_object(node);

	cfg_obj_set_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, "0");

	return;
}

void cfg_tr181_delete_object(char* node)
{	
	cfg_obj_set_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, "1");
	
	if (cfg_query_object(node,NULL,NULL) >= 0)
	{
		cfg_delete_object(node);
	}
	
	cfg_obj_set_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, "0");
	return;
}


/*______________________________________________________________________________
**	checkEntryExistByPvcLanIdx
**
**	descriptions:
**		check Entry exist by LanIndex or PvcIndex
**
**	parameters:
**		entryIdx:		return for exist entry
**		node:	
**		num:
**		index:  check for LanIndex or PvcIndex
**		wan_lan_flag:
**				0: check for LanIndex
**				1: check for PvcIndex
**
**	return:
**			0: Entry not find
**			1: Entry exist,  return entry num. entryIdx
**		     -1: Error
**_______________________________________________________________________________
*/
int checkEntryExistByPvcLanIdx(int* entryIdx, char* node, int num,  int index, int wan_lan_flag)
{
	int i = 0;
	char tmp[20] = {0};
	
	char tmp2[20] = {0};
	int fisrtEmptyEntry = -1;
	char attrValue[20] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};

	if(entryIdx == NULL || node == NULL || index < 0){
		return -1;
	}

	for( i = 0; i < num; i++)
	{		
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i + 1);

		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) < 0){
			/*node not exist, set fisrtEmptyEntry*/
			if(fisrtEmptyEntry < 0)
			{
				fisrtEmptyEntry = i;
			}
		}
		
		memset(tmp,0,sizeof(tmp));
		memset(tmp2,0,sizeof(tmp2));
		if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0 && (strlen(tmp) == 0 || strcmp(tmp, "-1") == 0) )
			&& (cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, tmp2, sizeof(tmp2)) >= 0 && (strlen(tmp2) == 0 || strcmp(tmp2, "-1") == 0))){
			/*node not exist, set fisrtEmptyEntry*/
			if(fisrtEmptyEntry < 0)
			{
				fisrtEmptyEntry = i;
			}
		}
		
		if(wan_lan_flag == 0){
			/*check for LanIndex*/
			strncpy(attrValue,LAN_INDEX_ATTR,sizeof(attrValue) - 1 );
		}else{
			/*check for PvcIndex*/
			strncpy(attrValue,PVC_INDEX_ATTR,sizeof(attrValue) - 1 );
		}
	
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, attrValue, 0, tmp, sizeof(tmp)) >= 0)
		{
			if(atoi(tmp) == index){
				/*find wan/Lan Entry*/
				*entryIdx = i;
				return 1;
			}
		}
	}
	
	*entryIdx = fisrtEmptyEntry;

	return 0;
}


int getIPIfByTR181Node(char* node, int index){ 
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char strInterface[256] = {0};
	int ipIfIdx = -1;
	
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName,sizeof(nodeName),"%s.entry.%d",node, index);
	if(cfg_obj_get_object_attr(nodeName, TR181_INTERFACE_ATTR, 0,strInterface,sizeof(strInterface)) > 0 && strstr(strInterface,TR181_IP_INTERFACE)){
		if(updateInterfaceNodeDbg()){
			tcdbg_printf("[%s:%d]strInterface=%s\n",__FUNCTION__,__LINE__,strInterface);
		}	
		
		ipIfIdx=getSuffixIntByInterface(strInterface);
		return ipIfIdx;
	}
	return -1;
}


int  findEmptyRouteAdvIfSetEntryForWan(void)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char tmp[16] = {0};
	int emptyEntryIdx = -1;	
		
	for( i = 1; i < RTADV_ITFSET_NUM_MAX; i++)
	{ 	
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_ROUTEADVIFSET_ENTRY_NODE, i);
					
		memset(tmp,0,sizeof(tmp));		
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) < 0)
		{	
			emptyEntryIdx = i;			
			break;;
		}	
	}
	
	return emptyEntryIdx;
}

int clearTR181PVCNodeByIdx(char* name, int idx)
{	
	char node[MAXLEN_NODE_NAME] = {0};

	/*unset PVC node*/
	memset( node, 0, sizeof(node) );
	snprintf( node, sizeof(node), "%s.pvc.%d",name , (idx + 1));	
	cfg_tr181_delete_object(node);

	return 0;
}

int clearTR181InterfaceNode(char* name, int num)
{	
	char tmp[20] = {0};
	int i = 0;
	char node[MAXLEN_NODE_NAME] = {0};
	
	for(i = 0; i < num; i++)
	{				
		memset(node, 0, sizeof(node) );
		snprintf(node, sizeof(node), "%s.entry.%d", name , i + 1);	
		cfg_tr181_delete_object(node);
	}

	/*clear num*/		
	memset(tmp, 0, sizeof(tmp));		
	memset(node, 0, sizeof(node));	
	snprintf(node, sizeof(node), "%s.common", name); 
	cfg_obj_set_object_attr(node, TR181_NUM_ATTR, 0, "0");

	return 0;
}

int clearTR181InterfaceNodeByWanIf(char* name, int num, int WanIf)
{	
	char tmp[20] = {0};
	char number[4] = {0};
	int i = 0;
	char node[MAXLEN_NODE_NAME] = {0};

	for(i = 1; i <= num; i++)
	{
		memset(node, 0, sizeof(node));
		snprintf(node, sizeof(node), "%s.entry.%d", name, i);

		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(node, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0
		&& (atoi(tmp) == WanIf))
		{
			cfg_delete_object(node);

			if(strcmp(name, TR181_IP_NODE) == 0)
			{
				clearTR181PVCNodeByIdx(TR181_IP_INTERFACE_IPV4ADDR_NODE, i - 1);
				clearTR181PVCNodeByIdx(TR181_IP_INTERFACE_IPV6ADDR_NODE, i - 1);
			}
			memset(tmp, 0, sizeof(tmp));
			snprintf(node, sizeof(node), "%s.common", name);
			
			cfg_obj_get_object_attr(node, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
			memset(number, 0, sizeof(number));
			snprintf(number, sizeof(number), "%d", (atoi(tmp) - 1));
			cfg_obj_set_object_attr(node, TR181_NUM_ATTR, 0, number);
		}
	}

	return 0;
}


int clearTR181InterfacePVCNode(char* name, int num)
{	
	int i = 0;
	char node[MAXLEN_NODE_NAME] = {0};
	
	for(i = 0; i < num; i++)
	{				
		memset(node, 0, sizeof(node) );
		snprintf( node, sizeof(node), "%s.pvc.%d", name , i + 1);	
		cfg_tr181_delete_object(node);
	}

	return 0;
}

int clearAllTR181WanInterfaceNodeByWanIF(int wanIf)
{		
	clearTR181InterfaceNodeByWanIf(TR181_IP_NODE, IP_INTERFACE_NUM, wanIf);	
	clearTR181InterfaceNodeByWanIf(TR181_PPP_NODE, PPP_INTERFACE_NUM, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_ETHERVLANT_NODE, ETHER_VLAN_T_NUM, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_ETHERLINK_NODE, ETHER_LINK_NUM, wanIf);
#if defined(TCSUPPORT_CT_DSL_EX)
	clearTR181InterfaceNodeByWanIf(TR181_ATMLINK_NODE, ATM_LINK_NUM, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_PTMLINK_NODE, PTM_LINK_NUM, wanIf);
#endif
	clearTR181InterfaceNodeByWanIf(TR181_NAT_INTERFACE_NODE, NAT_INTERFACE_NUM, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_DNS_CLIENT_NODE, DNS_CLIENT_NUM, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_DHCPV4_CLIENT_NODE, DHCPV4_CLIENT_NUM_MAX, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX, wanIf);

	clearTR181InterfaceNodeByWanIf(TR181_ROUTE_IPV4_FORWARDING_NODE, TR181_ROUTE_IPV4_FORWARDING_NUM, wanIf);
	clearTR181InterfaceNodeByWanIf(TR181_ROUTE_IPV6_FORWARDING_NODE, TR181_ROUTE_IPV6_FORWARDING_NUM, wanIf);

	return 0;
}

int clearAllInterfaceNode(void)
{	
	int num = 0;

	num = getLanPortNum();

#if defined(TCSUPPORT_CT_DSL_EX)
	clearTR181InterfaceNode(TR181_ATMLINK_NODE,ATM_LINK_NUM);
	clearTR181InterfaceNode(TR181_PTMLINK_NODE,PTM_LINK_NUM);
#endif
	clearTR181InterfaceNode(TR181_IP_NODE,IP_INTERFACE_NUM);	
	clearTR181InterfacePVCNode(TR181_IP_INTERFACE_IPV4ADDR_NODE, TR181_IP_INTERFACE_IPV4ADDR_NUM);
	clearTR181InterfacePVCNode(TR181_IP_INTERFACE_IPV6ADDR_NODE, TR181_IP_INTERFACE_IPV6ADDR_NUM);
	clearTR181InterfacePVCNode(TR181_IP_INTERFACE_IPV6PREF_NODE, TR181_IP_INTERFACE_IPV6PREF_NUM);

	clearTR181InterfaceNode(TR181_PPP_NODE, PPP_INTERFACE_NUM);
	clearTR181InterfaceNode(TR181_ETHERVLANT_NODE, ETHER_VLAN_T_NUM);
	clearTR181InterfaceNode(TR181_ETHERLINK_NODE, ETHER_LINK_NUM);
	clearTR181InterfaceNode(TR181_ETHER_INTERFACE_NODE, num);
	
	clearTR181InterfaceNode(TR181_BRIDGE_PORT_NODE, BRIDGE_PORT_NUM);
	/*clearTR181InterfaceNode(BRIDGINGE_VLAN_NODE, BRIDGINGE_VLAN_NUM);
	clearTR181InterfaceNode(BRIDGINGE_VLAN_PORT_NODE, BRIDGINGE_VLAN_PORT_NUM);*/
	
	/*clearTR181InterfaceNode(TR181_WIFI_SSID, WIFI_SSID_NUM_MAX);
	clearTR181InterfaceNode(TR181_WIFI_RDO, WIFI_RDO_NUM_MAX);
	clearTR181InterfaceNode(TR181_WIFI_AP, WIFI_AP_NUM_MAX);
	clearTR181InterfaceNode(TR181_WIFI_APSEC, WIFI_APSEC_NUM_MAX);*/
	
	clearTR181InterfaceNode(TR181_IPACTIVEPORT_NODE, TR181_IPACTIVEPORT_NUM);
	clearTR181InterfaceNode(TR181_DEVINF_PS_NODE, TR181_DEVINF_PS_NUM);
	clearTR181InterfaceNode(TR181_NAT_INTERFACE_NODE, NAT_INTERFACE_NUM);
	clearTR181InterfaceNode(TR181_DNS_CLIENT_NODE, DNS_CLIENT_NUM);
	clearTR181InterfaceNode(TR181_DHCPV4_CLIENT_NODE, DHCPV4_CLIENT_NUM_MAX);
	clearTR181InterfaceNode(TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX);
	clearTR181InterfaceNode(TR181_NATPORTMAPPING_NODE, TR181_NATPORTMAPPING_NUM);

	clearTR181InterfaceNode(TR181_ROUTE_IPV4_FORWARDING_NODE, TR181_ROUTE_IPV4_FORWARDING_NUM);
	clearTR181InterfaceNode(TR181_ROUTE_IPV6_FORWARDING_NODE, TR181_ROUTE_IPV6_FORWARDING_NUM);
	
	clearTR181InterfaceNode(TR181_NATPORTMAPPING_NODE, TR181_NATPORTMAPPING_NUM);
	clearTR181InterfaceNode(TR181_ROUTEADVIFSET_NODE, RTADV_ITFSET_NUM_MAX);
	clearTR181InterfaceNode(TR181_DSLITE_NODE, TR181_DSLITE_NUM);
	/*clearTR181InterfaceNode(TR181_DHCPV4_CLIENT, DHCPV4_CLIENT_NUM_MAX);*/

	clearTR181InterfacePVCNode(TR181_FIREWALLCH_NODE, TR181_FIREWALL_CHAIN_PVC_NUM);

	return 0;
}

int getTPSTCMode(void){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tpstcMode[16] = {0};

	snprintf(nodeName, sizeof(nodeName), WAN_COMMON_NODE); 

	cfg_obj_get_object_attr(nodeName, "TpstcMode", 0, tpstcMode, sizeof(tpstcMode));
	return atoi(tpstcMode);
}

int clearNatPortMapNodeByPVCIdx(int pvcIdx)
{		
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[20] = {0};
	int i = 0;
	int num = 0;
	char node[MAXLEN_NODE_NAME] = {0};
	
	/*del entry, set num*/	
	memset(tmp, 0, sizeof(tmp));		
	memset(nodeName, 0, sizeof(nodeName));		
	strncpy(nodeName, TR181_NATPORTMAPPING_COMMON_NODE, sizeof(nodeName) - 1);

	cfg_obj_get_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
	
	num = atoi(tmp);
	
	for( i = 0; i < TR181_NATPORTMAPPING_NUM; i++)
	{		
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_NATPORTMAPPING_ENTRY_NODE, i + 1);

		/*get isUpdate node to check wether the Entry need update*/
		memset(tmp,0,sizeof(tmp));		
		if(cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0
			&& pvcIdx == atoi(tmp))
		{		
			/*unset Entry*/
			memset(node, 0, sizeof(node) );
			snprintf(node, sizeof(node), TR181_NATPORTMAPPING_ENTRY_NODE, i + 1);	
			cfg_tr181_delete_object(node);
			num--;
		}
	}

	memset(tmp, 0, sizeof(tmp));		
	memset(nodeName, 0, sizeof(nodeName));		
	strncpy(nodeName, TR181_NATPORTMAPPING_COMMON_NODE, sizeof(nodeName) - 1);
	
	snprintf(tmp, sizeof(tmp), "%d", num);
	cfg_obj_set_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp);

	return 0;
}

int isMultiSerPVC(int pvc)
{
	if((pvc >= 8) && (pvc <=10))
	{
		return 1;
	}
	return 0;
}

int setWanNodeByWanIf(char *wanNode, int wanIf)
{
	int pvcIdx = -1;
	int entIdx = -1;

	if(wanIf < 0 || (wanIf >= WAN_INTERFACE_NUM))
	{
		tcdbg_printf("[%s:%d] wanIf =%d, error!\n",__FUNCTION__,__LINE__,wanIf);
		return -1;
	}
	
	pvcIdx = wanIf/PVC_NUM;
	entIdx = wanIf%PVC_NUM;

	snprintf(wanNode, MAXLEN_NODE_NAME, "root.wan.pvc.%d.entry.%d",pvcIdx + 1, entIdx + 1); 

	return 0;
}

int createTR181InterfaceEntry(struct tr181_interface_s interface)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char tmp[20] = {0};
	char tmp1[20] = {0};
	char enable[20] = {0};
	char lanindex[4] = {0};
	char pvcindex[4] = {0};
	int num = 0;
	int fisrtEmptyEntry = -1;
	int index = -1;
	int existFlag = 0;
	int flag = 0;

	for( i = 0; i < interface.num; i++)
	{		
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", interface.nodeName, i + 1);
		flag = 0;
		
		cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, lanindex, sizeof(lanindex));
		if(strlen(lanindex) == 0 || 0 == strcmp(lanindex, "-1") )
		{
			cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcindex, sizeof(pvcindex));
			if(0 == strcmp(pvcindex, "-1")){
				/*Entry is the same as empty wan and can be used*/
				flag = 1;
			}
		}

		if(interface.lanIndex >= 0)
		{
			/*for lan interface*/			
			memset(tmp, 0, sizeof(tmp));		
			if(cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, tmp, sizeof(tmp)) < 0
				|| (strlen(tmp) == 0) || flag == 1)
			{
				/*is not exist, find an empty entry*/
				if(fisrtEmptyEntry < 0)
				{
					fisrtEmptyEntry = i;
				}
				continue;
			}
			
			if(atoi(tmp) == interface.lanIndex)
			{
				/*find lanIndex*/
				existFlag = 1;
				break;
			}
		}

		if(interface.pvcIndex >= 0)
		{
			/*for wan interface*/			
			memset(tmp, 0, sizeof(tmp));		
			memset(tmp1, 0, sizeof(tmp1));		
			if(cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) < 0
				|| (strlen(tmp) == 0) || flag == 1)
			{
				/*is not exist, find an empty entry*/
				if(fisrtEmptyEntry < 0)
				{
					fisrtEmptyEntry = i;
				}
				continue;
			}
			
			if(atoi(tmp) == interface.pvcIndex)
			{
				/*find pvcIndex and find lanIndex*/
				existFlag = 1;
				break;
			}
		}
	}

	if( i != interface.num )
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
	if(strcmp(interface.nodeName,TR181_WIFIAP_NODE) == 0 || strcmp(interface.nodeName,TR181_WIFISSID_NODE) == 0 || strcmp(interface.nodeName,TR181_WIFIAPWPS_NODE) == 0)
	{
		index = interface.lanIndex - LAN_INDEX_WLAN_START;
	}

	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", interface.nodeName, index + 1);
	if (cfg_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}
	
	memset(tmp,0,sizeof(tmp));		
	memset(enable,0,sizeof(enable));		
	snprintf(enable, sizeof(enable), "%d", interface.active);
	
	cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, enable);
	#if 0
	if(interface.active == 1)
	{
		/*only if wan is enable ,set node enable*/
		cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, enable);
	}
	#endif

	if(interface.lanIndex == 0)
	{
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, "-1");
	}
	else
	{
		memset(tmp,0,sizeof(tmp));		
		snprintf(tmp, sizeof(tmp), "%d", interface.pvcIndex);
		cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);
	}
	
	memset(tmp,0,sizeof(tmp));		
	snprintf(tmp, sizeof(tmp), "%d", interface.lanIndex);
	cfg_obj_set_object_attr(nodeName, LAN_INDEX_ATTR, 0, tmp);

	if(existFlag == 0)
	{
		/*new entry, set num*/ 	
		memset(tmp,0,sizeof(tmp));		
		memset(nodeName,0,sizeof(nodeName));		
		snprintf(nodeName, sizeof(nodeName), "%s.common", interface.nodeName);

		cfg_obj_get_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
		
		num = atoi(tmp);	
		memset(tmp,0,sizeof(tmp));		
		snprintf(tmp, sizeof(num), "%d", num+1);
		cfg_obj_set_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp);
	}

	return index;
}

int getWanVlanTag(int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME];
	char dot1q[8], vlanid[8];
	int ret = -1;

	memset(nodeName, 0, sizeof(nodeName));
	ret = setWanNodeByWanIf(nodeName, wanIf);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	cfg_obj_get_object_attr(nodeName, "dot1q", 0, dot1q, sizeof(dot1q));

	if (!strcmp(dot1q, "Yes")) 
	{
		if (cfg_obj_get_object_attr(nodeName, "VLANID", 0, vlanid, sizeof(vlanid)) >= 0) 
		{
			return atoi(vlanid);
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

void setInterfaceValue(char* node, int index, char* strInterface)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, index + 1);
	cfg_obj_set_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, strInterface);

	return;
}

int getTR181NodeEntryNum(char* node, int maxNum)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[8] = {0};
	int num = 0, i = 0;

	memset(nodeName,0,sizeof(nodeName));
	for(i = 0; i < maxNum; i++) 
	{		
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i + 1);
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		{
			num++;
		}
	}
	return num;
}

int wanSync2TR181RouteFwd(int wanIf, int ipIndex){
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char tmp[32] = {0};	
	char enable[32] = {0};
	char strInterface[256] = {0};
	char ipversion[16] = {0};
	char gateWay[32] = {0},gateWay6[64] = {0};
	int isp = -1,ret = -1,idx = -1;
	
	
	if(wanIf < 0)
	{
		tcdbg_printf("[%s:%d]wanIf < 0 error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if(ipIndex < 0)
	{
		tcdbg_printf("[%s:%d]!ipIndex=%d\n",__FUNCTION__,__LINE__,ipIndex);
		return -1;
	}
	memset(wanNode, 0, sizeof(wanNode));		
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	memset(enable, 0, sizeof(enable));
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, "ISP", 0, tmp, sizeof(tmp));
	isp = atoi(tmp);

	switch(isp)
	{	
		case WAN_ENCAP_PPP_INT:			
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(wanNode, "BridgeInterface", 0, tmp, sizeof(tmp));
			if(strcmp(tmp, "No") == 0)
			{
				strncpy(enable, "1", sizeof(enable)-1);
			}
			else
			{
				strncpy(enable, "0", sizeof(enable)-1);
			}
			break;
		case WAN_ENCAP_BRIDGE_INT:
			strncpy(enable, "0", sizeof(enable)-1);
			break;
		case WAN_ENCAP_STATIC_INT:
			cfg_obj_get_object_attr(wanNode, "GATEWAY", 0, gateWay, sizeof(gateWay));
			cfg_obj_get_object_attr(wanNode, "GATEWAY6", 0, gateWay6, sizeof(gateWay6));
			strncpy(enable, "1", sizeof(enable)-1);
			break;
		default:			
			strncpy(enable, "1", sizeof(enable)-1);
			break;
	}
	
	memset(ipversion, 0, sizeof(ipversion));
	cfg_obj_get_object_attr(wanNode, "IPVERSION", 0, ipversion, sizeof(ipversion));
		
	if(strstr(ipversion, "IPv4")!= NULL )
	{
		idx = wanSync2Fwd4ByWanIf(wanIf, enable ,gateWay);
		if(idx >= 0)
		{				
			memset(strInterface, 0, sizeof(strInterface));
			snprintf(strInterface, sizeof(strInterface), TR181_IP_INTERFACE"%d", ipIndex + 1);	
			setInterfaceValue(TR181_ROUTE_IPV4_FORWARDING_NODE, idx, strInterface);
		}					
	}
#ifdef IPV6
	if(strstr(ipversion, "IPv6") != NULL )
	{
		idx = wanSync2Fwd6ByWanIf(wanIf, enable ,gateWay6);
		if(idx >= 0){				
			memset(strInterface,0,sizeof(strInterface));
			snprintf(strInterface, sizeof(strInterface), TR181_IP_INTERFACE"%d", ipIndex + 1);	
			setInterfaceValue(TR181_ROUTE_IPV6_FORWARDING_NODE, idx, strInterface);
		}						
	}
#endif
	return 0;
}

#if defined(TCSUPPORT_CT_DSL_EX)
int setTR181NodeEnable(char* name, int index, int active)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char enable[2] = {0};

	if(index >= 0)
	{
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", name, index + 1);
		
		snprintf(enable, sizeof(enable), "%d", active);
		cfg_obj_set_object_attr(nodeName, ENABLE_ATTR, 0, enable);
	}
	else
	{
		return -1;
	}

	return 0;
}

int createLayer1InterfacEntry(int WanIf, int etherLinkIndex)
{
	char lowerlayer[256] = {0};	
	int atmLinkIdx = -1;
	int ptmLinkIdx = -1;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char dslMode[8] = {0};
	char uplayer[256] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYS_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_get_object_attr(nodeName, "DslMode", dslMode, sizeof(dslMode));

	if( 0 == strcmp(dslMode,"ATM") )
	{
		atmLinkIdx = createATMLinkEntry(WanIf, 1); 	
		if(atmLinkIdx < 0)
		{
			tcdbg_printf("createATMLinkEntry fail!pvcIndex=%d\n", WanIf);
			return -1;
		}

		/*set lowerlayer of EtherLink*/
		snprintf(lowerlayer, sizeof(lowerlayer), TR181_ATMLINK_INTERFACE"%d", atmLinkIdx + 1);
		setEtherLinkLowerLayer(etherLinkIndex, lowerlayer);

		/*set uplayer of ATMLink*/
		snprintf(uplayer, sizeof(uplayer), TR181_ETHERNET_LINK"%d", etherLinkIndex+1);
		setATMLinkUpLayer(atmLinkIdx, uplayer);

		/*set lowerlayer of ATMLink*/
		setATMLinkLowerLayer(atmLinkIdx, TR181_DSL_CHANNEL_INTERFACE);

		/*set lowerlayer of DSLChannel*/
		setDSLChannelLowerLayer(0,TR181_DSL_LINE_INTERFACE);
		
		setTR181NodeEnable(TR181_ATMLINK_NODE, atmLinkIdx, 1);
	}
	else if( 0 == strcmp(dslMode,"PTM") )
	{		
		ptmLinkIdx = createPTMLinkEntry(WanIf, 1); 
		/*set lowerlayer of EtherLink*/
		setEtherLinkLowerLayer(etherLinkIndex, TR181_PTMLINK_INTERFACE);
	
		/*set lowerlayer of PTMLink*/
		setPTMLinkLowerLayer(0, TR181_DSL_CHANNEL_INTERFACE);

		/*set lowerlayer of DSLChannel*/
		setDSLChannelLowerLayer(0,TR181_DSL_LINE_INTERFACE);
		
		setTR181NodeEnable(TR181_PTMLINK_NODE, 0, 1);
	}

	setTR181NodeEnable(TR181_DSL_CHANNEL_NODE, 0, 1);
	setTR181NodeEnable(TR181_DSL_LINE_NODE, 0, 1);

	return 0;
}
#endif

int updateTR181IFByPvcIdx(int wanIf)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanPVCnode[MAXLEN_NODE_NAME] = {0};	
	char ipIFnodeName[MAXLEN_NODE_NAME] = {0};	
	char dhcpv4node[MAXLEN_NODE_NAME] = {0};
	char dhcpv6node[MAXLEN_NODE_NAME] = {0};
	char strActive[8]={0};	
	char isp[4]={0};
	char natEnable[20]={0};
	int active = 0;
	int type = 0;
	char ipversion[16] = {0};
	char strInterface[256] = {0};		
	char lowerlayer[256] = {0};	
	int ipIndex = -1;	
	int pppIndex = -1;
	int vlanTIndex = -1;	
	int etherLinkIndex = -1;	
	int iVlanTag = -1;
	int index = -1;
	int ret = -1;
	char uplayer[256] = {0};
	int pvcIndex = -1;
	int entIdx = -1;


	if(wanIf < 0)
	{
		tcdbg_printf("[%s:%d]wanif < 0 error!\n",__FUNCTION__,__LINE__);
		return 0;
	}

	if(wanIf >= WAN_INTERFACE_NUM)
	{
		/*only support 1 PTM, and not support ether wan*/
		tcdbg_printf("[%s:%d]wanif=%d, return \n",__FUNCTION__,__LINE__,wanIf);
		return 0;
	}
		
	ret = setWanNodeByWanIf(wanPVCnode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return 0;
	}
	
	if(cfg_obj_get_object_attr(wanPVCnode, "ISP", 0, isp, sizeof(isp)) < 0)
	{
		/*node not exist, exit*/				
		tcdbg_printf("[%s:%d]ISP not exist, wan not exist , ifIndex=%d no need to update \n",
		__FUNCTION__,__LINE__,pvcIndex);
		return 0;
	}		
	memset(strActive, 0, sizeof(strActive));
	cfg_obj_get_object_attr(wanPVCnode, "Active", 0, strActive, sizeof(strActive));
	if(strcmp(strActive, "Yes") == 0)
	{
		active = 1;
	}
	else
	{
		active = 0;
	}
		
	ipIndex = findIPInterfaceEntry(wanIf, active);	
	if(ipIndex < 0)
	{
		tcdbg_printf("findIPInterfaceEntry fail!wanIf=%d\n", wanIf);
		return -1;
	}
	

	etherLinkIndex = findEtherLinkEntry(wanIf, -1, active);
	if(etherLinkIndex < 0)
	{		
		tcdbg_printf("findEtherLinkEntry fail!wanIf=%d\n", wanIf);
		return -1;
	}
		
	iVlanTag = getWanVlanTag(wanIf);	
	if(iVlanTag >= 0)
	{	
		/*vlan exist*/
		vlanTIndex =findEtherVlanTermEntry(wanIf, active);
		wanSync2tr181EtherVlanT(vlanTIndex, wanIf);
	}
	
	memset(ipversion, 0, sizeof(ipversion));
	cfg_obj_get_object_attr(wanPVCnode, "IPVERSION", 0, ipversion, sizeof(ipversion));

	memset(isp, 0, sizeof(isp));
	cfg_obj_get_object_attr(wanPVCnode, "ISP", 0, isp, sizeof(isp));
	type = atoi(isp);
	if(type == WAN_ENCAP_PPP_INT)
	{
		pppIndex = findPPPInterfaceEntry(wanIf, active); 
		if(pppIndex < 0)
		{
			tcdbg_printf("createPPPInterfaceEntry fail!wanIf=%d\n", wanIf);
			return -1;
		}
		
		/*set lowerlayer of IPInterface*/
		snprintf(lowerlayer, sizeof(lowerlayer), TR181_PPP_INTERFACE"%d", pppIndex+1);
		setIPInterfaceLowerLayer(ipIndex,lowerlayer);
		
		if(iVlanTag >= 0)
		{	
		
			/*set uplayer of PPPInterface*/
			snprintf(uplayer, sizeof(uplayer), TR181_IP_INTERFACE"%d", ipIndex+1);
			setPPPInterfaceUpLayer(pppIndex,uplayer);
			
			/*set lowerlayer of PPPInterface*/
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_VLANTERMINATION"%d", vlanTIndex+1);
			setPPPInterfaceLowerLayer(pppIndex,lowerlayer);
						
			/*set uplayer of EtherVlanTerm*/
			snprintf(uplayer, sizeof(uplayer), TR181_PPP_INTERFACE"%d", pppIndex+1);
			setEtherVlanTermUpLayer(vlanTIndex,uplayer);
			
			/*set lowerlayer of EtherVlanTerm*/		
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_LINK"%d", etherLinkIndex+1);
			setEtherVlanTermLowerLayer(vlanTIndex, lowerlayer);

			/*set uplayer of EtherLink*/
			snprintf(uplayer, sizeof(uplayer), TR181_ETHERNET_VLANTERMINATION"%d", vlanTIndex+1);
			setEtherLinkUpLayer(etherLinkIndex,uplayer);
		}
		else
		{
			/*set uplayer of PPPInterface*/
			snprintf(uplayer, sizeof(uplayer), TR181_IP_INTERFACE"%d", ipIndex+1);
			setPPPInterfaceUpLayer(pppIndex,uplayer);
			
			/*set lowerlayer of PPPInterface*/
			sprintf(lowerlayer,TR181_ETHERNET_LINK"%d", etherLinkIndex+1);
			setPPPInterfaceLowerLayer(pppIndex, lowerlayer);
		}
			/*check  TR181_DHCPV4_CLIENT pvcIdx*/
			setDhcpv4ClientEnableState(wanIf, 0);
	}
	else
	{	
		if(iVlanTag >= 0)
		{
			/*set lowerlayer of IPInterface*/
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_VLANTERMINATION"%d", vlanTIndex+1);		
			setIPInterfaceLowerLayer(ipIndex, lowerlayer);

			/*set uplayer of EtherVlanTerm*/
			snprintf(uplayer, sizeof(uplayer), TR181_IP_INTERFACE"%d", ipIndex+1);
			setEtherVlanTermUpLayer(vlanTIndex,uplayer);
			
			/*set lowerlayer of EtherVlanTerm*/
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_LINK"%d", etherLinkIndex+1);
			setEtherVlanTermLowerLayer(vlanTIndex,lowerlayer);

			/*set uplayer of EtherLink*/
			snprintf(uplayer, sizeof(uplayer), TR181_ETHERNET_VLANTERMINATION"%d", vlanTIndex+1);
			setEtherLinkUpLayer(etherLinkIndex,uplayer);
		}
		else
		{
			/*set lowerlayer of IPInterface*/
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_LINK"%d", etherLinkIndex+1);	
			setIPInterfaceLowerLayer(ipIndex, lowerlayer);
		}
	

		if(type == WAN_ENCAP_DYN_INT)
		{
			if(strstr(ipversion, "IPv4") != NULL)
			{
				/*IPV4*/
				index = findDhcpv4ClientEntry(wanIf, 1);
				if(index < 0)
				{
					tcdbg_printf("[%s:%d]createDhcpv4ClientEntry fail\n",__FUNCTION__,__LINE__);
					return -1;
				}
				else
				{				
					memset(strInterface,0,sizeof(strInterface));
					snprintf(strInterface, sizeof(strInterface), TR181_IP_INTERFACE"%d", ipIndex+1);	
					setInterfaceValue(TR181_DHCPV4_CLIENT_NODE, index, strInterface);

					/*set enable */
					memset(dhcpv4node,0,sizeof(dhcpv4node));
					snprintf(dhcpv4node, sizeof(dhcpv4node), TR181_DHCPV4CLIENT_ENTRY_NODE, index + 1);
					cfg_obj_set_object_attr(dhcpv4node, TR181_ENABLE_ATTR, 0, "1");
				}
			}
			
		}
		else if(type == WAN_ENCAP_STATIC_INT)
		{
			/*set  TR181_DHCPV4_CLIENT */
			setDhcpv4ClientEnableState(wanIf, 0);
		}
		else
		{
			/*set  TR181_DHCPV4_CLIENT */
			setDhcpv4ClientEnableState(wanIf, 0);
		}
	}

	if( strstr(ipversion, "IPv4")!= NULL )
	{
		SnycIPIFv4AddressEntry(wanIf, ipIndex); 						
	}

#if defined(TCSUPPORT_CT_DSL_EX)
	index = createLayer1InterfacEntry(wanIf, etherLinkIndex);
	if(index < 0)
	{
		tcdbg_printf("[%s:%d]ifIndex=%d, etherLinkIndex=%d\n",__FUNCTION__,__LINE__,wanIf,etherLinkIndex);
		return -1;
	}
#else
	setEtherLinkLowerLayer(etherLinkIndex,TR181_OPTICAL_INTERFACE);
#endif

	cfg_obj_get_object_attr(wanPVCnode, "NATENABLE", 0, natEnable, sizeof(natEnable));
	
	/*set NAT.Interface*/
	if(strcmp(natEnable,TR181_ENABLE_ATTR) == 0)
	{
		index = findNATInterfaceEntry(wanIf, 1);
	}
	else
	{
		index = findNATInterfaceEntry(wanIf, 0);
	}
	
	if(index < 0)
	{
		tcdbg_printf("[%s:%d]createNATInterfaceEntry fail\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{	
		memset(strInterface,0,sizeof(strInterface));
		snprintf(strInterface, sizeof(strInterface), TR181_IP_INTERFACE"%d", ipIndex + 1);
		setInterfaceValue(TR181_NAT_INTERFACE_NODE, index, strInterface);
	}
	setDNSClientInfoByPvcIndex(wanIf, ipIndex, active);

	/*Routeforward4/6 init*/		
	wanSync2TR181RouteFwd(wanIf, ipIndex);
		
	/*sync v6 info*/
	syncWan2TR181V6Info(wanIf,ipIndex);

	return ipIndex;
}


int getLanIdxOfLowerLayer(char* node, int index){	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char lowerlayer[256] = {0};
	char lanIdx[8] = {0};
	int idx = 0;
	int ret = -1;
	int lowerLanIdx = -1;

	/*check lowerlayer*/	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName),"%s.entry.%d", node,index);	
	if( cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0,lowerlayer,sizeof(lowerlayer)) < 0 || strlen(lowerlayer) == 0 ){
		/*lowerlayer is not exist, wan connect info is not enough*/
		tcdbg_printf("[%s:%d]lowerlayer not exist\n",__FUNCTION__,__LINE__);
		return -1;
	}else{
		/*lowerlayer is exist, check next lowerlayer*/
		if(strstr(lowerlayer, TR181_BRIDGE_PORT) != NULL){
			/*the next layer is etherInterface*/	
			idx = getSuffixIntByInterface(lowerlayer);		
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName,sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, idx);
			if( cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0,lanIdx,sizeof(lanIdx)) >=0 && isNumber(lanIdx)){
				lowerLanIdx = atoi(lanIdx);
			}	
		}else if(strstr(lowerlayer, TR181_ETHER_INTERFACE) != NULL){
			/*the next layer is etherInterface*/	
			idx = getSuffixIntByInterface(lowerlayer);		
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName), TR181_ETHER_INTERFACE_ENTRY_NODE, idx);
			if(cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0,lanIdx,sizeof(lanIdx)) >=0 && isNumber(lanIdx)){
				lowerLanIdx = atoi(lanIdx);
			}	
		}else if(strstr(lowerlayer, TR181_WIFI_SSID_IF) != NULL){
			/*the next layer is wifi SSID:*/	
			idx = getSuffixIntByInterface(lowerlayer);		
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName),TR181_WIFISSID_ENTRY_NODE, idx);
			if(cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0,lanIdx,sizeof(lanIdx)) >=0 && isNumber(lanIdx)){
				lowerLanIdx = atoi(lanIdx);
			}
		}
	}
	return lowerLanIdx;
}



int clearTR181IFLanIdx(char *node, int num, int start, int lanIndex) {
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lanIdx[4] = {0};
	int i = 0;
	int lanIdx_get = 0;

	if(lanIndex <= 0) {
		tcdbg_printf("[%s:%d] lanIndex=%d, error!\n",__FUNCTION__,__LINE__,lanIndex);
		return -1;
	}

	for(i = start; i < num; i++) {
		memset(nodeName, 0, sizeof(nodeName));
		memset(lanIdx, 0, sizeof(lanIdx));
		snprintf(nodeName, "%s.entry.%d", node,i + 1);
		if(cfg_query_object(nodeName,NULL,NULL) < 0){
			cfg_create_object(nodeName);
		}
		if(cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, lanIdx,sizeof(lanIdx)) < 0 || strlen(lanIdx) == 0) {
			/*is not exist, check next entry*/
			tcdbg_printf("[%s:%d] \n",__FUNCTION__,__LINE__);
			continue;
		}
		lanIdx_get = atoi(lanIdx);
		if(lanIdx_get == 0) {
			/*br0 entry*/
			tcdbg_printf("[%s:%d] lanIndex=%d, do nothing!\n",__FUNCTION__,__LINE__,lanIdx_get);
			continue;
		}
		if(lanIdx_get == lanIndex) {
			/*clear lanIndex*/
			tcdbg_printf("[%s:%d] nodeName=%s, lanIndex=%d clear!\n",__FUNCTION__,__LINE__,nodeName,lanIndex);
			cfg_obj_set_object_attr(nodeName, LAN_INDEX_ATTR, 0,"");
		}
	}

	return -1;
}



int clearLanIdxOfTR181Interface(int lanIndex) {
	clearTR181IFLanIdx(TR181_BRIDGE_PORT_NODE, BRIDGE_PORT_NUM, 0, lanIndex);
	clearTR181IFLanIdx(TR181_WIFISSID_NODE, WIFI_SSID_NUM_MAX, 0, lanIndex);
	return 0;
}


int checkWifiRadioEnable(int index){
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};
	char enable[4] = {0};

	if(index < 0){
		tcdbg_printf("[%s:%d] index=%d error\n",__FUNCTION__,__LINE__,index);	
		return -1;
	}
			
	memset(tmp,0,sizeof(tmp));
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName),TR181_WIFIRDO_ENTRY_NODE, index +1);

	/*check enable*/
	if( cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0,enable,sizeof(enable)) >= 0 && strcmp(enable, "1") == 0){
		tcdbg_printf("[%s:%d]enable  pass, \n",__FUNCTION__,__LINE__);
		return 1;
	}else{
		tcdbg_printf("[%s:%d]enable not pass, \n",__FUNCTION__,__LINE__);
		return 0;
	}
}


int checkEtherIntefaceEnable(int index){
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};
	char enable[4] = {0};

	if(index < 1){
		tcdbg_printf("[%s:%d] index=%d error\n",__FUNCTION__,__LINE__,index);	
		return -1;
	}
			
	memset(tmp,0,sizeof(tmp));
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName),TR181_ETHER_INTERFACE_ENTRY_NODE, index);

	/*check enable*/
	if( cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0,enable,sizeof(enable)) >= 0 && strcmp(enable, "1") == 0){
		tcdbg_printf("[%s:%d]enable  pass, \n",__FUNCTION__,__LINE__);
		return 1;
	}else{
		tcdbg_printf("[%s:%d]enable not pass, \n",__FUNCTION__,__LINE__);
		return 0;
	}
}


int checkWifiSSIDEnable(int index){
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};
	char enable[4] = {0};
	int ret = -1;
	int nextEntryIdx = -1;

	if(index < 1){
		tcdbg_printf("[%s:%d] index=%d error\n",__FUNCTION__,__LINE__,index);	
		return -1;
	}
		
	memset(tmp,0,sizeof(tmp));
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName),TR181_WIFISSID_ENTRY_NODE, index);
	if( cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0,tmp,sizeof(tmp)) < 0 || strlen(tmp) == 0 ){		
		tcdbg_printf("[%s:%d]lowerlayer not set.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	nextEntryIdx = getSuffixIntByInterface(tmp) -1 ;
	if( nextEntryIdx < 0){
		return -1;
	}

	
	if(strstr(tmp, TR181_WIFI_RDAIO) != NULL){
		ret = checkWifiRadioEnable(nextEntryIdx);			
	}
	else{
		/*not support other lowerlayer, return 0;*/
		return -1;
	}
	
	/*check enable*/
	if( cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0,enable,sizeof(enable)) >= 0 && strcmp(enable, "1") == 0 && ret > 0){
		tcdbg_printf("[%s:%d]enable  pass, \n",__FUNCTION__,__LINE__);
		return 1;
	}else{
		tcdbg_printf("[%s:%d]enable not pass, \n",__FUNCTION__,__LINE__);
		return 0;
	}
}


int checkBridgePortEnable(int portIdx){
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};
	char enable[4] = {0};
	int ret = -1;
	int nextEntryIdx = -1;

	if(portIdx < 1){
		tcdbg_printf("[%s:%d] port=%d error\n",__FUNCTION__,__LINE__,portIdx);	
		return -1;
	}
			
	memset(tmp,0,sizeof(tmp));
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, portIdx);
	if( cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0,tmp,sizeof(tmp)) < 0 || strlen(tmp) == 0 ){		
		tcdbg_printf("[%s:%d]lowerlayer not set.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	nextEntryIdx = getSuffixIntByInterface(tmp);
	if( nextEntryIdx < 1){
		tcdbg_printf("[%s:%d]nextEntryIdx < 0 \n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(updateInterfaceNodeDbg()){
		tcdbg_printf("[%s:%d]nextEntryIdx =%d, portIdx=%d \n",__FUNCTION__,__LINE__,nextEntryIdx, portIdx);
	}

	if(strstr(tmp, TR181_ETHER_INTERFACE) != NULL){
		/*the next layer is Ether.Interface*/	
		ret = checkEtherIntefaceEnable(nextEntryIdx);	
		
		tcdbg_printf("[%s:%d]ret =%d\n",__FUNCTION__,__LINE__,ret);
	}
	else if(strstr(tmp, TR181_WIFI_SSID_IF) != NULL){
		
		/*the next layer is Wifi.SSID: for wlan*/	
		ret = checkWifiSSIDEnable(nextEntryIdx);
	}
	else{
		/*not support other lowerlayer, return 0;*/
		return -1;
	}
	
	/*check enable, if bridge port enable =1 && wifi ssid enable=0,we will enable this port*/
	if( cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0,enable,sizeof(enable)) >= 0 && strcmp(enable, "1") == 0 && ret == 0){
		tcdbg_printf("[%s:%d]enable  pass, \n",__FUNCTION__,__LINE__);
		return 1;
	}else{
		tcdbg_printf("[%s:%d]enable not pass, \n",__FUNCTION__,__LINE__);
		return 0;
	}
}


int  createBridgePortEntry(int wanIf,int lanIndex,int active){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char strEnable[4]={0};
	int i = 0;
	char tmp[20] = {0};
	int num = 0;
	int fisrtEmptyEntry = -1;
	char strIndex[4]={0};
	int ret = 0;
	int exist_flag = 0;
	int index = -1;

	if(lanIndex == 0){
		/*for br0*/
		ret = createBridgePortEntry0();
		if(ret < 0){
			tcdbg_printf("[%s:%d]:createBridgePortEntry0 fail\n",__FUNCTION__,__LINE__);
			return -1;
		}
	}

	for(i = 1; i < BRIDGE_PORT_NUM; i++){		
		memset(nodeName,0,sizeof(nodeName));		
		snprintf(nodeName,sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, i + 1);
		if(cfg_obj_get_object_attr(nodeName, TR181_ENABLE_ATTR, 0, strEnable,sizeof(strEnable)) < 0 ){
			/*is not exist, find an empty entry*/
			if(fisrtEmptyEntry < 0){
				fisrtEmptyEntry = i;
			}
			continue;
		}

		if(lanIndex > 0){
			/*for lan of wlan*/			
			memset(strIndex,0,sizeof(strIndex));		
			cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0,strIndex,sizeof(strIndex));
			if(strlen(strIndex) == 0 || strcmp(strIndex,"-1")==0 ){					
				if(updateInterfaceNodeDbg()){
					tcdbg_printf("[%s:%d]:not lan/wlan interface\n",__FUNCTION__,__LINE__);
				}
			}else{
				/*find exist lan port index(BridgePortEntry)*/
				if(lanIndex == atoi(strIndex)){
					exist_flag = 1;
					break;
				}
			}
		}
	}

	if(updateInterfaceNodeDbg()){
		tcdbg_printf("[%s:%d]:i =%d,pvcIndex=%d,lanIndex=%d,fisrtEmptyEntry=%d \n",__FUNCTION__,__LINE__,i,wanIf,lanIndex,fisrtEmptyEntry);
	}
	
	if( i != BRIDGE_PORT_NUM ){		
		/*find exist entry*/
		index = i;
	}else if(fisrtEmptyEntry >= 0 ){
		/*find empty entry*/
		index = fisrtEmptyEntry;
	}else{
		/*not find empty entry*/
		return -1;
	}
	
	memset(nodeName,0,sizeof(nodeName));		
	snprintf(nodeName,sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, index + 1);
	if(cfg_query_object(nodeName,NULL,NULL) <= 0){		
		cfg_obj_create_object(nodeName);
	}

	memset(tmp,0,sizeof(tmp));		
	snprintf(tmp,sizeof(tmp),"%d", active);
	cfg_obj_set_object_attr(nodeName, TR181_ENABLE_ATTR, 0, tmp);		
	
	memset(tmp,0,sizeof(tmp));		
	snprintf(tmp,sizeof(tmp),"%d",wanIf);
	cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);

	memset(tmp,0,sizeof(tmp));		
	snprintf(tmp,sizeof(tmp),"%d",lanIndex);
	cfg_obj_set_object_attr(nodeName, LAN_INDEX_ATTR, 0,tmp);	
	cfg_obj_set_object_attr(nodeName, "ManagementPort", 0, "0");

	if(updateInterfaceNodeDbg()){
		tcdbg_printf("[%s:%d]:exist_flag =%d ,exist_flag =%d\n",__FUNCTION__,__LINE__,exist_flag,exist_flag);
	}
	
	if(exist_flag == 0){
		/*new node, num++*/		
		memset(tmp,0,sizeof(tmp));		
		cfg_obj_get_object_attr(TR181_BRIDGEPORT_COMMON_NODE, TR181_NUM_ATTR, 0,tmp,sizeof(tmp));
		num = atoi(tmp);	
		memset(tmp,0,sizeof(tmp));		
		snprintf(tmp,sizeof(tmp), "%d", num+1);
		cfg_obj_set_object_attr(TR181_BRIDGEPORT_COMMON_NODE, TR181_NUM_ATTR, 0,tmp);
		if(updateInterfaceNodeDbg()){
			tcdbg_printf("[%s:%d]:num =%d \n",__FUNCTION__,__LINE__,num);
		}
	}
	return index;	
}


int getLanPortNum(){
	char tmp[20] = {0};
	int num = 0;
	
	if(cfg_obj_get_object_attr(LAN_COMMON_NODE, TR181_NUM_ATTR, 0,tmp,sizeof(tmp)) >= 0 ){
		num = atoi(tmp);
		if((num <= 0) || (num > LAN_PORT_NUM))
			num = LAN_PORT_NUM;
	}
	else{
		num = LAN_PORT_NUM;
	}

	return num;
}


void getBr0IPInterface(char *Interface, int len)
{
	int j = 0;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lanIndex[8] = {0};
	
	for(j = 0; j < IP_INTERFACE_NUM; j++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName,sizeof(nodeName), TR181_IP_ENTRY_NODE, j + 1);
		if(cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0,lanIndex,sizeof(lanIndex)) >= 0)
		{
			if(!strcmp(lanIndex,"0"))
			{
				snprintf(Interface, len,"%s%d", TR181_IP_INTERFACE, j + 1);
			}
		}
	}
	return;
}


int updateTR181AllWanInterface(void)
{
	int i = 0, j = 0;
	int ret = -1;
	int wanif = -1;

	for(i = 0; i < PVC_NUM; i++)
	{			
		for(j = 0; j < MAX_SMUX_NUM; j++)
		{
			wanif = i*PVC_NUM + j;
			ret = updateTR181IFByPvcIdx(wanif);
			if(ret < 0)
			{
				tcdbg_printf("[%s:%d] fail\n",__FUNCTION__,__LINE__);
				return -1;
			}
		}
	}
	
	return 0;
}

int updateTR181AllLanInterface(void)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	int i = 0;
	int ipIndex = -1;	
	int etherLinkIndex = -1;	
	int bridgePortIndex = -1;	
	int etherInterfaceIndex = -1;
	char lowerlayer[256] = {0};
	char upperlayer[256] = {0};
	int num = 0;

	ipIndex = findLanIPInterfaceEntry();
	if(ipIndex < 0){
		return -1;
	}
	SnycIPIFv4AddressEntry(-1, ipIndex);
	SnycIPIFv6AddressEntry(-1, ipIndex);
	/*sync Radvd to TR181IPV6PRE_PVC0_Entry0*/
	syncRadvd2routeAdvIfSet(ipIndex);
	/*sync Dhcp6s to TR181IPV6PRE_PVC0_Entry1*/
	syncDhcp6s2LanPreFix(ipIndex);
	etherLinkIndex = findEtherLinkEntry(-1,0,1);
	if(etherLinkIndex < 0){
		return -1;
	}
	
	/*set lowerlayer of IPInterface*/
	snprintf(lowerlayer, sizeof(lowerlayer),TR181_ETHERNET_LINK"%d", etherLinkIndex+1);		
	setIPInterfaceLowerLayer(ipIndex, lowerlayer);
	
	cfg_obj_set_object_attr(TR181_BRIDGE_COMMON_NODE, TR181_ENABLE_ATTR, 0, "1");
	
	/*br0:BridgePort_Entry0*/
	createBridgePortEntry0();

	
	/*set lowerlayer of etherlink*/
	memset(lowerlayer,0,sizeof(lowerlayer));
	setEtherLinkLowerLayer(etherLinkIndex, TR181_BRIDGE_PORT_BR0);

	/*set upperlayer of etherlink*/
	snprintf(upperlayer, sizeof(upperlayer), TR181_IP_INTERFACE"%d", ipIndex+1);
	setEtherLinkUpLayer(etherLinkIndex, upperlayer);

	if(updateInterfaceNodeDbg()){
		tcdbg_printf("[%s:%d]nodeName =%s \n",__FUNCTION__,__LINE__,nodeName);
	}	

	num = getLanPortNum();
	for(i = 1 ; i <= num; i++){
		bridgePortIndex = 	createBridgePortEntry(-1,i,1);
		if(bridgePortIndex < 0){			
			return -1;
		}
			
		/*set br0 lowerlayer*/
		memset(lowerlayer,0,sizeof(lowerlayer));
		snprintf(lowerlayer, sizeof(lowerlayer),TR181_BRIDGE_PORT"%d", bridgePortIndex+1);		
		setBridgePortLowerLayer(0, lowerlayer);	
		/*set br0 upperlayer*/
		memset(upperlayer,0,sizeof(upperlayer));
		snprintf(upperlayer, sizeof(upperlayer),TR181_ETHERNET_LINK"%d", etherLinkIndex+1); 	
		setBridgePortUpperLayer(0, upperlayer);	

		if(i <= num)
		{/*Lan*/
			etherInterfaceIndex = findEtherInterfaceEntry(i);
			if(etherInterfaceIndex < 0){			
				return -1;
			}
			
			/*set  lowerlayer*/
			memset(lowerlayer,0,sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer),TR181_ETHER_INTERFACE"%d", etherInterfaceIndex+1); 	
			setBridgePortLowerLayer(bridgePortIndex, lowerlayer); 
			/*set upperlayer*/
			memset(upperlayer,0,sizeof(upperlayer));
			setBridgePortUpperLayer(bridgePortIndex, TR181_BRIDGE_PORT_BR0);
			
			snprintf(upperlayer, sizeof(upperlayer),TR181_BRIDGE_PORT"%d", bridgePortIndex+1);		
			setEtherInterfaceUpperLayer(etherInterfaceIndex, upperlayer);
			}
	}

	updateTR181AllWLanInterface();
	
	return 0;
}

int updateAllStaticRoute4(void){
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[32] = {0};
	int i = 0;	
	int index = -1, pvcIndex = -1;

	for(i = 0; i < CFG_TYPE_MAX_ROUTE_ENTRY_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		memset(tmp, 0, sizeof(tmp));
		snprintf(nodeName, sizeof(nodeName), ROUTE_ENTRY_NODE, i + 1);

		if(cfg_obj_get_object_attr(nodeName, "DST_IP", 0, tmp, sizeof(tmp)) < 0)
		{
			/*Entry not exist, find next entry*/
			continue;
		}

		/*create RouteForward4 entry*/
		index = syncStaticRoute2RouteFwd4(i);
		if(index < 0)
		{			
			return -1;
		}
	}
	return 0;
}


int updateAllStaticRoute6(void){
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char tmp[32] = {0};
	int i = 0;	
	int index = -1, pvcIndex = -1;

	for(i = 0; i < CFG_TYPE_MAX_ROUTE6_ENTRY_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		memset(tmp, 0, sizeof(tmp));
		snprintf(nodeName, sizeof(nodeName), ROUTE6_ENTRY_NODE, i + 1);

		if(cfg_obj_get_object_attr(nodeName, "DST_IP", 0, tmp, sizeof(tmp)) < 0)
		{
			/*Entry not exist, find next entry*/
			continue;
		}

		/*create RouteForward4 entry*/
		index = syncStaticRoute2RouteFwd6(i);
		if(index < 0)
		{			
			return -1;
		}
	}
	return 0;
}



int updateInterfaceStack(void)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[20] = {0};
	int i = 0;	
	int interfaceStackIdx = 0;
	
	
	clearTR181InterfaceNode(TR181_INTERFACE_STACK_NODE, INTERFACE_STACK_NUM);

	for(i = 0; i < IP_INTERFACE_NUM; i++)
	{
		interfaceStackIdx = setIPInterfaceStackNode(interfaceStackIdx,i);
	}

	/*set NUM*/
	memset(nodeName,0,sizeof(nodeName));	
	memset(tmp,0,sizeof(tmp));
	strncpy(nodeName, TR181_INTERFACE_STACK_COMMON_NODE, sizeof(nodeName) - 1);
	
	snprintf(tmp, sizeof(tmp), "%d", interfaceStackIdx);
	cfg_obj_set_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp);

	return interfaceStackIdx;
}

void  datetimeFormat(int iTime, char *pszTime, int buf_len)
{
	time_t tCurrent;
	struct tm tmFmtDatetime;
	struct tm *ptmFmtDatetime = NULL;

	if (NULL == pszTime) 
	{
		return;
	}	
	if (iTime == 0) 
	{
		return;
	}
	tCurrent = iTime;
	ptmFmtDatetime = localtime_r(&tCurrent, &tmFmtDatetime);
	if(ptmFmtDatetime == NULL)
	{ 
		return;
	}
	snprintf(pszTime, buf_len, "%04d-%02d-%02dT%02d:%02d:%02dZ",
		ptmFmtDatetime->tm_year+1900,
		ptmFmtDatetime->tm_mon+1,
		ptmFmtDatetime->tm_mday,
		ptmFmtDatetime->tm_hour,		
		ptmFmtDatetime->tm_min,
		ptmFmtDatetime->tm_sec);
}

int clearQos()
{
	clearQosApp();
	clearQosClsficn();
	clearQosPolicer();
	clearQosQue();

	return SUCCESS;
}


int updateTr181QosAll()
{
	clearQos();
	initQosApp();	
	initQosClsficn();
	initQosPolicer();
	initQosQue();

	return SUCCESS;
}

int updateTr181QosByQoSNode()
{
	char buf[8] = {0};
	memset(buf, 0, sizeof(buf));
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, buf, sizeof(buf));
	if(strcmp(buf,"1"))
	{	
		/*not commit from tr181*/
		clearQosClsficn();
		initQosClsficn();
		initQosPolicer();
		initQosQue();
	}

	return SUCCESS;
}


int updateTR181AllInterface(void)
{	
	int ret = -1;

	clearAllInterfaceNode();
	
	/*lan interface: eth1~4,ra1~4,rai1~4*/
	ret = updateTR181AllLanInterface();	
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]:updateTR181AllLanInterface fail \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*wan interface*/	
	ret = updateTR181AllWanInterface();
	if(ret < 0){
		tcdbg_printf("[%s:%d]:updateTR181AllWanInterface fail \n",__FUNCTION__,__LINE__);
		return -1;
	}
	/*updatePortMappingNode();*/ /*virserver question*/
	updateAllStaticRoute4();

	updateAllStaticRoute6();
	
	updateInterfaceStack();

	updateTR181FireWall();
		
	updateFireWallChainRule();	

	updateUsers();

	updateDnsRelay();
	
	updateDHCPv4SFromLan();
	updateDHCPv6Server();
	
	updateDHCPv6ServerPool();

	updateTr181QosAll();

	updateAllPortMapNode();
	return 0;
}

int getPvcIdxByTR181NodeEntry(char* node, int index)
{ 
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[8] = {0};
	int pvcIdx = -1;

	if(index < 1)
	{
		tcdbg_printf("[%s:%d]index = %d error\n",__FUNCTION__,__LINE__, index);
		return -1;
	}
	
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, index);

	memset(tmp,0,sizeof(tmp));		
	if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0) && tmp[0] != '\0')
	{								
		pvcIdx = atoi(tmp);			
	}

	return pvcIdx;
}

int setPvcIdxByTR181NodeEntry(char* node, int index, int wanIf)
{ 
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[8] = {0};

	if(index < 1)
	{
		return -1;
	}
	
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, index);

	memset(tmp,0,sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), "%d", wanIf);
	cfg_obj_set_object_attr(nodeName, "pvcIndex", 0, tmp);
	
	return 0;
}


int getUpperIfByLowerlayerIf(char* node, int num,  char* strInterface)
{ 
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	int i = 0;

	for( i = 0; i < num; i++)
	{		
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i + 1);

		memset(lowerlayer,0,sizeof(lowerlayer));		
		if((cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) >= 0) 
			&& lowerlayer[0] != '\0')
		{ 							
			if(updateInterfaceNodeDbg())
			{
				tcdbg_printf("[%s:%d]lowerlayer=%s, i = %d\n",__FUNCTION__,__LINE__,lowerlayer,i);
			}		

			if(strcmp(lowerlayer, strInterface) == 0)
			{
				return i;
			}
		}
	}

	return -1;
}

int getIPIfByLowerlayerIf(char* strInterface)
{ 
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char *tmp = NULL;
	char EtherLinkNode[32] = {0};
	char pvcIdx[2] = {0};
	char Active[8] = {0};
	int i = 0, j = 0;
	int ipIfIdx = -1;
	int upIdx = -1;

	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d]strInterface=%s !\n",__FUNCTION__,__LINE__,strInterface);
	}
	
	if(strstr(strInterface, TR181_PPP_INTERFACE) != NULL)
	{
		ipIfIdx = getUpperIfByLowerlayerIf(TR181_IP_NODE, IP_INTERFACE_NUM, strInterface);
		if(updateInterfaceNodeDbg()){		
			tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
		}
		return ipIfIdx;
	}

	if(strstr(strInterface, TR181_ETHERNET_VLANTERMINATION) != NULL){		
		upIdx = getUpperIfByLowerlayerIf(TR181_PPP_NODE, PPP_INTERFACE_NUM, strInterface);
		if(upIdx >= 0)
		{			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_PPP_INTERFACE"%d", upIdx+1);
			ipIfIdx = getIPIfByLowerlayerIf(lowerlayer);
			
			if(updateInterfaceNodeDbg())
			{		
				tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
			}
			return ipIfIdx;
		}
	
		ipIfIdx = getUpperIfByLowerlayerIf(TR181_IP_NODE, IP_INTERFACE_NUM, strInterface);
		
		if(updateInterfaceNodeDbg())
		{		
			tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
		}
		return ipIfIdx;
	}	
#if defined(TCSUPPORT_CT_DSL_EX)
	if(strstr(strInterface, TR181_ETHERNET_LINK) != NULL)
	{		
		upIdx = getUpperIfByLowerlayerIf(TR181_PPP_NODE, PPP_INTERFACE_NUM, strInterface);
		if(upIdx >= 0)
		{			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_PPP_INTERFACE"%d", upIdx+1);
			ipIfIdx = getIPIfByLowerlayerIf(lowerlayer);
			
			if(updateInterfaceNodeDbg())
			{		
				tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
			}
			return ipIfIdx;
		}

		upIdx = getUpperIfByLowerlayerIf(TR181_ETHERVLANT_NODE, ETHER_VLAN_T_NUM, strInterface);
		if(upIdx >= 0)
		{			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_VLANTERMINATION"%d", upIdx+1);
			ipIfIdx = getIPIfByLowerlayerIf(lowerlayer);
			
			if(updateInterfaceNodeDbg())
			{		
				tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
			}
			return ipIfIdx;
		}
	
		ipIfIdx = getUpperIfByLowerlayerIf(TR181_IP_NODE, IP_INTERFACE_NUM, strInterface);
		
		if(updateInterfaceNodeDbg())
		{		
			tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
		}
		return ipIfIdx;
	}

	if(strstr(strInterface, TR181_ATMLINK_INTERFACE) != NULL 
		|| strstr(strInterface, TR181_PTMLINK_INTERFACE) != NULL)
	{		
		upIdx = getUpperIfByLowerlayerIf(TR181_ETHERLINK_NODE, ETHER_LINK_NUM, strInterface);
		if(upIdx >= 0){			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ETHERNET_LINK"%d", upIdx+1);
			ipIfIdx = getIPIfByLowerlayerIf(lowerlayer);
			
			if(updateInterfaceNodeDbg())
			{		
				tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
			}
			return ipIfIdx;
		}	
	}
	
	if(strstr(strInterface, TR181_DSL_CHANNEL_INTERFACE) != NULL)
	{		
		upIdx = getUpperIfByLowerlayerIf(TR181_ATMLINK_NODE, ATM_LINK_NUM, strInterface);
		if(upIdx >= 0){			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_ATMLINK_INTERFACE"%d", upIdx+1);
			ipIfIdx = getIPIfByLowerlayerIf(lowerlayer);
			
			if(updateInterfaceNodeDbg())
			{		
				tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
			}
			return ipIfIdx;
		}	

		upIdx = getUpperIfByLowerlayerIf(TR181_PTMLINK_NODE, PTM_LINK_NUM, strInterface);
		if(upIdx >= 0)
		{			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer), TR181_PTMLINK_INTERFACE"%d", upIdx+1);
			ipIfIdx = getIPIfByLowerlayerIf(lowerlayer);
			 
			if(updateInterfaceNodeDbg())
			{		
				tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
			}
			return ipIfIdx;
		}
	}
#endif
	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d]strInterface=%s,ipIfIdx=%d !\n",__FUNCTION__,__LINE__,strInterface,ipIfIdx);
	}

	return -1;
}

int setTR181NodeWanIf(char* node, int num, int start, int oldWanIf, int newWanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char pvcIdx[8] = {0};
	int newPvcIndex = -1;
	int i = 0;

	if(oldWanIf < 0)
	{
		tcdbg_printf("[%s:%d] oldWanIf=%d error!\n", __FUNCTION__, __LINE__, oldWanIf);
		return -1;
	}

	for(i = start; i < num; i++)
	{
		/*check lowerlayer*/
		memset(nodeName, 0, sizeof(nodeName));	
		memset(pvcIdx, 0, sizeof(pvcIdx));	
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i + 1);
		if( (cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx, sizeof(pvcIdx)) < 0)
		 || strlen(pvcIdx) == 0 )
		{
			/*is not exist, check next entry*/
			continue;
		}

		if(atoi(pvcIdx) == oldWanIf) 
		{
			if(newWanIf < 0)
			{
				cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, "");
			}
			else
			{		
				memset(pvcIdx, 0, sizeof(pvcIdx));
				snprintf(pvcIdx, sizeof(pvcIdx), "%d", newWanIf);
				cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx);
			}
		}
	}

	return -1;
}

int setRouteWanPvcIdxOfTR181IF(int oldWanIf, int newWanIf)
{
	setTR181NodeWanIf(TR181_IP_NODE, IP_INTERFACE_NUM, 0, oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_PPP_NODE, PPP_INTERFACE_NUM, 0,oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_ETHERLINK_NODE, ETHER_LINK_NUM, 0, oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_ETHERVLANT_NODE, ETHER_VLAN_T_NUM, 0, oldWanIf, newWanIf);
#if defined(TCSUPPORT_CT_DSL_EX)
	setTR181NodeWanIf(TR181_ATMLINK_NODE, ATM_LINK_NUM, 0, oldWanIf, newWanIf);	
	setTR181NodeWanIf(TR181_PTMLINK_NODE, PTM_LINK_NUM, 0, oldWanIf, newWanIf);	
#endif

	setTR181NodeWanIf(TR181_DHCPV4_CLIENT_NODE, DHCPV4_CLIENT_NUM_MAX, 0,oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_DHCPV6_CLIENT_NODE, DHCPV6_CLIENT_NUM_MAX, 0, oldWanIf, newWanIf);
	
	setTR181NodeWanIf(TR181_DNS_CLIENT_NODE, DNS_CLIENT_NUM, 0, oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_NAT_INTERFACE_NODE, NAT_INTERFACE_NUM, 0, oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_ROUTE_IPV4_FORWARDING_NODE, TR181_ROUTE_IPV4_FORWARDING_NUM, 0, oldWanIf, newWanIf);
	setTR181NodeWanIf(TR181_ROUTE_IPV6_FORWARDING_NODE, TR181_ROUTE_IPV6_FORWARDING_NUM, 0, oldWanIf, newWanIf);
	
	return 0;
}

void unsetWanByPvcEnt(int index)
{
	char node[MAXLEN_NODE_NAME] = {0};
	int pvcIndex = -1;
	int entIndex = -1;

	if(index >= 0)
	{
		pvcIndex = index/PVC_NUM;
		entIndex = index%PVC_NUM;
		
		memset(node,0,sizeof(node)) ;
		snprintf(node, sizeof(node), WAN_PVC_ENTRY_NODE, pvcIndex + 1, entIndex + 1);
		cfg_tr181_delete_object(node);
	}
	
	return ;
}

int getSuffixIntByInterface(char * interface)
{
	char *p = NULL;
	int ret = -1;

	if(NULL == interface)
	{
		return -1;
	}

	if('\0' == interface[0])
	{
		return -1;
	}


	p = strrchr(interface, '.');
	if(p == NULL){
		return -1;
	}
	
	ret = atoi(p + 1);
	if(ret > 0)
	{
		return ret;
	}
	return 0;
}

void dunmpLowerLayerStack(struct lowerlayer_stack_s lowerlayer_stack)
{
	tcdbg_printf("[%s:%d] ************************\n",__FUNCTION__,__LINE__);
	tcdbg_printf("[%s:%d] ipIFIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.ipIFIdx);
	tcdbg_printf("[%s:%d] pppIFIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.pppIFIdx);
	tcdbg_printf("[%s:%d] vlanTIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.vlanTIdx);
	tcdbg_printf("[%s:%d] EtherLinkIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.EtherLinkIdx);
	tcdbg_printf("[%s:%d] bridgePortIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.bridgePortIdx);
	tcdbg_printf("[%s:%d] atmLinkIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.atmLinkIdx);
	tcdbg_printf("[%s:%d] ptmLinkIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.ptmLinkIdx);
	tcdbg_printf("[%s:%d] dslChannelIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.dslChannelIdx);
	tcdbg_printf("[%s:%d] opticalIFIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer_stack.opticalIFIdx);
	tcdbg_printf("[%s:%d] ************************\n",__FUNCTION__,__LINE__);
}

int getTR181NodeEntryIdxByIPIf(char* node, int num, int ipIfIdx)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char strInterface[256] = {0};
	int i = 0;
	int idx = -1;
	
	for( i = 0; i < num; i++)
	{		
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i + 1);

		memset(strInterface,0,sizeof(strInterface));		
		if((cfg_obj_get_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, strInterface, sizeof(strInterface)) >= 0)
			&& strInterface[0] != '\0')
		{								
			idx = getSuffixIntByInterface(strInterface) -1 ;
			if(updateInterfaceNodeDbg())
			{
				tcdbg_printf("[%s:%d]strInterface=%s, i = %d\n",__FUNCTION__,__LINE__,strInterface,i);
			}		
			
			if(idx == ipIfIdx)
			{
				/*find the IP.interface*/
				return i;
			}			
		}
	}
	
	return -1;
}

int getTR181NodeEntryIdxByWanIf(char* node, int num, int wanIf)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char value[256] = {0};
	int i = 0;
	
	for( i = 0; i < num; i++)
	{		
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i + 1);

		memset(value,0,sizeof(value));		
		if(cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, value, sizeof(value)) > 0)
		{								
			if(atoi(value) == wanIf)
				return i;
		}
	}
	
	return -1;
}

int inet_pton4(const char *src,u_char *dst)
{
	int saw_digit, octets, ch;
	u_char tmp[4], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') 
	{

		if (ch >= '0' && ch <= '9') 
		{
			u_int new = *tp * 10 + (ch - '0');

			if (new>255)
				return (0);
			*tp = new;
			if (! saw_digit) 
			{
				if (++octets>4)
					return (0);
				saw_digit = 1;
			}
		} 
		else if (ch == '.' && saw_digit) 
		{
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} 
		else
			return (0);
	}
	if (octets < 4)
		return (0);
	memcpy(dst, tmp, 4);
	return (1);
}

int inet_pton6(src, dst)
	const char *src;
	unsigned char *dst;
{
	static const char xdigits_l[] = "0123456789abcdef",
			  xdigits_u[] = "0123456789ABCDEF";
	unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	unsigned int val;

	memset((tp = tmp), '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') 
	{
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) 
		{
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') 
		{
			curtok = src;
			if (!saw_xdigit) 
			{
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			}
			if (tp + NS_INT16SZ > endp)
				return (0);
			*tp++ = (unsigned char) (val >> 8) & 0xff;
			*tp++ = (unsigned char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
		    inet_pton4(curtok, tp) > 0) 
		{
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) 
	{
		if (tp + NS_INT16SZ > endp)
			return (0);
		*tp++ = (unsigned char) (val >> 8) & 0xff;
		*tp++ = (unsigned char) val & 0xff;
	}
	if (colonp != NULL) 
	{
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		for (i = 1; i <= n; i++) 
		{
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	memcpy(dst, tmp, NS_IN6ADDRSZ);
	return (1);
}

int findEmptyUntagWanPVCIdx(void)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char attrValue[64] = {0};
	int i = 0, j = 0;
	int ret = -1;
	int untagPvc = -1;
	int emptyPvc = -1;
	int pvcIdx = -1;


	for (i = 0; i < PVC_NUM; i++) 
	{
		memset(nodeName, 0, sizeof(nodeName));			
		memset(attrValue, 0, sizeof(attrValue));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, i + 1);
		if(cfg_obj_get_object_attr(nodeName, "VLANMode", 0, attrValue, sizeof(attrValue)) < 0)
		{
			if(emptyPvc < 0){
				emptyPvc = i;
			}
			continue;
		}

		if((strcmp(attrValue, "UNTAG") == 0)){
			if(untagPvc < 0){
				untagPvc = i;
			}
		}	
	}

	if(untagPvc >= 0){
		pvcIdx = untagPvc;
	}else if(emptyPvc >= 0){
		pvcIdx = emptyPvc;
	}else{
		return -1;
	}
			
	for (j = 0; j < MAX_SMUX_NUM; j++) 
	{
		memset(nodeName, 0, sizeof(nodeName));			
		memset(attrValue, 0, sizeof(attrValue));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvcIdx + 1, j + 1);
		ret = cfg_obj_get_object_attr(nodeName, "Active", 0, attrValue, sizeof(attrValue));	
		if(ret < 0)
		{	 /*this entry is not exist, so we use this entry for new wan.*/
			tcdbg_printf("[%s]%d: pvcIdx = %d, j = %d\n", __FUNCTION__, __LINE__, pvcIdx, j);
			return (pvcIdx*PVC_NUM + j);
		}
	}

	return -1;
}

void setDefaultBridgeWanCfg(char *name)
{
	/*set default wan info*/
	cfg_obj_set_object_attr(name, "Active", 0, "Yes");
	cfg_obj_set_object_attr(name, "ServiceList", 0, "INTERNET");
	cfg_obj_set_object_attr(name, "BandActive", 0, "N/A");
	cfg_obj_set_object_attr(name, "WanMode", 0, "Bridge");
	cfg_obj_set_object_attr(name, "LinkMode", 0, "linkPPP");
	cfg_obj_set_object_attr(name, "BridgeMode", 0, "PPPoE_Bridged");
	cfg_obj_set_object_attr(name, "DHCPRealy", 0, "No");
	cfg_obj_set_object_attr(name, "IPVERSION", 0, "IPv4");
	cfg_obj_set_object_attr(name, "ISP", 0, "3");
	cfg_obj_set_object_attr(name, "VLANMode", 0, "UNTAG");
	cfg_obj_set_object_attr(name, "dot1q", 0, "No");
	cfg_obj_set_object_attr(name, "dot1p", 0, "No");
	cfg_obj_set_object_attr(name, "dot1pData", 0, "0");
	cfg_obj_set_object_attr(name, "MulticastVID", 0, "");
	cfg_obj_set_object_attr(name, "NATENABLE", 0, "Disabled");
	cfg_obj_set_object_attr(name, "IGMPproxy", 0, "No");
	cfg_obj_set_object_attr(name, "DsliteEnable", 0, "No");
	cfg_obj_set_object_attr(name, "PDEnable", 0, "No");
	cfg_obj_set_object_attr(name, "DHCPv6PD", 0, "No");
	cfg_obj_set_object_attr(name, "PDOrigin", 0, "None");
	cfg_obj_set_object_attr(name, "DHCPEnable", 0, "1");
	cfg_obj_set_object_attr(name, "BridgeInterface", 0, "No");
	cfg_obj_set_object_attr(name, "ConnectionError", 0, "ERROR_NO_ANSWER");
	return;
}


void commitWanSetWanMask(int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char buf[16] = {0};
	char wanMaskAttr[16] = {0};
	int wanMask = 0;

	if(wanIf < 0) 
	{		
		return;
	}

	if(wanIf >= MAX_WAN_IF_INDEX/2)
	{
		strncpy(wanMaskAttr, TR181_WAN_MASK1_ATTR, sizeof(wanMaskAttr) - 1);
		cfg_obj_get_object_attr(TR181_COMMON_NODE, wanMaskAttr, 0,buf,sizeof(buf));		
		wanMask = atoi(buf) ;
		wanMask |= (1 <<  (wanIf - MAX_WAN_IF_INDEX/2));
	}
	else
	{	
		strncpy(wanMaskAttr, TR181_WAN_MASK0_ATTR, sizeof(wanMaskAttr) - 1);
		cfg_obj_get_object_attr(TR181_COMMON_NODE, wanMaskAttr, 0,buf,sizeof(buf));
		wanMask = atoi(buf) ;
		wanMask |= (1 <<  wanIf);
	}

	memset (buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d", wanMask);
	cfg_obj_set_object_attr(TR181_COMMON_NODE, wanMaskAttr, 0,buf);	
	return;
}

/*______________________________________________________________________________
**	commitWanByWanIf
**
**	descriptions:
**		set the wan index that want to commit to the node of tr181_common
**	parameters:
**		wanIf:	pvcIndex ==> WanPVC*PVC_NUM + WanEntry
**_______________________________________________________________________________
*/
void commitWanByWanIf(int wanIf)
{
	char node[MAXLEN_NODE_NAME] = {0};
	int pvcIdx = -1;
	int EntIdx = -1;
	int wanIndex = -1;

	if(wanIf < 0){
		/*commit All Wan*/
		for(pvcIdx = 0; pvcIdx < PVC_NUM; pvcIdx++ ){
			for(EntIdx = 0; EntIdx < MAX_SMUX_NUM; EntIdx++ ){
				wanIndex = pvcIdx*PVC_NUM + EntIdx;
				commitWanSetWanMask(wanIndex);
			}
		}
	}else{
		commitWanSetWanMask(wanIf);
	}
	return;
}

/*______________________________________________________________________________
**	SyncWanByTR181IPIf
**
**	descriptions:
**		sync interface info to wan.
**	parameters:
**		node:	node name
**	return:
**		index:	TR181IP_Entry%d's index.
**_______________________________________________________________________________
*/
int getIpIFIdxByUplayer(char *nodeName)
{
	char uplayer[64] = {0};
	char *tmp = NULL;
	char up_nodeName[MAXLEN_NODE_NAME] = {0};
	int index = -1;

	memset(uplayer, 0, sizeof(uplayer));
	if((cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer)) >= 0)
		&& (strlen(uplayer) != 0))
	{
		tmp = uplayer + (strlen(uplayer) - 1);
		index = atoi(tmp);		
		if(index < 0){
			return -1;
		}
	}
	else
	{
		return -1;
	}

	if(strstr(uplayer, TR181_IP_INTERFACE) != NULL)
	{
		return index;	
	}
	else if(strstr(uplayer, TR181_PPP_INTERFACE) != NULL)
	{
		snprintf(up_nodeName, sizeof(up_nodeName), TR181_PPP_ENTRY_NODE, index);
	}
	else if(strstr(uplayer, TR181_ETHERNET_VLANTERMINATION) != NULL)
	{
		snprintf(up_nodeName, sizeof(up_nodeName), TR181_ETHERVLANT_ENTRY_NODE, index);
	}
	else if(strstr(uplayer, TR181_ETHERNET_LINK) != NULL)
	{
		snprintf(up_nodeName, sizeof(up_nodeName), TR181_ETHERLINK_ENTRY_NODE, index);
	}
#if defined(TCSUPPORT_CT_DSL_EX)
	else if(strstr(uplayer, TR181_ATMLINK_INTERFACE) != NULL)
	{
		snprintf(up_nodeName, sizeof(up_nodeName), TR181_ATMLINK_ENTRY_NODE, index);
	}
	else if(strstr(uplayer, TR181_PTMLINK_INTERFACE) != NULL)
	{
		snprintf(up_nodeName, sizeof(up_nodeName), TR181_PTMLINK_ENTRY_NODE, index);
	}
#endif
	
	index =getIpIFIdxByUplayer(up_nodeName);
	return index;	
}

/*______________________________________________________________________________
**	SyncWanByTR181IPIf
**
**	descriptions:
**		sync interface info to wan.
**	parameters:
**		ipIFIdx:	TR181IP_Entry%d's index.
**		wanIf:	pvcIndex ==> WanPVC*PVC_NUM + WanEntry
**_______________________________________________________________________________
*/
void SyncWanByTR181IPIf(int ipIFIdx, int wanIf)
{	
	/*IP.Interface, DNS, static IP sync*/
	tr181IPIFNodeSync2Wan(ipIFIdx, wanIf);

	/*nat sync*/                                            
	natSync2WanByTR181IPIf(ipIFIdx, wanIf);   	
	                                                         
	/*DHCPv4.Client client sync*/                                   	
	dhcp4cSync2WanByTR181IPIf(ipIFIdx, wanIf);	
	                                                         
	/*DHCPv6.Client  sync*/                                    
	dhcp6cSync2WanByTR181IPIf(ipIFIdx, wanIf);	

	/*Routing.Route.{i}.IPv4Forwarding*/ 								   
	routeFwd4Sync2WanByTR181IPIf(ipIFIdx, wanIf);

	/*Routing.Route.{i}.IPv6Forwarding*/ 								   
	routeFwd6Sync2WanByTR181IPIf(ipIFIdx, wanIf);
}



int setPVCIndexByLowerlayerStack(struct lowerlayer_stack_s lowerlayer_stack, int wanIf)
{
	if(wanIf < 0)
	{
		return 0;
	}
	
	if(lowerlayer_stack.ipIFIdx > 0)
	{
		setPvcIdxByTR181NodeEntry(TR181_IP_NODE, lowerlayer_stack.ipIFIdx, wanIf);
	}

	
	if(lowerlayer_stack.pppIFIdx > 0)
	{		
		setPvcIdxByTR181NodeEntry(TR181_PPP_NODE, lowerlayer_stack.pppIFIdx, wanIf);
	}

	if(lowerlayer_stack.vlanTIdx > 0)
	{
		setPvcIdxByTR181NodeEntry(TR181_ETHERVLANT_NODE, lowerlayer_stack.vlanTIdx, wanIf);
	}

	if(lowerlayer_stack.EtherLinkIdx > 0)
	{
		setPvcIdxByTR181NodeEntry(TR181_ETHERLINK_NODE, lowerlayer_stack.EtherLinkIdx, wanIf);
	}
		
	return 0;
}

int createBridgeWanByLowerLayerStack(struct lowerlayer_stack_s lowerlayer_stack)
{		
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	int ret = -1;
	int natEnable = 0;
	int wanIf = -1;
	char node[MAXLEN_NODE_NAME] = {0};

	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d] : ipIFIdx=%d!\n",__FUNCTION__,__LINE__,lowerlayer_stack.ipIFIdx);
	}
	
	if(lowerlayer_stack.ipIFIdx <= 0 )
	{
		tcdbg_printf("[%s:%d]IP.Interface not exist, return\n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*check wan connection*/
	if(lowerlayer_stack.EtherLinkIdx <= 0
		&& lowerlayer_stack.opticalIFIdx <= 0)
	{
		tcdbg_printf("[%s:%d]lowerlayer is not all set yet, not wan connection \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*create route wan:get empty wan pvc index*/
	wanIf = findEmptyUntagWanPVCIdx();
	
	if(wanIf < 0)
	{
		tcdbg_printf("\r\nNo free pvc for use");
		return -1;
	}	

	/*set pvcIndex*/
	setPVCIndexByLowerlayerStack(lowerlayer_stack, wanIf);	

	/*sync Wan_PVC info*/
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	setDefaultBridgeWan(wanIf);
	
	/*check Enable*/
	ret = checkIPIFEnableByIdx(lowerlayer_stack.ipIFIdx - 1);
	
	if(updateInterfaceNodeDbg())
	{		
		tcdbg_printf("[%s:%d]checkIPIFEnableByIdx ret =%d, ipIFIdx=%d\n",
			__FUNCTION__,__LINE__,ret , lowerlayer_stack.ipIFIdx);
	}

	if(ret == 1) 
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
	}
	else
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
	}

	if(lowerlayer_stack.ipIFIdx > 0)
	{
		SyncWanByTR181IPIf(lowerlayer_stack.ipIFIdx - 1, wanIf);
	}
	
	/*ppp sync*/
	if(lowerlayer_stack.pppIFIdx > 0)
	{
		tcdbg_printf("[%s:%d]PPP.Interface exist, is ppp connection \n",__FUNCTION__,__LINE__);		
		cfg_obj_set_object_attr(wanNode, "ISP", 0, "2");
		tr181PPPNodeSync2Wan(lowerlayer_stack.pppIFIdx - 1, wanIf);
	}

	/*vlan sync*/
	if(lowerlayer_stack.vlanTIdx > 0)
	{
		tr181EtherVlanTSync2Wan(lowerlayer_stack.vlanTIdx -1, wanIf);
	}

	cfg_tr181_commit_object(wanNode);
	
	return wanIf;
}

int createBridgeWanByIPIf(int ipIFIdx)
{	
	struct lowerlayer_stack_s lowerlayer_stack;
	int ret = -1;
	int pvcIdx = -1;

	memset(&lowerlayer_stack, 0, sizeof(lowerlayer_stack));
	lowerlayer_stack.ipIFIdx = ipIFIdx;
	ret = getLowerLayerStackByIpIf(&lowerlayer_stack);
	if(ret < 0){
		/*lowerlayer set is not enough, can not create wan*/			
		tcdbg_printf("[%s:%d]lowerlayer set is not enough, can not create wan,i=%d\n",__FUNCTION__,__LINE__,ipIFIdx);
		return -1;
	}
	
	if(updateInterfaceNodeDbg()){		
		dunmpLowerLayerStack(lowerlayer_stack);
	}
	
	/*create wan connect:*/
	pvcIdx = createBridgeWanByLowerLayerStack(lowerlayer_stack);			

	return pvcIdx;
}

int getPvcIdxOfLowerLayer(char* node, int inIdx)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char lowerlayer[256] = {0};
	char pvcIdx[8] = {0};
	int idx = 0;
	int ret = -1;
	int lowerPvc = -1;

	/*check lowerlayer*/	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, inIdx);
	
	if((cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0)
	{
		/*lowerlayer is not exist, wan connect info is not enough*/
		tcdbg_printf("[%s:%d]lowerlayer not exist\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{
		/*lowerlayer is exist, check next lowerlayer*/
		if(strstr(lowerlayer, TR181_PPP_INTERFACE) != NULL)
		{
			/*the next layer is ppp: for ppp wan*/	
			idx = getSuffixIntByInterface(lowerlayer);
			
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, idx);
			if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx, sizeof(pvcIdx)) >= 0)
				&& isNumber(pvcIdx))
			{
				lowerPvc = atoi(pvcIdx);
			}	
		}
		else if(strstr(lowerlayer, TR181_ETHERNET_VLANTERMINATION) != NULL)
		{
			/*the next layer is VLANTermination: for wan with vlan*/	
			idx = getSuffixIntByInterface(lowerlayer);
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, idx);
			
			if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx, sizeof(pvcIdx)) >= 0)
				&& isNumber(pvcIdx))
			{
				lowerPvc = atoi(pvcIdx);
			}
		}
		else if(strstr(lowerlayer, TR181_ETHERNET_LINK) != NULL)
		{
			/*the next layer is EtherLink*/		
			idx = getSuffixIntByInterface(lowerlayer);
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, idx);
			if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx, sizeof(pvcIdx)) >= 0)
				&& isNumber(pvcIdx))
			{
				lowerPvc = atoi(pvcIdx);
			}
		}
#if defined(TCSUPPORT_CT_DSL_EX)
		else if(strstr(lowerlayer, TR181_ATMLINK_INTERFACE) != NULL)
		{
			/*the next layer is EtherLink*/		
			idx = getSuffixIntByInterface(lowerlayer);
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, idx);
			if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx, sizeof(pvcIdx)) >= 0)
				&& isNumber(pvcIdx))
			{
				lowerPvc = atoi(pvcIdx);
			}
		}
		else if(strstr(lowerlayer, TR181_PTMLINK_INTERFACE) != NULL)
		{
			/*the next layer is EtherLink*/		
			idx = getSuffixIntByInterface(lowerlayer);
			memset(nodeName, 0, sizeof(nodeName));	
			snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, idx);
			if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, pvcIdx, sizeof(pvcIdx)) >= 0)
				&& isNumber(pvcIdx))
			{
				lowerPvc = atoi(pvcIdx);
			}
		}
#endif
	}
	
	return lowerPvc;
}

int setInterfaceByTR181NodeEntry(char* node, int index, char* interface)
{ 
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[8] = {0};

	if(index < 0)
	{
		tcdbg_printf("[%s:%d]index = %d, error\n",__FUNCTION__,__LINE__, index);
		return -1;
	}
	
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, index);

	cfg_obj_set_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, interface);
	
	return 0;
}

/*______________________________________________________________________________
**	clearOtherIneterfaceByWanIf
**
**	descriptions:
**		clear PvcIndex and Interface by Entry Index
**
**	parameters:
**		node:	node name need to check.
**		num:	node num.
**		index:	
**				index>0: check all Entry node except this Entry	
**				-1: all Entry node need to check
**		num:	node num.
**		wanIf:    check node by PvcIndex
**_______________________________________________________________________________
*/
int clearOtherIneterfaceByWanIf(char* node, int num, int index, int wanIf)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmpwanIf[16] = {0};
	int i = 0;
	int tmp1 = -1;

	for(i  = 1; i <= num; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.entry.%d", node, i);

		memset(tmpwanIf, 0, sizeof(tmpwanIf));
		if(cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmpwanIf, sizeof(tmpwanIf)) >= 0)
		{
			tmp1 = atoi(tmpwanIf);
			if(wanIf == tmp1 && index != i)
			{
				/*clear old dhcpveclient entry*/
				setPvcIdxByTR181NodeEntry(node, i, -1);
				setInterfaceByTR181NodeEntry(node, i, "");
			}
		}
	}

	return 0;
}

/*______________________________________________________________________________
**	SetNextOldUpLayerByWanIf
**
**	descriptions:
**		set uplayer 
**	parameters:
**		interface:	lowerlayer.
**		uplayer:	uplayer.
**_______________________________________________________________________________
*/
void SetNextUpLayerByLowerLayer(char* interface, char* uplayer)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int nextEntryIdx = -1;

	nextEntryIdx = getSuffixIntByInterface(interface);
		
	if(strstr(interface, TR181_PPP_INTERFACE) != NULL)
	{
		snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, nextEntryIdx);
		cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer);
	}
	else if(strstr(interface, TR181_ETHERNET_VLANTERMINATION) != NULL)
	{
		snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, nextEntryIdx);
		cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer);
	}
	else if(strstr(interface, TR181_ETHERNET_LINK) != NULL)
	{
		snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, nextEntryIdx);
		cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer);
	}
	return;
}

/*______________________________________________________________________________
**	SetNextOldUpLayerByWanIf
**
**	descriptions:
**		set uplayer to ""
**	parameters:
**		Node:	nodename.
**		num:	the max entry number of nodename.
**		uplayer:	uplayer.
**_______________________________________________________________________________
*/
int SetNextOldUpLayerByWanIf(char* Node, int  num, char* uplayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	int i = 0;
	
	for(i = 1; i <= num; i++)
	{
		snprintf(nodeName, sizeof(nodeName), Node, i);
		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, tmp, sizeof(tmp)) >= 0)
			&& strcmp(uplayer, tmp) == 0)
		{
			cfg_obj_set_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, "");
			return 0;
		}
	}
	return -1;
}

/*______________________________________________________________________________
**	ClearOldUpLayer
**
**	descriptions:
**		clear all uplayer by interface.
**	parameters:
**		interface:		uplayer;
**_______________________________________________________________________________
*/
void ClearOldUpLayer(char* interface)
{	
	if(strstr(interface, TR181_IP_INTERFACE) != NULL)
	{
		SetNextOldUpLayerByWanIf(TR181_PPP_ENTRY_NODE, PPP_INTERFACE_NUM, interface);
		SetNextOldUpLayerByWanIf(TR181_ETHERVLANT_ENTRY_NODE, ETHER_VLAN_T_NUM, interface);
		SetNextOldUpLayerByWanIf(TR181_ETHERLINK_ENTRY_NODE, ETHER_LINK_NUM, interface);
	}
	else if(strstr(interface, TR181_PPP_INTERFACE) != NULL)
	{
		SetNextOldUpLayerByWanIf(TR181_ETHERVLANT_ENTRY_NODE, ETHER_VLAN_T_NUM, interface);
		SetNextOldUpLayerByWanIf(TR181_ETHERLINK_ENTRY_NODE, ETHER_LINK_NUM, interface);
	}
	else if(strstr(interface, TR181_ETHERNET_VLANTERMINATION) != NULL)
	{
		SetNextOldUpLayerByWanIf(TR181_ETHERLINK_ENTRY_NODE, ETHER_LINK_NUM, interface);
	}
	else
	{
		/*do nothing*/
	}
	return;
}

/*______________________________________________________________________________
**	setPVCIndexByUpLayer
**
**	descriptions:
**		sync tr181 node pvcIndex.
**	parameters:
**		interface:		uplayer;
**		newWanIf:	pvcIndex ==> WanPVC*PVC_NUM + WanEntry
**_______________________________________________________________________________
*/
void setPVCIndexByUpLayer(char *interface, int newWanIf)
{
	int index = -1;	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char uplayer[256] = {0};
	
	if(strstr(interface, TR181_IP_INTERFACE) != NULL)
	{
		index = getSuffixIntByInterface(interface);
		setPvcIdxByTR181NodeEntry(TR181_IP_NODE, index, newWanIf);
		return;
	}	
	else if(strstr(interface, TR181_PPP_INTERFACE) != NULL)
	{
		index = getSuffixIntByInterface(interface);		
		setPvcIdxByTR181NodeEntry(TR181_PPP_NODE, index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		setPVCIndexByUpLayer(uplayer, newWanIf);
	}
	else if(strstr(interface, TR181_ETHERNET_VLANTERMINATION) != NULL)
	{		
		index = getSuffixIntByInterface(interface);
		setPvcIdxByTR181NodeEntry(TR181_ETHERVLANT_NODE, index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		setPVCIndexByUpLayer(uplayer, newWanIf);
	}
	else if(strstr(interface, TR181_ETHERNET_LINK) != NULL)
	{
		index = getSuffixIntByInterface(interface);
		setPvcIdxByTR181NodeEntry(TR181_ETHERLINK_NODE, index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		setPVCIndexByUpLayer(uplayer, newWanIf);
	}
#if defined(TCSUPPORT_CT_DSL_EX)
	else if(strstr(interface, TR181_ATMLINK_INTERFACE) != NULL)
	{
		index = getSuffixIntByInterface(interface);
		setPvcIdxByTR181NodeEntry(TR181_ATMLINK_NODE, index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_ATMLINK_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		setPVCIndexByUpLayer(uplayer, newWanIf);
	}
	else if(strstr(interface, TR181_PTMLINK_INTERFACE) != NULL)
	{
		index = getSuffixIntByInterface(interface);
		setPvcIdxByTR181NodeEntry(TR181_PTMLINK_NODE, index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_PTMLINK_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		setPVCIndexByUpLayer(uplayer, newWanIf);
	}
#endif
	else
	{
		return;
	}
}

/*______________________________________________________________________________
**	sync2WanByUpLayer
**
**	descriptions:
**		sync interface info to wan.
**	parameters:
**		interface:		uplayer;
**		newWanIf:	pvcIndex ==> WanPVC*PVC_NUM + WanEntry
**_______________________________________________________________________________
*/
void sync2WanByUpLayer(char *interface, int newWanIf)
{
	int index = -1;	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char uplayer[256] = {0};
	
	if(strstr(interface, TR181_IP_INTERFACE) != NULL)
	{
		index = getSuffixIntByInterface(interface);
		SyncWanByTR181IPIf(index, newWanIf);
		return;
	}	
	else if(strstr(interface, TR181_PPP_INTERFACE) != NULL)
	{
		index = getSuffixIntByInterface(interface);		
		tr181PPPNodeSync2Wan(index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		sync2WanByUpLayer(uplayer, newWanIf);
	}
	else if(strstr(interface, TR181_ETHERNET_VLANTERMINATION) != NULL)
	{		
		index = getSuffixIntByInterface(interface);
		tr181EtherVlanTSync2Wan(index, newWanIf);
		
		memset(nodeName, 0, sizeof(nodeName));		
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, index);
		cfg_obj_get_object_attr(nodeName, TR181_UPLAYER_ATTR, 0, uplayer, sizeof(uplayer));
		sync2WanByUpLayer(uplayer, newWanIf);
	}
	else
	{
		return;
	}
}


void syncWan2TR181V6Info(int wanIf,int ipIndex) 
{
	char tmp[32] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	int ret = -1;

	if(wanIf < 0 || ipIndex < 0)
	{
		tcdbg_printf("[%s:%d]wanIf=%d < 0 ipIndex=%d < 0 error!\n",__FUNCTION__,__LINE__,wanIf,ipIndex);
		return -1;
	}

	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, "IPVERSION", 0, tmp, sizeof(tmp));
	if(strcmp(tmp, "IPv4") == 0){
		/*ipv4 only, no need to sync v6 info*/
		return -1;
	}
	
	SnycIPIFv6AddressEntry(wanIf,ipIndex);
	syncWan2Dslite(wanIf,ipIndex);

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, tmp, sizeof(tmp));
	if(!strcmp(tmp, "Yes")){
		/*DHCPv6 Enable*/
		syncWan2DHCPv6Client(wanIf, ipIndex ,1);
		syncWanSlaac2routeAdvIfSet(wanIf, ipIndex ,0);
			
		}else if(!strcmp(tmp, "No")){
			/*SLAAC*/	
			syncWan2DHCPv6Client(wanIf, ipIndex ,0);
			syncWanSlaac2routeAdvIfSet(wanIf, ipIndex ,1);
		
		}else{ 
			/*Static*/	
			syncWan2DHCPv6Client(wanIf, ipIndex ,0);
			syncWanSlaac2routeAdvIfSet(wanIf, ipIndex ,0);						
		}
	/*PD, IP*/	
	updateIPv6PreFix(wanIf,ipIndex);
	
}

#if defined(TCSUPPORT_CT_DSL_EX)
int checkIPEnable(int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char enable[4] = {0};
	int ret = -1;
	int nextEntryIdx = -1;
	int i, entryIdx = -1;
	char pvcIdx[4] = {0};

	if(wanIf < 0)
	{
		return -1;
	}
	/*get TR181IP_Entry%d*/
	for(i = 1; i <= IP_INTERFACE_NUM; i++)
	{
		memset(pvcIdx, 0, sizeof(pvcIdx));
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, i);
		
		cfg_obj_get_object_attr(nodeName, "pvcIndex", 0, pvcIdx, sizeof(pvcIdx));
		if(atoi(pvcIdx) == wanIf )
		{
			entryIdx = i;
			break;
		}
	}
	
	if( entryIdx < 0 )
	{
		return -1;
	}

	ret = checkIPIFEnableByIdx(entryIdx);
	
	return ret;
}

int setWanConnectionActive(char *Enable)
{
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char wanExist[8] = {0};
	int wanIf = 0;
	int ret = 0;
	
	/*dsl   enable*/
	if(!strcmp(Enable, "1"))
	{
		for(wanIf = 0; wanIf < WAN_INTERFACE_NUM; wanIf++)
		{
			memset(wanNode, 0, sizeof(wanNode));
			ret = setWanNodeByWanIf(wanNode, wanIf);
			if(ret < 0)
			{		
				tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
				return -1;
			}

			if(cfg_obj_get_object_attr(wanNode, "Active", 0, wanExist, sizeof(wanExist)) < 0) 
			{
				continue;
			}
			else 
			{
				ret = checkIPEnable(wanIf);
				if(ret == 1) 
				{
					cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
				}
				else if(ret == 0)
				{
					cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
				}
			}
		}
	}

	/*dsl  disable*/
	else if(!strcmp(Enable, "0") || !strcmp(Enable, ""))
	{	
		for(wanIf = 0; wanIf < WAN_INTERFACE_NUM; wanIf++)
		{
			memset(wanNode, 0, sizeof(wanNode));
			ret = setWanNodeByWanIf(wanNode, wanIf);
			if(ret < 0)
			{		
				tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
				return -1;
			}

			if(cfg_obj_get_object_attr(wanNode, "Active", 0, wanExist, sizeof(wanExist)) < 0) 
			{
				continue;
			}
			cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
		}
	}

	return 0;
}
#endif

