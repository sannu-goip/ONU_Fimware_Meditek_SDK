/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cfg_msg.h>
#include <utility.h>
#include "wan_related_vxlan_link_list.h"

static vxlan_entry_obj_t* svc_wan_related_vxlan_entry_list = NULL;

vxlan_entry_obj_t* svc_wan_related_get_vxlan_obj_list(void)
{
	return svc_wan_related_vxlan_entry_list;
}

void svc_wan_related_init_vxlan_obj_list(void)
{
	svc_wan_related_vxlan_entry_list = NULL;
}


int svc_wan_related_free_vxlan_obj(vxlan_entry_obj_t* obj)
{
	if ( obj )
	{
		free(obj);
		obj = NULL;
	}

	return 0;
}

static vxlan_entry_obj_t* svc_wan_related_alloc_vxlan_obj(void)
{
	vxlan_entry_obj_t* obj;

	if ( NULL == (obj = (vxlan_entry_obj_t* )malloc(sizeof(vxlan_entry_obj_t))) )
	{
		return NULL;
	}

	memset(obj, 0, sizeof(vxlan_entry_obj_t));

	return obj;
}

static int svc_wan_related_add_to_vxlan_list(vxlan_entry_obj_t* obj)
{
	/* insert tail */
	vxlan_entry_obj_t* head = NULL;
	vxlan_entry_obj_t* p1 = NULL;

	head = p1 = svc_wan_related_vxlan_entry_list;
	/* head */
	if ( NULL == head )
	{
		obj->next = NULL;
		svc_wan_related_vxlan_entry_list = obj;
		return 0;
	}

	/* list */
	while ( NULL != p1->next )
	{
		p1 = p1->next;
	}

	p1->next = obj;
	obj->next = NULL;

	return 0;
}


int svc_wan_related_del_vxlan_obj_from_list(vxlan_entry_obj_t* obj)
{
	vxlan_entry_obj_t* head = NULL;
	vxlan_entry_obj_t* p1 = NULL;
	vxlan_entry_obj_t* p2 = NULL;
	head = svc_wan_related_vxlan_entry_list;
	if ( NULL == head )
	{
		return 0;
	}

	p1 = head;
	while ( ( NULL != p1 ) && ( 0 != strcmp(p1->path, obj->path) ) )
	{
		p2 = p1;
		p1 = p1->next;
	}

	if ( p1 != NULL && 0 == strcmp(p1->path, obj->path) )
	{
		/*head*/
		if ( head == p1 )
		{
			svc_wan_related_vxlan_entry_list = svc_wan_related_vxlan_entry_list->next;
			svc_wan_related_free_vxlan_obj(p1);
		}
		/*tail*/
		else if ( NULL == p1->next )
		{
			p2->next = NULL;
			svc_wan_related_free_vxlan_obj(p1);
		}
		else
		{
			p2->next = p1->next;
			svc_wan_related_free_vxlan_obj(p1);
		}
	}
	else
	{
		tcdbg_printf("not found element.\n");
	}

	return 0;
}

vxlan_entry_obj_t* svc_wan_related_find_vxlan_obj(char* path)
{
	vxlan_entry_obj_t* obj;

	for ( obj = svc_wan_related_vxlan_entry_list; obj; obj = obj->next )
	{
		if ( 0 == strcmp(obj->path, path) )
		{
			break;
		}
	}

	return obj;
}

vxlan_entry_obj_t* svc_wan_related_find_vxlan_obj_by_dev(char* dev)
{
	vxlan_entry_obj_t* obj = NULL;

	for ( obj = svc_wan_related_vxlan_entry_list; obj; obj = obj->next )
	{
		if ( 0 == strcmp(dev, obj->vxlan_dev) )
		{
			break;
		}
	}

	return obj;
}

vxlan_entry_obj_t* svc_wan_related_find_vxlan_obj_by_bind_wanif(char* wanif)
{
	vxlan_entry_obj_t* obj = NULL;

	for ( obj = svc_wan_related_vxlan_entry_list; obj; obj = obj->next )
	{
		if ( 0 == strcmp(wanif, obj->cfg.bind_wanitf) )
		{
			break;
		}
	}

	return obj;
}


vxlan_entry_obj_t* svc_wan_related_create_vxlan_obj(char* path)
{
	vxlan_entry_obj_t* obj;

	if ( NULL == ( obj = svc_wan_related_alloc_vxlan_obj() ) )
	{
		return NULL;
	}

	svc_wan_related_add_to_vxlan_list(obj);

	return obj;
}


