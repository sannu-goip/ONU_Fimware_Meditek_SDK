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
#include <sys/time.h> 
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "cfg_romfile.h"
#include "utility.h"
#include "cfg_type_guestssidinfo.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#else
#include <libapi_lib_wifimgr.h>
#endif


int stopGuestSSID(int entryIndex) 
{
	char nodeName[64]={0};
	char nodeName1[64]={0};
	char ssidbuf[8] = {0};
	int ssid = 0;
	char cmd[128] = {0};
	int k = 0;
	char newValue[64] = {0}, oldValue[64] = {0}, buf[64] = {0};
	char guestNode[64]={0};
	int index = 0;

	char *wlan_entry_para[]=
	{
		"EnableSSID",
		"AuthMode",
		"HideSSID",
		"SSID",
		"EncrypType",
		"WPAPSK",
		"WPSConfMode",
		"Key1Str",
	};
	
	char *wlan_common_para[] = {
		"HT_BW",
		"VHT_BW",
		"HT_BSSCoexistence",
	};

	memset(guestNode, 0, sizeof(guestNode));
	snprintf(guestNode, sizeof(guestNode), GUESTSSIDINFO_ENTRY_NODE, entryIndex);
	if (cfg_obj_get_object_attr(guestNode, GSSIDINDEX, 0, ssidbuf, sizeof(ssidbuf)) < 0 
		|| (ssid = atoi(ssidbuf)) < 0 ){
		return 0;
	}
	memset(nodeName, 0, sizeof(nodeName));
	
	if (ssid > 0 && ssid <= MAX_ECNT_WALN_PORT_NUM) {
		snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, ssid);
		strncpy(nodeName1, WLAN_COMMON_NODE, sizeof(nodeName1)-1);
	}
#if defined(TCSUPPORT_WLAN_AC)
	else if (ssid > MAX_ECNT_WALN_PORT_NUM && ssid <= MAX_DUALBAND_BSSID_NUM) {
		snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, ssid-4);
		strncpy(nodeName1, WLAN11AC_COMMON_NODE, sizeof(nodeName1)-1);
	}
#endif
	else 
		return 0;
	
	/*reset the wlan config before guest ssid config*/
	for (k = 0; k < WLAN_ENTRY_ATTR_NUM; k++) {
		memset(newValue, 0, sizeof(newValue));
		snprintf(buf, sizeof(buf), "old%s", wlan_entry_para[k]);
		if (cfg_obj_get_object_attr(guestNode, buf, 0, oldValue, sizeof(oldValue)) < 0) {
			/*get value error, continue*/
			continue;
		}
		cfg_set_object_attr(nodeName, wlan_entry_para[k], oldValue);
	}
	/*set EnableSSID disable in WLan/WLan11ac*/
	cfg_set_object_attr(nodeName, wlan_entry_para[0], "0");
#if defined(TCSUPPORT_CUC)
	cfg_set_object_attr(nodeName, "IsGuest", "0");
#endif
	/*common attribute*/
	for (k = 0;k < WLAN_COMMON_ATTR_NUM; k++)
	{
		memset(newValue, 0, sizeof(newValue));
		snprintf(buf, sizeof(buf), "old%s", wlan_common_para[k]);
		if (cfg_obj_get_object_attr(guestNode, buf, 0, oldValue, sizeof(oldValue)) < 0) {
			/*get value error, continue*/
			continue;
		}
		cfg_set_object_attr(nodeName1, wlan_common_para[k], oldValue);
	}
	/*disable the guest ssid node*/
	cfg_set_object_attr(guestNode, GSSIDENABLE, "0");
	cfg_set_object_attr(guestNode, "EndTime", "0");

	cfg_set_object_attr(guestNode, GSSIDINDEX, "0");
	cfg_set_object_attr(guestNode, GSSIDPORTISOL, "0");
	cfg_set_object_attr(guestNode, GSSIDDURATION, "0");

	memset(cmd, 0, sizeof(cmd));
	if (ssid > 0 && ssid <= MAX_ECNT_WALN_PORT_NUM) { /*disconnet all sta*/
		wifimgr_lib_set_WIFI_CMD("ra", ssid - 1, sizeof(cmd), "DisConnectAllSta=1", cmd, 0);
	}
