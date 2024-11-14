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
#include <sys/types.h> 
#include <sys/stat.h> 
#include <cfg_cli.h> 
#include <cfg_xml.h> 
#include "cfg_types.h" 
#include <svchost_evt.h>
#include "utility.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#elif defined(TCSUPPORT_WLAN)
#include "libapi_lib_wifimgr.h"
#else
#define MAX_DUALBAND_BSSID_NUM 0
#endif
#if defined(TCSUPPORT_CWMP_TR181)
#include "cfg_tr181_utility.h"
#include "cfg_tr181_global.h"
#endif
#include "blapi_system.h"


#define TMP_MAC_FILE			"/tmp/macvalid"
#define TMP_IPUP_FILE			"/tmp/ipupvalid"
#define TMP_IPDOWN_FILE			"/tmp/ipdownvalid"
#define MACFILTER_SH			"/tmp/etc/macfilter.sh"
#define IPUPFILTER_SH			"/tmp/etc/ipupfilter.sh"
#define IPDOWNFILTER_SH			"/tmp/etc/ipdownfilter.sh"
#define	DEL_IP					2

int getfiltertype(void)
{
	char mac_ip_type[8] = {0};
	memset(mac_ip_type, 0, sizeof(mac_ip_type));
	cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", 0, mac_ip_type, sizeof(mac_ip_type));
	if (mac_ip_type[0] == '\0') 
	{
		printf("Error mac or ip type info is NULL.\n");
		return FAIL;
	}
	if (!strncmp(mac_ip_type, "Mac", sizeof(mac_ip_type))) 
	{
		return 0;
	}
	else if (!strncmp(mac_ip_type, "IpUp", sizeof(mac_ip_type))) 
	{
		return 1;
	}
	else if (!strncmp(mac_ip_type, "IpDown", sizeof(mac_ip_type))) 
	{
		return 2;
	}
	else 
	{
		printf("Error mac or ip type value is wrong\n");
		return FAIL;
	}
}

void DeleteFilterIndex(void)
{	
	char pAttr[400] = {0};
	char strDel[400] = {0};

	/*check attributea of action and do unset action*/
	memset(pAttr, 0, sizeof(pAttr));
	cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "Action", 0, pAttr, sizeof(pAttr));
	if(pAttr[0] != '\0')
	{
		if(!strncmp(pAttr, "Del", sizeof(pAttr)))
		{
			memset(pAttr, 0, sizeof(pAttr));
			cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "DeleteIndex", 0, pAttr, sizeof(pAttr));
			if(pAttr[0] != '\0')
			{
				strncpy(strDel, pAttr, sizeof(strDel)-1);
				unset_action(strDel, DEL_IP);
			}
		}			
	}
	return;
}

void FlushFilter(FILE *fp, int mac_ip)
{
	if(0 == mac_ip)
	{
		/* flush mac filter */
		fputs("iptables -F macfilter_chain\n", fp);
		fputs("iptables -Z macfilter_chain\n", fp);
		fputs("ebtables -F ebmacfilter\n", fp);
		fputs("ebtables -Z ebmacfilter\n", fp);
		
		fputs("ebtables -F ebmacfwfilter\n", fp);
		fputs("ebtables -Z ebmacfwfilter\n", fp);
	}
	else if(1 == mac_ip)
	{
		/* flush ip uplink filter */
		fputs("iptables -F ipupfilter_chain\n", fp);
		fputs("iptables -Z ipupfilter_chain\n", fp);
		fputs("ebtables -F ipv4upfilter\n", fp);
		fputs("ebtables -Z ipv4upfilter\n", fp);
	}
	else if(2 == mac_ip)
	{
		/* flush ip down link filter */
		fputs("iptables -F ipdownfilter_chain\n", fp);
		fputs("iptables -Z ipdownfilter_chain\n", fp);
		fputs("ebtables -F ipv4downfilter\n", fp);
		fputs("ebtables -Z ipv4downfilter\n", fp);
	}
	else
		/*do nothing*/

	return;
}


