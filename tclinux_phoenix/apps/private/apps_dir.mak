TRUNK_DIR=$(shell pwd)

APP_DIR=$(TRUNK_DIR)/apps
APP_BINARY_DIR=$(APP_DIR)/binary
export APP_BINARY_DIR
APP_PUBLIC_DIR=$(APP_DIR)/public
export APP_PUBLIC_DIR
APP_OPENSOURCE_DIR=$(APP_DIR)/opensource
export APP_OPENSOURCE_DIR
APP_PRIVATE_DIR=$(APP_DIR)/private

APP_INSTALL_BINARY_DIR=$(APP_BINARY_DIR)/$(TCPLATFORM)/
export APP_INSTALL_BINARY_DIR

APP_UPNP_DIR=$(APP_PUBLIC_DIR)/igd
APP_MTDUTILS_DIR=$(APP_PUBLIC_DIR)/mtd-utils 
APP_ECB3LIB_DIR=$(APP_PUBLIC_DIR)/libencrypt
APP_8021X_PARA_DIR=$(APP_PUBLIC_DIR)/wpa_supplicant-0.7.3/8021x_para
ifneq ($(strip $(TCSUPPORT_BUSYBOX_1_26_2)),)
APP_BUSYBOX_DIR=$(APP_PUBLIC_DIR)/busybox-1.26.2
else
APP_BUSYBOX_DIR=$(APP_PUBLIC_DIR)/busybox-1.00
endif
APP_BRCTL_DIR=$(APP_PUBLIC_DIR)/bridge-utils-1.0.6
APP_BR2684CTL_DIR=$(APP_PUBLIC_DIR)/br2684ctl
APP_LIBATM_DIR=$(APP_PUBLIC_DIR)/linux-atm
export APP_LIBATM_DIR
APP_JSON_DIR=$(APP_PUBLIC_DIR)/libjson-1.5
export APP_JSON_DIR
PUBLIC_LIB_DIR=$(APP_PUBLIC_DIR)/lib
export PUBLIC_LIB_DIR
PUBLIC_LIB_DIR_CWMP=$(APP_PUBLIC_DIR)/lib_cwmp
export PUBLIC_LIB_DIR_CWMP
APP_DMIPS_DIR=$(APP_PUBLIC_DIR)/dhrystone-2.1
APP_LIBSTRCMP_DIR=$(APP_DMIPS_DIR)/libstrcmp
APP_COREMARK_DIR=$(APP_PUBLIC_DIR)/coremark_v1.0
APP_CPU_DIR=$(APP_PUBLIC_DIR)/cpu
APP_IPERF3_DIR=$(APP_PUBLIC_DIR)/iperf-master/src
APP_IPTABLES_DIR=$(APP_OPENSOURCE_DIR)/iptables-1.4.21
APP_IPTABLES_INSTALL_DIR=$(APP_IPTABLES_DIR)/install
APP_IPROUTE_TC_DIR=$(APP_PUBLIC_DIR)/iproute2-2.6.22-070710
APP_IPROUTE_IP_DIR=$(APP_PUBLIC_DIR)/iproute
APP_EBTABLES_DIR=$(APP_PUBLIC_DIR)/ebtables-v2.0.8-2
APP_MTD_DIR=$(APP_PUBLIC_DIR)/mtd
ifneq ($(strip $(TCSUPPORT_CT)),)
ifneq ($(strip $(TCSUPPORT_SDN_OVS) $(TCSUPPORT_CT_VPN_PPTP)),)
APP_PPPD_DIR=$(APP_PUBLIC_DIR)/ppp-2.4.5
else
APP_PPPD_DIR=$(APP_PUBLIC_DIR)/ppp-2.4.1.pppoe4.orig
endif
else
APP_PPPD_DIR=$(APP_PUBLIC_DIR)/ppp-2.4.5
endif
APP_UTELNETD_DIR=$(APP_PUBLIC_DIR)/utelnetd-0.1.2
APP_MXML_DIR=$(APP_PUBLIC_DIR)/mxml-2.4
export APP_MXML_DIR
APP_BOA_DIR=$(APP_PUBLIC_DIR)/boa-asp/src
APP_INETD_DIR=$(APP_PUBLIC_DIR)/inetd
APP_SNMPD_573_DIR=$(APP_PUBLIC_DIR)/net-snmp-5.7.3
APP_NTPCLIENT_DIR=$(APP_PUBLIC_DIR)/ntpclient
APP_TFTPD_DIR=$(APP_PUBLIC_DIR)/tftp-1.0
APP_WRIELESSTOOL_DIR=$(APP_PUBLIC_DIR)/wireless_tools.28
export APP_WRIELESSTOOL_DIR
ifneq ($(strip $(TCSUPPORT_IGMP_PROXY_V3)),)
APP_IGMPPROXY_DIR=$(APP_PRIVATE_DIR)/igmpv3proxy
else
APP_IGMPPROXY_DIR=$(APP_PUBLIC_DIR)/igmpproxy
endif
APP_SSL_DIR=$(APP_PUBLIC_DIR)/matrixssl-1-8/src
APP_PPPOERELAY_DIR=$(APP_PUBLIC_DIR)/rp-pppoe-3.10/src
APP_FLEX_2_5_39_DIR=$(APP_PUBLIC_DIR)/flex-2.5.39
APP_FLEX_DIR=$(APP_PUBLIC_DIR)/flex-2.5.35
APP_RADVD_2_17_DIR=$(APP_PUBLIC_DIR)/radvd-2.17
APP_RADVD_DIR=$(APP_PUBLIC_DIR)/radvd-1.5
APP_DHCPV6_DIR=$(APP_PUBLIC_DIR)/wide-dhcpv6-20080615
APP_MLDPROXY_DIR=$(APP_PUBLIC_DIR)/ecmh-2005.02.09
APP_IPROUTE_DIR=$(APP_PUBLIC_DIR)/iproute
APP_8021X_DIR=$(APP_PUBLIC_DIR)/8021X
APP_VCONFIG_DIR=$(APP_PUBLIC_DIR)/vlan
APP_DNSMASQ_DIR=$(APP_PUBLIC_DIR)/dnsmasq-2.52
APP_USB_PRINTER_DIR=$(APP_PUBLIC_DIR)/p910nd-0.93
APP_MJPEG_LIB_DIR=$(APP_PUBLIC_DIR)/jpeg-6b
APP_USB_MJPEG_STREAM_SERVER_DIR=$(APP_PUBLIC_DIR)/mjpg-streamer-r63
APP_USB_3G_PPP_DIR=$(APP_PUBLIC_DIR)/ppp-2.4.5
export APP_USB_3G_PPP_DIR
ifneq ($(strip $(TCSUPPORT_CT_JOYME2)),)
APP_NTFS_DIR=$(APP_OPENSOURCE_DIR)/ntfs-3g_ntfsprogs-2017.3.23
else
APP_NTFS_DIR=$(APP_PUBLIC_DIR)/ntfs-3g-2010.5.22
endif
APP_FAT_DIR=$(APP_PUBLIC_DIR)/dosfstools-2.11
APP_CHARSET_DIR=$(APP_PUBLIC_DIR)/libiconv-1.13.1
export APP_CHARSET_DIR
APP_PPPOE_SVR_DIR=$(APP_PUBLIC_DIR)/rp-pppoe-3.10
APP_QUAGGA_DIR=$(APP_PUBLIC_DIR)/quagga-0.98.6
APP_LIBDAEMON_DIR=$(APP_PUBLIC_DIR)/libdaemon-0.14
APP_AUTOIP_DIR=$(APP_PUBLIC_DIR)/avahi-0.6.28
#APP_SSH_DIR=$(APP_PUBLIC_DIR)/sftp-server/openssh-8.6p1
APP_SSH_DIR=$(APP_PUBLIC_DIR)/dropbear-2017.75
APP_LIBOSIP_DIR=$(APP_PUBLIC_DIR)/libosip2-2.0.4
APP_SIPROXD_DIR=$(APP_PUBLIC_DIR)/siproxd-0.5.12
APP_DIFF_DIR=$(APP_PUBLIC_DIR)/diffutils-3.2
APP_SAMBA_DIR=$(APP_PUBLIC_DIR)/samba-3.0.2
APP_SAMBA4_DIR=$(APP_PUBLIC_DIR)/samba-4.0.24
APP_SAMBA_IPv6_DIR=$(APP_PUBLIC_DIR)/samba-3.2.5
APP_WPA_SUPP_DIR=$(APP_PUBLIC_DIR)/wpa_supplicant-0.7.3/wpa_supplicant
APP_OPENSSL_DIR=$(APP_PUBLIC_DIR)/openssl-1.0.0d
APP_BACKUPRESTORE_DIR=$(APP_PRIVATE_DIR)/backuprestore
APP_TRACEROUTE_DIR=$(APP_PUBLIC_DIR)/traceroute-2.0.17
APP_SYSSTAT_DIR=$(APP_PUBLIC_DIR)/tools/sysstat-9.0.4
APP_FONSMCD_DIR=$(APP_PUBLIC_DIR)/fonap-4ffea20
APP_FON_HOTSPOT_DIR=$(APP_PUBLIC_DIR)/hotspotd-2012-11-19
APP_PCRE_DIR=$(APP_PUBLIC_DIR)/pcre-8.32
APP_IPSEC_TOOLS=$(APP_PUBLIC_DIR)/ipsec-tools-0.8.0
APP_OPENSSL_VPN_DIR=$(APP_PUBLIC_DIR)/openssl-1.0.0e
APP_WGET_DIR=$(APP_PUBLIC_DIR)/wget-1.11.2
APP_CURL_DIR=$(APP_PUBLIC_DIR)/curl-7.48.0
APP_AXEL_DIR=$(APP_PUBLIC_DIR)/axel-2.4
APP_JOYME2_DIR=$(APP_PUBLIC_DIR)/joyme2.0
export APP_JOYME2_DIR
OSGI_DIR=$(APP_PUBLIC_DIR)/OSGi
DDNS_DIR=$(APP_PUBLIC_DIR)/phddns-2.0.5.19225
OPENJDK_DIR=$(APP_PUBLIC_DIR)/openjdk
export OPENJDK_DIR
APP_XL2TPD_DIR=$(APP_PUBLIC_DIR)/xl2tpd
APP_PCAP_DIR=$(APP_PUBLIC_DIR)/xl2tpd/libpcap_install
APP_GDB_DIR=$(APP_PUBLIC_DIR)/gdb-7.12.1
APP_GDB_INSTALL_DIR=$(APP_PUBLIC_DIR)/gdb-7.12.1/install
HPING_DIR=$(APP_PUBLIC_DIR)/hping
export HPING_DIR
NMAP_DIR=$(APP_OPENSOURCE_DIR)/nmap-7.80
export NMAP_DIR
APP_UDP_PROXY_DIR=$(APP_PUBLIC_DIR)/udpxy-1.0.23-12
APP_DHCPRELAY_DIR=$(APP_PUBLIC_DIR)/dhcp-isc
APP_DPROXY_DIR=$(APP_PUBLIC_DIR)/dproxy-nexgen
APP_EZ-IPUPDATE_DIR=$(APP_PUBLIC_DIR)/ez-ipupdate-3.0.10
APP_BFTPD_DIR=$(APP_PUBLIC_DIR)/bftpd

