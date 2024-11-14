#ifndef _SVC_OTHER_PARENTAL_COMMON_H__
#define _SVC_OTHER_PARENTAL_COMMON_H__

#define PARENTAL 					"Parental"
#define PARENTAL_NODE 				"Entry"
#define PARENTAL_COMMON 			"Common"
#define PARENTAL_NUM 				"Num"
#define PARENTAL_ACTIVE 			"Active"
#define PARENTAL_POLICY 			"Policy"
#define PARENTAL_ID 				"ParentalID"
#define PARENTAL_MAC 				"ParentalMac"
#define PARENTAL_MAC_MAC 			"Mac"
#define PARENTAL_MAC_DESC 			"Description"
#define PARENTAL_DURATION_START 	"StartTime"
#define PARENTAL_DURATION_END 		"EndTime"
#define PARENTAL_DURATION_REPEAT 	"RepeatDay"
#define PARENTAL_URLFILTER_URL 		"URL"
#define PARENTAL_DURATION_ACTIVE 	"DurationActive"
#define PARENTAL_DURATION_POLICY 	"DurationPolicy"
#define MAX_PARENTAL_NUM	8
#define MAX_MAC_PER_ENTRY	4
#define MAX_MAC_ARRAY_SIZE	20

#define MAX_DURATION_PER_ENTRY	4
#define MAX_URL_PER_ENTRY	10
#define MAX_PARENTAL_MAC_NUM		MAX_PARENTAL_NUM * MAX_MAC_PER_ENTRY
#define MAX_PARENTAL_DURATION_NUM	MAX_PARENTAL_NUM * MAX_DURATION_PER_ENTRY
#define MAX_PARENTAL_URLFILTER_NUM	MAX_PARENTAL_NUM * MAX_URL_PER_ENTRY
#define MAX_PARENTAL_MAC_LEN	18
#define PARENTAL_URL_PATTERN_NAME 	"parental_url"

typedef struct{
	unsigned int start;
	unsigned int end;
	unsigned char repeat_days;
	unsigned char matched_stat;
}parental_duration_t;

typedef struct
{
	parental_duration_t duration[MAX_DURATION_PER_ENTRY];
}parental_setting_t;


int isSetpIntoDuration(int node_idx, int dur_idx);
int isGoOutOfDuration(int node_idx, int dur_idx);
void restartParental(void);
void initParentalDuration(void);
#endif

