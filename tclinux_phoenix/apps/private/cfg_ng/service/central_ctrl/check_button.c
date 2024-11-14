#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cfg_api.h>
#include "central_ctrl_common.h"
#include "check_button.h"
#include "utility.h" 

/***************************************************/
static char is_skip_check(void);
static void reset_button_action(int op);
static int get_isbootset_value(void);
/***************************************************/
static char is_skip_check(void)
{
    char              node[32];
    char        led_status[16];
	char      led_flag = FALSE;
    char          skip = FALSE;

    memset(node, 0, sizeof(node));
    strcpy(node, SYS_ENTRY_NODE);
    memset(led_status, 0, sizeof(led_status));
    cfg_get_object_attr(node, "ledStatus", led_status, sizeof(led_status));
    if(0 == strcmp(led_status, "off"))
    {
        led_flag = TRUE ;
    }
    
    if (TRUE == led_flag && FALSE == skip) 
    {
        skip = TRUE;
    }

    return skip;
}


static int getSipStatus()
{
	int prevSipStatus = 0;
	char tmp[32] = {0};

	if(cfg_obj_get_object_attr(VOIPBASIC_COMMON_NODE, "prevSipStatus", 0, tmp, sizeof(tmp)) < 0)
		return -1;
	
	prevSipStatus = atoi(tmp);
	return prevSipStatus;
}

static int voip_led_recover()
{
	char strRegStatus[32] = {0}, trafficSta[32] = {0};
	char nodeName[64] = {0};
	
#if defined(TCSUPPORT_VOIP) && defined(TCSUPPORT_ECN_SIP)
#if defined(TCSUPPORT_ECN_MEGACO)
	if(0 == getSipStatus()) 
	{
#endif
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VOIPSYSPARAM_ENTRY_NODE, 1);
		cfg_obj_get_object_attr(nodeName, "SC_ACCT_INFO_MEDIA_STATUS", 0, trafficSta, sizeof(trafficSta));
		if (0 == strncmp(trafficSta, "Media-Begin", strlen("Media-Begin"))) 
		{
			append2file("/proc/fxs/sipRegLed","0 2");            /* for one fxs */
		}
		else
		{       
			/* VOIP registeted, turn on the voip led */
			cfg_obj_get_object_attr(nodeName, "SC_ACCT_INFO_REG_STATUS", 0, strRegStatus, sizeof(strRegStatus));
			if(0 == strncmp(strRegStatus, "Registered", strlen("Registered"))) 
			{
				append2file("/proc/fxs/sipRegLed","0 1");            /* for one fxs */
			}
		}

		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), VOIPSYSPARAM_ENTRY_NODE, 2);
		cfg_obj_get_object_attr(nodeName, "SC_ACCT_INFO_MEDIA_STATUS", 0, trafficSta, sizeof(trafficSta));
		if (0 == strncmp(trafficSta, "Media-Begin", strlen("Media-Begin"))) 
		{
			append2file("/proc/fxs/sipRegLed","1 2");            /* for two fxs */
		}
		else
		{      
			/* VOIP registeted, turn on the voip led */
			cfg_obj_get_object_attr(nodeName, "SC_ACCT_INFO_REG_STATUS", 0, strRegStatus, sizeof(strRegStatus));
			if(0 == strncmp(strRegStatus,"Registered",strlen("Registered"))) 
			{
				append2file("/proc/fxs/sipRegLed","1 1");            /* for two fxs */
			}
		}
#if defined(TCSUPPORT_ECN_MEGACO)
	}
#endif
#endif

#if defined(TCSUPPORT_VOIP) && defined(TCSUPPORT_ECN_MEGACO)
#if defined(TCSUPPORT_ECN_SIP)
	if(1 == getSipStatus()) 
	{
#endif
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), INFOVOIPH248_ENTRY_NODE, 2);
		cfg_obj_get_object_attr(nodeName, "lineInfoStatus", 0, trafficSta, sizeof(trafficSta));
		if (0 == strncmp(trafficSta, "Connect", strlen("Connect"))) 
		{
			append2file("/proc/fxs/sipRegLed","1 2");            /* for two fxs */
		}
        else 
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VOIPH248_ENTRY_NODE, 2);
            cfg_obj_get_object_attr(nodeName, "UserServiceState", 0, strRegStatus, sizeof(strRegStatus));
		    if (0 == strcmp(strRegStatus, "1")) 
			{
				append2file("/proc/fxs/sipRegLed","1 1");           	 	/* for two fxs */
		    }
        }

		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), INFOVOIPH248_ENTRY_NODE, 1);
		cfg_obj_get_object_attr(nodeName, "lineInfoStatus", 0, trafficSta, sizeof(trafficSta));
		if (0 == strncmp(trafficSta, "Connect", strlen("Connect"))) 
		{
			append2file("/proc/fxs/sipRegLed","0 2");            /* for one fxs */
		}
        else 
		{
			memset(nodeName, 0, sizeof(nodeName));
			snprintf(nodeName, sizeof(nodeName), VOIPH248_ENTRY_NODE, 1);
            cfg_obj_get_object_attr(nodeName, "UserServiceState", 0, strRegStatus, sizeof(strRegStatus));
		    if (0 == strcmp(strRegStatus, "1")) 
			{
				append2file("/proc/fxs/sipRegLed","0 1");           	 	/* for one fxs */
		    }
        }
