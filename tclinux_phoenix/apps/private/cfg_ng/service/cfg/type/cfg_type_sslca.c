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
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"
#include <flash_layout/tc_partition.h>

#define TMP_CA_PATH "/var/tmp/CA.pem" 
#define UPLOAD_CA_PATH "/var/tmp/ca"
#define TMP_ROMFILE_PATH "/var/romfile.cfg"
#define CA_NODE_NAME "Entry"
#define CA_UPGRADE_FLAG "UpgradeFlag"
#define MAX_CA_NUM 4
#define MAX_CA_SIZE 4096
#define MAX_ROMFILE_SIZE (64*1024*2)
#define FRAG_SIZE 512

#if defined(TCSUPPORT_CWMP_OPENSSL)
#define CA_CHECK_TEMP_FILE "/tmp/ca_file_temp.chk"
#define CA_CRT_ENDINFO "-----END CERTIFICATE-----"
#define CA_KEY_ENDINFO "-----END PRIVATE KEY-----"
#define CA_KEY_ENDINFO2 "-----END RSA PRIVATE KEY-----"

#define SSL_SERVER_CA "/etc/ssl_server.crt"
#define SSL_CLIENT_CERT "/etc/ssl_client.crt"
#define SSL_CLIENT_KEY "/etc/ssl_client.pem"
#define CAfile "/etc/CustomerCA.pem"

#define CA_PATH "/etc/ssl_server.crt"
#else
#define CA_PATH "/etc/CustomerCA.pem"
#endif

#if defined(TCSUPPORT_CT_E8GUI)
static int setCertistatus(int ista)
{
	char str[4] = {0};

	snprintf(str, sizeof(str), "%d", ista);
	cfg_set_object_attr(CWMP_ENTRY_NODE, "CertCode" ,str);
	
	return SUCCESS;
}

