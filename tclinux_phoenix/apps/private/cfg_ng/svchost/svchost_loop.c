
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


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <pthread.h>
#include <signal.h>

#include <libubus.h>
#include "svchost_internel.h"

#define svchost_out(lev,format,args...)  if (lev <= svchost_debug) fprintf(stderr,format,##args)


typedef  struct host_service_intf* (*SERVICE_ENTRY)(char*);


int svchost_debug = 0;


static const char* meth_name[] = {"__start","__stop","__status","__unload"};
static int svchost_nthread = 0;


int ubus_loop_poll(struct ubus_context *ctx, int timeout);

#if 0
#define SVCHOST_STATE "/tmp/svchost_state"
int is_svchost_running(void)
{
	FILE* fp;

	if ((fp = fopen(SVCHOST_STATE,"r")) != NULL)
	{
		fclose(fp);
		return 1;
	}

	return 0;
}

int svchost_done(void)
{	
	unlink(SVCHOST_STATE);
	return 0;
}
#endif


static int svchost_dump_svc(struct host_service* svc)
{
	int i;
	if (!svc)
		return 0;

	svchost_out(0,"Dump SVC: ID = %d \n",svc->intf->id);
	svchost_out(0,"SVC Flag: %x \n",svc->flag);
	if (svc->path)
		svchost_out(0,"Lib Path: %s \n",svc->path);

	if (svc->entry)
		svchost_out(0,"SVC Entry: %s \n",svc->entry);

	if (svc->param)
		svchost_out(0,"SVC Param: %s \n",svc->param);		

	svchost_out(0,"SVC Handle: %x \n",(int)svc->handle);
	svchost_out(0,"SVC Method: %d \n",svc->meth_cnt);
	for(i=0; i < svc->meth_cnt; i++)
	{
		svchost_out(0,"SVC Method[%d]: %s \n",i,svc->meth[i].name);
	}
	
	svchost_out(0,"SVC Event: %d \n",svc->evt_cnt);
	for(i=0; i < svc->evt_cnt; i++)
	{
		svchost_out(0,"SVC Event[%d]: %s \n",i,svc->event[i].name);
	}
	svchost_out(0,"SVC Inner Method: %d \n",svc->inner_cnt);
	for(i=0; i < svc->inner_cnt; i++)
	{
		svchost_out(0,"SVC Inner Meth[%d]: %s \n",i,svc->inner_meth[i].name);
	}	
	return 0;

}

static pthread_mutex_t svchost_mutex = PTHREAD_MUTEX_INITIALIZER;

static int svchost_thread_lock(void)
{
	pthread_mutex_lock(&svchost_mutex);
	
	return 0;
}

static int svchost_thread_unlock(void)
{
	pthread_mutex_unlock(&svchost_mutex);
	
	return 0;
}

static int svchost_init_conn(struct host_run_env *env)
{

	if ((env->ctx = ubus_connect(NULL)) == NULL)
	{
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		return -1;
	}
	ubus_add_uloop(env->ctx);
	
	return 0;
}

static int svchost_deinit_conn(struct host_run_env *env)
{
	if (env->ctx)
	{
		ubus_free(env->ctx);
		env->ctx = NULL;
	}
	
	return 0;
}

static int svchost_check_svc(struct host_service* svc)
{
	struct host_service* tmp = svc->env->svc;

	struct host_service_intf *intf = svc->intf;
	if (intf == NULL){
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		return -1;
	}

	if (!intf->start || !intf->stop || !intf->id){
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		return -1;
	}

	while(tmp)
	{
		
		if (tmp != svc && tmp->intf != NULL && tmp->intf->id == intf->id){
			printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
			return -1;
		}
		tmp = tmp->next;
	}

	return 0;
}




static int svchost_load_lib(struct host_service* svc)
{
	 struct host_service_intf* (*entry)(char*) = NULL;


	svchost_out(2, "Svc Host:Load Svc at [%s]:[%s] ......",svc->path,svc->entry);

	if ((svc->handle = dlopen(svc->path,RTLD_NOW)) == NULL)
	{
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		goto error;
	}
 
	if ((entry = (SERVICE_ENTRY) dlsym(svc->handle,svc->entry)) == NULL)
	{
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		goto error;
	}

	svc->intf = entry(svc->param);

	if (!svc->intf)
	{
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		goto error;
	}

	svc->flag = svc->intf->flag;
	
	svchost_out(2, " Ok! \n");
	return 0;
	
error:
	svchost_out(2, "Fail! \n");
	return -1;
}

