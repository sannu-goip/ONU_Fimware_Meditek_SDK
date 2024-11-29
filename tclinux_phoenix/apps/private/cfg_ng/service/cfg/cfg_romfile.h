
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

#ifndef __CFG_ROMFILE_H__
#define __CFG_ROMFILE_H__

#include "cfg_api.h"
#include <mxml.h>
#include <trx.h>
#include <flash_layout/prolinecmd.h>

#define ROMFILE_PATH 				"/tmp/var/romfile.cfg"
#define FRAG_ROMFILE_PATH 			"/tmp/var/frag_romfile.cfg"
#define TMP_ROMFILE_PATH 			"/tmp/cfg.save.tmp"
#define STORE_ROMFILE_FILE_NAME 	"/tmp/tc_store_romfile.conf"
#define UPDATE_ROMFILE_CMD			"/userfs/bin/mtd write %s romfile"
#define TC_FLASH_ERASE_SECTOR_CMD	"/userfs/bin/mtd erasesector %lu %s"
#define DR_FILE_NAME				"/tmp/tc_defaultromfile"
#define CREATE_ROMFILE_ERR_MSG 		"Fail to create the romfile!!!\n"
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
#define DEF_ROMFILE_PATH 			"/userfs/ctromfile.cfg"
#else
#define DEF_ROMFILE_PATH 			"/userfs/romfile.cfg"
#endif
#define RUNNING_ROMFILE_PATH 		"/dev/mtdblock1"
#define LOAD_ROMFILE_FILE_NAME 		"/tmp/tc_load_romfile.conf"
#define ROMFILE_CHECKSUM_ERR_MSG 	"Romfile checksum is wrong!!!\n"

#define TCAPI_SAVE_STATUS_FILE "/tmp/tcapi_save_status"
#define UPLOAD_ROMFILE_PATH			"/var/tmp/up_romfile.cfg"
#define GZ_TMP_PATH "/tmp/"
#define CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME "/tmp/cwmp_ctcheckromfile.cfg"
#define CWMP_GZ_TMP_PATH "/tmp/cwmp_check_dir/"

int cfg_evt_write_romfile_to_flash(void);
int cfg_evt_write_romfile_to_flash_raw(void);

int cfg_write_romfile_to_flash(void);
void cfg_load_romfile(void);

typedef struct _Key_Parameters_Info
{
	int flag;					  /*flag to judge if node operation or attribute operation*/	
	char *node;		/*node name-----for example:Cwmp_Entry*/
	char *attr;		/*attribute name----for example:acsUrl*/	
	int	(*funp)(struct _Key_Parameters_Info* parainfo,mxml_node_t* oldtree,mxml_node_t* newtree);     /* handle function poiter*/
}Key_Parameters_Info,*Key_Parameters_Info_Ptr;

typedef struct _Key_Parameters2_Info
{
	int flag;		/*flag to judge if node operation or attribute operation*/	
	char *node;		/*node name-----for example:Cwmp_Entry*/
	char *node2;	/*node name-----for example:Cwmp_Entry*/
	char *attr;		/*attribute name----for example:acsUrl*/	
	int	(*funp)(struct _Key_Parameters_Info* parainfo,mxml_node_t* oldtree,mxml_node_t* newtree);     /* handle function poiter*/
}Key_Parameters2_Info,*Key_Parameters2_Info_Ptr;


typedef struct _Backup_Parameters_Info
{
	int flag;					  /*flag to judge if node operation or attribute operation*/	
	char *node;		/*node name-----for example:Cwmp_Entry*/
	char *attr;		/*attribute name----for example:acsUrl*/	
	char value[20];
	int	(*funp)(struct _Key_Parameters_Info* parainfo,mxml_node_t* oldtree,mxml_node_t* newtree);     /* handle function poiter*/
}Backup_Parameters_Info,*Backup_Parameters_Info_Ptr;

#if 0
#define PRDDUCTCLASSLEN 64
#define MANUFACUREROUILEN 64
#define SERIALNUMLEN 128
#define SSIDLEN 32
#define WPAKEYLEN 64
#define PPPUSRNAMELEN 64
#define PPPPWDLEN 64
#define CFEUSRNAMELEN 64
#define CFEPWDLEN 64
#define XPONSNLEN 32
#define XPONPWDLEN 80
#define MACADDRLEN 32
#define XPONMODELEN	8
#define FONKEYWORDLEN 65
#define FONMACLEN 18
#define BARCODELEN	32
#define TELNETLEN 4
#define HWVERLEN 64
#define ROMFILESELECTLEN 64
#define MT7570BOBLEN 225
#define WEBPWDLEN 16
#define WEBACCOUNTLEN 16
#if defined(TCSUPPORT_CT_UBUS)
#define CTEILEN 16
#define PROVINCELEN 64
#endif
#if defined(TCSUPPORT_ECNT_MAP)
#define MAPDEVROLELEN 4
#endif

