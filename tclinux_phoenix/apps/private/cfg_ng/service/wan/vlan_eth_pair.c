#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cfg_api.h>
#include "wan_common.h"
#include "../cfg/utility.h"
#include "msg_notify.h"
#include "wan_link_list.h"
#include <svchost_evt.h>

#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif

#define UDHCPD_VLAN_CONF "/etc/udhcpd_vlan%s.conf"
#define UDHCPD_VLAN_LEASE "/etc/udhcpd_vlan%s.lease"
#define UDHCPD_VLAN_PID "/etc/udhcpd_vlan%s.pid"
#define ATTR_SIZE 					32
#define DHCPD_PUT_INS 				"%s %s\n"
#define MAX_STATIC_NUM 				8
#define MAX_VENDOR_CLASS_ID_LEN		64
char gVethMac[64]={0};
void change_mac_br0_to_veth(char *mac,unsigned int len)
{
    unsigned int tmp[6]={0};
    unsigned long long mac_index=0;
    int i =0;
    
    if(mac == NULL)
    {
        return;
    }
    sscanf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",&tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
    
    for(i = 0; i < 6; i++)
    {
        mac_index = (mac_index << 8);
        mac_index +=  tmp[i];    
    }
 
    mac_index += 1;
    
    for(i=0; i<6; i++)
    {
        tmp[i] = (mac_index & 0xff);
        mac_index = (mac_index >> 8);
    }
    
    snprintf(mac,len,"%02X:%02X:%02X:%02X:%02X:%02X",tmp[5],tmp[4],tmp[3],tmp[2],tmp[1],tmp[0]);
    
}

