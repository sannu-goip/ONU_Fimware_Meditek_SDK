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
#include <unistd.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "cfg_cli.h"
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_msg.h"
#include <flash_layout/tc_partition.h>
#include "blapi_traffic.h"


#if defined(TCSUPPORT_CWMP_OPENSSL)
#define CA_CHECK_TEMP_FILE "/tmp/ca_file_temp.chk"
#define CA_CRT_ENDINFO "-----END CERTIFICATE-----"
#define CA_KEY_ENDINFO "-----END PRIVATE KEY-----"
#define CA_KEY_ENDINFO2 "-----END RSA PRIVATE KEY-----"

#define SSL_SERVER_CA "/etc/ssl_server.crt"
#define SSL_CLIENT_CERT "/etc/ssl_client.crt"
#define SSL_CLIENT_KEY "/etc/ssl_client.pem"
#define MAX_CA_SIZE 4096
#define UPLOAD_CA_PATH "/var/tmp/ca"
#define MAX_CA_NUM 4
#define CA_UPGRADE_FLAG "UpgradeFlag"

#define CA_PATH "/etc/ssl_server.crt"
#else
#define CA_PATH "/etc/CustomerCA.pem"
#endif

#define CWMP_MAX_MSG_LEN 256
#define	SERVICE_LIST_LEN  		64
#define CWMP_MQ_FLAG_PATH "/tmp/cwmp/tr069agent_mq"
#define PROJID 2

#define MTD_BLOCK_PROC "/proc/mtd"
#define MOUNT_JFFS2_SH "/usr/script/mount_jffs2.sh"

char connReq_old_port[10];


void update_cpePort_rule()
{
	char url_buff[128] = {0};
	char cpePort_buff[10] = {0};
	char cwmpPath[64] = {0};  
	char *ptr_s = NULL;
	char *ptr_e = NULL;
	int new_cpePort = 0;
	int old_cpePort = 0;

	snprintf(cwmpPath, sizeof(cwmpPath), CWMP_ENTRY_NODE);
	if(cfg_obj_get_object_attr(cwmpPath, "acsUrl", 0, url_buff, sizeof(url_buff)) < 0)
	{
	    tcdbg_printf("get acsUrl fail\n");
	    return;
	}

	ptr_s = strchr(url_buff, ':');
	if(ptr_s != NULL)
	{
	    ptr_s++;
	    ptr_s = strchr(ptr_s, ':');
	    if(ptr_s != NULL)
    	{
    	    ptr_s++;
    	    ptr_e = strchr(ptr_s, '/');
    	    if(ptr_e != NULL)
    	        *ptr_e = '\0';
    	    new_cpePort = atoi(ptr_s);
    	}
    	else
	        new_cpePort = 80;
	}
	else
	    return;

	if(cfg_obj_get_object_attr(cwmpPath, "CpePort", 0, cpePort_buff, sizeof(cpePort_buff)) < 0)
	{
	    tcdbg_printf("get CpePort fail\n");
	    return;
	}

	old_cpePort = atoi(cpePort_buff);
}


