#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dbus_timer.h"
#include "check_button.h"
#include "cfg_cli.h"
#include "utility.h"
#include "cfg_msg.h"

timer_entry_node_t *timer_data = NULL;
int check_done = 0;
int need_done = 0;

static int check_time_match(int hour, int min, int real_hour, int real_min);

static int CaculateWeekDay(int year, int mon, int day)
{
	int iWeek = -1;
	if(1 == mon || 2 == mon) {
		mon += 12;
		year--;
	}

	/* Kim larsson calculation formula */
	iWeek = (day + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;

	return iWeek + 1;
}


void check_wireless_status(int itf_idx, char *itf_name, char* status, int maxlen)
{
	int fd, len = -1;
	char cmd[128] = {0};
	char stream[64] = {0};
	int res = 0;

	if(itf_idx < 0 || itf_idx > (WLAN_2_4_G_NUM - 1) || status == NULL)
	{
		return;
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s%d > /tmp/get_wireless_stat.tmp", itf_name, itf_idx);
	system(cmd);
	
	memset(stream,0,sizeof(stream));
	fd = open("/tmp/get_wireless_stat.tmp",O_RDONLY);
	if (fd < 0)
	{
		strncpy(status, "Off", maxlen);
		return;
	}

	len = read(fd, stream, sizeof(stream) - 1);
	stream[sizeof(stream) - 1] = '\0';
	close(fd);
	res = remove("/tmp/wireless_stat.tmp");
	if((len == 0) || (len == -1))
	{
		strncpy(status, "Off", maxlen);
		return;
	}

	if((strstr(stream,"HWaddr FF:FF:FF:FF:FF:FF") != 0)
		||(strstr(stream,"HWaddr 00:00:00:00:00:00") != 0))
	{
		strncpy(status, "Error", maxlen);
	}
	else
	{
		strncpy(status, "On", maxlen);
	}

	return;
}


int doWifiTimerEvent(char *enable, int ssidmask, int optflag)
{
	char cmd[128] = {0};
	char status[8] = {0};
	int wifi_type = 0; /* 0 - 2.4G, 1 - 5G */
	char itf_name[4] = {0};
	int itf_idx = -1;
	char nodeName[64]= {0}, EnableSSID[8] = {0};
	int ssid_cnt = 0, ssidac_cnt = 0, need_save = 0;
	int ssidindex = 0, mask = 0;
	char WifiEnableMask[8] = {0};
	int wifienablemask = 0;

	if(enable == NULL || ssidmask < 0 || ssidmask > WLAN_BITMASK)
		return -1;

	memset(WifiEnableMask, 0, sizeof(WifiEnableMask));
	if(cfg_get_object_attr(TIMER_COMMON_NODE, "WifiEnableMask", WifiEnableMask, sizeof(WifiEnableMask)) <= 0)
	{
		strncpy(WifiEnableMask, "65535",sizeof(WifiEnableMask) -1);
	}
	wifienablemask = atoi(WifiEnableMask);

	for( ssidindex = 0; ssidindex < WLAN_2_4_G_NUM + WLAN_5_G_NUM; ssidindex++ ) {
		mask = 1 << ssidindex;
		if ( !(mask & ssidmask) )
			continue;
		
		wifi_type = ssidindex/WLAN_2_4_G_NUM;

		memset(status, 0, sizeof(status));
		memset(itf_name, 0, sizeof(itf_name));
		memset(nodeName, 0, sizeof(nodeName));
		if (0 == wifi_type)
		{
			itf_idx = ssidindex;
			strncpy(itf_name, "ra", sizeof(itf_name) - 1);
			snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, itf_idx + 1);
			cfg_get_object_attr(INFO_WLAN_NODE, "isExist", status, sizeof(status));
		}
		else
		{
			itf_idx = ssidindex - WLAN_2_4_G_NUM;
			strncpy(itf_name, "rai", sizeof(itf_name) - 1);
			snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, itf_idx + 1);
			cfg_get_object_attr(INFO_WLAN11AC_NODE, "isExist", status, sizeof(status));
		}

		if ( 0 != strcasecmp(status, "on") )
			continue;

		memset(EnableSSID, 0, sizeof(EnableSSID));
		if ( cfg_get_object_attr(nodeName, "EnableSSID", EnableSSID, sizeof(EnableSSID)) <= 0 )
			continue;

		memset(cmd, 0, sizeof(cmd));

		if(0 == strcasecmp(enable, "True") && 0 == GET_BIT(wifienablemask, ssidindex))
		{
			/* do wifi interface up */
			SET_BIT(wifienablemask, ssidindex);

			if ( 0 == strcmp(EnableSSID, "0") && 1 == optflag)
				continue;
			
			cfg_set_object_attr(nodeName, "EnableSSID", "1");
			if ( 0 == wifi_type )
				ssid_cnt++;
			else
				ssidac_cnt++;

			need_save++;
		}
		else if(0 == strcasecmp(enable, "False") && 1 == GET_BIT(wifienablemask, ssidindex))
		{
			/* do wifi interface down */
			CLR_BIT(wifienablemask, ssidindex);
			snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s%d down", itf_name, itf_idx);
			system(cmd);
		}
		else
		{
			continue;
		}
	}

	memset(WifiEnableMask, 0, sizeof(WifiEnableMask));
	snprintf(WifiEnableMask, sizeof(WifiEnableMask), "%d", wifienablemask);
	cfg_set_object_attr(TIMER_COMMON_NODE,"WifiEnableMask",WifiEnableMask);

	if ( ssid_cnt > 0 )
		cfg_commit_object(WLAN_NODE);
	if ( ssidac_cnt > 0 )
		cfg_commit_object(WLAN11AC_NODE);
	if ( need_save > 0 )
		cfg_evt_write_romfile_to_flash();
	return 0;
}

