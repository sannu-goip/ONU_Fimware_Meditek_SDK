/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <cfg_type.h>
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include <utility.h>
#include "cfg_types.h"

#define MAX_NUM_OF_RADIO 6

const int NOISE_OFFSET_BY_CH_WIDTH[] = {
    -94, //20 Mhz
    -91, //40 Mhz
    -88, //80 Mhz
    -85, //160 Mhz
};

extern cfg_node_type_t cfg_type_mesh_radio2g;
extern cfg_node_type_t cfg_type_mesh_radio5gl;
extern cfg_node_type_t cfg_type_mesh_radio5gh;
extern cfg_node_type_t cfg_type_mesh_apcli;


static void cfg_type_mesh_send_evt(int type, int evt_id, char *buf)
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
    
    cfg_obj_send_event(evt_type, evt_id, (void *)&evt , sizeof(mesh_evt_t));
    return ;
}


int svc_cfg_boot_mesh(void)
{	
	cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_MESH_BOOT, MESH_NODE);
	return 0;
} 

static int cfg_type_mesh_radio_bss_cfg_func_commit(char* path)
{
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_RADIO, path);
    return 0;
}

static int cfg_type_mesh_action_commit(char* path)
{
    int ret = 0;
    int evt_id =  0;
    
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_ACTION, path);

    return ret;    
}

static int cfg_type_mesh_dat_commit(char* path)
{
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_DAT, path);
    return 0;
}

static int cfg_type_mesh_steercfg_commit(char* path)
{
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_STEER, path);
    return 0;
}

static int cfg_type_mesh_mapcfg_commit(char* path)
{
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_MAP, path);

	return 0;
}
static int cfg_type_mesh_mapdcfg_commit(char* path)
{
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_MAPD, path);
    return 0;
}

static int cfg_type_mesh_common_commit(char* path)
{
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_COMMON, path);
    return 0;
}

#if defined(TCSUPPORT_CT_JOYME4)
static int cfg_type_mesh_enable_conf_commit(char* path)
{
	char changeControllerFlag[8] = {0};
	char EasyMeshEnable[8] = {0};
	char cmd[256] = {0};
	char enableAuto[8] = {0};
	FILE *fp = NULL;
	char tmp[256] = {0};
	int res = 0;
	
	cfg_get_object_attr(MESH_ENABLE_CONF_NODE, "EasyMeshEnableAuto", enableAuto, sizeof(enableAuto));
	if (1 == atoi(enableAuto))
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "dbus-send --system --print-reply --dest=com.upointech.easymesh1 /com/ctc/map1/Management com.ctc.igd1.Properties.Get string:\"com.ctc.map1.Management\" string:\"Enable\" > /tmp/easymeshEnable.txt");
		system(cmd);
		fp = fopen("/tmp/easymeshEnable.txt", "r");
		if (fp)
		{
			res = fread(tmp, sizeof(char), sizeof(tmp) - 1, fp);
			fclose(fp);
			unlink("/tmp/easymeshEnable.txt");
			if (NULL != strstr(tmp, "true"))
			{
				cfg_set_object_attr(MESH_ENABLE_CONF_NODE, "EasyMeshEnable", "1");
			}
			else if (NULL != strstr(tmp, "false"))
			{
				cfg_set_object_attr(MESH_ENABLE_CONF_NODE, "EasyMeshEnable", "0");
			}
			else
				tcdbg_printf("get controller mesh switch failed\n");
		}
	}
	else
	{
		if (0 <= cfg_get_object_attr(MESH_ENABLE_CONF_NODE, "changeControllerFlag", changeControllerFlag, sizeof(changeControllerFlag)) &&
			atoi(changeControllerFlag) == 1)
		{
			memset(cmd, 0, sizeof(cmd));
			cfg_set_object_attr(MESH_ENABLE_CONF_NODE, "changeControllerFlag", "0");
			cfg_get_object_attr(MESH_ENABLE_CONF_NODE, "EasyMeshEnable", EasyMeshEnable, sizeof(EasyMeshEnable));
			if (atoi(EasyMeshEnable) == 1)
				snprintf(cmd, sizeof(cmd), "dbus-send --system --print-reply --dest=com.upointech.easymesh1 /com/ctc/map1/Management com.ctc.igd1.Properties.Set string:\"com.ctc.map1.Management\" string:\"Enable\" variant:boolean:true");
			else
				snprintf(cmd, sizeof(cmd), "dbus-send --system --print-reply --dest=com.upointech.easymesh1 /com/ctc/map1/Management com.ctc.igd1.Properties.Set string:\"com.ctc.map1.Management\" string:\"Enable\" variant:boolean:false");
			system(cmd);
		}
	}
	
    cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_DAT, path);
    return 0;
}
#endif

