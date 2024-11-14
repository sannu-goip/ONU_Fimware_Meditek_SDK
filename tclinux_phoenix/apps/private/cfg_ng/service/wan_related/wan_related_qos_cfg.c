

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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "libapi_lib_qosrule.h"

#include <svchost_api.h> 
#include <cfg_api.h>

#include "wan_related_mgr.h"
#include "wan_related_qos_cfg.h"
#include "cfg_cli.h"
#include "utility.h"
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
#include <lan_port/lan_port_info.h>
#include <lan_port/bind_list_map.h>
#endif

#include "blapi_traffic.h"
#include <ecnt_event_global/ecnt_event_global.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include "blapi_perform.h"
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef  TCSUPPORT_NP
#include "blapi_xpon.h"
#endif
#endif
#include <arpa/inet.h>

char appmode[64] = {0};

#if defined(TCSUPPORT_CT_JOYME2)
char oldtr69IP[32] = "0.0.0.0";
#endif

static int get_entry_number_by_path(char* path, char* keyword, int* index)
{
	char* tmp = NULL;
	char* ptr = NULL;

	if(path == NULL || keyword == NULL || index == NULL)
	{
		return -1;
	}
	ptr = strstr(path, keyword);
	if(ptr != NULL)
	{
		tmp = ptr + strlen(ptr) - 1;
		*index = atoi(tmp);
		return 0;
	}
	else
	{
		return -1;
	}
}

int getCurrentIndex(char* path)
{
	char idxBuf[64];
	int index = -1;
	
	if ( get_entry_number_by_path(path, "Entry", &index) != 0 )
	{
		if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "qos_id", idxBuf, sizeof(idxBuf)) > 0)
		{
			index = atoi(idxBuf);
		}
		else
		{
			return -1;
		}
	}

	return index;
}

int setQueueMask ( char *qm, char sw_pri[][32], int qNum )
{
	int i, q = 0;
	if (qm == NULL)
		return -1;
	
	for (i = 0; i < qNum; i++) {
		if ( strcmp(sw_pri[i], "Yes") == 0 ) {
			q += (1 << i);
		}
	}
	
#ifdef QOS_DEBUG
	printf(" q is %d\n", q );
#endif

	sprintf( qm, "%d", q );
	return 0;
}

int qosEBTModule( int type )
{
	int i;
	char modeCmd[128];
	char vername[CFG_BUF_16_LEN] = {0}, module_path[CFG_BUF_128_LEN] = {0};
	char qos_ebt_name[QOS_EBT_MODULE_NUM][32] =
	{
#if defined(TCSUPPORT_CT_VLAN_BIND)
		{"ebt_ip.ko"},{"ebt_ftos.ko"}
#else
		{"ebtables.ko"},{"ebtable_broute.ko"},{"ebt_ip.ko"},
		{"ebt_vlan.ko"},{"ebt_ftos.ko"},{"ebt_mark.ko"}
#endif
#ifdef IPV6
		,{"ebt_ip6.ko"},{"ebt_tc.ko"}
#endif
	};

	decideModulePath(vername, CFG_BUF_16_LEN);
	snprintf(module_path, sizeof(module_path), "/lib/modules/%s/kernel/net/bridge/netfilter/", vername);

	memset( modeCmd, 0, sizeof(modeCmd) );
	for ( i=0; i<QOS_EBT_MODULE_NUM; i++ ) {
		if ( type == INSMODE ) {
			snprintf( modeCmd,sizeof(modeCmd),"insmod %s%s\n", module_path, qos_ebt_name[i] );
		}
		else if ( type == RMMODE ) {
			snprintf( modeCmd,sizeof(modeCmd),"rmmod %s%s\n", module_path, qos_ebt_name[i] );
		}
		else {
			return -1;
		}
		system(modeCmd);
	}

	return 0;
}

void deleteQosTempRules()
{
	int i;
	for(i = 0; i < MAX_APP_RULE_NUM; i++)
	{
		qoscmd_lib_do_qos_app(TYPE_DEL, i, IPNULL_FLAG, NULL);
	}
	
	for(i = MAX_TYPE_RULE_NUM; i < MAX_APP_TYPE_NUM; i++)
	{
		qoscmd_lib_do_qos_type(TYPE_DEL, i, IPNULL_FLAG, 0, NULL);
	}
	
	for(i = MAX_TYPE_RULE_NUM; i < MAX_APP_TYPE_NUM; i++)
	{
		qoscmd_lib_do_qos_remark(TYPE_DEL, i, IPNULL_FLAG, 0, NULL);
	}
	return;	
}

void setWanIfQoS(char *wanIfName, char *MinVal, char *MaxVal)
{
	char ifBuf[64], ifName[10];
	int i,j, pvc_idx_begin = -1, if_idx_begin = -1, pvc_idx_end = -1, if_idx_end = -1;
	char pvc_arr[10], if_arr[10];
	char *p = NULL;
	char nodePath[64] = {0};
	
	if (wanIfName == NULL || MinVal == NULL || MaxVal == NULL)
	{
		return;
	}
	
	memset(pvc_arr, 0, sizeof(pvc_arr));
	memset(if_arr, 0, sizeof(if_arr));
	if ('\0' != MinVal[0]) {
		p = strstr(MinVal, ",");
		if (NULL != p) {
			strncpy(pvc_arr, MinVal, p - MinVal);
			strncpy(if_arr, p + 1,sizeof(if_arr)-1);
		}

		pvc_idx_end = pvc_idx_begin = atoi(pvc_arr);
		if_idx_end = if_idx_begin = atoi(if_arr);

		/* end pvc and inteface index */
		if ('\0' != MaxVal[0]) {
			p = strstr(MaxVal, ",");
			if (NULL != p) {
				strncpy(pvc_arr, MaxVal, p - MaxVal);
				strncpy(if_arr, p + 1,sizeof(if_arr)-1);
			}

			pvc_idx_end = atoi(pvc_arr);
			if_idx_end = atoi(if_arr);
		}
	}

	/* get interface from wan node */
	memset(ifBuf, 0, sizeof(ifBuf));
	if (pvc_idx_begin > 0 && if_idx_begin > 0 && pvc_idx_end > 0 && if_idx_end > 0) {
		for (i = pvc_idx_begin; i <= pvc_idx_end; i++) {
			for (j = if_idx_begin; j <= if_idx_end; j++) {
				memset(nodePath, 0, sizeof(nodePath));
				//snprintf(nodePath, sizeof(nodePath), "root.wan.pvc.%d.entry.%d", i-1, j-1);
				snprintf(nodePath, sizeof(nodePath), WAN_PVC_ENTRY_NODE, i, j);
				if(cfg_get_object_attr(nodePath, "NASName", ifName, sizeof(ifName)) > 0)
				{
					if ('\0' != ifName[0]) {
						if ('\0' == ifBuf[0]) {
							strncpy(ifBuf, ifName,sizeof(ifBuf)-1);
						}
						else {
							/* if buf can not be over then 64 */
							if (strlen(ifBuf) < 48) {
								snprintf(ifBuf,sizeof(ifBuf), "%s,%s", ifBuf, ifName);
							}
						}
					}
				}
			}
		}
	}

	if ('\0' != ifBuf[0]) 
	{
		strncpy(wanIfName, ifBuf,LENTH_CHAR_ARR_32 -1);
	}
	
	return;
}


