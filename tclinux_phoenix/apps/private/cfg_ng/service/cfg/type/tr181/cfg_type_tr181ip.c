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

void moveStaticWanIPToWan(int ipIf, int wanIf, char *ipversion)
{
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[64] = {0};
	char path[64] = {0};
	int i = 0, ipEntryIdx = -1, ret = -1;
	unsigned char buf[100] = {0};
	
	if(wanIf < 0)
	{
		return ;
	}
		
	if(ipIf < 0)
	{
		return;
	}

	if(NULL == ipversion)
	{
		return;
	}
	
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(strstr(ipversion, "IPv4") != NULL)
	{
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV4ADDR_PVC_ENTRY_NODE, ipIf + 1, 1);
		
		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, "IPAddress", 0, tmp, sizeof(tmp)) >= 0)
			 && strlen(tmp) > 0)
		{			
			cfg_obj_set_object_attr(wanNode, "IPADDR", 0, tmp);
		}
		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, "SubnetMask", 0, tmp, sizeof(tmp)) >= 0)
			&& strlen(tmp) > 0) 
		{
			cfg_obj_set_object_attr(wanNode, "NETMASK", 0, tmp);
		}
	}

	if(strstr(ipversion, "IPv6") != NULL){
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV6ADDR_PVC_ENTRY_NODE, ipIf + 1, 1);

		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, "IPAddress", 0, tmp, sizeof(tmp)) >= 0)
			&& strlen(tmp) > 0) 
		{			 
			cfg_obj_set_object_attr(wanNode, "IPADDR6", 0, tmp);
			
			memset(path, 0, sizeof(path));
			if((cfg_obj_get_object_attr(nodeName, "Prefix", 0, path, sizeof(path)) >= 0)
				&& strlen(path) > 0) 
			{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "Device.IP.Interface.%d.IPv6Prefix.", ipIf+1);
				if(strstr(path, tmp) != NULL && atoi(path+strlen(tmp)) >= 0)
				{
					memset(nodeName, 0, sizeof(nodeName));	
					snprintf(nodeName, sizeof(nodeName), TR181_IPV6PRE_PVC_ENTRY_NODE, ipIf + 1, atoi(path+strlen(tmp)));
					memset(tmp, 0, sizeof(tmp));
					if((cfg_obj_get_object_attr(nodeName, "Prefix", 0, tmp, sizeof(tmp)) >= 0)
						&& strlen(tmp) > 0) 
					{						
						cfg_obj_set_object_attr(wanNode, "PREFIX6", 0, tmp);
					}
				}		 
			}
		}
	}


	for(i=0; i<DNS_CLIENT_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));	
		snprintf(nodeName, sizeof(nodeName), TR181_DNSCLIENT_ENTRY_NODE, i + 1);

		memset(tmp, 0, sizeof(tmp));
		if((cfg_obj_get_object_attr(nodeName, TR181_INTERFACE_ATTR, 0, tmp, sizeof(tmp)) >= 0)
			&& strlen(tmp) > 0) 
		{
			ipEntryIdx = getSuffixIntByInterface(tmp) -1 ;

		}
		
		if(ipEntryIdx == ipIf)
		{
			memset(tmp, 0, sizeof(tmp));
			if((cfg_obj_get_object_attr(nodeName, "DNSIP", 0, tmp, sizeof(tmp)) >= 0)
				&& strlen(tmp) > 0) 
			{
				if(inet_pton4(tmp, buf) == 1)
				{
					cfg_obj_set_object_attr(wanNode, "DNSIP", 0, tmp);
				}					
				if(inet_pton6(tmp, buf) == 1)
				{
					cfg_obj_set_object_attr(wanNode, "DNSIPv61st", 0, tmp);
				}					
			}
			
			memset(tmp, 0, sizeof(tmp));
			if((cfg_obj_get_object_attr(nodeName, "SECDNSIP", 0, tmp, sizeof(tmp)) >= 0)
				&& strlen(tmp) > 0) 
			{
				if(inet_pton4(tmp, buf) == 1)
				{
					cfg_obj_set_object_attr(wanNode, "SECDNSIP", 0, tmp);
				}					
				if(inet_pton6(tmp, buf) == 1)
				{
					cfg_obj_set_object_attr(wanNode, "DNSIPv62nd", 0, tmp);
				}					
			}
		}		
	}
	return ;
}

