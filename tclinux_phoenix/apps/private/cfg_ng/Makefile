

.PHONY: all ubox ubus svchost service clean install header hal_itf

UBOX_PATH=$(APP_OPENSOURCE_DIR)/libubox-2019-12-28
UBUS_PATH=$(APP_OPENSOURCE_DIR)/ubus-2020-01-05

INSTALL_DIR = $(APP_CFG_NG_DIR)/install
INSTALL_LIB = $(INSTALL_DIR)/lib
INSTALL_BIN = $(INSTALL_DIR)/bin
export SERVICE_CFG_DIR = $(APP_CFG_NG_DIR)/service/cfg
export INSTALL_INC = $(INSTALL_DIR)/include

export CFG_COMMON_CFLAGS = -Wall -fPIC -O2  -I$(INSTALL_INC) -I$(SERVICE_CFG_DIR) 

export CFG_COMMON_LIBS = -L$(INSTALL_LIB)  -lhostservice  -L$(SERVICE_CFG_DIR) -lcfg_clisvc 

ifneq ($(strip $(COMPILE_DBG)),)
TARGETS:=$(subst :, , $(COMPILE_DBG))
else
TARGETS:=ubox ubus svchost svc_cfg svc_wan svc_wan_related svc_other svc_mesh svc_wlan svc_voip svc_central_ctrl hal_itf tcapi install
endif

all: target_show $(TARGETS)
clean: ubox_clean ubus_clean svchost_clean svc_cfg_clean svc_wan_clean svc_wan_related_clean svc_other_clean svc_mesh_clean svc_wlan_clean svc_voip_clean svc_central_ctrl_clean hal_itf_clean
	$(RM) -r $(INSTALL_DIR)
	
target_show:
	@echo $(TARGETS)
	