static char* cfg_type_mesh_radio2g_index[] = 
{
    "mesh_radio2gbssinfo",
    NULL
};

static char* cfg_type_mesh_radio5gl_index[] = 
{
    "mesh_radio5glbssinfo",
    NULL
};

static char* cfg_type_mesh_radio5gh_index[] = 
{
    "mesh_radio5ghbssinfo",
    NULL
};

static char* cfg_type_mesh_apcli_index[] = 
{
    "mesh_apclibh",
    NULL
};

#if defined(TCSUPPORT_MAP_R2)
int cfg_type_mesh_radio_bss_func_set(char* path,char* attr,char* val)
{
	int i;
	char tmp_primaryVlan[8] = {0};
	char nodePath[40] = {0};
	char bssidNum_2g[8] = {0}, bssidNum_5g[8];
	char deviceRole[8] = {0};

	if( cfg_get_object_attr(MESH_COMMON_NODE, "DeviceRole", deviceRole, sizeof(deviceRole)) < 0 )
	{
		return cfg_obj_set_object_attr(path,attr,0,val);
	}
	
	if (!strcmp(attr, "PrimaryVlan") && !strcmp("1", deviceRole))
	{
		if( cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", bssidNum_2g, sizeof(bssidNum_2g)) < 0 )
		{
			return cfg_obj_set_object_attr(path,attr,0,val);
		}

		if( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", bssidNum_5g, sizeof(bssidNum_5g)) < 0 )
		{
			return cfg_obj_set_object_attr(path,attr,0,val);
		}

		if( cfg_get_object_attr(path, attr, tmp_primaryVlan, sizeof(tmp_primaryVlan)) < 0 )
		{
			return cfg_obj_set_object_attr(path,attr,0,val);
		}
		
		if ( !strcmp("1", tmp_primaryVlan) && !strcmp("0", val) )
		{
			/* set all VlanID to 4059 */
			for (i = 0; i < atoi(bssidNum_2g); i++)
			{ 
				snprintf(nodePath, sizeof(nodePath), MESH_RADIO2G_BSSINFO_ENTRY_NODE, i+1);
				if(0 <= cfg_query_object(nodePath,NULL,NULL))
				{
					cfg_obj_set_object_attr(nodePath, "VlanID", 0, "4095");
				}
			}
			
			for (i = 0; i < atoi(bssidNum_5g); i++)
			{ 
				snprintf(nodePath, sizeof(nodePath), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, i+1);
				if(0 <= cfg_query_object(nodePath,NULL,NULL))
				{
					cfg_obj_set_object_attr(nodePath, "VlanID", 0, "4095");
				}
			}
			cfg_type_mesh_send_evt(EVT_MESH_INTERNAL_TYPE, EVT_CFG_UPDATE_DAT, "root.mesh.dat");	
			cfg_set_object_attr(MESH_COMMON_NODE, "ReinitWifiFlag", "1");
		}
	
		if ( !strcmp("0", tmp_primaryVlan) && !strcmp("1", val) )
		{
			/* reset old enable PrimaryVlan to 0 */
			for (i = 0; i < atoi(bssidNum_2g); i++)
			{
				
				snprintf(nodePath, sizeof(nodePath), MESH_RADIO2G_BSSINFO_ENTRY_NODE, i+1);
				if(0 <= cfg_query_object(nodePath, NULL, NULL))
				{
					/* if this is current node. */
					if ( !strcmp(nodePath, path) )
					{
						continue;
					}
					if ( cfg_get_object_attr(nodePath, attr, tmp_primaryVlan, sizeof(tmp_primaryVlan)) < 0 )
					{
						return cfg_obj_set_object_attr(path,attr,0,val);
					}
					if ( 0 != strcmp("0", tmp_primaryVlan) )
					{
						cfg_obj_set_object_attr(nodePath, attr, 0, "0");
					}
				}
			}

			for (i = 0; i < atoi(bssidNum_5g); i++)
			{			
			
				snprintf(nodePath, sizeof(nodePath), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, i+1);
			
				if(0 <= cfg_query_object(nodePath,NULL,NULL))
				{
			
					/* if this is current node. */
					if ( !strcmp(nodePath, path) )
					{
						continue;
					}
					if ( cfg_get_object_attr(nodePath, attr, tmp_primaryVlan, sizeof(tmp_primaryVlan)) < 0 )
					{
						return cfg_obj_set_object_attr(path,attr,0,val);
					}
					if ( 0 != strcmp("0", tmp_primaryVlan) )
					{
						cfg_obj_set_object_attr(nodePath, attr, 0, "0");
					}
				}			
			}
		}
	}
	return cfg_obj_set_object_attr(path,attr,0,val);
}
#endif

/***************************5gh*********************************/
static cfg_node_ops_t cfg_type_mesh_radio5gh_bss_cfg_ops = 
{
#if defined(TCSUPPORT_MAP_R2)
	.set	= cfg_type_mesh_radio_bss_func_set,
#else
	.set	= cfg_type_default_func_set,
#endif
    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_radio_bss_cfg_func_commit,
    .create  = cfg_type_default_func_create,
    .delete = cfg_type_default_func_delete,
};


static cfg_node_type_t cfg_type_mesh_radio5gh_bss_cfg = 
{
    .name = "entry",
    .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_WLAN_ENTRY_NUM,
    .parent = &cfg_type_mesh_radio5gh,
    .index  = cfg_type_mesh_radio5gh_index,
    .ops = &cfg_type_mesh_radio5gh_bss_cfg_ops,

};

static cfg_node_type_t* cfg_type_mesh_radio5gh_child[] = 
{
    &cfg_type_mesh_radio5gh_bss_cfg,
    NULL
};

static cfg_node_ops_t cfg_type_mesh_radio5gh_ops = 
{
    .set    = cfg_type_default_func_set,
    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_default_func_commit,
};

cfg_node_type_t cfg_type_mesh_radio5gh = 
{
    .name = "radio5ghbssinfo",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .nsubtype = sizeof(cfg_type_mesh_radio5gh_child) / sizeof(cfg_node_type_t *) - 1,
    .subtype = cfg_type_mesh_radio5gh_child,
    .ops = &cfg_type_mesh_radio5gh_ops,
};

/***************************5gl*********************************/
static cfg_node_ops_t cfg_type_mesh_radio5gl_bss_cfg_ops = 
{
#if defined(TCSUPPORT_MAP_R2)
    .set    = cfg_type_mesh_radio_bss_func_set,
#else
	.set    = cfg_type_default_func_set,
#endif
    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_radio_bss_cfg_func_commit,
    .create  = cfg_type_default_func_create,
    .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_mesh_radio5gl_bss_cfg = 
{
    .name = "entry",
    .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_WLAN_ENTRY_NUM,
    .parent = &cfg_type_mesh_radio5gl,
    .index  = cfg_type_mesh_radio5gl_index,
    .ops = &cfg_type_mesh_radio5gl_bss_cfg_ops,

};


static cfg_node_type_t* cfg_type_mesh_radio5gl_child[] = 
{
    &cfg_type_mesh_radio5gl_bss_cfg,
    NULL
};

static cfg_node_ops_t cfg_type_mesh_radio5gl_ops = 
{
    .set    = cfg_type_default_func_set,
    .get    = cfg_type_default_func_get,
    .commit = cfg_type_default_func_commit,
    .query  = cfg_type_default_func_query
};

cfg_node_type_t cfg_type_mesh_radio5gl = 
{
    .name = "radio5glbssinfo",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .nsubtype = sizeof(cfg_type_mesh_radio5gl_child) / sizeof(cfg_node_type_t *) - 1,
    .subtype = cfg_type_mesh_radio5gl_child,
    .ops = &cfg_type_mesh_radio5gl_ops,
};
/***************************2gl*********************************/
static cfg_node_ops_t cfg_type_mesh_radio2g_bss_cfg_ops = 
{
#if defined(TCSUPPORT_MAP_R2)
	.set	= cfg_type_mesh_radio_bss_func_set,
#else
	.set	= cfg_type_default_func_set,
#endif

    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_radio_bss_cfg_func_commit,
    .create  = cfg_type_default_func_create,
    .delete = cfg_type_default_func_delete,
};

static cfg_node_type_t cfg_type_mesh_radio2g_bss_cfg = 
{
    .name = "entry",
    .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_WLAN_ENTRY_NUM,
    .parent = &cfg_type_mesh_radio2g,
    .index  = cfg_type_mesh_radio2g_index,
    .ops = &cfg_type_mesh_radio2g_bss_cfg_ops,

};

static cfg_node_type_t* cfg_type_mesh_radio2g_child[] = 
{
    &cfg_type_mesh_radio2g_bss_cfg,
    NULL
};

static cfg_node_ops_t cfg_type_mesh_radio2g_ops = 
{
    .set    = cfg_type_default_func_set,
    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_default_func_commit,
};

cfg_node_type_t cfg_type_mesh_radio2g = 
{
    .name = "radio2gbssinfo",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .nsubtype = sizeof(cfg_type_mesh_radio2g_child) / sizeof(cfg_node_type_t *) - 1,
    .subtype = cfg_type_mesh_radio2g_child,
    .ops = &cfg_type_mesh_radio2g_ops,
};

static cfg_node_ops_t cfg_type_mesh_apcli_cfg_ops = 
{
    .set    = cfg_type_default_func_set,
    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_default_func_commit,
    .create = cfg_type_default_func_create,
    .delete = cfg_type_default_func_delete,
};


static cfg_node_type_t cfg_type_mesh_apcli_cfg = 
{
    .name = "entry",
    .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_NUM_OF_RADIO,
    .parent = &cfg_type_mesh_apcli,
    .index  = cfg_type_mesh_apcli_index,
    .ops = &cfg_type_mesh_apcli_cfg_ops,

};

static cfg_node_type_t* cfg_type_mesh_apcli_child[] = 
{
    &cfg_type_mesh_apcli_cfg,
    NULL
};


static cfg_node_ops_t cfg_type_mesh_apcli_ops = 
{
    .set    = cfg_type_default_func_set,
    .get    = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_default_func_commit,
};

cfg_node_type_t cfg_type_mesh_apcli = 
{
    .name = "apclibh",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .nsubtype = sizeof(cfg_type_mesh_apcli_child) / sizeof(cfg_node_type_t *) - 1,
    .subtype = cfg_type_mesh_apcli_child,
    .ops = &cfg_type_mesh_apcli_ops,
};


/***************************steer*********************************/
static cfg_node_ops_t cfg_type_mesh_steer_cfg_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_steercfg_commit
};

static cfg_node_type_t cfg_type_mesh_steer_cfg = 
{
    .name = "steercfg",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_steer_cfg_ops,
};

static int cfg_type_mesh_mapd_cfg_func_set(char* path,char* attr,char* val)
{
	int lowRssVal = 0;
	char lowRssBuf[8];
	
	if (!strcmp(attr, "APSteerRssiTh"))
	{
		sscanf(val, "%d", &lowRssVal);
		/*lowRssVal = lowRssVal + 94;*/
		lowRssVal = lowRssVal - NOISE_OFFSET_BY_CH_WIDTH[0];
		memset(lowRssBuf, 0, sizeof(lowRssBuf));
		snprintf(lowRssBuf, sizeof(lowRssBuf), "%d", lowRssVal);
		cfg_obj_set_object_attr("root.mesh.steercfg", "LowRSSIAPSteerEdge_RE", 0, lowRssBuf);
	}
	
	return cfg_obj_set_object_attr(path, attr, 0, val);
}

static int cfg_type_mesh_common_func_get(char* path,char* attr,char* val,int len)
{
	int i = 0, flag = 0;
	char nodeName[32] = {0};
	char EnableSSID[8] = {0};
	char AuthMode[32] = {0};
	char EncrypType[32] = {0};
	char isFrontHaul[32] = {0};
	char isBackHaul[32] = {0};
	char frontHaulNode[64] = {0};
	char backHaulNode[64] = {0};
	
	if (!strcmp(attr, "isMeshEncrypTypeAuthModeSupport"))
	{
		for(i = 1; i <= 16; i++)
		{
			memset(nodeName, 0, sizeof(nodeName));
			memset(frontHaulNode, 0, sizeof(frontHaulNode));
			if ( i < 9 ){
				snprintf(frontHaulNode, sizeof(frontHaulNode), MESH_RADIO2G_BSSINFO_ENTRY_NODE, i);
				snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, i);
			}
			else{
				snprintf(frontHaulNode, sizeof(frontHaulNode), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, i - 8);
				snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, i - 8);	
			}
			
			memset(EnableSSID, 0, sizeof(EnableSSID));
			cfg_obj_get_object_attr(nodeName, "EnableSSID", 0, EnableSSID, sizeof(EnableSSID));
			if (0 != strcmp(EnableSSID, "1"))
				continue;
			
			memset(AuthMode, 0, sizeof(AuthMode));
			memset(EncrypType, 0, sizeof(EncrypType));
			memset(isFrontHaul, 0, sizeof(isFrontHaul));
			cfg_obj_get_object_attr(nodeName, "AuthMode", 0, AuthMode, sizeof(AuthMode));
			cfg_obj_get_object_attr(nodeName, "EncrypType", 0, EncrypType, sizeof(EncrypType));
			cfg_obj_get_object_attr(frontHaulNode, "FrontHaul", 0, isFrontHaul, sizeof(isFrontHaul));

			if ((0 != strcmp(AuthMode, "OPEN") && 0 != strcmp(AuthMode, "WPA2PSK") && 0 !=  strcmp(AuthMode, "WPA2PSKWPA3PSK")  && 0 !=  strcmp(AuthMode, "WPA3PSK")) 
				|| (0 != strcmp(EncrypType, "NONE") && 0 != strcmp(EncrypType, "AES"))
				||  (1 == atoi(isFrontHaul) && 0 ==  strcmp(AuthMode, "WPA3PSK")))
			{
			    if (0 != strcmp(AuthMode, "OPEN"))
			    {
					flag = 1;
					break;
				}
			}
		}
		
		if (1 == flag)
			cfg_obj_set_object_attr(MESH_COMMON_NODE, "isMeshEncrypTypeAuthModeSupport", 0, "No");
		else
			cfg_obj_set_object_attr(MESH_COMMON_NODE, "isMeshEncrypTypeAuthModeSupport", 0, "Yes");
	}
	
	else if (!strcmp(attr, "isMeshAllBHOrFHClosedStatus"))
	{
		for( i = 1; i <= 16; i++ )
		{
			memset(nodeName, 0, sizeof(nodeName));
			memset(frontHaulNode, 0, sizeof(frontHaulNode));
			if ( i < 9 ){
				snprintf(frontHaulNode, sizeof(frontHaulNode), MESH_RADIO2G_BSSINFO_ENTRY_NODE, i);
				snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, i);
			}
			else{
				snprintf(frontHaulNode, sizeof(frontHaulNode), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, i - 8);
				snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, i - 8);	
			}
			
			memset(EnableSSID, 0, sizeof(EnableSSID));
			cfg_obj_get_object_attr(nodeName, "EnableSSID", 0, EnableSSID, sizeof(EnableSSID));
			if ( 0 != strcmp(EnableSSID, "1") )
			{
					continue;
			}					
			
			memset(isFrontHaul, 0, sizeof(isFrontHaul));
			memset(isBackHaul, 0, sizeof(isBackHaul));
			cfg_obj_get_object_attr(frontHaulNode, "FrontHaul", 0, isFrontHaul, sizeof(isFrontHaul));
			cfg_obj_get_object_attr(backHaulNode, "BackHaul", 0, isBackHaul, sizeof(isBackHaul));	
			if ( (1 == atoi(isFrontHaul)) || (1 == atoi(isBackHaul)) )				
			{
				flag = 1;
				break;			   
			}
		}		
		if ( 1 == flag )
		{
			cfg_obj_set_object_attr(MESH_COMMON_NODE, "isMeshAllBHOrFHClosedStatus", 0, "No");
		} 			
		else
		{
			cfg_obj_set_object_attr(MESH_COMMON_NODE, "isMeshAllBHOrFHClosedStatus", 0, "Yes");
		}		
	}
	
	return cfg_obj_get_object_attr(path, attr, 0, val, len);
}