APP_LIBEVENT_DIR=$(APP_OPENSOURCE_DIR)/libevent-2.1.8-stable
OPENSOURCE_LIB_DIR=$(APP_OPENSOURCE_DIR)/lib
export OPENSOURCE_LIB_DIR
APP_NETOPEER_DIR=$(APP_OPENSOURCE_DIR)/netopeer/server
APP_NETOPEER_CFGSYSTEM_DIR=$(APP_OPENSOURCE_DIR)/netopeer/transAPI/cfgsystem
APP_LIBNETCONF_DIR=$(APP_OPENSOURCE_DIR)/libnetconf-master
APP_NETCONF_LIB_DIR=$(APP_OPENSOURCE_DIR)/NETCONF_LIB
APP_ZLIB_DIR=$(APP_OPENSOURCE_DIR)/zlib-1.2.11
APP_LIBSSH_DIR=$(APP_OPENSOURCE_DIR)/libssh-0.7.5
APP_LIBXML2_DIR=$(APP_OPENSOURCE_DIR)/libxml2-2.9.5
APP_LIBXSLT_DIR=$(APP_OPENSOURCE_DIR)/libxslt-1.1.32
LIBCOAP_DIR=$(APP_OPENSOURCE_DIR)/libcoap-4.2.0
export LIBCOAP_DIR
export LIB_JSONC_DIR=$(APP_OPENSOURCE_DIR)/json-c-0.12.1
export APP_LIBUPNP_DIR=$(APP_OPENSOURCE_DIR)/libupnp-1.6.22
APP_DMIPS_CT_DIR=$(APP_OPENSOURCE_DIR)/dhrystone-2.1_CT
export APP_DMIPS_CT_DIR
APP_INSTALL_PLUGINS_DIR=$(APP_OPENSOURCE_DIR)/install_plugins
export APP_INSTALL_PLUGINS_DIR

