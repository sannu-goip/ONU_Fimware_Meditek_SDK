#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "check_button.h"
#include "device_alarm.h"
#include "central_ctrl_common.h"
#include "central_ctrl_polling.h"
#include "check_ping_diagnostic.h"
#include "lan_port_link.h"
#include "cfg_cli.h"
#include <svchost_evt.h>
#include "cfg_msg.h"
#include <time.h>
#include "utility.h"
#include "check_vpn_ppptunnels.h"
#if defined(TCSUPPORT_CT_JOYME2)
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include "libapi_lib_cputempmgr.h"
#include <dirent.h>
#endif
#include "blapi_traffic.h"
#include "blapi_system.h"
#include <sys/vfs.h>
#include "cfg_type_transferservices.h"
#if defined(TCSUPPORT_ECNT_MAP)
#include "uloop_timer.h"
#endif
#if defined(TCSUPPORT_CWMP_TR181)
#include "tr181/cfg_tr181_global.h"
#include "tr181/cfg_tr181_utility.h"
#endif
#include <syslog.h>

#define MAX_NTP_SYNC_TIME 		24*60*60
#define NTPTRYTIME 				60
#define NTPTRYMAXNUMBER 		10
unsigned int ntpnumber = 0;
#define NTPFLAGWITHNOEXECUTE 	0
#define NTPFLAGWITHEXECUTE 		1
#define NTPFLAGWITHSYNC 		2
#define NTPDELAYTIME 			5	/*max count after wan side up and get ip*/
#define NTPTRYMINTIME 			30
#define	PERCENT_CHECK_CNT					5
#define	PERCENT_GET_PERIOD					60

#define DHCPD_PATH 			"/etc/udhcpd.conf"
#define DPROXYAUTO_PATH 	"/etc/dproxy.auto"
#define DHCPRELAY_PATH 		"/etc/config/dhcprelay.sh"


#if defined(TCSUPPORT_CT_JOYME2)
int resumemem_times = 0;
int itvcheck_times = 0;
#endif

struct timespec req_time_pre,req_time_cur,req_time_diff;
void initNTPTimerStruct()
{
	memset(&req_time_pre,0,sizeof(req_time_pre));
	memset(&req_time_cur,0,sizeof(req_time_cur));
	memset(&req_time_diff,0,sizeof(req_time_diff));
	return;
}

static int cfg_type_timezone_entry_func_commit(char* path)
{
	wan_related_evt_t param;
	memset(&param, 0, sizeof(param));
	strncpy(param.buf, path, sizeof(param)-1);
	cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_TIMEZONE_UPDATE, (void *)&param, sizeof(param)); 
	return 0;
} 

void ntpexecute(void)
{
	char nodebuf[64] = {0};
	char tz_type[4] = {0};
	char syncbuf[10] = {0};
	unsigned int synctime = MAX_NTP_SYNC_TIME;
	static unsigned char ntpsta = 1;
	unsigned int synctrytime = NTPTRYTIME, ntptrynumber_max = NTPTRYMAXNUMBER;
	static unsigned int ntptrynumber = 0;
	char ntpdelayflag1str[18] = {0};
	char ntpdelayflag2str[18] = {0};
	int ntpdelayflag1 = 0;
	int ntpdelayflag2 = 0;
	char ntpneedexecute[4] = {0};

	snprintf(nodebuf, sizeof(nodebuf), TIMEZONE_ENTRY_NODE);
	cfg_get_object_attr(nodebuf, "TYPE", tz_type, sizeof(tz_type));
	if(atoi(tz_type) != 0)
	{
		cfg_set_object_attr(GLOBALSTATE_NODE, "ntpneedexecute", "2");
		ntpnumber = 0;
		initNTPTimerStruct();	
		ntpsta = 0;
		ntptrynumber = 0;
		return;	
	}
	
	if(req_time_pre.tv_sec == 0)
	{
		clock_gettime(CLOCK_MONOTONIC,&req_time_pre);
	}
	
	cfg_get_object_attr(GLOBALSTATE_NODE, "ntpdelayflag1", ntpdelayflag1str, sizeof(ntpdelayflag1str));
	cfg_get_object_attr(GLOBALSTATE_NODE, "ntpdelayflag2", ntpdelayflag2str, sizeof(ntpdelayflag2str));
	ntpdelayflag1 = atoi(ntpdelayflag1str);
	ntpdelayflag2 = atoi(ntpdelayflag2str);

	if(0 != ntpdelayflag1 || 0 != ntpdelayflag2)
	{
		cfg_get_object_attr(GLOBALSTATE_NODE, "ntpneedexecute", ntpneedexecute, sizeof(ntpneedexecute));
		if(NTPFLAGWITHEXECUTE == atoi(ntpneedexecute))
		{
			ntpnumber++;
			if(ntpnumber >= NTPDELAYTIME)
			{
				/*sprintf(nodebuf,"%s_%s",TIMEZONE,SUB_NODE_NAME);*/
				cfg_type_timezone_entry_func_commit(nodebuf);
				ntpnumber = 0;
				cfg_set_object_attr(GLOBALSTATE_NODE, "ntpneedexecute", "2");
			}
		}
		else if(NTPFLAGWITHSYNC == atoi(ntpneedexecute))
		{
			/*sprintf(nodebuf,"%s_%s",TIMEZONE,SUB_NODE_NAME);*/
			cfg_get_object_attr(nodebuf, "SYNCTIME", syncbuf, sizeof(syncbuf));
			if(0 != atoi(syncbuf))
			{
#if defined(TCSUPPORT_CT_2NTP)
				synctime = atoi(syncbuf);
#else
				synctime = atoi(syncbuf)*60;
#endif
			}
			
			if(ntpsta)
			{
				memset(syncbuf, 0, sizeof(syncbuf));
				cfg_get_object_attr(nodebuf, "SyncTryMaxNum", syncbuf, sizeof(syncbuf));
				if(0 != atoi(syncbuf))
					ntptrynumber_max = atoi(syncbuf);

				memset(syncbuf, 0, sizeof(syncbuf));
				cfg_get_object_attr(nodebuf, "SyncResult", syncbuf, sizeof(syncbuf));

				if((strcmp(syncbuf, "1")==0) || (ntptrynumber>=ntptrynumber_max))
				{
					ntpsta = 0;
					ntptrynumber = 0;
				}
				else
				{
					memset(syncbuf, 0, sizeof(syncbuf));
					cfg_get_object_attr(nodebuf, "SyncTryTime", syncbuf, sizeof(syncbuf));
					if(atoi(syncbuf) >= NTPTRYMINTIME)
					{
#if defined(TCSUPPORT_CT_2NTP)
						synctrytime = atoi(syncbuf);
#else
						synctrytime = atoi(syncbuf)*60;
#endif
					}
					if(synctrytime < synctime)
						synctime = synctrytime;
				}
				
			}
			
			clock_gettime(CLOCK_MONOTONIC,&req_time_cur);	
			req_time_diff.tv_sec = req_time_cur.tv_sec - req_time_pre.tv_sec;
			req_time_diff.tv_nsec += req_time_cur.tv_nsec - req_time_pre.tv_nsec;
			while(req_time_diff.tv_nsec > 1000000000)
			{
				req_time_diff.tv_sec++;
				req_time_diff.tv_nsec -= 1000000000;
			}
			if(req_time_diff.tv_nsec < 0)
			{
				req_time_diff.tv_sec--;
				req_time_diff.tv_nsec += 1000000000;
			}
			if(req_time_diff.tv_sec >= synctime)
			{
				cfg_type_timezone_entry_func_commit(nodebuf);
				initNTPTimerStruct();	
				synctime = MAX_NTP_SYNC_TIME;
				if(ntpsta)
					ntptrynumber ++;
			}
		}
	}

	return;
}