int tr181IPIFNodeSync2Wan(int ipIf, int wanIf)
{
	char ipIFNode[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};
	int mtu = 0;
	int mss = 0;
	char strMtu[16] = {0};
	char tmp[32] = {0};
	int v4Enable = 0;
	int v6Enable = 0;
	int isp = 0;
	int autoIPEnable = 0, ret = -1;
	char ipversion[20] = {0};
	
	if(ipIf < 1 || wanIf < 0)
	{
		return 0;
	}
	
	memset(ipIFNode,0,sizeof(ipIFNode));
	snprintf(ipIFNode, sizeof(ipIFNode), TR181_IP_ENTRY_NODE, ipIf);

	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	/*set ipversion*/
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(ipIFNode, "IPv4Enable", 0, tmp, sizeof(tmp));
	v4Enable = atoi(tmp);

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(ipIFNode, "IPv6Enable", 0, tmp, sizeof(tmp));
	v6Enable = atoi(tmp);

	if(v4Enable == 1 && v6Enable == 1)
	{
		strncpy(ipversion, "IPv4/IPv6", sizeof(ipversion)-1);
	}
	else if( v6Enable == 1)
	{
		strncpy(ipversion, "IPv6", sizeof(ipversion)-1);
	}
	else
	{
		strncpy(ipversion, "IPv4", sizeof(ipversion)-1);
	}

	cfg_obj_set_object_attr(wanNode, "IPVERSION", 0, ipversion);

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(ipIFNode, "DHCPServerEnable", 0, tmp, sizeof(tmp)) > 0)
		cfg_obj_set_object_attr(wanNode, "DHCPEnable", 0, tmp);

	/*check DHCP or static*/
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(ipIFNode, "AutoIPEnable", 0, tmp, sizeof(tmp)) < 0)
		|| strlen(tmp) == 0)
	{
		/*default as static*/
		autoIPEnable = 0;
	}
	else
	{
		autoIPEnable = atoi(tmp);
	}
	
	/*set ISP*/ 
	memset(tmp,0,sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, "ISP", 0, tmp, sizeof(tmp));
	isp = atoi(tmp);

	if(isp != WAN_ENCAP_PPP_INT)
	{
		/*if ISP is not PPP, need to check bridge DHCP or static */
		tcdbg_printf("[%s]%d: isp = %d\n", __FUNCTION__, __LINE__, isp);
		if((checkRouteForward4(ipIf) == 0) && (checkRouteForward6(ipIf) == 0))
		{
			/*RouteForward is not set, and not PPP, set ISP as bridge wan*/		
			isp = WAN_ENCAP_BRIDGE_INT;		
			cfg_obj_set_object_attr(wanNode, "ISP", 0, "3"); /*WAN_ENCAP_BRIDGE*/
			cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Bridge");
		}
		else
		{	
			/*route mode*/
			cfg_obj_set_object_attr(wanNode, "WanMode", 0, "Route");
			cfg_obj_set_object_attr(wanNode, "LinkMode", 0, "linkIP");
			if(autoIPEnable != 0)
			{
				cfg_obj_set_object_attr(wanNode, "ISP", 0, "0");/*WAN_ENCAP_DYN*/
				if(strstr(ipversion, "IPv6") != NULL)
				{
					cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "Yes");
				}
			}
			else
			{
				cfg_obj_set_object_attr(wanNode, "ISP", 0, "1"); /*WAN_ENCAP_STATIC*/
				if(strstr(ipversion, "IPv6") != NULL)
				{
					cfg_obj_set_object_attr(wanNode, "DHCPv6", 0, "N/A");
				}
				moveStaticWanIPToWan(ipIf, wanIf, ipversion);
			}		
		}
	}
	
	/*set MTU*/
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(ipIFNode, "MTU", 0, tmp, sizeof(tmp)) >= 0)
	{
		mtu = atoi(tmp);		
		switch(isp)
		{
			case WAN_ENCAP_DYN_INT: /* dynamic 576~1500, 0 is default value */
				if(((mtu < 576) && (mtu != 0)) || (mtu > 1500)) 
				{					
					tcdbg_printf("[%s:%d]mtu =%d error, mtu[576, 1500]\n",__FUNCTION__,__LINE__, mtu);
					mtu = 0;
				}
				break;
			case WAN_ENCAP_STATIC_INT: /*  static 100~1500, 0 is default value */
				if(((mtu < 100) && (mtu != 0)) || (mtu > 1500))
				{					
					tcdbg_printf("[%s:%d]mtu =%d error, mtu[100, 1500]\n",__FUNCTION__,__LINE__, mtu);
					mtu = 0;
				}
				break;
			case WAN_ENCAP_PPP_INT:/* ppp 100~1492, 0 is default value */			
				memset(tmp,0,sizeof(tmp));
				cfg_obj_get_object_attr(wanNode, "MSS", 0, tmp, sizeof(tmp));
				sscanf(tmp, "%d", &mss);
				if(((mtu < 100) && (mtu != 0)) || (mtu > 1492) || (mtu < mss+40))
				{
					tcdbg_printf("[%s:%d]mtu =%d error, mtu[100, 1492]\n",__FUNCTION__,__LINE__, mtu);
					mtu = 0;
				}
				break;
			default: /* pure bridge */
				break;
		}
 
		snprintf(strMtu, sizeof(strMtu), "%d", mtu);
		cfg_obj_set_object_attr(wanNode, "MTU", 0, strMtu);
	}

	return 0;	
}

