

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
#include <svchost_api.h> 
#include <cfg_api.h>
#include <cfg_cli.h>
#include <sys/fcntl.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include "cfg_msg.h"
#include "utility.h" 
#include "other_cfg_diagnostics.h"


#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
int getV6SlaacPrefix(char *outaddr)
{
	FILE *fp = NULL;
	char addr_all[64] = {0}, slaac_addr[64] = {0}, tmp[40] = {0}, prelen[12] = {0};

	if ( !outaddr )
		return -1;

	fp = fopen(PPPOE_EMULATOR_SLAACPREFIX_PATH, "r");
	if ( fp )
	{
		memset(addr_all, 0, sizeof(addr_all));
		memset(slaac_addr, 0, sizeof(slaac_addr));
		memset(tmp, 0, sizeof(tmp));
		memset(prelen, 0, sizeof(prelen));
		fgets(addr_all, sizeof(addr_all), fp);
		close(fp);

		if ( 0 != addr_all[0] && '\n' != addr_all[0] )
		{
			sscanf(addr_all, "%39s/%s", tmp, prelen);
			shrink_ipv6addr(tmp, slaac_addr, sizeof(slaac_addr));
			strcpy(outaddr, slaac_addr);
			return 0;
		}
	}

	return -1;
}

int saveWanV6Prefix(int mode, char *nodeName, char *wanip)
{
	struct sockaddr_in6 in_wan_v6addr = {0};
	int i = 0, prefixLEN = 64;
	char val[64] = {0};

	if ( !nodeName || !wanip )
		return -1;

	if ( 0 == mode ) /* dhcp mode */
	{
		if ( 1 != inet_pton(AF_INET6, wanip, &in_wan_v6addr.sin6_addr) )
		return -1;
		for ( i = prefixLEN/8; i < 16; i ++)
			in_wan_v6addr.sin6_addr.s6_addr[i] = 0;

		in_wan_v6addr.sin6_family = AF_INET6;
		getnameinfo((struct sockaddr *)&in_wan_v6addr,
					sizeof(struct sockaddr_in6),
					val,
					sizeof(val),
					NULL, 0, NI_NUMERICHOST);
	}
	else if ( 1 == mode ) /* slaac mode */
	{
		if ( 0 != getV6SlaacPrefix(val) )
			return -1;
	}
	else
		return -1;

	cfg_set_object_attr(nodeName, "WANIPv6Prefix", val);

	return 0;
}

int getV6geteway(char *outgw)
{
	FILE *fp = NULL;
	char gateway[64] = {0}, tmp[64] = {0};
	int res = 0;

	if ( !outgw )
		return -1;

	fp = fopen(PPPOE_EMULATOR_GWV6_PATH, "r");
	if ( fp )
	{
		memset(tmp, 0, sizeof(tmp));
		memset(gateway, 0, sizeof(gateway));
		res = fread(tmp, sizeof(char), 39, fp);
		close(fp);

		if ( 0 != tmp[0] && '\n' != tmp[0] )
		{
			shrink_ipv6addr(tmp, gateway, sizeof(gateway));
			strcpy(outgw, gateway);
			return 0;
		}
	}

	return -1;
}

int getV6SlaacAddr(char *outaddr)
{
	FILE *fp = NULL;
	char addr_all[64] = {0}, slaac_addr[64] = {0}, tmp[40] = {0}, prelen[12] = {0};

	if ( !outaddr )
		return -1;

	fp = fopen(PPPOE_EMULATOR_SLAACV6_PATH, "r");
	if ( fp )
	{
		memset(addr_all, 0, sizeof(addr_all));
		memset(slaac_addr, 0, sizeof(slaac_addr));
		memset(tmp, 0, sizeof(tmp));
		memset(prelen, 0, sizeof(prelen));
		fgets(addr_all, sizeof(addr_all), fp);
		close(fp);

		if ( 0 != addr_all[0] && '\n' != addr_all[0] )
		{
			sscanf(addr_all, "%39s %s", tmp, prelen);
			shrink_ipv6addr(tmp, slaac_addr, sizeof(slaac_addr));
			strcpy(outaddr, slaac_addr);
			return 0;
		}
	}

	return -1;
}

int checkRA_MFlags()
{
	FILE *fp = NULL;
	char temp[64] = {0};
	int ra_flags = 0;

	fp = fopen(PPPOE_EMULATOR_RAFLAGS_PATH, "r");
	if ( fp )
	{
		memset(temp, 0, sizeof(temp));
		fgets(temp, sizeof(temp), fp);
		close(fp);

		if ( 0 != temp[0] )
		{
			sscanf(temp, "%d", &ra_flags);
			if ( 0 != ra_flags )
				return ra_flags;
		}
	}

	return 0;
}


int startPPPoE_Emulator_timer()
{
	struct timespec pppemu_Start = {0};
	char tmpSec[64] = {0};
	char tmpNSec[64] = {0};

	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns1", "");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns2", "");
	memset(&pppemu_Start, 0, sizeof(pppemu_Start));
	clock_gettime(CLOCK_MONOTONIC, &pppemu_Start);
	snprintf(tmpSec, sizeof(tmpSec), "%ld", pppemu_Start.tv_sec);
	snprintf(tmpNSec, sizeof(tmpNSec), "%ld", pppemu_Start.tv_nsec);
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmu_startsec", tmpSec);
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmu_startnsec", tmpNSec);
	
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuDsliteOK", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmulatorStart", "1");

	return 0;
}

int stopPPPoE_Emulator_timer()
{
	char pppoeEmudnsroute_add[4] = {0};
	
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmulatorStart", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns1", "");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns2", "");
	unlink(PPPOE_EMULATOR_DOMAIN_FILE);

	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", 0, pppoeEmudnsroute_add, sizeof(pppoeEmudnsroute_add)) > 0){
		if( atoi(pppoeEmudnsroute_add) != 0 ){
			cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1");
		}
	}
		
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuDsliteOK", "0");

	return 0;
}