#if !defined(TCSUPPORT_CT_UBUS)
int doSleepTimerEvent(char *enable)
{
	int i = 0, number = 0, res = 0;
	char cmdbuf[128] = {0}, nodeName[64] = {0};
	char enablessid[4] = {0}, HGWSleepVal[4] = {0}, ledStatus[8] = {0};
	char strValue[32] = {0}, dev_path[32] = {0}, mountfs[32] = {0}, file_path[32] = {0};

	memset(HGWSleepVal, 0, sizeof(HGWSleepVal));
	cfg_obj_get_object_attr(TIMER_COMMON_NODE, "HGWSleep", 0, HGWSleepVal, sizeof(HGWSleepVal));
	if(0 == strcasecmp(enable, "False") &&  0 == strcmp(HGWSleepVal, "0"))
	{
		/* WIFI up */
		doWifiTimerEvent("True", WLAN_BITMASK, 1);

		/* Lan interface up */
		for (i = 0; i < LAN_NUM; i++)
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "ifconfig eth0.%d up", i + 1);
			system(cmdbuf);
		}

		/* LED up */
		cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "Status", 0, ledStatus, sizeof(ledStatus));
		if ( 0 == strcasecmp(ledStatus, "ON") )
			button_op(BUTTON_RECOVER);
		else if ( 0 == strcasecmp(ledStatus, "OFF") )
			button_op(BUTTON_OFF);			

		/* Storage up */
		memset(strValue, 0, sizeof(strValue));
		if (cfg_obj_get_object_attr("root.UsbMount.Common", "SleepStatus", 0, strValue, sizeof(strValue)) > 0
			&& !strcmp(strValue, "1"))
		{
			memset(strValue, 0, sizeof(strValue));
			cfg_obj_get_object_attr("root.UsbMount.Common", "number", 0, strValue, sizeof(strValue));
			number = atoi(strValue);
			for (i = 0; i < number; i++) {
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), "root.UsbMount.Entry.%d", i + 1);
				memset(mountfs, 0 ,sizeof(mountfs));
				cfg_get_object_attr(nodeName, "mountfs", mountfs, sizeof(mountfs));
				memset(dev_path, 0 ,sizeof(dev_path));
				cfg_get_object_attr(nodeName, "devpath", dev_path, sizeof(dev_path));
				memset(file_path, 0 ,sizeof(file_path));
				cfg_get_object_attr(nodeName, "filepath", file_path, sizeof(file_path));
				
				if (strlen(file_path) > 0) {
					memset(cmdbuf, 0 ,sizeof(cmdbuf));
					snprintf(cmdbuf, sizeof(cmdbuf), "/bin/mount -o iocharset=utf8 -t vfat %s %s", dev_path, file_path);	
					res = system(cmdbuf);	

					/* vfat mount fail, try to use ntfs */
					if (res != 0) {
						memset(cmdbuf, 0 ,sizeof(cmdbuf));
						snprintf(cmdbuf, sizeof(cmdbuf), "/userfs/bin/ntfs-3g %s %s", dev_path, file_path);
						system(cmdbuf);
					}
				}
			}
			cfg_set_object_attr("root.UsbMount.Common", "SleepStatus", "0");
		}

		/* more up */

		cfg_set_object_attr(TIMER_COMMON_NODE, "HGWSleep", "1");
	}
	else if(0 == strcasecmp(enable, "True") && 0 == strcmp(HGWSleepVal, "1"))
	{
		/* LED down */
		button_op(BUTTON_OFF);

		/* WIFI down */
		doWifiTimerEvent("False", WLAN_BITMASK, 0);

		/* Lan interface down */
		for (i = 0; i < LAN_NUM; i++)
		{
			memset(cmdbuf, 0, sizeof(cmdbuf));
			snprintf(cmdbuf, sizeof(cmdbuf), "ifconfig eth0.%d down", i + 1);
			system(cmdbuf);
		}

		/* Storage down */
		memset(strValue, 0, sizeof(strValue));
		if (cfg_obj_get_object_attr("root.UsbMount.Common", "SleepStatus", 0, strValue, sizeof(strValue)) > 0
			&& !strcmp(strValue, "0"))
		{
			memset(strValue, 0, sizeof(strValue));
			cfg_obj_get_object_attr("root.UsbMount.Common", "number", 0, strValue, sizeof(strValue));
			number = atoi(strValue);
			for (i = 0; i < number; i++)
			{
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), "root.UsbMount.Entry.%d", i + 1);
				memset(dev_path, 0, sizeof(dev_path));
				cfg_get_object_attr(nodeName, "devpath", dev_path, sizeof(dev_path));
				memset(cmdbuf, 0 ,sizeof(cmdbuf));
	            snprintf(cmdbuf, sizeof(cmdbuf), "/bin/umount -d %s", dev_path);        
				system(cmdbuf);   	
			}
			cfg_set_object_attr("root.UsbMount.Common", "SleepStatus", "1");
		}

		/* more down */

		cfg_set_object_attr(TIMER_COMMON_NODE, "HGWSleep", "0");
	}
	else
	{
		return -1;
	}

	return 0;
}
#endif

