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
#include <arpa/inet.h>

static char* cfg_type_dhcpv4svrpool_index[] = { 
	 "Entry_Idx", 
	 NULL 
}; 


typedef struct _DhcpdValue_
{
	char enable[8];
	char vendorID[32];
	char minAddress[32];
	char poolcount[8];
}DhcpdValue;

int setTR181DHCPv4SvrPoolValue(int entryIdx ,DhcpdValue Dvalue,char *value)
{	
	char nodeName[MAXLEN_NODE_NAME]={0};
	int etyIdx = -1;
	struct in_addr minAddress;
	struct in_addr maxAddress;
	char maxAddr[32] = {0};
	char subnetMask[32]={0};
	char leasetime[16]={0};
	char svrPolOptNode[MAXLEN_NODE_NAME]={0};
	int etyOptIdx = -1;
	
	cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "subnetMask", 0, subnetMask,sizeof(subnetMask)-1);
	cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "lease", 0, leasetime,sizeof(leasetime)-1);
	
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), TR181_DHCPV4_SVRPOOL_ENTRY_NODE ,entryIdx + 1);

	memset(svrPolOptNode,0,sizeof(svrPolOptNode));
	snprintf(svrPolOptNode,sizeof(svrPolOptNode), TR181_DHCPV4_SVRPOLOPT_ENTRY_NODE ,entryIdx*DHCPV4_SVRPOL_NUM_MAX + 1);

	if(cfg_query_object(nodeName,NULL,NULL) <= 0)
	{		
		etyIdx = cfg_obj_create_object(nodeName);
		if(etyIdx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,nodeName);
			return -1;
		}
	}

	if(cfg_query_object(svrPolOptNode,NULL,NULL) <= 0)
	{		
		etyOptIdx = cfg_obj_create_object(svrPolOptNode);
		if(etyOptIdx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,svrPolOptNode);
			return -1;
		}
	}
		
	if(entryIdx > 0) {
		/*Option*/
		cfg_obj_set_object_attr(nodeName, "Enable" , 0 , !strcmp(Dvalue.enable, "Yes") ? "1" : "0");
		cfg_obj_set_object_attr(nodeName, "VendorClassID" , 0, Dvalue.vendorID);
		cfg_obj_set_object_attr(svrPolOptNode, "Enable", 0, !strcmp(Dvalue.enable, "Yes") ? "1" : "0");
		cfg_obj_set_object_attr(svrPolOptNode, "Tag", 0, "60");
	}else{
		/*dhcpd_common*/
		cfg_obj_set_object_attr(nodeName, "Enable" , 0 , value);
	}
	cfg_obj_set_object_attr(nodeName, "SubnetMask" , 0, subnetMask);
	cfg_obj_set_object_attr(nodeName, "LeaseTime" , 0, leasetime);
	cfg_obj_set_object_attr(nodeName, "MinAddress" , 0, Dvalue.minAddress);
	
	inet_aton(Dvalue.minAddress, &minAddress);
	minAddress.s_addr = ntohl(minAddress.s_addr);
	minAddress.s_addr += (atol(Dvalue.poolcount) - 1);
	maxAddress.s_addr = htonl(minAddress.s_addr);
	snprintf(maxAddr, sizeof(maxAddr), "%s", inet_ntoa(maxAddress));
	cfg_obj_set_object_attr(nodeName, "MaxAddress" , 0, maxAddr);
		
	return 0;
}


