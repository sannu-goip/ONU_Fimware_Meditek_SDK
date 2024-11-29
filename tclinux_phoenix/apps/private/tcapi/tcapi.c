/******************************************************************************/
/*
 * Copyright (C) 1994-2008 TrendChip Technologies, Corp.
 * All Rights Reserved.
 *
 * TrendChip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of TrendChip Technologies, Corp. and
 * shall not be reproduced, copied, disclosed, or used in whole or
 * in part for any reason without the prior express written permission of
 * TrendChip Technologies, Corp.
 */
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
/*Need TC API interface header*/
#include "libtcapi.h"
#include "tcversion.h"
#include "libcompileoption.h"

#define MAXLEN_TCAPI_MSG		1024

#define DOSET 0x01
#define DOGET 0x02
#define DOSETGET 0x03
#define DOSHOW 0x04
#define NODE_HAS_INDEX 3

typedef struct attrList_s {
	char	*name;
	char	*valFormat;
	int	flag;
	char * description;
} attrList_t;

typedef struct node_Name_s {
	char	*name;
	int	flag;
	attrList_t* attrList;
	char * description;
} node_Name_t;

attrList_t Wan_PVC_attr[]={
	{"VPI", "xx" ,DOSET,"SET PVC VPI"},
	{"VCI",	"xx", DOSET,"SET PVC VCI"},
	{"ISP",	"xx", DOSET,"0--Static IP, 1--Dynamic IP, 2--PPPoE, 3--Bridge"},
	{"USERNAME", "xxx", DOSET,"SET PPPoE USERNAME"},
	{"PASSWORD", "xxx", DOSET,"SET PPPoE PASSWORD"},
	{NULL,NULL,0,NULL}
};

attrList_t Lan_Entry_attr[]={
	{"IP", "xxx.xxx.xxx.xxx" ,DOSET,"SET LAN IP"},
	{NULL,NULL,0,NULL}
};

attrList_t WLan_Entry_attr[]={
	{"HideSSID", "x" ,DOSET,"0/1"},
	{"AuthMode", "xxx", DOSET,"OPEN,WEP-64Bits,WEP-128Bits,WPAPSK,WPA2PSK,WPAPSKWPA2PSK"},
	{"EncrypType", "xxx", DOSET,"NONE,TKIP,AES,TKIPAES"},
	{NULL,NULL,0,NULL}
};

attrList_t Account_Entry_attr[]={
	{"username", "xxx" ,DOSET,"Account username"},
	{"web_passwd", "xxx", DOSET,"Web Account password"},
	{"console_passwd", "xxx", DOSET,"Console Account password"},
	{NULL,NULL,0,NULL}
};

attrList_t WLan_Common_attr[]={
	{"APOn", "x" ,DOSET,"1--On, 0--Off"},
	{"rt_device", NULL, DOGET,"Wifi Device "},
	{NULL,NULL,0,NULL}
};

attrList_t Dhcpd_Common_attr[]={
	{"start", "xxx.xxx.xxx.xxx" ,DOSET,"DHCP Start IP"},
	{"pool_count",	"xx", DOSET,"DHCP Pool Count"},
	{NULL,NULL,0,NULL}
};

attrList_t Dproxy_Entry_attr[]={
	{"Primary_DNS", NULL ,DOGET,"DNS Primary"},
	{"Secondary_DNS",NULL, DOGET,"DNS Secondary"},
	{NULL,NULL,0,NULL}
};

attrList_t Cwmp_Entry_attr[]={
	{"acsUrl", "xxx" ,DOSET,"acs url"},
	{"acsUserName",	"xxx", DOSET,"acs username"},
	{"acsPassword", "xxx" ,DOSET,"acs password"},
	{"ProductClass", "xxx", DOSET,"ProductClass"},
	{NULL,NULL,0,NULL}
};

attrList_t ACL_Common_attr[]={
	{"Activate", "xxx" ,DOSET,"Yes/No"},
	{NULL,NULL,0,NULL}
};

