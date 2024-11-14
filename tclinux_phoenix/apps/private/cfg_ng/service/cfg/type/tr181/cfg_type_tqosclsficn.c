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

int getQoSLanIfValuebyTr181(char*qosIfValue, char*tr181IfValue,int size)
{
	int lanIndex = -1;
	int WifiSSIDNum = -1;
	char nodeName[64] = {0};
	char tmp[32] = {0};
	
	if(qosIfValue == NULL || size <= 0)
	{
		return -1;
	}

	if(strstr(tr181IfValue, TR181_ETHER_INTERFACE))
	{
		/*  "Device.Ethernet.Interface.%d" means Lan interface */
		lanIndex = getSuffixIntByInterface(tr181IfValue);
	}
	else if(strstr(tr181IfValue, TR181_WIFI_SSID_IF))
	{
		/* "Device.WiFi.SSID.%d"  means Wifi interface */
		WifiSSIDNum = getSuffixIntByInterface(tr181IfValue);
		snprintf(nodeName, sizeof(nodeName), TR181_WIFISSID_ENTRY_NODE, WifiSSIDNum);
		if(cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0 && tmp[0] != '\0')
		{
			lanIndex =  atoi(tmp);
	}
	}

	if(lanIndex > 0)
	{
		snprintf(qosIfValue, size, "%d", lanIndex);
	}

	return 0;
}

int getTr181LanIfValueByQoS(char*qosIfValue, char*tr181IfValue,int size)
{
	int lanIndex = -1;
	int i = 0;
	char nodeName[64] = {0};
	char tmp[32]={0};
	
	if(qosIfValue == NULL || size <= 0 ||atoi(qosIfValue) <=0)
	{
		return -1;
	}
	
	lanIndex = atoi(qosIfValue);
	if(lanIndex <= LANIF_NUM_MAX)
	{
		snprintf(tr181IfValue, size, "%s%d",TR181_ETHER_INTERFACE, lanIndex);
		return 0;
	}
	else
	{
		for(i =1; i <= WIFI_NUM_MAX; i++)
		{
			snprintf(nodeName, sizeof(nodeName), TR181_WIFISSID_ENTRY_NODE, i);
			cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, tmp, sizeof(tmp));
			if(lanIndex == atoi(tmp))
			{
				snprintf(tr181IfValue, size, "%s%d",TR181_WIFI_SSID_IF, i);	
				return 0;
	}
	}
	}

	return -1;
}

int getQoSWanIfValuebyTr181(char*qosIfValue, char*tr181IfValue,int size)
{
	int ipIndex = -1;
	int pvcNum = -1;
	int entryNum = -1;
	char pvcIndex[8] = {0};
	char ipNodeName[32] = {0};
	
	if(qosIfValue == NULL || size <= 0)
	{
		return -1;
	}
	
	ipIndex = atoi(tr181IfValue + strlen(TR181_IP_INTERFACE));
	snprintf(ipNodeName, sizeof(ipNodeName), TR181_IP_ENTRY_NODE, ipIndex);

	if(cfg_obj_get_object_attr(ipNodeName, PVC_INDEX_ATTR, 0, pvcIndex, sizeof(pvcIndex)) < 0 ||atoi(pvcIndex) < 0)
	{
		return -1;
	}
	pvcNum = atoi(pvcIndex) / 8 ;
	entryNum = atoi(pvcIndex) % 8 ;
	snprintf(qosIfValue, size, "%d,%d", pvcNum+1, entryNum+1);
	
	return 0;
}


int getTr181WanIfValueByQoS(char*qosIfValue, char*tr181IfValue,int size)
{
	int i = 0;
	int ipIndex = -1;
	int pvcNum = -1;
	int entryNum = -1;
	int pvcIndex = -1;
	char strPvcIndex[32]= {0};
	char ipNodeName[32] = {0};
	
	if(qosIfValue == NULL || size <= 0)
	{
		return -1;
	}

	sscanf(qosIfValue, "%d,%d", &pvcNum, &entryNum);
	pvcIndex = (pvcNum -1) * 8 + (entryNum-1);	

	for(i = 0; i < IP_INTERFACE_NUM; i++)
	{
		memset(ipNodeName, 0, sizeof(ipNodeName));
		snprintf(ipNodeName, sizeof(ipNodeName), TR181_IP_ENTRY_NODE, i+1);
		if(cfg_obj_get_object_attr(ipNodeName, PVC_INDEX_ATTR, 0, strPvcIndex, sizeof(strPvcIndex)) >= 0 &&  strPvcIndex[0] != '\0')
		{
			if(atoi(strPvcIndex) == pvcIndex)
			{
				ipIndex = i+1;
				break;
			}
		}
	}
	if(ipIndex > 0)
	{
	snprintf(tr181IfValue, size, TR181_IP_INTERFACE_ENTRY, ipIndex);
	}
	
	return 0;
}


