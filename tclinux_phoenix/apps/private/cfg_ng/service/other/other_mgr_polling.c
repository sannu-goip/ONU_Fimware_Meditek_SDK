#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "parental_ctrl_polling.h"
#include "other_mgr_polling.h"
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
#include "other_cfg_diagnostics.h"
#endif
#include "cfg_api.h"


static void other_mgr_polling_pthread(void)
{
	char load_status[16] = {0};

    while(1)
    {
   		sleep(1);

		if(checkCFGload() == 0){
			continue;
		}
		
		checkParentalDuration();
		
#if defined(TCSUPPORT_CT_PPPOE_IPV6_EMULATOR)
		checkPPPoE_Emulator_timer();
#endif
    }
    
	pthread_exit(0);
}

int creat_other_mgr_polling_pthread(void)
{
    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, (void *)other_mgr_polling_pthread, NULL) < 0)
    {
        printf("creat_other_mgr_polling_pthread: create thread fail \n");
        return -1;
    }

    return 0;
}