static cfg_node_ops_t cfg_type_mesh_mapd_cfg_ops = 
{
    .set = cfg_type_mesh_mapd_cfg_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_mapdcfg_commit
};

static cfg_node_type_t cfg_type_mesh_mapd_cfg = 
{
    .name = "mapdcfg",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_mapd_cfg_ops,
};

static cfg_node_ops_t cfg_type_mesh_map_cfg_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_mapcfg_commit
};

static cfg_node_type_t cfg_type_mesh_map_cfg = 
{
    .name = "mapcfg",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_map_cfg_ops,
};
#if defined(TCSUPPORT_MAP_R2)
int cfg_type_mesh_common_func_set(char* path,char* attr,char* val)
{
	char DeviceRole[16] = {0};

	if (path == NULL || attr == NULL|| val == NULL)
		return cfg_obj_set_object_attr(path,attr,0,val);

	if (!strcmp(attr, "DeviceRole"))
	{
		cfg_obj_get_object_attr(path, "DeviceRole", 0, DeviceRole, sizeof(DeviceRole));
		if ('\0' != DeviceRole[0] && !strcmp(DeviceRole, "1") && strcmp(DeviceRole, val))
		{
			cfg_obj_set_object_attr(path, "CleanApclibhFlag", 0, " 1");
		}
	}

	return cfg_obj_set_object_attr(path,attr,0,val);
}
#endif
static cfg_node_ops_t cfg_type_mesh_common_ops = 
{
#if defined(TCSUPPORT_MAP_R2)
    .set = cfg_type_mesh_common_func_set,
#else
    .set = cfg_type_default_func_set,
#endif
    .get = cfg_type_mesh_common_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_common_commit
};

