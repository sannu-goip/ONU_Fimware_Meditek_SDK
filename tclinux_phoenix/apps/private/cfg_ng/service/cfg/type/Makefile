generated_c_objects = cfg_type_root.o 
ifneq ($(strip $(TCSUPPORT_CT_PON)),)
ifeq ($(strip $(TCSUPPORT_NP)),) 
generated_c_objects += cfg_type_epon.o 
generated_c_objects += cfg_type_oam.o 
generated_c_objects += cfg_type_gpon.o 
generated_c_objects += cfg_type_pm.o 
generated_c_objects += cfg_type_omci.o 
endif
generated_c_objects += cfg_type_xpon.o 
endif
generated_c_objects += cfg_type_sysinfo.o 
ifneq ($(strip $(TCSUPPORT_CT_IPV4_RADIO)),)
generated_c_objects += cfg_type_sys.o 
endif
generated_c_objects += cfg_type_lan.o 
generated_c_objects += cfg_type_ProbeVSIE.o
generated_c_objects += cfg_type_wan.o 
generated_c_objects += cfg_type_waninfo.o 
ifneq ($(strip $(TCSUPPORT_IPV6)),)
generated_c_objects += cfg_type_radvd.o 
generated_c_objects += cfg_type_dhcp6s.o 
endif
generated_c_objects += cfg_type_dhcpd.o 
generated_c_objects += cfg_type_dhcprelay.o 
generated_c_objects += cfg_type_dhcplease.o 
generated_c_objects += cfg_type_lanhost.o
generated_c_objects += cfg_type_lanhost2.o
generated_c_objects += cfg_type_lanhost2black.o 
ifneq ($(strip $(TCSUPPORT_CT_L2TP_VPN)),)
generated_c_objects += cfg_type_vpnlist.o 
endif
ifneq ($(strip $(TCSUPPORT_WLAN)),)
generated_c_objects += cfg_type_wlan.o 
generated_c_objects += cfg_type_wifimactab.o 
generated_c_objects += cfg_type_wifiprobeinfo.o 
ifneq ($(strip $(TCSUPPORT_WLAN_VENDIE)),)
generated_c_objects += cfg_type_BeaconVSIE.o
generated_c_objects += cfg_type_proberesp.o
endif
ifneq ($(strip $(TCSUPPORT_WLAN_AC)),)
generated_c_objects += cfg_type_wlan11ac.o 
endif
endif
generated_c_objects += cfg_type_firewall.o 
ifneq ($(strip $(TCSUPPORT_VPN)),)
generated_c_objects += cfg_type_ipsec.o 
endif
generated_c_objects += cfg_type_route.o 
generated_c_objects += cfg_type_route6.o 
generated_c_objects += cfg_type_nat.o 
generated_c_objects += cfg_type_dmz.o 
generated_c_objects += cfg_type_virserver.o 
ifneq ($(strip $(TCSUPPORT_CT_E8GUI)),)
generated_c_objects += cfg_type_algswitch.o 
endif
ifneq ($(strip $(TCSUPPORT_PORT_TRIGGER)),)
generated_c_objects += cfg_type_porttriggering.o
generated_c_objects += cfg_type_guitemp.o  
endif
ifeq ($(strip $(TCSUPPORT_WAN_GPON) $(TCSUPPORT_WAN_EPON) $(TCSUPPORT_NP)),)
generated_c_objects += cfg_type_adsl.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_E8DDNS)),)
generated_c_objects += cfg_type_ddns.o 
endif
generated_c_objects += cfg_type_snmpd.o 
generated_c_objects += cfg_type_upnpd.o 
generated_c_objects += cfg_type_timezone.o 
generated_c_objects += cfg_type_acl.o 
generated_c_objects += cfg_type_ipmacfilter.o
generated_c_objects += cfg_type_appfilter.o 
generated_c_objects += cfg_type_urlfilter.o 
ifneq ($(strip $(TCSUPPORT_CWMP)),)
generated_c_objects += cfg_type_cwmp.o 
endif
ifneq ($(strip $(TCSUPPORT_QOS)),)
generated_c_objects += cfg_type_qos.o
endif
generated_c_objects += cfg_type_mac.o 

