#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include "wan_link_list.h"
#include "wan_protocol_handle.h"
#include "utility.h"

#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
static void send_signal_ap_wan(int evt, char *itf)
{
	char str_path[64] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	int pid = 0;
#if defined(TCSUPPORT_NP_CMCC)
	FILE *fp = NULL;
	char *buff = NULL;
	int len = 0, res = 0;
#endif

	if ( NULL == itf )
		return;
	
	memset(str_path, 0, sizeof(str_path));
	snprintf(str_path, sizeof(str_path), "/var/run/%s/udhcpc.pid", itf);
	pid = svc_wan_get_pid(str_path);
	if ( pid <= 0 )
		return;

#if defined(TCSUPPORT_NP_CMCC)
	/* check udhcpc has been started or not */
	memset(str_path, 0, sizeof(str_path));
	snprintf(str_path, sizeof(str_path), "/proc/%d/status", pid);
	fp = fopen(str_path, "r");
	if ( NULL == fp )
	{
		/* udhcpc has not been started */
		getPreTime();
		np_wan_start_br_alink_dhcpc(itf);
		return;
	}
	else
	{
		res = getline(&buff, &len, fp);
		fclose(fp);
		if ( -1 != res )
		{
			if ( NULL == strstr(buff, "udhcpc") )
			{
				/* udhcpc has not been started */
				getPreTime();
				np_wan_start_br_alink_dhcpc(itf);
				if ( buff )
					free(buff);
				return;
			}
		}
		if ( buff )
			free(buff);
	}
#endif

	memset(str_path, 0, sizeof(str_path));
	snprintf(str_path, sizeof(str_path), "/var/run/%s/status", itf);
	svc_wan_get_file_string(str_path, info_string, 1);
	switch(evt)
	{
		case EVT_XPON_DOWN:
		{
			if ( 0 == strcmp(info_string[0], "down") )
				break;
			kill(pid, SIGUSR2);
			break;
		}
		case EVT_XPON_UP:
		{
			if ( 0 == strcmp(info_string[0], "up") )
				break;
			kill(pid, SIGUSR1);
			break;
		}
		default:
		{
			break;
		}
	}
	return;
}
#endif

int checkWanIpExist(wan_entry_obj_t* obj){
	char path[64] = {0};
	wan_entry_cfg_t* cfg = NULL;

	
	cfg  = &obj->cfg;	
	if(SVC_WAN_ATTR_ISP_PPP == cfg->isp)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "/tmp/pppuptime-ppp%d", obj->idx);
		if(!access(path, 0))
		{			
			return 1;
		}

		memset(path, 0, sizeof(path));
 	   	snprintf(path, sizeof(path), "/var/run/ppp%d/ip", obj->idx);
		if(!access(path, 0))
		{
			return 1;
		}		
		
	}else{
		snprintf(path, sizeof(path), "/var/run/%s/ip", obj->dev);
	if(!access(path, 0))
	{
		return 1;
	}
	}
	
	

	return 0;
}

void send_signal_wan(int evt, int isInternet)
{
	wan_entry_obj_t* obj = NULL;
	wan_entry_cfg_t* cfg = NULL;
	int pid = 0;
	int needSignal = 0;
	int protocol_version = 0;
	char path[SVC_WAN_BUF_128_LEN];

	for(obj = svc_wan_get_obj_list(); NULL != obj; obj = obj->next)
	{
		cfg  = &obj->cfg;
		if(!SVC_WAN_IS_ACTIVE(cfg))
		{
			continue;
		}
		if(isInternet && SVC_WAN_SRV_INTERNET(cfg))
		{
			needSignal = 1;
		}
		else if(!isInternet && !SVC_WAN_SRV_INTERNET(cfg))
		{
			needSignal = 1;
		}

		/* this check is only for IPv4. */
		if(needSignal == 1 && SVC_WAN_ATTR_VERSION_IPV4 == cfg->version ){
		if((checkWanIpExist(obj) && EVT_XPON_UP == evt)  
			|| (!checkWanIpExist(obj) && EVT_XPON_DOWN == evt)){
			
			tcdbg_printf("[%s:%d].obj->idx=%d,evt=%d, no need to singal\n",__FUNCTION__,__LINE__,obj->idx,evt);		
			needSignal = 0;
		}
		}

		if(needSignal)
		{
			if(SVC_WAN_ATTR_ISP_PPP == cfg->isp)
			{
		        pid = svc_wan_get_pid(obj->pid4);
		        if(0 >= pid)
		        {
		            continue;
		        }

				kill(pid, SIGHUP);
			}
			else if((SVC_WAN_ATTR_ISP_DHCP == cfg->isp))
			{
				if(SVC_WAN_IS_IPV4_DHCP(cfg))
				{
					pid = svc_wan_get_pid(obj->pid4);
					if(0 >= pid)
					{
						continue;
					}
					switch (evt)
					{
						case EVT_XPON_DOWN:
						{
							kill(pid, SIGUSR2);
							obj->dhcp_release = TRUE;
							break;
						}
						case EVT_XPON_UP:
						{
							if(obj->dhcp_release)
							{
								kill(pid, SIGUSR1);
								obj->dhcp_release = FALSE;
							}
							break;
						}
						default:
						{
							break;
						}
					}
				}
				if(SVC_WAN_IS_IPV6(cfg))
				{
					/* RS pkt restart */
					protocol_version = svc_wan_cfg_get_system_protocol_version();
					if ( E_SYSTEM_ONLY_IPV4_ENABLE != protocol_version )
					{
						bzero(path, sizeof(path));
						snprintf(path, sizeof(path)
								, "/proc/sys/net/ipv6/conf/%s/disable_ipv6", obj->dev);

						doValPut(path, "1");
						doValPut(path, "0");
					}

					pid = svc_wan_get_pid(obj->pid6);
					if(0 >= pid)
					{
						continue ;
					}
					switch (evt)
					{
						case EVT_XPON_DOWN:
						{
							kill(pid, SIGHUP);
							break;
						}
						case EVT_XPON_UP:
						{
							break;
						}
						default:
						{
							break;
						}
					}
				}
			}
		}
	}

#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
	send_signal_ap_wan(evt, NP_AP_B_WAN_IF);
#endif
	return ;
}