int isOnlyUpBlackProtocolRule(char* nodeName)
{
	char  valueBuf[32] = {0};
	int onlyProtocol = 0;
	int isUpBlack = 0;
	int isUp = 0;
	char src_ip[32] = {0};
	char des_ip[32] = {0};
	char src_port[8] = {0};
	char des_port[8] = {0};

	if(nodeName == NULL)
	{
		return -1;
	}
	
	memset(valueBuf, 0, sizeof(valueBuf));
	cfg_obj_get_object_attr(nodeName, "Protocol", 0, valueBuf, sizeof(valueBuf));
	if (valueBuf[0] != '\0')
	{
		onlyProtocol = 1;
	}

	memset(valueBuf, 0, sizeof(valueBuf));
	cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpUp", 0, valueBuf, sizeof(valueBuf));
	if (valueBuf[0] !='\0') 
	{
		if (!strcasecmp(valueBuf, "black")) 
		{
			isUpBlack = 1;
		}
	}

	memset(valueBuf, 0, sizeof(valueBuf));
	cfg_obj_get_object_attr(nodeName, "Direction", 0, valueBuf, sizeof(valueBuf));
	if (valueBuf[0] !='\0') 
	{
		if (!strcasecmp(valueBuf, "Outgoing")) 
		{
			isUp = 1;
		}
	}

	memset(src_ip, 0, sizeof(src_ip));
	memset(des_ip, 0, sizeof(des_ip));
	memset(src_port, 0, sizeof(src_port));
	memset(des_port, 0, sizeof(des_port));
	cfg_obj_get_object_attr(nodeName, "SrcIPAddr", 0, src_ip, sizeof(src_ip));
	cfg_obj_get_object_attr(nodeName, "DesIPAddr", 0, des_ip, sizeof(des_ip));
	cfg_obj_get_object_attr(nodeName, "SrcPort", 0, src_port, sizeof(src_port));
	cfg_obj_get_object_attr(nodeName, "DesPort", 0, des_port, sizeof(des_port));

	if(src_ip[0] != '\0' || des_ip[0] != '\0' || src_port[0] != '\0' || des_port[0] != '\0' )
	{
		onlyProtocol = 0;
	}

	if(onlyProtocol &&  isUpBlack && isUp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int  addProtocolRuleToFile(char* nodeName, FILE* fp, char *interface)
{
	char protocol[16] = {0};
	char ebtablesCmd[128] = {0};
	char iptablesCmd[128] = {0};
	
	if(nodeName == NULL || nodeName[0] == '\0')
	{
		return -1;
	}
	
	if (cfg_query_object(nodeName, NULL, NULL) <= 0)
	{
		return -1;
	}
	
	cfg_obj_get_object_attr(nodeName, "Protocol", 0, protocol, sizeof(protocol));
	
	if(protocol[0] != '\0')
	{	
		if(!strcasecmp(protocol, "TCP/UDP"))
		{
			/*TCP ebtables*/
			memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
			snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -A ipv4upfilter -p IPv4 -i %s --ip-proto TCP -j DROP\n", interface);
			fputs(ebtablesCmd, fp);

			/*udp ebtables*/
			memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
			snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -A ipv4upfilter -p IPv4 -i %s --ip-proto UDP -j DROP\n", interface);
			fputs(ebtablesCmd, fp);

		
			/*TCP/UDP iptables*/
			if(!strcasecmp(interface, "br0")){
				memset(iptablesCmd, 0, sizeof(iptablesCmd));
				snprintf(iptablesCmd, sizeof(iptablesCmd), "iptables -A ipupfilter_chain -i %s -p TCP -j DROP\n", interface);
				fputs(iptablesCmd, fp);

				memset(iptablesCmd, 0, sizeof(iptablesCmd));
				snprintf(iptablesCmd, sizeof(iptablesCmd), "iptables -A ipupfilter_chain -i %s -p UDP -j DROP\n", interface);
				fputs(iptablesCmd, fp);
			}
			
		}
		if(!strcasecmp(protocol, "ALL")){
			/* ebtables*/
			memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
			snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -A ipv4upfilter -p IPv4 -i %s -j DROP\n", interface);
			fputs(ebtablesCmd, fp);	


			if(!strcasecmp(interface, "br0")){
				memset(iptablesCmd, 0, sizeof(iptablesCmd));
				snprintf(iptablesCmd, sizeof(iptablesCmd), "iptables -A ipupfilter_chain -i %s -j DROP\n", interface);
				fputs(iptablesCmd, fp);
			}

		}
		else
		{
			/* ebtables*/
			memset(ebtablesCmd, 0, sizeof(ebtablesCmd));
			snprintf(ebtablesCmd, sizeof(ebtablesCmd), "ebtables -A ipv4upfilter -p IPv4 -i %s --ip-proto %s -j DROP\n", interface, protocol);
			fputs(ebtablesCmd, fp);	


			/* iptables*/			
			if(!strcasecmp(interface, "br0")){
				memset(iptablesCmd, 0, sizeof(iptablesCmd));
				snprintf(iptablesCmd, sizeof(iptablesCmd), "iptables -A ipupfilter_chain -i %s -p %s -j DROP\n", interface, protocol);
				fputs(iptablesCmd, fp); 
			}

		}
	}

	return 0;
}


int AssembleIpFilterFile(FILE *fp, char *target, char *ipmacfilter_cmd_head, 
	char *target_ipv4, char *ipv4filter_cmd_head, char *element_name, char *interface, char *protocol)
{
	char ipmacfilter_cmd_tail[256]={0};
	char ipv4filter_cmd_tail[256]={0};
	char src_ip[32] = {0};
	char ip_end[32] = {0};
	char src_mask[32] = {0};
	char src_mask_dec[8]={0};
	char des_ip[32] = {0};
	char des_mask[32] = {0};
	char des_mask_dec[8]={0};
	char src_port[8] = {0};
	char des_port[8] = {0};
	char ipmacfilter_cmd[256]={0};
	char *p_UDP = NULL;
	char *p_TCP = NULL;
	char *p_ICMP = NULL;
	char *chain_p_up = NULL;
	char *chain_p_dw = NULL;
	char ipmacfilter_cmd_glib[256]={0}; 
	char ipv4filter_cmd[512]={0};
	
	/* it will happen in general */
	if (fp == NULL) 
	{
		printf("port filter.\n");
		return -1;
	}


	if(isOnlyUpBlackProtocolRule(element_name) == 1)
	{
		addProtocolRuleToFile(element_name, fp, interface);
		return 0;
	}
	memset(ipmacfilter_cmd_tail, 0, sizeof(ipmacfilter_cmd_tail));
	memset(ipv4filter_cmd_tail, 0, sizeof(ipv4filter_cmd_tail));
	/* source ip address */
	memset(src_ip, 0, sizeof(src_ip));
	cfg_obj_get_object_attr(element_name, "SrcIPAddr", 0, src_ip, sizeof(src_ip));
	if (src_ip[0] != '\0') 
	{
		if (!check_ip_format(src_ip)) 
		{
			printf("ipfilter_write():check src ip failed.\n");
			return -1;
		}
#if defined(TCSUPPORT_CMCC)
		cfg_obj_get_object_attr(element_name, "SrcIPEndAddr", 0, ip_end, sizeof(ip_end));
		if (ip_end[0] != '\0') 
		{
			if (!check_ip_format(ip_end))
			{
				printf("ipfilter_write():check src end ip failed.\n");
				return -1;
			}
			strncat(ipmacfilter_cmd_tail, " -m iprange --src-range ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, src_ip, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, "-", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, ip_end, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));

			strncat(ipv4filter_cmd_tail, " -p IPv4 --ip-src ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			strncat(ipv4filter_cmd_tail, src_ip, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			strncat(ipv4filter_cmd_tail, "-", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			strncat(ipv4filter_cmd_tail, ip_end, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
		}
		else{
#endif
		strncat(ipmacfilter_cmd_tail, " -s ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipmacfilter_cmd_tail, src_ip, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipv4filter_cmd_tail, " -p IPv4 --ip-src ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
		strncat(ipv4filter_cmd_tail, src_ip, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
		memset(src_mask, 0, sizeof(src_mask));
		cfg_obj_get_object_attr(element_name, "SrcIPMask", 0, src_mask, sizeof(src_mask));

		if (src_mask[0] != '\0') 
		{
			if (!check_mask_format(src_mask, src_mask_dec,sizeof(src_mask_dec))) 
			{
				printf("ipfilter_write():check src ip mask failed.\n");
				return -1;
			}

			strncat(ipmacfilter_cmd_tail, "/", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, src_mask_dec, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipv4filter_cmd_tail, "/", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			strncat(ipv4filter_cmd_tail, src_mask_dec, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
		}
#if defined(TCSUPPORT_CMCC)
		}
#endif
	}

	/* des ip address */
	memset(des_ip, 0, sizeof(des_ip));
	cfg_obj_get_object_attr(element_name, "DesIPAddr", 0, des_ip, sizeof(des_ip));
	if (des_ip[0] != '\0') 
	{
		if (!check_ip_format(des_ip)) 
		{
			printf("ipfilter_write():check des ip failed.\n");
			return -1;
		}
#if defined(TCSUPPORT_CMCC)
		cfg_obj_get_object_attr(element_name, "DesIPEndAddr", 0, ip_end, sizeof(ip_end));
		if (ip_end[0] != '\0') 
		{
			if (!check_ip_format(ip_end)) 
			{
				printf("ipfilter_write():check des end ip failed.\n");
				return -1;
			}
			strncat(ipmacfilter_cmd_tail, " -m iprange --dst-range ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, des_ip, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, "-", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, ip_end, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			if (src_ip[0] != '\0') 
			{
				strncat(ipv4filter_cmd_tail, " --ip-dst ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			}
			else
			{
				strncat(ipv4filter_cmd_tail, " -p IPv4 --ip-dst ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			}
			strncat(ipv4filter_cmd_tail, des_ip, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipv4filter_cmd_tail, "-", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipv4filter_cmd_tail, ip_end, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		}
		else
		{
#endif
		strncat(ipmacfilter_cmd_tail, " -d ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipmacfilter_cmd_tail, des_ip, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		if (src_ip[0] != '\0') 
		{
			strncat(ipv4filter_cmd_tail, " --ip-dst ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		}
		else
		{
			strncat(ipv4filter_cmd_tail, " -p IPv4 --ip-dst ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		}
		strncat(ipv4filter_cmd_tail, des_ip, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		memset(des_mask, 0, sizeof(des_mask));
		cfg_obj_get_object_attr(element_name, "DesIPMask", 0, des_mask, sizeof(des_mask));

		if (des_mask[0] != '\0') 
		{
			if (!check_mask_format(des_mask, des_mask_dec,sizeof(des_mask_dec))) 
			{
				printf("ipfilter_write():check des ip mask failed.\n");
				return -1;
			}

			strncat(ipmacfilter_cmd_tail, "/", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, des_mask_dec, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipv4filter_cmd_tail, "/", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			strncat(ipv4filter_cmd_tail, des_mask_dec, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
		}
#if defined(TCSUPPORT_CMCC)
		}
#endif
	}
	if (interface != NULL && interface[0] != '\0') 
	{
		strncat(ipmacfilter_cmd_tail, " -i ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipmacfilter_cmd_tail, interface, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		
		strncat(ipv4filter_cmd_tail, " -i ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
		strncat(ipv4filter_cmd_tail, interface, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
	}
	if ( strlen(protocol) )
	{
		if(!strcasecmp(protocol, "ALL"))
		{		
			snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd), "%s%s",ipmacfilter_cmd_head, ipmacfilter_cmd_tail);
			strncat(ipmacfilter_cmd, " -j ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));				
			strncat(ipmacfilter_cmd, target, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, "\n", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			fputs_escape(ipmacfilter_cmd, fp);
				

			if ((src_ip[0] == '\0') && (des_ip[0] == '\0'))
	{
				strncat(ipv4filter_cmd_tail, " -p IPv4 ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			}

			
			snprintf(ipv4filter_cmd, sizeof(ipv4filter_cmd), "%s%s",ipv4filter_cmd_head, ipv4filter_cmd_tail);
			strncat(ipv4filter_cmd, " -j ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));				
			strncat(ipv4filter_cmd, target_ipv4, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, "\n", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			fputs_escape(ipv4filter_cmd, fp);				
		}
		else if (!strcasecmp(protocol, "TCP") ||!strcasecmp(protocol, "UDP") || !strcasecmp(protocol, "ICMP")) 
		{
			strncat(ipmacfilter_cmd_tail, " -p ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			strncat(ipmacfilter_cmd_tail, protocol, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			if ((src_ip[0] != '\0') || (des_ip[0] != '\0'))
			{
				strncat(ipv4filter_cmd_tail, " --ip-proto ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			}
			else
			{
				strncat(ipv4filter_cmd_tail, " -p IPv4 --ip-proto ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			}
			strncat(ipv4filter_cmd_tail, protocol, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			
			/* if its not icmp protocol */
			if (0 != strcasecmp(protocol, "ICMP")) 
			{
				/* source  port */
				memset(src_port, 0, sizeof(src_port));
				cfg_obj_get_object_attr(element_name, "SrcPort", 0, src_port, sizeof(src_port));
				if (src_port[0] != '\0') 
				{
					if (atoi(src_port) < 0 || atoi(src_port) > 65535) 
					{
						printf("ipfilter_write():check src port failed.\n");
						return -1;
					}
					
					strncat(ipmacfilter_cmd_tail, " --sport ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
					strncat(ipmacfilter_cmd_tail, src_port, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
					strncat(ipv4filter_cmd_tail, " --ip-sport ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
					strncat(ipv4filter_cmd_tail, src_port, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
#if defined(TCSUPPORT_CMCC)
					cfg_obj_get_object_attr(element_name, "SrcPortEnd", 0, src_port, sizeof(src_port));
					if (src_port[0] != '\0') 
					{
						if (atoi(src_port) < 0 || atoi(src_port) > 65535) 
						{
							printf("ipfilter_write():check src portend failed.\n");
							return -1;
						}
						strncat(ipmacfilter_cmd_tail, ":", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
						strncat(ipmacfilter_cmd_tail, src_port, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
						strncat(ipv4filter_cmd_tail, ":", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
						strncat(ipv4filter_cmd_tail, src_port, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
					}
#endif
				}

				/* des port */
				memset(des_port, 0, sizeof(des_port));
				cfg_obj_get_object_attr(element_name, "DesPort", 0, des_port, sizeof(des_port));
				if (des_port[0] != '\0') 
				{
					if (atoi(des_port) < 0 || atoi(des_port) > 65535) 
					{
						printf("ipfilter_write():check des port failed.\n");
						return -1;
					}

					strncat(ipmacfilter_cmd_tail, " --dport ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
					strncat(ipmacfilter_cmd_tail, des_port, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
					strncat(ipv4filter_cmd_tail, " --ip-dport ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
					strncat(ipv4filter_cmd_tail, des_port, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
#if defined(TCSUPPORT_CMCC)
					cfg_obj_get_object_attr(element_name, "DesPortEnd", 0, des_port, sizeof(des_port));
					if (des_port[0] != '\0') 
					{
						if (atoi(des_port) < 0 || atoi(des_port) > 65535) 
						{
							printf("ipfilter_write():check des portend failed.\n");
							return -1;
						}
						strncat(ipmacfilter_cmd_tail, ":", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
						strncat(ipmacfilter_cmd_tail, des_port, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
						strncat(ipv4filter_cmd_tail, ":", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
						strncat(ipv4filter_cmd_tail, des_port, sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
					}
#endif
				}
			}
	
			snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd), "%s%s",ipmacfilter_cmd_head, ipmacfilter_cmd_tail);
			
			strncat(ipmacfilter_cmd, " -j ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, target, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, "\n", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));

#if defined(CONFIG_GLIBC_VER) 
			if ( NULL != (p_UDP = strstr(ipmacfilter_cmd, "-p UDP"))
				|| NULL != (p_TCP = strstr(ipmacfilter_cmd, "-p TCP"))
				|| NULL != (p_ICMP = strstr(ipmacfilter_cmd, "-p ICMP")) )
			{
				chain_p_up = strstr(ipmacfilter_cmd, "ipupfilter_chain");
				chain_p_dw = strstr(ipmacfilter_cmd, "ipdownfilter_chain");
				if ( chain_p_up || chain_p_dw )
				{
					if ( p_UDP )
						memset(p_UDP, ' ', 6);
					
					if ( p_TCP )
						memset(p_TCP, ' ', 6);

					if ( p_ICMP)
						memset(p_ICMP, ' ', 7);
					
					memset(ipmacfilter_cmd_glib, 0, sizeof(ipmacfilter_cmd_glib));
					snprintf(ipmacfilter_cmd_glib, sizeof(ipmacfilter_cmd_glib) - 1,
						"iptables -t filter -A %s -p %s %s"
						, chain_p_up ? "ipupfilter_chain" : "ipdownfilter_chain"
						, p_UDP ? "UDP" : ( p_TCP ? "TCP" : "ICMP")
						, chain_p_up ? chain_p_up + strlen("ipupfilter_chain") : chain_p_dw + strlen("ipdownfilter_chain"));
		
					memset(ipmacfilter_cmd, 0, sizeof(ipmacfilter_cmd));
					snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd) - 1,
						"%s", ipmacfilter_cmd_glib);
				}
			}
#endif
			fputs_escape(ipmacfilter_cmd, fp);
			snprintf(ipv4filter_cmd, sizeof(ipv4filter_cmd), "%s%s", ipv4filter_cmd_head, ipv4filter_cmd_tail);
			
			strncat(ipv4filter_cmd, " -j ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, target_ipv4, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, "\n", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd)); 		
			fputs_escape(ipv4filter_cmd, fp);
		}
		else if (!strcasecmp(protocol, "TCP/UDP"))
		{
			/* tcp */
			strncat(ipmacfilter_cmd_tail, " -p ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
			snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd), "%s%s%s", ipmacfilter_cmd_head, ipmacfilter_cmd_tail, "TCP");
			if ((src_ip[0] != '\0') || (des_ip[0] != '\0'))
			{
				strncat(ipv4filter_cmd_tail, " --ip-proto ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			}
			else
			{
				strncat(ipv4filter_cmd_tail, " -p IPv4 --ip-proto ", sizeof(ipv4filter_cmd_tail) - strlen(ipv4filter_cmd_tail));
			}
			snprintf(ipv4filter_cmd, sizeof(ipv4filter_cmd), "%s%s%s", ipv4filter_cmd_head, ipv4filter_cmd_tail, "TCP");

			/* source  port */
			memset(src_port, 0, sizeof(src_port));
			cfg_obj_get_object_attr(element_name, "SrcPort", 0, src_port, sizeof(src_port));
			if (src_port[0] != '\0') 
			{
				if (atoi(src_port) < 0 || atoi(src_port) > 65535) 
				{
					printf("ipfilter_write():(tcp)check src port failed.\n");
					return -1;
				}
				
				strncat(ipmacfilter_cmd, " --sport ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipmacfilter_cmd, src_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipv4filter_cmd, " --ip-sport ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				strncat(ipv4filter_cmd, src_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
#if defined(TCSUPPORT_CMCC)
				cfg_obj_get_object_attr(element_name, "SrcPortEnd", 0, src_port, sizeof(src_port));
				if (src_port[0] != '\0') 
				{
					if (atoi(src_port) < 0 || atoi(src_port) > 65535) 
					{
						printf("ipfilter_write():(TCP/UDP-tcp)check src portend failed.\n");
						return -1;
					}								
					strncat(ipmacfilter_cmd, ":", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipmacfilter_cmd, src_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipv4filter_cmd, ":", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
					strncat(ipv4filter_cmd, src_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				}
#endif
			}

			/* des port */
			memset(des_port, 0, sizeof(des_port));
			cfg_obj_get_object_attr(element_name, "DesPort", 0, des_port, sizeof(des_port));
			if (des_port[0] != '\0') 
			{
				if (atoi(des_port) < 0 || atoi(des_port) > 65535)
				{
					printf("ipfilter_write():(tcp)check des port failed.\n");
					return -1;
				}

				strncat(ipmacfilter_cmd, " --dport ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipmacfilter_cmd, des_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipv4filter_cmd, " --ip-dport ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				strncat(ipv4filter_cmd, des_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
#if defined(TCSUPPORT_CMCC)
				cfg_obj_get_object_attr(element_name, "DesPortEnd", 0, des_port, sizeof(des_port));
				if (des_port[0] != '\0') 
				{
					if (atoi(des_port) < 0 || atoi(des_port) > 65535) 
					{
						printf("ipfilter_write():(TCP/UDP-tcp)check des portend failed.\n");
						return -1;
					}							
					strncat(ipmacfilter_cmd, ":", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipmacfilter_cmd, des_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipv4filter_cmd, ":", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
					strncat(ipv4filter_cmd, des_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				}
#endif
			}
				
			strncat(ipmacfilter_cmd, " -j ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, target, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, "\n", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
#if defined(CONFIG_GLIBC_VER) 
			if ( NULL != (p_UDP = strstr(ipmacfilter_cmd, "-p UDP"))
				|| NULL != (p_TCP = strstr(ipmacfilter_cmd, "-p TCP")) )
			{
				chain_p_up = strstr(ipmacfilter_cmd, "ipupfilter_chain");
				chain_p_dw = strstr(ipmacfilter_cmd, "ipdownfilter_chain");
				if ( chain_p_up || chain_p_dw )
				{
					if ( p_UDP )
						memset(p_UDP, ' ', 6);
					
					if ( p_TCP )
						memset(p_TCP, ' ', 6);
					
					memset(ipmacfilter_cmd_glib, 0, sizeof(ipmacfilter_cmd_glib));
					snprintf(ipmacfilter_cmd_glib, sizeof(ipmacfilter_cmd_glib) - 1,
						"iptables -t filter -A %s -p %s %s"
						, chain_p_up ? "ipupfilter_chain" : "ipdownfilter_chain"
						, p_UDP ? "UDP" : "TCP"
						, chain_p_up ? chain_p_up + strlen("ipupfilter_chain") : chain_p_dw + strlen("ipdownfilter_chain"));
		
					memset(ipmacfilter_cmd, 0, sizeof(ipmacfilter_cmd));
					snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd) - 1,
						"%s", ipmacfilter_cmd_glib);
				}
			}
#endif
			
			fputs_escape(ipmacfilter_cmd, fp);
			strncat(ipv4filter_cmd, " -j ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, target_ipv4, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, "\n", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			fputs_escape(ipv4filter_cmd, fp);

			/* udp */
			snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd), 
				"%s%s%s", ipmacfilter_cmd_head, ipmacfilter_cmd_tail, "UDP");
			snprintf(ipv4filter_cmd, sizeof(ipv4filter_cmd), "%s%s%s", ipv4filter_cmd_head, ipv4filter_cmd_tail, "UDP");

			/* source  port */
			memset(src_port, 0, sizeof(src_port));
			cfg_obj_get_object_attr(element_name, "SrcPort", 0, src_port, sizeof(src_port));
			if (src_port[0] != '\0') 
			{
				if (atoi(src_port) < 0 || atoi(src_port) > 65535) 
				{
					printf("ipfilter_write():(udp)check src port failed.\n");
					return -1;
				}
				
				strncat(ipmacfilter_cmd, " --sport ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipmacfilter_cmd, src_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipv4filter_cmd, " --ip-sport ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				strncat(ipv4filter_cmd, src_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
#if defined(TCSUPPORT_CMCC)
				cfg_obj_get_object_attr(element_name, "SrcPortEnd", 0, src_port, sizeof(src_port));
				if (src_port[0] != '\0') 
				{
					if (atoi(src_port) < 0 || atoi(src_port) > 65535) 
					{
						printf("ipfilter_write():(TCP/UDP-udp)check src portend failed.\n");
						return -1;
					}								
					strncat(ipmacfilter_cmd, ":", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipmacfilter_cmd, src_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipv4filter_cmd, ":", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
					strncat(ipv4filter_cmd, src_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				}
#endif
			}

			/* des port */
			memset(des_port, 0, sizeof(des_port));
			cfg_obj_get_object_attr(element_name, "DesPort", 0, des_port, sizeof(des_port));
			if (des_port[0] != '\0') 
			{
				if (atoi(des_port) < 0 || atoi(des_port) > 65535) 
				{
					printf("ipfilter_write():(udp)check des port failed.\n");
					return -1;
				}

				strncat(ipmacfilter_cmd, " --dport ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipmacfilter_cmd, des_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
				strncat(ipv4filter_cmd, " --ip-dport ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				strncat(ipv4filter_cmd, des_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
#if defined(TCSUPPORT_CMCC)
				cfg_obj_get_object_attr(element_name, "DesPortEnd", 0, des_port, sizeof(des_port));
				if (des_port[0] != '\0') 
				{
					if (atoi(des_port) < 0 || atoi(des_port) > 65535) 
					{
						printf("ipfilter_write():(TCP/UDP-udp)check des portend failed.\n");
						return -1;
					}
					strncat(ipmacfilter_cmd, ":", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipmacfilter_cmd, des_port, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
					strncat(ipv4filter_cmd, ":", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
					strncat(ipv4filter_cmd, des_port, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
				}
#endif
			}
			
			strncat(ipmacfilter_cmd, " -j ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, target, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			strncat(ipmacfilter_cmd, "\n", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
			
#if defined(CONFIG_GLIBC_VER) 
			if ( NULL != (p_UDP = strstr(ipmacfilter_cmd, "-p UDP"))
				|| NULL != (p_TCP = strstr(ipmacfilter_cmd, "-p TCP")) )
			{
				chain_p_up = strstr(ipmacfilter_cmd, "ipupfilter_chain");
				chain_p_dw = strstr(ipmacfilter_cmd, "ipdownfilter_chain");
				if ( chain_p_up || chain_p_dw )
				{
					if ( p_UDP )
						memset(p_UDP, ' ', 6);
					
					if ( p_TCP )
						memset(p_TCP, ' ', 6);
					
					memset(ipmacfilter_cmd_glib, 0, sizeof(ipmacfilter_cmd_glib));
					snprintf(ipmacfilter_cmd_glib, sizeof(ipmacfilter_cmd_glib) - 1,
						"iptables -t filter -A %s -p %s %s"
						, chain_p_up ? "ipupfilter_chain" : "ipdownfilter_chain"
						, p_UDP ? "UDP" : "TCP"
						, chain_p_up ? chain_p_up + strlen("ipupfilter_chain") : chain_p_dw + strlen("ipdownfilter_chain"));
		
					memset(ipmacfilter_cmd, 0, sizeof(ipmacfilter_cmd));
					snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd) - 1,
						"%s", ipmacfilter_cmd_glib);
				}
			}
#endif						
			fputs_escape(ipmacfilter_cmd, fp);
			
			strncat(ipv4filter_cmd, " -j ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, target_ipv4, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
			strncat(ipv4filter_cmd, "\n", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd)); 
			fputs_escape(ipv4filter_cmd, fp);
		}
		else 
		{
			return -1;
		}

	}
	else 
	{
		snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd), "%s%s", ipmacfilter_cmd_head, ipmacfilter_cmd_tail);
		
		strncat(ipmacfilter_cmd, " -j ", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
		strncat(ipmacfilter_cmd, target, sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
		strncat(ipmacfilter_cmd, "\n", sizeof(ipmacfilter_cmd) - strlen(ipmacfilter_cmd));
		
		fputs_escape(ipmacfilter_cmd, fp);
		snprintf(ipv4filter_cmd, sizeof(ipv4filter_cmd), "%s%s", ipv4filter_cmd_head, ipv4filter_cmd_tail);
		
		strncat(ipv4filter_cmd, " -j ", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
		strncat(ipv4filter_cmd, target_ipv4, sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd));
		strncat(ipv4filter_cmd, "\n", sizeof(ipv4filter_cmd) - strlen(ipv4filter_cmd)); 		
		fputs_escape(ipv4filter_cmd, fp);
	}

	return 0;
}

int AssembleMacFilterFile(FILE *fp, char *target, char *element_name, char *interface, int if_len)
{
	char ipmacfilter_cmd_head[48]={0};
	char ebmacfilter_cmd_head[48]={0};
	char ebmacfwfilter_cmd_head[48]={0};
	char mac_addr[24] = {0};
	char ipmacfilter_cmd_tail[96]={0};
	char ebmacfilter_cmd_tail_ipv4[128]={0};
	char ebmacfilter_cmd_tail_ipv6[128]={0};
	char ebmacfilter_cmd_tail_src_ipv4[128]={0};
	char ebmacfilter_cmd_tail_dst_ipv4[128]={0};
	char ebmacfilter_cmd_tail_src_ipv6[128]={0};
	char ebmacfilter_cmd_tail_dst_ipv6[128]={0};
	char ipmacfilter_cmd[256]={0};
	char ebmacfilter_cmd_ipv4[256]={0};
	char ebmacfilter_cmd_ipv6[256]={0};
	char ebmacfilter_cmd_src_ipv4[256]={0};
	char ebmacfilter_cmd_dst_ipv4[256]={0};
	char ebmacfilter_cmd_src_ipv6[256]={0};
	char ebmacfilter_cmd_dst_ipv6[256]={0};
	
	if (fp == NULL) 
	{
		printf("mac failed");
		return -1;
	}
	strncpy(ipmacfilter_cmd_head, "iptables -t filter -A macfilter_chain", sizeof(ipmacfilter_cmd_head)-1);
	strncpy(ebmacfilter_cmd_head, "ebtables -t filter -A ebmacfilter", sizeof(ebmacfilter_cmd_head)-1);
	strncpy(ebmacfwfilter_cmd_head, "ebtables -t filter -A ebmacfwfilter", sizeof(ebmacfwfilter_cmd_head)-1);
	
	memset(mac_addr, 0, sizeof(mac_addr));
	cfg_obj_get_object_attr(element_name, "MacAddr", 0, mac_addr, sizeof(mac_addr));
	if(mac_addr[0] == '\0')
	{
		return -1;
	}
	if(!check_mac_format(mac_addr))
	{
		printf("Error--Mac Address is invalide.\n");
		return -1;
	}
	else
	{
		/*compose iptables command*/
		strncpy(ipmacfilter_cmd_tail,  " -m mac --mac-source ", sizeof(ipmacfilter_cmd_tail)-1);
		strncat(ipmacfilter_cmd_tail, mac_addr, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipmacfilter_cmd_tail, " -j ", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipmacfilter_cmd_tail, target, sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));
		strncat(ipmacfilter_cmd_tail, "\n", sizeof(ipmacfilter_cmd_tail) - strlen(ipmacfilter_cmd_tail));

		strncpy(ebmacfilter_cmd_tail_ipv4,	" -s ", sizeof(ebmacfilter_cmd_tail_ipv4)-1);
		strncat(ebmacfilter_cmd_tail_ipv4, mac_addr, sizeof(ebmacfilter_cmd_tail_ipv4) - strlen(ebmacfilter_cmd_tail_ipv4));
		strncat(ebmacfilter_cmd_tail_ipv4,	" -p IPv4 -j ", sizeof(ebmacfilter_cmd_tail_ipv4) - strlen(ebmacfilter_cmd_tail_ipv4));
		strncat(ebmacfilter_cmd_tail_ipv4, target, sizeof(ebmacfilter_cmd_tail_ipv4) - strlen(ebmacfilter_cmd_tail_ipv4));				
		strncat(ebmacfilter_cmd_tail_ipv4, "\n", sizeof(ebmacfilter_cmd_tail_ipv4) - strlen(ebmacfilter_cmd_tail_ipv4));

		strncpy(ebmacfilter_cmd_tail_ipv6,	" -s ", sizeof(ebmacfilter_cmd_tail_ipv6)-1);
		strncat(ebmacfilter_cmd_tail_ipv6, mac_addr, sizeof(ebmacfilter_cmd_tail_ipv6) - strlen(ebmacfilter_cmd_tail_ipv6));
		strncat(ebmacfilter_cmd_tail_ipv6,	" -p IPv6 -j ", sizeof(ebmacfilter_cmd_tail_ipv6) - strlen(ebmacfilter_cmd_tail_ipv6));
		strncat(ebmacfilter_cmd_tail_ipv6, target, sizeof(ebmacfilter_cmd_tail_ipv6) - strlen(ebmacfilter_cmd_tail_ipv6));				
		strncat(ebmacfilter_cmd_tail_ipv6, "\n", sizeof(ebmacfilter_cmd_tail_ipv6) - strlen(ebmacfilter_cmd_tail_ipv6));

		strncpy(ebmacfilter_cmd_tail_src_ipv4,	" -s ", sizeof(ebmacfilter_cmd_tail_src_ipv4)-1);
		strncat(ebmacfilter_cmd_tail_src_ipv4, mac_addr, sizeof(ebmacfilter_cmd_tail_src_ipv4) - strlen(ebmacfilter_cmd_tail_src_ipv4));
		strncat(ebmacfilter_cmd_tail_src_ipv4, " -p IPv4 -j ", sizeof(ebmacfilter_cmd_tail_src_ipv4) - strlen(ebmacfilter_cmd_tail_src_ipv4));
		strncat(ebmacfilter_cmd_tail_src_ipv4, target, sizeof(ebmacfilter_cmd_tail_src_ipv4) - strlen(ebmacfilter_cmd_tail_src_ipv4));
		strncat(ebmacfilter_cmd_tail_src_ipv4, "\n", sizeof(ebmacfilter_cmd_tail_src_ipv4) - strlen(ebmacfilter_cmd_tail_src_ipv4));
		
		strncpy(ebmacfilter_cmd_tail_dst_ipv4,	" -d ", sizeof(ebmacfilter_cmd_tail_dst_ipv4)-1);
		strncat(ebmacfilter_cmd_tail_dst_ipv4, mac_addr, sizeof(ebmacfilter_cmd_tail_dst_ipv4) - strlen(ebmacfilter_cmd_tail_dst_ipv4));
		strncat(ebmacfilter_cmd_tail_dst_ipv4, " -p IPv4 -j ", sizeof(ebmacfilter_cmd_tail_dst_ipv4) - strlen(ebmacfilter_cmd_tail_dst_ipv4));
		strncat(ebmacfilter_cmd_tail_dst_ipv4, target, sizeof(ebmacfilter_cmd_tail_dst_ipv4) - strlen(ebmacfilter_cmd_tail_dst_ipv4));
		strncat(ebmacfilter_cmd_tail_dst_ipv4, "\n", sizeof(ebmacfilter_cmd_tail_dst_ipv4) - strlen(ebmacfilter_cmd_tail_dst_ipv4));

		strncpy(ebmacfilter_cmd_tail_src_ipv6,	" -s ", sizeof(ebmacfilter_cmd_tail_src_ipv6)-1);
		strncat(ebmacfilter_cmd_tail_src_ipv6, mac_addr, sizeof(ebmacfilter_cmd_tail_src_ipv6) - strlen(ebmacfilter_cmd_tail_src_ipv6));
		strncat(ebmacfilter_cmd_tail_src_ipv6, " -p IPv6 -j ", sizeof(ebmacfilter_cmd_tail_src_ipv6) - strlen(ebmacfilter_cmd_tail_src_ipv6));
		strncat(ebmacfilter_cmd_tail_src_ipv6, target, sizeof(ebmacfilter_cmd_tail_src_ipv6) - strlen(ebmacfilter_cmd_tail_src_ipv6));
		strncat(ebmacfilter_cmd_tail_src_ipv6, "\n", sizeof(ebmacfilter_cmd_tail_src_ipv6) - strlen(ebmacfilter_cmd_tail_src_ipv6));
		
		strncpy(ebmacfilter_cmd_tail_dst_ipv6,	" -d ", sizeof(ebmacfilter_cmd_tail_dst_ipv6)-1);
		strncat(ebmacfilter_cmd_tail_dst_ipv6, mac_addr, sizeof(ebmacfilter_cmd_tail_dst_ipv6) - strlen(ebmacfilter_cmd_tail_dst_ipv6));
		strncat(ebmacfilter_cmd_tail_dst_ipv6, " -p IPv6 -j ", sizeof(ebmacfilter_cmd_tail_dst_ipv6) - strlen(ebmacfilter_cmd_tail_dst_ipv6));
		strncat(ebmacfilter_cmd_tail_dst_ipv6, target, sizeof(ebmacfilter_cmd_tail_dst_ipv6) - strlen(ebmacfilter_cmd_tail_dst_ipv6));
		strncat(ebmacfilter_cmd_tail_dst_ipv6, "\n", sizeof(ebmacfilter_cmd_tail_dst_ipv6) - strlen(ebmacfilter_cmd_tail_dst_ipv6));

	}
	if (interface != NULL && interface[0] != '\0') 
	{
		if (!strncmp(interface, "LAN", if_len)) 
		{
			strncpy(interface, "br0", if_len-1);
		}
		snprintf(ipmacfilter_cmd, sizeof(ipmacfilter_cmd), "%s%s%s%s", ipmacfilter_cmd_head, " -i ", interface, ipmacfilter_cmd_tail);
		snprintf(ebmacfilter_cmd_ipv4, sizeof(ebmacfilter_cmd_ipv4), "%s%s", ebmacfilter_cmd_head, ebmacfilter_cmd_tail_ipv4);
		snprintf(ebmacfilter_cmd_ipv6, sizeof(ebmacfilter_cmd_ipv6), "%s%s", ebmacfilter_cmd_head, ebmacfilter_cmd_tail_ipv6);
		snprintf(ebmacfilter_cmd_src_ipv4, sizeof(ebmacfilter_cmd_src_ipv4), "%s%s", ebmacfwfilter_cmd_head, ebmacfilter_cmd_tail_src_ipv4);
		snprintf(ebmacfilter_cmd_dst_ipv4, sizeof(ebmacfilter_cmd_dst_ipv4), "%s%s", ebmacfwfilter_cmd_head, ebmacfilter_cmd_tail_dst_ipv4);
		snprintf(ebmacfilter_cmd_src_ipv6, sizeof(ebmacfilter_cmd_src_ipv6), "%s%s", ebmacfwfilter_cmd_head, ebmacfilter_cmd_tail_src_ipv6);
		snprintf(ebmacfilter_cmd_dst_ipv6, sizeof(ebmacfilter_cmd_dst_ipv6), "%s%s", ebmacfwfilter_cmd_head, ebmacfilter_cmd_tail_dst_ipv6);

	}

	fputs_escape(ipmacfilter_cmd, fp);
	fputs_escape(ebmacfilter_cmd_ipv4, fp);
	fputs_escape(ebmacfilter_cmd_ipv6, fp);
	fputs_escape(ebmacfilter_cmd_src_ipv4, fp);
	fputs_escape(ebmacfilter_cmd_dst_ipv4, fp);
	fputs_escape(ebmacfilter_cmd_src_ipv6, fp);
	fputs_escape(ebmacfilter_cmd_dst_ipv6, fp);


	return 0;
}

int svc_wan_related_ipmacfilter_write(void)
{
	int i=0,j=0, res = 0;
	char element_name[64]={0};
	char interface[16]={0};
	char protocol[8] = {0};
	char protocol_temp[8] = {0};
	char src_port[8] = {0};
	char des_port[8] = {0};
	char ipmacfilter_cmd_head[48]={0};
	char target[8] = {0}, target_mac[8]={0}, target_ipup[8] = {0}, target_ipdown[8] = {0};
	char direction[12] = {0};
	char interface_tmp[16] = {0};
	char list_type_mac[8] = {0};
	char list_type_ipup[8] = {0};
	char list_type_ipdown[8] = {0};
	char value[8] = {0};
	char ipv4filter_cmd_head[48]={0};
	char target_ipv4down[8] = {0},target_ipv4up[8] = {0},target_ipv4[8] = {0};
	char cmdBuf[64]={0};
	char tmp[8] = {0};
	char buf[64]={0};
	cfg_node_type_t *node = NULL;
	FILE *fp = NULL, *fp_mac = NULL, *fp_ipup = NULL, *fp_ipdown = NULL;
	char active[6] = {0};
	char mac_on_off[8] = {0};
	char ipup_on_off[8] = {0};
	char ipdown_on_off[8] = {0};
	char del_only[8] = {0};
	/* mac_ip = 0(mac filter) 1(ip uplink filter) 2(ip down link filter) */
	int mac_ip = -1, start_index = 0, end_index = 0, g_ip_mac_flag = 0;
	char g_ip_mac_flag1[2] = {0};
	int ConRet = -1;
	char count[8] = {0};
	int ret = -1;
	char cleanHW[32] = {0};

	unlink(MACFILTER_SH);
	unlink(IPUPFILTER_SH);
	unlink(IPDOWNFILTER_SH);
	
	cfg_get_object_attr(GLOBALSTATE_NODE, "g_ip_mac_flag", g_ip_mac_flag1, sizeof(g_ip_mac_flag1));
	g_ip_mac_flag = atoi(g_ip_mac_flag1);
	tcdbg_printf("[%s]%d: g_ip_mac_flag = %d\n", __FUNCTION__, __LINE__, g_ip_mac_flag);
	node = cfg_type_query(IPMACFILTER_ETHERTYPEFILTER_NODE);
	if (node != NULL)
	{
		for(i = 0; i < 4 + MAX_DUALBAND_BSSID_NUM; i++)
		{
			snprintf(buf,sizeof(buf),"P%d",i);
			memset(tmp,0,sizeof(tmp));
			cfg_obj_get_object_attr(IPMACFILTER_ETHERTYPEFILTER_NODE, buf, 0, tmp, sizeof(tmp));
			j = i;
#if !defined(TCSUPPORT_MULTI_USER_ITF)
			/* ignore USB and WDS */
			if(i >= 8)
				j = j + 2 ;
#endif
			sprintf(cmdBuf, "echo %d %s > /proc/tc3162/ethertypefilter_array\n", j, tmp);
			system_escape(cmdBuf);
		}	
		system("/userfs/bin/hw_nat -!");
	}

	/*node = IpMacFilter_common*/
	if (cfg_query_object(IPMACFILTER_COMMON_NODE,NULL,NULL) <= 0)
	{
		printf("Error--IpMacFilter value is NULL.\n");
		return FAIL;
	}

	/* not reboot */
	if (g_ip_mac_flag == 1) 
	{
		mac_ip = getfiltertype();
		if(mac_ip < 0)
			return FAIL;
	}

	/* when reboot, set mac filter, ip uplink filter and ipdown filter all */
	if (g_ip_mac_flag == 0) 
	{
		mac_ip = 0;
		goto ip_mac_handle;

next_ip_mac_handle:
		mac_ip++;
		if (mac_ip < 3)
			goto ip_mac_handle;

		g_ip_mac_flag = 1;
		snprintf(g_ip_mac_flag1, sizeof(g_ip_mac_flag1), "%d", g_ip_mac_flag);
		cfg_set_object_attr(GLOBALSTATE_NODE, "g_ip_mac_flag", g_ip_mac_flag1);		
		if(fp_mac)
		{
			fclose(fp_mac);
			fp_mac = NULL;
		}
		return SUCCESS;
	}

ip_mac_handle:	
	if (mac_ip == 0) 
	{
		/* mac list type */
		memset(list_type_mac, 0, sizeof(list_type_mac));
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeMac", 0, list_type_mac, sizeof(list_type_mac));
		if (list_type_mac[0] != '\0') 
		{
			if (!strcasecmp(list_type_mac, "black")) 
			{
				strncpy(target_mac, "DROP", sizeof(target_mac) - 1);
			}
			else if (!strcasecmp(list_type_mac, "white")) 
			{
				strncpy(target_mac, "RETURN", sizeof(target_mac)-1);
			}
			else 
			{
				if (g_ip_mac_flag == 0) 
				{
					goto next_ip_mac_handle;
				}
				else 
				{
					return FAIL;
				}
			}
		}
		else 
		{
			if (g_ip_mac_flag == 0) 
			{
				goto next_ip_mac_handle;
			}
			else 
			{
				return FAIL;
			}
		}
		start_index = 0;
		end_index = MAX_MAC_FILTER_RULE;
	}
	else if (mac_ip == 1) 
	{
		/* port uplink list type */
		memset(list_type_ipup, 0, sizeof(list_type_ipup));
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpUp", 0, list_type_ipup, sizeof(list_type_ipup));
		if (list_type_ipup[0] != '\0') 
		{
			if (!strcasecmp(list_type_ipup, "black")) 
			{
				strncpy(target_ipup, "DROP", sizeof(target_ipup)-1);
				strncpy(target_ipv4up, "DROP", sizeof(target_ipv4up)-1);
			}
			else if (!strcasecmp(list_type_ipup, "white")) 
			{
				strncpy(target_ipup, "RETURN", sizeof(target_ipup)-1);
				strncpy(target_ipv4up, "ACCEPT", sizeof(target_ipv4up)-1);
			}
			else 
			{
				if (g_ip_mac_flag == 0) 
				{
					goto next_ip_mac_handle;
				}
				else 
				{
					return FAIL;
				}
			}
		}
		else 
		{
			if (g_ip_mac_flag == 0) 
			{
				goto next_ip_mac_handle;
			}
			else 
			{
				return FAIL;
			}
		}
		start_index = MAX_MAC_FILTER_RULE;
		end_index = MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE;
	}
	else if (mac_ip == 2) 
	{
		/* port down link list type */
		memset(list_type_ipdown, 0, sizeof(list_type_ipdown));
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeIpDown", 0, list_type_ipdown, sizeof(list_type_ipdown));
		if (list_type_ipdown[0] != '\0') 
		{
			if (!strcasecmp(list_type_ipdown, "black")) 
			{
				strncpy(target_ipdown, "DROP", sizeof(target_ipdown)-1);
				strncpy(target_ipv4down, "DROP", sizeof(target_ipv4down)-1);
			}
			else if (!strcasecmp(list_type_ipdown, "white")) 
			{
				strncpy(target_ipdown, "RETURN", sizeof(target_ipdown)-1);
				strncpy(target_ipv4down, "ACCEPT", sizeof(target_ipv4down)-1);
			}
			else 
			{
				if (g_ip_mac_flag == 0) 
				{
					goto next_ip_mac_handle;
				}
				else 
				{
					return FAIL;
				}
			}
		}
		else 
		{
			if (g_ip_mac_flag == 0) 
			{
				goto next_ip_mac_handle;
			}
			else 
			{
				return FAIL;
			}
		}
		start_index = MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE;
		end_index = MAX_IPMACFILTER_RULE;
	}
	else 
	{
		printf("Error mac or ip type info is wrong.\n");
		if (g_ip_mac_flag == 0) 
		{
			goto next_ip_mac_handle;
		}
		else 
		{
			return FAIL;
		}
	}
	
#if defined(TCSUPPORT_CT_E8GUI)
	if (mac_ip == 0) 
	{
		memset(mac_on_off, 0, sizeof(mac_on_off));
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ActiveMac", 0, mac_on_off, sizeof(mac_on_off));
		unlink(MACFILTER_SH);
	}
	else if (mac_ip == 1) 
	{
		memset(ipup_on_off, 0, sizeof(ipup_on_off));
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ActivePortOut", 0, ipup_on_off, sizeof(ipup_on_off));
		unlink(IPUPFILTER_SH);		
	}
	else if (mac_ip == 2) 
	{
		memset(ipdown_on_off, 0, sizeof(ipdown_on_off));
		cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "ActivePortIn", 0, ipdown_on_off, sizeof(ipdown_on_off));
		unlink(IPDOWNFILTER_SH);
	}
	DeleteFilterIndex();

	/*when you deletes device on web macfilter, web only display delete and it does not take effect.*/
	cfg_obj_get_object_attr(IPMACFILTER_COMMON_NODE, "DelOnly", 0, del_only, sizeof(del_only));

	if(!strncmp(del_only, "Yes", sizeof(del_only)))
	{
	  cfg_obj_set_object_attr(IPMACFILTER_COMMON_NODE, "DelOnly", 0, "No");
	  return SUCCESS;
	}
	
#endif
	if (mac_ip == 0) 
	{
		/* for mac filter */
		fp_mac = fopen(MACFILTER_SH, "w+");
		if (fp_mac == NULL) 
		{
			printf("Error--mac file pointer is NULL.\n");
			if (g_ip_mac_flag == 0) 
			{
				goto next_ip_mac_handle;
			}
			else 
			{
				return FAIL;
			}
		}

		FlushFilter(fp_mac, mac_ip);
		if (mac_on_off[0] != '\0') 
		{
			if (mac_on_off[0] != '1') 
			{
				fclose(fp_mac);
				fp_mac = NULL;
				res = chmod(MACFILTER_SH, 777);
				if (g_ip_mac_flag == 0) 
				{
					goto next_ip_mac_handle;
				}
				else 
				{
					return SUCCESS;
				}
			}
		}
	}
	else if (mac_ip == 1) 
	{
		/* ip up link */
		fp_ipup = fopen(IPUPFILTER_SH, "w+");
		if (fp_ipup == NULL)
		{
			printf("Error--ip uplink file pointer is NULL.\n");
			if (g_ip_mac_flag == 0) 
			{
				goto next_ip_mac_handle;
			}
			else 
			{
				return FAIL;
			}
		}
		
		FlushFilter(fp_ipup, mac_ip);

		if (ipup_on_off[0] != '\0') 
		{
			if (ipup_on_off[0] != '1') 
			{
				fclose(fp_ipup);
				res = chmod(IPUPFILTER_SH, 777);
				if (g_ip_mac_flag == 0) 
				{
					goto next_ip_mac_handle;
				}
				else 
				{
					return SUCCESS;
				}
			}
		}

	}
	else if (mac_ip == 2) 
	{
		/* ip down link */
		fp_ipdown = fopen(IPDOWNFILTER_SH, "w+");
		if (fp_ipdown == NULL) 
		{
			printf("Error--ip uplink file pointer is NULL.\n");
			if (g_ip_mac_flag == 0) 
			{
				goto next_ip_mac_handle;
			}
			else 
			{
				return FAIL;
			}
		}
		
		FlushFilter(fp_ipdown, mac_ip);
		if (ipdown_on_off[0] != '\0') 
		{
			if (ipdown_on_off[0] != '1') 
			{
				fclose(fp_ipdown);
				res = chmod(IPDOWNFILTER_SH, 777);
				if (g_ip_mac_flag == 0) 
				{
					goto next_ip_mac_handle;
				}
				else 
				{
					return SUCCESS;
				}
			}
		}
	}
	/*else 
	{
		if (g_ip_mac_flag == 0) 
		{
			goto next_ip_mac_handle;
		}
		else 
		{
			return FAIL;
		}
	}*/

	for (i = start_index; i < end_index ; i++) 
	{
		snprintf(element_name, sizeof(element_name), IPMACFILTER_ENTRY_N_NODE, i + 1);
		if (cfg_query_object(element_name,NULL,NULL) <= 0)
		{
			continue;
		}

		/* check active */
		memset(value, 0, sizeof(value));
		cfg_obj_get_object_attr(element_name, "Active", 0, value, sizeof(value));
		if(value[0] != '\0')
		{
			if(strcasecmp(value, "yes"))
			{
				printf("Error --Entry active is not yes.\n");
				continue;
			}
		}
		else
		{
			printf("Error--Entry Active value is NULL.\n");
			continue;
		}

		/* uplink is outgoing, downlink is incoming, but it is not important */
		memset(direction, 0, sizeof(direction));
		cfg_obj_get_object_attr(element_name, "Direction", 0, direction, sizeof(direction));
		if(direction[0] != '\0')
		{
			if(strcasecmp(direction, "Both") && strcasecmp(direction, "Incoming")
				&& strcasecmp(direction, "Outgoing"))
			{
				printf("Error --direction is not Both, Incoming and Outgoing.\n");
				continue;
			}
		}
		else
		{
			printf("Error--Entry direction value is NULL.\n");
			continue;
		}

		/* check interface */
		memset(interface,0,sizeof(interface));
		memset(interface_tmp, 0, sizeof(interface_tmp));
		cfg_obj_get_object_attr(element_name, "Interface", 0, interface_tmp, sizeof(interface_tmp));
#if defined(TCSUPPORT_CMCCV2)
		if(interface_tmp[0] == '\0' && mac_ip != 2)
		{
			printf("interface is null.\n");
			continue;
		}
#else
		if(interface_tmp[0] == '\0')
		{
			printf("interface is null.\n");
			continue;
		}
#endif
		strncpy(interface, interface_tmp, sizeof(interface)-1);
		/* interface value should be br0/nasx_y/pppx, check wan interface */
		if (interface[0] == 'n' || interface[0] == 'p') 
		{
			if(get_waninfo_by_name(interface, "Active", active) == SUCCESS)
			{
				/*if this interface is not activated, jump to next rule*/		
				if(strncmp(active, "Yes", sizeof(active)))
					continue;
			}
			else
			{
				continue;
			}
		}
		
		
		memset(protocol, 0, sizeof(protocol));
		cfg_obj_get_object_attr(element_name, "Protocol", 0, protocol, sizeof(protocol));

		memset(value, 0, sizeof(value));		
		cfg_obj_get_object_attr(element_name, "RuleType", 0, value, sizeof(value));

		if(value[0] != '\0')
		{
			if (!strcasecmp(value, "IP")) 
			{	
				/* check if uplink or down link */
				if (mac_ip == 1) 
				{
					strncpy(target, target_ipup, sizeof(target)-1);
					fp = fp_ipup;
					
					strncpy(ipmacfilter_cmd_head, "iptables -t filter -A ipupfilter_chain", sizeof(ipmacfilter_cmd_head)-1);
					strncpy(target_ipv4, target_ipv4up, sizeof(target_ipv4)-1);
					strncpy(ipv4filter_cmd_head, "ebtables -t filter -A ipv4upfilter", sizeof(ipv4filter_cmd_head)-1);
				}
				else if (mac_ip == 2) 
				{
					strncpy(target, target_ipdown, sizeof(target)-1);
					fp = fp_ipdown;
					strncpy(ipmacfilter_cmd_head, "iptables -t filter -A ipdownfilter_chain", sizeof(ipmacfilter_cmd_head)-1);
					
					strncpy(target_ipv4, target_ipv4down, sizeof(target_ipv4)-1);
					strncpy(ipv4filter_cmd_head, "ebtables -t filter -A ipv4downfilter", sizeof(ipv4filter_cmd_head)-1);
				}
				else 
				{
					/* impossible happen */
					continue;
				}

				if(strcasecmp(protocol, "All") == 0)
				{
					memset(des_port, 0, sizeof(des_port));
					cfg_obj_get_object_attr(element_name, "DesPort", 0, des_port, sizeof(des_port));

					memset(src_port, 0, sizeof(src_port));
					cfg_obj_get_object_attr(element_name, "SrcPort", 0, src_port, sizeof(src_port));
					if (src_port[0] != '\0' || des_port[0] != '\0') {
						strncpy(protocol_temp, "TCP/UDP", sizeof(protocol_temp));
				
					}else{
						strncpy(protocol_temp, "ALL", sizeof(protocol_temp));
					}
				}else{
					strncpy(protocol_temp, protocol, sizeof(protocol_temp));
				}

#if defined(TCSUPPORT_CMCCV2)
				if('\0' == interface_tmp[0])
				{
					ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
								target_ipv4, ipv4filter_cmd_head, element_name, "ppp+", protocol_temp);
					ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
								target_ipv4, ipv4filter_cmd_head, element_name, "nas+", protocol_temp);
				}
				else
#endif
				if(strcmp(interface, "br0")== 0)
				{					
				ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
									target_ipv4, ipv4filter_cmd_head, element_name, "eth0.+", protocol_temp);

						ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
									target_ipv4, ipv4filter_cmd_head, element_name, "ra+", protocol_temp);
					
						ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
									target_ipv4, ipv4filter_cmd_head, element_name, "rai+", protocol_temp);

						ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
									target_ipv4, ipv4filter_cmd_head, element_name, "br0", protocol_temp);
				}else{
					ConRet = AssembleIpFilterFile(fp, target, ipmacfilter_cmd_head, 
						target_ipv4, ipv4filter_cmd_head, element_name, interface,protocol_temp);
				}
				
				if(ConRet < 0)
					continue;
			}
			else if(!strcasecmp(value, "MAC"))
			{
				strncpy(target, target_mac, sizeof(target)-1);
				fp = fp_mac;

				/*Assemble File for MacFilter Command*/
				ConRet = AssembleMacFilterFile(fp, target, element_name, interface, sizeof(interface));
				
				if(ConRet < 0)
					continue;
			}
			else
			{
				/*not valid RuleType value, jump to next rule*/
				continue;
			}
		}
		else
		{
			printf("Error--Entry RuleType value is NULL.\n");
			continue;
		}
	}
	ret = blapi_system_get_clearHWNAT_str(cleanHW,32);
	if (mac_ip == 0) 
	{
		/* for mac filter */
		if (mac_on_off[0] != '\0' && mac_on_off[0] == '1' && list_type_mac[0] != '\0') 
		{
			if(!strcasecmp(list_type_mac, "white"))
			{
				memset(count,0,sizeof(count));
				cfg_get_object_attr(IPMACFILTER_NODE, "mac_num", count, sizeof(count));
				if( count[0] && strcmp(count, "0") )
				{
					/*deny all connections which not in white list*/
					fputs("iptables -A macfilter_chain -i br0 -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv4 -i eth0.+ -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv4 -o eth0.+ -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv6 -i eth0.+ -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv6 -o eth0.+ -j DROP\n", fp_mac);
#if defined(TCSUPPORT_WLAN)
					fputs("ebtables -A ebmacfwfilter -p IPv4 -i ra+ -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv4 -o ra+ -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv6 -i ra+ -j DROP\n", fp_mac);
					fputs("ebtables -A ebmacfwfilter -p IPv6 -o ra+ -j DROP\n", fp_mac);

#endif

				}
			}
		}
		fputs(cleanHW, fp_mac);
		fclose(fp_mac);
		fp_mac = NULL;
		res = chmod(MACFILTER_SH, 777);
	}
	else if (mac_ip == 1) 
	{
		/* for ip uplink filter */
		if (ipup_on_off[0] != '\0' && ipup_on_off[0] == '1' && list_type_ipup[0] != '\0') 
		{
			if(!strcasecmp(list_type_ipup, "white"))
			{
				memset(count,0,sizeof(count));
				cfg_get_object_attr(IPMACFILTER_NODE, "ipup_num", count, sizeof(count));
				if( count[0] && strcmp(count, "0") )
				{
					/*deny all connections which not in white list*/
					fputs("iptables -A ipupfilter_chain -i br0 -j DROP\n", fp_ipup);
					fputs("ebtables -A ipv4upfilter -p IPv4 -i eth0.+ -j DROP\n", fp_ipup);
				}
			}
		}
		fputs(cleanHW, fp_ipup); /* clean all hwnat rules.*/
		fclose(fp_ipup);
		res = chmod(IPUPFILTER_SH, 777);
	}
	else if (mac_ip == 2) 
	{
		/* for ip down link filter */
		if (ipdown_on_off[0] != '\0' && ipdown_on_off[0] == '1' && list_type_ipdown[0] != '\0') 
		{
			if(!strcasecmp(list_type_ipdown, "white"))
			{
				memset(count,0,sizeof(count));
				cfg_get_object_attr(IPMACFILTER_NODE, "ipdown_num", count, sizeof(count));
				if( count[0] && strcmp(count, "0") )
				{
					/*deny all connections which not in white list*/
					fputs("iptables -A ipdownfilter_chain -i nas+ -j DROP\n", fp_ipdown);
					fputs("iptables -A ipdownfilter_chain -i ppp+ -j DROP\n", fp_ipdown);
					fputs("ebtables -A ipv4downfilter -p IPv4 -o eth0.+ -j DROP\n", fp_ipdown);
				}
			}
		}
		fputs(cleanHW, fp_ipdown); /* clean all hwnat rules.*/
		fclose(fp_ipdown);
		res = chmod(IPDOWNFILTER_SH, 777);
	}
	if (g_ip_mac_flag == 0) {
		goto next_ip_mac_handle;
	}
	else {
		return SUCCESS;
	}
}

int svc_cfg_boot_ipmacfilter(void)
{
	svc_wan_related_ipmacfilter_write();
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_IPMACFILTER_BOOT, NULL, 0); 
	return 0;
}

int cfg_type_ipmacfilter_entry_commit(char* path)
{
	wan_related_evt_t param;
	
	svc_wan_related_ipmacfilter_write();
#if defined(TCSUPPORT_CWMP_TR181)
	updateFireWallChByNodeFlag(IPMACFILTER_FLAG);
#endif
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_IPMACFILTER_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
}

int cfg_type_ipmacfilter_EtherTypeFilter_commit(char* path)
{
		char tmp[16] = {0}, buf[16] = {0} ,cmdBuf[128] = {0};
		int  i = 0,j =  0 ;
		
		for(i = 0; i < 4 + MAX_DUALBAND_BSSID_NUM; i++){

			snprintf(buf,sizeof(buf),"P%d",i);
			memset(tmp,0,sizeof(tmp));
			cfg_obj_get_object_attr(IPMACFILTER_ETHERTYPEFILTER_NODE, buf, 0, tmp, sizeof(tmp));
			j = i;
#if !defined(TCSUPPORT_MULTI_USER_ITF)
			/* ignore USB and WDS */
			if(i >= 8)
				j = j + 2 ;
#endif
			sprintf(cmdBuf, "echo %d %s > /proc/tc3162/ethertypefilter_array\n", j, tmp);
			system_escape(cmdBuf);
		}
		system("/userfs/bin/hw_nat -!");

	return 0;
}

static int cfg_type_ipmacfilter_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
char ipmac_attribute[][32]=
{
	{"MacName"},
	{"MacAddr"},
	{"Active"},
	{"Interface"},
	{"IPName"},
	{"Protocol"},
	{"SrcIPAddr"},
	{"SrcIPMask"},
	{"SrcPort"},
	{"DesIPAddr"},
	{"DesIPMask"},
	{"DesPort"},
#if defined(TCSUPPORT_CMCC)
	{"SrcIPEndAddr"},
	{"DesIPEndAddr"},
	{"SrcPortEnd"},
	{"DesPortEnd"},
#endif
	{""},
};

	char ipmacNodeName[64] = {0};
	int static_ip_index = 0;
	char addnum[4] = {0};
	char count[4] = {0};
	int norecord = 0;
	int	i, start_index, end_index, ip_flag = -1;
	int	j, s_attr = 2, e_attr = 12;
	char cipmac[50] = {0};
	int res = 0;

	FILE *fp = NULL;
	char p[50] = {0};
	if(!strcmp(attr,"mac_num") || !strcmp(attr,"add_macnum"))
	{
		ip_flag = 0;
		start_index = 0;
		end_index = MAX_MAC_FILTER_RULE;
		s_attr = 0; e_attr = 4;
	}
	else if(!strcmp(attr,"ipup_num") || !strcmp(attr,"add_ipupnum"))
	{
		ip_flag = 1;
		start_index = MAX_MAC_FILTER_RULE;
		end_index = MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE;
#if defined(TCSUPPORT_CMCC)
		e_attr = 16;
#endif
	}
	else if(!strcmp(attr,"ipdown_num") || !strcmp(attr,"add_ipdownnum"))
	{
		ip_flag = 2;
		start_index = MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE;
		end_index = MAX_IPMACFILTER_RULE;
#if defined(TCSUPPORT_CMCC)
		e_attr = 16;
#endif

	}
	else
	{
		return cfg_type_default_func_get(path,attr,val,len);
	}
	
	if (cfg_query_object(IPMACFILTER_NODE,NULL,NULL) <= 0)
	{
		printf("\r\n%s: can't find Node\n", __FUNCTION__);
		return FAIL;
	}
	printf("[%s]%d: flag = %d\n", __FUNCTION__, __LINE__, ip_flag);
	if(0 == ip_flag)
		fp= fopen(TMP_MAC_FILE, "w");
	else if(1 == ip_flag)
		fp= fopen(TMP_IPUP_FILE, "w");
	else if(2 == ip_flag)
		fp= fopen(TMP_IPDOWN_FILE, "w");
		
	if(fp== NULL)
	{
		printf("%s:Error--IpMacFilter's temp file is NULL.\n", __FUNCTION__);
		return FAIL;
	}
	
	memset(ipmacNodeName, 0, sizeof(ipmacNodeName));
	for(i=start_index; i<end_index; i++){
		snprintf(ipmacNodeName, sizeof(ipmacNodeName), IPMACFILTER_ENTRY_N_NODE, i + 1);
		if (cfg_query_object(ipmacNodeName,NULL,NULL) <= 0)
		{
			/*1.find the entry index which can be used*/
			if(!norecord)
			{
				snprintf(addnum, sizeof(addnum), "%d", i);
				if(0 == ip_flag)
					cfg_set_object_attr(IPMACFILTER_NODE, "add_macnum", addnum);
				else if(1 == ip_flag)
					cfg_set_object_attr(IPMACFILTER_NODE, "add_ipupnum", addnum);
				else if(2 == ip_flag)
					cfg_set_object_attr(IPMACFILTER_NODE, "add_ipdownnum", addnum);
				norecord++;
			}
		}
		else
		{
			memset(count,0x00, sizeof(count));
			snprintf(count, sizeof(count), "%d\n", i);
			fputs(count, fp);
			/*2.get ipmac value and save in file*/
			for(j= s_attr; j<e_attr; j++){
				memset(cipmac, 0x00, sizeof(cipmac));
				memset(p, 0, sizeof(p));
				cfg_obj_get_object_attr(ipmacNodeName, ipmac_attribute[j], 0, p , sizeof(p));
				if(p[0] != '\0')
				{
					strncpy(cipmac, p, sizeof(cipmac) - 1);
					strncat(cipmac, "\n", sizeof(cipmac) - strlen(cipmac) - 1);
					fputs_escape(cipmac, fp);	
				}
				else
				{
					fputs("\n", fp);
				}
			}
			static_ip_index++;
		}
	}
	
	fclose(fp);
	fp = NULL;
	memset(count,0x00, sizeof(count));
	snprintf(count, sizeof(count), "%d", static_ip_index);
	if(0 == ip_flag)
	{
		cfg_set_object_attr(IPMACFILTER_NODE, "mac_num", count);
		res = chmod(TMP_MAC_FILE, 777);
	}
	else if(1 == ip_flag)
	{
		cfg_set_object_attr(IPMACFILTER_NODE, "ipup_num", count);
		res = chmod(TMP_IPUP_FILE, 777);
	}
	else if(2 == ip_flag)
	{
		cfg_set_object_attr(IPMACFILTER_NODE, "ipdown_num", count);
		res = chmod(TMP_IPDOWN_FILE, 777);
	}

	 return cfg_type_default_func_get(path,attr,val,len); 
} 

#if defined(TCSUPPORT_CWMP_TR181)
int deleteFirewallChNodeByIpmacfilterNode(char*path)
{
	int entryIdx= -1;
	char nodeName[MAXLEN_NODE_NAME] ={0};
		
	if(path == NULL || get_entry_number_cfg2(path, "entry.", &entryIdx) != 0)
	{
		return -1;
	}
	
	if(entryIdx > 0 && entryIdx < MAX_MAC_FILTER_RULE )
	{
		snprintf(nodeName, sizeof(nodeName), TR181_FIREWALLCH_PVC_ENTRY_NODE, MAC_FILTER_RULE_FLAG, entryIdx);	
	}
	else if(entryIdx > MAX_MAC_FILTER_RULE && entryIdx < MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE)
	{
		snprintf(nodeName, sizeof(nodeName), TR181_FIREWALLCH_PVC_ENTRY_NODE, IP_UP_FILTER_RULE_FLAG, entryIdx - MAX_MAC_FILTER_RULE);	
	}
	else if(entryIdx > (MAX_MAC_FILTER_RULE + MAX_IPUP_FILTER_RULE) && entryIdx < MAX_IPMACFILTER_RULE)
	{
		snprintf(nodeName, sizeof(nodeName), TR181_FIREWALLCH_PVC_ENTRY_NODE, IP_DOWN_FILTER_RULE_FLAG, entryIdx - MAX_MAC_FILTER_RULE - MAX_IPUP_FILTER_RULE);
	}

	cfg_obj_delete_object(nodeName);	

	return 0;	
}
#endif

int cfg_type_ipmacfilter_entry_func_delete(char* path)
{
#if defined(TCSUPPORT_CWMP_TR181)
	deleteFirewallChNodeByIpmacfilterNode(path);
#endif
	return cfg_obj_delete_object(path);
}

static char* cfg_type_ipmacfilter_index[] = { 
	 "ipfilter_id", 
	 NULL 
}; 

static cfg_node_ops_t cfg_type_ipmacfilter_entry_ops  = { 
	 .get = cfg_type_ipmacfilter_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_ipmacfilter_entry_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipmacfilter_entry_commit 
}; 


static cfg_node_type_t cfg_type_ipmacfilter_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_MAX_IPMACFILTER_ENTRY_NUM, 
	 .parent = &cfg_type_ipmacfilter, 
	 .index = cfg_type_ipmacfilter_index, 
	 .ops = &cfg_type_ipmacfilter_entry_ops, 
}; 


static int cfg_type_ipmacfilter_common_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_ipmacfilter_entry_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_ipmacfilter_common_ops  = { 
	 .get = cfg_type_ipmacfilter_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipmacfilter_entry_commit 
}; 

static cfg_node_ops_t cfg_type_ipmacfilter_EtherTypeFilter_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipmacfilter_EtherTypeFilter_commit 
}; 


static cfg_node_type_t cfg_type_ipmacfilter_EtherTypeFilter = { 
	 .name = "EtherTypeFilter", 
	 .flag = 1, 
	 .parent = &cfg_type_ipmacfilter, 
	 .ops = &cfg_type_ipmacfilter_EtherTypeFilter_ops, 
}; 



static cfg_node_type_t cfg_type_ipmacfilter_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_ipmacfilter, 
	 .ops = &cfg_type_ipmacfilter_common_ops, 
}; 


static cfg_node_ops_t cfg_type_ipmacfilter_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_ipmacfilter_entry_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_ipmacfilter_entry_commit 
}; 


static cfg_node_type_t* cfg_type_ipmacfilter_child[] = { 
	 &cfg_type_ipmacfilter_entry, 
	 &cfg_type_ipmacfilter_common,
	 &cfg_type_ipmacfilter_EtherTypeFilter,
	 NULL 
}; 


cfg_node_type_t cfg_type_ipmacfilter = { 
	 .name = "IpMacFilter", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_ipmacfilter_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_ipmacfilter_child, 
	 .ops = &cfg_type_ipmacfilter_ops, 
}; 
