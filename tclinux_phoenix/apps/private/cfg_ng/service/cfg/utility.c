

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
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/version.h>
#include <svchost_api.h> 
#include <dirent.h>
#include "cfg_api.h"
#include <regex.h>
#include "cfg_cli.h"
#include "utility.h"
#include "cfg_msg.h"
#include <arpa/inet.h>
#include <netdb.h>
#if defined(TCSUPPORT_ECN_MEGACO)
#include <netinet/in.h>
#include <sys/un.h>
#endif
#include <linux/wireless.h>
#if defined(TCSUPPORT_ECN_MEGACO)
#define SOCKSIZE                (sizeof(struct sockaddr))
#define HTTP_SOCKET_ERROR	    -1
#endif

#if defined(TCSUPPORT_CT_JOYME2)
#include <modules/vpn_info/info_ioctl.h>
#endif
#ifdef TCSUPPORT_WLAN
#include <libapi_lib_wifimgr.h>
#endif
#include "blapi_traffic.h"
#include <syslog.h>

#define USE_DEFAULT_IFNAME_FORMAT 1
#define FLUSH_DEFAULT_CHAIN "/usr/script/filter_forward_stop.sh"
#define LOAD_DEFAULT_CHAIN "/usr/script/filter_forward_start.sh"



/*return 0:buf not all digit
		1: buf is all digit*/
int isNumber(char *buf)
{
	if(*buf == '\0')
		return 0;
	else {
		while(*buf != '\0') {
			if(*buf<= '9' && *buf++ >= '0')
				continue;
			else
				return 0;
			}
	}
	return 1;
}

char* itoa(register int i)
{
	static char a[7]; /* Max 7 ints */
	register char *b = a + sizeof(a) - 1;
	int   sign = (i < 0);

	if (sign)
		i = -i;
	*b = 0;
	do
	{
		*--b = '0' + (i % 10);
		i /= 10;
	}
	while (i);
	if (sign)
		*--b = '-';
	return b;
}

char *string_FirstCharToUp(char *string)
{
	string_tolower(string);
		string[0]=string[0] - 32;	
	
	return string;
}

char *string_tolower(char *string){
	int i =0;

	while(string[i]!='\0'){
		if (string[i]>=65 && string[i]<=90){
			string[i]=string[i]+32;
		}
		i++;
	}
	return string;
}/*end string_tolower*/

char *string_toUper(char *string)
{
	int i =0;

	while(string[i]!='\0'){
		if (string[i]>=97 && string[i]<=122){
			string[i]=string[i]-32;
		}
		i++;
	}
	return string;
}/*end string_tolower*/

char *escape_special_character(char *inp, char *outp, int len)
{
	char *index = NULL, *ptrbegin=NULL, *ptrend = NULL;
	unsigned char c;

	if ( NULL == outp || NULL == inp){
		return inp;
	}
	
	/*check whether need to escape*/
	ptrbegin = strchr(inp,'`');
	if (ptrbegin){
		ptrend =strrchr(ptrbegin,'`');
	}
	else{	
		return inp;
	}
	
	if (NULL == ptrend || ((ptrend-ptrbegin)<=1)){
		return inp;
	}
	else {/*Do escape*/
		index = outp;
		while (( c = *inp++)){
			if (needs_escape(c)){
				*index++ = '\\';
				*index++ = c;
			}
			else{
				*index++ = c;
			}
			if ((index-outp) >= len){
				printf("escape_special_character: buffer needs increase!!\n");
				return inp;
			}
		}
		*index='\0';
		
		return outp;
	}
		
}

/*
it will check command whether contain injection code.
general blacklist command injection code: ; & $ > < ' \ ! >> #
currently, > >> " will be white list.
*/
int check_injection_code(char ch_at)
{
	char s_inject_code[] = {';', '&', '|', '$', '<', '\'', '\\', '!', '#', 0};
	int code_len = sizeof(s_inject_code) - 1, idx = 0;

	for ( idx = 0; idx < code_len; idx ++ )
	{
		if ( ch_at == s_inject_code[idx] )
		{
			return -1;
		}
	}

	return 0;
}

/*
type:
0 -> system_escape
1 -> fputs_escape
*/
int check_injection_code_command(char *cmd, int cmd_len, int type)
{
	int idx = 0;
	char ch_at = 0;

	if ( NULL == cmd || cmd_len <= 0
		|| 0 == cmd[0] )
		return 0;

	for ( idx = 0; idx < cmd_len; idx ++ )
	{
		ch_at = cmd[idx];
		if ( -1 == check_injection_code(ch_at) )
		{
			/* symbol "&" can be at end. */
			if ( '&' == ch_at 
				&& cmd_len >= 2
				&& ( (cmd_len -1) == idx || (cmd_len -2) == idx ) )
			{
				continue;
			}

			/* symbol "#" can be at first. */
			if ( '#' == ch_at 
				&& cmd_len >= 1
				&& 0 == idx )
			{
				continue;
			}
	
			goto chk_err;
		}
	}

	return 0;

chk_err:
	/*
	display command when it contain injection code. 
	*/
	tcdbg_printf("\nWARN: injection command\n"
				"char [%c] type[%s] cmd:[%s].\n"
				, cmd[idx], ( 0 == type ? "cmd" : "shell"), cmd);

	return -1;
}

void fputs_escape(char *cmd, FILE *fp)
{
	char *pcmd = NULL;
	char cmd_buf[1025] = {0};

	if ( NULL == cmd || NULL == fp) {
		return ;
	}

	if ( -1 == check_injection_code_command(cmd, strlen(cmd), 1) )
		return -1;

	pcmd = escape_special_character(cmd, cmd_buf, sizeof(cmd_buf));
	
	fputs(pcmd,fp); 

	return;
}

/*
only for config file, won't execute shell.
*/
void fputs_escape_normal_file(char *cmd, FILE *fp)
{
	char *pcmd = NULL;
	char cmd_buf[1025] = {0};

	if ( NULL == cmd || NULL == fp)
	{
		return ;
	}

	pcmd = escape_special_character(cmd, cmd_buf, sizeof(cmd_buf));

	fputs(pcmd,fp); 

	return;
}

/*
it will convert 
general blacklist command injection code: \ ` " $
*/
char *escape_param(char *chk_val, char *out_val, int out_val_len)
{
	char ch_at = 0;
	int remain_len = out_val_len - 1, idx = 0, chk_vlan_len = 0, curr_out_idx = 0;

	if ( NULL == chk_val || NULL == out_val || remain_len < 2 )
		return "";

	chk_vlan_len = strlen(chk_val);
	if ( chk_vlan_len <= 0 )
		return "";

	for ( idx = 0; idx < chk_vlan_len; idx ++ )
	{
		if ( remain_len >= 2 )
		{
			if ( NEED_C(chk_val[idx]) )
			{
				out_val[curr_out_idx ++] = '\\';
				out_val[curr_out_idx ++] = chk_val[idx];
				remain_len -= 2;
			}
			else
			{
				out_val[curr_out_idx ++] = chk_val[idx];
				remain_len --;
			}
		}
		else
		{
			tcdbg_printf("%s() out buffer overflow.\n", __FUNCTION__);
			break;
		}
	}
	
	return out_val;
}

int system_escape(char *cmd)
{
	char *pcmd = NULL;
	char cmd_buf[1025] = {0};

	if(NULL == cmd){
		return -1;
	}

	if ( -1 == check_injection_code_command(cmd, strlen(cmd), 0) )
		return -1;

	pcmd = escape_special_character(cmd, cmd_buf, sizeof(cmd_buf));

	return (system(pcmd));		
}

#if defined(TCSUPPORT_ECN_MEGACO)
int voipCmdSend(char * SendData) 
{
	struct sockaddr_un serverAddr;
	int fClientSocket = HTTP_SOCKET_ERROR;
	int status  = HTTP_SOCKET_ERROR;

	fClientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fClientSocket == HTTP_SOCKET_ERROR){
		printf("StcpOpenActive: socket error!!!\n");
		return -1;
	}

	serverAddr.sun_family = AF_UNIX;
	strncpy(serverAddr.sun_path, "/tmp/mg_sock0", sizeof(serverAddr.sun_path)-1);

	status = connect(fClientSocket, (struct sockaddr*)&serverAddr, SOCKSIZE);
	if(status == HTTP_SOCKET_ERROR){		
		printf("StcpOpenActive: connect error!!!\n");
		close(fClientSocket);	
		return -1;
	}

	if(send(fClientSocket, SendData, strlen(SendData), 0) < 0)
	{
		perror("send");
		close(fClientSocket);
		return -1;
	}

	close(fClientSocket);
	return 0;
}/* StcpOpenActive */
#endif

