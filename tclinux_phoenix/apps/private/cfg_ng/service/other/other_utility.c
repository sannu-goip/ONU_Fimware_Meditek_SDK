

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <regex.h>
#include "svchost_api.h"
#include "cfg_api.h"
#include "other_mgr.h"


static char *svc_other_readline(int fd)
{
	char *str_enter=NULL;
	char stream[128]={0};
	char *str_return=NULL;
	int len,sub_len,offset=0;
	int ret;
	off_t pos;
	struct stat st;
	int res = 0;

	ret = fstat(fd,&st);
	if(ret == -1)
		return NULL;

	pos = lseek(fd,0,SEEK_CUR);
	if(pos == -1L)
			return NULL;
	memset(stream,0,128);
	while((len=read(fd,stream,16))>0)
	{
		stream[len] = '\0';
		str_enter = strchr(stream,'\n');
		if(str_enter ==NULL)
		{
			offset +=len;
			continue;
		}
		else
		{
			sub_len = str_enter-stream;
			offset +=sub_len;
			break;
		}
				
	}
	if(!offset)
		return NULL;
		
	res = lseek(fd,pos,SEEK_SET);
	if(res){
		/*do nothing,just for compile*/
	}
	str_return = (char *)malloc(offset+1);
	if(str_return == NULL)
		return NULL;
	memset(str_return,0,offset+1);
	ret = read(fd,str_return,offset);
	
	if(ret<0)
	{
		free(str_return);
		return NULL;
	}
	else
		res = lseek(fd,pos+offset+1,SEEK_SET);

	return str_return;
	
}


int svc_other_check_route(char* rule)
{
	int fd = -1;
	char *line = NULL;

	fd = open("/etc/route.sh",O_RDONLY);
	if(fd < 0)
		return -1;
	while((line = svc_other_readline(fd)))
	{
		if(!strcmp(line,rule))
		{
			free(line);
			close(fd);
			return 0;
		}
		free(line);
	}
	
	close(fd);

	return 0;
}


static int svc_other_check_ip_format(char *ip_arg){
	int z=0;
	char ip[16]={0};
	char * pattern;
	regex_t reg;
	regmatch_t pm[10];
	const size_t nmatch = 10;

	strncpy(ip, ip_arg, sizeof(ip)-1);
	/*fprintf(stderr,"ip=%s\n", ip);*/

	/*set regular expression of ip format*/
	pattern ="([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})";
	/*compile this regular expression*/
	z = regcomp(&reg, pattern, REG_EXTENDED);

	/*if compile fails, free its memory and return false*/
	if (z != 0){
		fprintf(stderr, "Failed to open regcomp.\n");
		regfree(&reg);
		return 0;
	}

	/*compare input string base on previous syntax*/
	z = regexec(&reg, ip, nmatch, pm, 0);
	/*if format is invalid, free its memory and return false*/
	if (z == REG_NOMATCH) {
		fprintf(stderr, "IP format is wrong.\n");
		regfree(&reg);
		return 0;
	}

	/*free memory anyway*/
	regfree(&reg);

	return 1;
}/*end check_ip_format*/


int svc_other_check_mask_format(char *mask_arg, char *mask_decimal)
{
	int i;
	int mask_counter = 0;
	char mask[16]={0};
	char *p[4];
	const char *delim = ".";
	unsigned int sample = 0x0;
	unsigned int binary_mask = 0x0;
	const unsigned int bit_1_mask = 0x1;

	strncpy(mask, mask_arg, sizeof(mask)-1);
	/*check is this mask a valide ip format*/
	if(!svc_other_check_ip_format(mask)){
		return 0;
	}

	/*divied mask into four token*/
	for(i=0; i<4; i++){
		if(i==0){
			p[i] = strtok(mask, delim);
		}else{
			p[i] = strtok(NULL, delim);
		}
	}

	/*transfer mask to bit type ex. 255 -> 11111111*/
	for(i=0; i<4; i++){
		binary_mask |= atoi(p[i]);

		if(i!=3){
			binary_mask <<= 8;
		}
	}

	/*check is this mask a valid mask ex. 11111011000 is invalid*/
	for(i=0; i<32; i++){
		sample = binary_mask & bit_1_mask;
		if(sample == 0x0){
			if(mask_counter != 0){
				return 0;
			}
		}else{
			mask_counter++;
		}
		binary_mask >>= 1;
	}

	snprintf(mask_decimal, 4, "%d", mask_counter);
	return 1;
}/*end check_mask_format*/



