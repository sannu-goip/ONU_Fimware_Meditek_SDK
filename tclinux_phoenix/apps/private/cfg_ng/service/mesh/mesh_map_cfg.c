/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cfg_api.h>
#include <utility.h>
#include "mesh_map_cfg.h"
#include <lan_port/lan_port_info.h>

static void insert_NonMapIfaceIDX(unsigned int *NonMapIfaceIDX_2G, unsigned int *NonMapIfaceIDX_5G, char *value)
{
	char *token = NULL;
	unsigned char idx = 0;

	token = strtok(value, ";");

	while (token != NULL) {
		if(strstr(token, "rai")){
			sscanf(token, WIFI_5G_ITF_NAME, &idx);	//tcdbg_printf("@DBG@ %s:idx:%d\n", __FUNCTION__, idx);
			*NonMapIfaceIDX_5G |= (1 << (idx+1));
		}
		else if(strstr(token, "ra")){
			sscanf(token, WIFI_ITF_NAME, &idx); 	//tcdbg_printf("@DBG@ %s:idx:%d\n", __FUNCTION__, idx);
			*NonMapIfaceIDX_2G |= (1 << (idx+1));
		}
		else{
			tcdbg_printf("%s():[error] wrong token content:%s\n", __FUNCTION__, token);
		}

		token = strtok(NULL, ";");

	}
	//tcdbg_printf("@DBG@ %s:NonMapIfaceIDX_2G:%u, NonMapIfaceIDX_5G:%u\n", __FUNCTION__, *NonMapIfaceIDX_2G, *NonMapIfaceIDX_5G);
	return;
}


