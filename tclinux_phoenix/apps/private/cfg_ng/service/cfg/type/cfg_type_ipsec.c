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
#include "utility.h" 
#include <svchost_evt.h>

#ifdef TCSUPPORT_CT
/* CT can support 7 VPN connections at most because it has only 7 available 
 * WAN interfaces at most for VPN (due to nas0_0 being occupied by TR69)  */
#define MAX_CONN_NUM    (MAX_IPSEC_NUM-1)
#else
#define MAX_CONN_NUM    (MAX_IPSEC_NUM)
#endif

static int isEntryEnSet=0;

static int cfg_type_ipsec_func_get(char* path,char* attr, char* val,int len) 
{ 
    char nodeName[32];
    char string[32];
	unsigned int i, totalConnCnt;
	char addnum[4] = {0}, connCnt[4] = {0};


    if (path == NULL || attr == NULL || val == NULL || len <= 0)
    {
        return FAIL;
    }

	if (strcmp(attr, "add_num") == 0)
	{	
		/* try to find an empty IPSEC Entry */	
		for (i = 0; i < MAX_IPSEC_NUM; i++)
		{
	        memset(nodeName, 0, sizeof(nodeName));
            /* path's entry starts from 1, that is , root.ipsec.entry.1 stands for IPSEC_Entry0 */
            snprintf(nodeName, sizeof(nodeName), IPSEC_ENTRY_NODE, i+1);

            memset(string, 0, sizeof(string));
            cfg_obj_get_object_attr(nodeName,"Gateway_IP", 0, string, sizeof(string));

			if (
				(0 == strcmp(string, "no attribute information")) ||
				(0 == strcmp(string, "no node information")) || 
				(0 == strcmp(string, "N/A")) ||
				(0 == strcmp(string, ""))
			)
			{
		        memset(nodeName, 0, sizeof(nodeName));
                snprintf(nodeName, sizeof(nodeName), IPSEC_NODE);
                memset(addnum, 0, sizeof(addnum));
				sprintf(addnum, "%d", i);
                cfg_obj_set_object_attr(nodeName, "add_num", 0, addnum);
				break;
			}
		}
	
		if (i == MAX_IPSEC_NUM)
		{
			return FAIL;
		}
	}
	else if (strcmp(attr, "Total_Conn_Num") == 0)
	{
  		totalConnCnt = 0;

		//count IPSEC entries		
		for (i = 0; i < MAX_IPSEC_NUM; i++)
		{
	        memset(nodeName, 0, sizeof(nodeName));
            /* path's entry starts from 1, that is , root.ipsec.entry.1 stands for IPSEC_Entry0 */
            snprintf(nodeName, sizeof(nodeName), IPSEC_ENTRY_NODE, i+1);

            memset(string, 0, sizeof(string));
            cfg_obj_get_object_attr(nodeName,"Gateway_IP", 0, string, sizeof(string));

			if (!(
				(0 == strcmp(string, "no attribute information")) ||
				(0 == strcmp(string, "no node information")) || 
				(0 == strcmp(string, "N/A")) ||
				(0 == strcmp(string, ""))
			))
			{
				totalConnCnt++;
			}
		}

        memset(nodeName, 0, sizeof(nodeName));
        snprintf(nodeName, sizeof(nodeName), IPSEC_NODE);
        memset(connCnt, 0, sizeof(connCnt));
		sprintf(connCnt, "%d", totalConnCnt);
        cfg_obj_set_object_attr(nodeName, "Total_Conn_Num", 0, connCnt);
	}
    else { /* for entry0_en ~ entry7_en */
        
        if (isEntryEnSet==0) {
            memset(nodeName, 0, sizeof(nodeName));
            snprintf(nodeName, sizeof(nodeName), IPSEC_NODE);
            memset(string, 0, sizeof(string));
            
            for (i = 0; i < MAX_IPSEC_NUM; i++) {
                sprintf(string, "entry%d_en", i);
                if (i<MAX_CONN_NUM)
                    cfg_obj_set_object_attr(nodeName, string, 0, "YES");
                else
                    cfg_obj_set_object_attr(nodeName, string, 0, "N/A");
            }
        }
        isEntryEnSet=1;
    }

    return cfg_type_default_func_get(path,attr,val,len); 
} 

int cfg_type_ipsec_func_commit(char* path)
{
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_IPSEC_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

static char* cfg_type_ipsec_index[] = { 
	 "ipsec_id", 
	 NULL 
};

static cfg_node_ops_t cfg_type_ipsec_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
 	 .create = cfg_type_default_func_create, /* for saving more than one entries after reboot */
	 .delete = cfg_type_default_func_delete, /* for tcWebApi_unset */
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipsec_func_commit 
}; 

static cfg_node_type_t cfg_type_ipsec_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_IPSEC_NUM, /* for Entry0~7 */
	 .parent = &cfg_type_ipsec,
	 .index = cfg_type_ipsec_index,
	 .ops = &cfg_type_ipsec_entry_ops, 
}; 

static cfg_node_ops_t cfg_type_ipsec_ops  = { 
	 .get = cfg_type_ipsec_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_type_t* cfg_type_ipsec_child[] = { 
	 &cfg_type_ipsec_entry, 
	 NULL 
}; 

cfg_node_type_t cfg_type_ipsec = { 
	 .name = "IPSEC", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, /* for customized .get function */
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_ipsec_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_ipsec_child, 
	 .ops = &cfg_type_ipsec_ops, 
}; 
