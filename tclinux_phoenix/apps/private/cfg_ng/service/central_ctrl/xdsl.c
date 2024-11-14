#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "check_button.h"
#include "device_alarm.h"
#include "central_ctrl_common.h"
#include "central_ctrl_polling.h"
#include "check_ping_diagnostic.h"
#include "lan_port_link.h"
#include "cfg_cli.h"
#include <svchost_evt.h>
#include "cfg_msg.h"
#include <time.h>
#include "utility.h"
#include "cfg_type_pppoeemulator.h"
#include <sys/vfs.h>
#include "cfg_type_transferservices.h"
#if defined(TCSUPPORT_ECNT_MAP)
#include "uloop_timer.h"
#endif
#include <fcntl.h>
#include "cfg_types.h"
#include "blapi_xdsl.h"

int g_dsl_state = 0;      /*last time dsl up state*/
#define MAX_SMUX_NUM 8
#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE_NUM 32
#define MAX_STATIC_ROUTE6_NUM 32
#else
#define MAX_STATIC_ROUTE_NUM 16
#define MAX_STATIC_ROUTE6_NUM 16 
#endif
#define MAX_DDNS_NUM 64

int is_adsl_link_up()
{
	int ret = -1;
	char stats[128] = {'\0'};

	blapi_xdsl_get_xdsl_linkstatus(stats, &ret);

	if (strstr(stats, "up") != NULL)
	{
		return 1;
	}
	return 0;
}

void copy_from_xdsl_node(char *dst_node, int entry_num, int common_flag)
{
	char src_nodeName[MAXLEN_NODE_NAME] = {0};
	char dst_nodeName[MAXLEN_NODE_NAME] = {0};
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char node[MAXLEN_NODE_NAME] = {0};
	char path[MAXLEN_NODE_NAME] = {0};
	char dslMode[8] = {0};
	int i = 0;

	/*check ATM or PTM*/
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYS_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_obj_get_object_attr(nodeName, "DslMode", 0, dslMode, sizeof(dslMode));
	if(strcmp(dslMode,"ATM") == 0)
	{
		snprintf(node, sizeof(node), "%satm", dst_node);
	}
	else
	{
		snprintf(node, sizeof(node), "%sptm", dst_node);
	}

	if(entry_num < 0)
	{
		/*clear destination node*/
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s.entry", dst_node);
		cfg_delete_object(path);
		
		/*copy xdsl node to destination node */
		memset(src_nodeName, 0, sizeof(src_nodeName));	
		snprintf(src_nodeName, sizeof(src_nodeName), "%s.entry", node);
		
		if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
		{/*node not exist*/
			return;
		}
		
		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.entry", dst_node);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);			
	}
	else
	{
		/*clear destination node*/
		for(i = 1 ; i <= entry_num; i++)
		{	
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "%s.entry.%d", dst_node, i);
			cfg_delete_object(path);
		}
		
		/*copy xdsl node to destination node */
		for(i = 1 ; i <= entry_num; i++)
		{
			memset(src_nodeName, 0, sizeof(src_nodeName));	
			snprintf(src_nodeName, sizeof(src_nodeName), "%s.entry.%d", node, i);
			
			if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
			{/*node not exist*/
				continue;
			}
			
			memset(dst_nodeName, 0, sizeof(dst_nodeName));
			snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.entry.%d", dst_node, i);
			cfg_check_create_object(dst_nodeName);
			cfg_obj_copy_private(dst_nodeName, src_nodeName);			
		}
	}

	
	if(common_flag == 1)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s.common", dst_node);
		cfg_delete_object(path);
		
		memset(src_nodeName, 0, sizeof(src_nodeName));
		snprintf(src_nodeName, sizeof(src_nodeName), "%s.common", node);
		
		if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
		{/*node not exist*/
			return;
		}
		
		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.common", dst_node);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);			
	}

	return;
}