int getQosClsTypeNum(char* nodeName,char*clsTypeName)
{
	int i = 0;
	int firstNull = -1;
	char typeName[32] = {0};
	char typeValue[32] = {0};

	if(nodeName == NULL || clsTypeName == NULL)
	{
		return -1;
	}
	
	for (i = 0; i < MAX_TYPE_NUM; i++) 
	{	
		snprintf(typeName, sizeof(typeName), ATTR_TYPE, i+1);
		if(cfg_obj_get_object_attr(nodeName, typeName, 0, typeValue, sizeof(typeValue)) >= 0)
		{
			if(!strcmp("N/A", typeValue))
			{				
				if(-1 == firstNull)
				{
					firstNull = i+1;	
				}
			}
			else if(!strcmp(clsTypeName, typeValue))
			{
				return i+1;
			}
		}
		else
		{
			if(firstNull == -1)
			{
				firstNull = i+1;
			}
		}
	}

	return firstNull;
}

int syncTr181QosClsToQosCls(char* nodeName,char* tr181node)
{
	int ret = -1;
	int typeNum = 1;
	char attrVlaue[32] = {0};
	char tmp[256] = {0};
	char buf[16] = {0};

	if(nodeName == NULL || tr181node == NULL)
	{
		return -1;
	}

	/* SourceIP*/
	memset(tmp, 0x00, sizeof(tmp)); 
	if(cfg_obj_get_object_attr(tr181node, "SrcIp", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "SIP");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "SIP");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/*SrcIpRngMax */
	memset(tmp, 0x00, sizeof(tmp)); 
	if(cfg_obj_get_object_attr(tr181node, "SrcIpRngMax", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "SIP");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "SIP");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}
	
	/* DestIp*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "DestIp", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "DIP");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "DIP");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/*DestIpRngMax */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "DestIpRngMax", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "DIP");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "DIP");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}
	
	/* DestPort*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "DestPort", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0' )
	{
		typeNum = getQosClsTypeNum(nodeName, "DPORT");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "DPORT");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/* DestPortRangeMax*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "DestPortRngMax", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "DPORT");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "DPORT");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}
	
	/* SourcePort*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "SrcPort", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "SPORT");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "SPORT");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/* SourcePortRangeMax*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "SrcPortRngMax", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "SPORT");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "SPORT");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/* SourceMACAddress*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "SrcMacAddr", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "SMAC");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "SMAC");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/*DSCPMark */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "DSCPMark", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "DSCP");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "DSCP");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/* DSCPRangeEnd */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "DSCPRangeEnd", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "DSCP");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "DSCP");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}
	
	/* dotpBegin */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "dotpBegin", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "8021P");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "8021P");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}
	
	/* dotpEnd */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "dotpEnd", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "8021P");
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "8021P");
		
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, tmp);
	}

	/* Interface*/
	memset(tmp, 0x00, sizeof(tmp));
	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(tr181node, "Interface", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "LANInterface");
		getQoSLanIfValuebyTr181(buf, tmp, sizeof(buf));
		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "LANInterface");

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, buf);

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, buf);
	}

	/*InterfaceRngMax */
	memset(tmp, 0x00, sizeof(tmp));
	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(tr181node, "InterfaceRngMax", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		typeNum = getQosClsTypeNum(nodeName, "LANInterface");
		getQoSLanIfValuebyTr181(buf, tmp, sizeof(buf));		

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "LANInterface");

		memset(attrVlaue, 0, sizeof(attrVlaue));
		snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
		cfg_obj_set_object_attr(nodeName, attrVlaue, 0, buf);
		}

	
	/* WanInterface*/
		memset(tmp, 0x00, sizeof(tmp));
		memset(buf, 0, sizeof(buf));
		if(cfg_obj_get_object_attr(tr181node, "WanInterface", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
		{
			typeNum = getQosClsTypeNum(nodeName, "WANInterface");
			getQoSWanIfValuebyTr181(buf, tmp, sizeof(buf));

			memset(attrVlaue, 0, sizeof(attrVlaue));
			snprintf(attrVlaue, sizeof(attrVlaue), ATTR_TYPE, typeNum);
			cfg_obj_set_object_attr(nodeName, attrVlaue, 0, "WANInterface");

			memset(attrVlaue, 0, sizeof(attrVlaue));
			snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MAX, typeNum);
			cfg_obj_set_object_attr(nodeName, attrVlaue, 0, buf);

			memset(attrVlaue, 0, sizeof(attrVlaue));
			snprintf(attrVlaue, sizeof(attrVlaue), ATTR_MIN, typeNum);
			cfg_obj_set_object_attr(nodeName, attrVlaue, 0, buf);	
		}
	
	return 0;
}