int get_entry_number_cfg2(char *buffer, char *keyword, int *number)
{
	char *tmp = NULL;
	char *tmp2 = NULL;
	
	if((buffer == 0)||(keyword == 0)||(number == 0))
	{
		return -1;
	}

	if((tmp2 = strstr(buffer,keyword)))
	{
		tmp=tmp2+strlen(keyword);
		(*number)=atoi(tmp);
		return 0;
	}
	else
	{
		return -1;
	}
}

void fileRead(char *path, char *buf, int size){
	int  fd=0, ret=0;

	memset(buf,0, size);
	fd = open(path,O_RDONLY);
	if(fd == -1){
		return;
	}

	ret = read(fd, buf, size - 1);
	if(ret <= 0){
		close(fd);
		buf[0] = '\0';
		return;
	}
	buf[size - 1] = '\0';
	close(fd);
}


void kill_process(char *pid_path)
{
	char pid[8]={0};

	fileRead(pid_path, pid, sizeof(pid));
	if(strlen(pid) > 0){
		kill(atoi(pid), SIGKILL);
		unlink(pid_path);
	}
}

void decideModulePath(char* vername, int len)
{
	FILE * Ver = NULL;
	char *buf = NULL; 
	char *start = NULL; 
	char *end = NULL;
	int ret = 0;
	size_t size = 0;

	Ver = fopen ("/proc/version","r");
	if(!Ver){
		printf("Failed to open /proc/version\r\n");
		return;
	}

	if((ret = getline(&buf, &size, Ver)) != -1) {	
	start = strchr(buf, ' ');
	start = strchr(start + 1, ' ') + 1;
	end = strchr(start + 1, ' ');
	if(end)
		*end = '\0';
	strncpy(vername, start, len - 1);
	}
	fclose(Ver);
	
	if (buf)
		free(buf);

	return;
}

int sendEventMessage(int state, char *mac,int port)
{
	nlk_msg_t nlkmsg;
	int ret_code = 0;

	if (NULL == mac)
		return -1;

	memset(&nlkmsg, 0, sizeof(nlkmsg));

	nlkmsg.nlmhdr.nlmsg_len = NLK_LOCAL_PAYLOAD;
	nlkmsg.nlmhdr.nlmsg_pid = -1;	/* self pid or -1 */
	nlkmsg.nlmhdr.nlmsg_flags = 0;

	nlkmsg.eventType = ( 0 == state ) ? MSG_DEVMAC_OFFLINE :
		(( 1 == state ) ? MSG_DEVMAC_ONLINE : 
			(( 2 == state) ? MSG_DEVMAC_WLAN_ONLINE : 
				((3 == state) ? MSG_DEVMAC_WLAN_OFFLINE :
					((4 == state) ? MSG_DEVMAC_REFUSED : MSG_DEVMAC_WLAN_REFUSED))));
	snprintf(nlkmsg.data.payload, sizeof(nlkmsg.data.payload), "%s %d", mac,port);

	ret_code = nlkmsg_send(NLKMSG_GRP_DEVINFO
		, NLK_LOCAL_PAYLOAD, &nlkmsg);

	if ( ret_code< 0 )
		printf("\n nlkmsg_send fail for NLKMSG_GRP_DEVINFO"
			"retcode =[%d] \n", ret_code);

	return 0;
}

void specialCharacterHandle(char *oriStr)
{
    int i;
    int j = 0;
    char handle[160] = {0};
    for(i = 0;i < strlen(oriStr);i++){
    	if(oriStr[i] == '"'){
    		handle[j++]='\\';
    		handle[j++]='"';
    	}else if(oriStr[i] == '\\'){
    		handle[j++]='\\';
    		handle[j++]='\\';
    	}else if(oriStr[i] == '\$'){
    		handle[j++]='\\';
    		handle[j++]='\$';
    	}else if(oriStr[i] == '\`'){
    		handle[j++]='\\';
    		handle[j++]='\`';
    	}
    	else{
    		handle[j] = oriStr[i];
    		j++;
    	}
    }
    handle[j] = '\0';
    strcpy(oriStr,handle);

	return;
}

int cutPath(char* path, char cutStr[][32])
{
	int i = 0;
	char * ptr = NULL;
	ptr = strtok(path, ".");
	if(ptr == NULL)
	{
		return -1;
	}
	while(ptr)
	{
		strcpy(cutStr[i], ptr);
		i++;
		ptr = strtok(NULL, ".");
	}

	return 0;
}

int get_profile_str(char *keyname,char *str_return, int size, int type, char *path)
{
	FILE *fp;
	char *str_key= NULL;
	char stream[MAXGET_PROFILE_SIZE]={0};
	int enterOffSet=1;
	int qmarkLength=0;
	int skipQmark=0;
	int totalLength=0;

	fp=fopen(path,"r");
	if(fp==NULL){
		fprintf(stderr,"Can't open %s\n",path);
		return FAIL;
	}

	memset(str_return, 0, size);
	fseek(fp, 0, SEEK_SET);
	if(type==QMARKS){
		qmarkLength=2;
		skipQmark=1;
	}
	else if(type == NO_QMARKS){
		qmarkLength=0;
		skipQmark=0;
	}
	else{
		fprintf(stderr, "The input qmark type of get_profile_str is wrong \n");
		fclose(fp);
		return FAIL;
	}

	while(fgets(stream, MAXGET_PROFILE_SIZE, fp) != NULL){
		if(strrchr(stream,'\n') == NULL){
			enterOffSet=0;
		}
		else{
			enterOffSet=1;
		}

		str_key = strstr(stream,keyname);
		if(str_key == NULL || str_key != stream){
			memset(stream, 0, sizeof(stream));
			continue;
		}

		totalLength=strlen(stream)-strlen(keyname)-enterOffSet-qmarkLength;
		if(size<totalLength+1){/*total length + '\0' should not less than buffer*/
			fprintf(stderr, "Too small buffer to catch the %s frome get_profile_str\n", keyname);
			fclose(fp);
			return FAIL;
		}
		else if(totalLength<0) {/*can't get a negative length string*/
			fprintf(stderr, "No profile string can get\n");
			fclose(fp);
			return FAIL;
		}
		else{
			strncpy(str_return, stream+strlen(keyname)+skipQmark, totalLength);
			str_return[totalLength]= '\0';
			fclose(fp);
			return strlen(str_return);
		}
	}
	
	fclose(fp);
	fprintf(stderr,"File %s content %s is worng\n",path,keyname);
	return FAIL;
}/* end get_profile_str */

int write2file(char *buf, char *pathname)
{
	FILE *fp=NULL;
	fp = fopen(pathname, "w");

	if(fp == NULL){
		return FAIL;
	}

	F_PUTS_NORMAL(buf,fp);
	fclose(fp);
	return SUCCESS;
}

int append2file(char *path, char *buff)
{
	FILE *fp = NULL;

	if(NULL == path || NULL == buff)
	{
		return -1;
	}
	
	fp = fopen(path, "w+");

	if (!fp) 
	{
		return -1;
	}

	fwrite(buff, 1, strlen(buff), fp);
	fclose(fp);

	return 0;
}


int get_profile_str_new(char *keyname,char *str_return, int size,char *path)
{
	FILE *fp;
	char *str_key= NULL;
	char stream[MAXGET_PROFILE_SIZE]={0};
	int totalLength=0;
	char *p = NULL;

	fp=fopen(path,"r");
	if(fp==NULL){
		fprintf(stderr,"Can't open %s\n",path);
		return FAIL;
	}

	memset(str_return, 0, size);
	fseek(fp, 0, SEEK_SET);

	while(fgets(stream, MAXGET_PROFILE_SIZE, fp) != NULL)
	{
		str_key = strstr(stream,keyname);
		if(str_key == NULL || str_key != stream)
		{
			memset(stream, 0, sizeof(stream));
			continue;
		}	

		p = strtok(stream,"\r");
		while(p)
		{
			p = strtok(NULL,"\r");
		}

		p = strtok(stream,"\n");
		while(p)
		{
			p = strtok(NULL,"\n");
		}

		totalLength=strlen(stream)-strlen(keyname);
		if(size<totalLength+1){/*total length + '\0' should not less than buffer*/
			fprintf(stderr, "Too small buffer to catch the %s frome get_profile_str_new\n", keyname);
			fclose(fp);
			return FAIL;
		}
		else if(totalLength<0) {/*can't get a negative length string*/
			fprintf(stderr, "No profile string can get\n");
			fclose(fp);
			return FAIL;
		}
		else{
			strncpy(str_return, stream+strlen(keyname), totalLength);
			str_return[totalLength]= '\0';
			fclose(fp);
			return strlen(str_return);
		}
	}
	fclose(fp);
	fprintf(stderr,"File %s content %s is worng\n",path,keyname);
	return FAIL;
}/* end get_profile_str */

