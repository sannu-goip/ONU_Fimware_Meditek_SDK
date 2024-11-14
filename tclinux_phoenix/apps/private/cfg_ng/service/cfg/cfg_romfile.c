

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
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <mxml.h>
#include "cfg_cli.h"
#include <flash_layout/tc_partition.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/wait.h>
#include "cfg_romfile.h"
#include <linux/types.h>
#include <linux/version.h>
#include "cfg_msg.h"
#include "utility.h"
#include <svchost_evt.h>
#include <notify/notify.h>
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
#include "blapi_xpon.h"
#endif
#endif
#include "blapi_system.h"
#include "blapi_traffic.h"
#include "blapi_xdsl.h"
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#define MAX_NODE_NAME		32
#define MAX_NODE_LEVEL		4
#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE_NUM 32
#else
#define MAX_STATIC_ROUTE_NUM 16
#endif
#define CWMP_UPGRADE_ROMFILE_FILE_NAME "/tmp/tc_cwmp_upgrade_romfile.conf"
#define UPGRADE_ROMFILE_CMD	"/userfs/bin/mtd %s write /var/romfile.cfg romfile"
#define GZ_ROMFILE_FILE_NAME "romfile.gz"
#define CT_ROMFILE_FILE_NAME "ctromfile.cfg"
#define DEFVALUEFLAG_ACTIVE	"1"
#define ROMFILE_MAX_PENDING   (128*1024)
#define ROMFILE_MAX_LEN   (128*1024)
#define CWMP_MQ_FLAG_PATH "/tmp/cwmp/tr069agent_mq"
#define PROJID 2
#define TRX_MAGIC3		0x33524448	/* "for romfile" */
int mtd_erasesize = 0;
int isRomfileCrcErr;
#define TMP_CONFIG_FILE_PATH "/tmp/ctromfile.cfg"
#define OPEN_FAIL_MSG "Open fail: %s %s\n"
#define DEF_ROMFILE_ERR_MSG_2 "Running romfile format is wrong.Please to check again!!!\n"
#define PRODUCTLINECWMPFILEPATH "/tmp/productlinecwmpfile"
#define PRODUCTLINEPARAMAGIC 0x12344321
#define DEF_ROMFILE_PATH_EPON 	"/userfs/ctromfile_epon.cfg"
#define DEF_ROMFILE_ERR_MSG "Default romfile format is wrong.Please to check again!!!\n"
#define VERIFY_ERR_MSG_2 "verify_romfile func: None %s node, will get it from running romfile\n"
#define VERIFY_ERR_MSG "verify_romfile func: None %s node, will get it from default romfile\n"
#define VERIFY_ERR_MSG_002 "verify_romfile func: None %s node from running romfile\n"
#define VERIFY_ERR_MSG_001 "verify_romfile func: None %s node from default romfile\n"
#define ROMFILE_ERR_MSG "The Romfile format is wrong!!!"
#define	RUN_ROMFILE_FLAG	0
#define	BK_ROMFILE_FLAG	1
#define	DEF_ROMFILE_FLAG	2
#define	DEFFILESYSTEM_ROMFILE_FLAG	3
#define ITMS_ROMFILE_FLAG		4
#define BR_FILE_NAME "/tmp/tc_backupromfile"
#define ITMSR_FILE_NAME "/tmp/tc_itmsromfile"
#define USE_DEF_ROMFILE_MSG "Romfile format is wrong, we use default romfile to replace current setting romfile!!\n"
#define FAIL -1
#define SUCCESS 0
#define MAX_SMUX_NUM 8
#if defined(TCSUPPORT_CMCC)
#define MAX_STATIC_ROUTE6_NUM 32
#else
#define MAX_STATIC_ROUTE6_NUM 16 
#endif
#define MAX_DDNS_NUM 64


#if defined(TCSUPPORT_CT_SIMCARD_SEPARATION)
int isNeedWait2SaveFlash(mxml_node_t* tree){
	char nodeName[64] = {0};
	char waitSave[8] = {0};
	char cpeFlag[8] = {0};
	char simAuthRet[8] = {0};
	char cardExist[8] = {0};
	
	memset(nodeName, 0, sizeof(nodeName) );
	snprintf(nodeName, sizeof(nodeName), SIMCARD_ENTRY_NODE);

	cfg_get_object_attr(nodeName, "cpeFlag", cpeFlag, sizeof(cpeFlag));
	if( cpeFlag != NULL && strncmp(cpeFlag, "1", sizeof(cpeFlag)) == 0)/*cpeFlag is card*/
	{	
		
		cfg_get_object_attr(nodeName, "cardExist", cardExist, sizeof(cardExist));
		cfg_get_object_attr(nodeName, "simAuthRet", simAuthRet, sizeof(simAuthRet));
		cfg_get_object_attr(nodeName, "itmsActionStart", waitSave, sizeof(waitSave));
		if( cardExist != NULL && strncmp(cardExist, "1", sizeof(cardExist)) == 0
			&& simAuthRet != NULL && strncmp(simAuthRet, "1", sizeof(simAuthRet)) == 0
			&& waitSave != NULL && strncmp(waitSave, "1", sizeof(waitSave)) == 0)
		{
				printf("[%s:%d]need wait to save flash!!!======>\n",__FUNCTION__,__LINE__);
				return 1;
		}
	}
	
	return 0;
}
#endif

static const unsigned long crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
unsigned long compute_crc32buf(char *buf, size_t len)
{
      unsigned long crc;

      crc = 0xFFFFFFFF;

      for ( ; len; --len, ++buf)
      {
            crc = UPDC32(*buf, crc);
      }

      return (unsigned long)crc;
}
#endif

int compute_checksum(char *inputfile,char *outputfile)
{
	char *buf = NULL;	
	int fdin = -1,fdout = -1;
	unsigned long read_count = 0;
	struct trx_header txheader;
	int maxlen = ROMFILE_MAX_LEN-sizeof(struct trx_header);
	char cmd[64]={0};
	int res = 0;
	long write_count = 0;

	memset(&txheader,0,sizeof(struct trx_header));

	/*first compute checksum and set txheader parameters*/
#if defined(TCSUPPORT_CT_ZIPROMFILE)
		buf = malloc(ROMFILE_MAX_LEN+ROMFILE_MAX_PENDING+1);
		maxlen += ROMFILE_MAX_PENDING;
#else
		buf= malloc(mtd_erasesize + 1);
#endif
	if (buf == NULL) 
	{
		printf("tc_checksum malloc failed\n");
		return -1;
	}

	fdin = open(inputfile, O_RDONLY);
	if(fdin <0)
	{
		printf("compute_checksum: open file(%s) failed\n",inputfile);
		goto error;
	}

	lseek(fdin,0,SEEK_SET);

#if defined(TCSUPPORT_CT_ZIPROMFILE)
		read_count = read(fdin,buf,ROMFILE_MAX_LEN+ROMFILE_MAX_PENDING);
#else
		read_count = read(fdin,buf,mtd_erasesize);
#endif
	if(read_count > maxlen)
	{
		printf("compute_checksum:len is over!read size is %lu,maxlen is %d\n",read_count,maxlen);
		goto error;
	}
	
	txheader.magic = TRX_MAGIC3;
	txheader.romfile_len= read_count;
	txheader.header_len= sizeof(struct trx_header);
	txheader.len = read_count + sizeof(struct trx_header);
	txheader.crc32 = compute_crc32buf((char *)buf, read_count);


	fdout = open(outputfile, O_RDWR | O_CREAT | O_TRUNC);
	if(fdout < 0)
	{
		printf("compute_checksum open file(%s) failed\n",STORE_ROMFILE_FILE_NAME);
		goto error;
	}

	write_count = write(fdout,&txheader,sizeof(struct trx_header));
	if ( write_count < 0 )
	{
		printf("compute_checksum write file(%s) failed\n",STORE_ROMFILE_FILE_NAME);
		goto error;
	}

	res = lseek(fdout,write_count,SEEK_SET);

	write_count = write(fdout,buf,read_count);
	if(write_count != read_count)
	{
		printf("compute_checksum write_count is not equel to read_count\n");
		printf("write_count is %lu\t\t",write_count);
		printf("read_count is %lu\n",read_count);
		goto error;
	}

	free(buf);
	close(fdin);
	close(fdout);
#if defined(TCSUPPORT_CT_ZIPROMFILE)
		memset(cmd, 0, sizeof(cmd));	
		sprintf(cmd, "/bin/cp -f %s %s%s", outputfile, GZ_TMP_PATH, CT_ROMFILE_FILE_NAME);
		system(cmd);
		sprintf(cmd, "/bin/tar -czvf %s%s -C %s %s", GZ_TMP_PATH, GZ_ROMFILE_FILE_NAME, GZ_TMP_PATH, CT_ROMFILE_FILE_NAME);
		system(cmd);
		unlink(outputfile);
		sprintf(cmd, "/bin/mv -f %s%s %s", GZ_TMP_PATH, GZ_ROMFILE_FILE_NAME, outputfile);
		system(cmd);
#endif

	return 0;

error:
	if(buf)
		free(buf);
	if(fdin >= 0)
		close(fdin);
	if(fdout >= 0)
		close(fdout);
	return -1;
}

