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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdarg.h>

/*add by brian for message mechanism*/
#include <sys/msg.h>
#include "tcapi.h"
#include "libtcapi.h"
#include "traffic/global_dnshost.h"
#define OMCI_CMD_Q_KEY 2016
#define OMCI_NOTIFY_Q_KEY 2017
#define TCAPI_DBUG_ON 0


#ifdef CFG2_FLAG
#include <cfg_api.h>

void
send2CfgManager_to_1(tcapi_msg_t *sendMsg){

	int sockfd=0, len=0;
	struct sockaddr_un remote;
	sendMsg->retval=TCAPI_CONN_ERR;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
		// frank delete for bug 9975 20110426
		//perror("connect");
		close(sockfd);
		return;
	}

	if (send(sockfd, sendMsg, sizeof(tcapi_msg_t), 0) == -1){
		perror("send");
		close(sockfd);
		return;
	}

	memset(sendMsg,0,sizeof(tcapi_msg_t));

	if ((recv(sockfd, sendMsg, sizeof(tcapi_msg_t), 0)) <= 0 ) {
		/*Failure to receive information from cfg manage*/
		perror("recv");
		close(sockfd);
		return;
	}
	close(sockfd);
}/*end send2CfgManager */

static char* get_action_string(int act)
{
	static char* g_action_str[4] = {"get","set","commit","N/A"};

	switch(act)
	{
		case TCAPI_SET_CMD:
			return g_action_str[1];
		case TCAPI_STATIC_GET_CMD:	
		case TCAPI_GET_CMD:	
			return g_action_str[0];
		case TCAPI_COMMIT_CMD:
			return g_action_str[2];
	}
	return g_action_str[3];
}


static int is_tcapi_debug_on(void)
{
	FILE* fp;
	char id[16];
	int pid;

#if TCAPI_DBUG_ON
	if ((fp = fopen("/tmp/tcapi_debug_on","r")) == NULL)
		return 0;

	memset(id,0,sizeof(id));
	
	fgets(id,sizeof(id),fp);
	
	fclose(fp);
	
	pid = atoi(id);
	
	if (pid == getpid() || pid == 0)
		return 1;
#endif
	return 0;
}


static int is_cfg_ng_enable(void)
{
	FILE* fp;

	if ((fp = fopen("/tmp/use_cfg_ng","r")) != NULL)
	{
		fclose(fp);
		return 1;
	}

	return 0;
}

static int is_omci_process(tcapi_msg_t* sendMsg)
{
	char path[128];
	int len;
	
	if ((len = readlink("/proc/self/exe",path,sizeof(path)-1)) < 0)
		return 0;

	path[len] = '\0';
	
	if (strstr(path,"boa") !=  NULL)
		return 1;

	return 0;
}


void send2CfgManager_to_2(tcapi_msg_t *sendMsg)
{
	char path[128];
	int ret = 0;
	char* node = sendMsg->node;
	char* attr = sendMsg->attr;
	char* val = sendMsg->value;


	sendMsg->retval = TCAPI_NONE_NODE;

	if (shm_gettree() == NULL)
		return;
	
	cfg_obj_set_root(shm_gettree());

	if (sendMsg->op_cmd  == TCAPI_SAVE_CMD)
	{
		ret = cfg_evt_write_romfile_to_flash(); 		
		sendMsg->retval = TCAPI_PROCESS_OK;
		return;
	}
	else if(sendMsg->op_cmd  == TCAPI_SAVE_RAW_CMD)
	{
		ret = cfg_evt_write_romfile_to_flash_raw(); 		
		sendMsg->retval = TCAPI_PROCESS_OK;
		return;
	}

	if (cfg_parse_node_name(node,path,sizeof(path)) < 0)
	{
		tcdbg_printf("parese node name  Fail! [%s]  \n",node);
		return;
	}
	
	switch(sendMsg->op_cmd)
	{
		case TCAPI_SET_CMD:
			if(cfg_obj_query_object(path, NULL, NULL) <= 0)
			{
				if(cfg_obj_create_object(path) < 0)
				{
					ret = -1;
					if (is_tcapi_debug_on())
						tcdbg_printf("====> [%s]: [%d] \n",path,ret);
					break;
				}
			}
			ret = cfg_set_object_attr(path,attr,val);
			if (is_tcapi_debug_on())
				tcdbg_printf("====> [%s]: [%d] \n",path,ret);
			break;
		case TCAPI_STATIC_GET_CMD:	
		case TCAPI_GET_CMD:
			ret = cfg_get_object_attr(path,attr,val,1024);	
			if (is_tcapi_debug_on())
				tcdbg_printf("====> [%s]:[%s] [%d] \n",path,val,ret);
			break;
		case TCAPI_COMMIT_CMD:
			ret = cfg_commit_object(path);
			if (is_tcapi_debug_on())
				tcdbg_printf("====> [%s]: [%d] \n",path,ret);	
			break;
		case TCAPI_SHOW_CMD:
			cfg_obj_dump(path,1);
			break;
		case TCAPI_UNSET_CMD:
			ret = cfg_delete_object(path);			
			break;
		case TCAPI_READALL_CMD:
			cfg_readAll();
			break;
		default:
			break;
	}
	if (ret >= 0)
		sendMsg->retval = TCAPI_PROCESS_OK;
	
	return;
}




