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
#if defined(TCSUPPORT_CWMP_TR181)
#include "cfg_tr181_global.h" 
#endif

static cfg_node_ops_t cfg_type_root_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_root_child[] = { 
#if defined(TCSUPPORT_CT_PON)
#ifndef TCSUPPORT_NP
	 &cfg_type_epon, 
	 &cfg_type_oam, 
	 &cfg_type_gpon, 
	 &cfg_type_pm, 
	 &cfg_type_omci, 
#endif
	 &cfg_type_xpon, 
#endif
	 &cfg_type_sysinfo, 
#if defined(TCSUPPORT_CT_IPV4_RADIO)	
	 &cfg_type_sys, 
#endif
	 &cfg_type_lan, 
	 &cfg_type_wan, 
	 &cfg_type_waninfo, 
#ifdef TCSUPPORT_WLAN_VENDIE	
	 &cfg_type_probeinfo,
	 &cfg_type_probevsie,
	 &cfg_type_proberesp,
#endif
#ifdef IPV6
	 &cfg_type_radvd, 
	 &cfg_type_dhcp6s, 
#endif
	 &cfg_type_dhcpd, 
	 &cfg_type_dhcprelay, 
	 &cfg_type_dhcplease, 
	 &cfg_type_lanhost, 
	 &cfg_type_lanhost2,
	 &cfg_type_lanhost2black,
#ifdef TCSUPPORT_WLAN_VENDIE
	 &cfg_type_beaconvsie,
#endif
#ifdef TCSUPPORT_WLAN
	 &cfg_type_wlan, 
#ifdef TCSUPPORT_WLAN_AC
	 &cfg_type_wlan11ac,
#endif
#endif
	 &cfg_type_firewall, 
#ifdef TCSUPPORT_VPN
     &cfg_type_ipsec,
#endif
	 &cfg_type_route, 
	 &cfg_type_route6, 
	 &cfg_type_nat, 
	 &cfg_type_dmz, 
#ifndef TCSUPPORT_NP
	 &cfg_type_virserver, 
#endif
#if defined(TCSUPPORT_CT_E8GUI)
	 &cfg_type_algswitch, 
#endif
#ifndef TCSUPPORT_NP
#if !defined(TCSUPPORT_WAN_GPON) && !defined(TCSUPPORT_WAN_EPON)
	 &cfg_type_adsl, 
#endif
#endif
#if defined(TCSUPPORT_CT_E8DDNS)
	 &cfg_type_ddns, 
#endif
	 &cfg_type_snmpd, 
	 &cfg_type_upnpd, 
	 &cfg_type_timezone, 
	 &cfg_type_mac, 
#ifdef TCSUPPORT_WLAN
	 &cfg_type_wifimactab, 
#endif
#ifdef CWMP
	 &cfg_type_cwmp, 
	 &cfg_type_tr069attr, 
	 &cfg_type_ipinterface, 
	 &cfg_type_cwmproute, 
	 &cfg_type_switchpara, 
#endif


#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT) || defined(TCSUPPORT_TRUE_LANDING_PAGE)
	 &cfg_type_portal, 
#endif
	 &cfg_type_account, 
#if defined(CT_COM_DEVICEREG)
	 &cfg_type_deviceaccount, 
#endif
	 &cfg_type_system, 
	 &cfg_type_webcurset, 
	 &cfg_type_webcustom, 
	 &cfg_type_dyndisp, 
#if defined(TCSUPPORT_CT_GUIACCESSLIMIT)
	 &cfg_type_dyncwmpattr, 
#endif
	 &cfg_type_diagnostic, 
	 &cfg_type_deviceinfo, 
	 &cfg_type_info, 
#ifdef TCSUPPORT_QOS
	 &cfg_type_qos, 
#endif
#if defined(TCSUPPORT_CT_VLAN_BIND)
	 &cfg_type_vlanbind,
#endif
	 &cfg_type_acl, 
	 &cfg_type_ipmacfilter, 
	 &cfg_type_appfilter, 
	 &cfg_type_urlfilter, 
	 &cfg_type_autopvc, 
#if defined(SSL) || defined(TCSUPPORT_CWMP_SSL)
	 &cfg_type_sslca, 
#endif
#ifdef TCSUPPORT_DMS
	 &cfg_type_dms, 
#endif
#if defined(TCSUPPORT_IGMP_PROXY)
	 &cfg_type_igmpproxy, 
#endif
#ifdef TCSUPPORT_MLD_PROXY
	 &cfg_type_mldproxy,
#endif	 
#ifdef TCSUPPORT_SYSLOG
	 &cfg_type_syslog, 
#endif
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
	 &cfg_type_accesslimit, 
#endif
#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
	 &cfg_type_ctdiagnostic, 
#endif
#if defined(TCSUPPORT_CT_ALARMMONITOR)
	 &cfg_type_devicealarm, 
	 &cfg_type_devicemonitor, 
#endif
#if defined(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)
	 &cfg_type_appftp, 
#endif
#if defined(TCSUPPORT_CT_USB_BACKUPRESTORE)
	 &cfg_type_usbrestore, 
#endif
#if defined(TCSUPPORT_CT_LOOPDETECT)
	 &cfg_type_loopdetect, 
#endif
#if defined(TCSUPPORT_CT_DS_LIMIT)
	 &cfg_type_dataspeedlimit, 
#endif
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_mobile, 
#endif
#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CT_JOYME2)
	 &cfg_type_usbmount, 
#endif
#if defined(TCSUPPORT_CT_UPNP_DM)
	 &cfg_type_upnpdm, 
	 &cfg_type_upnpdmswprofile, 
	 &cfg_type_upnpdmservice, 
	 &cfg_type_upnpdmapint, 
	 &cfg_type_upnpdmapiptv, 
#endif
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
	 &cfg_type_logicid, 
#endif
#endif
#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
	 &cfg_type_pppoeemulator, 
#endif
#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
	 &cfg_type_ipoe_emulator,
#endif
#if defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CUC)
	 &cfg_type_storageaccessright,
#endif
#if defined(TCSUPPORT_CT_JOYME)
	 &cfg_type_osgiupgrade, 
	 &cfg_type_laninfo, 
#endif

#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_JOYME)
	 &cfg_type_dnsspeedlimit,
	 &cfg_type_attachwandnstunnel,
	 &cfg_type_plugin,
#endif
#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
	 &cfg_type_bandwidth, 
	 &cfg_type_maxbandwidth,
#endif
	 &cfg_type_globalstate,
#if defined (CWMP) && defined(TR111)
	 &cfg_type_dhcpclient,
	 &cfg_type_dhcpclientlimit, 
#endif
#if defined(TCSUPPORT_VOIP)
	 &cfg_type_voipsysparam,
	 &cfg_type_voipbasic,
	 &cfg_type_voipadvanced,
	 &cfg_type_voipdigitmap,
	 &cfg_type_voiptest,
	 &cfg_type_voipsimulatetest,
	 &cfg_type_voipdiag,
	 &cfg_type_voipcallctrl,
	 &cfg_type_voipmedia,
	 &cfg_type_voipcodecs,
	 &cfg_type_voiph248,
	 &cfg_type_infovoip,
	 &cfg_type_infovoiph248,
	 &cfg_type_infovoippoorql,
	 &cfg_type_voipspeed,
#endif
#if defined(TCSUPPORT_EPON_OAM_CUC)
	 &cfg_type_cucping,
#endif
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER)
	 &cfg_type_user,
	 &cfg_type_macfilter,
	 &cfg_type_ipfilter,
	 &cfg_type_whiteurl,
	 &cfg_type_blackurl,
