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

static char* cfg_type_natportmap_index[] = { 
	 "PortMap_Idx", 
	 NULL 
}; 

void updateAllPortMapNode(void)
{
	int entry1 = 0;
	int entry2 = 0;
	char nodeName[MAXLEN_NODE_NAME] = {0};
#if defined(TCSUPPORT_CT_JOYME2)
	for(entry1 = 1; entry1 <= MAX_VIRSERV_RULE; entry1++)
	{
		updateNatPortMap(entry1);
	}
#else
	for(entry1 = 1; entry1 <= MAX_WAN_IF_INDEX; entry1++)
	{
		for(entry2 = 1; entry2 <= MAX_VIRSERV_RULE; entry2++)
		{
			snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_ENTRY_NODE, entry1, entry2);
			if (cfg_query_object(nodeName, NULL, NULL) >= 0)
			{
				updatePortMapNodeByIdx(entry1, entry2);
			}
		}
	}
#endif
}


int cfg_type_natportmap_entry_func_delete(char* path)
{
	int etyIdx = 0;
	int pvcIdx = 0;
	char pvcStr[8] = {0};
	char etyIdxStr[8] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	
#if defined(TCSUPPORT_CT_JOYME2)
	sscanf(path, TR181_NATPORTMAPPING_ENTRY_NODE, &etyIdx);
	if(etyIdx > 0)
	{
		snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, etyIdx);
		cfg_tr181_delete_object(nodeName);
	}
#else	
	cfg_obj_get_object_attr(path, PVC_INDEX_ATTR, 0, pvcStr, sizeof(pvcStr));
	pvcIdx = atoi(pvcStr);
	cfg_obj_get_object_attr(path, TR181_VSENTRYIDX_ATTR, 0, etyIdxStr, sizeof(etyIdxStr));
	etyIdx = atoi(etyIdxStr);
	if( pvcIdx >= 0 && etyIdx >= 0)
	{
		snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_ENTRY_NODE, pvcIdx+1,etyIdx+1);
		cfg_tr181_delete_object(nodeName);
	}
#endif
	return cfg_type_default_func_delete(path);
}


