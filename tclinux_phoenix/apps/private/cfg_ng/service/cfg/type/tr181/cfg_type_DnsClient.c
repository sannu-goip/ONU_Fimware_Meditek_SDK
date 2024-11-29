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

int createDNSClientInterfaceEntry(struct tr181_interface_s interface, char* ipversion)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int i = 0;
	char tmp[20] = {0};
	char enable[20] = {0};
	char lanindex[4] = {0};
	char pvcindex[4] = {0};
	int num = 0;
	int fisrtEmptyEntry = -1;
	int index = -1;
	int existFlag = 0;
	int flag = 0;
	char dnsip[64] = {0};
	char buf[256]={0};	
	char type[32] = {0};

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
				if(cfg_obj_get_object_attr(nodeName, TR181_TYPE_ATTR, 0, type, sizeof(type)) < 0)
				{
					continue;
				}
				else if(strcmp(type, "Static") == 0)
				{
					if((cfg_obj_get_object_attr(nodeName, TR181_DNSIP_ATTR, 0, dnsip, sizeof(dnsip)) >= 0)
						&& (((strcmp(ipversion, "IPv4") == 0) && (inet_pton4(dnsip, buf) == 1))
						|| ((strcmp(ipversion, "IPv6") == 0) && (inet_pton6(dnsip, buf) == 1))))
					{
						/*find pvcIndex*/
						existFlag = 1;
						break;
					}
				}
				else if((strcmp(type, "DHCPv4") == 0) && (strcmp(ipversion, "IPv4") == 0))
				{
					/*find pvcIndex*/
					existFlag = 1;
					break;
				}
				else if(((strcmp(type, "DHCPv6") == 0) || (strcmp(type, "RouterAdvertisement") == 0)) 
					&& (strcmp(ipversion, "IPv6") == 0))
				{
					/*find pvcIndex */
					existFlag = 1;
					break;
				}
				else
				{
					/*check next entry*/
				}
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

int findDNSClientInterfaceEntry(int wanIf, int active, char *ipversion)
{
	struct tr181_interface_s interface;
	int index = -1;

	memset(&interface,0,sizeof(interface));	
	strncpy(&interface.nodeName, TR181_DNS_CLIENT_NODE, CFG_BUF_64_LEN - 1);
	interface.num = DNS_CLIENT_NUM;
	interface.pvcIndex = wanIf;
	interface.lanIndex = -1;
	interface.active = active;
	index = createDNSClientInterfaceEntry(interface, ipversion);

	return index;
}

void setDNSServerType(int index, char * typeVal)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	
	memset(nodeName, 0, sizeof(nodeName));		
	snprintf(nodeName, sizeof(nodeName), TR181_DNSCLIENT_ENTRY_NODE, index + 1);
	cfg_obj_set_object_attr(nodeName, "Type", 0, typeVal);

	return ;
}

void setV6DNSServer(int entryIndex, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char DNSNode[MAXLEN_NODE_NAME] = {0};
	char dnsServer[64] = {0};
	char type[32] = {0};

	memset(DNSNode, 0, sizeof(DNSNode));		
	snprintf(DNSNode, sizeof(DNSNode), TR181_DNSCLIENT_ENTRY_NODE, entryIndex + 1);

	memset(nodeName, 0, sizeof(nodeName));
	if((cfg_obj_get_object_attr(DNSNode, TR181_TYPE_ATTR, 0, type, sizeof(type)) >= 0) 
		&& (strcmp(type, "Static") == 0))
	{
		setWanNodeByWanIf(nodeName, wanIf);
	}
	
	memset(dnsServer, 0, sizeof(dnsServer));			
	cfg_obj_get_object_attr(nodeName, "DNS6", 0, dnsServer, sizeof(dnsServer));

	if(strlen(dnsServer) > 0)
	{
		cfg_obj_set_object_attr(DNSNode, TR181_DNSIP_ATTR, 0, dnsServer);
	}

	return ;
}

void setV4DNSServer(int entryIndex, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char DNSNode[MAXLEN_NODE_NAME] = {0};
	char dnsServer[64] = {0};
	char type[32] = {0};

	memset(DNSNode, 0, sizeof(DNSNode));		
	snprintf(DNSNode, sizeof(DNSNode), TR181_DNSCLIENT_ENTRY_NODE, entryIndex + 1);

	memset(nodeName, 0, sizeof(nodeName));
	if((cfg_obj_get_object_attr(DNSNode, TR181_TYPE_ATTR, 0, type, sizeof(type)) >= 0) 
		&& (strcmp(type, "Static") == 0))
	{
		setWanNodeByWanIf(nodeName,wanIf);
	}
		
	memset(dnsServer, 0, sizeof(dnsServer));			
	cfg_obj_get_object_attr(nodeName, "DNS", 0, dnsServer, sizeof(dnsServer));
	if(strlen(dnsServer) > 0)
	{
		cfg_obj_set_object_attr(DNSNode, TR181_DNSIP_ATTR, 0, dnsServer);
	}

	return ;
}


