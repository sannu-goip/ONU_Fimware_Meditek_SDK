#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include "wan_common.h"
#include "wan_cfg.h"
#include "wan_link_list.h"
#include "internet_connect.h"
#include "utility.h"

#if defined(TCSUPPORT_NP_CMCC)
#define PING_CMD	"/bin/ping -c 1 -W 1 %s > %s"
#define GLOBAL_DNS	"8.8.8.8"
#define TMP_PING_PATH	"/tmp/pingInternet"
#define RECV1ICMPPKT	"1 packets received"
#define PING_FAIL_THR 3
#define LED_RED 1
#define LED_GREEN 0


static int ping_fail_count = 0;
static int internet_led_status = LED_RED;

int check_internet_ping(void)
{
	char pingCMD[128]={0};
	char tmpBuf[256]={0};

	snprintf(pingCMD, sizeof(pingCMD), PING_CMD, GLOBAL_DNS, TMP_PING_PATH);  /*ping GLOBAL DNS*/
	system(pingCMD);
	fileRead(TMP_PING_PATH, tmpBuf, 256);
	if(strstr(tmpBuf, RECV1ICMPPKT)) /*ping success*/
	{
		internet_led_status = LED_GREEN;
		ping_fail_count = 0;
	}
	else  /*ping fail*/
	{
		if(internet_led_status == LED_GREEN)
		{
			ping_fail_count++;
		}
		if(ping_fail_count >= PING_FAIL_THR)  /*change status to RED if ping_fail_count exceed PING_FAIL_THR*/
		{
			internet_led_status = LED_RED;
			ping_fail_count = 0;
		}
	}
	
	unlink(TMP_PING_PATH);

	return internet_led_status;
}
#endif
void check_internet_connect(void)
{
    int isInternetLedon = 0;/* 1:IP Connected; 0:No IP Connected*/ 
    int isInternetLedLedTrying = 0;/* 1:adsl up and trying to get WAN IP; 0:adsl not up or can't found defaule router*/
    wan_entry_obj_t* obj = NULL;
    wan_entry_cfg_t* cfg = NULL;
    char isInternetIf	 = FALSE;
    char isBridgeInterIf = FALSE;
    int     nIspEntry_V4 = -1;
    int     nIspEntry_V6 = -1;
#if defined(TCSUPPORT_NP_CMCC)
	int ping_ret = -1;
	char internetLedCheck[8] = {0};

#endif
    char xpon_traffic = E_XPON_LINK_DOWN;
    char node[SVC_WAN_BUF_32_LEN];
    char cmd[SVC_WAN_BUF_64_LEN];
    char def_route_index_v4[SVC_WAN_BUF_16_LEN];
    char def_route_index_v6[SVC_WAN_BUF_16_LEN];
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
	char transMode[16] = {0};
#endif
#if defined(TCSUPPORT_NP_CMCC)
	char ap_state[8] = {0}, ap_ip[16] = {0};
	char wanboot[4] = {0};
#endif

#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
	cfg_get_object_attr(WAN_COMMON_NODE, "TransMode", transMode,sizeof(transMode));
	if( 0 == strcmp(transMode, "Ethernet") )
	{
		xpon_traffic = is_ethernet_link_up();
	}
	else if( 0 == strcmp(transMode, "ActiveEtherWan") )
	{
		xpon_traffic = is_ActiveEtherWan_link_up();
	}
	else
	{
		xpon_traffic = is_xpon_traffic_up();
	}
#else
    xpon_traffic = is_xpon_traffic_up();
#endif

#if defined(TCSUPPORT_NP_CMCC) && defined(TCSUPPORT_WLAN_APCLIENT)
	if(E_XPON_LINK_DOWN == xpon_traffic)
	{
		if (NP_APCLI_DISCONNECTED != np_wlan_apcli_connstatus())
		{
			xpon_traffic = E_XPON_LINK_UP;
		}
	}
#endif

#if !defined(TCSUPPORT_CT_DSL_EX)
    if(xpon_traffic  == E_XPON_LINK_UP)
    {
        wan_mgr_update_status(E_WAN_PON_UP);
    }
    else
    {
        wan_mgr_update_status(E_WAN_PON_DOWN);
    }
#endif

    if(E_XPON_LINK_DOWN == xpon_traffic)
    {
        isInternetLedon = 0;
        isInternetLedLedTrying = 0;
        goto write_proc;
    }
    else
    {
#if defined(TCSUPPORT_NP_CMCC)
		/* check ap wan up state */
		memset(ap_state, 0, sizeof(ap_state));
		memset(ap_ip, 0, sizeof(ap_ip));
		memset(wanboot, 0, sizeof(wanboot));
		cfg_get_object_attr(APWANINFO_ENTRY_NODE, "Status", ap_state, sizeof(ap_state));
		cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanBoot", wanboot, sizeof(wanboot));
		if ( 0 == strcmp(wanboot, "1") && 0 == strcmp(ap_state, "up") )
		{
			cfg_get_object_attr(APWANINFO_ENTRY_NODE, "IP", ap_ip, sizeof(ap_ip));
			if ( 0 != ap_ip )
			{
				isInternetLedon = 1;
				isInternetLedLedTrying = 0;
				goto write_proc;
			}
		}
#endif
        for(obj = svc_wan_get_obj_list(); obj; obj = obj->next)
        {
            cfg = &obj->cfg;
            if(!SVC_WAN_IS_ACTIVE(cfg))
            {
                continue;
            }
            if(SVC_WAN_SRV_INTERNET(cfg))
            {
                isInternetIf = TRUE;//exist  internet interface.
                if(SVC_WAN_IS_BRIDGE(cfg))
                {
                    isBridgeInterIf = TRUE;//exist bridge internet interface.
                    break;
                }
            }
        }

        if(!isInternetIf) 
        {
            isInternetLedon = 0;
            isInternetLedLedTrying = 0;
            goto write_proc;
        }

        if(isBridgeInterIf)//exist bridge internet interface.
        {
            isInternetLedon = 1;
            isInternetLedLedTrying = 0;
            goto write_proc;
        }

        memset(node, 0, sizeof(node));
        snprintf(node, sizeof(node), WANINFO_COMMON_NODE);

        memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
        if(cfg_get_object_attr(node, "DefRouteIndexv4", def_route_index_v4, sizeof(def_route_index_v4)) < 0)
        {
            isInternetLedon = 0;
            isInternetLedLedTrying = 1;
            goto write_proc;
        }

        memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
        if(cfg_get_object_attr(node, "DefRouteIndexv6", def_route_index_v6, sizeof(def_route_index_v6)) < 0)
        {
            isInternetLedon = 0;
            isInternetLedLedTrying = 1;
            goto write_proc;
        }


        if(strcmp(def_route_index_v4, "N/A") != 0)
        {
            nIspEntry_V4 = atoi(def_route_index_v4);
        }

        if(strcmp(def_route_index_v6, "N/A") != 0)
        {
            nIspEntry_V6 = atoi(def_route_index_v6);
        }

        if((nIspEntry_V4 == -1) && (nIspEntry_V6 == -1))
        {
            isInternetLedon = 0;
            isInternetLedLedTrying = 1;
            goto write_proc;
        }
        else
        {
            isInternetLedon = 1;
            isInternetLedLedTrying = 0;
            goto write_proc;
        }
    }

write_proc: 
#if defined(TCSUPPORT_NP_CMCC)
		if( 0 <= cfg_get_object_attr(WAN_COMMON_NODE, "InternetLedCheck", internetLedCheck, sizeof(internetLedCheck))
			&& 0 == strcmp(internetLedCheck, "Yes"))
		{
			ping_ret = check_internet_ping();
			if (ping_ret == LED_RED)
			{
				isInternetLedon = 0;
				isInternetLedLedTrying =0;
			}
			else /*ping_ret == LED_GREEN*/
			{
				isInternetLedon = 1;
				isInternetLedLedTrying =0;
			}
		}
#endif
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%d %d", isInternetLedon, isInternetLedLedTrying);
    do_val_put("/proc/tc3162/led_internet", cmd);

    return ;
}





