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
#include <cfg_cli.h> 
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "cfg_msg.h"
#include "utility.h"
#if defined(TCSUPPORT_CT_JOYME2)
#include <vpn_info/info_ioctl.h>
#endif
#include "wan_related_vpn_cfg.h"
#include "blapi_system.h"


#define VPNSTR				"VPN"
#define VPN_COMMON			"Common"
#define VPN_ENTRY			"Entry"
#define VPN_ATTR_ACTION		"vpnaction"
#define VPN_ACT_ATTACH		"attach_chg"
#define VPN_ACT_WANRESTART	"wan_restart"

#define VPN_PREPATH		"/var/run/xl2tpd/"
#define VPN_CONF_PATH		VPN_PREPATH"xl2tpd.conf"
#define VPN_PPP_PREPATH		VPN_PREPATH"ppp/peers/"
#define VPN_TUNNEL_NAME_PRE	"vpn_tunnel_"

#define VPN_PPTP_CHAP_SECRETS_FILE 	"/etc/ppp/chap-secrets" 
#define VPN_PPTP_PEERS_TUNNEL_FILE 	"/etc/ppp/peers/"

#define VPN_PPP_UNIT_START 100
#define VPN_POLICY_TABLE_START 300

#define MAX_VPN_ENTRY 16

#if 0
static int VPN_RestartDNS = 0;
static int VPN_dns_count = 0;
#endif

/*
	clear ebtables for VPN dest IPs
*/
static void clear_VPN_LAN_rules_core(int idx)
{
	char cmdbuf[256] = {0};
	
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "ebtables -t broute -F vpn_lan_entry%d", idx);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "ebtables -t broute -Z vpn_lan_entry%d", idx);
	system(cmdbuf);
}

#if !defined(TCSUPPORT_CT_JOYME2)
static int clear_VPN_LAN_rules()
{
	int idx  = 0;

	/* flush and zero it */
	for ( idx = 0; idx < VPN_INSTANCE_NUM; idx++ )
	{
		clear_VPN_LAN_rules_core(idx);
	}

	return 0;
}
#endif

/*
	clear nat for vpn ppp
*/

static void clear_VPN_PPP_rules_core(int idx)
{
	char cmdbuf[256] = {0};
	
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "iptables -t nat -D POSTROUTING -o ppp%d -j MASQUERADE"
		, VPN_PPP_UNIT_START + idx);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "/usr/bin/iptables -D FORWARD -o ppp%d"
		" -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu"
		, VPN_PPP_UNIT_START + idx);
	system(cmdbuf);
}

int clear_VPN_PPP_rules()
{
	int idx  = 0;

	/* delete all rules */
	for ( idx = 0; idx < VPN_INSTANCE_NUM; idx++ )
	{
		clear_VPN_PPP_rules_core(idx);
	}
	return 0;
}

/*
	create nat for vpn ppp
*/
int create_VPN_PPP_rules(int unit)
{
	char cmdbuf[256] = {0};
	int idx = 0;
	int tunnel_mark = 0;
#define VPN_PPP_FWMAKR_START 0x6e0000
	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "/usr/bin/iptables -t nat -A POSTROUTING -o ppp%d -j MASQUERADE"
		, unit);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "/usr/bin/iptables -A FORWARD -o ppp%d"
		" -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu"
		, unit);
	system(cmdbuf);

	bzero(cmdbuf, sizeof(cmdbuf));
	tunnel_mark = (unit - VPN_PPP_UNIT_START) << 16;
	snprintf(cmdbuf, sizeof(cmdbuf) - 1
		, "iptables -t mangle -D OUTPUT -o ppp%d"
		" -j MARK --set-mark 0x%x/0x7f0000"
		, unit, tunnel_mark);
	system(cmdbuf);	

	/* delete fwmark and ip source table. */
	for ( idx = 0; idx < 2; idx ++)
	{
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "/usr/bin/ip rule del table ppp%d"
			, VPN_POLICY_TABLE_START + unit - VPN_PPP_UNIT_START);
		system(cmdbuf);	
	}

	return 0;
}


