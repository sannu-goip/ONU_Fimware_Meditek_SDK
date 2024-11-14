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

#ifndef __CFG_TR181_UTILITY_H__ 
#define __CFG_TR181_UTILITY_H__ 

#include <netdb.h>

/*a*/
#if defined(TCSUPPORT_CT_DSL_EX)
#define TR181_ATMLINK_NODE							"root.atmlink"
#define TR181_ATMLINK_ENTRY_NODE					"root.atmlink.entry.%d"

#define TR181_PTMLINK_NODE							"root.ptmlink"
#define TR181_PTMLINK_ENTRY_NODE					"root.ptmlink.entry.%d"

#define TR181_DSL_CHANNEL_NODE			        	"root.dslchannel"
#define TR181_DSL_CHANNEL_ENTRY_NODE			    "root.dslchannel.entry.%d"

#define TR181_DSL_LINE_NODE			        		"root.dslline"
#define TR181_DSL_LINE_ENTRY_NODE			    	"root.dslline.entry.%d"
#endif

/*b*/
#define TR181_BRIDGE_COMMON_NODE					"root.bridge.common"
#define TR181_BRIDGE_PORT_NODE						"root.bridgeport"
#define TR181_BRIDGEPORT_COMMON_NODE				"root.bridgeport.common"
#define TR181_BRIDGEPORT_ENTRY_0_NODE				"root.bridgeport.entry.1"
#define TR181_BRIDGEPORT_ENTRY_NODE					"root.bridgeport.entry.%d"

/*d*/
#define TR181_DEVINF_PS_NODE	    				"root.devinfps"
#define TR181_DEVINF_PS_COMMON_NODE	    			"root.devinfps.common"
#define TR181_DEVINF_PS_ENTRY_NODE	    			"root.devinfps.entry.%d"

#define TR181_DEVINF_TEM_NODE						"root.devinftem"
#define TR181_DEVINF_TEM_COMMON_NODE				"root.devinftem.common"
#define TR181_DEVINF_TEM_ENTRY_NODE					"root.devinftem.entry.%d"

#define TR181_DHCPV4_CLIENT_NODE					"root.dhcpv4client"
#define TR181_DHCPV4CLIENT_ENTRY_NODE				"root.dhcpv4client.entry.%d"

#define TR181_DHCPV6_CLIENT_NODE					"root.dhcpv6client"
#define TR181_DHCPV6_CLIENT_COMMON_NODE				"root.dhcpv6client.common"
#define TR181_DHCPV6_CLIENT_ENTRY_NODE				"root.dhcpv6client.entry.%d"

#define TR181_DNS_CLIENT_NODE						"root.dnsclient"
#define TR181_DNS_CLIENT_COMMON_NODE				"root.dnsclient.common"
#define TR181_DNSCLIENT_ENTRY_NODE					"root.dnsclient.entry.%d"

#define TR181_DNS_RELAY_FORWARDING_NODE				"root.dnsforward"
#define TR181_DNS_RELAY_FORWARDING_COMMON_NODE		"root.dnsforward.common"
#define TR181_DNS_RELAY_FORWARDING_ENTRY_NODE		"root.dnsforward.entry.%d"

#define TR181_DHCPV4_RELAY_COMMON_NODE         		"root.dhcpv4rly.common"
#define TR181_DHCPV4_RELAY_ENTRY_NODE				"root.dhcpv4rly.entry.%d"

#define TR181_DHCPV4_SERVER_COMMON_NODE         	"root.dhcpv4server.common"

#define TR181_DHCPV4_SVRPOOL_ENTRY_NODE         	"root.dhcpv4sp.entry.%d"

#define TR181_DHCPV4_SVRPOLOPT_ENTRY_NODE  	    	"root.dhcpv4spo.entry.%d"

#define TR181_DHCPV4_SVRPOLSTAADRC_ENTRY_NODE   	"root.dhcpv4sps.entry.%d"

#define TR181_DHCPV4_SVRPOLC_ENTRY_NODE 			"root.dhcpv4spc.entry.%d"

#define TR181_DHCPV6_SERVER_COMMON_NODE             "root.dhcpv6server.common"

#define TR181_DHCPV6_SVRPOOL_ENTRY_NODE         	"root.dhcpv6sp.entry.%d"

/*e*/
#define TR181_ETHERVLANT_NODE						"root.ethervlant"
#define TR181_ETHERVLANT_ENTRY_NODE					"root.ethervlant.entry.%d"

#define TR181_ETHERLINK_NODE						"root.etherlink"
#define TR181_ETHERLINK_ENTRY_NODE					"root.etherlink.entry.%d"

#define TR181_ETHER_INTERFACE_NODE			        "root.etherinterface"
#define TR181_ETHER_INTERFACE_ENTRY_NODE			"root.etherinterface.entry.%d"

/*f*/
#define TR181_FIREWALLCH_NODE							"root.firewallch."
#define TR181_FIREWALLCH_COMMON_NODE				"root.firewallch.common"
#define TR181_FIREWALLCH_PVC_0_NODE					"root.firewallch.pvc.1"  		/* MAC filter */
#define TR181_FIREWALLCH_PVC_1_NODE					"root.firewallch.pvc.2"		/*IP up filter */
#define TR181_FIREWALLCH_PVC_2_NODE					"root.firewallch.pvc.3"		/*IP down filter */
#define TR181_FIREWALLCH_PVC_3_NODE					"root.firewallch.pvc.4"		/*ACL filter */
#define TR181_FIREWALLCH_PVC_ENTRY_NODE				"root.firewallch.pvc.%d.entry.%d"
#define TR181_FIREWALL_ENTRY_NODE						"root.tr181firewall.entry"

