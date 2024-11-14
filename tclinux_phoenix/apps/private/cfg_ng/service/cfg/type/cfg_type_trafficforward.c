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
#include <svchost_evt.h>
#include "cfg_types.h" 
#include "modules/traffic_classify_global/traffic_classify_global.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "utility.h"
#include "cfg_type_transferservices.h"
#include <sys/utsname.h>
#include "traffic/global_dnshost.h"

int tf_ipv6_nat_rules_num = 0;
int tf_ipv6_filter_rules_num = 0;
int have_insmod_ip6t_nat = 0;
int have_insmod_ip6t_filter = 0;

/* kernel revision handling */
char kernel_version[32] = "3.18.21";
static void get_kernel_version(void) {
	struct utsname uts;

	if (uname(&uts) == -1) {
		fprintf(stderr, "Unable to retrieve kernel version.\n");
		return;
	}
	
	snprintf(kernel_version, sizeof(kernel_version), "%s", uts.release);
}

static unsigned int getFileSize(FILE *fp)
{
	unsigned int size;
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

static int getTFDbgLevel(void){
	FILE* fp = NULL;
	
	fp = fopen("/tmp/trafficforward_dbg","r");
	if(fp == NULL){
		return 0;
	}
	fclose(fp); 
	return 1;
}

static int cfg_type_trafficforward_func_write(void){

	char nodeName[64]={0};
	char remoteAddr[256] = {0}, op_remoteAddr[256] = {0}, remotePort[24] = {0};
	char op_remotePort[64] = {0};
	char forwardtoip[TRAFFIC_IP_ADDR_STR_MAX_LEN] = {0};
	char forwardtoport[16] = {0}, hostmac[32] = {0}, op_hostmac[40] = {0};
	char cmdbuf[512] = {0}, finalcmdbuf[600] = {0}, prot[12] = {0}, targetBuf[64] = {0}, targetTab[64] = {0};
	int idx = 0;
	struct in_addr in_s_v4addr = {0};
	FILE *fp_all_addr = NULL, *fp_rmt_domain = NULL, *fp_dst_domain = NULL, *fp_all_domain = NULL;
	int port_s = 0, port_e = 0, forward_port = 0;
	e_match_port_type port_match_type = E_MATCH_PORT_IGNORE;
	int ipv6_mode = 0;
	int forwardToIP_domain_mode = 0, remoteAddr_domain_mode = 0;
	struct in6_addr in_s_v6addr = {0};

	fp_all_addr = fopen(TF_ALL_ADDR_PATH, "w");
	fp_rmt_domain = fopen(TF_RMT_DOMAIN_PATH, "w");
	fp_dst_domain = fopen(TF_DST_DOMAIN_PATH, "w");
	fp_all_domain = fopen(TF_ALL_DOMAIN_PATH, "w");
	
	if ( !fp_all_addr || !fp_rmt_domain || !fp_dst_domain  || !fp_all_domain)
		goto tf_write_err;

	tf_ipv6_nat_rules_num = 0;
	//sendTodnshost( BIT_FORWARD, DEL_ACT, NULL);	
	bzero(nodeName, sizeof(nodeName));
	sendTodnshost(BIT_FORWARD, DEL_ACT, NULL);
	for (idx = 1; idx <= MAX_FLOWNUM_RULE; idx++ )
	{
		snprintf(nodeName, sizeof(nodeName), TRAFFICFORWARD_ENTRY_NODE, idx);
		bzero(targetBuf, sizeof(targetBuf));
		bzero(op_remoteAddr, sizeof(op_remoteAddr));
		bzero(remoteAddr, sizeof(remoteAddr));
		bzero(prot, sizeof(prot));
		bzero(remotePort, sizeof(remotePort));
		bzero(hostmac, sizeof(hostmac));
		bzero(op_remotePort, sizeof(op_remotePort));
		bzero(op_hostmac, sizeof(op_hostmac));
		ipv6_mode = 0;
		forwardToIP_domain_mode = 0;
		remoteAddr_domain_mode = 0;
		
		if(( 0 > cfg_obj_get_object_attr(nodeName, "forwardToIP", 0, forwardtoip, sizeof(forwardtoip)))
			|| 0 == forwardtoip[0])
			continue;
		
		if ( 1 != inet_pton(AF_INET, forwardtoip, &in_s_v4addr) )
		{
			if( 1 != inet_pton(AF_INET6, forwardtoip, &in_s_v6addr))
			{
				/* domain name */
				forwardToIP_domain_mode = 1;//continue;
				//sendTodnshost( BIT_FORWARD, ADD_ACT, forwardtoip);
			}else
				ipv6_mode = 1;
		}
		
		cfg_obj_get_object_attr(nodeName, "remoteAddr", 0, remoteAddr, sizeof(remoteAddr));
		if ( 0 != remoteAddr[0] )
		{
			sendTodnshost(BIT_FORWARD, ADD_ACT, remoteAddr);
			/* not IP range mode. */
			if ( NULL == strstr(remoteAddr, "/") )
			{
				if ( 1 != inet_pton(AF_INET, remoteAddr, &in_s_v4addr) )
				{
					if ( 1 != inet_pton(AF_INET6, remoteAddr, &in_s_v6addr) )
					{
						/* domain */
						remoteAddr_domain_mode = 1;
						//sendTodnshost( BIT_FORWARD, ADD_ACT, remoteAddr);
					}
					else
						ipv6_mode = 1;
				}
			}
			else
			{
				char remoteAddrTmp[256] = {0};
				sscanf(remoteAddr,"%s/", remoteAddrTmp);
				if (1 != inet_pton(AF_INET, remoteAddrTmp, &in_s_v4addr))
				{
					if (1 != inet_pton(AF_INET6, remoteAddrTmp, &in_s_v6addr))
					{
						continue;
					}
					ipv6_mode = 1;
				}
			}
			snprintf(op_remoteAddr, sizeof(op_remoteAddr), "-d %s", remoteAddr);
		}

		if ( 0 > cfg_obj_get_object_attr(nodeName, "protocol", 0, prot, sizeof(prot))
			|| 0 == prot[0] )
			continue; /* rule invalid if no Protocol. */

		cfg_obj_get_object_attr(nodeName, "remotePort", 0, remotePort, sizeof(remotePort));
		if ( 0 != remotePort[0] )
		{
			if ( NULL != strstr(remotePort, "-") ) /* PORT range */
			{
				sscanf(remotePort, "%d-%d", &port_s, &port_e);
				port_match_type = E_MATCH_PORT_RANGE_TYPE;
			}
			else if ( NULL != strstr(remotePort, "!") ) /* PORT NOR */
			{
				sscanf(remotePort, "%*[!]%d", &port_s);
				port_match_type = E_MATCH_EXCLUDE_PORT_TYPE;
			}
			else /* single PORT */
			{
				sscanf(remotePort, "%d", &port_s);
				if ( 0 == port_s )
					port_match_type = E_MATCH_PORT_ALL_TYPE;
				else
					port_match_type = E_MATCH_PORT_ONLY_TYPE;
			}
		}
		else
			continue;/* rule invalid if no remote port. */

		cfg_obj_get_object_attr(nodeName, "hostMAC", 0, hostmac, sizeof(hostmac));
		if ( 0 != hostmac[0] && 0 != strcmp(hostmac, "0") )
			snprintf(op_hostmac, sizeof(op_hostmac)
				, "-m mac --mac-source %s", hostmac);

		bzero(op_remotePort, sizeof(op_remotePort));
		if ( E_MATCH_PORT_ONLY_TYPE == port_match_type)
			snprintf(op_remotePort, sizeof(op_remotePort)
				, "--dport %d", port_s);
		else if ( E_MATCH_PORT_RANGE_TYPE == port_match_type)
			snprintf(op_remotePort, sizeof(op_remotePort)
				, "-m multiport --dport %d:%d", port_s, port_e);
		else if ( E_MATCH_EXCLUDE_PORT_TYPE == port_match_type)
			snprintf(op_remotePort, sizeof(op_remotePort)
				, "! --dport %d", port_s);

		if(( 0 > cfg_obj_get_object_attr(nodeName, "forwardToPort", 0, forwardtoport, sizeof(forwardtoport)) )
			|| 0 == forwardtoport[0] )
			forwardtoport[0] = 0;
		
		if ( 0 == forwardtoport[0] )
			continue; /* forward port cannot be empty. */
		forward_port = atoi(forwardtoport);
		if ( 0 != forward_port )
		{
			/* forward packets mode. */
			snprintf(targetTab, sizeof(targetTab), "%s", "-t nat -A "TF_REDIRECT_CHAIN);
			snprintf(targetBuf, sizeof(targetBuf), "-j DNAT --to");
		}
		else
		{
			/* drop packets mode. */
			snprintf(targetTab, sizeof(targetTab), "%s", "-t filter -A "TF_BLOCK_CHAIN);
			snprintf(targetBuf, sizeof(targetBuf), "%s", "-j DROP");
		}

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf),
				"%s "	/* target table */
				"-p %s " /* protocol */
				"%s " /* remoteAddr */
				"%s " /* remotePort */
				"%s " /* mac-source */
				"%s" /* target */
				, targetTab
				, prot
				, (0 == op_remoteAddr[0] ? "" : op_remoteAddr)
				, (0 == op_remotePort[0] ? "" : op_remotePort)
				, (0 == op_hostmac[0] ? "" : op_hostmac)
				, targetBuf);

		if(getTFDbgLevel())
			tcdbg_printf("writeBuf: %s\n", cmdbuf);

		if (0 == forward_port){/* drop packets mode. */
			bzero(finalcmdbuf, sizeof(finalcmdbuf));
			if(ipv6_mode)
			{
				snprintf(finalcmdbuf, sizeof(finalcmdbuf),
					"ip6tables %s\n", cmdbuf);
			}
			else
			{
				snprintf(finalcmdbuf, sizeof(finalcmdbuf),
					"iptables %s\n", cmdbuf);
			}
			if(remoteAddr_domain_mode){
				fputs(finalcmdbuf, fp_rmt_domain);
			}
			else{
				fputs(finalcmdbuf, fp_all_addr);
			}
		}
		else /* forward packets mode. */
		{
			if(remoteAddr_domain_mode && forwardToIP_domain_mode)
			{
				bzero(finalcmdbuf, sizeof(finalcmdbuf));
				/* write iptables rules for IPv4 & IPv6 */
				snprintf(finalcmdbuf, sizeof(finalcmdbuf),
					"%s %s:%d\n", cmdbuf, forwardtoip, forward_port);
				
				fputs(finalcmdbuf, fp_all_domain);
				
				tf_ipv6_nat_rules_num++;
			}
			else
			{
				bzero(finalcmdbuf, sizeof(finalcmdbuf));
				if(ipv6_mode)
				{
					tf_ipv6_nat_rules_num++;
					if(forwardToIP_domain_mode){
						snprintf(finalcmdbuf, sizeof(finalcmdbuf),
							"ip6tables %s %s:%d\n", cmdbuf, forwardtoip, forward_port);
					}
					else{/* address mode */
						snprintf(finalcmdbuf, sizeof(finalcmdbuf),
							"ip6tables %s [%s]:%d\n", cmdbuf, forwardtoip, forward_port);
					}
				}
				else
				{
					snprintf(finalcmdbuf, sizeof(finalcmdbuf),
						"iptables %s %s:%d\n", cmdbuf, forwardtoip, forward_port);
				}
				
				if(remoteAddr_domain_mode)
				{
					fputs(finalcmdbuf, fp_rmt_domain);
				}
				else if(forwardToIP_domain_mode)
				{
					fputs(finalcmdbuf, fp_dst_domain);
				}
				else
				{
					fputs(finalcmdbuf, fp_all_addr);
				}
				
			}
		}
		if(getTFDbgLevel())
			tcdbg_printf("finalWrite:%s\n", cmdbuf);
	}

	cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_all_domain_file_update", "1");
	cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_rmt_domain_file_update", "1");
	cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_dst_domain_file_update", "1");

	if ( fp_all_addr )
		fclose(fp_all_addr);
	if ( fp_rmt_domain )
		fclose(fp_rmt_domain);
	if ( fp_dst_domain )
		fclose(fp_dst_domain);
	if ( fp_all_domain )
		fclose(fp_all_domain);

	return SUCCESS;