int check_and_set_filter(unsigned int new_filter_state)
{
	char nodePath[64] = {0};	
	char filterState[32] = {0};
	char newFilterState[32] = {0};
	int flag = 0;
	
	snprintf(nodePath, sizeof(nodePath),GLOBALSTATE_PRESYSSTATE_NODE);
	cfg_obj_get_object_attr(nodePath, "FilterState", 0, filterState, sizeof(filterState)) ;

	if(cfg_obj_get_object_attr(nodePath, "FilterState", 0, filterState, sizeof(filterState)) < 0)
	{
		flag = 0;
	}else{
		flag = atoi(filterState);
	}
	
	if(flag != 0)
	{/*now filter is on*/
		if(new_filter_state == 0){
			system("iptables -t filter -F");
			system(FLUSH_DEFAULT_CHAIN);
			system("rmmod iptable_filter");
		}
	}
	else
	{/*now filter is down*/
		if(new_filter_state != 0){
			char vername[CFG_BUF_16_LEN] = {0}, module_path[CFG_BUF_128_LEN] = {0};

			decideModulePath(vername, CFG_BUF_16_LEN);
			snprintf(module_path, sizeof(module_path), INSMOD_IPTABLE_FILTER_CMD, vername);
			system(module_path);

			snprintf(module_path, sizeof(module_path), INSMOD_IP6TABLE_FILTER_CMD, vername);
			system(module_path);

			system(LOAD_DEFAULT_CHAIN);
		}
	}
	
	snprintf(newFilterState,sizeof(newFilterState),"%u",(int)new_filter_state);
	cfg_set_object_attr(nodePath, "FilterState", newFilterState) ;
	return 0;
}


int get_wanindex_by_name(char *wan_if_name)
{
	int if_index = -1;
	char pvc[5] = {0};
	char entry[5] = {0};

	if(wan_if_name == NULL || !strlen(wan_if_name))
		goto error;
	
	if(strstr(wan_if_name, "ppp"))/*ppp interface*/
	{  
		if(get_entry_number_cfg2(wan_if_name,"ppp",&if_index) != 0 ){
			goto error;
		}		
	}
	else/*nas interface,ext:nas0_1*/
	{   
		sscanf(wan_if_name, "nas%1s_%1s", pvc, entry);	
		if(strlen(pvc) == 0 || strlen(entry) == 0)
		{
			goto error;
		}
		if_index = atoi(pvc) * MAX_SMUX_NUM + atoi(entry);
		/*tcdbg_printf("get_wanindex_by_name:wan_if_name = %s,pvc=%s,entry=%s,if_index=%d\n",wan_if_name,pvc,entry,if_index);*/
		
	}
	if(if_index < 0 || if_index > MAX_WAN_IF_INDEX)
		goto error;
	
	return if_index;

error:
	
	if(wan_if_name == NULL || !strlen(wan_if_name))
		printf("get_wanindex_by_name:wan_if_name is NULL \n");
	else
	printf("get_wanindex_by_name:can not get index from %s \n", wan_if_name);
	return -1;
	
}

int get_waninfo_by_index(int if_index, char *attribute, char *reval, int size)
{
	char nodeName[64] = {0};
	int pvc_index = 0, entry_index = 0;
	
	if(if_index < 0 || if_index > PVC_NUM * MAX_SMUX_NUM)
		return -1;
	
	if(!attribute || !reval)
		return -1;
	
	pvc_index = if_index / MAX_SMUX_NUM + 1;
	entry_index = if_index % MAX_SMUX_NUM + 1;

	/*tcdbg_printf("get_waninfo_by_index:if_index = %d,pvc_index = %d,entry_index=%d\n",if_index,pvc_index,entry_index);*/
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WAN_PVC_NODE, pvc_index);

	/*find value from Wan_PVCn firstly*/
	if(cfg_obj_get_object_attr(nodeName, attribute, 0, reval, size) < 0)
	{
		/*find value from Wan_PVCn_Entryn if no attribute at Wan_PVCn*/		
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WAN_PVC_ENTRY_NODE, pvc_index, entry_index);
		if(cfg_obj_get_object_attr(nodeName, attribute, 0, reval, size) < 0)
		{
			return -1;
		}
	}
	return 0;
}

int find_pid_by_name( char *ProcName, int *foundpid, int *foundnum)
{
        DIR             *dir = NULL;
        struct dirent   *d = NULL;
        int             pid = 0, i = 0;
        char            *s = NULL;
        int pnlen = 0;

		if ( NULL == ProcName
			|| NULL == foundpid
			|| NULL == foundnum )
			return -1;
	
        foundpid[0] = 0;
        pnlen = strlen(ProcName);
		*foundnum = 0;

        /* Open the /proc directory. */
        dir = opendir("/proc");
        if (!dir)
        {
                printf("cannot open /proc");
                return -1;
        }

        /* Walk through the directory. */
        while ((d = readdir(dir)) != NULL) {
                char exe[PATH_MAX_DNSMASQ+1];
                char path[PATH_MAX_DNSMASQ+1];
                int len;
                int namelen;

                /* See if this is a process */
                if ((pid = atoi(d->d_name)) == 0)
					continue;

                snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
				
                if ((len = readlink(exe, path, PATH_MAX_DNSMASQ)) < 0)
                        continue;
                path[len] = '\0';

                /* Find ProcName */
                s = strrchr(path, '/');
                if(s == NULL)
					continue;
                s++;

                /* we don't need small name len */
                namelen = strlen(s);
                if(namelen < pnlen) 
					continue;

                if(!strncmp(ProcName, s, pnlen)) {
                        /* to avoid subname like search proc tao but proc taolinke matched */
                        if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                                foundpid[i] = pid;
                                i++;
                        }
                }
        }

		*foundnum = i;
        foundpid[i] = 0;
        closedir(dir);
        return  0;
}

void shrink_ipv6addr(char *in_ipv6addr, char *out_ipv6addr, int out_len)
{
	int res = 0;
	struct sockaddr_in6 ipv6addr;
	memset(&ipv6addr,0x0,sizeof(struct sockaddr_in6));
	res = inet_pton(AF_INET6, in_ipv6addr, (struct sockaddr *) &ipv6addr.sin6_addr);
	ipv6addr.sin6_family = AF_INET6;
	getnameinfo((struct sockaddr *) &ipv6addr, sizeof(struct sockaddr_in6), out_ipv6addr, out_len, NULL, 0, NI_NUMERICHOST);
	return;
}


int doValPut(char *path, char *value)
{
	FILE *fp = NULL;

	fp = fopen(path,"w");
	if(fp != NULL){
		fprintf(fp, value);
		fclose(fp);
	}else{
		return -1;
	}
	return 0;
}

int get_file_string(char *file_path, char line_return[][MAX_INFO_SIZE], int line_num)
{
	FILE *fp = NULL;
	int i = 0;
	char line_buf[MAX_INFO_SIZE] = {0};

	if(!file_path || line_num < 0)
		return FAIL;
	
	fp=fopen(file_path,"r");
	if(fp==NULL){		
		return FAIL;
	}
	while(fgets(line_buf, MAX_INFO_SIZE, fp) != NULL && (line_num > 0)){
		if(line_buf[0] != '\n'){
			/*replace "\n" by NULL*/
			strtok(line_buf, "\n");
			strcpy(line_return[i], line_buf);
		}
		else{
			strcpy(line_return[i], "");
		}
		line_num--;
		i++;
		memset(line_buf, 0, sizeof(line_buf));
	}
	fclose(fp);
	/*Get nothing*/
	if(i == 0)
		return FAIL;
	
	return SUCCESS;
}

int getPonLanStatus2(int portStatus[])
{
	int ret = -1;
	char line[MAXSIZE] = {0};

	ret = blapi_traffic_check_eth_port_status(line,portStatus);
	if(ret == -1)
	{
		tcdbg_printf("failed to read eth_port_status\n");
			return -1;
		}
	return 0;
}

int get_all_wan_index(int* buf,int len)
{
	int num = 0;
	int pvc[PVC_NUM];
	int entry[MAX_WAN_ENTRY_NUMBER];
	int i,j,curpvc,curentry,pvcnum,entrynum;
	char pvcname[32];

	pvcnum =  cfg_obj_query_object(WAN_NODE,"pvc",pvc);
	for (i = 0; i< pvcnum; i++)
	{
		curpvc = (pvc[i] & 0xff);
		if (curpvc == 0)
			continue;
		memset(pvcname, 0, sizeof(pvcname));
		snprintf(pvcname, sizeof(pvcname), WAN_PVC_NODE, curpvc);
		entrynum = cfg_obj_query_object(pvcname,"entry",entry);
		if (entrynum <= 0)
			continue;
		for(j=0; j < entrynum; j++)
		{
			curentry =(entry[j] & 0xff);
			if (curentry > 0 && num < len)
				buf[num++] = (curpvc - 1) * MAX_WAN_ENTRY_NUMBER +  curentry - 1;
		}
	}

	return num;
}

