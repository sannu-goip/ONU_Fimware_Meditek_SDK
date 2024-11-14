INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin
MXML_DIR = $(APP_PUBLIC_DIR)/mxml-2.4

CFLAGS += $(CFG_COMMON_CFLAGS) -I$(MXML_DIR) $(LIBINSTALL_CFLAGS)

VOIP_LIB_LIBS = $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) $(BLLIB_PERFORM)

VOIP_LIB_OBJS = voip_mgr.o voip_polling.o voip_svc.o voip_common.o voip_cfg.o

VOIP_LIB = libvoip_svc.so

TARGETS =  $(VOIP_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS)   -c $<


$(VOIP_LIB):   $(VOIP_LIB_OBJS)
	$(CC)   -shared   -o  $@   $^   $(VOIP_LIB_LIBS)
	
clean:
	-rm  *.o   $(VOIP_LIB)

install:
	if test -d $(INSTALL_LIB);\
	then cp -f  $(VOIP_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(APP_CFG_NG_DIR)/install/lib;\
			cp -f  $(VOIP_LIB)   $(INSTALL_LIB);\
	fi