tf_write_err:
	if ( fp_all_addr )
		fclose(fp_all_addr);
	if ( fp_rmt_domain )
		fclose(fp_rmt_domain);
	if ( fp_dst_domain )
		fclose(fp_dst_domain);
	if ( fp_all_domain )
		fclose(fp_all_domain);

	return FAIL;
}

int svc_cfg_execute_trafficforward_internal(void)
{
	char cmdbuf[128] = {0};
	char have_insmod_ip6t_nat[4] = {0};
	char have_insmod_ip6t_filter[4] = {0};
	
	get_kernel_version();
	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "have_insmod_ip6t_nat", 0, have_insmod_ip6t_nat, sizeof(have_insmod_ip6t_nat));
	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "have_insmod_ip6t_filter", 0, have_insmod_ip6t_filter, sizeof(have_insmod_ip6t_filter));

	if(tf_ipv6_nat_rules_num && (!(atoi(have_insmod_ip6t_nat))))
	{
		snprintf(cmdbuf, sizeof(cmdbuf)
			, INSMOD_IP6T_MODULES, kernel_version, IP6T_NAT_KO);
		system(cmdbuf);
		if(getTFDbgLevel())
			tcdbg_printf("[%s]:system(%s)\n", __FUNCTION__, cmdbuf);
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "have_insmod_ip6t_nat", "1");
	}
		
	if(tf_ipv6_filter_rules_num && (!(atoi(have_insmod_ip6t_filter))))
	{
		snprintf(cmdbuf, sizeof(cmdbuf)
			, INSMOD_IP6T_MODULES, kernel_version, IP6T_FILTER_KO);
		system(cmdbuf);
		if(getTFDbgLevel())
			tcdbg_printf("[%s]:system(%s)\n", __FUNCTION__, cmdbuf);
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "have_insmod_ip6t_filter", "1");
	}

	if(!tf_ipv6_nat_rules_num && atoi(have_insmod_ip6t_nat))
	{
		system("ip6tables -t nat -F");
		system("ip6tables -t nat -X "TF_REDIRECT_CHAIN);

		snprintf(cmdbuf, sizeof(cmdbuf)
			, RMMOD_IP6T_MODULES, kernel_version, IP6T_NAT_KO);
		system(cmdbuf);
		if(getTFDbgLevel())
			tcdbg_printf("[%s]:system(%s)\n", __FUNCTION__, cmdbuf);
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "have_insmod_ip6t_nat", "0");
	}
		
	#if 0//No need to rmmod ip6tables filter modules, for port filtering
	if(!traffic_forward_ipv6_filter_rules_num && atoi(have_insmod_ip6t_filter))
	{
		system("ip6tables -t filter -F");
		system("ip6tables -t filter -X "TRAFFIC_FORWARD_BLOCK_CHAIN);

		snprintf(cmdbuf, sizeof(cmdbuf)
			, RMMOD_IP6T_NETFILTER_MODULES, kernel_version, IP6T_FILTER_KO);
		system(cmdbuf);
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "have_insmod_ip6t_filter", "0");
	}
	#endif

	if(tf_ipv6_nat_rules_num){
		system("ip6tables -t nat -N "TF_REDIRECT_CHAIN);
		system("ip6tables -t nat -D PREROUTING -j "TF_REDIRECT_CHAIN);
		system("ip6tables -t nat -A PREROUTING -j "TF_REDIRECT_CHAIN);
		system("ip6tables -t nat -F "TF_REDIRECT_CHAIN);
	}

	if(tf_ipv6_filter_rules_num){
		system("ip6tables -t filter -N "TF_BLOCK_CHAIN);
		system("ip6tables -t filter -D FORWARD -j "TF_BLOCK_CHAIN);
		system("ip6tables -t filter -A FORWARD -j "TF_BLOCK_CHAIN);
		system("ip6tables -t filter -F "TF_BLOCK_CHAIN);
	}

	system("iptables -t nat -N "TF_REDIRECT_CHAIN);
	system("iptables -t nat -D PREROUTING -j "TF_REDIRECT_CHAIN);
	system("iptables -t nat -A PREROUTING -j "TF_REDIRECT_CHAIN);

	system("iptables -t filter -N "TF_BLOCK_CHAIN);
	system("iptables -t filter -D FORWARD -j "TF_BLOCK_CHAIN);
	system("iptables -t filter -A FORWARD -j "TF_BLOCK_CHAIN);

	system("iptables -t nat -F "TF_REDIRECT_CHAIN);
	system("iptables -t filter -F "TF_BLOCK_CHAIN);
	
	system("chmod 755 "TF_ALL_ADDR_PATH);
	system("chmod 755 "TF_ALL_DOMAIN_PATH);

	system(TF_ALL_ADDR_PATH);

	system("chmod 755 "TF_ALL_ADDR_PATH);
	system("chmod 755 "TF_ALL_DOMAIN_PATH);
		
	return 0;
}