#endif
#if defined(TCSUPPORT_PPPOE_SIMULATE)
	 &cfg_type_pppoesimulate,
#endif
#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
	  &cfg_type_samba, 
#endif
#ifdef TCSUPPORT_SSH
	  &cfg_type_ssh,
#endif
	  &cfg_type_dproxy, 
#if defined(TCSUPPORT_CT_L2TP_VPN)
	  &cfg_type_vpnlist,
#endif
#if defined(TCSUPPORT_CMCCV2)
	  &cfg_type_voipmonitor, 
	  &cfg_type_wlanshare, 
	  &cfg_type_apipermission, 
	  &cfg_type_cmcc_info,
	  &cfg_type_host_name,
	  &cfg_type_vpndest_list,
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
	  &cfg_type_trafficmonitor, 
	  &cfg_type_trafficforward, 
	  &cfg_type_trafficmirror, 
	  &cfg_type_trafficqosservice,
	  &cfg_type_trafficdetailprocess,
	  &cfg_type_guestssidinfo,
#endif
#if defined(TCSUPPORT_CUC)
	  &cfg_type_fastpath,
	  &cfg_type_accesscontrol,
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(RA_PARENTALCONTROL)
	  &cfg_type_parental,
	  &cfg_type_parental_mac,
#endif
#if defined(TCSUPPORT_CT_PMINFORM) || defined(TCSUPPORT_CT_JOYME2)
	  &cfg_type_pminform, 
