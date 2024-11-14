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

#ifndef __CFG_TR181_GLOBAL_H__ 
#define __CFG_TR181_GLOBAL_H__ 

#include <netdb.h>
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif

#define WAN_INTERFACE_NUM 					(MAX_WAN_IF_INDEX)
#define IP_INTERFACE_NUM					(WAN_INTERFACE_NUM+1)
#define PPP_INTERFACE_NUM 					(WAN_INTERFACE_NUM)
#define ETHER_VLAN_T_NUM 					(WAN_INTERFACE_NUM)
#define TR181_IP_INTERFACE_IPV4ADDR_NUM		(IP_INTERFACE_NUM)
#define MAX_IPV4ADDRESS_ENTRY_NUM          	1
#define TR181_IP_INTERFACE_IPV6ADDR_NUM		(IP_INTERFACE_NUM)
#define MAX_IPV6ADDRESS_ENTRY_NUM          	1
#define TR181_IP_INTERFACE_IPV6PREF_NUM		(IP_INTERFACE_NUM)
#define MAX_IPV6PREFIX_ENTRY_NUM          	2
#define NAT_INTERFACE_NUM					(WAN_INTERFACE_NUM)
#define ETHER_LINK_NUM 						(IP_INTERFACE_NUM)
#define TR181_ROUTE_IPV4_FORWARDING_NUM		(CFG_TYPE_MAX_ROUTE_ENTRY_NUM + WAN_INTERFACE_NUM)
#define DNS_CLIENT_NUM						(WAN_INTERFACE_NUM*2)

#if defined(TCSUPPORT_MULTI_USER_ITF)
#define LAN_INDEX_WLAN_START		9
#define LAN_INDEX_WLAN_5G_START				17
#define WIFI_NUM_MAX 16
#else
#define LAN_INDEX_WLAN_START		5
#define LAN_INDEX_WLAN_5G_START				9
#define MAX_BSSID_NUM 4
#define WIFI_NUM_MAX 8
#endif

#if defined(TCSUPPORT_MULTI_USER_ITF)
#define LAN_PORT_NUM						8
#define LANIF_NUM_MAX						8
#else
#define LANIF_NUM_MAX 						4
#define LAN_PORT_NUM						4
#endif
#define BRIDGE_PORT_NUM 					(1+LAN_PORT_NUM+WIFI_NUM_MAX)
#define INTERFACE_STACK_NUM					100
#define DHCPV4_CLIENT_NUM_MAX             	WAN_INTERFACE_NUM
#define DHCPV6_CLIENT_NUM_MAX             	WAN_INTERFACE_NUM

#if defined(TCSUPPORT_CT_JOYME2)
#define TR181_NATPORTMAPPING_NUM			MAX_VIRSERV_RULE
#else
#define TR181_NATPORTMAPPING_NUM			MAX_WAN_IF_INDEX * MAX_VIRSERV_RULE
#endif

#define MAX_ROUTE_NUM 						16
#define TR181_ROUTE_IPV6_FORWARDING_NUM		(CFG_TYPE_MAX_ROUTE6_ENTRY_NUM + WAN_INTERFACE_NUM)
#define TR181_ROUTE_ROUTE_NUM				16
#define TR181_FIREWALL_CHAIN_PVC_NUM		4
#define TR181_FIREWALL_CHAIN_ENTRY_NUM	MAX_MAC_FILTER_RULE
#define TR181_FIREWALL_LEVEL_NUM			10
#define IS_WLAN11AC_MODE                    5
#define IS_WLAN_MODE                        2
#define TR181_DSLITE_NUM                    WAN_INTERFACE_NUM
#define TR181_OPTICAL_MAXENRTRYNUM			1
#if defined(TCSUPPORT_CT_DSL_EX)
#define ATM_LINK_NUM						8
#define PTM_LINK_NUM						1
#define DSL_CHANNEL_NUM						1
#define DSL_LINE_NUM						1
#define TR181_ATMLINK_INTERFACE				"Device.ATM.Link."
#define TR181_PTMLINK_INTERFACE				"Device.PTM.Link.1"
#define TR181_DSL_CHANNEL_INTERFACE			"Device.DSL.Channel.1"
#define TR181_DSL_LINE_INTERFACE			"Device.DSL.LINE.1"
extern cfg_node_type_t cfg_type_tr181_atmlink;
extern cfg_node_type_t cfg_type_tr181_ptmlink;
extern cfg_node_type_t cfg_type_tr181dslChannel;
extern cfg_node_type_t cfg_type_tr181dslline;
#endif

