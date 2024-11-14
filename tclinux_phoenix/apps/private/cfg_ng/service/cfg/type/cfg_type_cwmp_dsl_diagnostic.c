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
#include "blapi_xdsl.h"
#define MAX_BUF_SIZE						2048
#ifdef CWMP
#define DSL_DIAGNOSTIC_PATH "/proc/tc3162/adsl_cwmp_diagnostic"
#define MAX_SPLIT_LEN 		512
#define MAX_SPLIT_OFFSET 	9/* mod 512*/
/*because the statistic data is very large,so we use more proc file to transfer from kernal space to user space */
#define OTHER_PROFILE_INDEX 		0
#define HLINpsds_PROFILE_INDEX_1 	1
#define HLINpsds_PROFILE_INDEX_2 	2
#define QLINpsds_PROFILE_INDEX 		3
#define SNRpsds_PROFILE_INDEX 		4
#define BITSpsds_PROFILE_INDEX 		5
#define GAINSpsds_PROFILE_INDEX 	6

#if defined(TCSUPPORT_CT_WAN_PTM)
#define HLOGpsds_PROFILE_INDEX 	7
#define HLOGpsus_PROFILE_INDEX 	8
#define HLINpsus_PROFILE_INDEX_1 	9
#define HLINpsus_PROFILE_INDEX_2 	10
#define QLNpsus_PROFILE_INDEX 	11
#define SNRpsus_PROFILE_INDEX 	12
#define SATNds_PROFILE_INDEX 	13
#define SATNus_PROFILE_INDEX 	14
#define LATNds_PROFILE_INDEX 	15
#define LATNus_PROFILE_INDEX 	16
#endif
#endif

void splitValue(char *strbuf,char*attr,char *nodeName)
{ 
	int i = 0;
	char *tmpBuf_total = NULL;
	int reallen = 0,splitnum = 0;
	char attrName[20] = {'\0'};
	char attrNameBuf[64] = {'\0'};
	char attrNumBuf[10] = {'\0'};
	char attrBuf[576] = {'\0'};
	char index[10] = {'\0'};
	
	tmpBuf_total = strbuf;
	reallen = strlen(tmpBuf_total);
	strncpy(attrName,attr, sizeof(attrName) - 1);

	if(reallen % MAX_SPLIT_LEN)
		splitnum = (reallen >> MAX_SPLIT_OFFSET) + 1;
	else
		splitnum = reallen >> MAX_SPLIT_OFFSET;

	if(splitnum == 1) /*if there is only one attr of this struct, set buf to cfg node.*/
	{ 
		cfg_obj_set_object_attr(nodeName, attrName, 0, tmpBuf_total);
	}
	else
	{
		/*set the split number*/
		strncpy(attrNameBuf, attrName, sizeof(attrNameBuf) - 1);
		strcat(attrNameBuf, "Len");
		snprintf(attrNumBuf, sizeof(attrNumBuf),"%d", splitnum);
		cfg_obj_set_object_attr(nodeName, attrName, 0, attrNumBuf);
		/*split the data and store*/
		for(i = 0; i < splitnum; i++)/*set attr like attrName0,attrName1... to cfg node.*/
		{	
			if(i == splitnum - 1)
			{
				strncpy(attrBuf, tmpBuf_total + i*MAX_SPLIT_LEN, reallen % MAX_SPLIT_LEN);
			}
			else
			{
				strncpy(attrBuf, tmpBuf_total + i*MAX_SPLIT_LEN, MAX_SPLIT_LEN);
			}
			strncpy(attrNameBuf, attrName, sizeof(attrNameBuf) - 1);
			snprintf(index, sizeof(index),"%d", i);
			strcat(attrNameBuf, index);
			cfg_obj_set_object_attr(nodeName, attrNameBuf, 0, attrBuf);
			memset(attrBuf, 0, sizeof(attrBuf));
		}
	}
}


