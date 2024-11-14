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
#include <cfg_cli.h> 
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <svchost_api.h> 
#include "cfg_msg.h"
#include "utility.h"
#include "global_def.h"
#include "libapi_lib_portbind.h"
#include "wan_related_mgr.h"
#include "wan_related_vxlan_link_list.h"
#include "wan_related_vxlan_cfg.h"
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
#include <lan_port/bind_list_map.h>
#endif

static void svc_wan_related_mtu_conf(vxlan_entry_obj_t* obj, int action)
{
	char cmd[256];
	vxlan_entry_cfg_t* cfg = &obj->cfg;

	/* bridge mode, not config mtu? */
	if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )
		return;

	if ( E_CREAT_VXLAN_DEVICE == action )
	{
		if(0 != cfg->max_mtu)
		{
			if ( 0 != obj->original_dev[0] )
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s mtu %d", obj->original_dev, cfg->max_mtu);
				system(cmd);
			}
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s mtu %d", obj->vxlan_dev, cfg->max_mtu);
			system(cmd);
		}
		else
		{
			if ( 0 != obj->original_dev[0] )
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s mtu 1440", obj->original_dev);
				system(cmd);
			}
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s mtu 1440", obj->vxlan_dev);
			system(cmd);
		}
	}

	memset(cmd, 0, sizeof(cmd));
	if ( E_CREAT_VXLAN_DEVICE == action )
	{
		snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -A FORWARD -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", obj->vxlan_dev);
	}
	else if ( E_DEL_VXLAN_DEVICE == action )
	{
		snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -D FORWARD -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu", obj->vxlan_dev);
	}

	system(cmd);

	return ;
}

int svc_wan_related_start_vxlan_nat(char* dev)
{
	char cmd[256];

	if ( ( NULL == dev ) || ( 0 >= strlen(dev) ) )
	{
		return 0;
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t nat -A POSTROUTING -o %s -j MASQUERADE", dev);	
	system(cmd);

	return 0;
}

int svc_wan_related_stop_vxlan_nat(char* dev)
{
	char cmd[256];

	if ( ( NULL == dev ) || ( 0 >= strlen(dev) ) )
	{
		return 0;
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t nat -D POSTROUTING -p all -o %s -j MASQUERADE", dev);
	system(cmd);

	return 0;
}

int get_vxlan_bind_info(vxlan_entry_obj_t *obj, vxlan_port_bind_info_t* info)
{
	vxlan_entry_cfg_t* cfg = NULL;
	int i = 0;
	char bind_flag = 0;

	if ( ( NULL == obj ) || ( NULL == info ) )
	{
		return -1;
	}

	cfg = &obj->cfg;
	for ( i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++ )
	{
		if ( 0 == (cfg->bind_lanbit & (1<<i)) )
		{
			continue;
		}

		if ( 0 == strlen(info->bind_ifName_list) )
		{
			strncpy(info->bind_ifName_list, bl_map_data[i].realif, sizeof(info->bind_ifName_list) - 1);
			strcpy(info->bind_ifType_list, "O_O");
		}
		else
		{
			snprintf(info->bind_ifName_list + strlen(info->bind_ifName_list), 
			sizeof(info->bind_ifName_list) - strlen(info->bind_ifName_list), ",%s", bl_map_data[i].realif);
			strcat(info->bind_ifType_list, ",O_O");
		}
	}

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
	if ( 0 == strlen(info->bind_ifName_list) )
	{
		snprintf(info->bind_lanif_list, sizeof(info->bind_lanif_list), NOTBINDSTRING);
	}
	else
	{
		snprintf(info->bind_lanif_list, sizeof(info->bind_lanif_list), info->bind_ifName_list);
	}
#endif

	if ( strlen(info->bind_ifName_list) > 0 ) 
	{
		snprintf(info->bind_ifName_list + strlen(info->bind_ifName_list), 
		sizeof(info->bind_ifName_list) - strlen(info->bind_ifName_list), ",%s", obj->vxlan_dev);
		bind_flag = 1;
	}
	else
	{
		strcpy(info->bind_ifName_list, obj->vxlan_dev);
		bind_flag = 0;
	}

	if ( bind_flag )
	{
		if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )
		{
		strcat(info->bind_ifType_list, ",O_B");
		}
		else
		{
		strcat(info->bind_ifType_list, ",O_R");
		}
	}
	else
	{
		if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )
		{
			strcpy(info->bind_ifType_list, "O_B");
		}
		else
		{
			strcpy(info->bind_ifType_list, "O_R");
		}
	}

	info->if_index  = obj->idx + MAX_WAN_ITF_NUM;
	info->mode_flag = (SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode);

	return 0;
}

void set_vxlan_port_binding_group(char *path, int action)
{
	vxlan_entry_obj_t* obj = NULL;
	vxlan_entry_cfg_t* cfg = NULL;
	vxlan_port_bind_info_t bind_info;
	char cmd[256] = {0};
	int ret = 0;

	if ( NULL == (obj = svc_wan_related_find_vxlan_obj(path)) )
	{
		return ;
	}

	cfg = &obj->cfg;
	if ( (E_ADD_VXLAN_PORT_BIND_RULE == action) && (SVC_WAN_RELATED_VXLAN_DISABLE == cfg->enable) )
	{
		return;
	}

	memset(&bind_info, 0, sizeof(bind_info));
	ret = get_vxlan_bind_info(obj, &bind_info);
	if ( 0 != ret )
	{
		return;
	}

	switch ( action ) 
	{
		case E_ADD_VXLAN_PORT_BIND_RULE:
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "portbindcmd addgroup %d %s %s", bind_info.if_index, bind_info.bind_ifName_list, bind_info.bind_ifType_list);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "portbindcmd enable");
			system(cmd);

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/accesslimitcmd addbindinfo %d %d %s", bind_info.if_index, bind_info.mode_flag, bind_info.bind_lanif_list);
			system(cmd);