int writecafiletoflash(int index)
{
	FILE	*fw = NULL;
	FILE	*fr = NULL;
	struct stat fstat;
	char *pCAfile = NULL;
	char	cmd_buf[128] = {0};
	char	filePath[64] = {0};
	char	value[16] = {0};
	char	*pStart, *pEnd = NULL;
	char *pBuf = NULL;
	int nLength = 0;
	long caSize = 0;
	int reVal = SUCCESS;
	unsigned long ulSize[4] = {CERM1_RA_SIZE, CERM2_RA_SIZE, CERM3_RA_SIZE, CERM4_RA_SIZE};
	unsigned long ulOffsite[4] = {CERM1_RA_OFFSET, CERM2_RA_OFFSET, CERM3_RA_OFFSET, CERM4_RA_OFFSET};
	int res = 0;

	memset(filePath,0,sizeof(filePath));
	strncpy(filePath, "/tmp/boa-temp", sizeof(filePath)-1);
	if(/*stat(filePath,&fstat) != 0 || */(fr = fopen(filePath, "r")) == NULL){
		reVal = FAIL;
		goto Error;
	}
	fseek(fr, 0, SEEK_END);
	caSize = ftell(fr);
	fseek(fr, 0, SEEK_SET);
	/*Check CA size and content*/
	if(caSize > MAX_CA_SIZE || caSize < 0){
		reVal = FAIL;
		setCertistatus(5);
		goto Error;
	}
	
	pBuf = (char*)malloc(caSize + 1);
	if(!pBuf)
	{
		reVal = FAIL;
		goto Error;
	}
	memset(pBuf, 0x00, caSize + 1);
	res = fseek(fr,0,SEEK_SET);
	res = fread(pBuf,sizeof(char), caSize, fr);
	pBuf[caSize] = '\0';

	if((pStart = strstr(pBuf, "-----BEGIN CERTIFICATE-----")) == NULL)
	{
		reVal = FAIL;
		setCertistatus(1);
		goto Error;
	}
	else
	{
		if((pEnd = strstr(pBuf, "-----END CERTIFICATE-----")) == NULL)
		{
			reVal = FAIL;
			setCertistatus(1);
			goto Error;
		}
		else
		{
			res = fseek(fr,(int)(pStart - pBuf),SEEK_SET);
			nLength = (int)(pEnd+25-pStart);
			if(nLength >= ((~(unsigned int)0)/2))
			{
				reVal = FAIL;
				goto Error;
			}
			pCAfile = (char*)malloc(nLength+1);
			if(!pCAfile)
			{
				reVal = FAIL;
				goto Error;
			}
			memset(pCAfile, 0x00, nLength+1);
			res = fread(pCAfile, sizeof(char), nLength, fr);
		}
	}
	
	memset(filePath,0,sizeof(filePath));
	snprintf(filePath,sizeof(filePath), "%s%d.pem", UPLOAD_CA_PATH, index);
	
	fw = fopen(filePath, "w");
	if(fw == NULL){
		printf("writecafiletoflash:CA temp file %s open failed\n", filePath);
		goto Error;
	}
	pCAfile[nLength] = '\n';
	fwrite(pCAfile, sizeof(char), nLength+1, fw);
	if(fw){
		fclose(fw);
		fw = NULL;
	}

	snprintf(cmd_buf, sizeof(cmd_buf), TC_FLASH_WRITE_CMD, filePath, ulSize[index-1], ulOffsite[index-1] ,RESERVEAREA_NAME);
	system(cmd_buf);
	setCertistatus(0);
	/*write ca to CA_PATH*/
	if(cfg_obj_get_object_attr(CWMP_ENTRY_NODE, "EnableCert", 0, value, sizeof(value)) > 0)
	{
		if(!strcmp(value,"1")){
			fw = fopen(CA_PATH, "w");
			if(fw == NULL){
				printf("writecafiletoflash:CA_PATH file open failed\n");
				goto Error;
			}
			fwrite(pCAfile, sizeof(char), nLength+1, fw);
			if(fw){
				fclose(fw);
				fw = NULL;
			}
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

	return reVal;
}
#endif

#if defined(TCSUPPORT_CWMP_OPENSSL)
int genCAfile(char *filename, int ca_index)
{
	char nodeName[64] = {0};
	char tmpbuf[64] = {0}, cabuf[FRAG_SIZE+32];
	FILE *fp = NULL;
	int FragNum = 0, idx = 0;

	if ( NULL == filename )
		return -1;

	snprintf(nodeName, sizeof(nodeName), SSLCA_ENTRY_NODE, ca_index);
	bzero(tmpbuf, sizeof(tmpbuf));
	if(cfg_obj_get_object_attr(nodeName, "FragNum", 0, tmpbuf, sizeof(tmpbuf)) < 0)
	{	
		return -1;
	}
	
	FragNum = atoi(tmpbuf);
	fp = fopen(filename, "w");
	if ( NULL == fp )
		return -2;

	for (idx = 0; idx < FragNum; idx++)
	{
		bzero(cabuf, sizeof(cabuf));
		bzero(tmpbuf, sizeof(tmpbuf));
		snprintf(tmpbuf, sizeof(tmpbuf), "Frag%d", idx);
		if(cfg_obj_get_object_attr(nodeName, tmpbuf, 0, cabuf, sizeof(cabuf)) < 0)
		{
			fclose(fp);
			fp = NULL;
			return -3;
		}
		fputs(cabuf, fp);
	}

	fclose(fp);
	fp = NULL;

	return 0;
}

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
	int ca_file_len = 0, endinfo_len = 0, ret = 0;
	unsigned int ca_write_len = 0;
	char nodeName[64] = {0};
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
					ret = fwrite(tmp_ca_buf, sizeof(char), ca_write_len + 1, fp_ca);
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

/*
it need check CA valid before CALL writeCA2flash.
*/
int writeCA2flash(char *file, int index)
{
	char cmd_buf[256] = {0};
	unsigned long ulSize[4] = {CERM1_RA_SIZE, CERM2_RA_SIZE
							, CERM3_RA_SIZE, CERM4_RA_SIZE};
	unsigned long ulOffsite[4] = {CERM1_RA_OFFSET, CERM2_RA_OFFSET
							, CERM3_RA_OFFSET, CERM4_RA_OFFSET};

	if ( NULL == file )
		return -1;

	snprintf(cmd_buf, sizeof(cmd_buf)
			, TC_FLASH_WRITE_CMD, file, ulSize[index], ulOffsite[index]
			, RESERVEAREA_NAME);
	system(cmd_buf);
	return 0;
}

#endif


int cfg_type_sslca_write(char* path)
{
	FILE *fp = NULL;
	int index=0, maxCaNum, FragNum;		   
	int i=0, randomFlag=0, reVal=SUCCESS; 	
	char tmpbuf[33], buf[513]={0}, caPath[33];
	char nodePath[64] = {0};

#if defined(TCSUPPORT_CWMP_OPENSSL)
		/* do NOTHING. */
		return SUCCESS;
#else
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(cfg_obj_get_object_attr(SSLCA_FLAG_NODE, CA_UPGRADE_FLAG, 0, tmpbuf, sizeof(tmpbuf)) > 0)
	{
		return SUCCESS;
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(cfg_obj_get_object_attr(SSLCA_COMMON_NODE, "CurIndex", 0, tmpbuf, sizeof(tmpbuf)) < 0)
	{
		return FAIL;
	}
	
	/*if CurIndex !=0,means user want to read a random CA file */
	if((index = atoi(tmpbuf)) == 0)
	{
		memset(tmpbuf, 0, sizeof(tmpbuf));
		if(cfg_obj_get_object_attr(nodePath, "ValidIndex", 0, tmpbuf, sizeof(tmpbuf)) < 0)
		{
			return FAIL;
		}
		index = atoi(tmpbuf);
		strncpy(caPath, CA_PATH, sizeof(caPath)-1);
	}
	else{
		strcpy(caPath, TMP_CA_PATH);
		randomFlag = 1;
	}
	/*Check index and maxCaNum*/
	memset(tmpbuf,0,sizeof(tmpbuf));
	if(cfg_obj_get_object_attr(nodePath, "MaxCANum", 0, tmpbuf, sizeof(tmpbuf)) < 0)
	{
		reVal = FAIL;
		goto Error;
	}
	maxCaNum = atoi(tmpbuf);
	if(maxCaNum < 1 || maxCaNum > MAX_CA_NUM)
	{
		reVal = FAIL;
		goto Error;
	}
	if(index == 0 || index > maxCaNum)
	{
		reVal = FAIL;
		goto Error;
	}
		
	memset(tmpbuf, 0, sizeof(tmpbuf));
	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), SSLCA_ENTRY_NODE,index);
	if(cfg_obj_get_object_attr(nodePath, "FragNum", 0, tmpbuf, sizeof(tmpbuf)) < 0)
	{
		reVal = FAIL;
		goto Error;
	}
	FragNum = atoi(tmpbuf);
	fp = fopen(caPath,"w");
	if(fp == NULL)
	{
		reVal = FAIL;
		goto Error;
	}
	else
	{
		for(i = 0; i < FragNum; i++)
		{
			memset(buf,0,sizeof(buf));
			memset(tmpbuf,0,sizeof(tmpbuf));
			snprintf(tmpbuf,sizeof(tmpbuf),"Frag%d",i);
			if(cfg_obj_get_object_attr(nodePath, tmpbuf, 0, buf, sizeof(buf)) < 0)
			{
				reVal = FAIL;
				goto Error;
			}
			fputs_escape(buf, fp);
		}			
		fclose(fp);
		fp = NULL;
	}
	
	chmod(caPath,777);
Error:
	/*Clear CurIndex to 0*/
	if(randomFlag)
	{
		cfg_set_object_attr(SSLCA_COMMON_NODE, "CurIndex", "0" );
	}
	if(reVal == FAIL)
		unlink(caPath);
	if(fp){
		fclose(fp);
		fp = NULL;
	}
	return reVal;
#endif
}/* end sslca_write */


int cfg_type_sslca_execute(char* path)
{
	char  tmp[32]={0},fileBuf[513]={0},filePath[33]={0};
	struct stat fstat;
	FILE  *fp = NULL;
	int i, index, maxCANum, fragNum, reVal = SUCCESS;
	long caSize = 0;
	char tmpPath[64] = {0};
	char tmpattr[32] = {0};
	char tmpValue[32] = {0};
	
	if(cfg_obj_get_object_attr(SSLCA_FLAG_NODE, CA_UPGRADE_FLAG, 0, tmp, sizeof(tmp)) < 0)
	{
		return SUCCESS;
	}
	index = atoi(tmp);
	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(SSLCA_COMMON_NODE, "MaxCANum", 0, tmp, sizeof(tmp)) < 0)
	{
		reVal = FAIL;
		goto Error;
	}
	maxCANum = atoi(tmp);

	memset(tmp,0,sizeof(tmp));
	if(cfg_obj_get_object_attr(SSLCA_COMMON_NODE, "ValidIndex", 0, tmp, sizeof(tmp)) < 0)
	{
		reVal = FAIL;
		goto Error;
	}
	
	if(index < 1 || index > maxCANum)
	{
		reVal = FAIL;
		goto Error;
	}
#if !defined(TCSUPPORT_CWMP_OPENSSL)		
#if defined(TCSUPPORT_CT_E8GUI)
	if(FAIL == writecafiletoflash(index))
	{
		printf("sslca_execute:write ca file to flash failed!!\n");
	}
#endif
#endif
	memset(filePath,0,sizeof(filePath));
	snprintf(filePath,sizeof(filePath), "%s%d.pem", UPLOAD_CA_PATH, index);
	if(/*stat(filePath,&fstat) != 0 || */(fp = fopen(filePath, "r")) == NULL){
		reVal = FAIL;
		goto Error;
	}
	fseek(fp, 0, SEEK_END);
	caSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	/*Check CA size and content*/
	if((caSize/* = fstat.st_size*/) > MAX_CA_SIZE){
		reVal = FAIL;
		goto Error;
	}	
#if !defined(TCSUPPORT_CWMP_OPENSSL)	
	/*Check romfile size*/
	if(stat(TMP_ROMFILE_PATH,&fstat) != 0 ){
		reVal = FAIL;
		goto Error;
	}
	/*(caSize + 100) means there are node information in romfile*/
	if((MAX_ROMFILE_SIZE - fstat.st_size) < (caSize + 100)){
		reVal = FAIL;
		goto Error;
	}
#endif
	memset(fileBuf,0,sizeof(fileBuf));
	/*Read the start and end of CA*/
	fread(fileBuf, sizeof(char), 40, fp);
	fseek(fp, caSize - 40, SEEK_SET );
	fread(fileBuf + 40, sizeof(char), 40, fp);

#if defined(TCSUPPORT_CWMP_OPENSSL)
	if ( 2 == index )
	{
		if( !( (strstr(fileBuf, "-----BEGIN PRIVATE KEY-----")
			&& strstr(fileBuf, "-----END PRIVATE KEY-----"))
			|| (strstr(fileBuf, "-----BEGIN RSA PRIVATE KEY-----")
			&& strstr(fileBuf, "-----END RSA PRIVATE KEY-----")) ) )
		{
			printf("sslca_execute:Save key failed,%s is not a correct KEY!\n", filePath);
		reVal = FAIL;
		goto Error;
	}
	}
	else
#endif
	{
		if(strstr(fileBuf, "-----BEGIN CERTIFICATE-----") == NULL || strstr(fileBuf, "-----END CERTIFICATE-----") == NULL)
		{
			reVal = FAIL;
			goto Error;
		}
	}

#if defined(TCSUPPORT_CWMP_OPENSSL)
		writeCA2flash(filePath, index - 1);
		loadCAFromFlash( index - 1);
#else
	/*Clean node data before set node*/  
	memset(tmpPath, 0, sizeof(tmpPath));
	snprintf(tmpPath, sizeof(tmpPath), SSLCA_ENTRY_NODE, index);  
	cfg_delete_object(tmpPath);  
	cfg_create_object(tmpPath);      
	
	fseek(fp,0,SEEK_SET);
	fragNum = (caSize + FRAG_SIZE - 1) / FRAG_SIZE;
	
	for(i = 0; i < fragNum; i++)
	{
		memset(fileBuf,0,sizeof(fileBuf));
		fread(fileBuf,sizeof(char), FRAG_SIZE, fp);
		memset(tmpattr, 0, sizeof(tmpattr));
		snprintf(tmpattr, sizeof(tmpattr),"Frag%d", i);
		if(cfg_set_object_attr(tmpPath, tmpattr, fileBuf ) < 0)
		{	
			reVal = FAIL;
			goto Error;
		}		
	}
	
	snprintf(tmpValue, sizeof(tmpValue), "%d", fragNum);
	if(cfg_set_object_attr(tmpPath, "FragNum", tmpValue ) <= 0)
	{	
		reVal = FAIL;
		goto Error;
	}
#endif	
Error:
	if(fp){
		fclose(fp);
		fp = NULL;
	}
	unlink(filePath);
	/*Delete the flag node*/
	cfg_delete_object(SSLCA_FLAG_NODE);      

	return reVal;
}/* end sslca_execute */



