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

#ifndef __CFG_TYPES_H__ 
#define __CFG_TYPES_H__ 

#include <netdb.h>

extern cfg_node_type_t cfg_type_root;
#if defined(TCSUPPORT_CT_PON)
extern cfg_node_type_t cfg_type_epon; 
extern cfg_node_type_t cfg_type_oam; 
extern cfg_node_type_t cfg_type_gpon; 
extern cfg_node_type_t cfg_type_pm; 
extern cfg_node_type_t cfg_type_omci; 
extern cfg_node_type_t cfg_type_xpon; 
#endif
extern cfg_node_type_t cfg_type_sysinfo; 
#if defined(TCSUPPORT_CT_IPV4_RADIO)
extern cfg_node_type_t cfg_type_sys; 
#endif
extern cfg_node_type_t cfg_type_lan; 
extern cfg_node_type_t cfg_type_wan; 
#ifdef TCSUPPORT_WLAN_VENDIE
extern cfg_node_type_t cfg_type_probeinfo;
extern cfg_node_type_t cfg_type_probevsie;
extern cfg_node_type_t cfg_type_proberesp;
#endif
extern cfg_node_type_t cfg_type_waninfo; 
#ifdef IPV6
extern cfg_node_type_t cfg_type_radvd; 
extern cfg_node_type_t cfg_type_dhcp6s; 
#endif
extern cfg_node_type_t cfg_type_dhcpd; 
extern cfg_node_type_t cfg_type_dhcprelay; 
extern cfg_node_type_t cfg_type_dhcplease; 
extern cfg_node_type_t cfg_type_lanhost;
extern cfg_node_type_t cfg_type_lanhost2;
extern cfg_node_type_t cfg_type_lanhost2black;
#ifdef TCSUPPORT_WLAN_VENDIE
extern cfg_node_type_t cfg_type_beaconvsie;
#endif
#ifdef TCSUPPORT_WLAN
extern cfg_node_type_t cfg_type_wlan; 
#ifdef TCSUPPORT_WLAN_AC
extern cfg_node_type_t cfg_type_wlan11ac;
#endif
#endif
extern cfg_node_type_t cfg_type_firewall; 
#ifdef TCSUPPORT_VPN
extern cfg_node_type_t cfg_type_ipsec;
#endif
extern cfg_node_type_t cfg_type_route; 
extern cfg_node_type_t cfg_type_route6; 
extern cfg_node_type_t cfg_type_nat; 
extern cfg_node_type_t cfg_type_dmz; 
extern cfg_node_type_t cfg_type_virserver; 
#if defined(TCSUPPORT_CT_E8GUI)
extern cfg_node_type_t cfg_type_algswitch; 
#endif
extern cfg_node_type_t cfg_type_adsl; 
#if defined(TCSUPPORT_CT_E8DDNS)
extern cfg_node_type_t cfg_type_ddns; 
#endif
extern cfg_node_type_t cfg_type_snmpd; 
extern cfg_node_type_t cfg_type_upnpd; 
extern cfg_node_type_t cfg_type_timezone; 
#ifdef CWMP
extern cfg_node_type_t cfg_type_cwmp; 
#endif
#ifdef TCSUPPORT_QOS
extern cfg_node_type_t cfg_type_qos; 
#endif

