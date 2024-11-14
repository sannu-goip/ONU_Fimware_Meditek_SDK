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
#include <time.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 
#include <unistd.h>
#include <arpa/inet.h>
#if defined(TCSUPPORT_CHARSET_CHANGE)
#include <ecnt_utility.h>
#endif



#define MAXSIZE	160
#define MAX_LANHOST_NUM	254

#define DHCPLEASE_PATH "/etc/udhcp_lease"
#define RADVDCONF_PATH  "/etc/radvd.conf"

#define LAN_IP	"IP"
#define LAN_MASK	"netmask"
#if defined(TCSUPPORT_CT_PMINFORM)
#define ETHDEVMAC_ATTR		"Eth%dDevMAC"
#define WLANDEVMAC_ATTR		"WlanDevMAC"
#endif

#define LANHOST			"LanHost"
#define LEASENUM_ATTR	"LeaseNum"
#if defined(TCSUPPORT_CT_STB_TEST)
#define STBMAC_ATTR		"StbMAC"
#endif

#define ARP_INFO_PATH		"/tmp/arp_info"
#define HOST_MODE_DHCP	0
#define HOST_MODE_STATIC	1
#define HOST_MODE_AUTOIP	1
#define LANHOST_GET_INTERVAL	3	/*seconds*/

#define WIFIMACTABLE	"wifiMacTab"
#define WIFIMACTABLE_NUM	"NUM"
#define WIFIMACTABLE_IP		"IP"
#define WIFIMACTABLE_MAC	"MAC"

#define BR_FDB_PATH "/proc/br_fdb_host/stb_list"

#define isspace(c)	(c == ' ' || c == '\t' || c == 10 || c == 13 || c == 0)


static char* cfg_type_lanhost_index[] = { 
	 NULL 
}; 


void trimIP(char* str)
{
	char* c = str + strlen(str) - 1;
	while(isspace(*c) && c>str)
	{
		*c ='\0';
		c--;
	}
}/*end trimIP*/


int check_arpIP_mode(char *strIP, char *strMAC, FILE *fp, char* getteddexpire, char* gettedhostname) 
{
	char buf[MAXSIZE]={0},mac[17]={0},ip[40]={0},expire[10]={0},hostname[32]={0};
#if defined(TCSUPPORT_CMCCV2)
	char devicetype[20] = {0};
	char optionvalue[128] = {0};
#endif
	
	if(fp == NULL)
		return HOST_MODE_STATIC;/*no dhcp*/

	fseek(fp, 0, 0);/*seek back to the file beginning. 0:SEEK_SET*/
	
	while (fgets(buf, MAXSIZE, fp))
	{
		memset(mac, 0, sizeof(mac));
		memset(ip, 0, sizeof(ip));
		memset(expire, 0, sizeof(expire));
		memset(hostname, 0, sizeof(hostname));
#if defined(TCSUPPORT_CMCCV2)
		memset(devicetype, 0, sizeof(devicetype));
		memset(optionvalue, 0, sizeof(optionvalue));
		sscanf(buf, "%s %s %s %s %s %s", mac, ip, expire, hostname, devicetype, optionvalue);
#else
		sscanf(buf, "%s %s %s %s",mac, ip, expire, hostname);
#endif
		trimIP(strIP);
		trimIP(ip);
		trimIP(strMAC);
		trimIP(mac);
		if((!strcmp(strIP,ip)) || (! strcasecmp(strMAC, mac))) {
			printf("\ncheck_arpIP_mode, same ip %s ,%s in dhcp lease\n", strIP, strMAC);	
			strncpy(getteddexpire,expire,9);
			strncpy(gettedhostname,hostname,31);
			return HOST_MODE_DHCP;
		}

		memset(buf, 0, sizeof(buf));
	}

	return HOST_MODE_STATIC;

}



struct timespec gLastGetTime;
int gTheFirstGet = 0;