#if defined(TCSUPPORT_CT_JOYME2)
void deleteNatPortMap(int etyIdx)
{
	char portMappingNode[MAXLEN_NODE_NAME] = {0};	
	char tmp[8] = {0};
	int num = 0;

	snprintf(portMappingNode, sizeof(portMappingNode), TR181_NATPORTMAPPING_ENTRY_NODE, etyIdx);
	if (cfg_query_object(portMappingNode,NULL,NULL) >= 0)
	{
		cfg_obj_delete_object(portMappingNode);
		memset(tmp, 0, sizeof(tmp));		
		cfg_obj_get_object_attr(TR181_NATPORTMAPPING_COMMON_NODE, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
		num = atoi(tmp);	
		memset(tmp,0,sizeof(tmp));		
		snprintf(tmp, sizeof(tmp), "%d", num -1);
		cfg_obj_set_object_attr(TR181_NATPORTMAPPING_COMMON_NODE, TR181_NUM_ATTR, 0, tmp);
	}		
		
	return;
}


int updateNatPortMap(int etyIdx)
{
	char portMappingNode[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	int num = 0;
	char numStr[8] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	memset(portMappingNode, 0, sizeof(portMappingNode));
	snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_NODE, etyIdx);

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(nodeName, "MapName", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	
	snprintf(portMappingNode, sizeof(portMappingNode), TR181_NATPORTMAPPING_ENTRY_NODE, etyIdx);
	if (cfg_query_object(portMappingNode,NULL,NULL) < 0)
	{
		cfg_obj_create_object(portMappingNode);
		memset(numStr, 0, sizeof(numStr));		
		cfg_obj_get_object_attr(TR181_NATPORTMAPPING_COMMON_NODE, TR181_NUM_ATTR, 0, numStr, sizeof(numStr));
		num = atoi(numStr);	
		memset(numStr,0,sizeof(numStr));		
		snprintf(numStr, sizeof(numStr), "%d", num + 1);
		cfg_obj_set_object_attr(TR181_NATPORTMAPPING_COMMON_NODE, TR181_NUM_ATTR, 0, numStr);
	}
	

	cfg_obj_set_object_attr(portMappingNode, TR181_DESCRIP_ATTR, 0, tmp);

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_LOCALIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{	
		cfg_obj_set_object_attr(portMappingNode, TR181_LOCALIP_ATTR, 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_STARTPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{		
		cfg_obj_set_object_attr(portMappingNode, TR181_STARTPORT_ATTR, 0, tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_ENDPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_ENDPORT_ATTR, 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_ACTIVE_ATTR, 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, "SrcIP", 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_REMOTEIP_ATTR, 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_INTPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_INTPORT_ATTR, 0, tmp);
	}
	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, "Protocal", 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_PROTOCOL_ATTR, 0, tmp);
	} 

	return 0;	
}


int syncVirServer1(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char portMapNode[MAXLEN_NODE_NAME] = {0};	
	char vsNode[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
 	int pmEntryIdx = -1 ; 

	/*get pmEntryIdx*/
	if ( get_entry_number_cfg2(path, "entry.", &pmEntryIdx) != SUCCESS ) 
	{
		memset(nodeName,0,sizeof(nodeName));
		strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName) - 1);
		
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "PortMap_Idx", 0, tmp, sizeof(tmp)) >= 0)
		{
			pmEntryIdx = atoi(tmp) + 1;
		}
		else
		{
			return -1;
		}
	}
	
	memset(vsNode, 0, sizeof(vsNode));
	memset(portMapNode, 0, sizeof(portMapNode));
	snprintf(portMapNode, sizeof(portMapNode), TR181_NATPORTMAPPING_ENTRY_NODE, pmEntryIdx);
	snprintf(vsNode, sizeof(vsNode), VIRSERVER_ENTRY_NODE, pmEntryIdx);	
	
	if (cfg_query_object(vsNode,NULL,NULL) < 0)
	{
		cfg_obj_create_object(vsNode);
	}
	
	/*set Active*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, TR181_ACTIVE_ATTR, 0, tmp);
	}
	
	/*set REMOTEIP*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_REMOTEIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, "SrcIP", 0, tmp);
	}
	/*set STARTPORT*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_STARTPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, TR181_STARTPORT_ATTR, 0, tmp);
	}
	/*set ENDPORT*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_ENDPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{		 
		cfg_obj_set_object_attr(vsNode, TR181_ENDPORT_ATTR, 0, tmp);
 	}
	
	/*set INTPORT*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_INTPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, TR181_INTPORT_ATTR, 0, tmp);
	}
	/*set LOCALIP*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_LOCALIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{		
		cfg_obj_set_object_attr(vsNode, TR181_LOCALIP_ATTR, 0, tmp);
	}
	/*set PROTOCOL*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_PROTOCOL_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, "Protocal", 0, tmp);
	}
	/*set DESCRIP*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_DESCRIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, "MapName", 0, tmp);
	}	

	cfg_tr181_commit_object(vsNode);

	return 0;	
}
#else
int updatePortMapNodeByIdx(int entry1Idx, int entry2Idx)
{
	char portMappingNode[MAXLEN_NODE_NAME] = {0};	
	char vsNodeName[MAXLEN_NODE_NAME] = {0};
	char entry1Index[4] = {0};
	char entry2Index[4] = {0};	
	int i = 0;
	int index = -1;
	char pmEntryIdx[4] = {0};
	char Active[4] = {0};

	snprintf(vsNodeName, sizeof(vsNodeName), VIRSERVER_ENTRY_ENTRY_NODE, entry1Idx, entry2Idx);
	
	if(cfg_obj_get_object_attr(vsNodeName, TR181_ACTIVE_ATTR, 0, Active, sizeof(Active)) < 0)
	{
		return -1;
	}
	cfg_obj_get_object_attr(vsNodeName, TR181_PMENTRYIDX_ATTR, 0, pmEntryIdx, sizeof(pmEntryIdx));

	if(strlen(pmEntryIdx) >0 && atoi(pmEntryIdx) >= 0)
	{
		moveVirServerNodeToPortMap(entry1Idx, entry2Idx, atoi(pmEntryIdx));
		return 0;
	}		
	for ( i = 0; i < TR181_NATPORTMAPPING_NUM; i++ ) 
	{
		
		memset(portMappingNode, 0, sizeof(portMappingNode));
		snprintf(portMappingNode, sizeof(portMappingNode), TR181_NATPORTMAPPING_ENTRY_NODE, i + 1);
		memset(entry1Index, 0, sizeof(entry1Index));
		memset(entry2Index, 0, sizeof(entry2Index));
	
		if((cfg_obj_get_object_attr(portMappingNode, PVC_INDEX_ATTR, 0, entry1Index, sizeof(entry1Index)) >= 0) 
			&& (cfg_obj_get_object_attr(portMappingNode, TR181_VSENTRYIDX_ATTR, 0, entry2Index, sizeof(entry2Index)) >= 0)
			&& ((atoi(entry1Index) + 1) == entry1Idx && (atoi(entry2Index) + 1) == entry2Idx))
		{
			/* update portmapping  virserver->portmapping*/ 				
			moveVirServerNodeToPortMap(entry1Idx, entry2Idx, i);			
			break;	
		}
	}

	if(TR181_NATPORTMAPPING_NUM == i )
	{
		/*create portmapping entry*/
		index = createPortMappingEntry(entry1Idx, entry2Idx);
		if(index < 0)
		{			
			return -1;
		}		

		moveVirServerNodeToPortMap(entry1Idx, entry2Idx, index);
	}
		
	return 0;
}


