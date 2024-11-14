
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
#ifndef __SVC_UTILITY_H__
#define __SVC_UTILITY_H__
#include <common/ecnt_global_macro.h>

/*length node define*/
#define CFG_BUF_8_LEN               (8)
#define CFG_BUF_16_LEN              (16)
#define CFG_BUF_32_LEN              (32)
#define CFG_BUF_64_LEN              (64)
#define CFG_BUF_128_LEN             (128)
#define CFG_BUF_256_LEN             (256)
#define CFG_BUF_512_LEN             (512)
#define CFG_BUF_1024_LEN            (1024)

#define MAXLEN_NODE_NAME			CFG_BUF_64_LEN

#define PATH_MAX_DNSMASQ 			CFG_BUF_128_LEN
#if defined(TCSUPPORT_CUC)
#define RESETSOURCEPATH				"/opt/cu/apps/info/reset_source"
#else
#define RESETSOURCEPATH				"/opt/upt/apps/info/reset_source"
#endif

#define INSMOD_IPTABLE_FILTER_CMD "taskset 2 insmod /lib/modules/%s/kernel/net/ipv4/netfilter/iptable_filter.ko &"
#define INSMOD_IP6TABLE_FILTER_CMD "taskset 2 /sbin/insmod /lib/modules/%s/kernel/net/ipv6/netfilter/ip6table_filter.ko &"
#define INSMOD_XT_LAYER7_CMD 	"taskset 2 insmod /lib/modules/%s/kernel/net/netfilter/xt_layer7.ko &"

/*message queue for cwmp*/
#define CONN_REQ 		0 		/*message queue for connection request*/
#define CWMP_CHANGED 	1	 	/*message queue for acs url change*/
#define VALUE_CHANGED 	2  		/*message queue for value change*/
#define HOST_REINIT 	3  	 	/*message queue for host information reinit*/
#define DEVICE_REINIT 	4 		/*message queue for host information reinit*/
#define DIAG_COMPLETE	5		/*message queue for diagnosis complete, then signal inform */
#define ALARM_REINIT 	6 		/*message queue for host information reinit*/
#define WAN_CHANGE		7 		/*message queue for WAN config change*/
#define TR69_BIND_SERVER  8
#define BUTTON_DETECT  9
#define CWMP_REBOOT  10
#define UPNP_DM  11
#define SIM_CARD  12
#define NAME_CHANGE  13
#define STB_CHANGE	14
#define LOID_CHANGE  15
#define IPOE_EMULATOR_OK 16
#define BRIDGE_CONNECTION_UP	17
#define ROUTE_PPPCONNECTION_UP  18
#define BIND1_REGISTER 19
#define MONITOR_COLLECTOR_SWITCH 20
#define DNS_LIMIT_ALERT 21
#define QOE_RESTART 22
#define WIFIMACTAB_REINIT 23
#define TR181_EVENT 24
#define BUNDLE_MSG_TR69	25
#define CWMP_INFORM	26
#define MAX_VIRSERV_RULE 10


