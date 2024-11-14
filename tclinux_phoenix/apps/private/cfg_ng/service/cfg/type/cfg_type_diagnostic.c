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
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <setjmp.h>
#include "utility.h"
#include "blapi_traffic.h"
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
#include "blapi_xpon.h"
#endif
#endif
#ifdef TCSUPPORT_CT_UBUS
#include <fcntl.h>
#endif

sigjmp_buf jmpbuf;

#define TEST_OK	"PASS"
#define TEST_SKIP "Skipped"
#define TEST_FAIL	"Fail"

#if defined(TCSUPPORT_CT_E8GUI)
#define RECV0ICMPPKT	"0 packets received"
#define RECVICMPPKT	"packets received"
#endif
#define RECV1ICMPPKT	"1 packets received"

#if defined(TCSUPPORT_CT_E8GUI)
#define TMP_TRACE_PATH "/tmp/traceOth"
#endif

#define ACTIVE		"Yes"

#define YAHOO_DNS	"www.yahoo.com"
#define TMP_PING_DNS_PATH "/tmp/pingDns"
#define TMP_PING_YAH_PATH "/tmp/pingYah"
#define TMP_PING_OTHER_PATH "/tmp/pingOth"  /*rodney_20090506 added*/
#define TMP_PING_TS_PATH  "/tmp/pingTs"  /*rodney_20090507 added*/
#define TMP_DNS_DIAG_PATH "/tmp/diagdns.conf"

#define OAM_PING_PATH	"/proc/tc3162/oam_ping"

#define NAS_IF	"nas%d"
#define PPP_IF	"ppp%d"

#define ADDROUTE_CMD	"route add -host %s dev %s"
#define DELROUTE_CMD	"route del %s"
#define PING_CMD_TIMES	"/bin/ping -c %s -W 2 %s > %s"
#define PING6_CMD_TIMES	"/bin/ping6 -I %s -c %s %s > %s"
#define TRACE_CMD	"traceroute -n -m 30 -a -w 2 -i %s > %s"
#define TRACE6_CMD	"/userfs/bin/traceroute6 -n -m 30 -w 2  -I --icmp -i %s  %s > %s"
#define ROUTE6_ADD_CMD  "ip -6 route add %s via %s dev %s"
#define ROUTE6_DEL_CMD  "ip -6 route del %s"

#define TIME_CMD	"/bin/date > /tmp/time"
#define SYSROUTE_CMD	"/sbin/route -n > /tmp/route"
#define SKBMGR_CMD	"echo %s > /proc/net/skbmgr_hot_list_len"
#define REBOOT_CMD	"/userfs/bin/mtd -r write %s romfile"
#define PING_CMD	"/bin/ping -c 1 -W 2 %s > %s"

#if defined(TCSUPPORT_CT_PON_GD)
#define REG_PING_CMD_TIMES	"/bin/ping -t -c %s -W 2 %s > %s"
#define REG_PING6_CMD_TIMES	"/bin/ping6 -t -I %s -c %s %s > %s"
#endif


#define DEF_GW_FLAG	"UG"
#define TMP_CHECK_IF "tmp/iface.conf"

#define PPP_DNS_INFO_PATH	"/etc/ppp/resolv.conf"

#define ATM_OAM_PING_SEGM	0
#define ATM_OAM_PING_E2E	1

#define NOT_CONNECTED	"0"
#define CONNECTED	"1"

#define MAX_BUF_SIZE	2048
#define BUF_SIZE_512	512

#ifdef TCSUPPORT_CT_UBUS
#define IFA_STATUS_TMP 			"/tmp/interface_status_tmp"
#define ETHIFA_STATUS_CMD 		"/sbin/ifconfig eth0.%d > /tmp/interface_status_tmp"
#define WLANIFA_STATUS_CMD		"/sbin/ifconfig ra%d > /tmp/interface_status_tmp"
#define WLANACIFA_STATUS_CMD 	"/sbin/ifconfig rai%d > /tmp/interface_status_tmp"
#define USBIFA_PATH 			"/sys/bus/usb"
#define IFA_STATUS 				"/tmp/interface_status"
#define IFA_STATUS_CMD 			"echo '%d=%s' >> /tmp/interface_status"
#endif

typedef enum {
	XPON_LINK_OFF = 0,
	XPON_LINK_GPON,
	XPON_LINK_EPON
} xPONLinkStatus_t ;


static char* cfg_type_diagnostic_index[] = { 
	 "diag_pvc", 
	 NULL 
}; 


int cfg_get_dslite_stateviacfg( char *nodeName)
{
	char dslite_enable[8] = {0};
	char serviceList[64] = {0};
	char isp[5] = {0};

	if( cfg_obj_get_object_attr(nodeName, "DsliteEnable", 0, dslite_enable, sizeof(dslite_enable)) < 0 )
		return 0;
	if( cfg_obj_get_object_attr(nodeName, "ServiceList", 0, serviceList, sizeof(serviceList)) )
		return 0;
	if( cfg_obj_get_object_attr(nodeName, "ISP", 0, isp, sizeof(isp)) < 0 )
		return 0;

	if( NULL != strstr(serviceList, "INTERNET")
		&& 0 != strcmp(isp, "3")  /* not BRIDGE_MODE */
		&& 0 == strcmp("Yes", dslite_enable))
		return 1;
	else
		return 0;
}



