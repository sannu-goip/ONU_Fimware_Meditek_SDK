#ifndef _DBUS_TIMER_H
#define _DBUS_TIMER_H

#define CTC_SLEEPTIMER_TYPE		(1)
#define CTC_WIFITIMER1_TYPE		(2)
#define CTC_LEDTIMER_TYPE		(3)
#define CTC_WIFITIMER_TYPE		(4)

#define CTC_MAX_SLEEPTIMER_NUM	(14)
#define CTC_MAX_WIFITIMER1_NUM	(14)
#if defined(TCSUPPORT_CT_UBUS)
#define CT_UBUS_MAX_WIFITIMER_NUM	(32)
#endif

#if defined(TCSUPPORT_MULTI_USER_ITF)
#define WLAN_2_4_G_NUM 8
#define WLAN_BITMASK   0xffff
#else
#define WLAN_2_4_G_NUM 4
#define WLAN_BITMASK   0xff
#endif
#define WLAN_5_G_NUM WLAN_2_4_G_NUM
#define LAN_NUM 4


typedef struct timer_entry_node_s{
	int type;
	char week_day[16];
	char start_time[16];
	char end_time[16];
	char status[8];
	char enable[8];
	char SSIDIndex[16];
	struct timer_entry_node_s *next;
}timer_entry_node_t;

int doSleepTimerEvent(char *enable);
int doWifiTimerEvent(char *enable, int ssidmask, int optflag);
int ctc_dbus_timer_init(void);
void ctc_dbus_timer_data_free();
int cfg_type_timer_data_boot(char* path);
int cfg_type_timer_data_update(int flag, char* path);

#endif
