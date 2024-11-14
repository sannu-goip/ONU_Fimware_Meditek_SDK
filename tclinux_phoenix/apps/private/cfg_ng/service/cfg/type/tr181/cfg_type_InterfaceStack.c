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

int setWifiSSIDIFStackNode(int interfaceStackIdx, int index){
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char tmp[256] = {0};	

	if( index < 0 ){
		tcdbg_printf("[%s:%d] interfaceStackIdx=%d,index=%d, error!\n",__FUNCTION__,__LINE__,interfaceStackIdx,index);
		return -1;
	}


	/*check lowerlayer*/
	memset(nodeName,0,sizeof(nodeName));	
	memset(lowerlayer,0,sizeof(lowerlayer));
	snprintf(nodeName, sizeof(nodeName), TR181_WIFISSID_ENTRY_NODE, index + 1);
	if(cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0
		|| strlen(lowerlayer) == 0 )
	{
		/*is not exist, return pre interfaceStackIdx*/
		return interfaceStackIdx;
	}	

	/*set interfaceStack Entry*/
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, interfaceStackIdx + 1);
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}
	
	memset(tmp,0,sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), TR181_WIFI_SSID_IF"%d", index+1);
	cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
	cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);

	/*next interfaceStack Entry i wifi radio, no more*/
	return interfaceStackIdx+1;
}

int setBridgePortIFStackNode(int interfaceStackIdx, int index)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};	
	char attrNode[32] = {0};
	char tmp[256] = {0};	
	int entryIdx = 0;	
	int idx = 0;
	int i = 0;

	if( index < 0 )
	{
		tcdbg_printf("[%s:%d] interfaceStackIdx=%d,index=%d, error!\n",__FUNCTION__,__LINE__,interfaceStackIdx,index);
		return -1;
	}

	/*check lowerlayer*/
	memset(nodeName,0,sizeof(nodeName));	
	memset(lowerlayer,0,sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, index + 1);

	if( index != 0 )
	{
		/*not br0*/
		if(cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0
			|| strlen(lowerlayer) == 0 )
		{
			/*is not exist, return pre interfaceStackIdx*/
			return interfaceStackIdx;
		}	
		
		/*set interfaceStack Entry*/
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, interfaceStackIdx + 1);
		
		if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
		{
			cfg_obj_create_object(nodeName);
		}
		
		memset(tmp,0,sizeof(tmp));	
		snprintf(tmp, sizeof(tmp), TR181_BRIDGE_PORT"%d", index+1);
		cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
		cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);

		/*set next interfaceStack Entry*/	
		entryIdx = interfaceStackIdx+1;
		if(strstr(lowerlayer,TR181_BRIDGE_PORT) != NULL)
		{
			/*for bridge Port*/
			sscanf(lowerlayer+strlen(TR181_BRIDGE_PORT), "%d", &idx);
			entryIdx = setBridgePortIFStackNode(entryIdx,idx-1);
		}
		else if(strstr(lowerlayer,TR181_WIFI_SSID_IF) != NULL)
		{		
			/*for wifi SSID Port*/
			sscanf(lowerlayer+strlen(TR181_WIFI_SSID_IF), "%d", &idx);
			entryIdx = setWifiSSIDIFStackNode(entryIdx,idx-1);
		}
		else if(strstr(lowerlayer,TR181_ETHER_INTERFACE) != NULL)
		{ 	
			/*TR181_ETHER_INTERFACE,no next lowerlayer*/
			tcdbg_printf("[%s:%d]TR181_ETHER_INTERFACE,no next lowerlayer, interfaceStackIdx=%d\n",__FUNCTION__,__LINE__,interfaceStackIdx);
		}

		return entryIdx;		
	}

	
	entryIdx = interfaceStackIdx;
	for(i = 0; i < BRIDGE_PORT_NUM; i++)
	{	
		memset(nodeName,0,sizeof(nodeName));	
		memset(lowerlayer,0,sizeof(lowerlayer));	
		snprintf(nodeName, sizeof(nodeName), TR181_BRIDGEPORT_ENTRY_NODE, index + 1);
		
		memset(attrNode,0,sizeof(attrNode));
		snprintf(attrNode, sizeof(attrNode), "LowerLayers%d", i);
		if(cfg_obj_get_object_attr(nodeName, attrNode, 0, lowerlayer, sizeof(lowerlayer)) < 0
			|| strlen(lowerlayer) == 0 )
		{
			/*is not exist, find next lowerlayer*/
			continue;
		}	
		
		/*set interfaceStack Entry*/
		memset(nodeName,0,sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, entryIdx + 1);
		if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
		{
			cfg_obj_create_object(nodeName);
		}
		
		
		memset(tmp,0,sizeof(tmp));	
		snprintf(tmp, sizeof(tmp), TR181_BRIDGE_PORT"%d", index + 1);
		cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
		cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);
	
		/*set next interfaceStack Entry*/
		entryIdx++;
		if(strstr(lowerlayer,TR181_BRIDGE_PORT) != NULL)
		{
			/*for bridge Port*/
			sscanf(lowerlayer+strlen(TR181_BRIDGE_PORT), "%d", &idx);
			entryIdx = setBridgePortIFStackNode(entryIdx, idx - 1);
		}
		else if(strstr(lowerlayer,TR181_WIFI_SSID_IF) != NULL)
		{		
			/*for wifi SSID Port*/
			sscanf(lowerlayer+strlen(TR181_WIFI_SSID_IF), "%d", &idx);
			entryIdx = setWifiSSIDIFStackNode(entryIdx,idx-1);
		}
		else if(strstr(lowerlayer,TR181_ETHER_INTERFACE) != NULL)
		{		
			/*TR181_ETHER_INTERFACE,no next lowerlayer*/
			tcdbg_printf("[%s:%d]TR181_ETHER_INTERFACE,no next lowerlayer, interfaceStackIdx=%d\n",__FUNCTION__,__LINE__,interfaceStackIdx);
		}
	}

	return entryIdx;
}

