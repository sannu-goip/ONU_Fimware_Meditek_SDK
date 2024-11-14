INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_INC = $(INSTALL_DIR)/include
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin
SERVICE_TYPE_DIR = $(APP_CFG_NG_DIR)/service/cfg/type

CFLAGS += $(CFG_COMMON_CFLAGS) -I$(SERVICE_TYPE_DIR) $(LIBINSTALL_CFLAGS) $(KERNELHEAD_CFLAGS)

CENTRAL_CTRL_LIB_LIBS += $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) $(ECNT_EVENT_LIB) -lm -lapi_lib_cputempmgr $(BLLIB_SYSTEM) $(BLLIB_TRAFFIC) $(BLLIB_XDSL)
ifneq ($(strip $(TCSUPPORT_WLAN)),)
CENTRAL_CTRL_LIB_LIBS += -lapi_lib_wifimgr_user
endif
CENTRAL_CTRL_LIB_OBJS = device_alarm.o lan_port_link.o msg_netlink.o check_ping_diagnostic.o conntrack_delete.o active_notify.o\
                        central_ctrl_common.o check_button.o central_ctrl_svc.o central_ctrl_mgr.o central_ctrl_polling.o wlan_link.o dbus_timer.o lanhost2_mgr_notify.o system_message_notify.o \
						central_ctrl_wan_evt.o

ifneq ($(strip $(TCSUPPORT_CT_L2TP_VPN)),)
CENTRAL_CTRL_LIB_OBJS += check_vpn_ppptunnels.o 
endif

ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
CENTRAL_CTRL_LIB_OBJS += uloop_timer.o
CENTRAL_CTRL_LIB_OBJS += mesh_trigger.o
endif

ifneq ($(strip $(TCSUPPORT_CT_DSL_EX)),)
CENTRAL_CTRL_LIB_OBJS += xdsl.o 
endif

CENTRAL_CTRL_LIB = libcentral_ctrl_svc.so

TARGETS =  $(CENTRAL_CTRL_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS)   -c $<


$(CENTRAL_CTRL_LIB):   $(CENTRAL_CTRL_LIB_OBJS)
	$(CC)   -shared   -o  $@   $^   $(CENTRAL_CTRL_LIB_LIBS)
	
clean:
	-rm  *.o   $(CENTRAL_CTRL_LIB)

install:
	cp -f  $(CENTRAL_CTRL_LIB)  $(INSTALL_LIB)
	if test -d $(INSTALL_LIB);\
	then cp -f  $(CENTRAL_CTRL_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(INSTALL_LIB);\
			cp -f  $(CENTRAL_CTRL_LIB)   $(INSTALL_LIB);\
	fi
