#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <netinet/in.h>
#include <utility.h>
#include "wlan_link.h"
#include "central_ctrl_wan_evt.h"
#include "central_ctrl_common.h"
/********************************************************************/
#define DHCP 1
/*
*/
#define WAN       1
#define LAN       2
/*
*/
#define ROUTER    1
#define BRIDGE    2
#define REPEATER  3
/********************************************************************/
int get_wan_isp(char *path)
{
	char value[64];
	memset(value, 0, sizeof(value));
	if(cfg_get_object_attr(path, "ISP", value, sizeof(value)) <= 0)
	{
		return -1;
	}
	if(0 == atoi(value))
	{
		return DHCP;
	}
	
	return 0;
}

/*
*/
static int get_num_from_string(char *buffer, char *keyword, int *number)
{
	char *tmp = NULL;
	char *tmp2 = NULL;
	if((buffer == 0) || (keyword == 0) || (number == 0))
	{
		return -1;
	}
	if((tmp2 = strstr(buffer,keyword)))
	{
		tmp = tmp2 + strlen(keyword);
		(*number) = atoi(tmp);
		return 0;
	}
	else
	{
		return -1;
	}
}

static int get_wan_pvc_entry_index_name(char *path, int *index)
{
	int   pvc = 0;
	int entry = 0;
	int ret = 0;
	
	if ((NULL == path) || (0 >= strlen(path)) || (NULL == index))
	{
		return -1;
	}
	ret = get_num_from_string(path, "pvc.", &pvc);
	if(0 != ret)
	{
		return -1;
	}
	ret = get_num_from_string(path, "entry.", &entry);
	if(0 != ret)
	{
		return -1;
	}
	
	*index = (pvc - 1) * MAX_WAN_ENTRY_NUMBER + (entry - 1);
		
	return 0;
	
}

static int wan_entry_path_convert_waninfo_path(char* dst, int dst_size, char* src)
{
	int ret = 0;
	int index = -1;
	if((NULL == src) || (NULL == dst) || (0 >= strlen(src)))
	{
		return -1;
	}
	
	ret = get_wan_pvc_entry_index_name(src, &index);
	if(0 != ret)
	{
		return -1;
	}
	memset(dst, 0, dst_size);
	snprintf(dst, dst_size, WANINFO_ENTRY_NODE, (index + 1));

	return 0;
}

/********************************************************************/
int get_v4_ip_netmask(unsigned char type, char *path, ipv4_info_t *info)
{
	char v4[INET_ADDRSTRLEN];
	struct in_addr ipv4;
	char waninfo[32];
	int ret = 0;
	if((NULL == path) || (NULL == info))
	{
		return -1;
	}
	switch (type)
	{
		/*WAN*/
		case 1:
		{
			memset(waninfo, 0, sizeof(waninfo));
			ret = wan_entry_path_convert_waninfo_path(waninfo, sizeof(waninfo), path);
			if(0 != ret)
			{
				return -1;
			}

			memset(v4, 0, sizeof(v4));
			if(cfg_get_object_attr(waninfo, "IP", v4, sizeof(v4)) <= 0)
			{
				return -1;
			}
			if(0 >= strlen(v4))
			{
				return -1;
			}
			memset(&ipv4, 0, sizeof(ipv4));
			if(1 == inet_pton(AF_INET, v4, &ipv4))
			{
				info->s_addr = ntohl(ipv4.s_addr);
			}
			
			memset(v4, 0, sizeof(v4));
			if(cfg_get_object_attr(waninfo, "NetMask", v4, sizeof(v4)) <= 0)
			{
				return -1;
			}
			if(0 >= strlen(v4))
			{
				return -1;
			}
			memset(&ipv4, 0, sizeof(ipv4));
			if(1 == inet_pton(AF_INET, v4, &ipv4))
			{
				info->subnet_mask = ntohl(ipv4.s_addr);
			}
			break;
		}
		/*LAN*/
		case 2:
		{
			memset(v4, 0, sizeof(v4));
			if ( cfg_get_object_attr(LAN_ENTRY0_NODE, "IP", v4, sizeof(v4) ) <= 0 )
			{
				return -1;
			}
			if(0 >= strlen(v4))
			{
				return -1;
			}
			memset(&ipv4, 0, sizeof(ipv4));
			if(1 == inet_pton(AF_INET, v4, &ipv4))
			{
				info->s_addr = ntohl(ipv4.s_addr);
			}
			memset(v4, 0, sizeof(v4));
			if ( cfg_get_object_attr(LAN_ENTRY0_NODE, "netmask", v4, sizeof(v4) ) <= 0 )
			{
				return -1;
			}
			if(0 >= strlen(v4))
			{
				return -1;
			}
			memset(&ipv4, 0, sizeof(ipv4));
			if(1 == inet_pton(AF_INET, v4, &ipv4))
			{
				info->subnet_mask = ntohl(ipv4.s_addr);
			}
 			break;
		}
		default:
		{
			return -1;			
		}
 	}
	
	return 0;
}

