/*************************
	author:  jw.ren
	date:    20190115
*************************/
#include <svchost_api.h> 
#include <cfg_api.h>
#include <libapi_lib_wifimgr.h>
#include "uloop_timer.h"
#include "mesh_trigger.h"
#include "utility.h"

#define MAP_TRIGGER_FLAG	"MeshTriggerFlag"
static void mesh_trigger_timeout_cb(struct ecnt_uloop_timeout *t)
{
	cc_send_evt(EVT_CC_EXTERNAL_TYPE, EVT_CC_TRIGGER_MESH_REINIT, "root.mesh");
}

static struct ecnt_uloop_timeout mesh_trigger_timeout = 
{
	.cb = mesh_trigger_timeout_cb,
};

void mesh_trigger(void)
{
	char buf[16];
	int triggerIdx = 0;
	int time = 0;

	memset(buf, 0, sizeof(buf));
	cfg_get_object_attr(MESH_COMMON_NODE, MAP_TRIGGER_FLAG, buf, sizeof(buf));
	triggerIdx = atoi(buf) & 0xFF;
	if ( 1 != triggerIdx )
	{	
		wifimgr_lib_get_MESH_TRIGGER_WAIT(&time);
		ecnt_uloop_timeout_set(&mesh_trigger_timeout, time);
	}
	
	return ;
}