int vpnlist_l2tp_write(char* entry_path, int entry_number)
{
	char nodeName[32];
	int idx = 0, ppp_unit_start = VPN_PPP_UNIT_START;
	int vpn_ppp_unit = 0, dbg_on = 0;
	char userIdBuf[128] = {0};
	char vpn_ppp_file[256] = {0};
	char lns_addr[64] = {0};
	char username[256] = {0}, password[256] = {0}, ns_name[100] = {0};
	char tunnelname[256] = {0}, action[64] = {0}, ns_val[128] = {0};
	char dbg_sw[12] = {0}, dbg_buf[64] = {0};
#if defined(TCSUPPORT_CT_VPN_ONDEMAND)
	char idle_time[64] = {0}, tmpbuf[64] = {0};
#endif
	FILE *fp_conf = NULL, *fp_ppp_conf = NULL;
#if defined(TCSUPPORT_CT_JOYME2)
	char vpn_type[32] = {0};
#endif
#if !defined(TCSUPPORT_CT_JOYME2)
	char cmdbuf[256] = {0};
	int ppp_pid = 0;
#endif

	const char *global_header = "[global]\n"
							"access control = no\n"
							"port = 1701\n";
	const char *tunne_param = "[lac %s]\n"
							"name = %s\n"
							"lns = %s\n"
							"pppoptfile = "VPN_PPP_PREPATH"vpn_tunnel_%d.l2tpd\n"
							"%s"
#if !defined(TCSUPPORT_CT_VPN_ONDEMAND)
							"redial = yes\n"
							"redial timeout = 3\n"
#endif
							"ppp debug = yes\n";
	const char *ppp_optfile = 
							"user %s\n"
							"password %s\n"
							"unit %d\n"
#if !defined(TCSUPPORT_CT_VPN_ONDEMAND)
							"persist\n"
#endif
							"nobsdcomp\n"
							"noccp\n"
							"nopcomp\n"
							"noaccomp\n"
							"usepeerdns\n"
							"vpn-pppup-script /etc/ppp/vpn_ip-up\n"
							"vpn-pppdw-script /etc/ppp/vpn_ip-down\n"
#if 0
#if defined(TCSUPPORT_SDN_OVS)
							"vpn-ppp-namespace %s\n"
#endif
#endif
#if defined(TCSUPPORT_CT_VPN_ONDEMAND)
							"%s"
#endif
							"%s";
							/*
							"maxfail 3\n"
							"idle 1800\n"
							"mtu 1410\n"
							"mru 1410\n"
							"ipcp-accept-local\n"
							"ipcp-accept-remote\n"
							"noauth\n"
							"connect-delay 500\n"
							"nodeflate\n""
							*/

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), VPN_COMMON_NODE);
	cfg_get_object_attr(nodeName, "debug", dbg_sw, sizeof(dbg_sw));
	if ( 0 == strcmp(dbg_sw, "Yes") )
	{
		dbg_on = 1;
	}
