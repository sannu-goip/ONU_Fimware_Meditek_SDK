/*************************
	author:  jw.ren
	date:    20190517
*************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include "mesh_map_common.h"
#include "mesh_action.h"
#include <libapi_lib_meshmgr.h>
#include "mesh_map_cfg.h"
#include <utility.h>
#include <cfg_cli.h>
#include "svchost_evt.h"

#define WAPP_READY       (1 << 0)
#define WAPP_DEV_READY   (1 << 1)

static void msg_process(char *buf, int len)
{
	
	struct evt_ct *event = NULL;
	event = (struct evt_ct *)buf;
	char value[64];
	unsigned char map_enable = 0;
	unsigned char bs20_enable = 0;
	unsigned char role = 0;
	
	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_DAT_NODE, MAP_ENABLE_ATTR, value, sizeof(value));
	map_enable = atoi(value);

	memset(value, 0, sizeof(value));
	cfg_get_object_attr(MESH_DAT_NODE, BS20_ENABLE_ATTR, value, sizeof(value));
	bs20_enable = atoi(value);
	static int wapp_bit = 0;
	SVC_MESH_DEBUG_INFO("********event->type = %d**********\n", event->type);
	switch (event->type)
	{
		case NOTIFY_WAPP_READY:
		case NOTIFY_WAPP_DEV_READY:
		{
			if(NOTIFY_WAPP_READY == event->type)
			{
				wapp_bit |= WAPP_READY;
			}
			else
			{
				wapp_bit |= WAPP_DEV_READY;
			}
			
			if((WAPP_READY | WAPP_DEV_READY) == wapp_bit)
			{
				sleep(3);
				wapp_bit = 0;
				if(map_enable)
				{
					init_p1905_managerd();
				}
				else if(bs20_enable)
				{
					init_bs20();
				}
			}
			break;
		}
		case NOTIFY_P1905_READY:
		{
			init_mapd();
			break;
		}
		case NOTIFY_MAPD_READY:
		{
			disconnect_all_sta();
			cfg_set_object_attr(MESH_COMMON_NODE, MAP_REINIT_WIFI_FLAG, MESH_BLOCK_PAGE_END);
			set_map_allow_pkt_proc(DROP_SOME_1905_PKT);

			cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
			role = dev_roller(atoi(value));
			if (role == MAP_CONTROLLER)
			{
				memset(value, 0, sizeof(value));
				sprintf(value,"/usr/script/1905_mesh.sh %d", 0);
				system(value);
			}
			break;
		}
		case NOTIFY_TOPOLOGY_PROCESS:
		{
			SVC_MESH_DEBUG_INFO("NOTIFY_TOPOLOGY_PROCESS\n");
			break;
		}
		case NOTIFY_WPS_START:
		{
			SVC_MESH_DEBUG_INFO("NOTIFY_WPS_START\n");
			break;
		}
		case NOTIFY_WPS_STOP:
		{
			SVC_MESH_DEBUG_INFO("NOTIFY_WPS_STOP\n");
			break;
		}
		case NOTIFY_APCLI_DISASSOCIATED:
		{
			tcdbg_printf("NOTIFY_APCLI_DISASSOCIATED\n");
			break;
		}
		case NOTIFY_APCLI_ASSOCIATED:
		{
			tcdbg_printf("NOTIFY_APCLI_ASSOCIATED\n");
			break;
		}
		case NOTIFY_BH_READY:
		{
			tcdbg_printf("NOTIFY_BH_READY\n");
			memset(value, 0, sizeof(value));
			snprintf(value, sizeof(value), "up");
			cfg_send_event(EVT_PON_EXTERNAL, EVT_MESH_BH_STATUS, (void *)(value), strlen(value));
			break;
		}
		case NOTIFY_NETWORK_COMPLETE:
		{
			tcdbg_printf("NOTIFY_NETWORK_COMPLETE\n");
			cfg_get_object_attr(MESH_COMMON_NODE, MAP_DEV_ROLLER_ATTR, value, sizeof(value));
			role = dev_roller(atoi(value));

			if (role == MAP_AGENTER)
			{
				memset(value, 0, sizeof(value));
				sprintf(value,"/usr/script/1905_mesh.sh %d", 1);
				system(value);
			}
			break;
		}
		case NOTIFY_BH_DISCONNET:
		{
			tcdbg_printf("NOTIFY_BH_DISCONNET ------");
			memset(value, 0, sizeof(value));
			snprintf(value, sizeof(value), "down");
#if defined(TCSUPPORT_MAP_R2)
			disconnect_all_sta();
#endif
			cfg_send_event(EVT_PON_EXTERNAL, EVT_MESH_BH_STATUS, (void *)(value), strlen(value));
			break;			
		}

		default:
		{
			break;
		}			
	}

	return ;
}

static int mesh_unix_open(const char *path)
{
	int sock = -1;
	
	struct sockaddr_un sun;

 	if (strlen(path) >= sizeof(sun.sun_path)) 
	{
		SVC_MESH_ERROR_INFO("path = %s too long\n", path);
		return -1;
	}
	
 	unlink(path);
  	sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) 
	{
		SVC_MESH_ERROR_INFO("create socket fail\n");
		return -1;
 	}
	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
 	snprintf(sun.sun_path, sizeof(sun.sun_path), "%s", path);
 	if (bind(sock, (struct sockaddr *)&sun, sizeof(sun))) 
	{
		SVC_MESH_ERROR_INFO("bind socket fail\n");
		close(sock);
		return -1;
	}

	return sock;
}

static void mesh_set_fd_nonblocking(int sock)
{
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
	return ;
}

void mesh_monitor_msg(void)
{
	int  bytes_recv = 0;
	int  sock = -1;
	int  remote_len = 0;
	fd_set rfds;
	char buf[MAX_EVT_BUFFER_LEN];
	struct sockaddr_un remote;
	
	sock = mesh_unix_open(TMP_MESH_CTRL_DIR);
	if(sock < 0)
	{
		return -1;
	}
	/*O_NONBLOCK*/
	mesh_set_fd_nonblocking(sock);
	
	for(;;)
	{
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		select(sock + 1, &rfds, NULL, NULL, NULL);
		memset(&remote, 0, sizeof(remote));
		remote_len = sizeof(remote);
		if(FD_ISSET(sock, &rfds))
		{
			bytes_recv = recvfrom(sock, buf, sizeof(buf), 0,(struct sockaddr *) &remote, &remote_len);
			if(0 > bytes_recv)
			{
				continue;
			}
			msg_process(buf, bytes_recv);
		}
	}

	if (0 <= sock) 
	{
		close(sock);
		sock= -1;
	}
	
	return ;
}


int mesh_pthread(void)
{
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, (void *)mesh_monitor_msg, NULL) < 0)
    {
        printf("mesh_monitor_msg: create thread fail \n");
        return -1;
    }

    return 0;
}


