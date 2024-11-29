#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <cfg_api.h>
#include <utility.h>
#include <cfg_msg.h>
#include <time.h>
#include <linux/version.h>
#include "parental_common.h"

#define MAXLEN_ATTR_NAME		32
#define MAX_URL_INPUT_LENGTH 	48
#define MAC_FILTER 1
#define URL_FILTER 0
#if defined(TCSUPPORT_CUC)
#define INTERNET_CRL 2
#endif
#define BLACK_TYPE 1
#define WHITE_TYPE 0



static parental_setting_t parental_setting[MAX_PARENTAL_NUM];

static void execParentalRule(const char *fmt, ...)
{
	va_list args;
	char rule[200]= {0};
	
	memset(rule, 0, sizeof(rule));
	
	va_start(args, fmt);
	vsprintf(rule, fmt, args);
	va_end(args);
	
	system(rule);
}

void initParentalDuration(void)
{
	char node_name[32];
    char attr_name[32];
	char node_val[128];
	int node_idx = 0;
	int dur_idx = 0;
	parental_duration_t *duration = NULL;
    
	bzero(parental_setting, sizeof(parental_setting));
	for (node_idx = 0; node_idx < MAX_PARENTAL_NUM; node_idx++)
	{
		duration = parental_setting[node_idx].duration;
		memset(duration, 0, sizeof(parental_duration_t)*MAX_DURATION_PER_ENTRY);
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (node_idx+1));
		memset(node_val, 0, sizeof(node_val));
#if !defined(TCSUPPORT_CUC)
		if(0 >= cfg_get_object_attr(node_name, PARENTAL_DURATION_ACTIVE, node_val, sizeof(node_val)) 
			|| 0 >= strlen(node_val))
		{
			continue;
		}
#endif
		
		for (dur_idx = 0; dur_idx < MAX_DURATION_PER_ENTRY; dur_idx++)
		{
            snprintf(attr_name, sizeof(attr_name), "%s%d", PARENTAL_DURATION_START, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration[dur_idx].start = atoi(node_val);
			}
			
            snprintf(attr_name, sizeof(attr_name), "%s%d", PARENTAL_DURATION_END, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration[dur_idx].end = atoi(node_val);
			}
			
            snprintf(attr_name, sizeof(attr_name), "%s%d", PARENTAL_DURATION_REPEAT, dur_idx);
			memset(node_val, 0, sizeof(node_val));
			if( 0 < cfg_get_object_attr(node_name, attr_name, node_val, sizeof(node_val)) )
			{
				duration[dur_idx].repeat_days = atoi(node_val);
			}
		}
	}
}

static void clearParental(void)
{
	int node_idx = 0;

	execParentalRule("iptables -t filter -F parental_black_chain_in");
	execParentalRule("iptables -t filter -Z parental_black_chain_in");
	execParentalRule("iptables -t filter -D INPUT -j parental_black_chain_in");
	execParentalRule("iptables -t filter -X parental_black_chain_in");

	execParentalRule("iptables -t filter -F parental_black_chain_fwd");
	execParentalRule("iptables -t filter -Z parental_black_chain_fwd");
	execParentalRule("iptables -t filter -D FORWARD -j parental_black_chain_fwd");
	execParentalRule("iptables -t filter -X parental_black_chain_fwd");

	for (node_idx = 0; node_idx < MAX_PARENTAL_NUM; node_idx++)
	{
		execParentalRule("rm -f /etc/l7-protocols/%s_%d.pat"
				, PARENTAL_URL_PATTERN_NAME, node_idx);
		execParentalRule("iptables -t filter -F parental_black_chain_in%d"
							, node_idx);
		execParentalRule("iptables -t filter -Z parental_black_chain_in%d"
							, node_idx);
		execParentalRule("iptables -t filter -X parental_black_chain_in%d"
							, node_idx);
		execParentalRule("iptables -t filter -F parental_black_chain_fwd%d"
							, node_idx);
		execParentalRule("iptables -t filter -Z parental_black_chain_fwd%d"
							, node_idx);
		execParentalRule("iptables -t filter -X parental_black_chain_fwd%d"
							, node_idx);
	}

	execParentalRule("rm -f /etc/l7-protocols/%s_%d.pat"
		, PARENTAL_URL_PATTERN_NAME, 999);
}