#define is_digit(c)     ((unsigned)((c) - '0') <= 9)
/*for tr069 reinit tree*/
#define TR181_EVENT 24
#define REINIT_PROCESS_TREE 		1
#define REINIT_TR181_TREE 			2
#define REINIT_TR181_IPAP_TREE 		3

#define TR181_WAN_MASK0_ATTR 				"wanMask0"
#define TR181_WAN_MASK1_ATTR 				"wanMask1"

#define TR181_UPLAYER_ATTR					"uplayer"
#define TR181_LOWERLAYER_ATTR				"LowerLayers"
#define TR181_INTERFACE_ATTR				"Interface"
#define PVC_INDEX_ATTR 			            "pvcIndex"
#define LAN_INDEX_ATTR 						"lanIndex"
#define TR181_ENABLE_ATTR 						"Enable"
#define TR181_NUM_ATTR 							"Num"
#define TR181_IPVERSION_ATTR 				"IPVERSION"
#define TR181_IPADDRESS_ATTR 				"IPAddress"
#define TR181_SUBNETMASK_ATTR 				"SubnetMask"
#define TR181_ACTIVE_ATTR 					"Active"
#define TR181_LOCALIP_ATTR 					"LOCALIP"
#define TR181_VIRSERVER_ENTRY_INDEX_ATTR 	"VirServerIdx"			/*-1:not wan interface;others:wan pvc inex*/
#define TR181_VSENTRYIDX_ATTR 				"VSEntryIdx"
#define TR181_LEASEDUR_ATTR 				"LEASEDUR"
#define TR181_REMOTEIP_ATTR 				"REMOTEIP"
#define TR181_STARTPORT_ATTR 				"STARTPORT"
#define TR181_ENDPORT_ATTR 					"ENDPORT"
#define TR181_INTPORT_ATTR 					"INTPORT"
#define TR181_PROTOCOL_ATTR 				"PROTOCOL"
#define TR181_DESCRIP_ATTR 					"DESCRIP"
#define TR181_PMENTRYIDX_ATTR 				"PMEntryIdx"
#define TR181_ISP_ATTR 						"ISP"
#define TR181_STATUS_ATTR 					"Status"
#define TR181_DNSIP_ATTR 					"DNSIP"
#define TR181_DNSIPV61ST_ATTR 				"DNSIPv61st"
#define TR181_TYPE_ATTR 					"Type"
#define TR181_SYNCFLAG_ATTR					"syncFlag"



#define TR181_IP_INTERFACE					"Device.IP.Interface."
#define TR181_IP_INTERFACE_ENTRY				"Device.IP.Interface.%d"
#define TR181_PPP_INTERFACE					"Device.PPP.Interface."
#define TR181_ETHERNET_VLANTERMINATION		"Device.Ethernet.VLANTermination."
#define TR181_ETHERNET_LINK					"Device.Ethernet.Link."
#define TR181_OPTICAL_INTERFACE				"Device.Optical.Interface.1"
#define TR181_BRIDGE_PORT_BR0				"Device.Bridging.Bridge.1.Port.1"
#define TR181_BRIDGE_PORT					"Device.Bridging.Bridge.1.Port."
#define TR181_WIFI_SSID_IF			    	"Device.WiFi.SSID."
#define TR181_ETHER_INTERFACE				"Device.Ethernet.Interface."
#define TR181_WIFI_RDAIO					"Device.WiFi.Radio."

typedef struct tr181_interface_s	
{
	char	nodeName[64];					/* Node name */
	int		num;							/* entry number */
	int		pvcIndex;
	int		entIndex;
	int		lanIndex;						/*-1:not lan interface;0:br0;1~4:lan1~lan4;5~8:ra0~ra3;9~12:rai0~rai3*/
	int 	active;
}tr181_interface_t;

