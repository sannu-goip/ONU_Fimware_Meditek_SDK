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
#include <cfg_cli.h> 
#include "cfg_types.h" 
#include "utility.h"
#include "cfg_type_porttriggering.h"


static char* cfg_type_portTriggering_index[] = { 
	 "portTriggering_id", 
	 NULL 
}; 



/*______________________________________________________________________________
**      getAddRuleIndex
**
**      descriptions:
**              get the portTriggering rule numbers which we can used
**      parameters:
**
**      return:
**              Success:        0
**              FAIL: -1
**_______________________________________________________________________________
*/
int getRuleNumCanBeUse(char *path)
{
        int i = 0;
        int canUserFlag  = 8;
		const char sportP[32] = {0};
        char canUsenum[4] ={0};
        char node[32] = {0}; 
		int  portTriggerSetting = -1;
		char portTriggeringNode[32]={0};

        memset(canUsenum , 0 , sizeof(canUsenum));
        memset(portTriggeringNode , 0 , sizeof(portTriggeringNode));
		
		if(cfg_obj_query_object(PORTTRIGGERING_NODE,NULL,NULL) <=0)
        {
                return FAIL;
        }

		if(cfg_query_object(PORTTRIGGERING_SETTING_NODE,NULL,NULL) <= 0)
		{		
			portTriggerSetting = cfg_obj_create_object(PORTTRIGGERING_SETTING_NODE);
			if(portTriggerSetting < 0) 
			{
				tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,PORTTRIGGERING_SETTING_NODE);
				return FAIL;
			}
		}
		cfg_obj_set_object_attr(PORTTRIGGERING_SETTING_NODE, "canUseNum",0, "8");
		
        for(i = 0; i < MAX_PORTTRIGGERING_NODE; i++)
        {
            snprintf(portTriggeringNode,sizeof(portTriggeringNode),PORTTRIGGERING_ENTRY_NODE, i+1);
			if(cfg_query_object(portTriggeringNode,NULL,NULL) > 0)
            {
		        cfg_obj_get_object_attr(portTriggeringNode, "TSPort", 0, sportP, sizeof(sportP));
                if(sportP[0] == '\0' )
                {
                	continue;
                }
                
                /* the port can not be "0"*/
                if( 0 != strcmp("0" , sportP) )
                {
                    canUserFlag--;  
                }                      
            }
        }

        snprintf(canUsenum,sizeof(canUsenum),"%d",canUserFlag);
		cfg_obj_set_object_attr(PORTTRIGGERING_SETTING_NODE, "canUseNum",0, canUsenum);
        return SUCCESS;        
}





