#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <cfg_cli.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include "utility.h"

#define DHCPD_PUT_INS 				"%s %s\n"
#define DHCPD_ATTR_INDEX			0
#define DHCPD_VALUE_INDEX			1
#define DHCPD_ATTR_IF_INDEX			0
#define DHCPD_ATTR_LEASE_INDEX		1
#define DHCPD 						"Dhcpd"
#define MAX_STATIC_NUM 				8
#define ACTIVE						"Yes"
#define DEACTIVE					"No"
#define WAN_NAS						"nas"
#define NODE_ACTIVE					"Active"
#define MAX_SKBMGR_LEN				512
#define LAN_IP						"IP"
#define LAN_MASK					"netmask"
#define LAN_DHCP_TYPE				"type"
#define DHCPD_PATH 					"/etc/udhcpd.conf"
#ifdef DHCP_PROFILE
#define DHCPD_OPTION_PATH 			"/etc/udhcpd_option.conf"
#define SUB_NODE_OPT60_NAME 		"Option60"
#define MAX_VENDOR_CLASS_ID_LEN		64
#define SUB_NODE_OPT240_NAME 		"Option240"
#endif
#define ATTR_SIZE 					32
#define LAN_IF  					"br0"
#if defined(TCSUPPORT_CT_E8GUI)
#define DHCPD_OPTION_STB_PATH 		"/etc/udhcpd_stb_option.conf"
#define DHCPD_OPTION_PHONE_PATH 	"/etc/udhcpd_phone_option.conf"
#define DHCPD_OPTION_CAMERA_PATH 	"/etc/udhcpd_camera_option.conf"
#define DHCPD_OPTION_HGW_PATH 		"/etc/udhcpd_hgw_option.conf"
#endif
#define DHCPRELAY_PATH 				"/etc/config/dhcprelay.sh"
#define DHCPLEASE_PATH				"/etc/udhcp_lease"
#define DHCPD_ARP_WAITTIME_PATH 	"/etc/dhcpdarpwaittime"

static int Get_dnsrelay_mode(char *primarydns, int sizePrimary, char *secondarydns, int sizeSecondary)
{
	char button[10] = {0};
	char dnsrelaymode[10] = {0};
	char primary_dns[30] = {0};
	char secondary_dns[30] = {0};
	char active_dns[128]={0};
	if(cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, NODE_ACTIVE, 0, active_dns, sizeof(active_dns)) <0)
	{
		return -1;
	}

	if(0 == strcmp(active_dns,"Yes"))
	{
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE,  "type", 0, dnsrelaymode, sizeof(dnsrelaymode));
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE,  "Primary_DNS", 0, primary_dns, sizeof(primary_dns));
		cfg_obj_get_object_attr(DPROXY_ENTRY_NODE,  "Secondary_DNS", 0, secondary_dns, sizeof(secondary_dns));
		
		if(strcmp(dnsrelaymode,"1") == 0)
		{
			strncpy(primarydns, primary_dns, sizePrimary-1);
			strncpy(secondarydns, secondary_dns, sizeSecondary-1);
			return 1;								
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 0;
	}
}

