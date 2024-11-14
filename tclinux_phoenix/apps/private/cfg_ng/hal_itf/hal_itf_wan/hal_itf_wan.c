#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <cfg_cli.h> 
#include <svchost_evt.h>
#include "hal_itf_wan.h"
#include "utility.h"
/***************************************************************************/
typedef struct _ecnt_wan_info_
{
    int  pos;
    char* attrname;
    char* value;
}ecnt_wan_info_t, *pt_ecnt_wan_info;

/***************************************************************************/
#if !defined(TCSUPPORT_CMCC)
static int get_default_pvc_info(int pos, ecnt_wan_info_t* info);
static void set_default_pvc_entry_info(int pvc_index, int entry_index);
static int find_pvc_index(char* vlan, char* pbit);
static int sync_wan_pvc_entry(int pvc,int entry, char* old_path);
static int wan_link_path_is_change(char* path, int path_size, int type, char* value, char* path_change);
static int update_pvc_info(int dstpvc, char* vlan, char* pbit);
#endif
static void set_default_pvc_info(int pvc_index);
static void set_default_wan_link(int pvc_index, int entry_index);
static void update_ifIdex_info(int pvc_index, int entry_index);
static int wan_link_get_empty_pvc(void);
static int wan_link_get_empty_entry(int pvc);
static int wan_link_create_empty_entry(int dstpvc);
static int creat_default_wan_link_pvc_entry(void);
static void wan_link_send_evt(int type, int evt_id, char* buf);
static char is_exist_srv_tr069(char* node, int size);
char is_exist_srv_voice(char* node, int size);

static int get_pvc_path(char* entry_path, char* pvc_path, int pvc_path_size);
static int convert_waninfo_attrname(ecnt_wan_if_type type, char* attrname, int size);
static int convert_wan_entry_attrname(ecnt_wan_if_type type, char* attrname, int size);
static void hal_itf_wan_printf(char *fmt, ...);
/***************************************************************************/
#define HAL_ITF_WAN_DEBUG_FILE  "/tmp/hal_itf_wan_debug"
static void hal_itf_wan_printf(char *fmt,...)
{
    FILE *proc_file = NULL;
    char msg[256];
    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 256, fmt, args);	

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