/*______________________________________________________________________________
**      setSigleTmpData2Node
**
**      descriptions:
**              save the a sigle rule which user setted to the portTriggering node.
**      parameters:
**
**      return:
**              Success:        0
**              FAIL: -1
**_______________________________________________________________________________
*/
int setSigleTmpData2Node(char* path, int TriggeringIndex)
{
        int faileFlag = 0;
        int i = 0 , j = 0;
        char attrName[][MAX_VALUE_SIZE] = {{"Name"},{"TSPort"},{"TEPort"},{"TProtocol"}, {"OSPort"},{"OEPort"},{"OProtocol"},{""}};
        char attrName_tmp[][MAX_VALUE_SIZE] = {{"NameTmp"},{"TSPortTmp"},{"TEPortTmp"},{"TProtocolTmp"}, {"OSPortTmp"},{"OEPortTmp"},{"OProtocolTmp"},{""}};
        char defaultValue[][MAX_VALUE_SIZE] = {{"-"},{"0"},{"0"},{"-"}, {"0"},{"0"},{"-"},{""}};
        char value_tmp[8][MAX_VALUE_SIZE];

		char tmpattr[16]={0};
		char tmpvalue[32]={0};
		char tmpnode[32]={0};

		char portTriggeringNode[32]={0};
		int portTriggerEntryIdx = -1;
		char attrname[16]={0};
		char portvalue[32]={0};
            
        memset(portTriggeringNode,0,sizeof(portTriggeringNode)); 
        memset(tmpnode,0,sizeof(tmpnode)); 
        memset(value_tmp,0,sizeof(value_tmp));  
        snprintf(portTriggeringNode,sizeof(portTriggeringNode),PORTTRIGGERING_ENTRY_NODE , TriggeringIndex);  

        for(j = 0; j < MAX_PORTTRIGGERING_NODE; j++)
        {
                snprintf(tmpnode,sizeof(tmpnode),GUITEMP_ENTRY_NODE , j+1);
                /* get all temp value to value_tmp*/
                for(i = 0;strlen(attrName_tmp[i])!=0;i++)
                {
                        memset(tmpattr,0,sizeof(tmpattr));
                        memset(tmpvalue,0,sizeof(tmpvalue));
                        strncpy(tmpattr,attrName_tmp[i],sizeof(tmpattr));
						tmpattr[sizeof(tmpattr) - 1] = 0;
						if(cfg_obj_get_object_attr(tmpnode, tmpattr, 0, tmpvalue, sizeof(tmpvalue )) < 0)
                        {
                                faileFlag = 1;
                                break;
                        }
                        else
                        {
                            strncpy(value_tmp[i] , tmpvalue , MAX_VALUE_SIZE);
                        }
                }

                if(1 == faileFlag)
                {
                    return -1;
                }

                if( 0 != strcmp("0" , value_tmp[1]) )
                {
                    for(i = 0;strlen(attrName_tmp[i])!=0;i++)
                    {
                            /* save to portTriggering node*/
                            memset(attrname,0,sizeof(attrname));
                            memset(portvalue,0,sizeof(portvalue));
                            strncpy(attrname,attrName[i],sizeof(attrname));
							attrname[sizeof(attrname) - 1] = 0;
                            strncpy(portvalue,value_tmp[i],sizeof(portvalue));
							portvalue[sizeof(portvalue) - 1] = 0;
							
							if(cfg_query_object(portTriggeringNode,NULL,NULL) <= 0)
							{		
								portTriggerEntryIdx = cfg_obj_create_object(portTriggeringNode);
								if(portTriggerEntryIdx < 0) 
								{
									tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,portTriggeringNode);
									return -1;
								}
							}
							cfg_obj_set_object_attr(portTriggeringNode, attrname, 0, portvalue);
								
                             /* clear the tmp data*/
                            memset(tmpattr,0,sizeof(tmpattr));
                            memset(tmpvalue,0,sizeof(tmpvalue));
                            strncpy(tmpattr,attrName_tmp[i],sizeof(tmpattr));
							tmpattr[sizeof(tmpattr) - 1] = 0;
                            strncpy(tmpvalue,defaultValue[i],sizeof(tmpvalue));
							tmpvalue[sizeof(tmpvalue) - 1] = 0;
							cfg_obj_set_object_attr(tmpnode, tmpattr, 0, tmpvalue);
                    }
                    return 0;      
                }
        }
		
        return 0;                
}



/*______________________________________________________________________________
**      setAllTempData2Node
**
**      descriptions:
**              save the all rules which user setted to the portTriggering node.
**      parameters:
**
**      return:
**              Success:        0
**              FAIL: -1
**_______________________________________________________________________________
*/
int setAllTempData2Node(char* path)
{
	int i = 0;
	const char sportP[32] = {0};
	char tmp[32]={0};
	if(cfg_obj_query_object(PORTTRIGGERING_NODE,NULL,NULL)<=0)
	{
		return FAIL;
	}

	for(i = 0; i < MAX_PORTTRIGGERING_NODE; i++)
	{
		memset(tmp, 0 , sizeof(tmp));
        snprintf(tmp ,sizeof(tmp),PORTTRIGGERING_ENTRY_NODE, i+1);
        if(cfg_obj_query_object(tmp,NULL,NULL) > 0)
        {
            cfg_obj_get_object_attr(tmp, "TSPort", 0, sportP, sizeof(sportP));
            if(sportP[0] == '\0')
            {
                    if(-1 == setSigleTmpData2Node(path , i+1))
                            return FAIL; 
                    continue;
            }
            
            /* the port can not be "0"*/
            if( 0 == strcmp("0" , sportP) )
            {
                    if(-1 == setSigleTmpData2Node(path , i+1))
                            return FAIL; 
            }                      
        }
        else
        {
            if(-1 == setSigleTmpData2Node(path , i+1))
                    return FAIL;    
        }
	}
	return SUCCESS;
}