int checkUILockState()
{
	char lock_state[4] = {0};
	int isLock = 0;
	struct timespec curtime;
	static struct timespec prev_time;
	static int isStoreTime = 0;
	char buf[8] = {0};
	
	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "isLockState", lock_state, sizeof(lock_state)) <= 0)
		return -1;
	
	isLock = atoi(lock_state);
	if ( isLock )
	{
		if ( !isStoreTime )
		{
			clock_gettime(CLOCK_MONOTONIC, &prev_time);
			isStoreTime = 1;
		}
		
		clock_gettime(CLOCK_MONOTONIC, &curtime);
		if ( (curtime.tv_sec - prev_time.tv_sec)  >= 60 )
		{
			isStoreTime = 0;
			cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "isLockState", "0");
#if defined(TCSUPPORT_CT_JOYME4)
#ifdef TCSUPPORT_SYSLOG_ENHANCE
			if ( cfg_obj_get_object_attr(SYSLOG_ENTRY_NODE, "LogEnable", 0, buf, sizeof(buf)) > 0 \
				&& !strncmp(buf,"Yes", sizeof(buf)) )
			{
				openlog("TCSysLog Dbus", 0, LOG_LOCAL2);
				syslog(LOG_INFO, "Web unlocked.\n");
				closelog();
			}
#endif
#endif
		}
	}
	
	return 0;
}

void save_PWR_reboot_info()
{
#if defined(TCSUPPORT_CT_JOYME2)
	FILE *fp = NULL;
	char buf[256] = {0}, source[16] = "Power";
	int year = 0, mon = 0, day = 0, hour = 0, min = 0, sec = 0;
	struct timespec curtime;
	struct tm nowTime;
	char now_time[20] = {0}, ntp_state[24] = {0}, pmboot[24] = {0};
#define FILE_PWR_REBOOT_SRC		"/opt/upt/apps/info/pwr_reboot_source"
	const char* json_format = "{\"Time\":\"%s\","
							  "\"Source\":\"%s\"}";
	static int timeout_delay = 0, is_first_boot = 0;

	/* check NTP state. */
	cfg_get_object_attr(PMINFORM_ENTRY_NODE, "NTPSync", ntp_state, sizeof(ntp_state));
	if ( 0 != strcmp(ntp_state, "Yes") )
		return;

	if ( 0 == is_first_boot )
	{
		cfg_get_object_attr(PMINFORM_ENTRY_NODE, "Boot", pmboot, sizeof(pmboot));
		if ( 0 != strcmp(pmboot, "Yes") )
			return;
		cfg_set_object_attr(PMINFORM_ENTRY_NODE, "Boot", "");

		is_first_boot = 1;
	}
	else
	{
		if ( ++ timeout_delay < 60 )
		{
			return;
		}

		timeout_delay = 0;
	}
	/* get nowtime */
	clock_gettime(CLOCK_REALTIME, &curtime);
	localtime_r(&curtime.tv_sec, &nowTime);
	year = nowTime.tm_year + 1900;
	mon = nowTime.tm_mon + 1;
	day = nowTime.tm_mday;
	hour = nowTime.tm_hour;
	min = nowTime.tm_min;
	sec = nowTime.tm_sec;

	/* add 30 seconds. */
	if ( sec < 30 )
		sec += 30;
	else
	{
		min += 1;
		sec -= 30;
	}

	snprintf(now_time, sizeof(now_time), "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);

	/* write to reboot_info */
	fp = fopen(FILE_PWR_REBOOT_SRC, "w");
	if( NULL == fp ){
		printf("\n file: %s open failed\n", FILE_PWR_REBOOT_SRC);
		return;
	}

	fprintf(fp, json_format, now_time, source);
	fclose(fp);
	sync();
#endif
	return;
}


static int guard_boot = 0;
#if defined(TCSUPPORT_CT_JOYME2)
void send_process_stat_msg(char *service_name)
{
	char msg[256] = {0}, cur_time[64] = {0};
	char reason[128] = {0}, logs[64] = {0};
	int ret = -1;
	FILE *fp = NULL;

	if ( NULL == service_name )
		return;
	if ( 0 == strcmp(service_name, CTC_GW_SERVICE_NAME_WIFI) )
	{
		fp = fopen(WIFI_SEND_FLAG, "w");
		if (fp) fclose(fp);
	}

	get_current_time(cur_time, sizeof(cur_time));
	snprintf(reason, sizeof(reason), "may receive error signal 9, %s will restart.", service_name);
	strncpy(logs, "no error logs", sizeof(logs) - 1);
	
	snprintf(msg, sizeof(msg), "{\"AppName\":\"%s\",\"Time\":\"%s\",\"Reason\":\"%s\",\"Logs\":\"%s\"}",
								service_name, cur_time, reason, logs);
	ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_PROC_STAT,
					msg,
					strlen(msg)+1);


	return;
}

unsigned int get_avg(unsigned int *sample, int cnt)
{
	int i;
	unsigned int avage = 0;

	for (i = 0; i < cnt; i++) {
		avage += sample[i];
	}

	return avage / cnt;
}

unsigned int get_flash_used(void)
{
		struct statfs s;
		long blocks_used;
		long blocks_percent_used = 0;
		int percent_used = 0;
	
		if (statfs("/opt/upt/apps/", &s) != 0) {
			return -1;
		}

		if (s.f_blocks > 0){
			blocks_used = s.f_blocks - s.f_bfree;
			blocks_percent_used = 0;
			if (blocks_used + s.f_bavail) {
				blocks_percent_used = (((long long) blocks_used) * 100 + (blocks_used + s.f_bavail)/2) 
									/ (blocks_used + s.f_bavail);
			}
		}
		percent_used = (int)blocks_percent_used;
		return percent_used;
			
}

unsigned int get_conn_num(void)
{
	FILE *fp = NULL;
	int line = 0,  ret = 0;
	size_t size = 0;
	char *buf = NULL;
	unsigned int conn_num = 0;
	char connbuf[8] = {0};
	
	fp = fopen("/proc/net/stat/nf_conntrack", "r");
	if (NULL == fp) {
		tcdbg_printf("open file fail \n");
		return 0;
	}
	
	while ((ret = getline(&buf, &size, fp)) != -1) {
		if (line == 0) {
			line++;
			continue;
		}
		sscanf(buf, "%x ", &conn_num);
		break;
	}

	snprintf(connbuf, sizeof(connbuf), "%d", conn_num);
	cfg_set_object_attr("root.sys.entry", "CONN_NUM", connbuf);

	if (buf) 
		free(buf);
	if (fp)
		fclose(fp);

	return conn_num;
	
}

unsigned int get_pon_temp(void) 
{
	char temp_buf[128] = {0};
	double pon_temp_d = 0;
	unsigned int pon_temp = 0;
	
	if (0 < cfg_get_object_attr("root.info.ponphy", "Temperature", temp_buf, sizeof(temp_buf)))
		pon_temp_d = atof(temp_buf);
	
	if (pon_temp_d >= pow(2,15))
		pon_temp_d = -round((pow(2,16) - pon_temp_d) / 256);
	else
		pon_temp_d = round(pon_temp_d / 256);

		pon_temp = (unsigned int)pon_temp_d;

	return pon_temp;
}


