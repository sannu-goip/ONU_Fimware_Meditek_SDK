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
#include <errno.h>
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include "cfg_types.h"
#include <unistd.h>
#include <sys/stat.h>
#include "utility.h"


#define ATTR_SIZE 			32
#define MAXLEN_ATTR_NAME	32
#define SHIFT_INDEX 		32
#define MAXSIZE				160
#define QM_LEN				2


#define ACTIVE				"Yes"

#define DPROXY_PATH 		"/etc/dproxy.conf"
#define DPROXYAUTO_PATH 	"/etc/dproxy.auto"
#define DPROXY_TMP_PATH 	"/etc/dproxy.conf.tmp"
#define DPROXY_TYPE_INDEX	0

#define WAN_DEACTIVE		"No"

#define DEF_GW_FLAG			"UG"
#define TMP_CHECK_IF		"tmp/iface.conf"

#define PPP_DNS_INFO_PATH	"/etc/ppp/resolv.conf"

#define MAXGET_PROFILE_SIZE	128
#define ESCAPE_CMD_BUF		1025

#define OPEN_FAIL_MSG		"Open fail: %s %s\n"
#define RENAME_FAIL_MSG		"Rename fail: %s\n"

#define TMP_CONF			"/tmp/tmp.conf"
#define DHCPRELAY_PATH		"/etc/config/dhcprelay.sh"


int cfg_updateString(char* file,const char* keyword, char *newStr){
	FILE* in_file = NULL;
	FILE* out_file = NULL;

	char line[MAXSIZE];

	in_file = fopen(file, "r");
	if(in_file == NULL){
		fprintf(stderr, OPEN_FAIL_MSG, file, strerror(errno));
		return -1;
	}

	out_file = fopen(TMP_CONF, "w");
	if(out_file == NULL){
		fclose(in_file);
		fprintf(stderr, OPEN_FAIL_MSG, TMP_CONF, strerror(errno));
		return -1;
	}

	while(fgets(line, MAXSIZE, in_file)){
		if((strstr(line, keyword) != NULL)
                   && strlen(keyword) != 0
                     && strlen(newStr) != 0){
			strncpy(line, newStr,sizeof(line)-1);
			keyword = keyword + SHIFT_INDEX;
			newStr = newStr + SHIFT_INDEX;
		}
		fputs(line, out_file);
	}

	fclose(in_file);
	fflush(out_file);
	fclose(out_file);
	unlink(file);

	if(rename(TMP_CONF, file) < 0){
		fprintf(stderr, RENAME_FAIL_MSG,strerror(errno));
		unlink(TMP_CONF);
		return -1;
	}
	return 0;
}/*end updateString*/


static int cfg_type_dproxy_func_write(char* path)
{
	char dproxy_info[3][ATTR_SIZE];
	char nodeName[64] = {0};
	char default_route[5]={0};
	char isp[4]={0};
	char buf[4]={0};
	char tmpBuf[128]={0}, tmpBuf1[128]={0};
	char dnsInfo[2][16];
	FILE *fp=NULL;
	struct stat fbuf;
	int i=0, default_route_isp=WAN_ENCAP_NONE_INT;

	memset(dproxy_info ,0 ,sizeof(dproxy_info));
	memset(dnsInfo ,0 ,sizeof(dnsInfo));
	memset(nodeName, 0, sizeof(nodeName));
	char dproxy_attrName[][MAXLEN_ATTR_NAME]={
		{"Primary_DNS"},
		{"Secondary_DNS"},
		{""},
	};
	char dproxy_keyword[][ATTR_SIZE]=
	{
		{"nameserver ="},
		{"nameserver ="},
		{""},
	};

	memset(nodeName,0,sizeof(nodeName));
	/*find default route*/
	for(i = 1 ; i <= PVC_NUM; i++){
		snprintf(nodeName,sizeof(nodeName),WAN_PVC_NODE,i);
		if(cfg_obj_get_object_attr(nodeName, "DEFAULTROUTE", 0, default_route, sizeof(default_route))< 0){
			continue;
		}
		if(!strcmp(default_route, ACTIVE)){
			break;
		}
	}
	/*find the isp of default route*/
	if(cfg_obj_get_object_attr(nodeName, "ISP", 0, isp, sizeof(isp)) > 0){
		default_route_isp = atoi(isp);
	}

	if(cfg_query_object(DPROXY_ENTRY_NODE, NULL, NULL) <= 0){
		unlink(DPROXYAUTO_PATH);
		/*remove information*/
		cfg_updateString(DPROXY_PATH, (char *)dproxy_keyword, (char *)dproxy_info+SHIFT_INDEX);
		if((default_route_isp == WAN_ENCAP_PPP_INT) && (stat(PPP_DNS_INFO_PATH,&fbuf) == 0)){
			snprintf(tmpBuf1,sizeof(tmpBuf1),"cp %s %s",PPP_DNS_INFO_PATH,"/etc/");	/*even isp is ppp, the real dns for system use is /etc/resolv.conf, so keep /etc/resolv.conf to be updated*/
			system(tmpBuf1);
		}
		return -1;
	}
	/*krammer change for bug 1318*/
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, LAN_DHCP_NODE,sizeof(nodeName)-1);
	if(cfg_obj_get_object_attr(nodeName, "type", 0, buf, sizeof(buf)) < 0){
		return -1;
	}
	if(strcmp(buf, "1")){/*dhcp is not on*/
		return 0;
	}

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, DPROXY_ENTRY_NODE,sizeof(nodeName)-1);
	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(nodeName, "type", 0, buf, sizeof(buf)) < 0){
		return -1;
	}
	strncpy(dproxy_info[0], buf,ATTR_SIZE-1);
	if(strlen(dproxy_info[0]) == 0){
		return -1;
	}

