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
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h" 

#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG_ENHANCE)
#include <syslog.h>
#endif

#define PLUGIN_C_PATHS			"/usr/osgi/plugin-c" 	/* path for plugin c*/
#define PLUGIN_B_PATHS			"/usr/osgi/plugin-b" 	/* path for plugin b*/
#define PLUGIN_C_PIDS			"/usr/osgi/plugin-c/%s/pid"  /* pid for plugin c*/
#define PLUGIN_B_STATES			"/usr/osgi/plugin-b/%s/state"  /* state for plugin b*/
#define PLUGININFO_PATH			"/tmp/plugininfo"
#define PLUGININFO_TEMP			"/tmp/temp_plugininfo"
#define CLIENTINFO_PATH			"/tmp/cloudclientinfo"
#define CLIENTINFO_TEMP			"/tmp/temp_clientinfo"
#define DEBUS_PAUSERUNPLUGIN_CMD "dbus-send --system --print-reply --dest=com.ctc.saf1 /com/ctc/saf1 com.ctc.saf1.framework.Pause uint32:\"%s\" "

#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG_ENHANCE)
#define	MAX_APP_NUM	20
#define 	PLUGINNAME_TEMP		"/tmp/temp_pluginname"
char appArray[MAX_APP_NUM][64] ={0};
int appNum = 0;
#endif

static int cfg_type_plugin_common_func_commit(char* path)
{
	char pauseRunFlag[8] = {0};
	char pauseRunTime[16] = {0};
	char pauseRunCmd[256] = {0};
	struct timeval curTime;
	long cur_time = 0;
	char cur_time_str[32] = {0};

	memset(pauseRunFlag, 0, sizeof(pauseRunFlag));
	memset(pauseRunTime, 0, sizeof(pauseRunTime));
	memset(pauseRunCmd, 0, sizeof(pauseRunCmd));
	memset(&curTime, 0, sizeof(curTime));
	memset(cur_time_str, 0, sizeof(cur_time_str));

	cfg_get_object_attr(PLUGIN_COMMON_NODE, "pauseRunFlag", pauseRunFlag, sizeof(pauseRunFlag));
	cfg_get_object_attr(PLUGIN_COMMON_NODE, "pauseRunTime", pauseRunTime, sizeof(pauseRunTime));
	if(!strcmp(pauseRunFlag , "0"))
	{	
		/* Pause all plugin, set the start pausing seconds*/
		snprintf(pauseRunCmd, sizeof(pauseRunCmd),DEBUS_PAUSERUNPLUGIN_CMD, pauseRunTime);
		gettimeofday(&curTime, NULL);
		cur_time = curTime.tv_sec +curTime.tv_usec/1000000;
		snprintf(cur_time_str, sizeof(cur_time_str), "%ld", cur_time);
		cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStartSec", cur_time_str);
		cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStaus", "1");  /*In pausing */
		
	}
	else if(!strcmp(pauseRunFlag , "1")) 
	{
		/*Start all plugin */
		snprintf(pauseRunCmd, sizeof(pauseRunCmd),DEBUS_PAUSERUNPLUGIN_CMD, "0");
		cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStaus", "0");   /*Not In pausing */
		cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStartSec", "0");
	}

	system_escape(pauseRunCmd); 
	
	return 0;
} 