static int cfg_type_lanhost_entry_func_read(char* path, char* attr, char* value, int len){
	FILE *fp = NULL;
	FILE *fp_dhcplease = NULL;
	int lanHostOid = 0;
	char lanHostGroupPath[64] = {0}, lanHostNodePath[64] = {0};
	int i, total = 0, skipOnce = 0;	
	char buf[MAXSIZE]={0};
	char subNode[64]={0}, count[3]={0};
#if defined(TCSUPPORT_CT_PMINFORM)
	char EthMacAttr[16]={0};
	char WlanDevMAC[512]={0};
#endif
	char cmd[64]={0};
	char ip[40]={0},lladdr[15]={0},mac[20]={0},stale[15]={0};
	char nodeName[64];
	char mask[64] = {0};
	char instance[128] = {0};
	char mac_fdb[32] = {0};
	char ip_gateway[40]={0};
	int id = 0;
#if defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON)
	int rt = -1;
	int res = 0;
	int dhcpLeaseNum[8] = {0};
	int val[12] = {0};
	char WlanDevNum[8] ={0}, NUM[8]={0};
	char mactab[18]={0};
	char *pstr = NULL;
	int str_len = 0;
#endif
	FILE *fp_br_fdb = NULL;
#ifdef TCSUPPORT_WLAN	
	int ssidNum = 0, ssidClientNum = 0;
	char attrValue[64] = {0};
	int ssidNum5g =0;
	int k = 0;
#endif	
#if defined(TCSUPPORT_WLAN) || (defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON))
    int j = 0;
#endif
	struct timespec cur;
	
	int timeLeft, days = 0,hours = 0, minutes = 0, seconds = 0;
	time_t curTime;
	char expire[10]={0}, hostname[32]={0};
	int ret = 0;

	int result = -1;
	int entrynum = -1;
	int maxEntryNum[MAX_LANHOST_NUM] = {0};

#if defined(TCSUPPORT_CHARSET_CHANGE)
	char utfValue[64] = {0};
	int inlen = 0;
	int outlen = 64;
#endif


	enum lanHost_en{
			dhcp_MAC=0,
			dhcp_IP,
			dhcp_HostName,
			dhcp_ExpireDay,
			dhcp_ExpireTime,
			host_AddressSrc,
			host_IfType,
			host_Active,
			host_Layer2Interface,
			dhcp_Max,
	};

	char lanHost_attr[][16] =
	{
		{"MAC"},
		{"IP"},
		{"HostName"},
		{"ExpireDay"},
		{"ExpireTime"},
		{"AddressSrc"},
		{"InterfaceType"},
		{"Active"},
		{"Layer2Interface"},
		{""},
	};
			
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, LAN_ENTRY_NODE, sizeof(nodeName)-1);

	if((cfg_obj_get_object_attr(nodeName, LAN_IP, 0, ip_gateway, sizeof(ip_gateway)) < 0) || 
		(cfg_obj_get_object_attr(nodeName, LAN_MASK, 0, mask, sizeof(mask)) < 0))
	{
		return -1;
	}
			
	/*Add a time check*/
	if(gTheFirstGet == 0) {
		clock_gettime(CLOCK_MONOTONIC,&gLastGetTime);
		gTheFirstGet = 1;
	}
	else {
		clock_gettime(CLOCK_MONOTONIC,&cur);
		if(cur.tv_sec - gLastGetTime.tv_sec < LANHOST_GET_INTERVAL) {
			return 0;
		}
		else {
			gLastGetTime.tv_sec = cur.tv_sec;
			gLastGetTime.tv_nsec = cur.tv_nsec;
		}
	}
	
	curTime = gLastGetTime.tv_sec;

	strncpy(lanHostGroupPath, LANHOST_NODE, sizeof(lanHostGroupPath)-1);
	if(cfg_query_object(lanHostGroupPath,NULL,NULL) <= 0){		
		lanHostOid = cfg_obj_create_object(lanHostGroupPath);
		if(lanHostOid < 0) {
			return -1;
	    }
	}

	/*delete all Entry subnode*/
	entrynum = cfg_obj_query_object(lanHostGroupPath,"entry",maxEntryNum);
	for(i=1;i<=entrynum;i++){
		snprintf(subNode, sizeof(subNode), LANHOST_ENTRY_NODE, i);
		result = cfg_obj_delete_object(subNode);
		if(result < 0)
		{
			if(skipOnce==0){
				/*skip once because there may be a null node in the LanHost node*/
				skipOnce++;
			}
			else{
				break;
			}
		}
	}
