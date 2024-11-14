#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <cfg_cli.h> 
#include <time.h>
#include "wan_link_list.h"
#include "dslite_info.h"
#include "update_ipv6_gateway.h"
#include "wan_polling.h"
#include "internet_connect.h"
#include "dns_info.h"
#include "cfg_msg.h"
#if defined(TCSUPPORT_NP_CMCC)
#include "np_wan_handle.h"
#endif
#include "utility.h"
#include <svchost_api.h>

#if defined(TCSUPPORT_TEST_WLAN_SHORTCUT) || defined(TCSUPPORT_CT_JOYME4)
#include "blapi_perform.h"
#endif
#if defined(TCSUPPORT_CT_JOYME4)
#include "blapi_traffic.h"
#endif

#if defined(TCSUPPORT_CT_DNSMASQ_MONITOR)
#define DNS_CHECK_TIME 60
#endif
int newPonStatus = 0;
pthread_mutex_t PonStatusLock;
#define XPON_CHANGE_MAX_SLEEP_TIME 5

#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
#define	OLT_REG_SUCC_TIME_1	30
#define	OLT_REG_SUCC_TIME_2	60
#endif
#endif


/************************************************************************/
static void prepare_wan_mgr_polling(void);
static void wan_mgr_polling_pthread(void);

/************************************************************************/

int dnsmasq_check(void){
	int dnsmasq_pid = 0;
	int rv = 0, pidnum = 0, pid_t[128] = {0};

	rv = find_pid_by_name( "dnsmasq", pid_t, &pidnum);
	if( 0 == rv 
		&& 1 == pidnum )
		dnsmasq_pid = pid_t[0];

	return dnsmasq_pid;
}

/************************************************************************/


#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
int AddRouteforOnDemandWan(int pvc_index, int entry_index)
{
	char nodeName[64] = {0}, waninfo_node[64] = {0};
	char cmdbuf[128] = {0};
	char connection[32] = {0};
	char ispMode[12] = {0};
	char ifidxbuf[12] = {0};
	char serviceList[64] = {0};
	char ifname[32] = {0};
	char ip_version[20] = {0};
	int ipVersion = 0;
	int isInternet = 0;
	int if_index = -1;
	char def_route_index_v4[10] = {0};
	int i_def_route_index_v4 = -1;
	char def_route_index_v6[10] = {0};
	int i_def_route_index_v6 = -1;
	char ondemand_route_index_v4[10] = {0};
	int i_ondemand_route_index_v4 = -1;
	char ondemand_route_index_v6[10] = {0};
	int i_ondemand_route_index_v6 = -1;

	if_index = pvc_index * SVC_WAN_MAX_PVC_NUM + entry_index;
	snprintf(ifidxbuf, sizeof(ifidxbuf),"%d", if_index);

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName), WAN_PVC_ENTRY_NODE,pvc_index+1, entry_index+1);
	memset(waninfo_node, 0, sizeof(waninfo_node));
	snprintf(waninfo_node,sizeof(waninfo_node), WANINFO_ENTRY_NODE, if_index + 1);

	if(cfg_get_object_attr(nodeName, "ISP", ispMode, sizeof(ispMode)) < 0
		|| atoi(ispMode) != SVC_WAN_ATTR_ISP_PPP)
		return -1;
	if(cfg_get_object_attr(nodeName, "CONNECTION", connection, sizeof(connection)) < 0
		|| strcmp(connection, "Connect_on_Demand") )
		return -1;
	if(cfg_get_object_attr(nodeName, "IPVERSION", ip_version, sizeof(ip_version)) < 0)
		return -1;
	if(cfg_get_object_attr(nodeName, "IFName", ifname, sizeof(ifname)) <= 0)
		return -1;
	if(cfg_get_object_attr(nodeName, "ServiceList", serviceList, sizeof(serviceList)) > 0
		&& NULL != strstr(serviceList, "INTERNET"))
		isInternet = 1;
	
	if ( 0 == strcmp(ip_version, "IPv4") )
		ipVersion = 0;
	else if ( 0 == strcmp(ip_version, "IPv6") )
		ipVersion = 1;
	else if ( 0 == strcmp(ip_version, "IPv4/IPv6") )
		ipVersion = 2;
	else
		return -1;

	/* get default route index for V4&V6*/
	memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
	memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
	if(cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_DEFROUTEIDXV4, def_route_index_v4, sizeof(def_route_index_v4)) > 0
		&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A") )
		i_def_route_index_v4 = atoi(def_route_index_v4);
	if(cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_DEFROUTEIDXV6, def_route_index_v6, sizeof(def_route_index_v6)) > 0
		&& '\0' != def_route_index_v6[0] && 0 != strcmp(def_route_index_v6, "N/A") )
		i_def_route_index_v6 = atoi(def_route_index_v6);

	/* get on-demand wan connection index for V4&V6*/
	memset(ondemand_route_index_v4, 0, sizeof(ondemand_route_index_v4));
	memset(ondemand_route_index_v6, 0, sizeof(ondemand_route_index_v6));
	if(cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV4, ondemand_route_index_v4, sizeof(ondemand_route_index_v4)) > 0
		&& '\0' != ondemand_route_index_v4[0] && 0 != strcmp(ondemand_route_index_v4, "N/A") )
		i_ondemand_route_index_v4 = atoi(ondemand_route_index_v4);
	if(cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV6, ondemand_route_index_v6, sizeof(ondemand_route_index_v6)) > 0
		&& '\0' != ondemand_route_index_v6[0] && 0 != strcmp(ondemand_route_index_v6, "N/A") )
		i_ondemand_route_index_v6 = atoi(ondemand_route_index_v6);

	if ( 1 == isInternet
		&& ( 0 == ipVersion || 2 == ipVersion )
		&& -1 == i_def_route_index_v4
		&& ( -1 == i_ondemand_route_index_v4 || ( i_ondemand_route_index_v4 >= 0 && if_index == i_ondemand_route_index_v4 ) ))
	{
		if ( i_ondemand_route_index_v4 >= 0 && if_index == i_ondemand_route_index_v4 )
		{
			snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route del default dev %s", ifname);
			system_escape(cmdbuf);
		}
		snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route add default dev %s", ifname);
		system_escape(cmdbuf);
		cfg_set_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV4, ifidxbuf);
		cfg_set_object_attr(waninfo_node, "isOndemandv4Del", "N/A");
	}
