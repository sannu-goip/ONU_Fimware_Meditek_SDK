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

static char* cfg_type_multilan_index[] = { 
	 "MultiLan_id", 
	 NULL 
}; 
int cfg_type_multilan_entry_func_commit_real(char* path,int boot_flag)
{
    char nodeName[64];
    char trunkVlan[MAX_VLAN_NUM][8];
	char mode[8],vlan[320],ethVlan[8],action[16],lanMask[16],pvid[8],mactemp[64];
    char cmd[128];
    unsigned int i = 0,j = 0,trunkVlanNum = 0;
    unsigned long int lan_mask= 0;
    int lan_index = -1,del_flag = 0,vlan_index = -1;
    char *pVlan = NULL,*pLan = NULL;
          /*get lan index*/
    if((pLan = strrchr(path,'.')) != NULL)
    {
        pLan++;
        lan_index=atoi(pLan);
        if(lan_index < 1 || lan_index > MAX_LAN_PORT_NUM-2)
        {
            return 0;
        }                    
    }
    else
    {
        return -1;
    }
    
    memset(nodeName,0,sizeof(nodeName));
    memset(pvid,0,sizeof(pvid));
    if(cfg_obj_get_object_attr(path, "pvid", 0, pvid, sizeof(pvid)) < 0
        || '\0' == pvid[0])
    {
        return 0;
    }
    vlan_index = atoi(pvid);
        
    if(vlan_index < 0 || vlan_index > 4095)
    {
        return -1;
    }
    
    memset(mode,0,sizeof(mode));
    cfg_obj_get_object_attr(path, "Mode", 0, mode, sizeof(mode));
    memset(action,0,sizeof(action));
    cfg_obj_get_object_attr(MULTILAN_COMMON_NODE, "Action", 0, action, sizeof(action));
    if(0 == strncmp(action,"Modify",sizeof(action)-1))
    {
        del_flag = 1;
        cfg_obj_set_object_attr(MULTILAN_COMMON_NODE, "Action", 0, "");
    }
  
    if(0 == strncmp(mode,"access",sizeof(mode)-1))
    {
        for(i = 0;i < MAX_VLAN_NUM;i++)
        {
            memset(ethVlan,0,sizeof(ethVlan));
            snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_NODE,i+1);
            cfg_obj_get_object_attr(nodeName, "Vlan", 0, ethVlan, sizeof(ethVlan));
            
            if(0 == strncmp(pvid,ethVlan,sizeof(pvid)-1))
            {
               /*update VEthPair Lan_mask*/
               memset(lanMask,0,sizeof(lanMask));
               if(cfg_obj_get_object_attr(nodeName, "LanMask", 0, lanMask, sizeof(lanMask)) < 0
                || lanMask[0] == '\0')
               {
                    snprintf(lanMask,sizeof(lanMask),"%d",0);
               }
               lan_mask = strtoul(lanMask,NULL,16);
               if(del_flag)
               {
                    if(lan_mask != 0)
                    {
                    /*unset port mode acess or trunk*/
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev %s pvid untagged",pvid,bl_map_data[lan_index-1].realif);
                    system(cmd);
                                                
                        lan_mask &= ~((unsigned long)(1) << (lan_index - 1));
                    if(lan_mask == 0)
                    {
                        cfg_obj_set_object_attr(nodeName, "LanMask", 0, "0");
                        cfg_commit_object(nodeName);
                    }
                    else
                    {
                        snprintf(lanMask,sizeof(lanMask),"%x",lan_mask);
                        cfg_obj_set_object_attr(nodeName, "LanMask", 0, lanMask);
                    }
               }
               }
               else
               {
                     /*delete default pvid 1 first*/
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid 1 dev %s pvid untagged",bl_map_data[lan_index-1].realif);
                    system(cmd);
                    /*add new pvid*/
                    memset(cmd,0,sizeof(cmd));
                    snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan add vid %s dev %s pvid untagged",pvid,bl_map_data[lan_index-1].realif);
                    system(cmd);
                    if(lan_mask == 0 || boot_flag == 1)
                    {
                        lan_mask |= (unsigned long)(1) << (lan_index - 1);
                        snprintf(lanMask,sizeof(lanMask),"%x",lan_mask);
                        cfg_obj_set_object_attr(nodeName, "LanMask", 0, lanMask);
                        cfg_commit_object(nodeName);
                    }
                    else
                    {
                        lan_mask |= (unsigned long)(1) << (lan_index - 1);
                        snprintf(lanMask,sizeof(lanMask),"%x",lan_mask);
                        cfg_obj_set_object_attr(nodeName, "LanMask", 0, lanMask);
                    }
               }
               break;
            }       
        }
       
    }
    else if(0 == strncmp(mode,"trunk",sizeof(mode)-1))
    {
        memset(vlan,0,sizeof(vlan));
        if(cfg_obj_get_object_attr(path, "Vlan", 0, vlan, sizeof(vlan)) < 0
            || '\0' == vlan[0])
        {
            return -1;
        }
        trunkVlanNum = 0;
        memset(trunkVlan,0,sizeof(trunkVlan));
        pVlan = strtok(vlan, "/");
		while(NULL != pVlan)
		{
            if(trunkVlanNum >= MAX_VLAN_NUM)
                break;
			strncpy(trunkVlan[trunkVlanNum],pVlan,sizeof(trunkVlan[trunkVlanNum]) - 1 );
			pVlan = strtok(NULL, "/");
	        trunkVlanNum++;
		}
        strncpy(trunkVlan[trunkVlanNum],pvid,sizeof(trunkVlan[trunkVlanNum])-1);
        /*pvid process*/
        if(del_flag)
        {
            /*unset port pvid*/
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev %s pvid untagged",pvid,bl_map_data[lan_index-1].realif);
            system(cmd);
        }
        else
        {
             /*delete default pvid 1 first*/
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid 1 dev %s pvid untagged",bl_map_data[lan_index-1].realif);
            system(cmd);
            /*add new pvid*/
            memset(cmd,0,sizeof(cmd));
            snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan add vid %s dev %s pvid untagged",pvid,bl_map_data[lan_index-1].realif);
            system(cmd);
        }
        for(i = 0; i <= trunkVlanNum;i++)
        {
            tcdbg_printf("===trunkVlan[%d]=%s====\n",i,trunkVlan[i]);

            vlan_index = atoi(trunkVlan[i]);
            if(vlan_index < 0 || vlan_index > 4095)
            {
                continue;
            }
            for(j = 0;j < MAX_VLAN_NUM;j++)
            {
                memset(ethVlan,0,sizeof(ethVlan));
                snprintf(nodeName,sizeof(nodeName),MULTIVLAN_ENTRY_NODE,j+1);
                cfg_obj_get_object_attr(nodeName, "Vlan", 0, ethVlan, sizeof(ethVlan));
                
                memset(lanMask,0,sizeof(lanMask));
                if(cfg_obj_get_object_attr(nodeName, "LanMask", 0, lanMask, sizeof(lanMask)) < 0
                    || lanMask[0] == '\0')
                {
                    snprintf(lanMask,sizeof(lanMask),"%d",0);
                }
               lan_mask = strtoul(lanMask,NULL,16);
                if(0 == strncmp(trunkVlan[i],ethVlan,sizeof(trunkVlan[i])-1))
                {
                   /*update VEthPair Lan_mask*/
                   
                   if(del_flag)
                   {
                        if(lan_mask != 0)
                        {
                        /*unset port vlan*/
                        if(i != trunkVlanNum)
                        {
                            memset(cmd,0,sizeof(cmd));
                            snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan del vid %s dev %s",ethVlan,bl_map_data[lan_index-1].realif);
                            system(cmd);
                        }
                                          
                            lan_mask &= ~((unsigned long)(1) << (lan_index - 1));
                        if(lan_mask == 0)
                        {
                            cfg_obj_set_object_attr(nodeName, "LanMask", 0, "0");
                            cfg_commit_object(nodeName);
                        }
                        else
                        {
                            snprintf(lanMask,sizeof(lanMask),"%x",lan_mask);
                            cfg_obj_set_object_attr(nodeName, "LanMask", 0, lanMask);
                        }
                   }
                   }
                   else
                   {
                         /*set port vlan*/
                       if(i != trunkVlanNum)
                       {
                            memset(cmd,0,sizeof(cmd));
                            snprintf(cmd,sizeof(cmd),"/usr/bin/bridge vlan add vid %s dev %s",ethVlan,bl_map_data[lan_index-1].realif);
                            system(cmd);
                       }
                        if(lan_mask == 0 || boot_flag == 1)
                        {
                            lan_mask |= (unsigned long)(1) << (lan_index - 1);
                            snprintf(lanMask,sizeof(lanMask),"%x",lan_mask);
                            cfg_obj_set_object_attr(nodeName, "LanMask", 0, lanMask);
                            cfg_commit_object(nodeName);
                        }
                        else
                        {
                            lan_mask |= (unsigned long)(1) << (lan_index - 1);
                            snprintf(lanMask,sizeof(lanMask),"%x",lan_mask);
                            cfg_obj_set_object_attr(nodeName, "LanMask", 0, lanMask);
                        }
                   }
                   break;
                }         
            }
        }
    }
    else
    {
        tcdbg_printf("[%s][%d] %s Mode error,just support access/trunk Mode!\n",__FUNCTION__,__LINE__,path);
    }
}