int qosWanInterfaceUpdate( int on_off, char *path)
{
	int idx = 0, i = 0;
	char buf[CMD_BUF_LENGTH] = {0};
	char typeName[20] = {0}, typeVal[20] = {0};
	char MaxName[10] = {0}, MinName[10] = {0}, MaxVal[64] = {0}, MinVal[64] = {0};
	unsigned char wantype_set[MAX_TYPE_RULE_NUM + 1] = {0};
	char wantype_name[MAX_TYPE_RULE_NUM][32];
	char dotp[10] = {0};
	int wantype_mark_set[MAX_TYPE_RULE_NUM + 1] = {0}, buf_val = 0;
	char dscpEnable[10] = {0};
	char queueBuf[10] = {0};
	char qosPath[64] = {0};
	qos_wanIf_value_t wanIfValue;
	char commNode[64] = {0};
	int direction = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	char *token = NULL, *safep = NULL;
#endif

	memset(wantype_name, 0, sizeof(wantype_name));

#if defined(TCSUPPORT_CT_JOYME4)
	if( strstr(path, "downlinkqos") )
	{
		direction = 1;
		snprintf(commNode, sizeof(commNode), DOWNLINKQOS_COMMON_NODE);
	}
	else
#endif
	{
		direction = 0;
		snprintf(commNode, sizeof(commNode), QOS_COMMON_NODE);
	}

	if ( 0 == direction )
	{
		/* reset all entry */
		for ( idx = 0; idx < MAX_TYPE_RULE_NUM; idx++ )
		{
			memset(&wanIfValue, 0, sizeof(qos_wanIf_value_t));
			wanIfValue.index = idx;
			blapi_traffic_set_wanInterface_qostype(&wanIfValue);
		}
	}

	if ( 0 == on_off && 0 == direction )
		return 0;

	bzero(wantype_name, sizeof(wantype_name));
	for ( idx = 1; idx <= MAX_TYPE_RULE_NUM; idx++ )
	{	
		memset(qosPath, 0, sizeof(qosPath));
#if defined(TCSUPPORT_CT_JOYME4)
		if( direction )
		{
			snprintf(qosPath, sizeof(qosPath), DOWNLINKQOS_ENTRY_NODE, idx);
		}
		else
#endif
		{
		snprintf(qosPath, sizeof(qosPath), QOS_ENTRY_NODE, idx);
		}
		bzero(buf, sizeof(buf));

		if(cfg_get_object_attr(qosPath, "ActQueue", buf, sizeof(buf)) > 0 && 0 != buf[0])
		{
			for ( i = 0; i < MAX_TYPE_NUM; i++ )
			{
				snprintf(typeName,sizeof(typeName), "Type%d", i + 1);
				snprintf(MaxName,sizeof(MaxName), "Max%d", i + 1);
				snprintf(MinName,sizeof(MinName), "Min%d", i + 1);
				bzero(MaxVal, sizeof(MaxVal));
				bzero(MinVal, sizeof(MinVal));

				if(cfg_get_object_attr(qosPath, typeName, typeVal, sizeof(typeVal)) > 0 && 
					cfg_get_object_attr(qosPath, MaxName, MaxVal, sizeof(MaxVal)) > 0 &&
					cfg_get_object_attr(qosPath, MinName, MinVal, sizeof(MinVal)) > 0)
				{
					if (!strcmp(typeVal, "N/A") || !strcmp(MaxVal, "N/A")
						|| !strcmp(MinVal, "N/A"))
						continue;

					if ( !strcmp(typeVal, "WANInterface") )
					{
						/* wan interface */
						setWanIfQoS(wantype_name[idx], MinVal, MaxVal);
						wantype_set[idx] = 1;
						snprintf(queueBuf, sizeof(queueBuf), "QueueP%d", atoi(buf));
						memset(buf, 0, sizeof(buf));
						if(cfg_get_object_attr(commNode, queueBuf, buf, sizeof(buf)) > 0)
						{
							if (atoi(buf) > 0 && atoi(buf) < 7) {
								wantype_mark_set[idx] |=
									((atoi(buf) & 0x0F) << QOSTYPE_QUEUE_INDEX_OFFSET);
							}
						}
					}
				}
			}
		}

		/* 802.1p remark */
		bzero(buf, sizeof(buf));
		if(cfg_get_object_attr(qosPath, "Act8021pRemarkingNum", buf, sizeof(buf)) > 0 && 0 != buf[0])
		{
			if(cfg_get_object_attr(commNode, "En8021PRemark", dotp, sizeof(dotp)) > 0)
			{
				/* 8~11 bits used for 802.1p */
				if ( !strcmp(dotp, "0") )
				{
					/* zero mark */
					wantype_mark_set[idx] |= (0 << QOSTYPE_PBIT_MARK_OFFSET);
				}
				else if ( !strcmp(dotp, "1") ) 
				{
					/* pass through */
					wantype_mark_set[idx] |= (9 << QOSTYPE_PBIT_MARK_OFFSET);
				}
				else
				{
					/* remark */
					buf_val = atoi(buf);
					wantype_mark_set[idx] |= 
						((buf_val & 0x0F)  << QOSTYPE_PBIT_MARK_OFFSET);
				}
			}
		}
		
		/* dscp or tc remark */
		memset(buf, 0, sizeof(buf));
		if(cfg_get_object_attr(qosPath, "ActDSCPRemarking", buf, sizeof(buf)) > 0)
		{
			if(cfg_get_object_attr(commNode, "EnableDSCPMark", dscpEnable, sizeof(dscpEnable)) > 0)
			{
				if ( !strcmp(dscpEnable, "Yes") && 0 != buf[0] )
				{
					buf_val = atoi(buf);
#if defined(TCSUPPORT_CT_JOYME4)
					if( direction )
					{
						wantype_mark_set[idx] = buf_val;
					}
					else
#endif
					{
						wantype_mark_set[idx] |= (1 << QOSTYPE_DSCP_SW_MARK_OFFSET);
						wantype_mark_set[idx] |= (buf_val & QOSTYPE_DSCP_MARK);
					}
				}
			}
		}
	}

	/* set wan interface */
	for ( idx = 0; idx < MAX_TYPE_RULE_NUM; idx++ )
	{
		if ( 0 != wantype_set[idx] )
		{
#if defined(TCSUPPORT_CT_JOYME4)
			if( direction )
			{
				token = strtok_r(wantype_name[idx], ",", &safep);
				while( token )
				{
					qoscmd_lib_do_downlinkqos_dscpremark(on_off, token, wantype_mark_set[idx]);
					token = strtok_r(NULL, ",", &safep);
				}
			}
			else
#endif
			{
				memset(&wanIfValue, 0, sizeof(qos_wanIf_value_t));
				wanIfValue.index = idx;
				wanIfValue.wanType_mask = wantype_mark_set[idx];
				strncpy(wanIfValue.wanType_name, "wan_if", sizeof(wanIfValue.wanType_name) - 1);
				strncpy(wanIfValue.wanType_value, wantype_name[idx], sizeof(wanIfValue.wanType_value) - 1);
				blapi_traffic_set_wanInterface_qostype(&wanIfValue);
			}
		}
	}
	
	return 0;
}

int svc_wan_related_set_qos_switch_rule(int sw )
{
	/* 0: qos is on 1: qos is off */
	static int qosFlag = 0;
	int oldQosFlag = -1;  
	int IPversion = -1;	
	int i = 0; 
	unsigned int new_filter_state = 0;   
	char nodePath[64] = {0};
	char newFilterState[32] = {0};
	
	if ( sw == qosFlag )
	{
		return oldQosFlag;
	}

#ifdef IPV6
	IPversion = IPALL_FLAG;
#else
	IPversion = IPV4_FLAG;
#endif

	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	if ( sw == 1 )
	{
		/* 1. insmode ebtables modules */
		qosEBTModule( INSMODE );
		/* 2. insmode iptables modules */
		cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;
		new_filter_state = atoi(newFilterState) | QOS_VECTOR;
		check_and_set_filter( new_filter_state );
		
		/* 3. create ebtables & iptables chains */
		qoscmd_lib_do_qos_switch(SW_ON, IPversion);
	}
	else if ( sw == 0 )
	{
		/* 1. del ebtables & iptables chains */
		qoscmd_lib_do_qos_switch(SW_OFF, IPversion);
		/* 2. rmmode ebtables modules */
		qosEBTModule( RMMODE );
		
		/* 3. rmmode iptables modules */
		cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;
		new_filter_state = atoi(newFilterState) & (~QOS_VECTOR);
		check_and_set_filter( new_filter_state );
		
		/* delete vlan rule */
		for ( i = 1; i <= MAX_VLAN_RULE_NUM; i++ ) {
			svc_wan_related_set_qos_vlanid_rule( i + MAX_TYPE_RULE_NUM + MAX_TEM_RULE_NUM, 0);
		}
	}
	else 
	{
		return -1;
	}

	/* save the current qos state */
	oldQosFlag = qosFlag;
	qosFlag = sw;

	return oldQosFlag;
}