#if !defined(TCSUPPORT_CT_JOYME2)
	if(cfg_get_object_attr(nodeName, VPN_ATTR_ACTION, action, sizeof(action)) > 0 \
		&& 0 != action[0] )
	{
		if ( 0 == strcmp(action, VPN_ACT_ATTACH) )
		{
			return SUCCESS;
		}
	}
	/* 1. close vpn ppp connections. */
	for ( idx = 0; idx < VPN_INSTANCE_NUM; idx ++ )
	{
		bzero(dbg_buf, sizeof(dbg_buf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "/var/run/ppp%d.pid"
			, idx+VPN_PPP_UNIT_START);
		fileRead(cmdbuf, dbg_buf, sizeof(dbg_buf));
		ppp_pid = atoi(dbg_buf);
		if ( ppp_pid > 0 )
		{
			kill(ppp_pid, SIGTERM);
			sleep(1);
			snprintf(cmdbuf, sizeof(cmdbuf) - 1
				, "'d "VPN_TUNNEL_NAME_PRE"%d'\n", idx);
			doValPut("/var/run/xl2tpd/l2tp-control", cmdbuf);
		}
	}

	/* 2. kill xl2tpd */
	system("/usr/bin/killall -SIGKILL xl2tpd");
	/* 3. remove file & create folder */
	system("/bin/rm -rf "VPN_PREPATH"*");
#endif
	system("/bin/mkdir -p "VPN_PPP_PREPATH);

	/* 4. clear all rules and re-create it. */
#if defined(TCSUPPORT_CT_JOYME2)
	clear_VPN_LAN_rules_core(entry_number);
	clear_VPN_PPP_rules_core(entry_number);
#else
	clear_VPN_LAN_rules();
	clear_VPN_PPP_rules();
#endif

	/* 5. create xl2tpd conf */
	fp_conf = fopen(VPN_CONF_PATH, "w");
	if ( !fp_conf )
		return FAIL;

	/* a. write global data. */
	fprintf(fp_conf, "%s", global_header);
	/* b. write tunnel data. */
	bzero(nodeName, sizeof(nodeName));
	for ( idx = 0; idx < VPN_INSTANCE_NUM; idx ++ )
	{
		snprintf(nodeName, sizeof(nodeName), VPN_ENTRY_NODE, (idx+1));
		if ( (0 < cfg_get_object_attr(nodeName, "userId", userIdBuf, sizeof(userIdBuf)))
			&& (0 != userIdBuf[0])
			&& (0 < cfg_get_object_attr(nodeName, "serverIpAddr", lns_addr, sizeof(lns_addr)))
			&& (0 != lns_addr[0])
#if defined(TCSUPPORT_CT_JOYME2)	
			&& (0 < cfg_get_object_attr(nodeName, "vpn_type", vpn_type, sizeof(vpn_type)))
			&& (0 == strcmp(vpn_type, "l2tp"))
			&& (0 < cfg_get_object_attr(nodeName, "action", action, sizeof(action)))
			&& (0 == strcmp(action, "add"))
#endif
			)
		{
			/* c. generate tunnel name */
			bzero(tunnelname, sizeof(tunnelname));
			bzero(ns_val, sizeof(ns_val));
			bzero(ns_name, sizeof(ns_name));
#if !defined(TCSUPPORT_CT_VPN_ONDEMAND)
			snprintf(tunnelname, sizeof(tunnelname) - 1
				, VPN_TUNNEL_NAME_PRE"%d", idx);
			cfg_set_object_attr(nodeName, "tunnelName", tunnelname);
			cfg_set_object_attr(nodeName, "namespace", ns_name);
#else
		if ( cfg_get_object_attr(nodeName, "tunnelName", tunnelname, sizeof(tunnelname)) <= 0 )
		{
			snprintf(tunnelname, sizeof(tunnelname) - 1
				, VPN_TUNNEL_NAME_PRE"%d", idx);
			cfg_set_object_attr(nodeName, "tunnelName", tunnelname);
		}
		cfg_get_object_attr(nodeName, "namespace", ns_name, sizeof(ns_name));
#endif
			snprintf(ns_val, sizeof(ns_val) - 1,
					"netns = %s\n", ns_name);
			fprintf(fp_conf, tunne_param, tunnelname
				, tunnelname, lns_addr, idx
				, (0 != ns_name[0]) ? ns_val : "");
			/* d. write tunnel ppp config file. */
			bzero(vpn_ppp_file, sizeof(vpn_ppp_file));
			snprintf(vpn_ppp_file, sizeof(vpn_ppp_file) - 1
				, VPN_PPP_PREPATH"vpn_tunnel_%d.l2tpd"
				, idx);
			fp_ppp_conf = fopen(vpn_ppp_file, "w");
			if ( fp_ppp_conf )
			{
				cfg_get_object_attr(nodeName, "username", username, sizeof(username));
				cfg_get_object_attr(nodeName, "password", password, sizeof(password));
#if defined(TCSUPPORT_CT_VPN_ONDEMAND)
				cfg_get_object_attr(nodeName, "idletime", tmpbuf, sizeof(tmpbuf));
#endif
				if ( 1 == dbg_on )
				{
					snprintf(dbg_buf, sizeof(dbg_buf) - 1
						, "logfile /var/run/xl2tpd/log%d.log\n"
						  "debug\n"
						, idx);
				}
#if defined(TCSUPPORT_CT_VPN_ONDEMAND)
				if ( 0 != atoi(tmpbuf) )
				{
					snprintf(idle_time, sizeof(idle_time), "idle %s", tmpbuf);
				}
				
#endif
				vpn_ppp_unit = ppp_unit_start + idx;
				fprintf(fp_ppp_conf, ppp_optfile, username, password
					, vpn_ppp_unit
#if defined(TCSUPPORT_SDN_OVS)
					, (0 != ns_name[0]) ? ns_name : ""
#endif
#if defined(TCSUPPORT_CT_VPN_ONDEMAND)
				, idle_time
#endif
					, ( (1 == dbg_on) ? dbg_buf : "") );
				create_VPN_PPP_rules(vpn_ppp_unit);

				fclose(fp_ppp_conf);
			}
			cfg_set_object_attr(nodeName, "vpnstatus", "1");
		}
	}

	fclose(fp_conf);

	return SUCCESS;
}
	
