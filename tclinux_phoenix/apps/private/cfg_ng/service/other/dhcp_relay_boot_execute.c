#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <cfg_cli.h> 
#include <unistd.h>
#include <sys/stat.h>
#include "utility.h"

#define DHCPRELAY_PATH 		"/etc/config/dhcprelay.sh"
#define DHCPRELAY_CMD 		"/userfs/bin/dhcrelay -pf /var/log/dhcrelay.pid %s\n"
#define DHCPD_PATH 			"/etc/udhcpd.conf"
#define DPROXYAUTO_PATH 	"/etc/dproxy.auto"
#define DHCPRELAY_PATH 		"/etc/config/dhcprelay.sh"

static int dhcprelay_func_write(void)
{
	char buf[128]={0};
	char tmp[64] = {0};
	int res = 0;

	if(cfg_query_object(DHCPRELAY_ENTRY_NODE, NULL, NULL) > 0){
		/*/userfs/bin/dhcrelay -pf /var/log/dhcrelay.pid 10.10.10.100 */
		if(cfg_obj_get_object_attr(DHCPRELAY_ENTRY_NODE, "Server", 0, tmp, sizeof(tmp)) < 0)
			return -1;
		snprintf(buf, sizeof(buf), DHCPRELAY_CMD,tmp);

		if(write2file(buf,DHCPRELAY_PATH) == -1){
			return -1;
		}
	}else{
		unlink(DHCPRELAY_PATH);
	}
	res = chmod(DHCPRELAY_PATH,777);
	if(res){
		/*do nothing,just for compile*/
	}
	return 0;
}

static int dhcprelay_func_execute(char *path)
{
	char buf[64] = {0};

	kill_process("/var/log/dhcrelay.pid");
	if(cfg_obj_get_object_attr(LAN_DHCP_NODE, "type", 0, buf, sizeof(buf)) < 0){
		return -1;
	}

	if(!strcmp(buf,"2")){/*2:dhcprelay*/
		kill_process("/var/run/udhcpd.pid");
		unlink(DHCPD_PATH);
		unlink(DPROXYAUTO_PATH);
		system(DHCPRELAY_PATH);
	}else{ 
		unlink(DHCPRELAY_PATH);
	}

	return 0;
}

int svc_other_handle_event_dhcp_relay_boot(void)
{
	FILE *startupSh=NULL;
	char cmd[128] = {0};
	
	dhcprelay_func_write();

	startupSh=fopen(DHCPRELAY_PATH,"r");
	if(startupSh){
		fclose(startupSh);
		system("chmod 755 /etc/config/dhcprelay.sh");
		snprintf(cmd, sizeof(cmd), "%s &", DHCPRELAY_PATH);
		system(cmd);
	}

	return 0;
}

int svc_other_handle_event_dhcp_relay_update(char *path)
{
	dhcprelay_func_write();
	dhcprelay_func_execute(path);

	return 0;
}