/*CFG node define*/
#define GLOBALSTATE_NODE					"root.globalstate"
#define GLOBALSTATE_PRESYSSTATE_NODE  		"root.globalstate.presysstate"
#define GLOBALSTATE_PPPOEEMUTIMER_NODE  	"root.globalstate.pppoeemutimer"
#define GLOBALSTATE_TRANSFERSERVICES_NODE  	"root.globalstate.transferservices"
#define GLOBALSTATE_PINGDIAGNOSTIC_NODE		"root.globalstate.pingdiagnostic"
#define GLOBALSTATE_COMMON_NODE  			"root.globalstate.common"
#define TIMEZONE_ENTRY_NODE					"root.timezone.entry"
#define WAN_NODE							"root.wan"
#define WAN_COMMON_NODE						"root.wan.common"
#define WAN_PVC								"root.wan.pvc."
#define WAN_PVC_NODE						"root.wan.pvc.%d"
#define WAN_PVC_ENTRY_NODE					"root.wan.pvc.%d.entry.%d"
#define IPSEC_ENTRY_NODE					"root.ipsec.entry.%d"
#define IPSEC_NODE					        "root.ipsec"
#define MONITORCOLLECTOR_COMMON_NODE		"root.monitorcollector.common"
#define SYSTEM_ENTRY_NODE					"root.system.entry"
#define LAN_ENTRY_NODE						"root.lan.entry"
#define LAN_ENTRY0_NODE						"root.lan.entry.1"
#define LAN_DHCP_NODE						"root.lan.dhcp"
#define LAN_COMMON_NODE						"root.lan.common"
#define LAN_IGMPSNOOP_NODE					"root.lan.igmpsnoop"
#define LAN_BRIDGEMACLIMIT_NODE				"root.lan.bridgemaclimit"
#define LAN_BSR_NODE						"root.lan.bsr"
#define LANALIAS_ENTRY_NODE					"root.lanalias.entry.%d"
#define DHCP6S_DNSRELAY_NODE					"root.dhcp6s.dnsrelay"
#if defined(TCSUPPORT_CT_MULTI_LAN_PD)
#define RADVD_COMMON_NODE					"root.radvd.common"
#define RADVD_ENTRY_NODE					"root.radvd.entry.%d"
#define DHCP6S_COMMON_NODE					"root.dhcp6s.common"
#define DHCP6S_ENTRY_NODE					"root.dhcp6s.entry.%d"
#else
#define RADVD_ENTRY_NODE					"root.radvd.entry"
#define DHCP6S_ENTRY_NODE					"root.dhcp6s.entry"
#endif
#define IPOEDIAG_COMMON_NODE				"root.ipoediag.common"
#define IPOEDIAG_ENTRY_NODE					"root.ipoediag.entry.%d"
#define VRSERVER_COMMON_NODE				"root.vrserver.common"
#define VRSERVER_ENTRY_NODE					"root.vrserver.entry.%d"
#if defined(TCSUPPORT_CT_JOYME4)
#define CLOUDSERVICE_COMMON_NODE				"root.cloudservice.common"
#endif
#define DMS_BASIC_NODE						"root.dms.basic"
#define DMS_DEVICE_NODE						"root.dms.device"
#define DMS_ENTRY_NODE						"root.dms.entry"
#define DMZ_ENTRY_NODE						"root.dmz.entry.%d"
#define SYSLOG_ENTRY_NODE					"root.syslog.entry"
#define WEBCURSET_ENTRY_NODE				"root.webcurset.entry"
#define WEBCUSTOM_ENTRY_NODE				"root.webcustom.entry"
#define XPON_COMMON_NODE					"root.xpon.common"
#define XPON_LINKCFG_NODE					"root.xpon.linkcfg"
#define GPON_COMMON_NODE					"root.gpon.common"
#define GPON_ONU_NODE						"root.gpon.onu"
#define GPON_ANI_NODE						"root.gpon.ani"
#define GPON_OLT_NODE						"root.gpon.olt"
#define GPON_SOFTIMAGE0_NODE				"root.gpon.softimage0"
#define GPON_SOFTIMAGE1_NODE				"root.gpon.softimage1"
#define GPON_CAPABILITY_NODE				"root.gpon.capability"
#define GPON_LOIDAUTH_NODE					"root.gpon.loidauth"
#define GPON_GEMPORT_NODE					"root.gpon.gemport"
#define EPON_ONU_NODE						"root.epon.onu"
#define EPON_PON_NODE						"root.epon.pon"
#define EPON_POWERSAVING_NODE				"root.epon.powersaving"
#define EPON_LOIDAUTH_NODE					"root.epon.loidauth"
#define EPON_SOFTIMAGE_NODE					"root.epon.softimage"
#define EPON_SERVICESLA_NODE				"root.epon.servicesla"
#define OMCI_ENTRY_NODE						"root.omci.entry"
#define OAM_ENTRY_NODE						"root.oam.entry"
#define OAM_CTC_NODE						"root.oam.ctc"
#define OAM_CUC_NODE						"root.oam.cuc"
#define PORTAL_ENTRY_NODE					"root.portal.entry"
#define IPMACFILTER_NODE					"root.ipmacfilter"
#define IPMACFILTER_COMMON_NODE				"root.ipmacfilter.common"
#define IPMACFILTER_ENTRY_N_NODE			"root.ipmacfilter.entry.%d"
#define IPMACFILTER_ENTRY_NODE				"root.ipmacfilter.entry"
#define IPMACFILTER_ETHERTYPEFILTER_NODE	"root.ipmacfilter.ethertypefilter"
#define URLFILTER_NODE						"root.urlfilter"
#define URLFILTER_COMMON_NODE				"root.urlfilter.common"
#define URLFILTER_ENTRY_N_NODE				"root.urlfilter.entry.%d"
#define URLFILTER_ENTRY_NODE				"root.urlfilter.entry"
#define APPFILTER_ENTRY_NODE				"root.appfilter.entry"
#define SSLCA_NODE							"root.sslca"
#define SSLCA_ENTRY_NODE					"root.sslca.entry.%d"
#define SSLCA_FLAG_NODE						"root.sslca.flag"
#define SSLCA_COMMON_NODE					"root.sslca.common"
#define CWMP_ENTRY_NODE						"root.cwmp.entry"
#define DHCPRELAY_ENTRY_NODE 				"root.dhcprelay.entry"
#define CWMP_NODE							"root.cwmp"
#define PPPOE_EMULATOR_ENTRY_NODE			"root.pppoeemulator.entry"
#define PPPOE_SIMULATE_ENTRY_NODE			"root.pppoesimulate.entry"
#define WLAN_NODE							"root.wlan"
#define WLAN_COMMON_NODE					"root.wlan.common"
#define WLAN_RATEPRIORITY_NODE				"root.wlan.ratepriority"
#define WLAN11AC_RATEPRIORITY_NODE			"root.wlan11ac.ratepriority"
#define WLAN_ENTRY_NODE						"root.wlan.entry"
#define WLAN_ENTRY1_NODE					"root.wlan.entry.1"
#define WLAN_ENTRY2_NODE					"root.wlan.entry.2"
#define WLAN_ENTRY3_NODE					"root.wlan.entry.3"
#define WLAN_ENTRY_N_NODE					"root.wlan.entry.%d"
#define WLAN_WDS_NODE 						"root.wlan.wds"
#define WLAN_ACS_NODE 						"root.wlan.acs" 
#define WLAN_BNDSTRG_NODE					"root.wlan.Bndstrg"
#define WLAN_MONITOR_NODE					"root.wlan.MonitorSTA"
#define WLAN11AC_NODE						"root.wlan11ac"
#define WLAN11AC_COMMON_NODE				"root.wlan11ac.common"
#define WLAN11AC_ENTRY_NODE					"root.wlan11ac.entry"
#define WLAN11AC_ENTRY1_NODE				"root.wlan11ac.entry.1"
#define WLAN11AC_ENTRY2_NODE				"root.wlan11ac.entry.2"
#define WLAN11AC_ENTRY3_NODE				"root.wlan11ac.entry.3"
#define WLAN11AC_ENTRY_N_NODE				"root.wlan11ac.entry.%d"
#define WLAN11AC_WDS_NODE 					"root.wlan11ac.wds"
#define WLAN11AC_ACS_NODE 					"root.wlan11ac.acs" 
#define INFO_WLAN_COMMON_NODE					"root.info.%s"
#define BEACONVSIE_NODE 					"root.beaconvsie" 
#define BEACONVSIE_COMMON_NODE 				"root.beaconvsie.common" 
#define BEACONVSIE_ENTRY_N_NODE				"root.beaconvsie.entry.%d"
#define INFO_WLAN_NODE						"root.info.wlan"
#define INFO_WLAN11AC_NODE 					"root.info.wlan11ac"
#define INFO_MESH_NODE						"root.info.mesh"
#define INFO_RA_N_NODE 					"root.info.ra.%d"
#define INFO_RAI_N_NODE 					"root.info.rai.%d"
#define INFO_DEVDEFINF_NODE					"root.info.devdefinf"
#define INFO_ETHER_NODE						"root.info.ether"
#define MOBILE_ENTRY_NODE	 				"root.mobile.entry"
#define MOBILE_WIFIEX_NODE	 				"root.mobile.wifiex"
#define IPOE_EMULATOR_ENTRY_NODE			"root.ipoeemulator.entry"
#define CMCCINFO_ENTRY_NODE					"root.cmccinfo.entry"
#define CMCCINFO_ENTRY3_NODE				"root.cmccinfo.entry.3"
#define LANINFO_COMMON_NODE					"root.laninfo.common"
#define LANINFO_ENTRY_NODE					"root.laninfo.entry.%d"
#define SSH_ENTRY_NODE						"root.ssh.entry"
#define PARENTAL_ENTRY_NODE					"root.parental.entry"
#define PARENTAL_COMMON_NODE				"root.parental.common"
#define PARENTAL_MAC_ENTRY_NODE				"root.parentalmac.entry"
#define PARENTAL_MAC_COMMON_NODE			"root.parentalmac.common"
#define DEVICEINFO_NODE						"root.deviceinfo"
#define DEVICEINFO_DEVPARASTATIC_NODE		"root.deviceinfo.devparastatic"
#define DEVICEINFO_DEVPARADYNAMIC_NODE		"root.deviceinfo.devparadynamic"
#define SIMCARD_ENTRY_NODE					"root.simcard.entry"
#define ACCOUNT_NODE						"root.account"
#define ACCOUNT_ENTRY1_NODE					"root.account.entry.1"
#define ACCOUNT_ENTRY2_NODE					"root.account.entry.2"
#define ACCOUNT_FTPCOMMON_NODE				"root.account.ftpcommon"
#define ACCOUNT_FTPENTRY_NODE				"root.account.ftpentry"
#define ACCOUNT_FTPENTRY_N_NODE				"root.account.ftpentry%d"
#define ACCOUNT_TELNETENTRY_NODE			"root.account.telnetentry"
#define ACCOUNT_WANTELNETENTRY_NODE			"root.account.wantelnetentry"
#define ACCOUNT_TELNETENTRY_N_NODE			"root.account.telnetentry%d"
#define ACCOUNT_CONSOLEENTRY_NODE			"root.account.consoleentry"
#define ACCOUNT_ENTRY_NODE					"root.account.entry.%d"
#define GPON_ONU_NODE						"root.gpon.onu"
#define GPON_GEMPORT_NODE					"root.gpon.gemport"
#define GPON_LOIDAUTH_NODE					"root.gpon.loidauth"
#define EPON_LOIDAUTH_NODE					"root.epon.loidauth"
#define ACCESSLIMIT_COMMON_NODE				"root.accesslimit.common"
#define ACCESSLIMIT_ENTRY_NODE				"root.accesslimit.entry.%d"
#define ALGSWITCH_ENTRY_NODE				"root.algswitch.entry"
#define APPFTP_DOWNLOADING_NODE				"root.appftp.downloading"
#define APPFTP_ENTRY_NODE					"root.appftp.entry.%d"
#define AUTOEXEC_ENTRY_NODE					"root.autoexec.entry"
#define BANDWIDTH_COMMON_NODE				"root.bandwidth.common"
#define CTDIAGNOSTIC_COMMON_NODE			"root.ctdiagnostic.common"
#define CTDIAGNOSTIC_ENTRY_NODE				"root.ctdiagnostic.entry.%d"
#define WANINFO_COMMON_NODE					"root.waninfo.common"
#define WANINFO_WANIF_NODE					"root.waninfo.wanif"
#define WANINFO_WANPVC_NODE					"root.waninfo.wanpvc"
#define WANINFO_INTERNET_NODE				"root.waninfo.internet"
#define WANINFO_ENTRY_NODE					"root.waninfo.entry.%d"
#define CWMPROUTE_ENTRY_NODE				"root.cwmproute.entry"
#define DATASPEEDLIMIT_ENTRY_NODE			"root.dataspeedlimit.entry"
#define QOS_COMMON_NODE						"root.qos.common"
#define QOS_ENTRY_NODE						"root.qos.entry.%d"
#define DOWNLINKQOS_NODE					"root.downlinkqos"
#define DOWNLINKQOS_COMMON_NODE				"root.downlinkqos.common"
#define DOWNLINKQOS_ENTRY_NODE				"root.downlinkqos.entry.%d"
#define DEVICEACCOUNT_ENTRY_NODE			"root.deviceaccount.entry"
#define DEVICEALARM_COMMON_NODE				"root.devicealarm.common"
#define DEVICEALARM_ENTRY_NODE				"root.devicealarm.entry.%d"
#define SYSINFO_ENTRY_NODE					"root.sysinfo.entry"
#define DEVICEMONITOR_COMMON_NODE			"root.devicemonitor.common"
#define DEVICEMONITOR_ENTRY_NODE			"root.devicemonitor.entry.%d"
#define DHCPCLIENT_INFO_NODE				"root.dhcpclient.info"
#define DHCPD_NODE							"root.dhcpd"
#define DHCPD_COMMON_NODE					"root.dhcpd.common"
#define DHCPD_ENTRYNODE						"root.dhcpd.entry"
#define DHCPD_ENTRY_NODE					"root.dhcpd.entry.%d"
#define DHCPD_OPTION60_NODE					"root.dhcpd.option60"
#define DHCPD_OPTION240_NODE				"root.dhcpd.option240"
#define DHCPLEASE_NODE						"root.dhcplease"
#define DHCPLEASE_ENTRY_NODE				"root.dhcplease.entry.%d"
#define DIAGNOSTIC_NODE						"root.diagnostic"
#define DIAGNOSTIC_PVC_NODE					"root.diagnostic.pvc.%d"
#define DPROXY_ENTRY_NODE					"root.dproxy.entry"
#define DYNCWMPATTR_ENTRY_NODE				"root.dyncwmpattr.entry"
#define TR069ATTR_ENTRY_NODE				"root.tr069attr.entry"
#define DYNDISP_ENTRY_NODE					"root.dyndisp.entry"
#define FIREWALL_NODE						"root.firewall"
#define FIREWALL_ENTRY_NODE					"root.firewall.entry"
#define FIREWALL_DOSENTRY_NODE				"root.firewall.dosentry"
#define FIREWALL_PORTSCAN_NODE				"root.firewall.portscanentry"
#define FIREWALL_IPV6SESSION_NODE			"root.firewall.ipv6sessionentry"
#define GUESTSSIDINFO_NODE					"root.guestssidinfo"
#define GUESTSSIDINFO_ENTRY_NODE			"root.guestssidinfo.entry.%d"
#define HOSTNAME_COMMON_NODE				"root.hostname.common"
#define HOSTNAME_ENTRY_NODE					"root.hostname.entry.%d"
#define VOIPBASIC_COMMON_NODE				"root.voipbasic.common"
#define VOIPBASIC_ENTRY_NODE				"root.voipbasic.entry.%d"
#define INFOVOIP_COMMON_NODE				"root.infovoip.common"
#define INFOVOIP_ENTRY_NODE					"root.infovoip.entry.%d"
#define INFOVOIPH248_COMMON_NODE			"root.infovoiph248.common"
#define INFOVOIPH248_ENTRY_NODE				"root.infovoiph248.entry.%d"
#define VOIPH248_NODE						"root.voiph248"
#define VOIPH248_COMMON_NODE				"root.voiph248.common"
#define VOIPH248_ENTRY_NODE					"root.voiph248.entry.%d"
#define INFOVOIPPOORQL_PVC_NODE				"root.infovoippoorql.pvc.%d"
#define INFOVOIPPOORQL_PVC_ENTRY_NODE		"root.infovoippoorql.pvc.%d.entry.%d"
#define VOIPSYSPARAM_NODE					"root.voipsysparam"
#define VOIPSYSPARAM_COMMON_NODE			"root.voipsysparam.common"
#define VOIPSYSPARAM_ENTRY_NODE				"root.voipsysparam.entry.%d"
#define VOIPTEST_ENTRY_NODE					"root.voiptest.entry.%d"
#define VOIPDIAGNOSTIC_ENTRY_NODE			"root.voipdiagnostic.entry"
#define VOIPMONITOR_PVC_NODE				"root.voipmonitor.pvc.%d"
#define VOIPMONITOR_PVC_ENTRY_NODE			"root.voipmonitor.pvc.%d.entry.%d"
#define VOIPADVANCED_NODE					"root.voipadvanced"
#define VOIPADVANCED_COMMON_NODE			"root.voipadvanced.common"
#define VOIPSIMULATETEST_ENTRY_NODE			"root.voipsimulatetest.entry.%d"
#define LANHOST_NODE						"root.lanhost"
#define LANHOST_ENTRY_NODE					"root.lanhost.entry.%d"
#define LANHOST2_ENTRY_NODE					"root.lanhost2.entry.%d"
#define LANHOST2_COMMON_NODE				"root.lanhost2.common"
#define LANHOST2BLACK_ENTRY_NODE			"root.lanhost2black.entry.%d"
#define LANHOST2BLACK_COMMON_NODE			"root.lanhost2black.common"
#define WIFIMACTAB_COMMON_NODE				"root.wifimactab.common"
#define WIFIMACTAB_PVC_NODE					"root.wifimactab.pvc.%d"
#define WIFIMACTAB_PVC_ENTRY_NODE			"root.wifimactab.pvc.%d.entry.%d"
#define WIFIMACTAB_ENTRY_NODE				"root.wifimactab.entry.%d"
#define LOGICID_ENTRY_NODE					"root.logicid.entry"
#define LOOPDETECT_ENTRY_NODE				"root.loopdetect.entry"
#define OSGIUPGRADE_ENTRY_NODE				"root.osgiupgrade.entry"
#define PLUGIN_COMMON_NODE					"root.plugin.common"
#define PLUGIN_ENTRY_NODE					"root.plugin.entry.%d"
#define PMINFORM_ENTRY_NODE					"root.pminform.entry"
#define ROUTE_ENTRY_NODE					"root.route.entry.%d"
#define ROUTE6_ENTRY_NODE					"root.route6.entry.%d"
#define SWITCHPARA_COMMON_NODE				"root.switchpara.common"
#define SWITCHPARA_ENTRY_NODE				"root.switchpara.entry.%d"
#define SYS_ENTRY_NODE						"root.sys.entry"
#define INFO_PONPHY_NODE					"root.info.ponphy"
#define TRAFFICFORWARD_COMMON_NODE			"root.trafficforward.common"
#define TRAFFICFORWARD_ENTRY_NODE			"root.trafficforward.entry.%d"
#define TRAFFICQOSSERVICE_PVC_NODE			"root.trafficqosservice.pvc.%d"
#define TRAFFICQOSSERVICE_PVC_ENTRY_NODE	"root.trafficqosservice.pvc.%d.entry.%d"
#define USBMOUNT_COMMON_NODE				"root.usbmount.common"
#define USBMOUNT_ENTRY_NODE					"root.usbmount.entry.%d"
#define USBRESTORE_ENTRY_NODE				"root.usbrestore.entry"
#define VIRSERVER_NODE						"root.virserver"
#define VIRSERVER_ENTRY_NODE				"root.virserver.entry.%d"
#define VIRSERVER_ENTRY_ENTRY_NODE			"root.virserver.entry.%d.entry.%d"
#define VPNDESTLIST_PVC_NODE				"root.vpndestlist.pvc.%d"
#define VPNDESTLIST_PVC_ENTRY_NODE			"root.vpndestlist.pvc.%d.entry.%d"
#define VPN_COMMON_NODE						"root.vpn.common"
#define VPN_ENTRY0_NODE						"root.vpn.entry.0"
#define VPN_ENTRY_NODE						"root.vpn.entry.%d"
#define VPNDOMAIN_ENTRY_NODE				"root.vpndomain.entry.%d"
#define VPNIPS_ENTRY_NODE					"root.vpnips.entry.%d"
#define VPNMAC_ENTRY_NODE					"root.vpnmac.entry.%d"
#define WLANSHARE_NODE						"root.wlanshare"
#define WLANSHARE_ENTRY_NODE				"root.wlanshare.entry.%d"
#define APIPERMISSION_ENTRY_NODE			"root.apipermission.entry.%d"
#define VLANBIND_NODE						"root.vlanbind"
#define VLANBIND_ENTRY_NODE					"root.vlanbind.entry.%d"
#define TRAFFICMONITOR_NODE					"root.trafficmonitor"
#define TRAFFICMONITOR_COMMON_NODE			"root.trafficmonitor.common"
#define TRAFFICMONITOR_PVC_NODE				"root.trafficmonitor.pvc.%d"
#define TRAFFICMONITOR_PVC_ENTRY_NODE		"root.trafficmonitor.pvc.%d.entry.%d"
#define TRAFFICMIRROR_NODE					"root.trafficmirror"
#define TRAFFICMIRROR_ENTRY_NODE			"root.trafficmirror.entry.%d"
#define TRAFFICDETAILPROCESS_NODE			"root.trafficdetailprocess"
#define TRAFFICDETAILPROCESS_COMMON_NODE	"root.trafficdetailprocess.common"
#define TRAFFICDETAILPROCESS_ENTRY_NODE		"root.trafficdetailprocess.entry.%d"
#define UPNPD_ENTRY_NODE					"root.upnpd.entry"
#define DMS_NODE							"root.dms"
#define IGMPPROXY_NODE						"root.igmpproxy"
#define IGMPPROXY_ENTRY_NODE				"root.igmpproxy.entry"
#define MLDPROXY_ENTRY_NODE					"root.mldproxy.entry"
#define SAMBA_COMMON_NODE					"root.samba.common"
#define SAMBA_ENTRY_NODE					"root.samba.entry.%d"
#define SNMPD_ENTRY_NODE					"root.snmpd.entry"
#define ACL_NODE							"root.acl"
#define ACL_COMMON_NODE						"root.acl.common"
#define ACL_ENTRY_NODE						"root.acl.entry"
#define ACL_ENTRY_N_NODE						"root.acl.entry.%d"
#define DDNS_COMMON_NODE					"root.ddns.common"
#define DDNS_ENTRY_NODE						"root.ddns.entry.%d"
#define DDNS_ENTRY_1_NODE					"root.ddns.entry.1"
#define DDNS_TR69ENTRY_NODE					"root.ddns.tr69entry"
#define CUCPING_ENTRY_NODE					"root.cucping.entry"
#define PORTTRIGGERING_NODE					"root.porttriggering"
#define PORTTRIGGERING_ENTRY_NODE			"root.porttriggering.entry.%d"
#define PORTTRIGGERING_SETTING_NODE			"root.porttriggering.setting"
#define GUITEMP_NODE                        "root.guitemp"
#define GUITEMP_ENTRY_NODE                  "root.guitemp.entry.%d"
#define STRING_COMMON_NODE					"root.string.common"
#define STRING_ENTRY_NODE					"root.string.entry"
#define LANGUAGESWITCH_ENTRY_NODE		"root.languageswitch.entry"
#define MACBLACKLIST_ENTRY_NODE				"root.macblacklist.entry.%d"
#define MACWHITELIST_ENTRY_NODE				"root.macwhitelist.entry.%d"
#define MACBLACKLIST_NODE					"root.macblacklist.entry"
#define MACBLACKLIST_COMMON_NODE			"root.macblacklist.common"
#define MACWHITELIST_NODE					"root.macwhitelist.entry"
#define MACWHITELIST_COMMON_NODE			"root.macwhitelist.common"
#define DNSSERVER_SERVERV4_NODE				"root.dnsserver.serverv4"
#define DNSSERVER_SERVERV6_NODE				"root.dnsserver.serverv6"
#define WLAN_ROAM11K_NODE					"root.wlan.roam11k"
#define WLAN_ROAM11V_NODE					"root.wlan.roam11v"
#define WLAN_ROAM_NODE						"root.wlan.roam"
#define WLAN_ROAM11AC_NODE				"root.wlan11ac.roam"
#define WLAN11AC_ROAM11K_NODE				"root.wlan11ac.roam11k"
#define WLAN11AC_ROAM11V_NODE				"root.wlan11ac.roam11v"
#define PROBERXVSIE_COMMON_NODE             "root.proberxvsie.common"
#define PROBERXVSIE_ENTRY_NODE              "root.proberxvsie.entry.%d"
#define PROBERESPVSIE_COMMON_NODE           "root.proberespvsie.common"
#define PROBERESPVSIE_INTERFACE             "root.proberespvsie.entry."
#define PROBERESPVSIE_ENTRY_NODE            "root.proberespvsie.entry.%d"
#define WIFIPROBEINFO_ENTRY_NODE            "root.wifiprobeinfo.entry.%d"
#define TIMER_ENTRY_NODE					"root.timer.entry.%d"
#define TIMER_COMMON_NODE					"root.timer.common"
#define TIMER_LEDTIMER_ENTRY_NODE			"root.timer.ledentry"
#define TIMER_WIFITIMER_ENTRY_NODE			"root.timer.wifitimerentry"
#define INCOMINGFILTER_ENTRY_NODE			"root.incomingfilter.entry.%d"
#define DNSFILTER_ENTRY_N_NODE				"root.dnsfilter.entry.%d"
#define DNSFILTER_COMON_NODE				"root.dnsfilter.common"
#define DNSFILTER_NODE						"root.dnsfilter"
#define WLANMAPPING_ENTRY_N_NODE			"root.wlanmapping.entry.%d"
#define SPEEDTEST_COMMON_NODE				"root.speedtest.common"
#define SPEEDTEST_ENTRY_NODE				"root.speedtest.entry.%d"
#define QOSATTR_WEIGHTENTRY_NODE			"root.qosattr.weightentry"
#define QOSATTR_CARENTRY_NODE				"root.qosattr.carentry"
#define MULTIVLAN_COMMON_NODE               "root.multivlan.common"
#define MULTIVLAN_ENTRY_NODE               "root.multivlan.entry.%d"
#define MULTIVLAN_ENTRY_DHCPD_NODE      "root.multivlan.entry.%d.dhcpd"
#define MULTILAN_COMMON_NODE               "root.multilan.common"
#define MULTILAN_ENTRY_NODE               "root.multilan.entry.%d" 
#define SDN_COMMON_NODE               "root.sdn.common"
#define STUN_ENTRY_NODE               "root.stun.entry"
#define STORAGEACCESSRIGHT_ENTRY_NODE      "root.storageaccessright.entry.%d"
#define ACCESSCONTROL_ENTRY_NODE      "root.accesscontrol.entry.%d"
#define WANINFO_BRENTRY_NODE				"root.waninfo.brentry"
#define VXLAN_COMMON_NODE					"root.vxlan.common"
#define VXLAN_ENTRY_NODE					"root.vxlan.entry.%d"
#define VXLAN_IPENTRY_NODE					"root.vxlan.ipentry.%d"
/**************************mesh start***********************************/
#define MESH_NODE                                   "root.mesh"
#define MESH_DAT_NODE                               "root.mesh.dat"
#define MESH_COMMON_NODE                            "root.mesh.common"
#define MESH_MAP_CFG_NODE                           "root.mesh.mapcfg"
#define MESH_MAPD_CFG_NODE                          "root.mesh.mapdcfg"
#define MESH_STEER_CFG_NODE                         "root.mesh.steercfg"
#define MESH_RADIO2G_BSSINFO_NODE             		"root.mesh.radio2gbssinfo"
#define MESH_RADIO2G_BSSINFO_ENTRY_NODE_FROMAT      ""MESH_RADIO2G_BSSINFO_NODE".entry."
#define MESH_RADIO2G_BSSINFO_ENTRY_NODE             ""MESH_RADIO2G_BSSINFO_ENTRY_NODE_FROMAT"%d"
#define MESH_RADIO5GL_BSSINFO_NODE					"root.mesh.radio5glbssinfo"
#define MESH_RADIO5GL_BSSINFO_ENTRY_NODE_FORMAT     ""MESH_RADIO5GL_BSSINFO_NODE".entry."
#define MESH_RADIO5GL_BSSINFO_ENTRY_NODE            ""MESH_RADIO5GL_BSSINFO_ENTRY_NODE_FORMAT"%d"
#define MESH_RADIO5GH_BSSINFO_NODE            		"root.mesh.radio5ghbssinfo"
#define MESH_RADIO5GH_BSSINFO_ENTRY_NODE_FORMAT     ""MESH_RADIO5GH_BSSINFO_NODE".entry."
#define MESH_RADIO5GH_BSSINFO_ENTRY_NODE            ""MESH_RADIO5GH_BSSINFO_ENTRY_NODE_FORMAT"%d"
#define MESH_ACTION_NODE                            "root.mesh.action"
#define MESH_ACTION_PARA_NODE                       "root.mesh.actionpara"
#define MESH_DEFSETTING_NODE  						"root.mesh.defsetting"
#define MESH_APCLIBH_NODE							"root.mesh.apclibh"
#define MESH_APCLIBH_ENTRY_NODE						""MESH_APCLIBH_NODE".entry.%d"
#define MESH_ENABLE_CONF_NODE                      	"root.mesh.enableconf"
#define MESH_APCLIBH_ENTRY1_NODE					"root.mesh.apclibh.entry.1"
#define MESH_APCLIBH_ENTRY2_NODE					"root.mesh.apclibh.entry.2"
#define MAP_BSSINFO_FRONT_HAUL_ATTR      			"FrontHaul"
#define MAP_BSSINFO_BACK_HAUL_ATTR       			"BackHaul"
#if defined(TCSUPPORT_MAP_R2)
#define MAP_RESTART_FLAG							"RestartMeshFlag"
#endif