int updateDHCPv4SvrPool(char *value)
{
	int num = 0;
	DhcpdValue nodeValue; 
	memset(&nodeValue,0,sizeof(nodeValue));

	/* Common pool*/
	cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "start", 0, nodeValue.minAddress,sizeof(nodeValue.minAddress)-1);
	cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "pool_count", 0, nodeValue.poolcount,sizeof(nodeValue.poolcount)-1);
	setTR181DHCPv4SvrPoolValue(num,nodeValue,value);
	num++;
	
	/*option60 pool Computer*/
	memset(&nodeValue,0,sizeof(nodeValue));
	if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "Active" , 0, nodeValue.enable,sizeof(nodeValue.enable)-1) >= 0
		&& cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorID", 0, nodeValue.vendorID,sizeof(nodeValue.vendorID)-1) >= 0){
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "start", 0, nodeValue.minAddress,sizeof(nodeValue.minAddress)-1);
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_count", 0, nodeValue.poolcount,sizeof(nodeValue.poolcount)-1);
		cfg_obj_set_object_attr(TR181_DHCPV4_SVRPOLOPT_ENTRY_NODE, "Enable", 0, nodeValue.poolcount);
		setTR181DHCPv4SvrPoolValue(num,nodeValue,value);
		num++;
	}
	
	/*option60 pool STB*/	
	memset(&nodeValue,0,sizeof(nodeValue));
	if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "Active" , 0, nodeValue.enable,sizeof(nodeValue.enable)-1) >= 0 
		&& cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDSTB", 0, nodeValue.vendorID,sizeof(nodeValue.vendorID)-1)){
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startSTB", 0, nodeValue.minAddress,sizeof(nodeValue.minAddress)-1);
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countSTB", 0, nodeValue.poolcount,sizeof(nodeValue.poolcount)-1);
		setTR181DHCPv4SvrPoolValue(num,nodeValue,value);
		num++;
	}


	/*option60 pool Phone*/
	memset(&nodeValue,0,sizeof(nodeValue));
	if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "Active" , 0, nodeValue.enable,sizeof(nodeValue.enable)-1) >= 0 
	&& cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDPhone", 0, nodeValue.vendorID,sizeof(nodeValue.vendorID)-1)){
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startPhone", 0, nodeValue.minAddress,sizeof(nodeValue.minAddress)-1);
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countPhone", 0, nodeValue.poolcount,sizeof(nodeValue.poolcount)-1);
		setTR181DHCPv4SvrPoolValue(num,nodeValue,value);
		num++;
	}

	/*option60 pool Camera*/
	memset(&nodeValue,0,sizeof(nodeValue));
	if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "Active" , 0, nodeValue.enable,sizeof(nodeValue.enable)-1) >= 0
		&& cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDCamera", 0, nodeValue.vendorID,sizeof(nodeValue.vendorID)-1)){
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startCamera", 0, nodeValue.minAddress,sizeof(nodeValue.minAddress)-1);
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countCamera", 0, nodeValue.poolcount,sizeof(nodeValue.poolcount)-1);
		setTR181DHCPv4SvrPoolValue(num,nodeValue,value);
		num++;
	}
	/*option60 pool HGW*/
	memset(&nodeValue,0,sizeof(nodeValue));
	if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "Active" , 0, nodeValue.enable,sizeof(nodeValue.enable)-1) >= 0
		&& cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDHGW", 0, nodeValue.vendorID,sizeof(nodeValue.vendorID)-1)){
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startHGW", 0, nodeValue.minAddress,sizeof(nodeValue.minAddress)-1);
		cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countHGW", 0, nodeValue.poolcount,sizeof(nodeValue.poolcount)-1);
		setTR181DHCPv4SvrPoolValue(num,nodeValue,value);
		num++;
	}
	return 0;
}