static int write_dhcpd_config(char *configFile)
{
	FILE *fp = NULL;
	char buf[128]={0}, tmpBuf[512]={0}, dnsInfo[4][16];  /*rodney_20090506 modified*/
	int	i=1;          /* Looping var */
	char lanInfo[2][20] = {0};/*0: netmask address,1: Lan IP address, */
	char start[16] = {0}, poll_cnt[8] = {0}, lease_time[11] = {0}, end_ip[16] = {0}, router[16] = {0};
	char domain[64] = {0};
	char nodeName[64] = {0};  /*rodney_20090506 added*/
	char tmp[128]={0};
	char element_name[64] = {0}, static_mac[19], static_ip[16], count[3]={0};
	struct in_addr startIp,staticIp,mask;
	int isValid = 0;	
	char dhcpInfo[4] = {0};
	int res =0 ;
	char dnsrelaymode[20] = {0};
	char primarydns[20] = {0};
	char secondarydns[20] = {0};
	int ret_dns = -1;
#if defined(TCSUPPORT_NP_CMCC)
	int staticcount = 0;
	int emptyEntry = -1;
	int sameEntry = -1;
	char pAttr[4] = {0};
	char strCount[4] = {0};
	char staticName[64] = {0};
	char StaticIP[20] = {0};
	char StaticMAC[20] = {0};
#endif

#ifdef DHCP_PROFILE
	char active[8]={0};
	char vendorID[MAX_VENDOR_CLASS_ID_LEN]={0};
	char conPool_start[16] = {0};
	char conPool_cnt[8] = {0};
	char conPool_lease_time[11] = {0};
	char conPool_end_ip[16] = {0};
	struct in_addr conPool_end;
	char conPoolRouter[16] = {0};
	char conPoolSubnetMask[16] = {0};
	char conPoolDomain[64] = {0};
#endif
	struct in_addr end;
	char dhcpd_sys_parm[][ATTR_SIZE]=
	{
		{"opt subnet"},
		{"opt router"},
		{"opt dns"},
		{"opt dns"}, 
		{"opt wins"},
		{"opt domain"},
		{""},
	};

	char dhcpd_default_parm[][2][ATTR_SIZE]=
	{
		{"interface", LAN_IF},
		{"lease_file", "/etc/udhcpd.leases"},
		{"",""},
	};
	int ret = -1;
	/*get laninfo*/
	ret_dns = Get_dnsrelay_mode(primarydns, sizeof(primarydns), secondarydns, sizeof(secondarydns));
	memset(lanInfo, 0, sizeof(lanInfo));
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(LAN_ENTRY0_NODE, LAN_MASK, 0, tmp, sizeof(tmp)) > 0){
		strncpy(lanInfo[0], tmp, sizeof(lanInfo[0])-1);
	}
	
	memset(tmp,0, sizeof(tmp));
	if(cfg_obj_get_object_attr(LAN_ENTRY0_NODE, LAN_IP, 0, tmp, sizeof(tmp)) > 0){
		strncpy(lanInfo[1], tmp, sizeof(lanInfo[1])-1);
	}

	/*get dhcpinfo*/
	memset(tmp, 0, sizeof(tmp));	
	if(cfg_obj_get_object_attr(LAN_DHCP_NODE, LAN_DHCP_TYPE, 0, tmp, sizeof(tmp)) > 0)
	{
		strncpy(dhcpInfo, tmp, sizeof(dhcpInfo)-1);
	}

#ifdef DHCP_PROFILE
	if(strcmp(configFile, DHCPD_OPTION_PATH) == 0) {
		if(cfg_query_object(DHCPD_OPTION60_NODE, NULL, NULL) > 0){	
			/*get conPool_start*/
			memset(tmp, 0, sizeof(tmp));
			memset(conPool_start, 0, sizeof(conPool_start));
			if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "start", 0, tmp, sizeof(tmp)) > 0){
				strncpy(conPool_start, tmp, sizeof(conPool_start)-1);
			}
			
			/*use LAN SubnetMask and IP as conditional pool subnetMask and router */
			memset(conPoolSubnetMask,0, sizeof(conPoolSubnetMask));
			memset(conPoolRouter,0, sizeof(conPoolRouter));
			strncpy(conPoolSubnetMask, lanInfo[0], sizeof(conPoolSubnetMask)-1);
			strncpy(conPoolRouter, lanInfo[1], sizeof(conPoolRouter)-1);

			/*get conditional pool domain name*/
			memset(tmp, 0, sizeof(tmp));
			memset(conPoolDomain, 0, sizeof(conPoolDomain));
			if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "domainName", 0, tmp, sizeof(tmp)) > 0){
				strncpy(conPoolDomain, tmp, sizeof(conPoolDomain)-1);
			}
		}
		else
			return -1;
	}
