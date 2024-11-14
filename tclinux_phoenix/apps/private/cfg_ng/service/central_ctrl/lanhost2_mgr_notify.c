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

/************************************************************************
*                  I N C L U D E S
*************************************************************************/
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_lanhost.h>
#include "lanhost2_mgr_notify.h"
#include "utility.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************/

/************************************************************************
*                  M A C R O S
*************************************************************************/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************/
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
extern int g_iswlan_dev_leave;
#endif
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************/
void ctc_lan_host2_update(struct ecnt_event_data *event_data)
{
	char node_name[32] = {0};
	char type[256] = {0}, lanport[64] = {0};
	char *data = NULL;
	int idx = 0, i_port = 0;

	data = (char*)event_data;
	sscanf(data, "%[^;];%d", type, &idx);
	snprintf(node_name, sizeof(node_name), LANHOST2_ENTRY_NODE, idx + 1);
	if( cfg_get_object_attr(node_name, "Port", lanport, sizeof(lanport)) > 0 )
		i_port = atoi(lanport);

	switch(data[0])
	{
		case 'a':
		case 'd':
#if !defined(TCSUPPORT_CT_DBUS)
		case 'u':
#endif
			/* update obj
			tcdbg_printf("\n LAN%d update. \n", i_port); */
#if defined(TCSUPPORT_CWMP_TR181)
			updateDHCPv4Spc();
#endif
			signalCwmpInfo(HOST_REINIT, 0, 0);
			break;
		default:
			break;
	}
}

#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
void ctc_wifi_update(struct ecnt_event_data *event_data)
{
    char *data = NULL;
	
	data = (char *)event_data;
	
	if ( 'd' == data[0] )
	{	
		g_iswlan_dev_leave = 1;
	}
}
#endif