int cfg_type_dhcpv4svrpool_func_commit(char *path)
{
	char tmp[64] = {0};
	char tr181_dhcpv4sp[MAXLEN_NODE_NAME]={0};	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	int etyIdx=0;
	char buf[8] = {0};
	char lanDhcpType[8] = {0};
	struct in_addr maxAddress;
	struct in_addr minAddress;
	char poolCount[8] = {0};
	int  iPoolCnt = 0;
	char minAddr[32]={0};
	char maxAddr[32]={0};
	char leasetime[8]={0};
	char enable[4] = {0};

	if(get_entry_number_cfg2(path, "entry.", &etyIdx) != 0)
	{
		snprintf(nodeName, sizeof(nodeName), WEBCURSET_ENTRY_NODE);
		memset(tmp,0,sizeof(tmp));
		if(cfg_obj_get_object_attr(nodeName, "Entry_Idx", 0, tmp, sizeof(tmp)) >= 0){
		etyIdx = atoi(tmp) + 1;
		}else{
			return -1;
		}
	}
	
	memset(tmp, 0, sizeof(tmp));
	memset(tr181_dhcpv4sp,0,sizeof(tr181_dhcpv4sp));
	snprintf(tr181_dhcpv4sp,sizeof(tr181_dhcpv4sp), TR181_DHCPV4_SVRPOOL_ENTRY_NODE , etyIdx);

	cfg_obj_get_object_attr(tr181_dhcpv4sp, "SubnetMask", 0, tmp,sizeof(tmp));
	cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "subnetMask", 0, tmp);
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(tr181_dhcpv4sp, "Enable" ,0 , enable,sizeof(enable));
	
	cfg_obj_get_object_attr(tr181_dhcpv4sp, "LeaseTime", 0, leasetime,sizeof(leasetime));
	cfg_obj_set_object_attr(DHCPD_COMMON_NODE, "lease", 0,leasetime);
	cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "lease", 0,leasetime);
	
	cfg_obj_get_object_attr(tr181_dhcpv4sp, "VendorClassID", 0, tmp,sizeof(tmp));
	cfg_obj_get_object_attr(tr181_dhcpv4sp, "MinAddress", 0, minAddr,sizeof(minAddr));
	cfg_obj_get_object_attr(tr181_dhcpv4sp, "MaxAddress", 0, maxAddr,sizeof(maxAddr));
	
	inet_aton(minAddr, &minAddress);
	inet_aton(maxAddr, &maxAddress);
	
	iPoolCnt = ntohl(maxAddress.s_addr) - ntohl(minAddress.s_addr) + 1;
	if(iPoolCnt > 0)	
		snprintf(poolCount, sizeof(poolCount), "%d", iPoolCnt);
	else 
		snprintf(poolCount, sizeof(poolCount), "%d", 0);

	if(etyIdx == 1)
	{	/*Dhcpd_common*/
		cfg_obj_get_object_attr(LAN_DHCP_NODE, "type" ,0, lanDhcpType,sizeof(lanDhcpType));
		if(!strcmp(enable, "1")){
			cfg_obj_set_object_attr(LAN_DHCP_NODE, "type", 0,"1");
		}else{
			if(strcmp(lanDhcpType, "2") != 0){
				/*Dhcp relay, no need to disable*/
				cfg_obj_set_object_attr(LAN_DHCP_NODE, "type", 0,"0");
			}
		}
		cfg_obj_set_object_attr(DHCPD_COMMON_NODE, "start", 0,minAddr);
		cfg_obj_set_object_attr(DHCPD_COMMON_NODE, "pool_count", 0,poolCount);
		cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "start", 0,minAddr);
		cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "pool_count", 0,poolCount);
	}
	else
	{	/*Dhcpd_Option60*/
		cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "Active", 0, !strcmp(enable, "1") ? "Yes" : "No");	

		if(strstr(tmp,"MSFT"))
		{
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "VendorID", 0,tmp);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "start", 0,minAddr);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "pool_count", 0,poolCount);
		}else if(strstr(tmp,"STB")){
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "VendorIDSTB", 0,tmp);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "startSTB", 0,minAddr);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "pool_countSTB", 0,poolCount);
		}else if(strstr(tmp,"Phone")){
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "VendorIDPhone", 0,tmp);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "startPhone", 0,minAddr);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "pool_countPhone", 0,poolCount);
		}else if(strstr(tmp,"Camera")){
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "VendorIDCamera", 0,tmp);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "startCamera", 0,minAddr);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "pool_countCamera", 0,poolCount);
		}else if(strstr(tmp,"HGW")){
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "VendorIDHGW", 0,tmp);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "startHGW", 0,minAddr);
			cfg_obj_set_object_attr(DHCPD_OPTION60_NODE, "pool_countHGW", 0,poolCount);
		}
	}
	
	cfg_tr181_commit_object("root.lan");
	cfg_tr181_commit_object(DHCPD_NODE);
	return 0; 

}


static cfg_node_ops_t cfg_type_dhcpv4svrpool_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4svrpool_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4svrpool_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | DHCPV4_SVRPOL_NUM_MAX, 
	 .parent = &cfg_type_dhcpv4svrpool, 
	 .index = cfg_type_dhcpv4svrpool_index, 
	 .ops = &cfg_type_dhcpv4svrpool_entry_ops, 
};

static cfg_node_ops_t cfg_type_dhcpv4svrpool_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcpv4svrpool_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_dhcpv4svrpool, 
	 .ops = &cfg_type_dhcpv4svrpool_common_ops, 
};


static cfg_node_ops_t cfg_type_dhcpv4svrpool_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dhcpv4svrpool_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcpv4svrpool_child[] = { 
	 &cfg_type_dhcpv4svrpool_entry, 
	 &cfg_type_dhcpv4svrpool_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcpv4svrpool= { 
	 .name = "DHCPv4SP", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcpv4svrpool_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcpv4svrpool_child, 
	 .ops = &cfg_type_dhcpv4svrpool_ops, 
}; 
