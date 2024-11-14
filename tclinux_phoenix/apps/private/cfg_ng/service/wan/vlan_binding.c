#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cfg_api.h>
#include "wan_common.h"
#include "dhcp_relay_info.h"
#include "dhcp_port_filter_info.h"
#include "vlan_binding.h"
#include "../cfg/utility.h"
#include "msg_notify.h"
#include "port_binding.h"
#include "wan_link_list.h"

#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
#include <lan_port/lan_port_info.h>
#endif
#include "blapi_traffic.h"

#define u8  unsigned char 
#define u16 unsigned short
#define u32 unsigned int


static int isVlanBindActive(int index)
{
	int control_on	= 0;
	char nodeName[128] = {0};
	char control_status[5]={0};

	if(index < 0 || index >= MAX_LAN_PORT_NUM)
	{
		SVC_WAN_ERROR_INFO("isVlanBindActive LAN port index error\n");
		return -1;
	}

	/*flush the buffer*/
	snprintf(nodeName, sizeof(nodeName), VLANBIND_ENTRY_NODE, index + 1);

	/*check the Active value*/
	if(cfg_get_object_attr(nodeName, "Active", control_status, sizeof(control_status)) <= 0)
	{
		SVC_WAN_ERROR_INFO("Get VlanBind Active error in isVlanBindActive\n");
	}
	/*see if control's attribute(Active) = Yes*/
	else if( (!strcmp(control_status,"Yes")) || (!strcmp(control_status,"yes")))
	{
		control_on = 1;
	}
	
	return control_on;
}


#if defined(TCSUPPORT_CMCC)
static int checkVlanBindGroup(vlan_map_t vBindArray[MAX_LAN_PORT_NUM][MAX_VLAN_GROUP], int in_portNum, int out_portNum)
{
	int i, j;
	for (i = 0; i < MAX_VLAN_GROUP; i ++) 
	{
		if(vBindArray[in_portNum][i].m < 1)
		{
			if(i == 0)
				return 1;
			else
				break;
		}
		for (j = 0; j < MAX_VLAN_GROUP; j ++)
		{
			if(vBindArray[out_portNum][j].m < 1)
			{
				if(j == 0)
					return 1;
				else
					break;
			}
			if(vBindArray[in_portNum][i].m == vBindArray[out_portNum][j].m)
			{
				return 1;
			}
		}
	}
	return 0;
}
#endif

/* add by xyzhu to fix dhcp dial fail from lan when vlan bind enable */
/* update dhcp relay rule */
static int update_vlan_bind_dhcp_relay_rule(void)
{
	wan_entry_obj_t* obj = NULL;

	for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
	{
		set_DHCPRelay_info(obj, E_DEL_DHCPRELAY_RULE);
#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
		set_DHCP_PortFilter_info(obj, E_DEL_DHCP_PORT_FILTER_RULE);
#endif
		set_DHCPRelay_info(obj, E_ADD_DHCPRELAY_RULE);
#if defined(TCSUPPORT_CT_DHCP_PORT_FILTER)
		set_DHCP_PortFilter_info(obj, E_ADD_DHCP_PORT_FILTER_RULE);
#endif
	}

	return 0;
}

static int svc_wan_vlanbind_write(wan_entry_obj_t* obj,int flag)
{
	FILE *fp 			= NULL;
	int i				= 0;
	int j				= 0;
	int k				= 0;
	int isActive		= 0;
	int isBridgeEntry	= 0;
	char nodeName[128] 	= {0}, node_val[128] = {0}, node_val2[128] = {0};
	vlan_map_t vlanMap 	= {0,0};
#if defined(TCSUPPORT_CMCC)
	vlan_map_t vlanMapArray[MAX_LAN_PORT_NUM][MAX_VLAN_GROUP];
    int port_group[6];
    memset(port_group,-1,sizeof(port_group));
#endif
	char strVlanMap[MAX_MAP_ONE_ENTRY*10]	= {0};
	char cmdBuf[128]						= {0};
	char *pStrVlanMap 						= NULL;
	char delims[] 							= ";";
	char lanIf[7]				= {0};
	char vlanMode[15]			= {0};
	char vlanID[10]				= {0};
	char entry_active[5]		= {0};
	char entry_wanMode[10]		= {0};
	int mark 					= 0;
	unsigned int entryActive 	= 0;
	int ii =0;
	char* savePtr = NULL;
#if defined(TCSUPPORT_CMCCV2)
    char tempVlan_m[8] = {0};
	char tempVlan_n[8] = {0};
#endif
	int groupid = 1, is_bind_ob = 0;
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CFG_NG_UNION)
	char str_lan_vlan_id[8] = {0};