#define HAL_ITF_WAN_DEBUG(fmt, ...)  do{ \
                                        if(0 == access(HAL_ITF_WAN_DEBUG_FILE, F_OK)) \
                                        { \
                                            hal_itf_wan_printf("Debug info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        } \
                                    }while(0)

#define HAL_ITF_WAN_ERR(fmt, ...)  do{ \
                                        if(0 == access(HAL_ITF_WAN_DEBUG_FILE, F_OK)) \
                                        { \
                                            hal_itf_wan_printf("err info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        } \
                                    }while(0)
/***************************************************************************/
enum _ecnt_wan_pvc_pos_
{
    E_VLAN_ID_OPS            = 0x0,
    E_DOT1P_DATA_OPS         = 0x1,
};
enum ecnt_wan_pvc_entry_pos
{
    E_ACTIVE_OPS        = 0x0,
    E_WAN_MODE_OPS      = 0x1,
    E_LINK_MODE_OPS     = 0x2,
    E_VLAN_MODE_OPS     = 0x3,
    E_ISP_OPS           = 0x4,
    E_IP_VERSION_OPS    = 0x5,
    E_SRV_LIST_OPS      = 0x6,
    E_MTU_OPS           = 0x7,
    E_DHCP_ENABLE_OPS   = 0x8,
    E_MVID_OPS          = 0x9,
    E_NAT_OPS           = 0xA,
    E_CREAT_OMCI_OPS    = 0xB,
	E_DOT1Q_OPS         = 0xC,
	E_DOT1P_OPS         = 0xD,
};
/***************************************************************************/
static ecnt_wan_info_t default_pvc_info[]=
{
    {E_VLAN_ID_OPS,         "VLANID",           "20"},
    {E_DOT1P_DATA_OPS,      "DOT1P",            "0"},
};
#define ECNT_DEFAULT_PVC_ARRAY_SIZE ((sizeof(default_pvc_info)) / (sizeof(default_pvc_info[0])))

#if !defined(TCSUPPORT_CMCC)
static int get_default_pvc_info(int pos, ecnt_wan_info_t* info)
{
    int i = 0;
    if(NULL == info)
    {
        HAL_ITF_WAN_ERR("info can't be NULL.\n");
        return HAL_ITF_WAN_FAIL;
    }

    if(pos == default_pvc_info[pos].pos)
    {
        memcpy(info, &default_pvc_info[pos], sizeof(ecnt_wan_info_t));
        return HAL_ITF_WAN_SUCCESS;
    }

    for(i=0; i < ECNT_DEFAULT_PVC_ARRAY_SIZE; i++)
    {
        if(pos == default_pvc_info[i].pos)
        {
            memcpy(info, &default_pvc_info[i], sizeof(ecnt_wan_info_t));
            return HAL_ITF_WAN_SUCCESS;
        }
    }

    return HAL_ITF_WAN_FAIL;
}
#endif

static void set_default_pvc_info(int pvc_index)
{
    int i = 0;
    char path[32];
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), WAN_PVC_NODE, pvc_index);
    for(i=0; i<ECNT_DEFAULT_PVC_ARRAY_SIZE; i++)
    {
        cfg_set_object_attr(path, default_pvc_info[i].attrname, default_pvc_info[i].value);
    }

    return ;
}
/***************************************************************************/
static ecnt_wan_info_t default_pvc_entry_info[] =
{
	{E_ACTIVE_OPS,          "Active",           "Yes"},
	{E_WAN_MODE_OPS,        "WanMode",          "Route"},
	{E_LINK_MODE_OPS,       "LinkMode",         "linkIP"},
	{E_VLAN_MODE_OPS,       "VLANMode",         "TAG"},
	{E_ISP_OPS,             "ISP",              "0"},
	{E_IP_VERSION_OPS,      "IPVERSION",        "IPv4"},
	{E_SRV_LIST_OPS,        "ServiceList",      ""},
	{E_MTU_OPS,             "MTU",              "1500"},
	{E_DHCP_ENABLE_OPS,     "DHCPEnable",       "1"},
	{E_NAT_OPS,             "NATENABLE",        "Enable"},
	{E_CREAT_OMCI_OPS,      "CreateByOMCI",     "Yes"},
	{E_DOT1Q_OPS,           "dot1q",            "Yes"},
	{E_DOT1P_OPS,           "dot1p",            "Yes"},
};
#define ECNT_DEFAULT_PVC_ENTRY_ARRAY_SIZE ((sizeof(default_pvc_entry_info)) / (sizeof(default_pvc_entry_info[0])))
#if 0
static int get_default_pvc_entry_info(int pos, ecnt_wan_info_t* info)
{
	int i = 0;
	if(NULL == info)
	{
		printf("info can't be NULL.\n");
		return HAL_ITF_WAN_FAIL;
	}
	
	if(pos == default_pvc_entry_info[pos].pos)
	{
		memcpy(info, &default_pvc_entry_info[pos], sizeof(ecnt_wan_info_t));
		return HAL_ITF_WAN_SUCCESS;
	}
	
	for(i=0; i < ECNT_DEFAULT_PVC_ENTRY_ARRAY_SIZE; i++)
	{
		if(pos == default_pvc_entry_info[i].pos)
		{
			memcpy(info, &default_pvc_entry_info[i], sizeof(ecnt_wan_info_t));
			return HAL_ITF_WAN_SUCCESS;
		}
	}

	return HAL_ITF_WAN_FAIL;
}
#endif
static void set_default_pvc_entry_info(int pvc_index, int entry_index)
{
    int i = 0;
    char path[32];
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);

    for(i=0; i<ECNT_DEFAULT_PVC_ARRAY_SIZE; i++)
    {
        if(0 == strcmp(default_pvc_info[i].attrname, "DOT1P"))
        {
            cfg_set_object_attr(path, "dot1pData", default_pvc_info[i].value);
        }
        else
        {
            cfg_set_object_attr(path, default_pvc_info[i].attrname, default_pvc_info[i].value);
        }
    }

    for(i=0; i<ECNT_DEFAULT_PVC_ENTRY_ARRAY_SIZE; i++)
    {
        cfg_set_object_attr(path, default_pvc_entry_info[i].attrname, default_pvc_entry_info[i].value);
    }

    return ;
}
/***************************************************************************/
static void update_ifIdex_info(int pvc_index, int entry_index)
{
    char path[32];
    char buf[32];
    int if_idx = 0;
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), WAN_COMMON_NODE);
    memset(buf, 0, sizeof(buf));
    if(cfg_get_object_attr(path, "LatestIFIdx", buf, sizeof(buf)) < 0)
    {
        return ;
    }

    if_idx = atoi(buf);
    if_idx += 1;
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", if_idx);
    cfg_set_object_attr(path, "LatestIFIdx", buf);

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
    cfg_set_object_attr(path, "IFIdx", buf);

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), WAN_COMMON_NODE);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", (pvc_index - 1) * MAX_WAN_ENTRY_NUMBER + entry_index -1);
    cfg_set_object_attr(path, "CurIFIndex", buf);

    return ;    
}