int ClearPortMapNodeByIdx(int entry1Idx, int entry2Idx)
{
	char portMappingNode[MAXLEN_NODE_NAME] = {0};	
	char vsNodeName[MAXLEN_NODE_NAME] = {0};
	char entry1Index[4] = {0};
	char entry2Index[4] = {0};	
	int i = 0;
	char pmEntryIdx[4] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[20] = {0};
	int num = 0;

	snprintf(vsNodeName, sizeof(vsNodeName), VIRSERVER_ENTRY_ENTRY_NODE, entry1Idx, entry2Idx);

	cfg_obj_get_object_attr(vsNodeName, TR181_PMENTRYIDX_ATTR, 0, pmEntryIdx, sizeof(pmEntryIdx));

	if(strlen(pmEntryIdx) >0 && atoi(pmEntryIdx) >= 0)
	{
		snprintf(portMappingNode, sizeof(portMappingNode), TR181_NATPORTMAPPING_ENTRY_NODE, atoi(pmEntryIdx) + 1);
		if (cfg_query_object(portMappingNode,NULL,NULL) >= 0)
		{
			cfg_obj_delete_object(portMappingNode);

			memset(tmp, 0, sizeof(tmp));		
			memset(nodeName, 0, sizeof(nodeName));		
			strncpy(nodeName, TR181_NATPORTMAPPING_COMMON_NODE, sizeof(nodeName) - 1);
			
			cfg_obj_get_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
			num = atoi(tmp);	
			memset(tmp,0,sizeof(tmp));		
			snprintf(tmp, sizeof(tmp), "%d", num - 1);
			cfg_obj_set_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp);
		}
		return 0;
	}	
	for ( i = 0; i < TR181_NATPORTMAPPING_NUM; i++ ) 
	{
		memset(portMappingNode, 0, sizeof(portMappingNode));
		snprintf(portMappingNode, sizeof(portMappingNode), TR181_NATPORTMAPPING_ENTRY_NODE, i + 1);
		memset(entry1Index, 0, sizeof(entry1Index));
		memset(entry2Index, 0, sizeof(entry2Index));
	
		if((cfg_obj_get_object_attr(portMappingNode, PVC_INDEX_ATTR, 0, entry1Index, sizeof(entry1Index)) >= 0) 
			&& (cfg_obj_get_object_attr(portMappingNode, TR181_VSENTRYIDX_ATTR, 0, entry2Index, sizeof(entry2Index)) >= 0)
			&& ((atoi(entry1Index)) == entry1Idx && (atoi(entry2Index)) == entry2Idx))
		{
			cfg_obj_delete_object(portMappingNode);
			
			memset(tmp, 0, sizeof(tmp));		
			memset(nodeName, 0, sizeof(nodeName));		
			strncpy(nodeName, TR181_NATPORTMAPPING_COMMON_NODE, sizeof(nodeName) - 1);
			
			cfg_obj_get_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
			num = atoi(tmp);	
			memset(tmp,0,sizeof(tmp));		
			snprintf(tmp, sizeof(tmp), "%d", num - 1);
			cfg_obj_set_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp);
			break;	
		}
	}
		
	return 0;
}


