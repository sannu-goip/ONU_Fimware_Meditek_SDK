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
#include <svchost_evt.h>
#include "cfg_tr181_global.h" 
#include "utility.h" 
#include "cfg_tr181_utility.h" 
#include "cfg_msg.h" 
#include <sys/msg.h>
#include <sys/ipc.h>

int updateTR181AllWLanInterface(void)
{
	char wlanPath[NODE_PATH_MAX] = {0};	
	int wifiSsidIndex = -1;
	int wifiRadioIndex = -1;
	int wifiAPIndex = -1;
	int wifiAPSecIndex = -1;
	char lowerlayer[256] = {0};
	char upperlayer[256] = {0};
	char tmp[32] = {0};
	int lanIdx = 0;
	int i = 0;
	int bridgePortIndex = -1;
	int bssidNum = 0;
	int	bssidNum5G = 0;
	char nodeNamePath[NODE_PATH_MAX] = {0};
	char wlanCommon[NODE_PATH_MAX] = {0};

	/*2.4G wifi*/
	memset(tmp,0,sizeof(tmp));		
	cfg_obj_get_object_attr(WLAN_COMMON_NODE, "BssidNum",0, tmp,sizeof(tmp));
	bssidNum = atoi(tmp);

	/*5G wifi*/
	memset(tmp,0,sizeof(tmp));		
	cfg_obj_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum",0, tmp,sizeof(tmp));
	bssidNum5G = atoi(tmp);
	
	/*WiFi.Radio*/
	wlanNodeSync2WIFIRdo(0);
	wlanNodeSync2WIFIRdo(1);	

	for(i = 0 ; i < bssidNum+bssidNum5G; i++){	
		memset(wlanCommon, 0x00, sizeof(wlanCommon));
		if(i <	bssidNum){/*2.4G*/
			lanIdx = LAN_INDEX_WLAN_START  + i;
			snprintf(wlanCommon,sizeof(wlanCommon), WLAN_COMMON_NODE);
		}else{/*5G*/
			lanIdx = LAN_INDEX_WLAN_5G_START + (i - bssidNum);	
			snprintf(wlanCommon,sizeof(wlanCommon), WLAN11AC_COMMON_NODE);
		}
		
		memset(wlanPath, 0x00, sizeof(wlanPath));
		setWlanNodeByLanPortIdx(wlanPath, lanIdx);
		
		memset(tmp, 0x00, sizeof(tmp));
		if(cfg_obj_get_object_attr(wlanPath, "EnableSSID",0, tmp,sizeof(tmp)) < 0){ 		
			tcdbg_printf("[%s:%d]wlan node not exist, i =%d,index=%d\n",__FUNCTION__,__LINE__, i,lanIdx);
			continue;
		}
		
		bridgePortIndex = createBridgePortEntry(-1,lanIdx, atoi(tmp));
		if(bridgePortIndex < 0){			
			return -1;
		}

		memset(lowerlayer, 0, sizeof(lowerlayer));
		snprintf(lowerlayer, sizeof(lowerlayer),TR181_BRIDGE_PORT"%d", bridgePortIndex+1);
		setBridgePortLowerLayer(0, lowerlayer); 
			
		/*WiFi.SSID*/
		wifiSsidIndex = createWifiSSIDEntry(lanIdx);
		if(wifiSsidIndex < 0){			
			return -1;
		}
		
		/*set upperlayer*/					
		setBridgePortUpperLayer(bridgePortIndex, TR181_BRIDGE_PORT_BR0);
		
		/*set lowerlayer*/
		memset(lowerlayer, 0, sizeof(lowerlayer));
		snprintf(lowerlayer, sizeof(lowerlayer),TR181_WIFI_SSID_IF"%d", wifiSsidIndex+1); 	
		setBridgePortLowerLayer(bridgePortIndex, lowerlayer); 
	
		memset(upperlayer,0,sizeof(upperlayer));
		snprintf(upperlayer, sizeof(upperlayer),TR181_BRIDGE_PORT"%d", bridgePortIndex+1);		
		setWifiSSIDUpperLayer(wifiSsidIndex, upperlayer);
		
		/*WiFi.AccessPoint*/
		wifiAPIndex = createWifiAPEntry(lanIdx,wlanCommon);
		if(wifiAPIndex < 0){			
			return -1;
		}else{
			memset(nodeNamePath, 0, sizeof(nodeNamePath));		
			snprintf(nodeNamePath,sizeof(nodeNamePath), TR181_WIFIAP_ENTRY_NODE, wifiAPIndex + 1);
			
			memset(lowerlayer, 0, sizeof(lowerlayer));
			snprintf(lowerlayer, sizeof(lowerlayer),TR181_WIFI_SSID_IF"%d", wifiSsidIndex+1);	
			cfg_obj_set_object_attr(nodeNamePath, "SSIDReference",0, lowerlayer);
			
		}

		/*WiFi.AccessPoint.Security*/
		wifiAPSecIndex = createWifiAPSecEntry(lanIdx);
		if(wifiAPSecIndex < 0){			
			return -1;
		}		

		/*WiFi.AccessPoint.WPS*/
		wifiAPSecIndex = createWifiAPWPSEntry(lanIdx);
		if(wifiAPSecIndex < 0){ 		
			return -1;
		}	

	}
	
	return 0;
}