unsigned int get_cpu_temp(void)
{
	unsigned int cpu_temp = 0;
	char buf[8] = {0};
	cpu_temp_msg_t msg;
	
    cputempmgr_lib_get(&msg);
    cpu_temp = msg.cpu_temp;
	snprintf(buf, sizeof(buf), "%d", cpu_temp);
	cfg_set_object_attr("root.sys.entry", "CPU_TEMP", buf);
	
	return cpu_temp;
}

void check_send_alarm(void)
{
	static int samble_cnt = 0, avg_flag = 0;
    static int index = 0;
	static unsigned int cpu_temp_sample[PERCENT_CHECK_CNT] = {0};
	static unsigned int pon_temp_sample[PERCENT_CHECK_CNT] = {0};
	static unsigned int flash_usage_sample[PERCENT_CHECK_CNT] = {0};
	static unsigned int conn_num_sample[PERCENT_CHECK_CNT] = {0};
	char msg[128] = {0};
	unsigned int avg_pon_temp = 0, avg_flash_useage = 0, avg_conn_num = 0, avg_cpu_temp = 0;
	int ConnNumThreshold = 0;
	char ConnNumThreshold_buf[32] = {0};
	char flash_threshold_buf[32] = {0};
	int flash_threshold = 0;
	char tempthreshold_buf[32] = {0};
	int tempthreshold = 0;
	char cputempthreshold_buf[32] = {0};
	int cputempthreshold = 0;
	int ret = -1;

	if(cfg_get_object_attr("root.sys.entry","ConnNumThreshold",ConnNumThreshold_buf,sizeof(ConnNumThreshold_buf)) == -1)
		return -1;
	    ConnNumThreshold = atoi(ConnNumThreshold_buf);
	
	if (cfg_get_object_attr("root.sys.entry","FlashThreshold", flash_threshold_buf, sizeof(flash_threshold_buf)) == -1)
		return -1;
		flash_threshold = atoi(flash_threshold_buf);
		
	if (cfg_get_object_attr("root.sys.entry","TempThreshold", tempthreshold_buf, sizeof(tempthreshold_buf)) == -1)
		return -1;
		tempthreshold = atoi(tempthreshold_buf);

	if(cfg_get_object_attr("root.sys.entry","CpuTempThreshold", cputempthreshold_buf, sizeof(cputempthreshold_buf)) == -1)
		return -1;
		cputempthreshold = atoi(cputempthreshold_buf);
		
	if (samble_cnt++ >= PERCENT_GET_PERIOD) {
		cpu_temp_sample[index % PERCENT_CHECK_CNT] = get_cpu_temp();
		pon_temp_sample[index % PERCENT_CHECK_CNT] = get_pon_temp();
		flash_usage_sample[index % PERCENT_CHECK_CNT] = get_flash_used();
		conn_num_sample[index % PERCENT_CHECK_CNT] = get_conn_num();
		
		index = index + 1;
		samble_cnt = 0;
		avg_flag = avg_flag + 1;
	}
	
	if (avg_flag >= PERCENT_CHECK_CNT) {
		avg_cpu_temp = get_avg(cpu_temp_sample,PERCENT_CHECK_CNT);
		if (avg_cpu_temp >= cputempthreshold) {
			snprintf(msg, sizeof(msg), "%u", avg_cpu_temp);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSCPUTEMPALARM,
							msg,
							strlen(msg)+1);
			
		}
		
		avg_pon_temp = get_avg(pon_temp_sample, PERCENT_CHECK_CNT);
		if (avg_pon_temp >= tempthreshold) {
			snprintf(msg, sizeof(msg), "%u", avg_pon_temp);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSPONTEMPALARM,
							msg,
							strlen(msg)+1);
			
		}

		avg_flash_useage = get_avg(flash_usage_sample, PERCENT_CHECK_CNT);
		if (avg_flash_useage >= flash_threshold) {
			snprintf(msg,sizeof(msg),"%u",avg_flash_useage);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSWRITABLEFLASHALARM,
							msg,
							strlen(msg)+1);
		}

		avg_conn_num = get_avg(conn_num_sample, PERCENT_CHECK_CNT);
		if (avg_conn_num >= ConnNumThreshold) {
			snprintf(msg,sizeof(msg),"%u",avg_conn_num);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSCONNNUMALARM,
							msg,
							strlen(msg)+1);
		}

		avg_flag = 0;
		index = 0;
	}
}
void check_cpwan_boot_status(void)
{	
	static int check_num = 600; /* 10mins */
	static int check_flag = 1;
	char node[64] = {0}, status[32] = {0};
	char msg[256] = {0}, reason[128] = {0}, cur_time[64] = {0}, inform_time[64] = {0};
	int ret = -1;

	if (check_flag == 1) {
		snprintf(node, sizeof(node), "%s", CWMPROUTE_ENTRY_NODE);
		cfg_get_object_attr(node, "Status", status, sizeof(status));
		if (!strcmp(status, "Up"))
			check_flag = 0;
		check_num--;
	}

	if (check_num == 0 && check_flag == 1) {
		get_current_time(cur_time, sizeof(cur_time));
		get_inform_time(inform_time, sizeof(inform_time));
		inform_time[sizeof(inform_time)-1] = '\0';
		snprintf(reason, sizeof(reason), "{TR069WANStatus:Down,TR069WANIP:%s,LastInformTime:%s}", "", inform_time);
		snprintf(msg, sizeof(msg), "{\"AppName\":\"%s\",\"Time\":\"%s\",\"Reason\":\"%s\",\"Logs\":\"\"}",
								CTC_GW_SERVICE_NAME_TR069C, cur_time, reason);
		ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_PROC_STAT,
					msg,
					strlen(msg)+1);
		check_flag = 0;
	}

	return;
}

#ifdef TCSUPPORT_DMS
extern int cfg_type_dms_func_commit(char* path);
void check_restart_dlna(void)
{
	FILE *fp = NULL;
	char node[32] = {0};
	char buf[32] = {0};
	static int boot_cnt = 0;
	static int dlna_flag = 0;
	
	boot_cnt++;
	
	snprintf(node, sizeof(node), "%s", DMS_ENTRY_NODE);
	cfg_get_object_attr(node, "restart", buf, sizeof(buf));
	
	if (boot_cnt > 60)
	{
		boot_cnt = 61;
		if (!strcmp(buf, "1")) {
			dlna_flag = 0;
			cfg_set_object_attr(node, "restart", "0");
			cfg_type_dms_func_commit(NULL);
		}
		fp = fopen("/tmp/dlna_flag", "r");
		if (fp != NULL)
		{	
			dlna_flag++;
			if(dlna_flag < 60)
			{
				fclose(fp);
				return;
			}
			cfg_set_object_attr(node, "restart", "1");
			
			fclose(fp);
		}
		else
			dlna_flag = 0;	
	}
	return;
}
#endif