extern cfg_node_type_t cfg_type_mac; 
#ifdef TCSUPPORT_WLAN
extern cfg_node_type_t cfg_type_wifimactab; 
#endif
#ifdef CWMP
extern cfg_node_type_t cfg_type_tr069attr; 
extern cfg_node_type_t cfg_type_ipinterface; 
extern cfg_node_type_t cfg_type_cwmproute; 
extern cfg_node_type_t cfg_type_switchpara; 
#endif
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT) || defined(TCSUPPORT_TRUE_LANDING_PAGE)
extern cfg_node_type_t cfg_type_portal; 
#endif
extern cfg_node_type_t cfg_type_account; 
#if defined(CT_COM_DEVICEREG)
extern cfg_node_type_t cfg_type_deviceaccount; 
#endif
extern cfg_node_type_t cfg_type_system; 
extern cfg_node_type_t cfg_type_webcurset; 
extern cfg_node_type_t cfg_type_webcustom; 
extern cfg_node_type_t cfg_type_dyndisp; 
#if defined(TCSUPPORT_CT_GUIACCESSLIMIT)
extern cfg_node_type_t cfg_type_dyncwmpattr; 
#endif
extern cfg_node_type_t cfg_type_diagnostic; 
extern cfg_node_type_t cfg_type_deviceinfo; 
extern cfg_node_type_t cfg_type_info; 
#if defined(TCSUPPORT_CT_VLAN_BIND)
extern cfg_node_type_t cfg_type_vlanbind; 
#endif
extern cfg_node_type_t cfg_type_acl; 
extern cfg_node_type_t cfg_type_ipmacfilter; 
extern cfg_node_type_t cfg_type_appfilter; 
extern cfg_node_type_t cfg_type_urlfilter; 
extern cfg_node_type_t cfg_type_autopvc; 
#if defined(SSL) || defined(TCSUPPORT_CWMP_SSL)
extern cfg_node_type_t cfg_type_sslca; 
#endif
#ifdef TCSUPPORT_SSH
extern cfg_node_type_t cfg_type_ssh; 
#endif

#ifdef TCSUPPORT_DMS
extern cfg_node_type_t cfg_type_dms; 
#endif

#if defined(TCSUPPORT_CT_L2TP_VPN)
extern cfg_node_type_t cfg_type_vpnlist;
#endif

#if defined(TCSUPPORT_IGMP_PROXY)
extern cfg_node_type_t cfg_type_igmpproxy; 
#endif
#ifdef TCSUPPORT_MLD_PROXY
extern cfg_node_type_t cfg_type_mldproxy;
#endif
#ifdef TCSUPPORT_SYSLOG
extern cfg_node_type_t cfg_type_syslog; 
#endif
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
extern cfg_node_type_t cfg_type_accesslimit; 
#endif
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
extern cfg_node_type_t cfg_type_ctdiagnostic; 
#endif
#if defined(TCSUPPORT_CT_ALARMMONITOR)
extern cfg_node_type_t cfg_type_devicealarm; 
extern cfg_node_type_t cfg_type_devicemonitor; 
#endif
#if defined(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)
extern cfg_node_type_t cfg_type_appftp; 
#endif
#if defined(TCSUPPORT_CT_USB_BACKUPRESTORE)
extern cfg_node_type_t cfg_type_usbrestore;
#endif
#if defined(TCSUPPORT_CT_LOOPDETECT)
extern cfg_node_type_t cfg_type_loopdetect; 
#endif
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
extern cfg_node_type_t cfg_type_mobile; 
#endif
#if defined(TCSUPPORT_CT_DS_LIMIT)
extern cfg_node_type_t cfg_type_dataspeedlimit; 
#endif

#if defined(TCSUPPORT_CT_JOYME)
extern cfg_node_type_t cfg_type_attachwandnstunnel; 
extern cfg_node_type_t cfg_type_osgiupgrade; 
extern cfg_node_type_t cfg_type_dnsspeedlimit; 
#endif

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_JOYME)
extern cfg_node_type_t cfg_type_attachwandnstunnel;
extern cfg_node_type_t cfg_type_dnsspeedlimit;
extern cfg_node_type_t cfg_type_usbmount; 
extern cfg_node_type_t cfg_type_plugin;
#endif

#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
extern cfg_node_type_t cfg_type_bandwidth; 
extern cfg_node_type_t cfg_type_maxbandwidth; 
#endif

#if defined (CWMP) && defined(TR111)
extern cfg_node_type_t cfg_type_dhcpclient;
extern cfg_node_type_t cfg_type_dhcpclientlimit;
#endif
#if defined(TCSUPPORT_CT_UPNP_DM)
extern cfg_node_type_t cfg_type_upnpdm; 
extern cfg_node_type_t cfg_type_upnpdmswprofile; 
extern cfg_node_type_t cfg_type_upnpdmservice; 
extern cfg_node_type_t cfg_type_upnpdmapint; 
extern cfg_node_type_t cfg_type_upnpdmapiptv; 
#endif

#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
extern cfg_node_type_t cfg_type_logicid; 
#endif
#endif
#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
extern cfg_node_type_t cfg_type_pppoeemulator; 
#endif

