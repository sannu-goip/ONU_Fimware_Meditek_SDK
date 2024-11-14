#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <stdarg.h>
#include "utility.h"
#include "wan_common.h"
#include "global_def.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#include <lan_port/bind_list_map.h>
#endif

int g_svc_wan_level = E_NO_INFO_LEVEL;

void svc_wan_printf(char *fmt,...)
{
    FILE *proc_file = NULL;
    char msg[512];
    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 512, fmt, args);	

    proc_file = fopen("/proc/tc3162/dbg_msg", "w");
    if (NULL == proc_file) 
    {
        printf("open /proc/tc3162/dbg_msg fail\n");
		va_end(args);
        return;
    }
    
    fprintf(proc_file, "%s", msg);
    fclose(proc_file);
    va_end(args);
    
    return ;
}

void svc_wan_debug_level(int level)
{
    g_svc_wan_level = level;
    printf("debug level = %d.\n", g_svc_wan_level);

    return ;
}

/*
	-1/0 :  not right get info
	> 0:     right get info
*/
int svc_wan_get_file_string(char *file_path, char line_return[][MAX_INFO_LENGTH], int line_num)
{
    int i = 0;
    FILE* fp = NULL;
    char line_buf[MAX_INFO_LENGTH];

    if( (NULL== file_path) || (line_num < 0))
    {
        return -1;
    }

    fp = fopen(file_path, "r");
    if(NULL == fp)
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

int check_ip_format(char *ip_arg)
{
    int z;
    char ip[16]={0};
    char * pattern;
    regex_t reg;
    regmatch_t pm[10];
    const size_t nmatch = 10;

    strncpy(ip, ip_arg, sizeof(ip) - 1);
    /*fprintf(stderr,"ip=%s\n", ip);*/

    /*set regular expression of ip format*/
    pattern ="([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})";
    /*compile this regular expression*/
    z = regcomp(&reg, pattern, REG_EXTENDED);

    /*if compile fails, free its memory and return false*/
    if (z != 0)
    {
        fprintf(stderr, "Failed to open regcomp.\n");
        regfree(&reg);
        return 0;
    }

    /*compare input string base on previous syntax*/
    z = regexec(&reg, ip, nmatch, pm, 0);
    /*if format is invalid, free its memory and return false*/
    if (z == REG_NOMATCH) 
    {
        fprintf(stderr, "IP format is wrong.\n");
        regfree(&reg);
        return 0;
    }

    /*free memory anyway*/
    regfree(&reg);

    return 1;
}

int check_mask_format(char *mask_arg, char *mask_decimal,int len)
{
    int i;
    int mask_counter = 0;
    char mask[16]={0};
    char *p[4];
    const char *delim = ".";
    unsigned int sample = 0x0;
    unsigned int binary_mask = 0x0;
    const unsigned int bit_1_mask = 0x1;

    strncpy(mask, mask_arg, sizeof(mask)-1);
    /*check is this mask a valide ip format*/
    if(!check_ip_format(mask))
    {
        return 0;
    }

    /*divied mask into four token*/
    for(i=0; i<4; i++)
    {
        if(i==0)
        {
            p[i] = strtok(mask, delim);
        }
        else
        {
            p[i] = strtok(NULL, delim);
        }
    }

    /*transfer mask to bit type ex. 255 -> 11111111*/
    for(i=0; i<4; i++)
    {
        binary_mask |= atoi(p[i]);

        if(i!=3)
        {
            binary_mask <<= 8;
        }
    }

    /*check is this mask a valid mask ex. 11111011000 is invalid*/
    for(i=0; i<32; i++)
    {
        sample = binary_mask & bit_1_mask;
        if(sample == 0x0)
        {
            if(mask_counter != 0)
            {
                return 0;
            }
        }
        else
        {
            mask_counter++;
        }
        binary_mask >>= 1;
    }

    snprintf(mask_decimal,len, "%d", mask_counter);
    return 1;
}

int isValidDnsIp(char * dnsIP)
{
    if(!check_ip_format(dnsIP))
    {
        /* dnsIp is invalid */
        SVC_WAN_ERROR_INFO("isValidDnsIp:dnsIp is invalid!\r\n");
        return 0;
    }

    if((strcmp(dnsIP,"255.255.255.255") == 0) || (strcmp(dnsIP,"0.0.0.0") == 0))
    {
        SVC_WAN_ERROR_INFO("isValidDnsIp:dnsIp = %s is invalid!\r\n",dnsIP);
        return 0;
    }

    return 1;
}

int svc_wan_check_xpon_up(void)
{
    return 1;
}

void do_val_put(char *path, char *value)
{
    FILE* fp = NULL;

    if((NULL == path) || (NULL == value))
    {
        SVC_WAN_ERROR_INFO("path or value can't be NULL.\n");
        return ;
    }

    fp = fopen(path,"w");
    if(NULL == fp)
    {
        SVC_WAN_DEBUG_INFO("path = %s.\n", path);
        return ;
    }

    fprintf(fp, value);
    fclose(fp);

    return ;
}


int svc_wan_get_pid(char* file)
{

    FILE* fp = NULL;
    int pid = 0, res = 0;

    if ( NULL == (fp = fopen(file,"r")) )
    {
        return 0;
    }

    res = fscanf(fp,"%d", &pid);
	if(res){
		/*do nothing,just for compile*/
	}
    fclose(fp);

    return pid;
}

int svc_wan_execute_cmd(char* cmd)
{
    SVC_WAN_DEBUG_INFO("cmd = %s.\n", cmd);

	if ( -1 == check_injection_code_command(cmd, strlen(cmd), 1) )
		return -1;

    system(cmd);
    return 0;
}

int svc_wan_execute_cmd_normal(char* cmd)
{
    SVC_WAN_DEBUG_INFO("cmd = %s.\n", cmd);

    system(cmd);
    return 0;
}

int svc_wan_kill_process(char *path, char release_source)
{
    int pid = 0;
    char cmd[SVC_WAN_BUF_256_LEN];

    if(NULL == path)
    {
        SVC_WAN_ERROR_INFO("kill process path can't NULL.\n");
        return -1;
    }

    if(release_source)
    {
        pid = svc_wan_get_pid(path);
        if(pid > 0)
        {
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "/bin/kill -15 %d", pid);
            svc_wan_execute_cmd(cmd);
        }
        sleep(1);
    }


    pid = svc_wan_get_pid(path);
    if(pid > 0)
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "/bin/kill -9 %d" ,pid);
        svc_wan_execute_cmd(cmd);
    }

    return 0;
}

