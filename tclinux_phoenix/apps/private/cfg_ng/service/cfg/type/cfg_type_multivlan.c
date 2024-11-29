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
#include <stdlib.h>
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include <svchost_evt.h>
#include "utility.h"

#if defined(TCSUPPORT_MULTI_USER_ITF) 
#include <lan_port/lan_port_info.h>
#endif
#include <lan_port/bind_list_map.h>

static cfg_node_type_t cfg_type_multivlan_entry;

static char* cfg_type_multivlan_index[] = { 
	 "MultiVLan_id", 
	 NULL 
}; 


static int cfg_type_multivlan_entry_func_commit(char* path)
{
	wan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	tcdbg_printf("path=%s\n",path);
	cfg_obj_send_event(EVT_WAN_INTERNAL, EVT_CFG_WAN_VETHPAIR_UPDATE, (void *)&param, sizeof(param)); 
	
	return 0;	
}
static int cfg_type_multivlan_entry_dhcpd_func_commit(char* path)
{
    wan_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	tcdbg_printf("path=%s\n",path);
	cfg_obj_send_event(EVT_WAN_INTERNAL, EVT_CFG_WAN_VETHPAIR_UPDATE, (void *)&param, sizeof(param)); 
    
    return 0;

}

static int cfg_type_multivlan_entry_func_delete(char* path)
{
    char nodeName[32];
    char vlan[8],wanif[8],tmp[32],multiVlan[512],resultVlan[512],lanMask[16];
    char cmd[128],mode[8],pvid[8];
    char ip_v4[16],netmask_v4[16],mask_dec[32];
    int i = 0, if_index = -1, vlan_index = -1,pvc_index = -1, entry_index = -1,wan_mode = -1,ipversion= -1;
    unsigned long int lan_mask = 0;
    unsigned long long int vlan_mask = 0;
    char *pStr = NULL,*pTmp = NULL;

      /*get lan index*/
    if((pTmp = strrchr(path,'.')) != NULL)
    {
            pTmp++;
            vlan_index=atoi(pTmp);
            if(vlan_index < 1 || vlan_index > MAX_VLAN_NUM)
            {
                return 0;
            }           
            
    }
    else
    {
        return -1;
    }
    
    memset(vlan,0,sizeof(vlan));
	if(cfg_obj_get_object_attr(path, "Vlan", 0, vlan, sizeof(vlan)) < 0
        || vlan[0] == '\0')
	{
        return cfg_type_default_func_delete(path);
	}
     memset(lanMask,0,sizeof(lanMask));
	if(cfg_obj_get_object_attr(path, "LanMask", 0, lanMask, sizeof(lanMask)) < 0
        || lanMask[0] == '\0')
	{
        return cfg_type_default_func_delete(path);
	}

    /*wan information update*/
    memset(wanif,0,sizeof(wanif));
    if(cfg_obj_get_object_attr(path, "WanIdx", 0, wanif, sizeof(wanif)) > 0
        && wanif[0] != '\0')
    {
        if_index = atoi(wanif);
        if(if_index >= 0 || if_index < MAX_WAN_IF_INDEX)
        {
            pvc_index = if_index / MAX_SMUX_NUM + 1;
        	entry_index = if_index % MAX_SMUX_NUM + 1;
            memset(nodeName,0,sizeof(nodeName));
            snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
            memset(tmp,0,sizeof(tmp));
            cfg_get_object_attr(nodeName, "WanMode", tmp, sizeof(tmp));
             
            if(0 == strncmp(tmp,"Route",sizeof(tmp)-1))
            {
                wan_mode = 1;/*SVC_WAN_ATTR_WANMODE_ROUTE*/
            }
            else if(0 == strncmp(tmp,"Bridge",sizeof(tmp)-1))
            {
                wan_mode = 2;/*SVC_WAN_ATTR_WANMODE_BRIDGE*/
            }
        }
        memset(tmp,0,sizeof(tmp));
        cfg_get_object_attr(nodeName, "IPVERSION", tmp, sizeof(tmp));
        if(0 == strncmp(tmp,"IPv6",sizeof(tmp)-1))
        {
            ipversion = 2;/*SVC_WAN_ATTR_VERSION_IPV6*/
        }
        else if(0 == strncmp(tmp,"IPv4",sizeof(tmp)-1))
        {
            ipversion = 1;/*SVC_WAN_ATTR_VERSION_IPV4*/
        }
        
        /*update vlanmask in wan info*/
        memset(tmp,0,sizeof(tmp));
        cfg_get_object_attr(nodeName, "VlanMask", tmp, sizeof(tmp));
        vlan_mask = strtoull(tmp,NULL,16); 
        vlan_mask &= ~((unsigned long long)(1) << (vlan_index - 1));
        snprintf(tmp,sizeof(tmp),"%llx",vlan_mask);
        cfg_set_object_attr(nodeName, "VlanMask", tmp);
        if(wan_mode == 2)
        {
            /*bridge mode*/
            
            if(ipversion == 1)
            {
                /*ipv4 */
                memset(cmd,0,sizeof(cmd));
                snprintf(cmd,sizeof(cmd),"/usr/bin/brctl delif br-lan%d veth%s_%d",if_index,vlan,vlan_index-1);
                system(cmd);
                
            }
            else if(ipversion == 2)
            {
                /*ipv6*/
            }
            
        }
        else if(wan_mode == 1)
        {
            /*route mode*/

            if(ipversion == 1)
            {
                /*ipv4 */
                memset(ip_v4, 0, sizeof(ip_v4));
                memset(netmask_v4, 0, sizeof(netmask_v4));
                if((cfg_get_object_attr(path, "IP", ip_v4, sizeof(ip_v4)) > 0) 
                    && (cfg_get_object_attr(path, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
                {
                    if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
                    {
                        snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
                    }
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
                    system(cmd);
                    
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/ip route del %s/%s dev veth%s_%d",ip_v4,mask_dec,vlan,vlan_index-1);
                    system(cmd);
                }
                memset(cmd,0,sizeof(cmd));
                snprintf(cmd,sizeof(cmd),"/etc/udhcpd_vlan%s.pid",vlan);
                kill_process(cmd);
            }
            else if(ipversion == 2)
            {
                /*ipv6*/
            }
        }
    }
    else
    {
        /*delete policy rule and kill udhcpd process*/
        memset(ip_v4, 0, sizeof(ip_v4));
        memset(netmask_v4, 0, sizeof(netmask_v4));
        if((cfg_get_object_attr(path, "IP", ip_v4, sizeof(ip_v4)) > 0) 
            && (cfg_get_object_attr(path, "NetMask", netmask_v4, sizeof(netmask_v4)) > 0))
        {
            if ( !check_mask_format(netmask_v4, mask_dec,sizeof(mask_dec)) )
            {
                snprintf(mask_dec, sizeof(mask_dec), "%d", 24);
            }
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip rule del from %s/%s table %d",ip_v4,mask_dec,100+vlan_index-1);
            system(cmd);

            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/ip route del %s/%s dev veth%s_%d",ip_v4,mask_dec,vlan,vlan_index-1);
            system(cmd);
        }
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"/etc/udhcpd_vlan%s.pid",vlan);
        kill_process(cmd);
    }
    
    /*MultiLan information update*/
    lan_mask = strtoul(lanMask,NULL,16);
    if(lan_mask == 0)
    {
        return cfg_type_default_func_delete(path);
    }
    /*delete veth-pair*/
    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev veth%s pvid untagged",vlan,vlan);
    system(cmd);
    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/usr/bin/ip link del veth%s type veth peer name veth%s_%d",vlan,vlan,vlan_index-1);
    system(cmd);

    
    memset(mode,0,sizeof(mode));
    for(i = 0; i < MAX_LAN_PORT_NUM-2; i++)
    {
        if((lan_mask & ((unsigned long int)(0x1) << i)) == 0)
        {
            continue;
        }
        memset(nodeName,0,sizeof(nodeName));
        snprintf(nodeName,sizeof(nodeName),MULTILAN_ENTRY_NODE,i+1);
        cfg_obj_get_object_attr(nodeName, "Mode", 0, mode, sizeof(mode));
        memset(multiVlan,0,sizeof(multiVlan));
        cfg_obj_get_object_attr(nodeName, "Vlan", 0, multiVlan, sizeof(multiVlan));
        cfg_obj_get_object_attr(nodeName, "pvid", 0, pvid, sizeof(pvid));
        if(0 == strncmp(mode,"access",sizeof(mode)-1))
        {
            cfg_obj_set_object_attr(nodeName, "Vlan", 0, "");
            /*delete interface pvid*/
             memset(cmd,0,sizeof(cmd));
             snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev %s pvid untagged",pvid,bl_map_data[i].realif);
             system(cmd);
        }
        else if( 0 == strncmp(mode,"trunk",sizeof(mode)-1))
        {
            memset(resultVlan,0,sizeof(resultVlan));
            pTmp = multiVlan;
            while(*pTmp != '\0' && (pStr = strstr(pTmp,vlan)) != NULL )
            {
                
                if(*(pStr+strlen(vlan)) == '/' 
                    && (pStr == multiVlan || *(pStr-1) == '/'))
                {
                    if(pStr == multiVlan)
                    {
                        strncpy(resultVlan,pStr+strlen(vlan)+1,sizeof(resultVlan)-1);
                    }
                    else
                    {
                        strncpy(resultVlan,multiVlan,pStr-multiVlan);
                        strncat(resultVlan,pStr+strlen(vlan)+1, sizeof(resultVlan)-strlen(resultVlan)-1);
                    }
                    cfg_obj_set_object_attr(nodeName, "Vlan", 0, resultVlan);
                    /*delete veth-pair*/
                    if(0 == strncmp(multiVlan,pvid,sizeof(multiVlan)-1))
                    {
                        memset(cmd,0,sizeof(cmd));
                        snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev %s pvid untagged",pvid,bl_map_data[i].realif);
                        system(cmd);
                    }
                    else
                    {
                        memset(cmd,0,sizeof(cmd));
                        snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev %s",pvid,bl_map_data[i].realif);
                        system(cmd);
                    }
                    
                    break;
                }
                else
                {
                    pTmp += strlen(vlan);
                }
            }
        }
        else
        {
            tcdbg_printf("[%s][%d] %s No Lan uses vlan %s!\n",__FUNCTION__,__LINE__,vlan);
        }
    }
    return cfg_type_default_func_delete(path);
}


static cfg_node_ops_t cfg_type_multivlan_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_multivlan_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_multivlan_entry_func_commit 
}; 
static cfg_node_ops_t cfg_type_multivlan_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 

static cfg_node_ops_t cfg_type_multivlan_entry_dhcpd_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_multivlan_entry_dhcpd_func_commit,
}; 