#if defined(TCSUPPORT_CT_DSL_EX)
int update_wan_node(void)
{
	char dst_nodeName[MAXLEN_NODE_NAME] = {0};
	char src_nodeName[MAXLEN_NODE_NAME] = {0};	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char path[MAXLEN_NODE_NAME] = {0};	
	int i,j;
	int dsl_state = 0, mode;		
	char dslMode[8] = {0};
	char node[MAXLEN_NODE_NAME] = {0};
	char xdsl_status[8] = {0};

	/*check: WanATM/WanPTM*/
	blapi_xdsl_get_xdsl_linkstatus(xdsl_status, &dsl_state);
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYS_ENTRY_NODE, sizeof(nodeName) - 1);
	if(dsl_state == 1) 
	{	/*ADSL*/
		strncpy(node, WANATM_NODE, sizeof(node) - 1);
	}
	else if(dsl_state == 2) 
	{	/*VDSL*/
		strncpy(node, WANPTM_NODE, sizeof(node) - 1);
	}
	else
	{	/*down*/	
		cfg_obj_get_object_attr(nodeName, "DslMode", 0, dslMode, sizeof(dslMode));
		if(strcmp(dslMode,"ATM") == 0)
		{
			strncpy(node, WANATM_NODE, sizeof(node) - 1);
		}
		else
		{
			strncpy(node, WANPTM_NODE, sizeof(node) - 1);
		}
	}
	
	/*clean pvc node before backup*/
	for(i = 0 ; i < PVC_NUM; i++)
	{		
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s.pvc.%d", node, i);
		if(cfg_obj_query_object(path, NULL, NULL) > 0)
		{
		cfg_delete_object(path);
	}
	}

	/*clear  common*/
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s.common", node);
	if(cfg_obj_query_object(path, NULL, NULL) > 0)
	{
	cfg_delete_object(path);
	}

	/*copy common node*/
	memset(src_nodeName, 0, sizeof(src_nodeName));
	strncpy(src_nodeName, WAN_COMMON_NODE, sizeof(src_nodeName) - 1);
	if(cfg_obj_query_object(src_nodeName, NULL, NULL) > 0)
	{		/*node  exist*/
		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.common", node);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);	
	}

	/*copy pvc node*/
	for(i = 1 ; i <= PVC_NUM; i++)
	{
		memset(src_nodeName, 0, sizeof(src_nodeName));
		snprintf(src_nodeName, sizeof(src_nodeName), "root.wan.pvc.%d", i);
		if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
		{		/*node not exist*/
			continue;
		}

		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.pvc.%d", node, i);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);	
		
		/*Entry*/		
		for(j = 1 ; j <= MAX_SMUX_NUM; j++)
		{
			memset(src_nodeName, 0, sizeof(src_nodeName));
			snprintf(src_nodeName, sizeof(src_nodeName), "root.wan.pvc.%d.entry.%d", i, j);
			if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
			{		/*node not exist*/
				continue;
			}

			memset(dst_nodeName, 0, sizeof(dst_nodeName));
			snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.pvc.%d.entry.%d", node, i, j);

			cfg_check_create_object(dst_nodeName);
			cfg_obj_copy_private(dst_nodeName, src_nodeName);	
		}
	}

	copy_to_xdsl_node("root.virserver", MAX_SMUX_NUM, 0);
	copy_to_xdsl_node("root.route", MAX_STATIC_ROUTE_NUM, 0);
	copy_to_xdsl_node("root.route6", MAX_STATIC_ROUTE6_NUM, 0);
	copy_to_xdsl_node("root.dmz", MAX_WAN_IF_INDEX, 0);
	copy_to_xdsl_node("root.igmpproxy", -1, 0);
	copy_to_xdsl_node("root.mldproxy", -1, 0);
	copy_to_xdsl_node("root.ddns", MAX_DDNS_NUM, 1);

	return 0;
}

void copy_to_xdsl_node(char *src_node, int entry_num, int common_flag)
{
	char src_nodeName[MAXLEN_NODE_NAME] = {0};
	char dst_nodeName[MAXLEN_NODE_NAME] = {0};	
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char node[MAXLEN_NODE_NAME] = {0};
	char path[MAXLEN_NODE_NAME] = {0};
	char dslMode[8] = {0};
	int i=0;

	/*check ATM or PTM*/	
	memset(nodeName, 0, sizeof(nodeName));
	strncpy(nodeName, SYS_ENTRY_NODE, sizeof(nodeName) - 1);
	cfg_obj_get_object_attr(nodeName, "DslMode", 0, dslMode, sizeof(dslMode));
	if(strcmp(dslMode,"ATM") == 0)
	{
		snprintf(node, sizeof(node), "%satm", src_node);
	}
	else
	{
		snprintf(node, sizeof(node), "%sptm", src_node);
	}

	if(entry_num < 0)
	{
		/*clear xdsl node*/
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s.entry", node);
		if(cfg_obj_query_object(path, NULL, NULL) > 0)
		{
		cfg_delete_object(path);
		}

		/*copy to xdsl node*/
		memset(src_nodeName, 0, sizeof(src_nodeName));
		snprintf(src_nodeName, sizeof(src_nodeName), "%s.entry", src_node);
		
		if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
		{/*node not exist*/
			return;
		}

		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.entry", node);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);			
	}
	else
	{
		/*clear xdsl node*/
		for(i = 1 ; i <= entry_num; i++)
		{
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "%s.entry.%d", node, i);
			if(cfg_obj_query_object(path, NULL, NULL) > 0)
			{
			cfg_delete_object(path);
		}
		}

		/*copy to xdsl node*/
		for(i = 1 ; i <= entry_num; i++)
		{
			memset(src_nodeName, 0, sizeof(src_nodeName));
			snprintf(src_nodeName, sizeof(src_nodeName), "%s.entry.%d", src_node, i);
			
			if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
			{/*node not exist*/
				continue;
			}

			memset(dst_nodeName, 0, sizeof(dst_nodeName));
			snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.entry.%d", node, i);
			cfg_check_create_object(dst_nodeName);
			cfg_obj_copy_private(dst_nodeName, src_nodeName);
		}
	}

	if(common_flag == 1)
	{
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s.common", node);
		if(cfg_obj_query_object(path, NULL, NULL) > 0)
		{
		cfg_delete_object(path);
		}
		
		memset(src_nodeName, 0, sizeof(src_nodeName));
		snprintf(src_nodeName, sizeof(src_nodeName), "%s.common", src_node);
		
		if(cfg_obj_query_object(src_nodeName, NULL, NULL) < 0)
		{/*node not exist*/
			return;
		}
		
		memset(dst_nodeName, 0, sizeof(dst_nodeName));
		snprintf(dst_nodeName, sizeof(dst_nodeName), "%s.common", node);
		cfg_check_create_object(dst_nodeName);
		cfg_obj_copy_private(dst_nodeName, src_nodeName);			
	}
	return;
}

#endif

int cfg_evt_write_romfile_to_flash(void)
{
	FILE *fp;
	char buf[4] = {0};

#if 0
	fp = fopen(TCAPI_SAVE_STATUS_FILE, "r");
	if(fp != NULL){ 			
		tcdbg_printf("[%s]%d,getpid=%d tcapi_save is still doing\n", __FUNCTION__, __LINE__,getpid()); 			
		fclose(fp);
		return -1;
	}
#endif
#if defined(TCSUPPORT_CT_DSL_EX)
	update_wan_node();
#endif

	cfg_get_object_attr("root.sys.entry", "ctcDbusFlag", buf, sizeof(buf));
	if(strcmp(buf, "5"))
	{
		cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "saveStatus", "1");
		write2file("1", TCAPI_SAVE_STATUS_FILE);
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_TCAPI_SAVE, NULL, 0);
	}
#if defined(TCSUPPORT_CT_DBUS)
	ctc_notify2dbus(NOTIFY_TCAPI_SAVE_CALLED, NULL, NULL);
#endif
#if defined(TCSUPPORT_CT_UBUS)
	ctc_notify2ctcapd(NOTIFY_TCAPI_SAVE_CALLED, NULL, 0);
#endif
	return 0;
}

int cfg_evt_write_romfile_to_flash_raw(void)
{
	char buf[4] = {0};
	
	cfg_get_object_attr("root.sys.entry", "ctcDbusFlag", buf, sizeof(buf));
	if(strcmp(buf, "5"))
	{
		cfg_obj_send_event(EVT_CFG_INTERNAL, EVT_CFG_TCAPI_SAVE, NULL, 0);
	}
	return 0;
}