#if defined(TCSUPPORT_CT_DBUS)
/*
  type-->  0:dbus-daemon, 1:ctc_igd1_dbus
*/
int chkDbusDaemon(int type)
{
	FILE *fp = NULL;
	char pid_buf[32] = "0", cmd_line_path[64] = {0}, cmd_line[128] = {0};
	char *cmd_chk = NULL;
	int status = 0, pid = 0, res = 0;

	if ( 0 == type )
		fp = fopen("/var/run/dbus.pid", "r");
	else
		fp = fopen("/var/run/ctc_igd1_dbus.pid", "r");

	if ( fp )
	{
		res = fread(pid_buf, sizeof(char), sizeof(pid_buf) - 1, fp);
		fclose(fp);
	}

	pid = atoi(pid_buf);
	if ( pid > 0 )
	{
		snprintf(cmd_line_path, sizeof(cmd_line_path), "/proc/%d/cmdline", pid);
		fp = fopen(cmd_line_path, "r");
		if ( fp )
		{
			res = fread(cmd_line, sizeof(char), sizeof(cmd_line) - 1, fp);
			cmd_line[sizeof(cmd_line) - 1] = '\0';
			fclose(fp);
		}

		if ( 0 == type )
			cmd_chk = strstr(cmd_line, "dbus-daemon");
		else
			cmd_chk = strstr(cmd_line, "ctc_igd1_dbus");

		if ( cmd_chk )
			status = 1;
	}

	if ( 0 == status )
	{
		tcdbg_printf("\nrestart dbus app %s.\n",
				(0 == type) ? "dbus-daemon" : "ctc_igd1_dbus");

		if ( 0 == type )
		{
			system("/usr/bin/killall -9 dbus-daemon");
			unlink("/var/run/dbus.pid");
			system("/usr/sbin/dbus-daemon --system");
		}

		/* check other exit case, for example, signal 9 */
		if (type == 1) {
			fp = fopen(DBUS_SEND_FLAG, "r");
			if (fp == NULL) {
				send_process_stat_msg(CTC_GW_SERVICE_NAME_DBUS);
			}
			else {
				fclose(fp);
				unlink(DBUS_SEND_FLAG);
			}
				
		}

		system("/usr/bin/killall -9 ctc_igd1_dbus");
		unlink("/var/run/ctc_igd1_dbus.pid");
		system("export LD_LIBRARY_PATH=/usr/lib/glib-2.0/ && /userfs/bin/ctc_igd1_dbus &> /dev/null");
		sleep(1);
		system("pidof ctc_igd1_dbus > /var/run/ctc_igd1_dbus.pid");
	}

	return 0;
}

int chkSafProcess(void)
{
	FILE *fp = NULL;
	char pid_buf[32] = "0", cmd_line_path[64] = {0}, cmd_line[128] = {0};
	char msg[256] = {0}, cur_time[64] = {0};
	char *cmd_chk = NULL;
	int status = 0, pid = 0, res = 0, ret = -1;

	fp = fopen("/var/run/ctc_saf.pid", "r");

	if ( fp )
	{
		res = fread(pid_buf, sizeof(char), sizeof(pid_buf) - 1, fp);
		fclose(fp);
	}

	pid = atoi(pid_buf);
	if ( pid > 0 )
	{
		snprintf(cmd_line_path, sizeof(cmd_line_path), "/proc/%d/cmdline", pid);
		fp = fopen(cmd_line_path, "r");
		if ( fp )
		{
			res = fread(cmd_line, sizeof(char), sizeof(cmd_line) - 1, fp);
			cmd_line[sizeof(cmd_line) - 1] = '\0';
			fclose(fp);
		}

		cmd_chk = strstr(cmd_line, "saf");

		if ( cmd_chk )
			status = 1;
	}

	if ( 0 == status )
	{
		tcdbg_printf("\nrestart saf app.\n");
		get_current_time(cur_time, sizeof(cur_time));
		snprintf(msg, sizeof(msg), "{\"AppName\":\"%s\",\"Time\":\"%s\",\"Reason\":\"saf exit and will restart.\",\"Logs\":\"no logs\"}",
								CTC_GW_SERVICE_NAME_SAF, cur_time);
		ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_PROC_STAT,
					msg,
					strlen(msg)+1);

		system("/usr/bin/killall -9 saf");
		unlink("/var/run/ctc_saf.pid");
		system("export LD_LIBRARY_PATH=/usr/lib/glib-2.0/ && /userfs/bin/saf service 8 9 10 &> /dev/null");
		sleep(1);
		system("pidof saf > /var/run/ctc_saf.pid");
	}

	return 0;
}

int dbus_guard(void)
{
	static int timeout_delay = 0;
	static int check_saf = 0;
	FILE *fp = NULL;

	if ( 0 == guard_boot )
		return -1;

	fp = fopen("/tmp/dbus_guard_off", "r");
	if ( NULL != fp )
	{
		fclose(fp);
		return -2;
	}

	if ( ++ timeout_delay > 8 )
	{
		/* can not get PID when rcS */
		if ( 0 == check_saf )
		{
			check_saf = 1;
			system("pidof saf > /var/run/ctc_saf.pid");
		}

		timeout_delay = 0;
		chkDbusDaemon(0);
		chkDbusDaemon(1);
#ifndef TCSUPPORT_CUC
		chkSafProcess();
#endif

	}
	return 0;
}


void check_is_cts_run(void)
{
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	static int flag = 1;
	static int duration = 0;
	char FileName[128] = {0};
	int index = 1;
	int i;

	
	duration++;
	if(duration < 5)
		return;
	
	duration = 0;

	if ((dir=opendir("/tmp/")) == NULL)
	{
		return;
	}
	while ((ptr=readdir(dir)) != NULL)
	{			
		if(	flag && strstr(ptr->d_name, ".csv"))
		{
			strncpy(FileName,ptr->d_name,sizeof(FileName)- 1);
			for(i = 0; i < strlen(ptr->d_name); i++)
			{
				if(i != 0 && FileName[i] == '_' && FileName[i - 1] != '_')
					index++;
				if(index == 4 && FileName[i] == '.')
					index++;
			}
			if(index == 5)
			{
				flag = 0;
				cfg_set_object_attr("root.sys.entry", "ctcDbusFlag", "5");
			}
		}
		if(!flag && !strcmp(ptr->d_name, "dbus_cts_test_result.txt"))
		{
			cfg_set_object_attr("root.sys.entry", "ctcDbusFlag", "0");
		}		
	}

	closedir(dir);
	return;
}
#endif
#endif

#if defined(TCSUPPORT_CT_JOYME4)
void check_tr069_wan_inform_status(void)
{
	static int check_num = 1800; /* 30 mins */
	char node[64] = {0}, status[32] = {0};
	char msg[256] = {0}, reason[128] = {0}, cur_time[64] = {0}, inform_time[64] = {0};
	char node_waninfo[32] = {0}, w_ip_v4[16] = {0};
	int ret = -1;

	check_num--;

	if (check_num == 0) {
		get_current_time(cur_time, sizeof(cur_time));
		get_inform_time(inform_time, sizeof(inform_time));
		snprintf(node, sizeof(node), "%s", CWMPROUTE_ENTRY_NODE);
		cfg_get_object_attr(node, "Status", status, sizeof(status));
		if (!strcmp(status, "Up")) {
			cfg_get_object_attr(node, "IP", w_ip_v4, sizeof(w_ip_v4));
			snprintf(reason, sizeof(reason), "TR069WANStatus:%s,TR069WANIP:%s,LastInformTime:%s", status, w_ip_v4, inform_time);
		}
		else {
			snprintf(reason, sizeof(reason), "TR069WANStatus:Down,TR069WANIP:%s,LastInformTime:%s", "", inform_time);
		}
		snprintf(msg, sizeof(msg), "{\"AppName\":\"%s\",\"Time\":\"%s\",\"Reason\":\"%s\",\"Logs\":\"\"}",
								CTC_GW_SERVICE_NAME_TR069C, cur_time, reason);

		ret = ecnt_event_send(ECNT_EVENT_DBUS,
					ECNT_EVENT_DBUS_PROC_STAT,
					msg,
					strlen(msg)+1);

		if (ret < 0)
			tcdbg_printf("check_tr069_wan_inform_status ecnt_event_send error\n");

		check_num = 1800; /* Count down 30 mins again.*/
	}

	return;
}
#endif

