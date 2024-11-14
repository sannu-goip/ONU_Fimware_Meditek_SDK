#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_wifi.h>
#include <ecnt_event_global/ecnt_event_veth.h>
#include <ecnt_event_global/ecnt_event_ether_wan.h>
#include <lan_port/lan_port_info.h>
#include "central_ctrl_common.h"
#include "utility.h"
#include "wlan_link.h"

#if defined(TCSUPPORT_ECNT_MAP)
#include "mesh_trigger.h"

static unsigned int itf_flag = 0;
static unsigned int entry_flag = 0xff;  
#ifdef TCSUPPORT_CT_MAP_INSIDE_AGENT
static unsigned int veth_flag = 0;
#endif
#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING)
static unsigned int ether_wan_itf_flag = 0;
static unsigned char is_ether_wan_itf_ready();
#endif
#endif

/***************************************************************************/
void wlan_wsc_action(struct ecnt_event_data *event_data){
#if defined(TCSUPPORT_WLAN)
	struct wifi_event_data *data = NULL;
	data = (struct wifi_event_data*)event_data;
	char node_name[128]	= {0};
	int j = 0;
	
	if(strncmp(data->ifname,"rai",3) == 0){
		j = atoi(&(data->ifname[3]));
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, j+1);	
		}
	else if(strncmp(data->ifname,"ra",2) == 0){
		j = atoi(&(data->ifname[2]));
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, j+1);	
		}
	else	
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY1_NODE);

	if(strlen(data->ssid) > 0)
		cfg_set_object_attr(node_name,"SSID",data->ssid);
	if(strlen(data->authMode) > 0)
		cfg_set_object_attr(node_name,"AuthMode",data->authMode);
	if(strlen(data->WPAKey) > 0)
		cfg_set_object_attr(node_name,"WPAPSK",data->WPAKey);
	if(strlen(data->encrypType) > 0)
		cfg_set_object_attr(node_name,"EncrypType",data->encrypType);
	
	cfg_evt_write_romfile_to_flash();
#endif
	return;
}

void prepare_trigger_mesh(void)
{
#if defined(TCSUPPORT_ECNT_MAP)
	if ((entry_flag == itf_flag)
#if defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
		&& (1 == veth_flag)
#endif		
#if 0//defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING)
		&& (is_ether_wan_itf_ready())
#endif		
	) 
	{
		if(ether_wan_itf_flag)
			cfg_set_object_attr(MESH_MAP_CFG_NODE, "wan", "nas0_0");
		else
			cfg_set_object_attr(MESH_MAP_CFG_NODE, "wan", "");
		mesh_trigger();
	}
#endif
	return;
}

#if defined(TCSUPPORT_ECNT_MAP)
static void get_wifi_entry_flag(unsigned int* flag)
{
	char value[64];
	int bssid_num = 0;
	int i = 0;
	char entryNode[32];

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, WLAN_COMMON_BSSID_NUM_ATTR, value, sizeof(value));
	bssid_num = atoi(value);
	
	for(i = 0; i < bssid_num; i++) 
	{
		memset(entryNode, 0, sizeof(entryNode));
		snprintf(entryNode,sizeof(entryNode),WLAN11AC_ENTRY_N_NODE,(i+1));
	
		if (cfg_query_object(entryNode,NULL,NULL) > 0)
		{
			memset(value, 0, sizeof(value));
			if( (0 > cfg_get_object_attr(entryNode, WLAN_ENTRY_ENABLE_SSID_ATTR, value, sizeof(value))) \
				|| (0 == atoi(value)) ) 
			{
					continue;
			}

			*flag |= (0x01 << i);
		}
	}
	
	memset(value, 0, sizeof(value));
	cfg_get_object_attr(WLAN_COMMON_NODE, WLAN_COMMON_BSSID_NUM_ATTR, value, sizeof(value));
	bssid_num = atoi(value);
	for(i = 0; i < bssid_num; i++)
	{
		memset(entryNode, 0, sizeof(entryNode));
		snprintf(entryNode,sizeof(entryNode),WLAN_ENTRY_N_NODE,(i+1));
	
		if (cfg_query_object(entryNode,NULL,NULL) > 0)
		{
			memset(value, 0, sizeof(value));
			if( (0 > cfg_get_object_attr(entryNode, WLAN_ENTRY_ENABLE_SSID_ATTR, value, sizeof(value))) \
				|| (0 == atoi(value)) ) 
			{	
					continue;
			}

			*flag |= (0x01 << (i + 0x10));
		}
	}

	return ;
}
#endif