static int isParentalEntryNodeActive(int idx)
{
	char node_val[32];
	char node_name[32];

	memset(node_val, 0, sizeof(node_val));
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (idx+1));
	if( 0 < cfg_get_object_attr(node_name, PARENTAL_ACTIVE, node_val, sizeof(node_val)) 
		&& (0 == strcmp(node_val, "Yes")))
	{
		return 1;
	}

	return 0;
}

static int isParentalEntryNodeDurationActive(int idx)
{
	char node_val[32];
	char node_name[32];

	memset(node_val, 0, sizeof(node_val));
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (idx+1));
	if( 0 < cfg_get_object_attr(node_name, PARENTAL_DURATION_ACTIVE, node_val, sizeof(node_val)) 
		&& (0 == strcmp(node_val, "Yes")))
	{
		return 1;
	}

	return 0;	
}

static void check_and_set_l7filter(unsigned int new_filter_state)
{
	char nodePath[64] = {0};
	char newFilterState[32] = {0};
	char vername[CFG_BUF_16_LEN] = {0}, module_path[CFG_BUF_128_LEN] = {0};
	
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)) ;

	if(( atoi(newFilterState) & L7_BIT_MASK) != 0)/*pre is on*/
	{
		if((new_filter_state & L7_BIT_MASK) == 0)/*now is down*/
		{
			fprintf(stderr,"========================rmmod l7filter=======================\n");
			system("iptables -t filter -F app_filter_chain");
			system("iptables -t filter -F url_filter_chain");
			system("rmmod xt_layer7");
		}
	}
	else/*now is down*/
	{
		if((new_filter_state & L7_BIT_MASK) != 0)/*now is down*/
		{
			fprintf(stderr,"========================insmod l7filter=======================\n");
			decideModulePath(vername, CFG_BUF_16_LEN);
			snprintf(module_path, sizeof(module_path), INSMOD_XT_LAYER7_CMD, vername);
			system(module_path);
		}
	}
}



static int checkParentalOn(void)
{
	unsigned int new_parental_sta = 0;
	char nodePath[64];
	char newFilterState[32];
	char node_val[32];
	
	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), GLOBALSTATE_PRESYSSTATE_NODE);
	if(0 >= cfg_get_object_attr(nodePath, "FilterState", newFilterState, sizeof(newFilterState)))
	{
		return 0;
	}
	
	memset(node_val, 0, sizeof(node_val));
	if( 0 < cfg_get_object_attr(PARENTAL_COMMON_NODE, PARENTAL_ACTIVE, node_val, sizeof(node_val)) 
		&& (0 == strcmp(node_val, "Yes")))
	{
		new_parental_sta = atoi(newFilterState) | PARENTAL_VECTOR;
	}
	else
	{
		new_parental_sta = atoi(newFilterState) & (~PARENTAL_VECTOR);
	}
	
	check_and_set_l7filter(new_parental_sta);
	check_and_set_filter(new_parental_sta);
	
	if (new_parental_sta & PARENTAL_VECTOR)
	{
		return 1;
	}

	return 0;
}



