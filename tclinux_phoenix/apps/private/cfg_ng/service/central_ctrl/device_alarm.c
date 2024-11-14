#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include <utility.h>
#include "central_ctrl_common.h"
#include "device_alarm.h"
#if defined(TCSUPPORT_CT_JOYME4)
#include <syslog.h>
#endif

/**********************************************************************/
struct stats 
{
        unsigned int    user;
        unsigned int    nice;
        unsigned int    system;
        unsigned int    idle;
        unsigned int    iowait;
        unsigned int    irq;
        unsigned int    softirq;
        unsigned int    total;
};

/**********************************************************************/
void CheckCPUAlarm(void)
{
	static unsigned int iCount = 0;
	unsigned int CpuUsage	= 0;
	char buf[16]		= {0};

	if(cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "CpuUsage", buf, sizeof(buf)) > 0)
	{
		CpuUsage = atoi(buf);
	}
	
	if(CpuUsage > 95)
	{
		iCount++;		
		SVC_CENTRAL_CTRL_DEBUG_INFO("CpuUsage State: %d \n", CpuUsage);
	}
	else
	{
		iCount = 0;		
		DelAlarmNumber("104030");
	}
	if(iCount>300)  //5 min
	{		
		AddAlarmNumber("104030");
#if defined(TCSUPPORT_CT_JOYME4)
		openlog("TCSysLog alarm", 0, LOG_LOCAL2);
		syslog(LOG_ALERT,"id:104030 CPU Usage more than 95%% for 5 miniutes\n");
		closelog();
#endif
	}

	return ;
	}

#if defined(TCSUPPORT_CT_JOYME4)
void CheckRAMAlarm(int RAMUsage)
{
	static unsigned int iCount = 0;

	if( RAMUsage > 95 )
	{
		iCount++;
	}
	else
	{
		iCount = 0;
		DelAlarmNumber("104036");
	}
	if(iCount>300)  //5 min
	{		
		AddAlarmNumber("104036");
		openlog("TCSysLog alarm", 0, LOG_LOCAL2);
		syslog(LOG_ALERT,"id:104036 RAM Usage more than 95%% for 5 miniutes\n");
		closelog();
	}
	return ;
}
#endif

void CalcuCpuUsage(void)
{
	static struct stats stat, stold;
	char value[80];
	char temp[80];
	unsigned int curtotal;
	char buf[64];
	unsigned int CpuUsage;
	memset(value,0, sizeof(value));
	memset(temp,0, sizeof(temp));
	file_read("/proc/stat", temp, sizeof(temp) - 1);
	temp[sizeof(temp) - 1] = '\0';
	if(0 == strlen(temp))
	{
		return;
	}

	sscanf(temp, "%s %d %d %d %d %d %d %d", value, &stat.user, &stat.nice,&stat.system, \
			&stat.idle, &stat.iowait, &stat.irq, &stat.softirq);
	
	stat.total = stat.user + stat.nice + stat.system + stat.idle + stat.iowait + stat.irq + stat.softirq;

	curtotal = stat.total - stold.total;
	if(0 == curtotal)
	{
		return;
	}

	CpuUsage = ((stat.total - stat.idle) - (stold.total - stold.idle))*100/curtotal;
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d", CpuUsage);
	
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "CpuUsage", buf);
	
	file_write(TMP_CPUS_USAGE_PATH, buf, sizeof(buf));
	
	memcpy(&stold, &stat, sizeof(stat));
	
	return ;
}



