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
#include "utility.h"
#include "blapi_traffic.h"
int filter_state = 0;

static int algswitch_check_set_filter(void)
{
	unsigned int new_filter_state 	= 0;	
	char newFilterState[CFG_BUF_32_LEN] = {0};
	int filter_need				  	= 0;
	char l2tp_sw_status[4]			= {0};
	char ipsec_sw_status[4]			= {0};
	char pptp_sw_status[4]			= {0};
	char nodePath[CFG_BUF_64_LEN] = {0};		

	/*check whether need filter module or not*/
	snprintf(nodePath, sizeof(nodePath), ALGSWITCH_ENTRY_NODE);
	if((cfg_obj_get_object_attr(nodePath, "L2TPSW", 0, l2tp_sw_status, sizeof(l2tp_sw_status)) > 0)
	&&(cfg_obj_get_object_attr(nodePath, "IPSECSW", 0, ipsec_sw_status, sizeof(ipsec_sw_status)) > 0)
	&&(cfg_obj_get_object_attr(nodePath, "PPTPSW", 0, pptp_sw_status, sizeof(pptp_sw_status)) > 0)){
		if(!strcmp(l2tp_sw_status,"on") || !strcmp(ipsec_sw_status,"on") || !strcmp(pptp_sw_status,"on")){
			filter_need = 1;
		}
	}
	
	/*set iptables filter*/
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_obj_get_object_attr(nodePath, "FilterState", 0, newFilterState, sizeof(newFilterState)) ;
	if(filter_need){/*filter on*/
		new_filter_state = atoi(newFilterState) | ALGSW_VECTOR;
	}
	else{/*filter down*/
		new_filter_state = atoi(newFilterState) & (~ALGSW_VECTOR);
	}
	
	check_and_set_filter(new_filter_state);

	return 0;
}

static int set_alg_switch(void)
{
	int i = 0;
	char switch_stat[4] 	= {0};
	char cmd_buf[64] 		= {0};
	char nodeName[128]		= {0};

	snprintf(nodeName, sizeof(nodeName), ALGSWITCH_ENTRY_NODE);

	char algsw_proto_name[][CFG_ATTR_SIZE]=
	{
		{"ftp"},
		{"sip"},
		{"h323"},
		{"rtsp"},  
		{"l2tp"},
		{"ipsec"},
		{"pptp"},
		{""},
	};
	
	char algsw_attr_parm[][CFG_ATTR_SIZE]=
	{
		{"FTPSW"},
		{"SIPSW"},
		{"H323SW"},
		{"RTSPSW"},  
		{"L2TPSW"},
		{"IPSECSW"},
		{"PPTPSW"},
		{""},
	};

	for(i = 0; strlen(algsw_attr_parm[i]) != 0; i++) {
		cfg_obj_get_object_attr(nodeName, algsw_attr_parm[i], 0, switch_stat, sizeof(switch_stat));
		if(0 == strcmp(switch_stat, "on")){
			snprintf(cmd_buf, sizeof(cmd_buf), "algcmd switch %s on", algsw_proto_name[i]);
		}
		else{
			snprintf(cmd_buf, sizeof(cmd_buf), "algcmd switch %s off", algsw_proto_name[i]);
		}	

		system(cmd_buf);
		memset(switch_stat, 0, sizeof(switch_stat));
		memset(cmd_buf, 0, sizeof(cmd_buf));
	}

	return 0;
}

void set_alg_rtsp_port(void)
{
	char RtspPort[64] = {0};
	if(cfg_obj_get_object_attr(ALGSWITCH_ENTRY_NODE, "Rtsp_Port", 0, RtspPort, sizeof(RtspPort)) > 0)
	{
		blapi_traffic_set_alg_rtsp_port(RtspPort);
	}
}

static int wan_related_algswitch_execute(void)
{	
	char switch_stat[4] = {0};
	
	if((cfg_obj_get_object_attr(ALGSWITCH_ENTRY_NODE, "RTSPSW", 0, switch_stat, sizeof(switch_stat)) > 0)
		&&(0 == strcmp(switch_stat, "on")))
	{
		set_alg_rtsp_port();
	}
	
	if(algswitch_check_set_filter() < 0){
		return -1;
	}
	if(set_alg_switch() < 0){
		return -1;
	}

	return 0;
}
static int cfg_type_algswitch_entry_func_commit(char* path)
{
	int ret  = -1;
	ret = wan_related_algswitch_execute();

	return ret;
}

int svc_cfg_boot_algswitch(void)
{
	return wan_related_algswitch_execute();
}

static cfg_node_ops_t cfg_type_algswitch_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_algswitch_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_algswitch_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_algswitch, 
	 .ops = &cfg_type_algswitch_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_algswitch_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_algswitch_entry_func_commit 
}; 


static cfg_node_type_t* cfg_type_algswitch_child[] = { 
	 &cfg_type_algswitch_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_algswitch = { 
	 .name = "ALGSwitch", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_algswitch_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_algswitch_child, 
	 .ops = &cfg_type_algswitch_ops, 
}; 