/*no used*/
int isPPPoE_Emulator_Start()
{
	int pppoeEmudnsroute_add = 0;
	char tmp[4] = {0};

	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", 0, tmp, sizeof(tmp)) > 0){
		pppoeEmudnsroute_add = atoi(tmp);
	}

	return pppoeEmudnsroute_add;
}

/*no used*/
int addEMUDNSConfig(FILE* fp, FILE* fpInfo)
{
	char  buf[256] = {0};
	char pppoeEmudns[2][64] = {0};

	if ( !fp || !fpInfo )
		return -1;

	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns1", 0, buf, sizeof(buf)) > 0)
	{
		strncpy(pppoeEmudns[0], buf, sizeof(pppoeEmudns[0])-1);
	}
	if ( 0 != pppoeEmudns[0][0] )
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "server=%s@%s\n", pppoeEmudns[0], PPPOE_EMULATOR_PPPNAME); 		
		fwrite(buf, sizeof(char), strlen(buf), fp);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%s %s %s\n", pppoeEmudns[0], PPPOE_EMULATOR_PPPNAME, "PPPEMU");
		fwrite(buf, sizeof(char), strlen(buf), fpInfo);
	}

	memset(buf, 0, sizeof(buf));
	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns2", 0, buf, sizeof(buf)) > 0)
	{
		strncpy(pppoeEmudns[1], buf, sizeof(pppoeEmudns[1])-1);
	}
	if ( 0 != pppoeEmudns[1][0] )
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "server=%s@%s\n", pppoeEmudns[1], PPPOE_EMULATOR_PPPNAME); 		
		fwrite(buf, sizeof(char), strlen(buf), fp);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%s %s %s\n", pppoeEmudns[1], PPPOE_EMULATOR_PPPNAME, "PPPEMU");
		fwrite(buf, sizeof(char), strlen(buf), fpInfo);

	}

	return 0;
}

int resolveDSLiteDomain(char *domain, char dns[][64], char *gw6, int gotanip6,  char *nodeName)
{
	char  cmdbuf[256] = {0};
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL;
	int retCode = -1;
	struct sockaddr_in6 *p_dslite_v6addr = NULL;
	char dslite_addr_buf[64] = {0};
	char pppoeEmuDsliteOK[4] = {0};
	char pppoeEmudnsroute_add[4] = {0};

	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuDsliteOK", 0, pppoeEmuDsliteOK, sizeof(pppoeEmuDsliteOK)) <= 0)
	{
		return -1;
	}
	if ( atoi(pppoeEmuDsliteOK) != 0 )
			return 0;

	if ( !domain || !gw6 || !nodeName )
		return retCode;

	if ( 0 == domain[0] || 0 == dns[0][0]
		|| 0 == gw6[0] || 0 == gotanip6 )
		return retCode;

	memset(cmdbuf, 0, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "%s %s\n", domain, PPPOE_EMULATOR_PPPNAME);
	doValPut(PPPOE_EMULATOR_DOMAIN_FILE, cmdbuf);

	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", 0, pppoeEmudnsroute_add, sizeof(pppoeEmudnsroute_add)) <= 0)
	{
		return -1;
	}	
	if ( atoi(pppoeEmudnsroute_add) == 0 )
	{
		cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", "1");
		if ( 0 == dns[1][0] )
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), PPPOE_EMULATOR_DNSROUTE_ADD, dns[0], gw6);
			system_escape(cmdbuf);
			cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns1", dns[0]);
		}
		else
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), PPPOE_EMULATOR_DNSROUTE_ADD, dns[0], gw6);
			system_escape(cmdbuf);
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), PPPOE_EMULATOR_DNSROUTE_ADD, dns[1], gw6);
			system_escape(cmdbuf);

			cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns1", dns[0]);
			cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns2", dns[1]);
		}

		cfg_set_object_attr(GLOBALSTATE_NODE, "dnsmaskRestart", "1");
	}

	/* resolve dns */
	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	if ( tc_getaddrinfo(domain, NULL, &hints, &res, 3) )
	{
		retCode = -1;
	}
	else
	{
		if ( NULL == res || AF_INET6 != res->ai_family )
			retCode = -1;
		else
		{
			p_dslite_v6addr = (struct sockaddr_in6 *)res->ai_addr;
			if ( NULL == p_dslite_v6addr ){
				freeaddrinfo(res);
				return -1;
			}
			inet_ntop(AF_INET6, &p_dslite_v6addr->sin6_addr, dslite_addr_buf, INET6_ADDRSTRLEN);
			cfg_set_object_attr(nodeName, "AftrAddress", dslite_addr_buf);
			cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuDsliteOK", "1");
			retCode = 0;
		}
	}
	if(res)
		freeaddrinfo(res);
	return retCode;
}

