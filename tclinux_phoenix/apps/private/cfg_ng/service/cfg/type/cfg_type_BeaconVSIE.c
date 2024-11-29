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
#include <time.h>
#include <sys/socket.h>    /* for connect and socket*/
#include <sys/stat.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/if.h>
#include <fcntl.h>
#include <linux/wireless.h>
#include <linux/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <libapi_lib_wifimgr.h>
#include <cfg_cli.h> 
#include <cfg_api.h>
#include "cfg_types.h"
#include "cfg_type_BeaconVSIE.h"
#include "../event_api.h"
#include <utility.h>

int hex2num_vendie(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

int vendie_aton(unsigned char *txt, char *vend_ie_driver_argu)
{
	int i = 0;
	while(1) {
		int a, b;
		
		a = hex2num_vendie(*txt++);
		
		if (a < 0)
		{
			printf("vendie_aton,a=%d\r\n",a);
			return -1;
		}
		b = hex2num_vendie(*txt++);
		if (b < 0)
		{
			printf("vendie_aton,b=%d\r\n",b);
			return -1;
		}
		*vend_ie_driver_argu++ = (a << 4) | b;
		
		if (*txt == ',')
		{
			txt++;
			++i;
			continue;

		}
		if ( *txt++ == '\0')
			break;
	}
	
	++i;
	*vend_ie_driver_argu = '\0';
	return i;
}

/*------------------------/
/		 ioctl function		   /										
/------------------------*/
int del_VSIE_by_interface(int interfaceidx)
{
	char nodePath[64] = {0};
	char value[1000];
	char interfacebuf[10] = {0};
	if(interfaceidx<0 || interfaceidx > MAX_INDEX_NUM -1)
	{
		tcdbg_printf("del_VSIE_by_interface:interface index error");
	}
		
	if(interfaceidx < MAX_INDEX_NUM/2)
		sprintf(interfacebuf, "ra%d", interfaceidx);
	else
		sprintf(interfacebuf, "rai%d", interfaceidx-MAX_INDEX_NUM/2);
					
	if (wifimgr_lib_OidQueryInformation(RT_OID_AP_VENDOR_IE_DEL, interfacebuf, NULL, 0) < 0)
	{
		tcdbg_printf("doVendorIESet() ERROR:RT_OID_AP_VENDOR_IE_DEL\n");
		return FAIL;			
	}
	else
	{
		printf("vendor IE del OK\n");		
		return SUCCESS;
	}	

}

int cfg_type_beaconvsie_entry_commit(char* path)
{
	tcdbg_printf("enter cfg_type_beaconvsie_entry_commit\n");
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_CFG_BEACONVSIE_START, (void *)&param, sizeof(param)); 
	return 0;
}

int cfg_type_beaconvsie_commit(char* path)
{
	
	return 0;
}

int cfg_type_beaconvsie_update(char* path)
{
	wlan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param.buf)-1);
	cfg_obj_send_event(EVT_WLAN_INTERNAL, EVT_CFG_BEACONVSIE_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

int insert_VSIE_to_Beacon_by_interface(char *DeviceName, void *ptr, unsigned long PtrLength)
{
	//tcdbg_printf("FUN:%s,DBG:%d\n",__FUNCTION__,__LINE__);
	/*
		Per Interface VSIE data structure:
	|--------------Entry X----------------|--------------Entry Y---------------| ......
	| ElementID 0 | LenofIEData 0 |  IEData 0  | ElementID 1 | LenofIEData 1 | IEData 1 |......
	|---1 Byte---|----1 Byte--- | ---len--- |
	*/
	if (wifimgr_lib_OidQueryInformation(RT_OID_AP_VENDOR_IE_SET, DeviceName, ptr, PtrLength) < 0)
	{
		tcdbg_printf("doVendorIESet() ERROR:RT_OID_AP_VENDOR_IE_SET\n");
		return FAIL;			
	}
	else
	{
		tcdbg_printf("vendor IE set OK\n");		
		return SUCCESS;
	}	
}



static cfg_node_ops_t cfg_type_beaconvsie_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_beaconvsie_common = { 
	 .name = "Common", 
	 .flag = 1,
	 .parent = &cfg_type_beaconvsie, 
	 .ops = &cfg_type_beaconvsie_common_ops, 
}; 


static cfg_node_ops_t cfg_type_beaconvsie_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete,
	 .commit = cfg_type_beaconvsie_entry_commit
};

static cfg_node_type_t cfg_type_beaconvsie_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_beaconvsie, 
	 .ops = &cfg_type_beaconvsie_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_beaconvsie_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set,
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_beaconvsie_commit 
}; 

static cfg_node_type_t* cfg_type_beaconvsie_child[] = { 
	 &cfg_type_beaconvsie_common,
	 &cfg_type_beaconvsie_entry,
	 NULL 
}; 


cfg_node_type_t cfg_type_beaconvsie = { 
	 .name = "BeaconVSIE", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_beaconvsie_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_beaconvsie_child, 
	 .ops = &cfg_type_beaconvsie_ops, 
}; 

