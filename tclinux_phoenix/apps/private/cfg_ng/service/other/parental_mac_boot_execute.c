#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include "utility.h"
#include "parental_common.h"
#include "parental_mac_boot_execute.h"

int svc_other_handle_event_parental_mac_boot(void)
{
	int idx = 0, cnt = 0, mac_idx = 0;
	char nodeName[32];
	char attr_val[MAX_PARENTAL_MAC_LEN + 1] = {0};
	char tmp[8] = {0};
	char mac_val[32] = {0};
	char parentalNode[32] = {0};

	for (idx = 0; idx < MAX_PARENTAL_MAC_NUM; idx++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.%d", PARENTAL_MAC_ENTRY_NODE, (idx+1));
		if(0 < cfg_get_object_attr(nodeName, PARENTAL_MAC_MAC, attr_val, sizeof(attr_val)))
		{
			cnt++;
			memset(tmp, 0, sizeof(tmp));
			if(0 < cfg_get_object_attr(nodeName, PARENTAL_ID, tmp, sizeof(tmp)) && tmp[0] != '\0')
			{
				memset(parentalNode, 0, sizeof(parentalNode));
				snprintf(parentalNode, sizeof(parentalNode), "%s.%d", PARENTAL_ENTRY_NODE, (atoi(tmp)+1));
				for (mac_idx = 0; mac_idx < MAX_MAC_PER_ENTRY; mac_idx++)
				{
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "MAC%d", mac_idx);
					if(cfg_query_object(parentalNode,NULL,NULL) <= 0)
					{		
						if(cfg_obj_create_object(parentalNode) < 0) 
						{
							tcdbg_printf("[%s:%d]cfg_obj_create_object %s fail! \n",__FUNCTION__,__LINE__,parentalNode);
							break;
						}
					}
					if(0 < cfg_get_object_attr(parentalNode, tmp, mac_val, sizeof(mac_val)) && 0 == strcmp(attr_val,mac_val))
						continue;
					cfg_set_object_attr(parentalNode, tmp, attr_val);
					break;
				}
			}
		}
	}

	memset(attr_val, 0, sizeof(attr_val));
	snprintf(attr_val, sizeof(attr_val), "%d", cnt);
	cfg_set_object_attr(PARENTAL_MAC_COMMON_NODE, PARENTAL_NUM, attr_val);

	return 0;

}

int svc_other_handle_event_parental_mac_update(void)
{
	char attrValue[64] = {0};
	char nodeName[32] = {0};
	char MacValue[64] = {0};
	char DescriptionValue[64] = {0};
	char ParentalIDValue[64] = {0};
	char strDeleteIndex[64] = {0};
	int i = 0;
	char *tp = NULL;
	char *p = NULL;
	int delnum = -1;

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s", PARENTAL_MAC_ENTRY_NODE);
	memset(attrValue, 0, sizeof(attrValue));
	if ( 0 < cfg_get_object_attr(nodeName, "Action", attrValue,sizeof(attrValue)) )
	{
		if(strcmp(attrValue, "Add") == 0)
		{
			cfg_get_object_attr(nodeName, PARENTAL_MAC_MAC, attrValue,sizeof(attrValue));
			strncpy(MacValue, attrValue, sizeof(MacValue) - 1);
			cfg_get_object_attr(nodeName, PARENTAL_MAC_DESC, attrValue, sizeof(attrValue));
			strncpy(DescriptionValue, attrValue, sizeof(DescriptionValue) - 1);
			cfg_get_object_attr(nodeName, PARENTAL_ID, attrValue, sizeof(attrValue));
			strncpy(ParentalIDValue, attrValue, sizeof(ParentalIDValue) - 1);
			for(i = 0; i < MAX_PARENTAL_MAC_NUM; i++)
			{
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), "%s.%d", PARENTAL_MAC_ENTRY_NODE, (i+1));
				if ( 0 >= cfg_get_object_attr(nodeName, PARENTAL_MAC_MAC, attrValue, sizeof(attrValue)) ) 
				{
					cfg_set_object_attr(nodeName, PARENTAL_MAC_MAC, MacValue);
					cfg_set_object_attr(nodeName, PARENTAL_MAC_DESC, DescriptionValue);
					cfg_set_object_attr(nodeName, PARENTAL_ID, ParentalIDValue);
					break;	 
				}   
			}
		}
		else if(strcmp(attrValue, "Del") == 0)
		{
			cfg_get_object_attr(nodeName, "DeleteIndex", attrValue, sizeof(attrValue));
			strncpy(strDeleteIndex, attrValue, sizeof(strDeleteIndex) - 1);
			tp = strtok_r(strDeleteIndex, ",", &p);
			while( tp != NULL)
			{
        		delnum = atoi(tp);
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), "%s.%d", PARENTAL_MAC_ENTRY_NODE, (delnum+1));
				cfg_delete_object(nodeName);
				tp = strtok_r(NULL, ",", &p);
			}
		}
		
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s", PARENTAL_MAC_ENTRY_NODE);
		cfg_set_object_attr(nodeName, "Action", "No");
	}

	svc_other_handle_event_parental_mac_boot();
	restartParental();

	return 0;

}

