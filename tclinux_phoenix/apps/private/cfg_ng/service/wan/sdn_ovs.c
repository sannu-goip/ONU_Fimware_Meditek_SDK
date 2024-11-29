#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/msg.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <cfg_api.h>
#include "cfg_msg.h"
#include "sdn_ovs.h"
#include "utility.h"
#include "wan_cfg.h"

#define VETH_PEER_NAME "vlan"
#define WAN_INTERNAL_NAME "ovs_wan"
#define VPN_INTERNAL_NAME "ovs_vpn"
#define VETH_PEER_PORT_BASE 100
#define WAN_PORT_BASE 21
#define OPENVSWITCH_PATH "/tmp/openvswitch/run/"
#define OVS_PATH "/tmp/var/ovs/"
#define DB_PATH "/tmp/test/db/"
#define OVSSCHEMA_PATH "/usr/ovs/share/openvswitch/vswitch.ovsschema"
#define CTL_PATH "/tmp/var/ovs/"

#define OVS_VSCTL "/usr/ovs/bin/ovs-vsctl"
#define OVSDB_TOOL "/usr/ovs/bin/ovsdb-tool"
#define OVSDB_SERVER "/usr/ovs/sbin/ovsdb-server"
#define OVS_VSWITCHED "/usr/ovs/sbin/ovs-vswitchd"
#if defined(TCSUPPORT_CMCCV2)
#define VPN_INSTANCE_NUM	1
#else
#define VPN_INSTANCE_NUM	16
#endif

struct sdnlan_map
{
    char *bindif;
    char *realif;
    char *rename;
    char  state;
};
struct sdnwan_map
{
    char *bindif;
	char entryid;
    char  state;
	
};


struct sdnlan_map sdn_map_lan[] = 
{
    {"LAN1",        "eth0.1",	"eth0.1_",  0},
    {"LAN2",        "eth0.2",   "eth0.2_",	0},
    {"LAN3",        "eth0.3",   "eth0.3_",	0},
    {"LAN4",        "eth0.4",   "eth0.4_",	0},
    {"WIFI_24_1",   "ra0",		"ra0_",     0},
    {"WIFI_24_2",   "ra1", 		"ra1_",     0},
    {"WIFI_24_3",   "ra2", 		"ra2_",     0},
    {"WIFI_24_4",   "ra3", 		"ra3_",     0},
    {"WIFI_24_5",   "ra4", 		"ra4_",    	0},
    {"WIFI_24_6",   "ra5",  	"ra5_",    	0},
    {"WIFI_24_7",   "ra6", 		"ra6_",     0},
    {"WIFI_24_8",   "ra7",  	"ra7_",    	0},
    {"WIFI_5_1",    "rai0", 	"rai0_",   	0},
    {"WIFI_5_2",    "rai1", 	"rai1_",    0},
    {"WIFI_5_3",    "rai2", 	"rai2_",   	0},
    {"WIFI_5_4",    "rai3", 	"rai3_",   	0},
    {"WIFI_5_5",    "rai4", 	"rai4_",  	0},
    {"WIFI_5_6",    "rai5", 	"rai5_",   	0},
    {"WIFI_5_7",    "rai6", 	"rai6_",  	0},
    {"WIFI_5_8",    "rai7", 	"rai7_",   	0},
};