#endif
	memset(nodeName, 0, sizeof(nodeName));
	memset(count, 0, sizeof(count));

	snprintf(count, sizeof(count),"%d",MAX_STATIC_NUM);
	cfg_set_object_attr(DHCPD_NODE, "MaxStaticNum", count);		
	strncpy(nodeName, DHCPD_COMMON_NODE, sizeof(nodeName)-1);

	if(0 != strcmp(dhcpInfo,"0")){/*0 means disable*/

#if defined(TCSUPPORT_CT_E8GUI)
		int j = 0;
		for(j=0; j<5; j++){
			if(0 == j)
				fp = fopen(DHCPD_OPTION_STB_PATH,"w");
			else if(1 == j)
				fp = fopen(DHCPD_OPTION_PHONE_PATH,"w");
			else if(2 == j)
				fp = fopen(DHCPD_OPTION_CAMERA_PATH,"w");
			else if(3 == j)
				fp = fopen(DHCPD_OPTION_HGW_PATH,"w");
			else
				fp = fopen(configFile,"w");
#else
		fp = fopen(configFile,"w");
#endif
		
		if(fp == NULL){
			return -1;
		}
		else{
			/*Write default parameters*/
			for(i = 0; strlen(dhcpd_default_parm[i][DHCPD_ATTR_INDEX]) != 0; i++){			
				memset(buf, 0, sizeof(buf));  
				snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_default_parm[i][DHCPD_ATTR_INDEX], dhcpd_default_parm[i][DHCPD_VALUE_INDEX]);
				fputs(buf,fp);
			}

			/*Write system parameters*/			
			memset(dnsInfo,0, sizeof(dnsInfo));  /*rodney_20090506 added*/
			for(i = 0; strlen(dhcpd_sys_parm[i]) != 0; i++){				
				memset(buf, 0, sizeof(buf));  
				if(i==0){					
#ifdef DHCP_PROFILE
					if(strcmp(configFile, DHCPD_OPTION_PATH) == 0) {
						snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], conPoolSubnetMask);
					}
					else
#endif
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], lanInfo[0]);
				}
				else if(i==2){  /*DNS1*/  /*rodney_20090506 added*/
#if defined(TCSUPPORT_NP_CMCC)
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], lanInfo[1]);
#else
					if(cfg_obj_get_object_attr(DPROXY_ENTRY_NODE, NODE_ACTIVE, 0, buf, sizeof(buf)) < 0){
						fclose(fp);
						return -1;
					}
					if(ret_dns == 1 && strlen(primarydns) != 0)	
					{
					
					
						sprintf(buf,DHCPD_PUT_INS, dhcpd_sys_parm[i],primarydns);
					}
					else if(ret_dns ==2){  /*If Dproxy enable, use Lan IP for DHCP DNS*/
						
					
						snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], lanInfo[1]);
					}
					else if(ret_dns == 0){ 
						/*If Dproxy disable, use /etc/resolv.conf info for DHCP DNS*/
						fileRead(DNS_INFO_PATH, tmpBuf, sizeof(tmpBuf));
						if(strstr(tmpBuf, DNS_ATTR)){
							sscanf(strstr(tmpBuf, DNS_ATTR),"%s %s %s %s",dnsInfo[0],dnsInfo[1],dnsInfo[2],dnsInfo[3]);
							snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], dnsInfo[1]);
						}
					}
#endif
				}
				else if(i==3){  /*DNS2*/  /*rodney_20090506 added*/
						if(ret_dns == 1 && strlen(secondarydns) != 0)
						{				
							sprintf(buf,DHCPD_PUT_INS, dhcpd_sys_parm[i], secondarydns);
					}
						else if(strcmp(dnsInfo[2],DNS_ATTR) == 0)
						{
						sprintf(buf,DHCPD_PUT_INS, dhcpd_sys_parm[i], dnsInfo[3]);
					}
						else
						{
						continue;
					}
				}
				else if(i == 1){ /*router*/
					memset(tmp,0, sizeof(tmp));
					cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "router", 0, tmp, sizeof(tmp));
					strncpy(router, lanInfo[1], sizeof(router)-1);
					
					memset(tmp,0, sizeof(tmp));
					cfg_set_object_attr(DHCPD_COMMON_NODE, "router", router);