static cfg_node_type_t cfg_type_multivlan_entry_dhcpd = { 
	 .name = "Dhcpd", 
	 .flag = 1, 
	 .parent = &cfg_type_multivlan_entry, 
	 .ops = &cfg_type_multivlan_entry_dhcpd_ops, 
};

static cfg_node_type_t* cfg_type_multivlan_entry_child[] = { 
	 &cfg_type_multivlan_entry_dhcpd, 
	 NULL 
}; 

static cfg_node_type_t cfg_type_multivlan_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_VLAN_NUM, 
	 .parent = &cfg_type_multivlan, 
	 .index = cfg_type_multivlan_index, 
	 .nsubtype = sizeof(cfg_type_multivlan_entry_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = &cfg_type_multivlan_entry_child,
	 .ops = &cfg_type_multivlan_entry_ops, 
}; 

static cfg_node_type_t cfg_type_multivlan_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_multivlan, 
	 .ops = &cfg_type_multivlan_common_ops, 
}; 

static cfg_node_ops_t cfg_type_multivlan_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_multivlan_child[] = { 
	 &cfg_type_multivlan_entry, 
     &cfg_type_multivlan_common,   
	 NULL 
}; 


cfg_node_type_t cfg_type_multivlan = { 
	 .name = "MultiVlan", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_multivlan_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_multivlan_child, 
	 .ops = &cfg_type_multivlan_ops, 
}; 
