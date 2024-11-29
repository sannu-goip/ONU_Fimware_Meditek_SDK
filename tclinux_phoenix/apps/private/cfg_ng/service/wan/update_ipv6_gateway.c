#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_common.h"
#include "wan_mgr.h"
#include "wan_link_list.h"
#include "update_ipv6_gateway.h"
#include "utility.h"
/*********************************************************************************/
/*static void shrink_ipv6addr(char *in_ipv6addr, char *out_ipv6addr, int out_len);*/
static int update_ipv6_gateway(wan_entry_obj_t* obj, char* itf_name);
static int update_slaac_addr(wan_entry_obj_t* obj, char* itf_name);

/*********************************************************************************/
/*static void shrink_ipv6addr(char *in_ipv6addr, char *out_ipv6addr, int out_len)
{
    struct sockaddr_in6 ipv6addr;
    memset(&ipv6addr,0x0,sizeof(struct sockaddr_in6));
    inet_pton(AF_INET6, in_ipv6addr, (struct sockaddr *) &ipv6addr.sin6_addr);
    ipv6addr.sin6_family = AF_INET6;

    getnameinfo((struct sockaddr *) &ipv6addr, sizeof(struct sockaddr_in6), out_ipv6addr, out_len, NULL, 0, NI_NUMERICHOST);
    return;
}
*/

static int update_slaac_addr(wan_entry_obj_t* obj, char* itf_name)
{
    int   fd = 0;
    int size = 0;
    char msg = FALSE;
    char node[SVC_WAN_BUF_32_LEN];
    char buf[SVC_WAN_BUF_64_LEN];
    char tmp[SVC_WAN_BUF_64_LEN];
    char path[SVC_WAN_BUF_64_LEN];
    char prefix_len[SVC_WAN_BUF_8_LEN];
    char slaac_addr[SVC_WAN_BUF_64_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
    wan_entry_cfg_t* cfg = NULL;

    if( (NULL == obj) || (NULL == itf_name) || 0 >= strlen(itf_name))
    {
        SVC_WAN_ERROR_INFO("pointer can't be NULL or itf_name length <= 0 \n");
        return 0;
    }

    cfg = &obj->cfg;
    if(SVC_WAN_IS_IPV6_SLAAC(cfg))
    {
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), "/proc/sys/net/ipv6/conf/%s/slaac_addr", itf_name);
        fd = open(path, O_RDONLY | O_NONBLOCK);
        if(-1 == fd)
        {
            return msg;
        }

        memset(buf, 0, sizeof(buf));
        size = read(fd, buf, 64);
        close(fd);
        if( (1 < size) && (0 != strcmp(buf, "")) )
        {
            memset(tmp, 0, sizeof(tmp));
            memset(prefix_len, 0, sizeof(prefix_len));
            sscanf(buf, "%39s %s", tmp, prefix_len);

            memset(slaac_addr, 0, sizeof(slaac_addr));
            shrink_ipv6addr(tmp, slaac_addr, sizeof(slaac_addr));

            SVC_WAN_CRITIC_INFO("slaac_addr = %s, prefix_len = %s, itf_name = %s.\n", \
                slaac_addr, prefix_len, itf_name);

            /*Check slaac address or prefix len been changed or not*/
            memset(node, 0, sizeof(node));
            snprintf(node, sizeof(node), WANINFO_ENTRY_NODE, obj->idx + 1);

            memset(tmp, 0, sizeof(tmp));
            cfg_get_object_attr(node, "IP6", tmp, sizeof(tmp));
            if (0 != strcmp(tmp, slaac_addr))
            {
                msg = TRUE;
            }

            memset(tmp, 0, sizeof(tmp));
            cfg_get_object_attr(node, "PrefixLen6", tmp, sizeof(tmp));
            if(0 != strcmp(tmp, prefix_len))
            {
                msg = TRUE;
            }

            if(msg)
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "echo \"%s\" > /var/run/%s/ip6", slaac_addr, itf_name);
                svc_wan_execute_cmd(cmd);

                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "echo \"%s\" > /var/run/%s/prefix6", prefix_len, itf_name);
                svc_wan_execute_cmd(cmd);
            }

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "echo %s > /proc/sys/net/ipv6/conf/%s/slaac_addr", "", itf_name);
            svc_wan_execute_cmd(cmd);

            if(msg && 1 < strlen(slaac_addr))
            {
                msg = E_SLAAC_ADDR_MSG_UPDATE;
            }
            else
            {
                msg = FALSE;
            }
        }
    }

    return msg;
}