int checkIPIFEnableByIdx(int entryIdx)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char enable[4] = {0};
	int ret = -1;
	int nextEntryIdx = -1;

	if( entryIdx < 1 )
	{
		tcdbg_printf("[%s:%d]:entryIdx < 0\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	/*check enable*/
	memset(nodeName, 0, sizeof(nodeName));	
	memset(lowerlayer, 0, sizeof(lowerlayer));	
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, entryIdx);
	
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		tcdbg_printf("[%s:%d]:get lowerlayer fail\n",__FUNCTION__,__LINE__);
		return -1;
	}

	nextEntryIdx = getSuffixIntByInterface(lowerlayer);
	if( nextEntryIdx < 0)
	{
		tcdbg_printf("[%s:%d]:nextEntryIdx < 0\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(updateInterfaceNodeDbg())
	{
		tcdbg_printf("[%s:%d]:lowerlayer=%s,nextEntryIdx=%d\n",__FUNCTION__,__LINE__,lowerlayer,nextEntryIdx);
	}
	
	if(strstr(lowerlayer, TR181_PPP_INTERFACE) != NULL)
	{
		/*the next layer is ppp: for ppp wan*/	
		ret = checkPPPEnable(nextEntryIdx);			
	}
	else if(strstr(lowerlayer, TR181_ETHERNET_VLANTERMINATION) != NULL){
		
		/*the next layer is VLANTermination: for wan with vlan*/	
		ret = checkEtherVTEnable(nextEntryIdx);
	}
	else if(strstr(lowerlayer, TR181_ETHERNET_LINK) != NULL){
		
		/*the next layer is Link: for wan without vlan*/	
		ret = checkEtherLinkEnable(nextEntryIdx);
	}

	if(ret < 0){
		return -1;
	}

	if(updateInterfaceNodeDbg())
	{
		tcdbg_printf("[%s:%d]:lowerlayer=%s,ret=%d\n",__FUNCTION__,__LINE__,lowerlayer,ret);
	}
	
	/*check enable*/
	memset(enable, 0, sizeof(enable));
	if( (cfg_obj_get_object_attr(nodeName, "Enable", 0, enable, sizeof(enable)) >= 0)
		&& strcmp(enable, "1") == 0 && ret > 0)
	{
		return 1;
	}
	else
	{		
		return 0;
	}
	
}

int getLowerLayerStackByIpIf(struct lowerlayer_stack_s *lowerlayer_stack)
{	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char lowerlayer[256] = {0};
	int idx = 0;
	int ret = -1;

	/*check lowerlayer*/	
	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, lowerlayer_stack->ipIFIdx);
	/*not exist, need to create wan pvc*/
	if( (cfg_obj_get_object_attr(nodeName, "LowerLayers", 0, lowerlayer, sizeof(lowerlayer)) < 0)
		|| strlen(lowerlayer) == 0 )
	{
		/*lowerlayer is not exist, wan connect info is not enough*/
		tcdbg_printf("[%s:%d]lowerlayer not exist\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{
		/*lowerlayer is exist, check next lowerlayer*/
		if(strstr(lowerlayer, TR181_PPP_INTERFACE) != NULL){
			/*the next layer is ppp: for ppp wan*/	
			sscanf(lowerlayer + strlen(TR181_PPP_INTERFACE), "%d", &idx);			
			lowerlayer_stack->pppIFIdx = idx;
			tcdbg_printf("[%s:%d] idx=%d\n",__FUNCTION__,__LINE__,idx);
			ret = getLowerLayerStackByPPPIf(lowerlayer_stack);
				
		}else if(strstr(lowerlayer, TR181_ETHERNET_VLANTERMINATION) != NULL){
			/*the next layer is VLANTermination: for wan with vlan*/	
			sscanf(lowerlayer + strlen(TR181_ETHERNET_VLANTERMINATION), "%d", &idx);
			lowerlayer_stack->vlanTIdx = idx;
			tcdbg_printf("[%s:%d] idx=%d\n",__FUNCTION__,__LINE__,idx);
			ret = getLowerLayerStackByVlanT(lowerlayer_stack);

		}else if(strstr(lowerlayer, TR181_ETHERNET_LINK) != NULL){
			/*the next layer is Link: for wan without vlan*/		
			sscanf(lowerlayer + strlen(TR181_ETHERNET_LINK), "%d", &idx);
			lowerlayer_stack->EtherLinkIdx= idx;
			tcdbg_printf("[%s:%d] idx=%d\n",__FUNCTION__,__LINE__,idx);
			ret = getLowerLayerStackByEtherLink(lowerlayer_stack);
		}
	}
	
	return ret;
}

void setIPInterfaceLowerLayer(int ipIndex, char* lowerlayer)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};

	memset(nodeName, 0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, ipIndex + 1);
	cfg_obj_set_object_attr(nodeName, "LowerLayers", 0, lowerlayer);

	return;
}