static void set_default_wan_link(int pvc_index, int entry_index)
{
    set_default_pvc_info(pvc_index);
    set_default_pvc_entry_info(pvc_index, entry_index);
    return ;
}
/***************************************************************************/
#if !defined(TCSUPPORT_CMCC)
static int find_pvc_index(char* vlan, char* pbit)
{
    int  i = 0;
    char tmp[32];
    char node[32];

    HAL_ITF_WAN_DEBUG("vlan = %s, pbit = %s.\n", vlan, pbit);

    for(i=1; i<= MAX_WAN_PVC_NUMBER; i++)
    {
        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), WAN_PVC_NODE,i);

        memset(tmp, 0, sizeof(tmp));
        if (cfg_get_object_attr(node, "VLANID", tmp, sizeof(tmp)) <= 0)
        {
            continue;
        }

        if (0 != strcmp(vlan,tmp))
        {
            continue;
        }

        memset(tmp, 0, sizeof(tmp));
        if (cfg_get_object_attr(node, "DOT1P", tmp, sizeof(tmp)) <= 0)
        {
            continue;
        }

        if (0 != strcmp(pbit,tmp))
        {
            continue;
        }

        return i;
    }

    return -1;
}

static int sync_wan_pvc_entry(int pvc,int entry, char* old_path)
{
    char tmp[32];

    snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE, pvc, entry);

    return cfg_obj_copy_private(tmp, old_path);
}
#endif

static int wan_link_get_empty_pvc(void)
{
    int i = 0;
    char tmp[32];

    for(i=1; i<= MAX_WAN_PVC_NUMBER; i++)
    {
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, i);
        if (cfg_query_object(tmp,NULL,NULL) <= 0)
        {
            return i;
        }
    }

    return -1;
}

static int wan_link_get_empty_entry(int pvc)
{
    int i;
    char tmp[32];

    for(i=1; i<= MAX_WAN_ENTRY_NUMBER; i++)
    {
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE, pvc, i);
        if (cfg_query_object(tmp,NULL,NULL) <= 0)
        {
            return i;
        }
    }

    return -1;
}

static int wan_link_create_empty_entry(int dstpvc)
{
    int dstentry = 0;
    char tmp[32];

    if (dstpvc > 0)
    {
        /*creat entry*/
        if ((dstentry = wan_link_get_empty_entry(dstpvc)) <= 0)
        {
            return -1;
        }
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE, dstpvc, dstentry);
        if (cfg_create_object(tmp) < 0)
        {
            return -1;
        }
    }
    else
    {
        /*creat pvc*/
        if ((dstpvc = wan_link_get_empty_pvc()) <= 0)
        {
            return -1;
        }
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, dstpvc);
        if (cfg_create_object(tmp) < 0)
        {
            return -1;
        }

        /*creat entry*/
        dstentry = 1;
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), WAN_PVC_ENTRY_NODE, dstpvc, dstentry);
        if (cfg_create_object(tmp) < 0)
        {
            return -1;
        }
    }
    
    return (dstpvc - 1) * MAX_WAN_ENTRY_NUMBER + dstentry - 1;
}

static int creat_default_wan_link_pvc_entry(void)
{
    int dstpvc = -1;
    int ifindex = -1;
#if defined(TCSUPPORT_CMCC)
    char path[32];
#else
    int ret = HAL_ITF_WAN_FAIL;
    char vlan[32];
    char pbit[32];
    ecnt_wan_info_t info; 
#endif

#if defined(TCSUPPORT_CMCC)
    dstpvc = 1;
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), WAN_PVC_NODE, dstpvc);
    if (cfg_query_object(path,NULL,NULL) <= 0)
    {
        if (cfg_create_object(path) < 0)
        {
            return -1;
        }
    }
#else
    /*prepare vlan,pbit*/
    memset(&info, 0, sizeof(info));
    memset(vlan, 0, sizeof(vlan));
    ret= get_default_pvc_info(E_VLAN_ID_OPS, &info);
    if(HAL_ITF_WAN_FAIL == ret)
    {
        return -1;
    }
    snprintf(vlan, sizeof(vlan), "%s", info.value);


    memset(&info, 0, sizeof(info));
    memset(pbit, 0, sizeof(pbit));
    ret= get_default_pvc_info(E_DOT1P_DATA_OPS, &info);
    if(HAL_ITF_WAN_FAIL == ret)
    {
        return -1;
    }
    snprintf(pbit, sizeof(pbit), "%s", info.value);

    /*find pvc_index entry_index*/
    dstpvc = find_pvc_index(vlan, pbit);