attrList_t Adsl_Entry_attr[]={
	{"ANNEXTYPEA", NULL ,DOGET,"ANNEX TYPE"},
	{NULL,NULL,0,NULL}
};

attrList_t Info_Adsl_attr[]={
	{"lineState", NULL ,DOGET,"ADSL state"},
	{"fwVer", NULL ,DOGET,"DMT version"},
	{NULL,NULL,0,NULL}
};

attrList_t SysInfo_Entry_attr[]={
	{"SWVer", NULL ,DOGET,"Software Version"},
	{NULL,NULL,0,NULL}
};

node_Name_t nodeName[]={
/*These are has index ,NODE_HAS_INDEX=3*/
	{"Wan_PVCx", DOSET ,Wan_PVC_attr,"WAN PVC node, x: pvc index ,eg: Wan_PVC0"},
	{"WLan_Entryx", DOSET ,WLan_Entry_attr,"WLAN SSIDx node,x: index ,eg: WLan_Entry0"},
	{"DeviceInfo_PVCx", DOSHOW ,NULL,"DEVICEINFO PVC node,x: index ,eg: DeviceInfo_PVC0"},

/*These are not has index*/
	{"Account_Entry0", DOSET ,Account_Entry_attr,"ACCOUNT node"},
	{"Lan_Entry0", DOSET ,Lan_Entry_attr,"LAN ENTRY node"},
	{"WLan_Common", DOSETGET ,WLan_Common_attr,"WLAN COMMON node"},
	{"Dhcpd_Common", DOSET ,Dhcpd_Common_attr,"DHCP COMMON node"},
	{"Dproxy_Entry", DOGET ,Dproxy_Entry_attr,"DPROXY node"},
	{"Cwmp_Entry", DOSET ,Cwmp_Entry_attr,"CWMP node"},
	{"ACL_Common", DOSET ,ACL_Common_attr,"ACL COMMON node"},
	{"Adsl_Entry", DOGET ,Adsl_Entry_attr,"ADSL ENTRY node"},
	{"Info_Adsl", DOGET ,Info_Adsl_attr,"ADSL INFO node"},
	{"SysInfo_Entry", DOGET ,SysInfo_Entry_attr,"SYSINFO node"},
	{NULL, 0 ,NULL,NULL}
};

void doSet(char* argv[]);
void doUnset(char* argv[]);
void doGet(char* argv[]);
void doShow(char* argv[]);
void doCommit(char* argv[]);
void doSave(char* argv[]);
void doRead(char* argv[]);
void doReadAll(char* argv[]);
void doStaticGet(char* argv[]);

typedef	struct {
	char	*name;		/* Name of command			*/
	void	(*func)(char *argv[]);/* Function to execute command*/
	int	  argcnum;	/* number of args		*/
	char	*argc_errmsg;	/* Message to print if insufficient args */
} cmds_t;

static const cmds_t tcapi_root_command[] = {
	{"set",   	doSet,	   3,	  "set <node_name attr value>"},
	{"unset",   doUnset,   1,	  "unset <node_name>"},
	{"get",   	doGet,	   2,	  "get <node_name attr>"},
	{"show",   	doShow,	   1,	  "show <node_name>"},
	{"commit",  doCommit,  1,	  "commit <node_name>"},
	{"save",   	doSave,	   0,	  "save"},
	{"read",    doRead,    1,     "read <node_name>"},
	{"readAll", doReadAll, 0,	  "readAll"},
        {"staticGet",         doStaticGet,     2,     "get <node_name attr>"},
	{NULL, NULL, 0, NULL}
};


