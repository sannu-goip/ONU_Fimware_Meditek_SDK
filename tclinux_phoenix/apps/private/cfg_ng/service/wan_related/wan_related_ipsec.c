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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <svchost_api.h> 
#include <cfg_api.h>
#include "../cfg/cfg_msg.h"

#include "wan_related_mgr.h"
#include "utility.h"

#define IPSEC		        "IPSEC"
#define IPSEC_PSK_FILE		"/etc/ipsec_psk.txt"
#define IPSEC_SP_FILE			"/etc/ipsec_setkey.conf"
#define IPSEC_SA_FILE			"/etc/ipsec_racoon.conf"
#define IPSEC_SETKEY_BIN		"/usr/bin/setkey"
#define IPSEC_RACOON_BIN		"/usr/bin/racoon"
#define IPSEC_ADD_RULE_FILE	"/etc/ipsec_addIpTableRules"
#define IPSEC_DEL_RULE_FILE	"/etc/ipsec_delIpTableRules"
#define IPSEC_DEL_RULE_FILE2	"/etc/ipsec_delIpTableRules2"
#define MAX_NODE_NAME       32

extern int cfg_obj_get_object_attr(char* path,char* attr,int type,char* val,int size);

static int isIpsecIptablesSet=0;

char subnetTable[33][16] =
{
	{"0.0.0.0"},{"128.0.0.0"},{"192.0.0.0"},{"224.0.0.0"},
	{"240.0.0.0"},{"248.0.0.0"},{"252.0.0.0"},{"254.0.0.0"},
	{"255.0.0.0"},{"255.128.0.0"},{"255.192.0.0"},{"255.224.0.0"},
	{"255.240.0.0"},{"255.248.0.0"},{"255.252.0.0"},{"255.254.0.0"},
	{"255.255.0.0"},{"255.255.128.0"},{"255.255.192.0"},{"255.255.224.0"},
	{"255.255.240.0"},{"255.255.248.0"},{"255.255.252.0"},{"255.255.254.0"},
	{"255.255.255.0"},{"255.255.255.128"},{"255.255.255.192"},{"255.255.255.224"},
	{"255.255.255.240"},{"255.255.255.248"},{"255.255.255.252"},{"255.255.255.254"},
	{"255.255.255.255"}
};

static unsigned int
subnetMask2Prefix(
	char *subnetMask
)
{
	unsigned int i;
	
	for (i = 0; i < 33; i++)
	{
		if (strcmp(subnetMask, subnetTable[i]) == 0)
			return i;		
	}
    return 0;
}


char exchangeModeTable[][32] =
{
	{"main, aggressive, base"},
	{"aggressive, main, base"},
	{"base, main, aggressive"}
};

static void
exchangeModeExtend(
	char *mode
)
{
	strcpy(mode, exchangeModeTable[atoi(mode)]);
}


char cipherAlgoTable[][64] =
{
	{"des, 3des, aes 128, aes 192, aes 256"},
	{"3des, des, aes 128, aes 192, aes 256"},
	{"aes 128, 3des, des, aes 192, aes 256"},
	{"aes 192, 3des, des, aes 128, aes 256"},
	{"aes 256, 3des, des, aes 128, aes 192"},
};

static void
cipherAlgoExtend(
	char *cipher
)
{
	strcpy(cipher, cipherAlgoTable[atoi(cipher)]);
}


char authAlgoTable[][64] =
{
	{"hmac_md5, hmac_sha1, hmac_sha256"},
	{"hmac_sha1, hmac_md5, hmac_sha256"},
	{"hmac_sha256, hmac_md5, hmac_sha1"},
};

static void
authAlgoExtend(
	char *auth
)
{
	strcpy(auth, authAlgoTable[atoi(auth)]);
}

static void
lifetimeChange(
	char *timeS,
	char *unit
)
{
	int timeI = atoi(timeS);

	if (strcmp(unit, "secs") == 0)
	{
		timeI = timeI * 12 / 10;
		sprintf(timeS, "%d", timeI);
		
	}
	else if (strcmp(unit, "mins") == 0)
	{
		timeI = timeI * 72; //60 * 12 / 10
		sprintf(timeS, "%d", timeI);
		strcpy(unit, "secs");
	}
	else if (strcmp(unit, "hours") == 0)
	{
		timeI = timeI * 72; //60 * 12 / 10
		sprintf(timeS, "%d", timeI);
		strcpy(unit, "mins");	
	}
}