static void initParental()
{
	int node_idx = 0;

	execParentalRule("iptables -t filter -N parental_black_chain_in");
	execParentalRule("iptables -t filter -I INPUT -j parental_black_chain_in");

	execParentalRule("iptables -t filter -N parental_black_chain_fwd");
	execParentalRule("iptables -t filter -I FORWARD -j parental_black_chain_fwd");

	for (node_idx = 0; node_idx < MAX_PARENTAL_NUM; node_idx++)
	{
	
		if (isParentalEntryNodeActive(node_idx) 
			|| isParentalEntryNodeDurationActive(node_idx))
		{
			execParentalRule("iptables -t filter -N parental_black_chain_in%d"
				, node_idx);
			execParentalRule("iptables -t filter -A parental_black_chain_in -j "
				"parental_black_chain_in%d"
				, node_idx);
			execParentalRule("iptables -t filter -N parental_black_chain_fwd%d"
				, node_idx);
			execParentalRule("iptables -t filter -A parental_black_chain_fwd -j "
				"parental_black_chain_fwd%d"
				, node_idx);
		}
	}
}

static int createL7Rules(char *url, char *out, int out_len)
{
	char *pwww = NULL, *phost = NULL;
	char *phttp = NULL;
	
	if ( NULL == url )
		return -1;

	phttp = strstr(url, "https://");
	if(phttp){
		phttp = phttp + strlen("https://");
	}
	else{
		phttp = strstr(url, "http://");
		if(phttp){
			phttp = phttp + strlen("http://");
		}
	}

	if(phttp){
		pwww = strstr(phttp, "www.");	
		if ( pwww && strlen(phttp) > 4){
			phost = pwww + 4;
		}
		else{
			phost = phttp;
		}
	}
	else
	{
	pwww = strstr(url, "www.");
		if ( pwww && strlen(url) > 4){
		phost = pwww + 4;
		}
		else{
		phost = url;
		}
	}

	snprintf(out, out_len, 
		"(Host:[\\x09-\\x0d ]* .*%s|Refer:[\\x09-\\x0d ]* .*%s)"
		, phost, phost);

	return 0;
}

static int write2ParentalFile(int node_idx, char *url)
{
	FILE *fp_url = NULL;
	char file_path[128]= "";
	int is_file_exist = 0;

	sprintf(file_path, "/etc/l7-protocols/%s_%d.pat", PARENTAL_URL_PATTERN_NAME, node_idx);
	
	fp_url = fopen(file_path, "r");
	if (fp_url){
		is_file_exist = 1;
		fclose(fp_url);
	}

	fp_url = fopen(file_path, "a+");
	if (!fp_url)
		return FAIL;
	
	if (is_file_exist)
		fprintf(fp_url, "|%s", url);
	else{
		fprintf(fp_url, "%s_%d\n", PARENTAL_URL_PATTERN_NAME, node_idx);
		fprintf(fp_url, "%s", url);
	}

	fclose(fp_url);

	return SUCCESS;
}


static void initParentalUrlfilter(void)
{
	char node_name[32];
	char node_val[128];
	char url[512];
	char fmt_url[MAX_URL_INPUT_LENGTH*2 + 30] = "";
	char attr_name[MAXLEN_ATTR_NAME + 1] = "";
	int url_idx = 0;
	int node_idx = 0;
	int urlNum = 0;
    
    for (node_idx = 0; node_idx < MAX_PARENTAL_NUM; node_idx++)
	{
		memset(node_name,0,sizeof(node_name));
		snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (node_idx+1));
		if(0 >= cfg_get_object_attr(node_name, PARENTAL_ACTIVE, node_val, sizeof(node_val)))
		{
        	continue;
		}
				
    	for (url_idx = 0; url_idx < MAX_URL_PER_ENTRY; url_idx++)
    	{	
			if (!isParentalEntryNodeActive(node_idx))
			{
				continue;
			}
            snprintf(attr_name, sizeof(attr_name), "%s%d", PARENTAL_URLFILTER_URL, url_idx);
			memset(url, 0, sizeof(url));
			if(0 >= cfg_get_object_attr(node_name, attr_name, url, sizeof(url))
				|| 0 >= strlen(url))
			{
				continue;
			}
			urlNum++;
			createL7Rules(url, fmt_url, sizeof(fmt_url));
			write2ParentalFile(node_idx, fmt_url);
		}

		memset(node_val, 0, sizeof(node_val));
		snprintf(node_val, sizeof(node_val), "%d", urlNum);
		cfg_obj_set_object_attr(node_name, "urlNum", 0, node_val);
		
	}

    write2ParentalFile(999, "(Host:[\\x09-\\x0d ]*)");
}


