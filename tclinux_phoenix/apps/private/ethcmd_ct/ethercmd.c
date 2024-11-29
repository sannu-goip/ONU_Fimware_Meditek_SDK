/************************************************************************
 *
 *	Copyright (C) 2009 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <linux/mii.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#ifdef ETHCMD_DBG
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "libcmd.h"
#include "libmii.h"
#include "ethercmd.h"
#include "ether.h"


#define CR_AHB_BASE       	0xBFB00000
#define CR_RSTCTRL2    		(CR_AHB_BASE + 0x834)
#define EPHY_RST    					(1<<24)
#define ESW_RST    						(1<<23)
#define FE_RST    						(1<<21)

static int doEth(int argc, char *argv[], void *p);
static int doAddwan(int argc, char *argv[], void *p);
static int doDelwan(int argc, char *argv[], void *p);
static int doPhyMediaType(int argc, char *argv[], void *p);
static int doPhyReset(int argc, char *argv[], void *p);
static int doVlanpt(int argc, char *argv[], void *p);
static int doLanchip(int argc, char *argv[], void *p);
static int doStatus(int argc, char *argv[], void *p);
static int doIgmpsnoop(int argc, char *argv[], void *p);
static int doHelp(int argc, char *argv[], void *p);
static int doVersion(int argc, char *argv[], void *p);
#ifdef ETHCMD_DBG
static int doEthcmdDbg(int argc, char *argv[], void *p);
void initEthcmdDbg(void);
#endif
static int doTCMiiRead(int argc, char *argv[], void *p);
static int doTCMiiWrite(int argc, char *argv[], void *p);
static int doGSWRead(int argc, char *argv[], void *p);
static int doGSWWrite(int argc, char *argv[], void *p);
/*VLAN related ci-cmd*/
static int doVlan(int argc, char *argv[], void *p);
static int doVlanSetActive(int argc, char *argv[], void *p);
static int doVlanSetPvid(int argc, char *argv[], void *p);
static int doVlanSetVid(int argc, char *argv[], void *p);
static int doVlanDisp(int argc, char *argv[], void *p);
/*end vlan related ci-cmd*/
#ifdef ETHCMD_EXTENSION
static int doPortBinding(int argc, char *argv[], void *p);
#endif

#if 1  //LAN_ETHERNET_INTERFACE
static int doSwitchPortNum(int argc, char *argv[], void *p);
#endif

void cmdHelp(void);
static int ethup_addwan(char *szBaseDev,int iVid,char *szVDev);
static int setLanchip(char* type, int portid);
static int setVport(char* type);
static int getEtherPort(void);
static int ethcmdQueryPortCount(int portid);
static u16 ethcmdGetMediaOption(char *option);
static int showMediaSetting(int phy_id);
static findNegoPortID(void);

extern swic_api_t swic;

extern int lan_portmap[6];

cmds_t Cmds[] = {
	{LAN_IF,		doEth,		0x12,	0,	NULL},
	{"addwan",	doAddwan,		0x12,	3,	"addwan vid devname vlan-priority"},
	{"delwan",	doDelwan,		0x12,	1,	"delwan devname"},
	{"miir",		doTCMiiRead,		0x12,	2,	"<phyAddr> <reg>"},
	{"miiw",		doTCMiiWrite,		0x12,	3,	"<phyAddr> <reg> <value>"},
	{"gswr",		doGSWRead,		0x12,	1,	"<reg>"},
	{"gsww",		doGSWWrite,		0x12,	2,	"<reg> <value>"},
	{"pbr",			doGSWRead,		0x12,	1,	"<reg>"},
	{"pbw",			doGSWWrite,		0x12,	2,	"<reg> <value>"},
	{"vlan",		doVlan,		0x12,	0,	NULL},
	#ifdef ETHCMD_EXTENSION
	/*Extend ci-cmd */
	#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
	{"portbinding",doPortBinding,		0x12,	4,	HELP_PORTBINDING_INFO},
	#else
	{"portbinding",doPortBinding,		0x12,	4,	HELP_PORTBINDING},
	#endif
	/*end extend ci-cmd*/
	#endif
	{"version",	doVersion,	0x12,	0,	"Display ethphxcmd version."},
	{"help",		doHelp,		0x12,	0,	"ethphxcmd help."},
	#ifdef ETHCMD_DBG
	{"dbg",		doEthcmdDbg,		0x12,	0,	""},
	#endif
	{NULL,		NULL,		0x10,	0,	NULL},	
};

static const cmds_t ethCmds[] = {
#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
	{"media-type",	doPhyMediaType,	0x12,	0,	HELP_MEDIA_TYPE_INFO},
	{"reset",			doPhyReset,		0x12,	0,	HELP_RESET_INFO},
	{"lanchip",		doLanchip,		0x12,	0,	HELP_LANCHIP_INFO},
#else
	{"media-type",	doPhyMediaType,	0x12,	0,	HELP_MEDIA_TYPE},
	{"reset",			doPhyReset,		0x12,	0,	HELP_RESET},
	{"lanchip",		doLanchip,		0x12,	0,	HELP_LANCHIP},	
#endif
	{"vlanpt",		doVlanpt,		0x12,	1,	HELP_VLANPT},	
	{"status",		doStatus,		0x12,	0,	HELP_STATUS},
	{"igmpsnoop",		doIgmpsnoop,	0x12,	1,	HELP_IGMPSNOOP},
#if 1 //LAN_ETHERNET_INTERFACE
	{"portNum",		doSwitchPortNum,	0x12,	0,	HELP_SWITCH_PORTNUM},
#endif
	{NULL,	NULL,		0x10,	0,	NULL},	
};

static const cmds_t vlanCmds[] = {
	{"active",		doVlanSetActive,	0x12,	1,	HELP_VLAN_ACTIVE},
	{"pvid",		doVlanSetPvid,	0x12,	2,	HELP_VLAN_PVID},
	{"vid",		doVlanSetVid,	0x12,	5,	HELP_VLAN_VID},
	{"disp",		doVlanDisp,	0x12,	0,	NULL},
	{NULL,		NULL,		0x10,	0,	NULL},
};

#ifdef ETHCMD_EXTENSION
static struct portMap tcPortMap[]={
		{"e0", "eth0.1"},
		{"e1", "eth0.2"},
#if !defined(TCSUPPORT_CT_2PWIFI) && !defined(TCSUPPORT_2PORTS)
		{"e2", "eth0.3"},
		{"e3", "eth0.4"},
#endif
		{"p0", "nas0"},
		{"p1", "nas1"},
		{"p2", "nas2"},
		{"p3", "nas3"},
		{"p4", "nas4"},
		{"p5", "nas5"},
		{"p6", "nas6"},
		{"p7", "nas7"},
		{"w0", "ra0"},
		{"w1", "ra1"},
		{"w2", "ra2"},
		{"w3", "ra3"},
		{"u0", "usb0"},
		{NULL, NULL},
};