static int cfg_type_plugin_common_read(char* path,char* attr, char* val,int len) 
{
	DIR *dir;
	FILE *fp = NULL;
	struct dirent *dirptr;
	unsigned int num = 0; 
	int ret = 0;
	char buffer[256] = {0}, buffer1[256] = {0};
	char nodePath[64] = {0};

	dir = opendir(PLUGIN_C_PATHS);
	if (NULL == dir){
		printf("\n [%s],line = %d,open C dir err ==================>\n",__FUNCTION__,__LINE__);
		return 0;
	}
	while ((dirptr = readdir(dir)) != NULL)
	{
		if ((strcmp(dirptr->d_name, ".") == 0) || (strcmp(dirptr->d_name, "..") == 0))
		{
			continue;
		}
			
		if (DT_DIR != dirptr->d_type) 
		{
			continue;
		}
			
		memset(nodePath, 0, sizeof(nodePath));
		snprintf(nodePath,sizeof(nodePath), PLUGIN_ENTRY_NODE, ++num);
		cfg_set_object_attr(nodePath,"Name", dirptr->d_name );
		snprintf(buffer, sizeof(buffer), PLUGIN_C_PIDS, dirptr->d_name);	
		ret = access( buffer, F_OK );
		cfg_set_object_attr(nodePath,"Run",  ret ? "0" : "1" );
	}
	closedir(dir);

	dir = opendir(PLUGIN_B_PATHS);
	if (NULL == dir){
		printf("\n [%s],line = %d,open B dir err ==================>\n",__FUNCTION__,__LINE__);	
		return 0;
	}
	while ((dirptr = readdir(dir)) != NULL)
	{
		if ((strcmp(dirptr->d_name, ".") == 0) || (strcmp(dirptr->d_name, "..") == 0))
		{
			continue;
		}
			
		if (DT_DIR != dirptr->d_type) 
		{
			continue;
		}
			
		memset(nodePath, 0, sizeof(nodePath));
		snprintf(nodePath,sizeof(nodePath), PLUGIN_ENTRY_NODE, ++num);
		cfg_set_object_attr(nodePath,"Name", dirptr->d_name );
		snprintf(buffer1, sizeof(buffer1), PLUGIN_B_STATES, dirptr->d_name);
		fp = fopen(buffer1, "r");
		if(fp != NULL){
			if(fgets(buffer1, sizeof(buffer1), fp) != NULL){
				ret = atoi(buffer1);
			}
			fclose(fp);
		}
		cfg_set_object_attr(nodePath,"Run",  ret ? "0" : "1" );
	}
	closedir(dir);
	
	snprintf(buffer, sizeof(buffer), "%d", num);
	memset(nodePath, 0, sizeof(nodePath));
	snprintf(nodePath, sizeof(nodePath), PLUGIN_COMMON_NODE);
	cfg_set_object_attr(nodePath,"Num", buffer);
	
	return 0;	
}


static char* cfg_type_plugin_index[] = { 
	 NULL 
}; 

int cfg_type_clientInfo_read(void)
{
	char cmd[400] = {0}, envbuf[128] = {0}, dbuscmd[256] = {0};
	FILE *fp = NULL, *cfp = NULL;
	char *token = NULL, *safep = NULL, *safep1 = NULL, *buf = NULL, *token1 = NULL;
	size_t len = 0;
	char infoval[256] = {0}, tmpaddr[256] = {0}, tmpstat[256] = {0};
	int i = 0, j = 0;

	snprintf(envbuf, sizeof(envbuf), "export LD_LIBRARY_PATH=/usr/lib/glib-2.0/");
	snprintf(dbuscmd, sizeof(dbuscmd), "/usr/bin/dbus-send --system --type=method_call --print-reply --dest=com.ctc.cloudclient1 /com/ctc/cloudclient1 com.ctc.cloudclient1.GetPlatformServers > %s", CLIENTINFO_TEMP);	
	snprintf(cmd, sizeof(cmd), "%s && %s", envbuf, dbuscmd);
	system(cmd);

	fp = fopen(CLIENTINFO_TEMP, "r");
	if ( !fp )
		return -1;

	cfp = fopen(CLIENTINFO_PATH, "w");
	if ( !cfp )
	{
		fclose(fp);
		unlink(CLIENTINFO_TEMP);
		return -1;
	}

	while ( getline(&buf, &len, fp) != -1 )
	{
		if ( 1 == i )
			break;
		i++;
	}
	if ( i != 1 )
	{
		fclose(fp);
		fclose(cfp);
		if ( buf )
			free(buf);
		unlink(CLIENTINFO_TEMP);
		return -1;
	}

	if(buf == NULL){
		fclose(fp);
		fclose(cfp);
		unlink(CLIENTINFO_TEMP);
		return -1;
	}

	i = 0;
	j = 0;
	token = strtok_r(buf, "}", &safep);
	while ( token )
	{
		memset(tmpaddr, 0, sizeof(tmpaddr));
		memset(tmpstat, 0, sizeof(tmpstat));
		sscanf(token, "%*[^:]:%[^}]})", infoval);
		token1 = strtok_r(infoval, ",", &safep1);
		while ( token1 )
		{
			if( 0 == i )
			{
				token1 = token1 + strlen("\"Server\":\"");
				sscanf(token1, "\"%[^\"]\"", tmpaddr);
				if ( 1 == j )
					cfg_set_object_attr(PLUGIN_COMMON_NODE, "CapabilityAddr", tmpaddr);
				tmpaddr[strlen(tmpaddr)] = '\n';
				fwrite(tmpaddr, sizeof(char), strlen(tmpaddr), cfp);
			}
			else if( 1 == i )
			{
				token1 = token1 + strlen("\"Status\":\"");
				sscanf(token1, "%[^\"]\"", tmpstat);
				tmpstat[strlen(tmpstat)] = '\n';
				fwrite(tmpstat, sizeof(char), strlen(tmpstat), cfp);
			}

			i++;
			token1 = strtok_r(NULL, ",", &safep1);
		}
		i = 0;
		j++;
		token = strtok_r(NULL, ">", &safep);
	}
	
	free(buf);
	fclose(fp);
	fclose(cfp);
	unlink(CLIENTINFO_TEMP);
	
	return 0;
}