#endif
			break;
		}
		case E_DEL_VXLAN_PORT_BIND_RULE:
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "portbindcmd delgroup %d", bind_info.if_index);
			system(cmd);	
#if defined(TCSUPPORT_CT_ACCESSLIMIT)
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/accesslimitcmd deletebindinfo %d", bind_info.if_index);
			system(cmd);	
#endif
			break;
		}
		default:
		{
			break;
		}
	}

	return ;
}

void set_vxlan_port_binding_route(vxlan_entry_obj_t* i_obj)
{
	int i=0;
	char node[32] = {0};
	char gate_way_v4[16] = {0};
	vxlan_entry_obj_t* obj = NULL;
	vxlan_entry_cfg_t* cfg = NULL;

	unsigned char add_linkroute[MAX_WAN_ITF_NUM];
	memset(add_linkroute, 0, sizeof(add_linkroute));

	/* first delete ip rule */
	if ( i_obj && i_obj->cfg.bind_lanbit )
	{
		for ( i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++ )
		{
			if( i_obj->cfg.bind_lanbit & (1<<i) )
			{
				portbind_lib_del_route("v4", bl_map_data[i].realif);
			}
		}
	}

	/* add ip rule*/
	for ( obj = svc_wan_related_get_vxlan_obj_list(); obj; obj = obj->next)
	{
		cfg = &obj->cfg;
		if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )
		{
			continue;
		}
		if ( 0 == (obj->flag & VXLAN_FLAG_UPV4) )
		{
			continue;
		}

		add_linkroute[obj->idx] = 0;
		memset(node, 0, sizeof(node));
		snprintf(node, sizeof(node), VXLAN_IPENTRY_NODE, obj->idx + 1); 
		memset(gate_way_v4, 0, sizeof(gate_way_v4));
		cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4));
		if ( 0 == gate_way_v4[0] )
		{
			continue;
		}

		if ( cfg->bind_lanbit )
		{
			for ( i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++ )
			{
				if ( 0 == (cfg->bind_lanbit & (1<<i)) )
				{
					continue;
				}

				/* add ip rule for every vxlan interface */
				if( 0 == add_linkroute[obj->idx] )
				{
					add_linkroute[obj->idx] = 1;
					vxlan_dealwith_link_local_route(obj, E_ADD_LINK_ROUTE);
				}
				/* for ipv4 */
				portbind_lib_add_route("v4", bl_map_data[i].realif, obj->vxlan_dev, gate_way_v4, NULL);
			}
		}
		if ( 1 == add_linkroute[obj->idx] )
		{
			add_linkroute[obj->idx] = 0;
			vxlan_dealwith_link_local_route(obj, E_DEL_LINK_ROUTE);
		}
	}

	return ;
}

static int svc_wan_related_create_vxlan_conf(vxlan_entry_obj_t* obj)
{
	char dir[32] = {0};
	int res = 0;

	if ( 0 == obj->vxlan_dev[0] )
	{
		return -1;
	}

	memset(dir, 0, sizeof(dir));
	snprintf(dir, sizeof(dir), "/var/run/%s", obj->vxlan_dev);

	res = mkdir(dir, 0777);
	if ( res )
	{
		/**do nothing,just for compile*/
	}
	return 0;
}

static int svc_wan_related_delete_vxlan_conf(vxlan_entry_obj_t* obj)
{
	char dir[32];
	char cmd[32];

	if ( 0 == obj->vxlan_dev[0] )
	{
		return -1;
	}

	memset(dir, 0, sizeof(dir));
	snprintf(dir, sizeof(dir), "/var/run/%s", obj->vxlan_dev);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/bin/rm -rf %s", dir);
	system(cmd);

	return 0;
}

static int svc_wan_related_create_vxlan_dev(vxlan_entry_obj_t* obj)
{
	char cmd[256] = {0};
	int destport = 4789;
	vxlan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip link add %s type vxlan id %s remote %s dstport %d", 
		obj->vxlan_dev, cfg->tunnelkey, cfg->remote_ip, destport);
	system(cmd);
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s up", obj->vxlan_dev);
	system(cmd);
	
	if ( SVC_WAN_RELATED_VXLAN_VLAN_ENABLE == cfg->vlan_enable 
		&& 0 != cfg->vlan )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/userfs/bin/vconfig add %s %u", obj->vxlan_dev, cfg->vlan);
		system(cmd);

		memset(obj->original_dev, 0, sizeof(obj->original_dev));
		strncpy(obj->original_dev, obj->vxlan_dev, sizeof(obj->original_dev) - 1);
		
		memset(obj->vxlan_dev, 0, sizeof(obj->vxlan_dev));
		snprintf(obj->vxlan_dev, sizeof(obj->vxlan_dev), "%s.%u", 
			obj->original_dev, cfg->vlan);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s up", obj->vxlan_dev);
		system(cmd);
	}
	
	if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )/* 2 layer */
	{
		set_vxlan_port_binding_group(obj->path, E_ADD_VXLAN_PORT_BIND_RULE);
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/brctl addif br0 %s", obj->vxlan_dev);
		system(cmd);
	}
	else/* 3 layer */
	{
		/* portbind addgroup && add route for remote IP */
		set_vxlan_port_binding_group(obj->path, E_ADD_VXLAN_PORT_BIND_RULE);
		if ( 0 != cfg->bind_wanitf[0] )
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/sbin/route add -net %s netmask 255.255.255.255 dev %s", 
				cfg->remote_ip, cfg->bind_wanitf);
			system(cmd);
		}
		svc_wan_related_mtu_conf(obj, E_CREAT_VXLAN_DEVICE);
	}
	
	return 0;
}