/* To handle only '-d' parameter is domain name in ip[6]tables rules */
int traffic_forward_rmt_domain_resolve(void){
	char tmpBuf[512] = {0};
	char remoteAddr[256] = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL;
	char *pValue = NULL;
	FILE * fp_running = NULL;
	FILE * fp_tmp = NULL;
	unsigned int fp_eof = 0;
	char tf_rmt_domain_file_update[4] = {0};
	int ret = 0;
	
	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_rmt_domain_file_update", 0, tf_rmt_domain_file_update, sizeof(tf_rmt_domain_file_update));

	if(atoi(tf_rmt_domain_file_update))
	{
		snprintf(tmpBuf, sizeof(tmpBuf), "cp -f %s %s", 
			TF_RMT_DOMAIN_PATH, TF_RMT_DOMAIN_PATH"_running");
		
		system(tmpBuf);
		bzero(tmpBuf, sizeof(tmpBuf));
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_rmt_domain_file_update", "0");
	}
	
	fp_running = fopen(TF_RMT_DOMAIN_PATH"_running", "r");
	if(fp_running == NULL){
		return 0;
	}
	
	fp_tmp = fopen(TF_RMT_DOMAIN_PATH"_tmp", "w");
	if(fp_tmp == NULL){
		fclose(fp_running);
		return 0;
	}
	
	while (fgets (tmpBuf, sizeof(tmpBuf), fp_running) != NULL){
		if(getTFDbgLevel())
			tcdbg_printf("readBuf[%d]:%s\n", __LINE__,  tmpBuf);
		if((pValue = strstr(tmpBuf,"-d "))){
			sscanf(pValue + 3,"%s", remoteAddr);
			
			/* parse domain */
			memset(&hints, 0, sizeof(hints));
			if(strstr(tmpBuf,"iptables "))
				hints.ai_family = AF_INET;
			else
				hints.ai_family = AF_INET6;
			
			if(0 != getaddrinfo(remoteAddr, NULL, &hints, &res)){
				fputs(tmpBuf, fp_tmp);
				continue;
			}
			
			cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_rmt_domain_file_update", 0, tf_rmt_domain_file_update, sizeof(tf_rmt_domain_file_update));
			if(atoi(tf_rmt_domain_file_update)){
				freeaddrinfo(res);
				break;
			}

			system(tmpBuf);
			if(getTFDbgLevel())
				tcdbg_printf("system[%d]:%s\n", __LINE__, tmpBuf);
			if(res)
				freeaddrinfo(res);
		}
	}

	fp_eof = getFileSize (fp_tmp);
	
	fclose(fp_running);
	fclose(fp_tmp);
		
	unlink(TF_RMT_DOMAIN_PATH"_running");
	if(fp_eof)
		ret = rename(TF_RMT_DOMAIN_PATH"_tmp", TF_RMT_DOMAIN_PATH"_running");
	else 
		unlink(TF_RMT_DOMAIN_PATH"_tmp");
	
	return ret;
}