int cfg_checkWan_by_name(char *wan_if_name, int *ipMode)
{
	int if_index;
	char nodeName[64];
	char temp[32] = {0};	
	char str_ipversion[16] = {0};
#if defined(TCSUPPORT_ANDLINK)
	char CurAPMode[12] = {0};
#endif
	
	if_index = get_wanindex_by_name(wan_if_name);
	if(get_waninfo_by_index(if_index, "Active", temp, sizeof(temp)) == 0 && strcmp(temp, ACTIVE) == 0){ 
		if(if_index < 0 || if_index > PVC_NUM * MAX_SMUX_NUM)
			return -1;
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, if_index/MAX_SMUX_NUM + 1, if_index%MAX_SMUX_NUM + 1);
		if ( cfg_obj_get_object_attr(nodeName, "IPVERSION", 0, str_ipversion, sizeof(str_ipversion)) < 0 )
		{
			strncpy(str_ipversion, "IPv4", sizeof(str_ipversion)-1);
		}
		if ( 0 == strcmp(str_ipversion,"IPv4") )
			*ipMode= 1;
		else
		{
#if defined(TCSUPPORT_CT_DSLITE)
			if ( 1 == cfg_get_dslite_stateviacfg(nodeName) )
				*ipMode = 4;
			else
#endif
			{
				if ( 0 == strcmp(str_ipversion,"IPv6") )
					*ipMode= 2;
				else
					*ipMode= 3;
			}
		}

		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, if_index + 1);
		memset(temp, 0, sizeof(temp));
		if(cfg_obj_get_object_attr(nodeName, "GateWay", 0, temp, sizeof(temp)) > 0){
			if(strlen(temp) != 0 && strcmp(temp,"N/A")){
				return 0;
			}
		}
		if(cfg_obj_get_object_attr(nodeName, "GateWay6", 0, temp, sizeof(temp)) > 0){
			if(strlen(temp) != 0 && strcmp(temp,"N/A")){
				return 0;
			}
		}

	}
#if defined(TCSUPPORT_ANDLINK)
	else if(( cfg_obj_get_object_attr(WANINFO_COMMON_NODE, "CurAPMode", 0, CurAPMode, sizeof(CurAPMode)) >= 0 )
		&& (strcmp(CurAPMode, "Bridge") == 0 || strcmp(CurAPMode, "APClient") == 0 ))
	{
		*ipMode= 1;
		return 0;
	}
#endif
	
	return -1;

}


#if defined(TCSUPPORT_CT_PON_GD)
void cfg_pingDiag(char *convertIP, char *buf_route, char *buf_times, int wanIPmode, int iFlagBandRegDiag)
#else
void cfg_pingDiag(char *convertIP, char *buf_route, char *buf_times, int wanIPmode)
#endif
{
	char addrouteCMD[128]={0};
	char pingCMD[128]={0};
	char devName[64] = {0};

	snprintf(devName, sizeof(devName) - 1, "%s%s", 
		( (4 == wanIPmode) ? "ds." : "" ), buf_route);

	/*execute [route add cmd]*/
#if !defined(TCSUPPORT_ANDLINK)
	snprintf(addrouteCMD,sizeof(addrouteCMD),ADDROUTE_CMD, convertIP,devName);
	system_escape(addrouteCMD);
#endif
	
	/*execute [ping cmd]*/
#if defined(TCSUPPORT_CT_PON_GD)
	if(iFlagBandRegDiag)
	{
		snprintf(pingCMD, sizeof(pingCMD), REG_PING_CMD_TIMES, buf_times, convertIP, TMP_PING_OTHER_PATH);
	}
	else
	{
		snprintf(pingCMD, sizeof(pingCMD), PING_CMD_TIMES, buf_times, convertIP, TMP_PING_OTHER_PATH);
	}
#else
	snprintf(pingCMD, sizeof(pingCMD), PING_CMD_TIMES, buf_times, convertIP, TMP_PING_OTHER_PATH);
#endif

	system_escape(pingCMD);

	/*execute [route delete cmd]*/			
	snprintf(addrouteCMD, sizeof(addrouteCMD), DELROUTE_CMD, convertIP);
	system_escape(addrouteCMD);
}

void cfg_ping6Diag(char *convertIP, char *buf_route, char *buf_times)
{
	char pingCMD[128]={0};
	char nodeName[64] = {0};
	char ip[40] = {0};
	int index = get_wanindex_by_name(buf_route);
	snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, index+1);
	
	if(cfg_obj_get_object_attr(nodeName,"GateWay6", 0, ip, sizeof(ip)) < 0)
		return;

	snprintf(pingCMD, sizeof(pingCMD), ROUTE6_ADD_CMD, convertIP, ip, buf_route);
	system_escape(pingCMD);
	memset(pingCMD, 0, sizeof(pingCMD));
	snprintf(pingCMD, sizeof(pingCMD), PING6_CMD_TIMES, buf_route, buf_times, convertIP, TMP_PING_OTHER_PATH);
	system_escape(pingCMD);
	memset(pingCMD, 0, sizeof(pingCMD));
	snprintf(pingCMD, sizeof(pingCMD), ROUTE6_DEL_CMD, convertIP);
	system_escape(pingCMD);

}