int copy_wan_node(int state)
{
	char dst_nodeName[MAXLEN_NODE_NAME] = {0};
	char src_nodeName[MAXLEN_NODE_NAME] = {0};	
	char nodeName[MAXLEN_NODE_NAME] = {0};	
	int i = 0, j = 0;
	int dsl_state = 0;		
	char dslMode[8] = {0};
	char node[MAXLEN_NODE_NAME] = {0};
	char path[MAXLEN_NODE_NAME] = {0};
	int mode = 0;
	char xdsl_status[128] = {'\0'};

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYS_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_get_object_attr(nodeName, "DslMode", dslMode, sizeof(dslMode));
	if ( 1 == state ) /* ATM*/
	{
		if ( 0 == strcmp(dslMode,"ATM") )
			return SUCCESS;
	}
	else if ( 2 == state ) /* PTM*/
	{
		if ( 0 == strcmp(dslMode,"PTM") )
			return SUCCESS;
	}

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, WEBCUSTOM_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_set_object_attr(nodeName, "WanLocked", "1");
	
	/*clear wan pvc*/
	cfg_set_object_attr(nodeName, "WanCPFLAG", "1");
	for(i = 1 ; i <= PVC_NUM; i++)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), WAN_PVC_NODE, i);
		cfg_delete_object(path);
	}
	cfg_set_object_attr(nodeName, "WanCPFLAG", "0");

	/*clear wan common*/
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), WAN_COMMON_NODE);
	cfg_delete_object(path);
	
	/*check: WanATM/WanPTM*/
	blapi_xdsl_get_xdsl_linkstatus(xdsl_status, &dsl_state);
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYS_ENTRY_NODE, sizeof(nodeName) - 1);
	
	if(dsl_state == 1) 
	{	/*ADSL*/
		strncpy(node, WANATM_NODE, sizeof(node) - 1);
		cfg_set_object_attr(nodeName, "DslMode", "ATM");
	}
	else if(dsl_state == 2) 
	{	/*VDSL*/
		strncpy(node, WANPTM_NODE, sizeof(node) - 1);
		cfg_set_object_attr(nodeName, "DslMode", "PTM");
	}
	else
	{	/*down*/
		cfg_get_object_attr(nodeName, "DslMode", dslMode, sizeof(dslMode));
		if(strcmp(dslMode,"ATM") == 0)
		{
			strncpy(node, WANATM_NODE, sizeof(node) - 1);
		}
		else
		{
			strncpy(node, WANPTM_NODE, sizeof(node) - 1);
		}
	}

	/*copy wan common node*/
	memset(src_nodeName, 0, sizeof(src_nodeName));
	snprintf(src_nodeName, sizeof(src_nodeName), "%s.common", node);
	if(cfg_obj_query_object(src_nodeName, NULL, NULL) >= 0)
	{		/*node  exist*/
		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		strncpy(dst_nodeName, WAN_COMMON_NODE, sizeof(dst_nodeName) - 1);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);	
	}

	/*copy wan pvc node*/
	for(i = 1 ; i <= PVC_NUM; i++)
	{
		memset(src_nodeName, 0, sizeof(src_nodeName));
		snprintf(src_nodeName, sizeof(src_nodeName), "%s.pvc.%d", node, i);
		if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
		{		/*node not exist*/
			continue;
		}

		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), WAN_PVC_NODE, i);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);	

		/*Entry*/
		
		for(j = 1 ; j <= MAX_SMUX_NUM; j++)
		{
			memset(src_nodeName, 0, sizeof(src_nodeName));
			snprintf(src_nodeName, sizeof(src_nodeName), "%s.pvc.%d.entry.%d", node, i, j);
			if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
			{		/*node not exist*/
				continue;
			}

			memset(dst_nodeName, 0, sizeof(dst_nodeName));
			snprintf(dst_nodeName, sizeof(dst_nodeName), WAN_PVC_ENTRY_NODE, i, j);
			
			cfg_check_create_object(dst_nodeName);
			cfg_obj_copy_private(dst_nodeName, src_nodeName);	
		}
	}

	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, WEBCUSTOM_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_set_object_attr(nodeName, "WanLocked", "0");

	copy_from_xdsl_node("root.virserver", MAX_SMUX_NUM, 0);
	copy_from_xdsl_node("root.route", MAX_STATIC_ROUTE_NUM, 0);
	copy_from_xdsl_node("root.route6", MAX_STATIC_ROUTE6_NUM, 0);
	copy_from_xdsl_node("root.dmz", MAX_WAN_IF_INDEX, 0);
	copy_from_xdsl_node("root.igmpproxy", -1, 0);
	copy_from_xdsl_node("root.mldproxy", -1, 0);
	copy_from_xdsl_node("root.ddns", MAX_DDNS_NUM, 1);

	return SUCCESS;
}

#if defined(TCSUPPORT_CT_SWQOS)
void reload_swqos(void)
{
	char val[8] = {0};
	wan_related_evt_t param;
	
	system_escape("rmmod swqos.ko");
	system_escape("insmod /lib/modules/swqos.ko");
	memset(val, 0, sizeof(val));
	if(cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "qosBoot", val, sizeof(val)) > 0 && !strcmp(val, "1"))
	{
		memset(&param, 0, sizeof(param));
		strncpy(param.buf, "root.qos", sizeof(param)-1);
		cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_QOS_UPDATE, (void *)&param, sizeof(param)); 
	}
	
	return;
}
#endif

int adsl_state_change(int adsl_state)
{
	int i = 0;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char string[80] = {0};
	int dsl_state = 0;
    char val[8] = {0};
	int ret = -1;
	char xdsl_status[128] = {0};
	switch(adsl_state)
	{
		case 1:
				/*Add action when adsl from down to up*/
#ifdef WAN_DBUG
				tcdbg_printf("adsl_state_change:adsl link from down to up\n");
#endif
				blapi_xdsl_get_xdsl_linkstatus(xdsl_status, &dsl_state);
				if(g_dsl_state != dsl_state)
				{		
					copy_wan_node(dsl_state);
					ret = blapi_xdsl_reload_ko(dsl_state);
#if defined(TCSUPPORT_CT_SWQOS)
						reload_swqos();
#endif
					}
				else if(g_dsl_state == 2)
			{
				/*PTM, when vdsl is from down to up, PTM MAC and TC will have chance to be un-sync, so reset is needed */
					blapi_xdsl_ptm_do_reset_sequence(adsl_state, g_dsl_state);
				}
			
				g_dsl_state = dsl_state;
			snprintf(val, sizeof(val), "up");
			cfg_send_event(EVT_PON_EXTERNAL,EVT_DSL_WAN_STATUS,(void *)(val),strlen(val));
			break;
			
		case 0:
				/*Add action when adsl from up to down*/
#ifdef WAN_DBUG
				tcdbg_printf("adsl_state_change:adsl link from up to down\n");
#endif
			snprintf(val, sizeof(val), "down");
			cfg_send_event(EVT_PON_EXTERNAL,EVT_DSL_WAN_STATUS,(void *)(val),strlen(val));
			if (g_dsl_state != 0)
				{
				/* when xdsl from up to down, let PTM MAC reset hold, in order to  make sure dmt reset(when vdsl is up) is inside PTM MAC reset */
				blapi_xdsl_ptm_do_reset_sequence(adsl_state, g_dsl_state);
				}
			break;

		default:
			tcdbg_printf("adsl_state_change:unknown adsl state type:%d\n",adsl_state);
			break;
	}

	return 0;
}