static int svchost_meth_handle_hook(struct ubus_context *ctx, struct ubus_object *obj,struct ubus_request_data *req,const char *method, struct blob_attr *msg)
{
	struct host_service_call call;
	struct host_service* svc;
	struct host_service_pbuf buf;
	int i,ret = UBUS_STATUS_METHOD_NOT_FOUND,len;
	
	svchost_out(3,"svchost_meth_handle_hook: enter[%s] \n",method);
	
	svc = container_of(obj,struct host_service,inner_obj);
	call.ctx = ctx;
	call.obj = obj;
	call.req = req;
	call.meth = method;
	
	memset(&buf,0,sizeof(struct host_service_pbuf));

	blob_buf_init(&buf.buf,0);
	blob_put_raw(&buf.buf,blob_data(msg),blob_len(msg));
//	buf.buf.head = buf.buf.buf = msg;
	
	for(i=0; i < svc->meth_cnt; i++)
	{
		if (!strcmp(svc->meth[i].name,method))
		{
			ret = svc->meth[i].method(&call,&buf);
			goto exit;
		}
	}
	
	for(i=0; i < svc->evt_cnt; i++)
	{
		len = strlen(svc->event[i].name);
		if (len == 0 || strcmp(svc->event[i].name,method) == 0)
		{
			ret = svc->event[i].event(&call,&buf);
			break;
		}
	}
	
exit:	
	svchost_out(3,"svchost_meth_handle_hook: leave[%s] \n",method);
	blob_buf_free(&buf.buf);
	return ret;
}




static int svchost_meth_intf_hook(struct ubus_context *ctx, struct ubus_object *obj,struct ubus_request_data *req,const char *method, struct blob_attr *msg)
{
	struct host_service* svc;
	int ret = 0;
	
	svc = container_of(obj,struct host_service,inner_obj);
	
	svchost_out(3,"svchost_meth_intf_hook: %s \n",method);
		
	if (!strncmp(method,"__start",7) && svc->intf->start) {
		ret = svc->intf->start(svc);
	}else if (!strncmp(method,"__stop",6) && svc->intf->stop){
		ret = svc->intf->stop(svc);
	}else if (!strncmp(method,"__status",7)){
		if (svc->intf->status)
			ret = svc->intf->status();
		else
			ret = svchost_dump_svc(svc);
	}else if (!strncmp(method,"__unload",8)){
		svc->flag  |= SVCHOST_FLAG_REQUNLOAD;
	}else{
		ret = UBUS_STATUS_METHOD_NOT_FOUND;
	}
	
	return ret;
}


static int svchost_reg_svc(struct host_service* svc)
{
	int i;
	
	svc->inner_cnt = sizeof(meth_name) / sizeof(char*);
	
	for(i=0; i<svc->inner_cnt;i++)
	{
		svc->inner_meth[i].name = meth_name[i];
		svc->inner_meth[i].handler = svchost_meth_intf_hook;
	}
	svc->inner_obj.id = svc->intf->id;
	svc->inner_obj.methods = svc->inner_meth;
	svc->inner_obj.n_methods = svc->inner_cnt;

	for ( i = svc->inner_cnt ; i< HOST_SERVICE_METHOD_MAX ; i++)
		svc->inner_meth[i].handler = svchost_meth_handle_hook;

	if (ubus_add_object(svc->env->ctx,&svc->inner_obj) != 0)
	{
		printf("[%s:%d]Error %s\n",__FUNCTION__,__LINE__,dlerror());
		return -1;
	}

	svc->flag |=  SVCHOST_FLAG_REGISTERED;
	
	svc->env->nsvc++;
	
	if (svc->intf->init != NULL)
		svc->intf->init(svc);
	
	return 0;
}


static int svchost_unreg_svc(struct host_service* svc)
{
	int i;
	svc->inner_cnt = 0;
	
	for (i = 2; i< HOST_SERVICE_METHOD_MAX ; i++)
	{
		svc->inner_meth[i].name = NULL;
		svc->inner_meth[i].handler = NULL;
	}
	
	ubus_remove_object(svc->env->ctx, &svc->inner_obj);

	return 0;
}

static int svchost_free_svc(struct host_service* svc)
{
	struct host_service* tmp = svc->env->svc;

	if (tmp == svc)
	{
		svc->env->svc = svc->next;
		free(svc);
		return 1;
	}
	
	while(tmp->next != NULL && tmp->next != svc)
		tmp = tmp->next;

	if (tmp->next == svc)
	{
		tmp->next = svc->next;
		free(svc);
		return 1;
	}
	
	return 0;
}