ifneq ($(strip $(TCSUPPORT_CWMP)),)
generated_c_objects += cfg_type_tr069attr.o 
generated_c_objects += cfg_type_ipinterface.o 
generated_c_objects += cfg_type_cwmproute.o 
generated_c_objects += cfg_type_switchpara.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_PORTAL_MANAGEMENT) $(TCSUPPORT_TRUE_LANDING_PAGE)),)
generated_c_objects += cfg_type_portal.o 
endif
generated_c_objects += cfg_type_account.o 
ifneq ($(strip $(TCSUPPORT_CT_DEVICEREG)),)
generated_c_objects += cfg_type_deviceaccount.o 
endif
generated_c_objects += cfg_type_system.o 
generated_c_objects += cfg_type_webcurset.o 
generated_c_objects += cfg_type_webcustom.o 
generated_c_objects += cfg_type_dyndisp.o 
ifneq ($(strip $(TCSUPPORT_CT_GUIACCESSLIMIT)),)
generated_c_objects += cfg_type_dyncwmpattr.o 
endif
generated_c_objects += cfg_type_diagnostic.o 
generated_c_objects += cfg_type_deviceinfo.o 
generated_c_objects += cfg_type_info.o 
ifneq ($(strip $(TCSUPPORT_CT_VLAN_BIND)),)
generated_c_objects += cfg_type_vlanbind.o 
endif
ifneq ($(strip $(TCSUPPORT_VLAN_ACCESS_TRUNK)),)
generated_c_objects += cfg_type_multivlan.o 
generated_c_objects += cfg_type_multilan.o 
endif
generated_c_objects += cfg_type_autopvc.o 
ifneq ($(strip $(TCSUPPORT_CWMP_SSL)),)
generated_c_objects += cfg_type_sslca.o 
endif
ifneq ($(strip $(TCSUPPORT_DMS)),)
generated_c_objects += cfg_type_dms.o 
endif
ifneq ($(strip $(TCSUPPORT_IGMP_PROXY)),)
generated_c_objects += cfg_type_igmpproxy.o 
endif
ifneq ($(strip $(TCSUPPORT_MLD_PROXY)),)
generated_c_objects += cfg_type_mldproxy.o 
endif
ifneq ($(strip $(TCSUPPORT_SYSLOG)),)
generated_c_objects += cfg_type_syslog.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_ACCESSLIMIT)),)
generated_c_objects += cfg_type_accesslimit.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_IPPINGDIAGNOSTIC)),)
generated_c_objects += cfg_type_ctdiagnostic.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_ALARMMONITOR)),)
generated_c_objects += cfg_type_devicealarm.o 
generated_c_objects += cfg_type_devicemonitor.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_FTP_DOWNLOADCLIENT)),)
generated_c_objects += cfg_type_appftp.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_USB_BACKUPRESTORE)),)
generated_c_objects += cfg_type_usbrestore.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_LOOPDETECT)),)
generated_c_objects += cfg_type_loopdetect.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_PHONEAPP) $(TCSUPPORT_CT_JOYME2)),)
generated_c_objects += cfg_type_mobile.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_DS_LIMIT)),)
generated_c_objects += cfg_type_dataspeedlimit.o 
endif