static void wifi_up_down_handle(struct ecnt_event_data *event_data, int event_type)
{
#if defined(TCSUPPORT_ECNT_MAP)
	unsigned int itf_num = 0;
	
	struct wifi_event_data *data = NULL;
	data = (struct wifi_event_data*)event_data;
	
	if (ECNT_EVENT_WIFI_UP == event_type) 
	{
		tcdbg_printf("wifi_up_handle %s.\n",data->ifname);
		if(NULL != strstr(data->ifname, APCLIENT_NAME_FORMAT))
			return;
		entry_flag = 0;
		get_wifi_entry_flag(&entry_flag);

	
		if(strncmp(data->ifname,WLAN_AC_ITF_NAME_FORMAT, strlen(WLAN_AC_ITF_NAME_FORMAT)) == 0) {
			itf_num = atoi(&(data->ifname[strlen(WLAN_AC_ITF_NAME_FORMAT)]));
			itf_flag |= (1 << itf_num);
		}
		else if(strncmp(data->ifname, WLAN_ITF_NAME_FORMAT, strlen(WLAN_ITF_NAME_FORMAT)) == 0) {
			itf_num = atoi(&(data->ifname[strlen(WLAN_ITF_NAME_FORMAT)]));
			itf_num += 0x10;			
			itf_flag |= (1 << itf_num);
		}
		prepare_trigger_mesh();
	}
	else if (ECNT_EVENT_WIFI_DOWN == event_type)  
	{		
 		tcdbg_printf("wifi_down_handle %s.\n",data->ifname);	
		entry_flag = 0;
		get_wifi_entry_flag(&entry_flag);
		
		if(strncmp(data->ifname,WLAN_AC_ITF_NAME_FORMAT, strlen(WLAN_AC_ITF_NAME_FORMAT)) == 0) {
			itf_num = atoi(&(data->ifname[strlen(WLAN_AC_ITF_NAME_FORMAT)]));
			itf_flag &= ~(1 << itf_num);
		}
		else if(strncmp(data->ifname, WLAN_ITF_NAME_FORMAT, strlen(WLAN_ITF_NAME_FORMAT)) == 0) {
			itf_num = atoi(&(data->ifname[strlen(WLAN_ITF_NAME_FORMAT)]));
			itf_num += 0x10;			
			itf_flag &= ~(1 << itf_num);
		}
	}
#endif	
	return ;
}

static void veth_down_up_handle(struct ecnt_event_data *event_data, int event_type)
{
#if defined(TCSUPPORT_ECNT_MAP) && defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
	unsigned int itf_num = 0;
	struct veth_event_data *data = NULL;
	data = (struct veth_event_data*)event_data;
	if (ECNT_EVENT_VETH_UP == event_type) 
	{
		tcdbg_printf("veth_up_handle %s.\n",data->ifname);
		veth_flag = 1;
		prepare_trigger_mesh();
	}
	else if (ECNT_EVENT_VETH_DOWN == event_type)  
	{		
 		tcdbg_printf("veth_down_handle %s.\n",data->ifname);	
		veth_flag = 0;
	}
#endif	
	return ;
}


void wifi_up_handle(struct ecnt_event_data *event_data)
{
	wifi_up_down_handle(event_data, ECNT_EVENT_WIFI_UP);
}

void wifi_down_handle(struct ecnt_event_data *event_data)
{
	wifi_up_down_handle(event_data, ECNT_EVENT_WIFI_DOWN);
}

void veth_up_handle(struct ecnt_event_data *event_data)
{
	veth_down_up_handle(event_data, ECNT_EVENT_VETH_UP);
}