void cfg_tracerouteDiag(char *convertIP, char *buf_route, int wanIPmode){
	char traceroutebuf[128]={0};
	char devName[64] = {0};

	snprintf(devName, sizeof(devName) - 1, "%s%s", 
		( (4 == wanIPmode) ? "ds." : "" ), buf_route);

	/*execute [route add cmd]*/
	snprintf(traceroutebuf, sizeof(traceroutebuf), ADDROUTE_CMD, convertIP,devName);
	system_escape(traceroutebuf);
	
	/*execute [traceroute cmd]*/
	memset(traceroutebuf,0,sizeof(traceroutebuf));
	snprintf(traceroutebuf, sizeof(traceroutebuf), TRACE_CMD, convertIP, TMP_TRACE_PATH);
	system_escape(traceroutebuf);

	/*execute [route delete cmd]*/
	memset(traceroutebuf,0,sizeof(traceroutebuf));
	snprintf(traceroutebuf, sizeof(traceroutebuf), DELROUTE_CMD, convertIP);
	system_escape(traceroutebuf);
}

void cfg_traceroute6Diag(char *convertIP, char *buf_route){
	char traceroutebuf[128]={0};
	char ip[40] = {0};
	char nodeName[64] = {0};
	int index = get_wanindex_by_name(buf_route);

	snprintf(nodeName, sizeof(nodeName), WANINFO_ENTRY_NODE, index+1);
	
	if(cfg_obj_get_object_attr(nodeName,"GateWay6", 0, ip, sizeof(ip)) < 0)
		return;

	memset(traceroutebuf, 0, sizeof(traceroutebuf));
	snprintf(traceroutebuf, sizeof(traceroutebuf), ROUTE6_ADD_CMD, convertIP, ip, buf_route);
	printf("\n%s\n", traceroutebuf);
	system_escape(traceroutebuf);
	memset(traceroutebuf, 0, sizeof(traceroutebuf));
	snprintf(traceroutebuf, sizeof(traceroutebuf), TRACE6_CMD, buf_route, convertIP, TMP_TRACE_PATH);
	printf("\n%s\n", traceroutebuf);
	system_escape(traceroutebuf);
	memset(traceroutebuf, 0, sizeof(traceroutebuf));
	snprintf(traceroutebuf, sizeof(traceroutebuf), ROUTE6_DEL_CMD, convertIP);
	printf("\n%s\n", traceroutebuf);
	system_escape(traceroutebuf);
}



static void alarm_func()
{
	siglongjmp(jmpbuf,1);
}


/*This function only for ping/traceroute diagnose, and it is Not reentrant*/
int tc_getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result, int timeout)
{
	int reval = -1;
	signal(SIGALRM, alarm_func);
	if(sigsetjmp(jmpbuf, 1) != 0)
	{
		alarm(0);/*timeout*/
		signal(SIGALRM, SIG_IGN);
		return reval;
	}
	alarm(timeout);
	reval = getaddrinfo(hostname, service, hints, result);
	signal(SIGALRM, SIG_IGN);

	return reval;
}


static int cfg_type_diagnostic_pvc_func_get(char* path,char* attr, char* val,int len) 
{
	if(cfg_query_object(DIAGNOSTIC_NODE, NULL, NULL)<= 0)
	{
		cfg_create_object(DIAGNOSTIC_NODE);
	}
	return cfg_type_default_func_get(path,attr,val,len); 
} 


void cfg_F5LoopbacktestResult(char *path,char *result)
{
	FILE  *fp = NULL;
	int index = 0;
	char buf[128] = {0};
	char *pbegin = NULL;
	char *pend = NULL;

	fp = fopen(path,"r");
	if(fp  == NULL){
		return;
	}

	while (fgets(buf, sizeof(buf), fp)) 
	{
		if((pbegin = strstr(buf,"SuccessCount:")) == NULL)
			continue;
		pend = strstr(buf," ");
		index = pend - pbegin -strlen("SuccessCount:");
		strncpy(result,pbegin+strlen("SuccessCount:"),index);
	}
	
	fclose(fp);
	return;
}


void cfg_test_atm_oam_ping(char *name,int type, char *buf){
	char arg[32]={0};
	#ifdef CMD_API
	char value[10]={0};
	#else
	char value[4]={0};
	#endif
	char ping_cmd[64]={0};
	char wanNodeName[64];
	char wanInfo[3][8];/*0:Active status, 1:VPI,2:VCI*/
	int i=0;
	char ping_para_keywd[][32]=
	{
		{"PVCACTIVE"},
		{"VPI"},
		{"VCI"},
		{""}
	};

	memset(wanInfo,0,sizeof(wanInfo));
	memset(wanNodeName, 0, sizeof(wanNodeName));

	snprintf(wanNodeName, sizeof(wanNodeName), "root.wan.%s", name);

	for(i=0;strlen(ping_para_keywd[i])!=0; i++){
		cfg_obj_get_object_attr(wanNodeName, ping_para_keywd[i], 0, wanInfo[i], sizeof(wanInfo[i]));
	}

	if(strcmp( wanInfo[0], ACTIVE) ==0){
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
		if (0 == blapi_pon_get_oam_ping_value(wanInfo[1], wanInfo[2], type)) {
			strncpy(buf, TEST_OK, 7);
			return;
		}
#endif
#endif
	}
	strncpy(buf, TEST_FAIL, 7);
}/*end test_atm_oam_ping*/