/*
	open all vpn ppp tunnel
*/
static int open_VPN_PPP_tunnel(void)
{
#if !defined(TCSUPPORT_CT_VPN_ONDEMAND)
	char nodeName[32];
	char userIdBuf[128] = {0};
	char lns_addr[64] = {0};
	char vpnstatus[32] = {0};
	char cmdbuf[256] = {0};
	int idx = 0;

	bzero(nodeName, sizeof(nodeName));
	for ( idx = 0; idx < VPN_INSTANCE_NUM; idx ++ )
	{
		snprintf(nodeName, sizeof(nodeName), VPN_ENTRY_NODE, (idx+1));
		bzero(userIdBuf, sizeof(userIdBuf));
		bzero(lns_addr, sizeof(lns_addr));
		bzero(vpnstatus, sizeof(vpnstatus));
		if ( 0 < cfg_get_object_attr(nodeName, "userId", userIdBuf, sizeof(userIdBuf))
			&& 0 != userIdBuf[0]
			&& 0 < cfg_get_object_attr(nodeName, "serverIpAddr", lns_addr, sizeof(lns_addr))
			&& 0 != lns_addr[0]
			&& 0 < cfg_get_object_attr(nodeName, "vpnstatus", vpnstatus, sizeof(vpnstatus))
#if defined(TCSUPPORT_CMCCV2)
			&& 0 != strcmp(vpnstatus, "2")
#else
			&& 0 != strcmp(vpnstatus, "0")
#endif
			)
		{
			snprintf(cmdbuf, sizeof(cmdbuf) - 1
				, "echo 'c "VPN_TUNNEL_NAME_PRE"%d' "
				">/var/run/xl2tpd/l2tp-control"
				, idx);
			system(cmdbuf);
		}
	}
#endif

	return 0;
}

#if defined(TCSUPPORT_CT_JOYME2)
int check_process_exist(char* process_name)
{
	int xl2tpd_pid = 0;
	int rv = 0, pidnum = 0, pid_t[4] = {0};
	int i = 0;

	rv = find_pid_by_name( process_name, pid_t, &pidnum);
	if(0 == rv)
	{
		if (1 == pidnum)
		{
			xl2tpd_pid = pid_t[0];
		}
		else if(pidnum > 1)
		{
			for (i = 0; (i < pidnum) && (i < 4); i++)
			{
				kill(pid_t[i], SIGKILL);
			}
		}	
	}
		
	return xl2tpd_pid;
}

#endif


