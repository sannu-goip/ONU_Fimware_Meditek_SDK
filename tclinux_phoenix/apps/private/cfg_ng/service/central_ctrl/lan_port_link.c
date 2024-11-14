#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <cfg_cli.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_eth.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include "central_ctrl_common.h"
#include "device_alarm.h"
#include "lan_port_link.h"
#include "utility.h"
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
#include <syslog.h>
#endif
#include "svchost_evt.h"
#include "blapi_system.h"
#include <lan_port/lan_port_info.h>
/***************************************************************************/
#define LANPORTNUM      (4)
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
int g_iswlan_dev_leave = 0;
#endif

//static int xpon_752x_get_lan_status(int lanStatus[]);
#if 0
static int xpon_752x_lan_led(int port, int led_st);

/***************************************************************************/
/*
 *  port = 0~3 & led_st = 0: OFF; 1: BLINK; 2: ON
 */
static int xpon_752x_lan_led(int port, int led_st)
{
    char buf[20]= {0};
    FILE *xpon_lan_led_proc;

#if defined(TCSUPPORT_CUC_C5_SFU)
    FILE *gse_link_sta_proc;

    gse_link_sta_proc = fopen("/proc/tc3162/gsw_link_st", "r"); 
    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf), gse_link_sta_proc);
    fclose(gse_link_sta_proc);
#endif

    xpon_lan_led_proc = fopen("/proc/tc3162/led_752x", "w+");

    if (!xpon_lan_led_proc)
    {
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d %d", port, led_st);
    fwrite(buf, 1, strlen(buf), xpon_lan_led_proc);
    fclose(xpon_lan_led_proc);

    return 0;
}

static int xpon_752x_get_lan_status(int lanStatus[])
{
	FILE *fp = NULL;
	char string[16] = "";
	int portNum = 0;

	fp = fopen(ETH_PORT_STS, "r");
	if(fp != NULL)
	{
		memset(string, 0, sizeof(string));
		fgets(string, sizeof(string), fp);
		portNum = sscanf(string, "%d %d %d %d", lanStatus, lanStatus+1, lanStatus+2, lanStatus+3);
		fclose(fp);
		if(portNum == LANPORTNUM) return 0;
	}
	
	return -1;
}

void detect_lan_port_link(void)
{
	int prexponLanStatus[LANPORTNUM] = {0};
	int curxponLanStatus[LANPORTNUM] = {0};
	int j = 0;

	if(!xpon_752x_get_lan_status(curxponLanStatus))
	{
		for(j = 0; j < LANPORTNUM; j++)
		{
			if(curxponLanStatus[j] == 1 && prexponLanStatus[j] == 0) 
			{
				printf("Link State: LAN_%d up.\n", j+1);
				prexponLanStatus[j] = 1;
				xpon_752x_lan_led(j, 2);
				
#if defined(TCSUPPORT_CMCCV2)
				DelAlarmNumber("104006");
#endif
			} 
			else if(curxponLanStatus[j] == 0 && prexponLanStatus[j] == 1) 
			{
				printf("Link State: LAN_%d down.\n", j+1);
				prexponLanStatus[j] = 0;
				xpon_752x_lan_led(j, 0);
				

			}
		}		
	}
		
	return ;
}
#endif

/*************************************************************/
char *lan_port[LANPORTNUM] = 
{
    "LAN_1",
    "LAN_2",
    "LAN_3",
    "LAN_4",
};

char *link_speed[3] = 
{
    "10Mbps",
    "100Mbps",
    "1000Mbps",
};

char *link_duplex[2] =
{
    "Half Duplex",
    "Full Duplex",
};