void send_signal_wan_dhcp(int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    int pid = 0;
#if defined(TCSUPPORT_CMCCV2)
    static int is_1boot_dhcp = 0;
#endif
#if defined(TCSUPPORT_NP_CMCC)
	char str_path[64] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	char cmdbuf[128] = {0}, pidbuf[32] = {0};
	FILE *fp = NULL;
	char *buff = NULL;
	int len = 0, res = 0;
#endif

    for(obj = svc_wan_get_obj_list(); NULL != obj; obj = obj->next)
    {
        cfg  = &obj->cfg;
        if(!SVC_WAN_IS_ACTIVE(cfg) || (SVC_WAN_ATTR_ISP_DHCP != cfg->isp))
        {
            continue;
        }

        if(SVC_WAN_IS_IPV4_DHCP(cfg))
        {
            pid = svc_wan_get_pid(obj->pid4);
            if(0 >= pid)
            {
                continue;
            }
            switch (evt)
            {
                case EVT_XPON_DOWN:
                {
#if defined(TCSUPPORT_CMCCV2)
                /* check up status for 1st boot up */
                if ( SVC_WAN_SRV_INTERNET(cfg) && 0 == is_1boot_dhcp )
                {
                    is_1boot_dhcp = 1;
                    if (obj->flag & SVC_WAN_FLAG_UPV4)
                    {
                        obj->dhcp_release = FALSE;
                        break;
                    }
                }
#endif
                    kill(pid, SIGUSR2);
                    obj->dhcp_release = TRUE;
                    break;
                }
                case EVT_XPON_UP:
                {
                    if(obj->dhcp_release)
                    {
                        kill(pid, SIGUSR1);
                        obj->dhcp_release = FALSE;
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        if(SVC_WAN_IS_IPV6(cfg))
        {
            pid = svc_wan_get_pid(obj->pid6);
            if(0 >= pid)
            {
                continue ;
            }
            switch (evt)
            {
                case EVT_XPON_DOWN:
                {
                    kill(pid, SIGHUP);
                    break;
                }
                case EVT_XPON_UP:
                {
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

#if defined(TCSUPPORT_NP_CMCC)
	memset(str_path, 0, sizeof(str_path));
	snprintf(str_path, sizeof(str_path), "/var/run/%s/udhcpc.pid", NP_AP_B_WAN_IF);
	pid = svc_wan_get_pid(str_path);
	if ( pid <= 0 )
		return;

	/* check udhcpc has been started or not */
	memset(str_path, 0, sizeof(str_path));
	snprintf(str_path, sizeof(str_path), "/proc/%d/status", pid);
	fp = fopen(str_path, "r");
	if ( NULL == fp )
	{
		/* udhcpc has not been started */
		getPreTime();
		np_wan_start_br_alink_dhcpc(NP_AP_B_WAN_IF);
		return;
	}
	else
	{
		res = getline(&buff, &len, fp);
		fclose(fp);
		if ( -1 != res )
		{
			if ( NULL == strstr(buff, "udhcpc") )
			{
				/* udhcpc has not been started */
				getPreTime();
				np_wan_start_br_alink_dhcpc(NP_AP_B_WAN_IF);
				if ( buff )
					free(buff);
				return;
			}
		}
		if ( buff )
			free(buff);
	}
	
	memset(str_path, 0, sizeof(str_path));
	snprintf(str_path, sizeof(str_path), "/var/run/%s/status", NP_AP_B_WAN_IF);
	svc_wan_get_file_string(str_path, info_string, 1);
	switch(evt)
	{
		case EVT_XPON_DOWN:
		{
			if ( 0 == strcmp(info_string[0], "down") )
				break;
			kill(pid, SIGUSR2);
			break;
		}
		case EVT_XPON_UP:
		{
			if ( 0 == strcmp(info_string[0], "up") )
				break;
			kill(pid, SIGUSR1);
			break;
		}
		default:
		{
			break;
		}
	}
#endif

    return ;
}

void send_signal_wan_pppd(int evt)
{
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    int pid = 0;

#if defined(TCSUPPORT_CMCCV2)
    static int is_1boot_ppp = 0;
#endif

    for(obj = svc_wan_get_obj_list(); NULL != obj; obj = obj->next)
    {
        cfg  = &obj->cfg;
        if(!SVC_WAN_IS_ACTIVE(cfg) || (SVC_WAN_ATTR_ISP_PPP != cfg->isp))
        {
            continue;
        }
        pid = svc_wan_get_pid(obj->pid4);
        if(0 >= pid)
        {
            continue;
        }

#if defined(TCSUPPORT_CMCCV2)
        /* check up status for 1st boot up */
        if ( SVC_WAN_SRV_INTERNET(cfg) && 0 == is_1boot_ppp  && EVT_XPON_UP == evt)
        {
            is_1boot_ppp = 1;
            if ((obj->flag & SVC_WAN_FLAG_UPV4) || (obj->flag & SVC_WAN_FLAG_UPV6))
            {
                continue;
            }
        }
#endif

        kill(pid, SIGHUP);
    }

    return ;
}

