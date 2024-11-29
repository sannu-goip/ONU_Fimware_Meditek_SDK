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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/msg.h>
#include "cfg_msg.h"
#include <fcntl.h>
#include "traffic/global_dnshost.h"

/*_____________________________________________________________________________
**      function name: tcdbg_printf
**      descriptions:
**            Show debug message in kernel mode.
**
**      parameters:
**           N/A
**
**      global:
**           N/A
**
**      return:
**	     	 N/A
**      call:
**      	 N/A
**      revision:
**      1. JasonLin 2008/11/25
**____________________________________________________________________________
*/
void
tcdbg_printf(char *fmt,...){

	FILE *proc_file;
	char msg[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, 256, fmt, args);	

    proc_file = fopen("/proc/tc3162/dbg_msg", "w");
	if (!proc_file) {
		printf("open /proc/tc3162/dbg_msg fail\n");
		va_end(args);
		return;
	}
	fprintf(proc_file, "%s", msg);
	fclose(proc_file);
	va_end(args);
}

/*_____________________________________________________________________________
**      function name: sendmegq()
**      descriptions:
**            Used to send message to tr69 application by message mechanism
**
**      parameters:
**            sendMsg:   Put the message that you want to send to cfg manager.

**
**      global:
**             
**      return:
**             -1: open message fail
**			   -2: write message fail
**				0: ok
**      call:
**
**      revision:
**      1. Brian 2009/12/01
**____________________________________________________________________________
*/
int sendmegq(long type,cwmp_msg_t *buf,int flag)
{	
	key_t mqkey;
	int oflag,mqid;
	tc_msg_t msg;
	int ret;
	cwmp_msg_t *ptr = buf;
	oflag = 0666 | IPC_CREAT;
	mqkey = ftok(CWMP_MQ_FLAG_PATH,PROJID);
	mqid = msgget(mqkey,oflag);   
	if(mqid < 0)
	{
		printf("\r\n open message queue fail!");
		return -1;
	}

	memset(&msg,0,sizeof(tc_msg_t));
	msg.mtype = type;
	memcpy((void*)&msg.msgtext,(void*)ptr,sizeof(cwmp_msg_t));
	ret = msgsnd(mqid,&msg,sizeof(cwmp_msg_t),flag|IPC_NOWAIT); /*send no block*/
	if(ret < 0)
	{
		printf("\r\n write message fail!");
		return -2;
	}
	return 0;
}

/*_____________________________________________________________________________
**      function name: send2dnshost
**      descriptions:
**            Used to send message to dnshost via unix socket.
**
**      parameters:
**            sendMsg:   Put the message that you want to send to dnshost.

**
**      global:
**             None
**      return:
**             None
**
**      call:
**
**      revision:
**      1. Xueyu.Zhang 2018/12/12
**____________________________________________________________________________
*/
void
send2dnshost(traffic2host_msg_t *sendMsg){
	int client_sockfd = 0, len = 0;
	struct sockaddr_un remote;
	char buf[128]={0};

	if ((client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, TRAFFICAPP_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(client_sockfd, (struct sockaddr *)&remote, len) == -1) {
		close(client_sockfd);
		return;
	}

	if (send(client_sockfd, sendMsg, sizeof(traffic2host_msg_t), MSG_NOSIGNAL) == -1){
		perror("send");
		close(client_sockfd);
		return;
	}

	memset(sendMsg,0,sizeof(traffic2host_msg_t));
	if ((recv(client_sockfd, sendMsg, sizeof(traffic2host_msg_t), MSG_NOSIGNAL)) <= 0 ) {
		perror("recv");
		close(client_sockfd);
		return;
	}
	close(client_sockfd);
	return;
}

void sendTodnshost(int bitflag, int action, char *name)
{
	traffic2host_msg_t sendMsg;
	
	memset(&sendMsg,0,sizeof(sendMsg));
	sendMsg.bitflag = bitflag;
	sendMsg.action= action;
	if( NULL != name && '\0' != name[0])
		strncpy(sendMsg.domain_name,name,sizeof(sendMsg.domain_name)-1);
	else
	{
		if( ADD_ACT == action )
			return;
	}
	send2dnshost(&sendMsg);
	return;
}

/*_____________________________________________________________________________
**      function name: nlksock_create()
**      descriptions:
**            Used to create netlink socket
**
**      parameters:
**           
**
**      global:
**             
**      return:
**             -1: create socket fail
**				0: sockfd
**      call:
**
**      revision:
**     
**____________________________________________________________________________
*/
int nlksock_create(int nlk_type, int pid, int group) {
	int sock_fd;
	struct sockaddr_nl nl_saddr;
	const int opt = 1;
	
	sock_fd = socket(PF_NETLINK, SOCK_RAW, nlk_type);
	if(sock_fd < 0) {
		tcdbg_printf("%s:nlksock create err:%s!\n", __FUNCTION__,strerror(errno));
		return -1;
	}

	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0){
		tcdbg_printf("%s:setsockopt err:%s!\n", __FUNCTION__,strerror(errno));
		close(sock_fd);
		return -1;
	}
	
	memset(&nl_saddr, 0, sizeof(nl_saddr));
	nl_saddr.nl_family = AF_NETLINK;
	nl_saddr.nl_pid = pid;  /* pid */
	nl_saddr.nl_groups = group;  /* mcast groups */

	if(bind(sock_fd, (struct sockaddr*)&nl_saddr, sizeof(nl_saddr)) < 0){
		tcdbg_printf("%s:nlksockt bind err:%s!\n", __FUNCTION__,strerror(errno));
		close(sock_fd);
		return -1;
	}
	
	return sock_fd;
}