#endif

    HAL_ITF_WAN_DEBUG("dstpvc = %d.\n", dstpvc);

    ifindex = wan_link_create_empty_entry(dstpvc);
    if(0 > ifindex)
    {
        HAL_ITF_WAN_ERR("create pvc or entry fail! \n");
        return -1;
    }

    return ifindex;
}
/***********************************************************************/
static void wan_link_send_evt(int type, int evt_id, char* buf)
{
    wan_evt_t wan_evt;
    memset(&wan_evt, 0, sizeof(wan_evt));
    char evt_type[EVT_TYPE_LENGTH];

    if((NULL == buf) || (strlen(buf) >= EVT_BUF_LENGTH))
    {
        HAL_ITF_WAN_ERR("error path is NULL || path length is too long.\n");
        return ;
    }

    strcpy(wan_evt.buf, buf);
    memset(evt_type, 0, sizeof(evt_type));
    switch (type)
    {
        case EVT_WAN_INTERNAL_TYPE :
        {
            snprintf(evt_type, sizeof(evt_type), EVT_WAN_INTERNAL);
            break;
        }
        case EVT_WAN_EXTERNAL_TYPE:
        {
            snprintf(evt_type, sizeof(evt_type), EVT_WAN_EXTERNAL);
            break;
        }
        default:
        {
            return ;
        }
    }

    cfg_send_event(evt_type, evt_id, (void*)&wan_evt, sizeof(wan_evt));

    return ;
}

/***************************************************************************/
int find_wan_path_by_serv(char *service,char* wanPath)
{
    char path[32];
    char buf[32];
    int i = 0;
    int j = 0;

    for (i = 0; i < MAX_WAN_PVC_NUMBER; i++)
    {
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), WAN_PVC_NODE,(i+1));
        if(cfg_query_object(path, NULL, NULL) <= 0)
        {
            continue;
        }
        
        for(j = 0; j < MAX_WAN_ENTRY_NUMBER; j++)
        {
            memset(path, 0, sizeof(path));
            snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE,(i+1), (j+1));
            if(cfg_query_object(path, NULL, NULL) <= 0)
            {
                continue;
            }

            memset(buf, 0, sizeof(buf));
            cfg_get_object_attr(path, "ServiceList", buf, sizeof(buf));
            if(0 == strcmp(buf, service))
            {
                strcpy(wanPath,path);
                return 1;
            }
        }
    }

    return 0;
}

/***************************************************************************/
static char is_exist_srv_tr069(char* node, int size)
{
    char path[32];
    char buf[32];
    int i = 0;
    int j = 0;

    for (i = 0; i < MAX_WAN_PVC_NUMBER; i++)
    {
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), WAN_PVC_NODE,(i+1));
        if(cfg_query_object(path, NULL, NULL) <= 0)
        {
            continue;
        }
        
        for(j = 0; j < MAX_WAN_ENTRY_NUMBER; j++)
        {
            memset(path, 0, sizeof(path));
            snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE,(i+1), (j+1));
            if(cfg_query_object(path, NULL, NULL) <= 0)
            {
                continue;
            }

            memset(buf, 0, sizeof(buf));
            cfg_get_object_attr(path, "ServiceList", buf, sizeof(buf));
            if(NULL != strstr(buf, "TR069"))
            {
                snprintf(node, size, path);
                return 1;
            }
        }
    }

    return 0;
}

char is_exist_srv_voice(char* node, int size)
{
    char path[32];
    char buf[32];
    int i = 0;
    int j = 0;

    for (i = 0; i < MAX_WAN_PVC_NUMBER; i++)
    {
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), WAN_PVC_NODE,(i+1));
        if(cfg_query_object(path, NULL, NULL) <= 0)
        {
            continue;
        }

        for(j = 0; j < MAX_WAN_ENTRY_NUMBER; j++)
        {
            memset(path, 0, sizeof(path));
            snprintf(path, sizeof(path), WAN_PVC_ENTRY_NODE,(i+1), (j+1));
            if(cfg_query_object(path, NULL, NULL) <= 0)
            {
                continue;
            }

            memset(buf, 0, sizeof(buf));
            cfg_get_object_attr(path, "ServiceList", buf, sizeof(buf));
            if(NULL != strstr(buf, "VOICE"))
            {
                snprintf(node, size, path);
                return 1;
            }
        }
    }

    return 0;
}

static int get_pvc_path(char* entry_path, char* pvc_path, int pvc_path_size)
{
    char* p = NULL;
    if((NULL == entry_path) || (0 >= strlen(entry_path)) || (NULL == pvc_path))
    {
        return -1;
    }

    p = strstr(entry_path, ".entry");
    if(NULL == p)
    {
        return -1;
    }
    if(pvc_path_size < (p-entry_path+1))
    {
        return -1;
    }
    memset(pvc_path, 0, pvc_path_size);
    snprintf(pvc_path, (p-entry_path+1), "%s", entry_path);

    return 0;
}