void lan_down_action(struct ecnt_event_data *event_data)
{
	char buf[64];
	char itf_name[16];
    struct eth_event_data *data = NULL;
    data = (struct eth_event_data*)event_data;
#if defined(TCSUPPORT_CT_JOYME2)
	char node_name[32] = {0};
	char lan_port[32] = {0};
	char lan_status[32] = {0};
	char msg[128] = {0};
	int ret = 0;
#endif

#if 0
   tcdbg_printf("app %s link down!\n", lan_port[data->lan_port]);
#endif
	blapi_system_set_lan_led(data->lan_port, 0);

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
    AddAlarmNumber("104006");
	openlog("TCSysLog alarm", 0, LOG_LOCAL2);
	syslog(LOG_ALERT,"id:104006 lan down\n");
	closelog();
#endif

	memset(buf, 0, sizeof(buf));
	memset(itf_name, 0, sizeof(itf_name));
	snprintf(itf_name, sizeof(itf_name), "%s%d", M_ETHER_ITF_NAME_FORMAT, (data->lan_port+1));
	snprintf(buf, sizeof(buf), "1 %s", itf_name);
	doValPut("/proc/br_fdb/operator", buf);

#if defined(TCSUPPORT_CT_JOYME2)
	snprintf(node_name, sizeof(node_name), "root.sys.entry");
	snprintf(lan_port, sizeof(lan_port), "speed_lan%d", data->lan_port + 1);
	cfg_set_object_attr(node_name, lan_port, "0");

	snprintf(lan_status, sizeof(lan_status), "status_lan%d", data->lan_port + 1);
	cfg_set_object_attr(node_name, lan_status, "0");

	snprintf(msg, sizeof(msg), IGD_PATH"/Info/Network;"IGD_NAME".NetworkInfo;LAN%dStatus:y:0", data->lan_port + 1);
	ret = ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
#endif
    return;
}

void lan_up_action(struct ecnt_event_data *event_data)
{
    struct eth_event_data *data = NULL;
    data = (struct eth_event_data*)event_data;
#if defined(TCSUPPORT_CT_JOYME2)
	char node_name[32] = {0};
	char lan_port[32] = {0};
	char lan_status[32] = {0};
	char msg[128] = {0};
	int ret = 0;
#endif


#if 0
   tcdbg_printf("app %s link up at %s/%s.\n", lan_port[data->lan_port], link_speed[data->link_speed], link_duplex[data->link_duplex]);
#endif
	blapi_system_set_lan_led(data->lan_port, 2);

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME4)
    DelAlarmNumber("104006");
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	snprintf(node_name, sizeof(node_name), "root.sys.entry");
	snprintf(lan_port, sizeof(lan_port), "speed_lan%d", data->lan_port + 1);
	
	snprintf(lan_status, sizeof(lan_status), "status_lan%d", data->lan_port + 1);
	cfg_set_object_attr(node_name, lan_status, "1");
	
	if (data->link_speed == SPEED_10M)
		cfg_set_object_attr(node_name, lan_port, "10000");
	else if (data->link_speed == SPEED_100M)
		cfg_set_object_attr(node_name, lan_port, "100000");
	else if (data->link_speed == SPEED_1000M)
		cfg_set_object_attr(node_name, lan_port, "1000000");

	snprintf(msg, sizeof(msg), IGD_PATH"/Info/Network;"IGD_NAME".NetworkInfo;LAN%dStatus:y:1", data->lan_port + 1);
	ret = ecnt_event_send(ECNT_EVENT_DBUS, ECNT_EVENT_DBUS_INFORM, msg, strlen(msg) + 1);
#endif

    return;
}

void ether_wan_down_action(struct ecnt_event_data *event_data)
{
    struct eth_event_data *data = NULL;
    char val[8];
	
    data = (struct eth_event_data*)event_data;

#if 0
   tcdbg_printf("app ether wan link down data->port=%d.\n",data->lan_port);
#endif
    memset(val, 0, sizeof(val));
    snprintf(val, sizeof(val), "down");
    cfg_set_object_attr(WANINFO_COMMON_NODE, "EthernetState", "down");

    cfg_send_event(EVT_PON_EXTERNAL,EVT_ETHER_WAN_STATUS,(void *)(val),strlen(val));

    return;
}