#ifdef DHCP_PROFILE
					if(strcmp(configFile, DHCPD_OPTION_PATH) == 0) {
						snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], conPoolRouter);
					}
					else
#endif
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], router);
				}
				else if(i == 5){ /*domain name*/
#ifdef DHCP_PROFILE
					if((strcmp(configFile, DHCPD_OPTION_PATH) == 0) && strlen(conPoolDomain) != 0) {/*if conditional pool domain name is not null, use it*/
						snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], conPoolDomain);
					}
					else {/*if conditional pool domain name is null, use main pool domain*/
#endif
					
					memset(tmp,0, sizeof(tmp));
					if(cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "DomainName", 0, tmp, sizeof(tmp)) > 0 ){
						strncpy(domain, tmp, sizeof(domain)-1);
						snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], domain);
					}
#ifdef DHCP_PROFILE
					}
#endif
				}
				else{
#ifdef DHCP_PROFILE
					if(strcmp(configFile, DHCPD_OPTION_PATH) == 0) {
						snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], conPoolRouter);
					}
					else
#endif
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[i], lanInfo[1]);
				}
				fputs_escape(buf,fp);
			}

#ifdef DHCP_PROFILE
			if(strcmp(configFile, DHCPD_OPTION_PATH) == 0) {
				/*get option60 parameters*/
				if(cfg_query_object(DHCPD_OPTION60_NODE, NULL, NULL) > 0){
					/*set conditional pool subnetmask and router*/
					cfg_set_object_attr(DHCPD_OPTION60_NODE, "subnetMask", conPoolSubnetMask);
					cfg_set_object_attr(DHCPD_OPTION60_NODE, "router", conPoolRouter);

					memset(tmp,0, sizeof(tmp));
					if(cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "Active", 0, tmp, sizeof(tmp)) > 0){
						strncpy(active, tmp, sizeof(active)-1);
					}

					memset(tmp,0, sizeof(tmp));
#if defined(TCSUPPORT_CT_E8GUI)
					if(0 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDSTB", 0, tmp, sizeof(tmp));
					}
					else if(1 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDPhone", 0, tmp, sizeof(tmp));
					}
					else if(2 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDCamera", 0, tmp, sizeof(tmp));
					}
					else if(3 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorIDHGW", 0, tmp, sizeof(tmp));
					}
					else{
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorID", 0, tmp, sizeof(tmp));
					}
#else
					ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "VendorID", 0, tmp, sizeof(tmp));
#endif
					if(ret > 0){
						strncpy(vendorID, tmp, sizeof(vendorID)-1);
					}

					/*Write option60 parameters*/
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "enable" , active);
					fputs_escape(buf, fp);
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "vendorClassID" , vendorID);
					fputs_escape(buf, fp);

					
					memset(tmp,0, sizeof(tmp));
#if defined(TCSUPPORT_CT_E8GUI)
					if(0 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startSTB", 0, tmp, sizeof(tmp));
					}
					else if(1 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startPhone", 0, tmp, sizeof(tmp));
					}
					else if(2 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startCamera", 0, tmp, sizeof(tmp));
					}
					else if(3 == j){
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "startHGW", 0, tmp, sizeof(tmp));
					}
					else{
						ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "start", 0, tmp, sizeof(tmp));
					}
#else
					ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "start", 0, tmp, sizeof(tmp));
#endif
					if(ret > 0){
						strncpy(conPool_start, tmp, sizeof(conPool_start)-1);
					}
					
					memset(tmp,0, sizeof(tmp));
#if defined(TCSUPPORT_CT_E8GUI)
					if(0 == j) ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countSTB", 0, tmp, sizeof(tmp));
					else if(1 == j) ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countPhone", 0, tmp, sizeof(tmp));
					else if(2 == j) ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countCamera", 0, tmp, sizeof(tmp));
					else if(3 == j) ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_countHGW", 0, tmp, sizeof(tmp));
					else ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_count", 0, tmp, sizeof(tmp));