#if defined(TCSUPPORT_WLAN_AC)
	else {
		wifimgr_lib_set_WIFI_CMD("rai", ssid - MAX_ECNT_WALN_PORT_NUM - 1, sizeof(cmd), "DisConnectAllSta=1", cmd, 0);
	}
#endif
	system(cmd);

	/*disable the portIsolation*/	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/portbindcmd portisolation 0 %d 1", ssid-1);
	system(cmd);

	cfg_commit_object(nodeName);
	cfg_evt_write_romfile_to_flash();

	return 0;
}


int setwlanattrvalue(int type, char nodeName[], char guestNode[]) {
	int k = 0, number = 0, flag = 0;
	char newValue[64] = {0}, oldValue[64] = {0}, buf[64] = {0};
	char **wlan_ptr = NULL;
	
	char *wlan_entry_para[]=
	{
		"EnableSSID",
		"AuthMode",
		"HideSSID",
		"SSID",
		"EncrypType",
		"WPAPSK",
		"WPSConfMode",
		"Key1Str",
	};

	char *wlan_common_para[] = {
		"HT_BW",
		"VHT_BW",
		"HT_BSSCoexistence",
	};

	if (type == 1) {
		wlan_ptr =wlan_entry_para;
		number = WLAN_ENTRY_ATTR_NUM;
	}
	else if (type ==2 ) {
		wlan_ptr =wlan_common_para;
		number = WLAN_COMMON_ATTR_NUM;
	}
	else {
		printf("type = %d, which is error!\n", type);
		return 0;
	}

	for (k = 0;k < number; k++) {
		memset(newValue, 0, sizeof(newValue));
		if (cfg_obj_get_object_attr(guestNode, wlan_ptr[k], 0, newValue, sizeof(newValue)) < 0) {
			/*get new value error, continue*/
			continue;
		}
	
		memset(oldValue, 0, sizeof(oldValue));
		if (cfg_obj_get_object_attr(nodeName, wlan_ptr[k], 0, oldValue, sizeof(oldValue)) < 0) {
			/*get old value error, continue*/	
			continue;
		}
	
		if (strncmp(newValue, oldValue, sizeof(newValue)-1) == 0){
			continue;
		}
		snprintf(buf, sizeof(buf), "old%s",wlan_ptr[k] );
		cfg_set_object_attr(guestNode, buf, oldValue);
		
		cfg_set_object_attr(nodeName, wlan_ptr[k], newValue);
		/*flag = 1, need commit wlan node*/
		flag =1;
	}

	return flag;
}


