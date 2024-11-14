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
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <sys/types.h>
#include "cfg_types.h" 
#include "utility.h" 
#include <svchost_evt.h>
#include <notify/notify.h>
#if defined(TCSUPPORT_CT_JOYME4)
#include <linux/sysinfo.h>
#endif

int svc_cfg_boot_lanhost2(void)
{
#if defined(TCSUPPORT_ANDLINK)
	char nodePath[64] = {0};
	system("/userfs/bin/lan_host_mgr &");

	snprintf(nodePath, sizeof(nodePath), BANDWIDTH_COMMON_NODE);
	cfg_obj_set_object_attr(nodePath, "Enable", 0, "1");	
	cfg_obj_commit_object(nodePath);
#else
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_LANHOST2_BOOT, NULL, 0); 
#endif
	return 0;
}

int cfg_type_lanhost2_commit(char* path)
{
	other_evt_t param;
	memset(&param, 0, sizeof(param));
	
	strncpy(param.buf, path, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_OTHER_INTERNAL, EVT_CFG_OTHER_LANHOST2_UPDATE, (void*)&param, sizeof(param));
	
	return 0;
}
char lan_host_list[CFG_TYPE_MAX_LANHOST2_ENTRY_NUM][32] = {{0},{0}};

/*
1-1:
	mac:a,active:0:means the old device ,no need to report the signal;
1-2:
	mac:b,active:1:possibility:this is the new device ,we need to report the signal,and record the message
				 possibility:this is the old device,and re-onine,no need to report the signal
*/
void laninfo_newdevice_inform(int entry_idx)
{
	char node[64] = {0};
	char mac[24] = {0};
	char vmac[24] = {0};
	char hostname[100] = {0};
	char conval[5] = {0};
	char portval[5] = {0};
	int connectiontype = 0;
	int port = 0;
	laninfo_msg_t msgdata;
	memset(node, 0, sizeof(node));
	memset(mac, 0, sizeof(mac));
	memset(vmac, 0, sizeof(vmac));
	memset(&msgdata, 0, sizeof(laninfo_msg_t));
	snprintf(node, sizeof(node),LANHOST2_ENTRY_NODE,entry_idx+1);
	cfg_get_object_attr(node, "MAC", mac, sizeof(mac));
	memcpy(msgdata.mac, mac, sizeof(mac));
	cfg_get_object_attr(node, "VMAC", vmac, sizeof(vmac));
	memcpy(msgdata.vmac, vmac, sizeof(vmac));
	cfg_get_object_attr(node, "ConnectionType", conval, sizeof(conval));
	connectiontype = atoi(conval);
	msgdata.connectionType = connectiontype;
	cfg_get_object_attr(node, "Port", portval, sizeof(portval));
	port = atoi(portval);
	msgdata.port= port;
	cfg_get_object_attr(node, "Hostname", hostname, sizeof(hostname));
	memcpy(msgdata.hostname, hostname, sizeof(hostname));
	ctc_notify2ctcapd(NOTIFY_LANINFO_NEWDEVICE, &msgdata, sizeof(laninfo_msg_t));
}

int cfg_type_lanhost_common2_commit(char* path) 
{
	char lanhost_path[32] = {0}, value[32] = {0};
	int i, host_num = 0, ctrl_num = 0;
	char s_max_lan_host[32] = {0}, s_max_ctrl_lan_host[32] = {0};
	int max_lan_host = 64, max_ctrl_lan_host = 32;

	snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_COMMON_NODE);
	cfg_obj_get_object_attr(lanhost_path, "LANHostMaxNumber", 0, 
		s_max_lan_host, sizeof(s_max_lan_host));
	cfg_obj_get_object_attr(lanhost_path, "ControlListMaxNumber", 0, 
		s_max_ctrl_lan_host, sizeof(s_max_ctrl_lan_host));
	if (s_max_lan_host[0] != '\0')
	{
		max_lan_host = atoi(s_max_lan_host);
	}
	if (s_max_ctrl_lan_host[0] != '\0')
	{
		max_ctrl_lan_host = atoi(s_max_ctrl_lan_host);
	}


	for (i = 0; i < max_lan_host; i++) 
	{
		snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_ENTRY_NODE, i + 1);
		bzero(value, sizeof(value));
		if ((cfg_obj_get_object_attr(lanhost_path, "MAC", 0, value, sizeof(value)) <= 0) || strlen(value) <= 0)
		{
#if defined(TCSUPPORT_CT_UBUS)
			memset(lan_host_list[i],0,sizeof(lan_host_list[i]));
#endif
			continue;
		}
#if defined(TCSUPPORT_CT_UBUS)
		if(strcmp(lan_host_list[i],value) != 0)
		{
			memcpy(lan_host_list[i],value,sizeof(value));
			laninfo_newdevice_inform(i);
		}
