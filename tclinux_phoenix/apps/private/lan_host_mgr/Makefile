INCLUDE = $(KERNELHEAD_CFLAGS)	$(LIBINSTALL_CFLAGS)

LIBS = $(GLIB_LIBS) $(LIBINSTALL_LDFLAGS) $(TCAPI_LIB) $(COMPILEOPTION_LIB)  -lpthread -lecntevent -lapi_lib_switchmgr $(ECNT_UTILITY_LIB) $(BLLIB_TRAFFIC)

ifneq ($(strip $(TCSUPPORT_WLAN)),)
LIBS += -lapi_lib_wifimgr
endif

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
else
CFLAGS += -O2
endif

ifneq ($(TC_CFLAGS),)
CFLAGS+=$(TC_CFLAGS)
endif

EXEC = lan_host_mgr
SRC = $(wildcard ./*.c)
OBJS = $(SRC:.c=.o)
OBJS += $(wildcard ./*.o)

all: $(EXEC) install

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

.c.o :
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

install:
	cp -rf $(EXEC) $(FILESYSTEM_DIR)/userfs/bin

clean:
	-rm -f $(OBJS)
	-rm -f $(EXEC)
	-rm -f $(FILESYSTEM_DIR)/userfs/bin/$(EXEC)