static int cfg_type_diagnostic_func_execute(char *path)
{

	char diagnostic_attr[][20]=
	{
		{"EtherLanConn"},
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
		{"xPONLinkSta"},
		{"xPONAppLinkSta"},
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
		{"EthernetLinkSta"},
#endif
#else
		{"ADSLSyn"},
		{"ATMOAMSeg"},{"ATMOAMEnd2End"},
#endif
		{"PingPriDNS"},{"PingYahoo"},
		{"PingOther"},
#if defined(TCSUPPORT_CT_E8GUI)
		{"TraceResult"},
#endif
		{""}  /*rodney_20090506 added*/ 
	};

	enum diagnostic_en{
		EthLanConn=0,
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
		XPONLinkStatus,
		XPONAppLinkStatus,
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
		EthernetLinkStatus,
#endif
#else
		AdslSyn,
		AtmOAMSeg,
		AtmOAME2E,
#endif

		PingPriDNS,
		PingYahoo,
		PingOther,	/*rodney_20090506 added*/
#if defined(TCSUPPORT_CT_E8GUI)
		TraceResult,
#endif
		Max_diag,
	};
	int rt = -1;
	int flag = 0;
	char nodeName[64];
	char buf[64]={0};
	char *tmpBuf=NULL;
	char dev[8]={0};
	char diag_value[Max_diag][8];
	int i=0;
	char pvc[4]={0};
	int flagRet = 0;
	char cmdbuf[128] = {0};
	int ipVersion = 1;
#if defined(TCSUPPORT_CT_E8GUI)
	char buf_times[32]={0};
	char buf_route[32]={0};
	struct sockaddr_in cvtaddr;
	struct sockaddr_in *cvtaddr2  = NULL;
	char convertIP[64]={0};
	char filebuf[512]={0};
	char filler1[16]={0},filler2[16]={0},filler3[16]={0},filler4[16]={0},filler5[16]={0},
		 filler6[16]={0},filler7[16]={0},filler8[16]={0},filler9[16]={0};
	struct sockaddr_in6 ipv6addr;
	struct sockaddr_in6 *ipv6addr2 = NULL;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
#if defined(TCSUPPORT_CT_PON_GDV20)
	struct addrinfo hintsv6;
	struct addrinfo *resv6 = NULL;	
	char convertIPv6[64]={0};
#endif
#endif
	int pvcindex = 0;
	char pvcName[8] = {0};
#if defined(TCSUPPORT_CT_PON_GD)
	int iFlagBandRegDiag = 0;
#endif
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
	char transMode[16] = {0};
#endif
	int xdsl_type = -1;

	/*Init Test value*/
	for(i=0; strlen(diagnostic_attr[i])!=0; i++){
		strncpy(diag_value[i], TEST_FAIL, 7);
	}

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName)-1);
	/*Get the pvc value that is user try to test*/
	cfg_obj_get_object_attr(nodeName, "diag_pvc", 0, pvc, sizeof(pvc));

	cfg_create_object(path);

	rt = blapi_traffic_check_eth_link_status(&flag);
	if(flag == 1)
	{
		strncpy(diag_value[EthLanConn],TEST_OK, 7);
	}

	if(get_entry_number_cfg2(path, "pvc.", &pvcindex) < 0){
		return -1;
	}
	snprintf(pvcName, sizeof(pvcName), "pvc.%d", pvcindex);

#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
	/*get xpon driver mpcp/ploam status*/
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, XPON_LINKCFG_NODE, sizeof(nodeName)-1);
	if (cfg_obj_get_object_attr(nodeName, "LinkSta", 0, buf, sizeof(buf)) > 0){
		if (atoi(buf) == XPON_LINK_GPON || atoi(buf) == XPON_LINK_EPON){ /*GPON link || EPON link*/
			strncpy(diag_value[XPONLinkStatus], TEST_OK, 7);
		}
	}

	/*get xpon app oam/omci status*/
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, XPON_COMMON_NODE, sizeof(nodeName)-1);
	if (cfg_obj_get_object_attr(nodeName, "trafficStatus", 0, buf, sizeof(buf)) > 0){
		if (strcmp(buf, "up") == 0){ /*traffic link status*/
			strncpy(diag_value[XPONAppLinkStatus], TEST_OK, 7);
		}
	}
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, WAN_COMMON_NODE, sizeof(nodeName)-1);
	if (cfg_obj_get_object_attr(nodeName, "TransMode", 0,transMode,sizeof(transMode)) >= 0){
		if ((strcmp(transMode, "Ethernet") == 0) || (strcmp(transMode, "ActiveEtherWan") == 0)){
			memset(nodeName,0,sizeof(nodeName));
			strncpy(nodeName, WANINFO_COMMON_NODE, sizeof(nodeName)-1);

			if (cfg_obj_get_object_attr(nodeName, "EthernetState", 0,buf,sizeof(buf)) >= 0){
				if (strcmp(buf, "up") == 0){
					strncpy(diag_value[EthernetLinkStatus], TEST_OK, sizeof(diag_value[EthernetLinkStatus])-1);
				}
			}
		}
	}