static int svc_wan_related_delete_vxlan_dev(vxlan_entry_obj_t* obj)
{
	char cmd[256] = {0};
	vxlan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;
	
	if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )/* 2 layer */
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/brctl delif br0 %s", obj->vxlan_dev);
		system(cmd);
	}
	else/* 3 layer */
	{
		/* portbind delgroup && del route for remote IP */
		set_vxlan_port_binding_group(obj->path, E_DEL_VXLAN_PORT_BIND_RULE);
		if ( 0 != cfg->bind_wanitf[0] )
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/sbin/route del -net %s netmask 255.255.255.255 dev %s", 
				cfg->remote_ip, cfg->bind_wanitf);
			system(cmd);
		}
		svc_wan_related_mtu_conf(obj, E_DEL_VXLAN_DEVICE);
	}
		
	if ( SVC_WAN_RELATED_VXLAN_VLAN_ENABLE == cfg->vlan_enable 
		&& 0 != cfg->vlan )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s down", obj->vxlan_dev);
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/userfs/bin/vconfig rem %s", obj->vxlan_dev);
		system(cmd);

		memset(obj->vxlan_dev, 0, sizeof(obj->vxlan_dev));
		strncpy(obj->vxlan_dev, obj->original_dev, sizeof(obj->vxlan_dev) - 1);
	}
	
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s down", obj->vxlan_dev);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip link del %s", obj->vxlan_dev);
	system(cmd);

	return 0;
}


int svc_wan_related_get_pid(char* file)
{
	FILE* fp = NULL;
	int pid = 0, res = 0;

	if ( NULL == (fp = fopen(file,"r")) )
	{
		return 0;
	}

	res = fscanf(fp,"%d", &pid);
	if ( res )
	{
		/*do nothing,just for compile*/
	}
	fclose(fp);

	return pid;
}

