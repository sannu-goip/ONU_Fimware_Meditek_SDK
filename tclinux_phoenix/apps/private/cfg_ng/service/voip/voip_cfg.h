
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

#ifndef __SVC_VOIP_CFG_H__
#define __SVC_VOIP_CFG_H__

#define     IAD_SIP_LINE_ID         0
#define     IAD_TEST_RESULT_PATH    "/tmp/IADTestResult"
#define     IAD_TIMEOUT             (60 * 2)

typedef enum {
    IAD_ERROR_NONE,
    IAD_INNER_ERROR,        /* 1- IAD inner error */
    IAD_ROUTE_FAIL,
    IAD_SERVER_UNREACHABLE,
    IAD_ACCOUNT_FAIL,
    IAD_OTHER_ERROR,        /* 5- other error*/
    IAD_ERRORMAX,
}IAD_ERROR_E;

enum simulate_test_type{
	SIMULATE_NONE,
	SIMULATE_CALLER,
	SIMULATE_CALLED,
	SIMULATE_MAX,
};

#if defined(TCSUPPORT_ECN_MEGACO)
int setVoIPWanIP(int if_index,int if_state);
void _mgappRestart(void);
void mgappKeepAlive(void);
int megaco_start_upgrade_wan_up(char* path);
int megaco_start_upgrade_wan_down(char* path);
#endif

int restart_sip_when_sip_down();
void restart_sip_when_line_unreg();
int voipStatUpgrade(void);
void update_sip_reg_param(int lineNum, int if_index_used, char* ifName, char* wanPath, int wanBindChange);
void get_reg_param_from_wan_info(char* wanPath, int bind_if_index, int lineNum, int useCurIfIndex);
int sip_start_upgrade_wan_up(char* path);
int sip_start_upgrade_wan_down(char* path);
#endif

