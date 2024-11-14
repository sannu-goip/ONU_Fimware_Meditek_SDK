#ifndef _MESH_MAP_FILE_IO_H_
#define _MESH_MAP_FILE_IO_H_

#include <svchost_api.h> 

int mesh_boot(void);
int restart_map(void);
int update_dat_evt_handle(void);
int mesh_debug(struct host_service_pbuf* pbuf);
int mesh_renew_bssinfo(struct host_service_pbuf* pbuf);
#endif

