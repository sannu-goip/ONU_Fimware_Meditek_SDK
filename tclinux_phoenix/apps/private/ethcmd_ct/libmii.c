/************************************************************************
 *
 *	Copyright (C) 2006 Trendchip Technologies, Corp.
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/mii.h>
#include <linux/sockios.h>
#include "libmii.h"
#include "libcmd.h"
#include "ether.h"
#include "ethercmd.h"
#include <fcntl.h>

#ifdef ETHCMD_DBG
extern u8 ethcmdDbg;
#endif

static struct swconfig swcfg;
swic_api_t swic;
int lan_portmap[6];

void tc2206_init_api(swic_api_t* swic);
void rt63365_init_api(swic_api_t* swic);
void ip175c_init_api(swic_api_t* swic);

struct vender swic_id[]=
{
	{4, 2, 0x243, 4, 3, 0xd80, "IP175C", IP175C, ip175c_init_api},
	{31, 31, 0x2206, 31, 31, 0x2206, "TC2206", TC2206, tc2206_init_api},
	{0, 2, 0x3a2, 0, 3, 0x940b, "RT63365", SWIC_RT63365, rt63365_init_api},
	{6, 2, 0x1c, 6, 3, 0xc912, "RT63365", SWIC_RT63365, rt63365_init_api},
	{0, 2, 0x243, 0, 3, 0xc54, "RT63365", SWIC_RT63365, rt63365_init_api},
};

/*Number of switch IC*/
#define NSWIC (sizeof(swic_id)/sizeof(swic_id[0]))

/*___________________________________________________________________
**      function name: getSwcfg
**      descriptions:
**         Get the global swcfg variable.
**
**      parameters:
**        None
**  
**      global:
**           swcfg
**             
**      return:
**          The address of global swcfg variable.
**	    
**      call:
**          None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
struct swconfig* getSwcfg(void){
	return &swcfg;
}/*end getSwcfg*/
#define PORT_REVERSE_PATH	"/proc/tc3162/port_reverse"
#define STAG_TO_VTAG_PATH	"/proc/tc3162/stag_to_vtag"
#define FAIL 			-1
/*_____________________________________________________________________________
**      function name: fileRead
**      descriptions:
**           Read file context and copy information into buf.
**
**      parameters:
**            path:  File path
**            buf:  Specify the buffer that you want to store it.
**            size: Specify the size that your want to read form file.
**
**      global:
**             None
**
**      return:
**             None
**
**      call:
**   	 None
**
**      revision:
**      1. Here 2008/5/7
**____________________________________________________________________________
*/
void
fileRead(char *path, char *buf, int size){
	int  fd=0, ret=0;

	memset(buf,0, size);
	fd = open(path,O_RDONLY);
	if(fd == FAIL){
		return;
	}

	ret = read(fd, buf, size);
	if(ret <= 0){
		close(fd);
		return;
	}

	close(fd);
}/*end fileRead*/

/*___________________________________________________________________
**      function name: mdio_read
**      descriptions:
**         Use mdio interface to read mii register.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**         phy_id: PHY address 
**         reg_num: Register  address
**  
**      global:
**           ethcmdDbg
**             
**      return:
**          The value of mii register.
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
int 
mdio_read(int skfd, struct ifreq ifr,int phy_id, int reg_num)
{
	struct mii_data *mii = (struct mii_data *)&ifr.ifr_data;
	mii->phy_id = phy_id;
	mii->reg_num = reg_num;

	if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
			strerror(errno));
		return -1;
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		fprintf(stderr, "mdio_read: phyid:%x reg_num:%x value:%x\r\n", phy_id, reg_num, mii->val_out);
	}
	#endif
	return mii->val_out;
}
/*___________________________________________________________________
**      function name: mdio_write
**      descriptions:
**         Use mdio interface to write mii register.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**         phy_id: PHY address 
**         reg_num: Register  address
**         value: Specify the value to fill into mii register.
**  
**      global:
**           ethcmdDbg
**             
**      return:
**          None
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void
mdio_write(int skfd, struct ifreq ifr,int phy_id, int reg_num, int value)
{
	struct mii_data *mii = (struct mii_data *)&ifr.ifr_data;
	mii->phy_id = phy_id;
	mii->reg_num = reg_num;
	mii->val_in = value;

	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		fprintf(stderr, "mdio_write: phyid:%x reg_num:%x value:%x\r\n", phy_id, reg_num, value);
	}
	#endif
}/*end mdio_write*/