#if defined(TCSUPPORT_CT_PMINFORM)
	for(i=1;i<5;i++){
		snprintf(EthMacAttr, sizeof(EthMacAttr), ETHDEVMAC_ATTR, i);
		cfg_set_object_attr(lanHostGroupPath, EthMacAttr, "");		
	}
	cfg_set_object_attr(lanHostGroupPath, WLANDEVMAC_ATTR, "");
#endif
#if defined(TCSUPPORT_CT_STB_TEST)
	cfg_set_object_attr(lanHostGroupPath, STBMAC_ATTR, "");
#endif

	/*2. Extract static host info from arp table*/
	snprintf(cmd, sizeof(cmd),"/usr/bin/ip neigh show dev br0 > %s", ARP_INFO_PATH);
	system(cmd);
	
	fp=fopen(ARP_INFO_PATH, "r");	
	if(fp == NULL){
		goto setLeaseNumAttr;
	}

	fp_dhcplease=fopen(DHCPLEASE_PATH, "r");	
	if(fp_dhcplease == NULL){
		printf("\nlanHost_read:open %s  failed!\n", DHCPLEASE_PATH);
	}

#if defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON)
	memset(buf,0,sizeof(buf));
	rt = blapi_traffic_check_eth_port_status(buf, val);
	if(rt == -1){
		printf("\nlanHost_read:open eth_port_status failed!\n");
		fclose(fp);
		fclose(fp_dhcplease);
		return -1;
	}

	
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, WIFIMACTAB_COMMON_NODE, sizeof(nodeName)-1);
	for(i=0; i<8; i++){
		snprintf(WlanDevNum, sizeof(WlanDevNum),"NUM%d", i);
		ret = cfg_obj_get_object_attr(nodeName, WlanDevNum, 0, NUM, sizeof(NUM));
		if(0 < ret)
			dhcpLeaseNum[i] = atoi(NUM);
		memset(NUM,0,sizeof(NUM));
	}
