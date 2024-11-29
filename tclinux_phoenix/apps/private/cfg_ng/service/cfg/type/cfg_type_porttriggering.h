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

#ifndef __CFG_TYPE_PORTTRIGGERING_H__
#define __CFG_TYPE_PORTTRIGGERING_H__

#define MAX_PORTTRIGGERING_NODE        8
#define MAX_VALUE_SIZE	               64
#define TOTAL_PORT_TRIGGER_PROTO	   2
#define CMD_MAX_SIZE				   300
#define TCP_UDP				 		   "TCP/UDP"
#define PORT_TRIGGER_SH_PATH 		   "/var/tmp/porttrigger.sh"
#define PREROUTING_RULE_CMD_FORMAT     "iptables -t nat -A PREROUTING_RULE -i br0 -p %s --dport %s:%s -j TRIGGER --trigger-type out --trigger-proto %s --trigger-match %s-%s --trigger-relate %s-%s\n"
#define PREROUTING_RULE_CMD_FORMAT2    "iptables -t nat -A PREROUTING_RULE -i br0 -p %s --dport %s:%s -j TRIGGER --trigger-type out  --trigger-match %s-%s --trigger-relate %s-%s\n"
#define PREROUTING_WAN_CMD_FORMAT      "iptables -t nat -A PREROUTING_WAN -p %s --dport %s:%s -j TRIGGER --trigger-type dnat\n"
#define FORWARD_WAN_CMD_FORMAT         "iptables -t filter -A FORWARD_WAN -p %s --dport %s:%s -j TRIGGER --trigger-type in\n" 

#define SKIP_FOR_MERGE 				   1		/*merge this rule with TCP or UDP to all*/
#define USE_MERGE_RULE 				   2		/*use all protocol to merge TCP&UDP rule*/

cfg_node_type_t cfg_type_portTriggering;
#endif