void ether_wan_up_action(struct ecnt_event_data *event_data)
{
    struct eth_event_data *data = NULL;
    char val[8];
    
    data = (struct eth_event_data*)event_data;

#if 0
   tcdbg_printf("app ether wan link up at %s/%s data->port=%d.\n",link_speed[data->link_speed], link_duplex[data->link_duplex],data->lan_port);
#endif
    memset(val,0,sizeof(val));
    snprintf(val,sizeof(val),"up");
    cfg_set_object_attr(WANINFO_COMMON_NODE, "EthernetState", "up");
    
    cfg_send_event(EVT_PON_EXTERNAL,EVT_ETHER_WAN_STATUS,(void *)(val),strlen(val));

    return;
}

static void get_lan_status(int lanStatus[])
{
#if defined(TCSUPPORT_CT_PON)
	getPonLanStatus2(lanStatus);
#endif
}


#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
int is_portal_on(void)
{
	char tmp[4] = {0};
	
	if(cfg_get_object_attr(PORTAL_ENTRY_NODE, "Enable", tmp, sizeof(tmp)) > 0 ){
		if (!strcmp(tmp, "1")){
			return SUCCESS;
		}
		else{
			return FAIL;
		}
	}
	else{
		return FAIL;
	}	
}
#endif


#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
void check_lanip_exist(void)
{
	FILE *fp;
	uint32_t yiaddr;
	uint8_t matchid;
	char str[64] = {0};
	int res = 0;

	struct in_addr client;
	
	fp = fopen("/tmp/var/tmp/ip_vendor.conf","r");
	if (fp == NULL)
	{
		return;
	}
	
	while (fread(&yiaddr, sizeof(uint32_t), 1, fp) == 1)
	{	
		res = fread(&matchid, sizeof(uint8_t), 1, fp);
		if(getArpReply("br0", yiaddr, 0) != 0)
		{
			client.s_addr = yiaddr;
			memset(str, 0, sizeof(str));
			snprintf(str, sizeof(str),"iptables -t nat -D PRE_PORTAL -s %s -j RETURN", inet_ntoa(client));
			system(str);
			conntrack_delete(AF_INET, IPPROTO_TCP, yiaddr, 80);
		}
	}
	fclose(fp);
}

int inet_open_rawsock(char *iface, int protocol)
{
	int sock;

	if((sock = socket(PF_PACKET, SOCK_RAW, htons(protocol))) < 0)
	{
		perror("socket");
		return -1;
	}

    
	if(iface != NULL)
	{
		struct ifreq ifr;
		struct sockaddr_ll sll;

		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);    

		if ( ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
		{
			perror("ioctl(SIOCGIFINDEX)");
			close(sock);
			return -1;
		}

		memset(&sll, 0, sizeof(sll));
		sll.sll_family = AF_PACKET;
		sll.sll_ifindex = ifr.ifr_ifindex;
		sll.sll_protocol = htons(protocol);

		if ( bind(sock, (struct sockaddr *) &sll, sizeof(sll)) == -1)
		{
			perror("bind()");
			close(sock);
			return -1;
		}
	}

    return sock;
}

int inet_get_ifaddr(char *iface, unsigned char* mac, unsigned int *ip)
{
	int sock;
	struct ifreq ifr;

	if((sock = inet_open_rawsock(iface, ETH_P_IP)) < 0){
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);

	if (mac != NULL)
	{
		if (get_br0_ifaddr(sock, mac) == -1)
		{
			close(sock);
			return -1;			
		}
	}

	if (ip != NULL)
	{
		if ( ioctl(sock, SIOCGIFADDR, &ifr) < 0 )
		{
			perror("ioctl(SIOCGIFADDR)");
			close(sock);
			return -1;
		}
		memcpy((char *)ip, ifr.ifr_addr.sa_data+2, sizeof(*ip));
	}
	close(sock);
	return 0;
}