static int update_ipv6_gateway(wan_entry_obj_t* obj, char* itf_name)
{
    int fd = 0;
    int size = 0;
    char msg = FALSE;
    char path[SVC_WAN_BUF_64_LEN];
    char buf[SVC_WAN_BUF_64_LEN];
    char tmp[SVC_WAN_BUF_64_LEN];
    char gw_addr[SVC_WAN_BUF_64_LEN];
    char cmd[SVC_WAN_BUF_256_LEN];
    wan_entry_cfg_t* cfg = NULL;

    if((NULL == obj) ||  (NULL == itf_name) || 0 >= strlen(itf_name))
    {
        SVC_WAN_ERROR_INFO("pointer can't be NULL or itf_name length <= 0 \n");
        return 0;
    }

    cfg = &obj->cfg;
    memset(path, 0, sizeof(path));
    if(!SVC_WAN_GWV6_IS_MANUAL(cfg) && (0 == (obj->flag & SVC_WAN_FLAG_UPV6)) )
    {
        snprintf(path, sizeof(path), "/proc/sys/net/ipv6/neigh/%s/default_route", itf_name);
        fd = open(path, (O_RDONLY | O_NONBLOCK));
        if(-1 == fd)
        {
            return msg;
        }

        memset(buf, 0, sizeof(buf));
        size = read(fd, buf, 39);
        close(fd);
        if( (1 < size) && (0 != strcmp(buf, "")) )
        {
            shrink_ipv6addr(buf, gw_addr, sizeof(gw_addr));

            memset(path, 0, sizeof(path));
            snprintf(path, sizeof(path), WANINFO_ENTRY_NODE, obj->idx + 1);
            memset(tmp, 0, sizeof(tmp));
            cfg_get_object_attr(path, "GateWay6", tmp, sizeof(tmp));
            if(0 != strcmp(tmp, gw_addr))
            {
                memset(cmd, 0, sizeof(cmd));
                snprintf(cmd, sizeof(cmd), "echo \"%s\" > /var/run/%s/gateway6", gw_addr, itf_name);
                svc_wan_execute_cmd(cmd);

                if(strlen(gw_addr) > 1)
                {
                    msg = E_GATEWAY_MSG_UPDATE;
                }
            }
        }
    }

    return msg;
}
static int parseIPv6Addr(char *wanname, char *addr6, int size)
{
	char tmp[1024] = {0}, *buf = NULL, tmpaddr[1024] = {0};
	FILE *fp = NULL;
	size_t len = 0;
	int index= 0;
	char *p = NULL;
	
	snprintf(tmp, sizeof(tmp), "ifconfig %s > /tmp/wan_interface_info", wanname);
	system(tmp);
	fp = fopen("/tmp/wan_interface_info", "r");

	if( !fp )
	{
		tcdbg_printf("[%s:%d]:open file fail:leave============>\n",__FUNCTION__,__LINE__);
		return -1;
	}

	while( getline(&buf, &len, fp) != -1 )
	{
		if( (p = strstr(buf, "inet6")) == NULL )
			continue;
		
		sscanf(p + strlen("inet6 addr: "), "%[^/]", tmpaddr);
		if(size - index < strlen(tmpaddr)){
			break;
		}
		if(strncmp(tmpaddr,"fe80",4) == 0)
		{
			strncpy(addr6,tmpaddr,size-1);
			break;
		}
		p = NULL;
	}

	addr6[strlen(addr6)] ='\0';
	
	if( buf )
		free(buf);
	fclose(fp);
	unlink("/tmp/wan_interface_info");
	
	return 0;
}

static int update_linklocal_addr(char *wanname,int id)
{
	char linklocalIP[32] = {0};
	char node[32] = {0};
	char linklocalAddr[32] = {0};

	if(wanname == NULL || id < -1)
		return 0;
	
	memset(node, 0, sizeof(node));
	snprintf(node, sizeof(node), WANINFO_ENTRY_NODE,id);
	
	cfg_obj_get_object_attr(node, "LinkLocalAddr", 0, linklocalAddr, sizeof(linklocalAddr));
	if(strlen(linklocalAddr) != 0)
		return 0;
	
	parseIPv6Addr(wanname ,linklocalIP, sizeof(linklocalIP));
	
	if(strlen(linklocalIP) != 0 && strcmp(linklocalAddr,linklocalIP) != 0)
	{
		cfg_set_object_attr(node,"LinkLocalAddr", linklocalIP);
		return 1;
	}
	else
	{
		return 0;
	}
}
void update_ipv6_slaac_addr_and_gateway(void)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;

    char itf_name[MAX_WAN_DEV_NAME_LEN];
    char msg = FALSE;
    char cmd[SVC_WAN_BUF_128_LEN];

    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
        cfg = &obj->cfg;
        if(!SVC_WAN_IS_IPV6(cfg) || !SVC_WAN_IS_ACTIVE(cfg))
        {
            continue;
        }
        memset(itf_name, 0, sizeof(itf_name));
        svc_wan_intf_name(obj, itf_name, sizeof(itf_name));
#if defined(TCSUPPORT_NP_CMCC)
	if(strlen(itf_name) != 0)
	{
		update_linklocal_addr(itf_name,obj->idx + 1);
	}
#endif	
        msg  = update_ipv6_gateway(obj, itf_name);

        msg |= update_slaac_addr(obj, itf_name);

        if(msg)
        {
            SVC_WAN_DEBUG_INFO("msg = %d, obj->path = %s.\n", msg, obj->path);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "echo %s > /var/run/%s/status6", "up", itf_name);
            svc_wan_execute_cmd(cmd);

            svc_wan_mgr_handle_event(EVT_WAN_CONN_GETV6, itf_name);
        }
    }

    return ;
}