#endif
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
	int dev_pos = 0;
#endif

#if defined(TCSUPPORT_CMCC)
	memset(vlanMapArray, 0, sizeof(vlanMapArray));
#endif
	/*open vlan_bind_rule.sh to write*/
	fp = fopen(VLAN_BIND_SH, "w+");
	if(NULL == fp)
	{
		SVC_WAN_ERROR_INFO("vlan_bind_rule.sh file pointer is NULL.\n");
		return -1;
	}

	memset(cmdBuf, 0, sizeof(cmdBuf));
	snprintf(cmdBuf, sizeof(cmdBuf), "echo reset > %s\n", VBIND_ENTRY_ARRAY_PATH);
	svc_wan_execute_cmd(cmdBuf);
#if defined(TCSUPPORT_CMCC)
	memset(cmdBuf, 0, sizeof(cmdBuf));
	snprintf(cmdBuf, sizeof(cmdBuf), "echo reset > %s\n", VBIND_ENTRY_GROUP_PATH);
	svc_wan_execute_cmd(cmdBuf);
#endif
	for(i = 0; i < MAX_LAN_PORT_NUM; i++)
	{
		isActive = isVlanBindActive(i);
#if defined(TCSUPPORT_CMCCV2)
		snprintf(nodeName, sizeof(nodeName), VLANBIND_ENTRY_NODE, i + 1);
		if((cfg_get_object_attr(nodeName, "tempVlan_m", tempVlan_m, sizeof(tempVlan_m)) > 0 && tempVlan_m[0] != '\0' )
			&& (cfg_get_object_attr(nodeName, "tempVlan_n", tempVlan_n, sizeof(tempVlan_n)) > 0 && tempVlan_n[0] != '\0'))
		{
			blapi_traffic_set_traffic_class("VLANID",atoi(tempVlan_m), atoi(tempVlan_n), DEL_TRAFFIC_CLASS);
			cfg_set_object_attr(nodeName, "tempVlan_m", "");
			cfg_set_object_attr(nodeName, "tempVlan_n", "");
		}
#endif
		if (isActive > 0)
		{
			entryActive |= (1<<i);
			memset(cmdBuf, 0, sizeof(cmdBuf));
			snprintf(cmdBuf, sizeof(cmdBuf), "%s -D BROUTING -j vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
			fputs(cmdBuf, fp);	
			
			memset(cmdBuf, 0, sizeof(cmdBuf));
			snprintf(cmdBuf, sizeof(cmdBuf), "%s -X vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
			fputs(cmdBuf, fp);
			
			memset(cmdBuf, 0, sizeof(cmdBuf));
			snprintf(cmdBuf, sizeof(cmdBuf), "%s -N vbind_entry%d_chain -P RETURN\n", EBTABLES_BROUTE, i);
			fputs(cmdBuf, fp);
			
			memset(cmdBuf, 0, sizeof(cmdBuf));
			snprintf(cmdBuf, sizeof(cmdBuf), "%s -I BROUTING -j vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
			fputs(cmdBuf, fp); 
			
			pStrVlanMap = strVlanMap;
			memset(strVlanMap, 0, sizeof(strVlanMap));
			memset(nodeName, 0, sizeof(nodeName));			
			snprintf(nodeName, sizeof(nodeName), VLANBIND_ENTRY_NODE, i + 1);		
			if(cfg_get_object_attr(nodeName, "mn", strVlanMap, sizeof(strVlanMap)) <= 0)
			{
				SVC_WAN_ERROR_INFO("vlanbind_write:get mn failed\n");
				goto ERROR;
			}


			pStrVlanMap = strtok_r(strVlanMap, delims, &savePtr);
			ii = 0; /*Reset vlan group index*/
			while(pStrVlanMap != NULL)
			{
				sscanf(pStrVlanMap, "%d/%d", &vlanMap.m, &vlanMap.n);
				if(vlanMap.m < 1 || vlanMap.m > 4094 || vlanMap.n < 1 || vlanMap.n > 4094)
				{
					SVC_WAN_ERROR_INFO("vlanbind_write:mn value error.\n");
					goto ERROR;
				}
				snprintf(cmdBuf, sizeof(cmdBuf), "echo %d %d %d %d > %s\n", i, ii, vlanMap.m, vlanMap.n, VBIND_ENTRY_ARRAY_PATH);
				svc_wan_execute_cmd(cmdBuf);
#if defined(TCSUPPORT_CMCCV2)
				blapi_traffic_set_traffic_class("VLANID", vlanMap.m, vlanMap.n, ADD_TRAFFIC_CLASS);
				snprintf(tempVlan_m, sizeof(tempVlan_m), "%d", vlanMap.m);
				snprintf(tempVlan_n, sizeof(tempVlan_n), "%d", vlanMap.n);
				cfg_set_object_attr(nodeName,"tempVlan_m",tempVlan_m);
				cfg_set_object_attr(nodeName,"tempVlan_n",tempVlan_n);
#endif
#if defined(TCSUPPORT_CMCC)
				memcpy(&vlanMapArray[i][ii], &vlanMap, sizeof(vlan_map_t));
#endif
				ii ++;		
				
#if defined(TCSUPPORT_MULTI_USER_ITF) || defined(TCSUPPORT_MULTI_SWITCH_EXT)
#if !defined(TCSUPPORT_MULTI_SWITCH_EXT)
				if ( i >=4 )
					dev_pos = i + 5;
				else
#endif
					dev_pos = i + 1;
				offset_convert_devname(dev_pos, lanIf, sizeof(lanIf));				
#else
				if(i<4)
					snprintf(lanIf, sizeof(lanIf), "eth0.%d", i+1);
#if defined(TCSUPPORT_WLAN_AC)
				else if ( i >= 10 )
					snprintf(lanIf, sizeof(lanIf), "rai%d", i-10);
#endif
				else
					snprintf(lanIf, sizeof(lanIf), "ra%d", i-4);
#endif

				/*get PVC route wan interface then get mark value.
				if n is not matched, do not mark the packets.*/
				for(j = 0; j < SVC_WAN_MAX_PVC_NUM; j++) 
				{
					memset(nodeName, 0, sizeof(nodeName));			
					snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, j + 1);
#if !defined(TCSUPPORT_CT_E8B_ADSL) && !defined(TCSUPPORT_CMCC)
					if(cfg_get_object_attr(nodeName, "VLANMode", vlanMode, sizeof(vlanMode)) <= 0)
					{
						continue;
					}	
					if(strcmp(vlanMode,"TAG") && strcmp(vlanMode,"tag"))
					{
						continue;
					}
					if(cfg_get_object_attr(nodeName, "VLANID", vlanID, sizeof(vlanID)) <= 0)
					{
						SVC_WAN_ERROR_INFO("vlanbind_write:get VLANID failed\n");
						goto ERROR;
					}	
					if(vlanMap.n != atoi(vlanID))
					{
						continue;
					}
#endif
				/* Here got the matched PVC according to the VLAN ID.*/
					isBridgeEntry = 0;
					for(k = 0; k < SVC_WAN_MAX_ENTRY_NUM; k++) 
					{
						memset(nodeName, 0, sizeof(nodeName));
						snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, j + 1, k + 1);
#if defined(TCSUPPORT_CMCC)
						if(cfg_get_object_attr(nodeName, "VLANMode", vlanMode, sizeof(vlanMode)) <= 0 )
						{
							continue;
						}

						if (0 != strcmp(vlanMode,"TAG") && 0 != strcmp(vlanMode,"tag"))
							continue;

						if (cfg_get_object_attr(nodeName, "VLANID", vlanID, sizeof(vlanID)) <= 0)
						{
							SVC_WAN_ERROR_INFO("vlanbind_write:get VLANID failed\n");
							goto ERROR;
						}

						if(vlanMap.n != atoi(vlanID) )
							continue;
#endif
						/*If there is no active route entry, use the first interface id to mark the packets.*/
						if(cfg_get_object_attr(nodeName, "Active", entry_active, sizeof(entry_active)) <= 0)
						{
							continue;
						}	
						if(cfg_get_object_attr(nodeName, "WanMode", entry_wanMode, sizeof(entry_wanMode)) <= 0)
						{
							SVC_WAN_ERROR_INFO("vlanbind_write:get entry active failed\n");
							goto ERROR;
						}	
						if(!strcmp(entry_wanMode,"Route") || !strcmp(entry_wanMode,"route"))
						{
							if( !strcmp(entry_active,"Yes") || !strcmp(entry_active,"yes"))
							{
								isBridgeEntry = 0;
								mark = (j*8 + k + 1) << 16;
								snprintf(cmdBuf, sizeof(cmdBuf), "%s -A vbind_entry%d_chain -i %s -p 802_1Q --vlan-id %d -j mark --mark-or 0x%x --mark-target CONTINUE\n", \
									EBTABLES_BROUTE, i, lanIf, vlanMap.m, mark);

								fputs_escape(cmdBuf, fp);
								break;	
							}
						}
						else 
						{
							if(!strcmp(entry_active,"Yes") || !strcmp(entry_active,"yes"))
							{
#if defined(TCSUPPORT_CT_JOYME2) && !defined(TCSUPPORT_CFG_NG_UNION)
								bzero(node_val, sizeof(node_val));
								if ( cfg_get_object_attr(nodeName, "ServiceList"
										, node_val, sizeof(node_val)) > 0
									&& strstr(node_val, "OTHER") )
								{
									snprintf(str_lan_vlan_id, sizeof(str_lan_vlan_id), "%d", vlanMap.m);
									cfg_set_object_attr(nodeName, "lan_vlan_id", str_lan_vlan_id);
								}
#endif
								isBridgeEntry = 1;
#if defined(TCSUPPORT_CMCC)
								break;
#endif
							}
						}
					}

#if !defined(TCSUPPORT_CMCC)
					if((k == MAX_PVC_ENTRY))
					{
#endif
						if (isBridgeEntry)
						{
#if defined(TCSUPPORT_CMCC)
							mark = (j*8 + k + 1) << 16;
#else
							mark = (j*8 + 1) << 16;
#endif
							snprintf(cmdBuf, sizeof(cmdBuf), "%s -A vbind_entry%d_chain -i %s -p 802_1Q --vlan-id %d -j mark --mark-or 0x%x --mark-target CONTINUE\n", 
								EBTABLES_BROUTE, i, lanIf, vlanMap.m, mark);
							fputs_escape(cmdBuf, fp);
						}					
						else
							continue;
#if !defined(TCSUPPORT_CMCC)
					}					
#endif
				/*Already got the PVC according VLAN ID, no need to check the rest of them.*/
					break;
				}

				pStrVlanMap = strtok_r(NULL, delims, &savePtr);
				if(vlanMap.m != vlanMap.n && vlanMap.n == atoi(vlanID))
				{
					blapi_traffic_set_traffic_class("VLANID", vlanMap.m, vlanMap.n, ADD_TRAFFIC_CLASS);
				}
			}


	
		}
		else 
		{
			entryActive &= ~(1<<i);
			/*eg:Change vlan bind to port bind, we should clean the ebtables entries.*/
			snprintf(cmdBuf, sizeof(cmdBuf), "%s -D BROUTING -j vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
			fputs(cmdBuf, fp);	
			snprintf(cmdBuf, sizeof(cmdBuf), "%s -X vbind_entry%d_chain\n", EBTABLES_BROUTE, i);
			fputs(cmdBuf, fp);
		}
		
	}
#if defined(TCSUPPORT_CMCC)
	for(i = 0; i < MAX_LAN_PORT_NUM; i++)
	{
		int group = 0;
		for(j = 0; j < MAX_LAN_PORT_NUM; j++)
		{
			if(checkVlanBindGroup(vlanMapArray,i,j))
			{
				group |= (1 << j);
			}
		}
		if(group)
		{
			snprintf(cmdBuf, sizeof(cmdBuf), "echo %d %d > %s\n", i, group, VBIND_ENTRY_GROUP_PATH);
			svc_wan_execute_cmd(cmdBuf);
		}
		if(((group & 0xf) == 0xf) && (i < 4))
			port_group[i] = 0;
	}
	/*port_group[] is array of groupid, groupid means ports in which group.
	*Ports in the same group can contact to each other
	*Ports whose groupid is 0 means default portid, and can contact to any port.
	*/
	
	for(i = 0; i < 4; i++)
	{
		if(port_group[i] == -1)
		{
			port_group[i] = groupid;
			groupid ++;
		}
		else
			continue;
		for(j = (i+1); j < 4; j++)
		{
			if((checkVlanBindGroup(vlanMapArray,i,j)))
			{
				if(port_group[j] == -1)
					port_group[j] = port_group[i];
			}
		}
	}
	macMT7530SetPortMatrix(port_group, 1);
	svc_wan_execute_cmd("/userfs/bin/hw_nat -!");
#endif
	snprintf(cmdBuf, sizeof(cmdBuf), "echo %d > %s\n", entryActive, VBIND_ENTRY_ACTIVE_PATH);
	svc_wan_execute_cmd(cmdBuf);		
	fclose(fp);
	set_port_binding_info(obj,flag, PORT_BIND_OP_ALL);

	update_vlan_bind_dhcp_relay_rule();

	return 0;

ERROR:
	fclose(fp);

	return -1;
}

int svc_wan_vlanbind_execute(wan_entry_obj_t* obj,int flag)
{
#if defined(TCSUPPORT_CT_VLAN_BIND)
	int res = 0;
	svc_wan_vlanbind_write(obj,flag);
	res = chmod(VLAN_BIND_SH, 777); 
	if(res){
		/*do nothing,just for compile*/
	}
	svc_wan_execute_cmd(VLAN_BIND_SH);
#endif
	return 0;
}






