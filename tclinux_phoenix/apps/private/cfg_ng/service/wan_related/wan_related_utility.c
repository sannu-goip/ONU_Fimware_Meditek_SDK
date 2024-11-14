

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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <linux/version.h>

#include <svchost_api.h> 
#include <cfg_api.h>
#include <regex.h>
#include "utility.h" 
#include "wan_related_mgr.h"

#define USE_DEFAULT_IFNAME_FORMAT 1
#define FLUSH_DEFAULT_CHAIN "/usr/script/filter_forward_stop.sh"
#define LOAD_DEFAULT_CHAIN "/usr/script/filter_forward_start.sh"

void check_and_set_l7filter(unsigned int new_filter_state)
{
	char nodePath[64] = {0};
	char newFilterState[32] = {0};
	char vername[CFG_BUF_16_LEN] = {0}, module_path[CFG_BUF_128_LEN] = {0};
	
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;

	if(( atoi(newFilterState) & L7_BIT_MASK) != 0)/*pre is on*/
	{
		if((new_filter_state & L7_BIT_MASK) == 0)/*now is down*/
		{
			fprintf(stderr,"========================rmmod l7filter=======================\n");
			system("iptables -t filter -F app_filter_chain");
			system("iptables -t filter -F url_filter_chain");
			system("rmmod xt_layer7");
		}
	}
	else/*now is down*/
	{
		if((new_filter_state & L7_BIT_MASK) != 0)/*now is down*/
		{
			fprintf(stderr,"========================insmod l7filter=======================\n");
			decideModulePath(vername, CFG_BUF_16_LEN);
			snprintf(module_path, sizeof(module_path), INSMOD_XT_LAYER7_CMD, vername);
			system(module_path);
		}
	}
}

#define isdigit(x)	((x)>='0'&&(x)<='9')

unsigned char checkName(char *Name){
	char c;

	if(Name == NULL)
		return 0;	/* Can't happen */
	
	while((c = *Name++) != '\0'){
		if((!isdigit(c)) && (c != '.') && (c != ':')){
			if(c == '/'){
				return 1;
			}
			return 0;
		}
	}
	return 1;

}

int resolve(char *ipstr, unsigned long *addr, unsigned short *netmask)
{
	unsigned long ipaddr = 0;
	int i, bits;
	unsigned long digit;
	char *index;
	short mask = -1;

	for (i = 0; i < 3; i++) {
		index = strchr(ipstr, '.');
		if (index == NULL) break;
		*index = '\0';
		digit = atoi(ipstr);
		*index = '.';
		if (digit > 255) return -1;
		ipaddr = (ipaddr << 8) | digit;
		ipstr = index + 1;
	}

	if(i == 0)
		return -1;
	index = strchr(ipstr, '/');     /* netmask ? */
	if (index != NULL) {
		mask = atoi(&index[1]);
		if (mask < 0 || mask > 32) return -1;
		*index = '\0';
	}

	digit = atoi(ipstr);
	bits = (4 - i) << 3;
	if (digit > ((1 << bits) - 1)) return -1;
	ipaddr = (ipaddr << bits) | digit;

	digit = ipaddr >> 28;		/* check multicast and reserved IP */
	if (digit >= 14) return -1;
	if (mask < 0) {
		if (digit < 8)
			mask = 8;
		else if (digit < 12)
			mask = 16;
		else if (digit < 14)
			mask = 24;
	}
	else
		*index = '/';

	*addr = htonl(ipaddr);
	if (netmask != NULL) *netmask = mask;
	return 0;
}

unsigned long resolveName(char *name)
{
	struct hostent *h = NULL;
	struct sockaddr_in addr = {0};
	unsigned long ipAddr = 0;

	if(name == NULL)
		return 0;
	printf("\r\nEnter reslove name: [%s]", name);
	/*add by brian*/
	if(checkName(name))
	{
		resolve((char *)name, &ipAddr, NULL);
		if(ipAddr != 0)
		{
			return ipAddr;
		}
	}

	h = gethostbyname(name);

	if(h != NULL)
	{
		memcpy(&addr.sin_addr, h->h_addr, sizeof(addr.sin_addr));
		printf("\r\nreslove name success: name [%s] IP is [%s]", name, inet_ntoa(addr.sin_addr));	
	}
	else
	{

		printf("\r\nresolve Name failed!");
	}

	return (unsigned long)addr.sin_addr.s_addr;
}