int cfg_write_romfile_to_flash(void)
{
	int flag = -1;
	char bootCmd[128]={0};
	int copyRomfileForUpgrade = 0;

#ifdef CWMP
	int ret = -1;
	long type = 1;/*tr69 must be 1*/
	int msgFlag = 0;
	cwmp_msg_t message;
	char nodeName[64] = {0};
#endif
	char attrValue[32] = {0};
	int retval = -1;
	char tmpBuf[16] = {0};

#if !defined(TCSUPPORT_NP)
#ifdef  TCSUPPORT_EPON_OAM_CTC
	blapi_pon_sendOamUpdateConf();
#endif
#endif

#ifdef CWMP
	/*
		add to support tr069 value changed.
	*/
	memset(nodeName, 0, sizeof(nodeName) );
	snprintf(nodeName, sizeof(nodeName), CWMP_ENTRY_NODE);
	
	cfg_get_object_attr(nodeName, "tr069Commit", attrValue, sizeof(attrValue));

	if( atoi(attrValue) == 0  ||  attrValue[0] != '\0' )
	{
		memset(&message,0,sizeof(cwmp_msg_t));
		message.cwmptype = VALUE_CHANGED;
		ret = sendmegq(type,&message,msgFlag);

		if(ret < 0)
		{
			printf("\r\nsend message error!!");
		}
	}	
	
	  cfg_set_object_attr(nodeName, "tr069Commit", "0");
	
	memset(attrValue, 0, sizeof(attrValue));
	cfg_get_object_attr(nodeName, "copyRomfileForUpgrade", attrValue, sizeof(attrValue));

	if(attrValue[0] != '\0' )
	{
		copyRomfileForUpgrade = atoi(attrValue);
	}	
	
	cfg_set_object_attr(nodeName, "copyRomfileForUpgrade", "0");
	
#endif

	cfg_save_object(CFG_OBJ_FILE_PATH);

	flag = compute_checksum(CFG_OBJ_FILE_PATH,STORE_ROMFILE_FILE_NAME);
	if(flag < 0)
	{
		tcdbg_printf("tcapi_save_req:Fail to compute checksum!\n");
		retval = -4;
		return -1;
	}
	
	if(copyRomfileForUpgrade)
	{
		tcdbg_printf("\r\ntcapi save: Not save to flash, only copy to tmp\r\n");
		memset(bootCmd, 0, sizeof(bootCmd));	
		snprintf(bootCmd, sizeof(bootCmd), "cp -f %s %s", 
			STORE_ROMFILE_FILE_NAME, CWMP_UPGRADE_ROMFILE_FILE_NAME);
		tcdbg_printf("\r\ntcapi save: exe cmd [%s]\r\n", bootCmd);
		system(bootCmd);
		unlink(STORE_ROMFILE_FILE_NAME);
		return -1; 	
	}


	snprintf(bootCmd, sizeof(bootCmd), UPDATE_ROMFILE_CMD,STORE_ROMFILE_FILE_NAME);	
	system(bootCmd);
	usleep(100000);

	snprintf(bootCmd, sizeof(bootCmd), TC_FLASH_ERASE_SECTOR_CMD, 
		(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(bootCmd);
	
	memset(bootCmd,0,sizeof(bootCmd));
	snprintf(bootCmd, sizeof(bootCmd), TC_FLASH_WRITE_CMD, 
		STORE_ROMFILE_FILE_NAME, (unsigned long)BACKUPROMFILE_RA_SIZE, 
		(unsigned long)BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
	system(bootCmd);
	usleep(100000);
	
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "saveStatus", "0");
	unlink(TCAPI_SAVE_STATUS_FILE);
	
	unlink(STORE_ROMFILE_FILE_NAME);
	cfg_get_object_attr(CWMP_ENTRY_NODE, "FWCompleteFlag", tmpBuf, sizeof(tmpBuf));
	if('\0' != tmpBuf[0] && 0 == atoi(tmpBuf)){
		cfg_set_object_attr(CWMP_ENTRY_NODE, "FWCompleteFlag", "1");
	}
	
	return 0;
	
}


int check_checksum(char *configinfo,char *outputfile, char *gzpath)
{
	 char *buf = NULL;	 
	 int fdin = -1,fdout = -1;
	 unsigned long read_count = 0,write_count = 0;
	 struct trx_header *pheader;
	 char cmd[64]={0};
	 char tmpgzpath[64]={0};
	 char tmpcfgfilepath[128]={0};

#if defined(TCSUPPORT_CT_ZIPROMFILE)
	 buf = malloc(ROMFILE_MAX_LEN+ROMFILE_MAX_PENDING+1);
#else
	 buf = malloc(mtd_erasesize + 1);	
#endif
	 if (buf == NULL) 
	 {
		 tcdbg_printf("[%s:%d]:tc_checksum malloc failed\n",__FUNCTION__,__LINE__);
		 return -1;
	 }

	memset(tmpcfgfilepath, 0, sizeof(tmpcfgfilepath));
	snprintf(tmpcfgfilepath, sizeof(tmpcfgfilepath), "%s", configinfo);
	 
#if defined(TCSUPPORT_CT_ZIPROMFILE)
	 memset(cmd, 0, sizeof(cmd));
	 memset(tmpgzpath, 0, sizeof(tmpgzpath));
	 memset(tmpcfgfilepath, 0, sizeof(tmpcfgfilepath));

	if(gzpath != NULL)
	{
		snprintf(tmpgzpath, sizeof(tmpgzpath), "%s", gzpath);
		snprintf(tmpcfgfilepath, sizeof(tmpcfgfilepath), "%s%s", gzpath, CT_ROMFILE_FILE_NAME);
	}
	else
	{
		snprintf(tmpgzpath, sizeof(tmpgzpath), "%s", GZ_TMP_PATH);
		snprintf(tmpcfgfilepath, sizeof(tmpcfgfilepath), "%s%s", GZ_TMP_PATH, CT_ROMFILE_FILE_NAME);
	}
	unlink(tmpcfgfilepath);

	snprintf(cmd, sizeof(cmd), "mkdir -p %s", CWMP_GZ_TMP_PATH);
	system(cmd);
	
	snprintf(cmd, sizeof(cmd), "tar -xzvf %s -C %s", configinfo, tmpgzpath);
	system(cmd);
#endif

	fdin = open(tmpcfgfilepath, O_RDONLY);
	 if(fdin < 0)
	 {
		 tcdbg_printf("[%s:%d]:tc_checksum open file(%s) failed===>\n",__FUNCTION__,__LINE__,tmpcfgfilepath);
		 goto error;
	 }

#if defined(TCSUPPORT_CT_ZIPROMFILE)
	read_count = read(fdin,buf,ROMFILE_MAX_LEN+ROMFILE_MAX_PENDING);
#else
 	read_count = read(fdin,buf,mtd_erasesize);
#endif
	 /*first need to check romfile correct or not*/
	 if(read_count < sizeof(struct trx_header))
	 {
		 tcdbg_printf("[%s:%d]:len is less then trx_crc_header,read_count=%lu,trx_header len is %d===>\n",__FUNCTION__,__LINE__,read_count,sizeof(struct trx_header));
		 goto error;
	 }

	 pheader = (struct trx_header *) buf;	 
	 
	 if (pheader->magic != TRX_MAGIC3) 
	 {
		 tcdbg_printf("[%s:%d]:magic num=[%x] is error===>\n",__FUNCTION__,__LINE__,pheader->magic);
		 goto error;
	 }
	 else
	 {
		 if(pheader->crc32 != compute_crc32buf((char *)(buf+sizeof(struct trx_header)), (pheader->len-sizeof(struct trx_header))))
		 {
#if defined(CT_COM_DEVICEREG) && defined(TCSUPPORT_ITMS_CONFIG_AS_DEFAULT)
				 isRomfileCrcErr = 1;
#endif
			 tcdbg_printf("\r\n[%s:%d]: %s romfile crc check error!",__FUNCTION__,__LINE__, configinfo);
			 goto error;
		 }
	 }

	 /*then,if check pass,fetch real romfile to "/tmp/tc_romfile.conf"*/
	 fdout = open(outputfile, O_RDWR | O_CREAT | O_TRUNC);
	 if(fdout < 0)
	 {
		 tcdbg_printf("[%s:%d]:tc_checksum open file(%s) failed===>\n",__FUNCTION__,__LINE__,outputfile);
		 goto error;
	 }

	 if ( pheader->len <= sizeof(struct trx_header) )
	 {
		 tcdbg_printf("[%s:%d]:pheader->len error\n",__FUNCTION__,__LINE__);
		 goto error;
	 }

	 write_count = write(fdout,buf+sizeof(struct trx_header),pheader->len-sizeof(struct trx_header));
	 if(write_count != pheader->len-sizeof(struct trx_header))
	 {
		 tcdbg_printf("[%s:%d]:tc_checksum write_count is not equel to pheader->len-sizeof(struct trx_crc_header)\n",__FUNCTION__,__LINE__);
		 tcdbg_printf("[%s:%d]:write_count is %lu\t\t",__FUNCTION__,__LINE__,write_count);
		 tcdbg_printf("[%s:%d]:pheader->len-sizeof(struct trx_crc_header) is %d\n",__FUNCTION__,__LINE__,pheader->len-sizeof(struct trx_header));
		 goto error;
	 }

	 free(buf);
	 close(fdin);
	 close(fdout);
	 return 0;
	 
	error:
	snprintf(cmd, sizeof(cmd), "cp %s /tmp/check_fail_romfile.cfg", tmpcfgfilepath);
		system(cmd);
	
	 if(buf)
		 free(buf);
	 if(fdin >= 0)
		 close(fdin);
	 if(fdout >= 0)
		 close(fdout);
	 return -1;
}

#ifdef TCSUPPORT_PRODUCTIONLINE
int tcgetProLinePara(void*buf,int flag)
{
	char cmdbuf[200] = {0};
	FILE *fp = NULL;	
	int res = 0;

	switch(flag)
	{
		case 0:
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)
			snprintf(cmdbuf, sizeof(cmdbuf), TC_FLASH_READ_CMD, 
			PRODUCTLINECWMPFILEPATH, (unsigned long)sizeof(proline_Para), 
			(unsigned long)PROLINE_CWMPPARA_RA_OFFSET,RESERVEAREA_NAME);
#else
			snprintf(cmdbuf, sizeof(cmdbuf), TC_FLASH_READ_CMD, 
			PRODUCTLINECWMPFILEPATH,(unsigned long)PROLINE_CWMPPARA_RA_SIZE, 
			(unsigned long)PROLINE_CWMPPARA_RA_OFFSET, RESERVEAREA_NAME);
#endif		
			system(cmdbuf);
			
			/*open file*/
			fp = fopen(PRODUCTLINECWMPFILEPATH, "r");
			if (fp == NULL) 
			{
				printf("\r\ngetProLineCwmpPara:open file(%s) fail!",PRODUCTLINECWMPFILEPATH);
				return FAIL;
			}
			/*read cwmp para */
			res = fread((proline_Para*)buf, sizeof(proline_Para), 1, fp);
			break;
		
		default:
			printf("\r\n[getProLinePara]Not support this para!!");
			break;

		
	}

	/*close file and unlink it*/
	if(fp)
		fclose(fp);
	unlink(PRODUCTLINECWMPFILEPATH);
	return SUCCESS;	
}
#endif

int getDefaultRomfile(char *path,unsigned int len)
{
	char romfileSelect[64]={0};
	char def_romfile[64]={0};
	int  fd=0;
	
#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND)
	proline_Para para;

	memset(&para, 0, sizeof(para));
	tcgetProLinePara(&para,0);
	para.xponmode[XPONMODELEN - 1] = 0;
	if((PRODUCTLINEPARAMAGIC == para.magic) && (para.flag & PL_CP_ROMFILE_SELECT_FLAG))
	{
		memset(romfileSelect,0,sizeof(romfileSelect));
		strncpy(romfileSelect, para.romfileselect, sizeof(romfileSelect)-1);
		snprintf(def_romfile, sizeof(def_romfile), "/userfs/%s", romfileSelect);
		fd = open(def_romfile,O_RDONLY);
		if(fd != FAIL)
		{
			close(fd);
			strncpy(path, def_romfile, len-1);
			path[len-1] = '\0';
			return 0;
		}
	}

	if((PRODUCTLINEPARAMAGIC == para.magic) && (para.flag & PL_CP_XPONMODE_FLAG))
	{
		if( 0 == strcmp("EPON", para.xponmode))
		{
			fd = open(DEF_ROMFILE_PATH_EPON,O_RDONLY);
			if(fd != FAIL){
				close(fd);
				strncpy(path, DEF_ROMFILE_PATH_EPON, len-1);
				path[len-1] = '\0';
				return 1;
			}
		}
	}
#endif	
#endif
	snprintf(def_romfile, sizeof(def_romfile), "%s", DEF_ROMFILE_PATH);
	strncpy(path, def_romfile, len-1);
	path[len-1] = '\0';
	return 2;
}

char *getDefine_DEF_ROMFILE_PATH()
{
	return DEF_ROMFILE_PATH;
}

int check_romfile(char* pathname)
{
	int ret = -1;
	int flag = -1;
	char filepath[128] = {0};

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	flag = check_checksum(pathname,LOAD_ROMFILE_FILE_NAME, GZ_TMP_PATH);
	if(flag <0)
	{
		tcdbg_printf("[%s:%d]:check_checksum=-1,Romfile checksum is wrong===>\n",__FUNCTION__,__LINE__);
		unlink(LOAD_ROMFILE_FILE_NAME);
		return -1;
	}
	strncpy(filepath, LOAD_ROMFILE_FILE_NAME, sizeof(filepath)-1);
#else
	strncpy(filepath, pathname, sizeof(filepath)-1);
#endif

	ret = cfg_load_object(filepath);
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	unlink(LOAD_ROMFILE_FILE_NAME);
#endif
	
	return ret;
}

 void loadRomfile(int types)
 {
	 char buf[128] = {0};
	 /*Write backup romfile into flash romfile block*/
	 if(BK_ROMFILE_FLAG == types)
	 {
		 snprintf(buf, sizeof(buf), TC_FLASH_READ_CMD,BR_FILE_NAME,
		 	BACKUPROMFILE_RA_SIZE, 
		 	BACKUPROMFILE_RA_OFFSET,RESERVEAREA_NAME);
		 system(buf);
		 printf("[BK_ROMFILE_FLAG]%s\n",buf);
		 usleep(100000);
	 }
	 /*Write backup romfile into flash romfile block*/
	 else if(DEF_ROMFILE_FLAG == types)
	 {
			 snprintf(buf, sizeof(buf), TC_FLASH_READ_CMD,DR_FILE_NAME,
			 DEFAULTROMFILE_RA_SIZE,
			 DEFAULTROMFILE_RA_OFFSET,RESERVEAREA_NAME);
			 system(buf);
			 printf("[DEF_ROMFILE_FLAG]%s\n",buf);
			 usleep(100000);
	 }
	 else if(ITMS_ROMFILE_FLAG == types)
	 {
		 snprintf(buf, sizeof(buf), TC_FLASH_READ_CMD,ITMSR_FILE_NAME,
		 	BACKUPROMFILE_RA_SIZE, (unsigned long)0,"reserve1");
		 system(buf);
		 printf("[ITMS_ROMFILE_FLAG]%s\n",buf);
		 usleep(100000);
	 }
	 else
	 {
		 /*do nothing now*/
	 }
	 
 }
 void loadBackupRomfile(void)
 {
	 loadRomfile(BK_ROMFILE_FLAG);
 }
 void loadDefaultRomfile(void)
 {
	 loadRomfile(DEF_ROMFILE_FLAG);
 }
 void loadItmsRomfile(void)
 {
	 loadRomfile(ITMS_ROMFILE_FLAG);
 }

 mxml_node_t *load_checksum_def_romfile(char *file)
 {
	 FILE *fp=NULL;
	 mxml_node_t *tree=NULL;
	 int flag = -1;
	 int ret = -1;
	 
	 flag = check_checksum(file,LOAD_ROMFILE_FILE_NAME, GZ_TMP_PATH);
	 if(flag < 0)
	 {
		 tcdbg_printf("[%s:%d]:check_checksum=-1,Romfile checksum is wrong===>\n",__FUNCTION__,__LINE__);
		 return NULL;
	 }
 
	 ret = cfg_load_object(LOAD_ROMFILE_FILE_NAME);
	 if(ret < 0)
 	 {
 		 return NULL;
 	 }
	 else
	 {
		 fp = fopen(LOAD_ROMFILE_FILE_NAME, "r");
		 if(fp == NULL){
			 printf(OPEN_FAIL_MSG, getDefine_DEF_ROMFILE_PATH(), strerror(errno));
			 return NULL;
		 }

		 tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
		 if(tree == NULL){
			 printf(DEF_ROMFILE_ERR_MSG);
			 fclose(fp);
			 return NULL;
		 }
		 fclose(fp);
		 unlink(LOAD_ROMFILE_FILE_NAME);
		 
		 return tree;
	 }
 }

mxml_node_t *load_def_romfile(char *file)
{
	 FILE *fp=NULL;
	 mxml_node_t *tree=NULL;
	 char buf[128];
	 char romfilepath[64]={0};
	 int ret = -1;
 
	 ret = cfg_load_object(file);
	 if(ret < 0)
 	 {
 		 return NULL;
 	 }
	 else
	 {
		 fp = fopen(file,"r");
		 if(fp == NULL){
			 printf(OPEN_FAIL_MSG, getDefine_DEF_ROMFILE_PATH(), strerror(errno));
			 return NULL;
		 }

		 tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
		 if(tree == NULL){
			 printf(DEF_ROMFILE_ERR_MSG);
			 fclose(fp);
			 return NULL;
		 }
		 fclose(fp);
	 }
	 /*Write default romfile into flash romfile block*/
	 getDefaultRomfile(romfilepath,sizeof(romfilepath));
	 snprintf(buf, sizeof(buf), UPDATE_ROMFILE_CMD, romfilepath);
	 system(buf);
	 return tree;
}

int checkParaValid(char *buf, int len)
{
	int i;
	for (i = 0; i < len - 1; i++) {
		if ((unsigned char)buf[i] == 0) /* go to end */
			break;

		if ((unsigned char)buf[i] == 0xff) /* invalid parameter */
			return 0;
	}

	return 1;
}

#if defined(TCSUPPORT_ECNT_MAP)
#define UNCONFIGURED  0
#define CONTROLLER    1
#define AGENTER       2

#define RADIO2G  0
#define RADIO5G  1

#define MAX_BSS_NUM  4

int get_map_bh_index(int radio)
{	
	int i = 0;
	char value[10];	
	char path[64];
	
	for(i=0; i<MAX_BSS_NUM; i++)
	{
		memset(path,  0, sizeof(path));
		switch (radio)
		{
			case RADIO2G:
			{
				snprintf(path, sizeof(path), MESH_RADIO2G_BSSINFO_ENTRY_NODE, i+1);
				break;
			}
			case RADIO5G:
			{
				snprintf(path, sizeof(path), MESH_RADIO5GL_BSSINFO_ENTRY_NODE, i+1);
				break;
			}
			default:
			{
				return -1;
			}			
		}

		memset(value, 0, sizeof(value));
		if ((cfg_get_object_attr(path, "BackHaul", value, sizeof(value)) > 0) 
			&& (1 == atoi(value))) 
		{
			return (i+1);
		}
		else
		{
			continue;
		}
	}

	return -1;
}


int update_mapbhinfo_from_proline(char *buff)
{
	char nodeName[40] = {0};
	unsigned char device_role = 0;
	char BhSsid2G[40] = {0};
	char BhWpapsk2G[80] = {0};
	char BhSsid5G[40] = {0};
	char BhWpapsk5G[80] = {0};
	int bh_index_2g = 0; 
	int bh_index_5g = 0; 
		
	if ((buff != NULL) && (strlen(buff)>0))
	{			
		cfg_set_object_attr(MESH_COMMON_NODE, "DeviceRole", buff);
		cfg_set_object_attr(MESH_DEFSETTING_NODE, "DeviceRole", buff);

		memset(BhSsid2G, 0, sizeof(BhSsid2G));
		memset(BhWpapsk2G, 0, sizeof(BhWpapsk2G));
		memset(BhSsid5G, 0, sizeof(BhSsid5G));
		memset(BhWpapsk5G, 0, sizeof(BhWpapsk5G));

		cfg_get_object_attr(MESH_APCLIBH_ENTRY1_NODE, "BhProfileSsid", BhSsid2G, sizeof(BhSsid2G));
		cfg_get_object_attr(MESH_APCLIBH_ENTRY1_NODE, "BhProfileWpaPsk", BhWpapsk2G, sizeof(BhWpapsk2G));
		cfg_get_object_attr(MESH_APCLIBH_ENTRY2_NODE, "BhProfileSsid", BhSsid5G, sizeof(BhSsid5G));
		cfg_get_object_attr(MESH_APCLIBH_ENTRY2_NODE, "BhProfileWpaPsk", BhWpapsk5G, sizeof(BhWpapsk5G));

		bh_index_2g = get_map_bh_index(RADIO2G);
		bh_index_5g = get_map_bh_index(RADIO5G);

		if ((strlen(BhSsid2G) > 0) && (strlen(BhWpapsk2G) > 0) 
			&& (strlen(BhSsid5G) > 0) && (strlen(BhWpapsk5G) > 0)
			&& (bh_index_2g > 0) && (bh_index_5g > 0))
		{
			device_role = atoi(buff);

			switch(device_role)
			{
				case UNCONFIGURED:
				case CONTROLLER:
				{
					cfg_set_object_attr(MESH_APCLIBH_ENTRY1_NODE, "BhProfileValid", "0");
					cfg_set_object_attr(MESH_APCLIBH_ENTRY2_NODE, "BhProfileValid", "0");

					memset(nodeName, 0 ,sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, bh_index_2g);	
					cfg_set_object_attr(nodeName, "SSID", BhSsid2G);
					cfg_set_object_attr(nodeName, "WPAPSK", BhWpapsk2G);
					memset(nodeName, 0 ,sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY_N_NODE, bh_index_5g);
					cfg_set_object_attr(nodeName, "SSID", BhSsid5G);
					cfg_set_object_attr(nodeName, "WPAPSK", BhWpapsk5G);
					break;
				}
				case AGENTER:
				{
				#ifndef TCSUPPORT_CT_JOYME4
					cfg_set_object_attr(MESH_APCLIBH_ENTRY1_NODE, "BhProfileValid", "1");
					cfg_set_object_attr(MESH_APCLIBH_ENTRY2_NODE, "BhProfileValid", "1");
				#endif
					break;
				}
				default:
				{
					break;
				}
			}
		}	
	}

	return 0;	
}
#endif

int parseProlinePara(int flag, proline_Para *pProline, char *value)
{
	int xpon_mode = 0;
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
	blapi_pon_get_onuMode_after_PON_Start(&xpon_mode);
#endif
#endif
#if defined(TCSUPPORT_CUC)
	 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & flag))
	 {
		 return FAIL;
	 }