void splitName(char *name, char buf[][32], const char* delim)
{
	char *pValue=NULL;
	char tmp[32]={0};
	int i=0;

	strncpy(tmp, name, sizeof(tmp) - 1);
	/*Get parent node name*/
	strtok(tmp,delim);
	strncpy(buf[0], tmp, 31);
	for(i=1; i < 4; i++){
		pValue=strtok(NULL, delim);
		if(pValue){
			strncpy(buf[i], pValue, 31);
		}
		else{
			break;
		}
	}
}

int check_mask_format(char *mask_arg, char *mask_decimal,int len)
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
	if(!check_ip_format(mask))
	{
		return 0;
	}

	/*divied mask into four token*/
	for(i=0; i<4; i++)
	{
		if(i==0)
		{
			p[i] = strtok(mask, delim);
		}
		else
		{
			p[i] = strtok(NULL, delim);
		}
	}

	/*transfer mask to bit type ex. 255 -> 11111111*/
	for(i=0; i<4; i++)
	{
		binary_mask |= atoi(p[i]);

		if(i!=3)
		{
			binary_mask <<= 8;
		}
	}

	/*check is this mask a valid mask ex. 11111011000 is invalid*/
	for(i=0; i<32; i++)
	{
		sample = binary_mask & bit_1_mask;
		if(sample == 0x0)
		{
			if(mask_counter != 0)
			{
				return 0;
			}
		}
		else
		{
			mask_counter++;
		}
		binary_mask >>= 1;
	}

	snprintf(mask_decimal, len, "%d", mask_counter);
	return 1;
}

int check_mac_format(char *mac_arg)
{
	int z;
	char mac[24]={0};
	char * pattern;
	regex_t reg;
	regmatch_t pm[10];
	const size_t nmatch = 10;

	strncpy(mac, mac_arg, sizeof(mac) - 1);
	fprintf(stderr,"mac=%s\n", mac);

	pattern ="([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}";
	z = regcomp(&reg, pattern, REG_EXTENDED);

	if (z != 0){
		printf("Failed to open regcomp.\n");
		regfree(&reg);
		return 0;
	}

	z = regexec(&reg, mac, nmatch, pm, 0);
	if (z == REG_NOMATCH) {
		printf("MAC format is wrong.\n");
		regfree(&reg);
		return 0;
	}

	regfree(&reg);

	return 1;
}

int check_ip_format(char *ip_arg)
{
	int z;
	char ip[16]={0};
	char * pattern;
	regex_t reg;
	regmatch_t pm[10];
	const size_t nmatch = 10;

	strncpy(ip, ip_arg, sizeof(ip)-1);

	/*set regular expression of ip format*/
	pattern ="([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})";
	/*compile this regular expression*/
	z = regcomp(&reg, pattern, REG_EXTENDED);

	/*if compile fails, free its memory and return false*/
	if (z != 0){
		printf("Failed to open regcomp.\n");
		regfree(&reg);
		return 0;
	}

	/*compare input string base on previous syntax*/
	z = regexec(&reg, ip, nmatch, pm, 0);
	/*if format is invalid, free its memory and return false*/
	if (z == REG_NOMATCH) 
	{
		printf("IP format is wrong.\n");
		regfree(&reg);
		return 0;
	}

	/*free memory anyway*/
	regfree(&reg);

	return 1;
}

int check_ip_range(char *ip_begin_arg, char *ip_end_arg)
{
	int i = 0;
	char *delim = ".";
	char ip_begin[16]={0}, ip_end[16]={0};
	char *ip_begin_tok[4], *ip_end_tok[4];

	strncpy(ip_begin, ip_begin_arg, sizeof(ip_begin)-1);
	strncpy(ip_end, ip_end_arg, sizeof(ip_end)-1);

	/*fprintf(stderr,"ip_begin=%s\n", ip_begin);*/
	/*fprintf(stderr,"ip_end=%s\n", ip_end);*/

	/*if input ip is 0.0.0.0, this means any ip, return true*/
	if((!strcmp(ip_begin, "0.0.0.0")) && (!strcmp(ip_end, "0.0.0.0"))){
		return 1;
	}

	/*decompose ip_begin string into four token*/
	for(i=0; i<4; i++){
		if(i==0){
			ip_begin_tok[i] = strtok(ip_begin, delim);
		}else{
			ip_begin_tok[i] = strtok(NULL, delim);
		}
	}

	/*decompose ip_end string into four token*/
	for(i=0; i<4; i++){
		if(i==0){
			ip_end_tok[i] = strtok(ip_end, delim);
		}else{
			ip_end_tok[i] = strtok(NULL, delim);
		}
	}

	/*check if ip_begin and ip_end are in the same subnet*/
	for(i=0; i<3; i++){
		if((atoi(ip_begin_tok[i]))!=(atoi(ip_end_tok[i]))){
			printf("Not in the same subnet.\n");
			return 0;
		}
	}

	/*check if ip_end exceed ip_begin*/
	if(atoi(ip_begin_tok[3])>atoi(ip_end_tok[3])){
		printf("IP range is invalid.\n");
		return 0;
	}

	return 1;
}/*end acl_check_ip_range*/

int get_waninfo_by_name(char *wan_if_name, char *attribute, char *reval)
{
	int wan_if_index = 0;
	if((wan_if_index = get_wanindex_by_name(wan_if_name)) < 0)
		return -1;

	if(get_waninfo_by_index(wan_if_index, attribute, reval, 6) != 0)
	{
		return -1;
	}
	return 0;
}

int unset_action(char *pstr, int itype)
{
	char *p = pstr;
	char strnum[5] = {0};
	char strCurfilter[20] = {0};
	char strEntry[64] = {0};
	char strentry[64] = {0};
	char nodeName[64] = {0};	
	char *token = NULL;
	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, WEBCURSET_ENTRY_NODE, sizeof(nodeName)-1);
	
	if(DEL_URL == itype)
	{
		strncpy(strCurfilter, "url_filter_id", sizeof(strCurfilter)-1);
		strncpy(strEntry, URLFILTER_ENTRY_NODE, sizeof(strEntry)-1);
	}
	else if(DEL_IP == itype)
	{
		strncpy(strCurfilter, "ipfilter_id", sizeof(strCurfilter)-1);
		strncpy(strEntry, IPMACFILTER_ENTRY_NODE, sizeof(strEntry)-1);
	}
	else if(DEL_ACL == itype)
	{
		strncpy(strCurfilter, "acl_id", sizeof(strCurfilter)-1);
		strncpy(strEntry, ACL_ENTRY_NODE, sizeof(strEntry)-1);
	}
	else if(DEL_DHCPD == itype)
	{
		strncpy(strEntry, DHCPD_ENTRYNODE, sizeof(strEntry)-1);
	}

	token = strstr(p, ",");
	while(token != NULL)
	{
		memset(strnum, 0x00, sizeof(strnum));
		token[0] = '\0';
		strncpy(strnum, p, sizeof(strnum)-1);
		if(!strlen(strnum)) break;
		/*unset action*/	
		if(DEL_DHCPD != itype)
		{
			cfg_set_object_attr(nodeName, strCurfilter, strnum);
		}
		snprintf(strentry, sizeof(strentry), "%s.%d", strEntry, ((atoi(strnum)) + 1));
		cfg_delete_object(strentry);
		/*get next token*/
		p = token + strlen(",");
		token = strstr(p, ",");
	}
	return SUCCESS;
}

char* getWanInfo(int type, char* device, char* buf)

