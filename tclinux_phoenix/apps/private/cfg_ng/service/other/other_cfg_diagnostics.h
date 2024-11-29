
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

#ifndef __SVC_OTHER_CFG_DIAGNOSTICS_H__
#define __SVC_OTHER_CFG_DIAGNOSTICS_H__

#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)
#if defined(TCSUPPORT_CT_PPPERRCODE_ENHANCE)
#define ERROR_OUT_OF_RESOURCES "ERROR_SERVER_OUT_OF_RESOURCES_IPv6"
#define ERROR_PARAMNEGOFAILV4 "ParamNegoFail_IPv4"
#else
#define ERROR_OUT_OF_RESOURCES "ERROR_SERVER_OUT_OF_RESOURCES"
#endif

#define PPPOE_EMULATOR_STOP_FLAG
#define PPPOE_EMULATOR_ENTRY "Entry"
#define PPPOE_EMULATOR_NODE_NAME "PppoeEmulator"
#define PPPOE_EMULATOR_PID_PATH "/var/run/pppoe_sim.pid"
#define PPPOE_EMULATOR_CONF		"/var/run/pppoe_sim.conf"
#define PPPOE_EMULATOR_SH "/usr/script/pppoe_simulate.sh"
#define PPPOE_EMULATOR_USER_STOP		1
#define PPPOE_EMULATOR_UNKNOW_STOP 		2
#define PPPOE_EMULATOR_COMPLETE			3
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
#define PPPOE_EMULATOR_ATTR_NASNAME "NASName"
#define PPPOE_EMULATOR_NAS_NAME "emulator"
#define PPPOE_EMULATOR_PPPNAME "ppp199"
#define PPPOE_EMULATOR_PPPIF_PATH "/var/run/ppp199/"
#define PPPOE_EMULATOR_PPPV6UP_PRE_SCRIPT "/usr/etc/ppp/emu_ipv6-up_pre"
#define PPPOE_EMULATOR_PPPV6UP_SCRIPT "/usr/etc/ppp/emu_ipv6-up"
#define PPPOE_EMULATOR_PPPDHCP6C_SCRIPT "/usr/script/emu_dhcp6c_script"
#define PPPOE_EMULATOR_PPPDHCP6C_PATH PPPOE_EMULATOR_PPPIF_PATH"dhcp6c.pid"
#define PPPOE_EMULATOR_PPPDHCP6C_CONF PPPOE_EMULATOR_PPPIF_PATH"dhcp6c.conf"
#define PPPOE_EMULATOR_GWV6_PATH "/proc/sys/net/ipv6/neigh/"PPPOE_EMULATOR_PPPNAME"/default_route"
#define PPPOE_EMULATOR_SLAACV6_PATH "/proc/sys/net/ipv6/conf/"PPPOE_EMULATOR_PPPNAME"/slaac_addr"
#define PPPOE_EMULATOR_RAFLAGS_PATH "/proc/sys/net/ipv6/neigh/"PPPOE_EMULATOR_PPPNAME"/ra_flags"
#define PPPOE_EMULATOR_SLAACPREFIX_PATH "/proc/sys/net/ipv6/conf/"PPPOE_EMULATOR_PPPNAME"/slaac_prefix"
#define	PPPOE_EMULATOR_MFLAG (1<<7)
#define	PPPOE_EMULATOR_OFLAG (1<<6)
#define PPPOE_EMULATOR_DOMAIN_FILE "/tmp/pppemulatordomain"
#define PPPOE_EMULATOR_DNSROUTE_ADD "/usr/bin/ip -6 route add %s/128 via %s dev "PPPOE_EMULATOR_PPPNAME
#endif

#define EMULATOR_TIMEOUT 30

int checkPPPoE_Emulator_timer();

void svc_other_handle_event_pppoeemulator_init();
int svc_other_handle_event_pppoeemulator_write();
int svc_other_handle_event_pppoeemulator_execute();
int svc_other_handle_event_pppoeemulator_update();
int svc_other_handle_event_pppoeemulator_boot(void);
#endif

#if defined(TCSUPPORT_CT_IPOE_EMULATOR)
#define MAX_INFO_LENGTH                 (128)
#define IPOE_EMULATOR_NAS_NAME "ipoe_emu"
#define IPOE_EMULATOR_PATH "/var/run/"IPOE_EMULATOR_NAS_NAME"/"
#define IPOE_EMULATOR_PID_PATH "/var/run/ipoe_emu.pid"
#define IPOE_EMULATOR_PING_PID_PATH "/var/run/ipoe_ping.pid"
#define IPOE_EMULATOR_CONF "/var/run/ipoe_emu.conf"
#define IPOE_EMULATOR_START "/usr/script/ipoe_simulate.sh"
#define IPOE_EMULATOR_DHCP_SH "/usr/script/udhcpc_ipoe_sim.sh"
#define IPOE_EMULATOR_ST_NONE "None"
#define IPOE_EMULATOR_ST_START "Start"
#define IPOE_EMULATOR_ST_STOP "Stop"
#define IPOE_EMULATOR_ST_COMPLETE "Complete"
#define IPOE_EMULATOR_ST_RUNNING "Running"
#define IPOE_EMULATOR_ATTR_DIGSTATE "DiagnosticsState"
#define IPOE_EMULATOR_ATTR_WANIFACE "WANInterface"
#define IPOE_EMULATOR_ATTR_USERMAC "UserMac"
#define IPOE_EMULATOR_ATTR_VENDORID "VendorClassID"
#define IPOE_EMULATOR_ATTR_RESULT "Result"
#define IPOE_EMULATOR_ATTR_LADDR "LocalAddress"
#define IPOE_EMULATOR_ATTR_DEFGW "DefaultGateway"
#define IPOE_EMULATOR_ATTR_PINGDSTIP "PingDestIPAddress"
#define IPOE_EMULATOR_ATTR_PINGNUM "PingNumberOfRepetitions"
#define IPOE_EMULATOR_ATTR_TIMEOUT "Timeout"
#define IPOE_EMULATOR_ATTR_SUCC_COUNT "SuccessCount"
#define IPOE_EMULATOR_ATTR_FAIL_COUNT "FailureCount"
#define IPOE_EMULATOR_ATTR_REP_AVRTIME "AverageResponseTime"
#define IPOE_EMULATOR_ATTR_REP_MINTIME "MinimumResponseTime"
#define IPOE_EMULATOR_ATTR_REP_MAXTIME "MaximumResponseTime"
#define IPOE_EMULATOR_ATTR_NASNAME "NASName"
#define IPOE_EMULATOR_RESULT_SUCC "Success"
#define IPOE_EMULATOR_RESULT_OTHER "Other"
#define IPOE_EMULATOR_RESULT_PINGFAIL "AllDestinationPingFail"
#define IPOE_EMULATOR_RESULT_SOMEPINGFAIL "SomeDestinationPingFail"
#define IPOE_EMULATOR_POLICY_TABLEID 234
#define IPOE_EMULATOR_PING_RESULT IPOE_EMULATOR_PATH"PingResult"

enum IPOE_EMULATOR_STOP_FLAGS
{
	IPOE_EMULATOR_STOP_SUCC = 0,
	IPOE_EMULATOR_STOP_OTHER,
};

int svc_other_handle_event_ipoe_emulator_boot(void);
int svc_other_handle_event_ipoe_emulator_write(void);
int svc_other_handle_event_ipoe_emulator_execute(void);
int svc_other_handle_event_ipoe_emulator_update(void);
#endif

#endif



