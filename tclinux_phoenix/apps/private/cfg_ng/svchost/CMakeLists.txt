cmake_minimum_required(VERSION 2.6)

PROJECT(svchost C)
ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99  -Wmissing-declarations)

SET(WORKPATH $ENV{APP_PUBLIC_DIR})

INCLUDE_DIRECTORIES(${WORKPATH}/ubus/ubus-2015-05-25/install/include)
INCLUDE_DIRECTORIES(${WORKPATH}/ubus/libubox-2015-06-14/install/include)

LINK_DIRECTORIES(${WORKPATH}/ubus/ubus-2015-05-25/install//lib)
LINK_DIRECTORIES(${WORKPATH}/ubus/libubox-2015-06-14/install/lib)


ADD_LIBRARY(hostservice SHARED svchost_api.c)
TARGET_LINK_LIBRARIES(hostservice ubus ubox)

ADD_EXECUTABLE(svchost svchost.c svchost_parse.c)
TARGET_LINK_LIBRARIES(svchost ubus ubox dl pthread)


INSTALL(TARGETS hostservice svchost
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

INSTALL(FILES svchost_evt.h  svchost_id.h svchost_api.h DESTINATION include)