void update_connReq_rule(char* path)
{
	char connReq_port[10], isp[4], status[4], svc_list[SERVICE_LIST_LEN], if_name[16];
	char cmd_buf[128];
	int i, pvc_index, entry_index;
	char cwmpPath[64] = {0};
	char wanPath[64] = {0};
	char wanInfoPath[64] = {0};

	/* get connection request port */
	snprintf(cwmpPath, sizeof(cwmpPath), CWMP_ENTRY_NODE);
	memset(connReq_port, 0, sizeof(connReq_port));
	if(cfg_obj_get_object_attr(cwmpPath, "conReqPort", 0, connReq_port, sizeof(connReq_port)) < 0)
	{
		strncpy(connReq_port, "7547",sizeof(connReq_port)-1);
	}

	/* use port by tr69 spec */
	if (connReq_port[0] == '\0') 
	{
		strncpy(connReq_port, "7547",sizeof(connReq_port)-1);
	}

	/* if connection port is not changed, it should not set iptables rules again */
	if (!strcmp(connReq_old_port, connReq_port))        
	{	
		return;
	}

	/* set iptables rule to let connection request packet come in */
	memset(wanPath, 0, sizeof(wanPath));
	memset(wanInfoPath, 0, sizeof(wanInfoPath));
	for (i = 1; i <= MAX_WAN_IF_INDEX; i++) {
		pvc_index = i / MAX_SMUX_NUM;
		entry_index = i - pvc_index * MAX_SMUX_NUM;
		snprintf(wanPath, sizeof(wanPath),WAN_PVC_ENTRY_NODE, pvc_index, entry_index);

		/* check isp, only for routing mode */
		memset(isp, 0, sizeof(isp));
		if(cfg_obj_get_object_attr(wanPath, "ISP", 0, isp, sizeof(isp)) > 0)
		{			
			if (!strcmp(isp, "3")) 
			{
				continue; /* only for routing mode */
			}
		}
		else 
		{
			continue;
		}

		/* check status */
		snprintf(wanInfoPath, sizeof(wanInfoPath), WANINFO_ENTRY_NODE, i);
		memset(status, 0, sizeof(status));
		if(cfg_obj_get_object_attr(wanInfoPath, "Status", 0, status, sizeof(status)) > 0)     
		{
			if (0 != strcmp(status, "up"))
			{
				continue; /* only for up wan interface */
			}
		}
		else
		{
			continue;
		}
		
		/* get interface name */
		memset(if_name, 0, sizeof(if_name));
		if(cfg_obj_get_object_attr(wanPath, "IFName", 0, if_name, sizeof(if_name)) < 0)
		{
			continue;
		}
		
		/* check service list only for TR069 and TR069_INTERNET */
		memset(svc_list, 0, sizeof(svc_list));
		if(cfg_obj_get_object_attr(wanPath, "ServiceList", 0, svc_list, sizeof(svc_list)) > 0)
		{
#if defined(TCSUPPORT_CT_SERVICELIST_E8C)  
			if (strstr(svc_list, "TR069") == NULL) 
#else
			if (0 != strcmp(svc_list, "TR069") && 0 != strcmp(svc_list, "TR069_INTERNET")) 
#endif
			{
				/* only for tr069 and tr069_internet */
				continue; 
			}
		}
		else
		{
			continue;
		}
		
		/* set iptables rules */		
		snprintf(cmd_buf, sizeof(cmd_buf), "iptables -t nat -D PRE_SERVICE -i %s -p tcp --dport %s -j ACCEPT 2>/dev/null", if_name, connReq_old_port);
		system_escape(cmd_buf);
		snprintf(cmd_buf, sizeof(cmd_buf), "iptables -t nat -A PRE_SERVICE -i %s -p tcp --dport %s -j ACCEPT", if_name, connReq_port);
		system_escape(cmd_buf);
	}
	
	/* update connection request port */
	strncpy(connReq_old_port, connReq_port, sizeof(connReq_old_port)-1);
	
	return;	
}

#if defined(TCSUPPORT_CT_E8GUI) 
#if defined(SSL) || defined(TCSUPPORT_CWMP_SSL)
static int setCertistatus(int ista)
{
	char str[4] = {0};
	char nodePath[64] = {0};
	
	snprintf(nodePath, sizeof(nodePath), CWMP_ENTRY_NODE);
	snprintf(str, sizeof(str), "%d", ista);
	cfg_set_object_attr(nodePath, "CertCode" ,str);
	
	return SUCCESS;
}

