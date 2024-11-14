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
#include <svchost_evt.h>
#include "cfg_types.h" 
#include <unistd.h>
#include "utility.h"


#define APIPERMISSION_API_PREFIX "(com.chinamobile.smartgateway.%s)\r\n"
#define APIPERMISSION_BUNDLE_NUM	40
#define APIPERMISSION_API_NUM		40
#define APIPERMISSION_PATH 		"/usr/osgi/plugin-b/%s.policy"


int apiPermission_configure()
{
	char nodeName[64]={0};
	int i = 0, j = 0, api_index = 0, num = 0;
	FILE *fp = NULL;
	char active[32] = {0};
	char api_list[APIPERMISSION_API_NUM*2+1] = {0}, api_name[256] = {0};
	char api_path[256] = {0}, buf[256] = {0};
	char index_str[8] = {0};

	/*api permission name num =36, and is arranged in alphabetical order, and must be same with tr069 apipermission */
	char *permission[] = {
		"accessservices.AccessInfoQueryService",
		"accessservices.IPDetectService",
		"accessservices.IPPingDiagnosticsService",
		"accessservices.TraceRouteDiagnosticsService",
		"accessservices.WanConfigService",
		"accessservices.HttpDownloadDiagnosticsService",
		"accessservices.HttpUploadDiagnosticsService",
		"addressservices.LANIPConfigService",
		"addressservices.LANIPInfoQueryService",
		"addressservices.PortMappingConfigService",
		"addressservices.PortMappingQueryService",
 		"commservices.DeviceAccessRightConfigService",
		"commservices.DeviceAccessRightQueryService", 
		"commservices.DeviceConfigService",
		"commservices.DeviceInfoQueryService",
		"commservices.DeviceRsetService",
		"commservices.DeviceSecretInfoQueryService",
		"commservices.RegistrationService",
		"commservices.UsbService",
		"lanservices.EthConfigService",
		"lanservices.EthQueryService",
		"lanservices.LANHostsInfoQueryService",
		"lanservices.LanHostSpeedLimitService",
		"lanservices.LanHostSpeedQueryService",
		"lanservices.LanNetworkAccessConfigService",
		"lanservices.LanNetworkNameConfigService",
		"lanservices.WlanConfigService",
		"lanservices.WlanQueryService",
		"lanservices.WlanSecretQueryService",
		"transferservices.L2TPVPNService",
		"transferservices.StaticLayer3ForwardingConfigService",
		"transferservices.TrafficDetailProcessService",
		"transferservices.TrafficForwardService",
		"transferservices.TrafficMirrorService",
		"transferservices.TrafficMonitoringConfigService",
		"transferservices.TrafficQoSService",
 		"transferservices.TransferQueryService",
		"transferservices.VLANBindConfigService",
		"voipservices.VoIPInfoQueryService",
		"voipservices.VoIPInfoSubscribeService",
	};
	
	memset(nodeName, 0, sizeof(nodeName));
	for (i=1; i <= APIPERMISSION_BUNDLE_NUM; i++) {
		snprintf(nodeName, sizeof(nodeName), APIPERMISSION_ENTRY_NODE, i);
		if (cfg_obj_get_object_attr(nodeName, "Name", 0, api_name, sizeof(api_name)) < 0) {
			continue;
		}
		
		snprintf(api_path, sizeof(api_path), APIPERMISSION_PATH, api_name);
		if (cfg_obj_get_object_attr(nodeName, "Active", 0, active, sizeof(active)) > 0) {
			if (strncmp (active, "Yes", sizeof(active))) {
				/*if active is No, delete the policy file, end the set*/
				unlink(api_path);
				continue;
			}
			
			memset(api_list, 0, sizeof(api_list));
			if (cfg_obj_get_object_attr(nodeName, "List", 0, api_list, sizeof(api_list)) >=0) {
				fp = fopen(api_path, "w+");
				if (fp == NULL) {
					printf("\r\n%s[%d]:Fail to open apifile =%s", __FUNCTION__, __LINE__, api_path);
					continue;
				}
				
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "SymbolicName: %s\r\n", api_name);
				fputs_escape(buf, fp);
				fputs("Authority: ", fp);
				num = 0;
				for (api_index= 0; api_index < strlen(api_list)/2; api_index++) {
					j = api_index*2;
					strncpy(index_str, &api_list[j], 2);

					if (index_str[0] == 0x20 && index_str[1] == 0x20) {
						/*the api index is "  ",, don't add the permission name*/
						continue;
					}
					
					j = atoi(index_str);
					if (j > APIPERMISSION_API_NUM || j <= 0) {
						continue;
					}
					num = num+1;
					
					memset(buf, 0, sizeof(buf));
					snprintf(buf, sizeof(buf), APIPERMISSION_API_PREFIX, permission[j-1]);
					fputs(buf, fp);
				}
				fclose(fp);
			}
		}
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%d", num);
		if (cfg_set_object_attr(nodeName, "Num", buf) < 0) {
			printf("apipermission_write:set Num error");
		}
	}
	return 0;
}


int cfg_type_apipermission_func_write()
{
	apiPermission_configure();
	return 0;
}


int cfg_type_apipermission_func_execute()
{
	return 0;
	
}


int svc_cfg_boot_apipermission()
{
	apiPermission_configure();
	return 0;
	
}


static int cfg_type_apipermission_func_commit(char* path)
{
	cfg_type_apipermission_func_write();
	cfg_type_apipermission_func_execute();

	return 0;
}


static cfg_node_ops_t cfg_type_apipermission_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_apipermission_func_commit 
}; 


static cfg_node_type_t cfg_type_apipermission_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | APIPERMISSION_BUNDLE_NUM, 
	 .parent = &cfg_type_apipermission, 
	 .ops = &cfg_type_apipermission_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_apipermission_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_apipermission_func_commit 
}; 


static cfg_node_type_t cfg_type_apipermission_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_apipermission, 
	 .ops = &cfg_type_apipermission_common_ops, 
}; 


static cfg_node_ops_t cfg_type_apipermission_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_apipermission_func_commit 
}; 


static cfg_node_type_t* cfg_type_apipermission_child[] = { 
	 &cfg_type_apipermission_entry, 
	 &cfg_type_apipermission_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_apipermission = { 
	 .name = "ApiPermission", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_apipermission_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_apipermission_child, 
	 .ops = &cfg_type_apipermission_ops, 
}; 