void send_signal_vxlan_dhcp(int evt)
{
	vxlan_entry_obj_t* obj = NULL;
	vxlan_entry_cfg_t* cfg = NULL;
	int pid = 0;

	for ( obj = svc_wan_related_get_vxlan_obj_list(); NULL != obj; obj = obj->next )
	{
		cfg  = &obj->cfg;
		if ( SVC_WAN_RELATED_VXLAN_ENABLE != cfg->enable)
		{
			continue;
		}

		if ( 0 == strcmp(cfg->addr_type, "DHCP") )
		{
			pid = svc_wan_related_get_pid(obj->pid);
			if ( pid <= 0 )
			{
				continue;
			}

			switch ( evt )
			{
				case E_VXLAN_XPON_DOWN:
				{
					kill(pid, SIGUSR2);
					obj->dhcp_release = 1;
					break;
				}
				case E_VXLAN_XPON_UP:
				{
					if ( obj->dhcp_release )
					{
						kill(pid, SIGUSR1);
						obj->dhcp_release = 0;
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	return ;
}

static int svc_wan_related_start_vxlan_dhcpc(vxlan_entry_obj_t* obj)
{
	char cmd[256];

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/udhcpc -i %s -s /usr/script/udhcpc_nodef.sh -p /var/run/%s/udhcpc.pid &", 
		obj->vxlan_dev,obj->vxlan_dev);
	snprintf(obj->pid, sizeof(obj->pid), "/var/run/%s/udhcpc.pid", obj->vxlan_dev);
	system(cmd);

	return 0;
}

static int svc_wan_related_stop_vxlan_dhcpc(vxlan_entry_obj_t* obj)
{
	int pid = 0;
	char cmd[256] = {0};

	pid = svc_wan_related_get_pid(obj->pid);
	if ( pid > 0 )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/bin/kill -15 %d", pid);
		system(cmd);
		sleep(1);
	}

	pid = svc_wan_related_get_pid(obj->pid);
	if ( pid > 0 )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/bin/kill -9 %d", pid);
		system(cmd);
	}

	svc_wan_related_mgr_handle_event(EVT_CFG_WAN_RELATED_VXLAN_LOST4, (wan_related_cfg_update_t*)(obj->vxlan_dev));

	return 0;
}

static int svc_wan_related_start_vxlan_static(vxlan_entry_obj_t* obj)
{
	char cmd[64];
	char fpath[32];

	memset(cmd, 0, sizeof(cmd));
	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s/status", obj->vxlan_dev);
	snprintf(cmd, sizeof(cmd),"/bin/echo up > %s", fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s/ip", obj->vxlan_dev);
	snprintf(cmd, sizeof(cmd), "/bin/echo %s > %s", obj->cfg.ip_addr, fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s/netmask", obj->vxlan_dev);
	snprintf(cmd, sizeof(cmd), "/bin/echo %s > %s", obj->cfg.subnetmask, fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s/gateway", obj->vxlan_dev);
	snprintf(cmd, sizeof(cmd), "/bin/echo %s > %s", obj->cfg.default_gateway, fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s/dns", obj->vxlan_dev);
	snprintf(cmd, sizeof(cmd),"/bin/echo %s > %s", obj->cfg.dnsservers_master, fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/bin/echo %s >> %s", obj->cfg.dnsservers_slave, fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s %s netmask %s", obj->vxlan_dev, obj->cfg.ip_addr, obj->cfg.subnetmask);
	system(cmd);

	svc_wan_related_mgr_handle_event(EVT_CFG_WAN_RELATED_VXLAN_GET4, (wan_related_cfg_update_t*)(obj->vxlan_dev));

	return 0;
}


static int svc_wan_related_stop_vxlan_static(vxlan_entry_obj_t* obj)
{
	char cmd[64];
	char fpath[32];

	memset(cmd, 0, sizeof(cmd));
	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s/status", obj->vxlan_dev);
	snprintf(cmd, sizeof(cmd), "/bin/echo down > %s", fpath);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s 0.0.0.0", obj->vxlan_dev);
	system(cmd);

	svc_wan_related_mgr_handle_event(EVT_CFG_WAN_RELATED_VXLAN_LOST4, (wan_related_cfg_update_t*)(obj->vxlan_dev));

	return 0;
}

int svc_wan_related_do_stop_vxlan_dhcpc(vxlan_entry_obj_t* obj)
{
	vxlan_entry_cfg_t* cfg = &obj->cfg;

	if ( 0 == strcmp(cfg->addr_type, "DHCP") )
	{
		svc_wan_related_stop_vxlan_dhcpc(obj);
	}
	else if ( 0 == strcmp(cfg->addr_type, "Static") )
	{
		svc_wan_related_stop_vxlan_static(obj);
	}
	else
	{
		return -1;	
	}

	svc_wan_related_delete_vxlan_conf(obj);

	return 0;
}


int svc_wan_related_do_start_vxlan_dhcpc(vxlan_entry_obj_t* obj)
{
	vxlan_entry_cfg_t* cfg = &obj->cfg;

	if ( 0 == strcmp(cfg->addr_type, "DHCP") )
	{
		svc_wan_related_start_vxlan_dhcpc(obj);
	}
	else if ( 0 == strcmp(cfg->addr_type, "Static") )
	{
		svc_wan_related_start_vxlan_static(obj);
	}
	else
	{
		goto error;
	}

	return 0;

error:
	svc_wan_related_do_stop_vxlan_dhcpc(obj);
	svc_wan_related_delete_vxlan_conf(obj);
	return -1;
}


static void svc_wan_related_stop_vxlan(vxlan_entry_obj_t* obj)
{
	vxlan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;
	if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )
	{
		svc_wan_related_delete_vxlan_dev(obj);
	}
	else if ( SVC_WAN_RELATED_VXLAN_3_LAYER == cfg->workmode )
	{
		svc_wan_related_do_stop_vxlan_dhcpc(obj);
		svc_wan_related_delete_vxlan_conf(obj);
		svc_wan_related_delete_vxlan_dev(obj);
	}
	
	return;
}

static void svc_wan_related_start_vxlan(vxlan_entry_obj_t* obj)
{
	vxlan_entry_cfg_t* cfg = NULL;

	cfg = &obj->cfg;
	if ( SVC_WAN_RELATED_VXLAN_2_LAYER == cfg->workmode )
	{
		svc_wan_related_create_vxlan_dev(obj);
	}
	else if ( SVC_WAN_RELATED_VXLAN_3_LAYER == cfg->workmode )
	{
		svc_wan_related_create_vxlan_dev(obj);
		svc_wan_related_create_vxlan_conf(obj);
		svc_wan_related_do_start_vxlan_dhcpc(obj);
	}

	return;
}

int get_vxlan_port_binding_interface(char* path)
{
	int i = 0;
	char tmp[64] = {0};
	unsigned int bindbit = 0;
	for ( i = 0; i < sizeof(bl_map_data) / sizeof(bl_map_data[0]); i++ )
	{
		memset(tmp, 0, sizeof(tmp));
		if ( cfg_get_object_attr(path, bl_map_data[i].bindif, tmp, sizeof(tmp)) > 0 )
		{
			if ( 0 == strcmp(tmp, "Yes") )
			{
				bindbit |= (1<<i);
			}
		}
	}

	return bindbit;
}

int svc_wan_related_load_vxlan_cfg(char* path, vxlan_entry_cfg_t* cfg, vxlan_entry_obj_t* obj)
{
	char tmpbuf[64] = {0};

	memset(cfg, 0, sizeof(vxlan_entry_cfg_t));

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if ( cfg_get_object_attr(path, "TunnelKey", tmpbuf, sizeof(tmpbuf)) > 0 )
	{
		strncpy(cfg->tunnelkey, tmpbuf, sizeof(cfg->tunnelkey) - 1);
	}
	else
	{
		strcpy(cfg->tunnelkey, "1");
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	cfg_get_object_attr(path, "Enable", tmpbuf, sizeof(tmpbuf));
	if ( 0 == strcmp(tmpbuf, "1") )
	{
		cfg->enable = SVC_WAN_RELATED_VXLAN_ENABLE;
	}
	else
	{
		cfg->enable = SVC_WAN_RELATED_VXLAN_DISABLE;
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if ( cfg_get_object_attr(path, "TunnelRemoteIp", tmpbuf, sizeof(tmpbuf)) > 0 )
	{
		strncpy(cfg->remote_ip, tmpbuf, sizeof(cfg->remote_ip) - 1);
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	cfg_get_object_attr(path, "WorkMode", tmpbuf, sizeof(tmpbuf));
	if ( 0 == strcmp(tmpbuf, "2") )
	{
		cfg->workmode = SVC_WAN_RELATED_VXLAN_3_LAYER;
	}
	else
	{
		cfg->workmode = SVC_WAN_RELATED_VXLAN_2_LAYER;
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if ( cfg_get_object_attr(path, "MaxMTUSize", tmpbuf, sizeof(tmpbuf)) > 0 )
	{
		cfg->max_mtu = atoi(tmpbuf);
	}

	if ( SVC_WAN_RELATED_VXLAN_3_LAYER == cfg->workmode )
	{
		memset(tmpbuf, 0, sizeof(tmpbuf));
		if ( cfg_get_object_attr(path, "IPAddress", tmpbuf, sizeof(tmpbuf)) > 0 )
		{
			strncpy(cfg->ip_addr, tmpbuf, sizeof(cfg->ip_addr) - 1);
		}
		
		memset(tmpbuf, 0, sizeof(tmpbuf));
		if ( cfg_get_object_attr(path, "SubnetMask", tmpbuf, sizeof(tmpbuf)) > 0 )
		{
			strncpy(cfg->subnetmask, tmpbuf, sizeof(cfg->subnetmask) - 1);
		}
		
		memset(tmpbuf, 0, sizeof(tmpbuf));
		cfg_get_object_attr(path, "AddressingType", tmpbuf, sizeof(tmpbuf));
		if ( 0 == strcmp(tmpbuf, "Static") )
		{
			strcpy(cfg->addr_type, "Static");
		}
		else
		{
			strcpy(cfg->addr_type, "DHCP");
		}
		
		memset(tmpbuf, 0, sizeof(tmpbuf));
		cfg_get_object_attr(path, "NATEnabled", tmpbuf, sizeof(tmpbuf));
		if ( 0 == strcmp(tmpbuf, "1") )
		{
			cfg->nat_enabled= SVC_WAN_RELATED_VXLAN_NAT_ENABLE;
		}
		else
		{
			cfg->nat_enabled= SVC_WAN_RELATED_VXLAN_NAT_DISABLE;
		}
		
		memset(tmpbuf, 0, sizeof(tmpbuf));
		if ( cfg_get_object_attr(path, "DNSServers_Master", tmpbuf, sizeof(tmpbuf)) > 0 )
		{
			strncpy(cfg->dnsservers_master, tmpbuf, sizeof(cfg->dnsservers_master) - 1);
		}
		
		memset(tmpbuf, 0, sizeof(tmpbuf));
		if ( cfg_get_object_attr(path, "DNSServers_Slave", tmpbuf, sizeof(tmpbuf)) > 0 )
		{
			strncpy(cfg->dnsservers_slave, tmpbuf, sizeof(cfg->dnsservers_slave) - 1);
		}
		
		memset(tmpbuf, 0, sizeof(tmpbuf));
		if ( cfg_get_object_attr(path, "DefaultGateway", tmpbuf, sizeof(tmpbuf)) > 0 )
		{
			strncpy(cfg->default_gateway, tmpbuf, sizeof(cfg->default_gateway) - 1);
		}
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	cfg_get_object_attr(path, "VLANEnable", tmpbuf, sizeof(tmpbuf));
	if ( 0 == strcmp(tmpbuf, "1") )
	{
		cfg->vlan_enable = SVC_WAN_RELATED_VXLAN_VLAN_ENABLE;
	}
	else
	{
		cfg->vlan_enable = SVC_WAN_RELATED_VXLAN_VLAN_DISABLE;
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if ( cfg_get_object_attr(path, "VLAN", tmpbuf, sizeof(tmpbuf)) > 0 )
	{
		cfg->vlan = atoi(tmpbuf);
	}

	cfg->bind_lanbit = get_vxlan_port_binding_interface(path);
	
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if ( cfg_get_object_attr(path, "WanIFName", tmpbuf, sizeof(tmpbuf)) > 0 )
	{
		strncpy(cfg->bind_wanitf, tmpbuf, sizeof(cfg->bind_wanitf) - 1);
	}

	return 0;
}
static int svc_wan_related_update_vxlan_obj(vxlan_entry_obj_t* obj)
{
	vxlan_entry_cfg_t tmp;
	int action = 0;

	svc_wan_related_load_vxlan_cfg(obj->path, &tmp, obj);

	/* tmp :current,        obj: mgr save */
	if ( obj->cfg.enable == SVC_WAN_RELATED_VXLAN_DISABLE && tmp.enable == SVC_WAN_RELATED_VXLAN_ENABLE )
	{
		action = E_VXLAN_LINK_DISABLE_2_ENABLE;
	}
	else if( obj->cfg.enable == SVC_WAN_RELATED_VXLAN_ENABLE && tmp.enable == SVC_WAN_RELATED_VXLAN_DISABLE )
	{
		action = E_VXLAN_LINK_ENABLE_2_DISABLE;
	}
	else if ( obj->cfg.enable == SVC_WAN_RELATED_VXLAN_ENABLE && tmp.enable == SVC_WAN_RELATED_VXLAN_ENABLE )
	{
		action = E_VXLAN_LINK_ENABLE_2_ENABLE;
	}
	else 
	{
		return 0;
	}

	if ( E_VXLAN_LINK_DISABLE_2_ENABLE == action )
	{
		memcpy(&obj->cfg, &tmp, sizeof(vxlan_entry_cfg_t));
		svc_wan_related_start_vxlan(obj);
	}
	else if( E_VXLAN_LINK_ENABLE_2_DISABLE == action )
	{
		svc_wan_related_stop_vxlan(obj);
		memcpy(&obj->cfg, &tmp, sizeof(vxlan_entry_cfg_t));
	}
	else if ( E_VXLAN_LINK_ENABLE_2_ENABLE == action )
	{
		svc_wan_related_stop_vxlan(obj);
		memcpy(&obj->cfg, &tmp, sizeof(vxlan_entry_cfg_t));
		svc_wan_related_start_vxlan(obj);
	}

	return 0;
}

int svc_wan_related_start_all_vxlan(void)
{
	int i = 0;
	char cmd[256] = {0}, nodeName[32] = {0};

	/* clear skb mark if this packet come from binding port */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t mangle -A POSTROUTING -o vxlan+ -j MARK --set-mark 0x00/0xf0000000");
	system(cmd);

	for ( i = 0; i < MAX_VXLAN_NUM; i++ )
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VXLAN_ENTRY_NODE, i + 1);
		if ( cfg_query_object(nodeName, NULL, NULL) <= 0 )
			continue;

		svc_wan_related_vxlan_update(nodeName);
	}

	

	return 0;
}

int svc_wan_related_stop_all_vxlan(void)
{
	vxlan_entry_obj_t* tmp = NULL;
	vxlan_entry_obj_t* obj = NULL;

	obj = svc_wan_related_get_vxlan_obj_list();

	while ( NULL != obj )
	{
		tmp = obj;
		obj = obj->next;
		svc_wan_related_stop_vxlan(tmp);
		svc_wan_related_del_vxlan_obj_from_list(tmp);
	}

	svc_wan_related_init_vxlan_obj_list();

	return 0;
}


int svc_wan_related_vxlan_xpon_handle(int evt)
{
	switch ( evt )
	{
		case E_VXLAN_XPON_UP:
		case E_VXLAN_XPON_DOWN:
		{
			send_signal_vxlan_dhcp(evt);
			break;
		}
		default:
		{
			break;
		}
	}
	return 0;
}


int svc_wan_related_vxlan_update(char *path)
{
	vxlan_entry_obj_t* obj = NULL;
	int entry_index = 0;

	if ( cfg_query_object(path, NULL, NULL) <= 0 )
	{
		tcdbg_printf("svc_wan_related_vxlan_update: cfg query [%s] fail  \n", path);
		return 0;
	}

	if ( NULL == ( obj = svc_wan_related_find_vxlan_obj(path) ) )
	{
		if ( 0 != get_entry_number_cfg2(path, "entry.", &entry_index) )
			return -1;
		
		if ( NULL == ( obj = svc_wan_related_create_vxlan_obj(path) ) )
		{
			return -1;
		}

		strncpy(obj->path, path, sizeof(obj->path) - 1);
		snprintf(obj->vxlan_dev, sizeof(obj->vxlan_dev), "vxlan%d", entry_index - 1);
		obj->idx = entry_index - 1;
		obj->dhcp_release = 0;
	}

	if ( svc_wan_related_update_vxlan_obj(obj) < 0 )
	{
		tcdbg_printf("svc_wan_related_vxlan_update: [%s] update fail \n", path);
		return -1;
	}
	
	return 0;
}

int svc_wan_related_vxlan_delete(char *path)
{
	vxlan_entry_obj_t* obj = NULL;

	if ( NULL == ( obj = svc_wan_related_find_vxlan_obj(path) ) )
	{
		return -1;
	}

	svc_wan_related_stop_vxlan(obj);

	svc_wan_related_del_vxlan_obj_from_list(obj);
	
	return 0;
}

/*
	-1/0 :  not right get info
	> 0:     right get info
*/
int svc_wan_related_get_file_string(char *file_path, char line_return[][32], int line_num)
{
	int i = 0;
	FILE* fp = NULL;
	char line_buf[32];

	if( (NULL== file_path) || (line_num < 0))
	{
		return -1;
	}

	fp = fopen(file_path, "r");
	if ( NULL == fp )
	{
		return -1;
	}

	memset(line_buf, 0, sizeof(line_buf));
	while( (NULL != fgets(line_buf, sizeof(line_buf) - 1, fp)) && (0 < line_num))
	{
		if(line_buf[0] != '\n')
		{
			/*replace "\n" by NULL*/
			strtok(line_buf, "\n");
			strcpy(line_return[i], line_buf);
		}
		else
		{
			strcpy(line_return[i], "");
		}

		line_num--;
		i++;
		memset(line_buf, 0, sizeof(line_buf));
	}
	fclose(fp);

	return i;
}


int svc_wan_related_set_vxlan_info(char* node, char* path)
{
	char info_string[2][32];
	char pathtmp[128] = {0};
	int num;

	memset(pathtmp, 0, sizeof(pathtmp));
	snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "ip");

	cfg_set_object_attr(node,"Status", "up");

	if ( svc_wan_related_get_file_string(pathtmp, info_string, 1) > 0 )
	{
		cfg_set_object_attr(node, "IP", info_string[0]);
	}

	memset(pathtmp, 0, sizeof(pathtmp));
	snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "netmask");
	if ( svc_wan_related_get_file_string(pathtmp, info_string, 1) > 0 )
	{
		cfg_set_object_attr(node, "NetMask", info_string[0]);
	}

	memset(pathtmp, 0, sizeof(pathtmp));
	snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "dns");
	num = svc_wan_related_get_file_string(pathtmp, info_string, 2);
	if ( num > 0 )
	{
		cfg_set_object_attr(node, "DNS", info_string[0]);
	}
	if ( num > 1 )
	{
		cfg_set_object_attr(node, "SecDNS", info_string[1]);
	}

	memset(pathtmp, 0, sizeof(pathtmp));
	snprintf(pathtmp, sizeof(pathtmp), "%s/%s", path, "gateway");
	if ( svc_wan_related_get_file_string(pathtmp, info_string, 1) > 0 )
	{
		cfg_set_object_attr(node, "GateWay", info_string[0]);
	}

	return 0;
}


int svc_wan_related_set_info(vxlan_entry_obj_t* obj, int evt)
{
	char fpath[32];
	char node[32];
	char* empty = "";

	memset(node, 0, sizeof(node));
	snprintf(node, sizeof(node), VXLAN_IPENTRY_NODE, obj->idx + 1);

	if ( cfg_query_object(node, NULL, NULL) <= 0 )
	{
		cfg_create_object(node);
	}

	memset(fpath, 0, sizeof(fpath));
	snprintf(fpath, sizeof(fpath), "/var/run/%s", obj->vxlan_dev);
	switch(evt)
	{
		case EVT_CFG_WAN_RELATED_VXLAN_GET4:
		{
			svc_wan_related_set_vxlan_info(node, fpath);
			break;
		}
		case EVT_CFG_WAN_RELATED_VXLAN_LOST4:
		{
			cfg_set_object_attr(node, "Status", "down");
			cfg_set_object_attr(node, "IP", empty);
			cfg_set_object_attr(node, "NetMask", empty);
			cfg_set_object_attr(node, "DNS", empty);
			cfg_set_object_attr(node, "SecDNS", empty);
			cfg_set_object_attr(node, "GateWay", empty);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

void vxlan_dealwith_link_local_route(vxlan_entry_obj_t* obj, int opt)
{
	char gate_way_v4[16];
	char net_mask_v4[16];
	char sub_ip_v4[16];
	char node[32];
	char cmd[256];
	struct in_addr gatewayIP;
	struct in_addr netmask;
	struct in_addr subIP;

	memset(node, 0, sizeof(node));
	snprintf(node, sizeof(node), VXLAN_IPENTRY_NODE, obj->idx + 1);
	memset(gate_way_v4, 0, sizeof(gate_way_v4));
	memset(net_mask_v4, 0, sizeof(net_mask_v4));
	if ( cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) < 0 )
	{
		return ;
	}

	if ( cfg_get_object_attr(node, "NetMask", net_mask_v4, sizeof(net_mask_v4)) < 0 )
	{
		return ;
	}

	memset(sub_ip_v4, 0, sizeof(sub_ip_v4));
	inet_aton(gate_way_v4, &gatewayIP);
	inet_aton(net_mask_v4, &netmask);
	subIP.s_addr = (gatewayIP.s_addr & netmask.s_addr);
	strncpy(sub_ip_v4, inet_ntoa(subIP), sizeof(sub_ip_v4) - 1);

	if ( E_ADD_LINK_ROUTE == opt )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/route add -net %s netmask %s dev %s", sub_ip_v4, net_mask_v4, obj->vxlan_dev);
		system(cmd);
	}
	else
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/route del -net %s netmask %s dev %s", sub_ip_v4, net_mask_v4, obj->vxlan_dev);
		system(cmd);
	}

	return ;
}

void set_vxlan_policy_route(vxlan_entry_obj_t* obj, int evt)
{
	char cmd[256], node[32];
	char ip_v4[16], netmask_v4[16], gate_way_v4[16];
	char w_ip_v4[16], ip_netmask_v4[16];
	char mask_dec[8];
	unsigned int mark;
	struct in_addr ip_v4_addr;
	struct in_addr netmask_v4_addr;

	if ( NULL == obj )
	{
		return ;
	}

	mark = (obj->idx + MAX_WAN_ITF_NUM + 1) << 16; 
	switch (evt) 
	{
		case EVT_CFG_WAN_RELATED_VXLAN_GET4:
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t mangle -D OUTPUT -o %s -j MARK --set-mark 0x%x/0x7f0000", obj->vxlan_dev, mark);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t mangle -A OUTPUT -o %s -j MARK --set-mark 0x%x/0x7f0000", obj->vxlan_dev, mark);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x7f0000 table %d", mark, 100 + MAX_WAN_ITF_NUM + obj->idx);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add fwmark 0x%x/0x7f0000 table %d", mark, 100 + MAX_WAN_ITF_NUM + obj->idx);
			system(cmd);

			memset(node, 0, sizeof(node));
			snprintf(node, sizeof(node), VXLAN_IPENTRY_NODE, obj->idx + 1);
			if ( cfg_get_object_attr(node, "GateWay", gate_way_v4, sizeof(gate_way_v4)) > 0 )
			{
				vxlan_dealwith_link_local_route(obj, E_ADD_LINK_ROUTE);
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add default via %s dev %s table %d", 
					gate_way_v4, obj->vxlan_dev, 100 + MAX_WAN_ITF_NUM + obj->idx);
				system(cmd);
				vxlan_dealwith_link_local_route(obj, E_DEL_LINK_ROUTE);
			}

			memset(node, 0, sizeof(node));
			snprintf(node, sizeof(node), VXLAN_IPENTRY_NODE, obj->idx + 1);

			memset(ip_v4, 0, sizeof(ip_v4));
			memset(netmask_v4, 0, sizeof(netmask_v4));
			if ( ( cfg_get_object_attr(node, "IP", ip_v4, sizeof(ip_v4)) > 0 ) 
			&& ( cfg_get_object_attr(node, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0 ) )
			{
				if ( (strlen(netmask_v4) > 0) && (strlen(ip_v4) > 0) )
				{
					if ( !check_mask_format(netmask_v4, mask_dec, sizeof(mask_dec)) )
					{
						return ;
					}
					inet_aton(ip_v4, &ip_v4_addr);
					inet_aton(netmask_v4, &netmask_v4_addr);
					ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);

					memset(ip_netmask_v4, 0, sizeof(ip_netmask_v4));
					snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));

					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd,  sizeof(cmd), "/usr/bin/ip route add %s/%s dev %s table %d",
						ip_netmask_v4, mask_dec, obj->vxlan_dev, 100 + MAX_WAN_ITF_NUM + obj->idx);
					system(cmd);
				}
			}

			memset(node, 0, sizeof(node));
			snprintf(node, sizeof(node), VXLAN_IPENTRY_NODE, obj->idx + 1);
			memset(w_ip_v4, 0, sizeof(w_ip_v4));
			if ( cfg_get_object_attr(node, "IP", w_ip_v4, sizeof(w_ip_v4)) > 0 )
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule add from %s/32 table %d", w_ip_v4, 100 + MAX_WAN_ITF_NUM + obj->idx);
				system(cmd);
				memset(node, 0, sizeof(node));
				snprintf(node, sizeof(node), "%s", LAN_ENTRY0_NODE);

				memset(ip_v4, 0, sizeof(ip_v4));
				memset(netmask_v4, 0, sizeof(netmask_v4));
				if ( ( cfg_get_object_attr(node, "IP", ip_v4, sizeof(ip_v4)) > 0 )
				&& ( cfg_get_object_attr(node, "netmask", netmask_v4, sizeof(netmask_v4)) > 0 ) )
				{
					if ( (strlen(ip_v4) > 0) && (strlen(netmask_v4) > 0) )
					{
						if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
						{
							snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
						}
						inet_aton(ip_v4, &ip_v4_addr);
						inet_aton(netmask_v4, &netmask_v4_addr);
						ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);
						snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add %s/%s via %s dev br0 table %d",
							ip_netmask_v4, mask_dec, ip_v4, 100 + MAX_WAN_ITF_NUM + obj->idx);
					}
				}
				else
				{
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "/usr/bin/ip route add 192.168.1.0/24 via 192.168.1.1 dev br0 table %d",
						100 + MAX_WAN_ITF_NUM + obj->idx);
				}

				system(cmd);
				memcpy(obj->ip_last, w_ip_v4, sizeof(obj->ip_last));
			}

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush cache 2>/dev/null");
			system(cmd);
			break;
		}
		case EVT_CFG_WAN_RELATED_VXLAN_LOST4:
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t mangle -D OUTPUT -o %s -j MARK --set-mark 0x%x/0x7f0000", obj->vxlan_dev, mark);
			system(cmd);

			memset(ip_v4, 0, sizeof(ip_v4));
			memcpy(ip_v4, obj->ip_last, sizeof(ip_v4));
			if ( strlen(ip_v4 ) > 0 )
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del from %s/32 table %d", ip_v4, 100 + MAX_WAN_ITF_NUM + obj->idx);
				system(cmd);
			}
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip rule del fwmark 0x%x/0x7f0000 table %d", mark, 100 + MAX_WAN_ITF_NUM + obj->idx);
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/usr/bin/ip route flush table %d", 100 + MAX_WAN_ITF_NUM + obj->idx);
			system(cmd);
			break;
		}
		default:
		{
			break;
		}
	}

	return;
}