#endif	
	if (strcmp(buf, "up") == 0) {
#else

	/*ADSLSyn*/
	memset(buf,0,sizeof(buf));
	blapi_xdsl_get_xdsl_linkstatus(buf, &xdsl_type);
	if((strlen(buf) > 0) && (strstr(buf,"up"))){
		strncpy(diag_value[AdslSyn], TEST_OK, 7);
	}
	else{
		for(i=AtmOAMSeg; i<Max_diag; i++){
			strncpy(diag_value[i],TEST_SKIP, 7);
		}
	}

	if(strcmp(diag_value[AdslSyn], TEST_OK) ==0){
		
		cfg_test_atm_oam_ping(pvcName, ATM_OAM_PING_SEGM, diag_value[AtmOAMSeg]);
		cfg_test_atm_oam_ping(pvcName, ATM_OAM_PING_E2E, diag_value[AtmOAME2E]);
#endif

		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, pvcindex);
		cfg_obj_get_object_attr(nodeName, "ISP", 0, buf, sizeof(buf));
		if(atoi(buf) == 2){
			/*PPPOE/PPPOA Mode*/
			snprintf(dev, sizeof(dev), PPP_IF, atoi(pvc));
		}else{
			snprintf(dev, sizeof(dev), NAS_IF, atoi(pvc));
		}


#if defined(TCSUPPORT_CT_E8GUI)
		memset(buf,0,sizeof(buf));
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), DIAGNOSTIC_PVC_NODE, pvcindex);
		
		if(cfg_obj_get_object_attr(nodeName, "TestType", 0, buf, sizeof(buf)) > 0)
		{
			/*do ping ip test*/
#if defined(TCSUPPORT_CT_PON_GD)
			if((strcmp(buf,"2") == 0) || (strcmp(buf,"1") == 0))
#else
			if(strcmp(buf,"2") == 0)
#endif
			{
#if defined(TCSUPPORT_CT_PON_GD)
				if (strcmp(buf,"1") == 0)
				{
					iFlagBandRegDiag = 1;
				}
				else
				{
					iFlagBandRegDiag = 0;
				}
#endif

				memset(buf,0,sizeof(buf));
				memset(buf_route,0,sizeof(buf_route));
				memset(buf_times,0,sizeof(buf_times));
				strncpy(diag_value[PingOther],TEST_FAIL,7);
				
				if(cfg_obj_get_object_attr(nodeName, "PingOtherType", 0, buf, sizeof(buf)) > 0){
					if(strcmp(buf,"Yes") == 0){
						if(cfg_obj_get_object_attr(nodeName, "PingOtherIPaddr", 0, buf, sizeof(buf)) > 0 && 
						cfg_obj_get_object_attr(nodeName, "PingOtherTimes", 0, buf_times, sizeof(buf_times)) > 0 && 
						cfg_obj_get_object_attr(nodeName, "AddRoute", 0, buf_route, sizeof(buf_route)) > 0)
						{							
							char cmd[64] = {0};
							/*delete old ping file*/
							unlink(TMP_PING_OTHER_PATH);
							if(cfg_checkWan_by_name(buf_route, &ipVersion) == -1)
							{
								flagRet = 1;
								goto save_result;
							}

#if 1
							snprintf(cmd, sizeof(cmd), "/bin/echo \"%s %s\" > %s", buf, buf_route, TMP_DNS_DIAG_PATH);
							system_escape(cmd);
#endif
							if(inet_aton(buf,&cvtaddr.sin_addr))
							{	
								/*ping diagnostic address is ipv4*/
								strncpy(convertIP, buf, sizeof(convertIP)-1);
#if defined(TCSUPPORT_CT_PON_GD)
								cfg_pingDiag(convertIP,buf_route,buf_times, ipVersion, iFlagBandRegDiag);
#else
								cfg_pingDiag(convertIP,buf_route,buf_times, ipVersion);
#endif
							}
							else if( inet_pton(AF_INET6, buf,(struct sockaddr *) &ipv6addr.sin6_addr) >0)
							{	
								/*ping diagnostic address is ipv6*/
								strncpy(convertIP, buf, sizeof(convertIP)-1);
								cfg_ping6Diag(convertIP,buf_route,buf_times);
							}
							else{
								/*ping diagnostic address is a domain name*/
								memset( &hints, 0 , sizeof(hints));
								hints.ai_family = ( 2 == ipVersion ) ? AF_INET6 : AF_INET;
								hints.ai_socktype = SOCK_DGRAM; 		/*DNS is UDP packet*/
								hints.ai_protocol = IPPROTO_UDP;
#if defined(TCSUPPORT_CT_PON_GDV20)
								memset( &hintsv6, 0 , sizeof(hintsv6));
								hintsv6.ai_family = AF_INET6;
								hintsv6.ai_socktype = SOCK_DGRAM;		/*DNS is UDP packet*/
								hintsv6.ai_protocol = IPPROTO_UDP;	
								memset(convertIPv6,0,sizeof(convertIPv6));
#endif

								if ( 1 == ipVersion
									|| 2 == ipVersion ) /*ipv4 only or ipv6 only*/
								{
									if ( tc_getaddrinfo(buf, NULL, &hints, &res, 5) )
									{
										flagRet = 1;
										goto save_result;
									}
								}
								else /* ipv4/ipv6 or dslite+ipv6*/
								{
									hints.ai_family = AF_INET6;
									if ( tc_getaddrinfo(buf, NULL, &hints, &res, 5) )
									{
										hints.ai_family = AF_INET;
										if( tc_getaddrinfo(buf, NULL, &hints, &res, 5) )
										{
											flagRet = 1;
											goto save_result;
										}
									}
#if defined(TCSUPPORT_CT_PON_GDV20)
									else
									{
										if( tc_getaddrinfo(buf, NULL, &hintsv6, &resv6, 5) == 0 )
										{
											ipv6addr2 = (struct sockaddr_in6 *)resv6->ai_addr;
											memset(buf, 0, sizeof(buf));
											inet_ntop(AF_INET6,&ipv6addr2->sin6_addr, buf, INET6_ADDRSTRLEN);
											strncpy(convertIPv6, buf, sizeof(convertIPv6)-1);
										}
									}
#endif
								}

								switch (res->ai_family)
								{
									case AF_INET:			/*ipv4*/
										cvtaddr2 = (struct sockaddr_in *) res->ai_addr;
										strncpy(convertIP, inet_ntoa(cvtaddr2->sin_addr), sizeof(convertIP)-1);
#if defined(TCSUPPORT_CT_PON_GD)
										cfg_pingDiag(convertIP,buf_route,buf_times, ipVersion, iFlagBandRegDiag);
#else
										cfg_pingDiag(convertIP,buf_route,buf_times, ipVersion);
#endif
										break;	
									case AF_INET6:			/*ipv6*/
										ipv6addr2 = (struct sockaddr_in6 *)res->ai_addr;
										inet_ntop(AF_INET6,&ipv6addr2->sin6_addr, buf, INET6_ADDRSTRLEN);
										strncpy(convertIP, buf, sizeof(convertIP)-1);
										cfg_ping6Diag(convertIP,buf_route,buf_times);
										break;
									default:	
										flagRet = 1;
										goto save_result;
										
								}
							}
#if defined(TCSUPPORT_CT_PON_GDV20)
						while(1){
#endif
							
							tmpBuf=malloc(MAX_BUF_SIZE);
							if(tmpBuf == NULL){
								flagRet = 1;
								goto save_result;
							}
							/*judge test result*/
							memset(filebuf,'\0',BUF_SIZE_512);
							FILE *fp = NULL;
							fp=fopen(TMP_PING_OTHER_PATH, "r");
							if(fp == NULL)
							{
								flagRet = 1;
								free(tmpBuf);
								goto save_result;
							}
							while (fgets(filebuf, BUF_SIZE_512, fp))
							{
								if(!strstr(filebuf,RECVICMPPKT))
								{
									continue;
								}
								sscanf(filebuf, "%s %s %s %s %s %s %s %s %s",filler1, filler2, filler3, filler4,filler5,filler6,filler7,
								filler8,filler9);
								if(strcmp(filler4,"0"))
								{
									strncpy(diag_value[PingOther],TEST_OK,7);
								}
								
							}
							fclose(fp);
								
							free(tmpBuf);
#if defined(TCSUPPORT_CT_PON_GDV20)
							if(strcmp(filler4,"0"))
							{
								break;
							}
							else if(strlen(convertIPv6))
							{
								cfg_ping6Diag(convertIPv6,buf_route,buf_times);
								memset(convertIPv6,0,sizeof(convertIPv6));
							}
							else
								break;
						}
#endif
						}
					}
					else{
						memset(diag_value[PingOther], 0, sizeof(diag_value[PingOther]));
						strncpy(diag_value[PingOther],TEST_SKIP,sizeof(diag_value[PingOther]) - 1);
					}
				}
			}
		}

		memset(buf,0,sizeof(buf));
		memset(buf_route,0,sizeof(buf_route));
		if(cfg_obj_get_object_attr(nodeName, "TestType", 0, buf, sizeof(buf)) > 0)
		{
			/*do tracert ip test*/
			if(strcmp(buf,"3") == 0)
			{	
				memset(buf,0,sizeof(buf));
				strncpy(diag_value[TraceResult],TEST_FAIL,7);
				
				if(cfg_obj_get_object_attr(nodeName, "TraceIPaddr", 0, buf, sizeof(buf)) > 0 &&
				   cfg_obj_get_object_attr(nodeName, "AddRoute", 0, buf_route, sizeof(buf_route)) > 0 )
				{
					/*delete old traceroute file*/
					unlink(TMP_TRACE_PATH);
					if(cfg_checkWan_by_name(buf_route, &ipVersion) == -1)
					{
						flagRet = 1;
						goto save_result;
					}

					snprintf(cmdbuf, sizeof(cmdbuf), "/bin/echo \"%s %s\" > %s", buf, buf_route, TMP_DNS_DIAG_PATH);
					system_escape(cmdbuf);

					if(inet_aton(buf,&cvtaddr.sin_addr))
					{	
						/*traceroute diagnostic address is ipv4*/
						strncpy(convertIP, buf, sizeof(convertIP)-1);
						cfg_tracerouteDiag(convertIP,buf_route, ipVersion);
					}
					else if( inet_pton(AF_INET6, buf,(struct sockaddr *) &ipv6addr.sin6_addr) >0)
					{	
						/*traceroute diagnostic address is ipv6*/
						strncpy(convertIP, buf, sizeof(convertIP)-1);
						cfg_traceroute6Diag(convertIP,buf_route);
					}
					else{
						/*ping diagnostic address is a domain name*/
						memset( &hints, 0 , sizeof(hints));
						hints.ai_family = ( 2 == ipVersion ) ? AF_INET6 : AF_INET;
						hints.ai_socktype = SOCK_DGRAM; 		/*DNS is UDP packet*/
						hints.ai_protocol = IPPROTO_UDP;

						if ( 1 == ipVersion
							|| 2 == ipVersion ) /* ipv4 only or ipv6 only*/
						{
							if ( tc_getaddrinfo(buf, NULL, &hints, &res, 5) )
							{
								flagRet = 1;
								goto save_result;
							}
						}
						else /* ipv4/ipv6 or dslite+ipv6*/
						{
							hints.ai_family = AF_INET6;
							if ( tc_getaddrinfo(buf, NULL, &hints, &res, 5) )
							{
								hints.ai_family = AF_INET;
								if( tc_getaddrinfo(buf, NULL, &hints, &res, 5) )
								{
									flagRet = 1;
									goto save_result;
								}
							}
						}

						switch (res->ai_family)
						{
							case AF_INET:			/*ipv4*/
								cvtaddr2 = (struct sockaddr_in *) res->ai_addr;
								strncpy(convertIP, inet_ntoa(cvtaddr2->sin_addr), sizeof(convertIP)-1);
								cfg_tracerouteDiag(convertIP,buf_route, ipVersion);
								break;	
							case AF_INET6:			/*ipv6*/
								ipv6addr2 = (struct sockaddr_in6 *)res->ai_addr;
								inet_ntop(AF_INET6,&ipv6addr2->sin6_addr, buf, INET6_ADDRSTRLEN);
								strncpy(convertIP, buf, sizeof(convertIP)-1);
								cfg_traceroute6Diag(convertIP,buf_route);
								break;
							default:	
								flagRet = 1;
								goto save_result;
								
						}
					}
					tmpBuf=malloc(MAX_BUF_SIZE);
					if(tmpBuf == NULL){
						flagRet = 1;
						goto save_result;
					}
					/*judge test result*/
					memset(tmpBuf,'\0',MAX_BUF_SIZE);
					fileRead(TMP_TRACE_PATH, tmpBuf, MAX_BUF_SIZE);
					
					if(strcmp(tmpBuf,"")){
						strncpy(diag_value[TraceResult],TEST_OK,7);
					}
					free(tmpBuf);
				}
			}
		}
