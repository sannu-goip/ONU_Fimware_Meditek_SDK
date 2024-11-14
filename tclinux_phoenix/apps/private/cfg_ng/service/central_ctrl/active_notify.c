#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include "active_notify.h"
#include "oid.h"
#include <linux/wireless.h>
#include <linux/version.h>
#include <linux/sysinfo.h>
#include "cfg_cli.h"
#include "utility.h"
#include "cfg_msg.h"

#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif

#if defined(TCSUPPORT_CT_PON)	

extern int WscOOBSeted;
#if defined(TCSUPPORT_CMCCV2)
void updateFile(char *Mac, int actType);
#endif
int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
	memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
	while (RTA_OK(rta, len)) {
		if (rta->rta_type <= max)
			tb[rta->rta_type] = rta;
		rta = RTA_NEXT(rta,len);
	}
	if (len)
		fprintf(stderr, "!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
	return 0;
}

const char *ll_addr_n2a(unsigned char *addr, int alen, char *buf, int blen)
{
	int i;
	int l;

	if (alen == 4) {
		return inet_ntop(AF_INET, (void *)addr, buf, blen);
	}
	if (alen == 16) {
		return inet_ntop(AF_INET6, (void *)addr, buf, blen);
	}
	l = 0;
	for (i=0; i<alen; i++) {
		if (i==0) {
			snprintf(buf+l, blen, "%02x", addr[i]);
			blen -= 2;
			l += 2;
		} else {
			snprintf(buf+l, blen, ":%02x", addr[i]);
			blen -= 3;
			l += 3;
		}
	}
	return buf;
}


int get_devPortInfo(char * macstr ,char *buf)
{
	FILE *fp = NULL;
	char tempbuf[512] = {0},port[4] = {0};
	char mac[17] = {0}; 
	int lanStatus[4] = {0};
	int pNum = 0;

	if(macstr == NULL || buf == NULL){		
		tcdbg_printf("[%s:%d]: macstr == NULL || buf == NULL\n", __FUNCTION__,__LINE__);
		return -1;
	}

	fp = fopen(BR_STB_LIST_PATH,"r");
	if(fp == NULL){
		tcdbg_printf("[%s:%d]: open %s fail\n", __FUNCTION__,__LINE__,BR_STB_LIST_PATH);
		return -1;
	}
	
	while(feof(fp) == 0 ){
		fgets(tempbuf,sizeof(tempbuf),fp);
		sscanf(tempbuf, "%[^=]",port);
		sscanf(tempbuf, "%*[^=]=%s",mac);
		if(strcmp(macstr,mac) == 0) {
			/*mac find*/
			strcpy(buf,port);
			pNum = atoi(port);
			if (pNum >= 1 && pNum <= 4) {
				getPonLanStatus2(lanStatus);
				if (lanStatus[pNum-1] == 0) { 
					fclose(fp);
					/* this lan is disconnect */
					tcdbg_printf("\r\nget_devinfo():this lan is disconnect, port = %d", atoi(port));
					return 1;
	             }else{
	             	fclose(fp);
					return 2;
				 }
            }
#if defined(TCSUPPORT_MULTI_USER_ITF)
			else if((SSID1_PORT_MASK <= atoi(port)) && (SSIDAC8_PORT_MASK >= atoi(port)))
#else
			else if((atoi(port) >= 5 && atoi(port) <= 8)
#if defined(TCSUPPORT_WLAN_AC)
			|| (atoi(port) >= 9 && atoi(port) <= 12)
#endif
			)
#endif 
			{
            	fclose(fp);
				/*wifi port*/
				return 3;
			}else{
				/*mac find, but not lan or wifi port*/
				fclose(fp);
				return -3;
			}

        }
	}
	fclose(fp);

	/*mac not find*/
	return -2;
}