int setEtherLinkInterfaceStackNode(int interfaceStackIdx, int index)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char tmp[256] = {0};	
	int entryIdx = 0;	
	int idx = 0;

	if( index < 0 )
	{
		tcdbg_printf("[%s:%d] interfaceStackIdx=%d,index=%d, error!\n",__FUNCTION__,__LINE__,interfaceStackIdx,index);
		return -1;
	}

	/*check lowerlayer*/
	memset(nodeName,0,sizeof(nodeName));	
	memset(lowerlayer,0,sizeof(lowerlayer));
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERLINK_ENTRY_NODE, index + 1);
	if(cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0
		|| strlen(lowerlayer) == 0 )
	{
		/*is not exist, return pre interfaceStackIdx*/
		return interfaceStackIdx;
	}	

	/*set interfaceStack Entry*/
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, interfaceStackIdx + 1);
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}
	
	memset(tmp,0,sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), TR181_ETHERNET_LINK"%d", index+1);
	cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
	cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);

	/*set next interfaceStack Entry*/
	entryIdx = interfaceStackIdx + 1;
	if(strstr(lowerlayer,TR181_BRIDGE_PORT) != NULL)
	{
		/*bridge port,for br0 or lan/wlan*/		
		sscanf(lowerlayer+strlen(TR181_BRIDGE_PORT), "%d", &idx);
		entryIdx = setBridgePortIFStackNode(entryIdx, idx - 1);
	}
	else if(strstr(lowerlayer,TR181_OPTICAL_INTERFACE) != NULL)
	{
		/*OPTICAL,no next lowerlayer*/
		tcdbg_printf("[%s:%d]TR181_OPTICAL_INTERFACE,no next lowerlayer, interfaceStackIdx=%d\n",__FUNCTION__,__LINE__,interfaceStackIdx);
	}
	
	return entryIdx;
}

int setEtherVTInterfaceStackNode(int interfaceStackIdx, int index)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};	
	char attrNode[32] = {0};
	char tmp[256] = {0};	
	int entryIdx = 0;	
	int idx = 0;
	int i = 0;

	if( index < 0 )
	{
		tcdbg_printf("[%s:%d] interfaceStackIdx=%d,index=%d, error!\n",__FUNCTION__,__LINE__,interfaceStackIdx,index);
		return -1;
	}

	/*check lowerlayer*/
	entryIdx = interfaceStackIdx;

	memset(nodeName,0,sizeof(nodeName));	
	memset(lowerlayer,0,sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_ETHERVLANT_ENTRY_NODE, index + 1);

	memset(attrNode,0,sizeof(attrNode));
	snprintf(attrNode, sizeof(attrNode), "LowerLayers");
	
	if(cfg_obj_get_object_attr(nodeName, attrNode, 0, lowerlayer, sizeof(lowerlayer)) < 0
		|| strlen(lowerlayer) == 0 )
	{		
		/*is not exist, find next lowerlayer*/
		return interfaceStackIdx;
	}	
	
	/*set interfaceStack Entry*/
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, entryIdx + 1);
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}
	
	memset(tmp,0,sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), TR181_ETHERNET_VLANTERMINATION"%d", index+1);
	cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
	cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);

	/*set next interfaceStack Entry*/
	entryIdx++;
	if(strstr(lowerlayer,TR181_ETHERNET_LINK) != NULL)
	{
		/*the next layer is Link: for wan without vlan*/
		sscanf(lowerlayer+strlen(TR181_ETHERNET_LINK), "%d", &idx);
		entryIdx = setEtherLinkInterfaceStackNode(entryIdx, idx - 1);
	}

	return entryIdx;
}