#if defined(TCSUPPORT_ECN_SIP)
	}
#endif
#endif
	return 0;
}

char button_op(char op)
{
    if(BUTTON_RECOVER == op)
    {
        svc_central_ctrl_execute_cmd("sys led recover");
        voip_led_recover();
#if defined(TCSUPPORT_CT_JOYME2)
		cfg_set_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "CurrStatus", "1");
#endif
    }
    else
    {
        svc_central_ctrl_execute_cmd("sys led off");
#if defined(TCSUPPORT_CT_JOYME2)
		cfg_set_object_attr(TIMER_LEDTIMER_ENTRY_NODE, "CurrStatus", "0");
#endif
    }
  
    return op;
}

void check_led_switch_button(char xpon_traffic_state)
{
	char buf[8];
    int                fd = -1;
    int             bytes = 0;
    int      button_state = 0; 
    static char      skip = FALSE;
    static int pre_led_op = BUTTON_RECOVER;
    
	fd = open(LED_SWITCH_BUTTON_PATH, O_RDONLY|O_NONBLOCK);
    if(-1 == fd)
    {
        printf("open %s faile.\n", LED_SWITCH_BUTTON_PATH);
        return ;
    }
    
    memset(buf, 0, sizeof(buf));
    bytes = read(fd, buf, sizeof(buf));
    if(0 >= bytes)
    {
        goto out;
    }
    
    /* 1 led switch button is pressed; 2 led off; 3 recover*/
    button_state = atoi(buf);
    switch (button_state)
    {
        case 1:
        {   
            if(FALSE == skip)
            {
                skip = is_skip_check();
                if(TRUE == skip)
                {
                    goto out;
                }
            }
            if(BUTTON_RECOVER == pre_led_op)
            {
                pre_led_op = button_op(BUTTON_OFF);
            }
            else
            {
                pre_led_op = button_op(BUTTON_RECOVER);
            }
            break;
        }
        case 2:
        {
            pre_led_op = button_op(BUTTON_OFF);
            break;
        }
        case 3:
        {
            pre_led_op = button_op(BUTTON_RECOVER);            
            break;
        }
        default:
        {
            break;
        }
    }
    
out:
    close(fd);
    return ;
}

static void reset_button_action(int op)
{
	char path[32];
 	char buf[8];
	memset(path, 0, sizeof(path));
 	memset(buf, 0, sizeof(buf));
	
 	snprintf(path, sizeof(path), SYSTEM_ENTRY_NODE);
	snprintf(buf, sizeof(buf), "%d", op);
	
	cfg_set_object_attr(path,"reboot_type", buf);
	cfg_commit_object(path);

 	return ;
}

static int get_isbootset_value(void)
{
	char isbootset[4] = {0};
	
    cfg_get_object_attr(GLOBALSTATE_NODE, "isbootset", isbootset, sizeof(isbootset));
	if(isbootset[0] != '\0')
		return atoi(isbootset);
	else
		return 0;
}