#if !defined(TCSUPPORT_CT_JOYME4)
	if(atoi(dproxy_info[0]) == 1){	/*DNS Relay Manually*/
		/*update resolv.conf*/
		fp = fopen(DNS_INFO_PATH, "w");
		/*for DNS Relay Manually, we need to update resolv.conf by manually*/
		if(fp != NULL){
			cfg_obj_get_object_attr(nodeName, dproxy_attrName[0], 0, tmpBuf, sizeof(tmpBuf));
			snprintf(tmpBuf1,sizeof(tmpBuf1),"nameserver %s\n",tmpBuf);
			fputs_escape(tmpBuf1, fp);
			cfg_obj_get_object_attr(nodeName, dproxy_attrName[1], 0, tmpBuf, sizeof(tmpBuf));
			snprintf(tmpBuf1,sizeof(tmpBuf1),"nameserver %s\n",tmpBuf);
			fputs_escape(tmpBuf1, fp);
			fclose(fp);
		}
	}
	else
#endif
	{  /*DNS Relay Automatically*/
		/*for ppp, it will only generate resolv.conf at /etc/ppp but the default DNS server for system is at /etc.*/
		/*So in order to make sure system could look up the correct DNS server, copy /etc/ppp/resolv.conf to /etc.*/
		if((default_route_isp == WAN_ENCAP_PPP_INT) && (stat(PPP_DNS_INFO_PATH,&fbuf) == 0)){  /*rodney_20090506 added*/
			snprintf(tmpBuf1,sizeof(tmpBuf1),"cp %s %s",PPP_DNS_INFO_PATH,"/etc/");
			system(tmpBuf1);
		}
	}

#if !defined(TCSUPPORT_CT_JOYME4)
	/*delete and new a node*/
	cfg_delete_object(nodeName);
	if(cfg_create_object(nodeName) < 0)
	{
		return -1;
	}
	cfg_set_object_attr(nodeName, "Active", "Yes"); /*Dproxy always enable at this moment*/
	cfg_set_object_attr(nodeName, "type", dproxy_info[0]);
	/*open the resolv.conf and put the dns server into our node*/
	fp = fopen(DNS_INFO_PATH, "r");
	if(fp){
		i=0;
		while(fgets(tmpBuf, MAXGET_PROFILE_SIZE, fp) != NULL){
			if(strstr(tmpBuf, DNS_ATTR) &&
				strlen(dproxy_attrName[i])>0 &&
				strlen(dproxy_keyword[i])>0){
				memset(dnsInfo ,0 ,sizeof(dnsInfo));
				sscanf(tmpBuf,"%s %s",dnsInfo[0],dnsInfo[1]);
				cfg_set_object_attr(nodeName, dproxy_attrName[i], dnsInfo[1]);
				snprintf(dproxy_info[i+1],ATTR_SIZE,"%s  %s\n", dproxy_keyword[i], dnsInfo[1]);
			}
			/*fix bug. If number of nameserver is more then 4, cfg_manager will set wrong dproxy information. 
				shnwind modify 20091206.*/
			else{
				break;	
			}
			i++;
		}
		fclose(fp);
	}
#endif
	
	return 0;
}


static int cfg_type_dproxy_func_execute(char* path)
{
	char buf[64]={0}, nodeName[64]={0};

	system("killall -9 dnsmasq");
	strncpy(nodeName,LAN_DHCP_NODE,sizeof(nodeName)-1);

	if(cfg_obj_get_object_attr(nodeName, "type", 0, buf, sizeof(buf)) < 0){
		return -1;
	}
	if(!strcmp(buf,"1")){/*dproxy and dhcpd*/
		kill_process("/var/log/dhcrelay.pid");
		unlink(DHCPRELAY_PATH);
	}

	system("/userfs/bin/dnsmasq");

	return 0;
}


static int cfg_type_dproxy_func_commit(char* path)
{

#if defined(TCSUPPORT_CWMP_TR181)
	char tmpValue[8]={0};
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "CommitFromTR181", 0, tmpValue, sizeof(tmpValue));
	if(strcmp(tmpValue,"1"))
	{	
		/*not commit from tr181*/
		updateDnsRelay();
	}
#endif

	cfg_type_dproxy_func_write(path);
	cfg_type_dproxy_func_execute(path);
	return 0;
}


static cfg_node_ops_t cfg_type_dproxy_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dproxy_func_commit 
}; 


static cfg_node_type_t cfg_type_dproxy_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_dproxy, 
	 .ops = &cfg_type_dproxy_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_dproxy_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dproxy_func_commit 
}; 


static cfg_node_type_t* cfg_type_dproxy_child[] = { 
	 &cfg_type_dproxy_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dproxy = { 
	 .name = "Dproxy", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dproxy_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dproxy_child, 
	 .ops = &cfg_type_dproxy_ops, 
}; 
