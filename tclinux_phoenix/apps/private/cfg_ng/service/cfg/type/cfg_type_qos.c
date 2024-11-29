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
#include <stdlib.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 
#include "svchost_evt.h"
#include "blapi_traffic.h"


#define 		MAX_TYPE_RULE_NUM	10
#define 		MAX_APP_RULE_NUM		4
#define 		MAX_TYPE_NUM			10


static char* cfg_type_qos_index[] = { 
	 "qos_id", 
	 NULL 
}; 

int svc_cfg_boot_qos(void)
{
	wan_related_evt_t param;

	memset(&param, 0, sizeof(param));
	strncpy(param.buf, QOS_COMMON_NODE, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_QOS_BOOT, (void *)&param, sizeof(param)); 
	return 0;
}

static int cfg_type_qos_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 



static void QosTemplateModeSwitch(void)
{
	char buf[64] = {0};
	int i = 1;
	char attr_buf[32] = {0};
	char *p = NULL;
	unsigned char priority[T_MAX] = {0};
	int enable_que_num = 0;
	
	bzero(priority, sizeof(priority));
	if(cfg_obj_get_object_attr(QOS_COMMON_NODE, "Mode", 0,buf, sizeof(buf)) > 0)
	{
		cfg_obj_set_object_attr(GLOBALSTATE_COMMON_NODE, "QosTemplateMode", 0, buf);
		p=strtok(buf, ","); 
		while(p){
#if defined(TCSUPPORT_CWMP)
			if(strstr(p, "TR069")){
				priority[T_TR069] = i;
				i++;
			}else
#endif
		
#if defined(TCSUPPORT_VOIP)
			if(strstr(p, "VOIP")){
				priority[T_VOIP] = i;
				i++;
			}else
#endif
			if(strstr(p, "IPTV")){
				priority[T_IPTV] = i;
				i++;
			}else
			
#if !defined(TCSUPPORT_CWMP) && !defined(TCSUPPORT_VOIP)
			if(strstr(p, "INTERNET")){
				priority[T_INTERNET] = i;
				i++;
			}else
#endif	

#if defined(TCSUPPORT_CT_JOYME4)
			if(strstr(p, "SPECIAL_SERVICE")){
				priority[T_SPECIAL_SERVICE] = i;
				i++;
			}
#endif
			p=strtok(NULL, ",");
		}
		
		/*enable queue(1~ enable_que_num)*/
		for(i = 1; i < T_MAX; i++){
			if(priority[i] != 0){
				enable_que_num++;
				memset(attr_buf, 0, sizeof(attr_buf));
				snprintf(attr_buf,sizeof(attr_buf), "QueueSW%d", priority[i]);
				cfg_obj_set_object_attr(QOS_COMMON_NODE, attr_buf, 0, "Yes");
			}
		}

		/* disable queue (enable_que_num+1 ~ MAX) */
		for (i = enable_que_num+1; i < MAX_QUEUE_NUM + 1; i++) {
			memset(attr_buf, 0, sizeof(attr_buf));
			snprintf(attr_buf,sizeof(attr_buf), "QueueSW%d", i);
			cfg_obj_set_object_attr(QOS_COMMON_NODE, attr_buf, 0, "No");
		}	
	}
}
#if defined(TCSUPPORT_CMCCV2)
int qoscmd_del_entry_rules(char* qosPath)
{
	char tmpMaxName[32] = {0}, tmpMinName[32] = {0}, tmpTypeName[32] = {0};
	char tmpMaxVal[64] = {0}, tmpMinVal[64] = {0}, tmpTypeVal[32] = {0};
	int i = 0;
	
	for (i = 0; i < MAX_TYPE_NUM; i ++)
	{
		memset(tmpMinVal, 0, sizeof(tmpMinVal));
		memset(tmpMaxVal, 0, sizeof(tmpMaxVal));
		memset(tmpTypeVal, 0, sizeof(tmpTypeVal));

		memset(tmpMinName, 0, sizeof(tmpMinName));
		memset(tmpMaxName, 0, sizeof(tmpMaxName));
		memset(tmpTypeName, 0, sizeof(tmpTypeName));

		snprintf(tmpTypeName, sizeof(tmpTypeName), "tmpType%d", i + 1);
		snprintf(tmpMaxName, sizeof(tmpMaxName), "tmpMax%d", i + 1);
		snprintf(tmpMinName, sizeof(tmpMinName), "tmpMin%d", i + 1);

			if ((cfg_get_object_attr(qosPath, tmpMinName, tmpMinVal, sizeof(tmpMinVal)) > 0) &&
					(cfg_get_object_attr(qosPath, tmpMaxName, tmpMaxVal, sizeof(tmpMaxVal)) > 0) &&
					(cfg_get_object_attr(qosPath, tmpTypeName, tmpTypeVal, sizeof(tmpTypeVal)) > 0))
				{
				/* Use tmp value to delete traffic classify rule first */
				blapi_traffic_set_traffic_class(tmpTypeVal, atoi(tmpMinVal), atoi(tmpMaxVal), DEL_TRAFFIC_CLASS);
			}
		}
	
	return 0;
}