#if  defined(TCSUPPORT_CT_STBMAC_REPORT)
void check_stb_change(void)
{
	cwmp_msg_t message;

	if ( 1 == blapi_system_check_stb_change())
	{
		memset(&message, 0, sizeof(message));
		message.cwmptype = STB_CHANGE;
		if(sendmegq(1, &message, 0) < 0)
		{
			tcdbg_printf("\r\n[%s]send message to cwmp error!!",__FUNCTION__);
		}
	}
	
	return;
}
#endif

void check_dhcprelay(void)
{
	char restart_dhcprelay[4] = {0};
	cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "restart_dhcprelay", restart_dhcprelay, sizeof(restart_dhcprelay));
	if(atoi(restart_dhcprelay))
	{
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "restart_dhcprelay", "0");
		kill_process("/var/run/udhcpd.pid");
		kill_process("/var/log/dhcrelay.pid");
		unlink(DHCPD_PATH);
		unlink(DPROXYAUTO_PATH);
		system(DHCPRELAY_PATH);
		tcdbg_printf("dhcprelay restarted\n");
	}
}

#if defined(TCSUPPORT_CT_JOYME2)
int get_wan_acct(int i, wan_acnt_t *cnt)
{
	char nasName[64] = {0};
	char rxbytes[64] = {0};
	unsigned long long  rx_bytes = 0;
	int ret = 0;
	
	ret = blapi_traffic_get_wan_acnt(i, cnt);
	if(ret == 0)
	{	
	/* get uniRxBytes */
	snprintf(nasName, sizeof(nasName), "root.info.nas%d.%d", i/8, i%8 + 1);
	cfg_get_object_attr(nasName, "rxbytes", rxbytes, sizeof(rxbytes));
	sscanf(rxbytes, "%llu", &rx_bytes);
	cnt->uniRxBytes = rx_bytes;
	}
	
	return 0;	
}

int send_itv_status(wan_acnt_t *cnt1, wan_acnt_t *cnt2)
{
	char data[2] = {0};
	int ret = -1;
	
	if(cnt2->mulRxBytes > cnt1->mulRxBytes)
	{
			strcpy(data, "0");
			cfg_set_object_attr("root.wan.common", "iTVLineStatus", data);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_ITVLINESTATUS,
							data,
							strlen(data)+1);
	}
	else
	{
		if(cnt2->uniRxBytes > cnt1->uniRxBytes)
		{
			strcpy(data, "1");
			cfg_set_object_attr("root.wan.common", "iTVLineStatus", data);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_ITVLINESTATUS,
							data,
							strlen(data)+1);
		}
		else
		{
			strcpy(data, "2");
			cfg_set_object_attr("root.wan.common", "iTVLineStatus", data);
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_ITVLINESTATUS,
							data,
							strlen(data)+1);
		}
	}
	return 0;
}

void check_itv_status(void)
{
	static int start_detect = 0;
	static  wan_acnt_t cnt1[64];
	wan_acnt_t cnt2[64];
	static long duration = 0;
	int i = 0;
	char isdetect[10] = {0};
	char buf[10] = {0};
	char buf1[10] = {0};
	char buf2[10] = {0};
	static unsigned int other_wan_list0 = 0;
	static unsigned int other_wan_list1 = 0;
	static struct timespec curtime1;
	struct timespec curtime2;
	
	cfg_get_object_attr("root.sysinfo.entry", "isdetect", isdetect, sizeof(isdetect));
	
	if (isdetect[0] == 'N' || isdetect[0] == '\0')
		return;

	if (start_detect == 0) {
		start_detect = 1;
	
		/* get duration and otherwan */
		cfg_get_object_attr("root.sysinfo.entry", "duration", buf, sizeof(buf));
		cfg_get_object_attr("root.wan.common", "other_wan_list0", buf1, sizeof(buf1));
		cfg_get_object_attr("root.wan.common", "other_wan_list1", buf2, sizeof(buf2));

		sscanf(buf, "%ld", &duration);
		sscanf(buf1, "%u", &other_wan_list0);
		sscanf(buf2, "%u", &other_wan_list1);
		
		/* get current time old time */
		clock_gettime(CLOCK_MONOTONIC, &curtime1);
		
		/* get cnt */
		for (i = 0; i < 32; i++) {
			if (other_wan_list0 & (1<<i)) {	
				memset(&cnt1[i], 0, sizeof(wan_acnt_t));
				get_wan_acct(i, &cnt1[i]);
			}

			if (other_wan_list1 & (1<<i)) {
				memset(&cnt1[i+32], 0, sizeof(wan_acnt_t));
				get_wan_acct(i+32, &cnt1[i+32]);
			}
		}
	}
	else {
		/* get current time, uptime */
		clock_gettime(CLOCK_MONOTONIC, &curtime2);
	
		if ((curtime2.tv_sec - curtime1.tv_sec) >= duration) {
			/* get cnt and send event */
			for (i = 0; i < 32; i++) {
				if (other_wan_list0 & (1<<i)) {	
					memset(&cnt2[i], 0, sizeof(wan_acnt_t));
					get_wan_acct(i, &cnt2[i]);
					send_itv_status(&cnt1[i], &cnt2[i]);		
				}
				
				if (other_wan_list1 & (1<<i)) {
					memset(&cnt2[i+32], 0, sizeof(wan_acnt_t));
					get_wan_acct(i+32, &cnt2[i+32]);
					send_itv_status(&cnt1[i+32], &cnt2[i+32]);
				}
			}
			/* clear flag */
			cfg_set_object_attr("root.sysinfo.entry", "isdetect", "No");
			start_detect = 0;
		}
	}
}
#endif

#define RESTART_SERVICE_SH "/etc/restart_service.sh"

#if defined(TCSUPPORT_CT_JOYME2)
int get_used_memroy_percent()
{
	FILE *fp = NULL;
	char buf[40] = {0};
	char temp[40] = {0};
	unsigned long  memTotal = 0;
    unsigned long  memFree = 0;
    unsigned long  buffers = 0;
    unsigned long  cached = 0;
	int i = 0;
	
	fp = fopen("/proc/meminfo","r");
    if(NULL == fp)
    {       
        tcdbg_printf("\r\nopen meminfo file error!");
        return -1;
    }

    while( !(feof(fp)) && (i<4) )
    {
        memset(buf,0, sizeof(buf));
        memset(temp,0, sizeof(temp));
        fgets(buf , sizeof(buf) , fp);
        sscanf(buf,"%*s%s",temp);
        if(strstr(buf,"MemTotal:") != NULL){
            memTotal = atoi(temp);
        } else if(strstr(buf,"MemFree:") != NULL){
            memFree = atoi(temp);
        } else if(strstr(buf,"Buffers:") != NULL){
            buffers = atoi(temp);
        } else if(strstr(buf,"Cached:") != NULL){
            cached = atoi(temp);
        } else {
         //   tcdbg_printf("memory_read can't find the key word!\n");
        }
        
        i++;
    }
	fclose(fp);
	if(memTotal == 0){
		return -1;
	}
   return ((memTotal-memFree-buffers-cached)*100)/memTotal;
}