void moveVirServerNodeToPortMap(int srcEntry1Index, int srcEntry2Index, int portMapEntryNum)
{
	char portMappingNode[MAXLEN_NODE_NAME] = {0};
	char portmapcomNode[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	int ipIndex = -1;
	int num = 0;
	
	memset(nodeName, 0, sizeof(nodeName));
	memset(portMappingNode, 0, sizeof(portMappingNode));

	snprintf(nodeName, sizeof(nodeName), VIRSERVER_ENTRY_ENTRY_NODE, srcEntry1Index, srcEntry2Index);
	snprintf(portMappingNode, sizeof(portMappingNode), TR181_NATPORTMAPPING_ENTRY_NODE, portMapEntryNum + 1);
	if (cfg_query_object(portMappingNode,NULL,NULL) < 0)
	{
		cfg_obj_create_object(portMappingNode);
		
		memset(tmp, 0, sizeof(tmp));		
		memset(portmapcomNode, 0, sizeof(portmapcomNode));		
		strncpy(portmapcomNode, TR181_NATPORTMAPPING_COMMON_NODE, sizeof(portmapcomNode) - 1);

		cfg_obj_get_object_attr(portmapcomNode, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
		num = atoi(tmp);	
		memset(tmp,0,sizeof(tmp));		
		snprintf(tmp, sizeof(tmp), "%d", num + 1);
		cfg_obj_set_object_attr(portmapcomNode, TR181_NUM_ATTR, 0, tmp);
	}

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, "MapName", 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_DESCRIP_ATTR, 0, tmp);
	}

	
	/* find IP.Interface index and set interfec under port mapping*/
	ipIndex = getTR181NodeEntryIdxByWanIf(TR181_IP_NODE, IP_INTERFACE_NUM, srcEntry1Index - 1);
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), TR181_IP_INTERFACE"%d", ipIndex + 1);
	setInterfaceValue(TR181_NATPORTMAPPING_NODE, portMapEntryNum, tmp);

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", srcEntry1Index - 1);
	cfg_obj_set_object_attr(portMappingNode, PVC_INDEX_ATTR, 0, tmp);

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", srcEntry2Index - 1);
	cfg_obj_set_object_attr(portMappingNode, TR181_VSENTRYIDX_ATTR, 0, tmp);

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_LOCALIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{	
		cfg_obj_set_object_attr(portMappingNode, TR181_LOCALIP_ATTR, 0, tmp);
	}	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_STARTPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{		
		cfg_obj_set_object_attr(portMappingNode, TR181_STARTPORT_ATTR, 0, tmp);
	}
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_ENDPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_ENDPORT_ATTR, 0, tmp);
	}	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_ACTIVE_ATTR, 0, tmp);
	}	

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, "SrcIP", 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_REMOTEIP_ATTR, 0, tmp);
	}	

	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(nodeName, "Protocal", 0, tmp, sizeof(tmp)) >= 0)
		&& strlen(tmp) > 0)
	{
		cfg_obj_set_object_attr(portMappingNode, TR181_PROTOCOL_ATTR, 0, tmp);
	}	


	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", portMapEntryNum);
	cfg_obj_set_object_attr(nodeName, TR181_PMENTRYIDX_ATTR, 0, tmp);

	return ;	
}