#define MAX_IFS	(sizeof(tcPortMap)/sizeof(tcPortMap[0])) 
#define PM_START_IF_ARG	3
#endif

static const char *media_names[] = {
	"10baseT", "10baseT-FD", "100baseTx", "100baseTx-FD", "100baseT4",
	"Flow-control", 0,
};

#ifdef ETHCMD_DBG
u8 ethcmdDbg=0;
#define ETHCMDDBG_FILE	"/var/tmp/ethdbg"
#define ETH_DEBUG(fmt,args...) if(ethcmdDbg) printf("\n %s %d: " fmt, __FUNCTION__ ,__LINE__, ##args)
#endif
/*___________________________________________________________________
**      function name: cmdHelp
**      descriptions:
**         Ethcmd ci-cmd helpful message.
**
**      parameters:
**         None
**  
**      global:
**           Cmds
**             
**      return:
**         None
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void cmdHelp(){
	const cmds_t *cmdp=NULL;
	const cmds_t *ethcmdp=NULL;
	
	printf("Usage: ethphxcmd <eth0|addwan|delwan> <command> [arg]\n\n");
	for(cmdp = Cmds;cmdp->name != NULL;cmdp++){
		if(!strcmp(LAN_IF, cmdp->name)){
			for(ethcmdp = ethCmds;ethcmdp->name != NULL;ethcmdp++){
				printf("  %s %s\n\n", ethcmdp->name, ethcmdp->argc_errmsg);
			}
		}
		else if(strcmp("help", cmdp->name)!=0){
			printf("  %s %s\n\n", cmdp->name, cmdp->argc_errmsg);
		}
	}
	#ifdef SUPPORT_ATP_WANETH
	ethup_wanHelp();
	#endif
	printf("  ethphxcmd help\n");
}/*end cmdHelp*/
/*___________________________________________________________________
**      function name: doEth
**      descriptions:
**         Register ethcmd subcmd.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethCmds
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doEth(int argc, char *argv[], void *p){
	return subcmd(ethCmds, argc, argv, p);
}/*end doEth*/
/*___________________________________________________________________
**      function name: doAddwan
**      descriptions:
**         Add vlan virtual interface for wan interface. 
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           ETH_DEBUG
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doAddwan(int argc, char *argv[], void *p){
	int iVid = 0;
	int iVlanPriority = 0;
	int iRet = -1;
	char szCmd[ETHUP_CMDSIZE];
	char szBaseDev[ETHUP_NAMESIZE];
	char szVDev[ETHUP_NAMESIZE];
	
	memset(szBaseDev,0,ETHUP_NAMESIZE);
	memset(szVDev,0,ETHUP_NAMESIZE); 
	memset(szCmd,0,ETHUP_CMDSIZE);
	
	iVlanPriority = atoi(argv[3]);
	/*if create nas0 do nothing "ethcmd addwan 65535 nas0 f "*/
	if(MAX_VID_VALUE == atoi(argv[1])){
	    printf("\n create wan without vlan id ok \n");
	    return 0;
	}

	if((NULL == argv[3]) || ( ETHUP_NAMESIZE < strlen(argv[2])) ||  \
	    ETHUP_MAXPRORITY < iVlanPriority || ETHUP_MINPRORITY > iVlanPriority)
	{
	    printf("\n ERROR: %s,%s wrong parameter .%d \n ",__FILE__,__FUNCTION__,__LINE__);
	    return -1;
	}

	memcpy(szBaseDev,ETHUP_WAN,ETHUP_NAMESIZE);
	memcpy(szVDev, argv[2], ETHUP_NAMESIZE);
	iVid = atoi(argv[1]);

	if (VLAN_GROUP_ARRAY_LEN-2 < iVid || 0 > iVid  \
	    || ETHUP_NAMESIZE < strlen(szBaseDev) || (ETHUP_NAMESIZE < strlen(szVDev))){
		printf("\n ERROR: Addwan device error. %s,%s,%d \n ",__FILE__,__FUNCTION__,__LINE__);    
		printf("\n ERROR: %s,%s,%d \n ",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}
	
	sprintf(szCmd,"vconfig add %s %d", szVDev, iVid);
	#ifdef ETHCMD_DBG
	ETH_DEBUG("%s",szCmd);
	#endif
	system(szCmd);
	
	sprintf(szCmd,"ifconfig %s up",szVDev);
	#ifdef ETHCMD_DBG
	ETH_DEBUG("%s",szCmd);
	#endif
	system(szCmd);

	/*now deal with prority*/
	sprintf(szCmd,"vconfig set_egress_map %s.%d 0 %d",szVDev,iVid,iVlanPriority);
	#ifdef ETHCMD_DBG
	ETH_DEBUG("%s",szCmd);
	#endif
	system(szCmd);

	sprintf(szCmd,"vconfig set_ingress_map %s.%d 0 %d",szVDev,iVid,iVlanPriority);
	#ifdef ETHCMD_DBG
	ETH_DEBUG("%s",szCmd);
	#endif
	system(szCmd);

	printf("\n ethphxcmd add wan interface %s ok \n",szVDev);
	return 0;
}/*end doAddwan*/
/*___________________________________________________________________
**      function name: doDelwan
**      descriptions:
**         Delete vlan virtual interface for wan interface. 
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doDelwan(int argc, char *argv[], void *p){
	char szCmd[MAX_CMD_LEN];
	char wanIf[ETHUP_NAMESIZE];
	int i=0;
	int find=ETHCMD_ERR;
	memset(szCmd,0,sizeof(szCmd));
	memset(wanIf,0,sizeof(wanIf));

	for(i=0;i<MAX_PVCS; i++){
		sprintf(wanIf,"nas%d",i);
		if(0 == strcmp(argv[1], wanIf)){
			find=0;
			break;
		}
	}
	if(find==0){/*send to wan site is no vlan header*/
		sprintf(szCmd,"ifconfig %s 0.0.0.0",wanIf);
		system(szCmd);
	}
	else{
		sprintf(szCmd,"ifconfig %s down", argv[1]);
		system(szCmd);
		sleep(1);
		sprintf(szCmd,"vconfig rem %s", argv[1]);
		system(szCmd);
	}
	printf("\n ethphxcmd delete wan interface %s ok \n",argv[1]);
	return 0;
}/*end doDelwan*/
/*___________________________________________________________________
**      function name: findNegoPortID
**      descriptions:
**         get the phy negotiation port id
**
**      parameters:
**         None
**         
**      global:
**         None
**             
**      return:
**         i: the phy negotiation port id
**         -1:fail
**	    
**      call:
**           getSwcfg
**           mdio_read
**         
**      
**      revision:
**      1. xflu 2011/07/21
**_______________________________________________________________
*/

