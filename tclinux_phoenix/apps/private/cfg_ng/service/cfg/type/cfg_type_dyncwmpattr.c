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
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 

#define	WAN_IF_COM_CUR_INDEX "CurIFIndex"

typedef unsigned char uint8;
typedef unsigned long	uint32;

typedef unsigned char       Unsigned8;
typedef unsigned short      Unsigned16;
typedef unsigned long       Unsigned32;

#define kAscii_0            0x30
#define kAscii_9            0x39
#define kAscii_A            0x41
#define kAscii_Z            0x5A
#define kAscii_a            0x61
#define kAscii_z            0x7A
#define kAscii_Plus         0x2B
#define kAscii_Slash        0x2F
#define kAscii_Equal        0x3D


#define CWMP_MAGIC_ATTR 	0xAABBCCDD 
#define CWMP_MAGIC	 		0xFFEEAABB
#define WLAN_MAGIC_ATTR 	0x22335566
#define IPCON_MAGIC_ATTR	0xAACCDDBB
#define CT_PONLLINK_MAGIC_ATTR    0x11227796


#define MAX_WAN_NODE 		8
#define MAX_WAN_NODEL2	8
#define MAX_PM_RULE		10


typedef struct cwmpAttr
{
	uint8  aURL;
	uint8  aUsername;	
	uint8  aPassword;	
	uint8  aPeriodicInformEnable;
	uint8  aPeriodicInformInterval;	
	uint8  aPeriodicInformTime;	
	uint8  aConnectionRequestURL;
	uint8  aConnectionRequestUsername;
	uint8  aConnectionRequestPassword;
	uint8  aUpgradesManaged;
	uint8  aParameterKey;
	uint8  aKickURL;   
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	uint8  aBindInterface;
	uint8  aRMSFlooding;
	uint8  Reserved_2[2];
#endif
	
	/*  there are 11 bytes used, 
	 ** if you add other attribute,please update the stats.
	  */
	  
	uint8 Reserved[8];
	uint32  magic;  
}cwmpAttr;