#endif

	 switch(flag)
	 {
#ifdef TCSUPPORT_WLAN
		 case PL_CP_SSID_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_SSID_FLAG) || (checkParaValid(pProline->ssid, SSIDLEN) == 0))
			 {
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
								 snprintf(value, SSIDLEN, "%s", "CMCC-AP1");
#else
				 snprintf(value, SSIDLEN, "%s", DEFAULT_SSID);
#endif
			 }
			 else
			 {
				 snprintf(value, SSIDLEN, "%s", pProline->ssid);
			 }
			 break;
		 case PL_CP_WPAKEY_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_WPAKEY_FLAG) || (checkParaValid(pProline->wpakey, WPAKEYLEN) == 0))
			 {
				 snprintf(value, WPAKEYLEN, "%s", DEFAULT_WPAKEY);
			 }
			 else
			 {
				 snprintf(value, WPAKEYLEN, "%s", pProline->wpakey);
			 }
			 break;
		 case PL_CP_SSID2nd_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_SSID2nd_FLAG) || (checkParaValid(pProline->ssid2nd, SSIDLEN) == 0))
			 {
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
								 snprintf(value, SSIDLEN, "%s", "CMCC-AP2");
#else
				 snprintf(value, SSIDLEN, "%s", DEFAULT_SSID);
#endif
			 }
			 else
			 {
				 snprintf(value, SSIDLEN, "%s", pProline->ssid2nd);
			 }
			 break;
		 case PL_CP_WPAKEY2nd_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_WPAKEY2nd_FLAG) || (checkParaValid(pProline->wpakey2nd, WPAKEYLEN) == 0))
			 {
				 snprintf(value, WPAKEYLEN, "%s", DEFAULT_WPAKEY);
			 }
			 else
			 {
				 snprintf(value, WPAKEYLEN, "%s", pProline->wpakey2nd);
			 }
			 break;