#endif
	memset(buf,0,sizeof(buf));
	while (fgets(buf, MAXSIZE, fp))
	{
		/*get ip and mac info from file*/
		ret = sscanf(buf, "%s %s %s %s",ip,lladdr,mac,stale);
#if defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON)
		res = 0;
		for( i = 0; i < 8; i++){
			for( j = 0; j < dhcpLeaseNum[i]; j++){
				memset(nodeName,0,sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, i+1, j+1);
				cfg_obj_get_object_attr(nodeName, WIFIMACTABLE_MAC, 0, mactab, sizeof(mactab));
				if(0 == strcasecmp(mactab,mac)){
					res = 1;
					break;
				}
			}
			if(1 == res)
				break;
		}
		if(0 == res){
			fp_br_fdb = fopen(BR_FDB_PATH, "r");
			if(fp_br_fdb == NULL){
				printf("\nlanHost_read:open %s failed!\n", BR_FDB_PATH);
				fclose(fp);
				fclose(fp_dhcplease);
				return -1;
			}

			while ( -1 != getline(&pstr, &str_len, fp_br_fdb) )
			{
				sscanf(pstr, "%d=%s", &id, mac_fdb);
				if( 1 == val[id-1] && 0 == strcasecmp(mac_fdb,mac) && id < 5 )
					res = 1;
			}
			if ( pstr )
				free(pstr);
			fclose(fp_br_fdb);
		}
#endif

		if(
#if defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON)
		(res == 1) &&
#endif
			(ret == 4) && (inet_addr(ip) != INADDR_NONE) 
			&& (((inet_addr(ip))&(inet_addr(mask))) == ((inet_addr(ip_gateway))&(inet_addr(mask)))))
		{
				snprintf(lanHostNodePath, sizeof(lanHostNodePath), LANHOST_ENTRY_NODE, total+1);/*Buf: Entryi*/
				lanHostOid = cfg_obj_create_object(lanHostNodePath);/*Create LanHost Entry*/
				if(lanHostOid < 0) {
					printf("\nlanHost_read: new a static enntry %d failed!\n", total);	
					break;
				}
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_MAC], mac);
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_IP], ip);
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_HostName], " ");	
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_ExpireDay], "0");	
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_ExpireTime], "0");	
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_AddressSrc], "Static");
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_IfType], "Ethernet");
				cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_Active], "1");
				/*2. check this node whether DHCP clinet*/
				memset(hostname, 0, sizeof(hostname));
				if ((fp_dhcplease != NULL) && check_arpIP_mode(ip, mac, fp_dhcplease, expire, hostname) == HOST_MODE_DHCP)
				{
					timeLeft = atoi(expire) - curTime;

					days = timeLeft / (60*60*24);
					timeLeft %= 60*60*24;
					hours = timeLeft / (60*60);
					timeLeft %= 60*60;
					minutes = timeLeft / 60;
					seconds = timeLeft % 60;
#if defined(TCSUPPORT_CHARSET_CHANGE)
					inlen = strlen(hostname);
					memset(utfValue, 0, sizeof(utfValue));
					if ( 0 == charsetconv("gb2312", "utf-8", &inlen, &outlen, hostname, utfValue) )
						cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_HostName], utfValue);
					else
						cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_HostName], "unkown");
#else
					cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_HostName], hostname);
#endif
					snprintf(expire, sizeof(expire),"%d",days);
					cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_ExpireDay], expire);	

					snprintf(expire, sizeof(expire),"%d:%d:%d",hours,minutes,seconds);
					cfg_set_object_attr(lanHostNodePath, lanHost_attr[dhcp_ExpireTime], expire);					
					cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_AddressSrc], "DHCP");
				}

				fp_br_fdb = fopen(BR_FDB_PATH, "r");
				if(fp_br_fdb == NULL){
					printf("\nlanHost_read:open %s failed!\n", BR_FDB_PATH);
				}
				
				if(fp_br_fdb != NULL)
				{
					memset(buf, 0, sizeof(buf));
					while (fgets(buf, MAXSIZE, fp_br_fdb))
					{
						sscanf(buf, "%d=%s", &id, mac_fdb);

						if(0 == strcasecmp(mac,mac_fdb))
						{
							total++;
							snprintf(instance, sizeof(instance),"InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.%d",id);
							cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_Layer2Interface], instance);
#if defined(TCSUPPORT_CT_PMINFORM)
							snprintf(EthMacAttr, sizeof(EthMacAttr), ETHDEVMAC_ATTR, id);
							cfg_set_object_attr(lanHostGroupPath, EthMacAttr, mac);
#endif
#if defined(TCSUPPORT_CT_STB_TEST)
							if(2==id)
								cfg_set_object_attr(lanHostGroupPath, STBMAC_ATTR, mac);
#endif
							break;
						}
					}

					fclose(fp_br_fdb);
				}	
			}
		memset(buf, 0, sizeof(buf));
	}
	fclose(fp);
	unlink(ARP_INFO_PATH);

	if(fp_dhcplease !=  NULL)
		fclose(fp_dhcplease);
	
setLeaseNumAttr:
	snprintf(count, sizeof(count),"%d",total);
	cfg_set_object_attr(lanHostGroupPath, LEASENUM_ATTR, count);

