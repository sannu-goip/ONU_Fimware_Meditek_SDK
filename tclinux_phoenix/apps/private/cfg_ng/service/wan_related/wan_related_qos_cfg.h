
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

#ifndef __SVC_WAN_RELATED_QOS_CFG_H__
#define __SVC_WAN_RELATED_QOS_CFG_H__

#include <linux/version.h>

#define LOAD_DEFAULT_CHAIN "/usr/script/filter_forward_start.sh"
#define FLUSH_DEFAULT_CHAIN "/usr/script/filter_forward_stop.sh"

#define FAIL 			-1
#define SUCCESS 		0

#define	SW_ON		1
#define   SW_OFF		0
#define  	TYPE_ADD 	1
#define  	TYPE_DEL	0


#define	INSMODE	1
#define	RMMODE		0
#define 	MAX_APP_RULE_NUM		4
#define 	MAX_TEM_RULE_NUM		8


#define 	MAX_VLAN_RULE_NUM	8
#define 	MAX_TYPE_RULE_NUM	10
#define 	MAX_APP_TYPE_NUM		15
#define 	MAX_CMD_LENGTH		256
#define 	CMD_BUF_LENGTH		64
#define 	MAX_TYPE_NUM			10

#define QOS_EBT_MODULE_NUM	8
#define LENTH_CHAR_ARR_32 		32

#define RTP_PRIORITY_QUEUE_PATH		"/proc/tc3162/RtpPriority"
#define QOS 								"QoS"
#define QOS_COMMON 					"Common"

/*#define MAXLEN_NODE_NAME	32*/
#define MAXLEN_ATTR_NAME	32
#define MAXLEN_TCAPI_MSG	1024
#define MAXLEN_TCAPI_PATH_LEN	128

#define QOSTYPE_QUEUE_INDEX_OFFSET	12
#define QOSTYPE_PBIT_MARK_OFFSET	8
#define QOSTYPE_DSCP_SW_MARK_OFFSET	6
#define QOSTYPE_DSCP_MARK			0x003F 

int qosWanInterfaceUpdate( int on_off, char *path);
int getCurrentIndex(char* path);
int parseUrl(const char *theUrlPtr, char *theHostNamePtr, unsigned int *thePortPtr) ;

int svc_wan_related_set_qos_switch_rule(int sw );
int svc_wan_related_set_qos_vlanid_rule(int index, int type );
void svc_wan_related_set_qos_temp_rule( int type, char *path);
int svc_wan_related_set_qos_type_rule( int index);
int svc_wan_related_set_qos_app_rule(int index, int type );
int svc_wan_related_set_qos_discpline_rule( char *path );
int svc_wan_related_qos_execute(wan_related_cfg_update_t* param);
int svc_wan_related_qos_boot(wan_related_cfg_update_t* param);
#if defined(TCSUPPORT_CT_JOYME4)
int svc_wan_related_downlinkqos_execute(wan_related_cfg_update_t* param);
int svc_wan_related_downlinkqos_boot(wan_related_cfg_update_t* param);
int update_qos_template_for_other_special_service(void);
#endif
#endif