/* To handle only '-to' parameter is domain name in ip[6]tables rules */
int traffic_forward_dst_domain_resolve(void){
	char tmpBuf[512] = {0};
	char finalTmpBuf[512] = {0};
	char ForwardToAddr[256] = {0};
	char ForwardToPort[32] = {0};
	char domain_addr[64] = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL, *rp = NULL;
	void *ptr = NULL;
	char *pValue = NULL;
	FILE * fp_running = NULL;
	FILE * fp_tmp = NULL;
	unsigned int fp_eof = 0;
	char tf_dst_domain_file_update[4] = {0};
	int ret = 0;
	
	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_dst_domain_file_update", 0, tf_dst_domain_file_update, sizeof(tf_dst_domain_file_update));

	if(atoi(tf_dst_domain_file_update))
	{
		snprintf(tmpBuf, sizeof(tmpBuf), "cp -f %s %s", 
			TF_DST_DOMAIN_PATH, TF_DST_DOMAIN_PATH"_running");
		
		system(tmpBuf);
		bzero(tmpBuf, sizeof(tmpBuf));
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_dst_domain_file_update", "0");
	}
	
	fp_running = fopen(TF_DST_DOMAIN_PATH"_running", "r");
	if(fp_running == NULL){
		return 0;
	}
	
	fp_tmp = fopen(TF_DST_DOMAIN_PATH"_tmp", "w");
	if(fp_tmp == NULL){
		fclose(fp_running);
		return 0;
	}

	while (fgets (tmpBuf, sizeof(tmpBuf), fp_running) != NULL){
		if(getTFDbgLevel())
			tcdbg_printf("readBuf[%d]:%s\n", __LINE__, tmpBuf);
		if((pValue = strstr(tmpBuf,"-to "))){
			sscanf(pValue + 4,"%[^:]:%s", ForwardToAddr, ForwardToPort);

			/* parse domain */
			memset(&hints, 0, sizeof(hints));
			hints.ai_socktype = SOCK_STREAM;
			
			if(strstr(tmpBuf,"iptables ")){
				hints.ai_family = AF_INET;
			}
			else{
				hints.ai_family = AF_INET6;
			}

			if(0 != getaddrinfo(ForwardToAddr, NULL, &hints, &res)){
				fputs(tmpBuf, fp_tmp);
				continue;
			}

			cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_dst_domain_file_update", 0, tf_dst_domain_file_update, sizeof(tf_dst_domain_file_update));
			if(atoi(tf_dst_domain_file_update)){
				freeaddrinfo(res);
				break;
			}

			#if 1
			for (rp = res; rp; rp = rp->ai_next)
			{
				switch(rp->ai_family)
				{
					case AF_INET:
						*(pValue + 4) = '\0';
						ptr = &((struct sockaddr_in *)rp->ai_addr)->sin_addr;
						inet_ntop(AF_INET, ptr, domain_addr, sizeof(domain_addr));
						snprintf(finalTmpBuf, sizeof(finalTmpBuf), "%s%s:%s",
							tmpBuf, domain_addr, ForwardToPort);
						break;
				
					case AF_INET6:
						*(pValue + 4) = '\0';
						ptr = &((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr;
						inet_ntop(AF_INET6, ptr, domain_addr, sizeof(domain_addr));
						snprintf(finalTmpBuf, sizeof(finalTmpBuf), "%s[%s]:%s",
							tmpBuf, domain_addr, ForwardToPort);
						break;
					default:
						break;
				}
			}
			
			if(getTFDbgLevel())
				tcdbg_printf("system[%d]:%s\n", __LINE__, finalTmpBuf);
			system(finalTmpBuf);
			
			freeaddrinfo(res);
			#endif
		}
	}

	fp_eof = getFileSize (fp_tmp);
	
	fclose(fp_running);
	fclose(fp_tmp);
		
	unlink(TF_DST_DOMAIN_PATH"_running");
	if(fp_eof)
		ret = rename(TF_DST_DOMAIN_PATH"_tmp", TF_DST_DOMAIN_PATH"_running");
	else 
		unlink(TF_DST_DOMAIN_PATH"_tmp");

	return ret;
}

/* To handle '-to' & '-d' parameter is domain name in ip[6]tables rules */
int traffic_forward_all_domain_resolve(void)
{
	char tmpBuf[512] = {0};
	char finalTmpBuf[512] = {0};
	char ForwardToAddr[256] = {0};
	char ForwardToPort[32] = {0};
	char remoteAddr[256] = {0};
	char domain_addr[40] = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res_d = NULL, *res_to = NULL;
	struct addrinfo *rp_d = NULL, *rp_to = NULL;
	void *ptr = NULL;
	char *pValue = NULL;
	FILE * fp_running = NULL;
	FILE * fp_tmp = NULL;
	unsigned int fp_eof = 0;
	char tf_all_domain_file_update[4] = {0};
	int ret = 0;
	
	cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_all_domain_file_update", 0, tf_all_domain_file_update, sizeof(tf_all_domain_file_update));
		
	if(atoi(tf_all_domain_file_update))
	{
		snprintf(tmpBuf, sizeof(tmpBuf), "cp -f %s %s", 
			TF_ALL_DOMAIN_PATH, TF_ALL_DOMAIN_PATH"_running");
		
		system(tmpBuf);
		bzero(tmpBuf, sizeof(tmpBuf));
		cfg_set_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_all_domain_file_update", "0");
	}
	
	fp_running = fopen(TF_ALL_DOMAIN_PATH"_running", "r");
	if(fp_running == NULL){
		return 0;
	}
	
	fp_tmp = fopen(TF_ALL_DOMAIN_PATH"_tmp", "w");
	if(fp_tmp == NULL){
		fclose(fp_running);
		return 0;
	}

	while (fgets (tmpBuf, sizeof(tmpBuf), fp_running) != NULL){
		if(getTFDbgLevel())
			tcdbg_printf("readBuf[%d]:%s\n", __LINE__, tmpBuf);
		if((pValue = strstr(tmpBuf,"-d "))){
			sscanf(pValue + 3,"%s", remoteAddr);
			
			/* parse domain */
			memset(&hints, 0, sizeof(hints));
			if(0 != getaddrinfo(remoteAddr, NULL, &hints, &res_d)){
				fputs(tmpBuf, fp_tmp);
				continue;
			}

			cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_all_domain_file_update", 0, tf_all_domain_file_update, sizeof(tf_all_domain_file_update));
			if(atoi(tf_all_domain_file_update)){
				freeaddrinfo(res_d);
				break;
			}
		}else{
			continue;
		}
		
		if((pValue = strstr(tmpBuf,"-to "))){
			sscanf(pValue + 4,"%[^:]:%s", ForwardToAddr, ForwardToPort);

			/* parse domain */
			memset(&hints, 0, sizeof(hints));
			hints.ai_socktype = SOCK_STREAM;
						
			if(0 != getaddrinfo(ForwardToAddr, NULL, &hints, &res_to)){
				fputs(tmpBuf, fp_tmp);
				continue;
			}

			cfg_obj_get_object_attr(GLOBALSTATE_TRANSFERSERVICES_NODE, "tf_all_domain_file_update", 0, tf_all_domain_file_update, sizeof(tf_all_domain_file_update));
			if(atoi(tf_all_domain_file_update)){
				freeaddrinfo(res_to);
				break;
			}
			#if 1
			for (rp_d = res_d; rp_d; rp_d = rp_d->ai_next)
			{
				for (rp_to = res_to; rp_to; rp_to = rp_to->ai_next)
				{
					if(AF_INET == rp_d->ai_family && AF_INET == rp_to->ai_family){
						*(pValue + 4) = '\0';
						ptr = &((struct sockaddr_in *)rp_to->ai_addr)->sin_addr;
						inet_ntop(AF_INET, ptr, domain_addr, sizeof(domain_addr));
						snprintf(finalTmpBuf, sizeof(finalTmpBuf), "iptables %s%s:%s",
							tmpBuf, domain_addr, ForwardToPort);
						break;
					}
					else if(AF_INET6 == rp_d->ai_family && AF_INET6 == rp_to->ai_family)
					{
						*(pValue + 4) = '\0';
						ptr = &((struct sockaddr_in6 *)rp_to->ai_addr)->sin6_addr;
						inet_ntop(AF_INET6, ptr, domain_addr, sizeof(domain_addr));
						snprintf(finalTmpBuf, sizeof(finalTmpBuf), "ip6tables %s[%s]:%s",
							tmpBuf, domain_addr, ForwardToPort);
						break;
					}
				}
				if(rp_to)/* find match domain result as the same family (V4 or V6) */{
					break;
				}
			}
			if(rp_to && rp_d){
				if(getTFDbgLevel())
					tcdbg_printf("system[%d]:%s\n", __LINE__, finalTmpBuf);
				system(finalTmpBuf);
			}
			else{
				fputs(tmpBuf, fp_tmp);
			}
			freeaddrinfo(res_to);
			#endif
		}
	}

	fp_eof = getFileSize (fp_tmp);
	
	fclose(fp_running);
	fclose(fp_tmp);
		
	unlink(TF_ALL_DOMAIN_PATH"_running");
	if(fp_eof)
		ret = rename(TF_ALL_DOMAIN_PATH"_tmp", TF_ALL_DOMAIN_PATH"_running");
	else 
		unlink(TF_ALL_DOMAIN_PATH"_tmp");

	if(res_d)
		freeaddrinfo(res_d);
	
	return ret;
}

int traffic_forward_domain_resolve(void)
{
	int ret;
	ret = traffic_forward_all_domain_resolve();
	if(ret != 0){
		tcdbg_printf("\ntraffic_forward_all_domain_resolve rename error!\n");
		return ret;
	}
	ret = traffic_forward_rmt_domain_resolve();
	if(ret != 0){
		tcdbg_printf("\ntraffic_forward_rmt_domain_resolve rename error!\n");
		return ret;
	}
	ret = traffic_forward_dst_domain_resolve();
	if(ret != 0){
		tcdbg_printf("\ntraffic_forward_dst_domain_resolve rename error!\n");
	}
	return ret;
}

static int cfg_type_trafficforward_func_execute(void)
{
	int flow_num = 0, is_exist = 0, idx = 0;
	char nodeName[64] = {0};
	char remotePort[24] = {0}, remoteAddr[256] = {0};
	char s_flow_class_num[32] = {0};

	bzero(nodeName, sizeof(nodeName));

	flow_num = 0;
	for ( idx = 1; idx <= TRAFFIC_FLOW_RULE_NUM; idx++ )
	{
		snprintf(nodeName, sizeof(nodeName), TRAFFICFORWARD_ENTRY_NODE, idx);
		
		is_exist = 0;
		
		if ( 0 > cfg_obj_get_object_attr(nodeName, "remoteAddr", 0, remoteAddr, sizeof(remoteAddr)))
			remoteAddr[0] = 0;
		else
			is_exist = 1;

		if ( 0 > cfg_obj_get_object_attr(nodeName, "remotePort", 0, remotePort, sizeof(remotePort)))
			remotePort[0] = 0;
		else
			is_exist = 1;

		if ( is_exist )
			flow_num ++;
	}
	
	BuffSet_Format(s_flow_class_num, "%d", flow_num);
	cfg_set_object_attr(TRAFFICFORWARD_COMMON_NODE, "Num", s_flow_class_num);
	
	return svc_cfg_execute_trafficforward_internal();
}

int svc_cfg_boot_trafficforward(void)
{
	cfg_type_trafficforward_func_write();
	cfg_type_trafficforward_func_execute();
	return 0;
}

static int cfg_type_trafficforward_func_commit(char* path)
{
	cfg_type_trafficforward_func_write();
	cfg_type_trafficforward_func_execute();
	return 0;
}


static cfg_node_ops_t cfg_type_trafficforward_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_trafficforward_func_commit 
}; 


static cfg_node_type_t cfg_type_trafficforward_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_trafficforward, 
	 .ops = &cfg_type_trafficforward_common_ops, 
}; 


static cfg_node_ops_t cfg_type_trafficforward_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_trafficforward_func_commit 
}; 


static cfg_node_type_t cfg_type_trafficforward_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | MAX_FLOWNUM_RULE, 
	 .parent = &cfg_type_trafficforward, 
	 .ops = &cfg_type_trafficforward_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_trafficforward_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_trafficforward_func_commit 
}; 


static cfg_node_type_t* cfg_type_trafficforward_child[] = { 
	 &cfg_type_trafficforward_entry, 
	 &cfg_type_trafficforward_common,
	 NULL 
}; 


cfg_node_type_t cfg_type_trafficforward = { 
	 .name = "TrafficForward", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_trafficforward_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_trafficforward_child, 
	 .ops = &cfg_type_trafficforward_ops, 
}; 
