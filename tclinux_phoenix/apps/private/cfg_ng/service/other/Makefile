INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_INC = $(INSTALL_DIR)/include
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin

CFLAGS += $(CFG_COMMON_CFLAGS) $(LIBINSTALL_CFLAGS) -I$(GLOBAL_INC_DIR)/ecnt_event_global/

ifneq ($(TC_CFLAGS), )
CFLAGS+=$(TC_CFLAGS)
endif

OTHER_LIB_LIBS = $(CFG_COMMON_LIBS) -lcrypt -L $(LIB_INSTALL_DIR) $(ECNT_EVENT_LIB) $(LIBINSTALL_LDFLAGS) -lapi_lib_switchmgr $(BLLIB_SYSTEM) $(BLLIB_PERFORM) $(BLLIB_TRAFFIC)

OTHER_LIB_OBJS = other_mgr.o other_svc.o other_cfg.o other_cfg_v6.o other_utility.o \
                 dhcpd_boot_execute.o dhcp_relay_boot_execute.o parental_common.o parental_boot_execute.o \
                 parental_mac_boot_execute.o parental_ctrl_polling.o other_mgr_polling.o other_snmp_cfg.o
ifneq ($(strip $(TCSUPPORT_CMCCV2) $(TCSUPPORT_CUC)),)
OTHER_LIB_OBJS += other_cfg_transferservices.o
endif

ifneq ($(strip $(TCSUPPORT_CT_PPPOE_EMULATOR) $(TCSUPPORT_CT_IPOE_EMULATOR)),)
OTHER_LIB_OBJS += other_cfg_diagnostics.o 
endif

OTHER_LIB_OBJS += lanhost2_mgr.o

OTHER_LIB = libother_svc.so

TARGETS =  $(OTHER_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS)   -c $<


$(OTHER_LIB):   $(OTHER_LIB_OBJS)
	$(CC)   -shared   -o  $@   $^   $(OTHER_LIB_LIBS)
	
clean:
	-rm  *.o   $(OTHER_LIB)

install:
	if test -d $(INSTALL_LIB);\
	then cp -f  $(OTHER_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(INSTALL_LIB);\
			cp -f  $(OTHER_LIB)   $(INSTALL_LIB);\
	fi