/**************************mesh end***********************************/
#if defined(TCSUPPORT_CT_JOYME4)
#define BRIDGEDATAACL_ENTRY_NODE			"root.bridgedataacl.entry.%d"
#define WANCFGDATA_ENTRY_NODE				"root.wancfgdata.entry.%d"
#endif
#define APWANINFO_NODE						"root.apwaninfo"
#define APWANINFO_COMMON_NODE				"root.apwaninfo.common"
#define APWANINFO_ENTRY_NODE				"root.apwaninfo.entry.1"
#define ALINKMGR_ENTRY_NODE					"root.alinkmgr.entry"
#define IF6REGIONINFO_COMMON_NODE					"root.if6regioninfo.common"
#define IF6REGIONINFO_ENTRY_NODE					"root.if6regioninfo.entry.%d"
#if defined(TCSUPPORT_WLAN_APCLIENT)
#define APCLI_COMMON_NODE					"root.apcli.common"
#define APCLI_ENTRY0_NODE					"root.apcli.entry.1"
#define APCLI_ENTRY1_NODE					"root.apcli.entry.2"
#define INFO_APCLI0_NODE					"root.info.apcli0"
#define INFO_APCLII0_NODE					"root.info.apclii0"
#endif
#define WLAN_ROAM11V_NODE					"root.wlan.roam11v"
#define WLAN_ROAM_NODE						"root.wlan.roam"
#define WLAN_ACS_NODE 						"root.wlan.acs" 
#define WLAN11AC_ACS_NODE 					"root.wlan11ac.acs"
#define WLAN_ROAM11AC_NODE				"root.wlan11ac.roam"
/* tag for new node add */