int load_wts_bss_info(mesh_config_bss_info info[], int max_bss_num)
{
	int i = 0;
	int j = 0;
	char value[128];
	char path[64];
	char wlan_path[64];
	char AL_MAC_path[64];
	char HT_BW[8];
	char VHT_BW[8];
	char HT_BSSCoex[8];
	int radio = RADIO_2G;
	int auth_mode = 0;
	int encryptype = 0;
	int channel = 0;
	char bandEnable_2g[2] = {0};	
	char bandEnable_5g[2] = {0};
	int band_width = 0;
	unsigned int NonMapIfaceIDX_2G = 0;
	unsigned int NonMapIfaceIDX_5G = 0;

//NonMapIface is a param which is used to record those wifi interface which is not wished being controlled by mesh.
//NonMapIface is set under mesh_common, ex: NonMapIface = "ra2;ra3;rai2;rai3". Those wifi Iface is not appeared on wts file.
	memset(value, 0, sizeof(value));
	if(0 < cfg_get_object_attr(MESH_COMMON_NODE, MAP_NON_MESH_INTERFACE, value, sizeof(value))) {
		value[64] = '\0';
		insert_NonMapIfaceIDX(&NonMapIfaceIDX_2G, &NonMapIfaceIDX_5G, value);
	}
	
	for(i=0; i<max_bss_num; i++)
	{
		memset(path,  0, sizeof(path));
		memset(wlan_path,  0, sizeof(wlan_path));
		memset(AL_MAC_path,  0, sizeof(AL_MAC_path));
		cfg_get_object_attr(WLAN_COMMON_NODE, WLAN_APON_ATTR, bandEnable_2g, sizeof(bandEnable_2g));
		cfg_get_object_attr(WLAN11AC_COMMON_NODE, WLAN_APON_ATTR, bandEnable_5g, sizeof(bandEnable_5g));
		radio = (i / MAX_RADIO_BSS_INFO_NUM);
		switch (radio)
		{
			case RADIO_2G:
			{
				if(strcmp(bandEnable_2g,"0"))
				{
				snprintf(path, sizeof(path), MESH_RADIO2G_BSSINFO_ENTRY_NODE, (i % MAX_RADIO_BSS_INFO_NUM));
				snprintf(wlan_path, sizeof(wlan_path),"%s.%d", WLAN_ENTRY_NODE, (i % MAX_RADIO_BSS_INFO_NUM));
				snprintf(AL_MAC_path, sizeof(AL_MAC_path),"%s", MESH_RADIO2G_BSSINFO_NODE);
                                }
				break;
			}
			case RADIO_5GL:
			{
				if(strcmp(bandEnable_5g,"0"))
				{
				snprintf(path, sizeof(path), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, (i % MAX_RADIO_BSS_INFO_NUM));
				snprintf(wlan_path, sizeof(wlan_path), "%s.%d", WLAN11AC_ENTRY_NODE, (i % MAX_RADIO_BSS_INFO_NUM));
				snprintf(AL_MAC_path, sizeof(AL_MAC_path),"%s", MESH_RADIO5GL_BSSINFO_NODE);
                                }
				break;
			}
			case RADIO_5GH:
			{
#if 0
				snprintf(path, sizeof(path), MESH_RADIO5GH_BSSINFO_ENTRY_PATH, (i % MAX_RADIO_BSS_INFO_NUM));
				snprintf(wlan_path, sizeof(wlan_path), "%s.%d", SVC_WLAN11AC_ENTRY_PATH, (i % MAX_RADIO_BSS_INFO_NUM));
				snprintf(AL_MAC_path, sizeof(AL_MAC_path),"%s", MESH_RADIO5GH_BSSINFO_PATH);
				break;
#endif
			}
			default:
			{
				return -1;
			}           
		}

		memset(value, 0, sizeof(value));
		if( (0 > cfg_get_object_attr(wlan_path, WLAN_ENTRY_ENABLE_SSID_ATTR, value, sizeof(value))) \
		|| (0 == atoi(value)) )
		{
			continue;
		}

		if(radio == RADIO_2G){
			if(NonMapIfaceIDX_2G & (1 << i))
				continue;
		}
		else if(radio == RADIO_5GL){
			if(NonMapIfaceIDX_5G & (1 << (i - MAX_RADIO_BSS_INFO_NUM)))
				continue;
		}

		memset(value, 0, sizeof(value));
		if(0 > cfg_get_object_attr(wlan_path, WLAN_ENTRY_SSID_ATTR, value, sizeof(value)))
		{
			continue;
		}
		snprintf(info[j].ssid, sizeof(info[i].ssid), "%s", value);

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(AL_MAC_path, MAP_BSSINFO_AL_MAC_ATTR, value, sizeof(value));
		if(ETH_ALEN != sscanf(value, MAP_ADDR_SCAN_FMT, MAP_ADDR_SCAN_ARGS(info[j].Al_mac)))
		{
			return -1;
		}
		
		switch(radio)
		{
			case RADIO_2G:
			{
				snprintf(info[j].oper_class, sizeof(info[j].oper_class), "%s", MAP_2G_OPER_CLASS);
				break;
			}
			case RADIO_5GL:
			{
				memset(value, 0, sizeof(value));
				cfg_get_object_attr(WLAN11AC_COMMON_NODE, WLAN_COMMON_CHANNEL_ATTR, value, sizeof(value));
				channel = atoi(value);
				SVC_MESH_DEBUG_INFO("channel = %d\n",channel);
				if(100 > channel)
				{
					snprintf(info[j].oper_class, sizeof(info[j].oper_class), "%s", MAP_5GL_OPER_CLASS);
				}
				else
				{
					snprintf(info[j].oper_class, sizeof(info[j].oper_class), "%s", MAP_5GH_OPER_CLASS);
				}
				break;
			}
			case RADIO_5GH:
			{
#if 0
			snprintf(info[j].oper_class, sizeof(info[j].oper_class), "%s", MAP_5GH_OPER_CLASS);
			break;
#endif
			}
			default:
			{
				return -1;
			}    
		}
		memset(value, 0, sizeof(value));
		cfg_get_object_attr(wlan_path, WLAN_ENTRY_AUTH_MODE_ATTR, value, sizeof(value));
		auth_mode = convert_authmod_string(value);
		if(-1 != auth_mode)
		{
			info[j].auth_mode = auth_mode;
		}
		else
		{
			memset(info[j].ssid, 0, sizeof(info[j].ssid));
			continue;
		}

		if (AUTHMODE_OPEN_VALUE == auth_mode)
		{
			info[j].encrypt_type = ENCRYPTYPE_NONE_VALUE;
		}
		else
		{
			memset(value, 0, sizeof(value));
			cfg_get_object_attr(wlan_path, WLAN_ENTRY_ENCRYPT_TYPE_ATTR, value, sizeof(value));
		encryptype = convert_encryptype_string(value);
		if(-1 != encryptype)
		{
			info[j].encrypt_type = encryptype;
		}
		else
		{
			memset(info[j].ssid, 0, sizeof(info[j].ssid));
			continue;
		}
		}

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(wlan_path, WLAN_ENTRY_KEY_ATTR, value, sizeof(value));
		if (AUTHMODE_OPEN_VALUE == info[j].auth_mode)
		{
			/*only for write file format*/
			snprintf(info[j].key, sizeof(info[j].key), BSSINFO_DEF_KEY);
		}
		else
		{
		snprintf(info[j].key, sizeof(info[j].key), "%s", value);
		}

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(wlan_path, WLAN_ENTRY_HIDDEN_SSID_ATTR, value, sizeof(value));
		info[j].hidden_ssid = atoi(value);

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(path, MAP_BSSINFO_FRONT_HAUL_ATTR, value, sizeof(value));
		info[j].front_haul = atoi(value);

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(path, MAP_BSSINFO_BACK_HAUL_ATTR, value, sizeof(value));
		info[j].back_haul = atoi(value);
#if defined(TCSUPPORT_MAP_R2)
		memset(value, 0, sizeof(value));
		cfg_get_object_attr(path, MAP_BSSINFO_VLAN_ID_ATTR, value, sizeof(value));
		if(strlen(value) == 0 || atoi(value) < 3 || atoi(value) > 4095)
			info[j].vlanId = 4095;
		else
			info[j].vlanId = atoi(value);

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(path, MAP_BSSINFO_PRIMARY_VLAN_ATTR, value, sizeof(value));
		if(atoi(value) == 1)
			info[j].primaryVlan = atoi(value);
		else
			info[j].primaryVlan = 0;

		memset(value, 0, sizeof(value));
		cfg_get_object_attr(path, MAP_BSSINFO_DEFAULT_PCP_ATTR, value, sizeof(value));
		info[j].defaultPCP= strlen(value) == 0 ? -1 : atoi(value);
#endif
		if (RADIO_2G == radio)
		{
			memset(HT_BW, 0, sizeof(HT_BW));
			memset(HT_BSSCoex, 0, sizeof(HT_BSSCoex));
			cfg_get_object_attr(WLAN_COMMON_NODE, MAP_BSSINFO_HT_BW_ATTR, HT_BW, sizeof(HT_BW));
			cfg_get_object_attr(WLAN_COMMON_NODE, MAP_BSSINFO_HT_BSSCOEXISTENCE_ATTR, HT_BSSCoex, sizeof(HT_BSSCoex));
			band_width = convert_bandwidth_24G_string(HT_BW, HT_BSSCoex);
			info[j].band_width= band_width;
		}
		else if (RADIO_5GL == radio)
		{
			memset(HT_BW, 0, sizeof(HT_BW));
			memset(HT_BSSCoex, 0, sizeof(HT_BSSCoex));
			memset(VHT_BW, 0, sizeof(VHT_BW));
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, MAP_BSSINFO_HT_BW_ATTR, HT_BW, sizeof(HT_BW));
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, MAP_BSSINFO_HT_BSSCOEXISTENCE_ATTR, HT_BSSCoex, sizeof(HT_BSSCoex));
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, MAP_BSSINFO_VHT_BW_ATTR, VHT_BW, sizeof(VHT_BW));
			band_width = convert_bandwidth_5G_string(HT_BW, HT_BSSCoex, VHT_BW);
			info[j].band_width= band_width;
		}
		
		j++;
	}
	
	/*set BssInfoCount*/
	memset(value, 0, sizeof(value));
	snprintf(value, sizeof(value), "%d", j);
    cfg_set_object_attr(MESH_COMMON_NODE, MAP_BSSINFO_COUNT_ATTR, value);
	
	return 0;
}