int portTriggerRuleShellExec()
{
	int i = 0, j = 0, k = 0, l = 0;
	int n = 0, ret = 0, ret2 = 0;
	FILE *fp = NULL;
	char nodeName[MAXLEN_NODE_NAME] = {0};
	char trigger_start_port[10]={0};
	char trigger_end_port[10]={0};
	char trigger_protocol[10]={0};
	char open_start_port[10]={0};
	char open_end_port[10]={0};
	char open_protocol[10]={0};
	char trigger_start_port1[10]={0};
	char trigger_end_port1[10]={0};
	char trigger_protocol1[10]={0};
	char open_start_port1[10]={0};
	char open_end_port1[10]={0};
	char open_protocol1[10]={0};
	int sameFlag[8] = {0,0, 0, 0,0, 0, 0, 0};
	char tmp[CMD_MAX_SIZE]={0};  
	char *trig_proto[] = {"udp", "tcp"};
	char *open_proto[] = {"udp", "tcp"};

	fp = fopen(PORT_TRIGGER_SH_PATH, "w");
	if(fp == NULL)
		return FAIL;

	snprintf(tmp,sizeof(tmp),"iptables -t nat -F PREROUTING_WAN\n");
	fputs(tmp, fp);
	snprintf(tmp,sizeof(tmp),"iptables -t nat -F PREROUTING_RULE\n");
	fputs(tmp, fp);
	snprintf(tmp,sizeof(tmp),"iptables -t filter -F FORWARD_WAN\n");
	fputs(tmp, fp);
	
	for(i = 0; i < MAX_PORTTRIGGERING_NODE; i++)
	{
		memset(nodeName,0,sizeof(nodeName));
		snprintf(nodeName ,sizeof(nodeName),PORTTRIGGERING_ENTRY_NODE, i+1);
		if(cfg_obj_get_object_attr(nodeName, "Name", 0, tmp, sizeof(tmp )) < 0)
			continue;
		if(cfg_obj_get_object_attr(nodeName, "TSPort", 0, trigger_start_port, sizeof(trigger_start_port )) < 0)
			continue;
		if(cfg_obj_get_object_attr(nodeName, "TEPort", 0, trigger_end_port, sizeof(trigger_end_port )) < 0)
			continue;
		if(cfg_obj_get_object_attr(nodeName, "TProtocol", 0, trigger_protocol, sizeof(trigger_protocol )) < 0)
			continue;
		if(cfg_obj_get_object_attr(nodeName, "OSPort", 0, open_start_port, sizeof(open_start_port )) < 0)
			continue;
		if(cfg_obj_get_object_attr(nodeName, "OEPort", 0, open_end_port, sizeof(open_end_port )) < 0)
			continue;
		if(cfg_obj_get_object_attr(nodeName, "OProtocol", 0, open_protocol, sizeof(open_protocol )) < 0)
			continue;
		
		if(sameFlag[i] == SKIP_FOR_MERGE)
		{
			continue;
		}
		for(l = i+1; l < MAX_PORTTRIGGERING_NODE; l++)
		{
			if(sameFlag[l] == SKIP_FOR_MERGE)
			{
				continue;
			}
			memset(nodeName,0,sizeof(nodeName));
			snprintf(nodeName ,sizeof(nodeName),PORTTRIGGERING_ENTRY_NODE, l+1);
			if(cfg_obj_get_object_attr(nodeName, "Name", 0, tmp, sizeof(tmp )) < 0)
				continue;
			if(cfg_obj_get_object_attr(nodeName, "TSPort", 0, trigger_start_port1, sizeof(trigger_start_port1 )) < 0)
				continue;
			if(cfg_obj_get_object_attr(nodeName, "TEPort", 0, trigger_end_port1, sizeof(trigger_end_port1 )) < 0)
				continue;
			if(cfg_obj_get_object_attr(nodeName, "TProtocol", 0, trigger_protocol1, sizeof(trigger_protocol1 )) < 0)
				continue;
			if(cfg_obj_get_object_attr(nodeName, "OSPort", 0, open_start_port1, sizeof(open_start_port1 )) < 0)
				continue;
			if(cfg_obj_get_object_attr(nodeName, "OEPort", 0, open_end_port1, sizeof(open_end_port1 )) < 0)
				continue;
			if(cfg_obj_get_object_attr(nodeName, "OProtocol", 0, open_protocol1, sizeof(open_protocol1 )) < 0)
				continue;

			if((strcmp(trigger_start_port1, trigger_start_port) == 0) && (strcmp(trigger_end_port1, trigger_end_port) == 0)
				&&(strcmp(trigger_protocol1, trigger_protocol) == 0) && (strcmp(open_start_port1, open_start_port) == 0) && (strcmp(open_end_port1, open_end_port) == 0))
			{
				if((strcmp(open_protocol1, "TCP")==0 && strcmp(open_protocol, "UDP")==0)
					|| (strcmp(open_protocol1, "UDP")==0 && strcmp(open_protocol, "TCP")==0)){
						sameFlag[i] = USE_MERGE_RULE;/*all*/
						sameFlag[l] = SKIP_FOR_MERGE;/*skip*/
					}
			}
		}
		/* iptables -t nat -A PREROUTING_WAN -p $OPEN_PROTO --dport $OPEN_START_PORT:$OPEN_END_PORT -j TRIGGER --trigger-type dnat
		  * iptables -t filter -A FORWARD_WAN -p $OPEN_PROTO --dport $OPEN_START_PORT:$OPEN_END_PORT -j TRIGGER --trigger-type in
		  * iptables -t nat -A PREROUTING_RULE -i br0 -p $TRIGGER_PROTO --dport $TRIGGER_START_PORT:$TRIGGER_END_PORT -j TRIGGER --trigger-type out --trigger-proto 
		  *                 $OPEN_PROTO --trigger-match $TRIGGER_START_PORT:$TRIGGER_END_PORT --trigger-relate $OPEN_START_PORT-$OPEN_END_PORT
		  */
	  	if (strcmp(open_protocol, TCP_UDP)==0 || sameFlag[i] == USE_MERGE_RULE) {
			for (j = 0; j < TOTAL_PORT_TRIGGER_PROTO; j++) {
				snprintf(tmp, sizeof(tmp),PREROUTING_WAN_CMD_FORMAT, open_proto[j], open_start_port, open_end_port);
				fputs_escape(tmp, fp);
				snprintf(tmp, sizeof(tmp),FORWARD_WAN_CMD_FORMAT, open_proto[j], open_start_port, open_end_port);
				fputs_escape(tmp, fp);
			}
			
		  	if (strcmp(trigger_protocol, TCP_UDP)==0) {
				for (j = 0; j < TOTAL_PORT_TRIGGER_PROTO; j++) {
					snprintf(tmp, sizeof(tmp),PREROUTING_RULE_CMD_FORMAT2, trig_proto[j], trigger_start_port, trigger_end_port, 
						open_start_port, open_end_port, open_start_port, open_end_port);
					fputs_escape(tmp, fp);
				}
			} else {
				snprintf(tmp, sizeof(tmp),PREROUTING_RULE_CMD_FORMAT2, trigger_protocol, trigger_start_port, trigger_end_port,
					open_start_port, open_end_port, open_start_port, open_end_port);
				fputs_escape(tmp, fp);
			} 
		}	
		else {
			snprintf(tmp, sizeof(tmp),PREROUTING_WAN_CMD_FORMAT, open_protocol, open_start_port, open_end_port);
			fputs_escape(tmp, fp);		
			snprintf(tmp, sizeof(tmp),FORWARD_WAN_CMD_FORMAT, open_protocol, open_start_port, open_end_port);
			fputs_escape(tmp, fp);
		  	if (strcmp(trigger_protocol, TCP_UDP)==0) {
				for (j = 0; j < TOTAL_PORT_TRIGGER_PROTO; j++) {
					snprintf(tmp, sizeof(tmp),PREROUTING_RULE_CMD_FORMAT, trig_proto[j], trigger_start_port, trigger_end_port, 
							open_protocol, open_start_port, open_end_port, open_start_port, open_end_port);	
					fputs_escape(tmp, fp);
				}
			} else {
				snprintf(tmp,sizeof(tmp), PREROUTING_RULE_CMD_FORMAT, trigger_protocol, trigger_start_port, trigger_end_port, 
						open_protocol, open_start_port, open_end_port, open_start_port, open_end_port);	
				fputs_escape(tmp, fp);		
			}
		}
	}
	
	fclose(fp);
	ret = chmod(PORT_TRIGGER_SH_PATH ,777);
	ret2 = system(PORT_TRIGGER_SH_PATH);
	unlink(PORT_TRIGGER_SH_PATH);
	
	return SUCCESS;
}