int nlksock_create_2(int nlk_type, int pid, int group) {
	int sock_fd;
	struct sockaddr_nl nl_saddr;
	int sndbuf = 32768;
	int rcvbuf = 32768;
	socklen_t addr_len;
	
	sock_fd = socket(AF_NETLINK, SOCK_RAW, nlk_type);
	if(sock_fd < 0) {
		tcdbg_printf("%s:nlksock create err:%s!\n", __FUNCTION__,strerror(errno));
		return -1;
	}

	if (setsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,&sndbuf,sizeof(sndbuf)) < 0) {
		perror("SO_SNDBUF");
		close(sock_fd);
		return -1;
	}

	if (setsockopt(sock_fd,SOL_SOCKET,SO_RCVBUF,&rcvbuf,sizeof(rcvbuf)) < 0) {
		perror("SO_RCVBUF");
		close(sock_fd);
		return -1;
	}
	
	memset(&nl_saddr, 0, sizeof(nl_saddr));
	nl_saddr.nl_family = AF_NETLINK;
	nl_saddr.nl_pid = pid;  /* pid */
	nl_saddr.nl_groups = group; /* mcast groups */

	if(bind(sock_fd, (struct sockaddr*)&nl_saddr, sizeof(nl_saddr)) < 0){
		tcdbg_printf("%s:nlksockt bind err:%s!\n", __FUNCTION__,strerror(errno));
		close(sock_fd);
		return -1;
	}

	addr_len = sizeof(nl_saddr);
	if (getsockname(sock_fd, (struct sockaddr*)&nl_saddr,  &addr_len) < 0) {
		perror("Cannot getsockname");		
		close(sock_fd);
		return -1;
	}
	if (addr_len != sizeof(nl_saddr)) {
		fprintf(stderr, "Wrong address length %d\n", addr_len);
		close(sock_fd);
		return -1;
	}
	if (nl_saddr.nl_family != AF_NETLINK) {
		fprintf(stderr, "Wrong address family %d\n",nl_saddr.nl_family);
		close(sock_fd);
		return -1;
	}
	
	return sock_fd;
}

/*_____________________________________________________________________________
**      function name: nlkmsg_send()
**      descriptions:
**            Used to send message to phone app by message mechanism
**
**      parameters:
**
**      global:
**             
**      return:

**      call:
**
**      revision:
**     
**____________________________________________________________________________
*/
int nlkmsg_send_func(int socktype, int group, int len, void *context){
	int fd, ret;
	struct sockaddr_nl nl_daddr;
	
	fd = nlksock_create(socktype, pthread_self() | getpid(), group);
	if(fd< 0 ){
		return -1;
	}

	memset(&nl_daddr, 0, sizeof(nl_daddr));
	nl_daddr.nl_family = AF_NETLINK;
	nl_daddr.nl_pid = pthread_self() | getpid();
	nl_daddr.nl_groups = group;
	
	ret = sendto(fd, context, len, 0, (struct sockaddr*)&nl_daddr, sizeof(nl_daddr));  
	close(fd);
	if(ret <0) tcdbg_printf("netlink send err:%s!\n", strerror(errno));
	return ret;
}


int nlkmsg_send(int group, int len, void *context){
	nlkmsg_send_func(NLKMSG_SOCK_TYPE, group, len, context);
	return 0;
}