static cfg_node_type_t cfg_type_mesh_common = 
{
    .name = "common",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_common_ops,
};

static cfg_node_ops_t cfg_type_mesh_actionpara_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_default_func_commit
};

static cfg_node_type_t cfg_type_mesh_actionpara = 
{
    .name = "actionpara",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_actionpara_ops,
};

static cfg_node_ops_t cfg_type_mesh_action_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_action_commit
};

static cfg_node_type_t cfg_type_mesh_action = 
{
    .name = "action",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_action_ops,
};

static cfg_node_ops_t cfg_type_mesh_dat_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_dat_commit
};

static cfg_node_type_t cfg_type_mesh_dat = 
{
    .name = "dat",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_dat_ops,
};

static cfg_node_ops_t cfg_type_mesh_DefSetting_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_default_func_commit
};

static cfg_node_type_t cfg_type_mesh_DefSetting = 
{
    .name = "defsetting",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_DefSetting_ops,
};

#if defined(TCSUPPORT_CT_JOYME4)
static cfg_node_ops_t cfg_type_mesh_enable_conf_ops = 
{
    .set = cfg_type_default_func_set,
    .get = cfg_type_default_func_get,
    .query  = cfg_type_default_func_query,
    .commit = cfg_type_mesh_enable_conf_commit
};