#if defined(TCSUPPORT_CT_VPN_PPTP)
static int write_pptp_ppers_tunnel_file(char* username, char* tunnelname, 
											char* server, char* idletime, int idx)
{
	FILE* fp = NULL;
	char file_name[128] = {0};
	char file_text[256] = {0};
	
	snprintf(file_name, sizeof(file_name), "%s%s", VPN_PPTP_PEERS_TUNNEL_FILE, tunnelname);
	fp = fopen(file_name, "w");
	if (fp)
	{
		/*set pty*/
		memset(file_text, 0, sizeof(file_text));
		snprintf(file_text, sizeof(file_text), "pty \"pptp %s --nolaunchpppd\"\n", server);
		fputs(file_text, fp);

		/*set lock*/
		fputs("lock\n", fp);

		/*set unit*/
		memset(file_text, 0, sizeof(file_text));
		snprintf(file_text, sizeof(file_text), "unit %d\n", VPN_PPP_UNIT_START + idx);
		fputs(file_text, fp);
		
		/*set noauth*/
		fputs("noauth\n", fp);

		/*set nobsdcomp*/
		fputs("nobsdcomp\n", fp);

		/*set nodeflate*/
		fputs("nodeflate\n", fp);

		/*set name*/
		memset(file_text, 0, sizeof(file_text));
		snprintf(file_text, sizeof(file_text), "name %s\n", username);
		fputs(file_text, fp);

		/*set remotename*/
		memset(file_text, 0, sizeof(file_text));
		snprintf(file_text, sizeof(file_text), "remotename %s\n", tunnelname);
		fputs(file_text, fp);

		/*set ipparam*/
		memset(file_text, 0, sizeof(file_text));
		snprintf(file_text, sizeof(file_text), "ipparam %s\n", tunnelname);
		fputs(file_text, fp);

		/*set vpn_ip-up*/
		fputs("vpn-pppup-script /etc/ppp/vpn_ip-up\n", fp);
		
		/*set vpn_ip-down*/
		fputs("vpn-pppdw-script /etc/ppp/vpn_ip-down\n", fp);

		/*set idletime*/
		if ((strlen(idletime) > 0) && (0 != atoi(idletime)))
		{
			memset(file_text, 0, sizeof(file_text));
			snprintf(file_text, sizeof(file_text), "idle %s\n", idletime);
			fputs(file_text, fp);
		}

		fclose(fp);
	}
	else
	{
		tcdbg_printf("Open file %s fail.\n", file_name);
		return -1;
	}

	return 0;
}

static int write_pptp_chap_secrets(void)
{
	FILE* fp = NULL;
	char file_text[128]  = {0};
	int idx = 0;
	char node_name[32] = {0};
	char vpn_type[32] = {0};
	char username[32] = {0};
	char tunnelname[32] = {0};
	char password[32] = {0};
	char action[16] = {0};
	int ret = -1;

	fp = fopen(VPN_PPTP_CHAP_SECRETS_FILE, "w");
	
	if (NULL == fp)	
	{
		tcdbg_printf("Open file %s fail.\n", VPN_PPTP_CHAP_SECRETS_FILE);
		return -1;
	}
	
	for ( idx = 0; idx < VPN_INSTANCE_NUM; idx++ )
	{
		snprintf(node_name, sizeof(node_name), VPN_ENTRY_NODE, idx + 1);
		if ((cfg_get_object_attr(node_name, "vpn_type", vpn_type, sizeof(vpn_type)) > 0)
			&& (0 == strcmp(vpn_type, "pptp"))
			&& (cfg_get_object_attr(node_name, "action", action, sizeof(action)) > 0)
			&& (0 == strcmp(action, "add")))
		{

			snprintf(tunnelname, sizeof(tunnelname), VPN_TUNNEL_NAME_PRE"%d", idx);

			if (cfg_get_object_attr(node_name, "username", username, sizeof(username)) <= 0)
			{
				tcdbg_printf("Get the username fail.\n");
				continue;
			}

			if (cfg_get_object_attr(node_name, "password", password, sizeof(password)) <= 0)
			{
				tcdbg_printf("Get the password fail.\n");
				continue;
			}

			snprintf(file_text, sizeof(file_text), "\n%s %s \"%s\" *\n\n", username, tunnelname, password);
			fwrite(file_text, strlen(file_text) + 1, 1, fp);			

			ret = 0;
		}			
	}

	fclose(fp);

	return ret;
}

