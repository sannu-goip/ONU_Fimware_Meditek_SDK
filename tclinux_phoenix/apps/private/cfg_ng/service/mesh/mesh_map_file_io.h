#ifndef _MESH_MAP_FILE_IO_H_
#define _MESH_MAP_FILE_IO_H_

#include "mesh_map_common.h"

int write_file_mesh_wapp_cfg();

int write_file_mesh_map_cfg();
int write_file_mesh_mapd_cfg();
int write_file_mesh_steer_cfg();

typedef struct _mesh_name_value_type_file_attr_
{
	const char *attr_nodeName;
	const char *attr_attribute;
	int attr_type;
	const char *attr_file;
}mesh_name_value_type_file_attr;


int write_file_mesh_wts_bss_info_config(mesh_config_bss_info info[], int bss_num);
#endif



