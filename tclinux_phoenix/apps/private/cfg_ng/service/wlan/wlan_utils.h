/*************************
	author:  jw.ren
	date:    20191212
*************************/
#ifndef __SVC_WLAN_UTILS_H__
#define __SVC_WLAN_UTILS_H__

#include <inttypes.h>

#ifndef TRUE
#define TRUE        (1)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif

/*
	addr format
*/
#define ADDR_FMT_STR \
	"%02"PRIx8":%02"PRIx8":%02"PRIx8":%02"PRIx8":%02"PRIx8":%02"PRIx8

#define ADDR_FMT_ARS(ea) \
	(ea)[0], (ea)[1], (ea)[2], (ea)[3], (ea)[4], (ea)[5]
	
#ifndef ETH_ALEN
#define ETH_ALEN       (6)
#endif

#ifndef IFNAMSIZ
#define IFNAMSIZ       (16)
#endif

enum wifi_itf_type
{
	RA_ITF_TYPE 	= 1,
	RAI_ITF_TYPE	= 2,
	APCLI_ITF_TYPE	= 3,
	APCLII_ITF_TYPE = 4,
};

typedef struct itf_type_idx_info
{
	int itf_type;
	int itf_idx;
	
}itf_type_idx_info_t;

int itf_name_convert_type_idx(char *itf_name, struct itf_type_idx_info *ptr);
void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);
int test_itf_status(const char *_iface, short if_flag);

#endif
