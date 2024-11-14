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
#include <ecnt_event_global/ecnt_event_wifi.h>

#ifdef WITHVOIP
#define VOIP        1 /*use WITHVOIP define to decide using voip function or not. shnwind 20100203*/
#else
#define VOIP        0
#endif

static char webCustom_attr[][32] = 
{
#if defined(TCSUPPORT_HTBW_40M)
	{"isHTBW40M"},
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH)
	{"isPerSSIDSupport"},
#endif
#if defined(TCSUPPORT_CT_E8GUI_TY)
	{"isE8TYSupported"},
#endif
#if defined(TCSUPPORT_CT_MIDWARE)
	{"isMidwareSupported"},
#endif
#if defined(TCSUPPORT_CT_2NTP)
#if defined(TCSUPPORT_CT_NTPSERVERTYPE)
	{"isNtpServerTypeSupported"},
#endif
#endif
#ifdef WSC_AP_SUPPORT
	{"isWPSSupported"},
#endif
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	{"isMaxStaNumSupported"},
#endif
#ifdef TCSUPPORT_DMS
	{"isDMSSupported"},
#endif
#if defined(TCSUPPORT_CT_E8B_ADSL)
	{"isAdslVer"},
#endif
#if defined(TCSUPPORT_CT_DSLITE)
	{"isDSLiteSupported"},
#endif
#if defined(TCSUPPORT_CT_DSL_EX)
	{"isDslEx"},
#endif
#if defined(TCSUPPORT_CY)
	{"isCYSupported"},
#endif
#if defined(TCSUPPORT_CUC_2PORT)
	{"isCuc2PortSupported"},
#endif
#if defined(TCSUPPORT_WLAN)
	{"isWLanSupported"},
#if defined(TCSUPPORT_WLAN_AC)
	{"isWLanACSupported"},
#endif
#endif
#if defined(TCSUPPORT_CT_USB)
	{"isCTUsbSupported"},
#endif
#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
	{"isCTDHCPPortFilterSupported"},
#endif
#if defined(TCSUPPORT_CT_PPPOEPROXY)
	{"isPPPoEProxySupported"},
#endif
#if defined(TCSUPPORT_CUC_TIME_DISPLAY)
	{"isCUCTimeDispSupported"},
#endif
#if defined(TCSUPPORT_CY_E8_SFU)
	{"isCYE8SFUSupported"},
#endif
#if defined(TCSUPPORT_ECN_SIP)	
	{"isMultiDigitMap"},
#endif
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	{"isPPPoEOnDemandSupported"},
#if defined(TCSUPPORT_CT_PPP_ONDEMAND_WEBUI)
	{"isPPPoEOnDemandWEBUISupported"},
#endif
#endif
#if defined(TCSUPPORT_CT_PPP_MANUALLY)
	{"isPPPoEManualWEBUISupported"},
#endif
#if defined(TCSUPPORT_CT_FJ)
	{"isCTFJSupported"},
#endif
#if defined(TCSUPPORT_CT_ECN_GZ)
	{"isCTECNGZSupported"},
#endif
#if defined(TCSUPPORT_CT_2PORTS)
	{"isCT2PORTSSupported"},
#endif
#if defined(TCSUPPORT_CT_1PORT)
	{"isCT1PORTSupported"},
#endif
#if defined(TCSUPPORT_CT_SIMCARD_SEPARATION)
	{"isCTSIMCARDSEPARATIONSupported"},
#endif
#if defined(TCSUPPORT_CT_PON)
	{"isCTPONSupported"},
