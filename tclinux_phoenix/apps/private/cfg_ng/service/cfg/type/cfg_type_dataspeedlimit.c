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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"
#include "blapi_traffic.h"

#define DSLIMIT_MODE_DISABLE 0
#define DSLIMIT_MODE_IFACE 1
#define DSLIMIT_MODE_VLANID 2
#define DSLIMIT_MODE_IPRANGE 3
#define DSLIMIT_MAX_QUEUE_NUM 6
#define DSLIMIT_UP 1
#define DSLIMIT_DW 2

#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
int commit_maxbandwidth()
{
	int idx = 0;
	char maxBandWidthPath[32] = {0}, upRate[64] = {0}, downRate[64] = {0};

	for(idx = 0; idx < CFG_TYPE_MAX_MAXBANDWIDTH_ENTRY_NUM; idx++)
	{
		snprintf(maxBandWidthPath, sizeof(maxBandWidthPath), MAXBANDWIDTH_ENTRY_NODE, idx);
		if(cfg_obj_get_object_attr(maxBandWidthPath, "upRate", 0, upRate, sizeof(upRate)) > 0 || 
			cfg_obj_get_object_attr(maxBandWidthPath, "downRate", 0, downRate, sizeof(downRate)) > 0)
		{
			cfg_obj_commit_object("root.MaxBandWidth");
			break;
		}
	}
	
	return SUCCESS;
}
#endif

int  dslimit_get_node_info_by_mode(int updwMode, int dsMode, char* dsValue, int size)
{
	char attri[32] = {0};
	int ret = -1;

	if(updwMode == DSLIMIT_UP)	
	{
		if(dsMode == DSLIMIT_MODE_IFACE )
		{
			strncpy(attri, "InterfaceLimitUP",sizeof(attri)-1);
		}
		else if(dsMode == DSLIMIT_MODE_VLANID )
		{
			strncpy(attri, "VlanTagLimitUP", sizeof(attri)-1);
		}
		else if(dsMode == DSLIMIT_MODE_IPRANGE )
		{
			strncpy(attri, "IPLimitUP", sizeof(attri)-1);
		}
	}
	else if(updwMode == DSLIMIT_DW)
	{
		if(dsMode == DSLIMIT_MODE_IFACE )
		{
			strncpy(attri, "InterfaceLimitDOWN",sizeof(attri)-1);
		}
		else if(dsMode == DSLIMIT_MODE_VLANID )
		{
			strncpy(attri, "VlanTagLimitDOWN", sizeof(attri)-1);
		}
		else if(dsMode == DSLIMIT_MODE_IPRANGE )
		{
			strncpy(attri, "IPLimitDOWN",sizeof(attri)-1);
		}
	}

	cfg_obj_get_object_attr(DATASPEEDLIMIT_ENTRY_NODE, attri, 0, dsValue, size);

	return SUCCESS;
}

int dislimit_set_qos_car_discipline_rule(int g_queue_speed[])
{
	int idx = 0;
	int queue_idx = 0;
	int queueSpeed = 0;
	char tmpvAttr[24] = {0};
	char tmpvValue[24] = {0};

	cfg_set_object_attr(QOS_COMMON_NODE, "Discipline", "CAR" );
	cfg_set_object_attr(QOS_COMMON_NODE, "QoSOptType", "discRule" );

	for ( idx = 1; idx <= DSLIMIT_MAX_QUEUE_NUM; idx ++ )
	{
		queue_idx = idx - 1;
		/* queue index */
		bzero(tmpvAttr, sizeof(tmpvAttr));
		bzero(tmpvValue, sizeof(tmpvValue));
		snprintf(tmpvAttr, sizeof(tmpvAttr), "QueueP1%d", idx);
		snprintf(tmpvValue, sizeof(tmpvValue), "%d", idx);
		cfg_set_object_attr(QOS_COMMON_NODE, tmpvAttr, tmpvValue );

		/* queue bandwidth */
		bzero(tmpvAttr, sizeof(tmpvAttr));
		bzero(tmpvValue, sizeof(tmpvValue));
		snprintf(tmpvAttr, sizeof(tmpvAttr), "QueueBW%d", idx);

		queueSpeed = g_queue_speed[queue_idx];
		
		snprintf(tmpvValue, sizeof(tmpvValue),"%d", queueSpeed);
		cfg_set_object_attr(QOS_COMMON_NODE, tmpvAttr, tmpvValue );

		/* queue switch */
		bzero(tmpvAttr, sizeof(tmpvAttr));
		bzero(tmpvValue, sizeof(tmpvValue));
		snprintf(tmpvAttr, sizeof(tmpvAttr), "QueueSW%d", idx);
		if ( 0 == queueSpeed )
		{
			snprintf(tmpvValue, sizeof(tmpvValue), "%s", "No");
		}	
		else
		{
			snprintf(tmpvValue, sizeof(tmpvValue), "%s", "Yes");
		}
		cfg_set_object_attr(QOS_COMMON_NODE, tmpvAttr, tmpvValue );
	}

	cfg_commit_object(QOS_COMMON_NODE);  

	return SUCCESS;
}

