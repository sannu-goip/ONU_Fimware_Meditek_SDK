
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
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "cfg_cli.h"

typedef	struct {
	char *name;		
	int	(*func)(int argc,char *argv[],void *p);
	int	flags;	
	int	argcmin;
	char *argc_errmsg;	
} cmds_t;


static int subcmd	(const cmds_t tab[], int argc, char *argv[], void *p)
{
	register const cmds_t *cmdp;
	int found = 0;
	int i;

	/* Strip off first token and pass rest of line to subcommand */
	if (argc < 2) {
		if (argc < 1)
			printf("SUBCMD - Don't know what to do?\n");
		else
			goto print_out_cmds;
			//printf("\"%s\" - takes at least one argument\n",argv[0]);
		return -1;
	}
	argc--;
	argv++;
	for(cmdp = tab;cmdp->name != NULL;cmdp++){
		if(strncmp(argv[0],cmdp->name,strlen(argv[0])) == 0){
			found = 1;
			break;
		}
	}
	if(!found){
		char buf[66];

print_out_cmds:
		printf("valid subcommands:\n");
		memset(buf,' ',sizeof(buf));
		buf[64] = '\n';
		buf[65] = '\0';
		for(i=0,cmdp = tab;cmdp->name != NULL;cmdp++){
			strncpy(&buf[i*16], cmdp->name, 15);
			if(i == 3){
				printf(buf);
				memset(buf,' ',sizeof(buf));
				buf[64] = '\n';
				buf[65] = '\0';
			}
			i = (i+1)%4;
		}
		if(i != 0)
			printf(buf);
		return -1;
	}
	if(argc <= cmdp->argcmin){
		if(cmdp->argc_errmsg != NULL)
			printf("Usage: %s\n",cmdp->argc_errmsg);
		return -1;
	}
	if(cmdp->func == NULL) 
		return 0;
	return (*cmdp->func)(argc,argv,p);
}


static int do_create(int argc, char *argv[], void *p)
{
	int id;

	if (argc < 2)
	{
		printf("Help: cfg create <obj path>  \n");
		return 0;
	}
	
	if ((id = cfg_create_object(argv[1])) <= 0)
		printf("Create [%s]: error \n",argv[1]);
	else
		printf("Create [%s]: ID = %d:%d \n",argv[1],(id>>16) & 0xffff,id & 0xffff);
	
	return 0;
}

static int do_delete(int argc, char *argv[], void *p)
{
	int id;
	
	if (argc < 2)
	{
		printf("Help: cfg delete <obj path>  \n");
		return 0;
	}
	
	if ((id = cfg_delete_object(argv[1])) <= 0)
		printf("Delete [%s]: error \n",argv[1]);
	else
		printf("Delete [%s]: ok \n",argv[1]);	
	
	return 0;
}

static int do_query(int argc, char *argv[], void *p)
{
	int id[16];
	int cnt, i;
	char* type;

	if (argc < 2)
	{
		printf("Help: cfg query <obj path> [subtype]  \n");
		return 0;
	}
	
	if (argc > 2)
		type = argv[2];
	else
		type = NULL;
	
	if ((cnt = cfg_query_object(argv[1],type,id)) <= 0)
		printf("Query [%s]: error \n",argv[1]);
	else
		printf("Query [%s]: ok \n",argv[1]);

	for(i=0;i<cnt;i++)
		printf("[%d] ", id[i]);

	printf("\n");
	
	return 0;
}



static int do_commit(int argc, char *argv[], void *p)
{
	if (argc < 2)
	{
		printf("Help: cfg commit <obj path>   \n");
		return 0;
	}	
	
	if ((cfg_commit_object(argv[1])) < 0)
		printf("Commit [%s]: error \n",argv[1]);
	else
		printf("Commit [%s]: ok \n",argv[1]);	

	return 0;
}

static int do_show(int argc, char *argv[], void *p)
{
	int  level = 1;
	
	if (argc < 2)
	{
		printf("Help: cfg show <obj path> [level] \n");
		return 0;
	}

	if (argc > 2)
		level = atoi(argv[2]);
		
	cfg_obj_dump(argv[1],level);

	return 0;
}

static int do_setattr(int argc, char *argv[], void *p)
{
	char tmp[CFG_MAX_PATH_NAME];

	if (argc < 4)
	{
		printf("Help: cfg set <obj path> <attr> <value>  \n");
		return 0;
	}

	if (strchr(argv[1],'_') != NULL)
		cfg_parse_node_name(argv[1],tmp,sizeof(tmp));
	else
		strncpy(tmp,argv[1],sizeof(tmp) - 1);

	if (cfg_set_object_attr(tmp,argv[2],argv[3]) < 0)  
		printf("Set [%s] Attr: error \n", tmp);
	else
		printf("Set [%s] Attr: ok \n", tmp);
	

	return 0;
}

