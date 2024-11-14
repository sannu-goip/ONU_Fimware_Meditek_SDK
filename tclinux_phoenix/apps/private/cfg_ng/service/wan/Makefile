INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_INC = $(INSTALL_DIR)/include
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin

CFLAGS = $(CFG_COMMON_CFLAGS) -I$(APP_CMDCI_DIR) $(LIBINSTALL_CFLAGS)

WAN_LIB_LIBS = $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) -lapi_lib_portbind $(ECNT_EVENT_LIB) $(BLLIB_PERFORM) $(BLLIB_TRAFFIC) $(ECNT_UTILITY_LIB) 

WAN_LIB_OBJS = internet_connect.o update_ipv6_gateway.o wan_polling.o dslite_info.o update_child_prefix.o \
               dhcp_port_filter_info.o dhcp_relay_info.o wan_protocol_handle.o full_route.o misc_srv.o wan_nat.o \
               policy_route.o static_route.o dns_info.o pd_info.o default_route_handle.o msg_notify.o wan_common.o \
               wan_link_list.o port_binding.o wan_mgr.o wan_svc.o wan_cfg.o wan_interface.o vlan_binding.o vlan_eth_pair.o

ifneq ($(strip $(TCSUPPORT_CT_DSL_EX)),)
WAN_LIB_OBJS += wan_xdsl.o
endif

ifneq ($(strip $(TCSUPPORT_NP_CMCC)),)
WAN_LIB_OBJS += np_wan_handle.o
endif

ifneq ($(strip $(TCSUPPORT_CT_SDN)),)
WAN_LIB_OBJS += sdn_ovs.o
endif

WAN_LIB = libwan_svc.so

TARGETS =  $(WAN_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS)   -c $<


$(WAN_LIB):   $(WAN_LIB_OBJS)
	$(CC)   -shared   -o  $@   $^   $(WAN_LIB_LIBS)
	
clean:
	-rm  *.o   $(WAN_LIB)

install:
	if test -d $(INSTALL_LIB);\
	then cp -f  $(WAN_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(INSTALL_LIB);\
			cp -f  $(WAN_LIB)   $(INSTALL_LIB);\
	fi