int checkPPPoE_Emulator_timer()
{
	char buf[128] = {0}, ipmode[12] = {0}, dhcpv6mode[12] = {0}, wan_mode[32] = {0};
	char info_string[2][MAX_INFO_SIZE];
	char path[128] = {0}, nas_name[64] = {0};
	int all_done = 1, ra_flags = 0, got_wanip6 = 0;
	char dns_svr6[2][64] = {0}, gw6_addr[64] = {0};
	struct timespec pppemu_Current = {0}, pppemu_diff = {0};
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
	char wan_ip[64] = {0};
#endif
	int pppoeEmulatorStart = 0;
	int pppoeEmuIfaceDel = 0;
	int pppoeEmuIfaceDel_timercnt = 0;
	int pppoeEmuDsliteOK = 0;
	char tmp[64] = {0};
	long tmpSec = 0;
	long tmpNSec = 0;
	
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmulatorStart", 0, tmp, sizeof(tmp)) > 0)
	{
		pppoeEmulatorStart = atoi(tmp);
	}
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel", 0, tmp, sizeof(tmp)) > 0)
	{
		pppoeEmuIfaceDel = atoi(tmp);
	}
	memset(tmp, 0, sizeof(tmp));
	if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel_timercnt", 0, tmp, sizeof(tmp)) > 0)
	{
		pppoeEmuIfaceDel_timercnt = atoi(tmp);
	}

	if ( pppoeEmuIfaceDel && 0 == pppoeEmulatorStart
		&& pppoeEmuIfaceDel_timercnt++ >= 1 )
	{
		cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel", "0");
		cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel_timercnt", "0");

		cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "NASName", 0, nas_name, sizeof(nas_name));

		/* remove interface */
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "/sbin/ifconfig %s down", nas_name);
		system_escape(buf);
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "/usr/bin/smuxctl rem %s", nas_name);
		system_escape(buf);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "/bin/rm -r %s", PPPOE_EMULATOR_PPPIF_PATH);
		system(buf);
	}
	else
	{
		memset(tmp, 0, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", pppoeEmuIfaceDel_timercnt);
		cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel_timercnt", tmp);
	}

	if ( pppoeEmulatorStart )
	{
		memset(&pppemu_Current, 0, sizeof(pppemu_Current));
		clock_gettime(CLOCK_MONOTONIC, &pppemu_Current);
		memset(&pppemu_diff, 0, sizeof(pppemu_diff));
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmu_startsec", 0, tmp, sizeof(tmp)) > 0)
		{
			tmpSec = strtol(tmp, NULL, 10);
		}
		memset(tmp, 0, sizeof(tmp));
		if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmu_startnsec", 0, tmp, sizeof(tmp)) > 0)
		{
			tmpNSec = strtol(tmp, NULL, 10);
		}
		pppemu_diff.tv_sec = pppemu_Current.tv_sec - tmpSec;
		pppemu_diff.tv_nsec += pppemu_Current.tv_nsec - tmpNSec;
		while ( pppemu_diff.tv_nsec > 1000000000 )
		{
			pppemu_diff.tv_sec++;
			pppemu_diff.tv_nsec -= 1000000000;
		}
		if ( pppemu_diff.tv_nsec < 0 )
		{
			pppemu_diff.tv_sec--;
			pppemu_diff.tv_nsec += 1000000000;
		}

		if ( pppemu_diff.tv_sec >= EMULATOR_TIMEOUT )
		{
			stopPPPoE_Emulator_timer();

			memset(buf, 0, sizeof(buf));
			if ( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", 0, buf, sizeof(buf))
				&& 0 == strcmp(buf, "unknown") )
			{
				if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANPPPConnectionIPMode", 0, ipmode, sizeof(ipmode))
					&& ( 0 == strcmp(ipmode, "2")
					|| 0 == strcmp(ipmode, "3") ))
				{
					cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", ERROR_OUT_OF_RESOURCES);
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
					memset(buf, 0, sizeof(buf));
					if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Dslite_Enable", 0, buf, sizeof(buf))
						&& 0 == strcmp(buf, "0")
						&& 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ExternalIPAddress", 0, wan_ip, sizeof(wan_ip))
						&& 0 == strcmp(wan_ip, "0.0.0.0") )
					{
						cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", ERROR_PARAMNEGOFAILV4);
					}
#endif
				}
				else
				{
					cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", "Timeout");
				}
			}

			cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DiagnosticsState", "Complete");
			cfg_commit_object(PPPOE_EMULATOR_ENTRY_NODE);
			return 0;
		}

		/* only check for IPv6 in 30s */
		memset(ipmode, 0, sizeof(ipmode));
		memset(wan_mode, 0, sizeof(wan_mode));
		memset(dhcpv6mode, 0, sizeof(dhcpv6mode));
		if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANPPPConnectionIPMode", 0, ipmode, sizeof(ipmode))
			&& 0 != ipmode[0] )
		{
			if ( 0 == strcmp(ipmode, "2") /* IPv6 */
				|| 0 == strcmp(ipmode, "3") ) /* IPv4/IPv6 */
			{
				if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WanMode", 0, wan_mode, sizeof(wan_mode))
					&& 0 == strcmp(wan_mode, "Route") )
				{
					/* get IPv6 info from file */
					if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DHCPv6", 0, dhcpv6mode, sizeof(dhcpv6mode))
						&& 0 == strcmp(dhcpv6mode, "Yes") )
					{
						snprintf(path, sizeof(path), "%s%s", PPPOE_EMULATOR_PPPIF_PATH, "ip6");
						memset(info_string, 0, sizeof(info_string));
						if ( SUCCESS == get_file_string(path, info_string, 1) )
						{
							got_wanip6 = 1;
							cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", info_string[0]);
							if ( 0 != saveWanV6Prefix(0, PPPOE_EMULATOR_ENTRY_NODE, info_string[0]) )
								all_done = 0;
						}
						else
							all_done = 0;
					}
					else
					{
						memset(info_string, 0, sizeof(info_string));
						if ( 0 == getV6SlaacAddr(info_string[0]) )
						{
							got_wanip6 = 1;
							cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", info_string[0]);
							if ( 0 != saveWanV6Prefix(1, PPPOE_EMULATOR_ENTRY_NODE, info_string[0]) )
								all_done = 0;
						}
						else
							all_done = 0;
					}
				}
				else /* bridge mode */
				{
					ra_flags = checkRA_MFlags();
					if ( 0 == ra_flags )
						all_done = 0;
					else
					{
						/* get IPv6 info from file */
						if( ra_flags & PPPOE_EMULATOR_MFLAG )
						{
							snprintf(path, sizeof(path), "%s%s", PPPOE_EMULATOR_PPPIF_PATH, "ip6");
							memset(info_string, 0, sizeof(info_string));
							if ( SUCCESS == get_file_string(path, info_string, 1) )
							{
								got_wanip6= 1; 
								cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", info_string[0]);
								if ( 0 != saveWanV6Prefix(0, PPPOE_EMULATOR_ENTRY_NODE, info_string[0]) )
									all_done = 0;
							}
							else
								all_done = 0;
						}
						else
						{
							memset(info_string, 0, sizeof(info_string));
							if ( 0 == getV6SlaacAddr(info_string[0]) )
							{
								got_wanip6 = 1;
								cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", info_string[0]);
								if ( 0 != saveWanV6Prefix(1, PPPOE_EMULATOR_ENTRY_NODE, info_string[0]) )
									all_done = 0;	
							}
							else
								all_done = 0;
						}
					}
				}

				snprintf(path, sizeof(path), "%s%s", PPPOE_EMULATOR_PPPIF_PATH, "pd6");
				memset(info_string, 0, sizeof(info_string));
				if ( SUCCESS == get_file_string(path, info_string, 1) )
				{
					sscanf(info_string[0], "%[^/]", buf);
					cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "LANIPv6Prefix", buf);
				}
				else
					all_done = 0;

				snprintf(path, sizeof(path), "%s%s", PPPOE_EMULATOR_PPPIF_PATH, "dns6");
				memset(info_string, 0, sizeof(info_string));
				if ( SUCCESS == get_file_string(path, info_string, 2) )
				{
					memset(dns_svr6, 0, sizeof(dns_svr6));
					if ( 0 != info_string[1][0] )
					{
						snprintf(buf, sizeof(buf), "%s,%s", info_string[0], info_string[1]);
						cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6DNSServers", buf);
						snprintf(dns_svr6[0], sizeof(dns_svr6[0]) - 1, "%s", info_string[0]);
						snprintf(dns_svr6[1], sizeof(dns_svr6[1]) - 1, "%s", info_string[1]);
					}
					else
					{
						cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6DNSServers", info_string[0]);
						snprintf(dns_svr6[0], sizeof(dns_svr6[0]) - 1, "%s", info_string[0]);
					}
				}
				else
					all_done = 0;
				
				memset(info_string, 0, sizeof(info_string));
				if ( 0 == getV6geteway( info_string[0]) )
				{
					cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANDefaultIPv6Gateway", info_string[0]);
					snprintf(gw6_addr, sizeof(gw6_addr) - 1, "%s", info_string[0]);
				}
				else
					all_done = 0;

				memset(buf, 0, sizeof(buf));
				if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Dslite_Enable", 0, buf, sizeof(buf))
					&& 0 == strcmp(buf, "1") )
				{
					snprintf(path, sizeof(path), "%s%s", PPPOE_EMULATOR_PPPIF_PATH, "dsliteaddr");
					memset(info_string, 0, sizeof(info_string));
					if ( SUCCESS == get_file_string(path, info_string, 1) )
						cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "AftrAddress", info_string[0]);
					else
					{
						snprintf(path, sizeof(path), "%s%s", PPPOE_EMULATOR_PPPIF_PATH, "dslitename");
						memset(info_string, 0, sizeof(info_string));
						if ( SUCCESS == get_file_string(path, info_string, 1) )
						{
							memset(tmp, 0, sizeof(tmp));
							if(cfg_obj_get_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuDsliteOK", 0, tmp, sizeof(tmp)) > 0)
							{
								pppoeEmuDsliteOK = atoi(tmp);
							}
							if ( 0 == pppoeEmuDsliteOK )
								cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "AftrAddress", info_string[0]);
							if ( 0 != resolveDSLiteDomain(info_string[0], dns_svr6, gw6_addr, got_wanip6, PPPOE_EMULATOR_ENTRY_NODE) )
								all_done = 0;
						}
						else
							all_done = 0;
					}
				}

				if ( all_done )
				{
					if ( 0 == strcmp(ipmode, "2") /* ipv6 */
						|| (0 == strcmp(ipmode, "3") /* ipv4/ipv6 */
						&& 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", 0, buf, sizeof(buf))
						&& 0 == strcmp(buf, ERROR_OUT_OF_RESOURCES)) )
					{
						stopPPPoE_Emulator_timer();
						cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DiagnosticsState", "Complete");
						cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", "Success");
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
						memset(buf, 0, sizeof(buf));
						memset(wan_ip, 0, sizeof(wan_ip));
						if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Dslite_Enable", 0, buf, sizeof(buf))
							&& 0 == strcmp(buf, "0")
							&& 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ExternalIPAddress", 0, wan_ip, sizeof(wan_ip))
							&& 0 == strcmp(wan_ip, "0.0.0.0") )
						{
							cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", ERROR_PARAMNEGOFAILV4);
						}
#endif
						cfg_commit_object(PPPOE_EMULATOR_ENTRY_NODE);
					}
				}
			}
		}
	}

	return 0;
}

