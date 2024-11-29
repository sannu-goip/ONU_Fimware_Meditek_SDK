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
#include <svchost_evt.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utility.h"

#include "../../common/pon_svc.h" 
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
#include "xpon_public_const.h"
#include <pon_api.h>
#endif
#endif

#define	PONCOMMON_LOS_STATUS			"losStatus"
#define PONCOMMON_ATTR_DATA_UP_TIME  	"upTime"
#define LOS_STATUS_PATH					"/proc/tc3162/los_status"
#define PONCOMMON_ATTR_XPON_LINK_STA  	"LinkSta"

int cfg_pon_send_event(unsigned char parent,unsigned char child,unsigned int sub_event,char* attr, char* val) 
{ 
	pon_cfg_update_t node_attr = {0};

    node_attr.parent = parent;
    node_attr.child	 = child;
	if((NULL != attr)&&(NULL != val)){
	    strncpy(node_attr.attr,attr,sizeof(node_attr.attr) - 1);
	    strncpy(node_attr.val,val,sizeof(node_attr.val) - 1);
	}

    if(cfg_send_event(EVT_PON_INTERNAL,sub_event,(void *)(&node_attr),sizeof(node_attr)) == -1)
    {
    	printf("%s: cfg_send_event fail\n",__FUNCTION__);
		return -1;
    }   
	return 0;
} 
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
int cfg_pon_get_info(methd_pon_id_t methd_id, void* pdata)
{
	int ret = 0;
	struct host_service_pbuf send = {0};
	struct host_service_pbuf rcv = {0};
	unsigned int len = 0;
	void* ptmp = NULL;

	if(NULL == pdata){
		return -1;
	}

	svchost_init_pbuf(&send);
	svchost_init_pbuf(&rcv);

	if((ret = svchost_put_param(&send, sizeof(methd_pon_id_t),(void*)&methd_id)) < 0)
	{
		printf("svchost_put_param return fail!\n");
		goto error;
	}

	if((ret = svchost_invoke( HOST_SERVICE_PONMGR_ID, METHD_PON_GET, &send, &rcv)) < 0)
	{
		printf("svchost_invoke return fail!\n");
		goto error;
	}

	if ((ptmp = svchost_get_param(&rcv,1,&len)) == NULL)
	{
		printf("%s: get method fail [%d] \n",__FUNCTION__,len);
		ret = -1;
		goto error;
	}
	memcpy(pdata, ptmp, len);	
	
error:
	svchost_free_pbuf(&send);
	svchost_free_pbuf(&rcv);
	return ret;
}
#endif
#endif

static int cfg_send_traffic_status_event( char* val) 
{ 
	unsigned int len = (sizeof(val) < CFG_MAX_ATTR_VALUE) ? sizeof(val) : CFG_MAX_ATTR_VALUE;

    if(cfg_send_event(EVT_PON_EXTERNAL,EVT_PON_TRAFFIC_STATUS,(void *)(val),len) == -1)
    {
    	printf("%s: cfg_send_event fail\n",__FUNCTION__);
		return -1;
    }   
	return 0;
} 

int getLosSta(char *buffer)
{
	int fd, losStatus, ret;
	char string[4] = {0};
	fd = open(LOS_STATUS_PATH,O_RDONLY|O_NONBLOCK);

	sprintf(buffer, "down");
	
	if (fd != -1) 
	{
		ret = read(fd, string, sizeof(string));
		close(fd);

		if(ret) 
		{
			losStatus = atoi(string);
			if(1 == losStatus) 
			{
				strcpy(buffer, "up");
			}
		}
	}
	else 
	{
		printf("no file %s\n",LOS_STATUS_PATH);
		
	}

	return 0;
}

static int cfg_type_xpon_common_func_get(char* path,char* attr, char* val,int len) 
{ 
	char buffer[8] = {0};
	unsigned long duration = 0;
	
	if(strcmp(attr,PONCOMMON_LOS_STATUS) == 0) 
	{		
		if (getLosSta(buffer) == 0)
		{
			cfg_set_object_attr(XPON_COMMON_NODE, attr, buffer);
		}
	}else if(strcmp(attr,PONCOMMON_ATTR_DATA_UP_TIME) == 0){
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
		if (cfg_pon_get_info(METHD_XPON_ONLINE_DURATION, (void *)&duration) == 0)
		{
			sprintf( buffer, "%ld", duration);
			cfg_set_object_attr(XPON_COMMON_NODE, attr, buffer);
		}
#endif
#endif
	}	
	return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_xpon_common_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_XPON,XPON_CHILD_COMMON,CFG_PON_SET,attr,val);

	if(0 == strcmp("trafficStatus",attr)){
		cfg_send_traffic_status_event(val);
	}
    
	return cfg_type_default_func_set(path,attr,val); 
} 


static cfg_node_ops_t cfg_type_xpon_common_ops  = { 
	 .get = cfg_type_xpon_common_func_get, 
	 .set = cfg_type_xpon_common_func_set,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_xpon_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_xpon, 
	 .ops = &cfg_type_xpon_common_ops, 
}; 


static int cfg_type_xpon_linkcfg_func_get(char* path,char* attr, char* val,int len) 
{ 
	char buffer[8] = {0};
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
	WAN_LINKCFG_t sysLinkCfg = {0};
#endif
#endif
	if(strcmp(attr,PONCOMMON_ATTR_XPON_LINK_STA) == 0) 
	{	
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
		if (cfg_pon_get_info(METHD_SYS_LINK_CFG, (void *)&sysLinkCfg)){
			printf("%s: get method SYS_LINK_CFG\n",__FUNCTION__);
			return -1;
		}
		
		sprintf(buffer, "%d", sysLinkCfg.linkStatus);
#endif
#endif
		cfg_set_object_attr(XPON_LINKCFG_NODE, attr, buffer);
	}
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_xpon_linkcfg_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_XPON,XPON_CHILD_LINKCFG,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_xpon_linkcfg_ops  = { 
	 .get = cfg_type_xpon_linkcfg_func_get, 
	 .set = cfg_type_xpon_linkcfg_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_xpon_linkcfg = { 
	 .name = "LinkCfg", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_xpon, 
	 .ops = &cfg_type_xpon_linkcfg_ops, 
}; 

static cfg_node_ops_t cfg_type_xpon_tr069_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_xpon_tr069 = { 
	 .name = "TR069", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_xpon, 
	 .ops = &cfg_type_xpon_tr069_ops, 
}; 

static cfg_node_ops_t cfg_type_xpon_voice_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_xpon_voice = { 
	 .name = "VOICE", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_xpon, 
	 .ops = &cfg_type_xpon_voice_ops, 
}; 

static cfg_node_ops_t cfg_type_xpon_iphost1_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_xpon_iphost1 = { 
	 .name = "IPHost1", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_xpon, 
	 .ops = &cfg_type_xpon_iphost1_ops, 
}; 

static cfg_node_ops_t cfg_type_xpon_iphost2_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t cfg_type_xpon_iphost2 = { 
	 .name = "IPHost2", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_xpon, 
	 .ops = &cfg_type_xpon_iphost2_ops, 
}; 


static cfg_node_ops_t cfg_type_xpon_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_xpon_child[] = { 
	 &cfg_type_xpon_common, 
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
	 &cfg_type_xpon_linkcfg, 
	 &cfg_type_xpon_tr069, 
	 &cfg_type_xpon_voice,
	 &cfg_type_xpon_iphost1,
	 &cfg_type_xpon_iphost2,
#endif
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_xpon = { 
	 .name = "XPON", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_xpon_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_xpon_child, 
	 .ops = &cfg_type_xpon_ops, 
}; 