#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
extern cfg_node_type_t cfg_type_ipoe_emulator; 
#endif

#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CUC)
extern cfg_node_type_t cfg_type_storageaccessright; 
#endif

#if defined(TCSUPPORT_CT_JOYME)
extern cfg_node_type_t cfg_type_laninfo; 
#endif
extern cfg_node_type_t cfg_type_globalstate; 
extern cfg_node_type_t cfg_type_autoexec;
extern cfg_node_type_t cfg_type_voipsysparam;
extern cfg_node_type_t cfg_type_voipbasic;
extern cfg_node_type_t cfg_type_voipadvanced;
extern cfg_node_type_t cfg_type_voipdigitmap;
extern cfg_node_type_t cfg_type_voiptest;
extern cfg_node_type_t cfg_type_voipsimulatetest;
extern cfg_node_type_t cfg_type_voipdiag;
extern cfg_node_type_t cfg_type_voipcallctrl;
extern cfg_node_type_t cfg_type_voipmedia;
extern cfg_node_type_t cfg_type_voipcodecs;
extern cfg_node_type_t cfg_type_voiph248;
extern cfg_node_type_t cfg_type_infovoip;
extern cfg_node_type_t cfg_type_infovoiph248;
extern cfg_node_type_t cfg_type_infovoippoorql;
extern cfg_node_type_t cfg_type_voipspeed;
#if defined(TCSUPPORT_EPON_OAM_CUC)
extern cfg_node_type_t cfg_type_cucping; 
#endif
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
extern cfg_node_type_t cfg_type_user; 
extern cfg_node_type_t cfg_type_macfilter;
extern cfg_node_type_t cfg_type_ipfilter; 
extern cfg_node_type_t cfg_type_whiteurl; 
extern cfg_node_type_t cfg_type_blackurl;
#endif
#if defined(TCSUPPORT_PPPOE_SIMULATE)
extern cfg_node_type_t cfg_type_pppoesimulate; 
#endif
#if defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
extern cfg_node_type_t cfg_type_samba; 
#endif
extern cfg_node_type_t cfg_type_dproxy; 
#if defined(TCSUPPORT_CMCCV2)
extern cfg_node_type_t cfg_type_voipmonitor; 
extern cfg_node_type_t cfg_type_wlanshare; 
extern cfg_node_type_t cfg_type_apipermission;
extern cfg_node_type_t cfg_type_vpndest_list;
extern cfg_node_type_t cfg_type_cmcc_info;
extern cfg_node_type_t cfg_type_host_name;
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
extern cfg_node_type_t cfg_type_trafficmonitor; 
extern cfg_node_type_t cfg_type_trafficforward; 
extern cfg_node_type_t cfg_type_trafficmirror; 
extern cfg_node_type_t cfg_type_trafficqosservice;
extern cfg_node_type_t cfg_type_trafficdetailprocess;
extern cfg_node_type_t cfg_type_guestssidinfo;
#endif
#if defined(TCSUPPORT_CUC)
extern cfg_node_type_t cfg_type_fastpath;
extern cfg_node_type_t cfg_type_accesscontrol;
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(RA_PARENTALCONTROL)
extern cfg_node_type_t cfg_type_parental;
extern cfg_node_type_t cfg_type_parental_mac;
#endif
#if defined(TCSUPPORT_CT_PMINFORM) || defined(TCSUPPORT_CT_JOYME2)
extern cfg_node_type_t cfg_type_pminform; 
#endif
#if defined(TCSUPPORT_GENERAL_MULTILANGUAGE)
extern cfg_node_type_t cfg_type_string;
extern cfg_node_type_t cfg_type_languageswitch;
#endif
#if defined(TCSUPPORT_PORT_TRIGGER)
extern cfg_node_type_t cfg_type_portTriggering; 
extern cfg_node_type_t cfg_type_GUITemp; 
#endif
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
extern cfg_node_type_t cfg_type_timer;
#endif
#if defined(TCSUPPORT_CT_JOYME2)
#if defined(TCSUPPORT_WLAN)
extern cfg_node_type_t cfg_type_wlanmapping;
extern cfg_node_type_t cfg_type_wlancollectinfo;
#endif
extern cfg_node_type_t cfg_type_ctc_dnsserver;
extern cfg_node_type_t cfg_type_incomingfilter;
extern cfg_node_type_t cfg_type_dnsfilter;
extern cfg_node_type_t cfg_type_speedtest;
extern cfg_node_type_t cfg_type_vpndomain;
extern cfg_node_type_t cfg_type_vpndomaincache;
extern cfg_node_type_t cfg_type_vpnips;
extern cfg_node_type_t cfg_type_vpnmac;
extern cfg_node_type_t cfg_type_process;
extern cfg_node_type_t cfg_type_processres;
extern cfg_node_type_t cfg_type_qosattr;
#endif
#if defined(TCSUPPORT_CT_SDN)
extern cfg_node_type_t cfg_type_sdn;
#endif
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
extern cfg_node_type_t cfg_type_macblacklist;
extern cfg_node_type_t cfg_type_macwhitelist;
#endif
#if defined(TCSUPPORT_ANDLINK)
extern cfg_node_type_t cfg_type_alinkmgr; 
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
extern cfg_node_type_t cfg_type_apcli; 
#endif
#if defined(TCSUPPORT_NP_CMCC)
extern cfg_node_type_t cfg_type_apwaninfo;
extern cfg_node_type_t cfg_type_if6regioninfo;
#endif
#if defined(TCSUPPORT_CWMP_TR181)
extern cfg_node_type_t cfg_type_tr181; 
extern cfg_node_type_t cfg_type_tr181ip;
extern cfg_node_type_t cfg_type_ipdiag; 
extern cfg_node_type_t cfg_type_users; 
extern cfg_node_type_t cfg_type_devinfps; 
extern cfg_node_type_t cfg_type_devinftem; 
extern cfg_node_type_t cfg_type_tr181ipap;
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
extern cfg_node_type_t cfg_type_dhcpv4svrpolcipv4addr; 
extern cfg_node_type_t cfg_type_dhcpv4svrPoolStaAddr; 
extern cfg_node_type_t cfg_type_routeforward4;
extern cfg_node_type_t cfg_type_tr181route;
extern cfg_node_type_t cfg_type_etherinterface;
extern cfg_node_type_t cfg_type_tr181rip;
extern cfg_node_type_t cfg_type_tr181ipv6rd;
extern cfg_node_type_t cfg_type_routeadvifset;
extern cfg_node_type_t cfg_type_routeadv;
extern cfg_node_type_t cfg_type_bridgeport;
extern cfg_node_type_t cfg_type_dhcpv6client;
extern cfg_node_type_t cfg_type_tr181ipv4addr;
extern cfg_node_type_t cfg_type_tr181ipv6addr;
extern cfg_node_type_t cfg_type_tr181dslite; 
extern cfg_node_type_t cfg_type_WIFIAP;
extern cfg_node_type_t cfg_type_WIFIAPSec;
extern cfg_node_type_t cfg_type_WIFIRdo;
extern cfg_node_type_t cfg_type_WIFIAPWPS;
extern cfg_node_type_t cfg_type_WIFISsid;
#if defined(TCSUPPORT_CT_DSL_EX)
extern cfg_node_type_t cfg_type_tr181_atmlink;
extern cfg_node_type_t cfg_type_tr181_dslChannel;
extern cfg_node_type_t cfg_type_tr181_dslLine;
extern cfg_node_type_t cfg_type_tr181_ptmlink;
#endif
#endif
#if defined(TCSUPPORT_ECNT_MAP)
extern cfg_node_type_t cfg_type_mesh; 
#endif
#if defined(TCSUPPORT_CT_JOYME4)
extern cfg_node_type_t cfg_type_bridgedata_acl; 
extern cfg_node_type_t cfg_type_iptv; 
extern cfg_node_type_t cfg_type_cfgauth; 
extern cfg_node_type_t cfg_type_downlinkqos; 
extern cfg_node_type_t cfg_type_wancfgdata; 
#endif
#if defined(TCSUPPORT_STUN)
extern cfg_node_type_t cfg_type_stun;
#endif
#if defined(TCSUPPORT_CT_DSL_EX)	 
extern cfg_node_type_t cfg_type_wanatm;
extern cfg_node_type_t cfg_type_wanptm;
extern cfg_node_type_t cfg_type_virserveratm;
extern cfg_node_type_t cfg_type_virserverptm;
extern cfg_node_type_t cfg_type_routeatm;
extern cfg_node_type_t cfg_type_routeptm;
extern cfg_node_type_t cfg_type_route6atm;
extern cfg_node_type_t cfg_type_route6ptm;
extern cfg_node_type_t cfg_type_dmzatm;
extern cfg_node_type_t cfg_type_dmzptm;
extern cfg_node_type_t cfg_type_igmpproxyatm;
extern cfg_node_type_t cfg_type_igmpproxyptm;
extern cfg_node_type_t cfg_type_mldproxyatm;
extern cfg_node_type_t cfg_type_mldproxyptm;
extern cfg_node_type_t cfg_type_ddnsatm;
extern cfg_node_type_t cfg_type_ddnsptm;
extern cfg_node_type_t cfg_type_cwmp_dsl_diagnostic; 