int svc_wan_intf_name(wan_entry_obj_t* obj, char* name, int size)
{
    wan_entry_cfg_t * cfg = &obj->cfg;

   if (!SVC_WAN_IS_BRIDGE(cfg) && SVC_WAN_IS_PPP(cfg))
    {
        snprintf(name, size, "ppp%d", obj->idx);
    }
    else
    {
    	snprintf(name, size, obj->dev);
    }

    return 0;
}


int svc_wan_add_default_ipv4_route(char* gw,char* dev)
{

    char cmd[SVC_WAN_BUF_256_LEN];

    if (gw[0] == 0 || dev[0] == 0)
        return 0;
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/route add default gw %s dev %s", gw, dev);
    
    svc_wan_execute_cmd(cmd);

    return 0;
}

int svc_wan_delete_default_ipv4_route(void)
{

    char cmd[SVC_WAN_BUF_256_LEN];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/bin/ip route delete default");

    svc_wan_execute_cmd(cmd);

    return 0;
}

int svc_wan_add_default_ipv6_route(char* gw,char* dev)
{

    char cmd[SVC_WAN_BUF_256_LEN];

    if (gw[0] == 0 || dev[0] == 0)
    {
        return 0;
    }
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/route -A inet6 add default gw %s dev %s", gw, dev);
    svc_wan_execute_cmd(cmd);

    return 0;
}

int svc_wan_delete_default_ipv6_route(void)
{

    char cmd[SVC_WAN_BUF_256_LEN];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/bin/ip -6 route delete default");
    svc_wan_execute_cmd(cmd);

    return 0;
}

#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
static int ondemand_route_state[MAX_WAN_IF_INDEX] = {0};
void set_ondemand_route_state(int if_index, int state)
{
	if ( if_index >= MAX_WAN_IF_INDEX )
		return;

	if ( state )
		ondemand_route_state[if_index] |= ONDEMAND_NEED_ADDROUTE;
	else
		ondemand_route_state[if_index] &= ~ONDEMAND_NEED_ADDROUTE;

	return;
}
int get_ondemand_route_state(int if_index)
{
	if ( if_index >= MAX_WAN_IF_INDEX )
		return 0;

	if (ondemand_route_state[if_index] & ONDEMAND_NEED_ADDROUTE)
		return 1;
	else
		return 0;
}