int cfg_type_qos_func_delete(char* path)
{
	char delEntry[8] = {0};
	char QoSDelPath[32] = {0};

	
	if(cfg_obj_get_object_attr(QOS_COMMON_NODE, "delEntry", 0, delEntry, sizeof(delEntry)) > 0 && delEntry[0] != '\0')
	{
		int entry_del = atoi(delEntry) + 1;
		snprintf(QoSDelPath, sizeof(QoSDelPath), QOS_ENTRY_NODE, entry_del);
		qoscmd_del_entry_rules(QoSDelPath);
	}

	
	return cfg_type_default_func_delete(path);
}
#endif
static int cfg_type_qos_func_commit(char* path)
{
	char tag[8] = {0};
	wan_related_evt_t param;
	
#if defined(TCSUPPORT_CWMP_TR181)
	updateTr181QosByQoSNode();
#endif
	if(cfg_obj_get_object_attr(QOS_COMMON_NODE, "SelectTag", 0,tag, sizeof(tag)) >= 0 && !strcmp(tag,"1"))
	{
		/*if change Qos template, only need to set Queue active/deactive */
		QosTemplateModeSwitch();
		cfg_obj_set_object_attr(QOS_COMMON_NODE, "SelectTag", 0,"0");
		return 0;
	}
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_QOS_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
} 

static cfg_node_ops_t cfg_type_qos_entry_ops  = { 
	 .get = cfg_type_qos_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
#if defined(TCSUPPORT_CMCCV2)
	 .delete = cfg_type_qos_func_delete, 
#else
	 .delete = cfg_type_default_func_delete,
#endif
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_qos_func_commit 
}; 


static cfg_node_type_t cfg_type_qos_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 30, 
	 .parent = &cfg_type_qos, 
	 .index = cfg_type_qos_index, 
	 .ops = &cfg_type_qos_entry_ops, 
}; 

static void  cfg_type_set_qos_common_curTypeIdx(void)
{
	int i = 0;
	char nodePath[64] = {0};
	char  buf[10] = {0},idx[10] = {0};
	
	/* get curTypeIdx */
	for ( i = 1; i <= MAX_TYPE_RULE_NUM; i++ ) {
		snprintf(nodePath, sizeof(nodePath), QOS_ENTRY_NODE, i);
		if(cfg_obj_get_object_attr(nodePath, "ActQueue", 0, buf, sizeof(buf)) <= 0)
		{
			/* it means this Entry is empty */
			snprintf(idx, sizeof(idx), "%d", i -1);
			cfg_set_object_attr(QOS_COMMON_NODE, "curTypeIdx", idx);
			break;
		}
	}
	if ( i == MAX_TYPE_RULE_NUM + 1 )
	{
		cfg_set_object_attr(QOS_COMMON_NODE, "curTypeIdx", "N/A");
	}

	return ;
}

static void  cfg_type_set_qos_common_curAppIdx(void)
{
	int i = 0;
	char nodePath[64] = {0};
	char  buf[10] = {0},idx[10] = {0};
	
	/* get cufAppIdx */
	for ( i = MAX_TYPE_RULE_NUM + 1; i <= MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM; i++ ) {
		snprintf(nodePath, sizeof(nodePath), QOS_ENTRY_NODE, i);
		if(cfg_obj_get_object_attr(nodePath, "ActQueue", 0, buf, sizeof(buf)) <= 0)
		{
			/* it means this Entry is empty */
			snprintf( idx,sizeof(idx), "%d", i-1);
			cfg_set_object_attr(QOS_COMMON_NODE, "curAppIdx", idx);
			break;
		}
	}
	if ( i == MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM +1 ) 
	{
		cfg_set_object_attr(QOS_COMMON_NODE, "curAppIdx", "N/A");
	}
	
	return ;
}

static int  cfg_type_set_qos_common_curSubTypeIdx(void)
{
	int i = 0;
	int entry_index = 0;
	char nodePath[64] = {0};
	char idxBuf[64] = {0}, typeName[20] = {0}, typeVal[20] = {0},idx[10] = {0};;
	
	/* get curSubTypeIdx */
	if(cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "qos_id", 0, idxBuf, sizeof(idxBuf)) > 0)
	{
		entry_index = atoi(idxBuf);
	}
	else
	{
		return -1;
	}

	snprintf(nodePath, sizeof(nodePath), QOS_ENTRY_NODE, entry_index);
	for (i = 0; i < MAX_TYPE_NUM; i++) {
		snprintf(typeName, sizeof(typeName), "Type%d", i + 1);
		if(cfg_obj_get_object_attr(nodePath, typeName, 0, typeVal, sizeof(typeVal)) <= 0 || (!strcmp(typeVal, "N/A")))
		{
			/* it means this Entry is empty */
			snprintf( idx, sizeof(idx), "%d", i + 1 );
			cfg_set_object_attr(QOS_COMMON_NODE, "curSubTypeIdx", idx );
			break;
		}
	}
	if (i == MAX_TYPE_NUM)
	{
		cfg_set_object_attr(QOS_COMMON_NODE, "curSubTypeIdx", "N/A");
	}

	return 0;
}

static int cfg_type_qos_common_func_get(char* path,char* attr, char* val,int len) 
{ 	
	if (!strcmp(attr, "curTypeIdx") != 0)
	{
		cfg_type_set_qos_common_curTypeIdx();	
	}
	else if (!strcmp(attr, "curAppIdx")) 
	{
		cfg_type_set_qos_common_curAppIdx();	
	}
	else if (!strcmp(attr, "curSubTypeIdx"))
	{
		if( cfg_type_set_qos_common_curSubTypeIdx() == -1)
		{
			return 0;
		}
	}
	
	 return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_qos_common_ops  = { 
	 .get = cfg_type_qos_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_qos_func_commit 
}; 


static cfg_node_type_t cfg_type_qos_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_qos, 
	 .ops = &cfg_type_qos_common_ops, 
}; 


static cfg_node_ops_t cfg_type_qos_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_qos_func_commit 
}; 


static cfg_node_type_t* cfg_type_qos_child[] = { 
	 &cfg_type_qos_entry, 
	 &cfg_type_qos_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_qos = { 
	 .name = "QoS", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_qos_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_qos_child, 
	 .ops = &cfg_type_qos_ops, 
}; 