#ifdef TCSUPPORT_WLAN_AC
		 case PL_CP_SSIDAC_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_SSIDAC_FLAG) || (checkParaValid(pProline->ssidac, SSIDLEN) == 0))
			 {
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
								 snprintf(value, SSIDLEN, "%s", "CMCC-AP1-5G");
#else
				 snprintf(value, SSIDLEN, "%s", DEFAULT_SSID);
#endif
			 }
			 else
			 {
				 snprintf(value, SSIDLEN, "%s", pProline->ssidac);
			 }
			 break;
		 case PL_CP_WPAKEYAC_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_WPAKEYAC_FLAG) || (checkParaValid(pProline->wpakeyac, WPAKEYLEN) == 0))
			 {
				 snprintf(value, WPAKEYLEN, "%s", DEFAULT_WPAKEY);
			 }
			 else
			 {
				 snprintf(value, WPAKEYLEN, "%s", pProline->wpakeyac);
			 }
			 break;
		 case PL_CP_SSID2ndAC_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_SSID2ndAC_FLAG) || (checkParaValid(pProline->ssid2ndac, SSIDLEN) == 0))
			 {
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP_CMCC)
								 snprintf(value, SSIDLEN, "%s", "CMCC-AP2-5G");
#else
				 snprintf(value, SSIDLEN, "%s", DEFAULT_SSID);
#endif
			 }
			 else
			 {
				 snprintf(value, SSIDLEN, "%s", pProline->ssid2ndac);
			 }
			 break;
		 case PL_CP_WPAKEY2ndAC_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_WPAKEY2ndAC_FLAG) || (checkParaValid(pProline->wpakey2ndac, WPAKEYLEN) == 0))
			 {
				 snprintf(value, WPAKEYLEN, "%s", DEFAULT_WPAKEY);
			 }
			 else
			 {
				 snprintf(value, WPAKEYLEN, "%s", pProline->wpakey2ndac);
			 }
			 break;
#endif
#endif
		 case PL_CP_WEBPWD_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_WEBPWD_FLAG) || (checkParaValid(pProline->webpwd, WEBPWDLEN) == 0))
			 {
				 snprintf(value, WEBPWDLEN, "%s", DEFAULT_WEBPWD);
			 }
			 else
			 {
				 snprintf(value, WEBPWDLEN, "%s", pProline->webpwd);
			 }
			 break;
