
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

#ifndef __SVC_VOIP_COMMON_H__
#define __SVC_VOIP_COMMON_H__

#include "cfg_msg.h"

/********************************************************debug*********************************************************************/
extern int g_svc_voip_level;

enum _svc_voip_dbg_level_
{
    E_NO_INFO_LEVEL         = 0,
    E_ERR_INFO_LEVEL        = 1,
    E_WARN_INFO_LEVEL       = 2, 
    E_CRITICAL_INFO_LEVEL   = 4,
    E_NOTICE_INFO_LEVEL     = 8,
    E_TRACE_INFO_LEVEL      = 16,
    E_DBG_INFO_LEVEL        = 32,
};

#define SVC_VOIP_ERROR_INFO(fmt, ...)  do{ \
                                            if(g_svc_voip_level & E_ERR_INFO_LEVEL)\
                                                tcdbg_printf("Error info, func: %s, line:%d  " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_VOIP_CRITIC_INFO(fmt, ...) do{ \
                                            if(g_svc_voip_level & E_CRITICAL_INFO_LEVEL)\
                                                tcdbg_printf("Critic info, func: %s, line:%d  " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_VOIP_NOTICE_INFO(fmt, ...) do{ \
                                            if(g_svc_voip_level & E_NOTICE_INFO_LEVEL) \
                                                tcdbg_printf("Notice info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_VOIP_WARN_INFO(fmt, ...)  do{ \
                                            if(g_svc_voip_level & E_WARN_INFO_LEVEL) \
                                                tcdbg_printf("Warn info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_VOIP_DEBUG_INFO(fmt, ...)  do{ \
                                            if(g_svc_voip_level & E_DBG_INFO_LEVEL) \
                                                tcdbg_printf("Debug info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

#define SVC_VOIP_TRACE_INFO(fmt, ...)  do{ \
                                            if(g_svc_voip_level & E_TRACE_INFO_LEVEL) \
                                                tcdbg_printf("Trace info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
                                        }while(0)

/*************************************************************************************************************************************/


											
#ifdef VOIP_MAX_CHAN
#define MAX_DSP_LINE VOIP_MAX_CHAN    /* VOIP_MAX_CHAN */
#else
#define MAX_DSP_LINE 4                /* VOIP_MAX_CHAN */
#endif
											
#define VOICE_IAD_ERROR                "1"
#define VOICE_ROUTE_ERROR              "2"
#define VOICE_RESPONSE_ERROR           "3"
#define VOICE_USER_PW_ERROR            "4"
#define VOICE_UNKNOWN_ERROR            "5"
#define VOICE_NO_WAN_ERROR             "6"


int svc_voip_execute_cmd(char* cmd);
void svc_voip_debug_level(int level);
int svc_voip_update_line_num();

#endif


