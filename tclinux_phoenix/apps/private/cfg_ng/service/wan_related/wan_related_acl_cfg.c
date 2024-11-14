

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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cfg_api.h>
#include <cfg_msg.h>
#include "utility.h"
#include "wan_related_mgr.h"
#include "wan_related_cfg.h"

#define ACL_TCP_CMD "iptables -t filter -A acl_chain -i %s -p TCP -m multiport --dport %s -m iprange --src-range %s -j ACCEPT\n "
#define ACL_UDP_CMD "iptables -t filter -A acl_chain -i %s -p TCP -m multiport --dport %s -m iprange --src-range %s -j ACCEPT\n "
#define ACL_ICMP_CMD "iptables -t filter -A acl_chain -i %s -p ICMP --icmp-type 8 -m iprange --src-range %s -j ACCEPT\n "

int acl_check_filter(void)
{
	int i=0,acl_flag=0,filter_on=0;
	char acl_status[4]={0},acl_rule_status[8]={0};
	char nodeName[64] = {0};
	memset(nodeName,0,sizeof(nodeName));
	for(i=0; i<MAX_ACL_RULE; i++){
		snprintf(nodeName, sizeof(nodeName), ACL_ENTRY_N_NODE, i);
		
		if(cfg_get_object_attr(nodeName, "Activate", acl_rule_status, sizeof(acl_rule_status)) < 0)
		{
			/*printf("Error occurs while getting ACL_Entry Activate value.\n");*/
			continue;
		}
		if(!strcasecmp(acl_rule_status, "Yes")){
			acl_flag=1;
			break;
		}
	}

	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, ACL_COMMON_NODE, sizeof(nodeName)-1);
	/*get current information*/
	
	cfg_get_object_attr(nodeName, "Activate", acl_status, sizeof(acl_status));
	if(acl_status[0] != '\0')
	{
		if((!strcasecmp(acl_status, "yes"))&&(acl_flag!=0))
		{
			filter_on = 1;
		}
	}
	return filter_on;
}

int acl_get_active_interface(char interfaces[][8], const char* intf_tpye)
{
	int i=0;
	int counter=0;
	char active[6] = {0}, ifname[32] = {0}, nasname[32] = {0} ;
	
	/*initializae nodeName and interfaces*/
	memset(interfaces,0,(MAX_WAN_IF_INDEX+1)*8);
	
	/*if query type is Wan or Both, query Wan confiture node tree*/
	if((!strcasecmp(intf_tpye, "Wan"))||(!strcasecmp(intf_tpye, "Both"))){
		for(i = 0; i < MAX_WAN_IF_INDEX; i++){
			memset(active, 0, sizeof(active));
			memset(ifname, 0, sizeof(ifname));
			memset(nasname, 0, sizeof(nasname));
			if(get_waninfo_by_index(i, "Active", active, sizeof(active)) == 0 &&
				get_waninfo_by_index(i, "IFName", ifname, sizeof(ifname)) == 0 &&
					get_waninfo_by_index(i, "NASName", nasname, sizeof(nasname)) == 0 ){
				if(!strcmp(active, "Yes")){
					strncpy(interfaces[counter], ifname, sizeof(interfaces[counter])-1);
					counter++;
					if(strcmp(ifname, nasname))
					{
						strncpy(interfaces[counter], nasname, sizeof(interfaces[counter])-1);
						counter++;
					}
				}
			}
		}
	}

	/*if interface type is Both, we should add Lan interface data to the end of the list*/
	if(!strcasecmp(intf_tpye, "Both") ||!strcasecmp(intf_tpye, "Lan")){
		/*fprintf(stderr,"interf=%s\n", "br0");*/
		strncpy(interfaces[counter], "br0", sizeof(interfaces[counter])-1);
		counter++;
	}
	return counter;
}/*end acl_get_active_interface*/