int svc_cfg_boot_adsl(void);
#endif

#ifdef TCSUPPORT_CUC
extern cfg_node_type_t cfg_type_alarm_info;
#endif
#if defined(TCSUPPORT_VXLAN)
extern cfg_node_type_t cfg_type_vxlan; 
#endif
#if defined(TCSUPPORT_CT_UBUS)
extern cfg_node_type_t cfg_type_ctcapd;
#endif
#if defined(TCSUPPORT_CT_VRWAN)
extern cfg_node_type_t cfg_type_vrserver;
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
extern cfg_node_type_t cfg_type_ipoediag;
#endif
#if defined(TCSUPPORT_CT_JOYME4)
cfg_node_type_t cfg_type_cloudservice;

#endif

extern cfg_node_type_t cfg_type_interfacerate;

#define FAIL -1
#define SUCCESS 0

int svc_cfg_boot_acl(void);
int svc_cfg_boot_ipmacfilter(void);
int svc_cfg_boot_urlfilter(void);
#if defined(TCSUPPORT_CT_UBUS)
int svc_cfg_boot_dnsfilter(void);
#endif
int svc_cfg_boot_upnpd(void);
int svc_cfg_boot_snmpd(void);
#if defined(TCSUPPORT_CT_E8DDNS)
int svc_cfg_boot_ddns(void);
#endif