#else
		/*rodney_20090506 modified*/
		/*ping DHCP Primary DNS*/
		memset(buf,0,sizeof(buf));
		getWanInfo(Pridns_type, dev, buf);
		snprintf(pingCMD, sizeof(pingCMD), PING_CMD, buf, TMP_PING_DNS_PATH);
		system(pingCMD);
		tmpBuf=malloc(MAX_BUF_SIZE);
		if(tmpBuf == NULL){
			flagRet = 1;
			goto save_result;
		}
		fileRead(TMP_PING_DNS_PATH, tmpBuf, MAX_BUF_SIZE);
		if(strstr(tmpBuf, RECV1ICMPPKT)){
			strncpy(diag_value[PingPriDNS],TEST_OK,7);
		}
		unlink(TMP_PING_DNS_PATH);

		snprintf(pingCMD, sizeof(pingCMD), PING_CMD, YAHOO_DNS, TMP_PING_YAH_PATH);  /*ping yahoo*/
		system(pingCMD);
		fileRead(TMP_PING_YAH_PATH, tmpBuf, MAX_BUF_SIZE);
		if(strstr(tmpBuf, RECV1ICMPPKT)){
			strncpy(diag_value[PingYahoo],TEST_OK,7);
		}
		unlink(TMP_PING_YAH_PATH);
		free(tmpBuf);

		/*do ping other ip test*/
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), DIAGNOSTIC_PVC_NODE, pvcindex);
		memset(buf,0,sizeof(buf));
		strncpy(diag_value[PingOther],TEST_FAIL,7);
		if(cfg_obj_get_object_attr(nodeName, "PingOtherType", 0, buf, sizeof(buf)) > 0){
			if(strcmp(buf,"Yes") == 0){  /*user want to ping other IP*/
				if(cfg_obj_get_object_attr(nodeName, "PingOtherIPaddr", 0, buf, sizeof(buf)) > 0){
					snprintf(pingCMD, sizeof(pingCMD), PING_CMD, buf, TMP_PING_OTHER_PATH);
					system_escape(pingCMD);
					tmpBuf=malloc(MAX_BUF_SIZE);
					if(tmpBuf == NULL){
						flagRet = 1;
						goto save_result;
					}
					fileRead(TMP_PING_OTHER_PATH, tmpBuf, MAX_BUF_SIZE);
					if(strstr(tmpBuf, RECV1ICMPPKT)){
						strncpy(diag_value[PingOther],TEST_OK,7);
					}
					free(tmpBuf);
					unlink(TMP_PING_OTHER_PATH);
				}
			}
			else{
				strncpy(diag_value[PingOther],TEST_SKIP,7);
			}
		}
