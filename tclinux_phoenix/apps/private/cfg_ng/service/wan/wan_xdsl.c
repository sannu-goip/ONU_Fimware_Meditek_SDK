

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
#include <unistd.h>
#include <sys/stat.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_cfg.h"
#include "wan_interface.h"
#include "wan_mgr.h"
#include "wan_nat.h"
#include "port_binding.h"
#include "full_route.h"
#include "dslite_info.h"
#include "dhcp_relay_info.h"
#include "dhcp_port_filter_info.h"
#include "vlan_binding.h"
#include "misc_srv.h"
#include "utility.h"
#include "atmcmdd.h"
#include <sys/socket.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/un.h>

void send2atmcmdd(atmcmd_msg_t *msg)
{
	int sockfd=0, len=0;
	struct sockaddr_un remote;

	msg->retval = ATMCMD_FAIL;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
		perror("socket");
		return;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, ATMCMD_SOCK_PATH);
	tcdbg_printf("[%s]%d: remote.sun_path = %s\n", __FUNCTION__, __LINE__, remote.sun_path);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
		tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
		perror("connect");
		close(sockfd);
		return;
	}

	if (send(sockfd, msg, sizeof(atmcmd_msg_t), 0) == -1){
		tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
		perror("send");
		close(sockfd);
		return;
	}

	memset(msg, 0, sizeof(atmcmd_msg_t));

	if ((recv(sockfd, msg, sizeof(atmcmd_msg_t), 0)) <= 0 ) {
		tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
		/*Failure to receive information from cfg manage*/
		perror("recv");
		close(sockfd);
		return;
	}
	close(sockfd);
}

int atmcmdc_create_pvc(int itfnum, int vpi, int vci, int encap, int payload,
		int qos_type, int pcr, int scr, int mbs, int sndbuf)
{
	atmcmd_msg_t msg;

	memset(&msg, 0, sizeof(msg));

	/*Fill information*/
	msg.op_cmd = ATMCMD_CREATE_PVC;
	msg.itfnum = itfnum;
	msg.vpi = vpi;
	msg.vci = vci;
	msg.encap = encap;
	msg.payload = payload;
	msg.qos_type = qos_type;
	msg.pcr = pcr;
	msg.scr = scr;
	msg.mbs = mbs;
	msg.sndbuf = sndbuf;

	send2atmcmdd(&msg);
	return msg.retval;
}