/* dbus timer handler */
int start_dbus_timer(void)
{
	int year, mon, day, hour, min, sec = 0;
	int oldyear, oldmon, oldday = 0;
	int week = -1;
	int hourVal, minVal = 0;
	char HGWSleepVal[4] = {0};
	/*struct timespec curtime;*/
	struct tm nowTime;
	timer_entry_node_t *current = NULL;
	char week_val[2] = {0};
	int ssid_mask = 0, mask = 0, i = 0;
	static int cnt = 0;
	char SyncResult[8] = {0}, ledStatus[8] = {0};
	/* static int led_flag = BUTTON_RECOVER; */
	int led_action = 0;
	time_t tm;

	while(1)
	{
		cfg_get_object_attr(TIMEZONE_ENTRY_NODE, "SyncResult", SyncResult,sizeof(SyncResult));
		if(0 == atoi(SyncResult))
		{
			sleep(1);
			continue;
		}
		/* get now datetime */
	#if  0
		clock_gettime(CLOCK_REALTIME, &curtime);
		localtime_r(&curtime.tv_sec, &nowTime);
	#else
		time(&tm);
		memcpy(&nowTime, localtime(&tm), sizeof(nowTime));
	#endif
		year = nowTime.tm_year + 1900;
		mon = nowTime.tm_mon + 1;
		day = nowTime.tm_mday;
		hour = nowTime.tm_hour;
		min = nowTime.tm_min;
		
		/* get new week when time becomes another day. 
		     first time will get weekday because old values are 0.
		*/
		if(week < 0 || day != oldday || mon != oldmon || year != oldyear)
		{
			oldyear = year;
			oldmon = mon;
			oldday = day;
			/* get weekday */
			week = CaculateWeekDay(year, mon, day);
			if(week < 0)
			{
				tcdbg_printf("start_dbus_timer: get week fail!\n");
				continue;
			}
		}

		/* 
		Add timer control event below 
		considering priority lever of each timer, check SleepTimer firstly. 
		*/

		current = timer_data;
		memset(week_val, 0, sizeof(week_val));
		snprintf(week_val, sizeof(week_val), "%d", week);
		while (current)
		{
			hourVal = minVal = 0;
#if !defined(TCSUPPORT_CT_UBUS)
			if ( CTC_SLEEPTIMER_TYPE == current->type )
			{
#if defined(TCSUPPORT_CUC)
				doSleepTimerEvent(current->enable);
#else
				if ( 0 == atoi(current->week_day) )
				{
					doSleepTimerEvent(current->enable);
				}
 				else if ( NULL != strstr(current->week_day, week_val) )
				{
					sscanf(current->start_time, "%d:%d", &hourVal, &minVal);
					if ( hourVal == hour && minVal == min )
						doSleepTimerEvent(current->enable);
				}
#endif
			}

			/* do not check WIFI and LED Timers when system is in sleep state. */
			memset(HGWSleepVal, 0, sizeof(HGWSleepVal));
			cfg_obj_get_object_attr(TIMER_COMMON_NODE, "HGWSleep", 0, HGWSleepVal, sizeof(HGWSleepVal));
			if(!strcmp(HGWSleepVal, "0"))
			{
				current = current->next;
				continue;
			}
			
#if !defined(TCSUPPORT_CUC)
			if ( CTC_LEDTIMER_TYPE == current->type )
			{
				/* set LED action recover */
				/*if (0 == check_done)
				{
					led_action = BUTTON_RECOVER;
					check_done = 1;
				}*/
				/* check LED timer enable */
				/*if(!strcmp(current->enable,"TRUE"))*/
				{
					sscanf(current->start_time, "%d:%d", &hourVal, &minVal);
					if ( hourVal == hour && minVal == min )
					{
						/* do LED recover */
						led_action = BUTTON_RECOVER;
					}
					sscanf(current->end_time, "%d:%d", &hourVal, &minVal);
					if ( hourVal == hour && minVal == min )
					{
						/* do LED off */
						led_action = BUTTON_OFF;
					}
				}

				/* check LED current status */
				memset(ledStatus, 0, sizeof(ledStatus));
				cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "CurrStatus", 0, ledStatus, sizeof(ledStatus));
				if(led_action != atoi(ledStatus))
				{
					button_op(led_action);
				}
			}
			else if ( CTC_WIFITIMER1_TYPE == current->type )
			{
				if ( NULL != strstr(current->week_day, week_val) )
				{
					sscanf(current->start_time, "%d:%d", &hourVal, &minVal);
					if ( hourVal == hour && minVal == min )
					{
						ssid_mask = atoi(current->SSIDIndex);
						doWifiTimerEvent(current->enable, ssid_mask, 0);
					}
				}
			}
			else if ( CTC_WIFITIMER_TYPE == current->type )
#else
			if ( CTC_WIFITIMER_TYPE == current->type )
#endif
			{
				if (0 == strcasecmp(current->enable, "TRUE"))
				{
					need_done = 1;
					sscanf(current->start_time, "%d:%d", &hourVal, &minVal);
					if ( hourVal == hour && minVal == min )
					{
#if defined(TCSUPPORT_CUC)
						ssid_mask = WLAN_BITMASK;
#else
						ssid_mask = atoi(current->SSIDIndex);
#endif
						doWifiTimerEvent("True", ssid_mask, 0);
					}

					hourVal = minVal = 0;
					sscanf(current->end_time, "%d:%d", &hourVal, &minVal);
					if ( hourVal == hour && minVal == min )
					{
#if defined(TCSUPPORT_CUC)
						ssid_mask = WLAN_BITMASK;
#else
						ssid_mask = atoi(current->SSIDIndex);
#endif
						doWifiTimerEvent("False", ssid_mask, 0);
					}
				}
				else
				{
					if ( need_done ) 
					{
						need_done = 0;
#if defined(TCSUPPORT_CUC)
						ssid_mask = WLAN_BITMASK;
#else
						ssid_mask = atoi(current->SSIDIndex);
#endif
						doWifiTimerEvent("True", ssid_mask, 0);
					}
				}
			}
#else
			if ( CTC_WIFITIMER1_TYPE == current->type )
			{
				if ( NULL != strstr(current->week_day, week_val) )
				{
					if(!strcmp(current->enable,"False"))
						sscanf(current->end_time, "%d:%d", &hourVal, &minVal);
					if(!strcmp(current->enable,"True"))
					sscanf(current->start_time, "%d:%d", &hourVal, &minVal);
					
					if ( hourVal == hour && minVal == min )
					{
						ssid_mask = 0xffff;
						doWifiTimerEvent(current->enable, ssid_mask, 0);
					}
				}
			}
#endif
			current = current->next;
		}

		/* according to timer standard, checked by min */
		sleep(1);
	}
	
	return 0;
}