int syncQosClsToTr181QosCls(char* nodeName,char* tr181Node)
{
	int i = 0;
	char buf[32] = {0};
	char typeName[32] = {0};
	char maxName[64] = {0};
	char minName[64] = {0};
	char typeValue[32] = {0};
	char maxValue[64] = {0};
	char minValue[64] = {0};

	if(nodeName == NULL || tr181Node == NULL)
	{
		return -1;
	}
	
	for (i = 0; i < MAX_TYPE_NUM; i++) 
	{
		memset(typeName, 0, sizeof(typeName));
		memset(maxName, 0, sizeof(maxName));
		memset(minName, 0, sizeof(minName));
		memset(typeValue, 0, sizeof(typeValue));
		memset(maxValue, 0, sizeof(maxValue));
		memset(minValue, 0, sizeof(minValue));
		snprintf(typeName, sizeof(typeName), "Type%d", i + 1);
		snprintf(maxName, sizeof(maxName), "Max%d", i + 1);
		snprintf(minName, sizeof(minName), "Min%d", i + 1);
		
		if(cfg_obj_get_object_attr(nodeName, typeName, 0, typeValue, sizeof(typeValue)) >= 0 )
		{	
			cfg_obj_get_object_attr(nodeName, maxName, 0, maxValue, sizeof(maxValue));
			cfg_obj_get_object_attr(nodeName, minName, 0, minValue, sizeof(minValue));
			
			 if(0 == strcmp(typeValue, "SIP"))  
			{	
				cfg_obj_set_object_attr(tr181Node, "SrcIp", 0, minValue);		
				cfg_obj_set_object_attr(tr181Node, "SrcIpRngMax", 0, maxValue);
			}
			else if(0 == strcmp(typeValue, "DIP"))  
			{
				cfg_obj_set_object_attr(tr181Node, "DestIp", 0, minValue);		
				cfg_obj_set_object_attr(tr181Node, "DestIpRngMax", 0, maxValue);
			}
			else if(0 == strcmp(typeValue, "SPORT"))
			{
				cfg_obj_set_object_attr(tr181Node, "SrcPort", 0, minValue);		
				cfg_obj_set_object_attr(tr181Node, "SrcPortRngMax", 0, maxValue);		
			}
			else if(0 == strcmp(typeValue, "DPORT"))
			{
				cfg_obj_set_object_attr(tr181Node, "DestPort", 0, minValue);		
				cfg_obj_set_object_attr(tr181Node, "DestPortRngMax", 0, maxValue);	
			}
			else if(0 == strcmp(typeValue, "SMAC"))
			{
				cfg_obj_set_object_attr(tr181Node, "SrcMacAddr", 0, maxValue); 
			}
			else if(0 == strcmp(typeValue, "DSCP"))
			{
				cfg_obj_set_object_attr(tr181Node, "DSCPMark", 0, minValue);	
				cfg_obj_set_object_attr(tr181Node, "DSCPRangeEnd", 0, maxValue);	
			}
			else if(0 == strcmp(typeValue, "8021P"))
			{
				cfg_obj_set_object_attr(tr181Node, "dotpBegin", 0, minValue);	
				cfg_obj_set_object_attr(tr181Node, "dotpEnd", 0, maxValue);	
			}
			else if(0 == strcmp(typeValue, "LANInterface"))
			{					
				memset(buf, 0, sizeof(buf));
				getTr181LanIfValueByQoS(minValue, buf, sizeof(buf));
				cfg_obj_set_object_attr(tr181Node, "Interface", 0, buf);	
				
				memset(buf, 0, sizeof(buf));
				getTr181LanIfValueByQoS(maxValue, buf, sizeof(buf));
				cfg_obj_set_object_attr(tr181Node, "InterfaceRngMax", 0, buf);		
			}
			else if(0 == strcmp(typeValue, "WANInterface"))
			{
				memset(buf, 0, sizeof(buf));
				getTr181WanIfValueByQoS(minValue, buf, sizeof(buf));
				cfg_obj_set_object_attr(tr181Node, "WanInterface", 0, buf);	
			}
		}
	}

	return 0;
}

