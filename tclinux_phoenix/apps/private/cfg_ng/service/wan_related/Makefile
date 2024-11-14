INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin

CFLAGS += -Werror $(CFG_COMMON_CFLAGS) $(LIBINSTALL_CFLAGS)  -I$(INSTALL_KERNELHEADER) -I$(INSTALL_GLOBALINC)/uapi/modules

WAN_RELATED_LIBS = $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) -lapi_lib_qosrule $(ECNT_EVENT_LIB) $(BLLIB_TRAFFIC) $(BLLIB_PERFORM) $(ECNT_UTILITY_LIB) $(BLLIB_SYSTEM)


WAN_RELATED_LIB_OBJS = wan_related_mgr.o wan_related_svc.o wan_related_cfg.o wan_related_conf.o \
											wan_related_utility.o wan_related_qos_cfg.o wan_related_maintenance.o ping_diagnostic.o \
											wan_related_vpn_cfg.o wan_related_acl_cfg.o
ifneq ($(strip $(TCSUPPORT_CT_JOYME4) $(TCSUPPORT_CT_UBUS)),)
WAN_RELATED_LIB_OBJS += filter_polling.o
endif

ifneq ($(strip $(TCSUPPORT_VPN)),)
WAN_RELATED_LIB_OBJS += wan_related_ipsec.o
endif

ifneq ($(strip $(TCSUPPORT_VXLAN)),)
WAN_RELATED_LIB_OBJS += wan_related_vxlan_link_list.o wan_related_vxlan_cfg.o
WAN_RELATED_LIBS += -lapi_lib_portbind
endif

WAN_RELATED_LIB = libwan_related_svc.so

TARGETS =  $(WAN_RELATED_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS)   -c $<


$(WAN_RELATED_LIB):   $(WAN_RELATED_LIB_OBJS)
	$(CC)   -shared   -o  $@   $^   $(WAN_RELATED_LIBS)
	
clean:
	-rm  *.o   $(WAN_RELATED_LIB)

install:
	if test -d $(INSTALL_LIB);\
	then cp -f  $(WAN_RELATED_LIB)   $(INSTALL_LIB);\
	else mkdir -p ../install/lib;\
			cp -f  $(WAN_RELATED_LIB)   $(INSTALL_LIB);\
	fi
