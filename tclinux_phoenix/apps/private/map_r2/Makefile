ROOTPATH=$(shell pwd)
include $(ROOTPATH)/ecnt-map.mk
include ../apps-function.mk

ecnt_map=hal_map  1905daemon libmapd mapd
ecnt_map_clean=hal_map_clean  1905daemon_clean libmapd_clean mapd_clean


all:$(ecnt_map)


	echo "$(APP_MAP_DIR)********************************"
	echo "$(APP_MAP_ENHANCE_DIR)********************************"
	echo "$(APP_MAP_HAL_MAP_DIR)********************************"


###########################################################################################
ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
hal_map:
	echo "Build hal_map"
	$(MAKE) -C $(APP_MAP_HAL_MAP_DIR)
	if test -e $(APP_MAP_HAL_MAP_DIR)/libmap_hal.so; \
	then echo "Compile hal_map success"; \
	else echo "Compile hal_map error!"; exit 1; \
	fi

hal_map_clean:
	echo "Clean hal_map"
	$(MAKE) -C $(APP_MAP_HAL_MAP_DIR) clean

else
hal_map:
hal_map_clean:
endif

###########################################################################################

###########################################################################################
ifneq ($(strip $(TCSUPPORT_MAP_1905DAEMON)),)
1905daemon:
	echo "Build 1905daemon"
ifneq ($(strip $(TC_BUILD_RELEASECODE)),)
	echo "TC_BUILD_RELEASECODE"
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/userfs/bin/p1905_managerd $(FILESYSTEM_DIR)/userfs/bin/
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/userfs/bin/1905ctrl $(FILESYSTEM_DIR)/userfs/bin/
	cp -rf $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/lib/libeth_1905ops.so $(FILESYSTEM_DIR)/lib/
	cp -rf $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/lib/libeth_1905ops.so $(LIB_INSTALL_DIR)/
else
	$(MAKE) -C $(APP_MAP_1905DAEMON_DIR)
	if test -e $(APP_MAP_1905DAEMON_DIR)/p1905_managerd; \
	then echo "Compile 1905daemon success"; \
	else echo "Compile 1905daemon error!"; exit 1; \
	fi
	cp $(APP_MAP_1905DAEMON_DIR)/p1905_managerd  $(FILESYSTEM_DIR)/userfs/bin/
	cp $(APP_MAP_1905DAEMON_DIR)/1905ctrl  $(FILESYSTEM_DIR)/userfs/bin/
	$(call EcntAPPReleaseUsrfsBinInstall, $(APP_MAP_1905DAEMON_DIR), p1905_managerd)
	$(call EcntAPPReleaseUsrfsBinInstall, $(APP_MAP_1905DAEMON_DIR), 1905ctrl)
	cp -rf $(APP_MAP_1905DAEMON_DIR)/ethernet/libeth_1905ops.so $(FILESYSTEM_DIR)/lib/		
	cp -rf $(APP_MAP_1905DAEMON_DIR)/ethernet/libeth_1905ops.so $(LIB_INSTALL_DIR)/
	$(call EcntAPPReleaseLibInstall, $(APP_MAP_1905DAEMON_DIR)/ethernet/, libeth_1905ops.so)
endif
		
1905daemon_clean:
	echo "Clean 1905daemon"
ifneq ($(strip $(TC_BUILD_RELEASECODE)),)
	echo "TC_BUILD_RELEASECODE"
else
	$(MAKE) -C $(APP_MAP_1905DAEMON_DIR) clean
endif
else
1905daemon:
1905daemon_clean:
endif
###########################################################################################
ifneq ($(strip $(TCSUPPORT_MAP_LIBMAPD)),)
libmapd:
	echo "Build libmapd"