#if !defined(TCSUPPORT_CT_WAN_PTM)
		 case PL_CP_PPPUSRNAME_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_PPPUSRNAME_FLAG) || (checkParaValid(pProline->pppusrname, PPPUSRNAMELEN) == 0))
			 {
				 snprintf(value, PPPUSRNAMELEN, "%s", DEFAULT_PPPUSRNAME);
			 }
			 else
			 {
				 snprintf(value, PPPUSRNAMELEN, "%s", pProline->pppusrname);
			 }
			 break;
		 case PL_CP_PPPPWD_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_PPPPWD_FLAG) || (checkParaValid(pProline->ppppwd, PPPPWDLEN) == 0))
			 {
				 snprintf(value, PPPPWDLEN, "%s", DEFAULT_PPPPWD);
			 }
			 else
			 {
				 snprintf(value, PPPPWDLEN, "%s", pProline->ppppwd);
			 }
			 break;
		 case PL_CP_CFEUSRNAME_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_CFEUSRNAME_FLAG) || (checkParaValid(pProline->cfeusrname, CFEUSRNAMELEN) == 0))
			 {
				 snprintf(value, CFEUSRNAMELEN, "%s", DEFAULT_CFEUSRNAME);
			 }
			 else
			 {
				 snprintf(value, CFEUSRNAMELEN, "%s", pProline->cfeusrname);
			 }
			 break;
		 case PL_CP_CFEPWD_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_CFEPWD_FLAG) || (checkParaValid(pProline->cfepwd, CFEPWDLEN) == 0))
			 {
				 snprintf(value, CFEPWDLEN, "%s", DEFAULT_CFEPWD);
			 }
			 else
			 {
				 snprintf(value, CFEPWDLEN, "%s", pProline->cfepwd);
			 }
			 break;
		 case PL_CP_XPONSN_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_XPONSN_FLAG))
			 {
				 snprintf(value, XPONSNLEN, "%s", DEFAULT_XPONSN);
			 }
			 else
			 {
				 snprintf(value, XPONSNLEN, "%s", pProline->xponsn);
			 }
			 break;
		 case PL_CP_XPONPWD_FLAG:
			if(xpon_mode != 1)
				return FAIL;

			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_XPONPWD_FLAG))
			 {
#if defined(TCSUPPORT_CMCCV2)
				snprintf(value, XPONPWDLEN, "%s", "");
#else
				 snprintf(value, XPONPWDLEN, "%s", DEFAULT_XPONPWD);
#endif
			 }
			 else
			 {
				 snprintf(value, XPONPWDLEN, "%s", pProline->xponpwd);
			 }
			 break;
		case PL_GPON_REGID_FLAG:
			if(xpon_mode != 6)
				return FAIL;

			if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_GPON_REGID_FLAG) || (checkParaValid(pProline->gponregid, XPONPWDLEN) == 0))
			{
#if defined(TCSUPPORT_CMCCV2)
				snprintf(value, XPONPWDLEN, "%s", "");
#else
				snprintf(value, XPONPWDLEN, "%s", DEFAULT_XPONPWD);
#endif
			}
			else
			{
				snprintf(value, XPONPWDLEN, "%s", pProline->gponregid);
			}
			break;
#if !defined(TCSUPPORT_ECNT_MAP)
		 case PL_CP_MACADDR_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_MACADDR_FLAG) || (checkParaValid(pProline->macaddr, MACADDRLEN) == 0))
			 {
				 snprintf(value, MACADDRLEN, "%s", DEFAULT_MACADDR);
			 }
			 else
			 {
				 snprintf(value, MACADDRLEN, "%s", pProline->macaddr);
			 }
			 break; 	 
#endif			 
		 case PL_CP_XPONMODE_FLAG:
			 if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_XPONMODE_FLAG) || (checkParaValid(pProline->xponmode, XPONMODELEN) == 0))
			 {
				 snprintf(value, XPONMODELEN, "%s", DEFAULT_XPONMODE);
			 }
			 else
			 {
				 snprintf(value, XPONMODELEN, "%s", pProline->xponmode);
			 }
			 break;  
#endif
#if defined(TCSUPPORT_ECNT_MAP)
		case PL_CP_MAPDEVROLE_FLAG:
		if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_MAPDEVROLE_FLAG) || (checkParaValid(pProline->mapdevrole, MAPDEVROLELEN) == 0))
			{
			#ifdef TCSUPPORT_CT_JOYME4
				snprintf(value, MAPDEVROLELEN, "%s", "2");
			#else
				snprintf(value, MAPDEVROLELEN, "%s", "1");
			#endif
			}
			else
			{
				snprintf(value, MAPDEVROLELEN, "%s", pProline->mapdevrole);
			}
			break;
		case PL_CP_MAPBHSSID2G_FLAG:
			if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_MAPBHSSID2G_FLAG) || (checkParaValid(pProline->mapbhssid2g, SSIDLEN) == 0))
			{
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP)
				snprintf(value, SSIDLEN, "%s", "CMCC-AP2");
#else
				snprintf(value, SSIDLEN, "%s", DEFAULT_SSID);
#endif
			}
			else
			{
				snprintf(value, SSIDLEN, "%s", pProline->mapbhssid2g);
			}
			break;
		case PL_CP_MAPBHWPAKEY2G_FLAG:
			if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_MAPBHWPAKEY2G_FLAG) || (checkParaValid(pProline->mapbhwpakey2g, WPAKEYLEN) == 0))
			{
				snprintf(value, WPAKEYLEN, "%s", DEFAULT_WPAKEY);
			}
			else
			{
				snprintf(value, WPAKEYLEN, "%s", pProline->mapbhwpakey2g);
			}			
			break;
		case PL_CP_MAPBHSSID5G_FLAG:
			if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_MAPBHSSID5G_FLAG) || (checkParaValid(pProline->mapbhssid5g, SSIDLEN) == 0))
			{
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP)
				snprintf(value, SSIDLEN, "%s", "CMCC-AP2-5G");
#else
				snprintf(value, SSIDLEN, "%s", DEFAULT_SSID);
#endif
			}
			else
			{
				snprintf(value, SSIDLEN, "%s", pProline->mapbhssid5g);
			}
			break;
		case PL_CP_MAPBHWPAKEY5G_FLAG:
			if ((PRODUCTLINEPARAMAGIC != pProline->magic) || !(pProline->flag & PL_CP_MAPBHWPAKEY5G_FLAG) || (checkParaValid(pProline->mapbhwpakey5g, WPAKEYLEN) == 0))
			{
				snprintf(value, WPAKEYLEN, "%s", DEFAULT_WPAKEY);
			}
			else
			{
				snprintf(value, WPAKEYLEN, "%s", pProline->mapbhwpakey5g);
			}
			break;