#ifdef IPV6
	if ( 1 == isInternet
		&& ( 1 == ipVersion || 2 == ipVersion )
		&& -1 == i_def_route_index_v6
		&& ( -1 == i_ondemand_route_index_v6 || ( i_ondemand_route_index_v6 >= 0 && if_index == i_ondemand_route_index_v6 ) ))
	{
		if ( i_ondemand_route_index_v6 >= 0 && if_index == i_ondemand_route_index_v6 )
		{
			snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route -A inet6 del default dev %s", ifname);
			system_escape(cmdbuf);
		}
		snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route -A inet6 add default dev %s", ifname);
		system_escape(cmdbuf);
		cfg_set_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV6, ifidxbuf);
		cfg_set_object_attr(waninfo_node, "isOndemandv6Del", "N/A");
	}
#endif
	set_portbinding_info_for_ondemand_wan(pvc_index, entry_index, ifname);

	return 0;
}


int check_add_route_ondemand(int if_index, int ipVersion)
{
	char status[8] = {0}, isDelAttr[8] = {0};
	char wan_node[32] = {0}, waninfo_node[32] = {0};
	int ret = 0;

	memset(waninfo_node, 0, sizeof(waninfo_node));
	snprintf(waninfo_node, sizeof(waninfo_node), WANINFO_ENTRY_NODE, if_index + 1);
	if ( 0 == ipVersion )
	{
		memset(status, 0, sizeof(status));
		memset(isDelAttr, 0, sizeof(isDelAttr));
		cfg_get_object_attr(waninfo_node, "Status", status, sizeof(status));
		cfg_get_object_attr(waninfo_node, "isOndemandv4Del", isDelAttr, sizeof(isDelAttr));
		if ( 0 != strcmp(status, "up") && ( 0 == isDelAttr[0] || 0 != strcmp(isDelAttr, "N/A") ) )
		{
			ret = 1;
			set_ondemand_route_state(if_index, 1);
		}
	}

	else if ( 1 == ipVersion )
	{
		memset(status, 0, sizeof(status));
		memset(isDelAttr, 0, sizeof(isDelAttr));
		cfg_get_object_attr(waninfo_node, "Status6", status, sizeof(status));
		cfg_get_object_attr(waninfo_node, "isOndemandv6Del", isDelAttr, sizeof(isDelAttr));
		if ( 0 != strcmp(status, "up") && ( 0 == isDelAttr[0] || 0 != strcmp(isDelAttr, "N/A") ) )
		{
			ret = 1;
			set_ondemand_route_state(if_index, 1);
		}
	}
	
	return ret;
}
void notify_OnDemandWan_Switch()
{
	int valid_ondemand_if[MAX_WAN_IF_INDEX] = {0};
	char ispMode[12] = {0}, connection[32] = {0}, str_oldifname[16] = {0};
	char serviceList[64] = {0}, connactive[8] = {0}, wan_lasterror[128] = {0};
	int idx = 0, pvc_index = 0, entry_index = 0, if_index = 0, ipVersion = 0;
	char def_route_index_v4[10] = {0}, def_route_index_v6[10] = {0};
	int i_def_route_index_v4 = -1, i_def_route_index_v6 = -1;
	char ondemand_route_index_v4[10] = {0}, ondemand_route_index_v6[10] = {0};
	int i_ondemand_route_index_v4 = -1, i_ondemand_route_index_v6 = -1;
	char pidbuf[64] = {0}, pidPath[128] = {0}, pidallbuf[128] = {0};
	int isFoundOnDemand = 0, isneedRestartDNS = 0;
	char cmdbuf[128] = {0}, str_idx[4] = {0};
	char wan_node[32] = {0};
	char wanboot[8] = {0};
	wan_entry_obj_t* obj = NULL;
	wan_entry_cfg_t* cfg = NULL;
	
	if ( cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", wanboot, sizeof(wanboot)) < 0 
		|| 0 != strcmp(wanboot, "1") )
		return;

	for( idx = 0; idx < MAX_WAN_IF_INDEX; idx ++)
		valid_ondemand_if[idx] = -1;

	
	for ( obj = svc_wan_get_obj_list(); obj; obj = obj->next )
	{
		cfg = &obj->cfg;
		
		if ( obj->idx >= MAX_WAN_IF_INDEX )
			return;
		
		pvc_index = obj->idx / SVC_WAN_MAX_PVC_NUM;
		entry_index = obj->idx % SVC_WAN_MAX_ENTRY_NUM;

		if ( get_ondemand_route_state(obj->idx) )
		{
			memset(pidbuf, 0, sizeof(pidbuf));
			memset(pidPath, 0, sizeof(pidPath));

			/* get ppp pid*/
			snprintf(pidPath, sizeof(pidPath), "%sppp%d.pid", "/var/run/", obj->idx);
			fileRead(pidPath, pidbuf, sizeof(pidbuf) -1);

			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf),"pidof pppd > %s", PPPD_PID_TEMP_PATH);
			system(cmdbuf);
			fileRead(PPPD_PID_TEMP_PATH, pidallbuf, sizeof(pidallbuf) -1);
			unlink(PPPD_PID_TEMP_PATH);
			if ( NULL != strstr(pidallbuf, pidbuf) ) /* pppd is up*/
			{
				if ( 0 == AddRouteforOnDemandWan(pvc_index, entry_index) )
					isneedRestartDNS = 1;
			}
			else
			{
				continue;
			}

			set_ondemand_route_state(obj->idx, 0);
		}

		if ( SVC_WAN_IS_CONN_ON_DEMAND(cfg) )
		{
			valid_ondemand_if[obj->idx] = obj->idx;
		}
	}

	if ( 1 == isneedRestartDNS )
	{				
		cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1") ;
	}
	
	/* get default route index for V4&V6*/
	memset(def_route_index_v4, 0, sizeof(def_route_index_v4));
	memset(def_route_index_v6, 0, sizeof(def_route_index_v6));
	if ( cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_DEFROUTEIDXV4, def_route_index_v4, sizeof(def_route_index_v4) ) > 0 
		&& '\0' != def_route_index_v4[0] && 0 != strcmp(def_route_index_v4, "N/A") )
		i_def_route_index_v4 = atoi(def_route_index_v4);
	if ( cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_DEFROUTEIDXV6, def_route_index_v6, sizeof(def_route_index_v6) ) > 0 
		&& '\0' != def_route_index_v6[0] && 0 != strcmp(def_route_index_v6, "N/A") )
		i_def_route_index_v6 = atoi(def_route_index_v6);
	
	if ( -1 != i_def_route_index_v4 && -1 != i_def_route_index_v6 )
		return; /* no need to check when V4/V6 default route exist*/

	/* get on-demand wan connection index for V4&V6*/
	memset(ondemand_route_index_v4, 0, sizeof(ondemand_route_index_v4));
	memset(ondemand_route_index_v6, 0, sizeof(ondemand_route_index_v6));
	if ( cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV4, ondemand_route_index_v4, sizeof(ondemand_route_index_v4) ) > 0 
		&& '\0' != ondemand_route_index_v4[0] && 0 != strcmp(ondemand_route_index_v4, "N/A") )
		i_ondemand_route_index_v4 = atoi(ondemand_route_index_v4);
	if ( cfg_get_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV6, ondemand_route_index_v6, sizeof(ondemand_route_index_v6) ) > 0 
		&& '\0' != ondemand_route_index_v6[0] && 0 != strcmp(ondemand_route_index_v6, "N/A") )
		i_ondemand_route_index_v6 = atoi(ondemand_route_index_v6);
	
	/* check self on-demand first*/
	if (  ( -1 == i_def_route_index_v4 && i_ondemand_route_index_v4 >= 0 )
		|| ( -1 == i_def_route_index_v6 && i_ondemand_route_index_v6 >= 0 ) )
	{
		if ( -1 == i_def_route_index_v4 && i_ondemand_route_index_v4 >= 0 )
		{
			if_index = i_ondemand_route_index_v4;
			ipVersion = 0;
		}
		else
		{
			if_index = i_ondemand_route_index_v6;
			ipVersion = 1;
		}

		pvc_index = if_index / SVC_WAN_MAX_PVC_NUM;
		entry_index = if_index % SVC_WAN_MAX_ENTRY_NUM;

		memset(wan_node, 0, sizeof(wan_node));
		snprintf(wan_node, sizeof(wan_node), WAN_PVC_ENTRY_NODE, pvc_index+1, entry_index+1);

		if ( cfg_get_object_attr(wan_node, "Active", connactive, sizeof(connactive)) < 0
			|| 0 != strcmp("Yes", connactive)
			|| cfg_get_object_attr(wan_node, "ISP", ispMode, sizeof(ispMode)) < 0
			|| atoi(ispMode) != SVC_WAN_ATTR_ISP_PPP
			|| cfg_get_object_attr(wan_node, "CONNECTION", connection, sizeof(connection)) < 0	
			|| 0 != strcmp(connection, "Connect_on_Demand")
			|| cfg_get_object_attr(wan_node, "ServiceList", serviceList, sizeof(serviceList)) < 0 
			|| NULL == strstr(serviceList, "INTERNET")
			||	cfg_get_object_attr(wan_node, "IFName", str_oldifname, sizeof(str_oldifname)) <= 0 )
		{
			cfg_set_object_attr(WANINFO_COMMON_NODE, ( 0 == ipVersion ) ? WAN_COMMON_ONDEMANDIDXV4 : WAN_COMMON_ONDEMANDIDXV6, "N/A");
			return; 
		}

		if ( cfg_get_object_attr(wan_node, "ConnectionError", wan_lasterror, sizeof(wan_lasterror)) < 0 
			||( 0 != strcmp(wan_lasterror, "ERROR_AUTHENTICATION_FAILURE")
			   &&  0 != strcmp(wan_lasterror, "ERROR_ISP_TIME_OUT")) )
		{
			if ( 0 != strcmp(wan_lasterror, "ERROR_IDLE_DISCONNECT") || 0 == check_add_route_ondemand(if_index, ipVersion) )
				return; /*no need to switch other on-demand wan connection.*/
		}
	}

	if ( -1 != i_ondemand_route_index_v4 && -1 != i_ondemand_route_index_v6 )
		return; /*no need to check when V4/V6 on-demand wan already exist*/

	if ( ( -1 != i_def_route_index_v4 && i_ondemand_route_index_v4 >= 0 )
		|| ( -1 != i_def_route_index_v6 && i_ondemand_route_index_v6 >= 0 ) )
		return; /* no need to check when V4/V6 on-demand wan already exist and default route exist*/

	for( idx = 0; idx < MAX_WAN_IF_INDEX; idx ++)
	{
		if ( -1 != valid_ondemand_if[idx]
			&& i_ondemand_route_index_v4 != valid_ondemand_if[idx]
			&& i_ondemand_route_index_v6 != valid_ondemand_if[idx])
		{
			isFoundOnDemand = 1;
			break;
		}
	}

	if (  (( -1 == i_def_route_index_v4 && -1 == i_ondemand_route_index_v4 )
			|| ( -1 == i_def_route_index_v6 && -1 == i_ondemand_route_index_v6 ))
		&& ( 0 == isFoundOnDemand ))
		return; /* no need to check when other on-demand wan isn't exist*/

	for ( obj = svc_wan_get_obj_list(); obj; obj = obj->next )
	{
		cfg = &obj->cfg;
		
		if ( obj->idx > MAX_WAN_IF_INDEX )
			return;
		
		if ( i_ondemand_route_index_v4 == obj->idx
			|| i_ondemand_route_index_v6 == obj->idx )
			continue;

		if ( !SVC_WAN_IS_ACTIVE(cfg) )
			continue;

		if ( !SVC_WAN_IS_PPP(cfg) )
			continue;
		
		if ( !SVC_WAN_IS_CONN_ON_DEMAND(cfg) )
			continue;

		if ( 0 == obj->dev[0] )
			continue;

		if ( !SVC_WAN_SRV_INTERNET(cfg) )
			continue;

		memset(wan_lasterror, 0, sizeof(wan_lasterror));
		if(cfg_get_object_attr(obj->path, "ConnectionError", wan_lasterror, sizeof(wan_lasterror)) < 0
			|| 0 == strcmp(wan_lasterror, "ERROR_AUTHENTICATION_FAILURE")
			|| 0 == strcmp(wan_lasterror, "ERROR_ISP_TIME_OUT"))
			continue;
		
		if ( !SVC_WAN_IS_IPV4(cfg) && !SVC_WAN_IS_IPV6(cfg) )
			continue;

		memset(str_idx, 0, sizeof(str_idx));
		snprintf(str_idx, sizeof(str_idx), "%u", obj->idx);
		/* switch to other V4 on-demand*/
		if ( ( 1 == isFoundOnDemand || i_ondemand_route_index_v4 >= 0 ) && SVC_WAN_IS_IPV4(cfg) )
		{
			if ( '\0' != str_oldifname[0] )
			{
				snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route del default dev %s", str_oldifname);
				system_escape(cmdbuf);
			}

			snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route add default dev %s", obj->dev);
			system_escape(cmdbuf);
			cfg_set_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV4, str_idx);
		}
		/* switch to other V6 on-demand*/
		if ( ( 1 == isFoundOnDemand || i_ondemand_route_index_v6 >= 0 ) && SVC_WAN_IS_IPV6(cfg) )
		{
			if ( '\0' != str_oldifname[0] )
			{
				snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route -A inet6 del default dev %s", str_oldifname);
				system_escape(cmdbuf);
			}

			snprintf(cmdbuf, sizeof(cmdbuf),"/sbin/route -A inet6 add default dev %s", obj->dev);
			system_escape(cmdbuf);
			cfg_set_object_attr(WANINFO_COMMON_NODE, WAN_COMMON_ONDEMANDIDXV6, str_idx);
		}

		return;
	}
}
#endif