void movePortMapNodeToVirServer(int vsPvcIndex, int vsEntryIndex, int portMapEntryNum)
{
	char portMapNode[MAXLEN_NODE_NAME] = {0};	
	char vsEntryNode[MAXLEN_NODE_NAME] = {0};
	char vsNode[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	char buf[32] = {0};
	char node[MAXLEN_NODE_NAME] = {0};
		
	memset(vsNode, 0, sizeof(vsNode));
	memset(portMapNode, 0, sizeof(portMapNode));

	snprintf(vsEntryNode, sizeof(vsEntryNode), VIRSERVER_ENTRY_NODE, vsPvcIndex + 1);	
	snprintf(vsNode, sizeof(vsNode), VIRSERVER_ENTRY_ENTRY_NODE, vsPvcIndex + 1, vsEntryIndex + 1);	
	snprintf(portMapNode, sizeof(portMapNode), TR181_NATPORTMAPPING_ENTRY_NODE, portMapEntryNum);

	createPVCNodeIfInexistence(VIRSERVER_NODE, vsPvcIndex, vsEntryIndex);
	
	memset(tmp, 0, sizeof(tmp));
	setWanNodeByWanIf(wanNode,vsPvcIndex);
	if(cfg_obj_get_object_attr(wanNode, "IFName", 0, tmp, sizeof(tmp)) <= 0)
		return -1;
	
	cfg_obj_set_object_attr(vsEntryNode, "IFName", 0, tmp);
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", vsPvcIndex);
	cfg_obj_set_object_attr(portMapNode, PVC_INDEX_ATTR, 0, tmp);
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", vsEntryIndex);
	cfg_obj_set_object_attr(portMapNode, TR181_VSENTRYIDX_ATTR, 0, tmp);

	/*set DESCRIP*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_DESCRIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, "MapName", 0, tmp);
	}	
	
	/*set REMOTEIP*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_REMOTEIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, "SrcIP", 0, tmp);
	}
	/*set STARTPORT*/
	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(portMapNode, TR181_STARTPORT_ATTR, 0, buf, sizeof(buf)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, TR181_STARTPORT_ATTR, 0, buf);
	}
	/*set ENDPORT*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_ENDPORT_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{		
		if(strcmp(tmp, "0") != 0)
		{
			cfg_obj_set_object_attr(vsNode, TR181_ENDPORT_ATTR, 0, tmp);
		}
		else
		{
			cfg_obj_set_object_attr(vsNode, TR181_ENDPORT_ATTR, 0, buf);
		}		
	}
	
	/*set LOCALIP*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_LOCALIP_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{		
		cfg_obj_set_object_attr(vsNode, TR181_LOCALIP_ATTR, 0, tmp);
	}
	
	/*set PROTOCOL*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_PROTOCOL_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, "Protocal", 0, tmp);
	}

	/*set Active*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(portMapNode, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
	{
		cfg_obj_set_object_attr(vsNode, TR181_ACTIVE_ATTR, 0, tmp);
	}

	/*set PMEntryIdx*/
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", portMapEntryNum - 1);
	cfg_obj_set_object_attr(vsNode, TR181_PMENTRYIDX_ATTR, 0, tmp);

	snprintf(node, sizeof(node), VIRSERVER_ENTRY_ENTRY_NODE, vsPvcIndex + 1, vsEntryIndex + 1);
	cfg_tr181_commit_object(node);

	return ;	
}