int clearQosClsficn()
{
	int i = 0;
	char tr181node[MAXLEN_NODE_NAME] = {0};	
	
	for(i = 0; i < QOS_CLSFICN_NUM_MAX ; i++)
	{
		memset(tr181node, 0, sizeof(tr181node) );
		snprintf(tr181node, sizeof(tr181node), TR181_TQOSCLSFICN_ENTRY_NODE , i+1);	
		if(cfg_query_object(tr181node,NULL,NULL) >= 0)
		{
			cfg_tr181_delete_object(tr181node);
		}
	}
	
	return 0;
}


int initQosClsficn()
{
	char buf[64] = {0};
	char traCls[32] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char itf[256] = {0};
	char allitf[256] = {0};
	int ret = -1;
	int i = 0;
	int j = 0;
	char rabuf[16] = {0};
	char appVal[32] = {0};
	int idx = -1;
	int ClsficnNum = 0;
	
	for(i = 0; i < QOS_CLSFICN_NUM_MAX ; i++)
	{
		memset(nodeName, 0x00, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), QOS_ENTRY_NODE, i+1);
		memset(tr181node, 0x00, sizeof(tr181node));
		snprintf(tr181node, sizeof(tr181node), TR181_TQOSCLSFICN_ENTRY_NODE, i+1 );

		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "Active", 0, buf, sizeof(buf)) < 0)
		{
			continue;
		}
		
		ClsficnNum++;

		if(cfg_query_object(tr181node,NULL,NULL) < 0)
		{
			ret = cfg_create_object(tr181node);
			if(ret < 0)
			{
				return -1;
			}
		}
		
		/* Enable */
		if(!strcmp(buf, "Yes"))
		{
			cfg_obj_set_object_attr(tr181node, "Enable", 0, "1");	
		}
		else
		{
			cfg_obj_set_object_attr(tr181node, "Enable", 0, "0");
		}
#if 0
		/* Order */
		memset(buf, 0x00, sizeof(buf));
		snprintf(buf, sizeof(buf), "%d", i + 1);
		cfg_obj_set_object_attr(tr181node, "Order", 0, buf);
#endif
		/* Protocol */
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "ProtocolID", 0, buf, sizeof(buf)) >= 0 && buf[0] !='\0')
		{
			cfg_obj_set_object_attr(tr181node, "Protocol", 0, buf);	
		}
#if 0
		/* ForwardingPolicy */ 
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(QOS_COMMON_NODE, "Discipline", 0, buf, sizeof(buf)) >= 0 && buf[0] !='\0')
		{
			cfg_obj_set_object_attr(tr181node, "FwdPolcy", 0, buf);  
		}
#endif
		/* ActDSCPRmk */
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "ActDSCPRemarking", 0, buf, sizeof(buf)) >= 0 && buf[0] !='\0')
		{
			cfg_obj_set_object_attr(tr181node, "ActDSCPRmk", 0, buf);	 
		}
		
		/* Act8021pRmkNum */
		memset(buf, 0x00, sizeof(buf));
		if(cfg_obj_get_object_attr(nodeName, "Act8021pRemarkingNum", 0, buf, sizeof(buf)) >= 0 && buf[0] !='\0')
		{
			cfg_obj_set_object_attr(tr181node, "Act8021pRmkNum", 0, buf);	 
		}
		
		/* TrafficClass */
		memset(buf, 0x00, sizeof(buf));
		memset(traCls, 0x00, sizeof(traCls));
		if(cfg_obj_get_object_attr(nodeName, "ActQueue", 0, buf, sizeof(buf)) >= 0 && buf[0] !='\0')
		{
			snprintf(traCls, sizeof(traCls), "%d", atoi(buf) -1);
			cfg_obj_set_object_attr(tr181node, "TraCls", 0, traCls);	
		}

		/*cls type */
		syncQosClsToTr181QosCls(nodeName,tr181node);
	}
	
	return SUCCESS;
}