/*if mac match,return the entry index;
**if mac not match, return the first unused entry index*/
int checkLanInfoMacExist(int* index, char* mac,int port, int check_port){	
	char strNodeName[64];	
	char buftemp[32] = {0};
	char bufport[8] = {0};
	int i = 0;
	int firstUnusedEntry = -1;

	
	for(i=0;i<MAX_DEVNUM_RULE;i++)
	{
		memset(strNodeName, 0, sizeof(strNodeName));
		memset(buftemp, 0, sizeof(buftemp));
		snprintf(strNodeName, sizeof(strNodeName), LANINFO_ENTRY_NODE, i+1);
		if(cfg_get_object_attr(strNodeName, LANINFO_ACTIVE_ATTR, buftemp, sizeof(buftemp)) > 0
			&& !strcmp(buftemp,"1"))		
		{
			memset(buftemp, 0, sizeof(buftemp));
			cfg_get_object_attr(strNodeName, LANINFO_MACADDR_ATTR, buftemp, sizeof(buftemp));
			memset(bufport, 0, sizeof(bufport));
			cfg_get_object_attr(strNodeName, LANINFO_PORT_ATTR, bufport, sizeof(bufport));
			if(!strcmp(buftemp,mac) && (check_port == 0 || port == atoi(bufport))){
				/*mac match,return the entry index*/
				*index = i;
				return 0;
			}	
		}else if(firstUnusedEntry == -1){
			firstUnusedEntry = i;
		}
	}

	*index = firstUnusedEntry;
	
	return -1;
}

/*if ip changed,return 1;must use checkLanInforMacExit for first to get index*/
int checkLanInfoIPChanged(int index, char* ip){	
	char strNodeName[64];	
	char buftemp[32] = {0};
	
	memset(strNodeName, 0, sizeof(strNodeName));
	memset(buftemp, 0, sizeof(buftemp));
	snprintf(strNodeName, sizeof(strNodeName), LANINFO_ENTRY_NODE, index + 1);
	if(cfg_get_object_attr(strNodeName, LANINFO_IPADDR_ATTR, buftemp, sizeof(buftemp)) > 0)		
	{	
		if(!strcmp(buftemp,ip))
			return 0;	
	}
	return 1;
}


/*delete lanInfo by mac*/
int delLanInfoNodeInfo(char* mac, int flag,int portid, int check_port){	
	char strNodeName[64];	
	char buftemp[32] = {0};
	int index = 0;	
	int devnum = 0;
	int port = 0, ssidNum = 0, i = 0;
	char nodeName[48] = {0}, nodeattr[32] = {0};
#if defined(TCSUPPORT_CMCCV2)
	cwmp_msg_t message;
	long type = 1;//tr69 must be 1
	int mq = VALUE_CHANGED; //value change type 
	int msgFlag = IPC_NOWAIT;//0;

	memset(&message,0,sizeof(cwmp_msg_t));
	message.cwmptype = mq;
#endif

	if(!checkLanInfoMacExist(&index, mac,portid,check_port)){
		/*mac exist,unset entry*/
		cfg_get_object_attr(LANINFO_COMMON_NODE, LANINFO_NUMBER_ATTR, buftemp, sizeof(buftemp));
		devnum = atoi(buftemp);

		memset(strNodeName, 0, sizeof(strNodeName));		
		snprintf(strNodeName, sizeof(strNodeName), LANINFO_ENTRY_NODE, index + 1);
		
		cfg_get_object_attr(strNodeName, LANINFO_PORT_ATTR, buftemp, sizeof(buftemp));
		port = atoi(buftemp);
		if (flag == 1 && (port >= 5 && port <= 12)) {
			snprintf(nodeattr, sizeof(nodeattr), "NUM%d", port-5);
			memset(buftemp, 0, sizeof(buftemp));
			cfg_get_object_attr(WIFIMACTAB_COMMON_NODE, nodeattr, buftemp, sizeof(buftemp));
			ssidNum = atoi(buftemp);
			if (ssidNum > 0)
			{
				for(i = 0; i < ssidNum; i++)
				{
					snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, port-4, i+1);
					memset(buftemp, 0, sizeof(buftemp));
					
					cfg_get_object_attr(nodeName, "MAC", buftemp, sizeof(buftemp));
					
					if(strncasecmp(mac,buftemp, sizeof(buftemp)-1) == 0)
					{
						tcdbg_printf("\r\n[%s]line=%d, mac=[%s] is still in mactab, no delete the node\n",__FUNCTION__, __LINE__, mac);
						return -1;
					}
				}
			}
		}

		cfg_delete_object(strNodeName);
		memset(buftemp, 0, sizeof(buftemp));
		snprintf(buftemp, sizeof(buftemp), "%d", devnum-1);
		cfg_set_object_attr(LANINFO_COMMON_NODE, LANINFO_NUMBER_ATTR, buftemp);
#if defined(TCSUPPORT_CMCCV2)
		updateFile(mac, 2);
#endif
	}