int findNegoPortID(void)
{
	int physid = 0;
	int i = 0;
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();
	
	for(i=0; i<MAX_PHY_NUM; i++){
		physid =mdio_read(swcfg->skfd, swcfg->ifr, i, MII_PHYSID1);
		if(physid == 0)
			physid = mdio_read(swcfg->skfd, swcfg->ifr, i, MII_PHYSID2);
		if( physid == PHY_COMPANYID ){
			return i;
		} 
	}
	
	return -1;
	
}
/*___________________________________________________________________
**      function name: doPhyMediaType
**      descriptions:
**         Set/query Ethernet PHY link state.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           ethcmdGetMediaOption
**           showMediaSetting
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doPhyMediaType(int argc, char *argv[], void *p){
	int portid =0;
	u16 val=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();

	int flag = 0;
	int singlePID = 0;
	singlePID = findNegoPortID();

	if(((argc==4) &&(strcmp(argv[3],"swap")))||((argc==5) &&(!strcmp(argv[4],"swap")))){/*Set media-type */
		val=ethcmdGetMediaOption(argv[1]);
		if(val==ERR_MEDIAY_TYPE){
			#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
			printf(HELP_MEDIA_TYPE, argv[0], vlanCap->max_port_no-1,vlanCap->max_port_no-1);
			#else
			printf("  %s %s\n\n", argv[0], HELP_MEDIA_TYPE);
			#endif
			return -1;
		}
		else{
			portid=atoi(argv[3]);
			if((strcmp(argv[2],ETHCMD_PORT)==0)\
			&&((portid==0) ||(ethcmdQueryPortCount(portid)==SWIC_PORTS_NUM))){
				if(argc == 5 || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE)){//swap
					mdio_write(swcfg->skfd, swcfg->ifr, vlanCap->max_port_no - portid -1, MII_BMCR, val);
					flag = 1;
				}else{
				#ifdef  TCSUPPORT_CPU_RT63365	
					portid = lan_portmap[portid];
					if (portid == -1){
						printf("Invalied port: pordid is -1\n");
						return -1;
					}
				#endif	
					if(vlanCap->max_port_no==SINGLE_PORT)
						mdio_write(swcfg->skfd, swcfg->ifr, singlePID, MII_BMCR, val);
					else
					mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_BMCR, val);
				}
				/*waiting the ethernet link up*/
				sleep(ETHUP_WAIT_PHY_READY);
			}
			else{
				#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
				printf(HELP_MEDIA_TYPE, argv[0], vlanCap->max_port_no-1,vlanCap->max_port_no-1);
				#else
				printf("  %s %s\n\n", argv[0], HELP_MEDIA_TYPE);
				#endif
				return -1;
			}
		}
	}
	else if(argc==3 ||((argc==4) &&(!strcmp(argv[3],"swap"))) ){/*Query media-type */
		portid=atoi(argv[2]);
		if((strcmp(argv[1],ETHCMD_PORT)!=0)\
			||(ethcmdQueryPortCount(portid)<=portid)){
			printf("interface %s is Ethernet Switch, please use port [0-%d] argument\n", swcfg->ifr.ifr_name, (vlanCap->max_port_no-1));
			#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
			printf(HELP_MEDIA_TYPE, argv[0], vlanCap->max_port_no-1,vlanCap->max_port_no-1);
			#else
			printf("  %s %s\n\n", argv[0], HELP_MEDIA_TYPE);
			#endif
			return -1;
		}
		else{
			if(argc==4 || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE)){
				//because argv[3] must be swap here!!!
				flag = 1;
			}
		}
	}
	else{
		printf("interface %s is Ethernet Switch, please use port [0-%d] argument\n", swcfg->ifr.ifr_name, (vlanCap->max_port_no-1));
		#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
		printf(HELP_MEDIA_TYPE, argv[0], vlanCap->max_port_no-1,vlanCap->max_port_no-1);
		#else
		printf("  %s %s\n\n", argv[0], HELP_MEDIA_TYPE);
		#endif
		return -1;
	}
	/*The single port's portid==0, otherwise the switch is not limit it*/
	if((portid==0)||(vlanCap->max_port_no==SWIC_PORTS_NUM))
	{
		if(flag)
			showMediaSetting(vlanCap->max_port_no - portid - 1);
		else{
			if(vlanCap->max_port_no==SINGLE_PORT)
				showMediaSetting(singlePID);
		else
			showMediaSetting(portid);
	}
	}
	return 0;
}/*end doPhyMediaType*/
/*___________________________________________________________________
**      function name: doPhyReset
**      descriptions:
**        Ethernet PHY Reset to use mii reigster to control.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           showMediaSetting
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doPhyReset(int argc, char *argv[], void *p){
	int portid =0;
	int val=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();
	int singlePID = 0;
	singlePID = findNegoPortID();

	int flag = 0;
	if(argc==3||((argc==4) &&(!strcmp(argv[3],"swap")))){/*PHY Reset*/
		portid=atoi(argv[2]);
		if((strcmp(argv[1],ETHCMD_PORT)!=0)\
			||(ethcmdQueryPortCount(portid)<=portid)){
			#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
			printf(HELP_RESET, argv[0], vlanCap->max_port_no-1,vlanCap->max_port_no-1);
			#else
			printf(HELP_RESET, argv[0], vlanCap->max_port_no-1);
			#endif
			return -1;
		}
		else{
			if(argc==4 || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE)){
				flag = 1;
				}
		
		}
	}
	else{
		printf("interface %s is Ethernet Switch, please use port [0-%d] argument\n", swcfg->ifr.ifr_name, (vlanCap->max_port_no-1));
		#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
		printf(HELP_RESET, argv[0], vlanCap->max_port_no-1,vlanCap->max_port_no-1);
		#else
		printf(HELP_RESET, argv[0], vlanCap->max_port_no-1);
		#endif
		return -1;
	}
	
	/*The single port's portid==0, otherwise the switch is not limit it*/
	if((portid==0)||(vlanCap->max_port_no==SWIC_PORTS_NUM)){
		/*Specify the PHY Reset*/
		if(flag)
		{
			val = mdio_read(swcfg->skfd, swcfg->ifr, vlanCap->max_port_no - portid - 1, MII_BMCR);
			val |= BMCR_RESET; // set bit15=1
			mdio_write(swcfg->skfd, swcfg->ifr, vlanCap->max_port_no - portid - 1, MII_BMCR, val);
		}
		else
		{
			if(vlanCap->max_port_no==SINGLE_PORT){
				val = mdio_read(swcfg->skfd, swcfg->ifr, singlePID, MII_BMCR);
				val |= BMCR_RESET; // set bit15=1
				mdio_write(swcfg->skfd, swcfg->ifr, singlePID, MII_BMCR, val);
			}
			else{
			val = mdio_read(swcfg->skfd, swcfg->ifr, portid, MII_BMCR);
			val |= BMCR_RESET; // set bit15=1
			mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_BMCR, val);
		}
		}
		/*Waitfor PHY link up and ready*/
		sleep(ETHUP_WAIT_PHY_READY);
		if(flag)
			showMediaSetting(vlanCap->max_port_no - portid - 1);
		else{
			if(vlanCap->max_port_no==SINGLE_PORT)
				showMediaSetting(singlePID);
		else
			showMediaSetting(portid);
	}
	}
	return 0;
}/*end doPhyReset*/
/*___________________________________________________________________
**      function name: doVlanpt
**      descriptions:
**        Ethernet PHY Reset to use mii reigster to control.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doVlanpt(int argc, char *argv[], void *p){
	char cmd[MAX_CMD_LEN]={0};
	int retval=0;
	int i=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();

	int flag = 0;
#ifdef  TCSUPPORT_CPU_RT63365	
	int portmap, wan_port=-1;
#endif	
	/*single port*/
	if(vlanCap->max_port_no ==SINGLE_PORT){
		printf("eth0 is not Switch chip.\n");
		return 0;
	}

	if(!strcmp(argv[1], ETHCMD_ENABLE)){
		for(i=START_VLAN_VID;i<(vlanCap->max_port_no+START_VLAN_VID);i++){
			/*Create 4 virtual vlan interface*/
			sprintf(cmd, "/userfs/bin/vconfig add eth0 %d", i);
			system(cmd);
			/*Let virtual vlan ethernet interface up*/
			sprintf(cmd, "ifconfig eth0.%d 0.0.0.0 up", i);
			system(cmd);
			/*Binding virtual vlan ethernet interface into br0 interface*/
			sprintf(cmd, "brctl addif br0 eth0.%d", i);
			system(cmd);
		}
		/*Remove the eth0 ethernet interface from br0 interface*/
		system("brctl delif br0 eth0");
		
		/*Active the switch vlan function*/
		if((swic.vlan_active!=NULL) && (swic.vlan_pvid!=NULL)&&(swic.vlan_vid!=NULL)){
			swic.vlan_active(ACTIVED);
	
#if 1//ndef VPORT			
			if (!(swcfg->flag & STAG_TO_VTAG)) {
			for(i=0; i<vlanCap->max_port_no; i++){
				/*Set the default PVID for each port, Default pvid value is start at 2.*/
					if((argc == 3 && !strcmp(argv[2],"swap")) || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE))
				{
					swic.vlan_pvid(i, START_VLAN_VID+vlanCap->max_port_no -i -1);
					flag = 1;
				}
				else
				{
					swic.vlan_pvid(i, i+START_VLAN_VID);
				}
				
				if(swcfg->swic_id==IP175C){
					/*Set the default vlan table rules*/
					if(flag)
						swic.vlan_vid(i+START_VLAN_VID, ACTIVED, START_VLAN_VID+vlanCap->max_port_no -i -1, (1<<i), 0x0);
					else
						swic.vlan_vid(i+START_VLAN_VID, ACTIVED, i+START_VLAN_VID, (1<<i), 0x0);
				}
				else{
					/*Set the default vlan table rules*/
					if(flag)
						swic.vlan_vid(i, ACTIVED, START_VLAN_VID+vlanCap->max_port_no -i -1, (1<<i), 0x0);	
					else
						swic.vlan_vid(i, ACTIVED, i+START_VLAN_VID, (1<<i), 0x0);	
				}
			}
#ifdef	TCSUPPORT_CPU_RT63365
				//bind all lan port to cpu port
				swic.vlan_vid(i, ACTIVED, i+START_VLAN_VID, ((1<<0) | (1<<1) | (1<<2) | (1<<3)), 0x0);
#endif	
		}
			else{
#ifdef  TCSUPPORT_CPU_RT63365
				portmap = (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5);
				wan_port = get_wan_port();
				if(wan_port == -1){
					//bind all lan port to cpu port
					swic.vlan_vid(0, ACTIVED, START_VLAN_VID, portmap, 0x0);
				}else{
					portmap &= ~(1<<wan_port);
					//bind all lan port to cpu port
					swic.vlan_vid(0, ACTIVED, START_VLAN_VID, portmap, 0x0);
					//bind etherwan port to cpu port. wan port vid 5
					swic.vlan_vid(1, ACTIVED, 5, (1<<wan_port), 0x0);
				}
#endif						
		}
#endif			
		}
		sprintf(cmd, "echo \"1\" > %s", VPORT_FILE);
		system(cmd);
	}
	else if(!strcmp(argv[1], ETHCMD_DISABLE)){
		for(i=START_VLAN_VID;i<(vlanCap->max_port_no+START_VLAN_VID);i++){
			/*Let virtual vlan ethernet interface down*/
			sprintf(cmd, "ifconfig eth0.%d down", i);
			system(cmd);
			/*Remove the virtual vlan ethernet interface from bridge interface*/
			sprintf(cmd, "brctl delif br0 eth0.%d", i);
			system(cmd);
			/*Delete 4 virtual vlan interface*/
			sprintf(cmd, "vconfig rem eth0.%d", i);
			system(cmd);
		}
		/*Binding the eth0 ethernet interface into br0 interface*/
		system("brctl addif br0 eth0");
		sprintf(cmd, "echo \"0\" > %s", VPORT_FILE);
		system(cmd);
		/*Disabled the switch vlan function*/
		if(swic.vlan_active!=NULL){
			swic.vlan_active(DEACTIVED);
		}
	}
	else if(!strcmp(argv[1], ETHCMD_QUERY)){
		printf("\nVport Number is:%d", vlanCap->max_port_no);
	}
	else{		
		retval=-1;
	}
	return retval;
}/*end doVlanpt*/
/*___________________________________________________________________
**      function name: doLanchip
**      descriptions:
**        Enable/disable ethernet port to use mii regsiter to control it.(Use power
**        down).
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           setLanchip
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doLanchip(int argc, char *argv[], void *p){
	int portid =0;
	int val=0;
	int i=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	struct swconfig *swcfg=NULL;
	int flag = -1;
	int singlePID = 0;
		
	singlePID = findNegoPortID();	
	swcfg=getSwcfg();

	if(argc==2){/*LAN Chip 4ports-switch Enable/Disable*/
		if(vlanCap->max_port_no == SWIC_PORTS_NUM){
		for(i=0;i<vlanCap->max_port_no; i++){
			if(setLanchip(argv[1], i)<0){
				break;
			}	
		}
		}
		else{/*for single port just set Lan chip port 1*/
			setLanchip(argv[1], singlePID);
			i = 1;
		}
		if(i !=vlanCap->max_port_no){
			#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
			printf(HELP_LANCHIP, argv[0],vlanCap->max_port_no-1);
			#else
			printf("  %s %s\n\n", argv[0], HELP_LANCHIP);
			#endif
			return -1;
		}
	}
	else if((argc==4)||((argc == 5) && !strcmp(argv[4],"swap"))){/*Specify the phy port Enable/Disable*/
		portid=atoi(argv[3]);
		if((strcmp(argv[2],ETHCMD_PORT)==0)\
		&&((portid==0) ||(ethcmdQueryPortCount(portid)==SWIC_PORTS_NUM))){
			if(argc == 5 || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE))
				flag = setLanchip(argv[1], vlanCap->max_port_no - portid - 1);
			else{
			#ifdef  TCSUPPORT_CPU_RT63365	
				portid = lan_portmap[portid];
				if (portid == -1){
					printf("Invalied port: pordid is -1\n");
					goto err_exit;
				}
			#endif	
				if(vlanCap->max_port_no==SINGLE_PORT)
					flag = setLanchip(argv[1], singlePID);
				else
					flag = setLanchip(argv[1], portid);
}
			if(flag<0){
				goto err_exit;
			}
		}
	}
	else{
		goto err_exit;
	}
	return 0;