#define USERS_NUM_MAX          		3
#define TR181_DEVINF_PS_NUM			128
#define TR181_DEVINF_TEM_NUM		10
#define TR181_IPACTIVEPORT_NUM				32
#define FORMAT_NETSTAT_CMD			"%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%[^ ]%*[ ]%[^ ]%*[ ]"

#define ROUTE_TYPE_STATIC 					0
#define ROUTE_TYPE_WAN_SYNC 				1

#define IPMACFILTER_FLAG 				1
#define ACLFILTER_FLAG 				2

#define MAC_FILTER_RULE_FLAG 				1
#define IP_UP_FILTER_RULE_FLAG 				2
#define IP_DOWN_FILTER_RULE_FLAG 			3
#define ACL_FILTER_RULE_FLAG 				4

#define DEVINF_LOC_MAX_NUM                  4
#define ETHER_NODE_NUM			WAN_INTERFACE_NUM
#define BRIDGINGE_NODE_NUM				8
#define BRIDGINGE_VLAN_NUM 				8
#define BRIDGINGE_VLAN_PORT_NUM 		8
#define BRIDGINGFILTER_NUM		        8
#define ETHER_INTERFACE_NUM 			4
#define DHCPV4_REQOPT_NUM_PER_CLENT       8
#define DHCPV4_CLIENT_REQ_OPT_NUM_MAX     DHCPV4_CLIENT_NUM_MAX * DHCPV4_REQOPT_NUM_PER_CLENT
#define DHCPV4_SENTOPT_NUM_PER_CLENT      8
#define DHCPV4_SVRPOL_NUM_MAX             8
#define DHCPV4_RELAY_NUM_MAX          		1	
#define DHCPV4_CLT_NUM_PER_POOL    	        8
#define DHCPV4_CLTI_NUM_PER_CLT    	        1
#define DHCPV4_STA_NUM_PER_POOL    	        8
#define DHCPV4_OPT_NUM_PER_POOL    	        1
#define MAX_BUF_SIZE						2048
#define DHCPV4_CLTO_NUM_PER_CLT    	      1
#define DHCPV6_SENTOPT_NUM_PER_CLENT      1
#define DHCPV6_SVRPOL_NUM_MAX             1
#define DHCPV6_OPT_NUM_PER_POOL    	      4
#define WIFI_APACC_NUM_MAX                4
#define WIFI_APASS_NUM_MAX                4
#define WIFI_EP_NUM_MAX                   8
#define PRO_NUM_PER_EP                    8
#define WIFI_EPPRO_NUM_MAX                WIFI_EP_NUM_MAX * PRO_NUM_PER_EP
#define WIFI_EPPROSEC_NUM_MAX             4
#define WIFI_EPSEC_NUM_MAX                4
#define WIFI_EPSTA_NUM_MAX                4
#define WIFI_EPWPS_NUM_MAX                4
#define WIFI_RDOSTA_NUM_MAX               16
#define WIFI_SSIDSTA_NUM_MAX              16
#define WIFI_APWPS_NUM_MAX                WIFI_NUM_MAX
#define	QOS_CLSFICN_NUM_MAX         MAX_TYPE_RULE_NUM
#define QOS_FLOW_NUM_MAX                  8
#define QOS_QUE_NUM_MAX             6
#define QOS_QUESTA_NUM_MAX                QOS_QUE_NUM_MAX
#define QOS_SHAPER_NUM_MAX          8
#define TR181_HOST_NUM					  10
#define MAX_DNS_FORWARDING_NUM			10
#define RTADV_ITFSET_NUM_MAX        	 	IP_INTERFACE_NUM
#define OPT_NUM_PER_SET              4
#define RTADV_ITFSET_OPT_NUM_MAX     RTADV_ITFSET_NUM_MAX * OPT_NUM_PER_SET
#define PERIDSTS_SAMP_PARA_ENTRY_NUM_MAX           16
#define ETHER_RMONSTATS_NUM 				IP_INTERFACE_NUM
#define MGRSER_DLD_GROUP_NUM 	10
#define DHCPV4_SPS_NUM_MAX                	DHCPV4_SVRPOL_NUM_MAX * DHCPV4_STA_NUM_PER_POOL
#define DHCPV4_SPO_NUM_MAX                	DHCPV4_SVRPOL_NUM_MAX * DHCPV4_OPT_NUM_PER_POOL
#define DHCPV4_SPC_NUM_MAX               	DHCPV4_SVRPOL_NUM_MAX * DHCPV4_CLT_NUM_PER_POOL
#define DHCPV6_CLIENT_SENT_OPT_NUM_MAX      DHCPV6_CLIENT_NUM_MAX * DHCPV6_SENTOPT_NUM_PER_CLENT
#define DHCPV4_CLIENT_SENT_OPT_NUM_MAX      DHCPV4_CLIENT_NUM_MAX * DHCPV4_SENTOPT_NUM_PER_CLENT
#define DHCPV6_SPO_NUM_MAX                  DHCPV6_SVRPOL_NUM_MAX * DHCPV6_OPT_NUM_PER_POOL
#define DHCPV4_SPCI_NUM_MAX                 DHCPV4_SVRPOL_NUM_MAX * DHCPV4_CLT_NUM_PER_POOL * DHCPV4_CLTI_NUM_PER_CLT
#define DHCPV4_SPCO_NUM_MAX                 DHCPV4_SVRPOL_NUM_MAX * DHCPV4_CLT_NUM_PER_POOL * DHCPV4_CLTO_NUM_PER_CLT

