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

#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CUC)
#define REDIRECT_BOA_PORT " --to 8080"
#else
#define REDIRECT_BOA_PORT ""
#endif

#if defined(TCSUPPORT_TRUE_LANDING_PAGE)     
static int landingPageEnableFlag = 0;
int setLandingPageEnableFlag(mxml_node_t *top)
{
	char nodePath[64] = {0};
	char portal_status[8] = {0};

 #ifdef TCSUPPORT_TRUE_LANDING_PAGE      
	snprintf(nodePath, sizeof(nodePath), PORTAL_ENTRY_NODE);
	cfg_obj_get_object_attr(nodePath, "Enable", 0, portal_status, sizeof(portal_status));
	
	if(strcmp(portal_status, "1") == 0)
	{
		cfg_set_object_attr(nodePath, "landingPageEnableRunning", "Yes");
	       landingPageEnableFlag = 1;
	}
	else
	{
		cfg_set_object_attr(nodePath, "landingPageEnableRunning", "No");  
	       landingPageEnableFlag = 0;
	}
#endif

	return 0;
}

int isLandingPageEnable()
{
	if(1 == landingPageEnableFlag)
	{
	        return 1;
	}
	else
	{
	        return 0;
	}
}

void addNatRuleForLandingPage()
{
        system("iptables -t nat -N PRE_PORTAL");
        system("iptables -t nat -A PREROUTING -j PRE_PORTAL");
}
#endif

int svc_cfg_boot_portal(void)
{
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
	char portal_status[2]={0};
	char nodePath[64] = {0}, lanip[32] = {0}, cmdbuf[256] = {0};

	snprintf(nodePath, sizeof(nodePath), PORTAL_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodePath, "Enable", 0, portal_status, sizeof(portal_status)) < 0)
	{
		return FAIL;
	}

	if ( cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "IP", 0, lanip, sizeof(lanip)) < 0 )
		strcpy(lanip, "192.168.1.1");

	system("iptables -t nat -N PRE_PORTAL");
	system("iptables -t nat -A PREROUTING -j PRE_PORTAL");
	if(!strcmp(portal_status,"1"))
	{
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf),
			"iptables -t nat -A PRE_PORTAL -i br0 -d %s -p tcp --dport 80 -j RETURN", lanip);
		system_escape(cmdbuf);
		system("iptables -t nat -A PRE_PORTAL -i br0 -p tcp --dport 80 -j REDIRECT"REDIRECT_BOA_PORT);
	}
#endif

#if defined(TCSUPPORT_TRUE_LANDING_PAGE)
	   setLandingPageEnableFlag( );
        if(1 == isLandingPageEnable())
        {
                addNatRuleForLandingPage();               
        }
#endif

	return SUCCESS;
}

int cfg_type_portal_execute( )
{
	char portal_status[8]={0};
	char nodePath[64] = {0}, lanip[32] = {0}, cmdbuf[256] = {0};
	
	snprintf(nodePath, sizeof(nodePath),PORTAL_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodePath, "Enable", 0, portal_status, sizeof(portal_status)) < 0)
	{
		return -1;
	}

	if ( cfg_obj_get_object_attr(LAN_ENTRY0_NODE, "IP", 0, lanip, sizeof(lanip)) < 0 )
		strcpy(lanip, "192.168.1.1");

	if(!strcmp(portal_status,"1"))
	{
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf),
			"iptables -t nat -D PRE_PORTAL -i br0 -d %s -p tcp --dport 80 -j RETURN", lanip);
		system_escape(cmdbuf);
		system_escape("iptables -t nat -D PRE_PORTAL -i br0 -p tcp --dport 80 -j REDIRECT"REDIRECT_BOA_PORT);
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf),
			"iptables -t nat -A PRE_PORTAL -i br0 -d %s -p tcp --dport 80 -j RETURN", lanip);
		system_escape(cmdbuf);
		system_escape("iptables -t nat -A PRE_PORTAL -i br0 -p tcp --dport 80 -j REDIRECT"REDIRECT_BOA_PORT);
	}
	else{
		system_escape("iptables -t nat -F PRE_PORTAL");
	}

	return 0;
}

int cfg_type_portal_func_commit(char* path)
{
	 return cfg_type_portal_execute( );
}

static cfg_node_ops_t cfg_type_portal_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_portal_func_commit 
}; 


static cfg_node_type_t cfg_type_portal_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_portal, 
	 .ops = &cfg_type_portal_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_portal_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_portal_func_commit 
}; 


static cfg_node_type_t* cfg_type_portal_child[] = { 
	 &cfg_type_portal_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_portal = { 
	 .name = "Portal", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_portal_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_portal_child, 
	 .ops = &cfg_type_portal_ops, 
}; 