ifneq ($(strip $(TCSUPPORT_CT_JOYME)),)
generated_c_objects += cfg_type_osgiupgrade.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME) $(TCSUPPORT_CUC)),)
generated_c_objects += cfg_type_storageaccessright.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME2) $(TCSUPPORT_CT_JOYME)),)
generated_c_objects += cfg_type_attachwandnstunnel.o 
generated_c_objects += cfg_type_dnsspeedlimit.o 
generated_c_objects += cfg_type_usbmount.o 
generated_c_objects += cfg_type_plugin.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME_BANDWIDTH)),)
generated_c_objects += cfg_type_bandwidth.o 
generated_c_objects += cfg_type_maxbandwidth.o 
endif
ifneq ($(strip $(TCSUPPORT_CWMP)),)
ifneq ($(strip $(TCSUPPORT_CWMP_TR111)),)
generated_c_objects += cfg_type_dhcpclient.o 
generated_c_objects += cfg_type_dhcpclientlimit.o 
endif
endif
ifneq ($(strip $(TCSUPPORT_CT_UPNP_DM)),)
generated_c_objects += cfg_type_upnpdm.o 
generated_c_objects += cfg_type_upnpdmswprofile.o 
generated_c_objects += cfg_type_upnpdmservice.o 
generated_c_objects += cfg_type_upnpdmapint.o 
generated_c_objects += cfg_type_upnpdmapiptv.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_PON)),)
ifeq ($(strip $(TCSUPPORT_CMCC)),)
generated_c_objects += cfg_type_logicid.o 
endif
endif
ifneq ($(strip $(TCSUPPORT_CT_PPPOE_EMULATOR)),)
generated_c_objects += cfg_type_pppoeemulator.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_IPOE_EMULATOR)),)
generated_c_objects += cfg_type_ipoe_emulator.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME)),)
generated_c_objects += cfg_type_laninfo.o 
endif
generated_c_objects += cfg_type_globalstate.o 
generated_c_objects += cfg_type_autoexec.o
ifneq ($(strip $(TCSUPPORT_VOIP)),)
generated_c_objects += cfg_type_voip.o
generated_c_objects += cfg_type_voipsysparam.o
generated_c_objects += cfg_type_voipbasic.o
generated_c_objects += cfg_type_voipadvanced.o
generated_c_objects += cfg_type_voipdigitmap.o
generated_c_objects += cfg_type_voiptest.o
generated_c_objects += cfg_type_voipsimulatetest.o
generated_c_objects += cfg_type_voipdiag.o
generated_c_objects += cfg_type_voipcallctrl.o
generated_c_objects += cfg_type_voipmedia.o
generated_c_objects += cfg_type_voipcodecs.o
generated_c_objects += cfg_type_voiph248.o
generated_c_objects += cfg_type_infovoip.o
generated_c_objects += cfg_type_infovoiph248.o
generated_c_objects += cfg_type_infovoippoorql.o
generated_c_objects += cfg_type_voipspeed.o
endif
ifneq ($(strip $(TCSUPPORT_EPON_OAM_CUC)),) 
generated_c_objects += cfg_type_cucping.o
endif 
ifneq ($(strip $(TCSUPPORT_CUC_CWMP_PARAMETER)),)
generated_c_objects += cfg_type_user.o
generated_c_objects += cfg_type_macfilter.o
generated_c_objects += cfg_type_ipfilter.o
generated_c_objects += cfg_type_whiteurl.o
generated_c_objects += cfg_type_blackurl.o
endif
ifneq ($(strip $(TCSUPPORT_PPPOE_SIMULATE)),)
generated_c_objects += cfg_type_pppoesimulate.o
endif
generated_c_objects += cfg_type_dproxy.o 
ifneq ($(strip $(TCSUPPORT_SAMBA) $(TCSUPPORT_SAMBA_IPv6) $(TCSUPPORT_SAMBA_4)),)
generated_c_objects += cfg_type_samba.o 
endif
ifneq ($(strip $(TCSUPPORT_CMCCV2)),) 
generated_c_objects += cfg_type_voipmonitor.o  
generated_c_objects += cfg_type_wlanshare.o 
generated_c_objects += cfg_type_apipermission.o  
generated_c_objects += cfg_type_vpndestlist.o 
generated_c_objects += cfg_type_cmccinfo.o
generated_c_objects += cfg_type_hostname.o
endif
ifneq ($(strip $(TCSUPPORT_VXLAN)),) 
generated_c_objects += cfg_type_vxlan.o
endif
ifneq ($(strip $(TCSUPPORT_CMCCV2) $(TCSUPPORT_CUC)),)
generated_c_objects += cfg_type_trafficmonitor.o 
generated_c_objects += cfg_type_trafficforward.o 
generated_c_objects += cfg_type_trafficmirror.o 
generated_c_objects += cfg_type_trafficqosservice.o
generated_c_objects += cfg_type_trafficdetailprocess.o
generated_c_objects += cfg_type_guestssidinfo.o
endif
ifneq ($(strip $(TCSUPPORT_CUC)),)
generated_c_objects += cfg_type_fastpathspeedup.o
generated_c_objects += cfg_type_accesscontrol.o
endif
ifneq ($(strip $(TCSUPPORT_CMCCV2) $(RA_PARENTALCONTROL)),)
generated_c_objects += cfg_type_parental.o
generated_c_objects += cfg_type_parental_mac.o
endif
ifneq ($(strip $(TCSUPPORT_CT_PMINFORM)$(TCSUPPORT_CT_JOYME2)),)
generated_c_objects += cfg_type_pminform.o 
endif
ifneq ($(strip $(TCSUPPORT_SSH)),)
generated_c_objects += cfg_type_ssh.o 
endif
ifneq ($(strip $(TCSUPPORT_GENERAL_MULTILANGUAGE)),)
generated_c_objects += cfg_type_string.o 
generated_c_objects += cfg_type_languageswitch.o 
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME2) $(TCSUPPORT_CT_UBUS)),)
generated_c_objects += cfg_type_timer.o
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME2)),)
ifneq ($(strip $(TCSUPPORT_WLAN)),)
generated_c_objects += cfg_type_wlanmapping.o 
generated_c_objects += cfg_type_wlancollectinfo.o
endif
generated_c_objects += cfg_type_ctc_dnsserver.o
generated_c_objects += cfg_type_incomingfilter.o
generated_c_objects += cfg_type_dnsfilter.o
generated_c_objects += cfg_type_speedtest.o
generated_c_objects += cfg_type_vpndomain.o
generated_c_objects += cfg_type_vpndomaincache.o
generated_c_objects += cfg_type_vpnips.o
generated_c_objects += cfg_type_vpnmac.o
generated_c_objects += cfg_type_process.o
generated_c_objects += cfg_type_processres.o
generated_c_objects += cfg_type_qosattr.o
endif
ifneq ($(strip $(TCSUPPORT_CT_SDN)),)
generated_c_objects += cfg_type_sdn.o
endif
ifneq ($(strip $(TCSUPPORT_CT_WLAN_JOYME3)),)
generated_c_objects += cfg_type_macblacklist.o
generated_c_objects += cfg_type_macwhitelist.o 
endif
ifneq ($(strip $(TCSUPPORT_ANDLINK)),)
generated_c_objects += cfg_type_alink.o 
endif
ifneq ($(strip $(TCSUPPORT_WLAN_APCLIENT)),)
generated_c_objects += cfg_type_apcli.o 
endif
ifneq ($(strip $(TCSUPPORT_NP_CMCC)),)
generated_c_objects += cfg_type_apwaninfo.o 
endif
ifneq ($(strip $(TCSUPPORT_CWMP_TR181)),)
generated_c_objects += cfg_type_tr181.o 
generated_c_objects += cfg_type_tr181ip.o 
generated_c_objects += cfg_type_tr181ppp.o 
generated_c_objects += cfg_type_EtherVlanT.o 
generated_c_objects += cfg_type_EtherLink.o 
generated_c_objects += cfg_type_Optical.o 
generated_c_objects += cfg_type_ipdiag.o 
generated_c_objects += cfg_type_users.o 
generated_c_objects += cfg_type_devinfloc.o 
generated_c_objects += cfg_type_mgrser.o
generated_c_objects += cfg_type_ether.o 
generated_c_objects += cfg_type_bridging.o 
generated_c_objects += cfg_type_DHCPv4Rly.o 
generated_c_objects += cfg_type_DHCPv4Server.o 
generated_c_objects += cfg_type_DHCPv4ServerPool.o 
generated_c_objects += cfg_type_DHCPv4SvrPoolClient.o  
generated_c_objects += cfg_type_DHCPv4SvrPoolClientIp4Addr.o   
generated_c_objects += cfg_type_DHCPv4ServerPoolStaAddr.o
generated_c_objects += cfg_type_bridge.o 
generated_c_objects += cfg_type_bridgevlan.o 
generated_c_objects += cfg_type_bridgevlanport.o
generated_c_objects += cfg_type_bridgefilter.o 
generated_c_objects += cfg_type_DHCPv4.o      
generated_c_objects += cfg_type_DHCPv4ClientReqOpt.o
generated_c_objects += cfg_type_DHCPv4ClientSentOpt.o
generated_c_objects += cfg_type_DHCPv4ServerPoolClientOpt.o
generated_c_objects += cfg_type_DHCPv4ServerPoolOpt.o
generated_c_objects += cfg_type_DHCPv6.o
generated_c_objects += cfg_type_DHCPv6Server.o
generated_c_objects += cfg_type_DHCPv6ServerPool.o
generated_c_objects += cfg_type_DHCPv6ClientRcvOpt.o
generated_c_objects += cfg_type_DHCPv6ClientSentOpt.o
generated_c_objects += cfg_type_DHCPv6ClientServer.o
generated_c_objects += cfg_type_DHCPv6ServerPoolClient.o
generated_c_objects += cfg_type_DHCPv6ServerPoolClientIp6Addr.o
generated_c_objects += cfg_type_DHCPv6ServerPoolClientIp6Prev.o
generated_c_objects += cfg_type_DHCPv6ServerPoolClientOpt.o
generated_c_objects += cfg_type_DHCPv6ServerPoolOpt.o
generated_c_objects += cfg_type_Wifi.o
generated_c_objects += cfg_type_WifiApAcc.o
generated_c_objects += cfg_type_WifiApAss.o
generated_c_objects += cfg_type_WifiEp.o
generated_c_objects += cfg_type_WifiEpProSec.o
generated_c_objects += cfg_type_WifiEpSec.o
generated_c_objects += cfg_type_WifiEpSta.o
generated_c_objects += cfg_type_WifiEpWps.o
generated_c_objects += cfg_type_WifiRdoSta.o
generated_c_objects += cfg_type_WifiSsidSta.o
generated_c_objects += cfg_type_RouteInfo.o
generated_c_objects += cfg_type_TQos.o
generated_c_objects += cfg_type_tqosapp.o
generated_c_objects += cfg_type_tqosclsficn.o
generated_c_objects += cfg_type_tqospolicer.o
generated_c_objects += cfg_type_tqosque.o
generated_c_objects += cfg_type_QosFlow.o
generated_c_objects += cfg_type_QosQueSta.o
generated_c_objects += cfg_type_QosShaper.o
generated_c_objects += cfg_type_host.o
generated_c_objects += cfg_type_dns.o
generated_c_objects += cfg_type_dns_forwarding.o
generated_c_objects += cfg_type_routerAdv_ItfSetting_opt.o
generated_c_objects += cfg_type_userinterface.o
generated_c_objects += cfg_type_captiveportal.o
generated_c_objects += cfg_type_perdsts.o
generated_c_objects += cfg_type_perdsts_samp.o
generated_c_objects += cfg_type_ether_rmonstats.o
generated_c_objects += cfg_type_tr181rip.o
generated_c_objects += cfg_type_devinfps.o 
generated_c_objects += cfg_type_devinftem.o 
generated_c_objects += cfg_type_tr181ipap.o 
generated_c_objects += cfg_type_InterfaceStack.o
generated_c_objects += cfg_type_tr181IPv4Addr.o
generated_c_objects += cfg_type_tr181IPv6Addr.o
generated_c_objects += cfg_type_tr181IPV6PRE.o
generated_c_objects += cfg_type_NatInterface.o
generated_c_objects += cfg_type_DHCPv4Client.o
generated_c_objects += cfg_type_DHCPv6Client.o
generated_c_objects += cfg_type_NatPortMap.o
generated_c_objects += cfg_type_RouteForward4.o
generated_c_objects += cfg_type_RouteForward6.o
generated_c_objects += cfg_type_DnsClient.o
generated_c_objects += cfg_type_BridgePort.o
generated_c_objects += cfg_type_RouteAdvIfSet.o
generated_c_objects += cfg_type_routeAdv.o
generated_c_objects += cfg_type_tr181dslite.o
generated_c_objects += cfg_type_tr181route.o
generated_c_objects += cfg_type_DnsClient.o
generated_c_objects += cfg_type_firewallch.o
generated_c_objects += cfg_type_TR181Firewall.o
generated_c_objects += cfg_type_FirewallLev.o
generated_c_objects += cfg_tr181_common.o
generated_c_objects += cfg_type_WifiEpPro.o
generated_c_objects += cfg_type_mgrser_dld_group.o
generated_c_objects += cfg_type_EtherInterface.o 
generated_c_objects += cfg_type_WIFIAP.o
generated_c_objects += cfg_type_WIFIAPSec.o
generated_c_objects += cfg_type_WIFIAPWPS.o
generated_c_objects += cfg_type_WIFIRdo.o
generated_c_objects += cfg_type_WIFISsid.o
generated_c_objects += cfg_type_tr181WIFI_common.o
generated_c_objects += cfg_type_tr181IPv6rd.o
ifneq ($(strip $(TCSUPPORT_CT_DSL_EX)),)
generated_c_objects += cfg_type_tr181_atmlink.o
generated_c_objects += cfg_type_tr181_dslChannel.o
generated_c_objects += cfg_type_tr181_dslLine.o
generated_c_objects += cfg_type_tr181_ptmlink.o
endif
endif
ifneq ($(strip $(TCSUPPORT_CT_DSL_EX)),)
generated_c_objects += cfg_type_wanatm.o
generated_c_objects += cfg_type_wanptm.o
generated_c_objects += cfg_type_virserveratm.o
generated_c_objects += cfg_type_virserverptm.o
generated_c_objects += cfg_type_routeatm.o
generated_c_objects += cfg_type_routeptm.o
generated_c_objects += cfg_type_route6atm.o
generated_c_objects += cfg_type_route6ptm.o
generated_c_objects += cfg_type_dmzatm.o
generated_c_objects += cfg_type_dmzptm.o
generated_c_objects += cfg_type_igmpproxyatm.o
generated_c_objects += cfg_type_igmpproxyptm.o
generated_c_objects += cfg_type_mldproxyatm.o
generated_c_objects += cfg_type_mldproxyptm.o
generated_c_objects += cfg_type_ddnsatm.o
generated_c_objects += cfg_type_ddnsptm.o
generated_c_objects += cfg_type_cwmp_dsl_diagnostic.o
endif
ifneq ($(strip $(TCSUPPORT_CT_VRWAN)),)
generated_c_objects += cfg_type_vrserver.o
endif
ifneq ($(strip $(TCSUPPORT_CT_IPOE_DETECT)),)
generated_c_objects += cfg_type_ipoediag.o
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME4)),)
generated_c_objects += cfg_type_cloudservice.o
endif

ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
generated_c_objects += cfg_type_mesh.o
endif
ifneq ($(strip $(TCSUPPORT_CT_JOYME4)),)
generated_c_objects += cfg_type_bridgedata_acl.o
generated_c_objects += cfg_type_iptv.o
generated_c_objects += cfg_type_cfgauth.o
generated_c_objects += cfg_type_downlinkqos.o
generated_c_objects += cfg_type_wancfgdata.o
endif