#define OPTICAL_CONNECTION					3
#define TR181_ROUTE_RIP_ENTRY_NUM			8

#define ATTR_TYPE	"Type%d"
#define	ATTR_MAX	"Max%d" 
#define	ATTR_MIN	"Min%d"

#define NS_INT16SZ	 						2
#define NS_INADDRSZ	 						4
#define NS_IN6ADDRSZ						16

#define INTERFACE_NODE_UPDATE_DBG			0x1

#define IPINTERFACE_EXECUTE_FLAG   			0
#define IPV4ADDR_EXECUTE_FLAG   			1
#define IPV6ADDR_EXECUTE_FLAG   			2
#define ENABLE_ATTR 			"Enable"
#define ISP_ATTR 				"ISP"
#define LAN_INDEX_WAN_START			-1
#define LAN_INDEX_BR0				0
#define LAN_INDEX_LAN_START			1

#define MAX_NODE_NAME 32
#define TR181_WIFIRDO_BND_5G "5GHz"
#define TR181_LOWERLAYER_ATTR	"LowerLayers"
#define TR181_WIFIRDO_BND_2_4G "2.4GHz"
#define WIFI_RDO_NUM_MAX                  2
#define WIFI_AP_NUM_MAX                   WIFI_NUM_MAX
#define WIFI_SSID_NUM_MAX                   WIFI_NUM_MAX
#define NODE_PATH_MAX                64

#define TR181_IPV4ADDR_ENTRY_INDEX   		1

typedef struct lowerlayer_stack_s	
{
	int		ipIFIdx;					
	int		pppIFIdx;
	int		vlanTIdx;
	int 	EtherLinkIdx;	
	int 	bridgePortIdx;
	int 	atmLinkIdx;
	int 	ptmLinkIdx;
	int 	dslChannelIdx;	
	int 	dslLineIdx;	
	int 	opticalIFIdx;
}lowerlayer_stack_t;

typedef struct tr181_lowerlayer_del_s	
{
	char	nodeName[32];			/* Node name */
	char	lowerlayer[256];
	int		start_entry;					
	int		entry_num;				/* entry number */
	int		attr_num;				/* the num of lowerlayer attr */
	int 	isMoreParant;

}tr181_lowerlayer_del_t;

