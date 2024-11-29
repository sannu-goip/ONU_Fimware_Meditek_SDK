#ifndef __SVC_CENTRAL_CTRL_CHECK__BUTTON_H__
#define __SVC_CENTRAL_CTRL_CHECK__BUTTON_H__


#define LED_SWITCH_BUTTON_PATH      "/proc/tc3162/led_switch_button"

#define RESET_BUTTON_PATH 			"/proc/tc3162/reset_button"

#define BUTTON_TYPE_PATH 			"/proc/tc3162/button_type"


#define SYSTEM				"System"
#define SUB_NODE_NAME		"Entry"

#define PTS_NUMBER 5

#define BUTTON_RECOVER      (1)
#define BUTTON_OFF          (0)

void check_led_switch_button(char xpon_traffic_state);
void check_reset_button(void);

void  check_button_type(void);

char button_op(char op);
#endif