static int udhcpd_vlan_write(char*vlan,int vlan_index)
{
	FILE *fp = NULL;
	char buf[128]={0};  /*rodney_20090506 modified*/
    char tmp[32]={0};
	char lanInfo[2][16];/*0: netmask address,1: Lan IP address, */
	char start_ip[16] = {0},poll_cnt[8] = {0}, lease_time[11] = {0}, end_ip[16] = {0};
    char nodeName[64],ifname[16];
	char domain[64] = {0};
	char count[3]={0};
	char dhcpInfo[4] = {0};

	struct in_addr start,end;
	char dhcpd_sys_parm[][ATTR_SIZE]=
	{
		{"opt subnet"},
		{"opt router"},
		{"opt dns"},
		{"opt dns"}, 
		{"opt wins"},
		{"opt domain"},
		{""},
	};
    enum dhcpd_sys_parm_index{
        SUBNET,
        ROUTER,
        DNS1,
        DNS2,
        WINS,
        DOMAIN
    };
	char dhcpd_default_parm[][ATTR_SIZE]=
	{
		{"interface"},
		{"lease_file"},
        {"pidfile"},
		{""},
	};
	int ret = -1;
    char configFile[128],leaseFile[128],pidfile[128];
     
    if(vlan == NULL)
    {
        return 0;
    }
     memset(configFile,0,sizeof(configFile));
     snprintf(configFile,sizeof(configFile),UDHCPD_VLAN_CONF,vlan);
     
     memset(pidfile,0,sizeof(pidfile));
     snprintf(pidfile,sizeof(pidfile),UDHCPD_VLAN_PID,vlan);

     memset(leaseFile,0,sizeof(leaseFile));
     snprintf(leaseFile,sizeof(leaseFile),UDHCPD_VLAN_LEASE,vlan);
     
     memset(ifname,0,sizeof(ifname));
     snprintf(ifname,sizeof(ifname),"veth%s_%d",vlan,vlan_index-1);

	/*get dhcpinfo*/
	memset(tmp, 0, sizeof(tmp));
    snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_DHCPD_NODE,vlan_index);
	if(cfg_obj_get_object_attr(nodeName, "Active", 0, tmp, sizeof(tmp)) > 0)
	{
		strncpy(dhcpInfo, tmp, sizeof(dhcpInfo)-1);
	}

	if(0 == strcmp(dhcpInfo,"Yes")){/*type 0 means disable*/

		fp = fopen(configFile,"w");
		if(fp == NULL){
			return -1;
		}
		else{
			/*Write default parameters*/
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_default_parm[0], ifname);
            fputs(buf,fp);
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_default_parm[1], leaseFile);
            fputs(buf,fp);
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_default_parm[2], pidfile);
            fputs(buf,fp);

			/*Write system parameters*/
            /*option net*/
            memset(lanInfo, 0, sizeof(lanInfo)); 
            if(cfg_obj_get_object_attr(nodeName, "subnetMask", 0, lanInfo[0], sizeof(lanInfo[0])) >= 0
                 && buf[0] != '\0')
            {
                snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[SUBNET], lanInfo[0]);
                fputs_escape(buf,fp);
            }
            /*option router and wins*/
            if(cfg_obj_get_object_attr(nodeName, "router", 0, lanInfo[1], sizeof(lanInfo[1])) >= 0
                 && buf[0] != '\0')
            {
                snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[ROUTER], lanInfo[1]);
                fputs_escape(buf,fp);
                snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[WINS], lanInfo[1]);
                fputs_escape(buf,fp);
            }
            /*option dns1*/
            memset(tmp, 0, sizeof(tmp)); 
            if(cfg_obj_get_object_attr(nodeName, "Primary_DNS", 0, tmp, sizeof(tmp)) >= 0
                 && tmp[0] != '\0')
            {
                snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[DNS1], tmp);
                fputs_escape(buf,fp);
            }
            /*option dns2*/
            memset(tmp, 0, sizeof(tmp)); 
            if(cfg_obj_get_object_attr(nodeName, "Secondary_DNS", 0, tmp, sizeof(tmp)) >= 0
                 && tmp[0] != '\0')
            {
                snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[DNS2], tmp);
                fputs_escape(buf,fp);
            }
            /*domain name*/
            memset(tmp, 0, sizeof(tmp));   
            if(cfg_obj_get_object_attr(nodeName, "domainName", 0, tmp, sizeof(tmp)) >= 0
                 && tmp[0] != '\0')
            {
                snprintf(buf, sizeof(buf), DHCPD_PUT_INS, dhcpd_sys_parm[DOMAIN], tmp);
                fputs_escape(buf,fp);
            }

            memset(start_ip,0, sizeof(start_ip));
            memset(end_ip,0, sizeof(end_ip));
            memset(lease_time,0, sizeof(lease_time));
			if(cfg_obj_get_object_attr(nodeName, "start", 0, start_ip, sizeof(start_ip)) < 0
                || start_ip[0] == '\0'
                || cfg_obj_get_object_attr(nodeName, "end", 0, end_ip, sizeof(end_ip)) < 0
                || end_ip[0] == '\0'
                || cfg_obj_get_object_attr(nodeName, "lease", 0, lease_time, sizeof(lease_time)) < 0
                || lease_time[0] == '\0')
			{
                fclose(fp);
                return -1;
			}

			/*calculate end IP*/
			inet_aton(start_ip, &start);
            inet_aton(end_ip, &end);
			start.s_addr = ntohl(start.s_addr);
            end.s_addr = ntohl(end.s_addr);
            
			snprintf(poll_cnt,sizeof(poll_cnt),"%d",end.s_addr - start.s_addr + 1);
			/*Write user setting parameters*/
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "start" , start_ip);
			fputs_escape(buf, fp);

			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "end" , end_ip);
			fputs_escape(buf, fp);
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "max_leases" , poll_cnt);
			fputs_escape(buf, fp);
			
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), DHCPD_PUT_INS, "option lease" , lease_time);
			fputs_escape(buf, fp);
			
			fclose(fp);
		}
	}else{
      		unlink(configFile);
	}
	return 0;
}
static int dhcpd_vlan_func_execute(char *vlan,int vlan_index)
{

	char buf[16], nodeName[64],cmd[128],configFile[64];

    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),UDHCPD_VLAN_PID,vlan);
    kill_process(cmd);

    memset(configFile,0,sizeof(configFile));
    snprintf(configFile,sizeof(configFile),UDHCPD_VLAN_CONF,vlan);
     
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_DHCPD_NODE, vlan_index);
    memset(buf,0,sizeof(buf));
	if(cfg_obj_get_object_attr(nodeName, "Active", 0, buf, sizeof(buf)) < 0){
		return -1;
	}
	if(!strncmp(buf,"Yes",sizeof(buf)-1)){
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"taskset 4 /usr/sbin/udhcpd %s &",configFile);
		system(cmd);
	}
	return 0;
}
void update_vlan_default_route(wan_entry_obj_t* obj,int evt)
{
    char nodeName[32],cmd[128];
    char if_name[MAX_WAN_DEV_NAME_LEN];
    char gate_way_v4[MAX_WAN_IPV4_ADDR_LEN];
    unsigned long long int vlan_mask = 0;
    int i = 0;
   
    if(obj->cfg.vlanmask[0] == '\0')
    {
        return;
    }
    memset(if_name, 0, sizeof(if_name));
    svc_wan_intf_name(obj, if_name, sizeof(if_name));
    switch (evt)
    {
        case EVT_WAN_CONN_GETV4:
        {
            memset(nodeName,0,sizeof(nodeName));
            snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE,obj->idx + 1);
            if(cfg_get_object_attr(nodeName, "GateWay", gate_way_v4, sizeof(gate_way_v4)) <= 0)
            {
                tcdbg_printf("%s:%d  wan interface %d gateway is NULL\n",__FUNCTION__,__LINE__,obj->idx);
                return;
            }
            vlan_mask = strtoull(obj->cfg.vlanmask,NULL,16);
            for(i = 0; i < MAX_VLAN_NUM ; i++)
            {
                if(vlan_mask & ((unsigned long long )(1) << i))
                {
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/ip route del default table %d",100+i);
                    system(cmd);

                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add default via %s dev %s table %d",gate_way_v4,if_name,100+i);
                    system(cmd);
                }
            }
            break;
        }
        case EVT_WAN_CONN_GETV6:
        {
            break;
        }
    }
}
void syncToVEthPair(wan_entry_obj_t* obj,int flag)
{
    unsigned long long int vlan_mask = 0;
    int i = 0,index = 0,br_flag=0;
    char nodeName[32],tmp[8];
    char cmd[128];
    
    if(obj->cfg.vlanmask[0] == '\0')
    {
        return;
    }
    
    vlan_mask = strtoull(obj->cfg.vlanmask,NULL,16); 
    tcdbg_printf("[%s : %d]  vlan_mask=%llx   obj->cfg.vlanmask=%s\n",__FUNCTION__,__LINE__,vlan_mask,obj->cfg.vlanmask);
    /*flag 1: start interface  0: stop interface*/
    if(flag)
    {
        for(i = 0; i < MAX_VLAN_NUM ; i++)
        {
            if(vlan_mask & ((unsigned long long )(1) << i))
            {
                memset(nodeName,0,sizeof(nodeName));
                snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_NODE,i+1);
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp,sizeof(tmp),"%d",obj->idx);
                cfg_obj_set_object_attr(nodeName, "WanIdx", 0, tmp);
                if( br_flag == 0 && obj->cfg.wanmode == SVC_WAN_ATTR_WANMODE_BRIDGE)
                {
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addbr br-lan%d",obj->idx);
                    system(cmd);

                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/sbin/ifconfig br-lan%d up",obj->idx);
                    system(cmd);
              
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/sys/class/net/br-lan%d/bridge/vlan_filtering",obj->idx);
                    doValPut(cmd, "0");
                
                    br_flag=1;
                }
                
            	svc_wan_vethpair_execute(nodeName); 
            }
        }
    }
    else
    {
        for(i = 0; i < MAX_VLAN_NUM ; i++)
        {
            if(vlan_mask & ((unsigned long long )(1) << i))
            {
                /*br delete */
                if(obj->cfg.wanmode == SVC_WAN_ATTR_WANMODE_BRIDGE)
                {
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/ip link set br-lan%d down",obj->idx);
                    system(cmd);
                    
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/brctl delbr br-lan%d",obj->idx);
                    system(cmd);
                }
                
                memset(nodeName,0,sizeof(nodeName));
                snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_NODE,i+1);
                svc_wan_delete_execute(obj,nodeName);  
            }
        }
    }
    
}
int svc_wan_delete_execute(wan_entry_obj_t* obj,char* path)
{
    char cmd[128];
    char ip_v4[MAX_WAN_IPV4_ADDR_LEN],netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char ip_netmask_v4[MAX_WAN_IPV4_ADDR_LEN],mask_dec[32],vlan[8];
    int vlan_index = -1;
    char *pTmp = NULL;
    struct in_addr ip_v4_addr,netmask_v4_addr;
    if((pTmp = strrchr(path,'.')) != NULL)
    {
        pTmp++;
        vlan_index=atoi(pTmp);
        if(vlan_index < 1 || vlan_index > MAX_VLAN_NUM)
        {
            return -1;
        }           
            
    }
    else
    {
        return -1;
    }
    memset(vlan,0,sizeof(vlan));
    if(cfg_get_object_attr(path, "Vlan", vlan, sizeof(vlan)) < 0
        || '\0' == vlan[0])
    {
        return 0;
    }
    
    /*kill udhcpd or vlanbind info*/
    cfg_set_object_attr(path, "WanIdx", "");

    memset(ip_v4, 0, sizeof(ip_v4));
    memset(netmask_v4, 0, sizeof(netmask_v4));
    if((cfg_get_object_attr(path, "IP", ip_v4, sizeof(ip_v4)) > 0) 
         && (cfg_get_object_attr(path, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
    {
        if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
        {
            snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
        }
        inet_aton(ip_v4, &ip_v4_addr);
        inet_aton(netmask_v4, &netmask_v4_addr);
        ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);
        snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));
    }
    if(obj->cfg.version == SVC_WAN_ATTR_VERSION_IPV4)
    {
        if(SVC_WAN_ATTR_WANMODE_BRIDGE != obj->cfg.wanmode )
        {
            /*del route */
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
            system(cmd);
        }
        else
        {
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip address add %s/%s dev veth%s_%d",ip_v4,mask_dec,vlan,vlan_index-1);
            system(cmd);
            
            /*start ipv4 udhcpd*/
            udhcpd_vlan_write(vlan,vlan_index);
    	    dhcpd_vlan_func_execute(vlan,vlan_index);
            memset(cmd,0,sizeof(cmd));
            
            /*add policy route*/
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule add from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
            system(cmd);
            
            
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add %s/%s dev veth%s_%d table %d",ip_netmask_v4,mask_dec,vlan,vlan_index-1,100+vlan_index-1);
            system(cmd);
            
        }
    }
    else if(obj->cfg.version == SVC_WAN_ATTR_VERSION_IPV6)
    {
        if(SVC_WAN_ATTR_WANMODE_BRIDGE != obj->cfg.wanmode  )
        {
            
        }
        else
        {
            
        }
    }
    return 0;
   
}