int checkCertificateswitch(void)
{
	int ret = SUCCESS;
	char	value[16] = {0};
	int index = 0;
	unsigned long ulSize[4] = {CERM1_RA_SIZE, CERM2_RA_SIZE, CERM3_RA_SIZE, CERM4_RA_SIZE};     
	unsigned long ulOffsite[4] = {CERM1_RA_OFFSET, CERM2_RA_OFFSET, CERM3_RA_OFFSET, CERM4_RA_OFFSET};
	FILE	*fw = NULL;
	FILE	*fr = NULL;
	char	cmd_buf[128] = {0};
	char	*pbuf = NULL;
	char	*pCAfile = NULL;
	char	*pEnd = NULL;
	int	nLength = 0;
	char sslcaPath[64] = {0};
	char cwmpPath[64] = {0};
	char tmpPath[64] = {0};
	int res = 0;

	snprintf(sslcaPath, sizeof(sslcaPath), SSLCA_COMMON_NODE);
	if(cfg_obj_get_object_attr(sslcaPath, "CurIndex", 0, value, sizeof(value)) > 0)
	{
		index = atoi(value);
		strncpy(tmpPath, SSLCA_NODE, sizeof(tmpPath)-1);
		if(cfg_query_object(tmpPath,NULL,NULL) >= 0)
		{
			memset(tmpPath, 0, sizeof(tmpPath));
			snprintf(tmpPath,sizeof(tmpPath), SSLCA_ENTRY_NODE,index +1);  
			if(cfg_query_object(tmpPath,NULL,NULL) >= 0)
			{
				strncpy(cwmpPath,CWMP_ENTRY_NODE, sizeof(cwmpPath));
				memset(value, 0x00, sizeof(value));
				if(cfg_obj_get_object_attr(cwmpPath, "EnableCert", 0, value, sizeof(value)) > 0)
				{
					if(!strcmp(value, "0")) 
					{
						unlink(CA_PATH);
						setCertistatus(2);
					}
					else if(!strcmp(value, "1"))
					{
						fw = fopen("/var/tmp/ca_temp.pem", "w");
						if(fw == NULL)
						{	
							printf("\n [%s],line = %d, CA file /var/tmp/ca_temp.pem open failed ==================>\n",__FUNCTION__,__LINE__);
							ret = FAIL;
							return ret;
						}
						snprintf(cmd_buf,sizeof(cmd_buf), TC_FLASH_READ_CMD, "/var/tmp/ca_temp.pem", ulSize[index], ulOffsite[index] ,RESERVEAREA_NAME);
						system(cmd_buf);
						if(fw){
							fclose(fw);
							fw = NULL;
						}
						/*parse the file of read from flash  */
						fr = fopen("/var/tmp/ca_temp.pem", "r");
						if(fr == NULL){
							printf("\n [%s],line = %d, CA file /var/tmp/ca_temp.pem open failed ==================>\n",__FUNCTION__,__LINE__);
							ret = FAIL;
							return ret;
						}
						pbuf = (char*)malloc(ulSize[index]);
						if(!pbuf){
							ret = FAIL;
							printf("\n [%s],line = %d, malloc size failed! ==================>\n",__FUNCTION__,__LINE__);
							if(fr){
								fclose(fr);
								fr = NULL;
							}
							return ret;
						}
						memset(pbuf, 0x00, ulSize[index]);
						res = fseek(fr,0,SEEK_SET);
						res = fread(pbuf,sizeof(char), ulSize[index], fr);
						pbuf[ulSize[index] - 1] = '\0';
						pEnd = strstr(pbuf, "-----END CERTIFICATE-----");
						res = fseek(fr,0,SEEK_SET);
						nLength = (int)(pEnd+25-pbuf);
						if(pbuf){
							free(pbuf);
							pbuf = NULL;
						}
						if((nLength + 1) <= ((~((unsigned int)0))/2))
							pCAfile = (char*)malloc(nLength+1);
						if(!pCAfile){
							ret = FAIL;
							printf("\n [%s],line = %d, malloc size failed! ==================>\n",__FUNCTION__,__LINE__);
							if(fr){
								fclose(fr);
								fr = NULL;
							}
							return ret;
						}
						memset(pCAfile, 0x00, nLength+1);
						res = fread(pCAfile, sizeof(char), nLength, fr);
						pCAfile[nLength] = '\n';
						if(fr){
							fclose(fr);
							fr = NULL;
						}
						unlink("/var/tmp/ca_temp.pem");
						/*write to CA_PATH */
						fw = fopen(CA_PATH, "w");
						if(fw == NULL){
							printf("\n [%s],line = %d, CA_PATH file open failed! ==================>\n",__FUNCTION__,__LINE__);
							ret = FAIL;
							if(pCAfile){
								free(pCAfile);
								pCAfile = NULL;
							}
							return ret;
						}
						fwrite(pCAfile, sizeof(char), nLength+1, fw);
						if(fw){
							fclose(fw);
							fw = NULL;
						}
						if(pCAfile){
							free(pCAfile);
							pCAfile = NULL;
						}
						setCertistatus(3);
					}
				}
			}
			else
			{
				setCertistatus(4);
				ret = FAIL;
			}
		}
	}
	return ret;
}
#endif
#endif


#if defined(TCSUPPORT_CT_MIDWARE)   
int uTr069Enable;     
int midware_start(char* path)
{
	char value[4];
	int enable=0;
	char cmdbuf[100] = "";
	char nodePath[64] = {0};
	
	snprintf(nodePath, sizeof(nodePath),CWMP_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodePath, "Tr069Enable", 0, value, sizeof(value)) > 0)
	{
		enable = atoi(value);
		if(uTr069Enable != enable)    
		{
			if((uTr069Enable == 0 && enable == 2) || (uTr069Enable == 2 && enable == 0))
			{
				return -1;
			}
			if((uTr069Enable == 1 && enable == 0) || (uTr069Enable == 0 && enable == 1))
			{
				printf("midware_start:need reboot for this change!!\n");
				if(uTr069Enable == 0 && enable == 1) 
				{
					return 0;
				}
				return -1;
			}
			if((uTr069Enable == 1 && enable == 2) || (uTr069Enable == 2 && enable == 1))
			{
				doValPut("/var/run/restart_tr069_itself", "1\n");
				snprintf(cmdbuf,sizeof(cmdbuf),"/usr/script/mw_restart.sh %d",enable);
				system(cmdbuf);
				uTr069Enable = enable;			
				return 0;
			}
		}
	}
	return -1;
}

void mount_jffs2(void)
{
	FILE *fp = NULL;
	int index = 0;
	char buf[256] = "\0";
	
	fp=fopen(MTD_BLOCK_PROC,"r");
	if(fp != NULL)
	{
		while(fgets(buf,sizeof(buf),fp))
		{
			if(strstr(buf,"jffs2")){
				sscanf(buf,"mtd%d:",&index);
				printf("\n [%s],line = %d, mount jffs2 mtdblock%d !==================>\n",__FUNCTION__,__LINE__,index);
				if(index != 0)
				{
					snprintf(buf,sizeof(buf),"%s %d",MOUNT_JFFS2_SH,index);
					system(buf);
				}
				break;
			}
		}
		fclose(fp);
	}	
}

