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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <cfg_cli.h>
#include "../utility.h"
#include "cfg_types.h" 
#include "../../wan/static_route.h"


#define ROUTE_ADD 			1
#define ROUTE_DEL 			0

static char* cfg_type_route_index[] = { 
	 "route_id", 
	 NULL 
}; 

static int cfg_type_route_read(char* path, char* attr)
{
	char route_nodeName[128] = {0};  
	cfg_node_type_t* group 	= NULL;  
	int static_route_index 	= 0;
	char addnum[4] = {0};
	char count[4] = {0};
	char value[8] = {0};
	int i = 0;
	int norecord = 0;

	if(0 != strcmp(attr, "Route_num") && 0 != strcmp(attr, "add_num"))
	{
		return SUCCESS;
	}	

	group = cfg_type_query(path);
	if(group == NULL)
	{
		tcdbg_printf("\r\nreturn fail");
		return FAIL;
	}

	for(i = 0; i < MAX_STATIC_ROUTE_NUM; i++)
	{
		snprintf(route_nodeName, sizeof(route_nodeName), ROUTE_ENTRY_NODE, i+1);
		memset(value,0,sizeof(value));
		cfg_obj_get_object_attr(route_nodeName, "Active", 0, value, sizeof(value));
		if ('\0' == value[0])
		{
			if(!norecord)
			{
				snprintf(addnum, sizeof(addnum), "%d", i);
				cfg_set_object_attr(path, "add_num", addnum);
				norecord++;
			}
		}
		else
		{
			static_route_index++;
		}	
	}
	
	snprintf(count, sizeof(count), "%d", static_route_index);
	cfg_set_object_attr(path, "Route_num", count);
	cfg_set_object_attr(path, "User_def_num", count);

	return SUCCESS;
}

static int cfg_type_route_func_get(char* path,char* attr, char* val,int len) 
{ 
	cfg_type_route_read(path, attr);
	return cfg_type_default_func_get(path,attr,val,len); 
} 