/*___________________________________________________________________
**      function name: mdio_reg_write
**      descriptions:
**         Use mdio interface to read mii register.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**         phy_id: PHY address 
**         reg_num: Register  address
**  
**      global:
**           ethcmdDbg
**             
**      return:
**          The value of mii register.
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
int 
mdio_reg_read(int skfd, struct ifreq ifr,int offset)
{
	gsw_reg reg;
	ifr.ifr_data = &reg;
	reg.off = offset;

	if (ioctl(skfd, RAETH_REG_READ, &ifr) < 0) {
		fprintf(stderr, "RAETH_REG_READ on %s failed: %s\n", ifr.ifr_name,
			strerror(errno));
		return -1;
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		fprintf(stderr, "gsw_reg_read: offset:%x value:%x\r\n", offset, reg.val);
	}
	#endif
	return reg.val;
}

/*___________________________________________________________________
**      function name: mdio_reg_write
**      descriptions:
**         Use mdio interface to write mii register.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**         phy_id: PHY address 
**         reg_num: Register  address
**         value: Specify the value to fill into mii register.
**  
**      global:
**           ethcmdDbg
**             
**      return:
**          None
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void
mdio_reg_write(int skfd, struct ifreq ifr,int offset, int value)
{
	gsw_reg reg;
	ifr.ifr_data = &reg;
	reg.off = offset;
	reg.val = value;

	if (ioctl(skfd, RAETH_REG_WRITE, &ifr) < 0) {
		fprintf(stderr, "RAETH_REG_WRITE on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		fprintf(stderr, "gsw_reg_write: offset:%x value:%x\r\n", offset, value);
	}
	#endif
}/*end mdio_reg_write*/


/*___________________________________________________________________
**      function name: gsw_reg_read
**      descriptions:
**         Use mdio interface to read mii register.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**         phy_id: PHY address 
**         reg_num: Register  address
**  
**      global:
**           ethcmdDbg
**             
**      return:
**          The value of mii register.
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
int 
gsw_reg_read(int skfd, struct ifreq ifr,int offset)
{
	gsw_reg reg;
	ifr.ifr_data = &reg;
	reg.off = offset;

	if (ioctl(skfd, RAETH_GSW_REG_READ, &ifr) < 0) {
		fprintf(stderr, "RAETH_GSW_REG_READ on %s failed: %s\n", ifr.ifr_name,
			strerror(errno));
		return -1;
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		fprintf(stderr, "gsw_reg_read: offset:%x value:%x\r\n", offset, reg.val);
	}
	#endif
	return reg.val;
}
/*___________________________________________________________________
**      function name: gsw_reg_write
**      descriptions:
**         Use mdio interface to write mii register.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**         phy_id: PHY address 
**         reg_num: Register  address
**         value: Specify the value to fill into mii register.
**  
**      global:
**           ethcmdDbg
**             
**      return:
**          None
**	    
**      call:
**           None
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void
gsw_reg_write(int skfd, struct ifreq ifr,int offset, int value)
{
	gsw_reg reg;
	ifr.ifr_data = &reg;
	reg.off = offset;
	reg.val = value;

	if (ioctl(skfd, RAETH_GSW_REG_WRITE, &ifr) < 0) {
		fprintf(stderr, "RAETH_GSW_REG_WRITE on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
	}
	#ifdef ETHCMD_DBG
	if(ethcmdDbg){
		fprintf(stderr, "gsw_reg_write: offset:%x value:%x\r\n", offset, value);
	}
	#endif
}/*end gsw_reg_write*/

