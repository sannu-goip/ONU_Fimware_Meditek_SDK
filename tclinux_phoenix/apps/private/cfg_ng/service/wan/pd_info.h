#ifndef __SVC_WAN_PD_INFO_H__
#define __SVC_WAN_PD_INFO_H__
#include "wan_common.h"

int check_pd_enable(int ifindex);

int get_dhcp6s_dns_wan_interface(wan_entry_obj_t* obj);

int get_run_pd_index(void);

void update_run_pd_index(int ifindex);

int get_ra_wan_pd_interface(wan_entry_obj_t* obj);

void restart_pd_function(int type, wan_entry_obj_t* obj);

int update_br0_pdaddress(int if_index, int evt);

void update_pd_info(wan_entry_obj_t* obj, int evt);

#if defined(TCSUPPORT_CT_VRWAN)
int setVRNPTRule(int action, wan_entry_obj_t* obj);
int send_update_evt_to_ra_dhcp(wan_entry_obj_t* obj, char *value);
int check_vr_wan(wan_entry_obj_t* obj);
int check_default_wan(wan_entry_obj_t* obj);
#endif
#if defined(TCSUPPORT_NPTv6)
int setNPTRule(unsigned int nptwan_mask_pre, unsigned int nptwan_mask,int wan_index, int action);

int update_npt_related_wan_rule(wan_entry_obj_t* obj, int action);
#endif

#endif
