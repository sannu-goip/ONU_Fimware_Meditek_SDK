
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

#ifndef __SVC_WAN_CFG_H__
#define __SVC_WAN_CFG_H__

#include "wan_common.h"

/*************************************************************************************************/
/*************************************************************************************************/
#define SVC_WAN_ATTR_ACTIVE                         "Active"

#define SVC_WAN_ATTR_WANMODE                        "WanMode"

#define SVC_WAN_ATTR_LINKMODE                       "LinkMode"

#define SVC_WAN_ATTR_VTAG                           "VLANMode"

#define SVC_WAN_ATTR_VID                            "VLANID"

#define SVC_WAN_ATTR_8021P                          "dot1pData"

#define SVC_WAN_ATTR_VERSION                        "IPVERSION"

#define SVC_WAN_ATTR_ISP                            "ISP"

#define SVC_WAN_ATTR_CONNECTION                     "CONNECTION"

#define SVC_WAN_ATTR_PPP_MANUAL_STATUS              "PPPManualStatus"

#define SVC_WAN_ATTR_PPP_USER                       "USERNAME"

#define SVC_WAN_ATTR_PPP_PWD                        "PASSWORD"

#define SVC_WAN_ATTR_PPP_HYBRID                     "BridgeInterface"

#define SVC_WAN_ATTR_ADDRV4                         "IPADDR"

#define SVC_WAN_ATTR_MASKV4                         "NETMASK"

#define SVC_WAN_ATTR_GWV4                           "GATEWAY"

#define SVC_WAN_ATTR_DNS1V4                         "DNS"

#define SVC_WAN_ATTR_DNS2V4                         "SecDNS"

#define SVC_WAN_ATTR_IPV6MODE                       "DHCPv6"

#define SVC_WAN_ATTR_ADDRV6                         "IPADDR6"

#define SVC_WAN_ATTR_PREFIX6                        "PREFIX6"

#define SVC_WAN_ATTR_GWV6                           "GATEWAY6"

#define SVC_WAN_ATTR_DNS1V6                         "DNS6"

#define SVC_WAN_ATTR_DNS2V6                         "SecDNS6"

#define SVC_WAN_ATTR_PD                             "PDEnable"

#define SVC_WAN_ATTR_PDMODE                         "DHCPv6PD"

#define SVC_WAN_ATTR_PD_ORIGIN                      "PDOrigin"

#define SVC_WAN_ATTR_PDADDR                         "PDPrefix"

#define SVC_WAN_ATTR_PDTIME1                        "PrefixPltime"

#define SVC_WAN_ATTR_PDTIME2                        "PrefixVltime"

#define SVC_WAN_ATTR_NAT                            "NATENABLE"

#define SVC_WAN_ATTR_SRV_LIST                       "ServiceList"

#define SVC_WAN_ATTR_MVID                           "MulticastVID"

#define SVC_WAN_ATTR_DSLITE                         "DsliteEnable"

#define SVC_WAN_ATTR_DSLITE_MODE                    "DsliteMode"

#define SVC_WAN_ATTR_GWV6_OP                        "GW6_Manual"

#define SVC_WAN_ATTR_MTU                            "MTU"

#define SVC_WAN_ATTR_PROXY_ENABLE                   "ProxyEnable"

#define SVC_WAN_ATTR_DHCPRELAY                      "DHCPRealy"

#define SVC_WAN_ATTR_DHCPENABLE                     "DHCPEnable"

#define SVC_WAN_ATTR_CHILD_PREFIX_BITS              "ChildPrefixBits"

#define SVC_WAN_ATTR_V8021P_ENABLE					"v8021PEnable"
#define SVC_WAN_ATTR_SDN							"sdn"

#define SVC_WAN_ATTR_PING_RESPONSE                  "PingResponse"
#define SVC_WAN_ATTR_NPT                 			"NPT"


/*************************************************************************************************/
/*************************************************************************************************/

int svc_wan_cfg_get_system_protocol_version(void);

int svc_wan_load_cfg(char* path,wan_entry_cfg_t* cfg, wan_entry_obj_t* obj);

int svc_wan_load_opt(wan_entry_obj_t* obj);

int svc_wan_is_cfg_equal(wan_entry_cfg_t* dst, wan_entry_cfg_t* src);

int svc_wan_check_cfg(wan_entry_cfg_t* cfg);

void dump_svc_wan_cfg(wan_entry_cfg_t* cfg, char is_bridge, char is_msg);

void svc_wan_cfg_update_attr(char* path, wan_entry_cfg_t* cfg, char active);

void svc_wan_set_intf_name(wan_entry_obj_t* obj);

char is_commit_from_tr069(void);

#endif



