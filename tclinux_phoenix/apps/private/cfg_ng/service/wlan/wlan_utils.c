/*************************
	author:  jw.ren
	date:    20191212
*************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <lan_port/lan_port_info.h>
#include "wlan_utils.h"

int itf_name_convert_type_idx(char *itf_name, struct itf_type_idx_info *ptr)
{
	char *p = NULL;
	if(NULL == itf_name)
	{
		return -1;
	}
	
	if((p = strstr(itf_name, WLAN_AC_ITF_NAME_FORMAT)) && (p != NULL))
	{
		ptr->itf_type = RAI_ITF_TYPE;
		p += strlen(WLAN_AC_ITF_NAME_FORMAT);
	}
	else if((p = strstr(itf_name, WLAN_ITF_NAME_FORMAT)) && (p != NULL))
	{
		ptr->itf_type = RA_ITF_TYPE;
		p += strlen(WLAN_ITF_NAME_FORMAT);
	}
	else if((p = strstr(itf_name, WLAN_AC_APCLI_ITF_NAME_FORMAT)) && (p != NULL))
	{
		ptr->itf_type = APCLII_ITF_TYPE;
		p += strlen(WLAN_AC_APCLI_ITF_NAME_FORMAT);
	}
	else if((p = strstr(itf_name, WLAN_APCLI_ITF_NAME_FORMAT)) && (p != NULL))
	{
		ptr->itf_type = APCLI_ITF_TYPE;
		p += strlen(WLAN_APCLI_ITF_NAME_FORMAT);
	}
	else
	{
		return -1;
	}
	ptr->itf_idx = atoi(p);

	return 0;
}

void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
	unsigned char *pt;
	int x;

	pt = pSrcBufVA;
	tcdbg_printf("%s: %p, len = %d\n",str,  pSrcBufVA, SrcBufLen);

	for (x=0; x<SrcBufLen; x++) {
		if (x % 16 == 0)
			tcdbg_printf("0x%04x : ", x);
		tcdbg_printf("%02x ", ((unsigned char)pt[x]));
		if (x%16 == 15) printf("\n");
	}
	tcdbg_printf("\n");
}


int test_itf_status(const char *_iface, short if_flag) 
{
	struct ifreq ifr;
	int sockfd;
	int ret = 0;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
	{
		return FALSE;
	}
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name , _iface , IFNAMSIZ-1);

	ret = ioctl(sockfd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) 
	{
		close(sockfd);
		return FALSE;
	}
	close(sockfd);
	if (ifr.ifr_flags & if_flag) 
	{
		return TRUE;
	}
	
	return FALSE;
}