int isOndemandWan(int pvc_index, int entry_index)
{
	char nodeName[64] = {0};
	char connection[32] = {0};
	char ispMode[8] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);

	if(cfg_get_object_attr(nodeName, "ISP", ispMode, sizeof(ispMode)) < 0
		|| 0 != strcmp(ispMode, "2") )
		return 0;
	if(cfg_get_object_attr(nodeName, "CONNECTION", connection, sizeof(connection)) < 0
		|| 0 != strcmp(connection, "Connect_on_Demand") )
		return 0;

	return 1;
}

void set_portbinding_info_for_ondemand_wan(int pvc_index, int entry_index,char *ifname)
{
	int idx = 0;
	char bl_buf[12] = {0};
	char portbind_cmd[128] = {0};
	char nodeName[64] = {0};
	char connection[32] = {0};
	char ispMode[12] = {0};
	char ip_version[20] = {0};
	int ipVersion = 0;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);
	if(cfg_get_object_attr(nodeName, "ISP", ispMode, sizeof(ispMode)) < 0
		|| atoi(ispMode) != SVC_WAN_ATTR_ISP_PPP)
		return -1;
	if(cfg_get_object_attr(nodeName, "CONNECTION", connection, sizeof(connection)) < 0
		|| strcmp(connection, "Connect_on_Demand") )
		return -1;
	if(cfg_get_object_attr(nodeName, "IPVERSION", ip_version, sizeof(ip_version)) < 0)
		return -1;

	if ( 0 == strcmp(ip_version, "IPv4") )
		ipVersion = 0;
	else if ( 0 == strcmp(ip_version, "IPv6") )
		ipVersion = 1;
	else if ( 0 == strcmp(ip_version, "IPv4/IPv6") )
		ipVersion = 2;
	else
		return -1;

	for (idx = 0; idx < sizeof(bl_map_data) / sizeof(bl_map_data[0]); idx++)
	{
		if (cfg_get_object_attr(nodeName, bl_map_data[idx].bindif, bl_buf, sizeof(bl_buf)) >0)
		{
			if ( 0 == strcmp(bl_buf, "Yes"))
			{
				if ( 0 == ipVersion || 2 == ipVersion )
				{
					snprintf(portbind_cmd, sizeof(portbind_cmd),"portbindcmd addroute v4 %s %s ''", bl_map_data[idx].realif, ifname);
					system_escape(portbind_cmd);
				}
#ifdef IPV6
				if ( 1 == ipVersion || 2 == ipVersion )
				{
					sprintf(portbind_cmd, sizeof(portbind_cmd), "portbindcmd addroute v6 %s %s ''", bl_map_data[idx].realif, ifname);
					system_escape(portbind_cmd);
				}
#endif
			}
		}
	}
}


int ClearOndemandWanHisAddr(int if_index)
{
	char nodeName[64] = {0};
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, if_index + 1);
	cfg_set_object_attr(nodeName, "OndemandHisAddr", "N/A");

	return 0;
}