struct sdnwan_map sdn_map_wan[] = 
{
    {"WAN1",        -1,         0},
    {"WAN2",        -1,         0},
    {"WAN3",        -1,         0},
    {"WAN4",        -1,         0}, 
	{"WAN5",        -1,         0},
    {"WAN6",        -1,         0},
    {"WAN7",        -1,         0},
    {"WAN8",        -1,         0}, 
};
struct sdnwan_map sdn_map_vpn[] = 
{
    {"VPN1",        -1,         0},
    {"VPN2",        -1,         0},
    {"VPN3",        -1,         0},
    {"VPN4",        -1,         0}, 
    {"VPN5",        -1,         0},
    {"VPN6",        -1,         0},
    {"VPN7",        -1,         0},
    {"VPN8",        -1,         0}, 
};
char preContlIP[32];
void dump_sdn_info(char *brname)
{
	char tmp[64] = {0};
	int i =0;

	/*SDN LAN*/
    for(i = 0; i < sizeof(sdn_map_lan) / sizeof(sdn_map_lan[0]); i++)
    {
        memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_lan[i].bindif, tmp, sizeof(tmp)) > 0 
            && brname[0] != '\0')
        {
			tcdbg_printf("sdn_map_lan[%d].bindif=%s state=%d\n",i,sdn_map_lan[i].bindif,sdn_map_lan[i].state);
        }
    }
	for(i = 0; i < sizeof(sdn_map_wan) / sizeof(sdn_map_wan[0]); i++)
	{
		memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_wan[i].bindif, tmp, sizeof(tmp)) > 0 
            && brname[0] != '\0')
        {
			tcdbg_printf("sdn_map_wan[%d].bindif=%s entry_index=%d state=%d\n",
				i,sdn_map_wan[i].bindif,sdn_map_wan[i].entryid,sdn_map_wan[i].state);
        }
	}
	for(i = 0; i < sizeof(sdn_map_vpn) / sizeof(sdn_map_vpn[0]); i++)
	{
		memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_vpn[i].bindif, tmp, sizeof(tmp)) > 0 
            && brname[0] != '\0')
        {
			tcdbg_printf("sdn_map_vpn[%d].bindif=%s entry_index=%d state=%d\n",
				i,sdn_map_vpn[i].bindif,sdn_map_vpn[i].entryid,sdn_map_vpn[i].state);
        }
	}
}

int get_portid_by_wan_index(unsigned int idx)
{
	int i = 0;

	for(i = 0; i < sizeof(sdn_map_wan) / sizeof(sdn_map_wan[0]); i++)
	{
		if(sdn_map_wan[i].entryid == idx)
		{
			return i;
		}
	}
	return -1;
}