ifneq ($(strip $(TCSUPPORT_CUC)),)
generated_c_objects += cfg_type_alarmlog.o
endif
ifneq ($(strip $(TCSUPPORT_STUN)),)
generated_c_objects += cfg_type_stun.o
endif
ifneq ($(strip $(TCSUPPORT_CT_UBUS)),)
generated_c_objects += cfg_type_ctcapd.o
endif

generated_c_objects += cfg_type_interfacerate.o

ARFLAGS	=crvs

CENTRAL_CTRL_LIB_LIBS += $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) $(ECNT_EVENT_LIB) -lapi_libbsp_ppe -L  $(BLLIB_SYSTEM) -L $(LIB_INSTALL_DIR) $(ECNT_EVENT_LIB) $(BLLIB_TRAFFIC) $(BLLIB_PERFORM) $(BLLIB_XDSL) $(BLLIB_PON) -lapi_lib_wifimgr_user 

CFLAGS += -fPIC -Wall  -O2  -I$(INSTALL_INC)  -I$(SERVICE_CFG_DIR) -I$(APP_CFG_NG_DIR)/service/other -I$(GLOBAL_INC_DIR)/modules -I$(APP_MXML_DIR) $(LIBINSTALL_CFLAGS) -I$(INSTALL_GLOBALINC)/uapi/ecnt_event_global
ifneq ($(TC_CFLAGS), )
CFLAGS+=$(TC_CFLAGS)
endif
ifneq ($(strip $(TCSUPPORT_CWMP_TR181)),)
CFLAGS += -I$(SERVICE_CFG_DIR)/type/tr181
endif