#endif
#if defined(TCSUPPORT_CT_MONITORCOLLECTOR)
	{"isCTMONITORCOLLECTORSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_C9)
	{"isCTPONC9Supported"},
#endif
#if defined(TCSUPPORT_CT_PON_GD)
	{"isCTGDSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_SC)
	{"isCTSCSupported"},
#endif
#if defined(TCSUPPORT_CT_PROLINE_SUPPORT)
	{"isCTPROLINESupported"},
#endif
#if defined(TCSUPPORT_CT_UPLOAD_ALLINONE)
	{"isAllinoneUploadSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_SN)
	{"isCTPONSNSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_NMG)
	{"isCTPONNMGSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_C7)
	{"isCTPONC7Supported"},
#endif
#if defined(TCSUPPORT_CT_PON_YN)
	{"isCTPONYNSupported"},
#if defined(TCSUPPORT_CT_1PORT)
	{"isCTPONYN1PortSupported"},
#endif
#endif
#if defined(TCSUPPORT_CT_PON_JS)
	{"isCTPONJSSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_GX)
	{"isCTPONGXSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CY)
	{"isCTPONCYSupported"},
#endif
#if defined(TCSUPPORT_CT_SFU_C9)
	{"isCTSFUC9Supported"},
#endif
#if defined(TCSUPPORT_CT_C9)
	{"isCTC9Supported"},
#endif
#if defined(TCSUPPORT_ECN_SIP) && defined(TCSUPPORT_ECN_MEGACO)
	{"isCTCUCSIPH248Supported"},
#endif
#if defined(TCSUPPORT_CT_PHONEAPP)
	{"isCTPHONEAPPSupported"},
#endif
#if (0 == VOIP)
	{"isCTPONNOVOIP"},
#endif
#if defined(TCSUPPORT_CT_PON_BIND2)
	{"isCTPONBIND2Supported"},
#if defined(TCSUPPORT_CT_PON_BIND2_WEB)
	{"isCTPONBIND2WebSupported"},
#endif
#endif
#if defined(TCSUPPORT_CT_PON_JX)
	{"isCTPONJXSupported"},
#endif
#if defined(TCSUPPORT_CT_1FXS)
	{"isCT1FXSSupported"},
#endif
#if defined(TCSUPPORT_VOIP)
	{"isVOIPSupported"},
#endif
#if defined(TCSUPPORT_CT_ADSL_HN)
	{"isCTADSLHNSupported"},
#endif
#if defined(TCSUPPORT_C7_CUC)
	{"isC7CUCSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CN_JS)	
	{"isCTPONCNJSSupported"},
#endif
#if defined(TCSUPPORT_CT_ADSL_BIND1)
	{"isXBIND1Supported"},
#endif
#if defined(TCSUPPORT_CT_ADSL_TJ)
	{"isCTADTJSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_HLJ)
	{"isCTPONHLJSupported"},
#endif
#if defined(TCSUPPORT_CMCC)
	{"isCMCCSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CY_JX)
	{"isCTPONCYJXSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CQ)
	{"isCTPONCYCQSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CAU)
	{"isCTPONCAUSupported"},
#endif
#if defined(TCSUPPORT_CUC_PON_SD)
	{"isCUCPONSDSupported"},
#endif
#if defined(TCSUPPORT_CT_JOYME)
	{"isCTJOYMESupported"},
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	{"isCTJOYME2Supported"},
#endif
#if defined(TCSUPPORT_CT_PON_NX)
	{"isCTPONNXSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_GD)
	{"isCTPONCZGDSupported"},
#endif
#if defined(TCSUPPORT_IPV6)
	{"isIPv6Supported"},
#endif
#if defined(TCSUPPORT_VPN)
	{"isIpsecSupported"},
#endif
#if defined(TCSUPPORT_CT_TY_LOGO)
	{"isCTPONTYLOGOSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_GZ) || defined(TCSUPPORT_CT_PON_CZ_QH)
	{"isCTPONOnlyTwoSSIDSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_GX)
	{"isCTPONCZGXSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_NX)
	{"isCTPONCZNXSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_SN) || defined(TCSUPPORT_CT_PON_CZ_GX) || defined(TCSUPPORT_CT_PON_CZ_GDCS) || defined(TCSUPPORT_CT_PON_CZ_CQ)
	{"isCTPONOnlyOneSSIDSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_GD) || defined(TCSUPPORT_CY_E8_SFU)
	{"isWebTYLOGOSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_GDCS)
	{"isCTPONGDTemperatureSupported"},
#endif
#if defined(TCSUPPORT_CT_PON_CZ_HN)	
	{"isCTPONHNSupported"},
#endif
#if defined(TCSUPPORT_WLAN_MULTI_WPS)
	{"isMultiSupported"},
#endif
#if defined(TCSUPPORT_SNOOPING_SEPERATION)
		{"isSnoopSeperationSupported"},
#endif
#if defined(TCSUPPORT_L2OGRE)
	{"isL2ogreWebSupported"},
#endif
#if defined(TCSUPPORT_SNMP)
	{"isSnmpWebSupported"},
#endif
#if defined(TCSUPPORT_SNMP_FULL)
	{"isSNMPFullSupported"},
#if defined(TCSUPPORT_START_TRAP)
	{"isStartTrap"},
#endif
#if defined(TCSUPPORT_SNMP_V3)
	{"isSNMPV3Supported"},
#endif
#if defined(TCSUPPORT_IPV6_SNMP)
	{"isSNMPIPv6Supported"},
#endif	
#endif
#ifdef TCSUPPORT_SNMP_TRUSTIP
	{"isSnmpTrustIp"},
#endif
#if defined(TCSUPPORT_CWMP)
	{"isCwmpSupported"},
#endif
#if defined(TCSUPPORT_IPV6_CWMP)
	{"isIPv6CwmpSupported"},
#endif
#if defined(TCSUPPORT_CWMP_OPENSSL)
	{"isCwmpOpensslSupported"},
#endif
#if defined(TCSUPPORT_WAN_ETHER)
	{"isWanEtherSupported"},
#endif
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
	{"isActiveEtherWanSupported"},
#endif
#if defined(TCSUPPORT_MULTI_USER_ITF)
	{"isMultiUserITFSupported"},
#endif
#if defined(TCSUPPORT_CT_DBUS)
	{"isCTDBUSSupported"},
#endif
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	{"isQoSEightQueueSupported"},
#endif
#if defined(TCSUPPORT_GENERAL_MULTILANGUAGE)
	{"isMultiLanguageSupport"},
#endif
#if defined(TCSUPPORT_PORT_TRIGGER)
	{"isProtTriggeringSupported"},
#endif
	{"isShowNumOfSsid"},
#if defined(TCSUPPORT_CT_DEVICEREG)
	{"isCTDeviceregSupported"},
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(RA_PARENTALCONTROL)
	{"isParentalSupported"},
#endif
#if defined(TCSUPPORT_ECNT_MAP)
	{"isEasyMeshSupport"},
#endif
#if defined(TCSUPPORT_ECNT_MAP_ENHANCE) || defined(TCSUPPORT_EASYMESH_R13) || defined(TCSUPPORT_MAP_R2)
	{"isNewEasyMeshSupport"},
#endif
#if defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
	{"isInsideAgentSupport"},
#endif
#if defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
	{"isSambaWebSupported"},
#endif
#if defined(TCSUPPORT_NP)
	{"isNPUSupported"},
	{"isHideLan4Supported"},
#endif
#if defined(TCSUPPORT_MULTI_SWITCH_EXT) 
	{"isMultiLanSupported"},
#endif
#if defined(TCSUPPORT_CGNX)
	{"isCGNXSupport"},
#endif
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
	{"isMultiLanPDSupported"},
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	{"isCTJOYME4Supported"},
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	{"isOnlyShowMainSSID"},
#endif
#if defined(TCSUPPORT_STUN)
	{"isCTSTUNSupported"},
#endif
#if defined(TCSUPPORT_CT_UBUS)
	{"isUBUSSupported"},
#endif
#if defined(TCSUPPORT_WLAN_AX)
	{"is11AXModeSupported"},
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
	{"isWPA3ModeSupported"},
#endif
#if defined(TCSUPPORT_NPTv6)
	{"isNPTv6Supported"},
#endif
#if defined(TCSUPPORT_CT_VRWAN)
	{"isCTVRWANSupported"},
#endif
#if defined(TCSUPPORT_EASYMESH_R13)
	{"isEasyMeshV2Supported"},
#endif
#if defined(TCSUPPORT_MAP_R2)
	{"isEasyMeshR2Supported"},
#endif
	/*Add your attributes above*/
	{""}
};

int svc_cfg_boot_webcustom(void)
{
	int i;
	
	for(i=0; strlen(webCustom_attr[i]) > 0; i++)
	{		
		cfg_set_object_attr(WEBCUSTOM_ENTRY_NODE, webCustom_attr[i], "Yes");
	}
	return 0;
}

static int cfg_type_webcustom_entry_func_get(char* path,char* attr, char* val,int len) 
{
	int i;
#if defined(TCSUPPORT_CT_JOYME4)
	char powerlevel[10] = {0};
	char Capabilities[32] = {0};
	char usbcap[8] = {0};
	int j = 0;
#endif
	
	val[0] = 0;
	
	for(i=0; strlen(webCustom_attr[i]) > 0; i++)
	{	
		if (strcmp(attr,webCustom_attr[i]) == 0)
		{
			cfg_set_object_attr(path, webCustom_attr[i], "Yes");
			break;
		}	
	}
#if defined(TCSUPPORT_CT_JOYME4)
	snprintf(powerlevel, sizeof(powerlevel), "%d", ECNT_EVENT_WIFI_POWERLEVEL);
	cfg_set_object_attr(path, "PowerLevel", powerlevel);

	if( !strcmp(attr, "isCTUsbSupported") )
	{
		cfg_get_object_attr(SYS_ENTRY_NODE, "Capability", Capabilities, sizeof(Capabilities));
		if( 0 != Capabilities[0] )
		{
			sscanf(Capabilities, "%*[^;];%*[^;];%[^;];%*[^;];%*[^ ]", usbcap);
			if( strcmp(usbcap, "1") )
				cfg_set_object_attr(path, "isCTUsbSupported", "No");
		}
	}
#endif

	return cfg_type_default_func_get(path,attr,val,len);
}; 

static cfg_node_ops_t cfg_type_webcustom_entry_ops  = { 
	 .get = cfg_type_webcustom_entry_func_get, 
	 .set = cfg_type_default_func_set,
	 .query = cfg_type_default_func_query
}; 


static cfg_node_type_t cfg_type_webcustom_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1, 
	 .parent = &cfg_type_webcustom, 
	 .ops = &cfg_type_webcustom_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_webcustom_ops  = { 
	 .get = cfg_type_default_func_get,
	 .set = cfg_type_default_func_set,
	 .query = cfg_type_default_func_query
}; 


static cfg_node_type_t* cfg_type_webcustom_child[] = { 
	&cfg_type_webcustom_entry,
	NULL 
}; 


cfg_node_type_t cfg_type_webcustom = { 
	 .name = "WebCustom", 
	 .flag = CFG_TYPE_FLAG_MEMORY |  1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_webcustom_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_webcustom_child, 
	 .ops = &cfg_type_webcustom_ops, 
}; 