typedef struct wlanAttr
{
	uint8  aWLanEnable[8];	
 	uint8  aWLanMaxBitRate[4];	
 	uint8  aWLanChannel[8];
 	uint8  aWLanSSID[8];

 	uint8  aWLanBeaconType[8];
 	uint8  aWLanMACAddressControlEnabled[4];
 	uint8  aWLanWEPKeyIndex[8];	
  	uint8  aWLanKeyPassphrase[4];
	
  	uint8  aWLanBasicEncryptionModes[8];
  	uint8  aWLanBasicAuthenticationMode[8];
  	uint8  aWLanWPAEncryptionModes[8];
  	uint8  aWLanWPAAuthenticationMode[8];
	
  	uint8  aWLanBasicDataTransmitRates[4];
  	uint8  aWLanOperationalDataTransmitRates[4];
  	uint8  aWLanRadioEnabled[4];
  	uint8  aWLanAutoRateFallBackEnabled[4];
		
	uint8  aWEPKey[8];	
	uint8  aWLanPreSharedKey[10];
	uint8  aWLanPreKeyPassphrase[10];
	
	uint8  aWLanStatus[8];
	uint8  aWLanBSSID[8];
	uint8  aWLanStandard[8];
	uint8  aWLanWEPEncryptionLevel[8];

	uint8  aWLanIEEE11iEncryptionModes[4];
	uint8  aWLanIEEE11iAuthenticationMode[4];
	
	uint8  aWLanPossibleChannels[4];
	uint8  aWLanPossibleDataTransmitRates[4];
	uint8  aWLanTotalBytesSent[8];
	uint8  aWLanTotalBytesReceived[8];
	
	uint8  aWLanTotalPacketsSent[8];
	uint8  aWLanTotalPacketsReceived[8];

	uint8  aWLanTotalAssociations[4];

	uint8  aWLanAssociatedDevMACAddr[4];
	uint8  aWLanAssociatedDevIPAddr[4];
	uint8  aWLanAssociatedDevAuthState[4];

	/*wps attribute start*/
	uint8  aWPSEnable;
	uint8  aWPSDeviceName;
	uint8  aWPSDevicePassword;
	uint8  aWPSUUID;
	
	uint8  aWPSVersion;
	uint8  aWPSConfigMethodsSupported;
	uint8  aWPSConfigMethodsEnabled;
	uint8  aWPSSetupLockedState;
	
	uint8  aWPSSetupLock;
	uint8  aWPSConfigurationState;
	uint8  aWPSLastConfigurationError;
	uint8  aWPSRegistrarNumberOfEntries;
	
	uint8  aWPSRegistrarEstablished;
	uint8  aWPSRegistrarEnable[1];
	uint8  aWPSRegistrarUUID[1];
	uint8  aWPSRegistrarDeviceName[1];
#if defined(TCSUPPORT_CMCC)
#if defined(TCSUPPORT_WLAN_AC)
	uint8  aWPSRegistration[8];
#endif
#endif
	/*wps attribute end*/
	
#if defined(TCSUPPORT_CT_WLAN_NODE)
	uint8  aSSIDEnable[8];
	uint8  aWLanSSIDHiden[8];
	uint8  aWPSKeyWord;
	uint8  aChannelInUse;
	uint8  aPowerlevel;
	uint8  aPowerValue;
#endif

#if defined(TCSUPPORT_CT_CWMP_E8BPARAMETER)
	uint8 aRFBand[8];
	uint8 aChannelWidth[8];
	uint8 aGuardInterval[8];
	uint8 aRetryTimeout[8];
	uint8  aX_CTCOM_Mode[4];
	uint8  aX_CTCOM_VLAN[4];
	uint8 aDetectionStatus[4];
#endif

#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	uint8 maxUserNum[4];
#endif
#if defined(TCSUPPORT_CT_PMINFORM)
	uint8 aAccessEquipmentMac[4];
#endif
#if defined(TCSUPPORT_CMCC)
	uint8 aWLanIfName[8];
#endif
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	uint8 aWLanCfgName[4];
	uint8 aWlanCfgUAPSDEnable[4];	
	uint8 aWlanCfgDualBandSupport;
	uint8 aWlanCfgPowerSupported;
	uint8 Reserved2;
	uint8 aWlanCfgUAPSDSupported;
#endif
	uint8 Reserved3[3];
	uint8 aWlanCfgWMMSupported;
	uint8 aWlanCfgWMMEnable[8];
#if defined(TCSUPPORT_CT_FJ)
	uint8 aWlanCfgErrorsSent[4];
	uint8 aWlanCfgErrorsReceived[4];
	uint8 aWlanCfgDiscardPacketsSent[4];
#endif
	uint8 aBeaconAdvertisementEnabled[4];
#if defined(TCSUPPORT_CMCC)
	/*InternetGatewayDevice.LanDevice.x.X_CMCC_SwitchTimerControl.{i}.*/
	uint8 aWlanSwitchTimerControlActive[8];
	uint8 aWlanSwitchTimerControlEnable[8];
	uint8 aWlanSwitchTimerControlTime[8];
	uint8 aWlanSwitchTimerControlWeekday[8];

	/*InternetGatewayDevice.LanDevice.x.X_CMCC_WLANForGuest.{i}.*/
	uint8 aWlanForGuestSSIDIndex[2];
	uint8 aWlanForGuestDuration[2];
	
	uint8 aWlanForGuestPortIsolation[2];
	/*InternetGatewayDevice.LanDevice.x.X_CMCC_WLANShare.{i}.*/
	uint8 aWlanshareSSIDIndex;
	uint8 aWlansharePortIsolation;
	
	uint8 aWlanshareSTAIsolation;
	uint8 aWlanshareEnableUserId;
	uint8 aWlanshareUserId;
        uint8 Reserved[9];
#else
	uint8 Reserved[52];
#endif
	
	uint32 magic;
}wlanAttr;