void send2CfgManager(tcapi_msg_t *sendMsg){

	char* node = sendMsg->node;
	char* attr = sendMsg->attr;
	char* val = sendMsg->value;
	char* action;

	action = get_action_string(sendMsg->op_cmd);
	
	if (is_tcapi_debug_on())	
		tcdbg_printf("[%d]: %s [%s] [%s] [%s] ",getpid(),action,node,attr,val); 

	if (is_cfg_ng_enable())
		send2CfgManager_to_2(sendMsg);
	else
	{	
		send2CfgManager_to_1(sendMsg);
		if (is_tcapi_debug_on())	
			tcdbg_printf(" ...... [%s] [%d] \n",val,sendMsg->retval); 

	}
}

#else

/*_____________________________________________________________________________
**      function name: send2CfgManager
**      descriptions:
**            Used to send message to cfg manager via unix socket.
**
**      parameters:
**            sendMsg:   Put the message that you want to send to cfg manager.

**
**      global:
**             None
**      return:
**             None
**
**      call:
**
**      revision:
**      1. Here 2008/5/7
**____________________________________________________________________________
*/
void
send2CfgManager(tcapi_msg_t *sendMsg){

	int sockfd=0, len=0;
	struct sockaddr_un remote;
	sendMsg->retval=TCAPI_CONN_ERR;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
		// frank delete for bug 9975 20110426
		//perror("connect");
		close(sockfd);
		return;
	}

	if (send(sockfd, sendMsg, sizeof(tcapi_msg_t), 0) == -1){
		perror("send");
		close(sockfd);
		return;
	}

	memset(sendMsg,0,sizeof(tcapi_msg_t));

	if ((recv(sockfd, sendMsg, sizeof(tcapi_msg_t), 0)) <= 0 ) {
		/*Failure to receive information from cfg manage*/
		perror("recv");
		close(sockfd);
		return;
	}
	close(sockfd);
}/*end send2CfgManager */

#endif

/*_____________________________________________________________________________
**      function name: tcapi_set
**      descriptions:
**            Update the value of nodes attribution.
**
**      parameters:
**            node:		node name
**            attr:		attribution name
**            value:	Specify the value that you want to update it.
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_set(char* node, char* attr, char* value){

	tcapi_msg_t sendMsg;

	memset(&sendMsg,0,sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_SET_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);
	strncpy(sendMsg.value, value, MAXLEN_TCAPI_MSG-1);
	strncpy(sendMsg.attr, attr, MAXLEN_ATTR_NAME-1);

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_set*/

/*_____________________________________________________________________________
**      function name: tcapi_unset
**      descriptions:
**            Remove the value of nodes attribution.
**
**      parameters:
**            node:      node name
**            attr:      attribution name
**
**      global:
**
**      return:
**            Success:        0
**            Otherwise:     -1
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_unset(char* node){

	tcapi_msg_t sendMsg;

	memset(&sendMsg,0,sizeof(sendMsg));
	/*Fill information*/
	sendMsg.op_cmd=TCAPI_UNSET_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_unset*/