#if defined(TCSUPPORT_CMCCV2)
	sendmegq(type,&message,msgFlag);
#endif
		
	return 0;
}


/*set LanInfo entry node*/
int setLanInfoNodeInfo(int index, char* ip, char* mac){	
	char strNodeName[64];	
	char buftemp[64] = {0};
	int i = 0;	
#if defined(TCSUPPORT_CMCCV2)
	struct sysinfo info;
    	unsigned long long sec = 0;
#endif
	char 
	laninfo_attr[][20]=
	{
		{"ipflag"},
		{"portflag"},
		{"devtypeflag"},
		{"conntypeflag"},
		{""},
	};

	memset(strNodeName, 0, sizeof(strNodeName));
	snprintf(strNodeName, sizeof(strNodeName), LANINFO_ENTRY_NODE, index + 1);
	if(cfg_obj_query_object(strNodeName, NULL, NULL) <= 0)
	{
		if(cfg_obj_create_object(strNodeName) < 0)
		{
			tcdbg_printf("\r\n[%s] create %s fail\n",__FUNCTION__, strNodeName);
			return -1;
		}
	}
	cfg_set_object_attr(strNodeName, LANINFO_ACTIVE_ATTR, "1");
	for(i=0;strlen(laninfo_attr[i])!=0;i++)
	{
		if(cfg_get_object_attr(strNodeName, laninfo_attr[i], buftemp, sizeof(buftemp))<=0){
			/*attr not exist, set the default value*/
			cfg_set_object_attr(strNodeName, laninfo_attr[i], "0");
		}
	}

	if ( NULL != ip )
	cfg_set_object_attr(strNodeName, LANINFO_IPADDR_ATTR, ip);
	if ( NULL != mac )
	cfg_set_object_attr(strNodeName, LANINFO_MACADDR_ATTR, mac);

	memset(buftemp, 0, sizeof(buftemp));
//	if(get_devinfo(mac,buftemp,1) == 0)
	if(get_devPortInfo(mac,buftemp) > 0)
		cfg_set_object_attr(strNodeName, LANINFO_PORT_ATTR, buftemp);
	else		
		cfg_set_object_attr(strNodeName, LANINFO_PORT_ATTR, "0");
#if defined(TCSUPPORT_CMCCV2)
	sysinfo(&info);
	sec = info.uptime;

	snprintf(buftemp, sizeof(buftemp), "%llu", sec);
	cfg_set_object_attr(strNodeName, "upTime", buftemp);
	updateFile(mac, 1);
#endif
	return 0;
}

