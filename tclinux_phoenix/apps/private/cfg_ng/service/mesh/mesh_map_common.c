/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <utility.h>
#include <libapi_lib_wifimgr.h>
#include "mesh_map_cfg.h"
#include "mesh_map_common.h"

int g_svc_mesh_level = E_NO_INFO_LEVEL;

void svc_mesh_printf(char *fmt,...)
{
	FILE *proc_file = NULL;
	char msg[512];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, 512, fmt, args);	

	proc_file = fopen("/proc/tc3162/dbg_msg", "w");
	if (NULL == proc_file) 
	{
		printf("open /proc/tc3162/dbg_msg fail\n");
		va_end(args);
		return;
	}

	fprintf(proc_file, "%s", msg);
	fclose(proc_file);
	va_end(args);

	return ;
}

void svc_mesh_debug_level(int level)
{
	g_svc_mesh_level = level;
	svc_mesh_printf("debug level = %d.\n", g_svc_mesh_level);
	
	return ;
}

int svc_mesh_execute_cmd(char* cmd, char* func, int line)
{
	if(g_svc_mesh_level & E_TRACE_INFO_LEVEL)
	{
		svc_mesh_printf("func = %s, line = %d, cmd = %s. \n",func, line, cmd);
	}
	system(cmd);
	
	return 0;
}

unsigned char value_convert_bool(int action)
{
	if(0 == action)
	{
		return 0;
	}

	return 1;
}

int convert_hidden_string(unsigned char hidden, unsigned char* buf, unsigned int buf_len)
{
	switch(hidden)
	{
		case 1:
		{
			snprintf(buf, buf_len, "hidden-Y");
			break;
		}
		default:
		{
			snprintf(buf, buf_len, "hidden-N");
			break;
		}
	}

	return 0;
}

unsigned char dev_roller(unsigned char roller)
{
	unsigned char real_roller = MAP_ROLE_UNCONFIGURED;
	switch(roller)
	{
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)
		case MAP_ROLE_UNCONFIGURED:
		{
			real_roller = MAP_ROLE_UNCONFIGURED;
			break;
		}
#endif		
		case MAP_CONTROLLER:
		{
			real_roller = MAP_CONTROLLER;
			break;
		}
		default:
		{
			real_roller = MAP_AGENTER;
			break;
		}
	}
	
	return real_roller;
}

static int get_br_itf_mac(unsigned char* mac)
{
	int ret = 0;
	char value[64];
	unsigned char br_itf_name[IFNAMSIZ];

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_MAP_CFG_NODE, MAP_BR_ITF_NAME_ATTR, value, sizeof(value));
	snprintf(br_itf_name, sizeof(br_itf_name), value);
	ret = wifimgr_lib_get_interface_mac(br_itf_name, mac);
	if(0 != ret)
	{
		return -1;
	}
	
	return 0;
}

int generate_1905_AL_mac(unsigned char* AL_mac)
{
	int ret = 0;
	unsigned char local_bit = 1 << 1;
	unsigned char br_itf_mac[ETH_ALEN];
	
	memset(br_itf_mac, 0, sizeof(br_itf_mac));
	ret = get_br_itf_mac(br_itf_mac);
	if(0 != ret)
	{
		return -1;
	}
	
	memcpy(AL_mac, br_itf_mac, ETH_ALEN);

#if 0//ndef TCSUPPORT_ECNT_MAP_ENHANCE
	AL_mac[0] |= local_bit;
#endif
	
	return 0;
}

int convert_authmod_string(char *authmode)
{
	if(NULL == authmode)
	{
		return -1;
	}

	if(0 == strcasecmp(authmode, AUTHMODE_WPA2PSK))
	{
		return AUTHMODE_WPA2PSK_VALUE;
	}
	else if(0 == strcasecmp(authmode, AUTHMODE_WPAPSK))
	{
		return AUTHMODE_WPAPSK_VALUE;
	}
	else if(0 == strcasecmp(authmode, AUTHMODE_WPAPSKWPA2PSK))
	{
		return (AUTHMODE_WPAPSK_VALUE | AUTHMODE_WPA2PSK_VALUE);
	}
	else if(0 == strcasecmp(authmode, AUTHMODE_OPEN))
	{
		return AUTHMODE_OPEN_VALUE;
	}
	else if(0 == strcasecmp(authmode, AUTHMODE_SAE))
	{
		return AUTHMODE_SAE_VALUE;
	}
	else if(0 == strcasecmp(authmode, AUTHMODE_WPA2PSKWPA3PSK))
	{
		return (AUTHMODE_SAE_VALUE | AUTHMODE_WPA2PSK_VALUE); 
	}

	
	return -1;
}