APP_CWMP_DIR=$(APP_PRIVATE_DIR)/TR69_64
ifneq ($(strip $(TCSUPPORT_CT)),)
APP_CFG_MANAGER_DIR=$(APP_PRIVATE_DIR)/cfg_manager_ct
else
APP_CFG_MANAGER_DIR=$(APP_PRIVATE_DIR)/cfg_manager
endif
export APP_CFG_MANAGER_DIR
APP_CFG_NG_DIR=$(APP_PRIVATE_DIR)/cfg_ng
export APP_CFG_NG_DIR
CFG_NG_LIB_DIR=$(APP_CFG_NG_DIR)/install/lib
export CFG_NG_LIB_DIR
CFG_NG_INC_DIR=$(APP_CFG_NG_DIR)/install/include
export CFG_NG_INC_DIR
APP_SKB_MANAGER_DIR=$(APP_PRIVATE_DIR)/skbmgr
APP_RESTORE_LINOS_INFO_DIR=$(APP_PRIVATE_DIR)/restore_linos_info
APP_CFG_PARSER_DIR=$(APP_PRIVATE_DIR)/cfg_parser
APP_CENTRAL_COORDINATOR_DIR=$(APP_PRIVATE_DIR)/Central_Coordinator
APP_CODE_REDUCE_PATCH_DIR=$(APP_PRIVATE_DIR)/code_reduce_patch
APP_ETC_SCRIPT_DIR=$(APP_PRIVATE_DIR)/etc_script
APP_FWUPGRADE_DIR=$(APP_PRIVATE_DIR)/FWUpgrade
APP_HUAWEI_CI_DIR=$(APP_PRIVATE_DIR)/huawei_ci
APP_LED_CONF_DIR=$(APP_PRIVATE_DIR)/led_conf
APP_OPENSOURCELIB_DIR=$(APP_PRIVATE_DIR)/opensource_lib
export APP_OPENSOURCELIB_DIR
APP_JOYME_COM_DIR=$(APP_PRIVATE_DIR)/joyme
APP_CMCC2_DIR=$(APP_JOYME_COM_DIR)/CMCC2
APP_DNSHOST_DIR=$(APP_JOYME_COM_DIR)/dnshost
APP_JOYME_CT_DIR=$(APP_JOYME_COM_DIR)/CT
APP_JOYME_CUC_DIR=$(APP_JOYME_COM_DIR)/CUC2
APP_CTC_GDBUS_DIR=$(APP_PRIVATE_DIR)/ctc-gdbus
export APP_CTC_GDBUS_DIR
BUNDLE_CMD_DIR=$(APP_JOYME_COM_DIR)/bundlecmd
USB_MONITOR_DIR=$(APP_JOYME_COM_DIR)/usb_monitor
APP_BANDWIDTH_DIR=$(APP_JOYME_COM_DIR)/BandWidth
APP_IPOEDIAG_DIR=$(APP_JOYME_COM_DIR)/ipoe_diag
BUNDLE_MONITOR_DIR=$(APP_JOYME_COM_DIR)/bundle_monitor
PROCESS_MONITOR_DIR=$(APP_JOYME_COM_DIR)/process_monitor
export PROCESS_MONITOR_DIR
export APP_BANDWIDTH_DIR
export APP_IPOEDIAG_DIR
export APP_PLUGIN_MONITOR_DIR
APP_STORE_DIR=$(APP_JOYME_CT_DIR)/appstore
APP_HTTPDETECT_DIR=$(APP_JOYME_CT_DIR)/httpdetect
APP_CTSGWLIB_DIR=$(APP_JOYME_CT_DIR)/ctsgw_lib
APP_BDSERVICE_DIR=$(APP_JOYME_CT_DIR)/bundleService
APP_PLUGIN_MONITOR_DIR=$(APP_JOYME_CT_DIR)/plugin_monitor
APP_HOMENAS_DIR=$(APP_JOYME_CT_DIR)/dirinfo/src
export APP_CTSGWLIB_DIR
APP_TRAFFIC_VPNINFO_DIR=$(APP_JOYME_CT_DIR)/traffic_vpninfo
export APP_TRAFFIC_VPNINFO_DIR
APP_HOMENAS_DIR_DST=$(APP_PRIVATE_DIR)/osgi/com.chinatelecom.econet.smartgateway.nas/src
BUNDLE_STATE_DIR=$(APP_CMCC2_DIR)/bundle_state
APP_TCAPILIB_DIR=$(APP_PRIVATE_DIR)/tcapi_lib
APP_TCAPILIB_ENHANCE_DIR=$(APP_PRIVATE_DIR)/tcapi_enhance_lib
APP_TCAPI_CWMP_LIB_DIR=$(APP_PRIVATE_DIR)/tcapi_lib_cwmp
export APP_TCAPILIB_DIR
export APP_TCAPILIB_ENHANCE_DIR
LIB_DIR_CWMP=$(APP_PRIVATE_DIR)/lib_cwmp
export LIB_DIR_CWMP
APP_MTK_UH_TEST_DIR=$(APP_PRIVATE_DIR)/mtk_xhci_test_cli
APP_FLASH_DIR=$(APP_PRIVATE_DIR)/flash
APP_TCCI_DIR=$(APP_PRIVATE_DIR)/tcci
APP_SENDICMP_DIR=$(APP_PRIVATE_DIR)/sendicmp
APP_TCWDOG_DIR=$(APP_PRIVATE_DIR)/tcwdog
APP_WEBPAGE_DIR=$(APP_PRIVATE_DIR)/webPage
APP_ROUTE_WEBPAGE_DIR=$(APP_PRIVATE_DIR)/webPage/Router/tc/boaroot
APP_BRIDGE_WEBPAGE_DIR=$(APP_PRIVATE_DIR)/webPage/Pure_Bridge/Huawei/MT881s-T/boaroot
APP_TCAPI_DIR=$(APP_PRIVATE_DIR)/tcapi
APP_ZIGBEE_DIR=$(APP_PRIVATE_DIR)/zigbee/app/buildler/econ_Z3GatewayHost
APP_SNMPD_DIR=$(APP_PRIVATE_DIR)/net-snmp-5.3.1_reduce
APP_STB_TEST_DIR=$(APP_PRIVATE_DIR)/stb_test
APP_LANHOST_MGR_DIR=$(APP_PRIVATE_DIR)/lan_host_mgr
APP_CFG_RECOVER_DIR=$(APP_PRIVATE_DIR)/RecoverCfg
APP_ZEBRA_DIR=$(APP_PRIVATE_DIR)/zebra-0.93a_reduced
ifneq ($(strip $(TCSUPPORT_CT)),)
APP_ETHCMD_DIR=$(APP_PRIVATE_DIR)/ethcmd
else
APP_ETHCMD_DIR=$(APP_PRIVATE_DIR)/ethcmd
endif
export APP_ETHCMD_DIR
APP_BNDSTRG_DIR=$(APP_PRIVATE_DIR)/bndstrg
export APP_BNDSTRG_DIR
APP_CMDCI_DIR=$(APP_PRIVATE_DIR)/cmd_ci
export APP_CMDCI_DIR
APP_TCLINUXBUILDER_DIR=$(APP_PRIVATE_DIR)/tclinux_builder
export APP_TCLINUXBUILDER_DIR
APP_RCS_DIR=$(APP_PRIVATE_DIR)/rcS
APP_SMUXCTL_DIR=$(APP_PRIVATE_DIR)/smuxctl
APP_AUTOMOUNT_DIR=$(APP_PRIVATE_DIR)/auto_mount
APP_PCM_TEST_DIR=$(APP_PRIVATE_DIR)/PCM_test
APP_SLIC_TEST_DIR=$(APP_PRIVATE_DIR)/SLIC_test
APP_DMS_DIR=$(APP_PRIVATE_DIR)/dlna_src
BACKUP_DIR=$(TRUNK_DIR)/backup
APP_RA_MENU_DIR=$(APP_PRIVATE_DIR)/ra_menu
APP_NEW_SHELL_DIR=$(APP_PRIVATE_DIR)/new_shell
APP_PWCTLCMD_DIR=$(APP_PRIVATE_DIR)/pwctlcmd
APP_RA_HWNAT_DIR=$(APP_PRIVATE_DIR)/hw_nat
APP_RA_HWNAT_7510_DIR=$(APP_PRIVATE_DIR)/hw_nat_7510
APP_EIP93_APPS=$(APP_PRIVATE_DIR)/eip93_apps
APP_UPNP_DM_DIR=$(APP_PRIVATE_DIR)/ctc-upnp-dm
APP_TCAPI_INIC_DIR=$(APP_PRIVATE_DIR)/tcapi_inic
APP_TCAPILIB_INIC_DIR=$(APP_PRIVATE_DIR)/tcapi_inic_lib
APP_INIC_CLIENT_HEARTBEAT_DIR=$(APP_PRIVATE_DIR)/inic_client_heartbeat
APP_BLOCK_PROCESS_DIR=$(APP_PRIVATE_DIR)/blockProcess
APP_SIM_CARD_DIR=$(APP_PRIVATE_DIR)/simcard_app
APP_API_DBG_DIR=$(APP_PRIVATE_DIR)/API/dbg
APP_API_CMD_PPE_DIR=$(APP_PRIVATE_DIR)/API/cmd/ppe
APP_API_CMD_QDMA_DIR=$(APP_PRIVATE_DIR)/API/cmd/qdma
APP_API_CMD_PCIE_DIR=$(APP_PRIVATE_DIR)/API/cmd/pcie
APP_API_CMD_CPU_INTERRUPT_DIR=$(APP_PRIVATE_DIR)/API/cmd/cpu_interrupt
APP_API_CMD_FE_DIR=$(APP_PRIVATE_DIR)/API/cmd/fe
APP_API_CMD_UART_DIR=$(APP_PRIVATE_DIR)/API/cmd/uart
APP_API_CMD_SWITCH_DIR=$(APP_PRIVATE_DIR)/API/cmd/switch
APP_API_CMD_IFC_DIR=$(APP_PRIVATE_DIR)/API/cmd/ifc
APP_API_LIB_DIR=$(APP_PRIVATE_DIR)/API/lib
APP_API_LIB_OUTPUT_DIR=$(LIB_INSTALL_DIR)
APP_API_LIB_ATM_DIR=$(APP_API_LIB_DIR)/atm
APP_API_LIB_VDSL_DIR=$(APP_API_LIB_DIR)/vdsl
APP_API_LIB_PPE_DIR=$(APP_API_LIB_DIR)/ppe
APP_API_LIB_QDMA_DIR=$(APP_API_LIB_DIR)/qdma
APP_API_LIB_PCIE_DIR=$(APP_API_LIB_DIR)/pcie
APP_API_LIB_CPU_INTERRUPT_DIR=$(APP_API_LIB_DIR)/cpu_interrupt
APP_API_LIB_QOSRULE_DIR=$(APP_API_LIB_DIR)/qosrule
APP_API_LIB_SWITCH_DIR=$(APP_API_LIB_DIR)/switch
APP_API_LIB_ECNT_IGMP_DIR=$(APP_API_LIB_DIR)/ecnt_igmp
APP_API_LIB_DBG_DIR=$(APP_API_LIB_DIR)/dbg
APP_API_LIB_IFC_DIR=$(APP_API_LIB_DIR)/ifc
export APP_API_LIB_DIR
export APP_API_LIB_OUTPUT_DIR
export APP_API_LIB_ATM_DIR
export APP_API_LIB_VDSL_DIR
export APP_API_LIB_PPE_DIR
export APP_API_LIB_QDMA_DIR
export APP_API_LIB_PCIE_DIR
export APP_API_LIB_CPU_INTERRUPT_DIR
export APP_API_LIB_QOSRULE_DIR
export APP_API_LIB_SWITCH_DIR
export APP_API_LIB_ECNT_IGMP_DIR
export APP_API_LIB_DBG_DIR
export APP_API_LIB_IFC_DIR
JVM_MONITOR_DIR=$(APP_PRIVATE_DIR)/joyme/jvm_monitor
LIBAPI_ACL_DIR=$(APP_API_LIB_DIR)/qos
ACL_CMD_DIR=$(APP_PRIVATE_DIR)/API/cmd/qos
BRIDGE_VLAN_DIR=$(APP_API_LIB_DIR)/bridge_vlan
BRIDGE_CMD_DIR=$(APP_PRIVATE_DIR)/API/cmd/bridge_vlan
APP_ECNT_UTILITY_DIR=$(APP_PRIVATE_DIR)/ecnt_utility
export APP_ECNT_UTILITY_DIR

ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
ifneq ($(strip $(TCSUPPORT_ECNT_MAP_ENHANCE)),)
APP_MAP_ENDIR=$(APP_PRIVATE_DIR)/map_enhance
APP_MAP_DIR=$(APP_MAP_ENDIR)
else
ifneq ($(strip $(TCSUPPORT_EASYMESH_R13)),)
APP_MAP_ENDIR=$(APP_PRIVATE_DIR)/map_v2
APP_MAP_DIR=$(APP_MAP_ENDIR)
else
ifneq ($(strip $(TCSUPPORT_MAP_R2)),)
APP_MAP_ENDIR=$(APP_PRIVATE_DIR)/map_r2
APP_MAP_DIR=$(APP_MAP_ENDIR)
else
APP_MAP_ENDIR=$(APP_PRIVATE_DIR)/map
APP_MAP_DIR=$(APP_PRIVATE_DIR)/map
endif
endif
endif
ifneq ($(strip $(TCSUPPORT_ECNT_MAP_ENHANCE)),)
#APP_MAP_WAPPD_DIR=$(APP_MAP_DIR)/wappd
else
ifneq ($(strip $(TCSUPPORT_EASYMESH_R13)),)
#APP_MAP_WAPPD_DIR=$(APP_MAP_DIR)/wappd_v2
else
APP_MAP_WAPPD_DIR=$(APP_MAP_DIR)/wappd
endif
endif
APP_MAP_1905DAEMON_DIR=$(APP_MAP_DIR)/1905daemon
APP_MAP_LIBMAPD_DIR=$(APP_MAP_DIR)/libmapd
APP_MAP_HAL_MAP_DIR=$(APP_MAP_DIR)/hal_map
APP_MAP_MAPD_DIR=$(APP_MAP_DIR)/mapd
endif
ALINK_MANAGER_DIR=$(APP_PRIVATE_DIR)/alink-mgr
export ALINK_MANAGER_DIR
APP_INSTALL_CMCC2_DIR=$(APP_PRIVATE_DIR)/joyme/CMCC2
export APP_INSTALL_CMCC2_DIR
APP_INSTALL_CFG_MANAGER_DIR=$(APP_PRIVATE_DIR)/cfg_manager_ct
export APP_INSTALL_CFG_MANAGER_DIR
APP_INSTALL_WEBPAGE_DIR=$(APP_PRIVATE_DIR)/webPage
export APP_INSTALL_WEBPAGE_DIR
ifneq ($(strip $(TCSUPPORT_CMCCV2) $(TCSUPPORT_CUC)),)	
MOBILE_MANAGEER_DIR=$(APP_CMCC2_DIR)/mobile-manager
APP_BUNDLELIB_DIR=$(APP_CMCC2_DIR)/bundle_lib
APP_JOYME_DIR=$(APP_CMCC2_DIR)/joyme_lib
else
MOBILE_MANAGEER_DIR=$(APP_JOYME_CT_DIR)/mobile-manager
APP_BUNDLELIB_DIR=$(APP_JOYME_CT_DIR)/bundle_lib
APP_JOYME_DIR=$(APP_JOYME_CT_DIR)/joyme_lib
endif
ifneq ($(strip $(TCSUPPORT_CUC)),)
RMS_SPEEDTEST_DIR=$(APP_JOYME_CUC_DIR)/rms_speedtest
endif
export APP_BUNDLELIB_DIR
export APP_JOYME_DIR
export MOBILE_MANAGEER_DIR