int wanSync2TR181IPIfNode(int wanIf, int ipIFIdx)
{
	char wanNode[MAXLEN_NODE_NAME];	
	char nodeName[MAXLEN_NODE_NAME];
	char ipversion[16] = {0};
	char tmp[32] = {0};	
	char isp[4] = {0};
	int ret = -1;

	if(ipIFIdx < 0 || wanIf < 0)
	{
		tcdbg_printf("[%s:%d]ipIFIdx < 0 || wanIf < 0 error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	/*set other IP.Interface info, eg. IPv4Enable,IPv6Enable*/	
	memset(wanNode,0,sizeof(wanNode)); 	
	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName)); 	
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, ipIFIdx + 1);

	memset(tmp, 0, sizeof(tmp)); 	
	cfg_obj_get_object_attr(wanNode, "Active", 0, tmp, sizeof(tmp));
	if(strcmp(tmp, "Yes") == 0)
	{
		/*Wan Enable, set TR181IP Enable*/	
		cfg_obj_set_object_attr(nodeName, "Enable", 0, "1");
	}
	
	memset(ipversion, 0, sizeof(ipversion)); 	
	cfg_obj_get_object_attr(wanNode, "IPVERSION", 0, ipversion, sizeof(ipversion));
	if(strstr(ipversion, "IPv4") != NULL )
	{
		cfg_obj_set_object_attr(nodeName, "IPv4Enable", 0, "1");
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, "IPv4Enable", 0, "0");
	}
	
	if(strstr(ipversion, "IPv6") != NULL )
	{
		cfg_obj_set_object_attr(nodeName, "IPv6Enable", 0, "1");
	}
	else
	{
		cfg_obj_set_object_attr(nodeName, "IPv6Enable", 0, "0");
	}

	memset(tmp, 0, sizeof(tmp)); 	
	cfg_obj_get_object_attr(wanNode, "MTU", 0, tmp, sizeof(tmp));
	cfg_obj_set_object_attr(nodeName, "MTU", 0, tmp);

	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(wanNode, "DHCPEnable", 0, tmp, sizeof(tmp)) > 0)
	cfg_obj_set_object_attr(nodeName, "DHCPServerEnable", 0, tmp);

	memset(isp, 0, sizeof(isp));
	cfg_obj_get_object_attr(wanNode, "ISP", 0, isp, sizeof(isp));
	if(strcmp(isp,"0") == 0)
	{
		/*DHCP*/
		cfg_obj_set_object_attr(nodeName, "AutoIPEnable", 0, "1");
	}