/* define node macro for default para and key para */
#define SYSTEM_WLAN_ENTRY0_NODE				"root.WLan.Entry0"
#define SYSTEM_ACCOUNT_ENTRY0_NODE			"root.Account.Entry0"
#define SYSTEM_ACCOUNT_ENTRY1_NODE			"root.Account.Entry1"
#define SYSTEM_ACCOUNT_TELNETENTRY_NODE		"root.Account.Telnetentry"
#define SYSTEM_VIRSERVER_NODE				"root.VirServer"
#define SYSTEM_WAN_NODE						"root.Wan"
#define SYSTEM_CTDIAGNOSTIC_NODE			"root.CtDiagnostic"
#define SYSTEM_DEVICEACCOUNT				"root.deviceAccount"

#define MAXBANDWIDTH_ENTRY_NODE				"root.MaxBandWidth.entry.%d"

#define ADSL_ENTRY_NODE						"root.adsl.entry"
#define WANATM_NODE							"root.wanatm"
#define WANPTM_NODE							"root.wanptm"
#define CWMPDSLDIAGNOSTIC_COMMON_NODE       "root.cdsldiagnostic.common"
#define IPTV_ENTRY_NODE						"root.IPTV.Entry"
#define IPTV_COMMON_NODE					"root.IPTV.common"