void atm_create_interfae(char *node, wan_entry_obj_t* obj)
{
    char transMode[16] = {0};
	char str_qos[12] = {0};
	char str_encap[30] = {0};
	char str_vpi[5] = {0};
	char str_vci[5] = {0};
	char str_pcr[7] = {0};
	char str_scr[7] = {0};
	char str_mbs[10] = {0};
	char pcr_v[32] = {0};
	char encap_t[8] = {0};
	char isp[4] = {0};
	char cmd[128] = {0};
	int vpi = -1;
	int vci = -1;
	int pcr = 0;
	int scr = 0;
	int mbs = 0;
	int encap = -1;
	int payload = -1;
	int qos_type = -1;

	
	tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
	if( 0 <= cfg_get_object_attr(SYS_ENTRY_NODE, "DslMode", transMode,sizeof(transMode))
		&& (0 == strcmp(transMode, "ATM")))
	{
		if(cfg_get_object_attr(node, "QOS", str_qos,sizeof(str_qos)) < 0)
		{
			tcdbg_printf("[%s]%d:fail get QOS\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		if(cfg_get_object_attr(node, "ENCAP", str_encap, sizeof(str_encap)) < 0)
		{
			tcdbg_printf("[%s]%d:fail get ENCAP\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		if(cfg_get_object_attr(node, "VPI", str_vpi, sizeof(str_vpi)) < 0) 
		{
			tcdbg_printf("[%s]%d:fail get VPI\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		if(cfg_get_object_attr(node, "VCI", str_vci, sizeof(str_vci)) < 0) 
		{
			tcdbg_printf("[%s]%d:fail get VCI\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		cfg_get_object_attr(node, "PCR", str_pcr, sizeof(str_pcr));
		cfg_get_object_attr(node, "SCR", str_scr, sizeof(str_scr));
		cfg_get_object_attr(node, "MBS", str_mbs, sizeof(str_mbs));

		if(0 == strlen(str_qos) || 0 == strlen(str_encap) || 0 == strlen(str_vpi) || 0 == strlen(str_vci))
		{
			tcdbg_printf("[%s]%d:fail get PVC attribute\n" , __FUNCTION__, __LINE__);
			return -1;
		}

		vpi = atoi(str_vpi);
		vci = atoi(str_vci);
		if(vpi == 0 && vci == 0)
		{
			tcdbg_printf("[%s]%d:wrong VPI,VCI(%s, %s)\n" , __FUNCTION__, __LINE__, str_vpi, str_vci);
			return -1;
		}

		if(!strcmp(str_encap, "1483 Bridged IP LLC") || !strcmp(str_encap, "PPPoE LLC"))
		{
			encap = 0;
			payload = 1;
		}
		else if(!strcmp(str_encap, "1483 Bridged IP VC-Mux") || !strcmp(str_encap, "PPPoE VC-Mux"))
		{
			encap = 1;
			payload = 1;
		}
		else
		{
			tcdbg_printf("[%s]%d:not support encap:%s\n", __FUNCTION__, __LINE__, str_encap);
			return -1;
		}

		if(!strcmp(str_qos, "ubr"))
		{
			qos_type = 1;
		}
		else if(!strcmp(str_qos, "ubr+"))
		{
			qos_type = 1;
			pcr = atoi(str_pcr);
		}
		else if(!strcmp(str_qos, "cbr"))
		{
			qos_type = 2;
			pcr = atoi(str_pcr);
		}
		else if(!strcmp(str_qos, "rt-vbr"))
		{
			qos_type = 3;
			pcr = atoi(str_pcr);
			scr = atoi(str_scr);
			mbs = atoi(str_mbs);
		}
		else if(!strcmp(str_qos, "nrt-vbr"))
		{
			qos_type = 6;
			pcr = atoi(str_pcr);
			scr = atoi(str_scr);
			mbs = atoi(str_mbs);
		}
		else
		{
			tcdbg_printf("[%s]%d:not support qos:%s\n", __FUNCTION__, __LINE__, str_qos);
			return -1;
		}

		atmcmdc_create_pvc((obj->idx / 8), vpi, vci, encap, payload, qos_type, pcr, scr, mbs, 8192);
		cfg_set_object_attr(node, "PVCACTIVE", "Yes");

		
		if(cfg_get_object_attr(obj->path, "ISP", isp, sizeof(isp)) < 0)
		{
			tcdbg_printf("[%s]%d:fail get QOS\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		
		if((0 == atoi(isp)) || (1 == atoi(isp)))
		{
			if(!strcmp(str_qos, "ubr"))
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d", pcr);
			}
			else
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d -q %d -m %d", pcr, scr, mbs);
			}

			if(!strcmp(str_encap, "1483 Bridged IP LLC"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 0");
			}
			else if(!strcmp(str_encap, "1483 Bridged IP VC-Mux"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 1");
			}
			else if(!strcmp(str_encap, "1483 Routed IP LLC(IPoA)"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 2");
			}
			else
			{
				snprintf(encap_t, sizeof(encap_t), "-e 3");
			}

		}
		else if(2 == atoi(isp))
		{
			if(!strcmp(str_qos, "ubr"))
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d", pcr);
			}
			else
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d -q %d -m %d", pcr, scr, mbs);
			}
			
			if(!strcmp(str_encap, "PPPoE LLC"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 0");
			}
			else
			{
				snprintf(encap_t, sizeof(encap_t), "-e 1");
			}
		}
		else
		{
			if(!strcmp(str_qos, "ubr"))
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d", pcr);
			}
			else
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d -q %d -m %d", pcr, scr, mbs);
			}
			
			if(!strcmp(str_encap, "1483 Bridged IP LLC"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 0");
			}
			else
			{
				snprintf(encap_t, sizeof(encap_t), "-e 1");
			}
		}
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "br2684ctl -c %d %s -t %s %s -a 0.%d.%d &", 
			(obj->idx / 8), encap_t, str_qos, pcr_v, vpi, vci);
		tcdbg_printf("[%s]%d: cmd = %s\n", __FUNCTION__, __LINE__, cmd);
		svc_wan_execute_cmd(cmd);
		
	}
	return;
}

void atm_create_device(wan_entry_obj_t* obj, char *dev, int dev_size)
{
    char mac[32] = {0};
    FILE* fp;
	size_t len = 0;
	ssize_t read = 0;
	char *line = NULL;
    char transMode[16] = {0};
    char cmd[SVC_WAN_BUF_256_LEN] = {0};


	if( 0 <= cfg_get_object_attr(SYS_ENTRY_NODE, "DslMode", transMode,sizeof(transMode))
		&& (0 == strcmp(transMode, "PTM")))
	{
		snprintf(dev, dev_size,"ptm0");
	}
	else
	{
		snprintf(dev, dev_size, "nas%d", (obj->idx / 8));

		if ((fp = fopen("/etc/mac.conf","r")) == NULL)
		{
			tcdbg_printf("[%s]%d\n", __FUNCTION__, __LINE__);
			return -1;
		}

		while((read =  getline(&line, &len, fp)) != -1)
		{
			if(strstr(line, "WAN_MAC=") != NULL)
			{
				sscanf(line, "WAN_MAC=%s", mac);
				break;
			}
		}

		if(line)
		{
			free(line);
		}
			
		fclose(fp);
		
			
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ifconfig %s hw ether %s", dev, mac);
		svc_wan_execute_cmd(cmd);
		
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ifconfig %s up", dev);
		svc_wan_execute_cmd(cmd);
	}
}