void svc_wan_related_vxlan_binding_wanif(char* wanif, int opt)
{
	vxlan_entry_obj_t* obj = NULL;
	char cmd[256] = {0};

	if ( NULL == ( obj = svc_wan_related_find_vxlan_obj_by_bind_wanif(wanif) ) )
	{
		return;
	}

	if ( E_VXLAN_WAN_UP == opt )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/route add -net %s netmask 255.255.255.255 dev %s", 
			obj->cfg.remote_ip, wanif);
		system(cmd);
	}
	else
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/sbin/route del -net %s netmask 255.255.255.255 dev %s", 
			obj->cfg.remote_ip, wanif);
		system(cmd);
	}
	return;
}


int svc_wan_related_handle_vxlan_evt(vxlan_entry_obj_t* obj, int evt)
{
	char cmd[256] = {0};
	
	switch (evt)
	{
		case EVT_CFG_WAN_RELATED_VXLAN_GET4:
		{
			if ( obj->cfg.nat_enabled )
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/usr/bin/iptables -t nat -A POSTROUTING -o %s -j MASQUERADE", obj->vxlan_dev); 
				system(cmd);
			}
			
#ifdef TCSUPPORT_PORTBIND
			set_vxlan_port_binding_route(obj);
#endif
			set_vxlan_policy_route(obj, evt);
			break;
		}
		case EVT_CFG_WAN_RELATED_VXLAN_LOST4:
		{
			if ( obj->cfg.nat_enabled)
			{
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd,sizeof(cmd), "/usr/bin/iptables -t nat -D POSTROUTING -p all -o %s -j MASQUERADE", obj->vxlan_dev);
				system(cmd);
			}
#ifdef TCSUPPORT_PORTBIND
			set_vxlan_port_binding_route(obj);
#endif
			set_vxlan_policy_route(obj, evt);
			break;
		}

		default:
		{
			break;
		}
	}

	return 0;
}