#ifdef CWMP
int svc_cfg_boot_cwmp(void);
#endif
#ifdef TCSUPPORT_QOS
int svc_cfg_boot_qos(void);
#endif

#if defined(TCSUPPORT_CT_L2TP_VPN)
int svc_cfg_vpnlist_boot(void);
#endif


#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT) || defined(TCSUPPORT_TRUE_LANDING_PAGE)
int svc_cfg_boot_portal(void);
#endif

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
int svc_cfg_boot_accesslimit(void);
#endif
int svc_cfg_boot_loopdetect(void);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
int svc_cfg_boot_mobile(void);
#endif
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
int svc_cfg_boot_logicid(void);
#endif
#endif
#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
int svc_cfg_boot_pppoeemulator(void);
#endif

#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
int svc_cfg_boot_ipoe_emulator(void);
#endif

#if defined(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)
int svc_cfg_boot_appftp(void);
#endif
int svc_cfg_boot_autoexec(void);
int svc_cfg_boot_account(void);
#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
int svc_cfg_boot_bandwidth(void);     		
#endif

#if defined(TCSUPPORT_CT_DS_LIMIT)
int svc_cfg_boot_dataspeedlimit(void);  
#endif

#if defined(TCSUPPORT_CT_JOYME)
int svc_cfg_boot_osgiupgrade( void);   		
#endif
#if defined(TCSUPPORT_ANDLINK)
int svc_cfg_boot_alinkmgr(void);
#endif

int tc_getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result, int timeout);
int getDownloadstate(void);

int svc_cfg_boot_wan(void);
int svc_cfg_boot_lan(void);
int svc_cfg_boot_lanhost2(void);
#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
int svc_cfg_boot_samba(void);
#endif

#if 1//
int svc_cfg_boot_lanhost_joyme(void);
#endif