int enable_ovs_bridge(int enable,char*brname)
{
	char cmd[256] = {0},pid[8] = {0},tmp[64] = {0},ctrlip[32] = {0};
	char wanNodeName[64] = {0};
	int pvc_index,entry_index;
	int i = 0,ret = 0;
	
	if(enable == 1)
	{
		memset(pid,0,sizeof(pid));
        fileRead(CTL_PATH"ovs-vswitchd.pid", pid, sizeof(pid));	
		/*if(pid[0] != '\0')
		{
			kill(atoi(pid), SIGTERM);
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/killall ovsdb-server");
            system(cmd);
			system("rm -rf "OPENVSWITCH_PATH);
		}*/

	    if(pid[0] == '\0')
        {
    		memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/sbin/insmod /lib/modules/openvswitch.ko");
            system(cmd);

            system("/bin/mkdir -p "OPENVSWITCH_PATH);
			system("/bin/mkdir -p "OVS_PATH);
			system("/bin/mkdir -p "DB_PATH);
			system(OVSDB_TOOL" create "DB_PATH"conf.db "OVSSCHEMA_PATH);

            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),OVSDB_SERVER" --unixctl="CTL_PATH"ovsdb-server.ctl --remote=punix:"OPENVSWITCH_PATH"db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --detach "DB_PATH"conf.db");
            system(cmd);

			memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),OVS_VSWITCHED" --unixctl="CTL_PATH"ovs-vswitchd.ctl unix:"OPENVSWITCH_PATH"db.sock --pidfile="CTL_PATH"ovs-vswitchd.pid --detach");
            system(cmd);
			sleep(1);
			
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),OVS_VSCTL" --db=unix:"OPENVSWITCH_PATH"db.sock --no-wait init");
            system(cmd);

            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),OVS_VSCTL" add-br %s",brname);
            system(cmd);

			memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),OVS_VSCTL" set bridge %s stp_enable=true",brname);
            system(cmd);
			ret = 1;
		}
			
		memset(ctrlip,0, sizeof(ctrlip));
		cfg_get_object_attr(SDN_COMMON_NODE, "CtrlIp", ctrlip, sizeof(ctrlip));
		if(ctrlip[0] != '\0' && check_ip_format(ctrlip) && strncmp(ctrlip,preContlIP,sizeof(ctrlip)-1) 
			&& brname[0] != '\0')
		{
			strncpy(preContlIP,ctrlip,sizeof(preContlIP)-1);
	        memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),OVS_VSCTL" set-controller %s tcp:%s:6633",brname,ctrlip);
	        system(cmd);
	        ret = 1;
		}
	}
	else
	{
		memset(pid,0,sizeof(pid));
        fileRead(CTL_PATH"ovs-vswitchd.pid", pid, sizeof(pid));
        if(strlen(pid) > 0 && brname[0] != '\0')
        {
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-br %s",brname);
            system(cmd);

            for(i = 0; i < sizeof(sdn_map_lan) / sizeof(sdn_map_lan[0]); i++)
            {
                memset(tmp, 0, sizeof(tmp));
                if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_lan[i].bindif, tmp, sizeof(tmp)) > 0)
                {
                    if(tmp[0] == '1')
                    {
                        memset(cmd,0,sizeof(cmd));
                        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link del %s type veth peer name "VETH_PEER_NAME"%d",
                            sdn_map_lan[i].rename,i+1);
                        system(cmd);

                        memset(cmd,0,sizeof(cmd));
                        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br0 %s",sdn_map_lan[i].realif);
                        system(cmd);

                    }
					cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_lan[i].bindif,"0");
					sdn_map_lan[i].state =0;
                }
            }
			for(i = 0; i < sizeof(sdn_map_wan) / sizeof(sdn_map_wan[0]); i++)
			{
				memset(tmp, 0, sizeof(tmp));
                if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_wan[i].bindif, tmp, sizeof(tmp)) > 0)
                {
                    if(tmp[0] == '1')
                    {
                        memset(tmp, 0, sizeof(tmp));
		                if(sdn_map_wan[i].entryid >= 0)
		                {
							pvc_index = sdn_map_wan[i].entryid / MAX_SMUX_NUM + 1;
							entry_index = sdn_map_wan[i].entryid % MAX_SMUX_NUM + 1;
							memset(wanNodeName,0,sizeof(wanNodeName));
							snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
							cfg_set_object_attr(wanNodeName,"sdn","");
							cfg_commit_object(wanNodeName);
							
	                        memset(cmd,0,sizeof(cmd));
					        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s "WAN_INTERNAL_NAME"%d",
					            brname,sdn_map_wan[i].entryid);
					        system(cmd);
							memset(cmd,0,sizeof(cmd));
					        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del iif "WAN_INTERNAL_NAME"%d",sdn_map_wan[i].entryid);
					        system(cmd); 
		                }
						cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_wan[i].bindif,"0");
						sdn_map_wan[i].state =0;
                    }
                }
			}
			for(i = 0; i < sizeof(sdn_map_vpn) / sizeof(sdn_map_vpn[0]); i++)
			{
				memset(tmp, 0, sizeof(tmp));
                if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_vpn[i].bindif, tmp, sizeof(tmp)) > 0)
                {
                    if(tmp[0] == '1')
                    {
                        memset(cmd,0,sizeof(cmd));
				        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s "VPN_INTERNAL_NAME"%d",
				            brname,i);
				        system(cmd);
						memset(cmd,0,sizeof(cmd));
				        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del iif "VPN_INTERNAL_NAME"%d",i);
				        system(cmd);
                    }
					cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_vpn[i].bindif,"0");
					sdn_map_vpn[i].state =0;
                }
			}
			kill(atoi(pid), SIGTERM);
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/killall ovsdb-server");
            system(cmd);

            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/bin/rm -rf "OPENVSWITCH_PATH);
            system(cmd);
			memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/bin/rm -rf "CTL_PATH);
            system(cmd);

        }
	}
	return ret;
}
void add_remove_from_ovs(int state,int lan_index,char* brname)
{
	char cmd[256] = {0};

	if(brname == NULL || lan_index < 0)
	{
		return;	
	}
	if(state == 1 )
	{
		/*add interface into ovs bridge*/
		memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl delif br0 %s",sdn_map_lan[lan_index].realif);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link add %s type veth peer name "VETH_PEER_NAME"%d",
            sdn_map_lan[lan_index].rename,lan_index+1);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/sbin/ifconfig "VETH_PEER_NAME"%d up",lan_index+1);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/sbin/ifconfig %s up",sdn_map_lan[lan_index].rename);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br0 %s",sdn_map_lan[lan_index].rename);
        system(cmd);
	
	    memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),OVS_VSCTL" add-port %s "VETH_PEER_NAME"%d -- set Interface "VETH_PEER_NAME"%d ofport_request=%d",
            brname,lan_index+1,lan_index+1,lan_index+1+VETH_PEER_PORT_BASE);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),OVS_VSCTL" add-port %s %s -- set Interface %s ofport_request=%d",
            brname,sdn_map_lan[lan_index].realif,sdn_map_lan[lan_index].realif,lan_index+1);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),OVS_VSCTL" set Port %s other_config:mcast-snooping-flood-reports=true",
            sdn_map_lan[lan_index].realif);
        system(cmd);

	}
	else
	{
		/*remove interface from ovs bridge*/
		memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s "VETH_PEER_NAME"%d",
            brname,lan_index+1);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s %s",
            brname,sdn_map_lan[lan_index].realif);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link del %s type veth peer name "VETH_PEER_NAME"%d",
            sdn_map_lan[lan_index].rename,lan_index+1);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br0 %s",sdn_map_lan[lan_index].realif);
        system(cmd);

	}
}
void add_remove_wan_from_ovs(int state,int wan_index,int wan_port,char* brname)
{
	char cmd[256] = {0},mactemp[64]={0};
	char wanNodeName[64] = {0};
    char ifname[16],nasname[16],wanmode[8];
	int pvc_index,entry_index;
	
	if(brname == NULL || wan_index < 0)
	{
		return;	
	}

	pvc_index = wan_index / MAX_SMUX_NUM + 1;
	entry_index = wan_index % MAX_SMUX_NUM + 1;
	memset(wanNodeName,0,sizeof(wanNodeName));
	snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
	memset(ifname,0,sizeof(ifname));
	if(cfg_get_object_attr(wanNodeName, "IFName", ifname, sizeof(ifname)) <= 0 )
	{
	 	tcdbg_printf("%s:%d can not get %s ifname!\n",__FUNCTION__,__LINE__,wanNodeName);
	 	return;
	}
	memset(nasname,0,sizeof(nasname));
	if(cfg_get_object_attr(wanNodeName, "NASName", nasname, sizeof(nasname)) <= 0 )
	{
	 	tcdbg_printf("%s:%d can not get %s ifname!\n",__FUNCTION__,__LINE__,wanNodeName);
	 	return;
	}
	memset(wanmode,0,sizeof(wanmode));
	if(cfg_get_object_attr(wanNodeName, "WanMode", wanmode, sizeof(wanmode)) <= 0 )
	{
	 	tcdbg_printf("%s:%d can not get %s wanmode!\n",__FUNCTION__,__LINE__,wanNodeName);
	 	return;
	}
	if(!strncmp(wanmode,"Route",sizeof(wanmode)-1))
	{
		if(state == 1)
		{
			
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),OVS_VSCTL" add-port %s "WAN_INTERNAL_NAME"%d -- set Interface "WAN_INTERNAL_NAME"%d type=internal ofport_request=%d",
	            brname,wan_index,wan_index,wan_port+21);
	        system(cmd);

			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),"/usr/bin/ip addr add 192.168.1.1/24 dev "WAN_INTERNAL_NAME"%d",wan_index);
	        system(cmd);

			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),"/sbin/ifconfig "WAN_INTERNAL_NAME"%d promisc up",wan_index);
	        system(cmd);
			
			memset(mactemp,0,sizeof(mactemp));
			if(get_profile_str("LAN_MAC=",mactemp,sizeof(mactemp),NO_QMARKS, "/etc/mac.conf") == -1)
			{
				tcdbg_printf("[%s %d] no mac file!!\n",__FUNCTION__,__LINE__);
	        	return;
			}
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),"/sbin/ifconfig "WAN_INTERNAL_NAME"%d hw ether %s",wan_index,mactemp);
	        system(cmd);
			
			
			cfg_set_object_attr(wanNodeName,SVC_WAN_ATTR_SDN,"1");
			
			cfg_commit_object(wanNodeName);
			//wan restart or not and default route change
				
		}
		else
		{
			cfg_set_object_attr(wanNodeName,SVC_WAN_ATTR_SDN,"");
			cfg_commit_object(wanNodeName);
			
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s "WAN_INTERNAL_NAME"%d",
	            brname,wan_index);
	        system(cmd);
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del iif "WAN_INTERNAL_NAME"%d",wan_index);
	        system(cmd);
		}
	}
	else/*Bridge*/
	{
		if(state == 1)
		{
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl delif br0 %s",nasname);
	        system(cmd);
			tcdbg_printf("%s:%d cmd=%s\n",__FUNCTION__,__LINE__,cmd);
			
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),OVS_VSCTL" add-port %s %s -- set Interface %s ofport_request=%d",
	            brname,nasname,nasname,wan_port+21);
	        system(cmd);
		}
		else
		{
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s %s",brname,nasname);
	        system(cmd);
			memset(cmd,0,sizeof(cmd));
	        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br0 %s",nasname);
	        system(cmd);
		}
		
	}


	
	
}

