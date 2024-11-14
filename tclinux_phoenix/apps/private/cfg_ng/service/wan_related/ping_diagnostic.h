#ifndef __SVC_WAN_RELATED_PING_DIAGNOSTIC_H__
#define __SVC_WAN_RELATED_PING_DIAGNOSTIC_H__

#define CTCOM_MAX_IPPINGDIAGNOSTIC_NUM 3
#define CTCOM_PING_PID_PATH "/tmp/cwmp/ct_ping%d.pid"
void recheckPingDiagnostic(int if_index);


#endif