#if 0
	else if(strcmp(isp, "2") == 0)
	{
		/*PPP*/	
		memset(tmp, 0, sizeof(tmp));	
		cfg_obj_get_object_attr(wanNode, "PPPGETIP", 0, tmp, sizeof(tmp));
		if(strcmp(tmp,"Dynamic") == 0)
		{
			cfg_obj_set_object_attr(nodeName, "AutoIPEnable", 0, "1");
		}
		else
		{
			cfg_obj_set_object_attr(nodeName, "AutoIPEnable", 0, "0");
		}
	}
#endif
	else
	{	/*Static*/
		cfg_obj_set_object_attr(nodeName, "AutoIPEnable", 0, "0");
	}	

	return 0;
}

int findLanIPInterfaceEntry(void)
{
	struct tr181_interface_s interface;
	int ret = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_IP_NODE, 64 - 1);
	interface.num = IP_INTERFACE_NUM;
	interface.pvcIndex = -1;
	interface.lanIndex = 0;
	interface.active = 1;
	ret = createTR181InterfaceEntry(interface);

	return ret;
}

int findIPInterfaceEntry(int wanIf, int active)
{
	struct tr181_interface_s interface;
	int index = -1;

	if(wanIf < 0 )
	{
		tcdbg_printf("[%s:%d]pvcIndex < 0 \n",__FUNCTION__,__LINE__);
		return 0;
	}

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_IP_NODE, 64 - 1);
	interface.num = IP_INTERFACE_NUM;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	interface.active = active;
	index = createTR181InterfaceEntry(interface);

	if(index < 0)
	{
		tcdbg_printf("[%s:%d]index < 0!\n",__FUNCTION__,__LINE__);
		return 0;
	}

	wanSync2TR181IPIfNode(wanIf, index);
	clearTR181PVCNodeByIdx(TR181_IP_INTERFACE_IPV4ADDR_NODE, index);
	clearTR181PVCNodeByIdx(TR181_IP_INTERFACE_IPV6ADDR_NODE, index);

	return index;
}

void moveIPInfoToLan(int flag, int pvcIdx, int  entryIdx)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char tmp[64] = {0};
	char prefixPath[64] = {0};

	if(pvcIdx < 0 || entryIdx < 0)
	{
		return ;
	}

	switch (flag)
	{
		case IPINTERFACE_EXECUTE_FLAG:
			/*IPv4Addr_Info*/
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV4ADDR_PVC_ENTRY_NODE, pvcIdx, entryIdx);

			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeName, "IPAddress", 0, tmp, sizeof(tmp)) > 0) 
			{	
				cfg_obj_set_object_attr(LAN_ENTRY0_NODE, "IP", 0, tmp);
			}
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeName, "SubnetMask", 0, tmp, sizeof(tmp)) > 0) 
			{
				cfg_obj_set_object_attr(LAN_ENTRY0_NODE, "netmask", 0, tmp);
			}
			/*IPv6Addr_Info*/
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV6ADDR_PVC_ENTRY_NODE, pvcIdx, entryIdx);

			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeName, "IPAddress", 0, tmp, sizeof(tmp)) > 0) 
			{	
				cfg_obj_set_object_attr(LAN_ENTRY0_NODE, "IP6", 0, tmp);
			}
	
			break;
			
		case IPV4ADDR_EXECUTE_FLAG:
			/*IPv4Addr_Info*/
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV4ADDR_PVC_ENTRY_NODE, pvcIdx, entryIdx);

			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeName, "IPAddress", 0, tmp, sizeof(tmp)) > 0) 
			{
				cfg_obj_set_object_attr(LAN_ENTRY0_NODE, "IP", 0, tmp);
			}
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeName, "SubnetMask", 0, tmp, sizeof(tmp)) > 0) 
			{
				cfg_obj_set_object_attr(LAN_ENTRY0_NODE, "netmask", 0, tmp);
			}
			break;
			
		case IPV6ADDR_EXECUTE_FLAG:
			/*IPv6Addr_Info*/
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), TR181_TR181IPV6ADDR_PVC_ENTRY_NODE, pvcIdx , entryIdx );

			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(nodeName, "IPAddress", 0, tmp, sizeof(tmp)) > 0) 
			{
				cfg_obj_set_object_attr(LAN_ENTRY0_NODE, "IP6", 0, tmp);
			}

			break;
			
		default:
			return ;
	}

	cfg_tr181_commit_object(LAN_ENTRY0_NODE);

	return ;
}