/*___________________________________________________________________
**      function name: searchSwic
**      descriptions:
**         Accroding the specify mii register to know which is the switch ic.
**
**      parameters:
**         skfd: Ioctl socket descriptor.
**         ifr: Interfce 
**  
**      global:
**           swcfg
**           ethcmdDbg
**             
**      return:
**          Switch Vender ID.
**	    
**      call:
**           mdio_read
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void 
searchSwic(int skfd, struct ifreq ifr){
	int i;
	
	for(i=0; i<NSWIC; i++){
		if((mdio_read(skfd, ifr, swic_id[i].phy_id1, swic_id[i].reg1)==swic_id[i].id1)
		&&(mdio_read(skfd, ifr, swic_id[i].phy_id2, swic_id[i].reg2)==swic_id[i].id2)){
			#ifdef ETHCMD_DBG
			if(ethcmdDbg){
				fprintf(stderr,"%s\r\n",swic_id[i].name);
			}
			#endif
			swcfg.swic_id=swic_id[i].swic_id;
			if(swic_id[i].api_init!=NULL){
				swic_id[i].api_init(&swic);
			}
			return;
		}
	}
	swcfg.swic_id=UNKNOW_SWIC;
} /*end searchSwic*/
/*___________________________________________________________________
**      function name: tc2206_init_api
**      descriptions:
**         Register the tc2206 relate function.
**
**      parameters:
**         swic: switch IC api.
**  
**      global:
**           None
**             
**      return:
**          None
**	    
**      call:
**           macTC2206VlanActive
**           macTC2206VlanSetPvid
**           macTC2206VlanSetVid
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void 
tc2206_init_api(swic_api_t* swic){
	swic->hw_igmpsnoop=macTC2206Igmpsnoop;
	swic->hw_fastleave=macTC2206IgmpFastLeave;
	swic->spTag=NULL;
	swic->vlan_active=macTC2206VlanActive;
	swic->vlan_pvid=macTC2206VlanSetPvid;
	swic->vlan_get_pvid=macTC2206VlanGetPvid;
	swic->vlan_vid=macTC2206VlanSetVid;
	swic->vlan_get_vtbl=macTC2206VlanGetVtbl;
	swic->vlan_pvid_disp=NULL;
	swic->vlan_disp=macTC2206VlanDisp;
	swic->vlan_getCap=macTC2206VlanCap;
}/*end tc2206_init_api*/

/*___________________________________________________________________
**      function name: tc2206_init_api
**      descriptions:
**         Register the tc2206 relate function.
**
**      parameters:
**         swic: switch IC api.
**  
**      global:
**           None
**             
**      return:
**          None
**	    
**      call:
**           macTC2206VlanActive
**           macTC2206VlanSetPvid
**           macTC2206VlanSetVid
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void 
ip175c_init_api(swic_api_t* swic){
	swic->hw_igmpsnoop=NULL;
	swic->hw_fastleave=NULL;
	swic->spTag=NULL;
	swic->vlan_active=macIP175CVlanActive;
	swic->vlan_pvid=macIP175CVlanSetPvid;
	swic->vlan_get_pvid=NULL;
	swic->vlan_vid=macIP175CVlanSetVid;
	swic->vlan_get_vtbl=NULL;
	swic->vlan_pvid_disp=NULL;
	swic->vlan_disp=NULL;
	swic->vlan_getCap=macIP175CVlanCap;
}/*end ip175c_init_api*/

/*___________________________________________________________________
**      function name: rt63365_init_api
**      descriptions:
**         Register the rt63365 relate function.
**
**      parameters:
**         swic: switch IC api.
**  
**      global:
**           None
**             
**      return:
**          None
**	    
**      call:
**           macTC2206VlanActive
**           macTC2206VlanSetPvid
**           macTC2206VlanSetVid
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
void 
rt63365_init_api(swic_api_t* swic){
	swic->hw_igmpsnoop=macRT63365Igmpsnoop;
	swic->hw_fastleave=macRT63365IgmpFastLeave;
	swic->spTag=NULL;
	swic->vlan_active=macRT63365VlanActive;
	swic->vlan_pvid=macRT63365VlanSetPvid;
	swic->vlan_get_pvid=macRT63365VlanGetPvid;
	#if defined(SQA_VERIFY)
	swic->sqa_vlan_vid=macRT63365VlanSetVid;
	swic->vlan_vid=NULL;
	#else
	swic->vlan_vid=macRT63365VlanSetVid;
	#endif
	swic->vlan_get_vtbl=macRT63365VlanGetVtbl;
	swic->vlan_pvid_disp=NULL;
	swic->vlan_disp=macRT63365VlanDisp;
	swic->vlan_getCap=macRT63365VlanCap;
}/*end tc2206_init_api*/

