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

#define ADSL_SH				"/etc/adsl.sh"
#define ADSL_ATTR_MODE		"MODULATIONTYPE"
#define ADSL_ATTR_TYPE		"ANNEXTYPEA"
#define ADSL_NAME_INDEX 	0
#define ADSL_CMD_INDEX 		1
#define ADSL_OPEN_CMD 		"wan adsl opencmd"
#define ADSL_SETANNEX_CMD 	"wan ghs set annex"
#define ATTR_SIZE 			32
#define ADSL_PATH 			"/etc/adsl.conf"
#define SAR_OPEN_CMD 		"wan adsl open"
#define SAR_CLOSE_CMD 		"wan adsl close"

int svc_cfg_boot_adsl(void)
{
	FILE *startupSh=NULL;
	int ret = -1;

	startupSh=fopen("/etc/adsl.sh","r");
	if(startupSh)
	{
		fclose(startupSh);
		ret = chmod("/etc/adsl.sh" ,755);	
		system("/etc/adsl.sh");
	}
	else
	{
		system("wan ghs set annex al");
		system("wan adsl reset");
	}
	return SUCCESS;
}

int adsl_write(char *path){
	int retval = 0;
	char adsl_cmd[3][40];
	char buf[128] = {0};
	char tmp[128] = {0};
	int i = 0;

	char adsl_mode[][2][ATTR_SIZE]=
	{
		{"Auto Sync-Up", "multimode"},			/*support ADSL+VDSL*/
	#if defined(TCSUPPORT_WAN_PTM)
	#if defined(TCSUPPORT_CWMP_VDSL)
		{"ADSL Auto Sync-Up", "adsl2plus_multi"},	/*support ADSL AUTO*/
	#endif
        {"VDSL2", "vdsl2"},
	#endif
		{"ADSL2+", "adsl2plus"},
		{"ADSL2", "adsl2"},
		{"G.DMT", "gdmt"},
		{"T1.413", "t1.413"},
		{"G.lite", "glite"},
		{"",""},
	};

	char adsl_type[][2][ATTR_SIZE]=
	{
		{"ANNEX A","a"},
		{"ANNEX I","i"},
		{"ANNEX A/L","al"},
		{"ANNEX M","m"},
		{"ANNEX A/I/J/L/M","aijlm"},
		{"ANNEX B", "b"},
		{"ANNEX J", "j"},
		{"ANNEX B/J/M", "bjm"},
		{"",""},
	};

	retval = cfg_save_attrs_to_file(path, ADSL_PATH, QMARKS);

	/*Create adsl.sh*/
	if(cfg_obj_query_object(path, NULL, NULL) > 0)
	{

		for(i = 0; strlen(adsl_mode[i][0])!=0; i++)
		{
		
			if(cfg_obj_get_object_attr(path, ADSL_ATTR_MODE, 0, tmp, sizeof(tmp)) < 0)
			{
				tcdbg_printf("Can't get adsl attr mode\n");
				continue;
			}
			if(strcmp(tmp, adsl_mode[i][ADSL_NAME_INDEX]) == 0)
			{
				snprintf(adsl_cmd[0], 40, "%s %s\n", ADSL_OPEN_CMD, adsl_mode[i][ADSL_CMD_INDEX]);
				break;
			}
		}

		for(i = 0; strlen(adsl_type[i][0])!=0; i++)
		{
			if(cfg_obj_get_object_attr(path, ADSL_ATTR_TYPE, 0, tmp, sizeof(tmp)) < 0)
			{
				tcdbg_printf("Can't get adsl attr type\n");
				continue;
			}
			if(strcmp(tmp, adsl_type[i][ADSL_NAME_INDEX]) == 0)
			{
				snprintf(adsl_cmd[1], 40, "%s %s\n", ADSL_SETANNEX_CMD, adsl_type[i][ADSL_CMD_INDEX]);
				break;
			}
		}
		snprintf(buf, sizeof(buf), "%s%s", adsl_cmd[0], adsl_cmd[1]);
		if(cfg_obj_get_object_attr(path, "Active", 0, tmp, sizeof(tmp)) >= 0)
		{
			if(strcmp(tmp, "Yes") == 0)
				snprintf(adsl_cmd[2], 40, "%s\n", SAR_OPEN_CMD);	
			if(strcmp(tmp, "No") == 0)
				snprintf(adsl_cmd[2], 40, "%s\n", SAR_CLOSE_CMD);
                
            snprintf(buf, sizeof(buf), "%s%s", buf, adsl_cmd[2]);	
		}
		write2file(buf, ADSL_SH);
	}
	return retval;
}/* end adsl_write */

int cfg_type_adsl_func_commit(char *path)
{
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char temp[4]={0};

	adsl_write(path);
	
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, ADSL_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_obj_get_object_attr(nodeName, "Active", 0, temp, sizeof(temp));
	system(ADSL_SH);
	
	if(strcmp(temp, "No") != 0)
		system("wan adsl reset");
	return SUCCESS;
}

static cfg_node_ops_t cfg_type_adsl_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_adsl_func_commit 
}; 


static cfg_node_type_t cfg_type_adsl_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_adsl, 
	 .ops = &cfg_type_adsl_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_adsl_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_adsl_child[] = { 
	 &cfg_type_adsl_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_adsl = { 
	 .name = "Adsl", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_adsl_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_adsl_child, 
	 .ops = &cfg_type_adsl_ops, 
}; 