typedef struct ipConAttr
{
	uint8  aWANIPCnt1_1Enable[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1Type[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1Name[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1DefGateway[MAX_WAN_NODE*MAX_WAN_NODEL2];

	uint8  aWANIPCnt1_1SubMask[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1ExtIPAddr[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1AddrType[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1CntTrigger[MAX_WAN_NODE*MAX_WAN_NODEL2];

	uint8  aWANIPCnt1_1RProRx[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1NATEnable[MAX_WAN_NODE*MAX_WAN_NODEL2];

	uint8  aWANIPCnt1_1CntStatus[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1PsbCntTypes[MAX_WAN_NODE*MAX_WAN_NODEL2];
	
	uint8  aWANIPCnt1_1Uptime[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1LastCntError[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1RSIPAvailable[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1DNSEnabled[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	
	uint8  aWANIPCnt1_1DNSOverAllow[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aWANIPCnt1_1DNSServers[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1MACAddress[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1TransportType[MAX_WAN_NODE*MAX_WAN_NODEL2];
	
	uint8  aWANIPConnectionNumOfEntries[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANPPPConnectionNumOfEntries[MAX_WAN_NODE*MAX_WAN_NODEL2];
	
	uint8  aWANIPCnt1_1EthBytesSent[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1EthBytesRcv[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1EthPktsSent[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1EthPktRcv[MAX_WAN_NODE*MAX_WAN_NODEL2];
#if defined(TCSUPPORT_CT_FJ)
	uint8  aWANIPCnt1_1EthErrRcv[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANIPCnt1_1EthDisPktRcv[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
	uint8  aPortMappingEntryNum[MAX_WAN_NODE*MAX_WAN_NODEL2];  

	uint8  aIPPMapping1Enable[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aIPPMapping1Descrip[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aIPPMapping1LeaseDur[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aIP1RemoteHost[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];

	uint8  aIP1ExternalPort[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aIP1InternalPort[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aIPPMapping1Protocol[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aIP1InternalClient[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE];
	uint8  aExternalPortEndRange[MAX_WAN_NODE*MAX_WAN_NODEL2][MAX_PM_RULE]; 

	uint8  aWANPPPCnt1_1PPPoESvcName[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANPPPCnt1_1Username[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANPPPCnt1_1Password[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  AuthenType[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANPPPCnt1_1PPPoEACName[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComServiceList[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDHCPEnable[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComInterfaceBond[MAX_WAN_NODE*MAX_WAN_NODEL2];
#if defined(TCSUPPORT_CT_E8DDNS)
	uint8  aWANCnt1_CTComDDNSCfgEnable[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDDNSProvider[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDDNSUsername[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDDNSPassword[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComServicePort[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDDNSDomainName[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDDNSHostName[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
	/*	there are 320 bytes used.
	 **	if add other attributes,please update the stats.
	  */
	  
#if defined(TCSUPPORT_CT_PPPOEPROXY)
	uint8  aWANIPCnt1_1proxyenable[MAX_WAN_NODE];
	uint8  aWANIPCnt1_1maxuser[MAX_WAN_NODE];
#endif

#if defined(TCSUPPORT_CT_CWMP_E8BPARAMETER)
	uint8  aWANCnt1_CTComIPMode[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTIPv6ConStatus[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTMulticastVlan[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aWANCnt1_CTComDhcpOption60Enable[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption60Type[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption60ValueMode[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption60Value[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption125Enable[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption125Value[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption125SubCode[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpOption125SubData[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpv6Option17Enable[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpv6Option17Type[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpv6Option17SubCode[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpv6Option17Value[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpv6Option16Enable[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComDhcpv6Option16Value[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_CTComIPv6Alias[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6IPAddress[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6DNSIPAddress[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6Origin[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6ChildPrefixBits[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6PrefixAlias[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6Prefix[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6PrefixOrigin[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComIPv6PrefixChildPrefixBits[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComDsliteEnable[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComAftrMode[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_CTComAftr[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aX_CTCOMWANIndex;
	uint8  Reserved1[3];	
	uint8  aWANCnt1_IPForwardList[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aWANCnt1_DhcpOption125Type[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_Dhcpv6Option16Type[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_Dhcpv6Option16ValueMode[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_Dhcpv6Option17SubData[MAX_WAN_NODE*MAX_WAN_NODEL2][4];
	uint8  aWANCnt1_IPv6IPAddress[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6IPAddressAlias[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPAddressOrigin[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6DNSServers[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aWANCnt1_IPv6PrefixDelegation[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6PrefixAlias[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6PrefixOrigin[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6Prefix[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aWANCnt1_IPv6PrefixPltime[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6PrefixVltime[MAX_WAN_NODE*MAX_WAN_NODEL2];	
	uint8  aWANCnt1_IPv6DefaultGateway[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IPv6DomainName[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	uint8  aWANCnt1_Reset[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_VlanEnable[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_VlanID[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_Priority[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANCnt1_IgmpProxy[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	uint8  aWANCnt1_IdleDisconnectTime[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
#if defined(TCSUPPORT_CT_CWMP_E8BPARAMETER) && !defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	uint8  aWANCnt1_PPPBIEnable[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
	uint8  aWANPPPCnt1_1LastCntErrorCode[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif
#if defined(TCSUPPORT_CMCC)
	uint8  aWANPPPCnt1_VLANMode[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANPPPCnt1_VLANIDMark[MAX_WAN_NODE*MAX_WAN_NODEL2];
	uint8  aWANPPPCnt1_8021PMark[MAX_WAN_NODE*MAX_WAN_NODEL2];
#endif

	uint8 Reserved[64];
		
	uint32 magic;
}ipConAttr;

typedef struct CT_PonLinkAttr
{
	uint8 aWANPonLinkCfg1EPonEnable[MAX_WAN_NODE];
	uint8 aWANPonLinkCfg1GPonEnable[MAX_WAN_NODE];
	uint8 aWANPonLinkCfg1Mode[MAX_WAN_NODE];
	uint8 aWANPonLinkCfg1VLANIDMark[MAX_WAN_NODE];
	uint8 aWANPonLinkCfg18021PMark[MAX_WAN_NODE];

	uint8  aWANVLANEnable[MAX_WAN_NODE];
	uint8  aWANVLAN[MAX_WAN_NODE];
	uint8  aWANVLAN8021p[MAX_WAN_NODE];

	uint32  magic;  
}CT_PonLinkAttr;

static Unsigned8 ConvertBase64Character(Unsigned8 theInputCharacter) 
{
    	Unsigned8   theResult = 0;

#ifdef TCSUPPORT_CT_GUIACCESSLIMIT     
	if (theInputCharacter >= kAscii_A && theInputCharacter <= kAscii_Z )
	{
	    theResult = theInputCharacter - kAscii_A;
	}
	else if (theInputCharacter >= kAscii_a && theInputCharacter <= kAscii_z)
	{
	    theResult = theInputCharacter - kAscii_a + 26;
	}
	else if (theInputCharacter >= kAscii_0 && theInputCharacter <= kAscii_9) 
	{
	    theResult = theInputCharacter - kAscii_0 + 52;
	}
	else if (theInputCharacter == kAscii_Plus) 
	{
	    theResult = 62;
	}
	else if (theInputCharacter == kAscii_Slash) 
	{
	    theResult = 63;
	}
#endif

    	return theResult;
}

Unsigned16 RpDecodeBase64DataNoNull(char *theInputPtr, Unsigned16 theInputCount,char *theOutputPtr)
{
	Unsigned32  thePackedGroup;
	char *      theOriginalOutputPtr;

#ifdef TCSUPPORT_CT_GUIACCESSLIMIT   
	theOriginalOutputPtr = theOutputPtr;
	theInputCount -= (Unsigned16) (theInputCount % 4);
	while (theInputCount > 0)
	{
		thePackedGroup = (Unsigned32) ConvertBase64Character(*theInputPtr++) << 18;
		thePackedGroup |=(Unsigned32) ConvertBase64Character(*theInputPtr++) << 12;
		*theOutputPtr++ = (Unsigned8) (thePackedGroup >> 16);
		if (*theInputPtr != kAscii_Equal)
		{
			thePackedGroup |=(Unsigned32) ConvertBase64Character(*theInputPtr++) << 6;
			*theOutputPtr++ = (Unsigned8) (thePackedGroup >> 8);
			if (*theInputPtr != kAscii_Equal)
			{
				thePackedGroup |=(Unsigned32) ConvertBase64Character(*theInputPtr++);
				*theOutputPtr++ = (Unsigned8) (thePackedGroup);
			}
		}
		theInputCount -= 4;
	}
	return theOutputPtr - theOriginalOutputPtr;
#endif

	return 0;
}


static int cfg_type_dyncwmpattr_read(char* path,char* attr, char* val,int len) 
{
	char curAttrValue[1024]={0}, setValue[4]={0};
	int i=0, idx=0, count=0;
	char tmp[32]={0};
	char nodePath[64] = {0};
	char tmpPath[64] = {0};

	if (path== NULL || attr == NULL|| val == NULL || len <= 0)
	{
		return -1;
	}
	snprintf(nodePath, sizeof(nodePath), DYNCWMPATTR_ENTRY_NODE);
	snprintf(tmpPath, sizeof(tmpPath), WANINFO_COMMON_NODE);
	memset(curAttrValue,0,sizeof(curAttrValue));
	if(cfg_obj_get_object_attr(tmpPath, "NoGUIAccessLimit", 0, curAttrValue, sizeof(curAttrValue)) > 0)
	{
		if(strcmp(curAttrValue, "1") == 0)	/*close the GUIAccessLimit Function */
		{
			snprintf(setValue,sizeof(setValue), "%d", 0);
			/*cwmp */
			cfg_set_object_attr(nodePath,"aURL", setValue );
			cfg_set_object_attr(nodePath,"aUsername", setValue );
			cfg_set_object_attr(nodePath,"aPassword", setValue );
			cfg_set_object_attr(nodePath,"aConnectionRequestUsername", setValue );
			cfg_set_object_attr(nodePath,"aConnectionRequestPassword", setValue );
#if defined(TCSUPPORT_CT_PON_GD) || defined(TCSUPPORT_CT_PON_CZ_GD)
			cfg_set_object_attr(nodePath,"aPeriodicInformInterval", setValue );
#endif
			snprintf(setValue, sizeof(setValue), "%d", 1);

#ifdef TCSUPPORT_WLAN
			/*wlan */
			cfg_set_object_attr(nodePath,"aSSIDEnable", setValue );
			cfg_set_object_attr(nodePath,"aWLanBeaconType", setValue );
			cfg_set_object_attr(nodePath,"aWLanSSID", setValue );
#endif
			/*wan */
			cfg_set_object_attr(nodePath,"aPPPUsername", setValue );
			cfg_set_object_attr(nodePath,"aPPPPassword", setValue );
			/*vlanid*/
			cfg_set_object_attr(nodePath,"aVLANIDMark", setValue );
			return SUCCESS;
		}
	}
	
	/*cwmp */
	memset(tmpPath, 0, sizeof(tmpPath));
	snprintf(tmpPath, sizeof(tmpPath),TR069ATTR_ENTRY_NODE);
	memset(curAttrValue,0,sizeof(curAttrValue));
	cfg_obj_get_object_attr(tmpPath, "cwmpAttr", 0, curAttrValue, sizeof(curAttrValue));
	cwmpAttr *cwmpflag = NULL;
	cwmpflag = (cwmpAttr*)malloc(sizeof(curAttrValue));
	if(cwmpflag == NULL)
	{
		return FAIL;
	}
	RpDecodeBase64DataNoNull(curAttrValue, strlen(curAttrValue), (char*)cwmpflag);
	if(cwmpflag->magic != CWMP_MAGIC_ATTR)
	{
		free(cwmpflag);
		cwmpflag = NULL;
		return FAIL;
	}

#if defined(TCSUPPORT_CT_PON_GD) || defined(TCSUPPORT_CT_PON_CZ_GD)
	if(cwmpflag->aPeriodicInformInterval&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	cfg_set_object_attr(nodePath,"aPeriodicInformInterval", setValue );
#endif

	if(cwmpflag->aURL&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aURL", setValue );

	if(cwmpflag->aUsername&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aUsername", setValue );

	if(cwmpflag->aPassword&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);

	cfg_set_object_attr(nodePath,"aPassword", setValue );

	if(cwmpflag->aConnectionRequestUsername&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aConnectionRequestUsername", setValue );

	if(cwmpflag->aConnectionRequestPassword&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);

	cfg_set_object_attr(nodePath,"aConnectionRequestPassword", setValue );
	free(cwmpflag);
	cwmpflag = NULL;

#ifdef TCSUPPORT_WLAN
	/*wlan */
	memset(curAttrValue,0,sizeof(curAttrValue));
	cfg_obj_get_object_attr(tmpPath, "wlanAttr", 0, curAttrValue, sizeof(curAttrValue));
	wlanAttr *wlanflag = NULL;
	wlanflag = (wlanAttr*)malloc(sizeof(curAttrValue));
	if(wlanflag == NULL)
	{
		return FAIL;
	}
	RpDecodeBase64DataNoNull(curAttrValue, strlen(curAttrValue), (char*)wlanflag);
	if(wlanflag->magic != WLAN_MAGIC_ATTR)
	{
		free(wlanflag);
		wlanflag = NULL;
		return FAIL;
	}
	
	memset(tmpPath, 0, sizeof(tmpPath));
	snprintf(tmpPath, sizeof(tmpPath), WEBCURSET_ENTRY_NODE);

	if(cfg_obj_get_object_attr(tmpPath, "wlan_id", 0, tmp, sizeof(tmp)) > 0)
	{
		idx = atoi(tmp);
	}
	else
	{
		free(wlanflag);
		wlanflag = NULL;
		return FAIL;
	}
	
	#if defined(TCSUPPORT_CT_WLAN_NODE)
	if(wlanflag->aSSIDEnable[idx]&0x01) /* from WLanEnable to SSIDEnable */
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aSSIDEnable", setValue ); /* from WLanEnable to SSIDEnable */
	#endif

	if(wlanflag->aWLanBeaconType[idx]&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aWLanBeaconType", setValue );

	if(wlanflag->aWLanSSID[idx]&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aWLanSSID", setValue );
	free(wlanflag);
	wlanflag = NULL;
#endif

	/*wan */
	memset(tmpPath, 0, sizeof(tmpPath));
	snprintf(tmpPath, sizeof(tmpPath), WANINFO_COMMON_NODE);
	if(cfg_obj_get_object_attr(tmpPath, WAN_IF_COM_CUR_INDEX, 0, tmp, sizeof(tmp)) > 0)	
	{
		idx = atoi(tmp);
	}
	else
	{
		return FAIL;
	}	
	
	memset(tmpPath, 0, sizeof(tmpPath));
	snprintf(tmpPath, sizeof(tmpPath), TR069ATTR_ENTRY_NODE);
	if(cfg_obj_get_object_attr(tmpPath, "ipConAttrLen", 0, tmp, sizeof(tmp)) > 0)		
	{
		count = atoi(tmp);
	}
	else
	{
		return FAIL;
	}	
	int attrLen = sizeof(ipConAttr);
	int lenth = attrLen + (attrLen >> 1); /*len equals attrLen*1.5 */
	char *buf = NULL;
	buf = (char*)malloc(lenth);
	if(buf == NULL)
	{
		return FAIL;
	}
	memset(buf, 0, lenth);
	for(i = 0; i < count; i++)
	{
		snprintf(tmp, sizeof(tmp), "ipConAttr%d", i);
		memset(curAttrValue,0,sizeof(curAttrValue));
		cfg_obj_get_object_attr(tmpPath, tmp, 0, curAttrValue, sizeof(curAttrValue));
		strcat(buf, curAttrValue);
	}
	ipConAttr *ipConflag = NULL;
	ipConflag = (ipConAttr*)malloc(lenth);
	if(ipConflag == NULL)
	{
		free(buf);
		buf = NULL;
		return FAIL;
	}
	RpDecodeBase64DataNoNull(buf, strlen(buf), (char*)ipConflag);
	free(buf);
	buf = NULL; 
	
	if(ipConflag->magic != IPCON_MAGIC_ATTR)
	{
		free(ipConflag);
		ipConflag = NULL;
		return FAIL;
	}
	
	if(ipConflag->aWANPPPCnt1_1Username[idx]&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aPPPUsername", setValue );

	if(ipConflag->aWANPPPCnt1_1Password[idx]&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aPPPPassword", setValue );
	free(ipConflag);
	ipConflag = NULL;

	idx = idx / MAX_SMUX_NUM;		

#if defined(TCSUPPORT_CT_PON)
	/*vlanid */
	memset(curAttrValue,0,sizeof(curAttrValue));
	cfg_obj_get_object_attr(tmpPath, "CTPonLinkAttr", 0, curAttrValue, sizeof(curAttrValue));
	CT_PonLinkAttr *CTPonLinkflag = NULL;
	CTPonLinkflag = (CT_PonLinkAttr*)malloc(sizeof(curAttrValue));
	if(CTPonLinkflag == NULL)
	{
		return FAIL;
	}
	RpDecodeBase64DataNoNull(curAttrValue, strlen(curAttrValue), (char*)CTPonLinkflag);
	if(CTPonLinkflag->magic != CT_PONLLINK_MAGIC_ATTR)
	{
		free(CTPonLinkflag);
		CTPonLinkflag = NULL;
		return FAIL;
	}	

	if(CTPonLinkflag->aWANPonLinkCfg1VLANIDMark[idx]&0x01)
		snprintf(setValue, sizeof(setValue), "%d", 0);
	else
		snprintf(setValue, sizeof(setValue), "%d", 1);
	
	cfg_set_object_attr(nodePath,"aVLANIDMark", setValue );
	free(CTPonLinkflag);
	CTPonLinkflag = NULL;
#endif
	
	return SUCCESS;
}


static int cfg_type_dyncwmpattr_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_dyncwmpattr_read(path, attr, val, len);
	return cfg_type_default_func_get(path,attr,val,len);
} 


static cfg_node_ops_t cfg_type_dyncwmpattr_entry_ops  = { 
	 .get = cfg_type_dyncwmpattr_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_dyncwmpattr_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_dyncwmpattr, 
	 .ops = &cfg_type_dyncwmpattr_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_dyncwmpattr_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_dyncwmpattr_entry_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_dyncwmpattr_child[] = { 
	 &cfg_type_dyncwmpattr_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_dyncwmpattr = { 
	 .name = "dynCwmpAttr", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_dyncwmpattr_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_dyncwmpattr_child, 
	 .ops = &cfg_type_dyncwmpattr_ops, 
}; 