int create_pppoe_dhcp6c_conf()
{
	FILE *fp = NULL;
	char buf[64] = {0};
	int DSLite = 0;

	if( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Dslite_Enable", 0, buf, sizeof(buf))
		&& 0 == strcmp(buf, "1") )
		DSLite = 1;

	memset(buf, 0, sizeof(buf));
	fp = fopen(PPPOE_EMULATOR_PPPDHCP6C_CONF, "w");
	if ( fp )
	{
		snprintf(buf, sizeof(buf), "interface %s {\n", PPPOE_EMULATOR_PPPNAME);
		fputs(buf, fp);
		
		fputs("\tsend ia-pd 213;\n", fp);
		fputs("\tsend ia-na 210;\n", fp);
		fputs("\trequest domain-name;\n\trequest domain-name-servers;\n\tscript \""PPPOE_EMULATOR_PPPDHCP6C_SCRIPT"\";\n", fp);
		
		if( DSLite )
			fputs("\trequest dslite-name;\n", fp);
		
		fputs("};\n", fp);
		fputs("id-assoc pd 213 {\n\tprefix-interface "PPPOE_EMULATOR_PPPNAME"\n\t{\n\t\tsla-len 0;\n\t};\n};\n", fp);
		fputs("id-assoc na 210 { };\n", fp);
	
		fclose(fp);
	}

	return 0;
}

