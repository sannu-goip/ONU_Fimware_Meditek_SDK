/******************************************************************************/
/*
 * Copyright (C) 1994-2008 TrendChip Technologies, Corp.
 * All Rights Reserved.
 *
 * TrendChip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of TrendChip Technologies, Corp. and
 * shall not be reproduced, copied, disclosed, or used in whole or
 * in part for any reason without the prior express written permission of
 * TrendChip Technologies, Corp.
 */
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <ares.h>
#include <sys/select.h>
#include "ecnt_utility.h"
#if defined(TCSUPPORT_CHARSET_CHANGE)
#include "iconv.h"
#endif


void dns_callback(void* arg, int status, int timeout, struct hostent* hptr)
{
	char **pptr = NULL;
	int i = 0;
	IPList *ips = (IPList*)arg;
	
	if( ips == NULL )
		return;
	
	if( status == ARES_SUCCESS)
	{
		strncpy(ips->host, hptr->h_name, sizeof(ips->host)-1);
		pptr = hptr->h_addr_list;
		for(i = 0; *pptr!=NULL && i< MAX_IPADDR_COUNT; pptr++, ++i){
			inet_ntop(hptr->h_addrtype, *pptr, ips->ip[ips->count], IP_LEN);
			ips->family[ips->count] = hptr->h_addrtype;
			ips->count++;
		}
	}
	else
	{
		ips->getaddr_fail = 1;		
	}

	if(ips->count)
		ips->getaddr_fail = 0;		
}


int ecnt_dns_resolve(char *host, int family, IPList *resolve_ip, int timeout )
{
	ares_channel channel;
	int fd;
	fd_set read, write;
	struct timeval tv, *tvp, maxtime;
	int res = 0;
		
	if( ARES_SUCCESS != (res = ares_init(&channel)) )
		return -1;
	
	ares_set_servers_csv(channel, "127.0.0.1");
	ares_gethostbyname(channel, host, family, dns_callback,(void*)(resolve_ip));
	
	FD_ZERO(&read);	
	FD_ZERO(&write);	
	fd = ares_fds(channel, &read, &write);
	if( fd == 0 ) 
		return -1;
	
	memset(&maxtime, 0, sizeof(maxtime));
	maxtime.tv_sec = timeout;
	tvp = ares_timeout(channel, &maxtime, &tv);
	if(-1 == select(fd, &read, &write, NULL, tvp))
		return -1;
	ares_process(channel, &read, &write);

	ares_destroy(channel);
	
	if( resolve_ip->getaddr_fail )
	{
		return -1;
	}
	
	return 0;
}

int ecnt_dns_resolve_adv(char *resolver, char *host, char *ip, int size,int family, int timeout )
{
	ares_channel channel;
	int fd;
	fd_set read, write;
	struct timeval tv, *tvp, maxtime;
	int res = 0;
	IPList resolve_ip;
	
	memset(&resolve_ip,0,sizeof(resolve_ip));
	
	if( ARES_SUCCESS != (res = ares_init(&channel)) )
		return -1;
	
	ares_set_servers_csv(channel, resolver);
	ares_gethostbyname(channel, host, family, dns_callback,(void*)(&resolve_ip));
	
	FD_ZERO(&read);	
	FD_ZERO(&write);	
	fd = ares_fds(channel, &read, &write);
	if( fd == 0 ) 
		return -1;
	
	memset(&maxtime, 0, sizeof(maxtime));
	maxtime.tv_sec = timeout;
	tvp = ares_timeout(channel, &maxtime, &tv);
	if(-1 == select(fd, &read, &write, NULL, tvp))
		return -1;
	ares_process(channel, &read, &write);

	ares_destroy(channel);
	
	if(resolve_ip.getaddr_fail)
	{
		return -1;
	}
	
	strncpy(ip,(const char*)(resolve_ip.ip),size);

	return 0;
}

#if defined(TCSUPPORT_CHARSET_CHANGE)
int charsetconv
(char *srcchr, char *deschr, int *inlen, int *outlen, char *src, char *des)
{
	int flag = 0;

	if ( NULL == srcchr || NULL == deschr || NULL == inlen
		|| NULL == outlen || NULL == src || NULL == des )
		return -1;

	iconv_t  cd = (iconv_t)iconv_open_new(deschr, srcchr);
	if ( cd == (iconv_t)(-1) ) 
	{
		flag = -2;
		goto charsetchangeexit;
	}

	if( ( iconv_new(cd, &src, inlen, &des, outlen) == (size_t)-1 )
		|| (*inlen !=0 ) )
	{
		flag = -3;
		goto charsetchangefail;
	}
charsetchangefail:
	iconv_close_new(cd);
charsetchangeexit:
	return flag;
}
#endif

