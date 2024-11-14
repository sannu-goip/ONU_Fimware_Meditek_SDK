INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin
SERVICE_TYPE_DIR = $(APP_CFG_NG_DIR)/service/cfg/type

RPATH_LINK =  -Wl,-rpath-link,$(LIB_INSTALL_DIR)

CFLAGS += $(CFG_COMMON_CFLAGS) -I$(SERVICE_TYPE_DIR) $(LIBINSTALL_CFLAGS) 

MESH_LIB_LIBS = $(CFG_COMMON_LIBS) $(LIBINSTALL_LDFLAGS) -lmapd_interface_client -lapi_lib_wifimgr_user

CFG_TYPE_LIB = -L../cfg -llibcfg_types.a

MESH_LIB_OBJS = mesh_map_mgr.o mesh_map_svc.o mesh_map_cfg.o mesh_map_common.o mesh_map_file_io.o mesh_action.o mesh_monitor_msg.o

MESH_LIB = libmesh_svc.so

TARGETS =  $(MESH_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS) $(CFG_TYPE_LIB) -c $<  

$(MESH_LIB):  $(MESH_LIB_OBJS)
	$(CC)   -shared  $(RPATH_LINK) -o  $@   $^  $(MESH_LIB_LIBS)

clean:
	-rm *.o $(MESH_LIB)

install:
	if test -d $(INSTALL_LIB);\
	then cp -f  $(MESH_LIB)   $(INSTALL_LIB);\
	else mkdir -p $(INSTALL_LIB);\
			cp -f  $(MESH_LIB)   $(INSTALL_LIB);\
	fi