int setDNSClientTypeByWanIf(int wanIf, int dnsClientIdx, int type)
{
	char dnsClientNode[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};
	char strISP[16] = {0};
	char v6type[16] = {0};
	int isp = -1;


	memset(dnsClientNode, 0, sizeof(dnsClientNode));
	snprintf(dnsClientNode, sizeof(dnsClientNode), TR181_DNSCLIENT_ENTRY_NODE, dnsClientIdx);

	memset(wanNode, 0, sizeof(wanNode));
	if(setWanNodeByWanIf(wanNode, wanIf) < 0){
		return -1;
	}
	
	cfg_obj_get_object_attr(wanNode, "ISP", 0, strISP, sizeof(strISP));
	cfg_obj_get_object_attr(wanNode, "DHCPv6", 0, v6type, sizeof(v6type));
	isp = atoi(strISP);

	if(isp == WAN_ENCAP_STATIC_INT)
	{
		/*statitc*/
		setDNSServerType(dnsClientIdx, "Static");	
		if(type == 1)
		{
			setV4DNSServer(dnsClientIdx, wanIf);
		}
		else if(type == 2)
		{
			setV6DNSServer(dnsClientIdx, wanIf);
		}
		else
		{
			/*do nothing*/
		}
		return 0;
	}

	switch(type)
	{
		case 0:
		case 1:
			if(isp == WAN_ENCAP_DYN_INT)
			{
				setDNSServerType(dnsClientIdx, "DHCPv4");
			}
			else if(isp == WAN_ENCAP_PPP_INT)
			{
				setDNSServerType(dnsClientIdx, "IPCP");
			}
			break;
		case 2:
			if(strcmp("No", v6type) == 0) 
			{
				setDNSServerType(dnsClientIdx, "RouterAdvertisement");
			}
			else 
			{
				setDNSServerType(dnsClientIdx, "DHCPv6");
			}
		break;
	}

	return 0;	
}


int getDNSClientType(int dnsClientIdx)
{
	char dnsClientNode[MAXLEN_NODE_NAME] = {0};
	char dnsType[32] = {0};
	char dnsServer[64] = {0};
	char buf[256] = {0};
	
	memset(dnsClientNode, 0, sizeof(dnsClientNode));
	snprintf(dnsClientNode, sizeof(dnsClientNode), TR181_DNSCLIENT_ENTRY_NODE, dnsClientIdx);

	memset(dnsType, 0, sizeof(dnsType));
	cfg_obj_get_object_attr(dnsClientNode, TR181_TYPE_ATTR, 0, dnsType, sizeof(dnsType));

	if(strlen(dnsType) == 0)
	{
		/*do not know v4 or v6*/
		return 0;
	}

	if(strcmp(dnsType, "Static") == 0)
	{		
		memset(buf, 0, sizeof(buf));
		memset(dnsServer, 0, sizeof(dnsServer));
		cfg_obj_get_object_attr(dnsClientNode, TR181_DNSIP_ATTR, 0, dnsServer, sizeof(dnsServer));
		if(inet_pton4(dnsServer, buf))
		{
			/*v4*/
			return 1;
		}

		memset(buf, 0, sizeof(buf));
		if(inet_pton6(dnsServer, buf))
		{			
			/*v6*/
			return 2;
		}
	}

	if(strcmp(dnsType, "DHCPv4") == 0 || strcmp(dnsType, "IPCP") == 0)
	{
		return 1;
	}

	if(strcmp(dnsType, "RouterAdvertisement") == 0 || strcmp(dnsType, "DHCPv6") == 0)
	{
		return 2;
	}

	return 0;
}


int setDNSClientInfoByPvcIndex(int wanIf, int ipIndex, int active)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char ipVersion[16] = {0};
	char strInterface[256] = {0};	
	int index = -1;
	
	memset(nodeName, 0, sizeof(nodeName));
	if(setWanNodeByWanIf(nodeName, wanIf) < 0){
		return -1;
	}
	
	cfg_obj_get_object_attr(nodeName, "IPVERSION", 0, ipVersion, sizeof(ipVersion));

	
	if(strstr(ipVersion, "IPv4"))
	{
		index = findDNSClientInterfaceEntry(wanIf, active, "IPv4");
		setDNSClientTypeByWanIf(wanIf, index, 1);
		
		memset(strInterface, 0, sizeof(strInterface));
		snprintf(strInterface, sizeof(strInterface), TR181_IP_INTERFACE"%d", ipIndex + 1);	
		setInterfaceValue(TR181_DNS_CLIENT_NODE, index, strInterface);
	} 

	if(strstr(ipVersion, "IPv6"))
	{
		index = findDNSClientInterfaceEntry(wanIf, active, "IPv6");
		setDNSClientTypeByWanIf(wanIf, index, 2);
		
		memset(strInterface, 0, sizeof(strInterface));
		snprintf(strInterface, sizeof(strInterface), TR181_IP_INTERFACE"%d", ipIndex + 1);	
		setInterfaceValue(TR181_DNS_CLIENT_NODE, index, strInterface);
	}

	return 0;

}