/*____________________________________________________________________*
**	function name: doSet
**
**	description:
*     do tcapi_set
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/
void
doSet(char* argv[]){
  int ret;

  ret = tcapi_set(argv[0],argv[1],argv[2]);
  switch(ret){
    case 0:
	break;
    case -1:
      printf("no specific node\n");
      break;
    case -2:
      printf("no specific attribute\n");
      break;
    case -3:
      printf("connection error\n");
      break;
    case -4:
	  printf("verify setting fail while tcapi set\n");
	  break;
    default:
      printf("unknown error\n");
      break;
  }
}
/*____________________________________________________________________*
**	function name: doUnSet
**
**	description:
*     do tcapi_unset
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/
void
doUnset(char* argv[]){
  int ret;

  ret = tcapi_unset(argv[0]);
  switch(ret){
    case 0:
	break;
    case -1:
      printf("no specific node \n");
      break;
    case -3:
      printf("connection error\n");
      break;
    default:
      printf("unknown error\n");
      break;
  }
}
/*____________________________________________________________________*
**	function name: doGet
**
**	description:
*     do tcapi_get
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/
void
doGet(char* argv[]){
  int ret;
//char buf[128]; 
//buff[128] is too small to  enough "tcapi get" data, FON function need more buff 
  char buf[MAXLEN_TCAPI_MSG];

  ret = tcapi_get(argv[0],argv[1],buf);
  switch(ret){
    case 0:
      printf("%s\n",buf);
      break;
    case -1:
      printf("no node information\n");
      break;
    case -2:
      printf("no attribute information\n");
      break;
    case -3:
      printf("connection error\n");
      break;
    default:
      printf("unknown error\n");
      break;
  }
}

/*____________________________________________________________________*
**      function name: doStaticGet
**
**      description:
*     do tcapi_get
*
**      parameters:
*     argv[]:the arguments that user input
**      global:
*     none
**      return:
*     none
**      call:
*     none
**      revision:
*     1.shnwind
* _______________________________________________________________*/
void
doStaticGet(char* argv[]){
  int ret;
  char buf[128];

  ret = tcapi_staticGet(argv[0],argv[1],buf);
  switch(ret){
    case 0:
      printf("%s\n",buf);
      break;
    case -1:
      printf("no node information\n");
      break;
    case -2:
      printf("no attribute information\n");
      break;
    case -3:
      printf("connection error\n");
      break;
    default:
      printf("unknown error\n");
      break;
  }
}

/*____________________________________________________________________*
**	function name: doShow
**
**	description:
*     do tcapi_show
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/
void
doShow(char* argv[]){
  int ret;
  char buf[MAXLEN_TCAPI_MSG];

  ret = tcapi_show(argv[0], buf);
  switch(ret){
     case 1:
  	system("cat /tmp/show_info");
  	unlink("/tmp/show_info");
  	break;
      case 0:
        printf("%s\n", buf);
        break;
      case -1:
        printf("no node information\n");
        break;
      case -3:
        printf("connection error\n");
        break;
      default:
	  printf("unknown error\n");
	  break;
    }

 }

/*____________________________________________________________________*
**	function name: doCommit
**
**	description:
*     do tcapi_commit
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/
void
doCommit(char* argv[]){
  int ret;

  ret = tcapi_commit(argv[0]);
  switch(ret){
    case 0:
	break;
    case -1:
      printf("no specific node\n");
      break;
    case -3:
      printf("connection error\n");
      break;
    default:
      printf("unknown error\n");
      break;
  }
}
/*____________________________________________________________________*
**	function name: doSave
**
**	description:
*     do tcapi_save
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/
void
doSave(char* argv[]){
  tcapi_save();
}
/*____________________________________________________________________*
**	function name: doRead
**
**	description:
*     do tcapi_read
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.xyzhu
* _______________________________________________________________*/
void
doRead(char* argv[]){
  //tcapi_read();
  int ret;

  ret = tcapi_read(argv[0]);
  switch(ret){
    case 0:
	break;
    case -1:
      printf("no specific node\n");
      break;
    case -3:
      printf("connection error\n");
      break;
    default:
      printf("unknown error\n");
      break;
  }
}

/*____________________________________________________________________*
**	function name: doReadAll
**
**	description:
*     do tcapi_readAll
*
**	parameters:
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.xyzhu
* _______________________________________________________________*/
void
doReadAll(char* argv[]){
  //tcapi_readAll();
  tcapi_readAll();
} 