static int do_test_tme(void)
{
//	int i;
//	struct timeval start,end;
//	gettimeofday(&start,NULL);
//	while(i++ < 1000)
//		ret = cfg_get_object_attr(argv[1],argv[2],val,128);
//	gettimeofday(&end,NULL);
//	printf("do_getattr: start[%u.%u] end[%u.%u] \n",(unsigned int)start.tv_sec,(unsigned int)start.tv_usec,(unsigned int)end.tv_sec,(unsigned int)end.tv_usec);
	return 0;
}
static int do_getattr(int argc, char *argv[], void *p)
{
	char val[CFG_MAX_ATTR_VALUE];
	char tmp[CFG_MAX_PATH_NAME];

	int ret = 0;
	
	if (argc < 3)
	{
		printf("Help: cfg get <obj path> <attr>  \n");
		return 0;
	}
	
	do_test_tme();

	if (strchr(argv[1],'_') != NULL)
		cfg_parse_node_name(argv[1],tmp,sizeof(tmp));
	else
		strncpy(tmp,argv[1],sizeof(tmp) - 1);
	
	ret = cfg_get_object_attr(tmp,argv[2],val,sizeof(val));

	if (ret < 0)  
		printf("Get [%s] Attr: no exist \n", tmp);
	else		
		printf("Get [%s] Attr: %s = %s \n", tmp,argv[2], val);
	
	return 0;
}

static int do_type(int argc, char *argv[], void *p)
{
	cfg_node_type_t* node,*tmp;

	if (argc < 2)
	{
		printf("Help: cfg type <type path or name>   \n");
		return 0;
	}	
	
	node = cfg_type_query(argv[1]);
	
	if (node == NULL)
	{
		printf("Dump [%s]: not exist \n",argv[1]);
		return 0;
	}

	tmp = cfg_type_get_child(node);

	cfg_type_dump(tmp);

	return 0;
}

#if 0
#define MAX_ROOT_CONN_INST	32
static int do_path(int argc, char *argv[], void *p)
{
	unsigned int id[MAX_ROOT_CONN_INST];
	char objpath[CFG_MAX_PATH_NAME];
	char attr[CFG_MAX_ATTR_VALUE];
	int cnt, i;

	if (argc < 2)
	{
		printf("Help: cfg path <devname>   \n");
		return 0;
	}	

	if ((cnt = cfg_query_object(CFG_TYPE_ROOT_CONN_PATH,CFG_TYPE_ROOT_CONN_PPP_NAME,id)) > 0)
	{
		for(i = 0; i< cnt; i++)
		{
			sprintf(objpath,"%s.%d",CFG_TYPE_ROOT_CONN_PPP_PATH,CFG_OID_TO_ID(id[i]));
			
			if (cfg_get_object_attr(objpath,CFG_TYPE_ROOT_CONN_PPP_ATTR_DEV_NAME,attr,CFG_MAX_ATTR_VALUE) <= 0)
				continue;

			if (strcmp(attr,argv[1]))
				continue;

			printf("%s\n",objpath);
			break;
		}
	}

	if ((cnt = cfg_query_object(CFG_TYPE_ROOT_CONN_PATH,CFG_TYPE_ROOT_CONN_IP_NAME,id)) > 0)
	{
		for(i = 0; i< cnt; i++)
		{
			sprintf(objpath,"%s.%d",CFG_TYPE_ROOT_CONN_IP_PATH,CFG_OID_TO_ID(id[i]));
			
			if (cfg_get_object_attr(objpath,CFG_TYPE_ROOT_CONN_IP_ATTR_DEV_NAME,attr,CFG_MAX_ATTR_VALUE) <= 0)
				continue;

			if (strcmp(attr,argv[1]))
				continue;

			printf("%s\n",objpath);
			break;
		}
	}
	return 0;
}
#endif

static int do_save(int argc, char *argv[], void *p)
{
	if (argc < 2)
	{
		printf("Help: cfg save <file> \n");
		return 0;
	}

	if (cfg_save_object(argv[1]) < 0)
	{
		printf("cfg save object fial \n");
	}
	
	return 0;
}

static int do_parse(int argc, char *argv[], void *p)
{
	char tmp[CFG_MAX_PATH_NAME];
	
	if (argc < 2)
	{
		printf("Help: cfg parse <node> \n");
		return 0;
	}

	if (cfg_parse_node_name(argv[1],tmp,sizeof(tmp)) < 0)
		printf("cfg parse [%s] fail  \n",argv[1]);
	else
		printf("cfg parse [%s] ok  \n",tmp);
	
	return 0;
}

static int do_load(int argc, char *argv[], void *p)
{
	cfg_node_obj_t* node,*obj;
	
	if (argc < 2)
	{
		printf("Help: cfg load <file> \n");
		return 0;
	}

	if ((obj = cfg_obj_load_from_type()) == NULL)
	{
		printf("do_load: create  object from type fail \n");
		return 0;
	}
	
	if ((node = cfg_obj_set_root(obj)) != NULL)
	{
		printf("do_load: root object not null \n");
	}
	
	cfg_load_object(argv[1]);

	shm_settree(obj);
	
	return 0;
}


static cmds_t cfgcmds[] = {
    {"create",     do_create,     0x10, 0, NULL},
    {"delete",   	do_delete,   	0x10, 0, NULL},
    {"query",     do_query,     0x10, 0, NULL},
    {"commit",   	do_commit,   	0x10, 0, NULL},    
    {"show",     do_show,     0x10, 0, NULL},
    {"set",   	do_setattr,   	0x10, 0, NULL},    
    {"get",   	do_getattr,   	0x10, 0, NULL},  
    {"type",   	do_type,   	0x10, 0, NULL}, 
 //   {"path",   	do_path,   	0x10, 0, NULL},  
    {"save",   	do_save,   	0x10, 0, NULL},
    {"load",   	do_load,   	0x10, 0, NULL}, 
    {"parse",   do_parse,   	0x10, 0, NULL}, 
    {NULL,      NULL,       0x10, 0, NULL}
};

int main(int argc, char **argv) 
{
	return subcmd(cfgcmds, argc, argv, NULL);
}