void ctc_dbus_timer_data_free()
{
	timer_entry_node_t *current = NULL, *tmp = NULL;

	if ( !timer_data )
		return;

	current = timer_data;
	while (current)
	{
		tmp = current;
		current = tmp->next;
		free(tmp);
		tmp = NULL;
	}
	timer_data = NULL;
}

/* return: 1 - match,  0 -not match */
static int check_time_match(int hour, int min, int real_hour, int real_min)
{	
	if ( hour == 23 && min == 59 )
	{
		if ( (real_hour == hour && real_min == min) 
			|| (0 == real_hour && 0 == real_min) )
		{
			return 1;
		}
	}
	else if ( real_hour == hour 
		&& (real_min - min >= 0) 
		&& (real_min - min <= 1) )
	{
		return 1;
	}
	return 0;
}

int init_wifi_enable_mask()
{
	int wifienablemask = 0;
	int ssidindex = 0, wifi_type = 0;
	char nodeName[64] = {0}, mask[8] = {0}, enablessid[4] = {0};
	
	for( ssidindex = 0; ssidindex < WLAN_2_4_G_NUM + WLAN_5_G_NUM; ssidindex++ ) 
	{
		wifi_type = ssidindex/WLAN_2_4_G_NUM;
		memset(enablessid, 0, sizeof(enablessid));
		memset(nodeName, 0, sizeof(nodeName));
		if (0 == wifi_type)
			snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, ssidindex + 1);
		else
			snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, ssidindex - WLAN_2_4_G_NUM + 1);
		
		cfg_get_object_attr(nodeName, "EnableSSID", enablessid, sizeof(enablessid));
		if ( 0 == strcmp(enablessid, "1") )
			SET_BIT(wifienablemask, ssidindex);
	}
	
	memset(mask, 0, sizeof(mask));
	snprintf(mask, sizeof(mask), "%d", wifienablemask);
	cfg_set_object_attr(TIMER_COMMON_NODE, "WifiEnableMask", mask);
	return 0;
}

