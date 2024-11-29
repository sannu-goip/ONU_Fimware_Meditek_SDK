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
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include <arpa/inet.h>
#include "libcompileoption.h"


static char* cfg_type_laninfo_index[] = { 
	 NULL 
}; 

#if 0
#define MAX_DEVNUM_RULE 64
#define MAX_BUF_SIZE	2048


#define IFINFO_ETHCMD_PATH	"/tmp/ifinfo_ethcmd_stats"


char laninfo_attr[][20]=
{
	{"ipflag"},
	{"portflag"},
	{"devtypeflag"},
	{"conntypeflag"},
	{""},
};


int cfg_checkCommitNeedAsync(){
	char strNeedAsync[10] = {'\0'};
	char nodeName[64];
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, "root.sys.entry", sizeof(nodeName)-1);
		
	if(cfg_obj_get_object_attr(nodeName, "noNeedAsync", 0, strNeedAsync, sizeof(strNeedAsync)) > 0){
		/*NeedAsync is exist*/
		if(strcmp(strNeedAsync,"1"))
			return 1;
		else
			return 0;
	}else{
		return 1;
	}
}

void  cfg_GetMacAddr(char *macstr, unsigned char *macnum)
{
	int temp[6];
    int i;
        
	sscanf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x", &temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
	for(i=0;i<6;i++){
        macnum[i] = (unsigned char)temp[i];
	}

}


