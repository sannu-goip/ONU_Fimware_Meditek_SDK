
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
#include <pthread.h>
#include <libubus.h>
#include "svchost_api.h"
#include "svchost_internel.h"

static pthread_mutex_t svchost_api_mutex = PTHREAD_MUTEX_INITIALIZER;

int svchost_reg_method(struct host_service* svc,host_service_method_t* ptr, int n)
{
	int i,k,ret = 0;
	host_service_method_t* tmp = ptr;
	
	if (svc->meth  || svc->meth_cnt > 0)
		return n;

	if ((svc->inner_cnt + n) >= HOST_SERVICE_METHOD_MAX)
		return -1;

	k = svc->inner_cnt;
	for( i = 0; i < n ; tmp++,i++)
	{
		if (! tmp->name || !tmp->method)
			continue;
		
		if (strlen(tmp->name) == 0)
			continue;

		svc->inner_meth[k].name = tmp->name;
		k++;
	}
	
	ret = k - svc->inner_cnt;
	if (ret != n)
		return 0;
	
	svc->inner_cnt = svc->inner_obj.n_methods = k;
	svc->meth = ptr;
	svc->meth_cnt = n;
	
	return ret;
		
}

int svchost_reg_event(struct host_service* svc,host_service_event_t* ptr, int n)
{
	int i,k, ret = 0, len;
	host_service_event_t* tmp = ptr;
	
	if (svc->event  || svc->evt_cnt > 0)
	{
		printf("svchost_reg_event: exist !\n");
		return n;
	}
	
	if ((svc->inner_cnt + n) >= HOST_SERVICE_METHOD_MAX)
	{
		printf("svchost_reg_event: too much [%d] [%d]  fail!\n",svc->inner_cnt,n);
		return -1;
	}
	
	k = svc->inner_cnt;
	for( i = 0; i < n ; tmp++,i++)
	{
		if (! tmp->name || ! tmp->event)
			continue;

		len = strlen(tmp->name);
		
		if (len == 0)
			continue;

		if((ret = ubus_register_event_handler(svc->env->ctx, (struct ubus_event_handler*)&svc->inner_obj, tmp->name)) != 0)
		{
			printf("svchost_reg_event: [%s] fail %d !\n",tmp->name,ret);
			continue;
		}
			
		svc->inner_meth[k].name = tmp->name;

		if (tmp->name[len-1] == '*')
			tmp->name[len-1] = 0;
			
		k++;
	}

	ret = k - svc->inner_cnt;
	
	if (ret != n)
		return 0;
	
	svc->inner_cnt = svc->inner_obj.n_methods = k;
	svc->event = ptr;
	svc->evt_cnt = n;

	return ret;

}

int svchost_init_pbuf(struct host_service_pbuf* pbuf)
{
	memset(pbuf,0,sizeof(struct host_service_pbuf));
	blob_buf_init(&pbuf->buf,0);
	return 0;
}

int svchost_free_pbuf(struct host_service_pbuf* pbuf)
{
	pbuf->nparam = 0;
	blob_buf_free(&pbuf->buf);
	return 0;
}


int svchost_put_param(struct host_service_pbuf* pbuf,unsigned int len, void* send)
{
	if (! blob_put(&pbuf->buf,++pbuf->nparam,send,len))
	{
		fprintf(stderr,"svchost_put_param: blob put %d error \n",len);
		return -1;
	}
	return  0;
}


void* svchost_get_param(struct host_service_pbuf* pbuf, int idx ,unsigned int* len)
{
	struct blob_attr * param[SVCHOST_MAX_PARAMETER_NUM];

	blob_parse(pbuf->buf.head,param,NULL,SVCHOST_MAX_PARAMETER_NUM);

	if (param[idx])
	{
		if (len)
			*len = blob_len(param[idx]);
		return blob_data(param[idx]);
	}
	return NULL;
}


int svchost_get_event_id(struct host_service_pbuf* pbuf)
{
	unsigned int* sub_evt_id = NULL;
	unsigned int len = 0;
	
	if ((sub_evt_id = (unsigned int* )svchost_get_param(pbuf,1,&len)) == NULL)
	{
		printf("%s: get param fail [%d] \n",__FUNCTION__,len);
		return -1;
	}

	return *sub_evt_id;
}

void* svchost_get_event_val(struct host_service_pbuf* pbuf)
{
	void *tmp = NULL;
	unsigned int len = 0;
	
	if ((tmp = svchost_get_param(pbuf,2,&len)) == NULL)
	{
		printf("%s: get param fail [%d] \n",__FUNCTION__,len);
		return NULL;
	}
	
	return tmp;
}


int svchost_send_reply(struct host_service_call* pcall,struct host_service_pbuf* psend)
{
	if (ubus_send_reply(pcall->ctx,pcall->req,psend->buf.head))
	{
		fprintf(stderr,"svchost_send_reply: error \n");
		return -1;
	}
	return 0;
}

static int svchost_api_lock(void)
{
	pthread_mutex_lock(&svchost_api_mutex);

	return 0;
}

static int svchost_api_unlock(void)
{
	pthread_mutex_unlock(&svchost_api_mutex);

	return 0;
}

int svchost_send_event(char* name,struct host_service_pbuf* pbuf)
{
	struct ubus_context* ctx;
	int ret = 0;

	if (!(ctx = ubus_connect(NULL)))
	{
		fprintf(stderr,"svchost_invoke: connect error \n");
		ret = -1;
		goto exit;
	}
	svchost_api_lock();	
	if (ubus_send_event(ctx,name,pbuf->buf.head))
	{
		fprintf(stderr,"svchost_invoke: send event error \n");
		ret = -1;
	}
	svchost_api_unlock();
exit:
	if(ctx)
		ubus_free(ctx);
	
	return ret;
}


static void svchost_invoke_data_handle(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct host_service_pbuf* precv = req->priv;
	
	blob_put_raw(&precv->buf,blob_data(msg),blob_len(msg));
	return ;
}

int svchost_invoke(int id, char* method, struct host_service_pbuf* psend,struct host_service_pbuf* precv)
{
	int ret = 0;
	struct ubus_context* ctx;

	if (!(ctx = ubus_connect(NULL)))
	{
		fprintf(stderr,"svchost_invoke: connect error \n");
		ret =  -1;
		goto exit;
	}
	svchost_api_lock();	
	if (ubus_invoke(ctx,id,method,psend->buf.head,svchost_invoke_data_handle,precv,0))
	{
		fprintf(stderr,"svchost_invoke: invoke %u:%s error \n",id,method);
		ret =  -1;
	}
	svchost_api_unlock();
exit:
	ubus_free(ctx);

	return ret;
}