static int get_num_from_string(char *buffer, char *keyword, int *number)
{
	char *tmp = NULL;
	char *tmp2 = NULL;
	if((buffer == 0)||(keyword == 0)||(number == 0))
	{
		return -1;
	}
	if((tmp2 = strstr(buffer,keyword)))
	{
		tmp = tmp2+strlen(keyword);
		(*number)=atoi(tmp);
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


static unsigned char is_need_convert_path(ecnt_wan_if_type type)
{
	unsigned char need = 0;
	switch(type)
	{
		case E_WAN_IF_TYPE_IP_ADDR:
		case E_WAN_IF_TYPE_NETMASK:
		case E_WAN_IF_TYPE_PRI_DNS:
		case E_WAN_IF_TYPE_SEC_DNS:		
		case E_WAN_IF_TYPE_GATEWAY:
		{
			need = 1;
			break;
		}
		default:
		{
			need = 0;
 			break;
		}
	}

	return need;
}

static int convert_wan_entry_attrname(ecnt_wan_if_type type, char* attrname, int size)
{
	if(NULL == attrname)
	{
		return -1;
	}

	memset(attrname, 0, size);

	switch(type)
	{
		case E_WAN_IF_TYPE_LINK_MODE:
		{
			snprintf(attrname, size, "LinkMode");
			break;
		}
		case E_WAN_IF_TYPE_IP_ADDR:
		{
			snprintf(attrname, size, "IPADDR");
			break;
		}
		case E_WAN_IF_TYPE_NETMASK:
		{
			snprintf(attrname, size, "NETMASK");
			break;
		}
		case E_WAN_IF_TYPE_PRI_DNS:
		{
			snprintf(attrname, size, "DNS");
			break;
		}
		case E_WAN_IF_TYPE_SEC_DNS:
		{
			snprintf(attrname, size, "SecDNS");
			break;
		}
		case E_WAN_IF_TYPE_GATEWAY:
		{
			snprintf(attrname, size, "GATEWAY");
			break;
		}
		case E_WAN_IF_TYPE_ISP:
		{
			snprintf(attrname, size, "ISP");
			break;
		}
		case E_WAN_IF_TYPE_VLAN_ID:
		{
			snprintf(attrname, size, "VLANID");
			break;
		}
		case E_WAN_IF_TYPE_DOT1P:
		{
			snprintf(attrname, size, "dot1pData");
			break;
		}
		case E_WAN_IF_TYPE_SRV_LIST:
		{
			snprintf(attrname, size, "ServiceList");
			break;
		}
		case E_WAN_IF_TYPE_CREAT_OMCI:
		{
			snprintf(attrname, size, "CreateByOMCI");
			break;	
		}
        case E_WAN_IF_TYPE_ENCAP:
		{
			snprintf(attrname, size, "ENCAP");
			break;	
		}
        case E_WAN_IF_TYPE_PPPGETIP:
		{
			snprintf(attrname, size, "PPPGETIP");
			break;	
		}
        case E_WAN_IF_TYPE_CONNECTION:
		{
			snprintf(attrname, size, "CONNECTION");
			break;	
		}
        case E_WAN_IF_TYPE_USERNAME:
		{
			snprintf(attrname, size, "USERNAME");
			break;	
		}
        case E_WAN_IF_TYPE_PASSWORD:
		{
			snprintf(attrname, size, "PASSWORD");
			break;	
		}
        case E_WAN_IF_TYPE_NAT_ENABLE:
		{
			snprintf(attrname, size, "NATENABLE");
			break;	
		}
        case E_WAN_IF_TYPE_DHCP_ENABLE:
		{
			snprintf(attrname, size, "DHCPEnable");
			break;	
		}
        case E_WAN_IF_TYPE_PPPUNIT:
		{
			snprintf(attrname, size, "PPPUNIT");
			break;	
		}
        case E_WAN_IF_TYPE_IFNAME:
		{
			snprintf(attrname, size, "IFName");
			break;	
		}
        case E_WAN_IF_TYPE_CONNECTIONERROR:
		{
			snprintf(attrname, size, "ConnectionError");
			break;	
		}
        case E_WAN_IF_TYPE_CLOSEIFIDLE:
		{
			snprintf(attrname, size, "CLOSEIFIDLE");
			break;	
		}
        case E_WAN_IF_TYPE_AUTHEN:
		{
			snprintf(attrname, size, "AUTHEN");
			break;	
		}
        case E_WAN_IF_TYPE_SRVNAME:
		{
			snprintf(attrname, size, "SRVNAME");
			break;	
		}

		default:
		{
			return -1;
		}
	}

	return 0;
}

static int convert_waninfo_attrname(ecnt_wan_if_type type, char* attrname, int size)
{
    if(NULL == attrname)
    {
        return -1;
    }
    
    memset(attrname, 0, size);
    
    switch(type)
    {
        case E_WAN_IF_TYPE_IP_ADDR:
        {
            snprintf(attrname, size, "IP");
            break;
        }
        case E_WAN_IF_TYPE_NETMASK:
        {
            snprintf(attrname, size, "NetMask");
            break;
        }
        case E_WAN_IF_TYPE_PRI_DNS:
        {
            snprintf(attrname, size, "DNS");
            break;
        }
        case E_WAN_IF_TYPE_SEC_DNS:
        {
            snprintf(attrname, size, "SecDNS");
            break;
        }
        case E_WAN_IF_TYPE_GATEWAY:
        {
            snprintf(attrname, size, "GateWay");
            break;
        }
        default:
        {
            return -1;
        }
    }
    
    return 0;
}

#if !defined(TCSUPPORT_CMCC)
static int get_pvc_entry_num(char* path)
{
    int i;
    char tmp[32];
    int num = 0;
    if((NULL == path) || 0 >= strlen(path))
    {
        return -1;
    }

    for(i=1; i<= MAX_WAN_ENTRY_NUMBER; i++)
    {
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), "%s.entry.%d", path, i);
        if (cfg_query_object(tmp,NULL,NULL) > 0)
        {
            num++;
        }
    }

    return num;

}

