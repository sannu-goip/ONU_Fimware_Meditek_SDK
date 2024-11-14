

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
#include "other_mgr.h"
#include "other_snmp_cfg.h"
#include <crypt.h>
#include <unistd.h>
#include <sys/stat.h>
#include "utility.h"
#include <linux/version.h>
#include <ctype.h>

int updateString(char* file,const char* keyword, char *newStr)
{
	FILE* in_file = NULL;
	FILE* out_file = NULL;
	char line[MAXSIZE];
	in_file = fopen(file, "r");
	if(in_file == NULL){
	  fprintf(stderr, OPEN_FAIL_MSG, file, strerror(errno));
	  return FAIL;
	}

	out_file = fopen(TMP_CONF, "w");
	if(out_file == NULL){
	  fclose(in_file);
	  fprintf(stderr, OPEN_FAIL_MSG, TMP_CONF, strerror(errno));
	  return FAIL;
	}

	while(fgets(line, MAXSIZE, in_file)){
	  if((strstr(line, keyword) != NULL)
				 && strlen(keyword) != 0
				   && strlen(newStr) != 0){
		  strcpy(line, newStr);
		  keyword = keyword + SHIFT_INDEX;
		  newStr = newStr + SHIFT_INDEX;
	  }
	  fputs(line, out_file);
	}

	fclose(in_file);
	fflush(out_file);
	fclose(out_file);
	unlink(file);

	if(rename(TMP_CONF, file) < 0){
	  fprintf(stderr, RENAME_FAIL_MSG,strerror(errno));
	  unlink(TMP_CONF);
	  return FAIL;
	}
	/*unuse function. marked by shnwind */
	/*unlink(TMP_CONF);*/
	return SUCCESS;
}/*end updateString*/


 int svc_other_handle_event_snmpd_write(char* path)
{
	int i=0, tlen = 0;
	char snmp_info[5][ATTR_SIZE];
	char *pSnmp_info=NULL;
	char tmpActive[32] = {0};
	char nodePath[64] = {0};
	char snmpd_keyword[][ATTR_SIZE]=
	{	
		/*Notice: dont change the elemnet sequence of snmpd_ketword. The rwcommunity has to write prior to rocommunity*/
		{"rwcommunity "},
		{"rocommunity "},
#if defined(TCSUPPORT_SNMP_FULL)
		{"trapsink "}, /* for snmp v1 trap */
		{"trap2sink "}, /* for snmp v2 trap */
#endif
		{""},
	};
	char keyword_for_firewall[][256]=
	{
		/*krammer add for firewall*/
		{"sysLocation"},
		{"sysContact"},
		{"sysName"},
		{""},
	};
	enum snmp_keyword_en
	{
		RW_Community=0,
		RO_Community,
#if defined(TCSUPPORT_SNMP_FULL)
		Trap_ManaIP, /* for snmp v1 trap */
		Trap2_ManaIP, /* for snmp v2 trap */
#endif
		snmp_keyword_maxNum,
	};
#if defined(TCSUPPORT_IPV6_SNMP)
	enum snmp_ipv6_keyword
	{
		agent_address=0,
		rw_community6,
		ro_community6,
		snmp_ipv6_keyword_max,
	};
	char keyword_for_ipv6[snmp_ipv6_keyword_max+1][SNMP_ARRAY_SIZE]=
	{
		{"agentAddress "},
		{"rwcommunity6 "},
		{"rocommunity6 "},
		{""},
	};
	char snmp_ipv6_info[snmp_ipv6_keyword_max][SNMP_ARRAY_SIZE] = {0};
	char *pSnmp_ipv6_info = NULL;
	static int first_snmp_ipv6 = 1;
	FILE * ipv6fp = NULL;
	char ipv6_tmp_value[32] = {0};
	char ipv6_attr_value[64] = {0};
	char ipv6_sink2trap_info[128] = {0};
	char ipv6_sink2trap_key[2][SNMP_ARRAY_SIZE] =
	{
		{"trap2sink udp6:"},
		{""},
	};
#endif
#if defined(TCSUPPORT_SNMP_V3)
	enum snmpv3_keyword
	{
		snmpv3_enable = 0,
		snmpv3_usrname,
		snmpv3_access,
		auth_protocol,
		auth_passwd,
		priv_protocol,
		priv_passwd,
		snmpv3_keyword_maxNum,
	};
	char snmpv3_attr[snmpv3_keyword_maxNum][SNMP_ARRAY_SIZE]=
	{
		{"V3Enable"},
		{"v3Name"},
		{"access"},
		{"authProto"},
		{"authPasswd"},
		{"privProto"},
		{"privPasswd"},
	};
	char snmpv3_info[snmpv3_keyword_maxNum][SNMP_ARRAY_SIZE] = {0};
	char temp_v3_info[128] = {0};
	int ret1 = 0, ret2 = 0;
#endif
#if defined(TCSUPPORT_SNMP_FULL)
	char trap_community[SNMP_ARRAY_SIZE] = {0};
	static char gLastTrapIP[32] = TRAPIP_DEFAULT;
#endif
	char trimed_snmpd_keyword[ATTR_SIZE]={0};
	char snmpd_attr_value[64] = {0};
	char snmp_config_path[]="/var/tmp/snmp.tmp";
	char snmp_location_and_contact[512]={0};

	snprintf(nodePath, sizeof(nodePath), SNMPD_ENTRY_NODE);
	cfg_get_object_attr(nodePath, "Active",tmpActive, sizeof(tmpActive));

	for(i = 0; i < sizeof(snmpd_keyword)/ATTR_SIZE ; i++)
	{
		if(i == SNMPD_STAT_INDEX)
		{
			snprintf(snmp_info[SNMPD_STAT_INDEX],sizeof(snmp_info[SNMPD_STAT_INDEX]), "%s", tmpActive);
		}
		else
		{
			/*there are space word after each snmpd_keyword element.
			 *mxmlElementGetAttr could not found attr's value if there are additional space word in attr name.
			 *Consequently,use another string trimed_snmpd_keyword to store trimed snmpd_keyword
			 *and pass it to mxmlElementGetAttr
			 * */
			strncpy(trimed_snmpd_keyword,snmpd_keyword[i-1],strlen(snmpd_keyword[i-1]));
			tlen = strlen(snmpd_keyword[i-1])-1;
			if(tlen >= 0 && tlen < ATTR_SIZE)
				trimed_snmpd_keyword[tlen] = '\0';

			if(cfg_get_object_attr(nodePath, trimed_snmpd_keyword, snmpd_attr_value, sizeof(snmpd_attr_value)) >= 0)
			{
#if defined(TCSUPPORT_SNMP_FULL)
				if ( RO_Community == (i-1) )
				{
					snprintf(snmp_info[i], sizeof(snmp_info[i]), "%s  %s \n",trimed_snmpd_keyword,snmpd_attr_value);
					memcpy(trap_community, snmpd_attr_value, sizeof(trap_community));
				}
				else if( Trap_ManaIP == (i-1) || Trap2_ManaIP == (i-1) )
				{
					if ( Trap2_ManaIP == i-1 )
					{
						/* If trap2ip changed, set the attribute "coldTrapFlag" value as 0.
						Then after save and reset snmpd, it will send cold start trap packet. */
						if ( strcmp(gLastTrapIP, snmpd_attr_value) != 0 /* value changed */
							&& (strcmp(gLastTrapIP, TRAPIP_DEFAULT) != 0) ) /* not the first time start */
						{
							cfg_set_object_attr(nodePath, SNMP_COLDTRAP, COLD_TRAP_VALUE);
						}
						snprintf(gLastTrapIP, sizeof(gLastTrapIP), "%s", snmpd_attr_value);
					}

					if ( strlen(trap_community) != 0 )
						snprintf(snmp_info[i], sizeof(snmp_info[i]), "%s  %s  %s \n", trimed_snmpd_keyword, snmpd_attr_value, trap_community);
					else
						snprintf(snmp_info[i], sizeof(snmp_info[i]), "%s  %s  %s \n", trimed_snmpd_keyword, snmpd_attr_value, TRAP_COMMUNITY);
				}
				else
#endif
					snprintf(snmp_info[i], sizeof(snmp_info[i]), "%s  %s \n", trimed_snmpd_keyword, snmpd_attr_value);
			}
			else
			{
				return FAIL;
			}
			memset(trimed_snmpd_keyword, 0, sizeof(trimed_snmpd_keyword));
			memset(snmpd_attr_value, 0, sizeof(snmpd_attr_value));
		}
	}

	for(i=0; strlen(keyword_for_firewall[i]); i++)
	{
		if(cfg_get_object_attr(nodePath, (char *)keyword_for_firewall[i], snmpd_attr_value, sizeof(snmpd_attr_value)) > 0)
		{
			snprintf(snmp_location_and_contact + strlen(snmp_location_and_contact), 
				sizeof(snmp_location_and_contact) - strlen(snmp_location_and_contact), "%s\n", snmpd_attr_value);
		}
		else
		{
			return FAIL;
		}
	}

	if(strcmp(snmp_info[SNMPD_STAT_INDEX], ACTIVE) == 0)
	{
		if(write2file(SNMPD_ACTIVE, SNMPD_STAT_PATH) == FAIL)
		{
			return FAIL;
		}	
		if(write2file(snmp_location_and_contact, snmp_config_path) == FAIL)
		{
			return FAIL;
		}
	}

	pSnmp_info = (char *)snmp_info;
	/*Only snmp_info[1] && snmp_info[2] parameters need to update snmpd.conf file*/
	updateString(SNMPD_PATH, (char *)snmpd_keyword, pSnmp_info + ATTR_SIZE);

#if defined(TCSUPPORT_IPV6_SNMP)
	memset(snmp_ipv6_info, 0, sizeof(snmp_ipv6_info));
	snprintf(snmp_ipv6_info[agent_address], sizeof(snmp_ipv6_info[agent_address]), "%s %s \n", keyword_for_ipv6[agent_address], AGENT_ADDRESS_INFO);
	
	memset(ipv6_tmp_value, 0, sizeof(ipv6_tmp_value));
	if ( cfg_get_object_attr(nodePath, "rwcommunity", ipv6_tmp_value, sizeof(ipv6_tmp_value)) < 0 )
		return FAIL;
	if ( strlen(ipv6_tmp_value) == 0 )
		strcpy(ipv6_tmp_value, "private");         /* default setting */
	snprintf(snmp_ipv6_info[rw_community6], sizeof(snmp_ipv6_info[rw_community6]), "%s %s \n", keyword_for_ipv6[rw_community6], ipv6_tmp_value);
	
	memset(ipv6_tmp_value, 0, sizeof(ipv6_tmp_value));
	if ( cfg_get_object_attr(nodePath, "rocommunity", ipv6_tmp_value, sizeof(ipv6_tmp_value)) < 0 )
		return FAIL;
	if ( strlen(ipv6_tmp_value) == 0 )
		strcpy(ipv6_tmp_value, "public");         /* default setting */
	snprintf(snmp_ipv6_info[ro_community6], sizeof(snmp_ipv6_info[ro_community6]), "%s %s \n", keyword_for_ipv6[ro_community6], ipv6_tmp_value);
	
	memset(ipv6_attr_value, 0, sizeof(ipv6_attr_value));
	if ( cfg_get_object_attr(nodePath, "trap2sink_ipv6", ipv6_attr_value, sizeof(ipv6_attr_value)) < 0 )
		strcpy(ipv6_attr_value, "::1");   		/* default setting */

	/* if link local address, we must specify the interface */
	snprintf(ipv6_sink2trap_info, sizeof(ipv6_sink2trap_info), "%s[%s%%%s]:162 %s\n", ipv6_sink2trap_key[0], ipv6_attr_value, LOCAL_INTERFACE, ipv6_tmp_value);
	
	/* add ipv6 information first time */
	if ( first_snmp_ipv6 )
	{
		first_snmp_ipv6 = 0;
		ipv6fp = fopen(SNMPD_PATH, "a");
		if ( !ipv6fp )
		{
			tcdbg_printf("open snmpd.conf  file failed\n");
			return FAIL;
		}
		fputs("\n################################################################\n", ipv6fp);
		fputs("#	SNMP IPv6 Information                                   #\n", ipv6fp);
		fputs("################################################################\n\n", ipv6fp);
		for ( i = 0; i < snmp_ipv6_keyword_max; i ++ )
		{
			fputs(snmp_ipv6_info[i], ipv6fp);
		}
		fputs(ipv6_sink2trap_info, ipv6fp);
		fclose(ipv6fp);
	}
	else
	{
		pSnmp_ipv6_info = (char*)snmp_ipv6_info;
		updateString(SNMPD_PATH, (char *)keyword_for_ipv6, pSnmp_ipv6_info);
		updateString(SNMPD_PATH, (char *)ipv6_sink2trap_key, ipv6_sink2trap_info);
	}
#endif

#if defined(TCSUPPORT_SNMP_V3)
	/* delete the config file */
	ret1 = remove(SNMPV3_AUTH_FILE);
	ret2 = remove(SNMPV3_ACCESS_FILE);
	memset(snmpv3_info, 0, sizeof(snmpv3_info));

	if ( cfg_get_object_attr(nodePath, snmpv3_attr[snmpv3_enable], snmpv3_info[snmpv3_enable], sizeof(snmpv3_info[snmpv3_enable])) < 0 )
		return SUCCESS;						/* snmpv3 disable */
	if ( strcmp(snmpv3_info[snmpv3_enable], "No") == 0 )
		return SUCCESS;						/* snmpv3 disable */

	for ( i = snmpv3_usrname; i < snmpv3_keyword_maxNum; i++ )
	{
		if ( cfg_get_object_attr(nodePath, snmpv3_attr[i], snmpv3_info[i], sizeof(snmpv3_info[i])) < 0 )
			return FAIL;	
	}

	if ( strcmp(snmpv3_info[snmpv3_access], "RW") == 0 )
	{	
		memset(snmpv3_info[snmpv3_access], 0, sizeof(snmpv3_info[snmpv3_access]));
		strcpy(snmpv3_info[snmpv3_access], "rwuser");
	}
	else
	{	
		memset(snmpv3_info[snmpv3_access], 0, sizeof(snmpv3_info[snmpv3_access]));
		strcpy(snmpv3_info[snmpv3_access], "rouser");
	}
	
	/* create access snmd.conf */
	memset(temp_v3_info, 0, sizeof(temp_v3_info));
	sprintf(temp_v3_info, "%s %s\n", snmpv3_info[snmpv3_access], snmpv3_info[snmpv3_usrname]);

	if ( write2file(temp_v3_info, SNMPV3_ACCESS_FILE) == FAIL )
	{
		tcdbg_printf("create access failed!\n");
		return FAIL;
	}	

	/* create auth snmd.conf */
	memset(temp_v3_info, 0, sizeof(temp_v3_info));
	snprintf(temp_v3_info, sizeof(temp_v3_info), "createUser %s %s \"%s\" %s %s\n", snmpv3_info[snmpv3_usrname], snmpv3_info[auth_protocol],
			snmpv3_info[auth_passwd], snmpv3_info[priv_protocol], snmpv3_info[priv_passwd]);

	if ( write2file(temp_v3_info, SNMPV3_AUTH_FILE) == FAIL )
	{
		tcdbg_printf("create auth failed!\n");
		return FAIL;
	}	
#endif 

	return SUCCESS;
}