void update_wanindex_info()
 {
 	char wanNodeName[MAXLEN_NODE_NAME] = {0},active[8]= {0},ifname[16] = {0},tmp[32]={0};
	char nodeName[MAXLEN_NODE_NAME] = {0},active1[8] = {0};
 	int first_unbind=-1,match =0, i = 0, j = 0,k =0,pvc_index,entry_index;
	for(i=0; i < PVC_NUM ; i++)
	{
		for(j=0; j < MAX_SMUX_NUM ; j++)
		{
			memset(wanNodeName, 0, sizeof(wanNodeName));
	    	snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, i+1,j+1);
			memset(active, 0, sizeof(active));
			if(cfg_get_object_attr(wanNodeName, "Active", active, sizeof(active)) > 0 
				&& !strncmp(active,"Yes",sizeof(active)-1))
			{
				for(k = 0; k < sizeof(sdn_map_wan) / sizeof(sdn_map_wan[0]); k++)
				{
					if(sdn_map_wan[k].entryid != -1)
					{
						if(sdn_map_wan[k].entryid == (i*PVC_NUM + j))
						{
							match = 1;
							memset(nodeName, 0, sizeof(nodeName));
			    			snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE,sdn_map_wan[k].entryid+1);
							memset(tmp,0,sizeof(tmp));
							if(cfg_get_object_attr(nodeName, "GUIInterfaceName", tmp, sizeof(tmp)) > 0)
							{
								snprintf(ifname,sizeof(ifname),"WAN%d_IF",k+1);
								cfg_set_object_attr(SDN_COMMON_NODE,ifname,tmp);
							}
						}
						else
						{
							memset(nodeName, 0, sizeof(nodeName));
	    					snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, (sdn_map_wan[k].entryid/PVC_NUM) +1,(sdn_map_wan[k].entryid%PVC_NUM)+1);
							memset(active1, 0, sizeof(active1));
							if(cfg_get_object_attr(nodeName, "Active", active1, sizeof(active1)) <= 0 
								|| strncmp(active1,"Yes",sizeof(active1)-1))
							{
								sdn_map_wan[k].entryid = -1;
								sdn_map_wan[k].state = 0;
								cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_wan[k].bindif,"");
							}
							else
							{
								memset(nodeName, 0, sizeof(nodeName));
				    			snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE,sdn_map_wan[k].entryid+1);
								memset(tmp,0,sizeof(tmp));
								if(cfg_get_object_attr(nodeName, "GUIInterfaceName", tmp, sizeof(tmp)) > 0)
								{
									snprintf(ifname,sizeof(ifname),"WAN%d_IF",k+1);
									cfg_set_object_attr(SDN_COMMON_NODE,ifname,tmp);
								}
							}
						}
					}
					else
					{
						if(first_unbind < 0)
						{
							first_unbind=k;
						}
					}
				}
				if(match == 0 && first_unbind >= 0)
				{
					tcdbg_printf("%s:%d index=%d first_unbind=%d\n",__FUNCTION__,__LINE__,i*PVC_NUM+j,first_unbind);
					sdn_map_wan[first_unbind].entryid= i*PVC_NUM+j;
					memset(nodeName, 0, sizeof(nodeName));
	    			snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE,i*PVC_NUM+j+1);
					memset(tmp,0,sizeof(tmp));
					if(cfg_get_object_attr(nodeName, "GUIInterfaceName", tmp, sizeof(tmp)) > 0)
					{
						snprintf(ifname,sizeof(ifname),"WAN%d_IF",first_unbind+1);
						cfg_set_object_attr(SDN_COMMON_NODE,ifname,tmp);
					}
				if(cfg_get_object_attr(wanNodeName, "sdn", tmp, sizeof(tmp)) > 0 && tmp[0] == '1')
				{
					cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_wan[first_unbind].bindif,"1");
				}
				else
				{
					cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_wan[first_unbind].bindif,"0");
				}
			}
		}
			first_unbind = -1;
			match = 0;
		}
	}
 }

