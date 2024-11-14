cmake_minimum_required(VERSION 2.6)

PROJECT(cfg_clisvc C)
ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99  -Wmissing-declarations)

SET(WORKPATH $ENV{APP_PUBLIC_DIR})

INCLUDE_DIRECTORIES(${WORKPATH}/ubus/svchost/install/include)
INCLUDE_DIRECTORIES(${WORKPATH}/ubus/ubus-2015-05-25/install/include)
INCLUDE_DIRECTORIES(${WORKPATH}/ubus/libubox-2015-06-14/install/include)
INCLUDE_DIRECTORIES(${WORKPATH}/mxml-2.4)

LINK_DIRECTORIES(${WORKPATH}/ubus/svchost/install/lib)
LINK_DIRECTORIES(${WORKPATH}/ubus/ubus-2015-05-25/install//lib)
LINK_DIRECTORIES(${WORKPATH}/ubus/libubox-2015-06-14/install/lib)
LINK_DIRECTORIES(${WORKPATH}/mxml-2.4)
LINK_DIRECTORIES(${WORKPATH}/ubus/service/cfg)

#ADD_LIBRARY(shm_mgr SHARED cfg_shm.c)
#TARGET_LINK_LIBRARIES(shm_mgr hostservice ubus ubox)

ADD_LIBRARY(cfg_clisvc SHARED cfg_shm.c cfg_cli.c cfg_svc.c cfg_obj.c cfg_type.c cfg_xml.c)
TARGET_LINK_LIBRARIES(cfg_clisvc libcfg_types.a)
TARGET_LINK_LIBRARIES(cfg_clisvc hostservice mxml)

ADD_EXECUTABLE(cfg cfg_cmd.c)
TARGET_LINK_LIBRARIES(cfg cfg_clisvc)

INSTALL(TARGETS cfg_clisvc 
	LIBRARY DESTINATION service
)
INSTALL(TARGETS cfg
	RUNTIME DESTINATION ../../install/bin
)