int cfg_type_sslca_func_commit(char* path)
{	
	cfg_type_sslca_write(path);
	return cfg_type_sslca_execute(path);
}


static char* cfg_type_sslca_index[] = { 
	 "sslca_id", 
	 NULL 
}; 


static cfg_node_ops_t cfg_type_sslca_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sslca_func_commit 
}; 


static cfg_node_type_t cfg_type_sslca_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | 4, 
	 .parent = &cfg_type_sslca, 
	 .index = cfg_type_sslca_index, 
	 .ops = &cfg_type_sslca_entry_ops, 
}; 


static cfg_node_ops_t cfg_type_sslca_common_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sslca_func_commit 
}; 


static cfg_node_type_t cfg_type_sslca_common = { 
	 .name = "Common", 
	 .flag = 1, 
	 .parent = &cfg_type_sslca, 
	 .ops = &cfg_type_sslca_common_ops, 
}; 


static cfg_node_ops_t cfg_type_sslca_flag_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sslca_func_commit 
}; 


static cfg_node_type_t cfg_type_sslca_flag = { 
	 .name = "Flag", 
	 .flag = 1, 
	 .parent = &cfg_type_sslca, 
	 .ops = &cfg_type_sslca_flag_ops, 
}; 


static cfg_node_ops_t cfg_type_sslca_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_sslca_func_commit 
}; 


static cfg_node_type_t* cfg_type_sslca_child[] = { 
	 &cfg_type_sslca_entry, 
	 &cfg_type_sslca_common, 
	 &cfg_type_sslca_flag, 
	 NULL 
}; 


cfg_node_type_t cfg_type_sslca = { 
	 .name = "SslCA", 
	 .flag = 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_sslca_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_sslca_child, 
	 .ops = &cfg_type_sslca_ops, 
}; 