/*filter bit map*/
#define WAN_VECTOR (1<<0)
#define FIREWALL_VECTOR (1<<1)
#define ACL_VECTOR (1<<2)
#define IPFILTER_VECTOR (1<<3)
#define APP_VECTOR (1<<4)
#define URL_VECTOR (1<<5)
#define ALGSW_VECTOR (1<<6)
#define QOS_VECTOR	(1<<7)
#define PARENTAL_VECTOR	(1<<8)
#define L7_BIT_MASK (APP_VECTOR|URL_VECTOR)

#undef MAX_WAN_ENTRY_NUMBER
#define MAX_WAN_ENTRY_NUMBER		8
#define COUNT_WIFITIMER_ITMS 8

#if defined(TCSUPPORT_CT_JOYME2)
#define MAX_INCOMFILTERRULES_NUM	16
#endif

#define ROUTE_PATH 			"/etc/route.sh"
#define ROUTE2_PATH 		"/etc/route2.sh"
#define ROUTE_EXE_SH 		"/etc/route_exe.sh"
#define ROUTE_TMP_PATH		"/tmp/route"
#define ROUTE_EXE_PRE_SH 	"/etc/route_exe_pre.sh"
#define ROUTE_EXE_AFT_SH 	"/etc/route_exe_aft.sh"
#define MAXSIZE	160
#define MAXGET_PROFILE_SIZE 128
#define FAIL -1
#define SUCCESS 0