extern cfg_node_type_t cfg_type_tr181; 
extern cfg_node_type_t cfg_type_tr181ip; 
extern cfg_node_type_t cfg_type_tr181ppp; 
extern cfg_node_type_t cfg_type_ethervlant; 
extern cfg_node_type_t cfg_type_etherlink; 
extern cfg_node_type_t cfg_type_optical; 
extern cfg_node_type_t cfg_type_interfacestack; 
extern cfg_node_type_t cfg_type_tr181ipv4addr; 
extern cfg_node_type_t cfg_type_tr181ipv6addr; 
extern cfg_node_type_t cfg_type_tr181ipv6pre; 
extern cfg_node_type_t cfg_type_natinterface; 
extern cfg_node_type_t cfg_type_dhcpv4client; 
extern cfg_node_type_t cfg_type_dhcpv6client; 
extern cfg_node_type_t cfg_type_natportmap; 
extern cfg_node_type_t cfg_type_routeforward4; 
extern cfg_node_type_t cfg_type_routeforward6; 
extern cfg_node_type_t cfg_type_dnsclient; 
extern cfg_node_type_t cfg_type_ipdiag;
extern cfg_node_type_t cfg_type_users;
extern cfg_node_type_t cfg_type_routeadv;
extern cfg_node_type_t cfg_type_devinfps;
extern cfg_node_type_t cfg_type_devinftem;
extern cfg_node_type_t cfg_type_tr181ipap;
extern cfg_node_type_t cfg_type_bridgeport; 
extern cfg_node_type_t cfg_type_routeadvifset; 
extern cfg_node_type_t cfg_type_tr181route; 
extern cfg_node_type_t cfg_type_tr181ipv4addr_pvc; 
extern cfg_node_type_t cfg_type_tr181ipv6addr_pvc; 
extern cfg_node_type_t cfg_type_dnsclient; 
extern cfg_node_type_t cfg_type_firewallch; 
extern cfg_node_type_t cfg_type_tr181firewall;
extern cfg_node_type_t cfg_type_firewalllev;
extern cfg_node_type_t cfg_type_firewallch_pvc; 
extern cfg_node_type_t cfg_type_tr181ipv6pre_pvc; 
extern cfg_node_type_t cfg_type_devinfloc;
extern cfg_node_type_t cfg_type_mgrser;
extern cfg_node_type_t cfg_type_ether;
extern cfg_node_type_t cfg_type_bridging;
extern cfg_node_type_t cfg_type_bridge;
extern cfg_node_type_t cfg_type_bridgevlan;
extern cfg_node_type_t cfg_type_bridgevlanport;
extern cfg_node_type_t cfg_type_bridgefilter;
extern cfg_node_type_t cfg_type_dhcpv4;
extern cfg_node_type_t cfg_type_dhcpv4clientreqopt;
extern cfg_node_type_t cfg_type_dhcpv4clientsentopt;
extern cfg_node_type_t cfg_type_dhcpv4serverpoolclientopt;
extern cfg_node_type_t cfg_type_dhcpv4serverpoolopt;
extern cfg_node_type_t cfg_type_dhcpv6;
extern cfg_node_type_t cfg_type_dhcpv6server;
extern cfg_node_type_t cfg_type_dhcpv6serverpool;
extern cfg_node_type_t cfg_type_dhcpv6clientrcvopt;
extern cfg_node_type_t cfg_type_dhcpv6clientsentopt;
extern cfg_node_type_t cfg_type_dhcpv6clientserver;
extern cfg_node_type_t cfg_type_dhcpv6serverpoolclient;
extern cfg_node_type_t cfg_type_dhcpv6serverpoolclientip6addr;
extern cfg_node_type_t cfg_type_dhcpv6serverpoolclientip6prev;
extern cfg_node_type_t cfg_type_dhcpv6serverpoolclientopt;
extern cfg_node_type_t cfg_type_dhcpv6serverpoolopt;
extern cfg_node_type_t cfg_type_wifi;
extern cfg_node_type_t cfg_type_wifiapacc;
extern cfg_node_type_t cfg_type_wifiapass;
extern cfg_node_type_t cfg_type_wifiep;
extern cfg_node_type_t cfg_type_wifieppro;
extern cfg_node_type_t cfg_type_wifiepprosec;
extern cfg_node_type_t cfg_type_wifiepsec;
extern cfg_node_type_t cfg_type_wifiepsta;
extern cfg_node_type_t cfg_type_wifiepwps;
extern cfg_node_type_t cfg_type_wifirdosta;
extern cfg_node_type_t cfg_type_wifissidsta;
extern cfg_node_type_t cfg_type_routeinfo;
extern cfg_node_type_t cfg_type_tqos;
extern cfg_node_type_t cfg_type_tqosapp;
extern cfg_node_type_t cfg_type_tqosclsficn;
extern cfg_node_type_t cfg_type_tqospolicer;
extern cfg_node_type_t cfg_type_tqosque;
extern cfg_node_type_t cfg_type_qosflow;
extern cfg_node_type_t cfg_type_qosquesta;
extern cfg_node_type_t cfg_type_qosshaper;
extern cfg_node_type_t cfg_type_host;
extern cfg_node_type_t cfg_type_dns;
extern cfg_node_type_t cfg_type_dns_forwarding;
extern cfg_node_type_t cfg_type_routeradv_itfsetting_opt;
extern cfg_node_type_t cfg_type_userinterface;
extern cfg_node_type_t cfg_type_captiveportal;
extern cfg_node_type_t cfg_type_perdSts;
extern cfg_node_type_t cfg_type_perdsts_samp;
extern cfg_node_type_t cfg_type_ether_rmonstats;
extern cfg_node_type_t cfg_type_mgrser_dld_group;
extern cfg_node_type_t cfg_type_dhcpv4rly;
extern cfg_node_type_t cfg_type_dhcpv4server;
extern cfg_node_type_t cfg_type_dhcpv4svrpool;
extern cfg_node_type_t cfg_type_dhcpv4svrpoolclient; 
extern cfg_node_type_t cfg_type_dhcpv4svrPoolStaAddr; 
extern cfg_node_type_t cfg_type_dhcpv4svrpolcipv4addr;
extern cfg_node_type_t cfg_type_etherinterface;
extern cfg_node_type_t cfg_type_tr181rip;
extern cfg_node_type_t cfg_type_tr181dslite; 