ifneq ($(strip $(TCSUPPORT_STUN)),)
APP_STUN_DIR=$(APP_OPENSOURCE_DIR)/stun/src
endif

#for voip
export VOIP_APP_DIR=$(APP_DIR)/voip_app
export IS_VOIP_APP_DIR_EXIST=$(shell if [ ! -d $(VOIP_APP_DIR) ]; then echo "N";else echo "Y"; fi;)

export BSP_EXT_INSTALL=$(TRUNK_DIR)/install_bsp
export FILESYSTEM_DIR=$(APP_DIR)/fs

export APP_INSTALL=$(APP_DIR)/install
export APP_PRE_FS=$(FILESYSTEM_DIR)
export APP_PRE_INC=$(LIB_INSTALL_DIR)
export APP_PRE_LIB=$(LIB_INSTALL_DIR)
export APP_PRE_FS_LIB=$(BSP_INT_FS)/lib
export APP_PRE_FS_BIN=$(BSP_INT_FS)/bin
export APP_PRE_FS_USR_BIN=$(APP_PRE_FS)/usr/bin
export APP_PRE_FS_USEFS_BIN=$(APP_PRE_FS)/userfs/bin
export APP_PRE_FS_USR_SCRIPT=$(APP_PRE_FS)/usr/script
export APP_PRE_FS_USR_ETC=$(APP_PRE_FS)/usr/etc
export APP_PRE_FS_USEFS=$(APP_PRE_FS)/userfs