int svc_wan_vethpair_execute(char* path)
{
    char nodeName[128],wanNode[128],mactemp[64],pidfile[64],procfile[64];
    char mode[16],vlan[8],wanif[8],lanMask[16],pidbuf[32] = {0};;
    char ip_v4[MAX_WAN_IPV4_ADDR_LEN],netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char ip_netmask_v4[MAX_WAN_IPV4_ADDR_LEN];
    char cmd[128],mask_dec[32];
    int if_index = -1,pvc_index = -1, entry_index = -1,vlan_index = -1,dhcpd_flag = 0;
    int wan_mode = -1,ipversion = SVC_WAN_ATTR_VERSION_IPV4;
    unsigned int lan_mask = 0;
    char *pTmp = NULL;
    struct in_addr ip_v4_addr,netmask_v4_addr;
    
    if((pTmp = strstr(path,"dhcpd")) != NULL)
    {
        *(pTmp-1) = '\0';
        pTmp = NULL;
        
        if((pTmp = strrchr(path,'.')) != NULL)
        {
            pTmp++;
            vlan_index=atoi(pTmp);
            if(vlan_index < 1 || vlan_index > MAX_VLAN_NUM)
            {
                return -1;
            } 
        }
        dhcpd_flag = 1;
    }
    else if((pTmp = strrchr(path,'.')) != NULL)
    {
        pTmp++;
        vlan_index=atoi(pTmp);
        if(vlan_index < 1 || vlan_index > MAX_VLAN_NUM)
        {
            return -1;
        }           
            
    }
    else
    {
        return -1;
    }
   
    memset(vlan,0,sizeof(vlan));
    if(cfg_get_object_attr(path, "Vlan", vlan, sizeof(vlan)) < 0
        || '\0' == vlan[0])
    {
        return 0;
    }
    if(dhcpd_flag == 1)
    {
        /*start ipv4 udhcpd*/
        udhcpd_vlan_write(vlan,vlan_index);
        dhcpd_vlan_func_execute(vlan,vlan_index);
        return 0;
    }
   
    memset(wanif,0,sizeof(wanif));
    if(cfg_get_object_attr(path, "WanIdx", wanif, sizeof(wanif)) >= 0 
        && wanif[0] != '\0')
    {
        if_index = atoi(wanif);
        if(if_index >= 0 && if_index < MAX_WAN_IF_INDEX)
        {
            pvc_index = if_index / MAX_SMUX_NUM + 1;
        	entry_index = if_index % MAX_SMUX_NUM + 1;
            memset(wanNode,0,sizeof(wanNode));
            snprintf(wanNode, sizeof(wanNode), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
            memset(mode,0,sizeof(mode));
            cfg_get_object_attr(wanNode, "WanMode", mode, sizeof(mode));
           
            if(0 == strncmp(mode,"Route",sizeof(mode)-1))
            {
                wan_mode = SVC_WAN_ATTR_WANMODE_ROUTE;
            }
            else if(0 == strncmp(mode,"Bridge",sizeof(mode)-1))
            {
                wan_mode = SVC_WAN_ATTR_WANMODE_BRIDGE;
            }
            else
            {
                tcdbg_printf("[%s %d] wan mode %s is valid!!\n",__FUNCTION__,__LINE__,mode);
                return -1;
            }
            
            memset(mode,0,sizeof(mode));
            cfg_get_object_attr(wanNode, "IPVERSION", mode, sizeof(mode));
         
            if(0 == strncmp(mode,"IPv6",sizeof(mode)-1))
            {
                ipversion = SVC_WAN_ATTR_VERSION_IPV6;
            }
            else if(0 == strncmp(mode,"IPv4",sizeof(mode)-1))
            {
                ipversion = SVC_WAN_ATTR_VERSION_IPV4;
            }
            else
            {
                tcdbg_printf("[%s %d] ip version %s is valid!!\n",__FUNCTION__,__LINE__,mode);
                return -1;
            }
            memset(wanif,0,sizeof(wanif));
            if(wan_mode == SVC_WAN_ATTR_WANMODE_ROUTE)
            {
                cfg_get_object_attr(wanNode, "IFName", wanif, sizeof(wanif));
            }
            else
            {
                cfg_get_object_attr(wanNode, "NASName", wanif, sizeof(wanif));
            }
        }
    }
    
    /*wan interface is delete when Action is Modify*/
    memset(ip_v4, 0, sizeof(ip_v4));
    memset(netmask_v4, 0, sizeof(netmask_v4));
    if((cfg_get_object_attr(path, "IP", ip_v4, sizeof(ip_v4)) > 0) 
                    && (cfg_get_object_attr(path, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
    {
        if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
        {
            snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
        }
        inet_aton(ip_v4, &ip_v4_addr);
        inet_aton(netmask_v4, &netmask_v4_addr);
        ip_v4_addr.s_addr = (ip_v4_addr.s_addr & netmask_v4_addr.s_addr);
        snprintf(ip_netmask_v4, sizeof(ip_netmask_v4), "%s", inet_ntoa(ip_v4_addr));
    }
   
    /*delete veth-pair when lanmask is 0*/
    memset(lanMask,0,sizeof(lanMask));
    if(cfg_obj_get_object_attr(path, "LanMask", 0, lanMask, sizeof(lanMask)) < 0
        || lanMask[0] == '\0')
   {
        snprintf(lanMask,sizeof(lanMask),"%d",0);
   }
    lan_mask = strtol(lanMask,NULL,16);
    if( lan_mask == 0 )
    {
        /*delete veth_pair*/
        if(SVC_WAN_ATTR_WANMODE_BRIDGE != wan_mode )
        {
            /*del route */
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
            system(cmd);
        }
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link del veth%s type veth peer name veth%s_%d",vlan,vlan,vlan_index-1);
        system(cmd);
        return 0;
    }
    
    /*add veth-pair and set interface mode*/
    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/usr/bin/ip link add veth%s type veth peer name veth%s_%d",vlan,vlan,vlan_index-1);
    system(cmd);
    
    memset(mactemp,0,sizeof(mactemp));
    if(get_profile_str("LAN_MAC=",mactemp,sizeof(mactemp),NO_QMARKS, "/etc/mac.conf") == -1)
    {
        tcdbg_printf("[%s %d] no mac file!!\n",__FUNCTION__,__LINE__);
        return -1;
    }
  
    if(gVethMac[0] == '\0')
    {
        strncpy(gVethMac,mactemp,sizeof(gVethMac)-1);
        change_mac_br0_to_veth(gVethMac,sizeof(gVethMac));
    }
      
    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/sbin/ifconfig veth%s up",vlan);
    system(cmd);
    if(SVC_WAN_ATTR_WANMODE_ROUTE == wan_mode)
    {
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link set veth%s_%d address %s up",vlan,vlan_index-1,gVethMac);
        system(cmd);
    }
    else
    {
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link set veth%s_%d up",vlan,vlan_index-1);
        system(cmd);
    }
    
    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br0 veth%s",vlan);
    system(cmd);
    
    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan add vid %s dev veth%s pvid untagged",vlan,vlan);
    system(cmd);
    if(vlan[0] != '1' || (vlan[0] == '1' && vlan[1] != '\0'))
    {
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid 1 dev veth%s",vlan);
        system(cmd);
    }
    
    if(SVC_WAN_ATTR_WANMODE_BRIDGE != wan_mode)/*route mode and without wan info*/
    {
        if(ipversion == SVC_WAN_ATTR_VERSION_IPV4)
        {
            memset(nodeName,0,sizeof(nodeName));
            snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_NODE,vlan_index-1);
            
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip address add %s/%s dev veth%s_%d",ip_v4,mask_dec,vlan,vlan_index-1);
            system(cmd);

            memset(pidfile,0,sizeof(pidfile));
            snprintf(pidfile,sizeof(pidfile),UDHCPD_VLAN_PID,vlan);
            
            memset(pidbuf, 0, sizeof(pidbuf));
		    fileRead(pidfile, pidbuf, sizeof(pidbuf));
            
            if(pidbuf[0] != '\0')
            {
                snprintf(procfile,sizeof(procfile),"/proc/%d/stat",atoi(pidbuf));
            }
            if(0 != access(pidfile, F_OK) || pidbuf[0] == '\0' || 0 != access(procfile, F_OK))
            {
                /*start ipv4 udhcpd*/
                udhcpd_vlan_write(vlan,vlan_index);
        	    dhcpd_vlan_func_execute(vlan,vlan_index);
            }
            
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
            system(cmd);
            /*add policy route*/
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule add from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
            system(cmd);
            
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip route add %s/%s dev veth%s_%d table %d",ip_netmask_v4,mask_dec,vlan,vlan_index-1,100+vlan_index-1);
            system(cmd);
        }
        else/*ipv6*/
        {
            
        }
        
    }
    else/*bridge mode*/
    {
        /*kill upnpd first*/
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),UDHCPD_VLAN_PID,vlan);
        kill_process(cmd);
        /*delete policy route*/
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
        system(cmd);
        /*delete veth wan interface IP*/
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip address del %s/%s dev veth%s_%d",ip_v4,mask_dec,vlan,vlan_index-1);
        system(cmd);
        
        memset(nodeName,0,sizeof(nodeName));
        snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_NODE,vlan_index-1);
        
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/ip link set br-lan%d address %s",if_index,gVethMac);
        system(cmd);
        
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br-lan%d veth%s_%d",if_index,vlan,vlan_index-1);
        system(cmd);
        
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/usr/bin/brctl addif br-lan%d %s",if_index,wanif);
        system(cmd);

    }
	return 0;
}