int createPortMappingEntry(int Entry1Index, int Entry2Index)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char strPvcIdx[20] = {0};
	char strVSEntryIdx[20] = {0};
	char tmp[20] = {0};
	int num = 0;
	int fisrtEmptyEntry = -1;
	int index = -1;
	int existFlag = 0;
		
	for( i = 0; i < TR181_NATPORTMAPPING_NUM; i++)
	{		
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_NATPORTMAPPING_ENTRY_NODE, i + 1);
		
		/*for wan interface*/					
		memset(tmp,0,sizeof(tmp));		
		if(cfg_obj_get_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) < 0)
		{
			/*is not exist, find an empty entry*/
			if(fisrtEmptyEntry < 0)
			{
				fisrtEmptyEntry = i;
			}
			continue;
		}
		
		memset(strPvcIdx, 0, sizeof(strPvcIdx));		
		memset(strVSEntryIdx, 0, sizeof(strVSEntryIdx));	
		cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, strPvcIdx, sizeof(strPvcIdx));		
		cfg_obj_get_object_attr(nodeName, TR181_VSENTRYIDX_ATTR, 0, strVSEntryIdx, sizeof(strVSEntryIdx));
		
		if((atoi(strPvcIdx)) == Entry1Index && (atoi(strVSEntryIdx)) == Entry2Index)
		{
			/*find pvcIndex*/
			existFlag = 1;
			break;
		}
	}
	

	if( i != TR181_NATPORTMAPPING_NUM )
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
	
	memset(nodeName, 0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_NATPORTMAPPING_ENTRY_NODE, index + 1);
	if (cfg_query_object(nodeName,NULL,NULL) < 0)
	{
		cfg_obj_create_object(nodeName);
	}

	cfg_obj_set_object_attr(nodeName, TR181_ACTIVE_ATTR, 0, "Yes");
	
	memset(tmp,0,sizeof(tmp));		
	snprintf(tmp, sizeof(tmp), "%d", Entry1Index - 1);
	cfg_obj_set_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp);

	if(existFlag == 0)
	{
		/*new entry, set num*/	
		memset(tmp, 0, sizeof(tmp));		
		memset(nodeName, 0, sizeof(nodeName));		
		strncpy(nodeName, TR181_NATPORTMAPPING_COMMON_NODE, sizeof(nodeName) - 1);

		cfg_obj_get_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp, sizeof(tmp));
		num = atoi(tmp);	
		memset(tmp,0,sizeof(tmp));		
		snprintf(tmp, sizeof(tmp), "%d", num + 1);
		cfg_obj_set_object_attr(nodeName, TR181_NUM_ATTR, 0, tmp);
		
		if(updateInterfaceNodeDbg())
		{
			tcdbg_printf("[%s:%d]:num =%d \n",__FUNCTION__,__LINE__,num);
		}
	}
	
	return index;
}


void createPVCNodeIfInexistence(char* name, int vsPvcIndex, int vsEntryIndex){
	char element_name[MAXLEN_NODE_NAME] = {0};

	snprintf(element_name, sizeof(element_name), "%s.entry.%d", name, vsPvcIndex + 1);
	if (cfg_obj_query_object(element_name,NULL,NULL) < 0)
	{
		cfg_obj_create_object(element_name);
	}

	snprintf(element_name, sizeof(element_name), "%s.entry.%d.entry.%d", name, vsPvcIndex + 1, vsEntryIndex + 1);
	if (cfg_obj_query_object(element_name,NULL,NULL) < 0)
	{
		cfg_obj_create_object(element_name);
		return ;
	}

	return ;
}


int findVirServerByIPPvcIdx(int pmEntryIdx, int ipPvcIdx)
{
	char vsNode[MAXLEN_NODE_NAME] = {0};
	int i;
	char tmp[64] = {0};
	
	for(i = 0; i < MAX_VIRSERV_RULE; i++)
	{
		memset(vsNode, 0, sizeof(vsNode));
		snprintf(vsNode, sizeof(vsNode), VIRSERVER_ENTRY_ENTRY_NODE, ipPvcIdx + 1, i + 1);	

		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(vsNode, TR181_LOCALIP_ATTR, 0, tmp, sizeof(tmp)) < 0)
			&& strlen(tmp) == 0)
		{
			break;
		}
	}
	if(i == MAX_VIRSERV_RULE)
	{
		return FAIL;
	}			

	movePortMapNodeToVirServer(ipPvcIdx, i, pmEntryIdx);
	
	return SUCCESS;
}


