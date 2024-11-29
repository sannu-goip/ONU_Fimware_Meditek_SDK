
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

#ifndef __SVC_WAN_PORT_BIND_H__
#define __SVC_WAN_PORT_BIND_H__

#include "wan_common.h"

#define     NOTBINDSTRING           "NOTBIND"
#define     MAX_BINDLIST_NUM        (64)

#define PORT_BIND_OP_V4		1
#define PORT_BIND_OP_V6		2
#define PORT_BIND_OP_ALL	3

enum
{
    E_ADD_PORT_BIND_RULE = 1,
    E_DEL_PORT_BIND_RULE = 2,
};

typedef struct _port_bind_info_
{
    int  if_index;
    char mode_flag;
    char bind_ifName_list[SVC_WAN_BUF_128_LEN];
    char bind_ifType_list[SVC_WAN_BUF_128_LEN];
    char bind_lanif_list[SVC_WAN_BUF_128_LEN];
}port_bind_info_t, *pt_port_bind_info;


int get_port_binding_interface_info(char* path, wan_entry_cfg_t* cfg, wan_entry_obj_t* obj);

#if defined(TCSUPPORT_CT_JOYME2)
int get_other_wan_bind(wan_entry_obj_t *obj, int action);
#endif

void set_port_binding_info(wan_entry_obj_t* obj,int portbind_flag, int ip_type);

void set_port_binding_info2(char *path, int action);

void dealwith_link_local_route(char* if_name, int if_index, int def_route);

#if !defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
void add_dhcprealy_port_binding_rule(wan_entry_obj_t* obj, char* RealyInputTabe, char* RealyForwardTabe);
#else
void add_dhcprealy_port_binding_rule(wan_entry_obj_t* obj, char* RealyForwardTabe);
#endif

void add_dhcp_port_filter_rule(wan_entry_obj_t* obj, char* PortFilterInputTable, char* PortFilterOutputTable);

void set_portbinding_info_for_dslite(wan_entry_obj_t* obj);

int set_port_bind_mask(int index);
int clear_port_bind_mask(int index);
int check_port_bind_mask(int index);
int pre_check_lanport_bind( void );

#endif