static void prepare_wan_mgr_polling(void)
{
    init_dslite_global_variable();
}

void wan_mgr_ponstatus_lock_init(void){
	pthread_mutex_init(&PonStatusLock, NULL);
}

void wan_mgr_update_status(E_PON_STATUS_T status){
	pthread_mutex_lock(&PonStatusLock);
	newPonStatus = status;
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
	if ( E_WAN_PON_DOWN == newPonStatus )
	{
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "OLTDownUp", "1");
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "OLTRegCnt", "0");
	}
#endif
#endif
	pthread_mutex_unlock(&PonStatusLock);
}

E_PON_STATUS_T wan_mgr_get_status(void){
	E_PON_STATUS_T status = E_WAN_PON_NULL;
	
	pthread_mutex_lock(&PonStatusLock);
	status = newPonStatus;
	/*newPonStatus = E_WAN_PON_NULL;*/
	pthread_mutex_unlock(&PonStatusLock);
	
	return status;
}


#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
int checkOLTstate()
{
	int checkTime = 0;
	int isOLT_downup = 0, isromfile_chg = 0;
	int olt_reg_counter = 0;
	char buf[4] = {0};

	memset(buf, 0, sizeof(buf));
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "OLTDownUp", 0, buf, sizeof(buf));
	isOLT_downup = atoi(buf);
	
	checkTime = ( 1 == isOLT_downup ) ? OLT_REG_SUCC_TIME_1 : OLT_REG_SUCC_TIME_2;
	if ( E_WAN_PON_DOWN == wan_mgr_get_status() )
		return -1;

	memset(buf, 0, sizeof(buf));
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "romfileChg", 0, buf, sizeof(buf));
	isromfile_chg = atoi(buf);
	if ( 1 != isromfile_chg && 0 == isOLT_downup )
		return -1;

	memset(buf, 0, sizeof(buf));
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "OLTRegCnt", 0, buf, sizeof(buf));
	olt_reg_counter = atoi(buf);

	if ( olt_reg_counter <= 2 )
		tcdbg_printf("\nit will start to check loid valid=[%d]! \n", olt_reg_counter);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d", ++olt_reg_counter);
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "OLTRegCnt", buf);
	if ( olt_reg_counter >= checkTime )
	{
		tcdbg_printf("\nloid valid check is end! \n");
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "OLTDownUp", "0");
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "OLTRegCnt", "0");
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "romfileChg", "0");
		saveLoid();
	}

	return 0;
}
#endif
#endif
#if defined(TCSUPPORT_CT_JOYME4)
void wan_related_cfg_process()
{
	char wanboot[8] = {0}, cmdbuf[128] = {0}, path[128] = {0}, wan_node[32] = {0};
	char wan_lasterror[128] = {0}, PPP_HYBRID[12] = {0}, LINKMODE[24] = {0};
	char avalActive[12] = {0};
	int pvc_index = 0, entry_index = 0, pid = 0, aval_need_start = 0, aval_need_tcp_start = 0;
	int is_avalActive_switch = 0, is_hwnat_ok = 0, meter_mode = -1, ret = 0;
	int route_flag = 0, other_flag = 0;
	static int is_avalanche_on = 0, is_first_boot = 1, is_aval_TCPSESSION_on = 0;
#if defined(TCSUPPORT_CT_JOYME4_JC)
	char aval_vlan[32] = {0}; 
	int aval_r_vid = 0, aval_b_vid = 0, aval_b_mvid = 0;
#endif
	wan_entry_obj_t* obj = NULL;
	wan_entry_cfg_t* cfg = NULL;
	
	if ( cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", wanboot, sizeof(wanboot)) < 0 
		|| 0 != strcmp(wanboot, "1") )
		return;

	if ( cfg_get_object_attr(SYSTEM_ENTRY_NODE, "avalActive", 
			avalActive, sizeof(avalActive)) > 0
			&& (0 == strcmp(avalActive, "1")) )
		is_avalActive_switch = 1;

	/*
	 hwnat ko is not loaded when wan boot
	 need re-install rules for wan when hwnat loaded 1st boot.
	*/
	ret = blapi_traffic_get_ratelimit_mode(DOWN_DIR, &meter_mode);
	if ( is_first_boot && -1 != meter_mode )
		is_hwnat_ok = 1;
#if defined(TCSUPPORT_CT_JOYME4_JC)
	if( cfg_get_object_attr(DEVICEINFO_DEVPARADYNAMIC_NODE, "secret_iv", aval_vlan, sizeof(aval_vlan)) > 0 )
		if( 3 != sscanf(aval_vlan, "%d:%d:%d", &aval_r_vid, &aval_b_vid, &aval_b_mvid) )
		{
			aval_r_vid = 0;
			aval_b_vid = 0;
			aval_b_mvid = 0;
		}
#endif

	for ( obj = svc_wan_get_obj_list(); obj; obj = obj->next )
	{
		cfg = &obj->cfg;
		
		if ( obj->idx > MAX_WAN_IF_INDEX )
			return;
		pvc_index = obj->idx / SVC_WAN_MAX_PVC_NUM;
		entry_index = obj->idx % SVC_WAN_MAX_ENTRY_NUM;
		snprintf(wan_node, sizeof(wan_node), WAN_PVC_ENTRY_NODE, pvc_index + 1, entry_index + 1);

		if ( is_avalActive_switch )
		{
			if ( SVC_WAN_SRV_INTERNET(cfg)
				&& 0 == route_flag
				&& (SVC_WAN_IS_ROUTE(cfg))
				&& (SVC_WAN_ATTR_ISP_STATIC == cfg->isp) )
			{
				aval_need_tcp_start = 1;
#if defined(TCSUPPORT_CT_JOYME4_JC)
				if( aval_r_vid != 0 && aval_b_vid != 0 && aval_b_mvid != 0 )
				{
					if( aval_r_vid == cfg->vid 
						|| -1 == aval_r_vid )
						route_flag = 1;
				}
				else
#endif
				if ( 900 == cfg->vid )
				{
					route_flag = 1;
				}
			}

			if( SVC_WAN_SRV_OTHER(cfg)
				&& 0 == other_flag 
				&& SVC_WAN_IS_BRIDGE(cfg) )
			{
#if defined(TCSUPPORT_CT_JOYME4_JC)
				if( aval_r_vid != 0 && aval_b_vid != 0 && aval_b_mvid != 0 )
				{
					if( ( aval_b_vid == cfg->vid || -1 == aval_b_vid )
						&& ( aval_b_mvid == cfg->mvid || -1 == aval_b_mvid ) )
						other_flag = 1;
				}
				else
#endif
				if( 900 == cfg->vid && 901 == cfg->mvid )
				{
					other_flag = 1;
				}
			}

			if( route_flag && other_flag )
				aval_need_start = 1;
			
		}

		/* check PPP-bridge interface state. */
		if ( SVC_WAN_IS_PPP(cfg)
			&& (obj->flag & SVC_WAN_FLAG_PPP_RELAY_CHECK) )
		{
			/* check ppp wan state. */
			if ( !(obj->flag & (SVC_WAN_FLAG_UPV4 | SVC_WAN_FLAG_UPV6 )) )
			{
				/* check ppp error code. */
				if ( cfg_get_object_attr(wan_node, "ConnectionError", wan_lasterror
					, sizeof(wan_lasterror)) <= 0 )
					continue;

				if ( !(0 == strcmp(wan_lasterror, "ERROR_AUTHENTICATION_FAILURE")
					/* || 0 == strcmp(wan_lasterror, "ERROR_ISP_DISCONNECT") */) )
					continue;
			}

			obj->flag &= (~SVC_WAN_FLAG_PPP_RELAY_CHECK);
			bzero(cmdbuf, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/pppoe-relay -C br0 -S %s &", obj->dev);
			svc_wan_execute_cmd(cmdbuf);

#if 0
            memset(cmdbuf, 0, sizeof(cmdbuf));
            snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/ip link set br0 promisc on");
            svc_wan_execute_cmd(cmdbuf);
#endif

			/* Redesignate process priority */
			bzero(path, sizeof(path));
			snprintf(path, sizeof(path), "/var/run/pppoe-relay_%s.pid", obj->dev);
			pid = svc_wan_get_pid(path);
			if( pid > 0 )
			{
				bzero(cmdbuf, sizeof(cmdbuf));
				snprintf(cmdbuf, sizeof(cmdbuf), "renice -10 %d", pid);
				svc_wan_execute_cmd(cmdbuf);
			}
		}

		if ( SVC_WAN_ATTR_ACTIVE_ENABLE == cfg->active )
		{
			if ( is_hwnat_ok )
			{
				bzero(path, sizeof(path));
				snprintf(path, sizeof(path), WANCFGDATA_ENTRY_NODE, obj->idx + 1);
				cfg_commit_object(path);
				is_first_boot = 0;
			}
		}
	}

	if ( is_avalActive_switch )
	{
		if ( aval_need_start )
		{
			if ( 0 == is_avalanche_on )
			{
				is_avalanche_on = 1;
				is_aval_TCPSESSION_on = 1;
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "is_avalanche_start", "Yes");
				system_escape("/usr/script/avalanche_start.sh &");
			}
		}
		else
		{
			if ( 1 == is_avalanche_on )
			{
				is_avalanche_on = 0;
				cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "is_avalanche_start", "");
				system_escape("/usr/script/avalanche_stop.sh &");
			}

			/*  change conntrack only one time. */
			if ( aval_need_tcp_start
				&& 0 == is_aval_TCPSESSION_on )
			{
				is_aval_TCPSESSION_on = 1;
				blapi_perform_avaTcpTest();
			}

		}
	}
}

