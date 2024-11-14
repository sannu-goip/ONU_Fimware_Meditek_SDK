
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

#ifndef __SVCHOST_API_H
#define __SVCHOST_API_H

#include <libubus.h>
#include "svchost_id.h"
#include "svchost_evt.h"

//#define HOST_SERVICE_DATA_BUFF 128
#define SVCHOST_MAX_PARAMETER_NUM	16

#define  host_service_parameters blob_attr

struct host_service;

typedef struct host_service_intf {
	unsigned int id;
	unsigned int flag;
	int (*start)(struct host_service* svc);
	int (*stop)(struct host_service* svc);
	int (*status)(void);
	int (*init)(struct host_service* svc);
}host_service_intf_t;


typedef struct host_service_call{
	struct ubus_context* ctx;
	struct ubus_object* obj;
	struct ubus_request_data* req;
	const char* meth;
}host_service_call_t;

typedef struct host_service_pbuf{
	int nparam;
	struct blob_buf buf;
}host_service_pbuf_t;

typedef struct host_service_method {
	const char* name;
	int (*method)(struct host_service_call* env,struct host_service_pbuf* pbuf);
}host_service_method_t;

typedef struct host_service_event {
	char* name;
	int (*event)(struct host_service_call* env, struct host_service_pbuf* pbuf);
}host_service_event_t;


extern int svchost_debug;

//register service method, used by svc server
int svchost_reg_method(struct host_service* env,host_service_method_t* meth_ptr, int n);

//register service method, used by svc server
int svchost_reg_event(struct host_service* env,host_service_event_t* event_ptr, int n);

int svchost_init_pbuf(struct host_service_pbuf* pbuf); 

int svchost_free_pbuf(struct host_service_pbuf* pbuf); 

int svchost_put_param(struct host_service_pbuf* pbuf,unsigned int len, void* send);

//int svchost_get_param(struct host_service_pbuf* pbuf, struct host_service_parameters** param);
void* svchost_get_param(struct host_service_pbuf* pbuf, int idx ,unsigned int* len);

//register service method, used by svc server
int svchost_send_reply(struct host_service_call* env, struct host_service_pbuf* param);

//register service method, used by svc server
int svchost_send_event(char* name, struct host_service_pbuf* param);

//register service method, used by svc client
int svchost_invoke(int id, char* method,  struct host_service_pbuf* send,struct host_service_pbuf* recv);

int svchost_get_event_id(struct host_service_pbuf* pbuf);

void* svchost_get_event_val(host_service_pbuf_t* pbuf);


#endif