int pppoeKillviafile(char *file)
{
	FILE *fp = NULL;
	char pid_tmp[50] = {0};
	int pid = 0;

	if ( !file )
		return -1;

	fp = fopen(file, "r");
	if ( fp )
	{
		fgets(pid_tmp, sizeof(pid_tmp), fp);
		close(fp);
		unlink(file);

		pid = atoi(pid_tmp);
		if( 0 != pid
			&& 0 != kill(pid, SIGTERM) )
		{
			if ( 0 !=  kill(pid, SIGKILL) )
				tcdbg_printf("\n%s kill old pppoe client failed(%s)!\n", __FUNCTION__, file);
		}
	}

	return 0;
}

#endif


void pppoe_Emulator_stop(int reason)
{
#if !defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
	FILE *fp = NULL;
	char pid_tmp[50] = {0};
	int pid = 0;
#endif
	char cmdbuf[128] = {0};
	char nas_name[16] = {0};
	char result[64] = {0};

#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
	if( 0 > cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, PPPOE_EMULATOR_ATTR_NASNAME, 0, nas_name, sizeof(nas_name))
		|| 0 == nas_name[0] )
		strncpy(nas_name, PPPOE_EMULATOR_NAS_NAME, sizeof(nas_name)-1);
	stopPPPoE_Emulator_timer();

	pppoeKillviafile(PPPOE_EMULATOR_PPPDHCP6C_PATH);
	pppoeKillviafile(PPPOE_EMULATOR_PID_PATH);
	sleep(1);

	if ( PPPOE_EMULATOR_USER_STOP == reason
		|| PPPOE_EMULATOR_COMPLETE == reason )
	{
		cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel", "1");
		cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel_timercnt", "0");
	}
	else if ( PPPOE_EMULATOR_UNKNOW_STOP == reason )
	{
		/* remove interface*/
		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig %s down", nas_name);
		system_escape(cmdbuf);
		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/smuxctl rem %s", nas_name);
		system_escape(cmdbuf);

		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "/bin/rm -r %s", PPPOE_EMULATOR_PPPIF_PATH);
		system(cmdbuf);
	}
#else
	fp = fopen(PPPOE_EMULATOR_PID_PATH, "r");
	if(fp)
	{
		/*kill the ping process*/
		fgets(pid_tmp, sizeof(pid_tmp), fp);
		pid = atoi(pid_tmp);
		if(pid != 0)
		{ 
			if(cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", 0, result, sizeof(result)) > 0)
			{
				if(0 == strcmp(result, "Success"))
				{
					memset(cmdbuf, 0, sizeof(cmdbuf));
					snprintf(cmdbuf, sizeof(cmdbuf), "kill -SIGTERM %d", pid);
					system(cmdbuf);

					goto CloseFile;
				}
				else
				{
					goto KillPPP;
					
				}
			}
			else
			{
				goto KillPPP;
			}
		
		}

KillPPP:
		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "kill -9 %d", pid);
	   	system(cmdbuf);

		if(cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "NASName", 0, nas_name, sizeof(nas_name)) > 0)
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/sbin/ifconfig %s down", nas_name);
			system_escape(cmdbuf);

			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/smuxctl rem %s", nas_name);
			system_escape(cmdbuf);
		}
		else
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			system("/sbin/ifconfig emulator down");
			system("/usr/bin/smuxctl rem emulator");
			
			tcdbg_printf("\npppoe_simulate_stop, get NASName error!!!");
		}

CloseFile:
		/*delete the pid file*/
		fclose(fp);
		unlink(PPPOE_EMULATOR_PID_PATH);
	}
#endif		
	if(reason == PPPOE_EMULATOR_COMPLETE){
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
		if ( 0 < cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result", 0, result, sizeof(result))
			&& 0 == strcmp(result, "UserAuthenticationFail") )
		{
			cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", "::");
			cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANDefaultIPv6Gateway", "::");
		}
#endif	
		return;
	}
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE,"PPPSessionID","");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE,"ExternalIPAddress","");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE,"DefaultGateway","");
	if(reason == PPPOE_EMULATOR_USER_STOP){
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE,"DiagnosticsState","None");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE,"Result","UserStop");
	}
	if(reason == PPPOE_EMULATOR_UNKNOW_STOP){
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE,"Result","unknown");
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ConnectionError", "");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ExternalIPAddress", "0.0.0.0");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DefaultGateway", "0.0.0.0");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "AftrAddress", "::");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", "::");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6DNSServers", "::");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6Prefix", "::");
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANDefaultIPv6Gateway", "::");
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
		cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ConnectionErrorCode", "");
#endif
#endif
	}
}