static int cfg_type_tqosclsficn_func_commit(char* path)
{
	char tr181node[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char node[MAXLEN_NODE_NAME] = {0};
	char tmp[256] = {0};
	char que[32] = {0};
	char rabuf[16] = {0};
	int etyIdx = -1;
	int j = 0;
	int idx = 0;
	int typeNum = 1;
	int addTypeFlag = 0;
	char attrVlaue[32] = {0};
	int ret = -1;

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0 &&  isNumber(tmp))
		{
			etyIdx = atoi(tmp) + 1;	
		}
		else
		{
			return -1;
		}
	}
	
	memset(tr181node, 0x00, sizeof(tr181node));
	snprintf(tr181node, sizeof(tr181node), TR181_TQOSCLSFICN_ENTRY_NODE, etyIdx );
	memset(nodeName, 0x00, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), QOS_ENTRY_NODE, etyIdx);

	if(cfg_query_object(nodeName,NULL,NULL) < 0)
	{
		ret = cfg_create_object(nodeName);
		if(ret < 0)
		{
			return -1;
		}
		else
		{
			cfg_obj_set_object_attr(nodeName, "Active", 0, "Yes");
			cfg_obj_set_object_attr(nodeName, "ActQueue", 0, "1");
			cfg_obj_set_object_attr(nodeName, "ActDSCPRemarking", 0, "0");
			cfg_obj_set_object_attr(nodeName, "Act8021pRemarkingNum", 0, "0");
		}
	}
	
	/* Enable  no used*/
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Enable", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		if(!strcmp(tmp, "1"))
		{
			cfg_obj_set_object_attr(nodeName, "Active", 0, "Yes");
		}
		else if(!strcmp(tmp, "0"))
		{
			cfg_obj_set_object_attr(nodeName, "Active", 0, "No");
		}
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, "Active", 0, "No");
	}

	/* Protocol */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Protocol", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		cfg_obj_set_object_attr(nodeName, "ProtocolID", 0, tmp);
	}
#if 0
	/* ForwardingPolicy  */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "FwdPolcy", 0, tmp, sizeof(tmp)) >= 0 && tmp[0] !='\0')
	{
		cfg_obj_set_object_attr(QOS_COMMON_NODE, "Discipline", 0, tmp);
	}
#endif
	/* ActDSCPRmk */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "ActDSCPRmk", 0, tmp, sizeof(tmp)) >= 0  && tmp[0] !='\0')
	{
		cfg_obj_set_object_attr(nodeName, "ActDSCPRemarking", 0, tmp);	
	}
	
	/* Act8021pRmkNum */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "Act8021pRmkNum", 0, tmp, sizeof(tmp)) >= 0  && tmp[0] !='\0')
	{
		cfg_obj_set_object_attr(nodeName, "Act8021pRemarkingNum", 0, tmp);	
	}
	
	/* TrafficClass */
	memset(tmp, 0x00, sizeof(tmp));
	if(cfg_obj_get_object_attr(tr181node, "TraCls", 0, tmp, sizeof(tmp)) >= 0  && tmp[0] !='\0')
	{
		snprintf(que, sizeof(que), "%d", atoi(tmp) +1);
		cfg_obj_set_object_attr(nodeName, "ActQueue", 0, que);	
	}

	/* cls type */
	syncTr181QosClsToQosCls(nodeName,tr181node);

	cfg_tr181_commit_object(nodeName);
	
	return SUCCESS;
}
 


static cfg_node_ops_t cfg_type_tqosclsficn_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqosclsficn_func_commit 
}; 


static cfg_node_ops_t cfg_type_tqosclsficn_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqosclsficn_func_commit 
}; 

static cfg_node_type_t cfg_type_tqosclsficn_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tqosclsficn, 
	 .ops = &cfg_type_tqosclsficn_common_ops, 
};


static cfg_node_type_t cfg_type_tqosclsficn_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | QOS_CLSFICN_NUM_MAX |1, 
	 .parent = &cfg_type_tqosclsficn, 
	 .ops = &cfg_type_tqosclsficn_entry_ops, 
};


static cfg_node_ops_t cfg_type_tqosclsficn_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tqosclsficn_func_commit 
}; 


static cfg_node_type_t* cfg_type_tqosclsficn_child[] = { 
	 &cfg_type_tqosclsficn_entry,
	 &cfg_type_tqosclsficn_common,
	 NULL 
}; 

cfg_node_type_t cfg_type_tqosclsficn= { 
	 .name = "TQosClsficn", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tqosclsficn) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tqosclsficn_child, 
	 .ops = &cfg_type_tqosclsficn_ops, 
}; 