all: cfg_type_tr181 libcfg_types.a

%.o:%.c
	$(CC)  $(CFLAGS) $(TC_CFLAGS) $(CENTRAL_CTRL_LIB_LIBS) -c $<

ifneq ($(strip $(TCSUPPORT_CWMP_TR181)),)
cfg_type_tr181:
	$(MAKE)  -C  ./tr181 all
	cp  -f ./tr181/cfg_type_tr181.o  ./
	cp  -f ./tr181/cfg_type_tr181ip.o  ./
	cp  -f ./tr181/cfg_type_tr181ppp.o  ./
	cp  -f ./tr181/cfg_type_EtherVlanT.o  ./
	cp  -f ./tr181/cfg_type_EtherLink.o  ./
	cp  -f ./tr181/cfg_type_Optical.o  ./
	cp  -f ./tr181/cfg_type_ipdiag.o  ./
	cp  -f ./tr181/cfg_type_users.o  ./
	cp  -f ./tr181/cfg_type_devinfloc.o  ./
	cp  -f ./tr181/cfg_type_mgrser.o  ./
	cp  -f ./tr181/cfg_type_ether.o  ./
	cp  -f ./tr181/cfg_type_bridging.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4Rly.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4Server.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4ServerPool.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4SvrPoolClient.o  ./   
	cp  -f ./tr181/cfg_type_DHCPv4SvrPoolClientIp4Addr.o  ./  
	cp  -f ./tr181/cfg_type_DHCPv4ServerPoolStaAddr.o  ./	 
	cp  -f ./tr181/cfg_type_bridge.o  ./
	cp  -f ./tr181/cfg_type_bridgevlan.o  ./
	cp  -f ./tr181/cfg_type_bridgevlanport.o  ./
	cp  -f ./tr181/cfg_type_bridgefilter.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4ClientReqOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4ClientSentOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4ServerPoolClientOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4ServerPoolOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6Server.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ServerPool.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ClientRcvOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ClientSentOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ClientServer.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ServerPoolClient.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ServerPoolClientIp6Addr.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ServerPoolClientIp6Prev.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ServerPoolClientOpt.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6ServerPoolOpt.o  ./
	cp  -f ./tr181/cfg_type_Wifi.o  ./
	cp  -f ./tr181/cfg_type_WifiApAcc.o  ./
	cp  -f ./tr181/cfg_type_WifiApAss.o  ./
	cp  -f ./tr181/cfg_type_WifiEp.o  ./
	cp  -f ./tr181/cfg_type_WifiEpProSec.o  ./
	cp  -f ./tr181/cfg_type_WifiEpSec.o  ./
	cp  -f ./tr181/cfg_type_WifiEpSta.o  ./
	cp  -f ./tr181/cfg_type_WifiEpWps.o  ./
	cp  -f ./tr181/cfg_type_WifiRdoSta.o  ./
	cp  -f ./tr181/cfg_type_WifiSsidSta.o  ./
	cp  -f ./tr181/cfg_type_RouteInfo.o  ./
	cp  -f ./tr181/cfg_type_TQos.o  ./
	cp  -f ./tr181/cfg_type_tqosapp.o  ./
	cp  -f ./tr181/cfg_type_tqosclsficn.o  ./
	cp  -f ./tr181/cfg_type_tqospolicer.o  ./
	cp  -f ./tr181/cfg_type_tqosque.o  ./
	cp  -f ./tr181/cfg_type_QosFlow.o  ./
	cp  -f ./tr181/cfg_type_QosQueSta.o  ./
	cp  -f ./tr181/cfg_type_QosShaper.o  ./
	cp  -f ./tr181/cfg_type_host.o  ./
	cp  -f ./tr181/cfg_type_dns.o  ./
	cp  -f ./tr181/cfg_type_dns_forwarding.o  ./
	cp  -f ./tr181/cfg_type_routerAdv_ItfSetting_opt.o  ./
	cp  -f ./tr181/cfg_type_userinterface.o  ./
	cp  -f ./tr181/cfg_type_captiveportal.o  ./
	cp  -f ./tr181/cfg_type_perdsts.o  ./
	cp  -f ./tr181/cfg_type_perdsts_samp.o  ./
	cp  -f ./tr181/cfg_type_ether_rmonstats.o  ./
	cp  -f ./tr181/cfg_type_tr181rip.o  ./
	cp  -f ./tr181/cfg_type_devinfps.o  ./
	cp  -f ./tr181/cfg_type_devinftem.o  ./
	cp  -f ./tr181/cfg_type_tr181ipap.o  ./
	cp  -f ./tr181/cfg_type_InterfaceStack.o  ./
	cp  -f ./tr181/cfg_type_tr181IPv4Addr.o  ./
	cp  -f ./tr181/cfg_type_tr181IPv6Addr.o  ./
	cp  -f ./tr181/cfg_type_tr181IPV6PRE.o  ./
	cp  -f ./tr181/cfg_type_NatInterface.o  ./
	cp  -f ./tr181/cfg_type_DHCPv4Client.o  ./
	cp  -f ./tr181/cfg_type_DHCPv6Client.o  ./
	cp  -f ./tr181/cfg_type_NatPortMap.o  ./
	cp  -f ./tr181/cfg_type_RouteForward4.o  ./
	cp  -f ./tr181/cfg_type_RouteForward6.o  ./
	cp  -f ./tr181/cfg_type_DnsClient.o  ./
	cp  -f ./tr181/cfg_type_BridgePort.o  ./
	cp  -f ./tr181/cfg_type_RouteAdvIfSet.o  ./
	cp  -f ./tr181/cfg_type_routeAdv.o  ./
	cp  -f ./tr181/cfg_type_tr181dslite.o  ./
	cp  -f ./tr181/cfg_type_tr181route.o  ./
	cp  -f ./tr181/cfg_type_DnsClient.o  ./
	cp  -f ./tr181/cfg_type_firewallch.o  ./
	cp  -f ./tr181/cfg_type_TR181Firewall.o  ./
	cp  -f ./tr181/cfg_type_FirewallLev.o  ./
	cp  -f ./tr181/cfg_tr181_common.o  ./
	cp  -f ./tr181/cfg_type_WifiEpPro.o  ./
	cp  -f ./tr181/cfg_type_mgrser_dld_group.o  ./
	cp  -f ./tr181/cfg_type_EtherInterface.o  ./
	cp  -f ./tr181/cfg_type_WIFIAP.o  ./
	cp  -f ./tr181/cfg_type_WIFIAPSec.o  ./
	cp  -f ./tr181/cfg_type_WIFIAPWPS.o  ./
	cp  -f ./tr181/cfg_type_WIFIRdo.o  ./
	cp  -f ./tr181/cfg_type_WIFISsid.o  ./
	cp  -f ./tr181/cfg_type_tr181WIFI_common.o  ./
	cp  -f ./tr181/cfg_type_tr181IPv6rd.o  ./	
ifneq ($(strip $(TCSUPPORT_CT_DSL_EX)),)
	cp  -f ./tr181/cfg_type_tr181_atmlink.o  ./
	cp  -f ./tr181/cfg_type_tr181_dslChannel.o  ./
	cp  -f ./tr181/cfg_type_tr181_dslLine.o  ./
	cp  -f ./tr181/cfg_type_tr181_ptmlink.o  ./
endif
else
cfg_type_tr181:
endif

libcfg_types.a:  $(generated_c_objects)
	$(AR) $(ARFLAGS) $@ $(generated_c_objects) 
#	$(RANLIB) $@

clean: 
ifneq ($(strip $(TCSUPPORT_CWMP_TR181)),)
	$(MAKE)  -C  ./tr181 clean
endif
	-rm  *.o  libcfg_types.a