/*_____________________________________________________________________________
**      function name: tcapi_get
**      descriptions:
**            Get the value of nodes attribution.
**
**      parameters:
**            node:      node name
**            attr:      attribution name
**            buf:    Put a buffer for cfg manager to put attribute value.
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_get(char* node, char* attr, char *buf){

	tcapi_msg_t sendMsg;

	memset(&sendMsg,0,sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_GET_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);
	strncpy(sendMsg.attr, attr, MAXLEN_ATTR_NAME-1);

	send2CfgManager(&sendMsg);
	sendMsg.value[sizeof(sendMsg.value) - 1] = '\0';
	if(sendMsg.retval !=TCAPI_CONN_ERR){
		strcpy(buf, sendMsg.value);
	}

	return sendMsg.retval;
}/*end tcapi_get*/


/*_____________________________________________________________________________
**      function name: tcapi_nget
**      descriptions:
**            Get the value of nodes attribution.
**
**      parameters:
**            node:      node name
**            attr:      attribution name
**            buf:    Put a buffer for cfg manager to put attribute value.
**            len:    buf length
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**             -5:          len is less or equal to 1
**
**      call:
**
**      revision:
**      1. Wei.Sun 2016/11/30
**____________________________________________________________________________
*/
int
tcapi_nget(char* node, char* attr, char *buf, int len){

	tcapi_msg_t sendMsg;

    if(len <= 1)
        return TCAPI_UNKNOW_ERR;
    
	memset(&sendMsg,0,sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_GET_CMD;
	strncpy(sendMsg.node, node,MAXLEN_NODE_NAME-1);
	strncpy(sendMsg.attr, attr,MAXLEN_ATTR_NAME-1);

	send2CfgManager(&sendMsg);
	if(sendMsg.retval !=TCAPI_CONN_ERR){
		strncpy(buf, sendMsg.value,len-1);
        buf[len-1] = '\0';
	}

	return sendMsg.retval;
}/*end tcapi_nget*/

/*_____________________________________________________________________________
**      function name: tcapi_staticGet
**      descriptions:
**            Get the value of nodes attribution.
**
**      parameters:
**            node:      node name
**            attr:      attribution name
**            buf:    Put a buffer for cfg manager to put attribute value.
**
**      global:
**
**      return:
**             Success:         0.
**             -1:                      The system isn't record this node information.
**             -2:                      The system isn't record this attribution information.
**             -3:                      Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_staticGet(char* node, char* attr, char *buf){

        tcapi_msg_t sendMsg;

        memset(&sendMsg,0,sizeof(sendMsg));

        /*Fill information*/
        sendMsg.op_cmd=TCAPI_STATIC_GET_CMD;
        strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);
        strncpy(sendMsg.attr, attr, MAXLEN_ATTR_NAME-1);

        send2CfgManager(&sendMsg);
	    sendMsg.value[sizeof(sendMsg.value) - 1] = '\0';
        if(sendMsg.retval !=TCAPI_CONN_ERR){
                strcpy(buf, sendMsg.value);
        }

        return sendMsg.retval;
}/*end tcapi_staticGet*/

/*_____________________________________________________________________________
**      function name: tcapi_show
**      descriptions:
**            Display the all value of nodes  attribution.
**
**      parameters:
**            node:      node name
**            buf:    string buffer to get node's information
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_show(char* node, char * buf){

	tcapi_msg_t sendMsg;

	memset(&sendMsg,0,sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_SHOW_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);

	send2CfgManager(&sendMsg);
	sendMsg.value[sizeof(sendMsg.value) - 1] = '\0';
	if(sendMsg.retval !=TCAPI_CONN_ERR){
		strcpy(buf, sendMsg.value);
	}

	return sendMsg.retval;
}/*end tcapi_show*/

/*_____________________________________________________________________________
**      function name: tcapi_show_file
**      descriptions:
**            Display the all value of nodes  attribution.
**
**      parameters:
**            node:      node name
**            buf:    string buffer to get node's information
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_show_file(char* node, char* path, int type){
	tcapi_msg_t sendMsg;

	memset(&sendMsg,0,sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_SHOW_ENHANCE_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);
	strncpy(sendMsg.path, path, MAXLEN_TCAPI_PATH_LEN-1);
	sendMsg.type = type;

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_show*/