static int isDurationMatched(int node_idx)
{
	int dur_idx = 0;
	char node_name[32] = {0};
	char induration[4] = {0};
	parental_duration_t *duration = NULL;

	duration = parental_setting[node_idx].duration;
	snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (node_idx+1));
	snprintf(induration, sizeof(induration), "%d", 0);
	cfg_obj_set_object_attr(node_name, "induration", 0, induration);
	
	for (dur_idx = 0; dur_idx < MAX_DURATION_PER_ENTRY; dur_idx++)
	{
		if (duration[dur_idx].matched_stat)
		{
			snprintf(induration, sizeof(induration), "%d", 1);
			cfg_obj_set_object_attr(node_name, "induration", 0, induration);
			return 1;
		}
	}
	return 0;
}

static int getDuationType(int idx)
{
	char node_val[32];
	char node_name[32];

	memset(node_val, 0, sizeof(node_val));
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (idx+1));
	if( 0 < cfg_get_object_attr(node_name, PARENTAL_DURATION_POLICY, node_val, sizeof(node_val)) 
		&& (0 == strcmp(node_val, "White")))
	{
		return 0;
	}

	return 1;	
}

static int getFilterType(int idx)
{
	char node_val[32];
	char node_name[32];

	memset(node_val, 0, sizeof(node_val));
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, (idx+1));
	if( 0 < cfg_get_object_attr(node_name, PARENTAL_POLICY, node_val, sizeof(node_val)) 
		&& (0 == strcmp(node_val, "White")))
	{
		return 0;
	}

	return 1;	
}

static int getParentalMac(int node_idx, char mac[][MAX_MAC_ARRAY_SIZE])
{
	char node_val[32] = {0};
	char node_name[32] = {0};
	int idx = 0, cnt = 0,node_val_i = 0;
	char macNum[4] = {0};

	for (idx = 0, cnt = 0; idx < MAX_PARENTAL_MAC_NUM && cnt < MAX_MAC_PER_ENTRY; idx++)
	{
		memset(node_val, 0, sizeof(node_val));
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_MAC_ENTRY_NODE, (idx+1));
		if(0 <= cfg_get_object_attr(node_name, PARENTAL_ID, node_val, sizeof(node_val)) &&  '\0' != node_val[0])
		{
			node_val_i = atoi(node_val);
		}

		if(node_idx == node_val_i)
		{
			memset(node_val, 0, sizeof(node_val));
			if(0 <= cfg_get_object_attr(node_name, PARENTAL_MAC_MAC, node_val, sizeof(node_val)) &&  '\0' != node_val[0])
			{
			strncpy(mac[cnt], node_val, MAX_MAC_ARRAY_SIZE -1);
			cnt++;
		}
	}
	}

	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "%s.%d", PARENTAL_ENTRY_NODE, node_idx+1);
	snprintf(macNum, sizeof(macNum), "%d", cnt);
	cfg_obj_set_object_attr(node_name, "macNum", 0, macNum);
	
	return cnt;
}