int svc_wan_related_vxlan_get4(char* dev)
{
	vxlan_entry_obj_t* obj = NULL;

	if ( NULL == (obj = svc_wan_related_find_vxlan_obj_by_dev(dev)) )
	{
		return -1;
	}

	if ( obj->flag & VXLAN_FLAG_UPV4 )
	{
		return 0;
	}

	obj->flag |= VXLAN_FLAG_UPV4;

	svc_wan_related_set_info(obj, EVT_CFG_WAN_RELATED_VXLAN_GET4);

	svc_wan_related_handle_vxlan_evt(obj, EVT_CFG_WAN_RELATED_VXLAN_GET4);
	return 0;
}

int svc_wan_related_vxlan_lost4(char* dev)
{
	vxlan_entry_obj_t* obj = NULL;

	if ( NULL == (obj = svc_wan_related_find_vxlan_obj_by_dev(dev)) )
	{
		return -1;
	}

	if ( 0 == (obj->flag & VXLAN_FLAG_UPV4) )
	{
		return 0;
	}

	obj->flag &= (~VXLAN_FLAG_UPV4);

	svc_wan_related_set_info(obj, EVT_CFG_WAN_RELATED_VXLAN_LOST4);

	svc_wan_related_handle_vxlan_evt(obj, EVT_CFG_WAN_RELATED_VXLAN_LOST4);

	return 0;
}


