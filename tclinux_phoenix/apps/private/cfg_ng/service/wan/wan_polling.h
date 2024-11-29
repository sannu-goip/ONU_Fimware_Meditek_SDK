#ifndef __SVC_WAN_POLLING_H__
#define __SVC_WAN_POLLING_H__

int creat_wan_mgr_polling_pthread(void);
void wan_mgr_update_status(E_PON_STATUS_T status);
void wan_mgr_ponstatus_lock_init(void);

#endif
