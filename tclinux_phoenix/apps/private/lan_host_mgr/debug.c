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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"

DebugLevel lanhost_dbglevel = DBG_MSG_ERR;

int get_lanhost_dbglevel(){
	FILE* fp = NULL;
	char line_buf[64] = {0};
	
	fp = fopen("/tmp/lanhost_dbg","r");
	if(fp == NULL){
		return lanhost_dbglevel;
	}
	
	if(fgets(line_buf, 64, fp) == NULL){
		fclose(fp);
		return lanhost_dbglevel;
	}
	fclose(fp); 	

	lanhost_dbglevel = atoi(line_buf);
	
	return lanhost_dbglevel;
}

void tc3162_console_dbg(char * ptr)
{
	FILE *proc_file;
    proc_file = fopen("/proc/tc3162/dbg_msg", "w");
	if (!proc_file) {
		printf("open /proc/tc3162/dbg_msg fail\n");
		return;
	}
	fprintf(proc_file, "%s", (char *)ptr);
	fclose(proc_file);	
}

#define DGBONCECNT 64

void lanhost_printf(DebugLevel dbg, const char *fmt, ...)
{
	int len = 0;
	int index = 0;
	static char msg[MAX_BUF];
	char *ptr = NULL;
	char tmp = 0;
	int rc = 0;
	va_list args;
	
	if (dbg > get_lanhost_dbglevel()) 
		return;
	
	va_start(args, fmt);
	rc = vsnprintf(msg, MAX_BUF, fmt, args);
	va_end(args);

	len = strlen(msg);
	ptr = msg;
	index = 0;
	
	if (strstr(msg, "%s") != NULL)
		return;
	
	while (len > 0) {
		if (len <= DGBONCECNT) {
			tc3162_console_dbg((char*)(ptr+index));
			len = 0;
		}
		else {
			tmp = ptr[index + DGBONCECNT];
			ptr[index + DGBONCECNT] = 0;
			tc3162_console_dbg((char*)(ptr+index));
			ptr[index + DGBONCECNT] = tmp;
			len -= DGBONCECNT;
			index += DGBONCECNT;
		}
	}
}

