#ifndef __SVC_CENTRAL_CTRL_DEVICE_ALARM_H__
#define __SVC_CENTRAL_CTRL_DEVICE_ALARM_H__


#define  TMP_CPUS_USAGE_PATH 	"/tmp/cpu_usage"

void CheckCPUAlarm(void);
#if defined(TCSUPPORT_CT_JOYME4)
void CheckRAMAlarm(int RAMUsage);
#endif
void CalcuCpuUsage(void);

#endif