#define FLUSH_DEFAULT_CHAIN "/usr/script/filter_forward_stop.sh"
#define LOAD_DEFAULT_CHAIN "/usr/script/filter_forward_start.sh"

#if 0
#define NO_QMARKS 		0
#define QMARKS 			1
#define NO_ATTRIBUTE 	2
#endif
#define CFG_ATTR_SIZE   32

#define WAN_ACTIVE			"Active"
#define WAN_LINKMODE		"LinkMode"
#define WAN_ISP				"ISP"

#define 		MAX_TYPE_RULE_NUM	10
#define 		MAX_APP_RULE_NUM		4
#define 		MAX_TYPE_NUM			10

#if defined(TCSUPPORT_CT_JOYME4)
#define MAX_IPUP_FILTER_RULE	40
#else
#define	MAX_IPUP_FILTER_RULE 100
#endif
#if defined(TCSUPPORT_ANDLINK)
#define	MAX_MAC_FILTER_RULE	40
#else
#define MAX_MAC_FILTER_RULE		100
#endif

#define MAX_IPMACFILTER_RULE (2 * MAX_MAC_FILTER_RULE + MAX_MAC_FILTER_RULE)

#if !defined(TCSUPPORT_MULTI_USER_ITF) && !defined(TCSUPPORT_MULTI_SWITCH_EXT)
#if defined(TCSUPPORT_WLAN_AC)
#define MAX_LAN_PORT_NUM    14
#else
#define MAX_LAN_PORT_NUM    10
#endif
#endif
#define MAX_PVC_NUM			8
#define MAX_PVC_ENTRY		8