int netlink_send(int sockType, int eventType, int group, char* buf){
	nlk_msg_t nlkmsg;
	int ret = 0;

	memset(&nlkmsg, 0, sizeof(nlk_msg_t));	
	nlkmsg.nlmhdr.nlmsg_len = sizeof(nlk_msg_t);
	nlkmsg.nlmhdr.nlmsg_pid = getpid();  /* self pid */
	nlkmsg.nlmhdr.nlmsg_flags = 0;
	
	nlkmsg.eventType = eventType;;
	strncpy(nlkmsg.data.payload,buf, sizeof(nlkmsg.data.payload) - 1);	
	
	tcdbg_printf("sockType:%d,group=%d,eventType =%d!\n",sockType,group, eventType);
	ret = nlkmsg_send_func(sockType,group, sizeof(nlkmsg), &nlkmsg);
	
	if(ret <0){
		tcdbg_printf("netlink send err:%s!\n", strerror(errno));
		return -1;
	}
	
	tcdbg_printf("buf =%s!ret=%d\n",buf,ret);
	return 0;
}


typedef struct pkt_buf {
	unsigned int length;
	unsigned char payload[];
} pkt_buf_t;

#define EVENT_INFORM_PATH		"/tmp/eventInform"
/*_____________________________________________________________________________
**      function name: msgpush_send()
**      descriptions:
**            Used to send messagepush pkt to phone app local sock
**
**      parameters:
**
**      global:
**             
**      return:

**      call:
**
**      revision:
**     
**____________________________________________________________________________
*/
int msgpush_send(int pid, char *name, char *buffer){
	int sockfd=0, len = 0;
	unsigned int len0 = 0, len1 = 0;
	char lenbuf[21] = {0};
	pkt_buf_t *pPktbuf = NULL;
	struct msghdr msg;
	struct iovec iov[3];
	struct sockaddr_un remote;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {	
		tcdbg_printf("%s: call socket error\r\n",__FUNCTION__); 
		return	-1;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, EVENT_INFORM_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
		tcdbg_printf("%s: call connect error!\n",__FUNCTION__); 	
		close(sockfd);
		return -1;
	}	

	if(name){
		len0 = strlen(name);
	}
	len1 = strlen(buffer);	
	pPktbuf = (pkt_buf_t *)lenbuf;
	sprintf(pPktbuf->payload, "%016X", pid);
	pPktbuf->length = htonl(len0 + len1 + 16);
	memset(&msg, 0, sizeof(msg));
	iov[0].iov_base = (void *)lenbuf;
	iov[0].iov_len = sizeof(unsigned int) +16;
	iov[1].iov_base = name;
	iov[1].iov_len = len0;
	iov[2].iov_base = (void *)buffer;
	iov[2].iov_len = len1;
	
	msg.msg_name = NULL; 
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 3;

	if (sendmsg(sockfd, &msg, 0)== -1){
		tcdbg_printf("%s: send error!\n", __FUNCTION__);		
		close(sockfd);
		return -1;
	}

	close(sockfd);
	return 0;
}

void sendEventInform(char *result, char *ver, char *errcode, char *upgid)
{
	char *pkt = NULL;

	if ( !result || !ver || !errcode || !upgid )
		return;

	pkt= calloc(1, 512);
	if ( !pkt )
		return;
	snprintf(pkt, 512 - 1, 
	"{\"CmdType\":\"DEVICE_UPGRADE\", \"result\":\"%s\", "
	"\"ver\":\"%s\", \"errcode\":\"%s\", \"upgrade_ID\":\"%s\"}"
	, result, ver, errcode, upgid);

	msgpush_send(0, NULL, pkt);

	if ( pkt )
		free(pkt);
}

void sendToMobileManagerFIFO(int type)
{
	int fd = -1, ret = 0;
	char buffer = 1;
	
	if (type == WIFITIMER_TYPE) {
		fd = open(MOBILEMANAGERFIFO, O_WRONLY|O_NONBLOCK);
	}
	else if (type == URL_TYPE) {
		fd = open(WANCHANGEFIFO, O_WRONLY|O_NONBLOCK);
		buffer = -1;
	}
	if(fd < 0) {
		tcdbg_printf("sendToMobileManagerFIFO: open fifo err.\n");
		return;
	}

	ret = write(fd, &buffer,sizeof(buffer));
	if(ret < 0 ){
		tcdbg_printf("sendToMobileManagerFIFO: write fifo err.\n");
	}
	
	close(fd);	
	return;
}