int cfg_type_timer_data_boot(char* path)
{
	need_done = 0;
	cfg_set_object_attr(TIMER_COMMON_NODE, "HGWSleep", "1");
#if 0
	cfg_set_object_attr(TIMER_COMMON_NODE, "WifiEnableMask", "65535"); /*1111 1111 1111 1111 */
#else
	init_wifi_enable_mask();
#endif
	/*cfg_set_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "CurrStatus", "1");
	cfg_set_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "Status", "ON");*/
	cfg_set_object_attr("root.UsbMount.Common", "SleepStatus", "0");
	cfg_type_timer_data_update(0, path);
	return 0;
}

int cfg_type_timer_data_update(int flag, char* path)
{
	char nodeName[64] = {0};
	char activeVal[4] = {0};
	char dayVal[16] = {0};
	char tmpTimeVal[16] = {0};
	char startTimeVal[16] = {0};
	char endTimeVal[16] = {0};
	int hourVal, minVal = 0;
	char statusVal[8] = {0};
	char enableVal[8] = {0}, webLedSwitch[16] = {0};
	char SSIDIndex[16] = {0};
	int i = 0, webCommit = 0;
	timer_entry_node_t **new_data = NULL;

	ctc_dbus_timer_data_free();
	new_data = &timer_data;

#if !defined(TCSUPPORT_CT_UBUS)
	/* update sleeptimer data */
#if defined(TCSUPPORT_CUC)
	memset(nodeName, 0, sizeof(nodeName));
	memset(enableVal, 0, sizeof(enableVal));
	snprintf(nodeName, sizeof(nodeName), TIMER_ENTRY_NODE, 1);
	
	if( 0 != flag && cfg_obj_get_object_attr(nodeName, "Enable", 0, enableVal, sizeof(enableVal)) > 0 )
	{
		*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
		if ( *new_data )
		{
			bzero(*new_data, sizeof(timer_entry_node_t));
			(*new_data)->type = CTC_SLEEPTIMER_TYPE;
			strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
			(*new_data)->next = NULL;
			new_data = &((*new_data)->next);
		}
	}
#else
	for(i = 0; i < CTC_MAX_SLEEPTIMER_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		memset(activeVal, 0, sizeof(activeVal));
		memset(dayVal, 0, sizeof(dayVal));
		memset(enableVal, 0, sizeof(enableVal));
		snprintf(nodeName, sizeof(nodeName), TIMER_ENTRY_NODE, i + 1);
		if(cfg_obj_get_object_attr(nodeName, "Day", 0, dayVal, sizeof(dayVal)) < 0)
			continue;

		if(flag == 0 && !strcmp(dayVal, "0"))
		{
			cfg_obj_delete_object(nodeName);
			continue;
		}

		if(cfg_obj_get_object_attr(nodeName, "Enable", 0, enableVal, sizeof(enableVal)) < 0)
			continue;
		
		if(!strcmp(dayVal, "0") && enableVal[0] != 0)
		{
			*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
			if ( *new_data )
			{
				bzero(*new_data, sizeof(timer_entry_node_t));
				(*new_data)->type = CTC_SLEEPTIMER_TYPE;
				strncpy((*new_data)->week_day, dayVal, sizeof((*new_data)->week_day) - 1);
				strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
				(*new_data)->next = NULL;
				new_data = &((*new_data)->next);
			}
		}
		else
		{
			if(cfg_obj_get_object_attr(nodeName, "Active", 0, activeVal, sizeof(activeVal)) < 0)
				continue;

			if(!strcmp(activeVal, "1"))
			{
				if ( cfg_obj_get_object_attr(nodeName, "Time", 0, tmpTimeVal, sizeof(tmpTimeVal)) < 0 
					|| NULL == strstr(tmpTimeVal, ":") )
					continue;

				*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
				if ( *new_data )
				{
					bzero(*new_data, sizeof(timer_entry_node_t));
					(*new_data)->type = CTC_SLEEPTIMER_TYPE;
					strncpy((*new_data)->week_day, dayVal, sizeof((*new_data)->week_day) - 1);
					strncpy((*new_data)->start_time, tmpTimeVal, sizeof((*new_data)->start_time) - 1);
					strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
					(*new_data)->next = NULL;
					new_data = &((*new_data)->next);
				}
			}
		}
	}


	/* check WEB page action */
	if ( cfg_obj_get_object_attr(WEBCURSET_ENTRY_NODE, "webLedSwitch", 0
			, webLedSwitch, sizeof(webLedSwitch) ) > 0 )
	{
		cfg_obj_set_object_attr(WEBCURSET_ENTRY_NODE, "webLedSwitch", 0, "0");
		webCommit = atoi(webLedSwitch);
	}

	/* update LEDTimer data */
	memset(statusVal, 0, sizeof(statusVal));
	cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "Status", 0, statusVal, sizeof(statusVal));
	if( !strcasecmp(statusVal, "ON") )
	{
		memset(enableVal, 0, sizeof(enableVal));
		memset(dayVal, 0, sizeof(dayVal));
		if ( cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "Enable", 0, enableVal, sizeof(enableVal)) > 0 
			&& cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "ControlCycle", 0, dayVal, sizeof(dayVal)) > 0
			&& !strcasecmp(enableVal, "TRUE")
			&& !strcmp(dayVal, "DAY") )
		{
			/* get LED start and end time */	
			memset(startTimeVal, 0, sizeof(startTimeVal));
			memset(endTimeVal, 0, sizeof(endTimeVal));
			if ( cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "StartTime", 0, startTimeVal, sizeof(startTimeVal)) > 0 
				 && strstr(startTimeVal, ":") != NULL
				 && cfg_obj_get_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "EndTime", 0, endTimeVal, sizeof(endTimeVal)) > 0
				 && strstr(endTimeVal, ":") != NULL)
			{
				check_done = 0;
				*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
				if ( *new_data )
				{
					bzero(*new_data, sizeof(timer_entry_node_t));
					(*new_data)->type = CTC_LEDTIMER_TYPE;
					strncpy((*new_data)->status, statusVal, sizeof((*new_data)->status) - 1);
					strncpy((*new_data)->week_day, dayVal, sizeof((*new_data)->week_day) - 1);
					strncpy((*new_data)->start_time, startTimeVal, sizeof((*new_data)->start_time) - 1);
					strncpy((*new_data)->end_time, endTimeVal, sizeof((*new_data)->end_time) - 1);
					strncpy((*new_data)->enable, enableVal,sizeof((*new_data)->enable) - 1);
					(*new_data)->next = NULL;
					new_data = &((*new_data)->next);
				}
			}
		}
		
		if ( 1 == webCommit )
		{
			button_op(BUTTON_RECOVER);
		}
	}
	/*
	else if( !strcasecmp(statusVal, "OFF") )
	{
		*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
		if ( *new_data )
		{
			bzero(*new_data, sizeof(timer_entry_node_t));
			(*new_data)->type = CTC_LEDTIMER_TYPE;
			strncpy((*new_data)->status, statusVal, sizeof((*new_data)->status) - 1);
			(*new_data)->next = NULL;
			new_data = &((*new_data)->next);
		}
	}*/
	else if( !strcasecmp(statusVal, "OFF") )
	{
		if ( flag == 0 || 1 == webCommit )
			button_op(BUTTON_OFF); /* LED down */
	}

	/* update WIFItimer1 data */
	for(i = CTC_MAX_SLEEPTIMER_NUM; i < CTC_MAX_SLEEPTIMER_NUM + CTC_MAX_WIFITIMER1_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		memset(activeVal, 0, sizeof(activeVal));
		snprintf(nodeName, sizeof(nodeName), TIMER_ENTRY_NODE, i + 1);
		if(cfg_obj_get_object_attr(nodeName, "Active", 0, activeVal, sizeof(activeVal)) < 0)
			continue;

		if(!strcmp(activeVal, "1"))
		{
			memset(dayVal, 0, sizeof(dayVal));
			if(cfg_obj_get_object_attr(nodeName, "WeekDay", 0, dayVal, sizeof(dayVal)) < 0)
				continue;

			if ( cfg_obj_get_object_attr(nodeName, "Time", 0, tmpTimeVal, sizeof(tmpTimeVal)) < 0 
				|| NULL == strstr(tmpTimeVal, ":") )
				continue;

			if(cfg_obj_get_object_attr(nodeName, "Enable", 0, enableVal, sizeof(enableVal)) < 0)
				continue;

			memset(SSIDIndex, 0, sizeof(SSIDIndex));
			if(cfg_obj_get_object_attr(nodeName, "SSIDMask", 0, SSIDIndex, sizeof(SSIDIndex)) < 0)
				continue;

			*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
			if ( *new_data )
			{
				bzero(*new_data, sizeof(timer_entry_node_t));
				(*new_data)->type = CTC_WIFITIMER1_TYPE;
				strncpy((*new_data)->week_day, dayVal, sizeof((*new_data)->week_day) - 1);
				strncpy((*new_data)->start_time, tmpTimeVal, sizeof((*new_data)->start_time) - 1);
				strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
				strncpy((*new_data)->SSIDIndex, SSIDIndex, sizeof((*new_data)->SSIDIndex) - 1);
				(*new_data)->next = NULL;
				new_data = &((*new_data)->next);
			}
		}
	}