int convert_encryptype_string(char *encryptype)
{
	if(NULL == encryptype)
	{
		return -1;
	}

	if(0 == strcasecmp(encryptype, ENCRYPTYPE_AES))
	{
		return ENCRYPTYPE_AES_VALUE;
	}
	else if(0 == strcasecmp(encryptype, ENCRYPTYPE_TKIP))
	{
		return ENCRYPTYPE_TKIP_VALUE;
	}
	else if(0 == strcasecmp(encryptype, ENCRYPTYPE_TKIPAES))
	{
		return (ENCRYPTYPE_AES_VALUE | ENCRYPTYPE_TKIP_VALUE);
	}
	else if(0 == strcasecmp(encryptype, ENCRYPTYPE_NONE))
	{
		return ENCRYPTYPE_NONE_VALUE;
	}

	return -1;
}

int convert_bandwidth_24G_string(char *HT_BW, char *HT_BSSCoex)
{
	if(NULL == HT_BW || NULL == HT_BSSCoex)
	{
		return -1;
	}

	if(0 == strcmp(HT_BW, "0"))
	{
		return MAP_20_VALUE;
	}
	else if(0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoex, "0"))
	{
		return MAP_40_VALUE;
	}
	else if(0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoex, "1"))
	{
		return MAP_20_40_VALUE;
	}
	else
	{
		return MAP_20_VALUE;
	}

}

int convert_bandwidth_5G_string(char *HT_BW, char *HT_BSSCoex, char *VHT_BW)
{
	if(NULL == HT_BW || NULL == VHT_BW)
	{
		return -1;
	}

	if(0 == strcmp(HT_BW, "0"))
	{
		return MAP_20_VALUE;
	}
	else if(0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoex, "1") && 0 == strcmp(VHT_BW, "0"))
	{
		return MAP_20_40_VALUE;
	}
	else if(0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoex, "0") && 0 == strcmp(VHT_BW, "0"))
	{
		return MAP_40_VALUE;
	}
	else if(0 == strcmp(HT_BW, "1") && 0 == strcmp(HT_BSSCoex, "1") && 0 == strcmp(VHT_BW, "1"))
	{
		return MAP_20_40_80_VALUE;
	}
	else
	{
		return MAP_20_VALUE;
	}
}

void mesh_send_evt(int type, int evt_id, char *buf)
{
	mesh_evt_t evt;
	char evt_type[EVT_TYPE_LENGTH];

	memset(&evt, 0, sizeof(evt));
	if((NULL == buf) || (strlen(buf) >= EVT_BUF_LENGTH))
	{
		return ;
	}
	snprintf(evt.buf, sizeof(evt.buf), buf);
	memset(evt_type, 0, sizeof(evt_type));
	switch(type)
	{
		case EVT_MESH_INTERNAL_TYPE:
		{
			snprintf(evt_type, sizeof(evt_type), EVT_MESH_INTERNAL);
			break;
		}
		case EVT_MESH_EXTERNAL_TYPE:
		{
			snprintf(evt_type, sizeof(evt_type), EVT_MESH_EXTERNAL);
			break;
		}
		default:
		{
			return;
		}
	}

	cfg_send_event(evt_type, evt_id, (void *)&evt , sizeof(mesh_evt_t));
	return ;
}

#ifdef TCSUPPORT_CT_MAP_INSIDE_AGENT
int veth_ready(const char *itf_name)
{
	int fd = 0;
	int ret = 0;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(struct ifreq));
	if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) 
	{
		ifr.ifr_addr.sa_family = AF_INET;
		snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", itf_name);
		ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
		if(ifr.ifr_flags & IFF_RUNNING){
			ret = 1;
		}else{
			ret = 0;
		}
	} 
	else 
	{
		return -1;
	}
	
	close(fd);
	
	return ret;
}
#endif

#if defined(TCSUPPORT_CMCC)
#define MAX_WAN_PVC_NUMBER		1
#else
#define MAX_WAN_PVC_NUMBER		8
#endif

int internetRouteWanExist()
{
	int i = 0, j = 0;
	char nodeName[32] = {0};
	char serviceList[32] = {0}, wanMode[32] = {0};
	char val[32];
	int flag = 0;
	int role = MAP_CONTROLLER ;
	
	for ( i = 0; i < MAX_WAN_PVC_NUMBER; i++ )
	{
		for ( j = 0; j < MAX_WAN_ENTRY_NUMBER; j++ )
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, i + 1, j + 1);
			if ( cfg_obj_query_object(nodeName, NULL, NULL) <= 0 )
				continue;
			cfg_get_object_attr(nodeName, "ServiceList", serviceList, sizeof(serviceList));
			if (0 == strcmp(serviceList, "INTERNET"))
			{
				cfg_get_object_attr(nodeName, "WanMode", wanMode, sizeof(wanMode));
				if (0 == strcmp(wanMode, "Route"))
				{
					memset(val, 0, sizeof(val));
					cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, val, sizeof(val));
					role = dev_roller(atoi(val));
					
					memset(val, 0, sizeof(val));
					cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, val, sizeof(val));
					if((MAP_CONTROLLER == role) && (1 == atoi(val)))
					{
						cfg_set_object_attr(LAN_DHCP_NODE, "type", "1");
						cfg_commit_object(LAN_DHCP_NODE);
						sleep(1);
						cfg_commit_object(DHCPD_COMMON_NODE);
						sleep(1);
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

