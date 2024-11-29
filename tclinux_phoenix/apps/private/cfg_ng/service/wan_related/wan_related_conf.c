#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cfg_api.h>
#include "cfg_msg.h"
#include "wan_related_conf.h"
#include "utility.h"

static wan_related_conf_obj* svc_wan_related_conf_list = NULL;

wan_related_conf_obj* svc_wan_related_get_obj_list(void)
{
    return svc_wan_related_conf_list;
}

void svc_wan_related_init_obj_list(void)
{
    svc_wan_related_conf_list = NULL;
}


int svc_wan_related_free_object(wan_related_conf_obj* obj)
{
    if (obj)
    {
        free(obj);
    }

    return 0;
}

static wan_related_conf_obj* svc_wan_related_malloc_object(void)
{
    wan_related_conf_obj* obj;

    if ((obj = (wan_related_conf_obj* )malloc(sizeof(wan_related_conf_obj))) == NULL)
    {
        return NULL;
    }

    memset(obj,0,sizeof(wan_related_conf_obj));

    return obj;
}

static int svc_wan_related_add_to_list(wan_related_conf_obj* obj)
{
    /*insert tail*/
    wan_related_conf_obj* head = NULL;
    wan_related_conf_obj* p1 = NULL;

    head = p1 = svc_wan_related_conf_list;
    /*head*/
    if(NULL == head)
    {
        obj->next = NULL;
        svc_wan_related_conf_list = obj;
        return 0;
    }

    /*list */
    while(NULL != p1->next)
    {
        p1 = p1->next;
    }

    p1->next = obj;
    obj->next = NULL;

    return 0;
}

int svc_wan_related_del_from_list(wan_related_conf_obj* obj)
{
    wan_related_conf_obj* head = NULL;
    wan_related_conf_obj* p1 = NULL;
    wan_related_conf_obj* p2 = NULL;
    head = svc_wan_related_conf_list;
    if(NULL == head)
    {
        return 0;
    }

    p1 = head;
    while ((NULL != p1) && (0 != strcmp(p1->path, obj->path)))
    {
        p2 = p1;
        p1 = p1->next;
    }

    if(p1 != NULL && 0 == strcmp(p1->path, obj->path))
    {
        /*head*/
        if(head == p1)
        {
            svc_wan_related_conf_list = svc_wan_related_conf_list->next;
            svc_wan_related_free_object(p1);
        }
        /*tail*/
        else if(NULL == p1->next)
        {
            p2->next = NULL;
            svc_wan_related_free_object(p1);
        }
        else
        {
            p2->next = p1->next;
            svc_wan_related_free_object(p1);
        }
    }
    else
    {
        tcdbg_printf("not found element.\n");
    }

    return 0;
}

wan_related_conf_obj*  svc_wan_related_find_object(char* path)
{
    wan_related_conf_obj* obj;

    for(obj = svc_wan_related_conf_list; obj; obj = obj->next)
    {
        if (strcmp(obj->path,path) == 0)
        {
            break;
        }
    }
    
    return obj;
}

wan_related_conf_obj*  svc_wan_related_find_object_by_dev(char* dev)
{
    wan_related_conf_obj* obj = NULL;
    char name[MAX_WAN_RELATED_DEV_NAME_LEN];
    wan_entry_cfg* cfg ;

    for(obj = svc_wan_related_conf_list; obj; obj = obj->next)
    {
        cfg = &obj->cfg;
        if (cfg->linkmode == SVC_WAN_RELATED_ATTR_LINKMODE_PPP)
        {
            snprintf(name, sizeof(name), "ppp%d",obj->idx);
        }
        else
        {
            strcpy(name,obj->dev);
        }

        if (strcmp(name,dev) == 0)
        {
            break;
        }
    }

    return obj;
}

wan_related_conf_obj* svc_wan_related_create_object(char* path)
{
    wan_related_conf_obj* obj;

    if ((obj = svc_wan_related_malloc_object()) == NULL)
    {
        return NULL;
    }

    svc_wan_related_add_to_list(obj);

    return obj;
}


int svc_wan_related_load_cfg(char* path, wan_entry_cfg* cfg)
{
    char tmp[64] = {0};

    memset(cfg, 0, sizeof(wan_entry_cfg));

    if(cfg_get_object_attr(path, WAN_ACTIVE, tmp, sizeof(tmp)) <= 0)
    {
    	return -1;
	}
    if(strcmp(tmp,"Yes") == 0)
    {
        cfg->active = SVC_WAN_RELATED_ATTR_ACTIVE_ENABLE;
    }
	else
	{
		cfg->active = SVC_WAN_RELATED_ATTR_ACTIVE_DISABLE;
	}

	cfg_get_object_attr(path, WAN_LINKMODE, tmp, sizeof(tmp));
    if(strcmp(tmp,"linkIP") == 0)
    {
        cfg->linkmode = SVC_WAN_RELATED_ATTR_LINKMODE_IP;
    }
    else if(strcmp(tmp,"linkPPP") == 0)
    {
        cfg->linkmode = SVC_WAN_RELATED_ATTR_LINKMODE_PPP;
    }

	if (cfg_get_object_attr(path, WAN_ISP, tmp, sizeof(tmp)) > 0)
    {
        cfg->isp = atoi(tmp);
    }
    else
    {
        cfg->isp = -1;
    }

    return 0;
}


/***********************************************************************/