int parseUrl(const char *theUrlPtr, char *theHostNamePtr, unsigned int *thePortPtr) 
{
    unsigned short  theLength;
    int    theResult = 0;
    char  *theSourcePtr;
	unsigned long theHostAddressPtr;
	struct sockaddr_in addr = {0};

    *theHostNamePtr = '\0';
    *thePortPtr = 80;

	if ( theUrlPtr == NULL ) {
		return -1;
	}
	
#if defined(SSL) || defined(TCSUPPORT_CWMP_SSL)
	if( strncmp(theUrlPtr, "https", 5) == 0){ 
		*thePortPtr = 443;
	}
#endif

    if ( (theSourcePtr = strstr(theUrlPtr, ":")) != NULL ) {
		/*
		    The path is absolute, so skip over the "http://".
        */
        theSourcePtr += 3;
		theUrlPtr = theSourcePtr;
		
        theLength = 0;

        while (*theSourcePtr != ':' && *theSourcePtr != '/' &&
                *theSourcePtr != '\0') {
            theSourcePtr++;
            theLength++;
        }

        if (theLength < 64) {
            strncpy(theHostNamePtr, theUrlPtr, theLength);
            *(theHostNamePtr + theLength) = '\0';
        }
        else {
            theResult = -1;
        }

        /*
            If there was a port specified, convert it to numeric
            otherwise return the default port value.
        */
        if (*theSourcePtr == ':') {
            theSourcePtr += 1;
            *thePortPtr = atoi(theSourcePtr);
        }
    }
	else {
		theResult = -1;
	}

	theHostAddressPtr = resolveName(theHostNamePtr);
	//addr.sin_addr = theHostAddressPtr;
	memcpy(&addr.sin_addr, &theHostAddressPtr, sizeof(addr.sin_addr));
	strcpy(theHostNamePtr, inet_ntoa(/*theHostAddressPtr*/addr.sin_addr));
	
#ifdef QOS_DEBUG
	printf("The theHostNamePtr is %s\n", theHostNamePtr);
#endif

    return theResult;
}

int isValidDnsIp(char * dnsIP)
{
	if(!check_ip_format(dnsIP))/*dnsIp is invalid*/
	{		
		printf("isValidDnsIp:dnsIp is invalid!\r\n");
		return 0;	
	}

	if((strcmp(dnsIP,"255.255.255.255") == 0) || (strcmp(dnsIP,"0.0.0.0") == 0))
	{		
		printf("isValidDnsIp:dnsIp = %s is invalid!\r\n",dnsIP);
		return 0;
	}

	return 1;
}

#if defined(TCSUPPORT_CT_E8GUI)
int ipfilter_check_switch(void)
{
	char ipmacfilter_switch[8]={0};
	char port_up_filter[8] = {0}, port_down_filter[8] = {0};
	char nodeName[64] = {0};
	memset(nodeName,0,sizeof(nodeName));
	strncpy(nodeName, IPMACFILTER_COMMON_NODE, sizeof(nodeName)-1);
	cfg_get_object_attr(nodeName, "ActiveMac", ipmacfilter_switch, sizeof(ipmacfilter_switch));
	cfg_get_object_attr(nodeName, "ActivePortOut", port_up_filter, sizeof(port_up_filter));
	cfg_get_object_attr(nodeName, "ActivePortIn", port_down_filter, sizeof(port_down_filter));
	
	if(!strcmp(ipmacfilter_switch, "1") || 
	   !strcmp(port_up_filter, "1") ||
	   !strcmp(port_down_filter, "1"))
	{
		return 1;
	}
	
	return 0;
}
#endif

int ipfilter_check_active()
{
	int i=0;
	char ipmacfilter_rule_status[8]={0};
	char nodeName[64] = {0};

	memset(nodeName,0,sizeof(nodeName));
	/*for each rule in ipfilter function, if any active rule exists, return true*/
	for(i=0; i < 120; i++)
	{
		snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, i);		
		if(cfg_get_object_attr(nodeName, "Active", ipmacfilter_rule_status, sizeof(ipmacfilter_rule_status)) < 0)
		{
			continue;
		}
		if(!strcasecmp(ipmacfilter_rule_status, "yes"))
		{
			return 1;
		}
	}
	return 0;
}
