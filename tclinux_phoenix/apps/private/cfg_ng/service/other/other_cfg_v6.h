
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

#ifndef __SVC_OTHER_CFG_V6_H__
#define __SVC_OTHER_CFG_V6_H__

#define DHCP6S_CONFPATH "/etc/dhcp6s.conf"
#define DHCP6S_ENABLE_SIMBOL "Enable"
#define DHCP6S_MODE "Mode"
#define DNS1_SERVER "DNSserver"
#define DNS2_SERVER "SecDNSserver"
#define DHCP6S_DEFAULT_DNS "fe80::1"
#define DHCP6S_DEF_PREFER_LIFE "3600"
#define DHCP6S_DEF_VALID_LIFE "7200"
#define DHCP6S_DOMAIN "DomainName"
#define DHCP6S_DNSTYPE "DNSType"
#define DHCP6S_DNSSOURCE "DNSWANConnection"

#define PREFIX_SIMBOL "PrefixIPv6"
#define PREFIX_LEN_SIMBOL "Prefixv6Len"
#define PREFFERED_LIFETIME_SIMBOL "PreferredLifetime"
#define VALID_LIFETIME_SIMBOL "ValidLifetime"
#define BR0_ADDR "Br0Addr"

#define PREFIX_ORIGN_DFT	0
#define PREFIX_ORIGN_SLLA	1
#define PREFIX_ORIGN_DHCP	2
#define PREFIX_ORIGN_STATIC	3
#define PREFIX_ORIGN_NONE	4
#define TMP_PREFIX_IF_PATH	"/tmp/prefix_if.conf"
#define IPV6_CONFPATH 		"/etc/radvd.conf"

#define MAX_PREFIX_ADDR_LEN	44 /*include mask*/
#define MAX_PREFIX_MASK_LEN	64

#define RADVD_VR_CONFPATH	"/etc/radvd_%d.conf"
#define DHCP6S_VR_CONFPATH	"/etc/dhcp6s_%d.conf"
#define RADVD_VR_PIDPATH	"/var/run/radvd_%d.pid"
#define DHCP6S_VR_PIDPATH	"/var/run/dhcp6s_%d.pid"
int svc_other_handle_event_radvd_write(char *path);
int svc_other_handle_event_radvd_execute(char *path);
int svc_other_handle_event_radvd_boot(char *path);

int svc_other_handle_event_dhcp6s_write(char *path);
int svc_other_handle_event_dhcp6s_execute(char *path);
int svc_other_handle_event_dhcp6s_boot(char *path);

#endif