export FS_GEN_DIR = $(APP_DIR)/filesystem_sdk
export BSP_EXT_INC=$(BSP_EXT_INSTALL)/inc
export BSP_EXT_BIN=$(BSP_EXT_INSTALL)/bin
export BSP_EXT_BIN_BIN=$(BSP_EXT_INSTALL)/bin/bin
export BSP_EXT_BIN_USR=$(BSP_EXT_INSTALL)/bin/usr/bin
export BSP_EXT_BIN_USEFS=$(BSP_EXT_INSTALL)/bin/userfs/bin
export BSP_EXT_FS=$(BSP_EXT_INSTALL)/fs
export BSP_EXT_LIB=$(BSP_EXT_FS)/lib
export BSP_EXT_FS_BIN=$(BSP_EXT_FS)/bin
export BSP_EXT_FS_USR=$(BSP_EXT_FS)/usr/bin
export BSP_EXT_FS_USEFS=$(BSP_EXT_FS)/userfs/bin
export BSP_EXT_MAKE=$(BSP_EXT_INSTALL)/make
export BSP_EXT_TOOLS=$(BSP_EXT_INSTALL)/tools
export BSP_EXT_TOOLS_DTC=$(BSP_EXT_TOOLS)/dtc-1.4.0
export INSTALL_KERNELHEADER=$(BSP_EXT_INSTALL)/kernel_header
export INSTALL_GLOBALINC=$(BSP_EXT_INSTALL)/global_inc_install
export BSP_EXT_TCLINUX_BUILDER=$(BSP_EXT_INSTALL)/tclinux_builder