/*i*/
#define TR181_INTERFACE_STACK_NODE					"root.interfacestack"
#define TR181_INTERFACE_STACK_COMMON_NODE			"root.interfacestack.common"
#define TR181_INTERFACE_STACK_ENTRY_NODE			"root.interfacestack.entry.%d"

/*n*/
#define TR181_NAT_INTERFACE_NODE					"root.natinterface"
#define TR181_NAT_INTERFACE_ENTRY_NODE				"root.natinterface.entry.%d"

#define TR181_NATPORTMAPPING_NODE					"root.natportmap"
#define TR181_NATPORTMAPPING_COMMON_NODE			"root.natportmap.common"
#define TR181_NATPORTMAPPING_ENTRY_NODE				"root.natportmap.entry.%d"

/*o*/
#define TR181_OPTICAL_ENTRY_0_NODE					"root.opticalinter.entry.1"

/*r*/
#define TR181_ROUTE_IPV4_FORWARDING_NODE			"root.routeforward4"
#define TR181_ROUTE_IPV4_COMMON_NODE				"root.routeforward4.common"
#define TR181_ROUTE_IPV4_ENTRY_NODE					"root.routeforward4.entry.%d"

#define TR181_ROUTE_IPV6_FORWARDING_NODE			"root.routeforward6"
#define TR181_ROUTE_IPV6_COMMON_NODE				"root.routeforward6.common"
#define TR181_ROUTE_IPV6_ENTRY_NODE					"root.routeforward6.entry.%d"

#define TR181_ROUTEADVIFSET_COMMON_NODE				"root.routeadvifset.common"
#define TR181_ROUTEADVIFSET_NODE					"root.routeadvifset"
#define TR181_ROUTEADVIFSET_ENTRY_NODE				"root.routeadvifset.entry.%d"

/*t*/
#define TR181_COMMON_NODE							"root.tr181.common"

#define TR181_IP_COMMON_NODE						"root.tr181ip.common"
#define TR181_IP_NODE								"root.tr181ip"
#define TR181_IP_ENTRY_NODE							"root.tr181ip.entry.%d"

#define TR181_IPACTIVEPORT_NODE         	    	"root.tr181ipap"
#define TR181_IPACTIVEPORT_COMMON_NODE   	    	"root.tr181ipap.common"
#define TR181_IPACTIVEPORT_ENTRY_NODE    	    	"root.tr181ipap.entry.%d"

#define TR181_IP_INTERFACE_IPV4ADDR_NODE			"root.tr181ipv4addr"
#define TR181_IP_INTERFACE_IPV4ADDR_PVC_NODE		"root.tr181ipv4addr.pvc.%d"
#define TR181_TR181IPV4ADDR_PVC_ENTRY_NODE			"root.tr181ipv4addr.pvc.%d.entry.%d"

#define TR181_IP_INTERFACE_IPV6ADDR_NODE			"root.tr181ipv6addr"
#define TR181_IP_IPV6ADDR_PVC_NODE					"root.tr181ipv6addr.pvc.%d"
#define TR181_TR181IPV6ADDR_PVC_ENTRY_NODE			"root.tr181ipv6addr.pvc.%d.entry.%d"

#define TR181_IP_INTERFACE_IPV6PREF_NODE			"root.tr181ipv6pre"
#define TR181_TR181IPV6PRE_PVC_NODE					"root.tr181ipv6pre.pvc.%d"
#define TR181_IPV6PRE_PVC_ENTRY_NODE				"root.tr181ipv6pre.pvc.%d.entry.%d"

#define TR181_ROUTE_COMMON_NODE						"root.tr181route.common"
#define TR181_ROUTE_ENTRY_0_NODE					"root.tr181route.entry.1"

#define TR181_PPP_NODE								"root.tr181ppp"
#define TR181_PPP_ENTRY_NODE						"root.tr181ppp.entry.%d"

#define TR181_TQOS_COMMON_NODE				"root.tqos.common"
#define TR181_TQOSAPP_ENTRY_NODE					"root.tqosapp.entry.%d"
#define TR181_TQOSCLSFICN_COMMON_NODE				"root.tqosclsficn.common"
#define TR181_TQOSCLSFICN_ENTRY_NODE					"root.tqosclsficn.entry.%d"
#define TR181_TQOSPOLICER_ENTRY_NODE					"root.tqospolicer.entry.%d"
#define TR181_TQOSQUE_ENTRY_NODE					"root.tqosque.entry.%d"

#define TR181_DSLITE_NODE 							"root.tr181dslite"
#define TR181_DSLITE_COMMON_NODE 					"root.tr181dslite.common"
#define TR181_DSLITE_ENTRY_NODE 					"root.tr181dslite.entry.%d"


/*u*/
#define TR181_USERS_ENTRY_NODE      	   	    	"root.users.entry.%d"

/*w*/
#define TR181_WIFIAPSEC_NODE						"root.wifiapsec"
#define TR181_WIFIAP_NODE							"root.wifiap"
#define TR181_WIFIRDO_NODE							"root.wifirdo"
#define TR181_WIFISSID_NODE							"root.wifissid"
#define TR181_WIFIAPWPS_NODE						"root.wifiapwps"

#define TR181_WIFIAPSEC_ENTRY_NODE					"root.wifiapsec.entry.%d"
#define TR181_WIFIAP_ENTRY_NODE						"root.wifiap.entry.%d"
#define TR181_WIFIRDO_ENTRY_NODE					"root.wifirdo.entry.%d"
#define TR181_WIFISSID_ENTRY_NODE					"root.wifissid.entry.%d"
#define TR181_WIFIAPWPS_ENTRY_NODE					"root.wifiapwps.entry.%d"

#endif 