void parse_wlan_entry(const char *wlanNode, const char *wlanInf, char *param, int length, int type)
{
	char value[32];
	int i;	
	int num = 0;
	int j = 0;
	char entryNode[64];
	char wlanCommon[32];
	char inf[16];	
	int	 bhfhvalue = 0;

	memset(value, 0, sizeof(value));
	memset(wlanCommon, 0, sizeof(wlanCommon));
	snprintf(wlanCommon,sizeof(wlanCommon),"%s.common",wlanNode);	
	cfg_get_object_attr(wlanCommon, WLAN_COMMON_BSSID_NUM_ATTR, value, sizeof(value));
	num = atoi(value);
	
	for(i = 0; i < num; i++)
	{
		memset(entryNode, 0, sizeof(entryNode));
		snprintf(entryNode,sizeof(entryNode),"%s.entry.%d",wlanNode,(i+1));

		if (cfg_query_object(entryNode,NULL,NULL) > 0)
		{
			memset(value, 0, sizeof(value));
			if( (0 > cfg_get_object_attr(entryNode, WLAN_ENTRY_ENABLE_SSID_ATTR, value, sizeof(value))) \
				|| (0 == atoi(value)) )
			{
				continue;
			}

			if(type)
			{
				memset(value, 0, sizeof(value));
				if( (0 >= cfg_get_object_attr(entryNode, WLAN_ENTRY_FT_SUPPORT_ATTR, value, sizeof(value))) \
					|| (0 == atoi(value)) )
				{
					continue;
				}
				
			}
#if !defined(TCSUPPORT_NP_CMCC)
			memset(entryNode, 0, sizeof(entryNode));
			if (NULL != strstr(wlanCommon, "wlan11ac"))
				snprintf(entryNode, sizeof(entryNode), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, (i+1));
			else
				snprintf(entryNode, sizeof(entryNode), MESH_RADIO2G_BSSINFO_ENTRY_NODE, (i+1));
#endif		
			bhfhvalue = getBhfhParam(entryNode);
			if(bhfhvalue)
			{		
				memset(inf, 0, sizeof(inf));
				if(type)
				{
					snprintf(inf,sizeof(inf),LOAD_11RPARAM"%s%d ",wlanInf,i);					
				}else
				{
				snprintf(inf,sizeof(inf),"%s%d;",wlanInf,i);
				}
				if(0 == j)
				{
					snprintf(param, length, "%s", inf);
					j++;
				}
				else
				{
					strcat(param,inf);
				}
			}	
		}
		
	}
	return ;	
}
int load_bss_config_priority(void)
{
	char value[32];
	char buffer[256];
	char bandEnable_2g[2] = {0};	
	char bandEnable[2] = {0};
	char bufpart[128];
	char inf[16];
	int i;	
	
	memset(buffer, 0, sizeof(buffer));
	memset(bufpart, 0, sizeof(bufpart));
	cfg_get_object_attr(WLAN_COMMON_NODE, WLAN_APON_ATTR, bandEnable_2g, sizeof(bandEnable_2g));
	if(strcmp(bandEnable_2g,"1") == 0)
	{
	parse_wlan_entry(WLAN_NODE,WLAN_ITF_NAME_FORMAT,bufpart,sizeof(bufpart), 0);
	snprintf(buffer,sizeof(buffer),"%s",bufpart);	
	}	
	memset(bandEnable, 0, sizeof(bandEnable));
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, WLAN_APON_ATTR, bandEnable, sizeof(bandEnable));
	if(strcmp(bandEnable,"1") == 0)
	{
		memset(bufpart, 0, sizeof(bufpart));	
		parse_wlan_entry(WLAN11AC_NODE,WLAN_AC_ITF_NAME_FORMAT,bufpart,sizeof(bufpart), 0);	
		if(strlen(buffer) == 0)
			snprintf(buffer,sizeof(buffer),"%s",bufpart);	
		else if(strlen(bufpart)>0)
		strcat(buffer,bufpart);
	}
	
	memset(value, 0, sizeof(value));
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT) && (TCSUPPORT_EASYMESH_R13)
	cfg_get_object_attr(INFO_MESH_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
#else
	cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
#endif
#if !defined(TCSUPPORT_CT_MAP_INSIDE_AGENT)
	if( (atoi(value) == MAP_AGENTER)
#if defined(TCSUPPORT_MESH_ROLE_AUTO_DETECT)
		|| (MAP_ROLE_UNCONFIGURED == atoi(value))
#endif		
	)
	{	
#if 1//!defined(TCSUPPORT_NP_CMCC)			
		if(strcmp(bandEnable_2g,"1") == 0)
		{
		for(i = 0; i < MAX_APCLIENT_NUM; i++)
		{
			memset(inf, 0, sizeof(inf));		
			snprintf(inf,sizeof(inf),M_APCLIENT_2G_NAME_FORMAT";",i);		
			strcat(buffer,inf);
		}
		}
#endif			
		if(strcmp(bandEnable,"1") == 0)
		{
			for(i = 0; i < MAX_APCLIENT_NUM; i++)
			{
				memset(inf, 0, sizeof(inf));		
				snprintf(inf,sizeof(inf),M_APCLIENT_5G_NAME_FORMAT";",i);					
				strcat(buffer,inf);
			}	
		}
	}
#endif
	if(strlen(buffer) > 0)
	{
		buffer[strlen(buffer)-1] = '\0';
		cfg_set_object_attr(MESH_MAP_CFG_NODE, MAP_BSS_CONFIG_PRIORITY_ATTR, buffer);
	}
	
	return 0;
}