err_exit:
	
	printf("interface %s is Ethernet Switch, please use port [0-%d] argument\n", swcfg->ifr.ifr_name, (vlanCap->max_port_no-1));
	#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
	printf(HELP_LANCHIP, argv[0],vlanCap->max_port_no-1);
	#else
	printf("  %s %s\n\n", argv[0], HELP_LANCHIP);
	#endif
	return -1;
}/*end doLanchip*/

/*___________________________________________________________________
**      function name: doStatus
**      descriptions:
**        Get the link status of ethernet port.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           mdio_read
**           ethcmdQueryPortCount
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doStatus(int argc, char *argv[], void *p){
	int i=0;
	int val=0;
	int portid=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	struct swconfig *swcfg=NULL;
	int singlePID = 0;

	singlePID = findNegoPortID();		
	swcfg=getSwcfg();
	if((argc==1) ||(argc==2 &&(!strcmp(argv[1],"swap")))){/*Get the Status of port 0~port 3 */
		if(vlanCap->max_port_no == SWIC_PORTS_NUM){
		for(i=0;i<vlanCap->max_port_no; i++){
				if(argc==2 || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE))
				val=mdio_read(swcfg->skfd, swcfg->ifr,vlanCap->max_port_no-i-1, MII_BMSR);
			else
				val=mdio_read(swcfg->skfd, swcfg->ifr, i, MII_BMSR);
			printf("%d ", ((val & BMSR_LSTATUS) ?1:0));
		}
		}
		else{/*fot single port*/
			val=mdio_read(swcfg->skfd, swcfg->ifr, singlePID, MII_BMSR);
			printf("%d ", ((val & BMSR_LSTATUS) ?1:0));
		}
		printf("\r\n");
	}
	else if(argc==3||(argc==4 &&(!strcmp(argv[1],"swap")))){/*Specify the phy port number*/
		portid=atoi(argv[2]);
		if((strcmp(argv[1],ETHCMD_PORT)==0)\
		&&((portid==0) ||(ethcmdQueryPortCount(portid)==SWIC_PORTS_NUM))){
			if(argc==4 || /*swcfg->reverse_flag == 1*/(swcfg->flag & PORT_REVERSE)){
				val=mdio_read(swcfg->skfd, swcfg->ifr, vlanCap->max_port_no -portid -1, MII_BMSR);
			}
			else{
				if(vlanCap->max_port_no==SINGLE_PORT)
					val=mdio_read(swcfg->skfd, swcfg->ifr, singlePID, MII_BMSR);
			else
				val=mdio_read(swcfg->skfd, swcfg->ifr, portid, MII_BMSR);
			}
			printf("%d\r\n", ((val & BMSR_LSTATUS) ?1:0));
		}
		else{
			printf("  %s %s\n\n", argv[0], HELP_STATUS);
			return -1;
		}
	}
	else{
		printf("  %s %s\n\n", argv[0], HELP_STATUS);
		return -1;
	}
	return 0;
}/*end doStatus*/