extern cfg_node_type_t cfg_type_WIFIAP;
extern cfg_node_type_t cfg_type_WIFIAPSec;
extern cfg_node_type_t cfg_type_WIFIRdo;
extern cfg_node_type_t cfg_type_WIFIAPWPS;
extern cfg_node_type_t cfg_type_WIFISsid;
void svc_cfg_boot_tr181(void);
int updateTR181AllInterface(void);
int findIPInterfaceEntry(int wanIf, int active);
int findTR181InterfaceEntry(struct tr181_interface_s interface);
int findEtherLinkEntry(int wanIf, int lanIndex, int active);
int findEtherVlanTermEntry(int wanIf, int active);
int findPPPInterfaceEntry(int wanIf, int active);
int wanSync2TR181PPPIfNode(int index, int wanIf);
int wanSync2TR181IPIfNode(int wanIf, int ipIFIdx);
int wanSync2tr181EtherVlanT(int entryIdx, int wanIf);
void setIPInterfaceLowerLayer(int ipIndex, char* lowerlayer);
void setPPPInterfaceLowerLayer(int pppIndex, char* lowerlayer);
void setEtherVlanTermLowerLayer(int vlanTIndex, char* lowerlayer);
void setEtherLinkLowerLayer(int etherLinkIndex, char* lowerlayer);
int findLanIPInterfaceEntry(void);
int setWanNodeByWanIf(char *wanNode, int wanIf);
int createTR181InterfaceEntry(struct tr181_interface_s interface);
int updateTR181AllWanInterface(void);

int wlanNodeSync2WIFIRdo(int bndFlag);
int updateTR181AllWLanInterface(void);
int setWlanNodeByLanPortIdx(char *path, int lanPortIdx);
int checkNodePath(char *path);

int createWifiSSIDEntry( int lanPort);
int getWifiRoIdxByBnd( int bndflag);
int getWifiBndByWifiRoIdx( int rdoIndex);
void setWifiSSIDLowerLayer( int wifiSSIDIndex, char *lowerlayer);
int createWifiAPEntry( int lanPort,char *wlanCommonPath);
int wlanNodeSync2WifiAPByIdx( int lanPortIdx, int apIdx);
int createWifiAPSecEntry( int lanPort);
int createWifiAPWPSEntry( int lanPort);
int findLanPortIdxbyApIdx(int idx);
#endif 
