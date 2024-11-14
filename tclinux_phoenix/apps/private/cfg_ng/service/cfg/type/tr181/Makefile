CFLAGS += -fPIC -Wall  -O2  -I$(INSTALL_INC)  -I$(SERVICE_CFG_DIR)  $(LIBINSTALL_CFLAGS) -I$(TOOLS_TRX_DIR) -I$(APP_MXML_DIR) -I$(SERVICE_CFG_DIR)/type
ifneq ($(TC_CFLAGS), )
CFLAGS+=$(TC_CFLAGS)
endif

CFG_TYPES_TR181_LIB = cfg_type_tr181.o 
CFG_TYPES_TR181_LIB += cfg_type_tr181ip.o
CFG_TYPES_TR181_LIB += cfg_type_tr181ppp.o 
CFG_TYPES_TR181_LIB += cfg_type_EtherVlanT.o 
CFG_TYPES_TR181_LIB += cfg_type_EtherLink.o 
CFG_TYPES_TR181_LIB += cfg_type_Optical.o 
CFG_TYPES_TR181_LIB += cfg_type_ipdiag.o 
CFG_TYPES_TR181_LIB += cfg_type_users.o 
CFG_TYPES_TR181_LIB += cfg_type_devinfloc.o 
CFG_TYPES_TR181_LIB += cfg_type_mgrser.o 
CFG_TYPES_TR181_LIB += cfg_type_ether.o 
CFG_TYPES_TR181_LIB += cfg_type_bridging.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4Rly.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4Server.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4ServerPool.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4SvrPoolClient.o  
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4SvrPoolClientIp4Addr.o  
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4ServerPoolStaAddr.o
CFG_TYPES_TR181_LIB += cfg_type_bridge.o 
CFG_TYPES_TR181_LIB += cfg_type_bridgevlan.o 
CFG_TYPES_TR181_LIB += cfg_type_bridgevlanport.o 
CFG_TYPES_TR181_LIB += cfg_type_bridgefilter.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4ClientReqOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4ClientSentOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4ServerPoolClientOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4ServerPoolOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6Server.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ServerPool.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ClientRcvOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ClientSentOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ClientServer.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ServerPoolClient.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ServerPoolClientIp6Addr.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ServerPoolClientIp6Prev.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ServerPoolClientOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6ServerPoolOpt.o 
CFG_TYPES_TR181_LIB += cfg_type_Wifi.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiApAcc.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiApAss.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiEp.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiEpProSec.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiEpSec.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiEpSta.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiEpWps.o
CFG_TYPES_TR181_LIB += cfg_type_WifiRdoSta.o 
CFG_TYPES_TR181_LIB += cfg_type_WifiSsidSta.o 
CFG_TYPES_TR181_LIB += cfg_type_RouteInfo.o 
CFG_TYPES_TR181_LIB += cfg_type_TQos.o 
CFG_TYPES_TR181_LIB += cfg_type_tqosapp.o
CFG_TYPES_TR181_LIB += cfg_type_tqosclsficn.o 
CFG_TYPES_TR181_LIB += cfg_type_tqospolicer.o 
CFG_TYPES_TR181_LIB += cfg_type_tqosque.o 
CFG_TYPES_TR181_LIB += cfg_type_QosFlow.o 
CFG_TYPES_TR181_LIB += cfg_type_QosQueSta.o 
CFG_TYPES_TR181_LIB += cfg_type_QosShaper.o 
CFG_TYPES_TR181_LIB += cfg_type_host.o 
CFG_TYPES_TR181_LIB += cfg_type_dns.o 
CFG_TYPES_TR181_LIB += cfg_type_dns_forwarding.o 
CFG_TYPES_TR181_LIB += cfg_type_routerAdv_ItfSetting_opt.o 
CFG_TYPES_TR181_LIB += cfg_type_userinterface.o 
CFG_TYPES_TR181_LIB += cfg_type_captiveportal.o 
CFG_TYPES_TR181_LIB += cfg_type_perdsts.o 
CFG_TYPES_TR181_LIB += cfg_type_perdsts_samp.o 
CFG_TYPES_TR181_LIB += cfg_type_ether_rmonstats.o 
CFG_TYPES_TR181_LIB += cfg_type_tr181rip.o 
CFG_TYPES_TR181_LIB += cfg_type_devinfps.o 
CFG_TYPES_TR181_LIB += cfg_type_devinftem.o  
CFG_TYPES_TR181_LIB += cfg_type_tr181ipap.o 
CFG_TYPES_TR181_LIB += cfg_type_InterfaceStack.o
CFG_TYPES_TR181_LIB += cfg_type_tr181IPv4Addr.o
CFG_TYPES_TR181_LIB += cfg_type_tr181IPv6Addr.o
CFG_TYPES_TR181_LIB += cfg_type_tr181IPV6PRE.o
CFG_TYPES_TR181_LIB += cfg_type_NatInterface.o
CFG_TYPES_TR181_LIB += cfg_type_DHCPv4Client.o
CFG_TYPES_TR181_LIB += cfg_type_DHCPv6Client.o
CFG_TYPES_TR181_LIB += cfg_type_NatPortMap.o
CFG_TYPES_TR181_LIB += cfg_type_RouteForward4.o
CFG_TYPES_TR181_LIB += cfg_type_RouteForward6.o
CFG_TYPES_TR181_LIB += cfg_type_DnsClient.o
CFG_TYPES_TR181_LIB += cfg_type_BridgePort.o
CFG_TYPES_TR181_LIB += cfg_type_RouteAdvIfSet.o
CFG_TYPES_TR181_LIB += cfg_type_routeAdv.o
CFG_TYPES_TR181_LIB += cfg_type_tr181dslite.o
CFG_TYPES_TR181_LIB += cfg_tr181_common.o 
CFG_TYPES_TR181_LIB += cfg_type_tr181route.o
CFG_TYPES_TR181_LIB += cfg_type_DnsClient.o
CFG_TYPES_TR181_LIB += cfg_type_firewallch.o
CFG_TYPES_TR181_LIB += cfg_type_TR181Firewall.o
CFG_TYPES_TR181_LIB += cfg_type_FirewallLev.o
CFG_TYPES_TR181_LIB += cfg_type_WifiEpPro.o
CFG_TYPES_TR181_LIB += cfg_type_mgrser_dld_group.o 
CFG_TYPES_TR181_LIB += cfg_type_EtherInterface.o 
CFG_TYPES_TR181_LIB += cfg_type_WIFIAP.o
CFG_TYPES_TR181_LIB += cfg_type_WIFIAPSec.o
CFG_TYPES_TR181_LIB += cfg_type_WIFIAPWPS.o
CFG_TYPES_TR181_LIB += cfg_type_WIFIRdo.o
CFG_TYPES_TR181_LIB += cfg_type_WIFISsid.o
CFG_TYPES_TR181_LIB += cfg_type_tr181WIFI_common.o
CFG_TYPES_TR181_LIB += cfg_type_tr181IPv6rd.o
ifneq ($(strip $(TCSUPPORT_CT_DSL_EX)),)
CFG_TYPES_TR181_LIB += cfg_type_tr181_atmlink.o
CFG_TYPES_TR181_LIB += cfg_type_tr181_dslChannel.o
CFG_TYPES_TR181_LIB += cfg_type_tr181_dslLine.o
CFG_TYPES_TR181_LIB += cfg_type_tr181_ptmlink.o
endif

all: $(CFG_TYPES_TR181_LIB)

%.o:%.c
	$(CC)  $(CFLAGS) $(TC_CFLAGS)  -c $<

clean: 
	-rm  *.o