#else
					ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "pool_count", 0, tmp, sizeof(tmp));
#endif
					if(ret > 0){
						strncpy(conPool_cnt, tmp, sizeof(conPool_cnt)-1);
					}

					
					memset(tmp,0, sizeof(tmp));
					ret = cfg_obj_get_object_attr(DHCPD_OPTION60_NODE, "lease", 0, tmp, sizeof(tmp));
					if(ret > 0){
						strncpy(conPool_lease_time, tmp, sizeof(conPool_lease_time)-1);
					}
					 /*calculate end IP*/
					 if(strlen(conPool_start) != 0 && strlen(conPool_cnt) != 0) {
					 	inet_aton(conPool_start, &conPool_end);
						conPool_end.s_addr = ntohl(conPool_end.s_addr);
				 		conPool_end.s_addr += atoi(conPool_cnt)-1;
						conPool_end.s_addr = ntohl(conPool_end.s_addr);
						strncpy(conPool_end_ip, inet_ntoa(conPool_end), sizeof(conPool_end_ip)-1);
					 }
					/*Write user setting parameters*/
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "start" , conPool_start);
					fputs_escape(buf, fp);
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "end" , conPool_end_ip);
					fputs_escape(buf, fp);
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "max_leases" , conPool_cnt);
					fputs_escape(buf, fp);
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "option lease" , conPool_lease_time);
					fputs_escape(buf, fp);
				}

				/*get option240 parameters*/
				if(cfg_query_object(DHCPD_OPTION240_NODE,NULL,NULL) > 0){
					memset(tmp,0, sizeof(tmp));
					ret = cfg_obj_get_object_attr(DHCPD_OPTION240_NODE, "Active", 0, tmp, sizeof(tmp));
					if(ret > 0){
						strncpy(active, tmp, sizeof(active)-1);
					}
					
					memset(tmp,0, sizeof(tmp));
					ret = cfg_obj_get_object_attr(DHCPD_OPTION240_NODE, "Value", 0, tmp, sizeof(tmp));
					if(ret > 0){
						strncpy(vendorID, tmp, sizeof(vendorID)-1);
					}

					/*Write option240 parameters*/
					memset(buf,0, sizeof(buf));
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "opt240Enable" , active);
					fputs_escape(buf, fp);
					
					memset(buf,0, sizeof(buf));
					snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "opt240Value" , vendorID);
					fputs_escape(buf, fp);
				}
				
			}
			else {
			
#endif
			memset(tmp,0, sizeof(tmp));
			ret = cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "start", 0, tmp, sizeof(tmp));
			if(ret > 0){
				strncpy(start, tmp, sizeof(start)-1);
			}
			memset(tmp,0, sizeof(tmp));
			ret = cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "pool_count", 0, tmp, sizeof(tmp));
			if(ret > 0){
				strncpy(poll_cnt, tmp, sizeof(poll_cnt)-1);
			}
			memset(tmp,0, sizeof(tmp));
			ret = cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "lease", 0, tmp, sizeof(tmp));
			if(ret > 0){
				strncpy(lease_time, tmp, sizeof(lease_time)-1);
			}

			/*calculate end IP*/
			inet_aton(start, &end);
			end.s_addr = ntohl(end.s_addr);
			end.s_addr += atoi(poll_cnt) -1;
			end.s_addr = ntohl(end.s_addr);
			strncpy(end_ip, inet_ntoa(end), sizeof(end_ip)-1);
			/*Write user setting parameters*/
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "start" , start);
			fputs_escape(buf, fp);

			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "end" , end_ip);
			fputs_escape(buf, fp);
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "max_leases" , poll_cnt);
			fputs_escape(buf, fp);
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "option lease" , lease_time);
			fputs_escape(buf, fp);
			/*User set Arp Wait Time. shnwind 20100112*/
			
			memset(tmp, 0, sizeof(tmp));
			if(cfg_obj_get_object_attr(DHCPD_COMMON_NODE, "ArpWaitTime", 0, tmp, sizeof(tmp)) > 0){
				res = remove(DHCPD_ARP_WAITTIME_PATH);
				if(res){
					/*do nothing,just for compile*/
				}
				memset(tmpBuf, 0, sizeof(tmpBuf));
				snprintf(tmpBuf, sizeof(tmpBuf), "echo %s > %s", tmp, DHCPD_ARP_WAITTIME_PATH);
				system_escape(tmpBuf);
			}
			