/*___________________________________________________________________
**      function name: doIgmpsnoop
**      descriptions:
**        Enable/disable/query igmpsnoop for lanswitch
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           swic
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           hw_igmpsnoop
**           hw_fastleave
**      
**      revision:
**      1. Here 2009/11/28
**_______________________________________________________________
*/
static int doIgmpsnoop(int argc, char *argv[], void *p){
	int retval=0;
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();

	if((swcfg->swic_id !=TC2206) 
	|| (swic.hw_igmpsnoop ==NULL)
	|| (swic.hw_fastleave ==NULL)){
		printf("%s is not support igmpsnoop funciton.\n", swcfg->ifr.ifr_name);
		return ETHCMD_ERR;
	}
	
	if(!strcmp(argv[1], ETHCMD_ENABLE)){
		swic.hw_igmpsnoop(ACTIVED);
		swic.hw_fastleave(ACTIVED);
	}
	else if(!strcmp(argv[1], ETHCMD_DISABLE)){
		swic.hw_igmpsnoop(DEACTIVED);
		swic.hw_fastleave(DEACTIVED);
	}
	else if(!strcmp(argv[1], ETHCMD_QUERY)){
		/*do nothing, just to display the igmpsnoop settings.*/
		;
	}
	else{
		printf("  %s %s\n\n", argv[0], HELP_IGMPSNOOP);
		retval=ETHCMD_ERR;
	}
	printf("Igmpsnoop function is :%s\n", (swic.hw_igmpsnoop(QUERY)?ETHCMD_ENABLE:ETHCMD_DISABLE));
	return retval;
}/*end doIgmpsnoop*/
/*___________________________________________________________________
**      function name: doHelp
**      descriptions:
**        Ethcmd helpful message sub ci-cmd.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           cmdHelp
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doHelp(int argc, char *argv[], void *p){
	cmdHelp();
	return 0;
}/*end doHelp*/
/*___________________________________________________________________
**      function name: doVersion
**      descriptions:
**        Ethcmd show the ethcmd version.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           cmdHelp
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doVersion(int argc, char *argv[], void *p){
	printf("\r\nethphxcmd version: %s.\n", MODULE_VERSION_ETHCMD);
	return 0;
}/*end doVersion*/