int cfg_type_cwmp_dsl_diagnostic_read(char* path,char* attr,char* val,int len)
{
	char nodeName[64] = {0};
	char *tmpBuf = NULL;
	char *tmpBuf_total = NULL;
	char tmppath[128] = {0};
	char retbuf[32] = {0};
	char attrName[32] = {0};
	int i=0;
	char filePath[128] = {0};
	memset (filePath, 0, sizeof (filePath));
	blapi_xdsl_get_cwmpdiag_file(filePath, sizeof(filePath));
	
	if(cfg_query_object(path,NULL,NULL) <= 0)
	{
		tcdbg_printf("\r\nno node!");
		return FAIL;
	}

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, CWMPDSLDIAGNOSTIC_COMMON_NODE, sizeof(nodeName)-1);
	
	if(attr != NULL)
	{
		tcdbg_printf("the attr is %s",attr);
		if(strncmp(attr,"HLINpsds",8) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*4);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
			
			tmpBuf = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf == NULL)
			{
				tcdbg_printf("\r\nerror!");
				free(tmpBuf_total);
				return FAIL;
			}
			for(i = 1; i < 3 ;i++)
			{
				memset(tmpBuf, 0, MAX_BUF_SIZE*2);
				snprintf(tmppath, sizeof(tmppath),"%s%d",filePath,i);
				fileRead(tmppath, tmpBuf, MAX_BUF_SIZE*2 - 1);
				snprintf(tmpBuf_total, sizeof(tmpBuf_total), "%s",tmpBuf);
			}
			free(tmpBuf);
			strncpy(attrName,"HLINpsds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,CWMPDSLDIAGNOSTIC_COMMON_NODE);
			free(tmpBuf_total);		
		}
		else if(strncmp(attr,"QLNpsds",7) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}

			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,QLINpsds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			strncpy(attrName,"QLNpsds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"SNRpsds",7) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
			tcdbg_printf("\r\nenter SNRpsds");
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,SNRpsds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);

			strncpy(attrName,"SNRpsds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"BITSpsds",8) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
			tcdbg_printf("\r\nenter BITSpsds");
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,BITSpsds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);

			strncpy(attrName,"BITSpsds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"GAINSpsds",9) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}

			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,GAINSpsds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);

			strncpy(attrName,"GAINSpsds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
#if defined(TCSUPPORT_CT_WAN_PTM)
		else if(strncmp(attr,"HLOGpsds",8) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,HLOGpsds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"HLOGpsds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"HLOGpsus",8) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,HLOGpsus_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"HLOGpsus", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		if(strncmp(attr,"HLINpsus",8) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*4);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
			
			tmpBuf = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf == NULL)
			{
				tcdbg_printf("\r\nerror!");
				free(tmpBuf_total);
				return FAIL;
			}
			for(i = 9; i < 11 ;i++)
			{
				memset(tmpBuf, 0, MAX_BUF_SIZE*2);
				snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,i);
				fileRead(tmppath, tmpBuf, MAX_BUF_SIZE*2 - 1);
				snprintf(tmpBuf_total, sizeof(tmpBuf_total), "%s",tmpBuf);
			}
			free(tmpBuf);
			strncpy(attrName,"HLINpsus", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);		
		}
		else if(strncmp(attr,"QLNpsus",7) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,QLNpsus_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"QLNpsus", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"SNRpsus",7) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,SNRpsus_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"SNRpsus", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"SATNds",6) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0,MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,SATNds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"SATNds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"SATNus",6) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,SATNus_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"SATNus", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"LATNds",6) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,LATNds_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"LATNds", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
		else if(strncmp(attr,"LATNus",6) == 0)
		{
			tmpBuf_total = malloc(MAX_BUF_SIZE*2);
			if(tmpBuf_total == NULL)
			{
				tcdbg_printf("\r\nerror!");
				return FAIL;
			}
		
			memset(tmpBuf_total, 0, MAX_BUF_SIZE*2);
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,LATNus_PROFILE_INDEX);
			fileRead(tmppath, tmpBuf_total, MAX_BUF_SIZE*2 - 1);
			
			strncpy(attrName,"LATNus", sizeof(attrName) - 1);
			splitValue(tmpBuf_total,attrName,nodeName);
			free(tmpBuf_total);
		}