#endif

#if defined(TCSUPPORT_CWMP_OPENSSL)
static int loadCAFromFlash(int ca_index)
{
	char cmd_buf[256] = {0};
	unsigned long ulSize[4] = {CERM1_RA_SIZE, CERM2_RA_SIZE
							, CERM3_RA_SIZE, CERM4_RA_SIZE};
	unsigned long ulOffsite[4] = {CERM1_RA_OFFSET, CERM2_RA_OFFSET
							, CERM3_RA_OFFSET, CERM4_RA_OFFSET};
	FILE *fp_tmp = NULL, *fp_ca = NULL;
	char *tmp_ca_buf = NULL, *pEnd = NULL, *pEnd2 = NULL;
	char *pCA_End = NULL, *caEndStr = NULL;
	int ca_file_len = 0, endinfo_len = 0;
	unsigned int ca_write_len = 0;
	char attrName[16] = {0};
	char caExist[16] = {0};
	int caExistFlag = 0;
	unsigned long res = 0;
	/*
	CA1: client cert
	CA2: client key
	CA3: server CA
	CA4: reserved.
	*/
	char *ca_files[4] = {SSL_CLIENT_CERT, SSL_CLIENT_KEY
						, SSL_SERVER_CA, NULL};
	char *ca_endinfo[4] = {CA_CRT_ENDINFO, CA_KEY_ENDINFO
					, CA_CRT_ENDINFO, NULL};
	char *ca_endinfo2[4] = {CA_CRT_ENDINFO, CA_KEY_ENDINFO2
					, CA_CRT_ENDINFO, NULL};

	if ( ca_index >= 4
		|| NULL == ca_files[ca_index]
		|| NULL == ca_endinfo[ca_index]
		|| NULL == ca_endinfo2[ca_index])
		return -1;

	/* delete old CA/KEY */
	unlink(ca_files[ca_index]);

	/* read CA/KEY into temp file */
	snprintf(cmd_buf, sizeof(cmd_buf)
			, TC_FLASH_READ_CMD, CA_CHECK_TEMP_FILE
			, ulSize[ca_index], ulOffsite[ca_index]
			, RESERVEAREA_NAME);
	system(cmd_buf);

	/* check CA/KEY valid simply */
	fp_tmp = fopen(CA_CHECK_TEMP_FILE, "r");
	if ( fp_tmp )
	{
		/* read CA */
		tmp_ca_buf = (char*)malloc(ulSize[ca_index] + 10);
		if ( tmp_ca_buf )
		{
			bzero(tmp_ca_buf, ulSize[ca_index] + 10);
			fseek(fp_tmp, 0, SEEK_SET);
			res = fread(tmp_ca_buf, sizeof(char), ulSize[ca_index], fp_tmp);
			if ( res > 0 )
				tmp_ca_buf[res] = 0;
			else
				tmp_ca_buf[0] = 0;
			/* check CA/KEY end info. */
			pEnd = strstr(tmp_ca_buf, ca_endinfo[ca_index]);
			pEnd2 = strstr(tmp_ca_buf, ca_endinfo2[ca_index]);
			if ( pEnd || pEnd2 )
			{
				if ( pEnd )
				{
					pCA_End = pEnd;
					caEndStr = ca_endinfo[ca_index];
				}
				else
				{
					pCA_End = pEnd2;
					caEndStr = ca_endinfo2[ca_index];
				}

				endinfo_len = strlen(caEndStr);
				/* CA/KEY valid length */
				if ( pCA_End - tmp_ca_buf > 0 )
					ca_file_len = (pCA_End - tmp_ca_buf + endinfo_len);
				else
					ca_file_len = endinfo_len;

				/* write valid part of CA/key to CA/KEY files */
				if ( ca_file_len > 0 )
				{
					*(pCA_End + endinfo_len) = '\n'; /* Add New Line */
					fp_ca = fopen(ca_files[ca_index], "w");
					ca_write_len = ca_file_len;
					fwrite(tmp_ca_buf, sizeof(char), ca_write_len + 1, fp_ca);
					fclose(fp_ca);
					caExistFlag = 1;
				}
			}
			else
			{
				printf("\n Invalid CA/KEY file [%s] \n", ca_files[ca_index]);
			}

			/* Release Buf. */
			free(tmp_ca_buf);
			tmp_ca_buf = NULL;
		}

		fclose(fp_tmp);
		unlink(CA_CHECK_TEMP_FILE);
	}
	snprintf(attrName, sizeof(attrName), "ca%dExist", ca_index + 1);
	snprintf(caExist, sizeof(caExist), "%d", caExistFlag);
	cfg_set_object_attr(SSLCA_COMMON_NODE, attrName, caExist);
	
	return 0;
}