#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_SYSLOG_ENHANCE)
int check_plugin_name(void)
{
	char cmd[400] = {0}, envbuf[128] = {0}, dbuscmd[256] = {0};
	FILE *fp = NULL;
	char *token = NULL, *safep = NULL, *buf = NULL;
	size_t len = 0;
	char sname[256] = {0}, infoval[256] = {0}, totalVal[1024] = {0};
	char tmpname[128] = {0};
	int i = 0;
	int j = 0;
	int flag = 0;
	int needUpdate = 0;
	char log[128] = {0};
	char curAppArray[MAX_APP_NUM][64] = {0};
	int curAppNum = 0;
	
	snprintf(envbuf, sizeof(envbuf), "export LD_LIBRARY_PATH=/usr/lib/glib-2.0/");
	snprintf(dbuscmd, sizeof(dbuscmd), "/usr/bin/gdbus call -y -d com.ctc.appframework1 -o /com/ctc/appframework1 -m com.ctc.appframework1.AppAgent.List > %s", PLUGINNAME_TEMP);
	snprintf(cmd, sizeof(cmd), "%s && %s", envbuf, dbuscmd);
	system(cmd);

	fp = fopen(PLUGINNAME_TEMP, "r");
	if ( !fp ){
		return -1;
	}
	
	if( getline(&buf, &len, fp) == -1){
		fclose(fp);
		unlink(PLUGINNAME_TEMP);
		return -1;
	}

	curAppNum = 0;
	memset(curAppArray, 0, sizeof(curAppArray));
	
	strncpy(totalVal, buf, sizeof(totalVal) - 1);
	totalVal[0] = 's';
	token = strtok_r(totalVal, ">", &safep);
	while ( token )
	{
		memset(tmpname, 0, sizeof(tmpname));
		sscanf(token, "%[^(](%[^)])", sname, infoval);
		
		if ( '\0' != sname[0] ){
			sscanf(sname, "%*[^']'%[^'])", tmpname);
			if('\0' != tmpname[0])
			{
				strncpy(curAppArray[curAppNum], tmpname, sizeof(curAppArray[curAppNum]) -1 );
				curAppNum++;
			}
		}
		token = strtok_r(NULL, ">", &safep);
	}
	
	free(buf);
	fclose(fp);
	unlink(PLUGINNAME_TEMP);

	/*when reboot, old plugin app array is empty, we need copy current plugin app to it at first*/
	if(appArray[0][0] == '\0'){
		memset(appArray, 0, sizeof(appArray));
		memcpy(appArray, curAppArray, sizeof(curAppArray));
		appNum = curAppNum;		
		return 0;
	}

	if(0 == memcmp(appArray,curAppArray,sizeof(appArray))){
		return 0;
	}

	/*search current plugin app in old app array,if no exist,wo know this plugin app is installed,and write to syslog*/
	for(i = 0;i < curAppNum; i++ ){
		flag = 0;
		for(j = 0;j < appNum; j++ ){
			if(0 == strcmp(curAppArray[i], appArray[j])){
				flag = 1;	
				break;
			}
		}

		if(0 == flag){
			needUpdate = 1;
			snprintf(log, sizeof(log), "DBUS Install plugin <%s>.\n", curAppArray[i]);
			openlog("TCSysLog DBUS", 0, LOG_LOCAL2);
			syslog(LOG_INFO, log);
			closelog();
		}
	}
	
	/*search old plugin app in current app array,if no exist,wo know this plugin app is uninstalled,and write to syslog*/
	for(i = 0;i < appNum; i++ ){
		flag = 0;
		for(j = 0;j < curAppNum; j++ ){
			if(0 == strcmp(appArray[i], curAppArray[j])){
				flag = 1;	
				break;
			}
		}

		if(0 == flag){
			needUpdate = 1;
			snprintf(log, sizeof(log), "DBUS Uninstall plugin <%s>.\n", appArray[i]);
			openlog("TCSysLog DBUS", 0, LOG_LOCAL2);
			syslog(LOG_INFO, log);
			closelog();
		}
	}

	/* update currrent plugin app array to old plugin app array*/
	if(needUpdate)
	{
		memset(appArray, 0, sizeof(appArray));
		memcpy(appArray, curAppArray, sizeof(curAppArray));
		appNum = curAppNum;
	}

	return 0;
}
#endif