{
    char cmd[128]={0};
    char tmpBuf[512]={0};
    char *pValue = NULL, *buffer = NULL;
    int size = 0, ret = 0;
    FILE * fp=NULL;
    char wanInfo[3][24];
    char dnsInfo[2][16];
    char macInfo[2][18];

    memset(wanInfo,0, sizeof(wanInfo));
    memset(dnsInfo,0, sizeof(dnsInfo));
    memset(macInfo,0, sizeof(macInfo));
    snprintf(cmd, sizeof(cmd), "/sbin/ifconfig %s > %s", device, TMP_IF_PATH);
    system(cmd);
    strcpy(buf,"");

    fp = fopen(TMP_IF_PATH, "r");
    if(fp==NULL)
    {
        return buf;
    }
    else
    {
        while((ret = getline(&buffer, &size, fp)) != -1)
        {
            /*krammer add*/
            if((pValue=strstr(buffer,"HWaddr")))
            {
                /*tmpBuf format: HWaddr 00:AA:BB:01:23:45*/
                sscanf(pValue,"%s %s", macInfo[0],macInfo[1]);
            }
            /*krammer add*/
            else if((pValue=strstr(buffer,"addr:")))
            {
                /*tmpBuf format: inet addr:10.10.10.1  Bcast:10.10.10.255  Mask:255.255.255.0*/
                sscanf(pValue,"%s %s %s", wanInfo[0],wanInfo[1], wanInfo[2]);
		free(buffer);
		buffer = NULL;
                break;
            }
            free(buffer);
            buffer = NULL;
        }
    }
    fclose(fp);
    memset(tmpBuf,0, sizeof(tmpBuf));

    switch(type)
    {
        case Ip_type:/*IP address information*/
        {
            pValue=strchr(wanInfo[0],':');
            if(pValue)
            {
                strcpy(buf, pValue+1);
            }
            break;
        }
        case PTP_type:/*PPP connection, get PTP Addr*/
        {
            pValue=strchr(wanInfo[1],':');
            if(pValue)
            {
                strcpy(buf, pValue+1);
            }
            break;
        }
        case Netmask_type:/*netmask address*/
        {
            pValue=strchr(wanInfo[2],':');
            if(pValue)
            {
                strcpy(buf, pValue+1);
            }
            break;
        }
        case Pridns_type: /*IP address of DNS Server*/
        case Secdns_type:
        {
            fileRead(DNS_INFO_PATH, tmpBuf, sizeof(tmpBuf));
            if(strstr(tmpBuf, DNS_ATTR))
            {
                /*tmpBuf format: nameserver x.x.x.x*/
                sscanf(strstr(tmpBuf, DNS_ATTR),"%s %s",dnsInfo[0],dnsInfo[1]);
                strcpy(buf, dnsInfo[1]);
                /*xyzhu_20091225 support secondary dns server*/
                if(type == Secdns_type)
                {
                    memset(buf, 0, strlen(buf) + 1);
                    if(strstr(tmpBuf + strlen(DNS_ATTR), DNS_ATTR))
                    {
                        sscanf(strstr(tmpBuf + strlen(DNS_ATTR), DNS_ATTR), "%s %s", dnsInfo[0], dnsInfo[1]);
                        strcpy(buf, dnsInfo[1]);
                    }
                }
            }
            break;
        }
        case Mac_type:/*Mac address*/
        {
            if(strlen(macInfo[1])>0)
            {
                strcpy(buf, macInfo[1]);
            }
            break;
            
        }
        default:
        {
            break;
        }
    }
    
    unlink(TMP_IF_PATH);
    return buf;
}

int getLanPortInfo(char *node)
{
	char port_info_attr[][32]=
	{
		{"Port1Speed"}, {"Port2Speed"},
		{"Port3Speed"}, {"Port4Speed"},
		{"Port1Duplex"}, {"Port2Duplex"},
		{"Port3Duplex"}, {"Port4Duplex"},
		{""}
	};

	char str[20] = {0};
	char speed[20] = {0}, duplex[20] = {0};
	int ret = 0;
	int i = 0;
	char *p = NULL;
#if defined(TCSUPPORT_ANDLINK)
	FILE *wanSta = NULL;
	char wanStabuf[MAXSIZE] = {0};
#endif
	lanport_info_t lanport_info;


	ret = blapi_traffic_get_lanport_info(&lanport_info);
	for(i = 0; i < 4; i++)
			{
		memset(speed,0,sizeof(speed));
		memset(duplex,0,sizeof(duplex));
		p = strtok(lanport_info.link_rate[i], "/");
				if ( p )
				{
					snprintf(speed, sizeof(speed), "%s", p);
				}
				p = strtok(NULL, "/");
				if ( p )
				{
					snprintf(duplex, sizeof(duplex), "%s", p);
				}
				cfg_set_object_attr(node, port_info_attr[i], speed);
				cfg_set_object_attr(node, port_info_attr[i+4], duplex);
			}
#if defined(TCSUPPORT_ANDLINK)
	wanSta = fopen("/proc/tc3162/eth1_link_st", "r");
	if ( NULL == wanSta )
	{
		tcdbg_printf("[%s:%d]: open /proc/tc3162/eth1_link_st fail!\n",__FUNCTION__,__LINE__);
		cfg_set_object_attr(node, "wansta", "down");
		return SUCCESS;
	}
	while ( fgets(wanStabuf, MAXSIZE, wanSta) )
	{
		ret = sscanf(wanStabuf, "%s %s", lanport_info.link_rate, str);

		if(strcmp(lanport_info.link_rate, "Down") == 0)
		{
			cfg_set_object_attr(node, "wansta", "down");
			break;
		}
		p = strtok(lanport_info.link_rate, "/");
		if ( p )
		{
			snprintf(speed, sizeof(speed), "%s", p);
		}
		p = strtok(NULL, "/");
		if ( p )
		{
			snprintf(duplex, sizeof(duplex), "%s", p);
		}
		cfg_set_object_attr(node, "wansta", "up");
		cfg_set_object_attr(node, port_info_attr[3], speed);
		cfg_set_object_attr(node, port_info_attr[7], duplex);
	}
	
	fclose(wanSta);
#endif
	return SUCCESS;
}
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
/*______________________________________________________________________________
**	function name: is_ethernet_link_up
**	description:
**         	check ethernet link status
**	parameters:
**	return:
**			0:ethernet link is down
**			1:ethernet link is up
**	call:
*     		none
**	revision:
*     		
**____________________________________________________________________________
*/
int is_ethernet_link_up()
{
    char etherState[8] = {0};
	int etherUp = 0;

    blapi_traffic_get_ether_wan_state(&etherUp);
	if(etherUp)
	{
		strncpy(etherState, "up", sizeof(etherState)-1);
	}
	else
	{
		strncpy(etherState, "down", sizeof(etherState)-1);
	}

	cfg_set_object_attr(WANINFO_COMMON_NODE, "EthernetState", etherState);


	return etherUp;
}

int is_ActiveEtherWan_link_up()
{
    char etherState[8] = {0};
	int etherUp = 0;

	blapi_traffic_get_active_ether_wan_state(&etherUp);
	
	if(etherUp)
	{
		strncpy(etherState, "up", sizeof(etherState)-1);
	}
	else
	{
		strncpy(etherState, "down", sizeof(etherState)-1);
	}

	cfg_set_object_attr(WANINFO_COMMON_NODE, "EthernetState", etherState);


	return etherUp;
}
#endif

void get_current_time(char *time_buf, int len)
{
	time_t cur_ltime;
	struct tm nowTime;
	
	time(&cur_ltime);
	localtime_r(&cur_ltime, &nowTime);

	snprintf(time_buf, len, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon + 1, nowTime.tm_mday,
			nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);

	return;
}

void get_current_only_time(char *time_buf, int len)
{
	time_t cur_ltime;
	struct tm *nowTime;
	
	time(&cur_ltime);
	nowTime = localtime(&cur_ltime);

	strftime(time_buf, len, "%H:%M:%S", nowTime);

	return;
}

void get_current_tz(int *flag, int *tz_hour, int *tz_minute)
{
	FILE* fp_tz = NULL;
	char* line = NULL;
	int size = 0;
	int ret = -1;
	char tmp[4] = {0};

	fp_tz = fopen("/etc/TZ", "r");
	if (NULL != fp_tz) 
	{
		if ((ret = getline(&line, &size, fp_tz)) != -1) 
		{
			sscanf(line, "%*[^+-]%[+-]%d:%d", tmp, tz_hour, tz_minute);

			if (0 == strncmp(tmp, "-", 1))
			{
				*flag = -1;
			}
			else
			{
				*flag = 1;
			}
		}
		fclose(fp_tz);
		if (line)
		{
			free(line);
		}
	}
	
	return;
}
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
void get_current_DiagTZ(signed char *flag, unsigned char *tz_hour, unsigned char *tz_minute)
{
	FILE* fp_tz = NULL;
	char* line = NULL;
	int size = 0;
	int ret = -1;
	char tmp[4] = {0};

	fp_tz = fopen("/etc/DIAGTZ", "r");
	if (NULL != fp_tz) 
	{
		if ((ret = getline(&line, &size, fp_tz)) != -1) 
		{
			sscanf(line, "%*[^+-]%[+-]%d:%d", tmp, tz_hour, tz_minute);

			if (0 == strncmp(tmp, "-", 1))
			{
				*flag = -1;
			}
			else
			{
				*flag = 1;
			}
		}
		fclose(fp_tz);
		if (line)
		{
			free(line);
		}
	}
	
	return;
}
void SetDiagTz(){
	int  socket_id;
	struct _diag_TZ_parameter diag_TZ_info;
	int i = 0;
	char *interface_info[10]={"ra0","rai0",""};

	memset(&diag_TZ_info,0,sizeof(diag_TZ_info));
	get_current_DiagTZ(&(diag_TZ_info.diag_tz_flag),&(diag_TZ_info.diag_tz_hour),&(diag_TZ_info.diag_tz_minute));

	for(i = 0; strlen(interface_info[i]) != 0; i++)
	{
		wifimgr_lib_set_Diag_Tz(interface_info[i], &diag_TZ_info, sizeof(diag_TZ_parameter));
	}
	return;


}
#endif

