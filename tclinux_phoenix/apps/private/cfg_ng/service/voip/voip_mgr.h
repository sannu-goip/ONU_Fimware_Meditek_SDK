#ifndef __SVC_VOIP_MGR_H__
#define __SVC_VOIP_MGR_H__

#include "voip_cfg.h"

#define VOIPSYSPARAM                   "voipsysparam"
#define VOIPADVANCED                   "VoIPAdvanced"
#define VOIPBASIC                      "voipbasic"
#define VOIPCALLCTRL                   "VoIPCallCtrl"
#define VOIPMEDIA                      "VoIPMedia"
#define VOIPSPEED                      "voipspeed"
#define VOIPCODECS                     "VoIPCodecs"
#define VOIPDIGITMAP                   "VoIPDigitMap"
#define VOIPTEST                       "voiptest"
#define VOIPENTRY                      "Entry"
#define VOIPSIMULATETEST               "voipsimulatetest"

#define MAX_VALUE_SIZE	               64
#define VOIP_LINE_NUM                  4

#define LINE_CALLER_SIMULATE           "Caller"
#define LINE_CALLED_SIMULATE           "Called"

#define LINE_TEST_PROC_NAME            "/proc/fxs/sliclinetest"
#define LINE_TEST_PHONE_CONN           "PhoneConnectivityTest"
#define LINE_TEST_TID_LINE_V           "LineVoltageTest"     
#define LINE_TEST_TID_RES_FLT          "LineResistiveTest"

#define MANDATORY_REGISTER             "MandatoryRegister"
#define MANDATORY_REGISTER_FLAG        "MandatoryRegisterFlag"
#define MGC_CONTROLER                  "MediaGatewayControler"
#define MGC_SB_CONTROLER               "SBMediaGatewayControler"
#define MAX_VOIP_WAN 4   /*voip max wan number is 4*/


typedef struct voipRegInfo
{
	int  voipPrevRegFlag[VOIP_LINE_NUM];/*0:down, 1:up*/
	long voipPrevCurTime[VOIP_LINE_NUM];
	unsigned int voipServerDownTime[VOIP_LINE_NUM];
}voipRegInfo_t;

typedef struct VoIP_Wan_info{
	int event;
	char evt_buf[64]; 
}VoIP_Wan_info_t;


int svc_voip_sip_update_handle(char* path);
int svc_voip_sip_normal_handle_event_update(char* path);
#if defined(TCSUPPORT_ECN_MEGACO)
int svc_voip_h248_normal_handle_event_update(char* path);
#endif
int svc_voip_mgr_handle_event(int event, void* evt_buf);
int svc_voip_mgr_handle_wan_event(int event, void* evt_buf);
void svc_voip_mgr_handle_boot2_ready(void);
#endif