int svc_cfg_porttriggering_boot(void)
{

	system("iptables -t nat -N PREROUTING_WAN");
	system("iptables -t nat -N PREROUTING_RULE");
	system("iptables -t nat -A PREROUTING -j PREROUTING_RULE");
	system("iptables -t nat -A PREROUTING -j PREROUTING_WAN");
	portTriggerRuleShellExec();

	return 0;

}


int cfg_type_portTriggering_func_commit(char* path) 
{ 
	setAllTempData2Node(path);
	portTriggerRuleShellExec();
	return 0;	
} 



int cfg_type_portTriggering_setting_func_get(char* path,char* attr,char* val,int len)
{
	getRuleNumCanBeUse(path);
	return cfg_obj_get_object_attr(path,attr,0,val,len);
}

static cfg_node_ops_t cfg_type_portTriggering_entry_ops  = { 
	 .get = cfg_type_default_func_get, 
	 .set = cfg_type_default_func_set, 
	 .create = cfg_type_default_func_create, 
	 .delete = cfg_type_default_func_delete, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_portTriggering_func_commit 
}; 


static cfg_node_ops_t cfg_type_portTriggering_setting_ops  = { 
	 .get = cfg_type_portTriggering_setting_func_get, 
	 .set = cfg_type_default_func_set, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_portTriggering_func_commit 
}; 