static void addParentalRule(int node_idx, char *mac, int filter_type, int is_black_type)
{
	char mac_rule[64] = {0};
	char url_rule[64] = {0};

	if ( NULL == mac || mac[0] == '\0')
		return;
	
	memset(mac_rule, 0, sizeof(mac_rule));
	memset(url_rule, 0, sizeof(url_rule));
	snprintf(mac_rule, sizeof(mac_rule), "-m mac --mac-source %s", mac);
	snprintf(url_rule, sizeof(url_rule), "-m layer7 --l7proto %s_%d"
		, PARENTAL_URL_PATTERN_NAME, node_idx);
	/* MAC FILTER */
	if ( 1 == filter_type )
	{
		/* BLACK */
		if ( 1 == is_black_type )
		{			
			execParentalRule("iptables -t filter -A parental_black_chain_in%d "
				"%s -p udp --dport 53 -j DROP", node_idx, mac_rule);
			execParentalRule("iptables -t filter -A parental_black_chain_fwd%d %s"
				" -p tcp --dport 80 -j DROP", node_idx, mac_rule);
			execParentalRule("iptables -t filter -A parental_black_chain_fwd%d %s"
				" -p tcp --dport 443 -j DROP", node_idx, mac_rule);
		}
	}
#if defined(TCSUPPORT_CUC)
	else if( 2 == filter_type )
	{
		execParentalRule("iptables -t filter -A parental_black_chain_fwd%d"
				" -i br0 %s -j DROP", node_idx, mac_rule);
	}
#endif
	else /* URL FILTER */
	{	
		/* BLACK */
		if ( 1 == is_black_type )
		{   
			execParentalRule("iptables -t filter -A parental_black_chain_fwd%d %s %s"
			" -j DROP", node_idx, mac_rule, url_rule);
		}
		else
		{
			/* WHITE */
			execParentalRule("iptables -t filter -A parental_black_chain_fwd%d %s %s"
			" -j ACCEPT", node_idx, mac_rule, url_rule);

			snprintf(url_rule, sizeof(url_rule), "-m layer7 --l7proto %s_%d"
			, PARENTAL_URL_PATTERN_NAME, 999);

			execParentalRule("iptables -t filter -A parental_black_chain_fwd%d %s %s"
			" -j DROP"
			, node_idx
			, mac_rule, url_rule);
		}
	}
}

static void startParental(void)
{
	int node_idx = 0, mac_idx = 0, mac_cnt = 0;
	char mac[MAX_MAC_PER_ENTRY][MAX_MAC_ARRAY_SIZE];
	int url_type_flag = 1, use_filter_mode = 99;
	int duation_in_flag = 0,duation_active_flag = 0,duation_type_flag =0,mac_rule_active = 0;

	for (node_idx = 0; node_idx < MAX_PARENTAL_NUM; node_idx++)
	{
		use_filter_mode = 99;
		/* TIME filter sw */
		duation_active_flag = isParentalEntryNodeDurationActive(node_idx);
		/* URL filter sw */
		mac_rule_active = isParentalEntryNodeActive(node_idx);
		
		/* BOTH disabled. */
		if (!mac_rule_active && !duation_active_flag)
			continue;
		/*
		* 1 --> in time range
		* 0 -> out of time range
		*/
		duation_in_flag = isDurationMatched(node_idx);
		/*
		* TIME mdoe
		* 1 --> black 
		* 0 --> white
		*/
		duation_type_flag = getDuationType(node_idx);
		/*
		* URL mode 
		* 1 --> black 
		* 0 --> white
		*/
		url_type_flag = getFilterType(node_idx);

#if defined(TCSUPPORT_CUC)
		duation_active_flag = 1;
		duation_type_flag = url_type_flag;
#endif
		/* TIME mode */
		if ( duation_active_flag )
		{
			/* BLACK type */
			if ( duation_type_flag )
			{
				/* in range */
				if ( duation_in_flag )
				{
					/* update MAC black table rule */
#if defined(TCSUPPORT_CUC)
					if ( mac_rule_active )
					{
						/* update URL black/white  table rule */
						use_filter_mode = URL_FILTER;
					}
#else
					use_filter_mode = MAC_FILTER;
#endif
				}
				else /* out time range*/
				{
#if defined(TCSUPPORT_CUC)
					use_filter_mode = INTERNET_CRL;
#else
					/* URL mode */
					if(mac_rule_active)
					{
						/* update URL black/white  table rule */
						use_filter_mode = URL_FILTER;
					}
#endif
				}
			}
			else 
			{
				/* in range */
				if ( duation_in_flag )
				{
					/* URL mode */
					if ( mac_rule_active )
					{
						/* update URL black/white  table rule */
						use_filter_mode = URL_FILTER;
					}
				}
				else /* out time range*/
				{
#if defined(TCSUPPORT_CUC)
					use_filter_mode = INTERNET_CRL;
#else
					/* update MAC black table rule */
					use_filter_mode = MAC_FILTER;
#endif
				}
			}
		}
		else
		{
			/* URL mode */
			if ( mac_rule_active )
			{
				/* update URL black/white  table rule */
				use_filter_mode = URL_FILTER;
			}
		}

		memset(mac, 0, (MAX_MAC_PER_ENTRY * sizeof(mac[0][0])));
		mac_cnt = getParentalMac(node_idx, mac);
		if ( MAC_FILTER == use_filter_mode )
		{
			/* update MAC black table rule */
			for (mac_idx = 0; mac_idx < mac_cnt; mac_idx++)
			{
				addParentalRule(node_idx, mac[mac_idx]
						, use_filter_mode, BLACK_TYPE);
			}
		}
		else if ( URL_FILTER == use_filter_mode )
		{
			/* update URL black/white  table rule */
			for ( mac_idx = 0; mac_idx < mac_cnt; mac_idx++ )
			{
				addParentalRule(node_idx, mac[mac_idx]
						, use_filter_mode
						, ( url_type_flag ? BLACK_TYPE : WHITE_TYPE));
			}
		}
#if defined(TCSUPPORT_CUC)
		else if( INTERNET_CRL == use_filter_mode )
		{
			for ( mac_idx = 0; mac_idx < mac_cnt; mac_idx++ )
			{
				addParentalRule(node_idx, mac[mac_idx]
						, use_filter_mode
						, 0);
			}
		}
#endif
		else
		{
			use_filter_mode = 99;
		}
	}
}