ifneq ($(strip $(TC_BUILD_RELEASECODE)),)
	echo "TC_BUILD_RELEASECODE"
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/lib/libmapd_interface_client.so $(FILESYSTEM_DIR)/lib/
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/lib/libmapd_interface_client.so $(LIB_INSTALL_DIR)/
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/apps/private/map_r2/libmapd/include/mapd_interface_ctrl.h $(LIB_INSTALL_DIR)/
#	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/apps/private/map_enhance/libmapd/include/mapd_interface_cust.h $(LIB_INSTALL_DIR)/
else
	$(MAKE) -C $(APP_MAP_LIBMAPD_DIR)
	if test -e $(APP_MAP_LIBMAPD_DIR)/libmapd_interface_client.so; \
	then echo "Compile libmapd success"; \
	else echo "Compile libmapd error!"; exit 1; \
	fi
	cp -rf $(APP_MAP_LIBMAPD_DIR)/libmapd_interface_client.so $(FILESYSTEM_DIR)/lib/
	cp -rf $(APP_MAP_LIBMAPD_DIR)/libmapd_interface_client.so $(LIB_INSTALL_DIR)/
	cp -rf $(APP_MAP_LIBMAPD_DIR)/include/mapd_interface_ctrl.h $(LIB_INSTALL_DIR)/
#	cp -rf $(APP_MAP_LIBMAPD_DIR)/include/mapd_interface_cust.h $(LIB_INSTALL_DIR)/
	$(call EcntAPPReleaseLibInstall, $(APP_MAP_LIBMAPD_DIR), libmapd_interface_client.so)
	$(call EcntAPPReleaseInstall, $(APP_MAP_LIBMAPD_DIR)/include, mapd_interface_ctrl.h, $(RELEASE_APP_DIR)/$(TCPLATFORM)/apps/private/map_r2/libmapd/include)	
#	$(call EcntAPPReleaseInstall, $(APP_MAP_LIBMAPD_DIR)/include, mapd_interface_cust.h, $(RELEASE_APP_DIR)/$(TCPLATFORM)/apps/private/map_enhance/libmapd/include)	
endif	

libmapd_clean:
	echo "Clean libmapd"
ifneq ($(strip $(TC_BUILD_RELEASECODE)),)
	echo "TC_BUILD_RELEASECODE"
else
	$(MAKE) -C $(APP_MAP_LIBMAPD_DIR) clean
endif
else
libmapd:
libmapd_clean:
endif
###########################################################################################
ifneq ($(strip $(TCSUPPORT_MAP_MAPDAEMON)),)
mapd:
	echo "Build wsq  map daemon"
ifneq ($(strip $(TC_BUILD_RELEASECODE)),)
	echo "TC_BUILD_RELEASECODE"
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/userfs/bin/mapd $(FILESYSTEM_DIR)/userfs/bin/
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/userfs/bin/mapd_cli $(FILESYSTEM_DIR)/userfs/bin/
	cp $(RELEASE_APP_DIR)/$(TCPLATFORM)/filesystem/userfs/bin/bs20  $(FILESYSTEM_DIR)/userfs/bin/
else
	echo "Build wsq 120  map daemon"
	$(MAKE) -C $(APP_MAP_MAPD_DIR)
	if test -e $(APP_MAP_MAPD_DIR)/mapd; \
	then echo "Compile map daemon success"; \
	else echo "Compile map daemon error!"; exit 1; \
	fi
	echo "Build wsq 126  map daemon"

	cp  $(APP_MAP_MAPD_DIR)/mapd  $(FILESYSTEM_DIR)/userfs/bin/
	cp  $(APP_MAP_MAPD_DIR)/mapd_cli  $(FILESYSTEM_DIR)/userfs/bin/
	cp  $(APP_MAP_MAPD_DIR)/bs20  $(FILESYSTEM_DIR)/userfs/bin/
	echo "Build wsq 131  map daemon"

	$(call EcntAPPReleaseUsrfsBinInstall, $(APP_MAP_MAPD_DIR), mapd)
	$(call EcntAPPReleaseUsrfsBinInstall, $(APP_MAP_MAPD_DIR), mapd_cli)
	$(call EcntAPPReleaseUsrfsBinInstall, $(APP_MAP_MAPD_DIR), bs20)
endif

mapd_clean:
	echo "Clean map daemon"
ifneq ($(strip $(TC_BUILD_RELEASECODE)),)
	echo "TC_BUILD_RELEASECODE"
else
	$(MAKE) -C $(APP_MAP_MAPD_DIR) clean
endif
else
mapd:
mapd_clean:
endif

clean: $(ecnt_map_clean)
	echo clean map
	
.PHONY:$(ecnt_map) $(ecnt_map_clean)

	
	
	
	
	
	
	
	