int setWlanNodeByLanPortIdx(char *path, int lanPortIdx){	
	if(path == NULL){
		return FAIL;
	}
	if(lanPortIdx < 0){
		tcdbg_printf("[%s:%d]lanPortIdx=%d, error\n",__FUNCTION__,__LINE__,lanPortIdx);
		return FAIL;
	}

	if( lanPortIdx >= LAN_INDEX_WLAN_5G_START && lanPortIdx < LAN_INDEX_WLAN_5G_START + MAX_BSSID_NUM){
		/*5G node*/
		snprintf(path,NODE_PATH_MAX - 1, WLAN11AC_ENTRY_N_NODE, lanPortIdx - LAN_INDEX_WLAN_5G_START + 1);
		return IS_WLAN11AC_MODE;
	}else if( lanPortIdx >= LAN_INDEX_WLAN_START && lanPortIdx < LAN_INDEX_WLAN_5G_START ){
		/*2.4G*/
		snprintf(path,NODE_PATH_MAX - 1, WLAN_ENTRY_N_NODE, lanPortIdx - LAN_INDEX_WLAN_START + 1);
		return IS_WLAN_MODE;
	}else{
		return FAIL;
	}
}

int checkNodePath(char *path){
	int idx = -1;
	if(cfg_query_object(path,NULL,NULL) <= 0)
	{		
		idx = cfg_obj_create_object(path);
		if(idx < 0) 
		{
			tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,path);
			return FAIL;
		}
	}
	return SUCCESS;
}

int findLanPortIdxbyApIdx(int idx){
	if(idx < 0){
		return -1;
	}

	char wifiAPPath[64] = {0};
	char wifiSSIDPath[64] = {0};
	char tempValue[256] = {0};
	int index = -1;
	int ssidIdx = -1;
	memset(wifiAPPath, 0x00, sizeof(wifiAPPath));
	snprintf(wifiAPPath,sizeof(wifiAPPath), TR181_WIFIAP_ENTRY_NODE, idx);
	memset(tempValue, 0x00, sizeof(tempValue));
	
	cfg_obj_get_object_attr(wifiAPPath, "SSIDReference",0, tempValue,sizeof(tempValue));
	ssidIdx = getSuffixIntByInterface(tempValue);
	if(ssidIdx < 0){
		return -1;
	}
	
	memset(wifiSSIDPath, 0x00, sizeof(wifiSSIDPath));
	snprintf(wifiSSIDPath,sizeof(wifiSSIDPath), TR181_WIFISSID_ENTRY_NODE, ssidIdx);
	
	memset(tempValue, 0x00, sizeof(tempValue));
	cfg_obj_get_object_attr(wifiSSIDPath, LAN_INDEX_ATTR,0, tempValue,sizeof(tempValue));

	if(!isNumber(tempValue)){
		return -1;
	}

	index = atoi(tempValue);
	return index;
}