#define CFG2_RET_SUCCESS 	0
#define CFG2_RET_FAIL		-1

#if defined(TCSUPPORT_MULTI_USER_ITF)
#define WLAN_MAPPING_NODE_LEN 16
#define WLAN_2_4_G_NODE_LEN 8
#else
#define WLAN_MAPPING_NODE_LEN 8
#define WLAN_2_4_G_NODE_LEN 4
#endif
#define WLAN_5_G_NODE_LEN WLAN_2_4_G_NODE_LEN


#define BOOT2CURSET				1
#define BOOT2DEFSET				2
#define WARM_REBOOT				3
#define	LONGRESET_BOOT2DEFSET	5
#define	BOOT2DEFSETITMS 		6
#define BOOT2PUREFACTORY 		7
#if defined(TCSUPPORT_CUC)
#define	CUC_LONGRESET_BOOT2DEFSET 8
#endif
#if defined(TCSUPPORT_CT_JOYME2)			
#define	JOYME2_RESTORE_NOT_KEYPARAM 9
#endif
#if defined(TCSUPPORT_CUC)	
#define DBUS_RESTORE_KEYPARAM 10
#endif


#define MAC_STR_LEN				12
#define	MAC_STR_DOT_LEN			17

#define MAX_INFO_SIZE		128
#define TRTCM_MATCH_MAC_RULE_NUM 10 

#define needs_escape(c)((c)=='`')

#define	DEL_URL		1
#define	DEL_IP		2
#define	DEL_ACL	3
#define	DEL_DHCPD	4
#define WLAN_MODE		0
#define WLAN11AC_MODE	1
#define MAX_VLAN_NUM    64

#if defined(TCSUPPORT_CT_JOYME4)
#define MAX_BRIDGEDATAACL_NUM	16
#endif

enum wanIFType_en{
	Ip_type=0,
	Netmask_type,
	Pridns_type,
	Secdns_type, 
	Mac_type,
	PTP_type,	
};

enum _xpon_link_state_
{
    E_XPON_LINK_DOWN    = 0, 
    E_XPON_LINK_UP      = 1, 
};
#define TMP_IF_PATH         "/tmp/if.conf"
#define DNS_ATTR	"nameserver"
#define DNS_INFO_PATH	"/etc/resolv.conf"

#ifdef TCSUPPORT_VPN
#define MAX_IPSEC_NUM 8
#endif

#ifndef OID_GET_SET_TOGGLE
#define	OID_GET_SET_TOGGLE		0x8000
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
#define SHARE_TYPE 1
#define GUEST_TYPE 2
#define GUESTSSIDINFO_NUM 2
#define WLANSHARE_INSTANCE_NUM 1
#define GSSIDINDEX 	"SSIDIndex"
int checkconflictssid(int type, int ssid);
#endif

#if defined(TCSUPPORT_NP_CMCC)
#define NP_WAN_IF			"nas0_0"
#define NP_AP_B_WAN_IF		"br0:1"

#define NP_BR_ALINK_IF		"nas_br"
#define NP_WLAN_IF			"apcli0"
#define NP_WLANAC_IF		"apclii0"
#define NP_BR0_IF			"br0"
#define NP_AP_BR0_IF		"br0:1"
#define NP_AP_R_WAN_IF		"nas0_0"
#define NP_GW_IP_DEFAULT	"192.168.1.1"
#define NP_ROUTE_WAN_PATH	"root.wan.pvc.1.entry.1"
#if defined(TCSUPPORT_WLAN_APCLIENT)
#define NP_APCLI_DISCONNECTED (0)
#define NP_APCLI_24G_CONNECTED (1)
#define NP_APCLI_5G_CONNECTED (2)
#endif
#endif

#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
#define LOGICID_REG_STATUS    	"registerStatus"
#define LOGICID_REG_RESULT  	"registerResult"
#define LOGICID_USERNAME		"Username"
#define LOGICID_PSW				"Password"

#define DEVACCOUNT_STATUS    "registerStatus"
#define DEVACCOUNT_RESULT  "registerResult"
#define DEVACCOUNT_USERNAME    "userName"
#define DEVACCOUNT_USERPASSWD    "userPasswordDEV"
#define DEVACCOUNT_WEBPAGELOCK  "webpageLock"

#define EPON_LOID_USR    "LOID0"
#define GPON_LOID_USR    "LOID"
#define EPON_LOID_PSW    "Password0"
#define GPON_LOID_PSW    "Password"
#endif
#endif

#if defined(TCSUPPORT_CT_UBUS)
#define WAN_ITF_BR0_1	"br0"
#define ETHERNET_ITF	"nas10"
#define APCLI_ITF_24G	"apcli0"
#define APCLI_ITF_5G	"apclii0"
#define NP_AP_B_WAN_IF	WAN_ITF_BR0_1
#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
#define NP_APCLI_DISCONNECTED (0)
#define NP_APCLI_24G_CONNECTED (1)
#define NP_APCLI_5G_CONNECTED (2)
#endif

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
#define MAX_LANPORT_NUM	8
#else
#define MAX_LANPORT_NUM	4
#endif