int cfg_getPonLanStatus2(int portStatus[])
{
	char line[160] = {0};
	FILE* fp = NULL;

	fp = fopen(ETH_PORT_STATUS_FILE_PATH, "r");
	if( NULL == fp )
		return -1;

	if( fgets(line, sizeof(line),fp ) )
	{
		if(sscanf(line,"%d %d %d %d", &portStatus[0], &portStatus[1],
				&portStatus[2],&portStatus[3] )!=4)
		{
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	return 0;
}



int cfg_get_devinfo(char * macstr ,char *buf,int flag)
{
	FILE *fp = NULL;
	unsigned char macnum[6] = {0};
	char tempbuf[512] = {0},port[4] = {0};
	char mac[17] = {0}, ip[16] = {0},expire[10] = {0}, hostname[32] = {0}; 
	
#if defined(TCSUPPORT_CT_JOYME)
	int lanStatus[4] = {0}, find = 0;
#endif
	
	if(macstr == NULL || buf == NULL)
		return -1;
	if(flag == 0){
		char *path = "etc/udhcp_lease";
		fp = fopen(path,"r");
		if(fp == NULL)
			return -1;

		while(feof(fp) == 0 ){
			fgets(tempbuf,sizeof(tempbuf),fp);
			sscanf(tempbuf, "%s %s %s %s",mac, ip, expire, hostname);
			cfg_GetMacAddr(mac,macnum);
			snprintf(mac,sizeof(mac),"%02x:%02x:%02x:%02x:%02x:%02x",macnum[0],
				macnum[1],macnum[2],macnum[3],macnum[4],macnum[5]);
			if(strcmp(macstr,mac) == 0)
				strncpy(buf,hostname,31);
		}
		fclose(fp);
	}
	else if(flag == 1){
		char *path = "proc/br_fdb_host/stb_list";
		fp = fopen(path,"r");
		if(fp == NULL)
			return -1;
		while(feof(fp) == 0 ){
			fgets(tempbuf,sizeof(tempbuf),fp);
			sscanf(tempbuf, "%[^=]",port);
			sscanf(tempbuf, "%*[^=]=%s",mac);
			if(strcmp(macstr,mac) == 0) {
				strncpy(buf,port,31);
#if defined(TCSUPPORT_CT_JOYME)
				if (atoi(port) >= 1 && atoi(port) <= 4) {
					cfg_getPonLanStatus2(lanStatus);
					if (lanStatus[atoi(port)-1] == 0) { 
						fclose(fp);
						/* this lan is disconnect */
						return 2;
								 }
								 }
				find = 1;
				break;
#endif
						 }
		}
		fclose(fp);
	}
#if defined(TCSUPPORT_CT_JOYME)
	if (find == 0)
		return 2;
#endif
	return 0;
}


static int cfg_type_laninfo_func_get(char* path,char* attr, char* val,int len)
{
	char NodeName[64]={0};
	int i=0,ret,devnum = 0,j = 0,k=0;
	char cmd[64]={0};
	char ip[40]={0},lladdr[15]={0},mac[20]={0},stale[15]={0};
	char buftemp[32] = {0};
	char buf[MAX_BUF_SIZE]={0};
	int isintable = 0,entry_index[64] = {0};
	char *pBuf[MAX_DEVNUM_RULE];
	char *pBuf1[MAX_DEVNUM_RULE];

#if defined(TCSUPPORT_CT_PON)
	if(cfg_checkCommitNeedAsync())
	{
		return 0;
	}
#endif
	
#if defined(TCSUPPORT_CT_JOYME)
	bzero(NodeName, sizeof(NodeName));
	strncpy(NodeName, "root.laninfo.common", sizeof(NodeName)-1);
	/* for read attributes fastly. */
	if ( cfg_obj_get_object_attr(NodeName, "NoRead", 0, buftemp, sizeof(buftemp)) > 0
		&&	0 == strcmp(buftemp, "Yes") )
	{
		return 0;
	}
	bzero(NodeName, sizeof(NodeName));
	bzero(buftemp, sizeof(buftemp));
#endif

	for(i=0; i<MAX_DEVNUM_RULE; i++){
		pBuf[i] = NULL;
		pBuf1[i] = NULL;
		pBuf[i] = (char *)malloc(20);
		pBuf1[i] = (char *)malloc(20);
		if(NULL == pBuf[i] || NULL == pBuf1[i] )
			goto FreeBUF;
		memset(pBuf[i], 0, 20);
		memset(pBuf1[i], 0, 20);
	}
	memset(NodeName, 0, sizeof(NodeName));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip neigh show dev br0 > %s", IFINFO_ETHCMD_PATH);
	system(cmd);

	FILE *fp = NULL;
	fp=fopen(IFINFO_ETHCMD_PATH, "r");
	if(fp == NULL)
		goto FreeBUF;

	while (fgets(buf,MAX_BUF_SIZE,fp))
	{
		ret = sscanf(buf, "%s %s %s %s",ip,lladdr,mac,stale);
#if defined(TCSUPPORT_CT_JOYME)
		if(cfg_get_devinfo(mac,buftemp,1) != 0) {
			continue;
		}
#endif
		if((ret == 4) && (inet_addr(ip) != INADDR_NONE))
		{
			snprintf(pBuf[devnum],20,"%s",mac);
			isintable = 0;
			k = 0;
			for(i=1;i<=MAX_DEVNUM_RULE;i++)
			{
				snprintf(NodeName, sizeof(NodeName), "root.laninfo.entry.%d", i);
				memset(buftemp, 0, sizeof(buftemp));
				if(cfg_obj_get_object_attr(NodeName,"Active",0,buftemp,sizeof(buftemp)) > 0 &&
					!strcmp(buftemp,"1"))
				{
					memset(buftemp, 0, sizeof(buftemp));
					if(cfg_obj_get_object_attr(NodeName,"MacAddr",0,buftemp,sizeof(buftemp)) > 0  &&
						!strcmp(buftemp,mac))
					{
						for(j=0;strlen(laninfo_attr[j])!=0;j++)
						{
							if(cfg_obj_get_object_attr(NodeName,laninfo_attr[j],0,buftemp,sizeof(buftemp)) < 0)
							{
								cfg_set_object_attr(NodeName,laninfo_attr[j],"0");
							}
						}
						memset(buftemp, 0, sizeof(buftemp));
						snprintf(buftemp, sizeof(buftemp), "%s", ip);
						cfg_set_object_attr(NodeName,"IpAddr",buftemp);

						memset(buftemp, 0, sizeof(buftemp));
						if(cfg_get_devinfo(mac,buftemp,1) == 0)
						{
							cfg_set_object_attr(NodeName,"Port",buftemp);
						}
						else
						{
							cfg_set_object_attr(NodeName,"Port","0");
						}
						isintable = 1;
						break;
					}
				}
				else{
					isintable = 0;
					entry_index[k] = i; 
					k++;
				}
			}
			if(isintable == 0)
			{
				snprintf(NodeName, sizeof(NodeName), "root.laninfo.entry.%d", entry_index[0]);				
				cfg_set_object_attr(NodeName,"Active","1");
				for(j=0;strlen(laninfo_attr[j])!=0;j++)
				{
					if(cfg_obj_get_object_attr(NodeName,laninfo_attr[j],0,buftemp,sizeof(buftemp)) < 0)
					{
						cfg_set_object_attr(NodeName,laninfo_attr[j],"0");
					}
				}
				memset(buftemp, 0, sizeof(buftemp));
				snprintf(buftemp, sizeof(buftemp), "%s", ip);
				cfg_set_object_attr(NodeName,"IpAddr",buftemp);
					
				memset(buftemp, 0, sizeof(buftemp));
				snprintf(buftemp, sizeof(buftemp), "%s", mac);
				cfg_set_object_attr(NodeName,"MacAddr",buftemp);

				memset(buftemp, 0, sizeof(buftemp));
				if(cfg_get_devinfo(mac,buftemp,1) == 0)
					cfg_set_object_attr(NodeName,"Port",buftemp);
				else
					cfg_set_object_attr(NodeName,"Port","0");
			}
			devnum++;
		}
	}

	for(i=0;i<MAX_DEVNUM_RULE;i++)
	{
		snprintf(NodeName, sizeof(NodeName), "root.laninfo.entry.%d", i+1);
		memset(buftemp, 0, sizeof(buftemp));
		if(cfg_obj_get_object_attr(NodeName,"MacAddr",0,buftemp,sizeof(buftemp)) > 0)
			snprintf(pBuf1[i],20,"%s",buftemp);
		else
			strcpy(pBuf1[i],"");
		isintable = 0;
		for(j=0;j<devnum;j++){
			if(!strcmp(pBuf1[i],pBuf[j]))
				isintable =1;
				
		}
		if(isintable == 0){
			cfg_delete_object(NodeName);
		}

	}
	
	snprintf(buftemp,sizeof(buftemp),"%d",devnum);
	strncpy(NodeName, "root.laninfo.common", sizeof(NodeName)-1);
	cfg_set_object_attr(NodeName,"Number",buftemp);
	
FreeBUF:
	for(i=0; i<MAX_DEVNUM_RULE; i++){
		if(NULL != pBuf[i]){
			free(pBuf[i]);
			pBuf[i] = NULL;
		}
		if(NULL != pBuf1[i]){
			free(pBuf1[i]);
			pBuf1[i] = NULL;
		}
	}
	fclose(fp);

	return cfg_type_default_func_get(path,attr,val,len); 

}
#endif

static cfg_node_ops_t cfg_type_laninfo_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_laninfo_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 64, 
	 .parent = &cfg_type_laninfo, 
	 .index = cfg_type_laninfo_index, 
	 .ops = &cfg_type_laninfo_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_laninfo_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_laninfo_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_laninfo, 
	 .ops = &cfg_type_laninfo_common_ops, 
}; 


static cfg_node_ops_t cfg_type_laninfo_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_laninfo_child[] = { 
	 &cfg_type_laninfo_entry, 
	 &cfg_type_laninfo_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_laninfo = { 
	 .name = "LanInfo", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_laninfo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_laninfo_child, 
	 .ops = &cfg_type_laninfo_ops, 
}; 
