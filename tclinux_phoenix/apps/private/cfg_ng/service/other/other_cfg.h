
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

#ifndef __SVC_OTHER_CFG_H__
#define __SVC_OTHER_CFG_H__

#if defined(TCSUPPORT_CT_JOYME2)
/* Speed Test */
#define SPEED_TEST_PATH "/proc/tc3162/speed_test"
#define SPEED_TEST_RESULT_PATH "/proc/tc3162/speedtest_result"
#define SPEED_TEST_TASK(x) doValPut(SPEED_TEST_PATH, x)

#define MAX_BUF_SIZE 2048
#define MAX_LEN_ULONG 10

#define DW_PEAK_RATE "Peak Rate (KB/s):"
#define DW_AVERAGE_RATE "Average Rate(KB/s): "
#define UP_PEAK_RATE "Peak Rate Upload(KB/s):"
#define UP_AVERAGE_RATE "Average Rate Upload(KB/s): "
#define SPEEDTEST_PACKET_CNT "All Session Test Len(Byte): "
#define SPEEDTEST_TOTAL_SUM "Total Receive Len (KB) : "

#endif

#if defined(TCSUPPORT_CMCC)
#define MAX_WAN_PVC_NUMBER		1
#else
#define MAX_WAN_PVC_NUMBER		8
#endif

#ifdef TCSUPPORT_IGMP_SNOOPING
void svc_other_operateIgmpSnooping(void);
#endif
#if defined(TCSUPPORT_CT_BRIDGEARP_NOFWD_LAN)
int svc_other_addBridgeArpRules ();
#endif
#if defined(TCSUPPORT_CMCC)
int svc_other_bsr_control(char* path);
#endif
#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
int svc_other_setBridgeMacLimit();
#endif
int svc_other_handle_event_lan_write();
int svc_other_handle_event_lan_execute(char* path);
int svc_other_handle_event_lan_boot(char* path);
#if  defined(TCSUPPORT_SAMBA) || defined(TCSUPPORT_SAMBA_IPv6) || defined(TCSUPPORT_SAMBA_4)
int svc_other_handle_event_samba_write(int flag, char* path);
int svc_other_handle_event_samba_execute(char* path);
int svc_other_handle_event_samba_boot(char* path);
#endif

int svc_other_handle_event_upnpd_write(char* path);
int svc_other_handle_event_upnpd_boot(void);
int svc_other_handle_event_upnpd_execute(char* path);
#if defined(TCSUPPORT_CT_JOYME2)
void svc_other_handle_event_speedtest_start();
int creat_other_cfg_speedtest_pthread();
int svc_other_handle_event_lanhost2_config();
void init_lanhost2black_node(void);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
int svc_other_handle_event_bridgedata_update(void);
#endif
#if defined(TCSUPPORT_CUC)
int svc_other_handle_event_speedtest_execute(char* path);
#endif
int svc_other_handle_event_syslog_boot(void);
int svc_other_handle_event_syslog_update(char* path);
#endif