/*
	1 group
*/
unsigned char is_v4_wan_br_ip_group(ipv4_info_t *wan_v4, ipv4_info_t *br_v4)
{
	if((NULL == wan_v4) || (NULL == br_v4))
	{
		return FALSE;
	}
	
	wan_v4->subnet_id = (wan_v4->s_addr & wan_v4->subnet_mask);
	br_v4->subnet_id =  (br_v4->s_addr & br_v4->subnet_mask);

	return !(wan_v4->subnet_id ^ br_v4->subnet_id);
}

int get_ap_work_mode(void)
{
	unsigned work_mode = 0;
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));

	if ( cfg_get_object_attr(WAN_COMMON_NODE, "WorkMode", buf, sizeof(buf)) <= 0)
	{
		return -1;
	}
	
	if(0 == strcmp(buf, "router"))
	{
		return ROUTER;
	}
	else if(0 == strcmp(buf, "bridge"))
	{
		return BRIDGE;
	}
	else if(0 == strcmp(buf, "repeater"))
	{
		return REPEATER;
	}
	
	return -1;
}

int update_ap_work_mode(int mode)
{
	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));
	
	switch(mode)
	{
		case ROUTER:
		{
			snprintf(buf, sizeof(buf), "%s", "router");
			break;
		}
		case BRIDGE:
		{
			snprintf(buf, sizeof(buf), "%s", "bridge");
			break;
		}
		default:
		{
			return -1;
		}
	}
	if(0 > cfg_set_object_attr(WAN_COMMON_NODE, "WorkMode", buf))
	{
		return -1;
	}

	cfg_commit_object(WAN_COMMON_NODE);
	return 0;
}

/*

*/
void ap_mode_bridge_route_switch(int mode)
{
	int work_mode = 0;
	work_mode = get_ap_work_mode();
	if(0 > work_mode)
	{
		return ;
	}
	
	if(!(work_mode ^ mode))
	{
		return ;
	}
	
	update_ap_work_mode(mode);

	return ;
}

#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT) && defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING)
int discern_easymesh_device_role(pt_wan_evt info, int* mode)
{
	unsigned char detect = 0;
	unsigned char group  = false;
	int link_type = 0;
	int ret = 0;
	ipv4_info_t wan_info;
	ipv4_info_t br_info;
	int work_mode = 0;

	detect = is_easymesh_role_detect();
	if(!detect)
	{
		SVC_CENTRAL_CTRL_DEBUG_INFO("device have a role\n");
		return -1;
	}

	work_mode = get_ap_work_mode();
	if(ROUTER != work_mode)
	{
		SVC_CENTRAL_CTRL_DEBUG_INFO("workmode not router\n");
		return -1;
	}
	
	link_type = get_wan_isp(info->buf);
	if(DHCP != link_type)
	{
		SVC_CENTRAL_CTRL_DEBUG_INFO("link_type not dhcp\n");
		return -1;
	}
	
	memset(&wan_info, 0, sizeof(wan_info));
	ret = get_v4_ip_netmask(WAN, info->buf, &wan_info);
	if(0 > ret)
	{
		SVC_CENTRAL_CTRL_DEBUG_INFO("get wan info faile\n");
		return -1;
	}
	memset(&br_info, 0, sizeof(br_info));
	ret = get_v4_ip_netmask(LAN, LAN_ENTRY0_NODE, &br_info);
	if(0 > ret)
	{
		SVC_CENTRAL_CTRL_DEBUG_INFO("get br info faile\n");
		return -1;
	}
	
	if(is_v4_wan_br_ip_group(&wan_info, &br_info))
	{
		/*Agent*/
		SVC_CENTRAL_CTRL_DEBUG_INFO("set role as agent\n");
		update_mesh_role(AGENTER);
		*mode = BRIDGE;
	}
	else
	{
		/*controller*/
		SVC_CENTRAL_CTRL_DEBUG_INFO("set role as controller\n");
		update_mesh_role(CONTROLLER);
		*mode = ROUTER;
	}
	
	return 0;
}

void detect_role(pt_wan_evt info)
{
	int ret = 0;
	int mode = 0;
	ret = discern_easymesh_device_role(info, &mode);
	if(0 > ret)
	{
		return ;
	}
	
	ap_mode_bridge_route_switch(mode);
		
	cfg_evt_write_romfile_to_flash();

	prepare_trigger_mesh();

	return ;
}
#endif
/*
*/
int cc_handle_wan_v4_up(struct host_service_pbuf* pbuf)
{
    pt_wan_evt p_evt_data = NULL;
	p_evt_data = (pt_wan_evt)svchost_get_event_val(pbuf);
	if(NULL == p_evt_data)
	{
		return 0;
	}

#if ( defined(TCSUPPORT_NP) && defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT) \
		&& defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING) )

	detect_role(p_evt_data);

#endif

	return 0;
}