static int wan_link_path_is_change(char* path, int path_size, int type, char* value, char* path_change)
{
    int         dstpvc = -1;
    int       dstentry = -1;
    char need_find_pvc = 0;
    int ifindex = -1;
    char tmp[32];
    char vlan[32];
    char pbit[8];
    char pvc_path[32];
    int entry_num = 0;
    char need_check_entry = 0;
    if((NULL == path) || (NULL == value) || (NULL == path_change))
    {
        return -1;
    }
    
    /*prepare pvc path*/
    memset(pvc_path, 0, sizeof(pvc_path));
    if(0 > get_pvc_path(path, pvc_path, sizeof(pvc_path)))
    {
        return -1;
    }

    /*prepare vlan && pbit*/
    if(E_WAN_IF_TYPE_VLAN_ID == type)
    {
        memset(tmp, 0 , sizeof(tmp));
        if(0 > cfg_get_object_attr(pvc_path, "VLANID", tmp, sizeof(tmp)))
        {
            return -1;
        }
        
        /*vlan not change*/
        if(0 == strcmp(tmp, value))
        {
            HAL_ITF_WAN_DEBUG("vlan not change.\n");
            return 0;
        }
        
        memset(tmp, 0 , sizeof(tmp));
        if(0 > cfg_get_object_attr(pvc_path, "DOT1P", tmp, sizeof(tmp)))
        {
            return -1;
        }
        
        snprintf(vlan, sizeof(vlan), "%s", value);
        snprintf(pbit, sizeof(pbit), "%s", tmp);
        need_find_pvc = 1;
    }
    else if(E_WAN_IF_TYPE_DOT1P == type)
    {
        memset(tmp, 0 , sizeof(tmp));
        if(0 > cfg_get_object_attr(pvc_path, "DOT1P", tmp, sizeof(tmp)))
        {
            return -1;
        }
        
        /*vlan not change*/
        if(0 == strcmp(tmp, value))
        {
            HAL_ITF_WAN_DEBUG("pbit not change.\n");
            return 0;
        }
        
        memset(tmp, 0 , sizeof(tmp));
        if(0 > cfg_get_object_attr(pvc_path, "VLANID", tmp, sizeof(tmp)))
        {
            return -1;
        }

        snprintf(vlan, sizeof(vlan), "%s", tmp);
        snprintf(pbit, sizeof(pbit), "%s", value);
        need_find_pvc = 1;
    }

    if(1 == need_find_pvc)
    {
        /*find new pvc_index entry_index*/
        dstpvc = find_pvc_index(vlan, pbit);
        HAL_ITF_WAN_DEBUG("dstpvc = %d.\n", dstpvc);
        /*not found*/
        if(0 > dstpvc)
        {
            need_check_entry = 1;
        }
        else
        {
            /*pasre pvc path diff*/
            memset(tmp, 0, sizeof(tmp));
            snprintf(tmp, sizeof(tmp), WAN_PVC_NODE, dstpvc);
            if(0 == strncmp(pvc_path, tmp, strlen(tmp)))
            {
                need_check_entry = 1;
            }
            else
            {
                *path_change = 1;
            }
        }
        
        if(1 == need_check_entry)
        {
            /*check old path entry*/
            entry_num = get_pvc_entry_num(pvc_path);
            if(0 >= entry_num)
            {
                return -1;
            }
            /*only one*/
            else if((MAX_WAN_ENTRY_NUMBER-1) == entry_num)
            {
                *path_change = 0;
            }
            /*multi entry*/
            else
            {
                *path_change = 1;
            }
        }
        
        HAL_ITF_WAN_DEBUG("need_check_entry = %d, entry_num = %d, path_change = %d.\n", need_check_entry, entry_num, *path_change);

        if(1 == *path_change)
        {
            ifindex = wan_link_create_empty_entry(dstpvc);
            if(0 > ifindex)
            {
                return -1;
            }
            dstpvc =   (ifindex / MAX_WAN_ENTRY_NUMBER + 1);
            dstentry = (ifindex % MAX_WAN_ENTRY_NUMBER + 1);
            /*sync info*/
            update_pvc_info(dstpvc, vlan, pbit);
            sync_wan_pvc_entry(dstpvc, dstentry, path);
            
            update_ifIdex_info(dstpvc, dstentry);
            /*del obj*/
            del_wan_link(path);
            /*update path*/
            *path_change = 1;
            memset(path, 0, path_size);
            snprintf(path, path_size, WAN_PVC_ENTRY_NODE, dstpvc, dstentry);
        }
    }

    return 0;
}