int svc_wan_related_acl_write(void)
{
	int i=0, j=0, res = 0;
	char lower_value[32]={0};
	int active_if_num = 0;
	char nodeName[64] = {0}; 
	char acl_cmd[256]={0};
	char iprange[32]={0};
	char Activate[8] = {0};
	char ip_value1[32]= {0};
	char ip_value2[32]= {0};
	FILE *fp = NULL;
	char Application[32] = {0};
	char Interface[32] = {0};
	char interfaces[MAX_WAN_IF_INDEX+1][8]; 
	char buf[64] = {0};
	/*When all pvc are avtived in wan interface and a ACL rule actived with interface "Both", we need to store 9 interface information.*/ 
	if(cfg_get_object_attr(ACL_COMMON_NODE, "Activate", Activate, sizeof(Activate)) < 0)
	{	
		return FAIL;
	}

	unlink(ACL_SH);
	fp = fopen(ACL_SH, "w+");
	if(fp == NULL)
	{
		return FAIL;
	}

	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(ACL_COMMON_NODE, "Action", buf, sizeof(buf)) >= 0 && !strcmp(buf, "Del"))
	{
		memset(buf, 0, sizeof(buf));
		if(cfg_get_object_attr(ACL_COMMON_NODE, "DeleteIndex", buf, sizeof(buf)) >= 0 && buf[0] != '\0')
		{
			unset_action(buf, DEL_ACL);
		}
	}
	
	/*if ACL function is activated, compose iptables command, else do nothing*/
	if(!strcasecmp(Activate, "yes"))
	{
		/*flush previous settings of acl_chain*/
		fputs("iptables -F acl_chain\n", fp);
		/*set counters of acl_chain as zero*/
		fputs("iptables -Z acl_chain\n", fp);
		/*remove acl_chain*/
		fputs("iptables -X acl_chain\n", fp);
		/*create acl_chain*/
		fputs("iptables -N acl_chain\n", fp);

		/*remove rule in INPUT chain which jumps to acl_chain*/
		fputs("iptables -t filter -D ACL -p TCP -m multiport --dport ftp,telnet,http,snmp -j acl_chain\n", fp);
		fputs("iptables -t filter -D ACL -p UDP -m multiport --dport ftp,telnet,http,snmp -j acl_chain\n", fp);
		fputs("iptables -t filter -D ACL -p ICMP --icmp-type 8 -j acl_chain\n", fp);

		/*create rule in INPUT chain which jumps to acl_chain*/
		fputs("iptables -t filter -I ACL 1 -p TCP -m multiport --dport ftp,telnet,http,snmp -j acl_chain \n", fp);
		fputs("iptables -t filter -I ACL 2 -p UDP -m multiport --dport ftp,telnet,http,snmp -j acl_chain \n", fp);
		fputs("iptables -t filter -I ACL 3 -p ICMP --icmp-type 8 -j acl_chain \n", fp);

		for(i=0; i<MAX_ACL_RULE; i++)
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), ACL_ENTRY_N_NODE, i+1);
			if(cfg_query_object(nodeName,NULL,NULL) < 0)
			{
				continue;
			}

			memset(Activate,0,sizeof(Activate));
			if(cfg_get_object_attr(nodeName, "Activate", Activate, sizeof(Activate)) < 0 || strcasecmp(Activate, "yes"))
			{	
				continue;
			}

			memset(ip_value1, 0, sizeof(ip_value1));
			memset(ip_value2, 0, sizeof(ip_value2));
			memset(iprange, 0, sizeof(iprange));
			cfg_get_object_attr(nodeName, "ScrIPAddrBegin", ip_value1, sizeof(ip_value1));
			cfg_get_object_attr(nodeName, "ScrIPAddrEnd", ip_value2, sizeof(ip_value2));
			if((ip_value1[0] != '\0') && (ip_value2[0] != '\0'))
			{
				if(!(check_ip_format(ip_value1)) || !(check_ip_format(ip_value2)) || !(check_ip_range(ip_value1, ip_value2)))
				{
					continue;
				}
				snprintf(iprange, sizeof(iprange),"%s-%s", ip_value1, ip_value2);
				if(!strcmp(iprange, "0.0.0.0-0.0.0.0"))
				{	
					strncpy(iprange, "0.0.0.0-223.255.255.255", sizeof(iprange)-1); /*0.0.0.0-0.0.0.0 means all ip*/
				}
			}
			else
			{	
				continue;
			}

			/*retrieve Interface value in ACL Entry*/
			memset(Interface, 0, sizeof(Interface));
			memset(interfaces, 0, sizeof(interfaces));
			if(cfg_get_object_attr(nodeName, "Interface", Interface, sizeof(Interface)) < 0 ||
				(strcasecmp(Interface, "Wan") && strcasecmp(Interface, "Lan") && strcasecmp(Interface, "Both")))
			{	
				continue;
			}
			else
			{
				/*if interface vaule is valid, retrieve currently activated interface*/
				active_if_num = acl_get_active_interface(interfaces, Interface);
			}

			/*retrieve Application value in ACL Entry*/
			memset(Application, 0, sizeof(Application));
			memset(lower_value, 0, sizeof(lower_value));
			if(cfg_get_object_attr(nodeName, "Application", Application, sizeof(Application)) < 0)
			{	
				continue;
			}
			else
			{
				/*because iptables command can only read lowercase application(port) data, so we change case before use*/
				strncpy(lower_value, Application, sizeof(lower_value)-1);
				string_tolower(lower_value);

				/*iptables command cannot recognize web, so we change it to http*/
				if(!strcasecmp(Application, "web"))
				{
					strncpy(lower_value, "http", sizeof(lower_value)-1);
				}

				/*if Application value is all, it means ftp,telnet,http,snmp. for icmp, it is handled individually */
				if(!strcasecmp(Application, "all"))
				{
					strncpy(lower_value, "ftp,telnet,http,snmp", sizeof(lower_value)-1);
				}

				/*if Application value is valid, start to compose iptables commands*/
				if((!strcasecmp(Application, "ftp")) || (!strcasecmp(Application, "telnet")) || (!strcasecmp(Application, "web"))
					|| (!strcasecmp(Application, "snmp")) || (!strcasecmp(Application, "ping")) || (!strcasecmp(Application, "all")))
				{
					/*for each active interface, compose iptables commands*/
					for(j=0; (j < active_if_num) && (strlen(interfaces[j])!=0); j++)
					{
						/*if Application value is not ping(icmp), compose iptables commands for TCP and UDP respectively*/
						if(strcasecmp(Application, "ping"))
						{
							/*compose iptables commands which using TCP*/
							memset(acl_cmd, 0, sizeof(acl_cmd));
							snprintf(acl_cmd, sizeof(acl_cmd), ACL_TCP_CMD, interfaces[j], lower_value, iprange);
							fputs_escape(acl_cmd, fp);

							/*compose iptables commands which using UDP*/
							memset(acl_cmd, 0, sizeof(acl_cmd));
							snprintf(acl_cmd, sizeof(acl_cmd), ACL_UDP_CMD, interfaces[j], lower_value, iprange);
							fputs_escape(acl_cmd, fp);							
						}

						/*if Application value is not ping(icmp) or all, compose iptables commands for ICMP*/
						if((!strcasecmp(Application, "ping"))||(!strcasecmp(Application, "all")))
						{
							memset(acl_cmd, 0, sizeof(acl_cmd));
							snprintf(acl_cmd, sizeof(acl_cmd), ACL_ICMP_CMD, interfaces[j], iprange);
							fputs_escape(acl_cmd, fp);
						}
					}
				}
				else
				{	
					/*Application vaule is invalid, do nothing and jump to next rule*/
					continue;
				}
			}
		}

		/*set policy of acl_chain, if no rule is meet, drop this pocket*/
		fputs("iptables -A acl_chain -j DROP\n", fp);
	}

	fclose(fp);
	res = chmod(ACL_SH ,777);
	if(res)
	{
		/*do nothing,just for compile*/
	}
	return SUCCESS;
}/* end acl_write */

int svc_wan_related_acl_execute(void)
{
	unsigned int new_filter_state=0;
	char acl_status[8]={0};
	char newFilterState[32] = {0};

	memset(acl_status, 0, sizeof(acl_status));
	if(cfg_get_object_attr(ACL_COMMON_NODE, "Activate", acl_status, sizeof(acl_status)) < 0)
	{	
		return FAIL;
	}
	
	cfg_get_object_attr(GLOBALSTATE_PRESYSSTATE_NODE, "FilterState", newFilterState, sizeof(newFilterState)) ;

		/*check if iptable_filter module is loaded or not*/
		if(acl_check_filter())
		{
			new_filter_state= atoi(newFilterState) | ACL_VECTOR; /*filter on*/
		system_escape(ACL_SH);
		}
		else
		{
			new_filter_state= atoi(newFilterState) & (~ACL_VECTOR); /*filter down*/
		system_escape(ACL_STOP_SH);
		}
		
		check_and_set_filter( new_filter_state );
	
	return SUCCESS;
}

