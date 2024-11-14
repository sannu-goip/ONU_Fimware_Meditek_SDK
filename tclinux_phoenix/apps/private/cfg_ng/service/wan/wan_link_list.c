#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wan_link_list.h"

static wan_entry_obj_t* svc_wan_entry_list = NULL;

wan_entry_obj_t* svc_wan_get_obj_list(void)
{
    return svc_wan_entry_list;
}

void svc_wan_init_obj_list(void)
{
    svc_wan_entry_list = NULL;
}


int svc_wan_free_object(wan_entry_obj_t* obj)
{
    if (obj)
    {
        free(obj);
	obj = NULL;
    }

    return 0;
}

static wan_entry_obj_t* svc_wan_alloc_object(void)
{
    wan_entry_obj_t* obj;

    if ((obj = (wan_entry_obj_t* )malloc(sizeof(wan_entry_obj_t))) == NULL)
    {
        return NULL;
    }

    memset(obj,0,sizeof(wan_entry_obj_t));
	obj->dhcp_release = 1;

    return obj;
}

static int svc_wan_add_to_list(wan_entry_obj_t* obj)
{
    //insert tail
    wan_entry_obj_t* head = NULL;
    wan_entry_obj_t* p1 = NULL;

    head = p1 = svc_wan_entry_list;
    //head
    if(NULL == head)
    {
        obj->next = NULL;
        svc_wan_entry_list = obj;
        return 0;
    }

    //list 
    while(NULL != p1->next)
    {
        p1 = p1->next;
    }

    p1->next = obj;
    obj->next = NULL;

    return 0;
}

int svc_wan_del_from_list(wan_entry_obj_t* obj)
{
    wan_entry_obj_t* head = NULL;
    wan_entry_obj_t* p1 = NULL;
    wan_entry_obj_t* p2 = NULL;
    head = svc_wan_entry_list;
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
            svc_wan_entry_list = svc_wan_entry_list->next;
            svc_wan_free_object(p1);
        }
        /*tail*/
        else if(NULL == p1->next)
        {
            p2->next = NULL;
            svc_wan_free_object(p1);
        }
        else
        {
            p2->next = p1->next;
            svc_wan_free_object(p1);
        }
    }
    else
    {
        SVC_WAN_ERROR_INFO("not found element.\n");
    }

    return 0;
}

wan_entry_obj_t*  svc_wan_find_object(char* path)
{
    wan_entry_obj_t* obj;

    for(obj = svc_wan_entry_list; obj; obj = obj->next)
    {
        if (strcmp(obj->path,path) == 0)
        {
            break;
        }
    }
    
    return obj;
}

wan_entry_obj_t*  svc_wan_find_object_by_dev(char* dev)
{
    wan_entry_obj_t* obj = NULL;
    char name[MAX_WAN_DEV_NAME_LEN];
    wan_entry_cfg_t* cfg ;
	int wanIf = -1;

	wanIf = get_wanindex_by_name(dev);
	if(wanIf < 0)
		return NULL;

    for(obj = svc_wan_entry_list; obj; obj = obj->next)
        {
		if(wanIf == obj->idx){
            break;
        }
    }

    return obj;
}

wan_entry_obj_t* svc_wan_create_object(char* path)
{
    wan_entry_obj_t* obj;

    if ((obj = svc_wan_alloc_object()) == NULL)
    {
        return NULL;
    }

    svc_wan_add_to_list(obj);

    return obj;
}
/***********************************************************************/