int parseCAfile(void)
{
	FILE	*fw = NULL;
	FILE	*fr = NULL;
	char 	*pCAfile = NULL;
	char	cmd_buf[128] = {0};
	char	filePath[64] = {0};
	char	value[16] = {0};
	char	*pStart, *pEnd = NULL;
	char 	*pBuf = NULL;
	int	caSize = 0;
	int	reVal = SUCCESS;

	unsigned long ulSize[4] = {CERM1_RA_SIZE, CERM2_RA_SIZE, CERM3_RA_SIZE, CERM4_RA_SIZE};
	unsigned long ulOffsite[4] = {CERM1_RA_OFFSET, CERM2_RA_OFFSET, CERM3_RA_OFFSET, CERM4_RA_OFFSET};

	char 	*pKeyfile = NULL;
	char	*pKeyStart, *pKeyEnd = NULL;
	int hasKey = 0;
	int	res = 0, res2 = 0;
	unsigned int nKeyLength = 0, nLength = 0;

	memset(filePath,0,sizeof(filePath));
	strncpy(filePath, "/tmp/boa-temp", sizeof(filePath));

#if 1
	/* Fix coverity 247513 Time of check time of use(TOCTOU) */
	if ( (fr = fopen(filePath, "r")) == NULL )
	{
		printf("parseCAfile:open %s failed!!\n", filePath);
		reVal = FAIL;
		goto Error;
	}

	fseek(fr, 0, SEEK_END);
	/* Check CA size and content */
	caSize = ftell(fr);
	if ( caSize < 0 || caSize > MAX_CA_SIZE )
	{
		printf("parseCAfile:Save ca failed,%s is too large!!\n", filePath);
		reVal = FAIL;
		setCertistatus(5);
		goto Error;
	}
#else
	if(stat(filePath,&fstat) != 0 || (fr = fopen(filePath, "r")) == NULL){
		printf("parseCAfile:open %s failed!!\n", filePath);
		reVal = FAIL;
		goto Error;
	}
	/*Check CA size and content*/
	if((caSize = fstat.st_size) > MAX_CA_SIZE){
		printf("parseCAfile:Save ca failed,%s is too large!!\n", filePath);
		reVal = FAIL;
		setCertistatus(5);
		goto Error;
	}
#endif

	pBuf = (char*)malloc(caSize + 1);
	if(!pBuf)
	{
		printf("parseCAfile:malloc size failed !!\n");
		reVal = FAIL;
		goto Error;
	}
	memset(pBuf, 0x00, caSize + 1);
	fseek(fr,0,SEEK_SET);
	res = fread(pBuf,sizeof(char), caSize, fr);
	if ( res > 0 )
		pBuf[res] = 0;
	else
		pBuf[0] = 0;

	if((pStart = strstr(pBuf, "-----BEGIN PRIVATE KEY-----")) == NULL)
	{
		hasKey = 0;
	}
	else
	{
		hasKey = 1;
		if((pEnd = strstr(pBuf, "-----END PRIVATE KEY-----")) == NULL)
		{
			printf("parseCAfile:write ca failed,%s end is not a correct CA !!\n", filePath);
			reVal = FAIL;
			setCertistatus(1);
			goto Error;
		}
		else
		{
			res = fseek(fr,(int)(pStart - pBuf),SEEK_SET);
			nKeyLength = (int)(pEnd+25-pStart);
			if ( 0xffffffff == nKeyLength )
				nKeyLength -= 1;
			pKeyfile = (char*)malloc(nKeyLength+1);
			if(!pKeyfile)
			{
				printf("parseCAfile:malloc size failed !!\n");
				reVal = FAIL;
				goto Error;
			}
			memset(pKeyfile, 0x00, nKeyLength+1);
			res2 = fread(pKeyfile, sizeof(char), nKeyLength, fr);
		}
	}

	if((pStart = strstr(pBuf, "-----BEGIN CERTIFICATE-----")) == NULL)
	{
		printf("parseCAfile:write ca failed,%s start is not a correct CA!!\n", filePath);
		reVal = FAIL;
		setCertistatus(1);
		goto Error;
	}
	else
	{
		if((pEnd = strstr(pBuf, "-----END CERTIFICATE-----")) == NULL)
		{
			printf("parseCAfile:write ca failed,%s end is not a correct CA !!\n", filePath);
			reVal = FAIL;
			setCertistatus(1);
			goto Error;
		}
		else
		{
			res = fseek(fr,(int)(pStart - pBuf),SEEK_SET);
			nLength = (int)(pEnd+25-pStart);
			if ( 0xffffffff == nLength )
				nLength -= 1;
			pCAfile = (char*)malloc(nLength+1);
			if(!pCAfile)
			{
				printf("parseCAfile:malloc size failed !!\n");
				reVal = FAIL;
				goto Error;
			}
			memset(pCAfile, 0x00, nLength+1);
			res = fread(pCAfile, sizeof(char), nLength, fr);
		}
	}

	pCAfile[nLength] = '\n';

	memset(value, 0x00, sizeof(value));
	if(cfg_obj_get_object_attr(CWMP_ENTRY_NODE, "EnableCert", 0, value, sizeof(value)) > 0)
	{
		if(!strcmp(value,"1")){
			if(hasKey == 0)
			{
				memset(filePath,0,sizeof(filePath));
				snprintf(filePath, sizeof(filePath), "%s%d.pem", UPLOAD_CA_PATH, 3);

				fw = fopen(filePath, "w");
				if(fw == NULL){
					printf("parseCAfile:CA_PATH file open failed\n");
				}
				if(fw){
					fwrite(pCAfile, sizeof(char), nLength+1, fw);
					fclose(fw);
					fw = NULL;
				}
				reVal = 1 << 2;
			}
			else
			{
				memset(filePath,0,sizeof(filePath));
				snprintf(filePath, sizeof(filePath), "%s%d.pem", UPLOAD_CA_PATH, 1);

				fw = fopen(filePath, "w");
				if(fw == NULL){
					printf("parseCAfile:CA_PATH file open failed\n");
				}
				if(fw){
					fwrite(pCAfile, sizeof(char), nLength+1, fw);
					fclose(fw);
					fw = NULL;
				}
				memset(filePath,0,sizeof(filePath));
				snprintf(filePath, sizeof(filePath), "%s%d.pem", UPLOAD_CA_PATH, 2);

				fw = fopen(filePath, "w");
				if(fw == NULL){
					printf("parseCAfile:CA_PATH file open failed\n");
				}
				if(fw){
					fwrite(pKeyfile, sizeof(char), nKeyLength+1, fw);
					fclose(fw);
					fw = NULL;
				}
				reVal = (1 << 1) | 1;

			}

			setCertistatus(0);

		}
	}
Error:
	if(fr){
		fclose(fr);
		fr = NULL;
	}
	if(pBuf){
		free(pBuf);
		pBuf = NULL;
	}
	if(pCAfile){
		free(pCAfile);
		pCAfile = NULL;
	}
	if(pKeyfile){
		free(pKeyfile);
		pKeyfile = NULL;
	}    
	return reVal;
}
#endif