int ipsec_write(void)
{
	FILE *fp_psk, *fp_sp, *fp_sa, *fp_add, *fp_del;
	char tmpString[256];
	unsigned int i, localPrefix, remotePrefix, isCfgFileCreate = 0;
	char nodeName[MAX_NODE_NAME];
	char connectionName[32], remoteGW[32], localGW[32], localSubnetOrIP[16], localAddr[32], localMask[32], remoteSubnetOrIP[16], remoteAddr[32], remoteMask[32], preSharedKey[32], pfsEnable[16], phase1Mode[32], phase1Cipher[16], phase1Auth[16], phase1DhGroup[8], phase1KeyLife[32], phase1KeyUnit[8], phase2Cipher[64], phase2Auth[64], phase2PfsGroup[8], phase2KeyLife[32], phase2KeyUnit[8];
#if defined(TCSUPPORT_CMCCV2)
	char tmpNodeStr[64];
	int phase1AuthMethod = 0; //0:PreShareKey, 1:RsaSignature
	int DPDEnable = 0; //0:Disable, 1:Enable
	int transformMode = 0; //0:ESP, 1:AH
	int encapsulationMode = 0; //0:Tunnel, 1:Transport
#endif
	int ret = 0;	

    //printf("Trey(%s)\n", __func__);
	fp_psk = fopen(IPSEC_PSK_FILE, "w");
	if(fp_psk == NULL)
	{
		return FAIL;
	}
	
	fp_sp =  fopen(IPSEC_SP_FILE, "w");
	if(fp_sp == NULL)
	{
		fclose(fp_psk);
		return FAIL;
	}
	
	fp_sa =  fopen(IPSEC_SA_FILE, "w");
	if(fp_sa == NULL)
	{
		fclose(fp_psk);
		fclose(fp_sp);
		return FAIL;
	}
	
	fp_del =  fopen(IPSEC_DEL_RULE_FILE, "r");
	if(fp_del)
	{
		fclose(fp_del);
		ret = rename(IPSEC_DEL_RULE_FILE, IPSEC_DEL_RULE_FILE2);
		if ( ret ) /* for fix cfgng warning compile error. */
			;
	}
	
	fp_add =  fopen(IPSEC_ADD_RULE_FILE, "w");
	if(fp_add == NULL)
	{
		fclose(fp_psk);
		fclose(fp_sp);
		fclose(fp_sa);
		return FAIL;
	}	

	fp_del =  fopen(IPSEC_DEL_RULE_FILE, "w");
	if(fp_del == NULL)
	{
		fclose(fp_psk);
		fclose(fp_sp);
		fclose(fp_sa);
		fclose(fp_add);
		return FAIL;
	}

		
	//pre-write for IPSEC_SP_FILE
	fputs("#!/usr/ipsec_tools/sbin/setkey -f\n\n\n", fp_sp);
	fputs("# Flush the SAD and SPD\n", fp_sp);
	fputs("flush;\n", fp_sp);
	fputs("spdflush;\n", fp_sp);
	//pre-write for IPSEC_SA_FILE
	sprintf(tmpString, "path pre_shared_key \"%s\";\n\n", IPSEC_PSK_FILE);
	fputs(tmpString, fp_sa);
#if defined(TCSUPPORT_CMCCV2)
	sprintf(tmpString, "path certificate \"/userfs/\";\n\n");
	fputs(tmpString, fp_sa);
#endif
	fputs("timer {\n", fp_sa);
	fputs("\tcounter 150;\n", fp_sa);
	fputs("\tphase1 30 mins;\n", fp_sa);
	fputs("\tphase2 30 mins;\n", fp_sa);
	fputs("}\n", fp_sa);
	
	//pre-write for IPSEC_ADD_RULE_FILE
	fputs("#!/bin/sh\n\n", fp_add);
	//pre-write for IPSEC_DEL_RULE_FILE
	fputs("#!/bin/sh\n\n", fp_del);
		
		
	for (i = 0; i < MAX_IPSEC_NUM; i++)
	{
		memset(nodeName,0,sizeof(nodeName));
        /* path's entry starts from 1, that is , root.ipsec.entry.1 stands for IPSEC_Entry0 */
		snprintf(nodeName, sizeof(nodeName), IPSEC_ENTRY_NODE, i+1);
		
		//get attributes from node IPSEC_Entry%d

        if (cfg_obj_get_object_attr(nodeName,"Connection_Name", 0, connectionName, sizeof(connectionName))<0)
            continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Gateway_IP", 0, remoteGW, sizeof(remoteGW))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Local_Gateway_IP", 0, localGW, sizeof(localGW))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Local_SubnetOrIP", 0, localSubnetOrIP, sizeof(localSubnetOrIP))<0)
            continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Local_IP", 0, localAddr, sizeof(localAddr))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Local_SubnetMask", 0, localMask, sizeof(localMask))<0)
            continue;			

        if (cfg_obj_get_object_attr(nodeName,"Remote_SubnetOrIP", 0, remoteSubnetOrIP, sizeof(remoteSubnetOrIP))<0)
            continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Remote_IP", 0, remoteAddr, sizeof(remoteAddr))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Remote_SubnetMask", 0, remoteMask, sizeof(remoteMask))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"PreShared_Key", 0, preSharedKey, sizeof(preSharedKey))<0)
            continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Pfs_Enable", 0, pfsEnable, sizeof(pfsEnable))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Phase1_Mode", 0, phase1Mode, sizeof(phase1Mode))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Phase1_EncryptAlgo", 0, phase1Cipher, sizeof(phase1Cipher))<0)
            continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Phase1_AuthAlgo", 0, phase1Auth, sizeof(phase1Auth))<0)
            continue;

