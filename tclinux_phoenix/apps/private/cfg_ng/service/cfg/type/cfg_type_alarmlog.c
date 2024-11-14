#include <stdio.h> 
#include <string.h> 
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "svchost_evt.h"
#include "utility.h"

#define ALARM_MAX_ENTRY_NUMBER 8

static cfg_node_ops_t cfg_type_alarm_info_common_ops  = { 
	.get 	= cfg_type_default_func_get, 
	.set 	= cfg_type_default_func_set, 
	.create = cfg_type_default_func_create, 
	.delete = cfg_type_default_func_delete, 
	.commit = cfg_type_default_func_commit,
	.query 	= cfg_type_default_func_query 
}; 

static cfg_node_ops_t cfg_type_alarm_info_entry_ops  = { 
	.get 	= cfg_type_default_func_get, 
	.set 	= cfg_type_default_func_set, 
	.create = cfg_type_default_func_create, 
	.delete = cfg_type_default_func_delete, 
	.commit = cfg_type_default_func_commit,
	.query 	= cfg_type_default_func_query 
}; 

static cfg_node_ops_t cfg_type_alarm_info_ops  = { 
	.set 	= cfg_type_default_func_set, 
	.get 	= cfg_type_default_func_get, 
	.query 	= cfg_type_default_func_query, 
	.commit = cfg_type_default_func_commit 
};

static cfg_node_type_t cfg_type_alarm_info_entry = { 
	.name 		= "Entry", 
	.flag 		=  CFG_TYPE_FLAG_MULTIPLE | ALARM_MAX_ENTRY_NUMBER , 
	.parent 	= &cfg_type_alarm_info, 
	.ops 		= &cfg_type_alarm_info_entry_ops, 
}; 

static cfg_node_type_t cfg_type_alarm_info_common = { 
	.name 		= "Common", 
	.flag 		=  1 , 
	.parent 	= &cfg_type_alarm_info, 
	.ops 		= &cfg_type_alarm_info_common_ops, 
}; 

static cfg_node_type_t* cfg_type_alarm_info_child[] = { 
	&cfg_type_alarm_info_entry, 
	&cfg_type_alarm_info_common,
	NULL 
}; 

cfg_node_type_t cfg_type_alarm_info = { 
	.name 		= "AlarmLog", 
	.flag 		=  1 , 
	.parent 	= &cfg_type_root, 
	.nsubtype 	= sizeof(cfg_type_alarm_info_child) / sizeof(cfg_node_type_t*) - 1 , 
	.subtype 	= cfg_type_alarm_info_child, 
	.ops 		= &cfg_type_alarm_info_ops, 
}; 