#ifdef DHCP_PROFILE
			}
#endif			

#if defined(TCSUPPORT_NP_CMCC)
			memset(pAttr, 0, sizeof(pAttr));
			if(cfg_get_object_attr(DHCPD_COMMON_NODE, "Action", pAttr, sizeof(pAttr)) >= 0) 
			{
				if(strcmp(pAttr, "Del") == 0)
				{
					memset(tmp, 0, sizeof(tmp));
					if((cfg_get_object_attr(DHCPD_COMMON_NODE, "DeleteIndex", tmp, sizeof(tmp)) >= 0)
						&& (tmp[0] != '\0'))
					{
						unset_action(tmp, DEL_DHCPD);
					}
				}
				else if(strcmp(pAttr, "Add") == 0)
				{
					for(i=1; i<=MAX_STATIC_NUM; i++)
					{
						memset(staticName, 0, sizeof(staticName));
						memset(tmp, 0, sizeof(tmp));
						snprintf(staticName, sizeof(staticName), DHCPD_ENTRY_NODE, i);	
						if(cfg_query_object(staticName,NULL,NULL) < 0)
						{
							if(emptyEntry < 0)
								emptyEntry = i;
						}

						memset(tmp, 0, sizeof(tmp));
						if((cfg_obj_get_object_attr(staticName, "IP", 0, tmp, sizeof(tmp)) >= 0)
							 && (tmp[0] != '\0'))
						{
							memset(StaticIP, 0, sizeof(StaticIP));
							cfg_get_object_attr(DHCPD_COMMON_NODE, "StaticIP", StaticIP, sizeof(StaticIP));
							if(strcmp(StaticIP, tmp) == 0)
							{
								sameEntry = i;
							}
							continue;
						}
						
						
						memset(tmp, 0, sizeof(tmp));
						if((cfg_obj_get_object_attr(staticName, "MAC", 0, tmp, sizeof(tmp)) < 0)
							&& (tmp[0] != '\0'))
						{
							memset(StaticMAC, 0, sizeof(StaticMAC));
							cfg_get_object_attr(DHCPD_COMMON_NODE, "StaticMAC", StaticMAC, sizeof(StaticMAC));
							if(strcmp(StaticMAC, tmp) == 0)
							{
								sameEntry = i;
							}
							continue;
						}
						
						if(emptyEntry < 0)
							emptyEntry = i;
						
					}

					if(emptyEntry > 0 && sameEntry < 0)
					{
						memset(staticName, 0, sizeof(staticName));
						snprintf(staticName, sizeof(staticName), DHCPD_ENTRY_NODE, emptyEntry);	

						cfg_create_object(staticName);

						memset(StaticIP, 0, sizeof(StaticIP));
						cfg_get_object_attr(DHCPD_COMMON_NODE, "StaticIP", StaticIP, sizeof(StaticIP));

						memset(StaticMAC, 0, sizeof(StaticMAC));
						cfg_get_object_attr(DHCPD_COMMON_NODE, "StaticMAC", StaticMAC, sizeof(StaticMAC));
						
						if((StaticIP[0] != '\0') && (StaticMAC[0] != '\0'))
						{
							cfg_set_object_attr(staticName, "IP", StaticIP);
							cfg_set_object_attr(staticName, "MAC", StaticMAC);
						}
					}
				}
			}