int cfg_type_tr181ip_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};		
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char ipIFNode[MAXLEN_NODE_NAME] = {0};
	char lowerlayer[256] = {0};
	char uplayer[256] = {0};
	int ipIfIdx = -1;
	int tmp[32] = {0};
	int ret = -1;
	int oldPvcIdx = -1;
	int newPvcIdx = -1;
	int enable = -1;
	int newWanIf = -1;
	int oldWanIf = -1;
	int lanIdx = -1;
	int i = 0;
	char IPv4Enable[2] = {0};
	char IPv6Enable[2] = {0};
	
	if ( get_entry_number_cfg2(path, "entry.", &ipIfIdx) != SUCCESS ) 
	{
		cfg_obj_get_object_attr(TR181_IP_COMMON_NODE, "IPv4Enable", 0, IPv4Enable, sizeof(IPv4Enable));
		cfg_obj_get_object_attr(TR181_IP_COMMON_NODE, "IPv6Enable", 0, IPv6Enable, sizeof(IPv6Enable));
		if((atoi(IPv4Enable) == 1) && (atoi(IPv6Enable) == 0))
		{
			cfg_set_object_attr(SYS_ENTRY_NODE, "IPProtocolVersion", "1");
		}
		else if((atoi(IPv4Enable) == 0) && (atoi(IPv6Enable) == 1))
		{
			cfg_set_object_attr(SYS_ENTRY_NODE, "IPProtocolVersion", "2");
		}
		else if((atoi(IPv4Enable) == 1) && (atoi(IPv6Enable) == 1))
		{
			cfg_set_object_attr(SYS_ENTRY_NODE, "IPProtocolVersion", "3");
		}
		else
		{
			tcdbg_printf("\r\nBoth IPv4Enable and IPv6Enable are not suppported  to be 0\n");
		}
		cfg_commit_object(SYS_ENTRY_NODE);
	}
	
	if(ipIfIdx < 1)
	{
		tcdbg_printf("[%s:%d]ipIfIdx is error, no need to sync\n",__FUNCTION__,__LINE__);	
		return -1;
	}
	
	memset(tmp,0,sizeof(tmp));
	memset(ipIFNode, 0, sizeof(ipIFNode));
	snprintf(ipIFNode, sizeof(ipIFNode), TR181_IP_ENTRY_NODE, ipIfIdx);
	if((cfg_obj_get_object_attr(ipIFNode, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) < 0)
		|| strlen(tmp) == 0 || !isNumber(tmp))
	{
		/*pvcindex is not exist, wan is not create*/	
		oldWanIf = -1;
	}
	else
	{
		oldWanIf = atoi(tmp);
	}

	memset(tmp,0,sizeof(tmp));
	if((cfg_obj_get_object_attr(ipIFNode, LAN_INDEX_ATTR, 0, tmp, sizeof(tmp)) < 0)
		|| strlen(tmp) == 0 || !isNumber(tmp))
	{
		/*pvcindex is not exist, wan is not create*/	
		lanIdx = -1;
	}
	else
	{
		lanIdx = atoi(tmp);
	}

	if(oldWanIf < 0 && lanIdx == 0)
	{
		moveIPInfoToLan(IPINTERFACE_EXECUTE_FLAG, 1, 1);
		return 0;
	}

	enable = checkIPIFEnableByIdx(ipIfIdx);
	
	/*get lowerlayer pvcindex*/
	newWanIf = getPvcIdxOfLowerLayer(TR181_IP_NODE, ipIfIdx);
