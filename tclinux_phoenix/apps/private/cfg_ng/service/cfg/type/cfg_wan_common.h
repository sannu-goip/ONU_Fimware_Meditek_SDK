#ifndef __CFG_WAN_COMMON_H__ 
#define __CFG_WAN_COMMON_H__ 
#if defined(TCSUPPORT_CMCC)
#define MAX_WAN_PVC_NUMBER		1
#else
#define MAX_WAN_PVC_NUMBER		8
#endif
#define MAX_WAN_ENTRY_NUMBER	8
#define MAX_WAN_INTF_NUMBER		(MAX_WAN_PVC_NUMBER * MAX_WAN_ENTRY_NUMBER)

#define DYNMIC_MODE	"0"
#define STATIC_MODE		"1"
#define PPP_MODE		"2"
#define BRIDGE_MODE	"3"

#endif 