int svc_other_handle_event_pppoeemulator_write()
{
	char fpath[64] = {0};

	/*create pppoe_sim.conf*/
	snprintf(fpath, sizeof(fpath), "%s", PPPOE_EMULATOR_CONF);

	if(cfg_query_object(PPPOE_EMULATOR_ENTRY_NODE, NULL, NULL) > 0){
		if(cfg_save_attrs_to_file(PPPOE_EMULATOR_ENTRY_NODE, fpath, QMARKS) != 0){
			tcdbg_printf("\npppoe_simulate_write write PPPoESimulate_Entry error!");
			return -1;
		}
	}

	return 0;
}

int svc_other_handle_event_pppoeemulator_execute()
{
	char diag_state[16] = {0};
	 
	if(cfg_obj_get_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DiagnosticsState", 0, diag_state, sizeof(diag_state)) > 0){
		if(0 == strcasecmp(diag_state,"Running")){
			tcdbg_printf("\nPPPoE Emulator Start");
			pppoe_Emulator_stop(PPPOE_EMULATOR_UNKNOW_STOP);
			cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DiagnosticsState", "Running");
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
			if ( 0 != mkdir(PPPOE_EMULATOR_PPPIF_PATH, 0666) )
				tcdbg_printf("\n%s warning create folder failed!\n", __FUNCTION__);
			create_pppoe_dhcp6c_conf();
#endif
			system(PPPOE_EMULATOR_SH);
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
			startPPPoE_Emulator_timer();
#endif
			return 0;
		}
		else if(0 == strcasecmp(diag_state,"Stop")){
			tcdbg_printf("\nPPPoE Emulator Stop");
			pppoe_Emulator_stop(PPPOE_EMULATOR_USER_STOP);
			return 0;
		}else if(0 == strcasecmp(diag_state,"Complete")){
			tcdbg_printf("\nPPPoE Emulator Complete");
			pppoe_Emulator_stop(PPPOE_EMULATOR_COMPLETE);
			cfg_set_object_attr(CWMP_ENTRY_NODE, "pppoe_emulator_finish_flag", "1");
			
			cfg_commit_object(CWMP_NODE);
			return 0;
		}
		else{
			tcdbg_printf("\nPPPoE Emulator DiagnosticsState Other State");
			return 0;
		}
	}
	else{
		tcdbg_printf("\nPPPoE Simulate get DiagnosticsState Fail!");
		return -1;
	}

	return 0;
}


int svc_other_handle_event_pppoeemulator_update()
{
	svc_other_handle_event_pppoeemulator_write();
	svc_other_handle_event_pppoeemulator_execute();
	return 0;
}


void svc_other_handle_event_pppoeemulator_init()
{
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmulatorStart", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuIfaceDel_timercnt", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudnsroute_add", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmuDsliteOK", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns1", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmudns2", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmu_startsec", "0");
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoeEmu_startnsec", "0");
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoe_emu_boot", "0");
#endif
}

int svc_other_handle_event_pppoeemulator_boot(void)
{
	int res = 0;

	svc_other_handle_event_pppoeemulator_init();
	
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DiagnosticsState","None");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Result","unknown");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "PPPSessionID","");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ExternalIPAddress","");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DefaultGateway","");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "NASName","emulator");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "NAS_IF","pon");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "PPPUnit","199");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "PPPAuthenticationProtocol", "");
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Username", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Password", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANInterface", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "PPPAuthenticationProtocol", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "RetryTimes", "2");
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "RetryTimes", "1");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ConnectionErrorCode", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "fakeRetryTimes", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "fakePPPAuthenticationProtocol", "");
#endif
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ConnectionError", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "PPPSessionID", "");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "ExternalIPAddress", "0.0.0.0");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "DefaultGateway", "0.0.0.0");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANPPPConnectionIPMode", "0");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "Dslite_Enable", "0");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "AftrAddress", "::");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6IPAddress", "::");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6DNSServers", "::");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANIPv6Prefix", "::");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "WANDefaultIPv6Gateway", "::");
	cfg_set_object_attr(PPPOE_EMULATOR_ENTRY_NODE, "LANIPv6Prefix", "::");
	res = chmod(PPPOE_EMULATOR_SH, 777); 
	res = chmod(PPPOE_EMULATOR_PPPV6UP_PRE_SCRIPT, 777); 
	res = chmod(PPPOE_EMULATOR_PPPV6UP_SCRIPT, 777); 
	res = chmod(PPPOE_EMULATOR_PPPDHCP6C_SCRIPT, 777); 
	cfg_set_object_attr(GLOBALSTATE_PPPOEEMUTIMER_NODE, "pppoe_emu_boot", "1");
#endif

	svc_other_handle_event_pppoeemulator_write();

	return 0;
}
#endif


#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
static int ipoeKillviafile(char *file)
{
	FILE *fp = NULL;
	char pid_tmp[50] = {0};
	int pid = 0;

	if ( !file )
		return -1;

	fp = fopen(file, "r");
	if ( fp )
	{
		fgets(pid_tmp, sizeof(pid_tmp), fp);
		close(fp);
		unlink(file);

		pid = atoi(pid_tmp);
		if( 0 != pid
			&& 0 != kill(pid, SIGTERM) )
		{
			if ( 0 !=  kill(pid, SIGKILL) )
			tcdbg_printf("\n%s kill old ipoe client failed(%s)!\n", __FUNCTION__, file);
		}
	}

	return 0;
}

static int reset_ipoeEmulator_statistic(void)
{
	char nodeName[32] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), IPOE_EMULATOR_ENTRY_NODE);

	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_LADDR, "0.0.0.0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DEFGW, "0.0.0.0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_SUCC_COUNT, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_FAIL_COUNT, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_AVRTIME, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_MINTIME, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_MAXTIME, "0");

	return 0;
}