int setPPPInterfaceStackNode(int interfaceStackIdx, int index)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char tmp[256] = {0};	
	int entryIdx = 0;	
	int idx = 0;

	if( index < 0 )
	{
		tcdbg_printf("[%s:%d] interfaceStackIdx=%d,index=%d, error!\n",__FUNCTION__,__LINE__,interfaceStackIdx,index);
		return -1;
	}

	/*check lowerlayer*/
	memset(nodeName,0,sizeof(nodeName));	
	memset(lowerlayer,0,sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_PPP_ENTRY_NODE, index + 1);
	if(cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0 
		|| strlen(lowerlayer) == 0 )
	{
		/*is not exist, return pre interfaceStackIdx*/
		return interfaceStackIdx;
	}	

	/*set interfaceStack Entry*/
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, interfaceStackIdx + 1);
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}
	
	memset(tmp,0,sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), TR181_PPP_INTERFACE"%d", index + 1);
	cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
	cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);


	/*set next interfaceStack Entry*/	
	entryIdx = interfaceStackIdx + 1;
	if(strstr(lowerlayer,TR181_ETHERNET_VLANTERMINATION) != NULL)
	{
		/*the next layer is VLANTermination: for wan with vlan*/				
		sscanf(lowerlayer+strlen(TR181_ETHERNET_VLANTERMINATION), "%d", &idx);
		entryIdx = setEtherVTInterfaceStackNode(entryIdx,idx-1);

	}
	else if(strstr(lowerlayer,TR181_ETHERNET_LINK) != NULL)
	{
		/*the next layer is Link: for wan without vlan*/		
		sscanf(lowerlayer+strlen(TR181_ETHERNET_LINK), "%d", &idx);
		entryIdx = setEtherLinkInterfaceStackNode(entryIdx, idx - 1);
	}

	return entryIdx;
}

int setIPInterfaceStackNode(int interfaceStackIdx, int index)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char tmp[256] = {0};		
	int entryIdx = 0;	
	int idx = 0;

	if( index < 0 )
	{
		tcdbg_printf("[%s:%d] interfaceStackIdx=%d,index=%d, error!\n",__FUNCTION__,__LINE__,interfaceStackIdx,index);
		return -1;
	}

	/*check lowerlayer*/
	memset(nodeName,0,sizeof(nodeName));	
	memset(lowerlayer,0,sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, index + 1);
	
	if(cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0 
		|| strlen(lowerlayer) == 0 )
	{
		/*is not exist, return pre interfaceStackIdx*/
		return interfaceStackIdx;
	}	

	/*set interfaceStack Entry*/
	memset(nodeName,0,sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_INTERFACE_STACK_ENTRY_NODE, interfaceStackIdx + 1);
	if (cfg_obj_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}

	memset(tmp,0,sizeof(tmp));	
	snprintf(tmp, sizeof(tmp), TR181_IP_INTERFACE"%d", index + 1);
	cfg_obj_set_object_attr(nodeName, "high", 0, tmp);
	cfg_obj_set_object_attr(nodeName, "lower", 0, lowerlayer);

	/*set next interfaceStack Entry*/	
	entryIdx = interfaceStackIdx + 1;
	if(strstr(lowerlayer,TR181_PPP_INTERFACE) != NULL)
	{
		/*the next layer is ppp: for ppp wan*/	
		sscanf(lowerlayer+strlen(TR181_PPP_INTERFACE), "%d", &idx);
		entryIdx = setPPPInterfaceStackNode(entryIdx, idx - 1);
			
	}
	else if(strstr(lowerlayer,TR181_ETHERNET_VLANTERMINATION) != NULL)
	{
		/*the next layer is VLANTermination: for wan with vlan*/	
		sscanf(lowerlayer+strlen(TR181_ETHERNET_VLANTERMINATION), "%d", &idx);
		entryIdx = setEtherVTInterfaceStackNode(entryIdx, idx - 1);

	}
	else if(strstr(lowerlayer,TR181_ETHERNET_LINK) != NULL)
	{
		/*the next layer is Link: for wan without vlan*/		
		sscanf(lowerlayer+strlen(TR181_ETHERNET_LINK), "%d", &idx);
		entryIdx = setEtherLinkInterfaceStackNode(entryIdx, idx - 1);
	}
	
	return entryIdx;	
}

static cfg_node_ops_t cfg_type_interfacestack_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_interfacestack_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | INTERFACE_STACK_NUM, 
	 .parent = &cfg_type_interfacestack, 
	 .ops = &cfg_type_interfacestack_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_interfacestack_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_interfacestack_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_interfacestack, 
	 .ops = &cfg_type_interfacestack_common_ops, 
}; 

static cfg_node_ops_t cfg_type_interfacestack_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_interfacestack_child[] = { 
	 &cfg_type_interfacestack_entry, 
	 &cfg_type_interfacestack_common,
	 NULL 
}; 


cfg_node_type_t cfg_type_interfacestack = { 
	 .name = "InterfaceStack", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_interfacestack_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_interfacestack_child, 
	 .ops = &cfg_type_interfacestack_ops, 
}; 