static int create_route_exe_sh(int action)
{
	char node_name[128] 	= {0};
	char route_tmp[16]		= {0};
	char tmp[32]			= {0};
	char string[128]		= {0};
	char wan_active[6]    	= {0};
	char route_active[8] 	= {0};
	FILE *fp 				= NULL;
	int route_id;
	int wan_device_index;
	int res = 0;

	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	/*get route id*/
	memset(route_tmp, 0, sizeof(route_tmp));
	if(cfg_obj_get_object_attr(node_name, "route_id", 0, route_tmp, sizeof(route_tmp)) > 0)
	{
		route_id = atoi(route_tmp);
	}
	else
	{
		return -1;
	}

	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), ROUTE_ENTRY_NODE, route_id + 1);
	if(action == ROUTE_ADD)
	{
		strncpy(string, "/sbin/route add ", sizeof(string) -1);
		memset(route_active, 0, sizeof(route_active));
		/* default will be active. */
		if (cfg_obj_get_object_attr(node_name, "Active", 0, route_active, sizeof(route_active)) < 0)
		{
			strncpy(route_active, "Yes", sizeof(route_active) - 1);
			cfg_set_object_attr(node_name, "Active", route_active);
		}

		if (0 != strcmp(route_active, "Yes"))
		{
			strncpy(string, "#/sbin/route add ", sizeof(string) - 1); /* mark add operation. */
		}
	}
	else if(action == ROUTE_DEL)
	{
		strncpy(string, "/sbin/route del ", sizeof(string) -1 );
	}
	else
	{
		return -1;
	}
	/*write dest ip address*/
	memset(route_tmp, 0, sizeof(route_tmp));
	if(cfg_obj_get_object_attr(node_name, "DST_IP", 0, route_tmp, sizeof(route_tmp)) > 0)
	{
		snprintf(tmp, sizeof(tmp), "-net %s ", route_tmp);
		strcat(string, tmp);
	}
	else
	{
		/*node has been delete*/
		return -1;
	}
	/*write sub mask*/
	memset(route_tmp, 0, sizeof(route_tmp));
	if(cfg_obj_get_object_attr(node_name, "Sub_mask", 0, route_tmp, sizeof(route_tmp)) > 0)
	{
		snprintf(tmp, sizeof(tmp), "netmask %s ", route_tmp);
		strcat(string, tmp);
	}
	/*write device info*/
	memset(route_tmp, 0, sizeof(route_tmp));
	if(cfg_obj_get_object_attr(node_name, "Device", 0, route_tmp, sizeof(route_tmp)) > 0)
	{
		if(strcmp(route_tmp, "br0") != 0)
		{		
			/*Device value is wan interface name, eg:nas2_1*/
			/*Check wan status*/
			if((wan_device_index = get_wanindex_by_name(route_tmp)) < 0)
			{
				tcdbg_printf("\n %s:get wan index from %s failed!\n", __func__, route_tmp);
				return -1;
			}
			
			if(get_waninfo_by_index(wan_device_index, "Active", wan_active, sizeof(wan_active)) == SUCCESS)
			{
				if(!strcmp(wan_active, "No"))
				{
					tcdbg_printf("\n %s:%s is not active,add static route failed!\n", __func__, route_tmp);
					return -1; 		
				}		
			}
			else
			{
				return -1;
			}
		}

		snprintf(tmp, sizeof(tmp), "dev %s ", route_tmp);
		strcat(string, tmp);
	}
	/*write gateway info*/
	memset(route_tmp, 0, sizeof(route_tmp));
	if(cfg_obj_get_object_attr(node_name, "Gateway", 0, route_tmp, sizeof(route_tmp)) > 0)
	{
		if(strcmp(route_tmp,"0.0.0.0"))
		{
			snprintf(tmp, sizeof(tmp), "gw %s ", route_tmp);
			strcat(string, tmp);
		}
	}
	/*write metric value*/
	memset(route_tmp, 0, sizeof(route_tmp));
	if(cfg_obj_get_object_attr(node_name, "metric", 0, route_tmp, sizeof(route_tmp)) > 0)
	{
		if(atoi(route_tmp) > 0)
		{
			snprintf(tmp, sizeof(tmp), "metric %s ", route_tmp);
			strcat(string, tmp);
		}
	}

	fp = fopen(ROUTE_EXE_SH, "a+");
	if(fp != NULL)
	{
		strcat(string, "\n");
		fputs_escape(string, fp);
		fclose(fp);
		res = chmod(ROUTE_EXE_SH,777);
	}
	
	return 0;
}


static void create_route_sh(void)
{
	char tmp[32]		= {0};
	char string[128]	= {0};
	char node_name[128]	= {0};
	char Value[32] 		= {0};
	FILE *fp 			= NULL;
	int route_count		= 0;
	struct cfg_node_type* node = NULL;
	int res = 0;

	unlink(ROUTE_PATH);
	for(route_count = 0; route_count < MAX_STATIC_ROUTE_NUM; route_count++)
	{
		/*sprintf(element_name, ROUTE_ENTRY, route_count);*/
		snprintf(node_name, sizeof(node_name), ROUTE_ENTRY_NODE, route_count);
		node = cfg_type_query(node_name);
		if(NULL == node)
		{	
			continue;
		}
		memset(string, 0,sizeof(string));
		strncpy(string, "/sbin/route add ", sizeof(string) - 1);
		memset(Value, 0, sizeof(Value));
		cfg_obj_get_object_attr(node_name, "Active", 0, Value, sizeof(Value));
		if (0 != strcmp(Value, "Yes"))
		{
			strcpy(string, "#/sbin/route add "); /* mark add operation. */
		}
		/*write destnation IP*/
		memset(Value, 0, sizeof(Value));
		cfg_obj_get_object_attr(node_name, "DST_IP", 0, Value, sizeof(Value));
		if(strlen(Value) > 0)
		{
			snprintf(tmp, sizeof(tmp), "-net %s ", Value);
			strcat(string, tmp);
		}
		/*write mask*/
		memset(Value, 0, sizeof(Value));
		cfg_obj_get_object_attr(node_name, "Sub_mask", 0, Value, sizeof(Value));
		if(strlen(Value) > 0)
		{
			snprintf(tmp, sizeof(tmp), "netmask %s ", Value);
			strcat(string, tmp);
		}
		/*write device information*/
		memset(Value, 0, sizeof(Value));
		cfg_obj_get_object_attr(node_name, "Device", 0, Value, sizeof(Value));
		if(strlen(Value) > 0)
		{
			snprintf(tmp, sizeof(tmp), "dev %s ", Value);
			strcat(string, tmp);
		}
		/*write Gateway information*/
		memset(Value, 0, sizeof(Value));
		cfg_obj_get_object_attr(node_name, "Gateway", 0, Value, sizeof(Value));
		if((0 != strcmp(Value, "0.0.0.0")))
		{
			snprintf(tmp, sizeof(tmp), "gw %s ", Value);
			strcat(string, tmp);
		}
		/*write metric number if number is bigger than 0.*/
		memset(Value, 0, sizeof(Value));
		cfg_obj_get_object_attr(node_name, "metric", 0, Value, sizeof(Value));
		if(0 != (strcmp(Value,"0")))
		{
			snprintf(tmp, sizeof(tmp), "metric %s ", Value);
			strcat(string, tmp);
		}
		strcat(string, "\n");
		fp = fopen(ROUTE_PATH, "a");
		if(fp != NULL)
		{
			fputs_escape(string, fp);
			fclose(fp);
		}
	}
	
	res = chmod(ROUTE_PATH,777);
	
	return ;
}