int forge_arp_request(arp_packet_t *pkt, unsigned char *smac,
                              unsigned int sip, unsigned int tip)
{
    memset(pkt->ethhdr.tmac, 0xFF, ETH_ALEN);
    memcpy(pkt->ethhdr.smac, smac, ETH_ALEN);
    pkt->ethhdr.type     = htons(ETH_P_ARP);
    pkt->arphdr.hwtype  = htons(HW_TYPE_ETHERNET);
    pkt->arphdr.prot    = htons(ETH_P_IP);
    pkt->arphdr.hwlen   = ETH_ALEN;
    pkt->arphdr.protlen = 4;   /* ipv4 */
    pkt->arphdr.opcode  = htons(ARP_OPCODE_REQUEST);
    memcpy(pkt->arphdr.smac, smac, ETH_ALEN);;
    pkt->arphdr.sip     = sip;
    memset(pkt->arphdr.tmac, 0, ETH_ALEN);
    pkt->arphdr.tip     = tip;

    return 0;
}


static int get_arp_reply(int sock, arp_packet_t *pkt, int timeout)
{
	fd_set rdfds;
	struct timeval tv;

	memset(pkt, 0, sizeof(arp_packet_t));
	tv.tv_sec = timeout;
	tv.tv_usec = 500000;//5000;

	FD_ZERO(&rdfds);
	FD_SET(sock, &rdfds);

	switch(select(sock + 1, &rdfds, NULL, NULL, &tv))
	{
	    case -1:
	        if (EINTR == errno)
	            break;
	        perror("select");
	    case 0:
	        /* timeout */
	        return -1;
	    default: 
	        if(FD_ISSET(sock, &rdfds))
	        {
	            if(recv(sock, pkt, sizeof(*pkt), 0) < 0 )
	            {
	                perror("recv");
	                return -1;
	            }
	        }
	}

    return 0;
}

int getArpReply(char *iface, unsigned int tip, int timeout)
{
	int sock;
	arp_packet_t pkt;
	unsigned int sip;
	unsigned char smac[ETH_ALEN];
	int ret = 0;

	if((sock = inet_open_rawsock(iface, ETH_P_ARP)) < 0){
		return -1;
	}
	if(inet_get_ifaddr(iface, smac, &sip) < 0){
		ret = -1;
		goto err;
	}
    
	forge_arp_request(&pkt, smac, sip, tip);

	if(send(sock, &pkt, sizeof(pkt), 0) < 0)
	{
		perror("send");
		ret = -1;
		goto err;
	}

	if(get_arp_reply(sock, &pkt, timeout) < 0)
	{
		perror("recv");
		ret = -1;
		goto err;
	}
 	if(pkt.arphdr.opcode == htons(ARP_OPCODE_REPLY) && pkt.arphdr.sip == tip){
		ret = 0;
 	}
	else{
		ret = -1;
	}
err:
	close(sock);
	return ret;
}
#endif

void flush_arp()
{
	char cmd[64] = {0};
	sprintf(cmd, "/usr/bin/ip neigh flush dev br0");	
	system(cmd);
}