#endif
#else
	for(i = 0; i < CT_UBUS_MAX_WIFITIMER_NUM; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), TIMER_ENTRY_NODE, i + 1);
		if(NULL == path)
			break;

		memset(dayVal, 0, sizeof(dayVal));
		if(cfg_obj_get_object_attr(nodeName, "weekday", 0, dayVal, sizeof(dayVal)) < 0)
			continue;

		if ( cfg_obj_get_object_attr(nodeName, "time", 0, tmpTimeVal, sizeof(tmpTimeVal)) < 0 
			|| NULL == strstr(tmpTimeVal, ":") )
			continue;

		if(cfg_obj_get_object_attr(nodeName, "enable", 0, enableVal, sizeof(enableVal)) < 0)
			continue;

		memset(SSIDIndex, 0, sizeof(SSIDIndex));
		strncpy(SSIDIndex, "65535", sizeof(SSIDIndex)-1);

		*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
		if ( *new_data )
		{
			bzero(*new_data, sizeof(timer_entry_node_t));
			(*new_data)->type = CTC_WIFITIMER1_TYPE;
			strncpy((*new_data)->week_day, dayVal, sizeof((*new_data)->week_day) - 1);
			if(!strcmp(enableVal,"True"))
			strncpy((*new_data)->start_time, tmpTimeVal, sizeof((*new_data)->start_time) - 1);
			else if (!strcmp(enableVal,"False"))
				strncpy((*new_data)->end_time, tmpTimeVal, sizeof((*new_data)->end_time) - 1);
			else{
				/*can not get here*/}
			strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
			strncpy((*new_data)->SSIDIndex, SSIDIndex, sizeof((*new_data)->SSIDIndex) - 1);
			(*new_data)->next = NULL;
			new_data = &((*new_data)->next);
		}

	}
