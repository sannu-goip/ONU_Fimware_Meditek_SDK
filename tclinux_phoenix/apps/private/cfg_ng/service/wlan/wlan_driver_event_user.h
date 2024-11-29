#ifndef __WLAN_DRIVER_EVENT_USER_
#define __WLAN_DRIVER_EVENT_USER_

#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#define IW_CHANNEL_CHANGE_EVENT_FLAG				0x0213
#define OID_802_11_RRM_EVENT	0x094D
#define OID_802_11_WNM_EVENT	0x094B
#define OID_802_11_CONN_FAIL_MSG 0x0958
#define RT_PROBE_REQ_REPORT_EVENT	0x1700

#define OID_802_11_WNM_BTM_RSP                  0x093a
#define OID_SET_OUI_FILTER			0x1220/**/
#define OID_GET_SET_TOGGLE							0x8000
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE                              0x8BE0
#endif
#define PROBERXVSIE_COMMON_NODE             "root.proberxvsie.common"
#define PROBERESPVSIE_COMMON_NODE             "root.proberespvsie.common"

#define MAC_ADDR_LEN				6

#ifndef GNU_PACKED
#define GNU_PACKED	__attribute__ ((packed))
#endif



enum rrm_event_subid {
    OID_802_11_RRM_EVT_BEACON_REPORT = 0x01,
    OID_802_11_RRM_EVT_NEIGHBOR_REQUEST,
};
enum wnm_event_subid {
	OID_802_11_WNM_EVT_BTM_QUERY = 0x01,
	OID_802_11_WNM_EVT_BTM_RSP,
};
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned char UCHAR;
#define	CHAR			char
#define	INT				int
#define	UINT			unsigned int
#define ULONG			unsigned int
#define USHORT			unsigned short
#define UCHAR			unsigned char
typedef struct GNU_PACKED rrm_event_s {
	UINT8 event_id;
	UINT32 event_len;
	UINT8 event_body[0];
} rrm_event_t, *p_rrm_event_t;

struct GNU_PACKED wnm_event {
	UINT8 event_id;
	UINT8 event_len;
	UINT8 event_body[0];
};

typedef struct GNU_PACKED bcn_rsp_data_s { 
	UINT8   dialog_token;
	UINT32  ifindex;
	UINT8   peer_address[6];
	UINT32  bcn_rsp_len;
	UINT8   bcn_rsp[0];
} bcn_rsp_data_t, *p_bcn_rsp_data_t;

struct GNU_PACKED btm_rsp_data {
	UINT32 ifindex;
	UINT8 peer_mac_addr[6];
	UINT32 btm_rsp_len;
	UINT8 btm_rsp[0];
};

#define REASON_MIC_FAILURE              14
#define REASON_AKMP_NOT_VALID           20
#define REASON_DECLINE			37
struct CONN_FAIL_MSG {
	CHAR Ssid[32];
	UCHAR SsidLen;
	UCHAR StaAddr[6];
	USHORT ReasonCode;
};

#define device_printf(msg...) printf("===device:=== "msg)
struct device_context {
    int event_sock;
    char ifname[IFNAMSIZ + 1];
} device_context = {
        .ifname = "wifi_device",  
    };

static void device_event_hook(struct device_context *ddctx,
             struct nlmsghdr *h, size_t len);

static void device_user_event_wireless(struct device_context *ddctx,
             char *data, int len);
 
struct vendor_ie{
	UCHAR element_id;
	UCHAR len;
	UCHAR custom_ie[255];
};

struct probe_req_report{
	UCHAR band;/*0:2.4G, 1:5G*/
	UCHAR sta_mac[MAC_ADDR_LEN];
	struct vendor_ie vendor_ie;
};



#endif 