#ifdef ETHCMD_DBG
/*___________________________________________________________________
**      function name: initEthcmdDbg
**      descriptions:
**        Debug the ethcmd ci-cmd, it's used the /var/tmp/ethdbg files exist to 
**        show debug message ,otherwise is not show debug message.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethcmdDbg
**             
**      return:
**         None
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void initEthcmdDbg(){
	struct stat fd_st;
	if(stat(ETHCMDDBG_FILE,&fd_st)==0){
		ethcmdDbg=1;/*Enable ethcmd debug message*/
	}
	else{
		ethcmdDbg=0;/*Disable ethcmd debug message*/
	}
}/*end initEthcmdDbg*/
/*___________________________________________________________________
**      function name: doEthcmdDbg
**      descriptions:
**        Debug the ethcmd ci-cmd,show the debug flag is enabled or not.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethcmdDbg
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
int doEthcmdDbg(int argc, char *argv[], void *p){
	printf("\r\nethcmdDbg: %s.\n", ((ethcmdDbg)?"on":"off"));
	return 0;
}/*end doEthcmdDbg*/
#endif
/*___________________________________________________________________
**      function name: doTCMiiRead
**      descriptions:
**        Basic command to read the ethernet mii register.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           mdio_read
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doTCMiiRead(int argc, char *argv[], void *p){
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();
	printf("*PhyAddr=%d Reg=%d value:%x\r\n", atoi(argv[1]) , atoi(argv[2]), \
		mdio_read(swcfg->skfd, swcfg->ifr, atoi(argv[1]), atoi(argv[2])));
	return 0;
}/*end doTCMiiRead*/
/*___________________________________________________________________
**      function name: doTCMiiWrite
**      descriptions:
**        Basic command to write the ethernet mii register.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           mdio_write
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doTCMiiWrite(int argc, char *argv[], void *p){
	struct swconfig *swcfg=NULL;
	u16 value=0;
	swcfg=getSwcfg();
	
	value=strtoul((const char*)(argv[3]), (char **)NULL, 16);
	mdio_write(swcfg->skfd, swcfg->ifr, atoi(argv[1]), atoi(argv[2]),value);
	printf("*PhyAddr=%d Reg=%d value:%x\r\n", atoi(argv[1]) , atoi(argv[2]), value);
	return 0;
}/*end doTCMiiWrite*/
/*___________________________________________________________________
**      function name: doGSWRead
**      descriptions:
**        Basic command to read the ethernet mii register.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           mdio_read
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doGSWRead(int argc, char *argv[], void *p){
	struct swconfig *swcfg=NULL;
	u32 offset;
	swcfg=getSwcfg();

	offset=strtoul((const char*)(argv[1]), (char **)NULL, 16);
	printf("*reg=%08lx value:%08lx\r\n", offset, 
		gsw_reg_read(swcfg->skfd, swcfg->ifr, offset));
	return 0;
}/*end doTCMiiRead*/
/*___________________________________________________________________
**      function name: doGSWWrite
**      descriptions:
**        Basic command to write the ethernet mii register.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           getSwcfg
**           mdio_write
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doGSWWrite(int argc, char *argv[], void *p){
	struct swconfig *swcfg=NULL;
	u32 offset;
	u32 value=0;
	swcfg=getSwcfg();
	
	offset=strtoul((const char*)(argv[1]), (char **)NULL, 16);
	value=strtoul((const char*)(argv[2]), (char **)NULL, 16);
	gsw_reg_write(swcfg->skfd, swcfg->ifr, offset, value);
	printf("*reg=%08lx value:%08lx\r\n", offset, value);
	return 0;
}/*end doGSWWrite*/
/*VLAN related ci-cmds*/
/*___________________________________________________________________
**      function name: doVlan
**      descriptions:
**         Register vlan subcmd.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethCmds
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doVlan(int argc, char *argv[], void *p){
	return subcmd(vlanCmds, argc, argv, p);
}/*end doVlan*/
/*___________________________________________________________________
**      function name: doVlanSetActive
**      descriptions:
**         Enable/disable the swtich tagged base vlan function.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           swic
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doVlanSetActive(int argc, char *argv[], void *p){
	u8 mode=0;
	mode=(u8)atoi(argv[1]);

	if(mode>ACTIVED){
		printf(HELP_VLAN_ACTIVE);
		return -1;
	}
	if(swic.vlan_active!=NULL){
		swic.vlan_active(mode);
	}
	#ifdef ETHCMD_DBG
	ETH_DEBUG("%s","ci-cmd:doVlanSetActive\r\n");
	#endif
	return 0;
}/*end doVlanSetActive*/
/*___________________________________________________________________
**      function name: doVlanSetPvid
**      descriptions:
**         Set the Etherent port PVID value.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethCmds
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doVlanSetPvid(int argc, char *argv[], void *p){
	u8 port=0;
	u16 pvid=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	
	port=atoi(argv[1]);
	pvid=atoi(argv[2]);
	/*Check the input parameters is right or not.*/
	if((port>=vlanCap->max_port_no)||(pvid>=vlanCap->max_vid)){
		printf(HELP_VLAN_PVID);
		return -1;
	}
	
	if(swic.vlan_pvid!=NULL){
		swic.vlan_pvid(port, pvid);
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg)
		printf("port:%d pvid:%d, vlancap: max_port:%d maxvid:%d\r\n",port, pvid, vlanCap->max_port_no, vlanCap->max_vid);
	#endif
	return 0;
}/*end doVlanSetPvid*/
/*___________________________________________________________________
**      function name: doVlanSetVid
**      descriptions:
**         Update a vlan table rule accroding the table index.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethCmds
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doVlanSetVid(int argc, char *argv[], void *p){
	u8 index=0;
	u8 active=0;
	u8 portMap=0;
	u8 tagPortMap=0;
	u16 vid=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	
	index=atoi(argv[1]);
	active=atoi(argv[2]);
	vid=atoi(argv[3]);
	
	/*Check the input parameters is right or not.*/
	if((index>=MAX_VLAN_RULE)
	|| (vid>=vlanCap->max_vid)
	|| (active>ACTIVED)){
		printf(HELP_VLAN_VID);
		return -1;
	}
	
	/*CPU Port is always the membership*/
	portMap=atoi(argv[4]);
	tagPortMap=atoi(argv[5]);
	if(swic.vlan_vid!=NULL){
		swic.vlan_vid(index, active, vid, portMap, tagPortMap);
	}
	
	#ifdef ETHCMD_DBG
	if(ethcmdDbg)
		printf("index:%d active:%d vid:%d\r\n",index, active, vid);
	#endif
	return 0;
}/*end doVlanSetVid*/
/*___________________________________________________________________
**      function name: doVlanDisp
**      descriptions:
**         Display the pvid and port binding information.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethCmds
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/10
**_______________________________________________________________
*/
static int doVlanDisp(int argc, char *argv[], void *p){
	if(swic.vlan_disp!=NULL){
		swic.vlan_disp();
	}
	return 0;
}/*end doVlanDisp*/
#ifdef ETHCMD_EXTENSION
/*___________________________________________________________________
**      function name: doPortBinding
**      descriptions:
**         Update a vlan table rule accroding the table index.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           ethCmds
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           subcmd
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int doPortBinding(int argc, char *argv[], void *p){
	int index=0;
	int vid=0;
	int i=0,j=0;
	int err=ETHCMD_ERR;
	char cmd[MAX_CMD_LEN]={0};
	int op=0;
	int pm_if[MAX_IFS]={0};
	u32 pm_flag=0;
	u8 portMap=0;

	#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	#endif
	for(i=0;i<MAX_IFS;i++){
		pm_if[i]=NONE_IF;
	}
	
	index=atoi(argv[1]);
	op=atoi(argv[2]);
	
	if((op==ACTIVED) ||(op==DEACTIVED)
		&&(index<MAX_VLAN_RULE)){
		err=0;
	}
	/*lookup the interface name*/
	for (i=PM_START_IF_ARG; i<argc; i++){
		for(j=0; tcPortMap[j].ifname!=NULL; j++){
			if(strcmp(argv[i], tcPortMap[j].ifname)==0){
				err=0;
				/*Record the tcPortMap index of real ifname,starting at arg 3*/
				pm_flag|=(1<<j);
				pm_if[i-PM_START_IF_ARG]=j;
			}
		}
	}
	
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		for(i=0; i<MAX_IFS; i++){
			printf("pm_if[%d]=%d ", i, pm_if[i]);
			if(i%5==4)
				printf("\r\n");
		}
		printf("\r\n err:%d\r\n",err);
	}
	#endif
	
	/*checking the interface name is right or not.*/
	if(err==ETHCMD_ERR){
		#if defined(TCSUPPORT_2PORTS) || defined(TCSUPPORT_CT_2PWIFI)
		printf(HELP_PORTBINDING,vlanCap->max_port_no-1);
		#else
		printf(HELP_PORTBINDING);
		#endif
		printf("\n");
		return ETHCMD_ERR;
	}
	
	if(op==DEACTIVED){
		/*Remove the virtual interface from bridge interface*/
		for(i=0; pm_if[i]!=NONE_IF; i++){
			sprintf(cmd,"brctl delif br%d %s",index, tcPortMap[pm_if[i]].real_name);
			system(cmd);
			#ifdef ETHCMD_DBG
			ETH_DEBUG("%s",cmd);
			#endif
		}
		if(index!=0){/*We can't delete default br0 bridge interface*/
			/*Let bridge interface down*/
			sprintf(cmd,"ifconfig br%d down",index);
			system(cmd);
			#ifdef ETHCMD_DBG
			ETH_DEBUG("%s",cmd);
			#endif
			/*Delete the bridge interface*/
			sprintf(cmd,"brctl delbr br%d",index);
			system(cmd);	
			#ifdef ETHCMD_DBG
			ETH_DEBUG("%s",cmd);
			#endif
		}
	}
	else{
		/*Add a new bridge interface to binding interface*/
		if(index!=0){/*We can't delete default br0 bridge interface*/
			/*Adding the bridge interface*/
			sprintf(cmd,"brctl addbr br%d",index);
			system(cmd);
			/*Let bridge interface up*/
			sprintf(cmd,"ifconfig br%d 0.0.0.0 up",index);
			system(cmd);
		}
		
		/*binding the virtual interface into bridge interface*/
		/*Note: Need to check the pvc operate at bridge mode or routing mode, Only
		operate at bridge mode can be enslave by bridge interface.Routing mode is 
		need used iptable to filter it.*/
		for(i=0; pm_if[i]!=NONE_IF; i++){
			sprintf(cmd,"brctl addif br%d %s",index, tcPortMap[pm_if[i]].real_name);
			system(cmd);
			#ifdef ETHCMD_DBG
			ETH_DEBUG("%s",cmd);
			#endif
		}
	}
	return 0;
}/*end doPortBinding*/
#endif

