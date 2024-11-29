#ifndef _GNU_SOURCE
#define _GNU_SOURCE  /* for declare getline, fix warning error */
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <cfg_cli.h>

#include "wan_related_maintenance.h"
#include "cfg_msg.h"
#include "utility.h"

int cfg_type_pppoesimulate_write(char *path){

	char fpath[64];
	cfg_node_type_t *node = NULL;

	/*create pppoe_sim.conf*/
	snprintf(fpath, sizeof(fpath), "%s", PPPOE_SIMULATE_CONF);
	node = cfg_type_query(path);
	if(node != NULL){
		if(cfg_save_attrs_to_file(path, fpath, QMARKS) != 0){
			tcdbg_printf("\npppoe_simulate_write write PPPoESimulate_Entry error!");
			return -1;
		}
	}
	return 0;
}

void pppoe_simulate_stop(char *nodeName, int reason){
	FILE *fp = NULL;
	int pid = 0;
	char cmdbuf[128] = {0};
	char nas_name[16] = {0};
	char *line = NULL;
	size_t len = 0;
	
	fp = fopen(PPPOE_SIMULATE_PID_PATH, "r");
	if(fp)
	{
		/*kill the ping process */
		if ( -1 != getline(&line, &len, fp) )
		{
			if ( line )
				pid = atoi(line);
		}

		if ( line )
			free(line);

		if(pid != 0)
		{ 
			memset(cmdbuf,0,sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "kill -9 %d &", pid);
		   	system(cmdbuf);
		}
		
		/*delete the pid file*/
		fclose(fp);
		unlink(PPPOE_SIMULATE_PID_PATH);
	}
	
	cfg_get_object_attr(nodeName, "NASName", nas_name, sizeof(nas_name));
	if(nas_name[0] != '\0'){
		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig %s down", nas_name);
		system(cmdbuf);

		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/smuxctl rem %s", nas_name);
		system(cmdbuf);
	}else{
		tcdbg_printf("\npppoe_simulate_stop, get VLANID or NASName error!!!");
	}
	if(reason == PPPOE_SIMULATE_COMPLETE){
		return;
	}
#if !defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
	cfg_set_object_attr(nodeName,"PPPSessionID","");
	cfg_set_object_attr(nodeName,"ExternalIPAddress","");
	cfg_set_object_attr(nodeName,"DefaultGateway","");
#endif
	if(reason == PPPOE_SIMULATE_USER_STOP){
		cfg_set_object_attr(nodeName,"DiagnosticsState","None");
		cfg_set_object_attr(nodeName,"Result","UserStop");
	}
	if(reason == PPPOE_SIMULATE_UNKNOW_STOP){
		cfg_set_object_attr(nodeName,"Result","unknown");
	}
}

int svc_wan_related_pppoesimulate()
{
	char nodeName[64] = {0};
	char diag_state[16] = {0};
	snprintf(nodeName,sizeof(nodeName),PPPOE_SIMULATE_ENTRY_NODE);
	if(cfg_type_pppoesimulate_write(nodeName) < 0)
	{
		return -1;
	}
	cfg_get_object_attr(nodeName, "DiagnosticsState", diag_state,sizeof(diag_state));
	if(diag_state[0] != '\0'){
		if(0 == strcasecmp(diag_state,"Start")){
			tcdbg_printf("\nPPPoE Simulate Start");
			pppoe_simulate_stop(nodeName, PPPOE_SIMULATE_UNKNOW_STOP);
			cfg_set_object_attr(nodeName,"DiagnosticsState","Running");
			system(PPPOE_SIMULATE_SH);
			return 0;
		}
		else if(0 == strcasecmp(diag_state,"Stop")){
			tcdbg_printf("\nPPPoE Simulate Stop");
			pppoe_simulate_stop(nodeName, PPPOE_SIMULATE_USER_STOP);
			return 0;
		}else if(0 == strcasecmp(diag_state,"Complete")){
			tcdbg_printf("\nPPPoE Simulate Complete");
			pppoe_simulate_stop(nodeName, PPPOE_SIMULATE_COMPLETE);
			return 0;
		}
		else{
			tcdbg_printf("\nPPPoE Simulate DiagnosticsState Other State");
			return 0;
		}
	}
	else{
		tcdbg_printf("\nPPPoE Simulate get DiagnosticsState Fail!");
		return -1;
	}
}