#endif
	}
save_result:
	/*unlink the tmp diagdns.conf file created*/
	unlink(TMP_DNS_DIAG_PATH);
	
	/*Fill the test result	into diagnostic */
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), DIAGNOSTIC_PVC_NODE, pvcindex);
	for(i=0;strlen(diagnostic_attr[i]) != 0;i++){
		cfg_set_object_attr(nodeName, diagnostic_attr[i], diag_value[i]);
	}
	if(res)
		freeaddrinfo(res);
	if(flagRet == 1)
		return -1;
	return 0;
}

static int cfg_type_diagnostic_func_commit(char *path)
{
	cfg_type_diagnostic_func_execute(path);
	return 0;
}

#ifdef TCSUPPORT_CT_UBUS
static int cfg_type_diagnostic_hw_func_execute(char *path)
{
	int fd = 0;
	int i = 0, res = 0, len = 0;
	char stream[64] = {0};
	char cmd_buf[64] = {0};

	unlink(IFA_STATUS);
	for ( i = 0; i < 12; i++ )
	{	
		if ( i < 4 )
		{
			/* LAN interface */
			snprintf(cmd_buf, sizeof(cmd_buf), ETHIFA_STATUS_CMD, i+1);
		}
		else if ( i < 8 )
		{
			/* WiFi2.4G interface */
			snprintf(cmd_buf, sizeof(cmd_buf), WLANIFA_STATUS_CMD, i-4);
		}
		else
		{
			/* WiFi5G interface */
			snprintf(cmd_buf, sizeof(cmd_buf), WLANACIFA_STATUS_CMD, i-8);
		}
		system(cmd_buf);
		
		memset(stream, 0, sizeof(stream));
		fd = open(IFA_STATUS_TMP, O_RDONLY);
		if ( fd == -1 )
		{
			snprintf(cmd_buf, sizeof(cmd_buf), IFA_STATUS_CMD, i+1, "Off");
			system(cmd_buf);
			continue;
		}

		len = read(fd, stream, sizeof(stream)-1);
		stream[sizeof(stream)-1] = '\0';
		close(fd);
		res = remove("/tmp/interface_status_tmp");
		if ( len == 0 || len == -1 )
		{	
			snprintf(cmd_buf, sizeof(cmd_buf), IFA_STATUS_CMD, i+1, "Off");
			system(cmd_buf);
			continue;
		}

		if ( NULL != strstr(stream, "HWaddr") )
		{
			snprintf(cmd_buf, sizeof(cmd_buf), IFA_STATUS_CMD, i+1, "On");	
		}
		else
		{
			snprintf(cmd_buf, sizeof(cmd_buf), IFA_STATUS_CMD, i+1, "Off");
		}
		system(cmd_buf);
	}
	
	/* USB interface */
	if ( access(USBIFA_PATH, 0) == 0 )
	{
		snprintf(cmd_buf, sizeof(cmd_buf), IFA_STATUS_CMD, i+1, "On");
	}
	else
	{
		snprintf(cmd_buf, sizeof(cmd_buf), IFA_STATUS_CMD, i+1, "Off");
	}
	system(cmd_buf);
	
	unlink(IFA_STATUS_TMP);
	return 0;
}