typedef struct _proline_Para{
	int flag;
	int magic;
	char telnet[TELNETLEN];
	char barcode[BARCODELEN];
	char productclass[PRDDUCTCLASSLEN];
	char manufacturerOUI[MANUFACUREROUILEN];
	char serialnum[SERIALNUMLEN];
	char ssid[SSIDLEN];
	char wpakey[WPAKEYLEN];
	char webpwd[WEBPWDLEN];
	char pppusrname[PPPUSRNAMELEN];
	char ppppwd[PPPPWDLEN];
	char cfeusrname[CFEUSRNAMELEN];
	char cfepwd[CFEPWDLEN];
	char xponsn[XPONSNLEN];
	char xponpwd[XPONPWDLEN];
	char macaddr[MACADDRLEN];
	char xponmode[XPONMODELEN];
	char fonKeyword[FONKEYWORDLEN];
	char fonMac[FONMACLEN];
	char ssid2nd[SSIDLEN];
	char wpakey2nd[WPAKEYLEN];
	char webAcct[WEBACCOUNTLEN];
	char hwver[HWVERLEN];
	char romfileselect[ROMFILESELECTLEN];
	char mt7570bob[MT7570BOBLEN];
	char ssidac[SSIDLEN];
	char wpakeyac[WPAKEYLEN];
	char ssid2ndac[SSIDLEN];
	char wpakey2ndac[WPAKEYLEN];
	char gponregid[XPONPWDLEN];
#if defined(TCSUPPORT_ECNT_MAP)
	char mapdevrole[MAPDEVROLELEN];
	char mapbhssid2g[SSIDLEN];
	char mapbhwpakey2g[WPAKEYLEN];
	char mapbhssid5g[SSIDLEN];
	char mapbhwpakey5g[WPAKEYLEN];	
#endif
#if defined(TCSUPPORT_CT_UBUS)	
	char ctei[CTEILEN];	
	char province[PROVINCELEN];
#if defined(TCSUPPORT_ECNT_MAP)
	char reserve[203]; 
#else
	char reserve[399]; 
#endif
#else
#if defined(TCSUPPORT_ECNT_MAP)
	char reserve[283]; 
#else
	char reserve[479];
#endif
#endif
} proline_Para;

enum PL_CP_Flags {
	PL_CP_PRODUCTCLASS=0,
	PL_CP_MANUFACUREROUI,
	PL_CP_SERIALNUM,
	PL_CP_SSID,
	PL_CP_WPAKEY,
	PL_CP_WEBPWD,
	PL_CP_PPPUSRNAME,
	PL_CP_PPPPWD,
	PL_CP_CFEUSRNAME,
	PL_CP_CFEPWD,
	PL_CP_XPONSN,
	PL_CP_XPONPWD,
	PL_CP_MACADDR,
	PL_CP_TELNET,
	PL_FON_KEYWORD,
	PL_FON_MAC,
	PL_CP_BARCODE,
	PL_CP_XPONMODE,
	PL_CP_SSID2nd,
	PL_CP_WPAKEY2nd,
	PL_CP_WEB_ACCOUNT,
	PL_CP_HW_VER,
	PL_CP_ROMFILE_SELECT,
	PL_CP_MT7570BOB,
	PL_CP_SSIDAC,
	PL_CP_WPAKEYAC,
	PL_CP_SSID2ndAC,
	PL_CP_WPAKEY2ndAC,
	PL_GPON_REGID,
#if defined(TCSUPPORT_CT_UBUS)	
	PL_CP_CTEI,	
	PL_CP_PROVINCE,
#endif
#if defined(TCSUPPORT_ECNT_MAP)
	PL_CP_MAPDEVROLE,
#endif
	PL_CP_END
};

#if defined(TCSUPPORT_ECNT_MAP)
#define   PL_CP_MAPBHSSID2G      PL_CP_MACADDR   
#define   PL_CP_MAPBHWPAKEY2G    PL_FON_KEYWORD
#define   PL_CP_MAPBHSSID5G      PL_FON_MAC
#define   PL_CP_MAPBHWPAKEY5G    PL_CP_BARCODE
#endif

