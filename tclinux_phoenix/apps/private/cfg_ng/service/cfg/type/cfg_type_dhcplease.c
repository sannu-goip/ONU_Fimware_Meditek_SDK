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


#define MAX_LEASE_NUM 254
#define DHCPLEASE_PATH "/etc/udhcp_lease"
#define RADVDCONF_PATH  "/etc/radvd.conf"

#define MAXSIZE	160


static char* cfg_type_dhcplease_index[] = { 
	 NULL 
}; 


static int cfg_type_dhcplease_entry_func_read(char* path,char* attr, char* val,int len) 
{
	FILE *fp = NULL;
	char groupPath[32] = {0};
	int i,skipOnce = 0, total = 0;
	char buf[MAXSIZE]={0},mac[17], ip[16],expire[10], hostname[32];
	char subNode[64]={0}, count[3]={0};
	time_t curTime;
	struct  timespec  curtime;
	int timeLeft, days = 0,hours = 0, minutes = 0, seconds = 0;
	clock_gettime(CLOCK_MONOTONIC, &curtime);
	curTime = curtime.tv_sec;
	int result = -1;
	int entrynum = -1;
	int maxEntryNum[MAX_LEASE_NUM] = {0};
#if defined(TCSUPPORT_CMCCV2)
	char devicetype[20] = {0};
	char optionvalue[128] = {0};
#endif	
	
	strncpy(groupPath, DHCPLEASE_NODE, sizeof(groupPath)-1);
	if(cfg_query_object(groupPath, NULL,NULL) <= 0){
		cfg_obj_create_object(groupPath);
	}
	
	/*delete all Entry subnode*/
	entrynum = cfg_obj_query_object(groupPath,"entry",maxEntryNum);
	for(i=1;i<=entrynum;i++){
		snprintf(subNode, sizeof(subNode), DHCPLEASE_ENTRY_NODE, i);
		result = cfg_obj_delete_object(subNode);
		if(result < 0)
		{
			if(skipOnce==0){
				/*skip once because there may be a null node in the Dhcpd node*/
				skipOnce++;
			}
			else{
				break;
			}
		}
	}
	
	fp=fopen(DHCPLEASE_PATH, "r");
	
	if(fp == NULL){
		snprintf(count, sizeof(count),"%d",total);
		cfg_set_object_attr(groupPath, "LeaseNum", count);
		return -1;
	}
	/*Add lease item node from udhcp_lease*/
	while (fgets(buf, MAXSIZE, fp)){
		snprintf(subNode,sizeof(subNode),DHCPLEASE_ENTRY_NODE, total+1);
#if defined(TCSUPPORT_CMCCV2)
		sscanf(buf, "%s %s %s %s %s %s",mac, ip, expire, hostname, devicetype, optionvalue);
#else
		sscanf(buf, "%s %s %s %s",mac, ip, expire, hostname);
#endif
		timeLeft = atoi(expire) - curTime;
		if(strcmp(mac, "00:00:00:00:00:00") && (timeLeft > 0)){
			days = timeLeft / (60*60*24);
			timeLeft %= 60*60*24;
			hours = timeLeft / (60*60);
			timeLeft %= 60*60;
			minutes = timeLeft / 60;
			seconds = timeLeft % 60;
			cfg_obj_create_object(subNode);
			cfg_set_object_attr(subNode, "MAC", mac);
			cfg_set_object_attr(subNode, "IP", ip);
			cfg_set_object_attr(subNode, "HostName", hostname);	
			snprintf(expire,sizeof(expire),"%d",days);
			cfg_set_object_attr(subNode, "ExpireDay", expire);	
#if defined(TCSUPPORT_CMCCV2)
			cfg_set_object_attr(subNode, "DeviceType", devicetype);
			cfg_set_object_attr(subNode, "OptionValue", optionvalue);
#endif		
			snprintf(expire,sizeof(expire),"%d:%d:%d",hours,minutes,seconds);
			cfg_set_object_attr(subNode, "ExpireTime", expire);	

			total++;
		}else
			continue;	
		memset(buf, 0, sizeof(buf));
	}
	fclose(fp);

	snprintf(count,sizeof(count),"%d",total);
	cfg_set_object_attr(groupPath, "LeaseNum", count);

	return 0;
}


static int cfg_type_dhcplease_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dhcplease_entry_func_read(path,attr,val,len);
	return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_dhcplease_entry_ops  = { 
	 .get = cfg_type_dhcplease_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_dhcplease_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 254, 
	 .parent = &cfg_type_dhcplease, 
	 .index = cfg_type_dhcplease_index, 
	 .ops = &cfg_type_dhcplease_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_dhcplease_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_dhcplease_entry_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_dhcplease_child[] = { 
	 &cfg_type_dhcplease_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dhcplease = { 
	 .name = "DhcpLease", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dhcplease_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dhcplease_child, 
	 .ops = &cfg_type_dhcplease_ops, 
}; 
