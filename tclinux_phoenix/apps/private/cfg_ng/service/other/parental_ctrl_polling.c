#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cfg_msg.h>
#include "parental_common.h"

void checkParentalDuration(void)
{
	int node_idx = 0, dur_idx = 0;

	/* Parental_Entry(i) */
	for (node_idx = 0; node_idx < MAX_PARENTAL_NUM; node_idx++)
	{
		/* Parental_Entry(I) ->StartTime(i)  */
		for (dur_idx = 0; dur_idx < MAX_DURATION_PER_ENTRY; dur_idx++)
		{
			if (isSetpIntoDuration(node_idx, dur_idx) || isGoOutOfDuration(node_idx, dur_idx))
			{
				restartParental();
			}
		}
	}
}