int load_11R_param(char *param, int length)
{
	char value[32];
	char entryNode[32];
	int i;	
	int num = 0;
	char inf[16];
	char bandEnable_2g[2] = {0};		
	char bandEnable[2] = {0};
	int j = 0;
	char bufpart[64];

	memset(param, 0, length);
	memset(bufpart, 0, sizeof(bufpart));
	cfg_get_object_attr(WLAN_COMMON_NODE, WLAN_APON_ATTR, bandEnable_2g, sizeof(bandEnable_2g));
	if(strcmp(bandEnable_2g,"1") == 0)
	{	
	parse_wlan_entry(WLAN_NODE,WLAN_ITF_NAME_FORMAT,bufpart,sizeof(bufpart), 1);
	snprintf(param,length,"%s",bufpart);
	}
	memset(bandEnable, 0, sizeof(bandEnable));
	cfg_get_object_attr(WLAN11AC_COMMON_NODE, WLAN_APON_ATTR, bandEnable, sizeof(bandEnable));
	if(strcmp(bandEnable,"1") == 0)
	{
		memset(bufpart, 0, sizeof(bufpart));	
		parse_wlan_entry(WLAN11AC_NODE,WLAN_AC_ITF_NAME_FORMAT,bufpart,sizeof(bufpart), 1);	
		if(strlen(param) == 0)	
			snprintf(param,length,"%s",bufpart);		
		else if(strlen(bufpart) > 0)
			strcat(param,bufpart);
	}
	if(strlen(param) > 0)
	{
		param[strlen(param)-1] = '\0';
	}	
	return 0;
}