static int svchost_auto_start(struct host_run_env *env)
{
	struct host_service* svc = env->svc;

	while(svc)
	{
		if (svc->flag & 1)
			(*svc->intf->start)(svc);
		svc = svc->next;
	}
	return 0;
}

static int svchost_init_lib(struct host_run_env *env)
{
	struct host_service* svc = env->svc;

	while(svc)
	{
		if (svchost_load_lib(svc) < 0)
			return -1;
		
		if (svchost_check_svc(svc) < 0)
			return -1;

		if (svchost_reg_svc(svc) < 0)
			return -1;
		
		svc = svc->next;
	}
	
	svchost_auto_start(env);
	
	return 0;
}


static int svchost_unload_svc(struct host_service* svc)
{
	svchost_out(2, "svchost_unload_svc: [%s] \n",svc->path);

	if (svc->intf && svc->intf->stop)
		svc->intf->stop(svc);

	if (svc->flag & SVCHOST_FLAG_REGISTERED)
		svchost_unreg_svc(svc);
	
	if (svc->handle)
		dlclose(svc->handle);
	
	svc->handle = 0;
	
	svc->env->nsvc--;
	
	svchost_free_svc(svc);
	
	return 0;
}

static int svchost_deinit_lib(struct host_run_env *env)
{
	struct host_service* svc = env->svc;

	while(svc)
	{
		svchost_unload_svc(svc);
		svc = svc->next;
	}
	
	return 0;
}


static int svchost_run_init(struct host_run_env *env)
{
	struct host_service* tmp = env->svc;

	while(tmp)
	{
		tmp->env =  env;
		tmp = tmp->next;
	}

	if (uloop_init() < 0)
	{
		svchost_out(1,"svchost_run_init: uloop_init fail \n");
		return -1;
	}

	while(svchost_init_conn(env) < 0){
		svchost_out(1,"svchost_run_init: svchost_init_conn fail \n");	
		usleep(1000000);
	}
	
	if (svchost_init_lib(env) < 0)
	{
		svchost_out(1,"svchost_run_init: svchost_init_lib fail \n");
		return -1;
	}
	
	return 0;
}

static int svchost_run_deinit(struct host_run_env * env)
{
	svchost_deinit_lib(env);
	svchost_deinit_conn(env);
	uloop_done();
	return 0;
}


static int svchost_check_unload(struct host_run_env * env)
{
	struct host_service* svc = env->svc;

	while(svc)
	{
		if (svc->flag & SVCHOST_FLAG_REQUNLOAD)
			svchost_unload_svc(svc);

		svc = svc->next;
	}
	
	return 0;
}



static int svchost_run_loop(struct host_run_env *env)
{
	struct ubus_context *ctx = env->ctx;

	while( !uloop_cancelled  &&  env->nsvc > 0)
	{
		ubus_loop_poll(ctx, 1000);
		svchost_check_unload(env);
	}

	return 0;
}



static void* svchost_run(void * ptr)
{
	struct host_run_env host_env;
	int ret;
	
	memset(&host_env,0,sizeof(struct host_run_env));
	
	host_env.svc = (struct host_service*)ptr;

	ret = svchost_run_init(&host_env);

	/*svchost_done();*/

	svchost_nthread++;
	
	svchost_thread_unlock();
	
	if (ret == 0)
		svchost_run_loop(&host_env);
	
	svchost_run_deinit(&host_env);

	svchost_thread_lock();
	
	svchost_nthread--;
	
	svchost_thread_unlock();
	
	return 0;
}


static int svchost_thread( struct host_service* svc)
{
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	
	if (pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) < 0)
	{
		svchost_out(1,"svchost_thread: set attr state fail \n");
	
	return -1;	

	}
	
	if (pthread_create(&tid,&attr,svchost_run,svc) < 0)
	{
		svchost_out(1,"svchost_thread: create thread fail \n");
	
	return -1;
	}
	
	return 0;
}

int svchost_loop(struct host_service* svc)
{
	struct host_service* tmp;

	while (svc)
	{
		tmp = svc;
		svc = svc->next;
		tmp->next = NULL;	
		svchost_thread_lock();
		if (svchost_thread(tmp) <0)
			goto error;
	}
	
	return 0;
	
error:
	svchost_thread_unlock();
	return -1;	
}


int svchost_mian(int mode , struct host_service* svc)
{
#if 0
	while(is_svchost_running()){
		sleep(1);
	}
#endif
	if (mode == 0)
	{
		svchost_thread_lock();
		svchost_run(svc);
		return 0;
	}
	
	if(svchost_loop(svc) < 0)
		return -1;
	
	while(svchost_nthread > 0)
		sleep(2);

	return 0;
}