ubox:

	if [ -f $(UBOX_PATH)/_is_configure ] ; then \
		echo "already configred..... "; \
	else \
		cd $(UBOX_PATH) && cmake -D CMAKE_INSTALL_PREFIX:PATH=$(UBOX_PATH)/install ./ ; \
		$(MAKE) -C $(UBOX_PATH)  all; \
		$(MAKE) -C $(UBOX_PATH) install; \
		touch $(UBOX_PATH)/_is_configure; \
	fi
	cp -rf $(LIB_JSONC_DIR)/_install/* $(UBOX_PATH)/install/
	rm -rf $(UBOX_PATH)/install/lib/pkgconfig
	cp -rf  $(UBOX_PATH)/install  ./	

ubox_clean:
	cd $(UBOX_PATH) && cmake -D CMAKE_INSTALL_PREFIX:PATH=$(UBOX_PATH)/install ./
	$(MAKE) -C $(UBOX_PATH) clean
	$(RM) -r $(UBOX_PATH)/_is_configure
	$(RM) -r $(UBOX_PATH)/CMakeCache.txt
	$(RM) -r $(UBOX_PATH)/install
		
ubus: ubox	
	cp -f $(APP_CFG_NG_DIR)/svchost/svchost_evt.h $(UBUS_PATH)/msg_ext.h 
	if [ -f $(UBUS_PATH)/_is_configure ] ; then \
		echo "already configred..... "; \
	else \
		cd $(UBUS_PATH)  && cmake -D CMAKE_INSTALL_PREFIX:PATH=$(UBUS_PATH)/install ./ ; \
		$(MAKE) -C  $(UBUS_PATH)  all; \
		$(MAKE) -C  $(UBUS_PATH)  install; \
		touch $(UBUS_PATH)/_is_configure; \
	fi
	cp -rf  $(UBUS_PATH)/install  ./	

ubus_clean:
	cd $(UBUS_PATH) && cmake -D CMAKE_INSTALL_PREFIX:PATH=$(UBUS_PATH)/install ./
	$(MAKE) -C  $(UBUS_PATH)/  clean	
	$(RM) -r $(UBUS_PATH)/_is_configure
	$(RM) -r $(UBUS_PATH)/CMakeCache.txt
	$(RM) -r $(UBUS_PATH)/install
	
svchost: ubus
	$(MAKE) -C ./svchost  all
	$(MAKE) -C ./svchost  install
	
svchost_clean: 
	$(MAKE) -C ./svchost  clean
	
header:
ifneq ($(strip $(TCSUPPORT_WAN_GPON) $(TCSUPPORT_WAN_EPON)),)
ifeq ($(strip $(TCSUPPORT_NP)),)
	cp  $(XPON_SDK_APP_PON_SVC_DIR)/pon_api.h  ./install/include
endif
endif

svc_cfg: svchost header
	$(MAKE) -C ./service/cfg all install

svc_cfg_clean:  	
	$(MAKE) -C ./service/cfg clean
			
svc_wan:
	$(MAKE) -C ./service/wan all
	$(MAKE) -C ./service/wan install

svc_wan_clean:  	
	$(MAKE) -C ./service/wan clean
	
svc_wan_related:
	$(MAKE) -C ./service/wan_related all
	$(MAKE) -C ./service/wan_related install

svc_wan_related_clean:
	$(MAKE) -C ./service/wan_related clean

svc_other:
	$(MAKE) -C ./service/other all
	$(MAKE) -C ./service/other install

svc_other_clean:  	
	$(MAKE) -C ./service/other clean
	
svc_wlan:
ifneq ($(strip $(TCSUPPORT_WLAN)),)
	$(MAKE) -C ./service/wlan all
	$(MAKE) -C ./service/wlan install
else
	echo "Do nothing for svc_wlan."
endif

svc_wlan_clean:
ifneq ($(strip $(TCSUPPORT_WLAN)),)
	$(MAKE) -C ./service/wlan clean
else
	echo "Do nothing for svc_wlan_clean."
endif
	
svc_voip:
ifneq ($(strip $(TCSUPPORT_VOIP)),)
	$(MAKE) -C ./service/voip all
	$(MAKE) -C ./service/voip install
else
	echo "Do nothing for svc_voip."
endif
	
svc_voip_clean:	
ifneq ($(strip $(TCSUPPORT_VOIP)),)
	$(MAKE) -C ./service/voip clean
else
	echo "Do nothing for svc_voip_clean."
endif

svc_central_ctrl:
	$(MAKE) -C ./service/central_ctrl all
	$(MAKE) -C ./service/central_ctrl install

svc_central_ctrl_clean:
	$(MAKE) -C ./service/central_ctrl clean
		
svc_mesh:
ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
	$(MAKE) -C ./service/mesh all
	$(MAKE) -C ./service/mesh install
endif

svc_mesh_clean:
ifneq ($(strip $(TCSUPPORT_ECNT_MAP)),)
	$(MAKE) -C ./service/mesh clean
endif

install:
	cp $(INSTALL_BIN)/*  $(FILESYSTEM_DIR)/userfs/bin
	cp $(INSTALL_LIB)/*  $(FILESYSTEM_DIR)/lib
	cp -rf $(INSTALL_LIB)/*  $(LIB_INSTALL_DIR)
	cp -rf $(INSTALL_INC)/*  $(LIB_INSTALL_DIR)

tcapi:
	$(MAKE) -C ../tcapi_lib clean
	$(MAKE) -C ../tcapi_lib   CFG2_FLAG=1 
	if test -d $(APP_TCAPILIB_DIR)/cfg_ng/;\
	then echo "$(APP_TCAPILIB_DIR)/cfg_ng/ exist "; \
	else \
			mkdir -p $(APP_TCAPILIB_DIR)/cfg_ng/;\
	fi
	
	cp -rf $(APP_TCAPILIB_DIR)/libtcapi.so  $(APP_TCAPILIB_DIR)/cfg_ng/
	cp -rf $(APP_TCAPILIB_DIR)/libtcapi.so.*  $(APP_TCAPILIB_DIR)/cfg_ng/

hal_itf:
	$(MAKE) -C ./hal_itf  all
	
hal_itf_clean:
	$(MAKE) -C ./hal_itf  clean