#endif
		 default:
			 printf("\r\nNo such parameter.");
			 return FAIL;
			 break;
 }

 return SUCCESS;
}

 Key_Parameters2_Info prolinePara[]=
 {
#ifdef TCSUPPORT_WLAN
	 {PL_CP_SSID_FLAG,WLAN_ENTRY1_NODE,"root.WLan.Entry0","SSID",NULL},/*SSID*/
	 {PL_CP_WPAKEY_FLAG,WLAN_ENTRY1_NODE,"root.WLan.Entry0","WPAPSK",NULL},/*WPAKEY*/
	 {PL_CP_SSID2nd_FLAG,WLAN_ENTRY2_NODE,"root.WLan.Entry1","SSID",NULL},/*SSID2nd*/
	 {PL_CP_WPAKEY2nd_FLAG,WLAN_ENTRY2_NODE,"root.WLan.Entry1","WPAPSK",NULL},/*WPAKEY2nd*/
#ifdef TCSUPPORT_WLAN_AC
	 {PL_CP_SSIDAC_FLAG,WLAN11AC_ENTRY1_NODE,"root.WLan11ac.Entry0","SSID",NULL},/*SSIDAC*/
	 {PL_CP_WPAKEYAC_FLAG,WLAN11AC_ENTRY1_NODE,"root.WLan11ac.Entry0","WPAPSK",NULL},/*WPAKEYAC*/
	 {PL_CP_SSID2ndAC_FLAG,WLAN11AC_ENTRY2_NODE,"root.WLan11ac.Entry1","SSID",NULL},/*SSID2ndAC*/
	 {PL_CP_WPAKEY2ndAC_FLAG,WLAN11AC_ENTRY2_NODE,"root.WLan11ac.Entry1","WPAPSK",NULL},/*WPAKEY2ndAC*/
#endif
#endif
	 {PL_CP_WEBPWD_FLAG,ACCOUNT_ENTRY2_NODE,"root.Account.Entry1","web_passwd",NULL},/* Attention: entry1 for user account*/
	#if !defined(TCSUPPORT_CT_WAN_PTM)
	 {PL_CP_XPONSN_FLAG,GPON_ONU_NODE,"root.GPON.ONU","SerialNumber",NULL},/* Attention: entry1 for user account*/
	#endif
	 {PL_CP_XPONPWD_FLAG,GPON_ONU_NODE,"root.GPON.ONU","Password",NULL},/* Attention: entry1 for user account*/
 	 {PL_GPON_REGID_FLAG,GPON_ONU_NODE,"root.GPON.ONU","Password",NULL},
 	 {PL_CP_XPONMODE_FLAG,XPON_LINKCFG_NODE,"root.XPON.LinkCfg","Mode",NULL}, 
 #if defined(TCSUPPORT_ECNT_MAP)
	{PL_CP_MAPBHSSID2G_FLAG,MESH_APCLIBH_ENTRY1_NODE,"root.Mesh.Apclibh.Entry0","BhProfileSsid",NULL},/*update MAPBHSSID2G*/
	{PL_CP_MAPBHWPAKEY2G_FLAG,MESH_APCLIBH_ENTRY1_NODE,"root.Mesh.Apclibh.Entry0","BhProfileWpaPsk",NULL},/*update MAPBHWPAKEY2G*/
	{PL_CP_MAPBHSSID5G_FLAG,MESH_APCLIBH_ENTRY2_NODE,"root.Mesh.Apclibh.Entry1","BhProfileSsid",NULL},/*update MAPBHSSID5G*/
	{PL_CP_MAPBHWPAKEY5G_FLAG,MESH_APCLIBH_ENTRY2_NODE,"root.Mesh.Apclibh.Entry1","BhProfileWpaPsk",NULL},/*update MAPBHWPAKEY5G*/
	{PL_CP_MAPDEVROLE_FLAG,MESH_COMMON_NODE,"root.Mesh.Common",NULL,NULL},/*not update Mesh.Common.DeviceRole, use flag to update DeviceRole and wlan bh info*/
#endif
	 {0,NULL,NULL,NULL}
 };

 void handle_proline_paras(mxml_node_t* defaulttree)/*used by unopen code */
 {
#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND) 
	 char nodeName[64] = {0};
	 proline_Para para;
	 Key_Parameters2_Info *pkeyPara;
	 char buff[128] = {0};				  /* the size should be the largest among all parameters*/
	 char oriValue[32] = {0}, buffer2[256] = {0};
		 
	 memset(&para, 0, sizeof(para));
	 tcgetProLinePara(&para,0);
	 for(pkeyPara = prolinePara ; 0 != pkeyPara->flag; pkeyPara++)
	 {
		 memset(nodeName, 0, sizeof(nodeName));
		 memset(buff, 0, sizeof(buff));
		 strncpy(nodeName, pkeyPara->node, sizeof(nodeName)-1);
	 
		 if (cfg_query_object(nodeName,NULL,NULL) <= 0)
		 {
			 printf("\r\n%s:%s not found.",pkeyPara->node,pkeyPara->attr);
			 continue; 		   /* try next one*/
		 }
 
		 memset(oriValue, 0, sizeof(oriValue));
#if 0
		 if (0 <= (cfg_get_object_attr(nodeName, pkeyPara->attr, oriValue, sizeof(oriValue))))
		 {
#endif
		 if (SUCCESS == parseProlinePara(pkeyPara->flag, &para, buff))
		 {
			  printf("\r\n %s %d %s:%s.\n",__FUNCTION__,__LINE__,pkeyPara->attr,buff);
			 if(strcmp(pkeyPara->node,XPON_LINKCFG_NODE) == 0){
			 	if(strcmp(buff,"EPON") == 0){
					cfg_set_object_attr(nodeName, "Mode", "2");
			 	}
				else if(strcmp(buff,"GPON") == 0)
				{
					cfg_set_object_attr(nodeName, "Mode", "1");
				}		
			 }
			 else			 
			 cfg_set_object_attr(nodeName, pkeyPara->attr, buff);
#if defined(TCSUPPORT_ECNT_MAP)

			if ( PL_CP_MAPDEVROLE_FLAG == pkeyPara->flag )
			{
				/*update MAP Backhaul info*/
				update_mapbhinfo_from_proline(buff);
			}
#endif	
#if defined(TCSUPPORT_NP_CMCC)
			if ( PL_CP_SSID_FLAG == pkeyPara->flag )
			{
				/* update wifi entry3 entry4 */
				if ( cfg_query_object("root.wlan.entry.3", NULL, NULL) > 0 )
				{
					bzero(buffer2, sizeof(buffer2));
					snprintf(buffer2, sizeof(buffer2), "%s-3", buff);
					cfg_set_object_attr("root.wlan.entry.3", pkeyPara->attr, buffer2);
				}

				if ( cfg_query_object("root.wlan.entry.4", NULL, NULL) > 0 )
				{
					bzero(buffer2, sizeof(buffer2));
					snprintf(buffer2, sizeof(buffer2), "%s-4", buff);
					cfg_set_object_attr("root.wlan.entry.4", pkeyPara->attr, buffer2);
				}
			}
#ifdef TCSUPPORT_WLAN_AC
			else if ( PL_CP_SSIDAC_FLAG == pkeyPara->flag ) 
			{
				/* update wifi entry3 entry4 */
				if ( cfg_query_object("root.wlan11ac.entry.3", NULL, NULL) > 0 )
				{
					bzero(buffer2, sizeof(buffer2));
					snprintf(buffer2, sizeof(buffer2), "%s-3", buff);
					cfg_set_object_attr("root.wlan11ac.entry.3", pkeyPara->attr, buffer2);
				}

				if ( cfg_query_object("root.wlan11ac.entry.4", NULL, NULL) > 0 )
				{
					bzero(buffer2, sizeof(buffer2));
					snprintf(buffer2, sizeof(buffer2), "%s-4", buff);
					cfg_set_object_attr("root.wlan11ac.entry.4", pkeyPara->attr, buffer2);
				}
			}
#endif
#endif
		 }
		 else
		 {
			 printf("\r\nparse proline para %s fail.",pkeyPara->attr);
		 }
#if 0
		 }
		 else
		 {
			 /* to do*/
			 /* add this config*/
			 printf("\r\nError--Entry %s value is NULL.",pkeyPara->attr);
		 }
		 
#endif
	 }
#endif	 
	 return;
 }

 void handle_proline_paras_restore(mxml_node_t* defaulttree)
 {
#if defined(TCSUPPORT_PRODUCTIONLINE) && defined(TCSUPPORT_RESERVEAREA_EXTEND) 
	char nodeName[4][32];
	mxml_node_t *curNode = NULL;
	int i = 0;
	proline_Para para;
	Key_Parameters2_Info *pkeyPara = NULL;
	char buff[512] = {0};
	char *oriValue = NULL;

	memset(&para, 0, sizeof(para));
	tcgetProLinePara(&para,0);
	for ( pkeyPara = prolinePara ; 0 != pkeyPara->flag; pkeyPara ++ )
	{
		memset(nodeName, 0, sizeof(nodeName));
		splitName(pkeyPara->node2, nodeName, ".");

		curNode = defaulttree;

		for ( i = 1; i < 4; i++ )
		{
			if ( 0 == nodeName[i][0] )
				break;
			curNode = mxmlFindElement(curNode, curNode, nodeName[i],
							NULL, NULL, MXML_DESCEND);
			if ( NULL == curNode )
				break; /* try next one */
		}

		if ( NULL != curNode )
		{
			if ( SUCCESS == parseProlinePara(pkeyPara->flag, &para, buff) )
			{
				if( strcmp(pkeyPara->node, XPON_LINKCFG_NODE) == 0 )
				{
				   if( strcmp(buff, "EPON") == 0 )
					   mxmlElementSetAttr(curNode, "Mode", "2");
				   else if( strcmp(buff,"GPON") == 0 )
					   mxmlElementSetAttr(curNode, "Mode", "1");
				   else
					   mxmlElementSetAttr(curNode, "Mode", "0");
				}
				else
					mxmlElementSetAttr(curNode, pkeyPara->attr, buff);
			}
		}
	}
#endif

	 return;
 }