int delRoute4ViaOndemand(int pvc_index, int entry_index, char *ifname)
{
	char ondemand_route_index_v4[10] = {0};
	int i_ondemand_route_index_v4 = -1;
	char def_route_index_v4[10] = {0};
	int i_def_route_index_v4 = -1;
	int if_index = -1;
	char nodeName[64] = {0}, wan_node[64] = {0};
	char cmdbuf[128] = {0};
	char isDelAttr[32] = {0}, if_name[16] = {0};

	if_index = pvc_index * MAX_SMUX_NUM + entry_index;

	memset(ondemand_route_index_v4, 0, sizeof(ondemand_route_index_v4));
	if (cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV4, ondemand_route_index_v4,sizeof(ondemand_route_index_v4)) > 0 \
		&& '\0' != ondemand_route_index_v4[0] && 0 != strcmp(ondemand_route_index_v4, "N/A") )
		i_ondemand_route_index_v4 = atoi(ondemand_route_index_v4);

	if (cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_DEFROUTEIDXV4, def_route_index_v4,sizeof(def_route_index_v4)) > 0 \
		&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A") )
		i_def_route_index_v4 = atoi(def_route_index_v4);

	if ( (-1 == i_ondemand_route_index_v4)
		|| ( i_ondemand_route_index_v4 != if_index 
			&& i_def_route_index_v4 == i_ondemand_route_index_v4 ) )
		return -1;

	memset(wan_node, 0, sizeof(wan_node));
	snprintf(wan_node,sizeof(wan_node), WAN_PVC_ENTRY_NODE, i_ondemand_route_index_v4/SVC_WAN_MAX_PVC_NUM + 1, i_ondemand_route_index_v4%SVC_WAN_MAX_ENTRY_NUM + 1);
	memset(if_name, 0, sizeof(if_name));
	if ( cfg_get_object_attr(wan_node, "IFName", if_name, sizeof(if_name)) <= 0 )
		return -1;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), WANINFO_ENTRY_NODE, i_ondemand_route_index_v4 + 1);

	if( cfg_get_object_attr(nodeName, "isOndemandv4Del", isDelAttr,sizeof(isDelAttr)) < 0 
		|| 0 == isDelAttr[0]
		|| 0 == strcmp(isDelAttr, "N/A") )
	{
		cfg_set_object_attr(nodeName, "isOndemandv4Del", "1");
		snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/route del default dev %s", if_name);
		svc_wan_execute_cmd(cmdbuf);
	}

	return 0;
}


int delRoute6ViaOndemand(int pvc_index, int entry_index, char *ifname)
{
	char ondemand_route_index_v6[10] = {0};
	int i_ondemand_route_index_v6 = -1;
	char def_route_index_v6[10] = {0};
	int i_def_route_index_v6 = -1;
	int if_index = -1;
	char nodeName[64] = {0}, wan_node[64] = {0};
	char cmdbuf[128] = {0};
	char isDelAttr[32] = {0}, if_name[16] = {0};

	if_index = pvc_index * MAX_SMUX_NUM + entry_index;

	memset(ondemand_route_index_v6, 0, sizeof(ondemand_route_index_v6));
	if (cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV6, ondemand_route_index_v6,sizeof(ondemand_route_index_v6)) > 0 \
		&& '\0' != ondemand_route_index_v6[0] && 0 != strcmp(ondemand_route_index_v6, "N/A") )
		i_ondemand_route_index_v6 = atoi(ondemand_route_index_v6);

	if (cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_DEFROUTEIDXV6, def_route_index_v6,sizeof(def_route_index_v6)) > 0 \
		&& '\0' != def_route_index_v6[0] && 0 != strcmp(def_route_index_v6, "N/A") )
		i_def_route_index_v6 = atoi(def_route_index_v6);

	if ( (-1 == i_ondemand_route_index_v6)
		|| ( i_ondemand_route_index_v6 != if_index
			&& i_def_route_index_v6 == i_ondemand_route_index_v6 ) )
		return -1;

	memset(wan_node, 0, sizeof(wan_node));
	snprintf(wan_node,sizeof(wan_node), WAN_PVC_ENTRY_NODE, i_ondemand_route_index_v6/SVC_WAN_MAX_PVC_NUM + 1, i_ondemand_route_index_v6%SVC_WAN_MAX_ENTRY_NUM + 1);
	memset(if_name, 0, sizeof(if_name));
	if ( cfg_get_object_attr(wan_node, "IFName", if_name, sizeof(if_name)) <= 0 )
		return -1;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), WANINFO_ENTRY_NODE, if_index + 1);

	if( cfg_get_object_attr(nodeName, "isOndemandv6Del", isDelAttr,sizeof(isDelAttr)) < 0 
		|| 0 == isDelAttr[0]
		|| 0 == strcmp(isDelAttr, "N/A") )
	{
		cfg_set_object_attr(nodeName, "isOndemandv6Del", "1");
		snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/route -A inet6 del default dev %s", if_name);
		svc_wan_execute_cmd(cmdbuf);
	}

	return 0;
}

int addDnsRoute6forOtherWan(char *ifname, char *ifsvrapp, char *hisaddrV6)
{
	char cmdbuf[128] = {0};

	if ( NULL == ifname || NULL == ifsvrapp || NULL == hisaddrV6 )
		return -1;

	if ( NULL == strstr(ifsvrapp, "INTERNET") )
	{
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/ip -6 route add %s dev %s", hisaddrV6, ifname);
		svc_wan_execute_cmd(cmdbuf);
	}

	return 0;
}
#endif