static void update_route_sh(void)
{
	FILE *exeSh			= NULL;
	FILE *routeSh		= NULL;
	FILE *tmpFp			= NULL;
	char line[MAXSIZE]	= {0};
	char tmp[MAXSIZE]	= {0};
	char *dest_ip_exe	= NULL;
	char *p				= NULL;
	char *delim			= " ";
	int action			= 0;
	int res				= 0;

	exeSh=fopen(ROUTE_EXE_SH, "r");
	if(!exeSh)
	{
		return;
	}
	
	routeSh=fopen(ROUTE_PATH, "a+");
	if(!routeSh)
	{
		fclose(exeSh);
		return;
	}

	fgets(line, MAXSIZE, exeSh);
	/*use the route_exe_sh to determin the action.*/
	if(strlen(line))
	{
		if('#' != line[0] && strstr(line,"/sbin/route add"))
		{
			action = ROUTE_ADD;
		}
		else
		{
			action = ROUTE_DEL;
		}
	}
	else
	{
		fclose(exeSh);
		fclose(routeSh);
		return;
	}
	/*action add: just put the line of route_exe_sh into route.sh*/
	if(ROUTE_ADD == action)
	{
		strcat(line, "\n");
		fputs(line, routeSh);
		fclose(exeSh);
		fclose(routeSh);
	}
	/*action del*/
	else if(ROUTE_DEL == action)
	{
		strcpy(tmp, line);
		p=strtok(tmp, delim);
		/*get the ip address */
		while(p != NULL)
		{
			p = strtok(NULL, delim);
			if(!strcmp(p,"-net"))
			{
				dest_ip_exe = strtok(NULL, delim);
				break;
			}
		}

		tmpFp=fopen(ROUTE2_PATH,"w");
		if(!tmpFp)
		{
			fclose(exeSh);
			fclose(routeSh);
			return;
		}

		while(fgets(line,MAXSIZE,routeSh))
		{
			if(dest_ip_exe && !strstr(line,dest_ip_exe))
			{
				/*if the dest_ip is not the same, we can put it into route2.sh*/
				fputs(line,tmpFp);
			}
		}

		fclose(exeSh);
		fclose(routeSh);
		fclose(tmpFp);
		unlink(ROUTE_PATH);
		res = rename(ROUTE2_PATH, ROUTE_PATH);
	}
	
  	return;
}