int get_inform_time(char *inform_time, int len)
{
	FILE *fp;
	int res = 0;

	fp = fopen("/opt/upt/apps/info/lastinformtm", "r");
	if (fp == NULL)
		return -1;

	res = fread(inform_time, sizeof(char), len-1, fp);
	fclose(fp);
	inform_time[len-1] = '\0';
	return 0;
}

int mac_add_dot(char *old_mac, char *new_mac)
{
	int i = 0, j = 0;

	if (old_mac == NULL || new_mac == NULL)
		return -1;

	for (j = 0; j < MAC_STR_DOT_LEN; j += 3){
		new_mac[j] = old_mac[i];
		new_mac[j + 1] = old_mac[i + 1];
		/* before the fifth round*/
		if (j < (MAC_STR_DOT_LEN-2))
		{
			if (old_mac[i+2] == ':'){
				i++;
			}
			new_mac[j + 2] = ':';
		}
		i += 2;
	}
	new_mac[MAC_STR_DOT_LEN] = '\0';
	
	return 0;
}

int mac_rm_dot(char *old_mac, char *new_mac)
{
	int i = 0, j = 0, mac_len = MAC_STR_DOT_LEN;

	if (old_mac == NULL || new_mac == NULL)
		return -1;
	
	for (i = 0; i < mac_len; i++) {
		if (old_mac[i] != ':')
			new_mac[j++] = old_mac[i];
	}
	new_mac[MAC_STR_LEN] = '\0';
	return 0;
}

/* cur string "bbb" from source string "aaabbbccc" to get result string "aaaccc" */
int cutString(char *in, int inlen, char *out, int outlen, char *cut, int flag)
{
	char *p = NULL;
	int len= 0;
	
	if ( NULL == in || NULL == out || NULL == cut )
		return -1;

	/* return if we can not find "bbb" from source string */
	if ( flag ) /* check characters for Capital */
	{
		if ( NULL == ( p = strstr(in, cut) ) )
			return -1;
	}
	else
	{
		if ( NULL == ( p = strcasestr(in, cut) ) )
			return -1;
	}

	/* ensure out string size enough */
	if ( outlen < ( strlen(in) - strlen(cut) ) )
		return -1;

	/* copy string "aaa" before "bbb" */
	memset(out, 0, outlen);
	strncpy(out, in, p - in);

	/* copy string "ccc" after "bbb" */
	p = p + strlen(cut);
	strcat(out, p);
	
	return 0;
}

/* add string "ccc" to source string "aaabbb" to get result string "aaabbbccc" */
int addString(char *in, int inlen, char *out, int outlen, char *add, int flag)
{
	char *p = NULL;

	if ( NULL == in || NULL == out || NULL == add )
		return -1;

	/* return if we can find "ccc" from source string */
	if ( flag ) /* check characters for Capital */
	{
		if ( NULL != ( p = strstr(in, add) ) )
			return -1;
	}
	else
	{
		if ( NULL != ( p = strcasestr(in, add) ) )
			return -1;
	}

	/* ensure out string size enough */
	if ( outlen < ( strlen(in) + strlen(add) ) )
		return -1;

	/* copy string "aaabbb" to source string */
	memset(out, 0, outlen);
	strncpy(out, in, outlen - 1);

	/* copy string "ccc" to source string */
	strcat(out, add);
	return 0;
}

int signalCwmpInfo(int mq, int reserved, int reserved2)
{
#if defined(TCSUPPORT_CWMP)

	int ret;
	long type = 1;
	int flag = 0;
	cwmp_msg_t message;

	memset(&message,0,sizeof(cwmp_msg_t));
	message.cwmptype = mq;
	
	if(reserved != 0)
	message.text.reserved[0] = reserved;

	if(reserved2 != 0)
	message.text.reserved[1] = reserved2;

	tcdbg_printf("\r\nmq=%d, reserved=%d,reserved2=%d!!",mq,reserved,reserved2);

	ret = sendmegq(type,&message,flag);
	if(ret < 0)
	{
		tcdbg_printf("\r\nsend message error!!");
		return ret;
	}
	else
	{
		tcdbg_printf("\r\nsend message ok!");
		return ret;
	}
#endif
	return 0;
}

char is_xpon_traffic_up(void)
{
	char buf[8];
    char node[32];
    char xpon_link = E_XPON_LINK_DOWN;
    
	memset(buf, 0, sizeof(buf));
    memset(node, 0, sizeof(node));
    strcpy(node, XPON_COMMON_NODE);
    
    cfg_get_object_attr(node, "trafficStatus", buf, sizeof(buf));	
	if(0 == strcmp(buf, "up")) 
	{
		xpon_link = E_XPON_LINK_UP;
	}
	else
	{
		xpon_link = E_XPON_LINK_DOWN;
	}
    
	return xpon_link;
}
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
int checkconflictssid(int type, int ssid) 
{
	char buf[8] = {0}, nodeName[64]={0};
	int newSSID = 0, i = 0, num = 0;
	int index = 0;
	
	memset(nodeName, 0, sizeof(nodeName));
	if (type == GUEST_TYPE) {
		index = snprintf(nodeName + index , sizeof(nodeName), "%s", WLANSHARE_NODE);
		num = WLANSHARE_INSTANCE_NUM;
	}
	else if (type == SHARE_TYPE) {
		index = snprintf(nodeName + index , sizeof(nodeName), "%s", GUESTSSIDINFO_NODE);
		num = GUESTSSIDINFO_NUM ;
	}
	else 
		return 0;
	for (i = 1; i <= num; i++) {
		snprintf(nodeName+index, sizeof(nodeName) - index, ".Entry.%d", i);
		if (cfg_obj_get_object_attr(nodeName, GSSIDINDEX, 0, buf, sizeof(buf)) < 0) {
			continue;
		}
		newSSID = atoi(buf);
		if (ssid == newSSID) {
			return -1;
		}
	}
	return 0;
}
#endif

int checkCFGload(void)
{
	FILE *fp = NULL;
	int res = 0;

	fp = fopen("/tmp/cfg_load", "r");
	if (fp == NULL)
		return 0;

	fclose(fp);
	return 1;
}

#if defined(TCSUPPORT_WLAN_APCLIENT)
int np_wlan_apcli_connstatus()
{
	char tmpBuf[8] = {0};
	int connStatus = NP_APCLI_DISCONNECTED, type = 0;


	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, tmpBuf, sizeof(tmpBuf));
	type = atoi(tmpBuf);
	connStatus = wifimgr_lib_get_apcli_status(type);

	if( connStatus )
	{
		if( type )
			return NP_APCLI_5G_CONNECTED;
		else
			return NP_APCLI_24G_CONNECTED;
	}
	
	return connStatus;
}

int np_wlan_apcli_get_sitesurvey(int type, char *SSID, char *channel, char *RSSI, char *Signal)
{
	char data[1024] = {0}, *p = NULL;
	char tmpSSID[64] = {0}, tmpRSSI[20] = {0}, tmpNo[4] = {0};
	
	snprintf(data, sizeof(data), "%s", SSID);
	
	wifimgr_lib_get_SiteSurvery(type, SSID, data, 32);
	p = strstr(data, "BSSID");
	if( p )
	{	
		sscanf(p, "%*[^\n]%s %s %s %*s %*s %s %s", tmpNo, channel, tmpSSID, RSSI, Signal);
	}
	else
		return -1;
		
	if( !strcmp(SSID, tmpSSID) )
		return 0;
	else
		return -1;
	
}

