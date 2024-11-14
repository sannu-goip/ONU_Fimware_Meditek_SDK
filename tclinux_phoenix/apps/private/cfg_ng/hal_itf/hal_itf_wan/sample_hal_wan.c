#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "hal_itf_wan.h"
typedef struct 
{
	const char * subCmd;
	const char * desp;
	int(*cbFunc)(int argc, char ** argv);
} subCmd_t;
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

static int creat_link(int argc, char*argv[])
{
	int ret = 0;
	char path[32];
	memset(path, 0, sizeof(path));	
	ret = creat_wan_link(path, sizeof(path));
	
	printf("path = %s, ret = %d, line = %d\n", path, ret, __LINE__);
	
	return 0;
}

static int del_link(int argc, char*argv[])
{
	int ret = 0;
	char path[32];
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s", argv[0]);

	ret = del_wan_link(path);
	printf("path = %s, ret = %d, line = %d\n", path, ret, __LINE__);

	return 0;
}

static int effect_link(int argc, char*argv[])
{
	int ret = 0;
	char path[32];
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s", argv[0]);

	ret = wan_link_effect(path);
	
	printf("path = %s, ret = %d, line = %d\n", path, ret, __LINE__);
	return 0;
}

static int set_wan_attr(int argc, char*argv[])
{	
	int ret = 0;
	char path[32];
	int type = 0;
	char value[64];
	char care = 0;
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s", argv[0]);
    type = atoi(argv[1]);
	memset(value, 0, sizeof(value));
	snprintf(value, sizeof(value), "%s", argv[2]);
    
    ret = set_wan_link_attr(path, sizeof(path), type, value, &care);
	printf("path = %s, type = %d, value = %s, care = %d, ret = %d, line = %d.\n", path, type, value,care, ret, __LINE__);

	return 0;
	
}

static int get_wan_attr(int argc, char*argv[])
{
	int ret = 0;
	char path[32];
    int type = 0;
    char value[64];
	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s", argv[0]);

    type = atoi(argv[1]);
    
	memset(value, 0, sizeof(value));
	ret = get_wan_link_attr(path, type, value, sizeof(value));
	printf("path = %s, type = %d, value = %s, ret = %d, line = %d.\n", path, type, value, ret, __LINE__);

	return 0;
}

static int list_wan_link_path(int argc, char*argv[])
{
    int ret = 0;
    int i = 0;
    wan_link_path_info_t info;
    memset(&info, 0, sizeof(info));

    ret = get_wan_link_all_entry_path(&info);
    
    printf("ret = %d, info.num = %d.\n", ret, info.num);
    for(i = 0; i < info.num; i++)
    {
        printf("i=%d, info.path[%d]=%s.\n", i, i, info.path[i]);
    }
    
   return 0;
}

static subCmd_t subCmdTbl_L1[] = 
{
	{"creat_link", 		"creat wan default link", 	creat_link},
	{"del_link",   		"del wan link", 			del_link},
	{"effect_link",   	"effect wan link", 			effect_link},
	{"set_wan_attr",   	"set wan link attr", 		set_wan_attr},
	{"get_wan_attr",   	"get wan link attr", 		get_wan_attr},
    {"list_wan_path",   "list wan link path",       list_wan_link_path},
    
};

int main(int argc, char* argv[])
{
	int idx = 0;

	if(argc < 2)
	{
		goto print_help;
	}
	
	for(idx = 0; idx < ARRAY_SIZE(subCmdTbl_L1); ++idx)
	{
		if(0 == strcmp(subCmdTbl_L1[idx].subCmd, argv[1]))
		{
			return subCmdTbl_L1[idx].cbFunc(argc - 2, argv + 2);
		}
	}

print_help:
	printf("Valid Subcmd for mtk_itf_cmd is:\n");

	for(idx = 0; idx < ARRAY_SIZE(subCmdTbl_L1); ++idx)
	{
		printf("\t%s: %s\n", subCmdTbl_L1[idx].subCmd, subCmdTbl_L1[idx].desp);
	}
	return 0;	
}