static int update_pvc_info(int dstpvc, char* vlan, char* pbit)
{
    char node[32];
    
    if ((NULL == vlan) || (NULL == pbit))
    {
        return -1;
    }

    memset(node, 0, sizeof(node));
    snprintf(node, sizeof(node), WAN_PVC_NODE, dstpvc);
    HAL_ITF_WAN_DEBUG("node = %s\n", node);

    if(0 > cfg_set_object_attr(node, "VLANID", vlan))
    {
        return -1;
    }
    
    if(0 > cfg_set_object_attr(node, "DOT1P", pbit))
    {
        return -1;
    }

    return 0;
}
#endif

/***************************************************************************/
/***************************************************************************/
int creat_wan_link(char* path, int size)
{
    int ifindex  = -1;
    int dstpvc   = -1;
    int dstentry = -1;

    if(NULL == path)
    {
        HAL_ITF_WAN_ERR("path can't be NULL.\n");
        return HAL_ITF_WAN_FAIL;
    }

    ifindex = creat_default_wan_link_pvc_entry();
    if(0 > ifindex)
    {
        HAL_ITF_WAN_ERR("ifindex = %d.\n", ifindex);
        return HAL_ITF_WAN_FAIL;
    }


    dstpvc =   (ifindex / MAX_WAN_ENTRY_NUMBER + 1);
    dstentry = (ifindex % MAX_WAN_ENTRY_NUMBER + 1);

    HAL_ITF_WAN_DEBUG("ifindex = %d, dstpvc = %d, dstentry, = %d.\n", ifindex, dstpvc, dstentry);

    memset(path, 0, size);
    snprintf(path, size, WAN_PVC_ENTRY_NODE, dstpvc, dstentry);

    set_default_wan_link(dstpvc, dstentry);

    update_ifIdex_info(dstpvc, dstentry);

    return HAL_ITF_WAN_SUCCESS;
}

int wan_link_effect(char* path)
{
    wan_link_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_UPDATE, path);

    return HAL_ITF_WAN_SUCCESS;
}

int set_wan_link_attr(char* path, int path_size, int type, char* value, char* change)
{
    char node[32];
    char attrname[32];
    char exist_tr069 = 0;
    int ret = HAL_ITF_WAN_FAIL;

    if((NULL == path) || (NULL == value) || (NULL == change))
    {
        HAL_ITF_WAN_ERR("pointer can't be NULL.\n");
        return HAL_ITF_WAN_FAIL;
    }
    
    HAL_ITF_WAN_DEBUG("path = %s, type = %d, value = %s.\n", path, type, value);
    memset(attrname, 0, sizeof(attrname));
    ret = convert_wan_entry_attrname(type, attrname, sizeof(attrname));
    if(0 > ret)
    {
        HAL_ITF_WAN_ERR("not support type = %d.\n", type);
        return HAL_ITF_WAN_FAIL;
    }

    HAL_ITF_WAN_DEBUG("type = %d, attrname = %s.\n", type, attrname);
    
#if !defined(TCSUPPORT_CMCC)
    if((E_WAN_IF_TYPE_VLAN_ID == type) || (E_WAN_IF_TYPE_DOT1P == type))
    {
        ret = wan_link_path_is_change(path, path_size, type, value, change);
        if(0 > ret)
        {
            HAL_ITF_WAN_ERR("call func faile.\n");
            return HAL_ITF_WAN_FAIL;
        }
        
        HAL_ITF_WAN_DEBUG("path = %s, change = %d.\n", path, *change);
    }
#endif

    if((E_WAN_IF_TYPE_SRV_LIST == type) && (0 == strcmp(value, WAN_SRV_TR069_STR)))
    {
        memset(node, 0, sizeof(node));
        exist_tr069 = is_exist_srv_tr069(node, sizeof(node));
        if((1 == exist_tr069) && (0 != strncmp(node, path, strlen(node))))
        {
            HAL_ITF_WAN_DEBUG("del tr069 srv wan link, node = %s.\n", node);
            del_wan_link(node);
        }
    }
    
   
    /*update pvc entry*/
    if(0 > cfg_set_object_attr(path, attrname, value))
    {
        return HAL_ITF_WAN_FAIL;
    }

    return HAL_ITF_WAN_SUCCESS;
}