#endif
#if defined(TCSUPPORT_GENERAL_MULTILANGUAGE)
	 &cfg_type_string,
	 &cfg_type_languageswitch,
#endif
#if defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CT_UBUS)
	  &cfg_type_timer,
#endif
#if defined(TCSUPPORT_CT_JOYME2)
#ifdef TCSUPPORT_WLAN
	  &cfg_type_wlanmapping,
#endif
	  &cfg_type_ctc_dnsserver,
	  &cfg_type_incomingfilter,
	  &cfg_type_dnsfilter,
#ifdef TCSUPPORT_WLAN
	  &cfg_type_wlancollectinfo,
#endif
	  &cfg_type_speedtest,
	  &cfg_type_vpndomain,
	  &cfg_type_vpndomaincache,
	  &cfg_type_vpnmac,
	  &cfg_type_vpnips,
	  &cfg_type_process,
	  &cfg_type_processres,
	  &cfg_type_qosattr,
#endif
#if defined(TCSUPPORT_CT_SDN)
	  &cfg_type_sdn,        
#endif
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
	  &cfg_type_macblacklist,
	  &cfg_type_macwhitelist,
#endif
#if defined(TCSUPPORT_PORT_TRIGGER)
	 &cfg_type_portTriggering, 
	 &cfg_type_GUITemp, 
#endif
#if defined(TCSUPPORT_CWMP_TR181)
	 &cfg_type_tr181,
	 &cfg_type_tr181ip,
	 &cfg_type_tr181ppp,
	 &cfg_type_ethervlant,
	 &cfg_type_etherlink,
	 &cfg_type_optical,
	 &cfg_type_interfacestack,
	 &cfg_type_tr181ipv4addr,
	 &cfg_type_tr181ipv6addr,
	 &cfg_type_tr181ipv6pre,
	 &cfg_type_natinterface,
	 &cfg_type_dhcpv4client,
	 &cfg_type_dhcpv6client,
	 &cfg_type_natportmap,
	 &cfg_type_routeforward4,
	 &cfg_type_routeforward6,
	 &cfg_type_dnsclient,
	 &cfg_type_ipdiag,
	 &cfg_type_users,
	 &cfg_type_devinfps,
	 &cfg_type_devinftem,
	 &cfg_type_tr181ipap,
	 &cfg_type_routeadvifset,
	 &cfg_type_routeadv,
	 &cfg_type_bridgeport,
	 &cfg_type_tr181route,
	 &cfg_type_firewallch,
	 &cfg_type_tr181firewall,
	 &cfg_type_firewalllev,
     &cfg_type_devinfloc,
	 &cfg_type_mgrser,
	 &cfg_type_ether,
	 &cfg_type_bridging,
	 &cfg_type_bridge,
	 &cfg_type_bridgevlan,
	 &cfg_type_bridgevlanport,
	 &cfg_type_bridgefilter,
	 &cfg_type_dhcpv4,
	 &cfg_type_dhcpv4rly,
	 &cfg_type_dhcpv4server,
	 &cfg_type_dhcpv4svrpool,
	 &cfg_type_dhcpv4svrpoolclient,
	 &cfg_type_dhcpv4svrpolcipv4addr,
	 &cfg_type_dhcpv4svrPoolStaAddr,
	 &cfg_type_dhcpv4serverpoolopt,
	 &cfg_type_dhcpv4clientreqopt,
	 &cfg_type_dhcpv4clientsentopt,
	 &cfg_type_dhcpv4serverpoolclientopt,
	 &cfg_type_dhcpv6,
	 &cfg_type_dhcpv6server,
	 &cfg_type_dhcpv6serverpool,
	 &cfg_type_dhcpv6clientrcvopt,
	 &cfg_type_dhcpv6clientsentopt,
	 &cfg_type_dhcpv6clientserver,
	 &cfg_type_dhcpv6serverpoolclient,
	 &cfg_type_dhcpv6serverpoolclientip6addr,
	 &cfg_type_dhcpv6serverpoolclientip6prev,
	 &cfg_type_dhcpv6serverpoolclientopt,
	 &cfg_type_dhcpv6serverpoolopt,
	 &cfg_type_wifi,
	 &cfg_type_wifiapacc,
	 &cfg_type_wifiapass,
	 &cfg_type_wifiep,
	 &cfg_type_wifieppro,
	 &cfg_type_wifiepprosec,
	 &cfg_type_wifiepsec,
	 &cfg_type_wifiepsta,
	 &cfg_type_wifiepwps,
	 &cfg_type_wifirdosta,
	 &cfg_type_wifissidsta,
	 &cfg_type_WIFIAP,
	 &cfg_type_WIFIAPSec,
	 &cfg_type_WIFIRdo,
	 &cfg_type_WIFIAPWPS,
	 &cfg_type_WIFISsid,
	 &cfg_type_routeinfo,
	 &cfg_type_tqos,
	 &cfg_type_tqosapp,
	 &cfg_type_tqosclsficn,
	 &cfg_type_tqospolicer,
	 &cfg_type_tqosque,
	 &cfg_type_qosflow,
	 &cfg_type_qosquesta,
	 &cfg_type_qosshaper,
	 &cfg_type_host,
	 &cfg_type_dns,
	 &cfg_type_dns_forwarding,
	 &cfg_type_routeradv_itfsetting_opt,
	 &cfg_type_userinterface,
	 &cfg_type_captiveportal,
	 &cfg_type_perdSts,
	 &cfg_type_perdsts_samp,
	 &cfg_type_ether_rmonstats,
	 &cfg_type_mgrser_dld_group,
	 &cfg_type_etherinterface,
	 &cfg_type_tr181rip,
	 &cfg_type_tr181ipv6rd,
	 &cfg_type_tr181dslite,
