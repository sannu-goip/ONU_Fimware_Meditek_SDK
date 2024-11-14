#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cfg_api.h>
#include <svchost_api.h>
#include "central_ctrl_common.h"
#include "../cfg/utility.h"


int g_svc_central_ctrl_level = E_NO_INFO_LEVEL;

void svc_cc_debug_level(int level)
{
	g_svc_central_ctrl_level = level;
	tcdbg_printf("g_svc_central_ctrl_level = %d\n", g_svc_central_ctrl_level);
	
	return ;
}

 void cc_send_evt(int type, int evt_id, char *buf)
{
	central_ctrl_evt_t evt;
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
		case EVT_CC_INTERNAL_TYPE:
		{
			snprintf(evt_type, sizeof(evt_type), EVT_CENTRAL_CTRL_INTERNAL);
			break;
		}
		case EVT_CC_EXTERNAL_TYPE:
		{
			snprintf(evt_type, sizeof(evt_type), EVT_CENTRAL_CTRL_EXTERNAL);
			break;
		}
		default:
		{
			return;
		}
	} 
	
 	cfg_send_event(evt_type, evt_id, (void *)&evt , sizeof(mesh_evt_t));
	return ;
}
void file_read(char *path, char *buf, int size)
{
	int    fd = 0;
	int bytes = 0;

	memset(buf, 0, size);
	if((NULL == path) || (NULL == buf))
	{
		SVC_CENTRAL_CTRL_ERROR_INFO("path or buf can't be NULL.\n");
		return ;
	}

	fd = open(path, O_RDONLY);
	if(-1 == fd)
	{
		return;
	}
	bytes = read(fd, buf, size);
	close(fd);
	if(bytes <= 0)
	{
		SVC_CENTRAL_CTRL_ERROR_INFO("path or buf can't be NULL.\n");
		return ;
	}

	return;
}


void file_write(char *path, char *buf, int size)
{
	int    fd = 0;
	int bytes = 0;
	
	if((NULL == path) || (NULL == buf))
	{
		SVC_CENTRAL_CTRL_DEBUG_INFO("path or buf can't be NULL.\n");
		return ;
	}
	fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0777);
	if(-1 == fd)
	{
		SVC_CENTRAL_CTRL_ERROR_INFO("open path = %s file fail.\n", path);
		return;
	}
	
	bytes = write(fd, buf, size);
	close(fd);
	if(bytes != size)
	{
		SVC_CENTRAL_CTRL_ERROR_INFO("bytes = %d, size = %d.\n", bytes, size);
		return ;
	}
	
	return;
}


int svc_central_ctrl_execute_cmd(char* cmd)
{
	SVC_CENTRAL_CTRL_DEBUG_INFO("cmd = %s.\n", cmd);

	system(cmd);

	return 0;
}
 