int updateLanInfoByMac(char* mac, int port,int update_type){
	FILE *fp = NULL;	
	char cmd[64]={0};
	char buf[MAX_BUF_SIZE]={0};	
	char buftemp[32] = {0};
	char ip[40]={0},macAddr[20]={0},lladdr[15]={0},stale[15]={0};
	int ret = -1,ret2 = -1;
	int index = -1;
	int devnum = 0;
	int need_update = 0;
#if defined(TCSUPPORT_CMCCV2)
	cwmp_msg_t message;
	long type = 1;//tr69 must be 1
	int mq = VALUE_CHANGED; //value change type 
	int msgFlag = IPC_NOWAIT;//0;

	memset(&message,0,sizeof(cwmp_msg_t));
	message.cwmptype = mq;
#endif
	snprintf(cmd, sizeof(cmd), "/usr/bin/ip neigh show dev br0 > %s", IFINFO_ETHCMD_PATH);
	system(cmd);
	
	fp=fopen(IFINFO_ETHCMD_PATH, "r");	
	if(fp == NULL)
		return -1;


	cfg_get_object_attr(LANINFO_COMMON_NODE, LANINFO_NUMBER_ATTR, buftemp, sizeof(buftemp));
	devnum = atoi(buftemp);

#if defined(TCSUPPORT_CMCCV2)
	if ( 1 == update_type && NULL != mac ) /* wifi mactable update */
	{
		if ( checkLanInfoMacExist(&index, mac,port,0) )
		{
			/* mac not exist,new item */
			devnum++;
			setLanInfoNodeInfo(index, NULL, mac);
			need_update = 1;
		}
		else
		{
			setLanInfoNodeInfo(index, NULL, mac);
		}
		
		goto updateLanInfo_End;
	}
#endif

	while (fgets(buf,MAX_BUF_SIZE,fp))
	{
		ret = sscanf(buf, "%s %s %s %s",ip,lladdr,macAddr,stale);
		ret2 = get_devPortInfo(mac,buftemp);
		
		if(ret2 == 1){
			/*lan disable, del lan*/					
			delLanInfoNodeInfo(mac, 0,port,0);

#if defined(TCSUPPORT_CMCCV2)
			/* dev number double reduce when devnum--*/
			fclose(fp);
			return 0;
#else
			need_update = 1;
			devnum--;			
			break;
#endif

		}else if(ret2  < 0) {
#if defined(TCSUPPORT_CMCCV2)
			break;
#else
			/*mac not find*/
			continue;
#endif
		}

		if(strcasecmp(mac,macAddr)){
			/*mac not match, check the next mac*/			
			continue;
		}

		if((ret == 4) && (inet_addr(ip) != INADDR_NONE))
		{
			if (0 == strncmp(ip, "169", 3)) {
				tcdbg_printf("\r\n[%s] ip[%s] error!\n",__FUNCTION__, ip);
				continue;
			}
			
			if(checkLanInfoMacExist(&index, mac,port,0)){
#if defined(TCSUPPORT_CMCCV2)
				if ( 2 == update_type ) /* wifi arp update */
					goto updateLanInfo_End;
#endif

			/*mac not exist,new item*/
				devnum++;
				setLanInfoNodeInfo(index,ip,mac);
				need_update = 1;
			}else{
				if(checkLanInfoIPChanged(index,ip)){
					setLanInfoNodeInfo(index,ip,mac);
					need_update = 2;
				}				
			}	
			break;
		}
	}
	
updateLanInfo_End:
	fclose(fp);
	if(need_update == 1)
	{
		snprintf(buftemp, sizeof(buftemp), "%d", devnum);
		cfg_set_object_attr(LANINFO_COMMON_NODE, LANINFO_NUMBER_ATTR, buftemp);
	}
#if defined(TCSUPPORT_CMCCV2)
	if(need_update){
		sendmegq(type,&message,msgFlag);
	}
#endif
	return 0;
}
void get_landevice_type(char *Mac,char *DeviceType,char *option60)
{
	int i;
	char num[5] = {0};
	char mac[40] = {0};
	char nodename[30] = {0};
	char devicetype[20] = {0};
	char optionvalue[128] = {0};
	
	if(Mac == NULL || DeviceType == NULL || option60 == NULL)
		return;

	if(cfg_get_object_attr(DHCPLEASE_NODE, "LeaseNum", num, sizeof(num)) <= 0)
		return;
	
	for(i = 0; i < atoi(num); i++)
	{
		snprintf(nodename,sizeof(nodename)-1,DHCPLEASE_ENTRY_NODE,i+1);
		cfg_get_object_attr(nodename, "MAC", mac, sizeof(mac));
		if(strcasecmp(mac,Mac) == 0)
		{
			cfg_get_object_attr(nodename, "DeviceType", devicetype, sizeof(devicetype));
			cfg_get_object_attr(nodename, "OptionValue", optionvalue, sizeof(optionvalue));
			strcpy(DeviceType,devicetype);
			strcpy(option60,optionvalue);
			return;
		}
	}
	return;
}