#define PL_CP_PRODUCTCLASS_FLAG (1<<PL_CP_PRODUCTCLASS)
#define PL_CP_MANUFACUREROUI_FLAG (1<<PL_CP_MANUFACUREROUI)
#define PL_CP_SERIALNUM_FLAG (1<<PL_CP_SERIALNUM)
#define PL_CP_SSID_FLAG (1<<PL_CP_SSID)
#define PL_CP_WPAKEY_FLAG (1<<PL_CP_WPAKEY)
#define PL_CP_WEBPWD_FLAG (1<<PL_CP_WEBPWD)
#define PL_CP_PPPUSRNAME_FLAG (1<<PL_CP_PPPUSRNAME)
#define PL_CP_PPPPWD_FLAG (1<<PL_CP_PPPPWD)
#define PL_CP_CFEUSRNAME_FLAG (1<<PL_CP_CFEUSRNAME)
#define PL_CP_CFEPWD_FLAG (1<<PL_CP_CFEPWD)
#define PL_CP_XPONSN_FLAG (1<<PL_CP_XPONSN)
#define PL_CP_XPONPWD_FLAG (1<<PL_CP_XPONPWD)
#if !defined(TCSUPPORT_ECNT_MAP)
#define PL_CP_MACADDR_FLAG (1<<PL_CP_MACADDR)
#define PL_FON_KEYWORD_FLAG (1<<PL_FON_KEYWORD)
#define PL_FON_MAC_FLAG (1<<PL_FON_MAC)
#define PL_CP_BARCODE_FLAG (1<<PL_CP_BARCODE)
#else
#define PL_CP_MAPDEVROLE_FLAG (1<<PL_CP_MAPDEVROLE)
#define PL_CP_MAPBHSSID2G_FLAG (1<<PL_CP_MAPBHSSID2G)
#define PL_CP_MAPBHWPAKEY2G_FLAG (1<<PL_CP_MAPBHWPAKEY2G)
#define PL_CP_MAPBHSSID5G_FLAG (1<<PL_CP_MAPBHSSID5G)
#define PL_CP_MAPBHWPAKEY5G_FLAG (1<<PL_CP_MAPBHWPAKEY5G)
#endif
#define PL_CP_XPONMODE_FLAG (1<<PL_CP_XPONMODE)
#define PL_CP_TELNET_FLAG (1<<PL_CP_TELNET)
#define PL_CP_SSID2nd_FLAG (1<<PL_CP_SSID2nd)
#define PL_CP_WPAKEY2nd_FLAG (1<<PL_CP_WPAKEY2nd)
#define PL_CP_WEB_ACCOUNT_FLAG (1<<PL_CP_WEB_ACCOUNT)
#define PL_CP_HW_VER_FLAG	(1<<PL_CP_HW_VER)
#define PL_CP_ROMFILE_SELECT_FLAG	(1<<PL_CP_ROMFILE_SELECT)
#define PL_CP_MT7570BOB_FLAG	(1<<PL_CP_MT7570BOB)
#define PL_CP_SSIDAC_FLAG (1<<PL_CP_SSIDAC)
#define PL_CP_WPAKEYAC_FLAG (1<<PL_CP_WPAKEYAC)
#define PL_CP_SSID2ndAC_FLAG (1<<PL_CP_SSID2ndAC)
#define PL_CP_WPAKEY2ndAC_FLAG (1<<PL_CP_WPAKEY2ndAC)
#define PL_GPON_REGID_FLAG (1<<PL_GPON_REGID)
#if defined(TCSUPPORT_CT_UBUS)
#define PL_CP_CTEI_FLAG (1<<PL_CP_CTEI)
#define PL_CP_PROVINCE_FLAG (1<<PL_CP_PROVINCE)
#endif

#define PRDDUCTCLASSLEN 64
#define MANUFACUREROUILEN 64
#define SERIALNUMLEN 128
#define SSIDLEN 32
#define WPAKEYLEN 64
#define PPPUSRNAMELEN 64
#define PPPPWDLEN 64
#define CFEUSRNAMELEN 64
#define CFEPWDLEN 64
#define XPONSNLEN 32
#define XPONPWDLEN 80
#define MACADDRLEN 32
#define XPONMODELEN	8
#define FONKEYWORDLEN 65
#define FONMACLEN 18
#define BARCODELEN	32
#define TELNETLEN 4
#define HWVERLEN 64
#define ROMFILESELECTLEN 64
#define MT7570BOBLEN 225
#endif

#if defined(TCSUPPORT_CMCCV2)
#define DEFAULT_SSID "CMCC-AP"
#else
#define DEFAULT_SSID "ChinaNet-AP"
#endif
#define DEFAULT_WPAKEY "12345678"
#define DEFAULT_WEBUSRNAME "admin"
#define DEFAULT_WEBPWD "1234"
#define DEFAULT_PPPUSRNAME "ppp"
#define DEFAULT_PPPPWD "ppp"
#define DEFAULT_CFEUSRNAME "telecomadmin"
#define DEFAULT_CFEPWD "nE7jA%5m"
#if defined(TCSUPPORT_CRJO)
#define DEFAULT_XPONSN "MSTC00223344"
#else
#define DEFAULT_XPONSN "FHTT12345678"
#endif
#define DEFAULT_XPONPWD "00000001"
#define DEFAULT_MACADDR "00AABB112233"
#define DEFAULT_XPONMODE "GPON"

int tcgetProLinePara(void*buf,int flag);
int compute_checksum(char *inputfile,char *outputfile);
int check_checksum(char *configinfo,char *outputfile,char *gzpath);
int getDefaultRomfile(char *path,unsigned int len);
void handle_proline_paras(mxml_node_t* defaulttree);
void handle_proline_paras_restore(mxml_node_t* defaulttree);
int update_UploadedRomfile(int imagefd, int offset, unsigned  int length, char *outputfile);
int check_romfile(char* pathname);
#endif