int tc_parse_romfile(void)
 {
	 int ret = -1;
	 mxml_node_t *tree=NULL;
	 int rechecknum = RUN_ROMFILE_FLAG;
	 char operatename[32] = {0};
	 char buf[128] = {0};
	 char romfilepath[64]={0};
	 int retvalue = -1;
 
	 /*check the valid of config file*/	 
	 strncpy(operatename, RUNNING_ROMFILE_PATH, sizeof(operatename)-1);
	 do
	 {
		 isRomfileCrcErr = 0;
		 
		 ret = check_romfile(operatename);
		 if(ret < 0)
		 {
			 if(RUN_ROMFILE_FLAG == rechecknum)
			 {	 
#if defined(CT_COM_DEVICEREG) && defined(TCSUPPORT_ITMS_CONFIG_AS_DEFAULT)
				/*JiangSu*/
				 if(1 == isRomfileCrcErr)
				 {
					 printf("Use Itms romfile to check!\r\n");
					 loadItmsRomfile();
					 strncpy(operatename, ITMSR_FILE_NAME, sizeof(operatename)-1);	 
					 rechecknum = ITMS_ROMFILE_FLAG;
				 }
				 else
				 {
					 printf("Use backup romfile to check!\r\n");
					 loadBackupRomfile();
					 strncpy(operatename, BR_FILE_NAME,  sizeof(operatename)-1);
					 rechecknum = BK_ROMFILE_FLAG;
				 }	 
#else
				 printf("Use backup romfile to check!\r\n");
				 loadBackupRomfile();
				 strncpy(operatename, BR_FILE_NAME, sizeof(operatename)-1);
				 rechecknum = BK_ROMFILE_FLAG;
#endif	 
			 }
			 else if(ITMS_ROMFILE_FLAG == rechecknum)
			 {
				 fprintf(stderr, "Use backup romfile to check!\r\n");
				 loadBackupRomfile();
				 strcpy(operatename,BR_FILE_NAME);
				 rechecknum = BK_ROMFILE_FLAG;
			 }		
			 else if(BK_ROMFILE_FLAG == rechecknum)
			 {
				 printf("Use default romfile to check!\r\n");
				 loadDefaultRomfile();
				 strncpy(operatename, DR_FILE_NAME, sizeof(operatename)-1);
				 rechecknum = DEF_ROMFILE_FLAG;
			 }
			 else
			 {
				 printf("Use usrfs romfile to check!\r\n");
				 getDefaultRomfile(romfilepath,sizeof(romfilepath));
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
					 tree = load_checksum_def_romfile(romfilepath);
#else
					 tree = load_def_romfile(romfilepath);
#endif	 
				 /* stupitd check,because initial romfile store in filesystem must be correct!!*/
				 if(tree == NULL)
				 {
					 printf("initial romfile store in filesystem is error!!so use exit(0)\r\n");
					 exit(0);
				 }
				 else
				 {	 
					 rechecknum = DEFFILESYSTEM_ROMFILE_FLAG;
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)
					 printf("\n<<call handle_proline_paras>>\n");
					 handle_proline_paras(tree);	/* replace proline parameters from proline block to tree*/
			 
				 	if(cfg_save_object(ROMFILE_PATH) == -1)
					{
						printf("\n----> Load config file fail.\n");
						return -1;
					}

					 retvalue = compute_checksum(ROMFILE_PATH,STORE_ROMFILE_FILE_NAME);
					 if(retvalue < 0)
					 {
						 printf("\r\nkeyParaRestore:Fail to compute checksum when save to flash!\n");
						 return -1;
					 }
					 else
					 {
						 printf("\n----> save config file to flash\n");
						 
						 snprintf(buf, sizeof(buf), UPDATE_ROMFILE_CMD, STORE_ROMFILE_FILE_NAME);	 
						 system(buf);
						 usleep(100000);
						 
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
							 snprintf(buf, sizeof(buf), 
							 	TC_FLASH_ERASE_SECTOR_CMD, 
							 	BACKUPROMFILE_RA_OFFSET, 
							 	RESERVEAREA_NAME);
							 system(buf);
					 
							 memset(buf,0,sizeof(buf));
							 snprintf(buf, sizeof(buf), 
							 	TC_FLASH_WRITE_CMD, STORE_ROMFILE_FILE_NAME, 
							 	BACKUPROMFILE_RA_SIZE, 
							 	BACKUPROMFILE_RA_OFFSET, 
							 	RESERVEAREA_NAME);
							 system(buf);
							 usleep(100000);
#endif	 
						 unlink(STORE_ROMFILE_FILE_NAME);
					 }
					 unlink(LOAD_ROMFILE_FILE_NAME);
#else
					 /*because backup area check fail,so must also write correct romfile to backup area*/
					 snprintf(buf, sizeof(buf), TC_FLASH_ERASE_SECTOR_CMD, 
					 BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
					 system(buf);
					 
					 memset(buf,0,sizeof(buf));
					 snprintf(buf, sizeof(buf), TC_FLASH_WRITE_CMD,
					 	romfilepath,BACKUPROMFILE_RA_SIZE,
					 	BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
					 system(buf);
					 printf("Check romfile pass!Use default romfile in filesystem to replace backup romfile\r\n");
#endif
				 }
			 }
		 }
		 else
		 {
			 printf("Check romfile pass!\r\n");
		 }
		 
	 } while(!tree && (ret < 0));
 
	 if(RUN_ROMFILE_FLAG == rechecknum)
	 {
		 loadBackupRomfile();		 
		 ret = check_checksum(BR_FILE_NAME,LOAD_ROMFILE_FILE_NAME, GZ_TMP_PATH);
		 if(ret < 0)
		 {
		 	tcdbg_printf("[%s:%d]:check_checksum=-1,write running romfile to backupromfile===>\n",__FUNCTION__,__LINE__);
			 /*for the first time,must write running romfile to backupromfile,if runing romfile check pass*/
			 snprintf(buf, sizeof(buf), TC_FLASH_ERASE_SECTOR_CMD,
			 BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
			 system(buf);
			 
			 memset(buf,0,sizeof(buf));
			 snprintf(buf, sizeof(buf), TC_FLASH_WRITE_CMD,RUNNING_ROMFILE_PATH,
			 	BACKUPROMFILE_RA_SIZE,
			 	BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
			 system(buf);
		 }
		 unlink(LOAD_ROMFILE_FILE_NAME);
	 }
	 else if(ITMS_ROMFILE_FLAG == rechecknum)
	 {
		 snprintf(buf, sizeof(buf), UPDATE_ROMFILE_CMD, ITMSR_FILE_NAME);
		 system(buf);
		 
		 snprintf(buf, sizeof(buf), TC_FLASH_ERASE_SECTOR_CMD,
		 	BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
		 system(buf);
			 
		 memset(buf,0,sizeof(buf));
		 snprintf(buf, sizeof(buf), TC_FLASH_WRITE_CMD,ITMSR_FILE_NAME,
		 	BACKUPROMFILE_RA_SIZE,
		 	BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
		 system(buf);
	 }
	 else if(BK_ROMFILE_FLAG == rechecknum)
	 {
		 printf("Check romfile pass!Load backup romfile to replace running romfile!\r\n");
		 snprintf(buf, sizeof(buf), UPDATE_ROMFILE_CMD, BR_FILE_NAME);
		 system(buf);		 
	 }
	 else if(DEF_ROMFILE_FLAG == rechecknum)
	 {
		 printf("Check romfile pass!Load default romfile to replace running romfile!\r\n");
		 snprintf(buf, sizeof(buf), UPDATE_ROMFILE_CMD, DR_FILE_NAME);
		 system(buf);	 
		 usleep(100000);
		 /*because backup area check fail,so must also write correct romfile to backup area*/
		 snprintf(buf, sizeof(buf), TC_FLASH_ERASE_SECTOR_CMD,
		 BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
		 system(buf);
		 
		 memset(buf,0,sizeof(buf));
		 snprintf(buf, sizeof(buf), TC_FLASH_WRITE_CMD,DR_FILE_NAME,
		 BACKUPROMFILE_RA_SIZE,
		 BACKUPROMFILE_RA_OFFSET, RESERVEAREA_NAME);
		 system(buf);
	 }
	 unlink(BR_FILE_NAME);
	 unlink(DR_FILE_NAME);
 
	 return 0;
}

 int parser_romfile(char* pathname)
 {
	 FILE *fp=NULL;
	 mxml_node_t *tree=NULL;
	 char romfilepath[64]={0};
	 int ret = -1;
 
	 /*Load romfile  file*/
	 fp = fopen(pathname, "r");
	 if(fp == NULL)
	 {
		 printf(OPEN_FAIL_MSG, pathname, strerror(errno));
		 getDefaultRomfile(romfilepath,sizeof(romfilepath));
		 tree = load_def_romfile(romfilepath);
		 if(tree == NULL)
		 	return -1;
	 }
 
 	 ret = cfg_load_object(pathname);
	 fclose(fp);
	 /*
	 Mxml xml parser isn't support verify mechanism,
	 so we need to implement a fucntion for verification.
	 */
	 if(ret < 0)
	 {
		 /*If the romfile format is wrong, we need to load default romfile*/
		 printf(USE_DEF_ROMFILE_MSG);
		 if(tree != NULL)
		 {
			 mxmlDelete(tree);
		 }
		 getDefaultRomfile(romfilepath,sizeof(romfilepath));
		 tree = load_def_romfile(romfilepath);
	 }
	 if(tree == NULL)
	 	return -1;
	 else
	 	return 0;
 }

 void cfg_load_romfile(void)
{
	int flag = -1;
#if defined(TCSUPPORT_BACKUPROMFILE)
		/*
			note:
			if execute function tc_parse_romfile() succeed,the function tc_parse_romfile() must return the true tree struct
		*/
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
			/*get flash information,because check config file crc need these!!!!*/
	flag = blapi_system_get_mtdsize("romfile", NULL, &mtd_erasesize);
	if(flag < 0)
	{
		printf("main:get mtd info error!");
		exit(0);
	}
	
#endif
	tc_parse_romfile();
#endif
}

int update_UploadedRomfile(int imagefd, int offset, unsigned  int length, char *outputfile)
{
	char *buf = NULL;	
	int fdout = -1;
	unsigned long read_count = 0,write_count = 0;

#if !defined(TCSUPPORT_CT_ZIPROMFILE)
		return 0;
#endif

	fdout = open(outputfile, O_RDWR | O_CREAT | O_TRUNC);
	if(fdout < 0)
	{
		return -1;
	}
	else
	{
#if defined(TCSUPPORT_CT_ZIPROMFILE)
		buf = malloc(ROMFILE_MAX_LEN+ROMFILE_MAX_PENDING+1);
#else
		buf = malloc(mtd_erasesize + 1);
#endif
		if (buf == NULL)
		{
			goto error;
		}

#if defined(TCSUPPORT_CT_ZIPROMFILE)
		read_count = read(imagefd, buf, ROMFILE_MAX_LEN+ROMFILE_MAX_PENDING);
#else
		read_count = read(imagefd, buf, mtd_erasesize);
#endif

		if(read_count < length)
		{
			goto error;
		}

		write_count = write(fdout, buf + offset, length);
		if(write_count != length)
		{
			goto error;
		}		
	}
	
	free(buf);
	close(fdout);
	return 0;

error:
	if(buf)
		free(buf);
	if(fdout >= 0)
		close(fdout);
	return -1;

}

int checkCwmpUploadedRomfile(void)
{
	int ret = -1;
	int flag = -1;
	char filepath[128] = {0};

#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	flag = check_checksum(UPLOAD_ROMFILE_PATH, CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME, CWMP_GZ_TMP_PATH);
	if(flag <0)
	{
		printf(CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME);
		unlink(UPLOAD_ROMFILE_PATH); 
		return -1;
	}
	strncpy(filepath, CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME, sizeof(filepath)-1);
#else
	strncpy(filepath, UPLOAD_ROMFILE_PATH, sizeof(filepath)-1);
#endif

	ret = cfg_load_object(filepath);
 
#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
	unlink(CT_CWMP_DOWNLOAD_CHKROMFILE_FILE_NAME);
#endif
	
	return ret;
}