#endif			

			/*Write static lease DHCP*/
			for(i=1; i<=MAX_STATIC_NUM; i++){
				/*set query data, indicate entry*/		
				snprintf(element_name, sizeof(element_name), DHCPD_ENTRY_NODE, i);		
				if(cfg_query_object(element_name,NULL,NULL) <= 0){
					continue;
				}
				
				memset(tmp,0, sizeof(tmp));
				cfg_obj_get_object_attr(element_name, "IP", 0, tmp, sizeof(tmp));
				strncpy(static_ip, tmp, sizeof(static_ip)-1);
				
				inet_aton(start, &startIp);
				inet_aton(lanInfo[0], &mask);	
				inet_aton(static_ip, &staticIp);
				/*Check the static lease ip whether valid,or delete it*/
				if((startIp.s_addr & mask.s_addr) == (staticIp.s_addr & mask.s_addr))
				{
					isValid = 1;			
				}
				/*Not valid ,delete it*/
				if(!isValid){
					cfg_delete_object(element_name);
					continue;
				}							
				
				memset(tmp,0, sizeof(tmp));
				cfg_obj_get_object_attr(element_name, "MAC", 0, tmp, sizeof(tmp));
				strncpy(static_mac, tmp, sizeof(static_mac)-1);
				
				snprintf(buf, sizeof(buf), "%s %s %s\n", "static_lease" , static_mac,static_ip);
				fputs_escape(buf, fp);
#if defined(TCSUPPORT_NP_CMCC)
				staticcount++;
#endif			
			}

			fclose(fp);
#if defined(TCSUPPORT_NP_CMCC)
#if defined(TCSUPPORT_CT_E8GUI)
			snprintf(strCount, sizeof(strCount), "%d", staticcount/5);		
#else
			snprintf(strCount, sizeof(strCount), "%d", staticcount);
#endif
			cfg_set_object_attr(DHCPD_NODE, "Static_Num", strCount);
#endif			
		}
#if defined(TCSUPPORT_CT_E8GUI)
		}
#endif
	}else{
      		unlink(configFile);
#if defined(TCSUPPORT_CT_E8GUI)
		unlink(DHCPD_OPTION_STB_PATH);
		unlink(DHCPD_OPTION_PHONE_PATH);
		unlink(DHCPD_OPTION_CAMERA_PATH);
		unlink(DHCPD_OPTION_HGW_PATH);
#endif
	}
	return 0;
}

static int dhcpd_func_write(void)
{
	write_dhcpd_config(DHCPD_PATH);
		
#ifdef DHCP_PROFILE
	write_dhcpd_config(DHCPD_OPTION_PATH);
#endif	

	return 0;
}

static int dhcpd_func_execute(void)
{

	char buf[64]={0}, nodeName[64]={0};
	
	system("killall -9 udhcpd");

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName,LAN_DHCP_NODE, sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, LAN_DHCP_TYPE, 0, buf, sizeof(buf)) < 0){
		return -1;
	}
	if(!strcmp(buf,"1")){
		kill_process("/var/log/dhcrelay.pid");
		unlink(DHCPRELAY_PATH);
		system("taskset 4 /usr/sbin/udhcpd &");
	}else {
		unlink(DHCPD_PATH);
	#ifdef STATIC_DHCP
		unlink(DHCPLEASE_PATH);
	#endif		
	}

	return 0;
}

int svc_other_handle_event_dhcpd_boot(void)
{
	FILE *startupSh=NULL;
	
	dhcpd_func_write();
	startupSh=fopen(DHCPD_PATH,"r");
	if(startupSh){
		fclose(startupSh);
		system("killall -9 udhcpd");
		system("taskset 4 /usr/sbin/udhcpd &");
		startupSh=fopen("/etc/dproxy.conf","r");
		if(startupSh){
			fclose(startupSh);
			system("/userfs/bin/dproxy  -c /etc/dproxy.conf &");
		}
	}

	return 0;
}

int svc_other_handle_event_dhcpd_update(void)
{
	dhcpd_func_write();
	dhcpd_func_execute();
	
	return 0;
}