#ifdef IPV6
int svc_cfg_boot_radvd(void);
int svc_cfg_boot_dhcp6s(void);
#endif
int svc_cfg_boot_dhcpd(void);
int svc_cfg_boot_dhcprelay(void);
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
int svc_cfg_boot_ctdiagnostic(void);
#endif
#ifdef CWMP
int svc_cfg_boot_switchpara(void);
#endif
#if defined(CT_COM_DEVICEREG)
int svc_cfg_boot_deviceaccount(void);
#endif
#ifdef TCSUPPORT_WLAN
int svc_cfg_boot_wlan(void);
#ifdef TCSUPPORT_WLAN_AC
int svc_cfg_boot_wlan11ac(void);
#endif
#endif
#if defined(TCSUPPORT_ECN_SIP) || defined(TCSUPPORT_ECN_MEGACO)
int svc_cfg_boot_infovoippoorql(void);
#endif

#if defined(TCSUPPORT_CMCCV2)
int svc_cfg_boot_voipmonitor(void);
int svc_cfg_boot_wlanshare(void);
int svc_cfg_boot_apipermission(void);
int svc_cfg_vpndestlist_boot(void);
int svc_cfg_cmcc_info_boot(void);
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
int svc_cfg_boot_trafficmonitor(void);
int svc_cfg_boot_trafficforward(void);
int svc_cfg_boot_trafficmirror(void);
int svc_cfg_boot_trafficqosservice(void);
int svc_cfg_boot_trafficdetailprocess(void);
int svc_cfg_boot_guestssidinfo(void);
#endif
#if defined(TCSUPPORT_CUC)
int svc_cfg_boot_storageaccessright(void);
int svc_cfg_boot_accesscontrol(void);
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(RA_PARENTALCONTROL)
int svc_cfg_boot_parental(void);
int svc_cfg_boot_parental_mac(void);
#endif
#if defined(TCSUPPORT_PORT_TRIGGER)
int svc_cfg_porttriggering_boot(void); 
#endif
#if defined(TCSUPPORT_CT_PMINFORM) || defined(TCSUPPORT_CT_JOYME2)
int svc_cfg_boot_pminform(void);
#endif
#if defined(TCSUPPORT_CT_JOYME2)
#ifdef TCSUPPORT_WLAN
int svc_cfg_wlanmapping_boot(void);
#endif
int svc_cfg_timer_boot(void);
int svc_cfg_boot_incomingfilter(void);
#endif
#if defined(TCSUPPORT_CT_SDN)
int svc_cfg_boot_sdn(void);
#endif

#if defined(TCSUPPORT_CT_WLAN_JOYME3)
int svc_cfg_maclist_commit(void);
#endif

#ifdef TCSUPPORT_DMS
int svc_cfg_boot_dms(void);
#endif
#ifdef TCSUPPORT_SYSLOG
int svc_cfg_boot_syslog(void);
#endif
#if defined(TCSUPPORT_CT_IPV4_RADIO)
int svc_cfg_boot_sys(void);
#endif

void fputs_escape(char *cmd, FILE *fp);
int svc_cfg_boot_firewall(void);
int svc_cfg_boot_system(void);
int svc_cfg_boot_account(void);
int svc_cfg_boot_qos(void);
#if defined(TCSUPPORT_CT_E8GUI)
int svc_cfg_boot_algswitch(void);
#endif
int svc_cfg_boot_dmz(void);
int svc_cfg_boot_virServ(void);
int svc_cfg_boot_route6(void);
#ifdef TCSUPPORT_WLAN_VENDIE
int svc_cfg_boot_proberxvsie(void);
int svc_cfg_boot_proberespvsie(void);
#endif 
#ifdef TCSUPPORT_SSH
int svc_cfg_boot_ssh(void);
#endif
int svc_cfg_boot_webcustom(void);

#if defined(TCSUPPORT_ECNT_MAP)
int svc_cfg_boot_mesh(void);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
int svc_cfg_boot_bridgedata_acl(void);
int svc_cfg_boot_downlinkqos(void);
#endif
#if defined(TCSUPPORT_STUN)
int svc_cfg_boot_stun(void);
#endif

#if defined(TCSUPPORT_VXLAN)
int svc_cfg_boot_vxlan(void);
#endif
int svc_cfg_boot_timezone(void);

void cfg_type_wan_send_evt(int type, int evt_id, char* buf);
#endif 