static int cfg_type_diagnostic_hw_func_commit(char *path)
{
	cfg_type_diagnostic_hw_func_execute(path);
	return 0;
}
#endif

static cfg_node_ops_t cfg_type_diagnostic_pvc_ops  = { 
	 .get = cfg_type_diagnostic_pvc_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_diagnostic_func_commit 
}; 

#ifdef TCSUPPORT_CT_UBUS
static cfg_node_ops_t cfg_type_diagnostic_hw_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_diagnostic_hw_func_commit 
}; 
#endif

static cfg_node_type_t cfg_type_diagnostic_pvc = { 
	 .name = "PVC", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 1, 
	 .parent = &cfg_type_diagnostic, 
	 .index = cfg_type_diagnostic_index, 
	 .ops = &cfg_type_diagnostic_pvc_ops, 
}; 

#ifdef TCSUPPORT_CT_UBUS
static cfg_node_type_t cfg_type_diagnostic_hw = { 
	 .name = "HW", 
	 .flag = 1, 
	 .parent = &cfg_type_diagnostic, 
	 .ops = &cfg_type_diagnostic_hw_ops, 
}; 
#endif

static cfg_node_ops_t cfg_type_diagnostic_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_diagnostic_func_commit 
}; 


static cfg_node_type_t* cfg_type_diagnostic_child[] = { 
	 &cfg_type_diagnostic_pvc, 
#ifdef TCSUPPORT_CT_UBUS
	 &cfg_type_diagnostic_hw, 
#endif
	 NULL 
}; 


cfg_node_type_t cfg_type_diagnostic = { 
	 .name = "Diagnostic", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_diagnostic_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_diagnostic_child, 
	 .ops = &cfg_type_diagnostic_ops, 
}; 