static void showNode(int flag){
	int i=0;
	int printHead=1;

	if(TCSUPPORT_CMDPROMPT_VAL){	
		for(i=0;nodeName[i].name!=NULL;i++){
			if(nodeName[i].flag&flag){
				if(printHead){
					printf("\nCommon node_name: \n");
					printHead=0;
				}
				printf(" %-15s : %s\n",nodeName[i].name ,nodeName[i].description);			
			}		
		}
	}
}

static void showAttr(char *ndName,int flag){
	int i=0;
	int printHead=1;
	attrList_t *curAttrList=NULL;

	if(TCSUPPORT_CMDPROMPT_VAL){
		//find node name
		for(i=0;nodeName[i].name!=NULL;i++){
			if(i<NODE_HAS_INDEX){
				if(strncmp(ndName,nodeName[i].name,strlen(nodeName[i].name)-1)==0){
					break;
				}			
			}else{
				if(strcmp(ndName,nodeName[i].name)==0){
					break;
				}			
			}		
		}
		if(nodeName[i].name!=NULL){
			curAttrList=nodeName[i].attrList;
			//disp attr
			if(curAttrList){
				for(i=0;curAttrList[i].name!=NULL;i++){
					if(curAttrList[i].flag&flag){
						if(flag&DOSET){
							if(printHead){
								printf("\nCommon attr and value: \n");
								printHead=0;
							}
							printf(" %-12s %-9s : %s\n",curAttrList[i].name ,curAttrList[i].valFormat,curAttrList[i].description);	
						}else if(flag&DOGET) {
							if(printHead){
								printf("\nCommon attr : \n");
								printHead=0;
							}
							printf(" %-15s : %s\n",curAttrList[i].name ,curAttrList[i].description);	
						}		
					}		
				}
			}
		}
	}
}
static void  cmdPrompt(int argc, char* argv[]){
	if(TCSUPPORT_CMDPROMPT_VAL){
		if(strcmp(argv[0],"set")==0){
			switch(argc){
				case 0:
					showNode(DOSET);
					break;
				case 1:
					showAttr(argv[1],DOSET);
					break;
			}
		}else if(strcmp(argv[0],"get")==0){
			switch(argc){
				case 0:
					showNode(DOGET);
					break;
				case 1:
					showAttr(argv[1],DOGET);
					break;
			}
		}else if(strcmp(argv[0],"show")==0){
			if (argc==0){
					showNode(DOSHOW);
			}
		}
	}
}


/*____________________________________________________________________*
**	function name: main
**
**	description:
*     tcapi main function
*
**	parameters:
*     acgc:  the argument number that user input
*     argv[]:the arguments that user input
**	global:
*     none
**	return:
*     none
**	call:
*     none
**	revision:
*     1.shnwind
* _______________________________________________________________*/

int
main (int argc, char* argv[]){

	const cmds_t	*cmdp = tcapi_root_command;

	init_compileoption_val();
	
	if(argc == 2){
		if(!strcmp(argv[1],"version")){
			printf("\r\n tcapi version: %s\n",MODULE_VERSION_TCAPI);
			return 0;
		}
	}
	
	if(argc > 1){
	  for(cmdp = tcapi_root_command; cmdp->name != NULL;cmdp++ ){
      if(!strcmp(cmdp->name,argv[1])){
        argc -= 2;
        argv += 2;
        if(argc == cmdp->argcnum){
          	(*cmdp->func)(argv);
        }else{
          	printf("%s\n",cmdp->argc_errmsg);
			if(TCSUPPORT_CMDPROMPT_VAL){
			  argv-=1;
			  cmdPrompt(argc,argv);
			}
        }
        break;
      }
    }
  }else{
    /*show all command name*/
    for(cmdp = tcapi_root_command; cmdp->name != NULL;cmdp++ ){
      printf("%s\n",cmdp->name);
    }
  }
  return 0;
}