#endif
		else
		{
			snprintf(tmppath, sizeof(tmppath), "%s%d",filePath,OTHER_PROFILE_INDEX);
			/*set the diagnosticstate*/
			/*memset(retbuf,0,sizeof(retbuf));//no used,because do it in function get_profile_str()*/
			get_profile_str("DiagnosticState=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"DiagnosticState", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set ACTPSDds*/
			get_profile_str("ACTPSDds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"ACTPSDds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set ACTPSDus*/
			get_profile_str("ACTPSDus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"ACTPSDus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set ACTATPds*/
			get_profile_str("ACTATPds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"ACTATPds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set ACTATPus*/
			get_profile_str("ACTATPus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"ACTATPus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLINSCds*/
			get_profile_str("HLINSCds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLINSCds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
#if defined(TCSUPPORT_CT_WAN_PTM)
			/*set HLINSCus*/
			get_profile_str("HLINSCus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLINSCus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLINGds*/
			get_profile_str("HLINGds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLINGds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLINGus*/
			get_profile_str("HLINGus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLINGus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLOGGds*/
			get_profile_str("HLOGGds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLOGGds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLOGGus*/
			get_profile_str("HLOGGus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLOGGus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLOGMTds*/
			get_profile_str("HLOGMTds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLOGMTds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set HLOGMTus*/
			get_profile_str("HLOGMTus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"HLOGMTus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set QLNGds*/
			get_profile_str("QLNGds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"QLNGds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set QLNGus*/
			get_profile_str("QLNGus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"QLNGus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set QLNMTds*/
			get_profile_str("QLNMTds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"QLNMTds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set QLNMTus*/
			get_profile_str("QLNMTus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"QLNMTus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set SNRGds*/
			get_profile_str("SNRGds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"SNRGds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set SNRGus*/
			get_profile_str("SNRGus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"SNRGus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set SNRMTds*/
			get_profile_str("SNRMTds=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"SNRMTds", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
			/*set SNRMTus*/
			get_profile_str("SNRMTus=",retbuf, sizeof(retbuf), NO_QMARKS,tmppath);
			strncpy(attrName,"SNRMTus", sizeof(attrName) - 1);
			cfg_obj_set_object_attr(nodeName, attrName, 0, retbuf);
#endif
		}
	}
	else
	{
		tcdbg_printf("the attr is NULL!");
	}

	return SUCCESS;
}



int cfg_type_cwmp_dsl_diagnostic_func_get(char* path,char* attr,char* val,int len) {
	cfg_type_cwmp_dsl_diagnostic_read(path,attr,val,len);
	return cfg_obj_get_object_attr(path,attr,0,val,len);

}



static cfg_node_ops_t cfg_type_cwmp_dsl_diagnostic_common_ops  = { 
	 .get = cfg_type_cwmp_dsl_diagnostic_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_cwmp_dsl_diagnostic_common = { 
	 .name = "Common", 
	 .flag =  CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1, 
	 .parent = &cfg_type_cwmp_dsl_diagnostic, 
	 .ops = &cfg_type_cwmp_dsl_diagnostic_common_ops, 
}; 


static cfg_node_ops_t cfg_type_cwmp_dsl_diagnostic_ops  = { 
	 .get = cfg_type_cwmp_dsl_diagnostic_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_cwmp_dsl_diagnostic_child[] = { 
	 &cfg_type_cwmp_dsl_diagnostic_common, 
	 NULL 
}; 

cfg_node_type_t cfg_type_cwmp_dsl_diagnostic = { 
	 .name = "CDSLDiagnostic", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MEMORY | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_cwmp_dsl_diagnostic_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_cwmp_dsl_diagnostic_child, 
	 .ops = &cfg_type_cwmp_dsl_diagnostic_ops, 
}; 