int syncVirServer2(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char portMapNode[MAXLEN_NODE_NAME] = {0};	
	char vsNode[MAXLEN_NODE_NAME] = {0};	
	char tmp[64] = {0};
	int ipPvcIdx, ret;
	char pvcIdxStr[4] = {0};
	char vsEntryIdxStr[4] = {0};
	int pmEntryIdx = -1 ;
	char ipInterface[32] = {0}; 
	int i, ipEntryIdx, vsEntryIdx, pvcIdx;
	char node[MAXLEN_NODE_NAME] = {0};	

	/*get pmEntryIdx*/
	if(get_entry_number_cfg2(path, "entry.", &pmEntryIdx) != 0)
	{
		return -1;
	}
	
	if(pmEntryIdx < 1){
		return -1;
	}
	memset(portMapNode, 0, sizeof(portMapNode));
	snprintf(portMapNode, sizeof(portMapNode), TR181_NATPORTMAPPING_ENTRY_NODE, pmEntryIdx);

	/*Interface_attr is not exsit*/
	memset(ipInterface, 0, sizeof(ipInterface));
	if((cfg_obj_get_object_attr(portMapNode, TR181_INTERFACE_ATTR, 0, ipInterface, sizeof(ipInterface)) < 0)
		|| strlen(ipInterface) == 0)
	{
		return 0;
	}

	/*Active_attr != 'Yes'*/
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(portMapNode, TR181_ACTIVE_ATTR, 0, tmp, sizeof(tmp)) < 0)
		|| strcmp("Yes", tmp) != 0)
	{
		return 0;
	}	
	/*LOCALIP_attr is not exsit*/
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(portMapNode, TR181_LOCALIP_ATTR, 0, tmp, sizeof(tmp)) < 0)
	|| strlen(tmp) == 0)
	{
		return 0;
	}
	
	/*get wanPvcIndex*/
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, atoi(ipInterface + strlen(TR181_IP_INTERFACE)));
	
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp));
	
	if(atoi(tmp) < 0)
	{
		return 0;
	}		
	ipPvcIdx = atoi(tmp);

	memset(pvcIdxStr, 0, sizeof(pvcIdxStr));	
	cfg_obj_get_object_attr(portMapNode, PVC_INDEX_ATTR, 0, pvcIdxStr, sizeof(pvcIdxStr));
	memset(vsEntryIdxStr, 0, sizeof(vsEntryIdxStr));
	cfg_obj_get_object_attr(portMapNode, TR181_VSENTRYIDX_ATTR, 0, vsEntryIdxStr, sizeof(vsEntryIdxStr));
	
	if(strlen(pvcIdxStr) == 0 || strlen(vsEntryIdxStr) == 0)
	{		
		ret = findVirServerByIPPvcIdx(pmEntryIdx, ipPvcIdx);
		if(ret < 0)
		{
			return FAIL;
		}
	}
	else
	{
		if(ipPvcIdx == atoi(pvcIdxStr))
		{
			movePortMapNodeToVirServer(ipPvcIdx, atoi(vsEntryIdxStr), pmEntryIdx);
		}
		else
		{
			memset(node, 0, sizeof(node)) ;
			snprintf(node, sizeof(node), VIRSERVER_ENTRY_ENTRY_NODE, (atoi(pvcIdxStr) + 1), (atoi(vsEntryIdxStr) + 1));
			cfg_tr181_delete_object(node);

			ret = findVirServerByIPPvcIdx(pmEntryIdx, ipPvcIdx);
			if(ret < 0)
			{
				return FAIL;
			}
		}
	}
	
	return 0;

}
#endif

int cfg_type_natportmapping_func_commit(char* path)
{
#if defined(TCSUPPORT_CT_JOYME2)
	syncVirServer1(path);
#else
	syncVirServer2(path);
#endif
	return 0;
}

static cfg_node_ops_t cfg_type_natportmap_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_natportmap_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_natportmapping_func_commit 
}; 


static cfg_node_type_t cfg_type_natportmap_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | TR181_NATPORTMAPPING_NUM, 
	 .parent = &cfg_type_natportmap, 
	 .index = cfg_type_natportmap_index, 
	 .ops = &cfg_type_natportmap_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_natportmap_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_natportmapping_func_commit 
}; 


static cfg_node_type_t cfg_type_natportmap_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_natportmap, 
	 .ops = &cfg_type_natportmap_common_ops, 
}; 


static cfg_node_ops_t cfg_type_natportmap_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_natportmapping_func_commit 
}; 


static cfg_node_type_t* cfg_type_natportmap_child[] = { 
	 &cfg_type_natportmap_common, 
	 &cfg_type_natportmap_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_natportmap = { 
	 .name = "NatPortMap", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_natportmap_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_natportmap_child, 
	 .ops = &cfg_type_natportmap_ops, 
}; 