int cfg_type_pluginInfo_read(char *attr)
{
	char cmd[400] = {0}, envbuf[128] = {0}, dbuscmd[256] = {0};
	FILE *fp = NULL, *nfp = NULL;
	char *token = NULL, *safep = NULL, *safep1 = NULL, *buf = NULL, *token1 = NULL;
	size_t len = 0;
	char sname[256] = {0}, infoval[256] = {0}, totalVal[1024] = {0};
	char tmpVal[1024] = {0};
	char tmpname[256] = {0}, tmpver[30] = {0}, tmpstat[4] = {0}, tmptype[10] = {0};
	int i = 0;

	struct timeval curTime;
	long cur_time = 0;
	char pauseStartSec[32] = {0};
	char pauseRunTime[32] = {0};
	long  pauseSec = {0};

	memset(&curTime, 0, sizeof(curTime));
	memset(pauseStartSec, 0, sizeof(pauseStartSec));
	memset(pauseRunTime, 0, sizeof(pauseRunTime));
	
	if(!strcmp(attr, "PauseStaus"))
	{
		gettimeofday(&curTime, NULL);
		cur_time = curTime.tv_sec +curTime.tv_usec/1000000;
		cfg_get_object_attr(PLUGIN_COMMON_NODE, "PauseStartSec", pauseStartSec, sizeof(pauseStartSec));
		cfg_get_object_attr(PLUGIN_COMMON_NODE, "pauseRunTime", pauseRunTime, sizeof(pauseRunTime));
		pauseSec = cur_time - atoi(pauseStartSec);
		if(pauseSec >= atoi(pauseRunTime) * 60)
		{
			cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStaus", "0");   /*Not in pausing */
			cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStartSec", "0");
		}
		else
		{
			cfg_set_object_attr(PLUGIN_COMMON_NODE,"PauseStaus", "1");  /*In pausing */
		}

		return 0;
	}
	
	if ( NULL != strstr(attr, "PluginName") )
	{
		snprintf(envbuf, sizeof(envbuf), "export LD_LIBRARY_PATH=/usr/lib/glib-2.0/");
		snprintf(dbuscmd, sizeof(dbuscmd), "/usr/bin/gdbus call -y -d com.ctc.appframework1 -o /com/ctc/appframework1 -m com.ctc.appframework1.AppAgent.List > %s", PLUGININFO_TEMP);
		snprintf(cmd, sizeof(cmd), "%s && %s", envbuf, dbuscmd);
		system(cmd);

		fp = fopen(PLUGININFO_TEMP, "r");
		if ( !fp )
			return -1;
		
		nfp = fopen(PLUGININFO_PATH, "w");
		if ( !nfp )
		{
			fclose(fp);
			unlink(PLUGININFO_TEMP);
			return -1;
		}
		
		if( getline(&buf, &len, fp) == -1)
		{
			fclose(fp);
			fclose(nfp);
			unlink(PLUGININFO_TEMP);
			return -1;
		}
		
		strncpy(totalVal, buf, sizeof(totalVal) - 1);
		totalVal[0] = 's';
		token = strtok_r(totalVal, ">", &safep);
		
		while ( token )
		{
			memset(tmpVal, 0, sizeof(tmpVal));
			memset(tmpname, 0, sizeof(tmpname));
			memset(tmpver, 0, sizeof(tmpver));
			memset(tmpstat, 0, sizeof(tmpstat));
			sscanf(token, "%[^(](%[^)])", sname, infoval);
			if ( '\0' != sname[0] )
				sscanf(sname, "%*[^']'%[^'])", tmpname);
			token1 = strtok_r(infoval, ",", &safep1);
			while ( token1 )
			{
				if( 0 == i )
				{
					/*sscanf(token1, "'%[^']'", tmpname);*/
					strncat(tmpVal, tmpname, sizeof(tmpVal) - strlen(tmpVal) - 1);
					strncat(tmpVal, ",", sizeof(tmpVal) - strlen(tmpVal) - 1);
				}
				else if( 1 == i )
				{
					sscanf(token1, "%*[^']'%[^']'", tmpver);
					strncat(tmpVal, tmpver, sizeof(tmpVal) - strlen(tmpVal) - 1);
					strncat(tmpVal, ",", sizeof(tmpVal) - strlen(tmpVal) - 1);
				}
				else if( 2 == i )
				{
					sscanf(token1, "%s %s", tmptype, tmpstat);
					strncat(tmpVal, tmpstat, sizeof(tmpVal) - strlen(tmpVal) - 1);
					strncat(tmpVal, "\n", sizeof(tmpVal) - strlen(tmpVal) - 1);
				}

				i++;
				token1 = strtok_r(NULL, ",", &safep1);
			}
			i = 0;
			if ( '\0' != tmpVal[0])
				fwrite(tmpVal, sizeof(char), strlen(tmpVal), nfp);
			token = strtok_r(NULL, ">", &safep);
		}
		
		free(buf);
		fclose(fp);
		fclose(nfp);
		unlink(PLUGININFO_TEMP);
	}

	cfg_type_clientInfo_read();
	
	cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "PluginState", "Done");
	
	return 0;
}

