
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


#ifndef __SVC_OTHER_MGR_H__
#define __SVC_OTHER_MGR_H__

#include "other_cfg.h"
#include "other_snmp_cfg.h"
#include "cfg_xml.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>

#define OTHER_EVENT_LAN_UPDATE			1
#define OTHER_EVENT_SAMBA_UPDATE		2
#define OTHER_EVENT_ARPRULE_CHANGE		3

#define MAX_PREFIX_MASK_LEN	64
#define START_NEW_ADD_lAN_IPV6	2
#define MAX_NEW_ADD_lAN_IPV6	3

#define NODE_ACTIVE	"Active"
#define WAN_ISP	"ISP"

#define LAN_IP	"IP"
#define LAN_MASK	"netmask"
#define LAN_CFG_IP_INDEX	2
#define LAN_ATTR_MASK_INDEX	3
#define LAN_CFG_MASK_INDEX	4
#define LAN_IFCONFG_CMD  "/sbin/ifconfig"
#define LAN_IF_INDEX  0
#define LAN_MASK_NAME_INDEX  1
#define LAN_MASK_INDEX  2
#define LAN_IF  "br0"
#define LAN_PATH  "/etc/lanconfig.sh"
#define LAN_DHCP	"Dhcp"
#define LAN_DHCP_TYPE	"type"
#ifdef IPV6
#define LAN_IPV6	"IP6"
#define LAN_PREFIXV6    "PREFIX6"
#endif

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
#define LAN_ACCESSLIMIT_CMD  "/usr/bin/accesslimitcmd laninfo"
#endif

#define WEBPASSWD_PATH 	"/etc/passwd"
#define SAMBA_EXE_PATH "/var/tmp/samba_exe.sh"
#define SAMBA_CMD			"/usr/script/samba.sh"
#define SAMBA_ADD_DIR_CMD	"/usr/script/samba_add_dir.sh"
#if defined(TCSUPPORT_CT_JOYME4)
#define SAMBA_FILE_PATH	"/etc/samba/smb.conf"
#endif

#define SAMBA_NMBD_CMD		"/userfs/bin/nmbd -D\n"
#define SAMBA_SMBD_CMD		"/userfs/bin/smbd -D\n"
#define SAMBA_NMBD_PID_PATH	"/var/run/nmbd.pid"
#define SAMBA_SMBD_PID_PATH	"/var/run/smbd.pid"
#define SAMBA_KILL_SMBD_CMD	"killall smbd"

#define SLAT "$1$"
#define ACCOUNT_DEFAULT_STRING "%s:%s:0:0:root:/:/bin/sh\n"
#define SAMBA_PASSWD_CMD	"/userfs/bin/smbpasswd -a"

#ifdef ALIAS_IP
#define LAN_ALIAS "LanAlias"
#define LAN_ALIAS_NUM 1
#define LAN_ALIAS_PATH "/etc/lanAlias%d.conf"
#define LAN_ALIAS_START_SH "/usr/script/lanAlias_start.sh"
#define LAN_ALIAS_STOP_SH "/usr/script/lanAlias_stop.sh"
#endif

//snmpd
#define FAIL -1
#define SUCCESS 0

#define SNMPD_PID_PATH	"/var/log/snmpd.pid"
#define SNMPD_CMD	"/userfs/bin/snmpd -c %s -p %s"
#define SNMPD_PATH 	"/etc/snmp/snmpd.conf"
#define ATTR_SIZE 32
#define SNMPD_STAT_INDEX  0
#define SNMPD_ACTIVE  "ACTIVE=\"Yes\""
#define SNMPD_STAT_PATH 	"/etc/snmp/snmpd_stat.conf"

#define NODE_ACTIVE			"Active"
#define ACTIVE				"Yes"
#define DEACTIVE				"No"
#define LAN_IP				"IP"

#define MAXSIZE	160
#define OPEN_FAIL_MSG "Open fail: %s %s\n"
#define TMP_CONF	"/tmp/tmp.conf"
#define SHIFT_INDEX 32
#define RENAME_FAIL_MSG "Rename fail: %s\n"

//upnpd
#define	UPNPD_PID_PATH		"/var/log/upnpd.pid"
#define	UPNPD_CMD 			"/userfs/bin/upnpd &"
#define	UPNPD_PATH			"/etc/igd/igd.conf"
#define	UPNPD_ACTIVE	"enable"
#define	WAN_IFNAME 	"IFName"
#define	INTERNET			"INTERNET"
#define	TR069          	 	"TR069"
#define	TR069_INTERNET   	"TR069_INTERNET"
#define	SERVICE_LIST_LEN  		64

#define NODE_ACTIVE			"Active"
#define ACTIVE				"Yes"
#define DEACTIVE				"No"
#define LAN_IP				"IP"

#define ACTIVE		"Yes"
#define ACCOUNT_NODE_ACTIVE	"Active"
#define  TELNET_ENTRY		"telnetentry"
#define  WANTELNET_ENTRY		"wantelnetentry"

#if defined(TCSUPPORT_CT_JOYME2)
#define  FTP_ENTRY			"ftpcommon"	
#else
#define  FTP_ENTRY			"ftpentry"	
#endif
#define CONSOLE_ENTRY		"consoleentry"	
#define TELNETDEFAULTPORT	"23"
#define FTPDEFAULTPORT	"21"
#define CONSOLE_PID_PATH	"/etc/console.pid"
#define CONSOLE_PID_TEMP_PATH "/tmp/console_pid"

#define CONFIGTELNETRIGHT	3
#define CONFIGFTPRIGHT	2
#define CONFIGHTTPRIGHT	1
#define WEBPASSWD_PATH 	"/etc/passwd"
#define ACCOUNT_DEFAULT_STRING "%s:%s:0:0:root:/:/bin/sh\n"
#define ACCOUNT_TELNET_STRING "%s:%s:1:1:telnet:/:/bin/sh\n"
#define ACCOUNT_TELNET_ROOT_STRING "%s:%s:0:0:telnet:/:/bin/sh\n"
#define ACCOUNT_NORMAL_USER_STRING "%s:%s:2:3:sys:/:/bin/sh\n"

#define CONSOLE_PATH 	"/etc/usertty"
#define FTPPASSWD_PATH 	"/etc/ftppasswd"
#define CONSOLE_PATH2	"/etc/userttyconsole"
#define SLAT "$1$"
#define FTPSERVER_FILE_PATH "/etc/bftpd.conf"
#define FTPSERVER_FILE_PATH_TMP "/etc/bftpdtmp.conf"

#define ESCAPE_CMD_BUF 1025
#define MAXGET_PROFILE_SIZE 128


int svc_other_mgr_handle_event(unsigned int event, void* param);
int svc_other_mgr_external_handle_event(unsigned int event);

int svc_other_start_lan(void);

int svc_other_stop_lan(void);

int svc_other_check_mask_format(char *mask_arg, char *mask_decimal);

int svc_other_check_route(char* rule);

#endif