static int get_pingResult(void)
{
	FILE *fp = NULL;
	char nodeName[32] = {0}, tmpbuf[64] = {0};
	int sent = 0, responses = 0, minrtt = 0, avrrtt = 0, maxrtt= 0;
	int res = 0;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), IPOE_EMULATOR_ENTRY_NODE);
	fp = fopen(IPOE_EMULATOR_PING_RESULT, "r");
	if(fp != NULL)
	{
		res = fscanf(fp,"%d,%d,%d,%d,%d", &sent, &responses, &minrtt, &avrrtt, &maxrtt);
		fclose(fp);
	}
	unlink(IPOE_EMULATOR_PING_RESULT);

	if ( 0 == responses )
	{
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_RESULT, IPOE_EMULATOR_RESULT_PINGFAIL);
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", responses);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_SUCC_COUNT, tmpbuf);
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", sent - responses);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_FAIL_COUNT, tmpbuf);
	}
	else
	{
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", responses);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_SUCC_COUNT, tmpbuf);
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", sent - responses);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_FAIL_COUNT, tmpbuf);
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", avrrtt);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_AVRTIME, tmpbuf);
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", minrtt);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_MINTIME, tmpbuf);
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", maxrtt);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_MAXTIME, tmpbuf);

		if(sent == responses)
		{
			cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_RESULT, IPOE_EMULATOR_RESULT_SUCC);
		}
		else if(sent > responses) 
		{
			cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_RESULT, IPOE_EMULATOR_RESULT_SOMEPINGFAIL);
		}
	}

	return 0;
}

static int get_dhcpResult(void)
{
	char nodeName[32]= {0}, tmpbuf[64] = {0};
	char info_string[2][MAX_INFO_LENGTH];
	char path[128] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), IPOE_EMULATOR_ENTRY_NODE);
	snprintf(path, sizeof(path), "%s%s", IPOE_EMULATOR_PATH, IPOE_EMULATOR_ATTR_LADDR);
	memset(info_string, 0, sizeof(info_string));
	if ( 0 == get_file_string(path, info_string, 1) )
	{
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_LADDR, info_string[0]);
	}

	sprintf(path, "%s%s", IPOE_EMULATOR_PATH, IPOE_EMULATOR_ATTR_DEFGW);
	memset(info_string, 0, sizeof(info_string));
	if ( 0 == get_file_string(path, info_string, 1) )
	{
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DEFGW, info_string[0]);
	}

	return 0;
}

static int ipoeEmulator_stop(int method)
{
	char nodeName[32]= {0};
	char nas_name[16] = {0}, cmdbuf[128] = {0}, localaddr[64] = {0};
	int pid = 0;
	char info_string[2][MAX_INFO_LENGTH];
	char path[128] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), IPOE_EMULATOR_ENTRY_NODE);
	memset(nas_name, 0, sizeof(nas_name));
	if( 0 >= cfg_get_object_attr(nodeName, IPOE_EMULATOR_ATTR_NASNAME, nas_name, sizeof(nas_name))
		|| 0 == nas_name[0] )
	{
		snprintf(nas_name, sizeof(nas_name), IPOE_EMULATOR_NAS_NAME);
	}
	/* kill ping */
	if(IPOE_EMULATOR_STOP_OTHER == method)
	{
		ipoeKillviafile(IPOE_EMULATOR_PING_PID_PATH);
	}
	/* kill client */
	ipoeKillviafile(IPOE_EMULATOR_PID_PATH);
	sleep(1);

	/* remove policy route */
	memset(cmdbuf, 0, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/iptables -t mangle -D OUTPUT -o %s -j MARK --set-mark 0x7e0000/0x7f0000", nas_name);
	system_escape(cmdbuf);

	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s%s", IPOE_EMULATOR_PATH, IPOE_EMULATOR_ATTR_LADDR);
	memset(info_string, 0, sizeof(info_string));
	memset(localaddr, 0, sizeof(localaddr));
	if ( SUCCESS == get_file_string(path, info_string, 1)
		&& '\0' != info_string[0][0] )
	{
		sprintf(cmdbuf, "/usr/bin/ip rule del from %s/32 table %d", info_string[0], IPOE_EMULATOR_POLICY_TABLEID);
		system(cmdbuf);
	}
	else if( 0 > cfg_get_object_attr(nodeName, IPOE_EMULATOR_ATTR_LADDR, localaddr, sizeof(localaddr))
		&& 0 != localaddr[0]
		&& 0 != strcmp(localaddr, "0.0.0.0") )
	{
		sprintf(cmdbuf, "/usr/bin/ip rule del from %s/32 table %d", localaddr, IPOE_EMULATOR_POLICY_TABLEID);
		system(cmdbuf);
	}

	sprintf(cmdbuf, "/usr/bin/ip rule del fwmark 0x7e0000/0x7f0000 table %d", IPOE_EMULATOR_POLICY_TABLEID);
	system(cmdbuf);	
	sprintf(cmdbuf, "/usr/bin/ip route flush table %d", IPOE_EMULATOR_POLICY_TABLEID);
	system(cmdbuf);
	
	/* remove interface */
	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "/sbin/ifconfig %s down", nas_name);
	system_escape(cmdbuf);
	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "/usr/bin/smuxctl rem %s", nas_name);
	system_escape(cmdbuf);
	if ( IPOE_EMULATOR_STOP_OTHER == method )
	{
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DIGSTATE, IPOE_EMULATOR_ST_NONE);
		cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_RESULT, IPOE_EMULATOR_RESULT_OTHER);
	}

	return 0;
}