#if defined(TCSUPPORT_CMCCV2)
        if (cfg_obj_get_object_attr(nodeName,"Phase1_AuthMethod", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
            continue;
        else
        {
            if (strcmp(tmpNodeStr, "RsaSignature") == 0) //IKEAuthenticationMethod = RsaSignature
                phase1AuthMethod = 1;
        }
#endif

        if (cfg_obj_get_object_attr(nodeName,"Phase1_DhGroup", 0, phase1DhGroup, sizeof(phase1DhGroup))<0)
            continue;

#if defined(TCSUPPORT_CMCCV2)
        if (cfg_obj_get_object_attr(nodeName,"DPD_Enable", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
            continue;
        else
        {
            if (strcmp(tmpNodeStr, "TRUE") == 0) //DPD Enable
                DPDEnable = 1;
        }
#endif

        if (cfg_obj_get_object_attr(nodeName,"Phase1_KeyLifeTime", 0, phase1KeyLife, sizeof(phase1KeyLife))<0)
            continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Phase1_KeyLifeUnit", 0, phase1KeyUnit, sizeof(phase1KeyUnit))<0)
            continue;

#if defined(TCSUPPORT_CMCCV2)
        if (cfg_obj_get_object_attr(nodeName,"IPSec_Transform", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
            continue;
        else
        {
            if (strcmp(tmpNodeStr, "AH") == 0) //IPSecTransform = AH
                transformMode = 1;
            else //IPSecTransform = ESP
            {
                if (cfg_obj_get_object_attr(nodeName,"Phase2_EncryptAlgo", 0, phase2Cipher, sizeof(phase2Cipher))<0)
                    continue;

                if (cfg_obj_get_object_attr(nodeName,"Phase2_AuthAlgo", 0, phase2Auth, sizeof(phase2Auth))<0)
                    continue;
            }
        }
#else
        if (cfg_obj_get_object_attr(nodeName,"Phase2_EncryptAlgo", 0, phase2Cipher, sizeof(phase2Cipher))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Phase2_AuthAlgo", 0, phase2Auth, sizeof(phase2Auth))<0)
            continue;

#endif

        if (strcmp(pfsEnable, "Enable") == 0)
            if (cfg_obj_get_object_attr(nodeName,"Phase2_PfsGroup", 0, phase2PfsGroup, sizeof(phase2PfsGroup))<0)
                continue;
        
        if (cfg_obj_get_object_attr(nodeName,"Phase2_KeyLifeTime", 0, phase2KeyLife, sizeof(phase2KeyLife))<0)
            continue;

        if (cfg_obj_get_object_attr(nodeName,"Phase2_KeyLifeUnit", 0, phase2KeyUnit, sizeof(phase2KeyUnit))<0)
            continue;

#if defined(TCSUPPORT_CMCCV2)
        if (cfg_obj_get_object_attr(nodeName,"IPSec_Encap_Mode", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
            continue;
        else
        {
            if (strcmp(tmpNodeStr, "Transport") == 0) //Transport mode
                encapsulationMode = 1;
        }
#endif

		localPrefix = subnetMask2Prefix(localMask);
		remotePrefix = subnetMask2Prefix(remoteMask);
		exchangeModeExtend(phase1Mode);
		cipherAlgoExtend(phase2Cipher);
		authAlgoExtend(phase2Auth);
		lifetimeChange(phase1KeyLife, phase1KeyUnit);
		lifetimeChange(phase2KeyLife, phase2KeyUnit);
			
					
		//write to IPSEC_PSK_FILE
		
		sprintf(tmpString, "%s %s\n", remoteGW, preSharedKey);
		fputs_escape(tmpString, fp_psk);
	
		
		//write to IPSEC_SP_FILE
		
		sprintf(tmpString, "\n\n# Security Policy for Connection \"%s\"\n", connectionName);
		fputs_escape(tmpString, fp_sp);
		
#if defined(TCSUPPORT_CMCCV2)
		if (transformMode == 1) //IPSecTransform = AH
		{
			system("echo 10001 > /proc/tc3162/crypto_debug"); // to set ipsec_ah_flag = 1
		}
		else //IPSecTransform = ESP
		{
			system("echo 10000 > /proc/tc3162/crypto_debug"); // to set ipsec_ah_flag = 0
		}
		if (encapsulationMode == 1) //Transport mode
		{
			sprintf(tmpString, "spdadd %s %s any -P out ipsec %s/transport//require;\n", localGW, remoteGW, (transformMode == 1) ? "ah" : "esp");
		}
		else //Tunnel mode
		{
			sprintf(tmpString, "spdadd %s/%d %s/%d any -P out ipsec %s/tunnel/%s-%s/require;\n", localAddr, localPrefix, remoteAddr, remotePrefix, (transformMode == 1) ? "ah" : "esp", localGW, remoteGW);
		}
		fputs_escape(tmpString, fp_sp);

		if (encapsulationMode == 1) //Transport mode
		{
			sprintf(tmpString, "spdadd %s %s any -P in ipsec %s/transport//require;\n", remoteGW, localGW, (transformMode == 1) ? "ah" : "esp");
		}
		else //Tunnel mode
		{
			sprintf(tmpString, "spdadd %s/%d %s/%d any -P in ipsec %s/tunnel/%s-%s/require;\n", remoteAddr, remotePrefix, localAddr, localPrefix, (transformMode == 1) ? "ah" : "esp", remoteGW, localGW);
		}
		fputs_escape(tmpString, fp_sp);
#else
		sprintf(tmpString, "spdadd %s/%d %s/%d any -P out ipsec esp/tunnel/%s-%s/require;\n", localAddr, localPrefix, remoteAddr, remotePrefix, localGW, remoteGW);
		fputs_escape(tmpString, fp_sp);
		
		sprintf(tmpString, "spdadd %s/%d %s/%d any -P in ipsec esp/tunnel/%s-%s/require;\n", remoteAddr, remotePrefix, localAddr, localPrefix, remoteGW, localGW);
		fputs_escape(tmpString, fp_sp);
#endif
		
        //write to IPSEC_SA_FILE
        
        sprintf(tmpString, "\n\n# Security Association for Connection \"%s\"\n", connectionName);
        fputs_escape(tmpString, fp_sa);
        
#if defined(TCSUPPORT_CMCCV2)
        sprintf(tmpString, "remote %s {\n"
                            "\texchange_mode %s;\n"
                            "\tproposal_check obey;\n"
                            "\trekey force;\n"
                            "\tlifetime time %s %s;\n"
                            ,remoteGW, phase1Mode, phase1KeyLife, phase1KeyUnit);
        fputs_escape(tmpString, fp_sa);

        if (DPDEnable == 1)
        {
            if (cfg_obj_get_object_attr(nodeName,"DPD_Delay", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
                continue;
            else
            {
                sprintf(tmpString, "\tdpd_delay %s;\n", tmpNodeStr);
                fputs_escape(tmpString, fp_sa);
            }

            if (cfg_obj_get_object_attr(nodeName,"DPD_Retry", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
                continue;
            else
            {
                sprintf(tmpString, "\tdpd_retry %s;\n", tmpNodeStr);
                fputs_escape(tmpString, fp_sa);
            }
        }

        if (phase1AuthMethod == 1) //IKEAuthenticationMethod = RsaSignature
        {
            sprintf(tmpString, "\tcertificate_type x509 \"ipsec.crt\" \"ipsec.key\";\n"
                               "\tpeers_certfile x509 \"ipsec.crt\";\n"
                               "\tmy_identifier asn1dn;\n");
            fputs_escape(tmpString, fp_sa);
        }

        sprintf(tmpString, "\tproposal {\n"
                            "\t\tencryption_algorithm %s;\n"
                            "\t\thash_algorithm %s;\n"
                            ,phase1Cipher, phase1Auth);
        fputs_escape(tmpString, fp_sa);

        if (phase1AuthMethod == 1) //IKEAuthenticationMethod = RsaSignature
        {
            sprintf(tmpString, "\t\tauthentication_method rsasig;\n");
            fputs_escape(tmpString, fp_sa);
        }
        else //IKEAuthenticationMethod = PreShareKey
        {
            sprintf(tmpString, "\t\tauthentication_method pre_shared_key;\n");
            fputs_escape(tmpString, fp_sa);
        }

        sprintf(tmpString, "\t\tdh_group %s;\n"
                            "\t}\n}\n"
                            ,phase1DhGroup);
        fputs_escape(tmpString, fp_sa);

        if (encapsulationMode == 1) //Transport mode
        {
            sprintf(tmpString, "\nsainfo address %s any address %s any {\n", localGW, remoteGW);
        }
        else //Tunnel mode
        {
            sprintf(tmpString, "\nsainfo address %s/%d any address %s/%d any {\n", localAddr, localPrefix, remoteAddr, remotePrefix);
        }
        fputs_escape(tmpString, fp_sa);
#else
        sprintf(tmpString, "remote %s {\n"
                            "\texchange_mode %s;\n"
                            "\tproposal_check obey;\n"
                            "\trekey force;\n"
                            "\tlifetime time %s %s;\n"
                            "\tproposal {\n"
                            "\t\tencryption_algorithm %s;\n"
                            "\t\thash_algorithm %s;\n"
                            "\t\tauthentication_method pre_shared_key;\n"
                            "\t\tdh_group %s;\n"
                            "\t}\n}\n"
                            ,remoteGW, phase1Mode, phase1KeyLife, phase1KeyUnit, phase1Cipher, phase1Auth, phase1DhGroup);
        fputs_escape(tmpString, fp_sa);

        sprintf(tmpString, "\nsainfo address %s/%d any address %s/%d any {\n", localAddr, localPrefix, remoteAddr, remotePrefix);
        fputs_escape(tmpString, fp_sa);
#endif
        
        if (strcmp(pfsEnable, "Enable") == 0)
        {
            sprintf(tmpString, "\tpfs_group %s;\n"
                            ,phase2PfsGroup);
            fputs_escape(tmpString, fp_sa);         
        }
        
#if defined(TCSUPPORT_CMCCV2)
        sprintf(tmpString, "\tlifetime time %s %s;\n"
                            ,phase2KeyLife, phase2KeyUnit);
        fputs_escape(tmpString, fp_sa);

        if (cfg_obj_get_object_attr(nodeName,"Phase2_LifeKbyte", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
            continue;
        else
        {
            sprintf(tmpString, "\tlifetime byte %s KB;\n"
                                , tmpNodeStr);
            fputs_escape(tmpString, fp_sa);
        }

        if (transformMode == 1) //IPSecTransform = AH
        {
            if (cfg_obj_get_object_attr(nodeName,"Phase2_AH_AuthAlgo", 0, tmpNodeStr, sizeof(tmpNodeStr))<0)
                continue;
            else
            {
                authAlgoExtend(tmpNodeStr);
            }
            sprintf(tmpString, "\tencryption_algorithm 3des, des, aes 128, aes 192, aes 256;\n"
                                "\tauthentication_algorithm %s;\n"
                                ,tmpNodeStr);
        }
        else //IPSecTransform = ESP
        {
            sprintf(tmpString, "\tencryption_algorithm %s;\n"
                                "\tauthentication_algorithm %s;\n"
                                ,phase2Cipher, phase2Auth);
        }
        fputs_escape(tmpString, fp_sa);

        sprintf(tmpString, "\tcompression_algorithm deflate;\n"
                            "}\n");
        fputs_escape(tmpString, fp_sa);
#else
        sprintf(tmpString, "\tlifetime time %s %s;\n"
                            "\tencryption_algorithm %s;\n"
                            "\tauthentication_algorithm %s;\n"
                            "\tcompression_algorithm deflate;\n"
                            "}\n"
                            ,phase2KeyLife, phase2KeyUnit, phase2Cipher, phase2Auth);
        fputs_escape(tmpString, fp_sa);
        
#endif
        
        //write to IPSEC_ADD_RULE_FILE    
        sprintf(tmpString, "iptables -t nat -I POSTROUTING -s %s/%d -d %s/%d -j ACCEPT\n",localAddr, localPrefix, remoteAddr, remotePrefix);
        fputs_escape(tmpString, fp_add);
        
        //write to IPSEC_DEL_RULE_FILE
        sprintf(tmpString, "iptables -t nat -D POSTROUTING -s %s/%d -d %s/%d -j ACCEPT\n",localAddr, localPrefix, remoteAddr, remotePrefix);
        fputs_escape(tmpString, fp_del);    
        
        isCfgFileCreate = 1;
    }       
        
    
    fclose(fp_psk);
    fclose(fp_sp);
    fclose(fp_sa);
    fclose(fp_add);
    fclose(fp_del);
    
    if (isCfgFileCreate)
    {
        sprintf(tmpString, "chmod 400 %s", IPSEC_PSK_FILE);
        system(tmpString);
        sprintf(tmpString, "chmod 777 %s %s %s %s", IPSEC_SP_FILE, IPSEC_SA_FILE, IPSEC_ADD_RULE_FILE, IPSEC_DEL_RULE_FILE);
        system(tmpString);
    }
    else
    {
        unlink(IPSEC_PSK_FILE);
        unlink(IPSEC_SP_FILE);
        unlink(IPSEC_SA_FILE);
        unlink(IPSEC_ADD_RULE_FILE);
        unlink(IPSEC_DEL_RULE_FILE);
    }
        
    
    return SUCCESS;
}

int ipsec_execute()
{
    FILE *fp;
    char tmpString[64];
    unsigned int isSaFileExist = 0;


    //printf("Trey(%s)\n", __func__);
    fp = fopen(IPSEC_SA_FILE, "r");
    if (fp != NULL)
    {
        isSaFileExist = 1;
        fclose(fp);
        system("kill `pidof racoon`");
    }
    
    fp = fopen(IPSEC_SP_FILE, "r");
    if (fp != NULL)
    {
        fclose(fp);
        sprintf(tmpString, "%s -f %s", IPSEC_SETKEY_BIN, IPSEC_SP_FILE);
        system(tmpString);
        
        if (isSaFileExist)
        {
            sprintf(tmpString, "%s -f %s", IPSEC_RACOON_BIN, IPSEC_SA_FILE);
            system(tmpString);
        }       
    }
    
    fp = fopen(IPSEC_DEL_RULE_FILE2, "r");
    if (fp != NULL)
    {
        fclose(fp);
        system(IPSEC_DEL_RULE_FILE2);
        unlink(IPSEC_DEL_RULE_FILE2);
    }
    
    fp = fopen(IPSEC_ADD_RULE_FILE, "r");
    if (fp != NULL)
    {
        fclose(fp);
        system(IPSEC_ADD_RULE_FILE);
    }


    return SUCCESS;
}

void ipsec_iptables_set (void)
{
    if (isIpsecIptablesSet)
        return;

    system_escape("iptables -t filter -D FIREWALL -p udp --dport 500 -j ACCEPT 2>/dev/null");
    system_escape("iptables -t filter -I FIREWALL -p udp --dport 500 -j ACCEPT 2>/dev/null");
    system_escape("iptables -t filter -D FIREWALL -p esp -j ACCEPT 2>/dev/null");
    system_escape("iptables -t filter -I FIREWALL -p esp -j ACCEPT 2>/dev/null");
#if defined(TCSUPPORT_CMCCV2)
    system_escape("iptables -t filter -D FIREWALL -p ah -j ACCEPT 2>/dev/null");
    system_escape("iptables -t filter -I FIREWALL -p ah -j ACCEPT 2>/dev/null");
#endif

    isIpsecIptablesSet=1;
    return;
}

int svc_wan_related_ipsec_update(void)
{
    if (ipsec_write() == FAIL) {
        tcdbg_printf("ipsec_write fail!\n");
        return FAIL;
    }

    if (ipsec_execute() == FAIL) {
        tcdbg_printf("ipsec_execute fail!\n");
        return FAIL;
    }

    ipsec_iptables_set();

    tcdbg_printf("%s done!\n", __func__);
    return SUCCESS;
}