int cfg_type_cwmp_write(void)
{
	int retval=0;
#ifdef TCSUPPORT_DNSEACHPVC 
	char value[64] = {0};
	char *urlTemp1 = NULL, *urlTemp2 = NULL;
	int length = 0;
	FILE *fp = NULL;
	char buf[64] = {0};
	char nodePath[64] = {0};
	
	snprintf(nodePath, sizeof(nodePath),CWMP_ENTRY_NODE);
	if(cfg_obj_get_object_attr(nodePath, "Active", 0, value, sizeof(value)) < 0)
	{
		return -1;
	}
	if(strcmp(value, "Yes") != 0)
	{
		return 0;
	}
			
	memset(value, 0, sizeof(value));
	if(cfg_obj_get_object_attr(nodePath, "acsUrl", 0, value, sizeof(value)) < 0)
	{
		return -1;
	}
	urlTemp1 = value;
	strncpy(buf, value, 4);
	if(strcmp(buf, "http") == 0)
	{
		if(*(urlTemp1+5) == ':')	/*https*/
		{
			urlTemp1 += strlen("https://");
		}	
		else
		{
			urlTemp1 += strlen("http://");
		}	
	}
	memset(buf, 0, sizeof(buf));
	urlTemp2 = urlTemp1;
	while(*urlTemp1 != '\0')
	{
		if(*urlTemp1 != ':' && *urlTemp1 != '/')
		{
			length++;
			urlTemp1++;
		}
		else
		{
			break;
		}	
	}
	memcpy(buf, urlTemp2, length);
	fp = fopen("/etc/tr69Url.conf", "w");
	if(fp == NULL){
		printf("\n open failed");
		return -1;
	}
	fwrite(buf, sizeof(char), strlen(buf), fp);
	fclose(fp);
#if defined(TCSUPPORT_CT_MIDWARE)	
	if(cfg_obj_get_object_attr(nodePath, "MWMgtUrl", 0, value, sizeof(value)) <= 0)
	{
		return -1;
	}

	fp = fopen("/etc/midwareUrl.conf", "w");
	if(fp == NULL)
	{
		printf("\n open failed");
		return -1;
	}
	fwrite(value, sizeof(char), strlen(value), fp);
	fclose(fp);
#endif
#endif	

	return retval;
}