#if defined(TCSUPPORT_CT_DSL_EX)
	 &cfg_type_tr181_atmlink,
	 &cfg_type_tr181_ptmlink,
	 &cfg_type_tr181dslChannel,
	 &cfg_type_tr181dslline,
#endif
#endif
#if defined(TCSUPPORT_ANDLINK)
	 &cfg_type_alinkmgr,
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
	 &cfg_type_apcli,
#endif
#if defined(TCSUPPORT_NP_CMCC)
	 &cfg_type_apwaninfo,
	 &cfg_type_if6regioninfo,
#endif
#if defined(TCSUPPORT_VXLAN)
	 &cfg_type_vxlan,
#endif
	 &cfg_type_autoexec, 

#if defined(TCSUPPORT_ECNT_MAP)
	 &cfg_type_mesh,
#endif
#if defined(TCSUPPORT_CT_DSL_EX)
	 &cfg_type_wanatm,
	 &cfg_type_wanptm,
	 &cfg_type_virserveratm,
	 &cfg_type_virserverptm,
	 &cfg_type_routeatm,
	 &cfg_type_routeptm,
	 &cfg_type_route6atm,
	 &cfg_type_route6ptm,
	 &cfg_type_dmzatm,
	 &cfg_type_dmzptm,
	 &cfg_type_igmpproxyatm,
	 &cfg_type_igmpproxyptm,
	 &cfg_type_mldproxyatm,
	 &cfg_type_mldproxyptm,
	 &cfg_type_ddnsatm,
	 &cfg_type_ddnsptm,
	 &cfg_type_cwmp_dsl_diagnostic, 
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
	 &cfg_type_ipoediag,
#endif

#if defined(TCSUPPORT_CT_JOYME4)
	 &cfg_type_cloudservice,
	 &cfg_type_bridgedata_acl,
	 &cfg_type_iptv,
	 &cfg_type_cfgauth,
	 &cfg_type_downlinkqos,
	 &cfg_type_wancfgdata,
#endif
#ifdef TCSUPPORT_CUC
	 &cfg_type_alarm_info,
#endif
#if defined(TCSUPPORT_STUN)
	 &cfg_type_stun,
#endif
#if defined(TCSUPPORT_CT_UBUS)
	  &cfg_type_ctcapd,
#endif
#if defined(TCSUPPORT_CT_VRWAN)
	 &cfg_type_vrserver,
#endif

	&cfg_type_interfacerate,

	 NULL 
}; 


cfg_node_type_t cfg_type_root = { 
	 .name = "root", 
	 .flag = 1 , 
	 .nsubtype = sizeof(cfg_type_root_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_root_child, 
	 .ops = &cfg_type_root_ops, 
}; 