static int getSubnetInfo(int if_index, char *out_subip, int len_subip, char *out_netmask, 
							int len_netmask, char *out_gateway, int len_gateway)
{
	char wanNode[128] 		= {0};
	char wanInfoNode[128] 	= {0};
	char gate_way[32] 		= {0};
	char net_mask[32] 		= {0};
	char sub_ip[32] 		= {0};
	char linkmode[32] 		= {0};
	int pvc_index = 0, entry_index = 0;
	int isPPP = 0;
	struct in_addr gatewayIP;
	struct in_addr netmask;
	struct in_addr subIP;
	
	if(NULL == out_gateway || NULL == out_subip || NULL == out_netmask )
		return -1;

	pvc_index = if_index / MAX_SMUX_NUM + 1;
	entry_index = if_index % MAX_SMUX_NUM + 1;

	memset(wanNode, 0, sizeof(wanNode));
	//BuffSet_Str(wanNode[0], "Wan");
	//BuffSet_Format(wanNode[1], "PVC%d", pvc_index);
	//BuffSet_Format(wanNode[2], "Entry%d", entry_index);
	snprintf(wanNode, sizeof(wanNode), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
	memset(linkmode, 0, sizeof(linkmode));
	if(cfg_obj_get_object_attr(wanNode, "LinkMode", 0, linkmode, sizeof(linkmode)) <= 0)
		return -1;

	if(strcmp(linkmode, "linkPPP") == 0)
	isPPP = 1;

	memset(wanInfoNode, 0, sizeof(wanInfoNode));
	//BuffSet_Str(wanInfoNode[0], "WanInfo");
	//BuffSet_Format(wanInfoNode[1], "Entry%d", if_index);
	snprintf(wanInfoNode, sizeof(wanInfoNode), WANINFO_ENTRY_NODE, if_index+1);
	memset(gate_way, 0, sizeof(gate_way));
	if(cfg_obj_get_object_attr(wanInfoNode, "GateWay", 0, gate_way, sizeof(gate_way)) <= 0
		|| 0 == gate_way[0])
	{
		return -1;
	}	
	
	memset(net_mask, 0, sizeof(net_mask));
	if(cfg_obj_get_object_attr(wanInfoNode, "NetMask", 0, net_mask, sizeof(net_mask)) <= 0
		|| 0 == net_mask[0] )
	{
		return -1;
	}

	if (isPPP)
	{
		//BuffSet_Str(sub_ip, gate_way);
		//BuffSet_Str(net_mask, "255.255.255.255");
		snprintf(sub_ip, sizeof(sub_ip), "%s", gate_way);
		snprintf(net_mask, sizeof(net_mask), "255.255.255.255");
	}
	else
	{
		inet_aton(gate_way, &gatewayIP);
		inet_aton(net_mask, &netmask);
		subIP.s_addr = (gatewayIP.s_addr & netmask.s_addr);
		snprintf(sub_ip, sizeof(sub_ip), "%s", inet_ntoa(subIP));
	}

	snprintf(out_subip, len_subip, "%s", sub_ip);
	snprintf(out_netmask, len_netmask, "%s", net_mask);
	snprintf(out_gateway, len_gateway, "%s", gate_way);

	return 0;
}

static int procLinkLocalRoute(char* if_name, int if_index, int add_del, char *out_subip, 
									int len_subip, char *out_netmask, int len_netmask)
{
	char gate_way[32] 	= {0};
	char net_mask[32] 	= {0};
	char sub_ip[32] 	= {0};
	char route_cmd[128] = {0};
	
	if (NULL == if_name )
		return -1;

	if ( 0 != getSubnetInfo(if_index, sub_ip, sizeof(sub_ip), net_mask, sizeof(net_mask)
									, gate_way, sizeof(gate_way)))
	{
		tcdbg_printf("error :getSubnetInfo faile\n");
		return -1;
	}
	if (add_del)
	{
		snprintf(route_cmd, sizeof(route_cmd)
			, "/sbin/route add -net %s netmask %s dev %s"
			, sub_ip, net_mask, if_name);
	}
	else
	{
		snprintf(route_cmd, sizeof(route_cmd)
			, "/sbin/route del -net %s netmask %s dev %s"
			, sub_ip, net_mask, if_name);
	}

	snprintf(out_subip, len_subip, "%s", sub_ip);
	snprintf(out_netmask, len_netmask, "%s", net_mask);
	system_escape(route_cmd);
	return 0;
}

static int checkSameSubnet(char *in_dstaddr, char *in_submask, char *in_dev)
{
	int route_idx = 0;
	char active_s[16] = {0}, dst_ip_s[32] = {0}, submask_s[32] = {0};
	char dev_s[32] = {0};
	char nodename[128] = {0};

	if (NULL == in_dstaddr || NULL == in_submask || NULL == in_dev )
		return 0;

	memset(nodename, 0, sizeof(nodename));
	for (route_idx = 0; route_idx < MAX_STATIC_ROUTE_NUM; route_idx ++)
	{
		snprintf(nodename, sizeof(nodename), ROUTE_ENTRY_NODE, route_idx+1);
		memset(active_s, 0, sizeof(active_s));
		if(cfg_obj_get_object_attr(nodename, "Active", 0, active_s, sizeof(active_s)) < 0
			|| 0 != strcmp(active_s, "Yes"))
		{
			continue;
		}	
	
		memset(dst_ip_s, 0, sizeof(dst_ip_s));
		memset(submask_s, 0, sizeof(submask_s));
		memset(dev_s, 0, sizeof(dev_s));
		
		cfg_obj_get_object_attr(nodename, "DST_IP", 0, dst_ip_s, sizeof(dst_ip_s));
		cfg_obj_get_object_attr(nodename, "Sub_mask", 0, submask_s, sizeof(submask_s));
		cfg_obj_get_object_attr(nodename, "Device", 0, dev_s, sizeof(dev_s));

		if ( 0 == strcmp(in_dstaddr, dst_ip_s)
			&& 0 == strcmp(in_submask, submask_s)
			&& 0 == strcmp(in_dev, dev_s) )
		{
			return 1;
		}	
	}

	return 0;
}

static int	wan_related_route_execute(void)
{
	FILE *fp = NULL;
	FILE *fp_r = NULL;
	char wanNode[128] = {0};
	int ii = 0, pvc_index = 0, entry_index = 0;
	char def_route_index_v4[10] = {0};
	int i_def_route_index_v4 = -1;
	char str_servicelist[64] = {0}, iface_name[16] = {0};
	int store_add_idx[MAX_WAN_IF_INDEX] = {0};
	char dst_ip[32] = {0}, netmask[32] = {0};
	char *line = NULL;
	int len = 0;

	/* get default route for ipv4 */
	memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
	memset(wanNode, 0, sizeof(wanNode));
	snprintf(wanNode, sizeof(wanNode), WANINFO_COMMON_NODE);
	memset(def_route_index_v4, 0, sizeof(def_route_index_v4));	
	if (cfg_obj_get_object_attr(wanNode, "DefRouteIndexv4", 0, def_route_index_v4, sizeof(def_route_index_v4)) > 0
		&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A"))
	{
		i_def_route_index_v4 = atoi(def_route_index_v4);
	}
	/* Add Link Route for backup INTERNET connection, 
	otherwise static route will be added failed. */
	memset(store_add_idx, 0, sizeof(store_add_idx));
	for (ii = 0; ii < MAX_WAN_IF_INDEX; ii++)
	{
		if (i_def_route_index_v4 == ii)
		{
			continue;
		}

		pvc_index = ii / MAX_SMUX_NUM;
		entry_index = ii - pvc_index * MAX_SMUX_NUM;
		memset(wanNode, 0, sizeof(wanNode));
		snprintf(wanNode, sizeof(wanNode), WAN_PVC_ENTRY_NODE, pvc_index+1, entry_index+1);
		memset(str_servicelist, 0, sizeof(str_servicelist));
		if(cfg_obj_get_object_attr(wanNode, "ServiceList", 0, str_servicelist, sizeof(str_servicelist)) <= 0)
		{
			continue;
		}
		
		if(strstr(str_servicelist, "INTERNET"))
		{
			memset(iface_name, 0, sizeof(iface_name));
			if (cfg_obj_get_object_attr(wanNode, "IFName", 0, iface_name, sizeof(iface_name)) <= 0)
			{
				continue;
			}
			memset(dst_ip, 0, sizeof(dst_ip));
			memset(netmask, 0, sizeof(netmask));
			procLinkLocalRoute(iface_name, ii, 1, dst_ip, sizeof(dst_ip), netmask, sizeof(netmask));
			store_add_idx[ii] = 1;

			/* will not delete the link route when same subnet exist in static rules. */
			if ( 1 == checkSameSubnet(dst_ip, netmask, iface_name))
			{
				store_add_idx[ii] = 0;
			} 		
		}
	}

	fp = fopen(ROUTE_EXE_SH, "r");
	if(fp != NULL)
	{
		fp_r = fopen(ROUTE_EXE_PRE_SH, "r");
		if(fp_r)
		{
			fclose(fp_r);
			fp_r = NULL;
			system(ROUTE_EXE_PRE_SH);
			unlink(ROUTE_EXE_PRE_SH);
		}

		while ( getline(&line, &len, fp) != -1 )
		{		
			if ( line && 0 != line[0])
			system(line);
		}

		if ( line )
			free(line);

		fclose(fp);
		unlink(ROUTE_EXE_SH);

		fp_r = fopen(ROUTE_EXE_AFT_SH, "r");
		if (NULL != fp_r)
		{
			fclose(fp_r);
			fp_r = NULL;
			system(ROUTE_EXE_AFT_SH);
			unlink(ROUTE_EXE_AFT_SH);
		}
	}
	/* Remove temp Link Route. */
	for (ii = 0; ii < MAX_WAN_IF_INDEX; ii++)
	{
		if (1 != store_add_idx[ii])
		{
			continue;
		}

		pvc_index = ii / MAX_SMUX_NUM;
		entry_index = ii - pvc_index * MAX_SMUX_NUM;
		memset(wanNode, 0, sizeof(wanNode));
		//strcpy(wanNode[0], "Wan");
		//sprintf(wanNode[1], "PVC%d", pvc_index);
		//sprintf(wanNode[2], "Entry%d", entry_index);
		snprintf(wanNode, sizeof(wanNode), WAN_PVC_ENTRY_NODE,
		pvc_index + 1, entry_index + 1);

		memset(dst_ip, 0, sizeof(dst_ip));
		memset(netmask, 0, sizeof(netmask));
		memset(iface_name, 0, sizeof(iface_name));		
		if (cfg_obj_get_object_attr(wanNode, "IFName", 0, iface_name, sizeof(iface_name)) <= 0)
		{
			continue;
		}		

		procLinkLocalRoute(iface_name, ii, 0, dst_ip, sizeof(dst_ip)
			, netmask, sizeof(netmask));
	}

	return SUCCESS;
}

static int cfg_type_route_func_commit(char* path)
{
	FILE *fp = NULL;
	
	/*krammer move create_route_exe_sh up for update_route_sh*/
	fp = fopen(ROUTE_EXE_SH, "r");
	if(NULL == fp)
	{
		create_route_exe_sh(ROUTE_ADD);
	}
	else
	{
		fclose(fp);
	}

	fp=fopen(ROUTE_PATH,"r");
	if(fp == NULL)
	{
		create_route_sh();/*only for boot time*/
	}
	else
	{
		update_route_sh();/*after boot time*/
		fclose(fp);
	}
	
	wan_related_route_execute();

#if defined(TCSUPPORT_CWMP_TR181)
	syncRouteFwd4ByRouteNode(path);
#endif
	return SUCCESS;
}

static int cfg_type_route_entry_pre_unset()
{
	return create_route_exe_sh(ROUTE_DEL);	
}

static int cfg_type_route_entry_func_delete(char* path)
{
	cfg_type_route_entry_pre_unset();
	cfg_type_route_func_commit(path);
	
#if defined(TCSUPPORT_CWMP_TR181)
	delRouteFwd4ByRouteNode(path);
#endif

	return cfg_type_default_func_delete(path);
}

static cfg_node_ops_t cfg_type_route_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_route_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_route_func_commit 
}; 


static cfg_node_type_t cfg_type_route_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_ROUTE_ENTRY_NUM, 
	 .parent = &cfg_type_route, 
	 .index = cfg_type_route_index, 
	 .ops = &cfg_type_route_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_route_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_route_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_route_func_commit 
}; 


static cfg_node_type_t* cfg_type_route_child[] = { 
	 &cfg_type_route_entry, 
	 NULL 
}; 


cfg_node_type_t cfg_type_route = { 
	 .name = "Route", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_route_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_route_child, 
	 .ops = &cfg_type_route_ops, 
}; 