PROJECT_APP_DIR=$(TRUNK_DIR)/install_bsp/Project
ifneq ($(strip $(CUSTOM)),)
PROFILE_APP_DIR=$(PROJECT_APP_DIR)/profile/$(CUSTOM)/$(PROFILE)
else
PROFILE_APP_DIR=$(PROJECT_APP_DIR)/profile/$(PROFILE)
endif
export PROJECT_APP_DIR
export PROFILE_APP_DIR
TOOLS_APP_DIR=$(TRUNK_DIR)/install_bsp/tools
export TOOLS_APP_DIR
export XPON_APP_SDK_DIR=$(TRUNK_DIR)/apps/xpon_app
PROJECT_MAKE_INC_DIR=$(TRUNK_DIR)/Project/make_inc
PROJECT_MAKE_INC_APP_DIR=$(PROJECT_APP_DIR)/make_inc
export PROJECT_MAKE_INC_DIR
export PROJECT_MAKE_INC_APP_DIR
export LIB_INSTALL_DIR=$(TRUNK_DIR)/install_bsp/lib_install
export LIB_DIR=$(LIB_INSTALL_DIR)
export APP_OPENSSL_DIR_V1=$(LIB_INSTALL_DIR)
PROJECT_LIB_DIR=$(TRUNK_DIR)/Project/lib
PROJECT_LIB_APP_DIR=$(PROJECT_APP_DIR)/lib
export PROJECT_LIB_DIR
export PROJECT_LIB_APP_DIR

export IS_APPS_DIR_EXIST=$(shell if [ ! -d $(APP_DIR) ]; then echo "N";else echo "Y"; fi;)
ifeq ("$(IS_APPS_DIR_EXIST)", "N")
XPON_APP_DIR=$(XPON_APP_BSP_DIR)
else
XPON_APP_DIR=$(XPON_APP_SDK_DIR)
endif
export XPON_SDK_APP_PON_SVC_DIR=$(XPON_APP_DIR)/pon_service

TOOLS_TRX_APP_DIR=$(TOOLS_APP_DIR)/trx
export TOOLS_TRX_APP_DIR
SPI_NAND_ECC_GEN=$(TOOLS_APP_DIR)/spi_nand_ctrl_ecc

APP_BSP_LIB_DIR=$(LIB_INSTALL_DIR)
export APP_BSP_LIB_DIR
MBEDTLS=$(TRUNK_DIR)/tools/mbedtls-2.5.1
export MBEDTLS

VERSION_APP_DIR=$(APP_PRIVATE_DIR)/version
export VERSION_APP_DIR

export RELEASE_APP_DIR=$(TRUNK_DIR)/release_app
BIN_BAK_PATH=$(TRUNK_DIR)