void veth_down_handle(struct ecnt_event_data *event_data)
{
	veth_down_up_handle(event_data, ECNT_EVENT_VETH_DOWN);
}


static void ether_wan_itf_down_up_handle(struct ecnt_event_data *event_data, int event_type)
{
#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING)
	unsigned int itf_num = 0;
	struct ether_wan_event_data *data = NULL;
	data = (struct ether_wan_event_data*)event_data;
	
	if (ECNT_EVENT_ETHER_WAN_UP == event_type) 
	{
		ether_wan_itf_flag = 1;
		prepare_trigger_mesh();
	}
	else if (ECNT_EVENT_ETHER_WAN_DOWN == event_type)  
	{		
		ether_wan_itf_flag = 0;
		prepare_trigger_mesh();
	}
#endif

	return ;
}

void ether_wan_itf_up_handle(struct ecnt_event_data *event_data)
{
	ether_wan_itf_down_up_handle(event_data, ECNT_EVENT_ETHER_WAN_UP);
}

void ether_wan_itf_down_handle(struct ecnt_event_data *event_data)
{
	ether_wan_itf_down_up_handle(event_data, ECNT_EVENT_ETHER_WAN_DOWN);
}

#if defined(TCSUPPORT_ECNT_MAP)
int get_easymesh_role(void)
{
	unsigned char device_role = 0;

	unsigned char buf[32];
	memset(buf, 0, sizeof(buf));
	if ( cfg_get_object_attr(MESH_COMMON_NODE, "DeviceRole", buf, sizeof(buf) ) <= 0 )
	{
		return -1;
	}
	
	device_role = atoi(buf);

	return device_role;
}
#endif

#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING)
static unsigned char is_ether_wan_itf_ready()
{
	int device_role = 0;
	unsigned char buf[32];
	unsigned char ready = FALSE;
	device_role = get_easymesh_role();
	if(0 > device_role)
	{
		return FALSE;
	}
	switch(device_role)
	{
		case CONTROLLER:
		{
			ready = TRUE;
			break;
		}
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)
		case UNCONFIGURED:
		{
			memset(buf, 0, sizeof(buf));
#ifdef TCSUPPORT_NP_CMCC			
			if ( cfg_get_object_attr(WANINFO_COMMON_NODE, "CurAPMode", buf, sizeof(buf)) <= 0)
			{
				ready = TRUE;
			}
			else if(0 == strcmp(buf, "Bridge"))
			{
				ready = ether_wan_itf_flag;
			}

#else
			if ( cfg_get_object_attr(WAN_COMMON_NODE, "WorkMode", buf, sizeof(buf)) <= 0)
			{
				ready = FALSE;
			}
			else if(0 == strcmp(buf, "bridge"))
			{
				ready = ether_wan_itf_flag;
			}

#endif				
			else
			{
				ready = TRUE;
			}
			break;
		}
#endif		
		case AGENTER:
		{
			ready = ether_wan_itf_flag;
			break;
		}
		default:
		{
			ready = FALSE;
			break;
		}
	}
	
	return ready;
}
#endif

#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)
unsigned char is_easymesh_role_detect(void)
{
	int device_role = 0;
	unsigned char is_autorole = FALSE;
	device_role = get_easymesh_role();
	if(0 > device_role)
	{
		return FALSE;
	}
	
	switch(device_role)
	{
		case CONTROLLER:
		case AGENTER:
		{
			is_autorole = FALSE;
			break;
		}
		default:
		{
			is_autorole = TRUE;
			break;
		}
	}

	return is_autorole;
}

int update_mesh_role(char role)
{
	unsigned char buf[32];
	int device_role = AGENTER;
	switch(role)
	{
		case CONTROLLER:
		{
			device_role = CONTROLLER;
			break;
		}
		default:
		{
			device_role = AGENTER;
			break;
		}
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d", device_role);
	if(0 > cfg_set_object_attr(MESH_COMMON_NODE, "DeviceRole", buf))
	{
		return -1;
	}
	
	return 0;
}
#endif