void update_wan_sdn_info(char *brname)
{
	int  k = 0;
	char tmp[64] = {0};

	update_wanindex_info();
	for(k = 0; k < sizeof(sdn_map_wan) / sizeof(sdn_map_wan[0]); k++)
    {
    	memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_wan[k].bindif, tmp, sizeof(tmp)) > 0 
			&& sdn_map_wan[k].state != atoi(tmp))
        {
			if(tmp[0] == '1')
			{
				add_remove_wan_from_ovs(1,sdn_map_wan[k].entryid,k,brname); 
			}
			else
			{
		    	add_remove_wan_from_ovs(0,sdn_map_wan[k].entryid,k,brname); 
			}
			sdn_map_wan[k].state=atoi(tmp);
        }
	}
}
 
 void update_vpn_info(char *brname)
 {
 	char nodeName[MAXLEN_NODE_NAME] = {0},mactemp[64]={0},tmp[64] = {0},gateway[16]={0};
	char cmd[256] = {0},serverIpAddr[32]= {0},username[64] = {0},password[64] = {0},ifname[16] = {0};
	char tmpNode[MAXLEN_NODE_NAME] = {0},serverIpAddr1[32]= {0},username1[64] = {0},password1[64] = {0};
 	int first_unbind=-1,match =0, i = 0, j = 0;
	
	for(i=0; i < VPN_INSTANCE_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
    	snprintf(nodeName, sizeof(nodeName), VPN_ENTRY_NODE, i+1);
		memset(serverIpAddr, 0, sizeof(serverIpAddr));
		memset(username, 0, sizeof(username));
		memset(password, 0, sizeof(password));
		if(cfg_get_object_attr(nodeName, "serverIpAddr", serverIpAddr, sizeof(serverIpAddr)) > 0 
			&& serverIpAddr[0] != '\0' 
			&& cfg_get_object_attr(nodeName, "username", username, sizeof(username)) > 0 
			&& username[0] != '\0'
			&& cfg_get_object_attr(nodeName, "password", password, sizeof(password)) > 0 
			&& password[0] != '\0')
		{
			for(j = 0; j < sizeof(sdn_map_vpn) / sizeof(sdn_map_vpn[0]); j++)
			{
				if(sdn_map_vpn[j].entryid != -1)
				{
					if(sdn_map_vpn[j].entryid == i)
					{
						match = 1;
					}
					else
					{
						memset(tmpNode, 0, sizeof(tmpNode));
    					snprintf(tmpNode, sizeof(tmpNode), VPN_ENTRY_NODE, sdn_map_vpn[j].entryid+1);
						memset(serverIpAddr1, 0, sizeof(serverIpAddr1));
						memset(username1, 0, sizeof(username1));
						memset(password1, 0, sizeof(password1));
						if(cfg_get_object_attr(tmpNode, "serverIpAddr", serverIpAddr, sizeof(serverIpAddr)) <= 0 
							|| serverIpAddr[0] == '\0' 
							|| cfg_get_object_attr(tmpNode, "username", username1, sizeof(username1)) <= 0 
							|| username[0] == '\0'
							|| cfg_get_object_attr(tmpNode, "password", password1, sizeof(password1)) <= 0 
							&& password[0] == '\0')
						{
							sdn_map_vpn[j].entryid = -1;
							sdn_map_vpn[j].state = 0;
							cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_vpn[j].bindif,"");
						}
					}
				}
				else
				{
					if(first_unbind < 0)
					{
						first_unbind=j;
					}
				}
			}
			if(match == 0 && first_unbind >= 0)
			{
				sdn_map_vpn[first_unbind].entryid= i;
				memset(tmp,0,sizeof(tmp));
				if(cfg_get_object_attr(nodeName, "tunnelName", tmp, sizeof(tmp)) > 0)
				{
					snprintf(ifname,sizeof(ifname),"VPN%d_IF",first_unbind+1);
					cfg_set_object_attr(SDN_COMMON_NODE,ifname,tmp);
					cfg_set_object_attr(SDN_COMMON_NODE,sdn_map_vpn[first_unbind].bindif,"0");
					snprintf(cmd,sizeof(cmd),"echo 'c %s' "
            		">/var/run/xl2tpd/l2tp-control",tmp);
					svc_wan_execute_cmd(cmd);
				}
			}
		}
		first_unbind = -1;
		match = 0;
	}

	for(i = 0; i < sizeof(sdn_map_vpn) / sizeof(sdn_map_vpn[0]); i++)
    {
    	memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_vpn[i].bindif, tmp, sizeof(tmp)) > 0 
			&& sdn_map_vpn[i].state != atoi(tmp))
        {
			if(tmp[0] == '1')
			{
				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),OVS_VSCTL" add-port %s "VPN_INTERNAL_NAME"%d -- set Interface "VPN_INTERNAL_NAME"%d type=internal ofport_request=%d",
		            brname,i,i,i+33);
		        system(cmd);

				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),"/usr/bin/ip addr add 192.168.1.1/24 dev "VPN_INTERNAL_NAME"%d",i);
		        system(cmd);

				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),"/sbin/ifconfig "VPN_INTERNAL_NAME"%d promisc up",i);
		        system(cmd);
				
				memset(mactemp,0,sizeof(mactemp));
				if(get_profile_str("LAN_MAC=",mactemp,sizeof(mactemp),NO_QMARKS, "/etc/mac.conf") == -1)
				{
					tcdbg_printf("[%s %d] no mac file!!\n",__FUNCTION__,__LINE__);
		        	return;
				}
				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),"/sbin/ifconfig "VPN_INTERNAL_NAME"%d hw ether %s",i,mactemp);
		        system(cmd);

				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule add iif "VPN_INTERNAL_NAME"%d table %d",i, SDN_POLICY_TABLE+64+sdn_map_vpn[i].entryid);
		        system(cmd);
				
				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add 192.168.1.0/24 via 192.168.1.1 dev "VPN_INTERNAL_NAME"%d table %d",
					i,SDN_POLICY_TABLE+64+sdn_map_vpn[i].entryid);
		        system(cmd);
				memset(nodeName, 0, sizeof(nodeName));
				memset(gateway, 0, sizeof(gateway));
    			snprintf(nodeName, sizeof(nodeName), VPN_ENTRY_NODE, sdn_map_vpn[i].entryid+1);
				if(cfg_get_object_attr(nodeName, "gateway", gateway, sizeof(gateway)) > 0 
					&& gateway[0] != '\0')
				{
					snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add %s dev ppp%d",
					gateway,100+sdn_map_vpn[i].entryid);
		        	system(cmd);
					snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add default via %s dev ppp%d table %d",
					gateway,100+sdn_map_vpn[i].entryid,SDN_POLICY_TABLE+64+sdn_map_vpn[i].entryid);
		        system(cmd);
				}
			}
			else
			{
		    	memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),OVS_VSCTL" del-port %s "VPN_INTERNAL_NAME"%d",
		            brname,i);
		        system(cmd);
				memset(cmd,0,sizeof(cmd));
		        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del iif "VPN_INTERNAL_NAME"%d",i);
		        system(cmd);
			}
			sdn_map_vpn[i].state=atoi(tmp);
	        tcdbg_printf("sdn_map_data_vpn[%d].state =%d  \n",i,sdn_map_vpn[i].state);
        }
	}
 }
 void update_lan_info(char *brname)
 {
 	char tmp[64] = {0};
 	int i = 0;
 	/*SDN LAN*/
    for(i = 0; i < sizeof(sdn_map_lan) / sizeof(sdn_map_lan[0]); i++)
    {
        memset(tmp, 0, sizeof(tmp));
        if(cfg_get_object_attr(SDN_COMMON_NODE, sdn_map_lan[i].bindif, tmp, sizeof(tmp)) > 0 
            && brname[0] != '\0')
        {
			if(sdn_map_lan[i].state != atoi(tmp))
			{
	            //lan state change
	            printf("Lan%d state change\n",i+1);
	            if(tmp[0] == '1')
	            {
		                add_remove_from_ovs(1,i,brname);    
	            }
	            else
	            {
		            	add_remove_from_ovs(0,i,brname);
	            }
	            sdn_map_lan[i].state=atoi(tmp);
	            printf("sdn_map_data[%d].state =%d  \n",i,sdn_map_lan[i].state);
        	}
        }
    }
 }