void check_memory(void)
{
	FILE *fp = NULL;
	char memalarm[10] = {0};
	char memlimit[10] = {0};
	char data[20] = {0};
	int percent = 0;
	int tempalarm = 0;
	int templimit = 0;
	int ret = 0;
	static int event_flag = 0;

    percent = get_used_memroy_percent();
	
#if defined(TCSUPPORT_CT_JOYME4)
	CheckRAMAlarm(percent);
#endif

	if (cfg_get_object_attr("root.sys.entry", "MemAlarm", memalarm, sizeof(memalarm)) > 0)
		sscanf(memalarm, "%d", &tempalarm);
	else
		tempalarm = 80;
	
	if (cfg_get_object_attr("root.sys.entry", "MemLimit", memlimit, sizeof(memlimit)) > 0)
		sscanf(memlimit, "%d", &templimit);
	else
		templimit = 90;

	if (percent >= tempalarm && percent < templimit)
	{	
		resumemem_times = 0;
		
		if (event_flag != 1) {
			strcpy(data, "MemAlarm");
			
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSMEMCONTROL,
							data,
							strlen(data)+1);
			event_flag = 1;
		}
		
	}
	else if (percent > templimit)
	{
		resumemem_times = 0;
		
		if (event_flag != 2) {
			strcpy(data, "MemLimit");
			
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSMEMCONTROL,
							data,
							strlen(data)+1);
			event_flag = 2;
		}
	}
	else if (percent <= (tempalarm - 10) && ((event_flag == 1) || (event_flag == 2)))
	{
		resumemem_times++;
		if (resumemem_times > 59)
		{
			strcpy(data, "MemResumeNormal");
			
			ret = ecnt_event_send(ECNT_EVENT_DBUS,
							ECNT_EVENT_DBUS_SYSMEMCONTROL,
							data,
							strlen(data)+1);
			resumemem_times = 0;
			event_flag = 3;
		}
		
	}
	
	
	return 0;
}
#endif
void service_restart_run(void)
{
	int ret1 = 0, ret2 = 0;

	ret1 = chmod(RESTART_SERVICE_SH,777);
	ret2 = system(RESTART_SERVICE_SH);
	unlink(RESTART_SERVICE_SH);

}
int get_pollingcount(void)
{
	char tmpbuf[128] = {0};
	int tmpCount = 90; 
	
	if (cfg_get_object_attr("root.wlan.common", "PollingCount", tmpbuf, sizeof(tmpbuf)) > 0)
	{
		tmpCount = atoi(tmpbuf);
	}
	return tmpCount;
}
int check_sevice_isexit(void)
{
	int i = 0, j = 0, res = 0;
	char cmdbuf[128] = {0};
	FILE *fp2 = NULL;
	int isexist = 0;
	char tmp_pid[6]={0};
	int pid_val = 0;
	char tmpbuf[128] = {0};
	FILE *fpid = NULL;
	FILE *fp = NULL;
	int check_exist = 0;	
	char *svclist[] = {
		/*"cfg.svc",*/
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_NP)
		"pon.svc",
#endif
#endif

		"wan.svc",
#if defined(TCSUPPORT_WLAN)
		"wlan.svc",
#endif
		"wan_related.svc",
		"other.svc",
		/*"central_ctrl.svc",*/
#if defined(TCSUPPORT_VOIP)
		"voip.svc",
#endif
		""
	};

	char *pidlist[] = {
		/*"cfg_svc.pid",*/
#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_NP)
		"pon_svc.pid",
#endif
#endif
		"wan_svc.pid",
#if defined(TCSUPPORT_WLAN)
		"wlan_svc.pid",
#endif
		"wan_related_svc.pid",
		"other_svc.pid",
		/*"central_ctrl_svc.pid",*/
#if defined(TCSUPPORT_VOIP)
		"voip_svc.pid",
#endif
		""
	};

	int rv = 0, pidnum = 0;
	int pid_svchost_t[128] = {0};
	
	rv = find_pid_by_name("svchost", pid_svchost_t, &pidnum);	

	if(rv < 0 || pidnum < 1)
	{
		tcdbg_printf("get pid_svchost_t fail\n");
		return check_exist;
	}

	fp = fopen(RESTART_SERVICE_SH, "a+");
	if(fp == NULL)
	{
		tcdbg_printf("file %s create fail\n", RESTART_SERVICE_SH);
		return check_exist;
	}
	
	
	for(i = 0; strlen(svclist[i]) != 0; i++)
	{
		memset(tmpbuf, 0, sizeof(tmpbuf));
		snprintf(tmpbuf, sizeof(tmpbuf), "/var/run/%s", pidlist[i]);
		fpid = fopen(tmpbuf, "r");
		if(fpid == NULL){
			tcdbg_printf("file %s open fail\n", tmpbuf);
			fclose(fp);
			unlink(RESTART_SERVICE_SH);
			return check_exist;
		}
		fgets(tmp_pid, sizeof(tmp_pid), fpid);
		fclose(fpid);
		pid_val = atoi(tmp_pid);

		for(j = 0; j < pidnum; j++)
		{
			if(pid_svchost_t[j] == pid_val)
			{
				isexist = 1;
				break;
			}
		}

		if(isexist)
		{
			isexist = 0;
			continue;
		}
		else
		{
#if defined(TCSUPPORT_CT_JOYME2)
			/* check other exit case, for example, signal 9 */
			if (!strcmp(svclist[i], "wlan.svc")) {
				fp2 = fopen(WIFI_SEND_FLAG, "r");
				if (fp2 == NULL) {
					send_process_stat_msg(CTC_GW_SERVICE_NAME_WIFI);
				}
				else {
					fclose(fp2);
					/*unlink(WIFI_SEND_FLAG);*/
				}
				memset(cmdbuf, 0, sizeof(cmdbuf));
				snprintf(cmdbuf, sizeof(cmdbuf), "/bin/rm -f %s\n", WIFI_SEND_FLAG);
				fputs(cmdbuf, fp);
			}
#endif
			check_exist = 1;
	
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/svchost -c /etc/%s &\n", svclist[i]);
			fputs(cmdbuf, fp);
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "echo $! > /var/run/%s\n", pidlist[i]);
			fputs(cmdbuf, fp);
		}			
	}
	if(fp != NULL)
		fclose(fp);
	return check_exist;

}
void check_service_restart()
{
	
	static int tCount = 0;
	static int check_exist = 0;
	static int max_count = 90;
	static int restartCount = 10;
		
	tCount++;
	
	if(1 == check_exist)
		restartCount--;

	if(tCount < max_count)
		return;
	
	if(check_exist && restartCount == 0)
	{
		service_restart_run();
		check_exist = 0;
		tcdbg_printf("check_service_restart restart!\n");
	}

	max_count = get_pollingcount();
	tCount = 0; 
	
	if(1 == check_sevice_isexit())
	{
		restartCount = max_count;
		check_exist = 1;
	}
	return;
}