#if defined(TCSUPPORT_CMCCV2)
int getDeviceName(char *mac, char *devname, int size)
{
	int  j = 0, maxNum = 0;
	char tNum[4] = {0};
	char nodeName[30] = {0};
	char tmpMac[20] = {0};
	char dhcpname[64] = {0};

	if (strncmp(devname, "anonymous",size-1) != 0 && devname[0] != '\0') {
		/*devname is not anonymous, or not null*/
		return -1;
	}

	cfg_get_object_attr("LanHost", "LeaseNum", tNum, sizeof(tNum));
	maxNum = atoi(tNum);
	
	for(j = 0; j < maxNum; j++){
		snprintf(nodeName, sizeof(nodeName), "LanHost_Entry%d", j);
		if(cfg_get_object_attr(nodeName, "MAC", tmpMac, sizeof(tmpMac)) == 0 && strlen(tmpMac) > 0){
			if(0 == strcmp(tmpMac, mac)) {
				/*mac match*/
				cfg_get_object_attr(nodeName, "HostName", dhcpname, sizeof(dhcpname));
				
				if (dhcpname[0] !='\0' && dhcpname[0] !=' ')
				{	
					/*dhcp name is not null , update to devname*/
					strncpy(devname, dhcpname, size-1);
					return 0;
				}
			}
		}
	}
	return -1;
}
void updateFile(char *Mac, int actType){
	char tmpMac[20] = {0};
	char buf[512] = {0};
	int tmpTime = 0;	
	FILE *fp = NULL;
	char devname[64] = {0};
	char osname[64] = {0};
	char brandname[64] = {0};
	int flag = 0;
	long len = 0,linelen = 0;
	int isintable = 0;
	char devicetype[20] = {0};
	char option60[128] = {0};

	if(NULL == Mac)
	{
		return;
	}
	if (access("/usr/osgi/config/laninfo.conf", 0) == 0){
		system("cp /usr/osgi/config/laninfo.conf /tmp/");
	}
	get_landevice_type(Mac,devicetype,option60);
	fp = fopen("/tmp/laninfo.conf", "r+");
	if(fp == NULL){
		fp = fopen("/tmp/laninfo.conf", "w");
		if(fp == NULL){
			return;
		}
		tmpTime = 0;
		flag =1;
		if (getDeviceName(Mac, &devname, sizeof(devname)) != 0)
		strncpy(devname,"anonymous", sizeof(devname));
		strncpy(osname,"anonymous",sizeof(osname));
		strncpy(brandname,"anonymous",sizeof(brandname));
		fprintf(fp,"%20s %10lu %5d %64s %64s %64s %20s %128s\n",Mac,tmpTime,flag,			
			devname,osname,brandname,devicetype,option60);
	}
	else{
		while(fgets(buf, sizeof(buf), fp)){
			linelen = strlen(buf);
			len = len + linelen;
			sscanf(buf, "%s %d %d %s %s %s", tmpMac, &tmpTime, &flag, devname, osname, brandname);
			if(!strcmp(Mac, tmpMac)){
				if(actType == 1){
					flag = 1;
					getDeviceName(Mac, &devname, sizeof(devname));
				}else{
					flag = 0;
				}
				len -= strlen(buf);
				if( 0 == fseek(fp,len,SEEK_SET))
				{
					fprintf(fp,"%20s %10d %5d %64s %64s %64s %20s %128s\n",tmpMac,tmpTime,flag,					
						devname,osname,brandname,devicetype,option60);
				}
				isintable = 1;
				break;
			}
		}
		if(isintable == 0){
			flag = 1;
			
			if (getDeviceName(Mac, &devname, sizeof(devname)) != 0)
			strncpy(devname,"anonymous", sizeof(devname));
			strncpy(osname,"anonymous",sizeof(osname));
			strncpy(brandname,"anonymous",sizeof(brandname));
			fprintf(fp,"%20s %10d %5d %64s %64s %64s %20s %128s\n",Mac,tmpTime,flag,				
					devname,osname,brandname,devicetype,option60);
		}
	}
	fclose(fp);
	system("cp /tmp/laninfo.conf /usr/osgi/config/");
	return;
}
#endif

