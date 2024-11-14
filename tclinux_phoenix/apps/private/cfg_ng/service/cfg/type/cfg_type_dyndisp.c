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


static int	cfg_type_dyndisp_entry_func_get_attr(int main,int off, char* val, int len)
{
	char tmp[64],path[64];
	int idx,page,mask[9],bit;

//	printf("cfg_type_dyndisp_entry_func_get_attr: [%d] [%d] \n",main,off);
	
	if (cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE,"CurrentAccess",0,tmp,sizeof(tmp)) <= 0)
	{
		printf("cfg_type_dyndisp_entry_func_get_attr: get [CurrentAccess] fail \n");
		return -1;
	}
	
	idx = atoi(tmp);

	sprintf(path,ACCOUNT_ENTRY_NODE,idx + 1);

	if (cfg_obj_get_object_attr(path,"display_mask",0,tmp,sizeof(tmp)) <= 0)
	{
		printf("cfg_type_dyndisp_entry_func_get_attr: get [display_mask] fail \n");
		return -1;
	}
	
	sscanf(tmp, "%x %x %x %x %x %x %x %x %x", &mask[0],&mask[1],&mask[2],&mask[3],&mask[4],&mask[5],&mask[6],&mask[7],&mask[8]);

	if(main)
	{
		bit = (mask[0] >> off) & 0x1;
	}
	else
	{
		if (cfg_obj_get_object_attr(DYNDISP_ENTRY_NODE,"CurPage",0,tmp,sizeof(tmp)) <= 0)
		{
			printf("cfg_type_dyndisp_entry_func_get_attr: get [CurPage] fail \n");
			return -1;
		}
		
		page = atoi(tmp);
		
		bit = (mask[page + 1] >> off) & 0x1;
	}
	
	sprintf(val,"%d",bit);
	
	return 1;
}

static int cfg_type_dyndisp_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	if (strcmp(attr,"CurPage") == 0)
		return cfg_obj_get_object_attr(path,attr,0,val,len);

	if (memcmp(attr,"MainMaskBit",11) == 0 && strlen(attr) == 12)
		return cfg_type_dyndisp_entry_func_get_attr(1,atoi(&attr[11]),val,len);

	if (memcmp(attr,"CurMaskBit",10) == 0 && strlen(attr) == 11)
		return cfg_type_dyndisp_entry_func_get_attr(0,atoi(&attr[10]),val,len);
		
	return -2;
}; 


static cfg_node_ops_t cfg_type_dyndisp_entry_ops  = { 
	 .get = cfg_type_dyndisp_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_dyndisp_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_dyndisp, 
	 .ops = &cfg_type_dyndisp_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_dyndisp_ops  = { 
	 .query = cfg_type_default_func_query 
}; 


static cfg_node_type_t* cfg_type_dyndisp_child[] = { 
	 &cfg_type_dyndisp_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dyndisp = { 
	 .name = "dynDisp", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dyndisp_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dyndisp_child, 
	 .ops = &cfg_type_dyndisp_ops, 
}; 
