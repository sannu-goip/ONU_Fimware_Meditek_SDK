#ifndef __SVC_WAN_RELATED_PPPOESIMULATE_CFG_H__
#define __SVC_WAN_RELATED_PPPOESIMULATE_CFG_H__

#define PPPOE_SIMULATE_ENTRY "Entry"
#define PPPOE_SIMULATE_NODE_NAME "PPPoESimulate"
#define PPPOE_SIMULATE_PID_PATH "/var/run/pppoe_sim.pid"
#define PPPOE_SIMULATE_CONF		"/var/run/pppoe_sim.conf"
#define PPPOE_SIMULATE_SH "/usr/script/pppoe_simulate.sh &"
#define PPPOE_SIMULATE_USER_STOP		1
#define PPPOE_SIMULATE_UNKNOW_STOP 	2
#define PPPOE_SIMULATE_COMPLETE			3
#define NO_QMARKS 		0
#define QMARKS 			1

#define CUC_PING_TEST_RESULT_RUNNING	"1"
#define CUC_PING_TEST_RESULT_NOT_START	"2"
#define CUC_PING_TEST_RESULT_UNKNOWN_IP	"4"
#define CUC_PING_TEST_RESULT_NO_ROUTE		"5"
#define CUC_PING_TEST_PID_PATH "/var/run/cuc_voip_ping.pid"
#define CUC_PING_TEST_ADDROUTE_CMD	"/sbin/route add -host %s dev %s"
#define CUC_PING_TEST_DELROUTE_CMD	"/sbin/route del %s"
#define CUC_PING_TEST_CMD	"/bin/ping -U 1 -c %s  -s %s -W %s %s &"
#define CUC_PING_TEST_ACTION_COMPLETE	"2"
#define IFNAME_LEN 16

int svc_wan_related_pppoesimulate(void);
int svc_wan_related_cucping(void);

#endif
