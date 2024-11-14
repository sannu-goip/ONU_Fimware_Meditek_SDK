#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wan_link_list.h"
#include "wan_nat.h"

/**********************************************************************/
/**********************************************************************/
void check_nat_enable(void)
{
    wan_entry_obj_t* obj = NULL;
    char nat_flag = 0;
    char cmd[SVC_WAN_BUF_128_LEN];
    for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
    {
        if(obj->nat)
        {
            nat_flag = 1;
            break;
        }
    }

    if(nat_flag)
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -F", IPTABLES_RAW);
        svc_wan_execute_cmd(cmd);
    }
    else
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -A PREROUTING -j NOTRACK", IPTABLES_RAW);
        svc_wan_execute_cmd(cmd);
        
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s -A OUTPUT -j NOTRACK", IPTABLES_RAW);
        svc_wan_execute_cmd(cmd);
    }

    return ;
}

int svc_wan_start_intf_nat(char* dev)
{
    char cmd[SVC_WAN_BUF_256_LEN];

    if((NULL == dev) || (0 >= strlen(dev)))
    {
        return 0;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -A POSTROUTING -o %s -j MASQUERADE", IPTABLES_NAT, dev);	
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -N VS_%s", IPTABLES_NAT, dev);	
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -N DMZ_%s", IPTABLES_NAT, dev);	
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -A PREROUTING -j VS_%s", IPTABLES_NAT, dev);	
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -A PREROUTING -j DMZ_%s", IPTABLES_NAT, dev);	
    svc_wan_execute_cmd(cmd);

    /*restart nat*/
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/script/nat_stop.sh %s", dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "/usr/script/nat_start.sh %s", dev);
    svc_wan_execute_cmd(cmd);

    return 0;
}

int svc_wan_stop_intf_nat(char* dev)
{
    char cmd[SVC_WAN_BUF_256_LEN];

    if((NULL == dev) || (0 >= strlen(dev)))
    {
        return 0;
    }
    
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd,sizeof(cmd), "%s -D POSTROUTING -p all -o %s -j MASQUERADE", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -D PREROUTING -j VS_%s", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -D PREROUTING -j DMZ_%s", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -F VS_%s", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -F DMZ_%s", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -X VS_%s", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s -X DMZ_%s", IPTABLES_NAT, dev);
    svc_wan_execute_cmd(cmd);

    return 0;
}


