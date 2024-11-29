#ifndef _MESH_MAP_ACTION_H_
#define _MESH_MAP_ACTION_H_

#define MAPD_CTRL_FILE  "/tmp/mapd_ctrl"
#define MAP_ALLOW_PKT_PATH 		"/proc/tc3162/map_allow_pkt"

#define DROP_ALL_1905_PKT  (0)
#define ALLOW_ALL_1905_PKT (1)
#define DROP_SOME_1905_PKT (2)


typedef int (*mesh_action_handler)(void);

typedef struct _mesh_action_attr_
{
	const char *attr;
	mesh_action_handler cb;
}mesh_action_attr;

typedef struct _mesh_name_value_attr_
{
	const char *attr_name;
	const char *attr_value;
}mesh_name_value_attr;

int action_evt_handle(void);
int bss_config_renew(void);
void disconnect_all_sta(void);
int set_rssi_thresh(void);
int set_channel_utilization_threshold(void);
int set_bh_priority(void);
void clear_mesh_action(void);
void init_wapp(void);
void init_mapd(void);
void init_p1905_managerd(void);
//void ether_port_up_down(int op); //currently this func is not used due to wifi-eth loop issue---Geo
void apclient_up_down(int op);
int trigger_restart_map(void);
#endif