int checkBlackList(char *mac)
{
	char nodeName[32] = {0};
	char ActiveMac[4] = {0};
	char FilterType[10] = {0};
	char strMac[20] = {0};
	char totalMacNum[4] = {0};
	char BlackOrWhite[10] = {0};
	int i = 0, maxNum = 0;
	
	if(cfg_get_object_attr(IPMACFILTER_COMMON_NODE, "ActiveMac", ActiveMac, sizeof(ActiveMac)) <= 0)
	{
		return 0;
	}
	if(strcmp(ActiveMac, "1") != 0)
	{
		return 0;
	}
	if(cfg_get_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType",FilterType, sizeof(FilterType)) <= 0)
	{
		return 0;
	}
	if(strcmp(FilterType, "Mac") != 0)
	{
		return 0;
	}
	if(cfg_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeMac", BlackOrWhite, sizeof(BlackOrWhite)) <= 0)
	{
		return 0;	
	}
	if(strcmp(BlackOrWhite, "Black") != 0)
	{
		return 0;
	}
	
	cfg_get_object_attr(IPMACFILTER_NODE, "mac_num", totalMacNum, sizeof(totalMacNum));
	maxNum = atoi(totalMacNum) + 1;
	for(i = 1; i < maxNum; i++)
	{
		snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, i);
		if(cfg_get_object_attr(nodeName, "MacAddr", strMac, sizeof(strMac)) > 0)
		{
			if(!strcasecmp(strMac, mac))
			{
				return 1;
			}
		}
	}
	return 0;
}


int notifyBrHostMacs(int portmask, int is_online)
{
	FILE *fp = NULL;
	int lanStatus[4] = {0};
	int i_port = 0, port_state = 0;
	char f_buf[128]={0}, mac_s[24] = {0}, s_port[16] = {0};

	get_lan_status(lanStatus);
	fp = fopen("/proc/br_fdb_host/stb_list", "r");
	if ( NULL == fp )
		return -1;

	bzero(f_buf, sizeof(f_buf));
	while ( fgets(f_buf, sizeof(f_buf), fp) )
	{
		sscanf(f_buf, "%[^=]=%s", s_port, mac_s);
		i_port = atoi(s_port);
		if ( i_port >= 1 && i_port <= 4 )
		{
			i_port -= 1;
			port_state = lanStatus[i_port];

			if ( portmask & (1 <<i_port) )
			{
				if(checkBlackList(mac_s))
				{
					sendEventMessage(4, mac_s,i_port+1);	
				}else{
					sendEventMessage((port_state ? 1 : 0), mac_s,i_port+1);
				}
			}
		}
	}

	fclose(fp);
	return 0;
}


void check_flush_arp()
{
	static int preLanStatus[LANPORTNUM] = {0};
	int curLanStatus[LANPORTNUM] = {0};
	int i, flushflag = 0;
#if defined(TCSUPPORT_CT_JOYME)
	int port_mask = 0;
	static int lan_online_wait = 0;
#endif
#if defined(TCSUPPORT_CMCCV2)
	port_mask = 0;
#endif

	/* Check lan link status */
	get_lan_status(curLanStatus);
	for(i = 0; i < LANPORTNUM; i++)
	{
		if((preLanStatus[i] == 1) && (curLanStatus[i] == 0))
		{
			flushflag = 1;
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
			if(is_portal_on() == SUCCESS)
			{
				check_lanip_exist();
			}
#endif
#if defined(TCSUPPORT_CMCCV2)
			port_mask |= (1 << i);
#endif
		}
#if defined(TCSUPPORT_CMCCV2)
		else if( 0 == preLanStatus[i]
			&& 1 == curLanStatus[i] )
		{
			flushflag = 2;
			port_mask |= (1 << i);
		}

		if ( 2 == flushflag )
		{
			/* wait for 2 seconds when LAN up */
			if ( ++ lan_online_wait <= 2 )
			{
				flushflag = 0;
				port_mask= 0;
				break;
			}
		}
#endif
		preLanStatus[i] = curLanStatus[i];
	}
	
#if defined(TCSUPPORT_CT_PORTAL_MANAGEMENT)
	if ( 1 == flushflag || 1 == g_iswlan_dev_leave )
	{
		if ( SUCCESS == is_portal_on() )
		{
			check_lanip_exist();
		}
		g_iswlan_dev_leave = 0;
	}
#endif

	if(flushflag == 1)
	{
		flushflag = 0;
#if !defined(TCSUPPORT_CT_JOYME)
		flush_arp();
#endif
#if defined(TCSUPPORT_CMCCV2)
		notifyBrHostMacs(port_mask, 0);
#endif
	}
#if defined(TCSUPPORT_CMCCV2)
	else if( 2 == flushflag )
	{
		flushflag = 0;
		lan_online_wait = 0;
		notifyBrHostMacs(port_mask, 1);
	}
#endif
	return;
}