static cfg_node_type_t cfg_type_mesh_enable_conf = 
{
    .name = "EnableConf",
    .flag = 1,
    .parent = &cfg_type_mesh,
    .ops = &cfg_type_mesh_enable_conf_ops,
};
#endif

static cfg_node_type_t* cfg_type_mesh_child[] = 
{
    &cfg_type_mesh_dat,
    &cfg_type_mesh_common,
    &cfg_type_mesh_action,
    &cfg_type_mesh_actionpara,
    &cfg_type_mesh_map_cfg,
    &cfg_type_mesh_mapd_cfg,
    &cfg_type_mesh_steer_cfg,
    &cfg_type_mesh_radio2g,
    &cfg_type_mesh_radio5gl,
    &cfg_type_mesh_radio5gh,
    &cfg_type_mesh_DefSetting,
#if defined(TCSUPPORT_CT_JOYME4)
    &cfg_type_mesh_enable_conf,
#endif
    &cfg_type_mesh_apcli,
    NULL
};

static cfg_node_ops_t cfg_type_mesh_ops = 
{
	.set = cfg_type_default_func_set,
	.get = cfg_type_default_func_get,
	.query  = cfg_type_default_func_query,
	.commit = cfg_type_default_func_commit
};

cfg_node_type_t cfg_type_mesh = 
{
    .name = "Mesh",
    .flag = 1,
    .parent = &cfg_type_root,
    .nsubtype = sizeof(cfg_type_mesh_child) / sizeof(cfg_node_type_t *) - 1,
    .subtype = cfg_type_mesh_child,
    .ops = &cfg_type_mesh_ops,
};