static int isCurrentInDuration(parental_duration_t *duration)
{
#define GET_BIT_IN_BYTE(idx, byte)	(((unsigned char)(1<<(idx))) & ((unsigned char)byte))
	struct tm *cur_time = NULL;
	time_t t;
	int    week = 0;
	int cur_sec = 0;

	time(&t);
	cur_time = localtime(&t);
	cur_sec = cur_time->tm_hour * 3600 + cur_time->tm_min * 60 + cur_time->tm_sec;
	for (week = 0; week < 7; week++)
	{
		if (!GET_BIT_IN_BYTE(week, duration->repeat_days) || week != cur_time->tm_wday)
		{
			continue;
		}
		if (cur_sec >= duration->start && cur_sec <= duration->end)
		{
			return 1;
		}
	}

	return 0;
}

int isSetpIntoDuration(int node_idx, int dur_idx)
{
	parental_duration_t *duration = &parental_setting[node_idx].duration[dur_idx];

	if (duration->matched_stat)
	{
		return 0;
	}
	
	if (!isCurrentInDuration(duration))
	{
		return 0;
	}

	duration->matched_stat = 1;
	return 1;
}

int isGoOutOfDuration(int node_idx, int dur_idx)
{
	parental_duration_t *duration = &parental_setting[node_idx].duration[dur_idx];

	if (!duration->matched_stat)
	{
		return 0;
	}

	if (isCurrentInDuration(duration))
	{
		return 0;
	}

	duration->matched_stat = 0;
	return 1;
}

void restartParental(void)
{
	char buf[4] = {0};
	
	cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "isParentalRestarting", buf, sizeof(buf));
	if(atoi(buf) != 1)
	{
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "isParentalRestarting", "1");
	clearParental();
	if (checkParentalOn()){
		initParental();
		initParentalUrlfilter();
		startParental();
	}
		
		system("/usr/bin/killall -9 dnsmasq");
		system("/userfs/bin/dnsmasq &");
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "isParentalRestarting", "0");
	}
}