int np_wlan_set_wps_profile()
{
	char data[4096] = {0}, *p = NULL;
	int ret = -1, wifi_type = 0, key_length = 0;
	char currentRadio[4] = {0};
	char ssid[32] = {0}, AuthType[32] = {0}, EncrypType[32] = {0};
	char Key[64] = {0}, KeyBit[8] = {0}, KeyIndex[8] = {0};
	char nodepath[64] = {0};

	memset(currentRadio, 0, sizeof(currentRadio));
	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, currentRadio, sizeof(currentRadio));
	memset(nodepath, 0, sizeof(nodepath));
	if ( 0 == strcmp(currentRadio, "0") )
	{
		wifi_type = 0;
		strncpy(nodepath, APCLI_ENTRY0_NODE, sizeof(nodepath) - 1);
	}
	else if ( 0 == strcmp(currentRadio, "1") )
	{
		wifi_type = 1;
		strncpy(nodepath, APCLI_ENTRY1_NODE, sizeof(nodepath) - 1);
	}
	else
		return -1;

	
	memset(ssid, 0, sizeof(ssid));
	memset(AuthType, 0, sizeof(AuthType));
	memset(EncrypType, 0, sizeof(EncrypType));
	memset(Key, 0, sizeof(Key));
	memset(KeyIndex, 0, sizeof(KeyIndex));

	ret = wifimgr_lib_get_Statistics(wifi_type, data, sizeof(data));
	if( 0 == ret )
	{
		if ( 0 == strlen(data) )
			return -1;
		
		p = strstr(data, "SSID");
		if( p )
		{
			sscanf(p + strlen("SSID"), "%*[^=]= %s", ssid);
			if ( 0 == ssid[0] )
				return -1;
		}
		else
			return -1;
		
		p = strstr(data, "AuthType");
		if( p )
		{
			sscanf(p + strlen("AuthType"), "%*[^=]= %s", AuthType);
			if ( 0 == AuthType[0] )
				return -1;
		}
		else
			return -1;
		
		p = strstr(data, "EncrypType");
		if( p )
		{
			sscanf(p + strlen("EncrypType"), "%*[^=]= %s", EncrypType);
			if ( 0 == EncrypType[0] )
				return -1;
		}
		else
			return -1;
		
		cfg_set_object_attr(nodepath, "SSID", ssid);
		cfg_set_object_attr(nodepath, "AuthMode", AuthType);
		cfg_set_object_attr(nodepath, "EncrypType", EncrypType);

		p = strstr(data, "KeyIndex");
		if( p )
		{
			sscanf(p + strlen("KeyIndex"), "%*[^=]= %s", KeyIndex);
			p = p + strlen("KeyIndex");
		}
		else
		{
			p = data;
			strcpy(KeyIndex, "1");
		}
		p = strstr(p, "Key");
		if( p )
		{
			sscanf(p + strlen("Key"), "%*[^=]= %s", Key);
		}

		if ( NULL != strstr(AuthType, "WEP") )
		{
			key_length = strlen(Key);
			if ( 5 == key_length || 10 == key_length )
				strcpy(KeyBit, "64");
			else if ( 0 == key_length || 13 == key_length || 26 == key_length )
				strcpy(KeyBit, "128");
			
			cfg_set_object_attr(nodepath, "KeyBit", KeyBit);
			cfg_set_object_attr(nodepath, "Key", Key);
			cfg_set_object_attr(nodepath, "DefaultKeyID", KeyIndex);
		}
		else
			cfg_set_object_attr(nodepath, "WPAPSK", Key);
	}

	return ret;
}
#endif
static char DeviceAlarmList[19][8] = 
{
		"104001","104006","104012","104013","104030",
		"104032","104036","104050","104051","104052",
		"104053","104064","104057","104058","104104",
		"104105","104106","104142","104143",
};

int AddAlarmNumber(char * Num)
{
	int i = 0;
	int match = 0;	
	char alarmNumber[256]; 
	char * p = NULL;	
	char * pNode = NULL;
	char node[32];
	char Active[16] = {0};
	
	memset(node, 0, sizeof(node));
	snprintf(node, sizeof(node), DEVICEALARM_COMMON_NODE);
	cfg_get_object_attr(node, "Enable", Active, sizeof(Active));

	if( 0 == Active[0] || strcmp(Active, "Yes") )
	{
		cfg_set_object_attr(node, "AlarmNumber","");
		return 0;
	}
	
	for( i=0 ; i<sizeof(DeviceAlarmList)/sizeof(DeviceAlarmList[0]) ; i++ )
	{
		if(strcmp(DeviceAlarmList[i],Num) == 0)			
		{
			match = 1;
			pNode = DeviceAlarmList[i];
			break;
		}
	}
	if(match == 1)
	{
		memset(alarmNumber, 0, sizeof(alarmNumber));
		cfg_get_object_attr(node, "AlarmNumber", alarmNumber, sizeof(alarmNumber));
		if(strcmp(alarmNumber, ""))
		{
			if ( NULL == (p = strstr(alarmNumber, pNode)))
			{
				strcat(alarmNumber, ",");
				strcat(alarmNumber, pNode);
				cfg_set_object_attr(node, "AlarmNumber",alarmNumber);
			}
		}	
		else
		{
			cfg_set_object_attr(node, "AlarmNumber", pNode);
		}
	}

	return 0;
}

int DelAlarmNumber(char * Num)
{
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CMCCV2)
	int i = 0;
	int match = 0;	
	char alarmNumber[256]; 	
	char * p = NULL;
	char * q = NULL;	
	char * pNode = NULL;
	char node[32];
	char Active[16] = {0};

	memset(node, 0, sizeof(node));
	snprintf(node, sizeof(node), DEVICEALARM_COMMON_NODE);
	cfg_get_object_attr(node, "Enable", Active, sizeof(Active));

	if( 0 == Active[0] || strcmp(Active, "Yes") )
	{
		cfg_set_object_attr(node, "AlarmNumber","");
		return 0;
	}
	
	for( i=0 ; i<sizeof(DeviceAlarmList)/sizeof(DeviceAlarmList[0]) ; i++ )
	{
		if(strcmp(DeviceAlarmList[i],Num) == 0)			
		{
			match = 1;
			pNode = DeviceAlarmList[i];
			break;
		}
	}
	
	if(match == 1)
	{
		memset(alarmNumber, 0, sizeof(alarmNumber));
		cfg_get_object_attr(node, "AlarmNumber", alarmNumber, sizeof(alarmNumber));
		
		if(strcmp(alarmNumber, ""))
		{
			if ( (p = strstr(alarmNumber, pNode)) != NULL)
			{
				q = p+7;	
				if(q != NULL)
				{
					if(*q == '\0')//the alarm number is the last one
					{
						if(strcmp(alarmNumber, p)==0)//only one alarm number
						{
							strcpy(alarmNumber, "");
						}
						else
						{
							strcpy(p-1,q);
						}
					}
					else
					{
						strcpy(p, q);		
					}
					cfg_set_object_attr(node, "AlarmNumber", alarmNumber);
				}
			}
		}	
	}
#endif	
	return 0;
	
}

/*
	check wlan or lan status 
	type : 0 check lan,   1 check wlan
*/
int check_wlan_lan_status(int type)
{
	FILE *fp = NULL;
	char cmd[128] = {0}, *buf = NULL;
	char file_name[32] = {0}, *p = NULL;
	char log_info[128] = {0};
	int i = 0, j = 0, ret = 0;
	int down_num = 0, loopmax = 0;
	size_t len = 0;

	if( 0 == type )
		loopmax= MAX_LANPORT_NUM;
	else
		loopmax = 2;
	
	for(i = 0; i < loopmax; i++)
	{
		memset(file_name, 0, sizeof(file_name));
		memset(cmd, 0, sizeof(cmd));
		memset(log_info, 0, sizeof(log_info));
		if( 0 == type )
		{
			snprintf(file_name, sizeof(file_name), "/tmp/check_lan");
			snprintf(cmd, sizeof(cmd), "ifconfig eth0.%d > %s", i + 1, file_name);
		}
		else
		{
			snprintf(file_name, sizeof(file_name), "/tmp/check_wlan");
			if( 0 == i )
			{
				snprintf(cmd, sizeof(cmd), "ifconfig ra0 > %s", file_name);
			}
			else
			{
#if defined(TCSUPPORT_WLAN_AC)
				snprintf(cmd, sizeof(cmd), "ifconfig rai0 > %s", file_name);
#else
				break;
#endif
			}
		}
		
		system(cmd);

		fp = fopen(file_name, "r");
		if( !fp )
			continue;

		ret = getline(&buf, &len, fp);
		if( -1 == ret || strstr(buf, "Device not found") )
		{
			if( 0 == type )
			{
				snprintf(log_info, sizeof(log_info), "[Alert] alarm id:104006 lan%d is disabled\n", i + 1);
			}
			else
			{
				if( 0 == i )
				{
					snprintf(log_info, sizeof(log_info), "[Alert] alarm id:104013 2.4G software error\n");
				}
#if defined(TCSUPPORT_WLAN_AC)
				else
				{
					snprintf(log_info, sizeof(log_info), "[Alert] alarm id:104013 5G software error\n");
				}
#endif
			}

			openlog("TCSysLog alarm", 0, LOG_LOCAL2);
			syslog(LOG_ALERT,log_info);
			closelog();
			fclose(fp);
			unlink(file_name);
			down_num++;
			continue;
		}
				
		fclose(fp);
		unlink(file_name);
	}

	if( 0 == down_num )
	{
		if( 0 == type )
			DelAlarmNumber("104006");
		else
			DelAlarmNumber("104013");
	}
	else
	{
		if( 0 == type )
			AddAlarmNumber("104006");
		else
			AddAlarmNumber("104013");
	}

	if( buf )
		free(buf);
	
	return 0;
}