#if defined(TCSUPPORT_IGMP_PROXY)
int checkIgmpproxyProcess(void)
{
	static int tmCount = 0;
	int igmp_pid = 0;
	int rv = 0, pidnum = 0, pid_t[128] = {0};
	wan_related_evt_t param;
	char active[12] = {0}, active2[12] = {0};

	tmCount++;
	if ( tmCount < 15 )
		return -1;
	tmCount = 0;

	if ( cfg_get_object_attr(IGMPPROXY_ENTRY_NODE, "Active", active, sizeof(active)) > 0
		&& 0 == strcmp(active, "Yes")
		&& cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "proxyActive", active2, sizeof(active2)) > 0
		&& 0 == strcmp(active2, "Yes") )
	{
		rv = find_pid_by_name( "igmpproxy", pid_t, &pidnum);
		if( 0 == rv && 1 == pidnum )
			igmp_pid = pid_t[0];

		if ( 0 == igmp_pid )
		{
			bzero(&param, sizeof(param));
			strncpy(param.buf, IGMPPROXY_ENTRY_NODE, sizeof(param) - 1);
			cfg_obj_send_event(EVT_WAN_RELATED_INTERNAL, EVT_CFG_WAN_RELATED_IGMPPROXY_UPDATE, (void *)&param, sizeof(param)); 
			/*tcdbg_printf("@@@@ restart igmpproxy.\n");*/
		}
	}

	return 0;
}
#endif

#if defined(TCSUPPORT_CWMP)
#if defined(TCSUPPORT_CT_JOYME4)
#define CHECK_PERIOD 5
#else
#define CHECK_PERIOD 60
#endif

void checkTR69Process(void){
	static int tCount = 0;
	int tr69_pid = 0;
	int rv = 0, pidnum = 0, pid_t[128] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
	char nodeName[64] = {0}, nodeValue[128] = {0};
	char cmd_buf[256] = {0}, tr69boot_buf[16] = {0};
#endif
	FILE *fp = NULL;
#if defined(TCSUPPORT_CT_DBUS)
	FILE *fp_tr69sig = NULL;
#endif

#if defined(TCSUPPORT_CT_JOYME4)
	cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "tr69boot_flag", tr69boot_buf, sizeof(tr69boot_buf));
	if( strcmp(tr69boot_buf, "1") )
		return;
#endif

	fp = fopen("/tmp/dbus_guard_off", "r");
	if ( NULL != fp )
	{
		fclose(fp);
		return ;
	}

	tCount++;
	if(tCount < CHECK_PERIOD)
		return;
	tCount = 0;
	
	rv = find_pid_by_name( "tr69", pid_t, &pidnum);
	if( 0 == rv && 1 == pidnum )
		tr69_pid = pid_t[0];

#if defined(TCSUPPORT_CT_JOYME4)
	snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, 3);
	cfg_get_object_attr(nodeName, "username", nodeValue, sizeof(nodeValue));
#endif

	if(0 == tr69_pid){
#if defined(TCSUPPORT_CT_DBUS)
/* check other exit case, for example, signal 9 */
	fp_tr69sig = fopen(TR069C_SEND_FLAG, "r");
	if (fp_tr69sig == NULL) {
		send_process_stat_msg(CTC_GW_SERVICE_NAME_TR069C);
	}
	else {
		fclose(fp_tr69sig);
		unlink(TR069C_SEND_FLAG);
	}	
#endif	

#if defined(TCSUPPORT_CT_JOYME4)
		bzero(cmd_buf, sizeof(cmd_buf));
		snprintf(cmd_buf, sizeof(cmd_buf), "su - %s -c \"/userfs/bin/tr69 &> /dev/null && /bin/pidof tr69 > /var/run/ctc_tr69.pid\""
					, nodeValue);
		system(cmd_buf);
#else
		system("/userfs/bin/tr69 &>/dev/null");
#endif

	}
	return;
}
#endif

#if defined(TCSUPPORT_CWMP_TR181)
void tr181IPAPCheckNum(void)
{
	char numModify[8] = {0};
	char isRebulding[8] = {0};

	cfg_get_object_attr(TR181_IPACTIVEPORT_COMMON_NODE, "isRebuildingIPAPTree",isRebulding,sizeof(isRebulding));
	if(!strcmp(isRebulding,"1")){
		return;
	}
	
	cfg_get_object_attr(TR181_IPACTIVEPORT_COMMON_NODE, "NumModify",  numModify,sizeof(numModify));

	if(!strcmp(numModify,"1")){
		sendTR181MsgtoCwmp(REINIT_TR181_IPAP_TREE);
	}
	
	return ;
}

void DevinfPSCheckNum(void)
{
	char numModify[8] = {0};
	char isRebulding[8] = {0};

	cfg_get_object_attr(TR181_DEVINF_PS_COMMON_NODE, "isRebuildingPsTree",isRebulding,sizeof(isRebulding));
	if(!strcmp(isRebulding,"1")){
		return;
	}
	
	cfg_get_object_attr(TR181_DEVINF_PS_COMMON_NODE, "NumModify",numModify,sizeof(numModify));

	if(!strcmp(numModify,"1")){
		sendTR181MsgtoCwmp(REINIT_PROCESS_TREE);
	}
	
	return ;
}
#endif

void check_cwmp_change()
{
	char cwmp_change[8] = {0};
	static int duration = 0;
	
	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "cwmp_change", 0, cwmp_change, sizeof(cwmp_change));
	if(!strcmp(cwmp_change, "1"))
	{
		return;
	}
	else if(!strcmp(cwmp_change, "0"))
	{
		duration++;
		if(duration < 10)
			return;
		
		duration = 0;
		cfg_commit_object(CWMP_ENTRY_NODE);
	}

	return;
}

#if defined(TCSUPPORT_CT_JOYME4)
void check_cwmp_work(int timer)
{	
	static int seconds_i = 0;
	int cwmp_work_timer = 10;
	char cwmp_wan_change[8] = {0};
	char cwmp_work[8] = {0};
	char wanbuf[32] = {0};
	int pvc_idx = -1, ety_idx = -1;

	cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "cwmp_wan_change", 0, cwmp_wan_change, sizeof(cwmp_wan_change));
	if(!strcmp(cwmp_wan_change, "1"))
	{
		cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "cwmp_work", 0, cwmp_work, sizeof(cwmp_work));
		if(!strcmp(cwmp_work, "1"))   /*TR69 wan is working*/
		{
			seconds_i = 0;
			cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "cwmp_work" ,"0");
		}
		else if(!strcmp(cwmp_work, "0"))
		{
			if(timer > 0)
				cwmp_work_timer = timer;
			
			if ( ++ seconds_i >= cwmp_work_timer )
			{
				if(cfg_obj_get_object_attr(GLOBALSTATE_COMMON_NODE, "WanNode", 0, wanbuf, sizeof(wanbuf)) > 0)
				{
					sscanf(wanbuf, "Wan_PVC%d_Entry%d", &pvc_idx, &ety_idx);
				}

				if((pvc_idx >= 0) && (ety_idx >= 0))
				{
					snprintf(wanbuf, sizeof(wanbuf), WAN_PVC_ENTRY_NODE, pvc_idx+1, ety_idx+1);
					cfg_commit_object(wanbuf);
					cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "WanNode" ,"");
					cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "cwmp_wan_change" ,"0");				
				}
			}
		}
	}

	return;
}
#endif

