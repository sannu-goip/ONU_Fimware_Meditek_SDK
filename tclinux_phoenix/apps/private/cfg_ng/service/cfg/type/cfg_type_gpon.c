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

#include "../../common/pon_svc.h" 
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
#include "xpon_public_const.h"
#include <pon_api.h>
#endif
#endif

#define ENCRYPTION_STATE_ALL	"EncryptionStateALL"
#define MAXSIZE	160
#define GPON_OMCC_GEMPORTID    1022


extern int cfg_pon_send_event(unsigned char parent,unsigned char child,unsigned int sub_event,char* attr, char* val); 

static int cfg_type_gpon_common_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
}

static int cfg_type_gpon_common_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_COMMON,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_common_ops  = { 
	 .get = cfg_type_gpon_common_func_get, 
	 .set = cfg_type_gpon_common_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_common_ops, 
}; 


static int cfg_type_gpon_onu_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_onu_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_ONU,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static int cfg_type_gpon_onu_func_commit(char* path) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_ONU,CFG_PON_COMMIT,NULL,NULL);
  
	return cfg_type_default_func_commit(path); 
} 

static cfg_node_ops_t cfg_type_gpon_onu_ops  = { 
	 .get = cfg_type_gpon_onu_func_get, 
	 .set = cfg_type_gpon_onu_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_gpon_onu_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_onu = { 
	 .name = "ONU", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_onu_ops, 
}; 


static int cfg_type_gpon_olt_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_olt_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_OLT,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_olt_ops  = { 
	 .get = cfg_type_gpon_olt_func_get, 
	 .set = cfg_type_gpon_olt_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_olt = { 
	 .name = "OLT", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_olt_ops, 
}; 


static int cfg_type_gpon_ani_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_ani_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_ANI,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_ani_ops  = { 
	 .get = cfg_type_gpon_ani_func_get, 
	 .set = cfg_type_gpon_ani_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_ani = { 
	 .name = "ANI", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_ani_ops, 
}; 


static int cfg_type_gpon_tcont_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_gpon_tcont_ops  = { 
	 .get = cfg_type_gpon_tcont_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_tcont = { 
	 .name = "TCONT", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_tcont_ops, 
}; 


static int cfg_type_gpon_gemport_func_get(char* path,char* attr, char* val,int len) 
{ 
	char buffer[1024] = {0};	
	char tempBuffer[MAXSIZE] = {0};
	int i = 0;
	int totalCnt = 0;
	int xpon_mode = 0;
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
	struct XMCS_GemPortInfo_S *gponGemInfo;
	blapi_pon_get_onuMode_after_PON_Start(&xpon_mode);

	gponGemInfo = malloc(sizeof(struct XMCS_GemPortInfo_S));	
	if(NULL == gponGemInfo){
		printf("func:%s line:%d kzalloc fail\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if(strcmp(attr,ENCRYPTION_STATE_ALL) == 0) 
	{		
		if (cfg_pon_get_info(METHD_GPON_GEMPORT, (void *)gponGemInfo)){
			printf("%s: get method GPON_GEMPORT\n",__FUNCTION__);
			free(gponGemInfo);
			return -1;
		}
		memset(buffer, 0, 1024);
		for(i=0 ; i < gponGemInfo->entryNum ; i++) 
		{
			if ( GPON_UNICAST_GEM != gponGemInfo->info[i].gemType )
				continue;
            
            if(xpon_mode == XMCS_IF_WAN_DETECT_MODE_XGPON || xpon_mode == XMCS_IF_WAN_DETECT_MODE_XGSPON)
            {
    			if(GPON_OMCC_GEMPORTID > gponGemInfo->info[i].gemPortId)
    				continue;
    		}


			memset(tempBuffer, 0, sizeof(tempBuffer));
			totalCnt += snprintf(tempBuffer, sizeof(tempBuffer)-1, "%d:%d#",
				gponGemInfo->info[i].gemPortId, gponGemInfo->info[i].enMode);
			if ( totalCnt >= 1024 )
				break;
			strcat(buffer, tempBuffer);
		}
		cfg_set_object_attr(GPON_GEMPORT_NODE, attr, buffer);
	}
	free(gponGemInfo);
#endif
#endif

	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_gemport_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_GEMPORT,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_gemport_ops  = { 
	 .get = cfg_type_gpon_gemport_func_get, 
	 .set = cfg_type_gpon_gemport_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_gemport = { 
	 .name = "GEMPort", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_gemport_ops, 
}; 


static int cfg_type_gpon_trtcm_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_gpon_trtcm_ops  = { 
	 .get = cfg_type_gpon_trtcm_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_trtcm = { 
	 .name = "Trtcm", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_trtcm_ops, 
}; 


static int cfg_type_gpon_softimage0_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_softimage0_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_SOFTIMAGE_0,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_softimage0_ops  = { 
	 .get = cfg_type_gpon_softimage0_func_get, 
	 .set = cfg_type_gpon_softimage0_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_softimage0 = { 
	 .name = "SoftImage0", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_softimage0_ops, 
}; 


static int cfg_type_gpon_softimage1_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_softimage1_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_SOFTIMAGE_1,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_softimage1_ops  = { 
	 .get = cfg_type_gpon_softimage1_func_get, 
	 .set = cfg_type_gpon_softimage1_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_softimage1 = { 
	 .name = "SoftImage1", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_softimage1_ops, 
}; 


static int cfg_type_gpon_capability_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_capability_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_CAPABILITY,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static cfg_node_ops_t cfg_type_gpon_capability_ops  = { 
	 .get = cfg_type_gpon_capability_func_get, 
	 .set = cfg_type_gpon_capability_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_capability = { 
	 .name = "Capability", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_capability_ops, 
}; 


static int cfg_type_gpon_loidauth_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 

static int cfg_type_gpon_loidauth_func_set(char* path,char* attr, char* val) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_LOIDAUTH,CFG_PON_SET,attr,val);
  
	return cfg_type_default_func_set(path,attr,val); 
} 

static int cfg_type_gpon_loidauth_func_commit(char* path) 
{ 
	cfg_pon_send_event(SVC_PON_CFG_TYPE_GPON,GPON_CHILD_LOIDAUTH,CFG_PON_COMMIT,NULL,NULL);
#if !defined(TCSUPPORT_CMCC)
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "romfileChg", "1");
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "OLTRegCnt", "0");
#endif
  
	return cfg_type_default_func_commit(path); 
} 

static cfg_node_ops_t cfg_type_gpon_loidauth_ops  = { 
	 .get = cfg_type_gpon_loidauth_func_get, 
	 .set = cfg_type_gpon_loidauth_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_gpon_loidauth_func_commit 
}; 


static cfg_node_type_t cfg_type_gpon_loidauth = { 
	 .name = "LOIDAuth", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_gpon, 
	 .ops = &cfg_type_gpon_loidauth_ops, 
}; 


static cfg_node_ops_t cfg_type_gpon_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_gpon_child[] = { 
	 &cfg_type_gpon_common, 
	 &cfg_type_gpon_onu, 
	 &cfg_type_gpon_olt, 
	 &cfg_type_gpon_ani, 
	 &cfg_type_gpon_tcont, 
	 &cfg_type_gpon_gemport, 
	 &cfg_type_gpon_trtcm, 
	 &cfg_type_gpon_softimage0, 
	 &cfg_type_gpon_softimage1, 
	 &cfg_type_gpon_capability, 
	 &cfg_type_gpon_loidauth, 
	 NULL 
}; 


cfg_node_type_t cfg_type_gpon = { 
	 .name = "GPON", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_gpon_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_gpon_child, 
	 .ops = &cfg_type_gpon_ops, 
}; 