/* active notify handler */
int active_notify_handler(void)
{
	nlk_msg_t nlkmsg;
	int ret = -1;
	int sockfd = -1;
	int i=0;
	int ret_code = 0;
	char mac[20] = {0};
	int port = -1;

	sockfd = nlksock_create(NLKMSG_SOCK_TYPE, 
							pthread_self() << 16 | getpid(), 
							NLKMSG_GRP_DEVINFO);
	if ( sockfd < 0 )
	{
		tcdbg_printf( "%s: nlksock_create fail\n", __FUNCTION__);
		return -1;
	}

	bzero(&nlkmsg, sizeof(nlk_msg_t));
	while ( (ret = recv(sockfd, &nlkmsg, sizeof(nlk_msg_t), 0))> 0 )
	{
		/*tcdbg_printf( "[%s]nlkmsg.eventType=%d\n", __FUNCTION__, nlkmsg.eventType);
		tcdbg_printf( "[%s]payload=%s\n", __FUNCTION__, nlkmsg.data.payload);*/

		switch ( nlkmsg.eventType )
		{
#if defined(TCSUPPORT_CMCCV2)
			case MSG_DEVMAC_ONLINE:
				sscanf(nlkmsg.data.payload, "%s %d",mac,&port);
				updateLanInfoByMac(mac,port, 0);
				snprintf(nlkmsg.data.fmsg.bundlename, MAX_BUNDLE_NAME_LEN, "ALL");
				snprintf(nlkmsg.data.fmsg.buf, NLK_LOCAL_PAYLOAD, "{\"Event\":\"LAN_DEV_ONLINE\", \"MacAddr\":\"%s\"}", mac);
				break;
			case MSG_DEVMAC_WLAN_ONLINE:
				sscanf(nlkmsg.data.payload, "%s %d",mac,&port);
				updateLanInfoByMac(mac,port+5, 1);
				snprintf(nlkmsg.data.fmsg.bundlename, MAX_BUNDLE_NAME_LEN, "ALL");
				snprintf(nlkmsg.data.fmsg.buf, NLK_LOCAL_PAYLOAD, "{\"Event\":\"WLAN_DEV_ONLINE\", \"MacAddr\":\"%s\"}", mac);
				break;
			case MSG_DEVMAC_OFFLINE:																
				sscanf(nlkmsg.data.payload, "%s %d",mac,&port);
				delLanInfoNodeInfo(mac, 0,port,0);
				snprintf(nlkmsg.data.fmsg.bundlename, MAX_BUNDLE_NAME_LEN, "ALL");
				snprintf(nlkmsg.data.fmsg.buf, NLK_LOCAL_PAYLOAD, "{\"Event\":\"LAN_DEV_OFFLINE\", \"MacAddr\":\"%s\"}", mac);
				break;
			case MSG_DEVMAC_WLAN_OFFLINE:
				sscanf(nlkmsg.data.payload, "%s %d",mac,&port);
				delLanInfoNodeInfo(mac, 1,port+5,1);
				snprintf(nlkmsg.data.fmsg.bundlename, MAX_BUNDLE_NAME_LEN, "ALL");
				snprintf(nlkmsg.data.fmsg.buf, NLK_LOCAL_PAYLOAD, "{\"Event\":\"WLAN_DEV_OFFLINE\", \"MacAddr\":\"%s\"}", mac);
				break;
			case MSG_DEVMAC_REFUSED:
				sscanf(nlkmsg.data.payload, "%s %d",mac,&port);
				snprintf(nlkmsg.data.fmsg.bundlename, MAX_BUNDLE_NAME_LEN, "ALL");
				snprintf(nlkmsg.data.fmsg.buf, NLK_LOCAL_PAYLOAD, "{\"Event\":\"LAN_DEV_REFUSED\", \"MacAddr\":\"%s\"}", mac);
				break;
			case MSG_DEVMAC_WLAN_REFUSED:
				sscanf(nlkmsg.data.payload, "%s %d",mac,&port);
				snprintf(nlkmsg.data.fmsg.bundlename, MAX_BUNDLE_NAME_LEN, "ALL");
				snprintf(nlkmsg.data.fmsg.buf, NLK_LOCAL_PAYLOAD, "{\"Event\":\"WLAN_DEV_REFUSED\", \"MacAddr\":\"%s\"}", mac);
				break;
#endif
			default:	
				break;
		}
#if defined(TCSUPPORT_CMCCV2)
		nlkmsg.eventType = MSG_NOTIFY2MOBILE;
		ret_code = nlkmsg_send(NLKMSG_GRP_MOBILE, NLK_LOCAL_PAYLOAD, &nlkmsg);
		if ( ret_code < 0 )
			tcdbg_printf("\n nlkmsg_send fail for NLKMSG_GRP_MOBILE  retcode =[%d] \n", ret_code);
#endif

	}
	close(sockfd);
	return 0;
}


