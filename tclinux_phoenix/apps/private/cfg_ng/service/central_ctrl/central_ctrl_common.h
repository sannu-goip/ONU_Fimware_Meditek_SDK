#ifndef __SVC_CENTRAL_CTRL_COMMON_H__
#define __SVC_CENTRAL_CTRL_COMMON_H__
#include "../cfg/cfg_msg.h"

/******************************************/
#ifndef TRUE
#define TRUE        (1)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif


#ifndef ENABLE
#define ENABLE      (1)
#endif

#ifndef DISABLE
#define DISABLE     (0)
#endif

enum _svc_central_ctrl_dbg_level_
{
	E_NO_INFO_LEVEL         = 0,
	E_ERR_INFO_LEVEL        = 1,
	E_WARN_INFO_LEVEL       = 2, 
	E_CRITICAL_INFO_LEVEL   = 4,
	E_NOTICE_INFO_LEVEL     = 8,
	E_TRACE_INFO_LEVEL      = 16,
	E_DBG_INFO_LEVEL        = 32,
};

extern int g_svc_central_ctrl_level;

#define SVC_CENTRAL_CTRL_ERROR_INFO(fmt, ...)  do{ \
													if(g_svc_central_ctrl_level & E_DBG_INFO_LEVEL) \
														tcdbg_printf("err info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
												 }while(0)


#define SVC_CENTRAL_CTRL_DEBUG_INFO(fmt, ...)  do{ \
													if(g_svc_central_ctrl_level & E_DBG_INFO_LEVEL) \
														tcdbg_printf("Debug info, func: %s, line:%d  " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__); \
												 }while(0)

/******************************************/
void file_read(char *path, char *buf, int size);
void file_write(char *path, char *buf, int size);

int svc_central_ctrl_execute_cmd(char* cmd);
void svc_cc_debug_level(int level);
void cc_send_evt(int type, int evt_id, char *buf);

#endif