/*___________________________________________________________________
**      function name: init_swconfig
**      descriptions:
**         Init the switch ic and register switch ic api function.
**
**      parameters:
**         None
**  
**      global:
**           swcfg
**             
**      return:
**          None
**	    
**      call:
**           searchSwic
**      
**      revision:
**      1. Here 2009/11/7
**_______________________________________________________________
*/
int 
init_swconfig(void){
	char buf[32];
	/*init swcfg*/
	memset(&swcfg, 0, sizeof(struct swconfig));
	
	/* Open a basic socket.*/
	if ((swcfg.skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
		perror("socket");
		exit(-1);
	}

	/* port reverse */
	memset(buf, 0, sizeof(buf));
	fileRead(PORT_REVERSE_PATH, buf, sizeof(buf));
	
	/* dbg info */
	//fprintf(stderr, "buf is %s\n", buf);

	
	if (buf[0] == '1') {
	 //swcfg.reverse_flag = 1;
		swcfg.flag |= PORT_REVERSE;
	}
	
	/* special tag to vlan tag */
	memset(buf, 0, sizeof(buf));
	fileRead(STAG_TO_VTAG_PATH, buf, sizeof(buf));
	if (buf[0] == '1') {
		//swcfg.reverse_flag = 1;
		swcfg.flag |= STAG_TO_VTAG;
	}
	
	strncpy(swcfg.ifr.ifr_name, LAN_IF, IFNAMSIZ);
	if (ioctl(swcfg.skfd, SIOCGMIIPHY, &swcfg.ifr) < 0) {
		if (errno != ENODEV)
			fprintf(stderr, "SIOCGMIIPHY on '%s' failed: %s\n",
			LAN_IF, strerror(errno));
		close(swcfg.skfd);
		return -1;
	}
	/*Find  out the switch vernder id and register api function*/
	searchSwic(swcfg.skfd, swcfg.ifr);
	/*Set VLAN Capability*/
	if(swic.vlan_getCap){
		swic.vlan_getCap();
	}
	else{
		/* single port */
		macDefVlanCap();
	}

#ifdef  TCSUPPORT_CPU_RT63365	
	eth_port_init();
#endif	
	return 0;
}/*end init_swconfig*/
int get_wan_port(void){
	char buf[4];
	FILE *fp = NULL;

	memset(buf, 0, sizeof(buf));
	fp = fopen("/proc/tc3162/eth_portmap","r");
	if(fp != NULL){
		if(fgets (buf, sizeof(buf), fp) != NULL){
			fclose(fp);	
				return atoi(buf);
		}	
		fclose(fp);	
	}
	return -1;
}

void eth_port_init(void){
	char buf[8];
	FILE *fp = NULL;
	int i, index, port;

	memset(lan_portmap, -1, sizeof(lan_portmap));
	
	fp = fopen("/proc/tc3162/eth_portmap","r");
	if(fp != NULL){
		memset(buf, 0, sizeof(buf));
		if(fgets (buf, sizeof(buf), fp) != NULL){
			memset(buf, 0, sizeof(buf));
			for (i=0; i<6; i++){
				if(fgets(buf, sizeof(buf), fp) != NULL){
					if (sscanf(buf,"%d %d", &index, &port) == 2){
						if (0<=index && index<=5){
							lan_portmap[index] = port;
						}	
					}
				}
			}	
		}	
		fclose(fp);
	}		
}