int svc_other_handle_event_snmpd_boot(void)
{	
	FILE *startupSh=NULL;
	startupSh=fopen("/etc/snmp/snmpd_stat.conf","r");
	if(startupSh)
	{
		fclose(startupSh);
		system("/userfs/bin/snmpd -f -c /etc/snmp/snmpd.conf -P /var/log/snmpd.pid &");
	}
	
	return SUCCESS;
}


int svc_other_handle_event_snmpd_execute(char* path)
{
	char snmpd_stat[8]={0};
	char cmd[64]={0};

	cfg_get_object_attr(path, NODE_ACTIVE, snmpd_stat, sizeof(snmpd_stat));
	kill_process(SNMPD_PID_PATH);

	if(strcmp(snmpd_stat, ACTIVE) == 0)
	{
		snprintf(cmd, sizeof(cmd), SNMPD_CMD, SNMPD_PATH, SNMPD_PID_PATH);
		system(cmd);
	}
	/*reset firewall*/
	sleep(1);
	cfg_commit_object(FIREWALL_NODE);

	return 0;
}

int svc_send_signal_snmpd(void)
{
#if defined(TCSUPPORT_SNMP_FULL)
	char pid[32]={0};
	const char *snmpd_pid  = "/var/log/snmpd.pid";

	fileRead(snmpd_pid, pid, sizeof(pid));
	if ( strlen(pid) > 0 )
	{
		kill(atoi(pid), SIGUSR2);
	}
#endif
	return 0;
}