#endif
		host_num++;
		if (ctrl_num < max_ctrl_lan_host)
		{
			bzero(value, sizeof(value));
			if (cfg_obj_get_object_attr(lanhost_path, "ControlStatus", 0, value, sizeof(value)) > 0)
			{
				if (atoi(value) == 1)
				ctrl_num++;
			}
		}
	}

	/* update host num and ctrl num */
	snprintf(lanhost_path, sizeof(lanhost_path), LANHOST2_COMMON_NODE);
	if ( cfg_query_object(lanhost_path, NULL, NULL) < 0 )
		cfg_create_object(lanhost_path);

	snprintf(value, sizeof(value), "%d", host_num);
	cfg_obj_set_object_attr(lanhost_path, "LANHostNumber", 0, value);

	snprintf(value, sizeof(value), "%d", ctrl_num);
	cfg_obj_set_object_attr(lanhost_path, "ControlListNumber", 0, value);

	return 0;
	
}


/* LANHost Common node defined start */
static cfg_node_ops_t cfg_type_lanhost2_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_lanhost_common2_commit 
}; 

static cfg_node_type_t cfg_type_lanhost2_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_lanhost2, 
	 .ops = &cfg_type_lanhost2_common_ops, 
}; 

/* LANHost Common node defined end */

/* LANHost Entry node defined start */
static char* cfg_type_lanhost2_index[] = {
	 NULL 
}; 

#if defined(TCSUPPORT_CT_JOYME4)
static int update_lanhost2_time(char* path, char* attr)
{
	char str_time[32] = {0}, onlinetime[32] = {0};
	time_t utime = 0, realtime = 0;
	struct tm curTime, upTime, syncTime;
	struct sysinfo info;

	/* get device up time */
	bzero(str_time, sizeof(str_time));
	cfg_obj_get_object_attr(path, attr, 0, str_time, sizeof(str_time));
	if ( 0 == str_time[0] )
		return -1;

	/* unsynchronized time */
	if ( 0 == strncmp(str_time, "1970", 4) )
	{
		/* tzset() should be called before localtime_r() according to POSIX.1-2004 */
		tzset();

		/* get system realtime(sys_realtime) */
		time(&utime);
		localtime_r(&utime, &curTime);

		/* check ntp server sync */
		if ( 70 == curTime.tm_year ) /* nptserver not sync */
			return -1;

		/* get system uptime(sys_uptime) */
		sysinfo(&info);

		/* calc device up time to second(dev_uptime) */
		strptime(str_time, "%Y-%m-%d %H:%M:%S", &upTime);
		realtime = mktime(&upTime);

		/* 
		** calc time offset for ntp sync, use system RealTime - system UpTime(offset = sys_realtime - sys_uptime). 
		** calc device up RealTime by plus offset( realtime = dev_uptime + offset )
		*/
		realtime = realtime + ( utime - info.uptime );

		/* convert to tm struct */
		localtime_r(&realtime, &syncTime);

		/* update attr value */
		bzero(str_time, sizeof(str_time));
		snprintf(str_time, sizeof(str_time), "%04d-%02d-%02d %02d:%02d:%02d", syncTime.tm_year + 1900, syncTime.tm_mon + 1, syncTime.tm_mday,
				syncTime.tm_hour, syncTime.tm_min, syncTime.tm_sec);
		cfg_set_object_attr(path, attr, str_time);
	}

	return 0;
}

static int cfg_type_lanhost2_entry_func_get(char* path, char* attr, char* val, int len) 
{
	char dbusflag[4] = {0};

	cfg_get_object_attr("root.sys.entry", "ctcDbusFlag", dbusflag, sizeof(dbusflag));
	if ( 0 == strcmp(dbusflag, "5") )
	{
		return cfg_type_default_func_get(path,attr,val,len); 
	}

	if ( 0 == strcmp(attr, "LatestActiveTime")
		|| 0 == strcmp(attr, "LatestInactiveTime") )
	{
		update_lanhost2_time(path, attr);
	}

	 return cfg_type_default_func_get(path,attr,val,len);
}
#endif

static cfg_node_ops_t cfg_type_lanhost2_entry_ops  = { 
#if defined(TCSUPPORT_CT_JOYME4)
	 .get = cfg_type_lanhost2_entry_func_get, 
#else
	 .get = cfg_type_default_func_get, 
#endif
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_lanhost2_commit 
}; 

static cfg_node_type_t cfg_type_lanhost2_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_LANHOST2_ENTRY_NUM, 
	 .parent = &cfg_type_lanhost2, 
	 .index = cfg_type_lanhost2_index, 
	 .ops = &cfg_type_lanhost2_entry_ops, 
}; 
/* LANHost entry node define end */


/* LANHost node defined start */
static cfg_node_type_t* cfg_type_lanhost2_child[] = { 
	 &cfg_type_lanhost2_entry, 
	 &cfg_type_lanhost2_common,
	 NULL 
};

static cfg_node_ops_t cfg_type_lanhost2_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_lanhost2_commit 
}; 

cfg_node_type_t cfg_type_lanhost2 = { 
	 .name = "LANHost2", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_lanhost2_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_lanhost2_child, 
	 .ops = &cfg_type_lanhost2_ops, 
};
/* LANHost node end */