static cfg_node_type_t cfg_type_portTriggering_entry = { 
	 .name = "Entry", 
	 .flag = CFG_TYPE_FLAG_MULTIPLE | CFG_TYPE_FLAG_UPDATE | MAX_PORTTRIGGERING_NODE, 
	 .parent = &cfg_type_portTriggering, 
	 .index = cfg_type_portTriggering_index, 
	 .ops = &cfg_type_portTriggering_entry_ops, 
}; 


static cfg_node_type_t cfg_type_portTriggering_setting = { 
	 .name = "setting", 
	 .flag =  1, 
	 .parent = &cfg_type_portTriggering, 
	 .ops = &cfg_type_portTriggering_setting_ops, 
}; 


static cfg_node_ops_t cfg_type_portTriggering_ops  = { 
	 .set = cfg_type_default_func_set, 
	 .get = cfg_type_default_func_get, 
	 .query = cfg_type_default_func_query, 
	 .commit = cfg_type_portTriggering_func_commit 
}; 


static cfg_node_type_t* cfg_type_portTriggering_child[] = { 
	 &cfg_type_portTriggering_entry, 
	 &cfg_type_portTriggering_setting, 
	 NULL 
}; 


cfg_node_type_t cfg_type_portTriggering = { 
	 .name = "portTriggering", 
	 .flag = CFG_TYPE_FLAG_UPDATE | 1 , 
	 .parent = &cfg_type_root, 
	 .nsubtype = sizeof(cfg_type_portTriggering_child) / sizeof(cfg_node_type_t*) - 1 , 
	 .subtype = cfg_type_portTriggering_child, 
	 .ops = &cfg_type_portTriggering_ops, 
}; 
