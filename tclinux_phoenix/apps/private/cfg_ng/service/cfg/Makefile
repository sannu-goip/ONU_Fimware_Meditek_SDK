
MXML_DIR = $(APP_PUBLIC_DIR)/mxml-2.4
INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin


CFLAGS += -Wall  -fPIC -O2   -I$(INSTALL_DIR)/include   -I$(MXML_DIR)  -I$(APP_MXML_DIR) $(LIBINSTALL_CFLAGS) 

CFG_LIB_LIBS = $(LIBINSTALL_LDFLAGS) -lcrypt -L$(INSTALL_LIB)  -lhostservice  -L./ -lcfg_types -L$(MXML_DIR) -lmxml $(BLLIB_SYSTEM) $(BLLIB_PERFORM) $(BLLIB_TRAFFIC) $(BLLIB_XDSL) $(BLLIB_TC_PARTITION) 

ifneq ($(TCSUPPORT_WLAN), )
CFG_LIB_LIBS += -lapi_lib_wifimgr_user 
endif
CFG_EXE_LIBS = $(LIBINSTALL_LDFLAGS) -L$(INSTALL_LIB)  -lubox -lubus  -lhostservice  -L./ -lcfg_clisvc -L$(MXML_DIR) $(BLLIB_SYSTEM) $(BLLIB_PERFORM) $(BLLIB_TRAFFIC) $(BLLIB_XDSL) $(BLLIB_TC_PARTITION) 

ifneq ($(TCSUPPORT_WLAN), )
CFG_EXE_LIBS += -lapi_lib_wifimgr_user 
endif
CFG_EXE_OBJS =  cfg_cmd.o

CFG_LIB_OBJS =   cfg_shm.o cfg_xml.o  cfg_type.o cfg_obj.o cfg_cli.o  cfg_svc.o cfg_msg.o cfg_romfile.o utility.o hashmap.o queue_api.o 

CFG_LIB_RELEASEOBJS = cfg_cmd.o cfg_type.o cfg_obj.o cfg_cli.o cfg_svc.o cfg_msg.o cfg_romfile.o utility.o hashmap.o queue_api.o 

CFG_EXE = cfg 

CFG_LIB = libcfg_clisvc.so

CFG_TYPES_LIB = libcfg_types.a

TARGETS =  $(CFG_LIB) $(CFG_EXE)

ifneq ($(TC_CFLAGS), )
		CFLAGS+=$(TC_CFLAGS)
endif

LIBS =$(LIBINSTALL_LDFLAGS) $(BLLIB_TRAFFIC)
ifneq ($(strip $(TCSUPPORT_CMCCV2) $(TCSUPPORT_CUC)),)
LIBS += -lapi_lib_flowmgr -lecnt_lib_dbg $(ECNT_EVENT_LIB) 
endif

ifneq ($(TCSUPPORT_CHARSET_CHANGE),)
LIBS += $(ECNT_UTILITY_LIB)
endif

ifneq ($(strip $(TCSUPPORT_WAN_GPON) $(TCSUPPORT_WAN_EPON)),)
ifeq ($(TCSUPPORT_NP), )
LIBS+= -L$(LIB_INSTALL_DIR) -lxpon
endif
endif

ifneq ($(strip $(TCSUPPORT_WLAN_APCLIENT)),)
CFG_LIB_LIBS += -lapi_lib_wifimgr 
endif

.SUFFIXES:  .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<


all: mxml  cfg_lib cfg_exe

mxml:
	$(MAKE)  -C  $(MXML_DIR) 


cfg_type_lib:
	$(MAKE)  -C  ./type
	cp  -f ./type/$(CFG_TYPES_LIB)  ./

cfg_lib:  cfg_type_lib  $(CFG_LIB_OBJS) 
	$(CC)   -shared   -o  $(CFG_LIB)  $(CFG_TYPES_LIB) $(CFG_LIB_OBJS) $(CFG_LIB_LIBS) $(LIBS) 


cfg_exe: $(CFG_EXE_OBJS)
	$(CC)   -o  $(CFG_EXE) $^   $(CFG_EXE_LIBS) $(LIBS) 


install:
	if test -d $(INSTALL_LIB);\
	then cp -f $(CFG_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(INSTALL_LIB)\
			cp -f $(CFG_LIB) $(INSTALL_LIB);\
	fi
	
	cp -f $(CFG_LIB) $(LIB_INSTALL_DIR) 
	cp -rf cfg_msg.h $(LIB_INSTALL_DIR)
	cp -rf hashmap.h $(LIB_INSTALL_DIR) 	
	cp -rf queue_api.h $(LIB_INSTALL_DIR) 
	
	if test -d $(INSTALL_BIN);\
	then cp -f $(CFG_EXE)  $(INSTALL_BIN);\
	else mkdir -p $(INSTALL_BIN);\
			cp -f $(CFG_EXE)  $(INSTALL_BIN);\
	fi

	
clean:
	$(MAKE)  -C  ./type clean
	-rm  $(CFG_LIB_RELEASEOBJS)  $(CFG_LIB)  $(CFG_EXE)  $(CFG_TYPES_LIB)