void arp_notify_handler()
{	
	struct nlmsghdr *h;
	int status;
	char resp[1024];
	struct rtattr * tb[NDA_MAX+1];
	struct ndmsg *r;
	char abuf[256];
	char lladdr[64] = {0};
	int wlanIndex = -1;
	int ret = -1;
	int sockfd = -1;
	int i=0;
#if defined(TCSUPPORT_CMCCV2)
	char portidx[32] = {0};
#endif
	
	sockfd = nlksock_create_2(NETLINK_ROUTE, 0, RTMGRP_NEIGH);

	if(sockfd < 0){
		tcdbg_printf( "%s: nlksock_create fail\n", __FUNCTION__);
		return -1;
	}

	while((status = recv(sockfd, resp, sizeof(resp), 0))> 0){
		for (h = (struct nlmsghdr *)resp; NLMSG_OK(h, status);
		     h = NLMSG_NEXT(h, status)) {
			switch(h->nlmsg_type){
				case RTM_NEWNEIGH:	
				case RTM_DELNEIGH:
					r = NLMSG_DATA(h);
					parse_rtattr(tb, NDA_MAX, NDA_RTA(r), h->nlmsg_len - NLMSG_LENGTH(sizeof(*r)));
					if (tb[NDA_LLADDR]) {	
						/*get Mac*/
						ll_addr_n2a(RTA_DATA(tb[NDA_LLADDR]),
					      RTA_PAYLOAD(tb[NDA_LLADDR]),lladdr, sizeof(lladdr));
						
						/*get IP*/
						inet_ntop(r->ndm_family, RTA_DATA(tb[NDA_DST]), abuf, sizeof(abuf));
						
						if(h->nlmsg_type == RTM_NEWNEIGH){
#if defined(TCSUPPORT_CT_JOYME)														
#if defined(TCSUPPORT_CMCCV2)
							if ( 3 == get_devPortInfo(lladdr, portidx) )
							{
								/* wifi port only use wifi mac table to update */
								updateLanInfoByMac(lladdr, 0, 2);
								continue;
							}
#endif

							/*update LanInfo*/
							updateLanInfoByMac(lladdr, 0, 0);
#endif
						}
						else{	
#if defined(TCSUPPORT_CT_JOYME)																			
							delLanInfoNodeInfo(lladdr, 1, 0, 0);
#endif
						}

					}else{
						//tcdbg_printf( "%s:tb[NDA_LLADDR] not exist\n", __FUNCTION__);
					}
					break;
				default:
					//tcdbg_printf( "%s: h->nlmsg_type=%d\n", __FUNCTION__,h->nlmsg_type);
					break;
			}
		}
	}
	close(sockfd);	
	return;
}
#endif


/* notify service init */
int 
notify_service_init(void)
{
	int res = 0;
#if defined(TCSUPPORT_CMCCV2)
	char is_arp_notifiy_off[16] = {0};
	int is_arp_off = 0;
#endif

#if defined(TCSUPPORT_CT_PON)	
	pthread_t thread_id;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED); 

	/* create notify service */
	if (pthread_create(&thread_id, &thread_attr, (void *)active_notify_handler, NULL) != 0) {
		tcdbg_printf("\r\n[%s] notify service init fail!\n",__FUNCTION__);
		return -1;
	}

#if defined(TCSUPPORT_CMCCV2)
	if ( 0 < cfg_get_object_attr(CMCCINFO_ENTRY3_NODE, "ArpNfyOff", is_arp_notifiy_off, sizeof(is_arp_notifiy_off))
		&& 0 == strcmp(is_arp_notifiy_off, "Yes") )
		is_arp_off = 1;

	if ( 0 == is_arp_off )
	{
#endif
		/* create notify service */
		if (pthread_create(&thread_id, &thread_attr, (void *)arp_notify_handler, NULL) != 0) {
			tcdbg_printf("\r\n[%s] arp_notify init fail!\n",__FUNCTION__);
			return -1;
		}
#if defined(TCSUPPORT_CMCCV2)
	}
#endif
#endif
	return 0;
}