#if 0
	if(newWanIf < 0)
	{
		/*new Wan not exist,do nothing*/
		tcdbg_printf("[%s:%d]new wan not exist!\n", __FUNCTION__, __LINE__);
		return -1;
	}
#endif
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, ipIfIdx);
	cfg_obj_get_object_attr(nodeName, TR181_LOWERLAYER_ATTR, 0, lowerlayer, sizeof(lowerlayer));

	if(newWanIf != oldWanIf)
	{
		/*set old Wan Active to Disable*/
		memset(wanNode, 0, sizeof(wanNode));
		ret = setWanNodeByWanIf(wanNode, oldWanIf);
		if(ret < 0)
		{		
			tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		}
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
		for(i = 0; i < IP_INTERFACE_NUM; i++)
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, i + 1);
			if((cfg_obj_get_object_attr(nodeName, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0)
				&& (newWanIf == atoi(tmp)))
			{
				cfg_obj_set_object_attr(nodeName, "Enable", 0, "0");
				cfg_obj_set_object_attr(nodeName, "pvcIndex", 0, "-1");
				cfg_obj_set_object_attr(nodeName, "LowerLayers", 0, "");
			}
		}
				
		memset(uplayer, 0, sizeof(uplayer));
		snprintf(uplayer, sizeof(uplayer), "%s%d", TR181_IP_INTERFACE, ipIfIdx);

		ClearOldUpLayer(uplayer);
		
		commitWanByWanIf(oldWanIf);
	}

	memset(uplayer, 0, sizeof(uplayer));
	snprintf(uplayer, sizeof(uplayer), "%s%d", TR181_IP_INTERFACE, ipIfIdx);
	SetNextUpLayerByLowerLayer(lowerlayer, uplayer);
	
	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%d", newWanIf);
	cfg_obj_set_object_attr(ipIFNode, PVC_INDEX_ATTR, 0, tmp);

	/*sync new WanInfo*/
	memset(wanNode, 0, sizeof(wanNode));
	ret = setWanNodeByWanIf(wanNode, newWanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(newWanIf != oldWanIf)
	{
		/*Wan modify, need to sync all info by uplayer.  to new wan*/
		sync2WanByUpLayer(uplayer, newWanIf);
	}
	else
	{
		/*lowerlayer is not modify, only need to sync TR181IP info*/
		tr181IPIFNodeSync2Wan(ipIfIdx, newWanIf);
	}
	if(enable == 1) 
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "Yes");
	}
	else
	{
		cfg_obj_set_object_attr(wanNode, "Active", 0, "No");
	}
		
	commitWanByWanIf(newWanIf);

	return 0;	
}

static cfg_node_ops_t cfg_type_tr181ip_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ip_func_commit
}; 


static cfg_node_type_t cfg_type_tr181ip_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | IP_INTERFACE_NUM, 
	 .parent = &cfg_type_tr181ip, 
	 .ops = &cfg_type_tr181ip_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_tr181ip_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ip_func_commit 
}; 


static cfg_node_type_t cfg_type_tr181ip_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_tr181ip, 
	 .ops = &cfg_type_tr181ip_common_ops, 
}; 


static cfg_node_ops_t cfg_type_tr181ip_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_tr181ip_func_commit 
}; 


static cfg_node_type_t* cfg_type_tr181ip_child[] = { 
	 &cfg_type_tr181ip_common, 
	 &cfg_type_tr181ip_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_tr181ip = { 
	 .name = "TR181IP", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_tr181ip_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_tr181ip_child, 
	 .ops = &cfg_type_tr181ip_ops, 
}; 
