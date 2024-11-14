
/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svchost_internel.h"

//config format =  ibso.so : init : aaaa
static int svchost_parse_check_line(char* buf,int len)
{
	char *tmp;
	
	tmp = strchr(buf,':');	

	if (!tmp || tmp > buf + len || tmp == buf)
		return -1;
	
	return 0;
}


static void svchost_parse_get_line(char* buf, int* start,int* end)
{
	char* tmp = buf;
	int t = 0;
		
	while((*tmp) && ((*tmp) == 0x20)) { tmp++; t++; } 
	*start = t;
	while((*tmp) && ((*tmp) != '\n')) { tmp++; t++; } 
	*end = t;
	if (buf[*start] == '#')  *start = t;
}

static int svchost_parse_check_conf(char* buf,int len)
{
	int start = 0,end = 0;
	char* tmp = buf;
	
	while(tmp < buf+len)
	{
		svchost_parse_get_line(tmp,&start,&end);
		 if (start != end)
		 { 
		 	if (svchost_parse_check_line(tmp,end) < 0)
				return -1;
		 }
		 tmp += (end + 1);
	}
	return 0;
}

static int svchost_parse_check_blank(char* str)
{
	char* tmp = str;

	if (tmp == NULL)
		return 0;
	
	while((*tmp) && (*tmp) == 0x20) tmp++;

	return *tmp == 0;
}

static char* svchost_parse_skip_blank(char* str)
{
	char* tmp = str;

	if (tmp == NULL)
		return NULL;
	
	while((*tmp) && (*tmp) == 0x20) tmp++;

	return tmp;
}

static struct host_service* svchost_parse_alloc_svc(char* buf, int end)
{
	char *path = buf;
	char *tmp , *entry, *param = NULL;
	struct host_service* ptr;

	buf[end] = 0;
	
	if ((tmp=strchr(path,':')) == NULL)
		return NULL;

	*tmp = 0;
	entry = tmp + 1;
	
	if ((tmp=strchr(entry,':')) != NULL)
	{
		param = tmp + 1;
		*tmp = 0;
	}
	
	if (svchost_parse_check_blank(path) || svchost_parse_check_blank(entry) || svchost_parse_check_blank(param))
		return NULL;

	ptr = (struct host_service*) malloc(sizeof(struct host_service));

	if (!ptr)
		return NULL;

	memset(ptr,0, sizeof(struct host_service));
	
	ptr->path = svchost_parse_skip_blank(path);
	ptr->entry = svchost_parse_skip_blank(entry);
	ptr->param = svchost_parse_skip_blank(param);
	
	return ptr;
}



static void svchost_parse_free_svc(struct host_service* ptr)
{
	struct host_service* tmp = ptr;

	while(tmp)
	{
		ptr = tmp->next;
		free(tmp);
		tmp = ptr;
	}
	return;
}

	
static struct host_service* svchost_parse_conf(char* buf, int len)
{
	int start = 0,end = 0;
	char* tmp = buf;
	struct host_service* head = NULL, *tail = NULL, *cur = NULL;

	while(tmp < buf+len)
	{
		svchost_parse_get_line(tmp,&start,&end);
		 if (start != end)
		 { 
		 	if ((cur = svchost_parse_alloc_svc(tmp,end)) == NULL)
				goto error;
			if (!head)
				head = cur;
			if (tail)
				tail->next = cur;
			tail = cur;
		 }
		 tmp += (end + 1);
	}
	return head;
	
error:
	svchost_parse_free_svc(head);
	return NULL;
}


static int svchost_parse_get_file_len(char* path)
{
	FILE* fp;
	int len = 0;
	
	fp = fopen(path,"r");

	if (fp == NULL)
		return -1;

	if (fseek(fp,0,SEEK_END)){
		fclose(fp);
		return -1;
	}
	
	len = ftell(fp);

	fclose(fp);
	
	return len;
}

static int svchost_parse_read_file(char* path, char* buf,int len)
{
	FILE* fp = NULL;
	int res = 0;
	
	fp = fopen(path,"r");

	if (fp == NULL)
		return -1;

	if (!(res = fread(buf,len,1,fp))){
		fclose(fp);
		return -1;
	}
	fclose(fp);
	
	return 0;
}

static char* svchost_parse_read_conf(char* path, int* plen)
{
	int len;
	char* buf;
	
	if ((len = svchost_parse_get_file_len(path)) <= 0)
		return NULL;

	if ((buf = malloc(len + 1)) == NULL)
		return NULL;

	buf[len] = 0;
	
	if (svchost_parse_read_file(path,buf,len) < 0)
	{
		free(buf);
;		return NULL;
	}
	
	*plen = len + 1;
	
	return buf;
}

static void svchost_parse_print_conf(struct host_service* svc)
{
	struct host_service* ptr = svc;
	while(ptr)
	{
		if (ptr->path)
			printf("svchost_parse_print_conf: Path = %s \n",ptr->path);

		if (ptr->entry)
			printf("svchost_parse_print_conf: Entry = %s \n",ptr->entry);
	
		if (ptr->param)
			printf("svchost_parse_print_conf: Param = %s \n",ptr->param);

		ptr = ptr->next;
	}
	
}

struct host_service*  svchost_parse(char* path, char** pbuf)
{
	char* buf;
	struct host_service* svc;
	int len;
	
	if ((buf = svchost_parse_read_conf(path, &len)) == NULL)
		return NULL;

	if (svchost_parse_check_conf(buf,len) < 0)
		goto error;
	
	 if ((svc = svchost_parse_conf(buf,len)) == NULL)
	 	goto error;
	 
	 svchost_parse_print_conf(svc);


	*pbuf = buf;
	return svc;
	
error:
	free(buf);
	return NULL;

}