int handle_up_data_speed_limit()
{
	char dsupmode[12] = {0};
	int i_dsupmode = 0;
	int ret = -1;
	int idx = 0;
	char dsValue[512] = {0};
	int g_queue_speed[DSLIMIT_MAX_QUEUE_NUM] = {0};
	
	if(cfg_obj_get_object_attr(DATASPEEDLIMIT_ENTRY_NODE, "SpeedLimitModeUP", 0, dsupmode, sizeof(dsupmode)) < 0)
	{
		strncpy(dsupmode, "0", sizeof(dsupmode)-1);
	}
	i_dsupmode = atoi(dsupmode);
	
	dslimit_get_node_info_by_mode(DSLIMIT_UP, i_dsupmode, dsValue, sizeof(dsValue));
	
	bzero(g_queue_speed, sizeof(g_queue_speed));
	ret = blapi_traffic_dslimit_set_up_data( i_dsupmode, dsValue, g_queue_speed);
	if(ret != SUCCESS)
	{
		return FAIL;
	}

	if(i_dsupmode)
	{
		dislimit_set_qos_car_discipline_rule(g_queue_speed);
	}

	return SUCCESS;
}

int handle_down_data_speed_limit()
{
	char dsdwmode[12] = {0};
	int i_dsdwmode = 0;
	char tmpvValue[24] = {0};
	int ret = -1;
	int is_boot = 0;
	char dsValue[512] = {0};
	
	if(cfg_obj_get_object_attr(DATASPEEDLIMIT_ENTRY_NODE, "SpeedLimitModeDOWN", 0, dsdwmode, sizeof(dsdwmode)) < 0)
	{
		strncpy(dsdwmode, "0", sizeof(dsdwmode)-1);
	}	
	i_dsdwmode = atoi(dsdwmode);

#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
	bzero(tmpvValue, sizeof(tmpvValue));
	if(cfg_obj_get_object_attr(DATASPEEDLIMIT_ENTRY_NODE, "isBOOT", 0, tmpvValue, sizeof(tmpvValue)) > 0 && 0 == strcmp(tmpvValue, "1"))
	{
		cfg_set_object_attr(DATASPEEDLIMIT_ENTRY_NODE, "isBOOT", "" );
		is_boot = 1;
	}
#endif

	dslimit_get_node_info_by_mode(DSLIMIT_DW, i_dsdwmode, dsValue, sizeof(dsValue));

	ret = blapi_traffic_dslimit_set_down_data(i_dsdwmode, dsValue, is_boot);
	if(ret != SUCCESS)
	{
		return FAIL;
	}
	
#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
	if(i_dsdwmode == DSLIMIT_MODE_DISABLE)
	{
		commit_maxbandwidth();
	}
#endif	

	return SUCCESS;
}

int svc_cfg_boot_dataspeedlimit(void)
{
	int ret = -1;
	if( SUCCESS != handle_up_data_speed_limit() || SUCCESS != handle_down_data_speed_limit() )	
	{
		ret = FAIL;
	}
	else
	{
		ret = SUCCESS;
	}
	
#if defined(TCSUPPORT_CT_HWQOS)
	system("/userfs/bin/hw_nat -!");
#endif

	return ret;
}

int cfg_type_dataspeedlimit_execute( )
{
	return svc_cfg_boot_dataspeedlimit();
}


int cfg_type_dataspeedlimit_func_commit(char* path)
{
	return cfg_type_dataspeedlimit_execute( );
}

static cfg_node_ops_t cfg_type_dataspeedlimit_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dataspeedlimit_func_commit 
}; 


static cfg_node_type_t cfg_type_dataspeedlimit_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_dataspeedlimit, 
	 .ops = &cfg_type_dataspeedlimit_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_dataspeedlimit_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_dataspeedlimit_func_commit 
}; 


static cfg_node_type_t* cfg_type_dataspeedlimit_child[] = { 
	 &cfg_type_dataspeedlimit_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dataspeedlimit = { 
	 .name = "DataSpeedLimit", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dataspeedlimit_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dataspeedlimit_child, 
	 .ops = &cfg_type_dataspeedlimit_ops, 
}; 