int
cuc_ping_test_get_wanif( char* ifName){
	char nodeName[64] = {0};
	char str_valid_if[128] = {0};
	char* p=NULL;
	int i, pvc_index, entry_index;	
	char value[64] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName),WANINFO_COMMON_NODE);
	cfg_get_object_attr(nodeName, "ValidIFIndex", str_valid_if, sizeof(str_valid_if));
	if(!strlen(str_valid_if)){
		printf("\ncuc_ping_test_get_wanif, get WanInfo ValidIFIndex fail!!");
		return 0;
	}
	
	p=strtok(str_valid_if, ",");
	while(p)
	{
		i = atoi(p);
		pvc_index = i / 8;
		entry_index = i % 8;	

		pvc_index++;
		entry_index++;
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName,sizeof(nodeName),WAN_PVC_ENTRY_NODE,pvc_index,entry_index);
		cfg_get_object_attr(nodeName, "ServiceList", value, sizeof(value));
		if(strlen(value) && (strstr(value, "VOICE"))){
			cfg_get_object_attr(nodeName,"IFName",ifName,IFNAME_LEN);
			printf("ifname=%s\n ",ifName);
			if( strlen(ifName) && strcmp(ifName, "")){
				//check if wan is active and have ip
				memset(value, 0, sizeof(value));
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName,sizeof(nodeName),WANINFO_ENTRY_NODE,i+1);
				cfg_get_object_attr(nodeName,"GateWay",value,sizeof(value));	
				if((strlen(value) != 0) && (strcmp(value,"N/A"))){
					printf("\ncuc_ping_test_get_wanif, find wan interface");
					return 1;
				}
				else{
					printf("\ncuc_ping_test_get_wanif, checkWan_by_name fail!!");
				}
			}
			else{
				printf("\ncuc_ping_test_get_wanif, get IFName fail");
			}
		}
		p=strtok(NULL, ",");
	}
	printf("\ncuc_ping_test_get_wanif, not found VOICE Wan");
	return 0;	
}

void cuc_ping_test_reset(char *nodeName){
	cfg_set_object_attr(nodeName, "Result", CUC_PING_TEST_RESULT_NOT_START);
	cfg_set_object_attr(nodeName, "TXPkt", "0");
	cfg_set_object_attr(nodeName, "RxPkt", "0");
	cfg_set_object_attr(nodeName, "LostPkt", "0");
	cfg_set_object_attr(nodeName, "LostPktRatio", "0");
	cfg_set_object_attr(nodeName, "MinDelay", "0");
	cfg_set_object_attr(nodeName, "MaxDelay", "0");
	cfg_set_object_attr(nodeName, "AvgDelay", "0");
}