#ifdef TCSUPPORT_WLAN
	/*3. Check the interface type for each host by look up wifi mac table*/
	for(i=1; i<=total; i++) {
		snprintf(lanHostNodePath, sizeof(lanHostNodePath), LANHOST_ENTRY_NODE, i);/*Buf: Entryi*/
		if(cfg_query_object(lanHostNodePath,NULL,NULL) > 0){
			memset(mac, 0, sizeof(mac));
			if(cfg_obj_get_object_attr(lanHostNodePath, lanHost_attr[dhcp_MAC], 0, attrValue, sizeof(attrValue)) > 0)
				memcpy(mac, attrValue, sizeof(mac));
			else
				printf("\nlanHost_read: check entry %d interface type, get mac failed!\n", i);
	
			/*look for wifi mac table to check if it is 802.11*/
			/*Do tcapi_get to update WLan node*/			
			if(cfg_obj_get_object_attr(WLAN_COMMON_NODE, "BssidNum", 0, attrValue, sizeof(attrValue)) > 0)
	   			ssidNum = atoi(attrValue);	   	
			else
 				ssidNum = 0;

#if defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON)
			if(cfg_obj_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", 0, attrValue, sizeof(attrValue)) > 0)
	   			ssidNum5g = atoi(attrValue);	   	
			else
 				ssidNum5g = 0;

			res = 0;
#endif
			for(j=0; j<ssidNum + ssidNum5g; j++){
				/*Do tcapi_get to update wifiMacTab node*/
#if defined(TCSUPPORT_CMCCV2) && defined(TCSUPPORT_CT_PON)
				if( j >= ssidNum && 0 == res ){
					j = 4;
					res = 1;
				}
#endif
				snprintf(subNode, sizeof(subNode), "NUM%d", j);
				if(cfg_obj_get_object_attr(WIFIMACTAB_COMMON_NODE, subNode, 0, attrValue, sizeof(attrValue)) > 0)
	   				ssidClientNum = atoi(attrValue);	   	
				else
 					ssidClientNum = 0;
	
				if(0 != ssidClientNum){
					for(k=1; k<=ssidClientNum; k++){
#if defined(TCSUPPORT_CT_PON)
						snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, j+1, k);
						strncpy(subNode, "MAC", sizeof(subNode)-1);
#else
						snprintf(subNode, sizeof(subNode), WIFIMACTAB_ENTRY_NODE, k);
						snprintf(subNode, sizeof(subNode),"MAC%d", j);
#endif
						if(cfg_obj_get_object_attr(nodeName, subNode, 0, attrValue, sizeof(attrValue)) < 0)
	   						break;

						if(0 == strcasecmp(mac, attrValue)) {
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
							cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_IfType], "WLAN");
#else
							cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_IfType], "802.11");
#endif
	                                                snprintf(instance, sizeof(instance),"InternetGatewayDevice.LANDevice.1.WLANConfiguration.%d",j+1);
							cfg_set_object_attr(lanHostNodePath, lanHost_attr[host_Layer2Interface], instance);
#if defined(TCSUPPORT_CT_PMINFORM)
							if(strlen(WlanDevMAC)>0)
								strcat(WlanDevMAC,"-");
							strcat(WlanDevMAC,mac);
#endif
							j = ssidNum+ssidNum5g; /*break the upper loop*/
							break;
						}
					}
				}
			}
		}
	}
#if defined(TCSUPPORT_CT_PMINFORM)
	cfg_set_object_attr(lanHostGroupPath, WLANDEVMAC_ATTR, WlanDevMAC);
#endif
#endif

	return 0;
}


static int cfg_type_lanhost_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_lanhost_entry_func_read(path,attr,val,len);
	cfg_type_default_func_get(path,attr,val,len);
	return 0;
} 


static cfg_node_ops_t cfg_type_lanhost_entry_ops  = { 
	 .get = cfg_type_lanhost_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_lanhost_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 254, 
	 .parent = &cfg_type_lanhost, 
	 .index = cfg_type_lanhost_index, 
	 .ops = &cfg_type_lanhost_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_lanhost_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_lanhost_entry_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_lanhost_child[] = { 
	 &cfg_type_lanhost_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_lanhost = { 
	 .name = "LanHost", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_lanhost_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_lanhost_child, 
	 .ops = &cfg_type_lanhost_ops, 
}; 