static int vpnlist_pptp_tunnel_write(char* path, int idx)
{
	char tunnelname[64]	= {0};
	char username[64]	= {0};
	char password[64]	= {0};
	char server_ip_addr[32] = {0};
	char idle_time[32]	= {0};

	snprintf(tunnelname, sizeof(tunnelname), VPN_TUNNEL_NAME_PRE"%d", idx);
	
	if (cfg_get_object_attr(path, "username", username, sizeof(username)) <= 0)
	{
		tcdbg_printf("Get the username fail.\n");
		return -1;
	}

	if (cfg_get_object_attr(path, "password", password, sizeof(password)) <= 0)
	{
		tcdbg_printf("Get the password fail.\n");
		return -1;
	}

	if (cfg_get_object_attr(path, "serverIpAddr", server_ip_addr, sizeof(server_ip_addr)) <= 0)
	{
		tcdbg_printf("Get the serverIpAddr fail.\n");
		return -1;
	}

	if (cfg_get_object_attr(path, "idletime", idle_time, sizeof(idle_time)) <= 0)
	{
		tcdbg_printf("Get the idletime fail or idletime is not exist.\n");
	}
	
	if (0 != write_pptp_ppers_tunnel_file(username, tunnelname, server_ip_addr, idle_time, idx))
	{
		tcdbg_printf("write pptp tunnel file fail.\n");
		return -1;
	}	

	create_VPN_PPP_rules(VPN_PPP_UNIT_START + idx);
	
	return 0;
}

int vpnlist_pptp_write(char *path, int idx)
{
	write_pptp_chap_secrets();

	vpnlist_pptp_tunnel_write(path, idx);

	cfg_set_object_attr(path, "vpnstatus", "1");

	return 0;
}

int vpnlist_pptp_write_for_boot(void)
{
	int idx = 0;
	char node_name[32] = {0};

	if (0 != write_pptp_chap_secrets())
	{
		return -1;
	}

	for(idx = 0; idx < VPN_INSTANCE_NUM; idx++)
	{
		snprintf(node_name, sizeof(node_name), VPN_ENTRY_NODE, idx + 1);
		vpnlist_pptp_tunnel_write(node_name, idx);
	}

	return 0;
}

#endif

static int vpnlist_l2tp_execute(int entry_number)
{
	char nodeName[32] = {0};
	char action[64] = {0};

	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), VPN_COMMON_NODE);
	if ( 0 < cfg_get_object_attr(nodeName, VPN_ATTR_ACTION, action, sizeof(action))
		&& 0 != action[0] )
	{
		if ( 0 == strcmp(action, VPN_ACT_ATTACH) )
		{
			cfg_set_object_attr(nodeName, VPN_ATTR_ACTION, "");
			cfg_set_object_attr(nodeName, "vpn_restart_dns", "1");
			cfg_set_object_attr(nodeName, "vpn_dns_count", "0");
			return SUCCESS;
		}
		else if ( 0 == strcmp(action, VPN_ACT_WANRESTART) )
		{
			cfg_set_object_attr(nodeName, VPN_ATTR_ACTION, "");
			cfg_set_object_attr(nodeName, "vpn_restart_dns", "1");
			cfg_set_object_attr(nodeName, "vpn_dns_count",   "5");
		}
	}
#if defined(TCSUPPORT_CT_JOYME2)
	system("killall xl2tpd");
	system("/userfs/bin/xl2tpd -c "VPN_CONF_PATH" &");
#else
	system("/userfs/bin/xl2tpd -c "VPN_CONF_PATH);
#endif
	open_VPN_PPP_tunnel();

	return 0;	
}