int svc_cfg_boot_cwmp(void)
{
#if defined(TCSUPPORT_CT_MIDWARE)
	char value[4] ="";
	char buf[100] = "";
	char nodePath[64] = {0};
	snprintf(nodePath, sizeof(nodePath), CWMP_ENTRY_NODE);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	char nodeName[64] = {0}, nodeValue[128] = {0};
	char cmd_buf[256] = {0};
#endif
	int ret = 0;

	cfg_type_cwmp_write( );

#if defined(TCSUPPORT_CT_UPNP_DM)
	system("/userfs/bin/upnp-dm &");
#endif

#if defined(TCSUPPORT_CT_MIDWARE)
	mount_jffs2();

	if((cfg_obj_get_object_attr(nodePath, "Tr069Enable", 0, value, sizeof(value)) > 0)
	{
		if (!strcmp(value, "0")) 
		{
			uTr069Enable = 0;
		}
		else if (!strcmp(value, "2")) 
		{
			uTr069Enable = 2;
		}
		else
		{
			uTr069Enable = 1;
		}
		snprintf(buf, sizeof(buf), "/usr/script/mw_restart.sh %d &",uTr069Enable);
		system(buf);
	}
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	snprintf(nodeName, sizeof(nodeName), ACCOUNT_ENTRY_NODE, 3);
	cfg_get_object_attr(nodeName, "username", nodeValue, sizeof(nodeValue));
#endif

#if defined(TCSUPPORT_CT_SDN)
#if defined(TCSUPPORT_CT_JOYME4)
		bzero(cmd_buf, sizeof(cmd_buf));
		snprintf(cmd_buf, sizeof(cmd_buf), "su - %s -c \"/userfs/bin/tr69 &> /dev/null && /bin/pidof tr69 > /var/run/ctc_tr69.pid\""
					, nodeValue);
		tcdbg_printf("cmd_buf = %s\n",cmd_buf);
		system(cmd_buf);
#else
		system("/userfs/bin/tr69 &> /dev/null && pidof tr69 > /var/run/ctc_tr69.pid");
#endif
#else
#if !defined(TCSUPPORT_CT_MIDWARE)
#if !defined(TCSUPPORT_SDN_OVS)
#if defined(TCSUPPORT_CT_JOYME2)
#if defined(TCSUPPORT_CT_JOYME4)
	bzero(cmd_buf, sizeof(cmd_buf));
	snprintf(cmd_buf, sizeof(cmd_buf), "su - %s -c \"/userfs/bin/tr69 &> /dev/null && /bin/pidof tr69 > /var/run/ctc_tr69.pid\""
				, nodeValue);
	system(cmd_buf);
#else
	system("/userfs/bin/tr69 &> /dev/null && pidof tr69 > /var/run/ctc_tr69.pid");
#endif
#else
	system("/userfs/bin/tr69 &");
#endif
#endif
#endif
#endif

#if defined(TCSUPPORT_CWMP_OPENSSL)
	int idx = 0;
	for ( idx = 0; idx < 4; idx ++ )
	{
		loadCAFromFlash(idx);
	}
#else
#if defined(TCSUPPORT_CT_E8GUI)
#if defined(SSL) || defined(TCSUPPORT_CWMP_SSL)
	checkCertificateswitch();
#endif
#endif
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	ret = blapi_traffic_set_vip_by_port(53,0,VIP_P_UDP);
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "tr69boot_flag", "1");
#endif
	return SUCCESS;
}