int svc_other_handle_event_ipoe_emulator_boot(void)
{
	char nodeName[32] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), IPOE_EMULATOR_ENTRY_NODE);

	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DIGSTATE, IPOE_EMULATOR_ST_NONE);
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_WANIFACE, "");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_USERMAC, "");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_VENDORID, "");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_RESULT, IPOE_EMULATOR_RESULT_OTHER);
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_LADDR, "0.0.0.0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DEFGW, "0.0.0.0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_PINGDSTIP, "");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_PINGNUM, "5");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_TIMEOUT, "2000");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_SUCC_COUNT, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_FAIL_COUNT, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_AVRTIME, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_MINTIME, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_REP_MAXTIME, "0");
	cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_NASNAME, IPOE_EMULATOR_NAS_NAME);
	cfg_set_object_attr(nodeName, "NAS_IF", "pon");

	reset_ipoeEmulator_statistic();

	return 0;
}

int svc_other_handle_event_ipoe_emulator_write(void)
{
	char fpath[64] = {0};
	int res = 0;

	if(cfg_query_object(IPOE_EMULATOR_ENTRY_NODE, NULL, NULL) > 0)
	{
		memset(fpath, 0, sizeof(fpath));
		snprintf(fpath, sizeof(fpath), "%s", IPOE_EMULATOR_CONF);
		if(cfg_save_attrs_to_file(IPOE_EMULATOR_ENTRY_NODE, fpath, QMARKS) != 0)
		{
			tcdbg_printf("\n emulator_write write IPOE_EMULATOR_ENTRY_NODE error!");
			return -1;
		}
	}

	res = chmod(IPOE_EMULATOR_START, 777);
	res = chmod(IPOE_EMULATOR_DHCP_SH, 777);

	return 0;
}

int svc_other_handle_event_ipoe_emulator_execute(void)
{
	char nodeName[32] = {0};
	char cwmpNodeName[32] = {0};
	char diag_state[16] = {0}, vlanmode[16] = {0};
#define IPOE_EMULATOR_OK 16
	char info_string[2][MAX_INFO_LENGTH];
	char path[128] = {0}, cmdbuf[128] = {0};
	int rm_folder = 0;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), IPOE_EMULATOR_ENTRY_NODE);	
	memset(cwmpNodeName, 0, sizeof(cwmpNodeName));
	snprintf(cwmpNodeName, sizeof(cwmpNodeName), CWMP_ENTRY_NODE);
	
	memset(diag_state, 0, sizeof(diag_state));
	if( 0 < cfg_get_object_attr(nodeName, IPOE_EMULATOR_ATTR_DIGSTATE, diag_state, sizeof(diag_state)) )
	{
		if ( 0 == strcmp(IPOE_EMULATOR_ST_START, diag_state) )
		{
			/* check old process first and stop it. */
			ipoeEmulator_stop(IPOE_EMULATOR_STOP_OTHER);
			/* remove dir */
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/bin/rm -r %s", IPOE_EMULATOR_PATH);
			system(cmdbuf);
			/* start it */
			reset_ipoeEmulator_statistic();
			cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DIGSTATE, IPOE_EMULATOR_ST_RUNNING);
			/* create dir */
			if ( 0 != mkdir(IPOE_EMULATOR_PATH, 0666) )
			{
				tcdbg_printf("\n%s warning create folder failed!\n", __FUNCTION__);
				return -1;
			}
			/* check vlan mode */
			memset(vlanmode, 0, sizeof(vlanmode));
			if( 0 < cfg_get_object_attr(nodeName, "VLANMode", vlanmode, sizeof(vlanmode))
				&& 0 != vlanmode[0] )
				system(IPOE_EMULATOR_START);
			else
				cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DIGSTATE, IPOE_EMULATOR_ST_COMPLETE);
		}
		else if ( 0 == strcmp(IPOE_EMULATOR_ST_STOP, diag_state) )
		{
			ipoeEmulator_stop(IPOE_EMULATOR_STOP_SUCC);
			rm_folder = 1;
		}
		else if ( 0 == strcmp(IPOE_EMULATOR_ST_RUNNING, diag_state) )
		{
			/* get state from file */
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "%s%s", IPOE_EMULATOR_PATH, IPOE_EMULATOR_ATTR_DIGSTATE);
			memset(info_string, 0, sizeof(info_string));
			if ( SUCCESS == get_file_string(path, info_string, 1)
 				&& 0 == strcmp(IPOE_EMULATOR_ST_COMPLETE, info_string[0]) ) /* Complete */
 			{
				/* get result */
				snprintf(path, sizeof(path), "%s%s", IPOE_EMULATOR_PATH, IPOE_EMULATOR_ATTR_RESULT);
				memset(info_string, 0, sizeof(info_string));
				if ( SUCCESS != get_file_string(path, info_string, 1) )
					strcpy(info_string[0], IPOE_EMULATOR_RESULT_OTHER);

				if ( 0 == strcmp("DHCPOK", info_string[0]) )
				{
					/* get dhcp result */
					get_dhcpResult();
					/* get ping result */
					get_pingResult();
				}
				else
					cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_RESULT, info_string[0]);

				/* stop it */
				ipoeEmulator_stop(IPOE_EMULATOR_STOP_SUCC);
#ifdef CWMP
				signalCwmpInfo(IPOE_EMULATOR_OK, 0, 0);
#endif
				cfg_set_object_attr(nodeName, IPOE_EMULATOR_ATTR_DIGSTATE, IPOE_EMULATOR_ST_COMPLETE);

				rm_folder = 1;
			}
		}

		if ( rm_folder )
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/bin/rm -r %s", IPOE_EMULATOR_PATH);
			system(cmdbuf);
		}
	}
	else
	{
		tcdbg_printf("\n%s get DiagnosticsState failed\n", __FUNCTION__);
		return FAIL;
	}

	return SUCCESS;	
}

int svc_other_handle_event_ipoe_emulator_update(void)
{
	svc_other_handle_event_ipoe_emulator_write();
	svc_other_handle_event_ipoe_emulator_execute();
	return 0;
}

#endif