#if defined(TCSUPPORT_CT_JOYME2)
int vpnlist_l2tp_del_entry_execute(char* path, int idx)
{
	char cmdbuf[256] = {0};
	char vpn_related_path[64] = {0};
	char vpn_ppp_file[256] = {0};

	/*1.delete interface*/
	snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/xl2tpd-control disconnect %s%d &", 
			VPN_TUNNEL_NAME_PRE, idx);
	system(cmdbuf);

	memset(cmdbuf, 0, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/xl2tpd-control remove %s%d &", 
			VPN_TUNNEL_NAME_PRE, idx);
	system(cmdbuf);

	/*2. clear ebtables rule*/
	clear_VPN_LAN_rules_core(idx);

	/*3. clear iptables rule*/
	clear_VPN_PPP_rules_core(idx);

	/*4.remove vpn_tunnel_x.conf*/
	bzero(vpn_ppp_file, sizeof(vpn_ppp_file));
	snprintf(vpn_ppp_file, sizeof(vpn_ppp_file) - 1, 
		VPN_PPP_PREPATH"vpn_tunnel_%d.l2tpd", idx);

	unlink(vpn_ppp_file);
	
	/*5.unset vpn & related entry*/
	cfg_delete_object(path);

	snprintf(vpn_related_path, sizeof(vpn_related_path), VPNDOMAIN_ENTRY_NODE, idx + 1);
	cfg_delete_object(vpn_related_path);
	
	snprintf(vpn_related_path, sizeof(vpn_related_path), VPNIPS_ENTRY_NODE, idx + 1);
	cfg_delete_object(vpn_related_path);
	
	snprintf(vpn_related_path, sizeof(vpn_related_path), VPNMAC_ENTRY_NODE, idx + 1);
	cfg_delete_object(vpn_related_path);
	
	return 0;
}

int vpnlist_pptp_del_entry_execute(char* path, int idx)
{
	char cmd[128] = {0};
	char tunnel_name[32] = {0};
	char file_path[64] = {0};
	char vpn_related_path[64] = {0};

	/*1.delete interface*/
	snprintf(tunnel_name, sizeof(tunnel_name), "vpn_tunnel_%d", idx);
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "pppd call %s", tunnel_name);
	system(cmd);

	/*2.remove config file*/
	memset(file_path, 0, sizeof(file_path));
	snprintf(file_path, sizeof(file_path), "/etc/ppp/peers/%s", tunnel_name);
	unlink(file_path);

	/*3. clear ebtables rule*/
	clear_VPN_LAN_rules_core(idx);

	/*4. clear iptables rule*/
	clear_VPN_PPP_rules_core(idx);

	/*5. delete vpn_entry*/
	cfg_delete_object(path);

	snprintf(vpn_related_path, sizeof(vpn_related_path), VPNDOMAIN_ENTRY_NODE, idx + 1);
	cfg_delete_object(vpn_related_path);
	
	snprintf(vpn_related_path, sizeof(vpn_related_path), VPNIPS_ENTRY_NODE, idx + 1);
	cfg_delete_object(vpn_related_path);
	
	snprintf(vpn_related_path, sizeof(vpn_related_path), VPNMAC_ENTRY_NODE, idx + 1);
	cfg_delete_object(vpn_related_path);

	return 0;
}
#endif

int vpnlist_boot_execute(void)
{
	int idx = 0;
#if !defined(TCSUPPORT_CMCCV2)
	char cmdbuf[256] = {0};
#endif
	char nodeName[32]= {0};
	char strValue[64]={0};
#if defined(TCSUPPORT_CT_JOYME2)
	char action[8] = {0};
	int vpn_entry_exist = 0;
#endif

	bzero(nodeName, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), VPN_COMMON_NODE);
	cfg_set_object_attr(nodeName, VPN_ATTR_ACTION, "");

	/* create vpn chains for LAN */
	for ( idx = VPN_INSTANCE_NUM - 1; idx >=0; idx -- )
	{
#if !defined(TCSUPPORT_CMCCV2)
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -N vpn_lan_entry%d -P RETURN", idx);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -I BROUTING -j vpn_lan_entry%d", idx);
		system(cmdbuf);

		/* flush and zero it */
		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -F vpn_lan_entry%d", idx);
		system(cmdbuf);

		bzero(cmdbuf, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf) - 1
			, "ebtables -t broute -Z vpn_lan_entry%d", idx);
		system(cmdbuf);