int parseUrlHostInfo(char *pUrl, char *pHost, int hostlen, unsigned short *pPort)
{
	char *pSource = NULL, *p = NULL;

	if ( NULL == pUrl || NULL == pHost || 0 == hostlen || NULL == pPort )
		return -1;

	/* init default host and port for http */
	*pHost = '\0';
	*pPort = 80;

	/* check https or http protocol, and skip over the "http*://" */
 	if ( 0 == strncasecmp(pUrl, "https://", 8) )
	{
		*pPort = 443;
		pSource = pUrl + 8;
	}
	else if ( 0 == strncasecmp(pUrl, "http://", 7) )
	{
		pSource = pUrl + 7;
	}
	else
	{
		pSource = pUrl;
	}

	/* get host and port */
	if ( NULL != ( p = strstr(pSource, ":") ) )
	{
		/* host length is larger than hostbuf, reutrn error */
		if ( p - pSource > hostlen )
			return -1;
		*pPort = strtoul(p + 1, NULL, 10);

		*p = '\0';
		strncpy(pHost, pSource, hostlen);
	}
	else
	{
		/* host length is larger than hostbuf, reutrn error */
		if ( strlen(pSource) > hostlen )
			return -1;

		strncpy(pHost, pSource, hostlen);
	}

	return 0;
}

#if defined(TCSUPPORT_CT_IPOE_DETECT)
int svc_other_handle_event_ipoe_update(int index)
{
	char tmp[128] = {0};
	char cmd[128] = {0};
	char ifname[16] = {0};
	char wan_active[8] = {0};
	char node_path[32] = {0};
	int pvc = -1, entry = -1;
	char pidfile[32] = {0};
	FILE *fp = NULL;
	char *buf = NULL;
	size_t len = 0;
	int pid = 0;
	int wan_index = -1;
	int prewan_index = -1;

	snprintf(node_path, sizeof(node_path), IPOEDIAG_ENTRY_NODE, index);
	cfg_obj_get_object_attr(node_path, "wan_index", 0, tmp, sizeof(tmp));
	wan_index = atoi(tmp);

	snprintf(pidfile, sizeof(pidfile), "/var/run/ipoediag_%d", index - 1);
	fp = fopen(pidfile, "r");
	if( fp )
	{
		if(getline(&buf, &len, fp) != -1)
		{
			kill(atoi(buf), SIGTERM);
		}
		fclose(fp);
		if( buf )
			free(buf);
	}
	unlink(pidfile);
	
	memset(tmp, 0, sizeof(tmp));
	cfg_obj_get_object_attr(node_path, "Enable", 0, tmp, sizeof(tmp));
	if( !strcmp(tmp, "1") )
	{
		pvc = wan_index / 8;
		entry = wan_index % 8;
		if( pvc >= 0 && entry >= 0 )
		{
			snprintf(node_path, sizeof(node_path), WAN_PVC_ENTRY_NODE, pvc+ 1, entry+ 1);
			memset(tmp, 0, sizeof(tmp));
			cfg_obj_get_object_attr(node_path, "Active", 0, tmp, sizeof(tmp));
			if( !strcmp(tmp, "Yes") )
			{
				snprintf(ifname, sizeof(ifname), "nas%d_%d", pvc, entry);
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "/userfs/bin/ipoe_diag -I %s -i %d -P /var/run/ipoediag_%d &", ifname, index - 1, index - 1);
				system(cmd);

				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "%d", 1);
			}
		}
	}
	
}
#endif

int get_br0_v6addr(unsigned char *ifid)
{
	char ifname[] = "br0";
    int fd6 = 0;
    struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
    if ((fd6 = socket(AF_INET6, SOCK_DGRAM, 0)) == -1)
    {
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);	
    if(ioctl(fd6, SIOCGIFHWADDR, &ifr) < 0)
    {
        close(fd6);
        return -1;
    }

    ifid[2] = ifr.ifr_hwaddr.sa_data[0];
    ifid[2] ^= 0x02; /* reverse the u/l bit*/
    ifid[3] = ifr.ifr_hwaddr.sa_data[2];
    ifid[4] = ifr.ifr_hwaddr.sa_data[1];
    ifid[5] = 0xe3;
    ifid[6] = ifr.ifr_hwaddr.sa_data[4];
    ifid[7] = 0xef;

    ifid[8] = ifr.ifr_hwaddr.sa_data[0];
    ifid[8] ^= 0x02; /* reverse the u/l bit*/
    ifid[9] = ifr.ifr_hwaddr.sa_data[1];
    ifid[10] = ifr.ifr_hwaddr.sa_data[2];
    ifid[11] = 0xff;
    ifid[12] = 0xfe;
    ifid[13] = ifr.ifr_hwaddr.sa_data[3];
    ifid[14] = ifr.ifr_hwaddr.sa_data[4];
    ifid[15] = ifr.ifr_hwaddr.sa_data[5];

    close(fd6);

	return 0;
}

int get_br0_ifaddr(int sock, char *mac)
{	
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, "br0", sizeof(ifr.ifr_name) - 1);
	
	if ( ioctl(sock, SIOCGIFHWADDR, &ifr) < 0 )
	{
		return -1;
	}
	memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);

	return 0;
}

#if defined(TCSUPPORT_ECNT_MAP)
int check_mesh_enable()
{	
	char enable[10] = {0};		

	if(cfg_get_object_attr(MESH_DAT_NODE, "MapEnable", enable, sizeof(enable)) > 0  
		&& !strcmp(enable,"1"))	{
		return 1;			
	}
	else
		return 0;	
}

int check_mesh_role()
{	
	char DeviceRole[10] = {0};		

	if(cfg_get_object_attr(MESH_COMMON_NODE, "DeviceRole", DeviceRole, sizeof(DeviceRole)) > 0)
	{
		return atoi(DeviceRole);			
	}
	else
		return 0;	
}
#endif

#if defined(TCSUPPORT_CT_PON)
#if !defined(TCSUPPORT_CMCC)
int saveLoid()
{
	char nodeName[64] = {0};
	char epon_nodeName[64] = {0};
	char v_username_old[128] = {0};
	char v_password_old[128] = {0};
	char v_username_new[128] = {0};
	char v_password_new[128] = {0};

	memset(nodeName, 0, sizeof(nodeName));
	memset(epon_nodeName, 0, sizeof(epon_nodeName));
	
	snprintf(nodeName, sizeof(nodeName), LOGICID_ENTRY_NODE);
	snprintf(epon_nodeName, sizeof(epon_nodeName), EPON_LOIDAUTH_NODE);

	memset(v_username_old, 0, sizeof(v_username_old) );
	if( cfg_get_object_attr(nodeName, LOGICID_USERNAME, v_username_old, sizeof(v_username_old)) < 0 )
		v_username_old[0] = '\0';

	memset(v_password_old, 0, sizeof(v_password_old) );
	if( cfg_get_object_attr(nodeName, LOGICID_PSW, v_password_old, sizeof(v_username_old)) < 0 )
		v_password_old[0] = '\0';

	/* get loid from epon only, because epon loid will be same with gpon */
	memset(v_username_new, 0, sizeof(v_username_new) );
	if( cfg_get_object_attr(epon_nodeName, EPON_LOID_USR, v_username_new, sizeof(v_password_new)) < 0
		|| '\0' == v_username_new[0] )
		return -1;
	
	memset(v_password_new, 0, sizeof(v_password_new) );
	if( cfg_get_object_attr(epon_nodeName, EPON_LOID_PSW, v_password_new, sizeof(v_password_new)) < 0 )
		return -1;

	if ( 0 != strcmp(v_username_old, v_username_new)
		|| 0 != strcmp(v_password_old, v_password_new) )
	{
		cfg_set_object_attr(nodeName, LOGICID_USERNAME, v_username_new);
		cfg_set_object_attr(nodeName, LOGICID_PSW, v_password_new);
		cfg_evt_write_romfile_to_flash();
		tcdbg_printf("\nnew loid is saved! \n");
	}

	return 0;
}
#endif
#endif
int getBhfhParam(const char *entryNode){
char value[8];	int bhfhvalue = 0;
	memset(value, 0, sizeof(value));
	if( (0 < cfg_get_object_attr(entryNode, MAP_BSSINFO_FRONT_HAUL_ATTR, value, sizeof(value))) \
		&& (1 == atoi(value)) )
	{
		bhfhvalue = 1;
		return bhfhvalue;
	}
	memset(value, 0, sizeof(value));
	if( (0 < cfg_get_object_attr(entryNode, MAP_BSSINFO_BACK_HAUL_ATTR, value, sizeof(value))) \
		&& (1 == atoi(value)) )
	{
		bhfhvalue = 1;
	}
	return bhfhvalue;
}

