INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin
SERVICE_TYPE_DIR = $(APP_CFG_NG_DIR)/service/cfg/type

RPATH_LINK =  -Wl,-rpath-link,$(LIB_INSTALL_DIR)

CFLAGS += $(CFG_COMMON_CFLAGS) -I$(SERVICE_TYPE_DIR) $(LIBINSTALL_CFLAGS) -I$(INSTALL_GLOBALINC)/uapi/notify

WLAN_LIB_LIBS = $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) $(ECNT_EVENT_LIB) $(BLLIB_SYSTEM) -lapi_lib_wifimgr -lapi_lib_wifimgr_user

ifneq ($(strip $(TCSUPPORT_ANDLINK)),)
WLAN_LIB_LIBS += -lmtkswitch
endif

CFG_TYPE_LIB = -L../cfg -llibcfg_types.a

WLAN_LIB_OBJS = wlan_mgr.o wlan_svc.o wlan_cfg.o wlan_utils.o
ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
WLAN_LIB_OBJS += wlan_mesh.o
endif 

WLAN_LIB = libwlan_svc.so 

TARGETS =  $(WLAN_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS) $(CFG_TYPE_LIB) -c $<  


$(WLAN_LIB):  $(WLAN_LIB_OBJS)
	$(CC)   -shared  $(RPATH_LINK) -o  $@   $^  $(WLAN_LIB_LIBS) 
	
clean:
	-rm  *.o   $(WLAN_LIB)

install:
	if test -d $(INSTALL_LIB);\
	then cp -f  $(WLAN_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(INSTALL_LIB);\
			cp -f  $(WLAN_LIB)   $(INSTALL_LIB);\
	fi