void cuc_ping_test_stop(char *nodeName, int reason){
	char destIP[32] = {0};
	FILE *fp= NULL;
	char tmp[64] = {0};
	int pid = 0;
	struct sockaddr_in ipAddr;
	char *line = NULL;
	size_t len = 0;

	/*kill ping */
	fp = fopen(CUC_PING_TEST_PID_PATH, "r");
	if(fp){
		/*kill the ping process*/
		if ( -1 != getline(&line, &len, fp) )
		{
			if ( line )
				pid = atoi(line);
		}

		if ( line )
			free(line);

		if(pid != 0){ 
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp,"kill -9 %d", pid);
		   	system(tmp);
		}
		
		/*delete the pid file*/
		fclose(fp);
		unlink(CUC_PING_TEST_PID_PATH);
	}
	
	/*delete route*/
	cfg_get_object_attr(nodeName, "DestIP", destIP, sizeof(destIP));
	if(strcmp(destIP,"")){
		if(inet_aton(destIP,&ipAddr.sin_addr)){
			memset(tmp,0,sizeof(tmp));
			snprintf(tmp,sizeof(tmp),CUC_PING_TEST_DELROUTE_CMD, destIP);
			/*tcdbg_printf("\ncuc_ping_test_stop, CMD = %s", tmp);*/
			system(tmp);
		}
	}
	else{
		printf("\ncuc_ping_test_stop, get DestIP fail");
	}
	if(reason == 2){
		return;
	}
	cuc_ping_test_reset(nodeName);
}
int svc_wan_related_cucping()
{
	char nodeName[64] = {0};
	char pingAction[64] = {0};
	char count[16] = {0}, datalen[16]={0}, timeout[16] = {0}, destIP[32] = {0};
	struct sockaddr_in ipAddr;
	char ifName[IFNAME_LEN] = {0};
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName,sizeof(nodeName),CUCPING_ENTRY_NODE);
	cfg_get_object_attr(nodeName, "PingAction", pingAction, sizeof(pingAction));
	if(!strlen(pingAction))
	{
		printf("\nPing test, get PingAction value faill!");
		return -1;
	}
	if(0 == strcmp(pingAction,"1")){
		/*printf("\nPing test start");*/
		cuc_ping_test_stop(nodeName,1);
		cfg_set_object_attr(nodeName, "Result", CUC_PING_TEST_RESULT_RUNNING);

		cfg_get_object_attr(nodeName, "PingTimes", count, sizeof(count));
		cfg_get_object_attr(nodeName, "PingSize", datalen, sizeof(datalen));
		cfg_get_object_attr(nodeName, "PingTimeout", timeout, sizeof(timeout));
		cfg_get_object_attr(nodeName, "DestIP", destIP, sizeof(destIP));
		
		if(strlen(count) && strlen(datalen) && strlen(timeout) && strlen(destIP)){
			if(!strcmp(destIP,"") ||! inet_aton(destIP,&ipAddr.sin_addr)){
				printf("\nPing test, destIP format error, destIP = %s", destIP);
				cfg_set_object_attr(nodeName, "Result", CUC_PING_TEST_RESULT_UNKNOWN_IP);
				return 0;
			}
			if(cuc_ping_test_get_wanif(ifName)){/* get VOIP interface*/
				/*route add*/
				memset(pingAction,0,sizeof(pingAction));
				snprintf(pingAction,sizeof(pingAction),CUC_PING_TEST_ADDROUTE_CMD, destIP, ifName);
				/*tcdbg_printf("\nPing test CMD1 = %s", tmp);*/
				system(pingAction);
				/*ping cmd*/
				memset(pingAction,0,sizeof(pingAction));
				snprintf(pingAction,sizeof(pingAction),CUC_PING_TEST_CMD, count, datalen, timeout, destIP);
				/*tcdbg_printf("\nPing test CMD2 = %s", tmp);*/
				system(pingAction);
				return 0;
			}
			else{
				printf("\nPing test, not found VOIP interface!!!");
				cfg_set_object_attr(nodeName,"Result",CUC_PING_TEST_RESULT_NO_ROUTE);
				return 0;
			}
		}
		else{
			printf("\nPing test, get Attribute  value fail");
			return -1;
		}
	}
	else if(0 == strcmp(pingAction,"0")){
		/*tcdbg_printf("\nPing test stop");*/
		cuc_ping_test_stop(nodeName,0);
		return 0;
	}
	else if(0 == strcmp(pingAction,CUC_PING_TEST_ACTION_COMPLETE)){
		tcdbg_printf("\nPing test complete");
		cuc_ping_test_stop(nodeName,2);
		return 0;
	}
	else{
		printf("\nPing test, PingAction not 0,1,2");
		return 0;
	}
}


