#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "update_child_prefix.h"

void set_childprefx_file(wan_entry_obj_t* obj)
{
    char cmd[SVC_WAN_BUF_128_LEN];
    char tmpBuf[SVC_WAN_BUF_512_LEN];
    char path[SVC_WAN_BUF_64_LEN];
    char child_orign_str[SVC_WAN_BUF_8_LEN];
    char child_orign = PREFIX_ORIGN_DFT;
    FILE *fp = NULL;
    char *p  = NULL;
    char macInfo[2][18];
    char itf_name[MAX_WAN_DEV_NAME_LEN];
    wan_entry_cfg_t* cfg = NULL;


    if(NULL == obj)
    {
        SVC_WAN_ERROR_INFO("pointer can't be NULL.\n");
        return ;
    }
    
    cfg = &obj->cfg;
    if(!SVC_WAN_IS_IPV6(cfg))
    {
        SVC_WAN_DEBUG_INFO("not ipv6 protocol.\n");
        return ; 
    }

    if(0 >= strlen(obj->child_prefix_bits))	
    {
        return ; 
    }
    
    SVC_WAN_TRACE_INFO("child_prefix_bits = %s.\n", obj->child_prefix_bits);
    if(SVC_WAN_IS_IPV6_DHCP(cfg))
    {
        child_orign = PREFIX_ORIGN_DHCP;
    }
    else if(SVC_WAN_IS_IPV6_SLAAC(cfg))
    {
        child_orign = PREFIX_ORIGN_SLLA;
    }
    else if(SVC_WAN_IS_IPV6_STATIC(cfg))
    {
        child_orign = PREFIX_ORIGN_STATIC;
    }
    else
    {
        child_orign = PREFIX_ORIGN_DFT;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s > %s", obj->dev, TMP_PREFIX_IF_PATH);
    svc_wan_execute_cmd(cmd);
    fp = fopen(TMP_PREFIX_IF_PATH, "r");
    if(NULL == fp)
    {
        SVC_WAN_ERROR_INFO("can't open %s file .\n", TMP_PREFIX_IF_PATH);
        return;
    }
    memset(macInfo, 0, sizeof(macInfo));
    while (fgets(tmpBuf, sizeof(tmpBuf), fp) != NULL)
    {
        if((p = strstr(tmpBuf,"HWaddr")))
        {
            /*tmpBuf format: HWaddr 00:AA:BB:01:23:45*/
            sscanf(p, "%s %s", macInfo[0],macInfo[1]);
            break;
        }
    }
    fclose(fp);

    memset(itf_name, 0, sizeof(itf_name));
    svc_wan_intf_name(obj, itf_name, sizeof(itf_name));

    /*write info to file*/
    /*1.write mac info*/
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "/var/run/%s/child_prefix_hwaddr", itf_name);
    fp = fopen(path, "w");
    if(NULL == fp)
    {
        SVC_WAN_ERROR_INFO("can't open %s  mode w file .\n", path);
        return;
    }
    fputs(macInfo[1], fp);
    fclose(fp);
    fp = NULL;

    /*2.return in ppp*/
    if(strstr(itf_name, "ppp") != NULL)
    {
        SVC_WAN_TRACE_INFO("itf_name = %s.\n", itf_name);
        return ;
    }

    /*3.write child prefix info*/
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "/proc/sys/net/ipv6/conf/%s/child_prefix", itf_name);
    fp = fopen(path, "w");
    if(NULL == fp)
    {
        SVC_WAN_ERROR_INFO("can't open %s  mode w file .\n", path);
        return;
    }
    fputs(obj->child_prefix_bits, fp);
    fclose(fp);
    fp = NULL;


    /*4.write child prefix orign*/
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "/proc/sys/net/ipv6/conf/%s/child_prefix_orign", itf_name);
    fp = fopen(path, "w");
    if(NULL == fp)
    {
        SVC_WAN_ERROR_INFO("can't open %s  mode w file .\n", path);
        return;
    }
    
    memset(child_orign_str, 0, sizeof(child_orign_str));
    snprintf(child_orign_str, sizeof(child_orign_str),  "%d", child_orign);
    fputs(child_orign_str, fp);
    fclose(fp);
    fp = NULL;

    return ;
}