int svc_cfg_multilan_boot(void)
{
    char nodeName[64];
    int i = 0;
    /*without device USB and WDS*/
    for(i = 0; i < MAX_LAN_PORT_NUM-2;i++)
    {
        memset(nodeName,0,sizeof(nodeName));
        snprintf(nodeName,sizeof(nodeName),MULTILAN_ENTRY_NODE,i+1);
        cfg_type_multilan_entry_func_commit_real(nodeName,1);
    }
    return 0;
}

int cfg_type_multilan_entry_func_commit(char* path)
{
	
	cfg_type_multilan_entry_func_commit_real(path,0);
	return 0;	
}

static cfg_node_ops_t cfg_type_multilan_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_multilan_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1,  
	 .parent = &cfg_type_multilan, 
	 .ops = &cfg_type_multilan_common_ops, 
};

static cfg_node_ops_t cfg_type_multilan_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_multilan_entry_func_commit 
}; 


static cfg_node_type_t cfg_type_multilan_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | (MAX_LAN_PORT_NUM-2), 
	 .parent = &cfg_type_multilan, 
	 .index = cfg_type_multilan_index, 
	 .ops = &cfg_type_multilan_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_multilan_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_multilan_entry_func_commit 
}; 


static cfg_node_type_t* cfg_type_multilan_child[] = { 
	 &cfg_type_multilan_entry,
     &cfg_type_multilan_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_multilan = { 
	 .name = "MultiLan", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_multilan_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_multilan_child, 
	 .ops = &cfg_type_multilan_ops, 
}; 