#endif

		/* clear ip status */
		bzero(strValue, sizeof(strValue));
		bzero(nodeName, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VPN_ENTRY_NODE, (idx+1));
		if ( 0 < cfg_get_object_attr(nodeName, "vpnstatus", strValue, sizeof(strValue))
			&& 0 != strValue[0] )
		{
			cfg_set_object_attr(nodeName, "ip", "");
			cfg_set_object_attr(nodeName, "netmask", "");
			cfg_set_object_attr(nodeName, "gateway", "");
			cfg_set_object_attr(nodeName, "status", "down");
#if defined(TCSUPPORT_CMCCV2)
			if ( 0 != strcmp(strValue, "3")
				&& 0 != strcmp(strValue, "4") )
#endif
			cfg_set_object_attr(nodeName, "vpnstatus", "1");
		}
		
#if defined(TCSUPPORT_CT_JOYME2)
		if ((0 < cfg_get_object_attr(nodeName, "action", action, sizeof(action)))
			&& ( 0 == strcmp(action, "add")))
		{
			vpn_entry_exist = 1;
		}
#endif
	}

#if defined(TCSUPPORT_CT_JOYME2)
	if (1 == vpn_entry_exist)
	{
		system("/userfs/bin/xl2tpd -c "VPN_CONF_PATH" &");
	}
#endif

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), VPN_COMMON_NODE);
	cfg_set_object_attr(nodeName, "vpn_boot",        "1");
	cfg_set_object_attr(nodeName, "vpn_restart_dns", "0");
	cfg_set_object_attr(nodeName, "vpn_dns_count",   "0");

	return SUCCESS;
	
}

int vpnlist_write(char* path, int idx)
{
#if defined(TCSUPPORT_CT_JOYME2)
	char buffer[32] = {0};

	if (cfg_get_object_attr(path, "vpn_type", buffer, sizeof(buffer)) > 0)
	{
		if(0 == strcmp(buffer, "l2tp"))
		{
			vpnlist_l2tp_write(path, idx);
		}	
#if defined(TCSUPPORT_CT_VPN_PPTP)		
		else if (0 == strcmp(buffer, "pptp"))
		{
			vpnlist_pptp_write(path, idx);
		}
#endif
		else
		{
			tcdbg_printf("The vpn type is invalid.\n");
			return -1;
		}
	}
#else
	vpnlist_l2tp_write(path, idx);
#endif

	return 0;
}

int vpnlist_execute(char* path, int idx)
{
#if defined(TCSUPPORT_CT_JOYME2)
	char buffer[32] = {0};
	char action[32] = {0};

	blapi_system_vpn_free_ip_hashmap(idx);
	
	if (cfg_get_object_attr(path, "vpn_type", buffer, sizeof(buffer)) > 0)
	{
		if (0 == strcmp(buffer, "l2tp"))
		{	
			if (cfg_get_object_attr(path, "action", action, sizeof(action)) > 0)
			{
				if (0 == strcmp(action, "add"))
				{
					vpnlist_l2tp_execute(idx);
				}
				else if(0 == strcmp(action, "delete"))
				{
					vpnlist_l2tp_del_entry_execute(path, idx);
				}
			}
			
		}
		else if (0 == strcmp(buffer, "pptp"))
		{
			if (cfg_get_object_attr(path, "action", action, sizeof(action)) > 0)
			{
				if(0 == strcmp(action, "delete"))
				{
					vpnlist_pptp_del_entry_execute(path, idx);
				}
			}
		}
	}
#else
	vpnlist_l2tp_execute(idx);
#endif
	
	return 0;	
}