#if 0
/*Use ebtable to set do it*/
/*______________________________________________________________________________
**	function name:decideModulePath
**
**	description:
*     decide module path according to the kernel version
**	parameters:
*     		vername: used to obtain kernel version
**	global:
*     		none
**	return:
*     		none
**	call:
*     		none
**	revision:
*     1.Ian 20091129
**____________________________________________________________________________*/
void decideModulePath(char* vername){
	FILE * Ver;
	char buf[50], *start, *end;

	Ver = fopen ("/proc/version","r");
	if(!Ver){
		printf("Failed to open /proc/version\r\n");
		return;
	}
	fgets(buf, 50, Ver);
	fclose(Ver);
	start = strchr(buf, ' ');
	start = strchr(start + 1, ' ') + 1;
	end = strchr(start + 1, ' ');
	*end = '\0';
	strcpy(vername, start);
}/*end decideModulePath*/
/*___________________________________________________________________
**      function name: ebtSettings
**      descriptions:
**         Loading/Unloading ebtables and setting the FORWARD chain.
**        1. The FORWARD chain is used to drop the packet for ethernet switch site 
**   	  	  packet.
**
**      parameters:
**         type:ACTIVED/DEACTIVED ebtables.
**         
**      global:
**           None
**             
**      return:
**         None
**	    
**      call:
**           getEtherVlanCap
**           decideModulePath
**      
**      revision:
**      1. Here 2009/11/29
**_______________________________________________________________
*/
void
ebtSettings(u8 type){
	FILE *fp=NULL;
	char getStr[8];
	int isActive=0;
	char linuxVersion[16];
	char cmd[MAX_CMD_LEN]={0};
	int i=0,j=0;
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	char ebt_modules[][32]={"ebtables.ko", "ebtable_filter.ko", ""};

	/*Get the current linux kernel version*/
	decideModulePath(linuxVersion);
	if(type==ACTIVED){
		/*check the modules is load or not*/
		fp=fopen(VPORT_FILE, "r");
		if(fp!=NULL){
			fgets(getStr, sizeof(getStr), fp);
			isActive=atoi(getStr);
			fclose(fp);
		}
		
		if(isActive==DEACTIVED){/*If it's not load ebt modules and to loading it.*/
			for(i=0; strlen(ebt_modules[i])!=0; i++){
				sprintf(cmd, EBT_MODULES_INSTALL, linuxVersion, ebt_modules[i]);
				system(cmd);
			}
		}
		system(EBT_FWD_FLUSH);
		system(EBT_FWD_ACCEPT);
		/*SET FORWARD chain to drop the packet is from ethernet site*/
		for(i=1; i<=vlanCap->max_port_no; i++){
			for(j=1; j<=vlanCap->max_port_no; j++){
				if(i !=j){/*Not set at same interface with input/output interface*/
					sprintf(cmd, EBT_FWD_DROP_RULE, i, j);
					system(cmd);
				}
			}
		}
	}
	else{
		/*FLUSH FORWARD Chain*/
		system(EBT_FWD_FLUSH);
		system(EBT_MODULES_REMOVE);
	}
}/*end ebtSettings*/
#endif
/*___________________________________________________________________
**      function name: setLanchip
**      descriptions:
**        Enable/disable the ethernet port that is used mii register to let ethernet 
**        phy to power down or not.
**
**      parameters:
**         type: enable: Power up the ethernet PHY ,disable: Power down the ethernet phy.
**         portid: Specify the ethernet Port number.
**         
**      global:
**           ethcmdDbg
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**           mdio_read
**           mdio_write
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int 
setLanchip(char* type, int portid){
	int val=0;
	int retval=0;
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();

	if(strcmp(type, ETHCMD_DISABLE)==0){/*Power Down the Ethernet PHY Port*/
		if(swcfg->swic_id==TC2206){
			/*Read FLP register*/
			val=mdio_read(swcfg->skfd, swcfg->ifr, portid, MII_ADVERTISE);
			/*Set FLP register to disable all capability 0x1*/
			mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_ADVERTISE, EXPANSION_NWAY);
			/*restart-AN*/
			mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_BMCR, (BMCR_ANENABLE |BMCR_ANRESTART));
			/*Power Down*/
			mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_BMCR, (BMCR_ANENABLE |BMCR_PDOWN));
			/*Restore FLP register*/
			mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_ADVERTISE, val);
		}
		else{
			val=(mdio_read(swcfg->skfd, swcfg->ifr, portid, MII_BMCR)|BMCR_PDOWN);
			mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_BMCR, val);
		}
	}
	else if(strcmp(type, ETHCMD_ENABLE)==0){/*Power UP the Ethernet PHY Port*/
		val= (mdio_read(swcfg->skfd, swcfg->ifr, portid, MII_BMCR) & ENABLE_LAN_MASK);
		mdio_write(swcfg->skfd, swcfg->ifr, portid, MII_BMCR, val);
	}
	else{
		retval=-1;
	}

	return retval;
}/*end setLanchip*/
/*___________________________________________________________________
**      function name: ethcmdQueryPortCount
**      descriptions:
**        Check user specify portid is fit with this platform.
**
**      parameters:
**         portid: port id.
**         
**      global:
**           None
**             
**      return:
**        switch port number, 1: single port , 4: 4ports-switch.
**	    
**      call:
**           getEtherPort
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static int ethcmdQueryPortCount(int portid){
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	if(vlanCap->max_port_no !=SWIC_PORTS_NUM){
		 printf("eth0 is not Switch chip.\n");
		
	}
	else{
		if((portid<0)||(portid > vlanCap->max_port_no)){
			  printf("Invalid port: should is <%d-%d>", 0, (vlanCap->max_port_no-1));
		}
	}
	return vlanCap->max_port_no;
}/*end ethcmdQueryPortCount*/
/*___________________________________________________________________
**      function name: ethcmdGetMediaOption
**      descriptions:
**        Check user specify media-type option is right or not, if the parameters is
**        return the right value to set mii register.
**
**      parameters:
**         option: user specify the parameters of media-type.
**         
**      global:
**           None
**             
**      return:
**       The value to set mii register to fit the specfiy link type.
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
static u16 ethcmdGetMediaOption(char *option){
	u16 val=ERR_MEDIAY_TYPE;
	int i=0;
	struct linkType mediaOption[]={
		{"auto", (BMCR_ANENABLE | BMCR_ANRESTART)},
		{"100FD", (BMCR_SPEED100 | BMCR_FULLDPLX|BMCR_ANRESTART)},
		{"100HD", (BMCR_SPEED100|BMCR_ANRESTART)},
		{"10FD", (BMCR_FULLDPLX|BMCR_ANRESTART)},
		{"10HD", 0},
		{NULL, 0},
	};
	
	for(i=0; mediaOption[i].name!=NULL ; i++){
		if(strcmp(option,mediaOption[i].name)==0){
			val=mediaOption[i].val;
			break;
		}
	}
	return val;
}/*end ethcmdGetMediaOption*/
/*___________________________________________________________________
**      function name: showMediaSetting
**      descriptions:
**        Display the ethernet port link type.
**
**      parameters:
**         phy_id: Port ID.
**         
**      global:
**           None
**             
**      return:
**       The value to set mii register to fit the specfiy link type.
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/	
static int showMediaSetting(int phy_id)
{
	int i;
	int bmcr, bmsr, nway_advert, lkpar;
	int max_capability = 0;
	/* Scan for the highest negotiated capability, highest priority
	(100baseTx-FDX) to lowest (10baseT-HDX). */
	int media_priority[] = {8, 9, 7, 6, 5}; 	/* media_names[i-5] */
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();
	
	 bmcr =mdio_read(swcfg->skfd, swcfg->ifr, phy_id, MII_BMCR);
	 bmsr =mdio_read(swcfg->skfd, swcfg->ifr, phy_id, MII_BMSR);	
    
	if (bmcr == 0xffff  ||  bmsr == 0x0000) {
		printf("  No MII transceiver present!.\n");
		return -1;
	}

	nway_advert = mdio_read(swcfg->skfd, swcfg->ifr, phy_id, MII_ADVERTISE);
	lkpar = mdio_read(swcfg->skfd, swcfg->ifr, phy_id, MII_LPA);

	if (bmcr & BMCR_ANENABLE) {
		printf("auto, ");
		printf("Auto-negotiation enabled.\n");
		/*for some device, bit "ADVERTISE_LPACK" in link partner register when auto negotiation
		 *is not set to 1, and these devices can not show link status when use ethcmd.
		 * mark the condition in order to show link status of all devices in LAN 
		 * xflu @ 20110722
		 */
//		if (lkpar & ADVERTISE_LPACK) {
			int negotiated = nway_advert & lkpar & 
							(ADVERTISE_100BASE4 |
							ADVERTISE_100FULL |
							ADVERTISE_100HALF |
							ADVERTISE_10FULL |
							ADVERTISE_10HALF );
			
			for (i = 0; media_priority[i]; i++) {
				if (negotiated & (1 << media_priority[i])) {
					max_capability = media_priority[i];
					break;
				}
			}
			if (max_capability)
				printf("The autonegotiated media type is %s.\n",
					media_names[max_capability - 5]);
			else
				printf("No common media type was autonegotiated!\n"
				"This is extremely unusual and typically indicates a "
				"configuration error.\n" "Perhaps the advertised "
				"capability set was intentionally limited.\n");
//		}
	} 
	else {
		if(bmcr & BMCR_SPEED100)
			printf("100");
		else
			printf("10");

		if(bmcr & BMCR_FULLDPLX)
			printf("FD, ");
		else
			printf("HD, ");

		printf("Auto-negotiation disabled, with\n"
		" Speed fixed at 10%s mbps, %s-duplex.\n",
		bmcr & BMCR_SPEED100 ? "0" : "",
		bmcr & BMCR_FULLDPLX ? "full":"half");
	}
	bmsr = mdio_read(swcfg->skfd, swcfg->ifr, phy_id, MII_BMSR);
	printf("Link is %s\n", (bmsr & BMSR_LSTATUS) ? "up" : "down");
	return 0;
}/*end showMediaSetting*/
#if 1   //LAN_ETHERNET_INTERFACE
/*___________________________________________________________________
**      function name: doSwitchPortNum
**      descriptions:
**        Get the Switch port number.
**
**      parameters:
**         argc: The arguments number.
**         argv: The vectors arguments 
**         p:function pointer
**         
**      global:
**           None
**             
**      return:
**         0:succuces
**         -1:fail
**	    
**      call:
**      
**      revision:
**_______________________________________________________________
*/
static int doSwitchPortNum(int argc, char *argv[], void *p)
{
	etherChannVlanCap_t*  vlanCap=getEtherVlanCap();
	if(argc==1){ /*Get the Swtich port num */
		printf("%d\n", vlanCap->max_port_no);
	}
	else{
		printf("  %s %s\n\n", argv[0], HELP_SWITCH_PORTNUM);
		return -1;
	}

	return 0;
}
#endif
