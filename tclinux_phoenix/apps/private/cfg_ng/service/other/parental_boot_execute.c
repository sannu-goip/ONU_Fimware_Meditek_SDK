#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_api.h>
#include <utility.h>
#include <cfg_msg.h>
#include "parental_common.h"
#include "parental_boot_execute.h"

int svc_other_handle_event_parental_boot(void)
{
	int idx = 0, cnt = 0;
	char nodeName[32], attr_val[5] = {0};
	static int is_parental_init = 0;
	char tmp[32] = {0};

	for (idx = 0; idx < MAX_PARENTAL_NUM; idx++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), "%s.%d", PARENTAL_ENTRY_NODE, (idx+1));
		if (0 < cfg_get_object_attr(nodeName, PARENTAL_ACTIVE, attr_val, sizeof(attr_val)))
		{
			cnt++;
		}
	}

	snprintf(attr_val, sizeof(attr_val), "%d", cnt);
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s", PARENTAL_COMMON_NODE);
	cfg_set_object_attr(nodeName, PARENTAL_NUM, attr_val);

    initParentalDuration();
	if ( 0 == is_parental_init )
	{
		is_parental_init = 1;
		restartParental();
		cfg_get_object_attr(nodeName,"parental_id",tmp,sizeof(tmp));
		cfg_set_object_attr(WEBCURSET_ENTRY_NODE, "parental_id", tmp);
	}
	else
	{
		cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "parental_id", tmp, sizeof(tmp));
		cfg_set_object_attr(nodeName,"parental_id",tmp);
	}
	return 0;
}

int svc_other_handle_event_parental_update(void)
{
	tcdbg_printf("func = %s. line = %d\n", __func__, __LINE__);

	svc_other_handle_event_parental_boot();
	restartParental();
	return 0;
}