int get_wan_link_attr(char* path, int type, char* value, int size)
{
    char attrname[32];
    int ret = -1;
	char waninfo[32];
	unsigned char is_need = 0;
	char buf[32];
    if((NULL == path) || (NULL == value))
    {
        return HAL_ITF_WAN_FAIL;
    }

	is_need = is_need_convert_path(type);
	if(is_need)
	{
		memset(waninfo, 0, sizeof(waninfo));
		ret = wan_entry_path_convert_waninfo_path(waninfo, sizeof(waninfo), path);
		if(0 != ret)
		{
			HAL_ITF_WAN_ERR("convert waninfo path faile.\n");
			return HAL_ITF_WAN_FAIL;
		}
		
		memset(attrname, 0, sizeof(attrname));
		ret = convert_waninfo_attrname(type, attrname, sizeof(attrname));
		if(0 > ret)
		{
			HAL_ITF_WAN_ERR("not support type = %d.\n", type);
			return HAL_ITF_WAN_FAIL;
		}
		snprintf(buf, sizeof(buf), waninfo);
	}
	else
	{
		memset(attrname, 0, sizeof(attrname));
		ret = convert_wan_entry_attrname(type, attrname, sizeof(attrname));
		if(0 > ret)
		{
			HAL_ITF_WAN_ERR("not support type = %d.\n", type);
			return HAL_ITF_WAN_FAIL;
		}
		snprintf(buf, sizeof(buf), path);
	}
	
	HAL_ITF_WAN_DEBUG("buf = %s, attrname = %s, type = %d.\n", buf, attrname, type);
	
    memset(value, 0, size);
    if(0 > cfg_get_object_attr(buf, attrname, value, size))
    {
        return HAL_ITF_WAN_FAIL;
    }

    HAL_ITF_WAN_DEBUG("attrname = %s, value = %s.\n", attrname, value);

    return HAL_ITF_WAN_SUCCESS;
}

int del_wan_link(char* path)
{
    char node[32];

    if(NULL == path)
    {
        return HAL_ITF_WAN_FAIL;
    }

    HAL_ITF_WAN_DEBUG("path = %s.\n", path);

    memset(node, 0, sizeof(node));
    if(0 > get_pvc_path(path, node, sizeof(node)))
    {
        HAL_ITF_WAN_ERR("parse path fail.\n");
        return HAL_ITF_WAN_FAIL;
    }

    cfg_delete_object(path);

    wan_link_send_evt(EVT_WAN_INTERNAL_TYPE, EVT_CFG_WAN_ENTRY_DELETE, path);

    if (cfg_query_object(node, "entry", NULL) <= 0)
    {
        cfg_delete_object(node);
    }

    return HAL_ITF_WAN_SUCCESS;
}

int get_wan_link_all_entry_path(wan_link_path_info_t* info)
{
    int i = 0;
    int j = 0;
    char buf[32];

    if(NULL == info)
    {
        HAL_ITF_WAN_ERR("pointer can't be NULL.\n");
        return HAL_ITF_WAN_FAIL;
    }
    
    for (i = 0; i < MAX_WAN_PVC_NUMBER; i++)
    {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), WAN_PVC_NODE,(i+1));
        if(cfg_query_object(buf, NULL, NULL) <= 0)
        {
            continue;
        }
        for(j = 0; j < MAX_WAN_ENTRY_NUMBER; j++)
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), WAN_PVC_ENTRY_NODE,(i+1), (j+1));
            if(cfg_query_object(buf, NULL, NULL) <= 0)
            {
                continue;
            }
            
            if (MAX_WAN_INTF_NUMBER > info->num)
            {
                snprintf(info->path[info->num], sizeof(info->path[info->num]), buf);
                info->num += 1;
            }
        }
    }
    
    HAL_ITF_WAN_DEBUG("info->num = %d.\n", info->num);
    for(i = 0; i < info->num; i++)
    {
        HAL_ITF_WAN_DEBUG("i = %d, info->path[%d] = %s.\n", i, i, info->path[i]);
    }

    return HAL_ITF_WAN_SUCCESS;
}

int get_wan_link_index(char* path, int * pvcIdx, int * entryIdx)
{
    if(NULL == path)
    {
        HAL_ITF_WAN_ERR("path can't be NULL.\n");
        return HAL_ITF_WAN_FAIL;
    }
    if (sscanf(path,WAN_PVC_ENTRY_NODE,pvcIdx,entryIdx) < 2)
    {
        HAL_ITF_WAN_ERR("path format error.\n");
        return HAL_ITF_WAN_FAIL;
    }
    return HAL_ITF_WAN_SUCCESS;
}