static void central_ctrl_mgr_polling_pthread(void)
{
    char  xpon_traffic_state = E_XPON_LINK_DOWN;
	char      loadJoymeCount = 0;
	char loadUsbHostCount = 0;
	struct timespec curtime;
	static int count = 0;
	char WanStatus[32] = {0};
	int flag = 0;
        int tr181flag = 0;
	int adsl_status = -1;
	int g_adsl_status = -1;
	struct timespec curTime;
	char buf[64] = {0};
	char      loadDbusCount = 0;
	char str_work_timer[8] = {0};
#if defined(TCSUPPORT_CT_JOYME4)
	static int check_lan_count = 0;
#if defined(TCSUPPORT_SYSLOG_ENHANCE)
	static int check_plugin_count = 0;
	int check_plugin_interval = 0;
	char interval_buf[16] = {0};
#endif
#endif
	char wanboot[2] = {0};			
	
	memset(WanStatus, 0, sizeof(WanStatus));
	
#if defined(TCSUPPORT_CUC_LANDING_PAGE)
	system("iptables -t nat -N PRE_PONSTATE");
	system("iptables -t nat -A PREROUTING -j PRE_PONSTATE");
	system("iptables -t nat -A PRE_PONSTATE -i br0 -p tcp --dport 80 -j REDIRECT");
#endif
	
    while(1)
    {
#if defined(TCSUPPORT_CT_DSL_EX)	 
		adsl_status = -1;
		adsl_status = is_adsl_link_up();

		if(g_adsl_status != adsl_status)
		{
			adsl_state_change(adsl_status);
		}
		g_adsl_status = adsl_status;
		
#else
        xpon_traffic_state = is_xpon_traffic_up();
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		if( 30 == check_lan_count )
		{
			check_wlan_lan_status(0);
#if defined(TCSUPPORT_WLAN)
			check_wlan_lan_status(1);
#endif
			check_lan_count = 0;
		}
		check_lan_count++;
		
#if defined(TCSUPPORT_SYSLOG_ENHANCE)
		cfg_get_object_attr(PLUGIN_COMMON_NODE, "Check_Interval", interval_buf, sizeof(interval_buf));
		check_plugin_interval = atoi(interval_buf);
		if( check_plugin_interval <= 0 )
			check_plugin_interval = 10;
		
		if(check_plugin_interval <= check_plugin_count)
		{
			check_plugin_name();
			check_plugin_count = 0;
		}
		check_plugin_count++;
#endif

#endif

		if(flag == 0)
		{
			clock_gettime(CLOCK_MONOTONIC, &curTime);

			if(curTime.tv_sec >= 50 && loadUsbHostCount==0)
			{
			  system("/usr/script/usbhost_load.sh &");
			  loadUsbHostCount = 1;
			}

#if defined(TCSUPPORT_CT_DBUS)	 
			if(curTime.tv_sec >= 55 && loadDbusCount==0)
			{
				loadDbusCount = 1;
				system("/usr/script/dbus_start.sh &");
			}
			
#endif
			if(curTime.tv_sec >= 60)
			{
				if((cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wanboot", wanboot, sizeof(wanboot)) >= 0)
					&& (strcmp(wanboot, "1") == 0))
				{
					system("/usr/script/restore_cpu_bind.sh &");			
					flag =1;
					memset(buf, 0, sizeof(buf));
					strncpy(buf, WAN_NODE, sizeof(buf)-1);
					cfg_obj_send_event(EVT_WAN_INTERNAL, EVT_CFG_WAN_ENTRY_BOOT2, buf, sizeof(buf)); 
					system("/userfs/bin/msg call 4097 boot2");
				}
			}
		}	

#if defined(TCSUPPORT_CT_JOYME4)
		if(cfg_obj_get_object_attr(CWMP_ENTRY_NODE, "cwmp_work_timer", 0, str_work_timer, sizeof(str_work_timer)) > 0)
			check_cwmp_work(atoi(str_work_timer));
#endif
		check_cwmp_change();
		
#if !defined(TCSUPPORT_CT_FJ)
		check_reset_button();
#endif

#if defined(TCSUPPORT_LED_BTN_CHECK)
#if defined(TCSUPPORT_BTN_CHECK)
		check_button_type();
#endif
#endif

#ifdef TCSUPPORT_LED_SWITCH_BUTTON
		check_led_switch_button(xpon_traffic_state);
#endif

		ntpexecute();

#if defined(TCSUPPORT_ECNT_MAP)
		ecnt_uloop_run();
#endif

		if(flag == 1){
#if defined(TCSUPPORT_CUC_CWMP_PARAMETER) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_ANDLINK)
		CalcuCpuUsage();
#endif


#if !defined(TCSUPPORT_CMCCV2)
		check_dhcprelay();
#endif


#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
		CheckCPUAlarm();
#endif

#if defined(TCSUPPORT_CT_IPPINGDIAGNOSTIC)
		check_ctIppingDiagnosatic();
#endif

		check_flush_arp();

		checkUILockState();

#if defined(TCSUPPORT_CT_L2TP_VPN)
#if !defined(TCSUPPORT_CT_VPN_ONDEMAND)
		checkVPNPPPTunnels(xpon_traffic_state);
#endif
#endif
#if defined(TCSUPPORT_CT_STBMAC_REPORT)
		check_stb_change();
#endif
		if(0 == loadJoymeCount)
		{
			clock_gettime(CLOCK_MONOTONIC, &curtime);
			if(curtime.tv_sec > 75)
			{
				system("/usr/script/joyme_load.sh &");
				loadJoymeCount = 1;
				guard_boot = 1;
			}
		}

		check_service_restart();
#if defined(TCSUPPORT_CT_JOYME2)
		check_itv_status();	
		check_memory();
#endif
		save_PWR_reboot_info();
#if defined(TCSUPPORT_CMCCV2)
		traffic_qos_service_rate_update();
		if(3 == count)
		{
			if(traffic_forward_domain_resolve() != 0){
				tcdbg_printf("\ntraffic_forward_domain_resolve error!\n");
			}				
			count = 0;
		}
		count++;


#endif
#if defined(TCSUPPORT_CWMP)
#if !defined(TCSUPPORT_CT_DBUS)
		checkTR69Process();
#endif
#endif
#if defined(TCSUPPORT_IGMP_PROXY)
		checkIgmpproxyProcess();
#endif
#if defined(TCSUPPORT_CT_JOYME2)
		check_send_alarm();
		check_cpwan_boot_status();
#ifdef TCSUPPORT_DMS
		check_restart_dlna();
#endif
#if defined(TCSUPPORT_CT_DBUS)
		check_send_alarm();
		check_cpwan_boot_status();
		dbus_guard();
		check_is_cts_run();
#endif
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		check_tr069_wan_inform_status();
#endif
#if defined(TCSUPPORT_CWMP_TR181)
		if(tr181flag == 5)
		{
		tr181IPAPCheckNum();
		DevinfPSCheckNum();
			tr181flag = 0;
		}
		tr181flag++;
#endif
		}
        sleep(1);
    }
    
	pthread_exit(0);

}

int creat_centrlal_mgr_polling_pthread(void)
{
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, (void *)central_ctrl_mgr_polling_pthread, NULL) < 0)
    {
        printf("creat_centrlal_mgr_polling_pthread: create thread fail \n");
        return -1;
    }

    return 0;
}