void checkBssCapabilityAddrChanged()
{
	char wanboot[4] = {0}, bssaddr_status[4] = {0};
	char capabilityaddr[64] = {0};
	static char capabilityaddr_last[64] = {0};
	char temp_buf[8] = {0}, retrytimes[4] = {0};
	int cnt = 0, i_retrytimes = 0, need_update = 0, timeout = 0;
	int timeout_tab[8] = {30, 30, 30, 60, 60, 120, 120, 300};

	/* wait for wan boot complete */
	if ( cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", wanboot, sizeof(wanboot)) < 0 
		|| 0 != strcmp(wanboot, "1") )
		return;

	/* do nothing until bssaddr_changed or internet wan up/down */
	if ( cfg_get_object_attr(GLOBALSTATE_NODE, "bssaddr_changed", bssaddr_status, sizeof(bssaddr_status)) < 0 
		|| 0 != strcmp(bssaddr_status, "1") )
		return;

	/* check capability addr changed in 30s */
	memset(capabilityaddr, 0, sizeof(capabilityaddr));
	cfg_get_object_attr(PLUGIN_COMMON_NODE, "CapabilityAddr", capabilityaddr, sizeof(capabilityaddr));
	if ( 0 == capabilityaddr[0] || 0 == strcmp(capabilityaddr, capabilityaddr_last) )
	{
		memset(temp_buf, 0, sizeof(temp_buf));
		cfg_obj_get_object_attr(GLOBALSTATE_NODE, "bssaddr_times", 0, temp_buf, sizeof(temp_buf));
		cnt = atoi(temp_buf) + 1;

		memset(retrytimes, 0, sizeof(retrytimes));
		cfg_get_object_attr(GLOBALSTATE_NODE, "bss_dnsretrytimes", retrytimes, sizeof(retrytimes));
		i_retrytimes = atoi(retrytimes);
		if ( i_retrytimes >= 8 )
			timeout = timeout_tab[7];
		else
			timeout = timeout_tab[i_retrytimes];
		
		/* do not update bss ifc vip rules and reset bssaddr_changed & bssaddr_times,
		** when  capability addr is not changed in 30s.
		*/
		if ( cnt >= timeout )
		{
			if ( 0 != capabilityaddr[0] )
				need_update = 1; /* update bss ifc vip rules, to avoid default wan was not up normal before */
		}

		memset(temp_buf, 0, sizeof(temp_buf));
		snprintf(temp_buf, sizeof(temp_buf), "%d", cnt);
		cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_times", temp_buf);
	}
	else
	{
		/* save new capability addr, update bss ifc vip rules */
		strncpy(capabilityaddr_last, capabilityaddr, sizeof(capabilityaddr_last) - 1);
		need_update = 1;
	}

	if ( 0 == need_update )
		return;

	cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_changed", "0");
	cfg_set_object_attr(GLOBALSTATE_NODE, "bssaddr_times", "0");
	svc_wan_mgr_handle_event(EVT_CFG_WAN_BSSADDR_UPDATE, "");
	return;
}
#endif