#endif
#if !defined(TCSUPPORT_CT_UBUS)
	/* update WIFItimer data */
	memset(enableVal, 0, sizeof(enableVal));
	memset(dayVal, 0, sizeof(dayVal));
	memset(SSIDIndex, 0, sizeof(SSIDIndex));
	if ( cfg_obj_get_object_attr(TIMER_WIFITIMER_ENTRY_NODE, "Enable", 0, enableVal, sizeof(enableVal)) > 0 
		/* && cfg_obj_get_object_attr(TIMER_WIFITIMER_ENTRY_NODE, "ControlCycle", 0, dayVal, sizeof(dayVal)) > 0 */
#if !defined(TCSUPPORT_CUC)
		&& cfg_obj_get_object_attr(TIMER_WIFITIMER_ENTRY_NODE, "SSIDMask", 0, SSIDIndex, sizeof(SSIDIndex)) > 0 
#endif
		)
	{
		if ( !strcasecmp(enableVal, "TRUE") 
			/*&& !strcmp(dayVal, "DAY" ) */
#if !defined(TCSUPPORT_CUC)
			&& SSIDIndex[0] != 0 
#endif
			)
		{
			/* get wifi start and end time */
			memset(startTimeVal, 0, sizeof(startTimeVal));
			memset(endTimeVal, 0, sizeof(endTimeVal));
  			if ( cfg_obj_get_object_attr(TIMER_WIFITIMER_ENTRY_NODE, "StartTime", 0, startTimeVal, sizeof(startTimeVal)) > 0 
				&& strstr(startTimeVal, ":") != NULL
				&& cfg_obj_get_object_attr(TIMER_WIFITIMER_ENTRY_NODE, "EndTime", 0, endTimeVal, sizeof(endTimeVal)) > 0 
				&& strstr(endTimeVal, ":") != NULL )
  			{
  				*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
				if ( *new_data )
				{
					bzero(*new_data, sizeof(timer_entry_node_t));
					(*new_data)->type = CTC_WIFITIMER_TYPE;
					strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
					strncpy((*new_data)->start_time, startTimeVal, sizeof((*new_data)->start_time) - 1);
					strncpy((*new_data)->end_time, endTimeVal, sizeof((*new_data)->end_time) - 1);
#if !defined(TCSUPPORT_CUC)
					strncpy((*new_data)->SSIDIndex, SSIDIndex, sizeof((*new_data)->SSIDIndex) - 1);
#endif
					(*new_data)->next = NULL;
					new_data = &((*new_data)->next);
				}
  			}
		}
		else if ( !strcasecmp(enableVal, "FALSE")
#if !defined(TCSUPPORT_CUC)
			&& SSIDIndex[0] != 0 
#endif
			)
		{
			*new_data = (timer_entry_node_t *)malloc(sizeof(timer_entry_node_t));
			if ( *new_data )
			{
				bzero(*new_data, sizeof(timer_entry_node_t));
				(*new_data)->type = CTC_WIFITIMER_TYPE;
				strncpy((*new_data)->enable, enableVal, sizeof((*new_data)->enable) - 1);
#if !defined(TCSUPPORT_CUC)
				strncpy((*new_data)->SSIDIndex, SSIDIndex, sizeof((*new_data)->SSIDIndex) - 1);
#endif
				(*new_data)->next = NULL;
				new_data = &((*new_data)->next);
			}
		}
	}
#endif

	return 0;
}

/* dbus timer init */
int 
ctc_dbus_timer_init(void)
{
	pthread_t thread_id;

	/* create notify service */
	if (pthread_create(&thread_id, NULL, (void *)start_dbus_timer, NULL) != 0) {
		return -1;
	}

	return 0;
}