/* vlanid Entry attribute: Active vpidotvci vlanid*/
int svc_wan_related_set_qos_vlanid_rule(int index, int type )
{
	char buf[CMD_BUF_LENGTH] = {0};
	int sw =0, vpi = 0, vci = 0, vlanid =0;
	char Cvpi[4] = {0}, Cvci[4] = {0};
	char qosPath[64] = {0}, wanPath[64] = {0};
	
	index -= (MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM + MAX_TEM_RULE_NUM);
	if ( index < 0 || index > MAX_VLAN_RULE_NUM - 1 ) 
	{
		return -1;
	}
	snprintf(qosPath,sizeof(qosPath), QOS_ENTRY_NODE, index + MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM + MAX_TEM_RULE_NUM );
	if (type == 1) 
	{
		/* 1. Active */
		if(cfg_get_object_attr(qosPath, "Active", buf, sizeof(buf)) > 0) 
		{
			if ( strcmp(buf, "Yes") == 0 ) {
				sw = SW_ON;
			}
			else if ( strcmp(buf, "No") == 0 ) {
				sw = SW_OFF;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	else {
		sw = SW_OFF;
	}

	/* 2. vpidotvci get vpi and vci from wan node */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(qosPath, "pvcIndex", buf, sizeof(buf)) > 0) 
	{
		if (!strlen(buf) || atoi(buf) < 0 || atoi(buf) > 8)
		{
			return -1;
		}
		
		snprintf(wanPath,sizeof(wanPath),WAN_PVC_NODE, atoi(buf));
		if(cfg_get_object_attr(wanPath, "VPI", Cvpi, sizeof(Cvpi)) > 0) 
		{
			if(cfg_get_object_attr(wanPath, "VCI", Cvpi, sizeof(Cvci)) > 0)
			{
				 vpi = atoi(Cvpi);
				 vci = atoi(Cvci);
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}

	/* 3. vlanid */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(qosPath, "vlanid", buf, sizeof(buf)) > 0)
	{
		if ( strlen(buf) > 0 ) {
			vlanid = atoi(buf);
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}

	qoscmd_lib_do_qos_vlanid(sw, vpi, vci, vlanid);
	return 0;
}

void svc_wan_related_set_qos_temp_rule(int type, char *path)
{
	char buf[CMD_BUF_LENGTH] = {0};
	char tmpbuf[CMD_BUF_LENGTH] = {0};
	char attr_buf[32] = {0};
	char GlobalQosBuf[64] = {0};
#if defined(TCSUPPORT_CT_VOIP_QOS) 
	char qosCmd[MAX_CMD_LENGTH];
#endif
	char *p = NULL;
	unsigned char priority[T_MAX] = {0};
	int i = 1;
	int enable_que_num = 0;
#if defined(TCSUPPORT_CWMP) && defined(TCSUPPORT_VOIP)
	char temp[8] = {0};
	int tr69_index = 1, voip_index = 1;
#endif
	char qosPath[64] = {0};
	
	bzero(priority, sizeof(priority));
	if(cfg_get_object_attr(QOS_COMMON_NODE, "Mode", buf, sizeof(buf)) > 0)
	{
		if (strcmp(appmode,buf) == 0) 
		{
			return;	
		}	
		if (type == 1)
		{
			/*when set template qos rules,delete the old rules first */
			deleteQosTempRules();
		}
#if defined(TCSUPPORT_CT_VOIP_QOS) 
		/*clear rtp priority first */
		memset( qosCmd, 0, sizeof(qosCmd) );
		doValPut(RTP_PRIORITY_QUEUE_PATH, "0\n");
#endif

		strncpy(appmode,buf,sizeof(appmode)-1);
		/*reset attribute value */
		/*
		cfg_set_object_attr(QOS_COMMON_NODE, "UplinkBandwidth", "0");
		cfg_set_object_attr(QOS_COMMON_NODE, "Discipline", "PQ");
		cfg_set_object_attr(QOS_COMMON_NODE, "EnableForceWeight", "No");
		cfg_set_object_attr(QOS_COMMON_NODE, "EnableDSCP", "No");
		cfg_set_object_attr(QOS_COMMON_NODE, "En8021PRemark", "0");
		*/
		strncpy(tmpbuf,buf,sizeof(tmpbuf)-1);
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
#if defined(TCSUPPORT_CT_JOYME4)
			if(strstr(p, "SPECIAL_SERVICE")){
				priority[T_SPECIAL_SERVICE] = i;
				i++;
			}
#endif
			p=strtok(NULL, ",");
		}
		
		cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "QosTemplateMode", GlobalQosBuf,sizeof(GlobalQosBuf) );
		if (strcmp(GlobalQosBuf,tmpbuf) != 0 ) 
		{
		/*enable queue(1~ enable_que_num)*/
		for(i = 1; i < T_MAX; i++){
			if(priority[i] != 0){
				enable_que_num++;
				memset(attr_buf, 0, sizeof(attr_buf));
				snprintf(attr_buf,sizeof(attr_buf), "QueueSW%d", priority[i]);
				cfg_set_object_attr(QOS_COMMON_NODE, attr_buf, "Yes");
			}
		}
		/* disable queue (enable_que_num+1 ~ MAX) */
		for (i = enable_que_num+1; i < MAX_QUEUE_NUM + 1; i++) {
			memset(attr_buf, 0, sizeof(attr_buf));
			snprintf(attr_buf,sizeof(attr_buf), "QueueSW%d", i);
			cfg_set_object_attr(QOS_COMMON_NODE, attr_buf, "No");
		}
		}	
		
		if (type == 1) 
		{
			svc_wan_related_set_qos_discpline_rule( path );
		}
		/*clean type and app rule*/
		for(i = 1; i <= MAX_APP_TYPE_NUM; i++){
			memset(qosPath, 0, sizeof(qosPath));
			snprintf(qosPath, sizeof(qosPath),QOS_ENTRY_NODE, i);
			cfg_delete_object(qosPath);
		}
		
		/*set  type rule */
		for (i = 1; i <= QUEUE_NUM; i++)		
		{
			memset(qosPath, 0, sizeof(qosPath));
			snprintf(qosPath, sizeof(qosPath), QOS_ENTRY_NODE, i);
#if defined(TCSUPPORT_CT_JOYME4)
			/* NO need class type rules. */
			if ( cfg_query_object(qosPath, NULL, NULL) > 0 )
				cfg_delete_object(qosPath);
#else
			if(cfg_query_object(qosPath,NULL,NULL) < 0)
			{
				cfg_create_object(qosPath);      
			}
			if((priority[T_IPTV] != 0) && (1 == i)){

				cfg_set_object_attr(qosPath, "Active", "Yes");
				cfg_set_object_attr(qosPath, "ActDSCPRemarking", "0");
				cfg_set_object_attr(qosPath, "Act8021pRemarkingNum", "0");
				cfg_set_object_attr(qosPath, "Type1", "LANInterface");

				/*Lan2*/
				cfg_set_object_attr(qosPath, "Max1", "2");
				cfg_set_object_attr(qosPath, "Min1", "2");
				cfg_set_object_attr(qosPath, "ProtocolID", "all");
				snprintf(attr_buf, sizeof(attr_buf),"%d", priority[T_IPTV]);
				cfg_set_object_attr(qosPath, "ActQueue", attr_buf);
			}			
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
			else if ( 11 == i || 12 == i )
			{
				cfg_set_object_attr(qosPath, "Active", "No");
			}
#endif
			else{
#if defined(TCSUPPORT_CT_DBUS) 
				cfg_set_object_attr(qosPath, "Active", "");
#else
				cfg_set_object_attr(qosPath, "Active", "No");
#endif
				cfg_set_object_attr(qosPath, "ActDSCPRemarking", "0");
				cfg_set_object_attr(qosPath, "Act8021pRemarkingNum", "0");
				snprintf(attr_buf,sizeof(attr_buf), "%d", i  );
				cfg_set_object_attr(qosPath, "ActQueue", attr_buf);
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
				/*default type rule*/
				cfg_set_object_attr(qosPath, "Type1", "N/A");
				cfg_set_object_attr(qosPath, "Max1", "0");
				cfg_set_object_attr(qosPath, "Min1", "0");
				cfg_set_object_attr(qosPath, "ProtocolID", "all");
#endif
			}
#endif
			if (type == 1) {
				svc_wan_related_set_qos_type_rule( i );
			}
		}

#if defined(TCSUPPORT_CWMP) && defined(TCSUPPORT_VOIP)
		if((priority[T_TR069] != 0) && (priority[T_VOIP] != 0)){
			if(priority[T_TR069] < priority[T_VOIP]){
				tr69_index = 1;
				voip_index = 2;
			}
			else{
				voip_index = 1;
				tr69_index = 2;
			}
		}
		
		if(priority[T_TR069] != 0){
			/* tr069 to first queue */
			memset(qosPath, 0, sizeof(qosPath));
			snprintf(qosPath, sizeof(qosPath), QOS_ENTRY_NODE, (MAX_TYPE_RULE_NUM+tr69_index));
			if(cfg_query_object(qosPath,NULL,NULL) < 0)
			{
				cfg_create_object(qosPath);  
			}
			cfg_set_object_attr(qosPath, "Active", "Yes");
			cfg_set_object_attr(qosPath, "AppName", "TR069");

			memset(temp, 0, sizeof(temp));
			snprintf(temp,sizeof(temp), "%d", priority[T_TR069]);
			cfg_set_object_attr(qosPath, "ActQueue", temp);
			if (type == 1) {
				svc_wan_related_set_qos_app_rule((MAX_TYPE_RULE_NUM+tr69_index), 0);
			}
		}
		
		if(priority[T_VOIP] != 0){
			/* voip to second queue */
			memset(qosPath, 0, sizeof(qosPath));
			snprintf(qosPath, sizeof(qosPath), QOS_ENTRY_NODE, (MAX_TYPE_RULE_NUM+voip_index));
			if(cfg_query_object(qosPath, NULL, NULL) < 0)
			{
				cfg_create_object(qosPath);   
			}
			cfg_set_object_attr(qosPath, "Active", "Yes");
			cfg_set_object_attr(qosPath, "AppName", "VOIP");
			cfg_set_object_attr(qosPath, "ProtocolID", "rtp");
			
			memset(temp, 0, sizeof(temp));
			snprintf(temp,sizeof(temp), "%d", priority[T_VOIP]);
			cfg_set_object_attr(qosPath, "ActQueue", temp);
			if (type == 1) {
				svc_wan_related_set_qos_app_rule((MAX_TYPE_RULE_NUM+voip_index), 0);
			}
		}		
#endif
	}
	else {
			printf("get mode failed.\n");
	}
}

#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
static int prepare_itf_name(int mark, char* itf_name, int size)
{
	char buf[16];
	if(NULL == itf_name)
	{
		return -1;
	}
	
	memset(itf_name, 0, size);
	memset(buf, 0, sizeof(buf));
	if(0 != mark_convert_real_itf_name(mark, buf, sizeof(buf)))
	{
		return -1;
	}
#ifdef TCSUPPORT_CT_2PWIFI
	if ( (NULL != strstr(buf, M_ETHER_ITF_NAME_FORMAT)) \
		&& ((0 == strcasecmp(buf, ETHER_ITF_NAME_1)) || (0 == strcasecmp(buf, ETHER_ITF_NAME_2))) )
	{
		snprintf(itf_name, size, buf);
	}
#else
	if( NULL != strstr(buf, M_ETHER_ITF_NAME_FORMAT) )
	{
		snprintf(itf_name, size, buf);
	}
#endif
	else if( NULL != strstr(buf, WLAN_AC_ITF_NAME_FORMAT) )
	{
		snprintf(itf_name, size, buf);
	}
	else if( (NULL != strstr(buf, WLAN_ITF_NAME_FORMAT)) \
		&& (0 != strcasecmp(buf, WLAN_ITF_NAME_4)) )
	{
		snprintf(itf_name, size, buf);
	}
	else
	{
		return -1;
	}

	return 0;
}
#endif

int svc_wan_related_set_qos_type_rule(int index)
{
	char typeName[20] = {0}, typeVal[20] = {0};
	char MaxName[10] = {0}, MinName[10] = {0}, MaskName[10] = {0}, MaxVal[64] = {0}, MinVal[64] = {0}, MaskVal[64] = {0};
#if defined(TCSUPPORT_CMCCV2)
	char tmpMaxName[32] = {0}, tmpMinName[32] = {0}, tmpTypeName[32] = {0};
	char tmpMaxVal[64] = {0}, tmpMinVal[64] = {0}, tmpTypeVal[32] = {0};
#endif
	char qosCmdBuf[MAX_CMD_LENGTH] = {0}, buf[CMD_BUF_LENGTH] = {0};
	char queueBuf[10] = {0}, dotp[10] = {0}, dscpEnable[10] = {0};
	int i = 0, j = 0, min = 0, max = 0, queue = 0;
	int type = 0;
	int ruleIdx = 0;
	int IPversion = 0;
	int paramNum = 0;
	typeParam_t paramArr[MAX_TYPE_PARAM_NUM];
	char qosPath[64] = {0};
	int isGetProtol = 0;
	char wanIfName[32] = {0};
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
	char itf_name[16];
#else
	char g_lanif[][16] = 
	{
#ifdef TCSUPPORT_CT_2PWIFI
		"eth0.1", "eth0.2", "", "",
#else
		"eth0.1", "eth0.2", "eth0.3", "eth0.4", 
#endif
	     "ra0", "ra1", "ra2", "ra3"
#if defined(TCSUPPORT_WLAN_AC)
		,"rai0", "rai1", "rai2", "rai3"
#endif
	};
#endif

	if (index < 0 || index > MAX_TYPE_RULE_NUM + MAX_APP_RULE_NUM - 1) 
	{
		return -1;
	}

#ifdef IPV6
	IPversion = IPALL_FLAG;
#else
	IPversion = IPV4_FLAG;
#endif
	type = TYPE_ADD;
	ruleIdx = index -1;

	memset(paramArr, 0, sizeof(typeParam_t)*MAX_TYPE_PARAM_NUM);
	snprintf(qosPath, sizeof(qosPath), QOS_ENTRY_NODE, index);
	/* 1. have or no this entry */
	if(cfg_get_object_attr(qosPath, "Active", buf, sizeof(buf)) > 0)
	{
		if (strlen(buf) <= 0) 
		{
			goto DelQosTypeRule;
		}
	}
	else
	{
		goto DelQosTypeRule;
	}

	/* 1. match  */
	for (i = 0; i < MAX_TYPE_NUM; i++)
	{
		memset(typeVal, 0, sizeof(typeVal));
		memset(MaxVal, 0, sizeof(MaxVal));
		memset(MinVal, 0, sizeof(MinVal));
		memset(MaskVal, 0, sizeof(MaskVal));

		memset(typeName, 0, sizeof(typeName));
		memset(MaxName, 0, sizeof(MaxName));
		memset(MinName, 0, sizeof(MinName));
		memset(MaskName, 0, sizeof(MaskName));

#if defined(TCSUPPORT_CMCCV2)
		memset(tmpMinVal, 0, sizeof(tmpMinVal));
		memset(tmpMaxVal, 0, sizeof(tmpMaxVal));
		memset(tmpTypeVal, 0, sizeof(tmpTypeVal));

		memset(tmpMinName, 0, sizeof(tmpMinName));
		memset(tmpMaxName, 0, sizeof(tmpMaxName));
		memset(tmpTypeName, 0, sizeof(tmpTypeName));
#endif
	
		snprintf(typeName, sizeof(typeName), "Type%d", i + 1);
		snprintf(MaxName, sizeof(MaxName), "Max%d", i + 1);
		snprintf(MinName,sizeof(MinName), "Min%d", i + 1);
		snprintf(MaskName,sizeof(MaskName), "Mask%d", i + 1);

#if defined(TCSUPPORT_CMCCV2)
		snprintf(tmpTypeName, sizeof(tmpTypeName), "tmpType%d", i + 1);
		snprintf(tmpMaxName, sizeof(tmpMaxName), "tmpMax%d", i + 1);
		snprintf(tmpMinName, sizeof(tmpMinName), "tmpMin%d", i + 1);
#endif

		if((cfg_get_object_attr(qosPath, typeName, typeVal, sizeof(typeVal)) > 0) &&
			(cfg_get_object_attr(qosPath, MaxName, MaxVal, sizeof(MaxVal)) > 0) &&
			(cfg_get_object_attr(qosPath, MinName, MinVal, sizeof(MinVal)) > 0))
		{
#if defined(TCSUPPORT_CMCCV2)
			if ((cfg_get_object_attr(qosPath, tmpMinName, tmpMinVal, sizeof(tmpMinVal)) > 0) &&
					(cfg_get_object_attr(qosPath, tmpMaxName, tmpMaxVal, sizeof(tmpMaxVal)) > 0) &&
					(cfg_get_object_attr(qosPath, tmpTypeName, tmpTypeVal, sizeof(tmpTypeVal)) > 0))
			{/* Use tmp value to delete traffic classify rule first */
				blapi_traffic_set_traffic_class(tmpTypeVal, atoi(tmpMinVal), atoi(tmpMaxVal), DEL_TRAFFIC_CLASS);
				cfg_set_object_attr(qosPath, tmpMinName, "");
				cfg_set_object_attr(qosPath, tmpMaxName, "");
				cfg_set_object_attr(qosPath, tmpTypeName, "");
			}
#endif
			/* this type may be has been deleted */
			if (!strcmp(typeVal, "N/A") || !strcmp(MaxVal, "N/A") || !strcmp(MinVal, "N/A")) 
			{
				continue;
			}
			
			if(isGetProtol == 0)
			{
				/* get proto from QoS node */
				memset(buf, 0, sizeof(buf));
				if(cfg_get_object_attr(qosPath, "ProtocolID", buf, sizeof(buf)) > 0)
				{
					if ('\0' != buf[0]) 
					{
						strncpy(paramArr[paramNum].paramName, "proto", sizeof(paramArr[paramNum].paramName)-1);
						strncpy(paramArr[paramNum].value, buf, sizeof(paramArr[paramNum].value)-1);
						paramNum++;
						isGetProtol = 1;	
					}
				}
			}
			
			if (!strcmp(typeVal, "SMAC"))
			{
				if (!strcmp(MaxVal, MinVal) && '\0' != MaxVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "smac", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MaxVal, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
			}
#if defined(TCSUPPORT_CMCCV2)
			else if (!strcmp(typeVal, "DMAC")) 
			{
				
				if (!strcmp(MaxVal, MinVal) && '\0' != MaxVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "dmac", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MaxVal, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
			}
#endif
			else if (!strcmp(typeVal, "8021P")) 
			{
				if ('\0' != MinVal[0])
				{
					strncpy(paramArr[paramNum].paramName, "dot1p", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if ('\0' != MaxVal[0]) 
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s:%s", MinVal, MaxVal );
					}
#if defined(TCSUPPORT_CMCCV2)
					cfg_set_object_attr(qosPath, tmpMinName, MinVal);
					cfg_set_object_attr(qosPath, tmpMaxName, MaxVal);
					cfg_set_object_attr(qosPath, tmpTypeName, "Pbit");
					blapi_traffic_set_traffic_class("Pbit", atoi(MinVal), atoi(MaxVal), ADD_TRAFFIC_CLASS);
#endif
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "SIP")) 
			{
				if ('\0' != MinVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "sip", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if(cfg_get_object_attr(qosPath, MaskName, MaskVal, sizeof(MaskVal)) > 0){
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s/%s", MinVal, MaskVal );
					}else if ('\0' != MaxVal[0] && strcmp(MinVal, MaxVal) != 0) 
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s-%s", MinVal, MaxVal );
					}
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "DIP"))
			{
				if ('\0' != MinVal[0])
				{
					strncpy(paramArr[paramNum].paramName, "dip", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if ('\0' != MaxVal[0] && strcmp(MinVal, MaxVal) != 0)
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s-%s", MinVal, MaxVal );
					}
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "SPORT")) 
			{
				if ('\0' != MinVal[0])
				{
					strncpy(paramArr[paramNum].paramName, "sport", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if ('\0' != MaxVal[0])
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s:%s", MinVal, MaxVal );
					}
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "DPORT")) 
			{
				if ('\0' != MinVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "dport", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if ('\0' != MaxVal[0]) 
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s:%s", MinVal, MaxVal );
					}
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "DSCP")) 
			{
				if ('\0' != MinVal[0]) 
				{
#if defined(TCSUPPORT_CT_JOYME4)
					strncpy(paramArr[paramNum].paramName, "dscp", sizeof(paramArr[paramNum].paramName)-1);
#else
					strncpy(paramArr[paramNum].paramName, "dscpORTC", sizeof(paramArr[paramNum].paramName)-1);
#endif
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if ('\0' != MaxVal[0]) 
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s:%s", MinVal, MaxVal );
					}
#if defined(TCSUPPORT_CMCCV2)
					cfg_set_object_attr(qosPath, tmpMinName, MinVal);
					cfg_set_object_attr(qosPath, tmpMaxName, MaxVal);
					cfg_set_object_attr(qosPath, tmpTypeName, "DSCP");
					blapi_traffic_set_traffic_class("DSCP", atoi(MinVal), atoi(MaxVal), ADD_TRAFFIC_CLASS);
#endif
					paramNum++;
				}
			}
#if defined(TCSUPPORT_CT_JOYME4)
			else if (!strcmp(typeVal, "TC")) 
			{
				if ('\0' != MinVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "tc", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					if ('\0' != MaxVal[0]) 
					{
						snprintf(paramArr[paramNum].value, sizeof(paramArr[paramNum].value), "%s:%s", MinVal, MaxVal );
					}
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "FL")) 
			{
				if ('\0' != MinVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "fl", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
			}
#endif
			else if (!strcmp(typeVal, "TOS")) 
			{
				if ('\0' != MinVal[0]) 
				{
					strncpy(paramArr[paramNum].paramName, "tos", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, MinVal, sizeof(paramArr[paramNum].value)-1);
#if defined(TCSUPPORT_CMCCV2)
					cfg_set_object_attr(qosPath, tmpMinName, MinVal);
					cfg_set_object_attr(qosPath, tmpMaxName, MaxVal);
					cfg_set_object_attr(qosPath, tmpTypeName, "TOS");
					blapi_traffic_set_traffic_class("TOS", atoi(MinVal), atoi(MaxVal), ADD_TRAFFIC_CLASS);
#endif
					paramNum++;
				}
			}
			else if (!strcmp(typeVal, "LANInterface"))
			{
				memset(qosCmdBuf, 0, sizeof(qosCmdBuf));
				if ('\0' != MinVal[0]) {
					max = min = atoi(MinVal);
					if ('\0' != MaxVal[0]) {
						max = atoi(MaxVal);
					}
					for (j = min; j <= max; j++) 
					{
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
						memset(itf_name, 0, sizeof(itf_name));
						if( (0 == prepare_itf_name(j, itf_name, sizeof(itf_name))) \
							&& ( 0 < strlen(itf_name)) )
						{
							snprintf(qosCmdBuf + strlen(qosCmdBuf), \
								sizeof(qosCmdBuf) - strlen(qosCmdBuf), ",%s", itf_name);
						}				
#else
						if (strlen(g_lanif[j - 1]) > 0) 
						{
							snprintf(qosCmdBuf + strlen(qosCmdBuf),sizeof(qosCmdBuf) - strlen(qosCmdBuf), ",%s", g_lanif[j - 1]);
						}
#endif
					}

					if ('\0' != qosCmdBuf[0]) {
						strncpy(paramArr[paramNum].paramName, "LanIf", sizeof(paramArr[paramNum].paramName)-1);
						strncpy(paramArr[paramNum].value, qosCmdBuf, sizeof(paramArr[paramNum].value)-1);
						paramNum++;
					}
				}
			}
			else if (!strcmp(typeVal, "WANInterface")) 
			{
				setWanIfQoS(wanIfName, MinVal, MaxVal);
				if(wanIfName[0] != '\0')
				{
					strncpy(paramArr[paramNum].paramName, "WanIf", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, wanIfName, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
				
			}
#if defined(TCSUPPORT_CUC_QOS) || defined(TCSUPPORT_CMCC)
			else if (!strcmp(typeVal, "EtherType"))
			{				
				if ('\0' != MinVal[0])
				{
					if(strcmp(MinVal,"ARP")== 0){
						strncpy(qosCmdBuf, "0x0806", sizeof(qosCmdBuf)-1);
					}else if(strcmp(MinVal,"IPv4")== 0){
						strncpy(qosCmdBuf, "0x0800", sizeof(qosCmdBuf)-1);
					}else if(strcmp(MinVal,"IPv6")== 0){
						strncpy(qosCmdBuf, "0x86dd",sizeof(qosCmdBuf)-1);
					}else{
						strncpy(qosCmdBuf, MinVal,sizeof(qosCmdBuf)-1);
					}	
					strncpy(paramArr[paramNum].paramName, "EtherType", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, qosCmdBuf, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}				
			}
#endif
		}	
	}
	
	/* excute "qoscmd type" rule */
	qoscmd_lib_do_qos_type(type, ruleIdx, IPversion,paramNum, paramArr);

	/* 2. remark queue and 8021p etc. */
	paramNum = 0;
	memset(paramArr, 0, sizeof(typeParam_t)*MAX_TYPE_PARAM_NUM);

	/* queue priority */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(qosPath, "ActQueue", buf, sizeof(buf)) > 0)
	{
		if ('\0' != buf[0]) {
			queue = atoi(buf);
		}
		else {
			return -1;
		}

		snprintf(queueBuf,sizeof(queueBuf), "QueueP%d", queue);
		/* get queue from QoS Common node */
		memset(buf, 0, sizeof(buf));
		if(cfg_get_object_attr(QOS_COMMON_NODE, queueBuf, buf, sizeof(buf)) > 0)
		{
			if (atoi(buf) > 0 && atoi(buf) < 7) {
				strncpy(paramArr[paramNum].paramName, "queue", sizeof(paramArr[paramNum].paramName)-1);
				strncpy(paramArr[paramNum].value, buf, sizeof(paramArr[paramNum].value)-1);
				paramNum++;
			}
			else {
				return -1;
			}
		}
	}

	/* 802.1p remark */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(qosPath, "Act8021pRemarkingNum", buf, sizeof(buf)) > 0)
	{
		if ('\0' != buf[0])
		{
			if(cfg_get_object_attr(QOS_COMMON_NODE, "En8021PRemark",dotp, sizeof(dotp)) > 0)
			{
				if (!strcmp(dotp, "0")) {
					/* zero mark */
					strncpy(paramArr[paramNum].paramName, "dot1p", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, "8", sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
				else if (!strcmp(dotp, "1")) {
					/* pass through */
					strncpy(paramArr[paramNum].paramName, "dot1p", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, "9", sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
				else {
					/* remark */
					strncpy(paramArr[paramNum].paramName, "dot1p", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, buf, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
			}
		}
	}

	/* dscp or tc remark */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(qosPath, "ActDSCPRemarking", buf, sizeof(buf)) > 0)
	{
		if(cfg_get_object_attr(QOS_COMMON_NODE, "EnableDSCPMark", dscpEnable, sizeof(dscpEnable)) > 0)
		{
			if (!strcmp(dscpEnable, "Yes")) {
				if ('\0' != buf[0]) {
					strncpy(paramArr[paramNum].paramName, "dscpORTC", sizeof(paramArr[paramNum].paramName)-1);
					strncpy(paramArr[paramNum].value, buf, sizeof(paramArr[paramNum].value)-1);
					paramNum++;
				}
			}
		}
	}
	
	/* excute "qoscmd remark" rule */
	qoscmd_lib_do_qos_remark(type, ruleIdx, IPversion,paramNum, paramArr);
	return 0;
	
DelQosTypeRule:
	qoscmd_lib_do_qos_type(TYPE_DEL, ruleIdx, IPNULL_FLAG, 0, NULL);
	qoscmd_lib_do_qos_remark(TYPE_DEL, ruleIdx, IPNULL_FLAG, 0, NULL);
	return 0;
}

int svc_wan_related_set_qos_app_rule( int index, int type )
{
	char buf[CMD_BUF_LENGTH] = {0};
	char url[128]={0}, dstIP[32]={0}, conreqport[12]={0};
	unsigned int dstPort;
	char cwmp_commit[4] = {0};
	char queuebuf[10] = {0};
	int queuenum = 0;
	int ruleType = 0;
	int ruleIdx = 0;
	int IPversion = 0; 
	appParam_t param;
#if defined(TCSUPPORT_CT_JOYME2)
	struct in_addr in_s_v4addr = {0};
#endif
	char qosPath[64] = {0};
#if defined(TCSUPPORT_CT_VOIP_QOS) 
	int activeQueue = 0;
	char qosCmd[MAX_CMD_LENGTH] = {0};
#endif

	memset(&param, 0, sizeof(param));
	index -= MAX_TYPE_RULE_NUM;
	if ( index < 0 || index > MAX_APP_RULE_NUM - 1 ) {
		return -1;
	}

	ruleIdx = index - 1;
	ruleType = TYPE_ADD;
	IPversion = IPV4_FLAG;
	
	snprintf(qosPath, sizeof(qosPath), QOS_ENTRY_NODE, index + MAX_TYPE_RULE_NUM);
	/* 1. active or deactive */
	if(cfg_get_object_attr(qosPath, "AppName", buf, sizeof(buf)) > 0)
	{
		if (strlen(buf) <= 0) 
		{
			goto DelQosAppRule;
		}
	}
	else {
		goto DelQosAppRule;
	}

	/* 2. app_name */
	if(cfg_get_object_attr(qosPath, "AppName", buf, sizeof(buf)) > 0)
	{
		/* now we only handle tr069 app */
		/* delete type rule first for it may voip and iptv before */
		qoscmd_lib_do_qos_type(TYPE_DEL, ruleIdx + MAX_TYPE_RULE_NUM, IPNULL_FLAG, 0, NULL);
		qoscmd_lib_do_qos_remark(TYPE_DEL, ruleIdx + MAX_TYPE_RULE_NUM, IPNULL_FLAG, 0, NULL);
		
		if (!strcmp(buf, "TR069")) {
			if (type == 0) {
				/* when boot or qos template, just return, set qos rule when send inform */
				return 0;
			}
			
			/* borrow this buf to store QoS_Common node */
			memset(cwmp_commit, 0, sizeof(cwmp_commit));
			if(cfg_get_object_attr(QOS_COMMON_NODE, "cwmpCommitFlag", cwmp_commit, sizeof(cwmp_commit)) > 0)
			{
				if (cwmp_commit[0] != '1') {
					cfg_set_object_attr(QOS_COMMON_NODE, "webCommitFlag", "1");
					return 0;
				}
			}
			else {
				cfg_set_object_attr(QOS_COMMON_NODE, "webCommitFlag", "1");
				return 0;
			}
			cfg_set_object_attr(QOS_COMMON_NODE, "cwmpCommitFlag", "0");
		//follow need to modify!!!
			if(cfg_get_object_attr(CWMP_ENTRY_NODE, "acsUrl", url, sizeof(url)) > 0)
			{
				if (parseUrl(url, dstIP, &dstPort) == 0)
				{
					if(cfg_get_object_attr(CWMP_ENTRY_NODE, "conReqPort", conreqport, sizeof(conreqport)) > 0 && '\0' != conreqport[0])
					{
						strncpy(param.appName, "tr069", sizeof(param.appName)-1);
						strncpy(param.dstIP, dstIP, sizeof(param.dstIP)-1);
						strncpy(param.dstProto, conreqport, sizeof(param.dstProto)-1);
						param.dstPort = dstPort;
					}	
					else
					{
						strncpy(param.appName, "tr069", sizeof(param.appName)-1);
						strncpy(param.dstIP, dstIP, sizeof(param.dstIP)-1);
						strncpy(param.dstProto, "tcp", sizeof(param.dstProto)-1);
						param.dstPort = dstPort;
					}		
				}
				else
				{
					printf("%s:parser tr069 url failed. \n", __FUNCTION__);
					return -1;
				}
			}
		
		}
		else if (!strcmp(buf, "IPTV")) {
			/* delete app rule first, for it may be tr069 rule before */
			qoscmd_lib_do_qos_app(TYPE_DEL, ruleIdx, IPNULL_FLAG, NULL);
			cfg_set_object_attr(qosPath, "ProtocolID", "igmp");
			svc_wan_related_set_qos_type_rule(index + MAX_TYPE_RULE_NUM);
			return 0;
		}
		else if (!strcmp(buf, "VOIP")) {
#if defined(TCSUPPORT_CT_VOIP_QOS)
			memset(buf, 0, sizeof(buf));
			if(cfg_get_object_attr(qosPath, "ActQueue", buf, sizeof(buf)) > 0)
			{
				activeQueue= atoi(buf);
				if (activeQueue< 1 || activeQueue > MAX_QUEUE_NUM + 1) 
				{
					return -1;
				}
				snprintf(qosCmd, sizeof(qosCmd), "%d\n", activeQueue);
				doValPut(RTP_PRIORITY_QUEUE_PATH, qosCmd);
			}
#else
			/* delete app rule first, for it may be tr069 rule before */
			qoscmd_lib_do_qos_app(TYPE_DEL, ruleIdx, IPNULL_FLAG, NULL);
			cfg_set_object_attr(qosPath, "ProtocolID", "rtp");
			svc_wan_related_set_qos_type_rule( index + MAX_TYPE_RULE_NUM);
#endif
			return 0;
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}

	/* 3. queue */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(qosPath, "ActQueue", buf, sizeof(buf)) > 0)
	{
		queuenum = atoi(buf);
		if (queuenum < 1 || queuenum > MAX_QUEUE_NUM + 1)
		{
			return -1;
		}

		memset(buf, 0, sizeof(buf));
		snprintf(queuebuf,sizeof(queuebuf), "QueueP%d", queuenum);
		if(cfg_get_object_attr(QOS_COMMON_NODE, queuebuf, buf, sizeof(buf)) > 0)
		{
			queuenum = atoi(buf);
			if (queuenum < 0 || queuenum > MAX_QUEUE_NUM + 1)
			{
				return -1;
			}
			snprintf(param.queueNum, sizeof(param.queueNum), "%d", queuenum);
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}

	qoscmd_lib_do_qos_app(ruleType, ruleIdx, IPversion, &param);
	
#if defined(TCSUPPORT_CT_JOYME2)
	/*set qos app high priority*/
	if(strcmp(dstIP, oldtr69IP))
	{
		if(strcmp("0.0.0.0", oldtr69IP))
		{
			memset(&in_s_v4addr, 0, sizeof(struct in_addr));
			inet_pton(AF_INET, oldtr69IP, &in_s_v4addr);
			blapi_traffic_del_vip_by_sip_and_sport(htonl(in_s_v4addr.s_addr), 0);
		}
		if(strcmp("0.0.0.0", dstIP))
		{
			memset(&in_s_v4addr, 0, sizeof(struct in_addr));
			inet_pton(AF_INET, dstIP, &in_s_v4addr);
			blapi_traffic_set_vip_by_sip_and_sport(htonl(in_s_v4addr.s_addr),0);
		}
		strncpy(oldtr69IP, dstIP, sizeof(oldtr69IP)-1);
	}
	
#endif

	return 0;
	
DelQosAppRule:
	qoscmd_lib_do_qos_app(TYPE_DEL, ruleIdx, IPNULL_FLAG, NULL);
	return 0;
}

int svc_wan_related_set_qos_discpline_rule( char *path )
{
	char discipline[32] = {0};
	char wrr_pri[WRR_QUEUE_NUM][32];
	char bandWidth[32] = {0};
	char car_pri[CAR_QUEUE_NUM][32];
	char sw_pri[CAR_QUEUE_NUM][32];
	char qm[10] = {0};
	char forcebw[10] = {0};
	int bandWidthFlag = 0;
	char buf[32] = {0};
	char hwCar[8] = {0};
	unsigned long lbandwidth = 0;

	char upBandWith[32] = {0};
	char queueMask[32] = {0};
	char forceBW[16] = {0}; 
	char hwCAR[8] = {0};
	int i = 0;
	char commNode[64] = {0}, atttrName[32] = {0};
	int direction = 0;

	memset(wrr_pri, 0, sizeof(wrr_pri));
	memset(car_pri, 0, sizeof(car_pri));
	memset(sw_pri, 0, sizeof(sw_pri));
	
#if defined(TCSUPPORT_CT_JOYME4)
	if( strstr(path, "downlinkqos") )
	{
		direction = 1;
		snprintf(commNode, sizeof(commNode), DOWNLINKQOS_COMMON_NODE);
		snprintf(atttrName, sizeof(atttrName), "DownlinkBandwidth");
	}
	else
#endif
	{
		direction = 0;
		snprintf(commNode, sizeof(commNode), QOS_COMMON_NODE);
		snprintf(atttrName, sizeof(atttrName), "UplinkBandwidth");
	}
	/* check if there is uplink bandwidth in cfg node */
	if(cfg_get_object_attr(commNode, atttrName, bandWidth, sizeof(bandWidth)) > 0)
	{
		if ( strlen(bandWidth) > 0 && strcmp(bandWidth, "0") != 0 )
		{
			/* delete the last three characters */
			if (strlen(bandWidth) > 3) {
				sscanf(bandWidth, "%lu", &lbandwidth);
				snprintf(bandWidth,sizeof(bandWidth), "%lu", (lbandwidth / 1000));
			}
			else {
				strncpy(bandWidth, "1",sizeof(bandWidth)-1);
			}
			if(cfg_get_object_attr(commNode, "Active", buf, sizeof(buf)) > 0)
			{
				if( strcmp(buf, "No") == 0 )
				{
					strncpy(bandWidth, "0",sizeof(bandWidth)-1);
				}	
			}
			bandWidthFlag  = 1;
		}
		else {
			strncpy(bandWidth, "0",sizeof(bandWidth)-1);
			bandWidthFlag = 1;
		}
	}
	else {
		strncpy(bandWidth, "0",sizeof(bandWidth)-1);
		bandWidthFlag = 1;
	}

	/* set qos discpline */
	if(cfg_get_object_attr(commNode, "Discipline", discipline, sizeof(discipline)) > 0)
	{
		if (!strcmp(discipline,"SP") || !strcmp(discipline, "PQ"))
		{
			/* priority queue */
			if ( bandWidthFlag == 1 ) 
			{
				strncpy(upBandWith, bandWidth, sizeof(upBandWith) - 1);
			}
			if(cfg_get_object_attr(commNode, "QueueSW1", sw_pri[0], sizeof(sw_pri[0])) > 0 &&
				cfg_get_object_attr(commNode, "QueueSW2", sw_pri[1], sizeof(sw_pri[1])) > 0 &&
				cfg_get_object_attr(commNode, "QueueSW3", sw_pri[2], sizeof(sw_pri[2])) > 0 &&
				cfg_get_object_attr(commNode, "QueueSW4", sw_pri[3], sizeof(sw_pri[3])) > 0 
				#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
				&& cfg_get_object_attr(commNode, "QueueSW5", sw_pri[4], sizeof(sw_pri[4])) > 0 
				&& cfg_get_object_attr(commNode, "QueueSW6", sw_pri[5], sizeof(sw_pri[5])) > 0 
				&& cfg_get_object_attr(commNode, "QueueSW7", sw_pri[6], sizeof(sw_pri[6])) > 0 
				&& cfg_get_object_attr(commNode, "QueueSW8", sw_pri[7], sizeof(sw_pri[7])) > 0 
				#endif
			)
			{
				setQueueMask( qm, sw_pri, PQ_QUEUE_NUM);
				strncpy(queueMask, qm, sizeof(queueMask) - 1);
			}
			else
			{
				return -1;
			}

			if( 0 == direction )
			{
			qoscmd_lib_do_qos_discPQ(upBandWith, queueMask);
		}
#if defined(TCSUPPORT_CT_JOYME4)
			else
			{
				/* set downlinkqos discPQ */
			}
#endif
		}
		else if ( !strcmp(discipline,"WRR"))
		{  	/* WRR */
			cfg_get_object_attr(commNode, "QueueBW1", wrr_pri[0], sizeof(wrr_pri[0]));
			cfg_get_object_attr(commNode, "QueueBW2", wrr_pri[1], sizeof(wrr_pri[1]));
			cfg_get_object_attr(commNode, "QueueBW3", wrr_pri[2], sizeof(wrr_pri[2]));
			cfg_get_object_attr(commNode, "QueueBW4", wrr_pri[3], sizeof(wrr_pri[3]));
			#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
			cfg_get_object_attr(commNode, "QueueBW5", wrr_pri[4], sizeof(wrr_pri[4]));
			cfg_get_object_attr(commNode, "QueueBW6", wrr_pri[5], sizeof(wrr_pri[5]));
			cfg_get_object_attr(commNode, "QueueBW7", wrr_pri[6], sizeof(wrr_pri[6]));
			cfg_get_object_attr(commNode, "QueueBW8", wrr_pri[7], sizeof(wrr_pri[7]));
			#endif
			for ( i = 0; i < WRR_QUEUE_NUM; i++ ) {
				if ( '\0' == wrr_pri[i][0] )
					strncpy(wrr_pri[i], "0", sizeof(wrr_pri[i]));
			}
				
			if ( bandWidthFlag == 1 )
			{
				strncpy(upBandWith, bandWidth, sizeof(upBandWith) - 1);
			}

			if(cfg_get_object_attr(commNode, "QueueSW1", sw_pri[0], sizeof(sw_pri[0])) > 0 &&
				cfg_get_object_attr(commNode, "QueueSW2", sw_pri[1], sizeof(sw_pri[1])) > 0 &&
				cfg_get_object_attr(commNode, "QueueSW3", sw_pri[2], sizeof(sw_pri[2])) > 0 &&
				cfg_get_object_attr(commNode, "QueueSW4", sw_pri[3], sizeof(sw_pri[3])) > 0 
			#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
				&& cfg_get_object_attr(commNode, "QueueSW5", sw_pri[4], sizeof(sw_pri[4])) > 0 
				&& cfg_get_object_attr(commNode, "QueueSW6", sw_pri[5], sizeof(sw_pri[5])) > 0 
				&& cfg_get_object_attr(commNode, "QueueSW7", sw_pri[6], sizeof(sw_pri[6])) > 0 
				&& cfg_get_object_attr(commNode, "QueueSW8", sw_pri[7], sizeof(sw_pri[7])) > 0 
			#endif
			)
			{
				setQueueMask( qm, sw_pri, WRR_QUEUE_NUM);
				strncpy(queueMask, qm, sizeof(queueMask) - 1);
			}
			else 
			{
				return -1;
			}

			/* check if need force bandwidth */
			if(cfg_get_object_attr(commNode, "EnableForceWeight", forcebw, sizeof(forcebw)) > 0)
			{
				if (!strcmp(forcebw, "Yes")) 
				{
					strncpy(forceBW,"Yes",sizeof(forceBW) - 1);
				}
			}

			if( 0 == direction )
			{
				qoscmd_lib_do_qos_discWRR(wrr_pri, upBandWith, queueMask, forceBW);
			}
#if defined(TCSUPPORT_CT_JOYME4)
			else
			{
				/* set downlinkqos discWRR */
			}
#endif
		}
		else if ( !strcmp(discipline, "CAR"))
		{ 	/* CAR */
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW1", car_pri[0], sizeof(car_pri[0]));
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW2", car_pri[1], sizeof(car_pri[1]));
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW3", car_pri[2], sizeof(car_pri[2]));
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW4", car_pri[3], sizeof(car_pri[3]));
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW5", car_pri[4], sizeof(car_pri[4]));
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW6", car_pri[5], sizeof(car_pri[5]));
			#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW7", car_pri[6], sizeof(car_pri[6]));
			cfg_get_object_attr(QOS_COMMON_NODE, "QueueBW8", car_pri[7], sizeof(car_pri[7]));
			#endif
			for ( i = 0; i < CAR_QUEUE_NUM; i++ ) {
				if ( '\0' == car_pri[i][0] )
					strncpy(car_pri[i], "0", sizeof(car_pri[i]));
			}
			
			if ( bandWidthFlag == 1 )
			{
				strncpy(upBandWith, bandWidth, sizeof(upBandWith) - 1);
			}

			if(cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW1", sw_pri[0], sizeof(sw_pri[0])) > 0 &&
				cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW2", sw_pri[1], sizeof(sw_pri[1])) > 0 &&
				cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW3", sw_pri[2], sizeof(sw_pri[2])) > 0 &&
				cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW4", sw_pri[3], sizeof(sw_pri[3])) > 0 &&
				cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW5", sw_pri[4], sizeof(sw_pri[4])) > 0 &&
				cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW6", sw_pri[5], sizeof(sw_pri[5])) > 0
			#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
				&& cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW7", sw_pri[6], sizeof(sw_pri[6])) > 0 
				&& cfg_get_object_attr(QOS_COMMON_NODE, "QueueSW8", sw_pri[7], sizeof(sw_pri[7])) > 0
			#endif
			)
			{
				setQueueMask( qm, sw_pri, CAR_QUEUE_NUM);
				strncpy(queueMask, qm, sizeof(queueMask) - 1);
			}
			else
			{
				return -1;
			}

			if(cfg_get_object_attr(SYS_ENTRY_NODE, "HWCAR", hwCar, sizeof(hwCar)) > 0)
			{
				strncpy(hwCAR, hwCar, sizeof(hwCAR) - 1);
			}
			else
			{
				strncpy(hwCAR, "1", sizeof(hwCAR) - 1);
			}						
			qoscmd_lib_do_qos_discCAR(car_pri, upBandWith, queueMask, hwCAR);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2)
static void get_weight_car_attr()
{
	char nodeName[64] = {0}, plan[8] = {0};
	char attrName[16]= {0}, attrVal[32] = {0};
	int i = 0;

	cfg_obj_get_object_attr(QOS_COMMON_NODE, "Discipline", 0, plan, sizeof(plan));
	bzero(nodeName, sizeof(nodeName));
	if (0 == strcasecmp(plan, "WRR"))
		strncpy(nodeName, QOSATTR_WEIGHTENTRY_NODE, sizeof(nodeName) - 1);
	else if (0 == strcasecmp(plan, "CAR"))
		strncpy(nodeName, QOSATTR_CARENTRY_NODE, sizeof(nodeName) - 1);
	else 
		return;
	
	for(i = 1; i <= 8; i++)
	{
		bzero(attrName, sizeof(attrName));
		bzero(attrVal, sizeof(attrVal));
		snprintf(attrName, sizeof(attrName), "QueueBW%d", i);
		if (cfg_obj_get_object_attr(nodeName, attrName, 0, attrVal, sizeof(attrVal)) >= 0)
			cfg_set_object_attr(QOS_COMMON_NODE, attrName, attrVal);
	}
	return;
}
#endif

#if defined(TCSUPPORT_CT_JOYME4)
int svc_wan_related_downlinkqos_execute(wan_related_cfg_update_t* param)
{
	char buf[64] = {0};
	if( cfg_get_object_attr(DOWNLINKQOS_COMMON_NODE, "Active", buf, sizeof(buf)) > 0 )
	{
		if( !strcmp(buf, "Yes") )
		{
			svc_wan_related_set_qos_discpline_rule( param->path );	
			qosWanInterfaceUpdate(1, param->path);
		}
		else
		{
			qosWanInterfaceUpdate(0, param->path);
			svc_wan_related_set_qos_discpline_rule( param->path );
		}
	}
	else
	{
		return -1;
	}
	return 0;
	/* set qos rule */
	
}

int svc_wan_related_downlinkqos_boot(wan_related_cfg_update_t* param)
{
	svc_wan_related_downlinkqos_execute(param);
	return 0;
}

int update_qos_template_for_other_special_service(void)
{
	char active_s[12] = {0}, attr_name[24] = {0}, attr_val[12] = {0};
	char mode_s[64] = {0}, tmp_mode[64] = {0}, node_path[32] = {0}, svr_list[64] = {0};
	int isIPTV_on = 0, isSpecial_on = 0;
	int pvc_idx = 0, entry_idx = 0, if_idx = 0, cnt = 1;
	unsigned char wan_remark_queue_cfg[MAX_WAN_IF_INDEX];
	unsigned char priority[T_MAX] = {0};
	char *token = NULL, *safep = NULL;

	/* CHECK ACTIVE. */
	bzero(active_s, sizeof(active_s));
	bzero(wan_remark_queue_cfg, sizeof(wan_remark_queue_cfg));
	if ( cfg_get_object_attr(QOS_COMMON_NODE, "Active", active_s, sizeof(active_s)) > 0
		&& 0 == strcmp(active_s, "Yes") )
	{
		/* CHECK TEMPLATE. */
		if( cfg_get_object_attr(QOS_COMMON_NODE, "Mode", mode_s, sizeof(mode_s)) > 0 )
		{
			strcpy(tmp_mode, mode_s);
			token = strtok_r(tmp_mode, ",", &safep);
			while ( token )
			{
			#if defined(TCSUPPORT_CWMP)
				if ( strstr(token, "TR069") ){
					priority[T_TR069] = cnt ++;
				}else
			#endif
			
			#if defined(TCSUPPORT_VOIP)
				if( strstr(token, "VOIP") ){
					priority[T_VOIP] = cnt ++;
				}else
			#endif
				if( strstr(token, "IPTV") ){
					priority[T_IPTV] = cnt ++;
				}
				else if( strstr(token, "SPECIAL_SERVICE") ){	
					priority[T_SPECIAL_SERVICE] = cnt ++;
				}

				token = strtok_r(NULL, ",", &safep);
			}

			bzero(attr_val, sizeof(attr_val));
			bzero(attr_name, sizeof(attr_name));
			snprintf(attr_name, sizeof(attr_name), "QueueSW%d", priority[T_IPTV]);
			if( cfg_get_object_attr(QOS_COMMON_NODE, attr_name, attr_val, sizeof(attr_val)) > 0
				&& 0 == strcmp(attr_val, "Yes") /* Queue 3 is ACTIVE */
				&& strstr(mode_s, "IPTV") ) /* Template include IPTV. */
				isIPTV_on = 1;

			bzero(attr_val, sizeof(attr_val));
			bzero(attr_name, sizeof(attr_name));
			snprintf(attr_name, sizeof(attr_name), "QueueSW%d", priority[T_SPECIAL_SERVICE]);
			if( cfg_get_object_attr(QOS_COMMON_NODE, attr_name, attr_val, sizeof(attr_val)) > 0
				&& 0 == strcmp(attr_val, "Yes") /* Queue 4 is ACTIVE */
				&& strstr(mode_s, "SPECIAL_SERVICE") ) /* Template include IPTV. */
				isSpecial_on = 1;

			/* check ALL wan and update wan index to qos type module. */
			if ( isIPTV_on || isSpecial_on )
			{
				for ( pvc_idx = 0; pvc_idx < PVC_NUM; pvc_idx ++ )
				{
					for ( entry_idx = 0; entry_idx < MAX_SMUX_NUM; entry_idx ++ )
					{
						if_idx = pvc_idx * MAX_SMUX_NUM + entry_idx;
						if ( if_idx > MAX_WAN_IF_INDEX )
							return -1;
						bzero(svr_list, sizeof(svr_list));
						snprintf(node_path, sizeof(node_path), WAN_PVC_ENTRY_NODE, pvc_idx + 1, entry_idx + 1);
						if ( cfg_get_object_attr(node_path, "ServiceList", svr_list, sizeof(svr_list)) > 0 )
						{
							/* CHECK OTHER (IPTV) WAN. */
							if ( isIPTV_on && strstr(svr_list, "OTHER") )
								wan_remark_queue_cfg[if_idx] = priority[T_IPTV]; 
							else if ( isSpecial_on && strstr(svr_list, "SPECIAL_SERVICE") ) /* CHECK SPECIAL_SERVICE WAN. */
								wan_remark_queue_cfg[if_idx] = priority[T_SPECIAL_SERVICE];
						}
					}
				}
			}

			/* update rules to QoS type module. */
			blapi_traffic_set_wanInterface_qos_template(wan_remark_queue_cfg);
		}
	}
	else
		blapi_traffic_set_wanInterface_qos_template(wan_remark_queue_cfg);

	return 0;
}

#endif

int svc_wan_related_qos_execute(wan_related_cfg_update_t* param)
{
	char buf[CMD_BUF_LENGTH] = {0};
	int i = 0;
	int ret = -1;
#if defined(TCSUPPORT_CT_SWQOS)	
	char attr_buf[32];
	int que_enable[MAX_QUEUE_NUM] = {0};
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	get_weight_car_attr();
#endif

	ret = cfg_get_object_attr(GPON_ONU_NODE, "OnuType", buf, sizeof(buf));
	if(ret > 0)
	{
		/* if onu is sfu mode should use the qos confg form GPON_OMCI*/
		if ( strcmp(buf, "1") == 0 )
		{
			return SUCCESS;
		}		
	}
	
	/* check if there is uplink bandwidth in cfg node */
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(QOS_COMMON_NODE, "Active",buf, sizeof(buf)) > 0)
	{
		if ( strcmp(buf, "Yes") == 0 )
		{
			if ( svc_wan_related_set_qos_switch_rule( 1 ) == 0 )
			{
				/* reset vlan rule again */
				for ( i = 1; i <= MAX_VLAN_RULE_NUM; i++ )
				{
					svc_wan_related_set_qos_vlanid_rule(i + MAX_TYPE_RULE_NUM + MAX_TEM_RULE_NUM, 1);
				}
			}	
			svc_wan_related_set_qos_temp_rule(1, param->path);

#if defined(TCSUPPORT_CT_SWQOS)
			for(i = 0; i<MAX_QUEUE_NUM; i++)
			{
				snprintf(attr_buf,sizeof(attr_buf), "QueueSW%d", i+1);
				memset(buf, 0, sizeof(buf));
				cfg_get_object_attr(QOS_COMMON_NODE, attr_buf, buf, sizeof(buf));
				if(strcmp(buf, "Yes") == 0 )
				{
					que_enable[i] = 1;
				}	
			}
			memset(buf, 0, sizeof(buf));
			#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
			snprintf(buf,sizeof(buf),"%d %d %d %d %d %d %d %d 1 \n",
				que_enable[0],que_enable[1],que_enable[2],que_enable[3],que_enable[4],que_enable[5],que_enable[6],que_enable[7]);
			#else
			snprintf(buf,sizeof(buf),"%d %d %d %d %d %d 1\n",
				que_enable[0],que_enable[1],que_enable[2],que_enable[3],que_enable[4],que_enable[5]);
			#endif
			doValPut("/proc/tc3162/qos_queue_status", buf);
#endif

			qosWanInterfaceUpdate(1, param->path);
		}
		else
		{
			qosWanInterfaceUpdate(0, param->path);
#if defined(TCSUPPORT_CT_SWQOS)
			doValPut("/proc/tc3162/qos_enable", "0\n");
#endif
			/* qos is off, but set qos rule by qos template */
			svc_wan_related_set_qos_discpline_rule( param->path );
			svc_wan_related_set_qos_temp_rule( 0, param->path );
			svc_wan_related_set_qos_switch_rule( 0);
			goto QOS_EXE_SUCC;
		}

#if defined(TCSUPPORT_CT_JOYME4)
		update_qos_template_for_other_special_service();
#endif
	}
	else
	{
		goto QOS_EXE_FAIL;
	}
	
	/* 0. set qos discpline */
	if ( svc_wan_related_set_qos_discpline_rule( param->path ) == -1 )
	{
		goto QOS_EXE_FAIL;
	}

	/* 1. excute type rules */
	for ( i = 1; i <= MAX_TYPE_RULE_NUM; i++ )
	{
		svc_wan_related_set_qos_type_rule( i );
	}
			
	/* 2. excute app rules */
	for ( i = 1; i <= MAX_APP_RULE_NUM; i++ )
	{
		svc_wan_related_set_qos_app_rule( i + MAX_TYPE_RULE_NUM, 1);
	}

	/* 3. excute vlan id rules */
	for ( i = 1; i <= MAX_VLAN_RULE_NUM; i++ )
	{
		svc_wan_related_set_qos_vlanid_rule( i + MAX_TYPE_RULE_NUM + MAX_TEM_RULE_NUM, 1);
	}

QOS_EXE_SUCC:
	system("/userfs/bin/hw_nat -!");
	ret = ecnt_event_send(ECNT_EVENT_DBUS,
			ECNT_EVENT_DBUS_QOS,
			NULL,
			0);
	return SUCCESS;

QOS_EXE_FAIL:
	system("/userfs/bin/hw_nat -!");
	ret = ecnt_event_send(ECNT_EVENT_DBUS,
			ECNT_EVENT_DBUS_QOS,
			NULL,
			0);

	return FAIL;
}


int svc_wan_related_qos_boot(wan_related_cfg_update_t* param)
{
	char buf[CMD_BUF_LENGTH] = {0};
	int ret, i;
	char nodePath[64] = {0};
	
#if defined(TCSUPPORT_CT_JOYME2)
	get_weight_car_attr();
#endif
	snprintf(nodePath, sizeof(nodePath),QOS_COMMON_NODE);
	memset(appmode,0,sizeof(appmode));
	if(cfg_get_object_attr(nodePath, "Mode", buf, sizeof(buf)) > 0)
	{
		printf("qos_boot: init app mode for boot [%s]\n", buf);
		strncpy(appmode, buf, sizeof(appmode)-1);
	}
	
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "qosBoot", "1");
	
	if(cfg_get_object_attr(nodePath, "Active", buf, sizeof(buf)) > 0)
	{
		if ( strcmp(buf,"Yes") == 0 ) 
		{
			/* qos is on at boot time */
			ret = svc_wan_related_set_qos_switch_rule(1);
			if ( ret == -1 ) 
			{
				return FAIL;
			}
			/* if QoS is from down to up, we set QoS template rules by template name*/
			svc_wan_related_set_qos_temp_rule(1, param->path);
			ret = svc_wan_related_set_qos_discpline_rule( param->path );
			if ( ret == -1 ) 
			{
				return FAIL;
			}
	
			/* 1. excute type rules */
			for ( i = 1; i <= MAX_TYPE_RULE_NUM; i++ )
			{
				svc_wan_related_set_qos_type_rule( i );
			}	
	
#if defined(TCSUPPORT_CT_VOIP_QOS)	
			doValPut(RTP_PRIORITY_QUEUE_PATH, "0\n");
#endif
			for ( i = 1; i <= MAX_APP_RULE_NUM; i++ )
			{
				svc_wan_related_set_qos_app_rule(i + MAX_TYPE_RULE_NUM, 0);
			}
			
			for ( i = 1; i <= MAX_VLAN_RULE_NUM; i++ )
			{
				svc_wan_related_set_qos_vlanid_rule(i + MAX_TYPE_RULE_NUM + MAX_TEM_RULE_NUM, 1);
			}
			
			memset(buf,0,sizeof(buf));	
			if(cfg_get_object_attr(nodePath, "Mode", buf, sizeof(buf)) > 0)
			{
				strncpy(appmode,buf,sizeof(appmode)-1);
			}

			qosWanInterfaceUpdate(1, param->path);
		}
		else 
		{
			svc_wan_related_set_qos_temp_rule(0, param->path);
			qosWanInterfaceUpdate(0, param->path);
		}
	}

	return SUCCESS;
}