int dnsClientEntrySync2WanByWanPvc(int index, int wanIf)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char buf[256]={0};	
	char dnsIP[32] = {0};	
	int ret = -1;

	if(index < 1 ||  wanIf < 0)
	{
		tcdbg_printf("[%s:%d]index=%d wanIf=%d error!\n",__FUNCTION__,__LINE__,index,wanIf);
		return -1;
	}

	memset(nodeName, 0, sizeof(nodeName));	
	snprintf(nodeName, sizeof(nodeName), TR181_DNSCLIENT_ENTRY_NODE, index);

	ret = setWanNodeByWanIf(wanNode, wanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	memset(dnsIP, 0, sizeof(dnsIP));
	if(cfg_obj_get_object_attr(nodeName, TR181_DNSIP_ATTR, 0, dnsIP, sizeof(dnsIP)) >= 0)
	{
		if(inet_pton4(dnsIP, buf) == 1)
		{
			cfg_obj_set_object_attr(wanNode, "DNS", 0, dnsIP);
		}
		else if(inet_pton6(dnsIP, buf) == 1)	
		{
			cfg_obj_set_object_attr(wanNode, "DNS6", 0, dnsIP);
		}
	}
			
	return 0;
}


void syncDNSServer2Dhcp6s(int etyIdx)
{
	char dnsNode[MAXLEN_NODE_NAME] = {0};
	char tmp[128] = {0};

	snprintf(dnsNode,sizeof(dnsNode),TR181_DNSCLIENT_ENTRY_NODE,etyIdx);
	
	/*set DNSServer*/
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(dnsNode, TR181_DNSIP_ATTR, 0, tmp, sizeof(tmp)) > 0)
	{
		cfg_obj_set_object_attr(DHCP6S_ENTRY_NODE, "DNSserver", 0, tmp);
	}
	
	cfg_tr181_commit_object(DHCP6S_ENTRY_NODE);
	return ;
}

int cfg_type_dnsclient_func_commit(char* path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	char dnsClientNode[MAXLEN_NODE_NAME] = {0};
	char wanNode[MAXLEN_NODE_NAME] = {0};	
	char tmp[256] = {0};
	char lanIndex[8] = {0};
	int dnsClientIdx = -1;	
	int ipIfIdx = -1;
	int oldWanIf = -1;
	int newWanIf = -1;
	int i = 0;
	int ret = -1;
	int oldtype = 0;

	if(get_entry_number_cfg2(path, "entry.", &dnsClientIdx) != 0 || dnsClientIdx < 0)
	{
		return -1;
	}
	
	memset(dnsClientNode, 0, sizeof(dnsClientNode));
	snprintf(dnsClientNode, sizeof(dnsClientNode), TR181_DNSCLIENT_ENTRY_NODE, dnsClientIdx);

	/*check Enable: is not enable ,not take effect*/	
	memset(tmp, 0, sizeof(tmp));
	if((cfg_obj_get_object_attr(dnsClientNode, TR181_ENABLE_ATTR, 0, tmp, sizeof(tmp)) < 0)
		|| strcmp(tmp, "0") ==0)
	{
		return 0;
	}
	
	ipIfIdx = getIPIfByTR181Node(TR181_DNS_CLIENT_NODE, dnsClientIdx);	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), TR181_IP_ENTRY_NODE, ipIfIdx);

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(nodeName, LAN_INDEX_ATTR, 0, tmp, sizeof(tmp));
	if(!strcmp(tmp,"0"))
	{
		syncDNSServer2Dhcp6s(dnsClientIdx);
		return 0;
	}

	oldWanIf = getPvcIdxByTR181NodeEntry(TR181_DNS_CLIENT_NODE, dnsClientIdx);	
	newWanIf =getPvcIdxByTR181NodeEntry(TR181_IP_NODE, ipIfIdx);
	if(oldWanIf != newWanIf && newWanIf >= 0)
	{
		/*if new interface is exist , clear old dnsclient_entry pvcIndex*/
		oldtype = getDNSClientType(dnsClientIdx);
		for(i = 1; i <= DNS_CLIENT_NUM; i++)
		{
			memset(dnsClientNode, 0, sizeof(dnsClientNode));
			memset(tmp, 0, sizeof(tmp));
			snprintf(dnsClientNode, sizeof(dnsClientNode), TR181_DNSCLIENT_ENTRY_NODE, i);
			if((cfg_obj_get_object_attr(dnsClientNode, PVC_INDEX_ATTR, 0, tmp, sizeof(tmp)) >= 0)
				&& (newWanIf == atoi(tmp)) && (oldtype = getDNSClientType(i)))
			{
				cfg_obj_set_object_attr(dnsClientNode, PVC_INDEX_ATTR, 0, "-1");
				cfg_obj_set_object_attr(dnsClientNode, TR181_DNSIP_ATTR, 0, "");
				cfg_obj_set_object_attr(dnsClientNode, TR181_INTERFACE_ATTR, 0, "");
			}
		}
	}
	
	if(newWanIf < 0)
	{
		tcdbg_printf("[%s:%d]newWanPvc = %d, error\n",__FUNCTION__,__LINE__, newWanIf); 
		setPvcIdxByTR181NodeEntry(TR181_DNS_CLIENT_NODE, dnsClientIdx, -1);
		return -1;
	}

	ret = setWanNodeByWanIf(wanNode, newWanIf);
	if(ret < 0)
	{		
		tcdbg_printf("[%s:%d]setWanNodeValue error!\n",__FUNCTION__,__LINE__);
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(wanNode, TR181_IPVERSION_ATTR, 0, tmp, sizeof(tmp));

	if(strcmp(tmp, "IPv4") == 0)
	{
		setDNSClientTypeByWanIf(newWanIf, dnsClientIdx, 1);
	}
	else if(strcmp(tmp, "IPv6") == 0)
	{
		setDNSClientTypeByWanIf(newWanIf, dnsClientIdx, 2);
	}
	else
	{
		setDNSClientTypeByWanIf(newWanIf, dnsClientIdx, 0);
	}
	
	setPvcIdxByTR181NodeEntry(TR181_DNS_CLIENT_NODE, dnsClientIdx, newWanIf);
	
	dnsClientEntrySync2WanByWanPvc(dnsClientIdx, newWanIf);	
	
	commitWanByWanIf(newWanIf);

	return 0;
}