void check_reset_button(void)
{
    int                fd = -1;
    int             bytes = 0;
	char 			buf[8];
	int 			h_sec = 0;
	int 			sec = 0;
#if defined(TCSUPPORT_CT_PON_SC)
	char 			path[32];
#endif
#ifdef TCSUPPORT_CUC
	const int 	PHASE3_SEC = 12;
	const int 	PHASE4_SEC = 18;
#endif
	int 		PHASE1_SEC = 2;
	int 		PHASE2_SEC = 20;
	char	 reset_default = 0;
	int          isbootset = 0;

#if defined(TCSUPPORT_NP)
	PHASE1_SEC = 5;
#endif

	isbootset = get_isbootset_value();
	if( isbootset )
		return;

	do{
		fd = open(RESET_BUTTON_PATH, O_RDONLY|O_NONBLOCK);
		if((-1 == fd) )
		{
			break;
		}
		memset(buf, 0, sizeof(buf));
		bytes = read(fd, buf, 4);
		close(fd);
		if(0 >= bytes)
		{
			break;
		}
		/*record last h_sec*/
		sec = h_sec;
		
		h_sec = atoi(buf);

		if(h_sec <= 0)
			break;
		
		sleep(1);
	}while(h_sec > 0 && h_sec <= PHASE2_SEC);

	if ( h_sec > 0 )
		sec = h_sec;

	if( sec > 0 )
		tcdbg_printf("====>[debug]%s(): %d sec:%d\n", __FUNCTION__,__LINE__,sec);
	
	if(sec >= PHASE2_SEC)
	{
#if defined(TCSUPPORT_CT_JOYME2)			
#if defined(TCSUPPORT_CT_DBUS)
			doValPut(RESETSOURCEPATH, "1");
#if defined(TCSUPPORT_CT_JOYME4)
			doValPut("/opt/upt/apps/info/reset_syslog", "1");
#endif
#endif
			doValPut("/tmp/restore_source", "1");
			sync();
#endif
#if defined(TCSUPPORT_CT_LONG_RESETBTN)
#if defined(TCSUPPORT_CMCC)
		reset_button_action(BOOT2DEFSET);
#else	
		reset_button_action(LONGRESET_BOOT2DEFSET);
#endif

#endif
	}
	else if(sec >= PHASE1_SEC)
	{
#if defined(TCSUPPORT_CT_JOYME2)			
#if defined(TCSUPPORT_CT_DBUS)
				doValPut(RESETSOURCEPATH, "2");
#if defined(TCSUPPORT_CT_JOYME4)

				doValPut("/opt/upt/apps/info/reset_syslog", "2");
#endif
#endif
				doValPut("/tmp/restore_source", "2");
				sync();
#endif
#if !defined(TCSUPPORT_CMCC)
		reset_button_action(BOOT2DEFSET);
#endif
	}
#ifdef TCSUPPORT_CUC
	/*6s~9s:do nothing*/
	else if (PHASE3_SEC < sec && PHASE4_SEC > sec) {		
		reset_default = 0;
	}
#endif
	return ;	
}




void  check_button_type(void)
{
	int                fd = -1;
	int             bytes = 0;
	int 			type  = 0;
	int        			i = 0 ;
	char 			 buf[8];
	char 			pts[16];
	char 		   cmd[128];
	
	fd = open(BUTTON_TYPE_PATH, O_RDONLY|O_NONBLOCK);
	if(-1 == fd)
	{
		printf("open %s faile.\n", BUTTON_TYPE_PATH);
		return;
	}
	bytes = read(fd, buf, 4); 
	close(fd);
    if(0 >= bytes)
    {
		return ;
    }
	type = atoi(buf);
	
#ifdef TCSUPPORT_LED_SWITCH_BUTTON
	if((type > 0) && (type < 5))
#else
	if((type > 0) && (type < 4))
#endif
	{
		for(i=0; i<PTS_NUMBER; i++)
		{
			memset(pts, 0, sizeof(pts));
			snprintf(pts, sizeof(pts), "/dev/pts/%d", i);
			fd = open(pts, O_RDONLY|O_NONBLOCK);
			if(-1 == fd)
			{
				continue;
			}
			close(fd);


			memset(cmd, 0, sizeof(cmd));
			if(1 == type)
			{
				snprintf(cmd, sizeof(cmd), "echo %s > /dev/pts/%d\n", "Reset button is pressed!", i);
			}
			else if(2 == type)
			{
				snprintf(cmd, sizeof(cmd), "echo %s > /dev/pts/%d\n", "Wifi button is pressed!", i);
			}
#ifdef TCSUPPORT_LED_SWITCH_BUTTON
			else if(3 == type)
			{
				snprintf(cmd, sizeof(cmd), "echo %s > /dev/pts/%d\n", "WPS button is pressed!", i);
			}
			else
			{
				snprintf(cmd, sizeof(cmd), "echo %s > /dev/pts/%d\n", "LED switch button is pressed!", i);
			}
#else
			else
			{
				snprintf(cmd, sizeof(cmd),"echo %s > /dev/pts/%d\n", "WPS button is pressed!", i);
			}
#endif
			svc_central_ctrl_execute_cmd(cmd);
		}
	}

	if(type != 0)
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "echo %d > /proc/tc3162/button_type\n", 0);
		svc_central_ctrl_execute_cmd(cmd);
	}

	return ;	
}