static int cfg_type_plugin_entry_func_get(char* path,char* attr, char* val,int len) 
{ 
	 return cfg_type_default_func_get(path,attr,val,len); 
} 


static cfg_node_ops_t cfg_type_plugin_entry_ops  = { 
	 .get = cfg_type_plugin_entry_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t cfg_type_plugin_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | CFG_TYPE_FLAG_MULTIPLE | 100, 
	 .parent = &cfg_type_plugin, 
	 .index = cfg_type_plugin_index, 
	 .ops = &cfg_type_plugin_entry_ops, 
}; 


static int cfg_type_plugin_common_func_get(char* path,char* attr, char* val,int len) 
{ 
#if defined(TCSUPPORT_CT_JOYME2)
	cfg_type_pluginInfo_read(attr);
#else
	cfg_type_plugin_common_read(path, attr, val, len);
#endif
	return cfg_type_default_func_get(path,attr,val,len);
} 


static cfg_node_ops_t cfg_type_plugin_common_ops  = { 
	 .get = cfg_type_plugin_common_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_plugin_common_func_commit 
}; 


static cfg_node_type_t cfg_type_plugin_common = { 
	 .name = "Common", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1, 
	 .parent = &cfg_type_plugin, 
	 .ops = &cfg_type_plugin_common_ops, 
}; 


static cfg_node_ops_t cfg_type_plugin_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_default_func_commit 
}; 


static cfg_node_type_t* cfg_type_plugin_child[] = { 
	 &cfg_type_plugin_entry, 
	 &cfg_type_plugin_common, 
	 NULL 
}; 


cfg_node_type_t cfg_type_plugin = { 
	 .name = "Plugin", 
	 .flag = CFG_TYPE_FLAG_MEMORY | CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_plugin_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_plugin_child, 
	 .ops = &cfg_type_plugin_ops, 
}; 