int guestssidinfo_configure()
{	
	char nodeName[64]={0}, nodeName1[64]={0}, guestNode[64]={0};
	int i = 0, ssid = 0, time = 0, en = -1, oldssid = -1, flag1 = 0;
	long sec_time = 0;
	struct timeval curTime;
	char cmd[128] = {0};
	char duration[8] = {0}, portIsolation[8] = {0}, enable[8] = {8}, endtime[16] = {0};
	char buf[8] = {0},bssid_num[8];
	int index = 0;
	char delByAPI[2] = {0};

	memset(guestNode, 0, sizeof(guestNode));
	for (i = 1; i <= GUESTSSIDINFO_NUM; i++) {
		snprintf(guestNode, sizeof(guestNode), GUESTSSIDINFO_ENTRY_NODE, i);
		
		if (cfg_obj_get_object_attr(guestNode, GSSIDINDEX, 0, duration, sizeof(duration)) < 0) {
			continue;
		}
		ssid = atoi(duration);

		if (checkconflictssid(GUEST_TYPE, ssid) == -1) {
			printf("guestssidinfo_configure:ssid %d is wlanshare ssid\n", ssid);
			continue;
		}
	memset(bssid_num,0,sizeof(bssid_num));
	if(ssid >= 1 && ssid <= MAX_ECNT_WALN_PORT_NUM)
	{
		if(cfg_obj_get_object_attr(WLAN_COMMON_NODE,"BssidNum", 0, bssid_num,sizeof(bssid_num)) < 0)
		{
			tcdbg_printf("2.4G Invalide BssidNum!!\n");
			return -1;
		}
		else
		{
			if(atoi(bssid_num) < ssid)
			{
				tcdbg_printf("%s %d : 2.4G ssid %d is not exist!!\n", __FUNCTION__,__LINE__,ssid);
				continue;
			}
		}
	}
#if defined(TCSUPPORT_WLAN_AC)
	else if(ssid > MAX_ECNT_WALN_PORT_NUM && ssid <= MAX_DUALBAND_BSSID_NUM)
	{
		if(cfg_obj_get_object_attr(WLAN11AC_COMMON_NODE,"BssidNum", 0,bssid_num,sizeof(bssid_num)) < 0)
		{
			tcdbg_printf("5G Invalide BssidNum!!\n");
			return -1;
		}
		else
		{
			if(atoi(bssid_num) < (ssid - MAX_ECNT_WALN_PORT_NUM))
			{
				tcdbg_printf("%s %d :5G ssid %d is not exist!!\n", __FUNCTION__,__LINE__,ssid - MAX_ECNT_WALN_PORT_NUM);
				continue;
			}
		}
	}
#endif
	else
	{
		continue;
	}	
		if (cfg_obj_get_object_attr(guestNode, GSSIDENABLE, 0, enable, sizeof(enable)) < 0
			|| (en = atoi(enable)) == 0) {
			continue;
		}
		if(en  == 2 ){ /* stop gust ssid now */
			stopGuestSSID(i);
			
			if (cfg_obj_get_object_attr(guestNode, "delByAPI", 0, delByAPI, sizeof(delByAPI)) >= 0
				&& atoi(delByAPI) == 1)
			{
				cfg_obj_delete_object(guestNode);
			}
			continue;
		}

		if(cfg_obj_get_object_attr(guestNode, "EndTime", 0, buf, sizeof(buf)) > 0 && strncmp(buf, "0", sizeof(buf)) != 0)
		{
			/*enable = 1, endtime != 0, guest entry is running */
			continue;
		}
		
		if (cfg_obj_get_object_attr(guestNode, GSSIDDURATION, 0, duration, sizeof(duration)) < 0) {
			continue;
		}
		
		time = atoi(duration);
		if (time < 0) {
			continue;
		}
		
		cfg_obj_get_object_attr(guestNode, GSSIDPORTISOL, 0, portIsolation, sizeof(portIsolation));

		memset(nodeName, 0, sizeof(nodeName));
		memset(nodeName1, 0, sizeof(nodeName1));
		/* set wlan entry value */
		if (ssid >= 1 && ssid <= MAX_ECNT_WALN_PORT_NUM) {
			snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, ssid);
			strncpy(nodeName1, WLAN_COMMON_NODE, sizeof(nodeName1)-1);
		}
#if defined(TCSUPPORT_WLAN_AC)
		else if (ssid > MAX_ECNT_WALN_PORT_NUM && ssid <= MAX_DUALBAND_BSSID_NUM) {
			snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, ssid-MAX_ECNT_WALN_PORT_NUM);
			strncpy(nodeName1, WLAN11AC_COMMON_NODE, sizeof(nodeName1)-1);
		}
#endif
		flag1 = setwlanattrvalue(WLAN_ENTRY_TYPE, nodeName, guestNode);
	
		/*set wlan or wlan11ac common value*/
		flag1 += setwlanattrvalue(WLAN_COMMON_TYPE, nodeName1, guestNode);

		/*commit and save wlan or wlan11ac config*/
		if (flag1 > 0)
		{
			cfg_commit_object(nodeName);
			cfg_evt_write_romfile_to_flash();
		}
		
		
		if (time == 0) {
			sec_time = 0;
		}
		else {
			gettimeofday(&curTime, NULL);
			sec_time = curTime.tv_sec +curTime.tv_usec/1000000 + time*60;
		}
		snprintf(endtime, sizeof(endtime), "%ld", sec_time);
		cfg_set_object_attr(guestNode, "EndTime", endtime);
		
		/*portIsolation*/	
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd),"/usr/bin/portbindcmd portisolation 1 %d %s",  ssid-1, portIsolation);
		system_escape(cmd);
	}

	return 0;
}