int svc_wan_sdn_update()
{
    char cmd[256] = {0};
	char enable[8] = {0},tmp[64] = {0},brname[16] = {0};
    char ctrlip[32] = {0},ctrlwan[16] = {0},def_route_index_v4[10] = {0};
    char wanNodeName[MAXLEN_NODE_NAME] = {0},svc_list[64] = {0},ifname[16] = {0};
    int i = 0, j = 0,default_flow = 0;

	memset(enable,0,sizeof(enable));
    memset(brname,0,sizeof(brname));
	cfg_get_object_attr(SDN_COMMON_NODE, "Enable", enable, sizeof(enable));
    cfg_get_object_attr(SDN_COMMON_NODE, "brname", brname, sizeof(brname));
	if(enable[0] == '1' && brname[0] != '\0')
	{	
		memset(ctrlwan,0, sizeof(ctrlwan));
		memset(tmp,0, sizeof(tmp));
		cfg_get_object_attr(SDN_COMMON_NODE, "CtrlWan", ctrlwan, sizeof(ctrlwan));
		if(!strncmp(ctrlwan,"internet",sizeof(ctrlwan)-1))
    	{
			memset(def_route_index_v4,0, sizeof(def_route_index_v4));
			if(cfg_get_object_attr(WANINFO_COMMON_NODE, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) > 0
				&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A"))
    		{
				/*consider internet in sdn or not*/
    		}
		}
		else if(!strncmp(ctrlwan,"tr069",sizeof(ctrlwan)-1))
		{
	        for(i = 0; i < PVC_NUM; i++) 
		    {
	    		for(j = 0; j < MAX_SMUX_NUM; j++) 
	    		{
	    			memset(wanNodeName, 0, sizeof(wanNodeName));
	    			snprintf(wanNodeName, sizeof(wanNodeName), WAN_PVC_ENTRY_NODE, i + 1, j + 1);
	                memset(svc_list, 0, sizeof(svc_list));
			        if(cfg_get_object_attr(wanNodeName, "ServiceList", svc_list, sizeof(svc_list)) > 0
						&& strstr(svc_list, "TR069") != NULL)
			        {			        
	                    memset(ifname,0,sizeof(ifname));
	                    if(cfg_get_object_attr(wanNodeName, "IFName", ifname, sizeof(ifname)) > 0 
							&& cfg_get_object_attr(SDN_COMMON_NODE, "CtrlIp", ctrlip, sizeof(ctrlip)) > 0
	                        && ifname[0] != '\0' && ctrlip[0] != '\0' && check_ip_format(ctrlip))
	                    {
	                        memset(cmd,0,sizeof(cmd));
			                        snprintf(cmd,sizeof(cmd),"/usr/bin/ip route del %s dev %s",ctrlip,ifname);
			                        system(cmd);
									
			                        memset(cmd,0,sizeof(cmd));
	                        snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add %s dev %s",ctrlip,ifname);
	                        system(cmd);
	                    }
	                    break;			       
	    		    }
	            }
	        }
    		
		}
		default_flow = enable_ovs_bridge(1,brname);
		/*SDN_WAN*/
		update_wan_sdn_info(brname);
		/*VPN*/
		update_vpn_info(brname);
		update_lan_info(brname);
		if(default_flow)
		{
			snprintf(cmd,sizeof(cmd),"/usr/script/sdn_default_flow.sh %s",brname);
			system(cmd);
		}
        
	}
    else if(enable[0] == '0')
    {
        enable_ovs_bridge(0,brname);
		dump_sdn_info(brname);
		return SUCCESS;
    }
    else
    {
        printf("Error--SDN_Common Enable value is NULL.\n");
		return FAIL;
    }
	dump_sdn_info(brname);
    return SUCCESS;
}