int cfg_type_cwmp_execute(char* path)
{
	int ret;
	char value[64];
	long type = 1;
	int flag = 0;
	int mq = CWMP_CHANGED;
	cwmp_msg_t message;
	char cwmpPath[64] = {0};
	char cwmpRoutePath[64] = {0};
#if defined(TCSUPPORT_CT_INFORM_NODE)   
	char ManuaValue[4] = {0};
	char MaintainValue[4] = {0};
	char changeFlagStr[4] = {0};
#endif

	int caResult = -1;
	char caFileFlag[8] = {0};
	int caIdx = 0;
	
	memset(&message,0,sizeof(cwmp_msg_t));
	snprintf(cwmpPath, sizeof(cwmpPath),CWMP_ENTRY_NODE);
	
#if defined(TCSUPPORT_CWMP_OPENSSL)
	if (cfg_obj_get_object_attr(cwmpPath, "UpgradeCAFlag", 0, value, sizeof(value)) > 0)
	{
		if(atoi(value) > 0)
		{
			caResult = parseCAfile();
			if(caResult > 0)
			{
				for(caIdx = 0; caIdx < MAX_CA_NUM; caIdx ++)
				{
					if(caResult & (1 << caIdx))
					{
						snprintf(caFileFlag, sizeof(caFileFlag), "%d", caIdx + 1);
						cfg_set_object_attr(SSLCA_FLAG_NODE, CA_UPGRADE_FLAG, caFileFlag);
						cfg_commit_object(SSLCA_NODE);
					}
				}
			}
			cfg_set_object_attr(cwmpPath, "UpgradeCAFlag", "0");
		}		 
	}
#endif

	/* if tr069 is deactive, del cwmp route */
	if(cfg_obj_get_object_attr(cwmpPath, "Active", 0, value, sizeof(value)) > 0)
	{
		if (!strcmp(value, "Yes")) 
		{
			snprintf(cwmpRoutePath, sizeof(cwmpRoutePath),CWMPROUTE_ENTRY_NODE);
			/* tr069 will use the first route, so set it to new */
			cfg_set_object_attr(cwmpRoutePath, "ifStatus0" ,"new");
#if defined(TCSUPPORT_CT_E8GUI)   
#if defined(SSL) || defined(TCSUPPORT_CWMP_SSL)
#if !defined(TCSUPPORT_CWMP_OPENSSL)
			checkCertificateswitch(); 
#endif
#endif
#endif
		}
	}
	/* set connection request rule */
	update_connReq_rule(path);
    /*set cpePor rule*/
	update_cpePort_rule();

#if defined(TCSUPPORT_CT_INFORM_NODE) 
	cfg_set_object_attr(cwmpPath, "Inform_Status" ,"0");

	memset(ManuaValue, 0, sizeof(ManuaValue));
	cfg_obj_get_object_attr(cwmpPath, "Manual_Inform", 0, ManuaValue, sizeof(ManuaValue));

	memset(MaintainValue, 0, sizeof(MaintainValue));
	cfg_obj_get_object_attr(cwmpPath, "Maintain_Inform", 0, MaintainValue, sizeof(MaintainValue));

	cfg_obj_get_object_attr(cwmpPath, "account_change_flag", 0, changeFlagStr, sizeof(changeFlagStr));
	if(strcmp(ManuaValue, "1") == 0)
	{
		strncpy(message.text.reserved, "1", sizeof(message.text.reserved)-1);
		cfg_set_object_attr(cwmpPath, "Manual_Inform" ,"0");	
	}
	if((strcmp(MaintainValue, "1") == 0) || (strcmp(changeFlagStr, "1") == 0))
	{
		strncpy(message.text.reserved, "2", sizeof(message.text.reserved) - 1);
		cfg_set_object_attr(cwmpPath, "Maintain_Inform" ,"0");	
	}
#endif

#if defined(TCSUPPORT_CT_MIDWARE)  
	if(midware_start(path) == 1){
		strncpy(message.text.reserved ,"8",sizeof(message.text.reserved)-1);
	}
#endif
#if defined(TCSUPPORT_CT_PPPOE_EMULATOR)	 
	cfg_obj_get_object_attr(cwmpPath, "pppoe_emulator_finish_flag", 0, changeFlagStr, sizeof(changeFlagStr));
	if(strcmp(changeFlagStr, "1") == 0)
	{
		strncpy(message.text.reserved ,"5", sizeof(message.text.reserved));
		cfg_set_object_attr(cwmpPath, "pppoe_emulator_finish_flag" ,"0");
	}
#endif
	cfg_set_object_attr(GLOBALSTATE_COMMON_NODE, "cwmp_change" ,"0");
	message.cwmptype = mq;
	ret = sendmegq(type,&message,flag);
	if(ret < 0)
	{
		printf("\n [%s],line = %d, send message error!leave ==================>\n",__FUNCTION__,__LINE__);
		return ret;
	}
	else
	{
		return SUCCESS;
	}
}

int cfg_type_cwmp_func_commit(char* path)
{
	cfg_type_cwmp_write( );
	return cfg_type_cwmp_execute(path);
}


static cfg_node_ops_t cfg_type_cwmp_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_cwmp_func_commit 
}; 


static cfg_node_type_t cfg_type_cwmp_entry = { 
	 .name = "Entry", 
	 .flag = 1, 
	 .parent = &cfg_type_cwmp, 
	 .ops = &cfg_type_cwmp_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_cwmp_tr143udpecho_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_cwmp_func_commit 
}; 


static cfg_node_type_t cfg_type_cwmp_tr143udpecho = { 
	 .name = "tr143UDPEcho", 
	 .flag = 1, 
	 .parent = &cfg_type_cwmp, 
	 .ops = &cfg_type_cwmp_tr143udpecho_ops, 
}; 


static cfg_node_ops_t cfg_type_cwmp_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_cwmp_func_commit 
}; 


static cfg_node_type_t* cfg_type_cwmp_child[] = { 
	 &cfg_type_cwmp_entry, 
	 &cfg_type_cwmp_tr143udpecho, 
	 NULL 
}; 


cfg_node_type_t cfg_type_cwmp = { 
	 .name = "Cwmp", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_cwmp_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_cwmp_child, 
	 .ops = &cfg_type_cwmp_ops, 
}; 