/*_____________________________________________________________________________
**      function name: tcapi_commit
**      descriptions:
**            Cfg manager will be created a node configuration file and restart the node service.
**
**      parameters:
**            node:      node name
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_commit(char* node){

	tcapi_msg_t sendMsg;

	memset(&sendMsg,0,sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_COMMIT_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_commit*/

/*_____________________________________________________________________________
**      function name: tcapi_save
**      descriptions:
**            Cfg manager will be saved the system parameters into romfile block.
**
**      parameters:
**           None
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. Here 2008/5/5
**____________________________________________________________________________
*/
int
tcapi_save(void){

	tcapi_msg_t sendMsg;

	memset(&sendMsg, 0, sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_SAVE_CMD;

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_save*/

int
tcapi_save_raw(void){

	tcapi_msg_t sendMsg;

	memset(&sendMsg, 0, sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_SAVE_RAW_CMD;

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_save*/


/*_____________________________________________________________________________
**      function name: tcapi_read
**      descriptions:
**			  Cfg manager will reload system parameters from romfile block.
**
**      parameters:
**           None
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. xyzhu 2009/11/14
**____________________________________________________________________________
*/
int
tcapi_read(char* node){

	tcapi_msg_t sendMsg;

	memset(&sendMsg, 0, sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_READ_CMD;
	strncpy(sendMsg.node, node, MAXLEN_NODE_NAME-1);
	
	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_read*/

/*_____________________________________________________________________________
**      function name: tcapi_readAll
**      descriptions:
**			  Cfg manager will reload system parameters from romfile block.
**
**      parameters:
**           None
**
**      global:
**
**      return:
**             Success:		0.
**             -1:			The system isn't record this node information.
**             -2:			The system isn't record this attribution information.
**             -3:			Send tcapi message to cfg manager was failure.
**
**      call:
**
**      revision:
**      1. xyzhu 2009/11/14
**____________________________________________________________________________
*/
int
tcapi_readAll(void){

	tcapi_msg_t sendMsg;

	memset(&sendMsg, 0, sizeof(sendMsg));

	/*Fill information*/
	sendMsg.op_cmd=TCAPI_READALL_CMD;

	send2CfgManager(&sendMsg);
	return sendMsg.retval;
}/*end tcapi_readAll*/
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
/* jrchen modified 20100310*/
//	vsprintf(msg, fmt, args);
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
}/*end tcapi_save*/

#ifndef CFG2_FLAG
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
}/*end send2dnshost */

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
//	tcdbg_printf("\r\nenter sendmegq!");	
	oflag = 0666 | IPC_CREAT;
	mqkey = ftok(CWMP_MQ_FLAG_PATH,PROJID);
	mqid = msgget(mqkey,oflag);   
	if(mqid < 0)
	{
		tcdbg_printf("\r\n open message queue fail!");
		return -1;
	}
#if 0
	else
	{
		tcdbg_printf("\r\nopen message queue ok!");
	}
#endif	
	memset(&msg,0,sizeof(tc_msg_t));
	msg.mtype = type;
	memcpy((void*)&msg.msgtext,(void*)ptr,sizeof(cwmp_msg_t));
	ret = msgsnd(mqid,&msg,sizeof(cwmp_msg_t),flag);
	if(ret < 0)
	{
		tcdbg_printf("\r\n write message fail!");
		return -2;
	}
#if 0
	else
	{
		tcdbg_printf("\r\n write message ok!");
	}
#endif
	return 0;
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
	
	sock_fd = socket(PF_NETLINK, SOCK_RAW, nlk_type);
	if(sock_fd < 0) {
		tcdbg_printf("%s:nlksock create err:%s!\n", __FUNCTION__,strerror(errno));
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
	
	fd = nlksock_create(socktype, getpid(), group);
	if(fd< 0 ){
		return -1;
	}

	memset(&nl_daddr, 0, sizeof(nl_daddr));
	nl_daddr.nl_family = AF_NETLINK;
	nl_daddr.nl_pid = 0;
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
	pPktbuf = lenbuf;
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
	//tcdbg_printf("%s: send= %s!!\n", __FUNCTION__, buffer);
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
#endif