int cfg_type_guestssidinfo_func_read(char* path,char* attr,char* val,int len)
{	
	long cur_time = 0, remain_time = 0;
	int time = 0;
	struct timeval curTime;
	char endTime[8] = {0}, remainStr[8] = {0};
	cur_time = 0;
	remain_time = 0;

	if (attr != NULL && strncmp(attr, "RemainDuration", 14) == 0) {
		if (cfg_obj_get_object_attr(path, "EndTime", 0, endTime, sizeof(endTime)) < 0) {
			return 0;
		}
		time = atol(endTime);
		gettimeofday(&curTime, NULL);
		cur_time = curTime.tv_sec +curTime.tv_usec/1000000;

		if ((cur_time > time + GUESTSSIDINFO_MAX_TIME) || (time > cur_time + GUESTSSIDINFO_MAX_TIME)){
			printf("guestssidinfo_read: remain time error");
			cfg_set_object_attr(path, "RemainDuration", "0");
			return 0;
		}
		else {
			if (cur_time <= time){
				remain_time = time - cur_time;
				memset(remainStr, 0, sizeof(remainStr));
				
				snprintf(remainStr, sizeof(remainStr), "%ld", remain_time);
				
				cfg_set_object_attr(path, "RemainDuration", remainStr);
			}
			else {
				cfg_set_object_attr(path, "RemainDuration", "0");
			}
			

		}
	}
	return 0;
}

int cfg_type_guestssidinfo_func_execute()
{
	return guestssidinfo_configure();	
}


int svc_cfg_boot_guestssidinfo(){
	int i = 0;	
	char guestNode[64]={0};
	char buffer[32] = {0};

	memset(guestNode, 0, sizeof(guestNode));
	for (i=1; i <= GUESTSSIDINFO_NUM; i++) {
		snprintf(guestNode, sizeof(guestNode), GUESTSSIDINFO_ENTRY_NODE, i);
		if (cfg_obj_get_object_attr(guestNode, GSSIDENABLE, 0, buffer, sizeof(buffer)) < 0
			|| strncmp(buffer, "1", sizeof(buffer) - 1)) {
			continue;
		}
		/* always start guset ssid */
		if (cfg_obj_get_object_attr(guestNode, GSSIDDURATION, 0, buffer, sizeof(buffer)) < 0
			|| strncmp(buffer, "0", sizeof(buffer) - 1) == 0) {
			continue;
		}
		stopGuestSSID(i);
	}

	return 0;
	
}


static int cfg_type_guestssidinfo_func_get(char* path,char* attr,char* val,int len)
{
	cfg_type_guestssidinfo_func_read(path,attr,val,len);

	return cfg_type_default_func_get(path,attr,val,len);
}

static int cfg_type_guestssidinfo_func_commit(char* path)
{
	cfg_type_guestssidinfo_func_execute();

	return 0;
}


static cfg_node_ops_t cfg_type_guestssidinfo_entry_ops  = { 
	 .get = cfg_type_guestssidinfo_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_guestssidinfo_func_commit 
}; 


static cfg_node_type_t cfg_type_guestssidinfo_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | GUESTSSIDINFO_NUM, 
	 .parent = &cfg_type_guestssidinfo, 
	 .ops = &cfg_type_guestssidinfo_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_guestssidinfo_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_guestssidinfo_func_commit 
}; 


static cfg_node_type_t cfg_type_guestssidinfo_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_guestssidinfo, 
	 .ops = &cfg_type_guestssidinfo_common_ops, 
}; 


static cfg_node_ops_t cfg_type_guestssidinfo_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_guestssidinfo_func_commit 
}; 


static cfg_node_type_t* cfg_type_guestssidinfo_child[] = { 
	 &cfg_type_guestssidinfo_entry, 
	 &cfg_type_guestssidinfo_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_guestssidinfo = { 
	 .name = "GuestSSIDInfo", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_guestssidinfo_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_guestssidinfo_child, 
	 .ops = &cfg_type_guestssidinfo_ops, 
}; 