/* QOS */
typedef enum {
	T_START = 0,
#if defined(TCSUPPORT_CWMP)
    	T_TR069,
#endif
#if defined(TCSUPPORT_VOIP)
	T_VOIP,
#endif
	T_IPTV, /* OTHER type wan. */
#if defined(TCSUPPORT_CT_JOYME4)
	T_SPECIAL_SERVICE,
#endif
	T_INTERNET,
	T_MAX
}QOS_TEMPLATE;

#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
#ifndef PQ_QUEUE_NUM
#define 	PQ_QUEUE_NUM			8
#endif
#ifndef WRR_QUEUE_NUM
#define 	WRR_QUEUE_NUM			8
#endif
#ifndef CAR_QUEUE_NUM
#define 	CAR_QUEUE_NUM			8
#endif
#ifndef MAX_QUEUE_NUM
#define 	MAX_QUEUE_NUM			8
#endif
#else
#ifndef PQ_QUEUE_NUM
#define 	PQ_QUEUE_NUM			4
#endif
#ifndef WRR_QUEUE_NUM
#define 	WRR_QUEUE_NUM			4
#endif
#ifndef CAR_QUEUE_NUM
#define 	CAR_QUEUE_NUM			6
#endif
#ifndef MAX_QUEUE_NUM
#define 	MAX_QUEUE_NUM			6
#endif
#endif

#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
#define 	QUEUE_NUM			12
#else
#define 	QUEUE_NUM			4
#endif
/* QOS END */

#if defined(TCSUPPORT_CMCCV2)
#define VPN_INSTANCE_NUM	1
#else
#define VPN_INSTANCE_NUM	16
#endif
#define MAX_IPOE_DIAG_NUM 8

#define GET_BIT(x,y)	((x)>>(y) & 1)
#define SET_BIT(x,y) ((x) |= (1 << (y)))
#define CLR_BIT(x,y) ((x) &= ~(1 << (y)))

int DelAlarmNumber(char * Num);
int AddAlarmNumber(char * Num);
int check_wlan_lan_status(int type);
int isNumber(char *buf);
char* itoa(register int i);
char *string_FirstCharToUp(char *string);
char *string_tolower(char *string);
char *string_toUper(char *string);
char *escape_special_character(char *inp, char *outp, int len);
void fputs_escape(char *cmd, FILE *fp);
int system_escape(char *cmd);
#if defined(TCSUPPORT_ECN_MEGACO)
int voipCmdSend(char * SendData); 
#endif
int get_entry_number_cfg2(char *buffer, char *keyword, int *number);
void fileRead(char *path, char *buf, int size);
void kill_process(char *pid_path);
int sendEventMessage(int state, char *mac,int port);
void specialCharacterHandle(char *oriStr);
int tcgetProLinePara(void*buf,int flag);
int cutPath(char* path, char cutStr[][32]);
int get_profile_str(char *keyname,char *str_return, int size, int type, char *path);
int get_profile_str_new(char *keyname,char *str_return, int size,char *path);
int write2file(char *buf, char *pathname);
int append2file(char *path, char *led_st);
void decideModulePath(char* vername, int len);
int check_and_set_filter(unsigned int new_filter_state);
int get_wanindex_by_name(char *wan_if_name);
int get_waninfo_by_index(int if_index, char *attribute, char *reval, int size);
int find_pid_by_name( char *ProcName, int *foundpid, int *foundnum);
void shrink_ipv6addr(char *in_ipv6addr, char *out_ipv6addr, int out_len);
int doValPut(char *path, char *value);
int get_file_string(char *file_path, char line_return[][MAX_INFO_SIZE], int line_num);
int getPonLanStatus2(int portStatus[]);
int get_all_wan_index(int* buf,int len);
void splitName(char *name, char buf[][32], const char* delim);
int check_mask_format(char *mask_arg, char *mask_decimal,int len);
int check_mac_format(char *mac_arg);
int mac_rm_dot(char *old_mac, char *new_mac);
int check_ip_format(char *ip_arg);
int check_ip_range(char *ip_begin_arg, char *ip_end_arg);
int get_waninfo_by_name(char *wan_if_name, char *attribute, char *reval);
int unset_action(char *pstr, int itype);
char* getWanInfo(int type, char* device, char* buf);
int getLanPortInfo(char *node);

#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
int is_ethernet_link_up();
int is_ActiveEtherWan_link_up();
#endif
#if defined(TCSUPPORT_CT_IPOE_DETECT)
int svc_other_handle_event_ipoe_update(int index);
#endif
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
int saveLoid();
#endif
#endif
int cutString(char *in, int inlen, char *out, int outlen, char *cut, int flag);
int addString(char *in, int inlen, char *out, int outlen, char *add, int flag);
int parseUrlHostInfo(char *pUrl, char *pHost, int hostlen, unsigned short *pPort);

int mac_rm_dot(char *old_mac, char *new_mac);
#if defined(TCSUPPORT_WLAN_APCLIENT)
int np_wlan_apcli_connstatus();
int np_wlan_apcli_get_sitesurvey(int type, char *SSID, char *channel, char *RSSI, char *Signal);
int np_wlan_set_wps_profile();
#endif
#if defined(TCSUPPORT_ECNT_MAP)
int check_mesh_enable();
int check_mesh_role();
#endif
int getBhfhParam(const char *entryNode);
int check_injection_code_command(char *cmd, int cmd_len, int type);
void ctc_dbg_msg (const char *format, ...);
void fputs_escape_normal_file(char *cmd, FILE *fp);
char *escape_param(char *chk_val, char *out_val, int out_val_len);

#define NEED_C(x) ( ('`' == (x)) || ('"' == (x)) || ('\\' == (x)) || ('$' == (x)) )

#define F_PUTS_NORMAL fputs_escape_normal_file
#define F_PUTS_SHELL fputs_escape
#define ESCAPE_VAL	escape_param

#endif
void get_current_time(char *time_buf, int len);
void get_current_only_time(char *time_buf, int len);
void get_current_tz(int *flag, int *tz_hour, int *tz_minute);
int get_inform_time(char *inform_time, int len);

int cfg_type_virServer_entry_entry_func_delete_core(char* path);

char is_xpon_traffic_up(void);
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
void get_current_DiagTZ(signed char *flag, unsigned char *tz_hour, unsigned char *tz_minute);
void SetDiagTz(void);
#endif

int checkCFGload(void);

int get_br0_ifaddr(int sock, char *mac);
int get_br0_v6addr(unsigned char *ifid);
