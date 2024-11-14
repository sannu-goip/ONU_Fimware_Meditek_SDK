#ifndef __SVC_WAN_DSLITE_INFO_H__
#define __SVC_WAN_DSLITE_INFO_H__

#include "wan_common.h"

#define START_DSLITE "/usr/script/dslite_start.sh %s %s %s"
#define STOP_DSLITE "/usr/script/dslite_stop.sh %s"

#define DS_ACTIVE           (1<<1)
#define DS_BINDSTATE_UP     (1<<2)

void init_dslite_global_variable(void);

void stop_dslite(wan_entry_obj_t* obj);

void notify_dslite_state(void);


#endif