static void wan_mgr_polling_pthread(void)
{
    static int count = 0;
    static int tCount = 0;
    static int Flag_Check = 0;
    int gWait = 0;
    int lastStatus = 0;
	E_PON_STATUS_T ponStatus = E_WAN_PON_NULL;
	static E_PON_STATUS_T old_ponStatus = E_WAN_PON_NULL;
	char load_status[16] = {0};
#if defined(TCSUPPORT_NP_CMCC)
	char curAPMode[16] = {0};
#endif
#if defined(TCSUPPORT_CT_UBUS)
	int repeater_wanboot = 0;
	char workmode[16] = {0}, wlanload[4] = {0}, wanboot[4] = {0};
#endif	
	static int ponFirstUp = 1;
    prepare_wan_mgr_polling();

    while(1)
    {    
    	sleep(1);
		
		if(checkCFGload() == 0){
			continue;
		}

		ponStatus = wan_mgr_get_status();
        if(gWait == 0 && E_WAN_PON_NULL != ponStatus && old_ponStatus != ponStatus){
#if defined(TCSUPPORT_CT_DSL_EX)	
			old_ponStatus = ponStatus;
			svc_wan_mgr_handle_event(E_WAN_PON_UP == ponStatus ? EVT_XPON_UP : EVT_XPON_DOWN, "");			
#else			
			if(ponStatus == E_WAN_PON_DOWN && old_ponStatus == E_WAN_PON_NULL){
			}else{		
			old_ponStatus = ponStatus;
			if(ponFirstUp == 1 && E_WAN_PON_UP == ponStatus){
				ponFirstUp = 0;
			}else{
			svc_wan_mgr_handle_event(E_WAN_PON_UP == ponStatus ? EVT_XPON_UP : EVT_XPON_DOWN, "");
			}
            gWait = XPON_CHANGE_MAX_SLEEP_TIME;
			}
#endif
    	}else if(gWait > 0){
            gWait--;
    	}
		
#if defined(TCSUPPORT_NP_CMCC)
		np_wan_start_process();
#endif

#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
		checkOLTstate();
#endif
#endif
#if defined(TCSUPPORT_CT_UBUS)
		if ( 0 == repeater_wanboot )
		{
			/* repeater wan interface boot check */
			cfg_get_object_attr(WAN_COMMON_NODE, "WorkMode", workmode, sizeof(workmode));
			if ( 0 != workmode[0] && 0 == strcmp(workmode, "repeater") )
			{
				cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", wanboot, sizeof(wanboot));
				if ( 0 == wanboot[0] || 0 != strcmp(wanboot, "1") )
				{
					cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanLoad", wlanload, sizeof(wlanload));
					if ( 0 != wlanload[0] && 0 == strcmp(wlanload, "1") )
					{
						tcdbg_printf("\nrepeater wan boot...\n");
						svc_wan_mgr_handle_event(EVT_CFG_WAN_ENTRY_BOOT, "");
						repeater_wanboot++;
					}
				}
			}
			else
				repeater_wanboot++;
		}
#endif

    	check_internet_connect();

        if ( E_WAN_PON_UP == ponStatus )
        	update_ipv6_slaac_addr_and_gateway();

        if(3 == count)
        {
#if defined(TCSUPPORT_CT_DSLITE)
            notify_dslite_state();
#endif
            count = 0;
        }

#if defined(TCSUPPORT_CT_DNSMASQ_MONITOR)
		tCount++;
		if( tCount > DNS_CHECK_TIME ){	 
			tCount = 0;
			if( dnsmasq_check() == 0 ){
				if(Flag_Check){ 		 /* dnsmasq does not exist for the second time */
					Flag_Check=0;
					tcdbg_printf("restart dnsmasq!\n");		
					cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1") ;
				}else { 				 /* dnsmasq does not exist for the first time */
					Flag_Check=1;
				}
			}else Flag_Check=0; 		 /* dnsmasq exists */
		}
#endif
		
		checkDnsMaskRestart();

#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
		notify_OnDemandWan_Switch();
#endif

#if defined(TCSUPPORT_CT_JOYME4)
		wan_related_cfg_process();

		checkBssCapabilityAddrChanged();
#endif

#ifdef TCSUPPORT_TEST_WLAN_SHORTCUT
		blapi_perform_check_wifi_ixia_test();
#endif
        count++;
    }
    
    pthread_exit(0);
}

int creat_wan_mgr_polling_pthread(void)
{
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, (void *)wan_mgr_polling_pthread, NULL) < 0)
    {
        printf("creat_wan_mgr_polling_pthread: create thread fail \n");
        return -1;
    }

    return 0;
}