int  cfg_type_dnsclient_func_get(char* path,char* attr,char* val,int len)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char wanNodeName[MAXLEN_NODE_NAME] = {0};
	char status[16] = {"Disabled"};/* default value*/
	char active[10] = {0};
	char isp[10] = {0};
	int i = 0, j = 0;

	for(i = 0; i < PVC_NUM; i++) 
	{
		for(j = 0; j < MAX_SMUX_NUM; j++) 
		{
			memset(wanNodeName, 0, sizeof(wanNodeName));
			snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, i + 1, j + 1);
			if(cfg_obj_get_object_attr(wanNodeName, TR181_ACTIVE_ATTR, 0, active, sizeof(active)) >= 0) 
			{
				cfg_obj_get_object_attr(wanNodeName, TR181_ISP_ATTR, 0, isp, sizeof(isp));
				/*isp: 0=WAN_ENCAP_DYN 1=WAN_ENCAP_STATIC 2=WAN_ENCAP_PPP*/ 
				if(!strcmp("0", isp) || !strcmp("1", isp) || !strcmp("2", isp) ) 
				{			
					strncpy(status, "Enabled", sizeof(status) - 1);
					break;
				}
			}
		}
	}	
	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, TR181_DNS_CLIENT_COMMON_NODE, sizeof(nodeName) - 1);
	cfg_obj_set_object_attr(nodeName, TR181_STATUS_ATTR, 0, status);

	return cfg_type_default_func_get(path, attr, val, len);
}

static cfg_node_ops_t cfg_type_dnsclient_entry_ops  = { 
	 .get = cfg_type_dnsclient_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dnsclient_func_commit 
}; 


static cfg_node_type_t cfg_type_dnsclient_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DNS_CLIENT_NUM, 
	 .parent = &cfg_type_dnsclient, 
	 .ops = &cfg_type_dnsclient_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_dnsclient_common_ops  = { 
	 .get = cfg_type_dnsclient_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dnsclient_func_commit 
}; 


static cfg_node_type_t cfg_type_dnsclient_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dnsclient, 
	 .ops = &cfg_type_dnsclient_common_ops, 
}; 


static cfg_node_ops_t cfg_type_dnsclient_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_dnsclient_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dnsclient_func_commit 
}; 


static cfg_node_type_t* cfg_type_dnsclient_child[] = { 
	 &cfg_type_dnsclient_common, 
	 &cfg_type_dnsclient_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dnsclient = { 
	 .name = "DnsClient", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dnsclient_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dnsclient_child, 
	 .ops = &cfg_type_dnsclient_ops, 
}; 
