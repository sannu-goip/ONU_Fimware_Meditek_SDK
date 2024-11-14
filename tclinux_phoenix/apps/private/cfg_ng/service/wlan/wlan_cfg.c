
/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <svchost_api.h> 
#include <cfg_api.h>
#include <libapi_lib_wifimgr.h>
#include <cfg_type_info.h>
#include <cfg_type_wlan.h>
#include <cfg_type_guestssidinfo.h>
#include "wlan_cfg.h"
#include "wlan_mgr.h"
#include "../cfg/utility.h"
#include "../cfg/cfg_msg.h"
#include <cfg_msg.h>
#include <utility.h>
#include <ecnt_event_global/ecnt_event_global.h>
#include <libecntevent.h>
#include <ecnt_event_global/ecnt_event_dbus.h>
#include <ecnt_event_global/ecnt_event_wifi.h>
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif
#if defined(TCSUPPORT_ECNT_MAP)
#include "wlan_mesh.h"
#endif
#include "blapi_system.h"
#if defined(TCSUPPORT_CT_UBUS)
#include <notify.h>
#endif

extern int iswpsstart;
#if defined(TCSUPPORT_WPS_BTN_DUALBAND) || defined(TCSUPPORT_WPS_5G_BTN)
extern int iswps5gstart;
#endif
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
extern int wps_both_start;
extern int wps24g_done;
extern int wps5g_done;
#endif
extern int WscOOBSeted;
extern int WscOOBSeted_ac;

extern wlan_switch_cfg_t g_wlan_switch_cfg;
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
#define WPS_UUID_VALUE  "BC329E001DD811B2860100AABB280759"
#endif
int isBoot = 1;

#ifdef TCSUPPORT_WLAN_VENDIE
#define ECNT_BEACONVSIE_DBG_ENABLE "/tmp/ecnt_beaconvsie_dbg_enable"
#endif

#if defined(TCSUPPORT_CT_UBUS)
typedef struct notify_time_s {
	int start_inform;
	int first_set;
	int notify_time;
}notify_time_t;
notify_time_t gNotifyTime[MAX_DUALBAND_BSSID_NUM][MAX_STA_NUM];
#endif


static mac_table_t gMacTable[MAX_DUALBAND_BSSID_NUM];
static unsigned int needUpdateFlag = 0; /*ssid1: 1<<0; ssid2:1<<1;ssid3: 1<<2; ssid4:1<<3*/

TcWlanChannelMode	GVWlanChannelList[] =
{
	{"1", 	0},
	{"2", 	1},
	{"5.5",	2},
	{"11",	3},
	{"6",	0},
	{"9",	1},
	{"12",	2},
	{"18",	3},
	{"24",	4},
	{"36",	5},
	{"48",	6},
	{"54",	7},
	{"0",	33}
};

#define NUM_COUNTRY (sizeof(all_country)/sizeof(COUNTRY_NAME_TO_CODE))
COUNTRY_NAME_TO_CODE all_country[]=
{
	{"AL", "ALBANIA"	             },
	{"DZ", "ALGERIA"               },
	{"AR", "ARGENTINA"             },
	{"AM", "ARMENIA"               },
	//{"AW",  Aruba                },
	{"AU", "AUSTRALIA"             },
	{"AT", "AUSTRIA"               },
	{"AZ", "AZERBAIJAN"            },
  {"BH", "BAHRAIN"               },
	//{"BD", Bangladesh            },
	//{"BB",  Barbados             },
	{"BY", "BELARUS"               },
	{"BE", "BELGIUM"               },
	{"BZ", "BELIZE"                },
	{"BO", "BOLIVIA"               },
	//{"BA", Bosnia and Herzegovina},
	{"BR", "BRAZIL"                },
	{"BN", "BRUNEI DARUSSALAM"             },
	{"BG", "BULGARIA"              },
	//{"KH", Cambodia              },
	{"CA", "CANADA"                },
	{"CL", "CHILE"                 },
	{"CN", "CHINA"                 },
	{"CO", "COLOMBIA"              },
	{"CR", "COSTA RICA"             },
	{"HR", "CROATIA"               },
	{"CY", "CYPRUS"                },
	{"CZ", "CZECH REPUBLIC"             },
	{"DK", "DENMARK"               },
	{"DO", "DOMINICAN REPUBLIC"             },
	{"EC", "ECUADOR"               },
	{"EG", "EGYPT"                 },
	{"SV", "ELSALVADO"             },
	//{"EE",  Estonia              },
	{"FI", "FINLAND"               },
	{"FR", "FRANCE"                },
	{"GE", "GEORGIA"               },
	{"DE", "GERMANY"               },
	{"GR", "GREECE"                },
	//{"GL", Greenland             },  
	//{"GD", Grenada               },    
  //{"GU", Guam                  },    
	{"GT", "GUATEMALA"             },
	//{"HT", Haiti                 },    
	{"HN", "HONDURAS"              },
	{"HK", "HONGKONG"              },
	{"HU", "HUNGARY"               },
	{"IS", "ICELAND"               },
	{"IN", "INDIA"                 },
	{"ID", "INDONESIA"             },
	{"IR", "IRAN"                  },
	{"IE", "IRELAND"               },
	{"IL", "ISRAEL"                },
	{"IT", "ITALY"                 },
	//{"JM", Jamaica               },     
	{"JP", "JAPAN"                 },
	//{"JO", Jordan 	             },     
	{"KZ", "KAZAKHSTAN"             },
	//{"KE", Kenya 	               },     
	{"KP", "KOREA DEMOCRATIC"             },
	{"KR", "KOREA REPUBLIC"             },
	//{"KW", Kuwait 	             },  
	{"LV", "LATVIA"                },
	{"LB", "LEBANON"               },
	{"LI", "LIECHTENSTEIN"             },
	{"LT", "LITHUANIA"             },
	{"LU", "LUXEMBOURG"             },
	{"MO", "MACAU"                 },
	{"MK", "MACEDONIA"             },
	{"MY", "MALAYSIA"              },
	//{"MT", Malta 	               },  
	{"MX", "MEXICO"                },
	{"MC", "MONACO"                },
	{"MA", "MOROCCO"               },
	//{"NP", Nepal 	               },
	{"NL",  "NETHERLANDS" 	       },
	//{"AN", "NETHERLAN"             },
	{"NZ", "NEW ZEALAND"             },
	{"NO", "NORWAY"                },
	{"OM", "OMAN"                  },   
	{"PK", "PAKISTAN"              },    
	{"PA", "PANAMA"                }, 
	//{"PG", Papua New Guinea      },    
	{"PE", "PERU"                  },
	{"PH", "PHILIPPINES"             },
	{"PL", "POLAND"                },
	{"PT", "PORTUGAL"              },
	{"PR", "PUERTO RICO"             },
	{"QA", "QATAR"                 },
	{"RO", "ROMANIA"               },
	{"RU", "RUSSIA"                },
  //{"BL",  Saint Barth'elemy    },     
	{"SA", "SAUDI ARABIA"             },
	{"SG", "SINGAPORE"             },
	{"SK", "SLOVAKIA"              },
	{"SI", "SLOVENIA"              },
	{"ZA", "SOUTH AFRICA"             }, 	
	{"ES", "SPAIN"                 },
	//{"LK", Sri Lanka 	           },            
	{"SE", "SWEDEN"                },   		
	{"CH", "SWITZERLAND"             },  		
	{"SY", "SYRIAN ARAB REPUBLIC"	           },  			
	{"TW", "TAIWAN"      	         },
	{"TH", "THAILAND"    	         },  	
	{"TT", "TRINIDAD AND TOBAGO"     	       },  
	{"TN", "TUNISIA"    	         },  
	{"TR", "TURKEY"     	         },  	
	{"UA", "UKRAINE"    	         },  	
	{"AE", "UNITED ARAB EMIRATES"  	         },  			
	{"GB", "UNITED KINGDOM"    	       },  
	{"US", "UNITED STATES"    	       },  
	{"UY", "URUGUAY"    	         },  	
	{"UZ", "UZBEKISTAN"    	       },	
	{"VE", "VENEZUELA"    	       },  
	{"VN", "VIETNAM"    	         },  	
	{"YE", "YEMEN"                 },  			
	{"ZW", "ZIMBABWE"              },
	//{"EU",         	             },
	//{"NA", North America         },
	{"WO", "Undefined"	           }
};

char svc_wlan_common_path[SVC_WLAN_PATH_SIZE] 	= {0};
char svc_wlan_entry_path[SVC_WLAN_PATH_SIZE] 	= {0};
char svc_wlan_apon_path[SVC_WLAN_PATH_SIZE] 	= {0};
char svc_wlan_name[SVC_WLAN_PATH_SIZE] 			= {0};
char svc_wlan_script_path[SVC_WLAN_PATH_SIZE]	= {0};
char svc_wlan_ra_name[SVC_WLAN_PATH_SIZE]		= {0};
char svc_wlan_script_prefix[SVC_WLAN_PATH_SIZE]	= {0};
char svc_wlan_wds_name[SVC_WLAN_PATH_SIZE] 		= {0};

int type = -1;/*wifi type, 0:2.4G or  1:5G*/


char * WscGetAuthTypeStr(unsigned short authFlag)
{
	switch(authFlag)
	{
		case WSC_AUTHTYPE_OPEN:
			return "OPEN";
        default:
		case WSC_AUTHTYPE_WPAPSK:
			return "WPAPSK";
		case WSC_AUTHTYPE_SHARED:
			return "SHARED";
		case WSC_AUTHTYPE_WPANONE:
			return "WPANONE";
		case WSC_AUTHTYPE_WPA:
			return "WPA";
		case WSC_AUTHTYPE_WPA2:
			return "WPA2";
		case WSC_AUTHTYPE_WPA1WPA2:
			return "WPA1WPA2";
		case (WSC_AUTHTYPE_WPAPSK | WSC_AUTHTYPE_WPA2PSK):
		case WSC_AUTHTYPE_WPA2PSK:
			return "WPA2PSK";
#if defined(TCSUPPORT_CT_WLAN_WAPI)
		case WSC_AUTHTYPE_WAPI_CERT:
			return "WAICERT";
		case WSC_AUTHTYPE_WAPI_PSK:
			return "WAI-PSK";			
#endif
	}
}

char*  WscGetEncryTypeStr(unsigned short encryFlag)
{
	switch(encryFlag)
	{
		case WSC_ENCRTYPE_NONE:
			return "NONE";
		case WSC_ENCRTYPE_WEP:
			return "WEP";
        default:
		case WSC_ENCRTYPE_TKIP:
			return "TKIP";
		case (WSC_ENCRTYPE_TKIP | WSC_ENCRTYPE_AES):
		case WSC_ENCRTYPE_AES:
			return "AES";
#if defined(TCSUPPORT_CT_WLAN_WAPI)
		case WSC_ENCRTYPE_SMS4:
			return "SMS4";
#endif
	}
}

#ifdef WSC_AP_SUPPORT
int isWPSRunning(void)
{
  	char tmp[160] 		= {0};
	char node_name[128] = {0};
	int isWPSrunning	= 0;

#ifdef TCSUPPORT_WLAN_MULTI_WPS
	char APOn[4]		= {0};
	char restartWPS[4]	= {0};
	char Bssid_Num[4]	= {0};
	char tmp2[128]		= {0};
	int i;

	if((cfg_get_object_attr(svc_wlan_common_path, "APOn", APOn, sizeof(APOn)) < 0)
		|| (cfg_get_object_attr(svc_wlan_common_path, "BssidNum", Bssid_Num, 
												sizeof(Bssid_Num)) < 0)){
		return isWPSrunning;
	}

	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "root.Info.%s", svc_wlan_name);
	for(i = 0; i < atoi(Bssid_Num); i++){		
		snprintf(tmp2, sizeof(tmp2), "wlanWPStimerRunning_%d", i);					
		if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) >= 0){
			if(1 == atoi(tmp)){				
				memset(node_name, 0, sizeof(node_name));
				snprintf(node_name, sizeof(node_name), "%s.%d", svc_wlan_entry_path, i + 1);
				if((cfg_get_object_attr(node_name, "restartWPS", restartWPS, sizeof(restartWPS)) >= 0)
					&& 1 == atoi(restartWPS)){
					return isWPSrunning;
				}
				
				isWPSrunning = 1;
				break;
			}
		}
	}
#else	
	snprintf(node_name, sizeof(node_name), "root.Info.%s", svc_wlan_name);
	if(cfg_get_object_attr(node_name, "wlanWPStimerRunning", tmp, sizeof(tmp)) > 0){
		if(1 == atoi(tmp)){
			isWPSrunning = 1;
		}
	}	
#endif
	return isWPSrunning;
}


int run_wps(void)
{
	char tmp[4]			= {0};
	char node_name[128]	= {0};
#ifdef TCSUPPORT_WLAN_MULTI_WPS
	char pincode[16]	= {0};
#endif
	char cmd[50] 	= {0};
	FILE *fp 		= NULL;
#ifdef TCSUPPORT_WLAN_MULTI_WPS
	char APOn[4]		= {0};
	char Bssid_Num[4]	= {0};
	int i 				= 0;
	int wlan_id 		= 0;
	int res = 0;
	char cmd_value[64] = {0};

	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	if(0 == type){
		if(cfg_get_object_attr(node_name, "wlan_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else if(1 == type){
		if(cfg_get_object_attr(node_name, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else{
		printf("The wifi type is invalid.\n");
		return 0;
	}
	if((cfg_get_object_attr(svc_wlan_common_path, "APOn", APOn, sizeof(APOn)) < 0)
	|| (cfg_get_object_attr(svc_wlan_common_path, "BssidNum", Bssid_Num, sizeof(Bssid_Num)) < 0)){
		return 0;
	}	
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "root.Info.%s", svc_wlan_name);
	if(cfg_get_object_attr(node_name, "WPSActiveStatus", tmp, sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0){/* no active */
			printf("WPSActiveStatus = 0\n");
		}
		else{ 
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "%s.%d", svc_wlan_entry_path, wlan_id+1);					
			if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) < 0){
				printf("WPSConfMode = NULL\n");
			}
			else if((strcmp(tmp, "1") == 0)||(strcmp(tmp, "2") == 0 )
											||(strcmp(tmp, "7") == 0)){
				fp = fopen(svc_wlan_script_path, "w");
				if(fp != NULL){	
					if(isWPSRunning()){								
						for(i = 0; i < atoi(Bssid_Num); i++){		
							wifimgr_lib_set_WIFI_CMD(svc_wlan_ra_name, i, sizeof(cmd_value), "WscStop\n", cmd_value, 0);
							fprintf(fp, cmd_value);	
						}
						if(WIFI_2_4G == type){
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
						}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
							snprintf(cmd, sizeof(cmd), "%d\n", WPS5GSTOP);
							wifimgr_lib_set_WPS_5G_BUTTON(cmd);
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
#endif
#endif
						}												
					}else{
						if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) > 0){
							fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscConfMode", tmp);/* set wsc conf mode */
						}
						if(cfg_get_object_attr(node_name, "WPSConfStatus", tmp, sizeof(tmp)) > 0){
							if(strcmp(tmp, "1") == 0){
								fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscConfStatus", "1");
							}else if(strcmp(tmp, "2") == 0){
								fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscConfStatus", "2");
							}else{
								printf("error in WscConfStatus!!!\n\n");
								fclose(fp);
								return 0;
							}
						}
						else{
							fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscConfStatus", "1");/*unconfigured*/
						}
						if(cfg_get_object_attr(node_name, "WPSMode", tmp, sizeof(tmp)) > 0){
							if(strcmp(tmp, "0") == 0)/* Pin code */{
								fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscMode", "1");
								if(cfg_get_object_attr(node_name, "enrolleePinCode", pincode, sizeof(pincode)) > 0){
									fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscPinCode", pincode);
								}
									
								/* set genPinCode */
								if(cfg_get_object_attr(node_name, "genPinCodeSupport", tmp, sizeof(tmp)) > 0){
									if (strcmp(tmp, "Yes") == 0){
										fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscGenPinCode", tmp);
									}	
								}						
							}
							else if(strcmp(tmp, "1") == 0)/*  PBC  */{
								fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscMode", "2");
							}
						}
						fprintf(fp, svc_wlan_script_prefix, wlan_id, "WscGetConf", "1");/* start WPS */
						if(WIFI_2_4G == type){
							iswpsstart = 1;
						}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
							res = wifimgr_lib_set_iswps5gstart_val(&iswps5gstart, 1);
#endif
						}
					}
					fclose(fp);
					res = chmod(svc_wlan_script_path, S_IRUSR|S_IWUSR|S_IXUSR);
					return 1;/* wlan execute will only run wps*/
				}
				else{
					printf("can't open file \n");
				}
			}
			else{
				printf("WPSConfMode is wrong!!!\n");
			}
		}
	}
	else{
		printf("WPSActiveStatus = NULL\n");
	}
#else
	snprintf(node_name, sizeof(node_name), "root.Info.%s", svc_wlan_name);
	if(cfg_get_object_attr(node_name, "WPSActiveStatus", tmp, sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0){/*  no active  */
			printf("WPSActiveStatus = 0\n");
		}
		else{ 
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "root.%s.Entry.%d", svc_wlan_name,wlan_id+1);
			if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, 4) < 0){
				printf("WPSConfMode = NULL\n");
			}
			else if((strcmp(tmp, "1") == 0)||(strcmp(tmp, "2") == 0)
											||(strcmp(tmp, "7") == 0)){
				fp = fopen(svc_wlan_script_path,"w");
				if(fp != NULL){
						if(isWPSRunning()){
							wifimgr_lib_set_WIFI_CMD(svc_wlan_ra_name, 0, sizeof(cmd_value), "WscStop\n", cmd_value, 0);
							fprintf(fp, cmd_value);
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
						}else{
							if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) > 0){
								fprintf(fp, svc_wlan_script_prefix, 0, "WscConfMode", sizeof(tmp));/* set wsc conf mode */
							}
							
							if(cfg_get_object_attr(node_name, "WPSConfStatus", tmp, 4) >= 0){
								if(strcmp(tmp, "1") == 0){
									fprintf(fp, svc_wlan_script_prefix, 0, "WscConfStatus", "1");
								}else if(strcmp(tmp, "2") == 0){
									fprintf(fp, svc_wlan_script_prefix, 0, "WscConfStatus", "2");
								}else{
									printf("error in WscConfStatus!!!\n\n");
									fclose(fp);
									return 0;
								}
							}
							else{
								fprintf(fp, svc_wlan_script_prefix, 0, "WscConfStatus", "1");/*unconfigured*/
							}
							if(cfg_get_object_attr(node_name, "WPSMode", tmp, sizeof(tmp)) > 0){
								if(strcmp(tmp, "0") == 0)/* Pin code */{
									fprintf(fp, svc_wlan_script_prefix, 0, "WscMode", "1");
									if(cfg_get_object_attr(node_name, "enrolleePinCode", tmp, sizeof(tmp)) > 0){
										fprintf(fp, svc_wlan_script_prefix, 0, "WscPinCode", tmp);
									}
									
									/* set genPinCode */
									if(cfg_get_object_attr(node_name, "genPinCodeSupport", tmp, sizeof(tmp)) > 0){
										if (strcmp(tmp, "Yes")==0){
											fprintf(fp, svc_wlan_script_prefix, 0, "WscGenPinCode", tmp);
										}		
									}				
								}
								else if(strcmp(tmp, "1") == 0)/* PBC */{
									fprintf(fp, svc_wlan_script_prefix, 0, "WscMode", "2");
								}
							}
							fprintf(fp, svc_wlan_script_prefix, 0, "WscGetConf", "1");/* start WPS */
							iswpsstart = 1;
						}
						fclose(fp);
						res = chmod(svc_wlan_script_path,S_IRUSR|S_IWUSR|S_IXUSR);
						return 1;/* wlan execute will only run wps*/
					}
					else{
						printf("can't open file \n");
					}
				}
				else{
					printf("WPSConfMode is wrong!!!\n");
				}
			}
		}
	else{
		printf("WPSActiveStatus = NULL\n");
	}
#endif	
	return 0;
}


int wps_oob(void)
{
	char tmp[4]			= {0};
	char node_name[128]	= {0};
	FILE *fp			= NULL;
	char cmd[50]		= {0};
	int res = 0;
	int wlan_id 		= 0;
	char cmd_value[64]  = {0};

	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	if(0 == type){
		if(cfg_get_object_attr(node_name, "wlan_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else if(1 == type){
		if(cfg_get_object_attr(node_name, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else{
		tcdbg_printf("The wifi type is invalid.\n");
		return 0;
	}

	snprintf(node_name, sizeof(node_name), "root.Info.%s", svc_wlan_name);
	if(cfg_get_object_attr(node_name, "WPSOOBActive", tmp, sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0){/* no active */
			printf("WPSOOBActive = 0\n");
		}
		else{ 
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "root.%s.Entry.%d", svc_wlan_name,wlan_id+1);
			if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) < 0){
				printf("WPSConfMode = NULL\n");
			}
			else if((strcmp(tmp, "1") == 0)||(strcmp(tmp, "2") == 0)
											||(strcmp(tmp, "7") == 0)){
				fp = fopen(svc_wlan_script_path, "w");
				if(fp != NULL){
					if(isWPSRunning()){
						wifimgr_lib_set_WIFI_CMD(svc_wlan_ra_name, wlan_id, sizeof(cmd_value), "WscStop\n", cmd_value, 0);
						fprintf(fp, cmd_value);
						if(WIFI_2_4G == type){
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
						}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
							snprintf(cmd, sizeof(cmd), "%d\n", WPS5GSTOP);
							wifimgr_lib_set_WPS_5G_BUTTON(cmd);
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
#endif					
#endif									
						}	
					}
					else
					{
						wifimgr_lib_set_WIFI_CMD(svc_wlan_ra_name, wlan_id, sizeof(cmd_value), "WscOOB=1\n", cmd_value, 0);
						fprintf(fp, cmd_value);
						
						cfg_set_object_attr(node_name, "WPSConfStatus", "1");
						if ( WIFI_2_4G == type)
						{
							WscOOBSeted = 1;
						}
						else if ( WIFI_5G == type )
						{
							WscOOBSeted_ac = 1;
						}
					}
					fclose(fp);
					res = chmod(svc_wlan_script_path, S_IRUSR|S_IWUSR|S_IXUSR);
					return 1;
				}
				else{
					printf("can't open file in wps_oob()\n");
				}
			}
			else{
				printf("WPSConfMode is wrong!!!\n");
			}
		}
	}
	else{
		printf("WPSOOBActive = NULL\n");
	}
	return 0;
}

int wps_genpincode(void)
{
	char tmp[4]			= {0};
	char node_name[128]	= {0};
	FILE *fp			= NULL;
	char cmd[50]		= {0};
	int res = 0;
	int wlan_id 		= 0;
	char cmd_value[64]  = {0};

	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	if(0 == type){
		if(cfg_get_object_attr(node_name, "wlan_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else if(1 == type){
		if(cfg_get_object_attr(node_name, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else{
		tcdbg_printf("The wifi type is invalid.\n");
		return 0;
	}


	snprintf(node_name, sizeof(node_name), "root.Info.%s", svc_wlan_name);
	if(cfg_get_object_attr(node_name, "WPSGenPinCode", tmp, sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0){/* no active */
			printf("WPSGenPinCode = 0\n");
		}
		else{ 
			memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), "root.%s.Entry.%d", svc_wlan_name,wlan_id+1);
			if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) < 0){
				printf("WPSConfMode = NULL\n");
			}
			else if((strcmp(tmp, "1") == 0)||(strcmp(tmp, "2") == 0)||(strcmp(tmp, "7") == 0)){
				fp = fopen(svc_wlan_script_path, "w");
				if(fp != NULL){
					if(isWPSRunning()){
						wifimgr_lib_set_WIFI_CMD(svc_wlan_ra_name, wlan_id, sizeof(cmd_value), "WscStop\n", cmd_value, 0);
						fprintf(fp, cmd_value);
						if(WIFI_2_4G == type){
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
						}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
							snprintf(cmd, sizeof(cmd), "%d\n", WPS5GSTOP);
							wifimgr_lib_set_WPS_5G_BUTTON(cmd);
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
							snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
							wifimgr_lib_set_WPS_BUTTON(cmd);
#endif
#endif
						}
					}else{
						wifimgr_lib_set_WIFI_CMD(svc_wlan_ra_name, wlan_id, sizeof(cmd_value), "WscGenPinCode=1\n", cmd_value, 0);
						fprintf(fp, cmd_value);
					}
					fclose(fp);
					res = chmod(svc_wlan_script_path, S_IRUSR|S_IWUSR|S_IXUSR);
					return 1;
				}
				else{
					printf("can't open file in %s\n", __FUNCTION__);
				}
			}
			else{
				printf("WPSConfMode is wrong!!!\n");
			}
		}
	}
	else{
		printf("WPSGenPinCode = NULL\n");
	}
	return 0;
}
#endif

/*check if wireless mode is 11n*/ 
static int Is11nWirelessMode(void)
{
	int is11Nmode		= 0;
  	char  tmp[160] 		= {0};
	char node_name[128] = {0};

	snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
	if(cfg_get_object_attr(node_name, "WirelessMode", tmp, sizeof(tmp)) > 0){
		if((atoi(tmp)==PHY_MODE_N_ONLY)||(atoi(tmp)==PHY_MODE_GN_MIXED)
									|| (atoi(tmp)==PHY_MODE_BGN_MIXED) || (atoi(tmp)==PHY_MODE_GNAX_MIXED)){
			is11Nmode=1;
		}
	}
	return is11Nmode;
}

#if defined(TCSUPPORT_WLAN_AC)
int reset_ac_counter(void)
{
	char node_name[128] = {0};
	char tmp[4]			= {0};
	FILE *fp 			= NULL;
	int res = 0;
	char cmd_value[64]  = {0};

	snprintf(node_name, sizeof(node_name), INFO_WLAN11AC_NODE);
	if(cfg_get_object_attr(node_name, "ReCounterActive", tmp, sizeof(tmp)) > 0){
		if(strcmp(tmp, "0") == 0){/* no active */
			printf("ReCounterActive = 0\n");
		}
		else{
			fp = fopen(svc_wlan_script_path, "w");
			if(fp != NULL){					
				wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd_value), "ResetCounter=1\n", cmd_value, 0);
				fprintf(fp, cmd_value);
				printf("dgk:set ResetCounter = 1 in %s\n", __FUNCTION__);
				fclose(fp);
				res = chmod(svc_wlan_script_path, S_IRUSR|S_IWUSR|S_IXUSR);
				return 1;
			}
			else{
				printf("can't open file in reset_counter()\n");
			}
		}
	}
	else{
		printf("ReCounterActive = NULL\n");
	}
	return 0;
}

int txBurst_or_not_ac(int BssidNum)
{
	int txBurst	= 1;
  	char nodeName[128] 	= {0};
	char tmp[160]		= {0};
	int i				= 0;

	for(i = 0; i < BssidNum; i++){
		snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN11AC_ENTRY_NODE, i + 1);
		if(cfg_get_object_attr(nodeName, "WMM", tmp, sizeof(tmp)) > 0){
			if(strcmp(tmp, "1") == 0){
				txBurst=0;
				break;
			}
		}
	}

	return txBurst;
}

/* check if wireless mode is 11ac */
static int	Is11acWirelessMode(void)
{
	int is11acmode		= 0;
  	char node_name[128] = {0};
	char tmp[160] 		= {0};
	
	snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
	if(cfg_get_object_attr(node_name, "WirelessMode", tmp, sizeof(tmp)) > 0){
		if((atoi(tmp)==PHY_11VHT_N_A_MIXED) || (atoi(tmp)==PHY_11VHT_N_MIXED)
			#if defined(TCSUPPORT_WLAN_AX)
			|| (atoi(tmp)==PHY_MODE_N_AC_AX_MIX)
			#endif
			){
			is11acmode=1;
		}
	}
	return is11acmode;
}
#endif


/*check if wmm enabled or not, txburst should disable when wmm enable.*/
static int txBurst_or_not(int Bssid_Num)
{
	int i=0, txBurst=1;
  	char node_name[128] = {0};
	char tmp[160] = {0};
	
	for(i = 0; i < Bssid_Num; i++){
		snprintf(node_name, sizeof(node_name), "%s.%d", svc_wlan_entry_path, i+1);
		if(cfg_get_object_attr(node_name, "WMM", tmp, sizeof(tmp)) > 0){
			if(strcmp(tmp, "1") == 0){
				txBurst=0;
				break;
			}
		}
	}
	return txBurst;
}
#ifdef TCSUPPORT_WPS_BTN_DUALBAND
int WscGenerateUUID(unsigned char *uuidHexStr,unsigned char *uuidAscStr)
{
	WSC_UUID_T uuid_t;
	unsigned long long uuid_time;
	int i;
	unsigned short clkSeq;
	char macaddr[80] = {0};

	memset(uuid_t.node, 0 ,sizeof(uuid_t.node));
	getWanInfo(Mac_type, "br0", macaddr);
	for(i = 0; i < 6; i++){
		uuid_t.node[2*i] = macaddr[3*i];
		uuid_t.node[2*i+1] = macaddr[3*i+1];
	}	

	/* Get the current time. */

	uuid_time = 2860; /*xtime.tv_sec;	// Well, we fix this to make JumpStart  happy! */
	uuid_time *= 10000000;
	uuid_time += 0x01b21dd213814000LL;
	uuid_t.timeLow = (unsigned int)uuid_time & 0xFFFFFFFF;
	uuid_t.timeMid = (unsigned short)((uuid_time >> 32) & 0xFFFF);
	uuid_t.timeHi_Version = (unsigned short)((uuid_time >> 48) & 0x0FFF);
	uuid_t.timeHi_Version |= (1 << 12);
	/* Get the clock sequence. */
	clkSeq = (unsigned short)(0x0601/*jiffies*/ & 0xFFFF);		/* Again, we fix this to make JumpStart happy! */
	uuid_t.clockSeqLow = clkSeq & 0xFF;
	uuid_t.clockSeqHi_Var = (clkSeq & 0x3F00) >> 8;
	uuid_t.clockSeqHi_Var |= 0x80;


	sprintf(uuidHexStr, "%08x%04x%04x%02x%02x%s", 
			uuid_t.timeLow, uuid_t.timeMid, uuid_t.timeHi_Version, uuid_t.clockSeqHi_Var, uuid_t.clockSeqLow, 
			uuid_t.node);
	sprintf(uuidAscStr, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
			uuid_t.timeLow, uuid_t.timeMid, uuid_t.timeHi_Version, uuid_t.clockSeqHi_Var, uuid_t.clockSeqLow, 
			uuid_t.node[0], uuid_t.node[1], uuid_t.node[2], uuid_t.node[3], uuid_t.node[4], uuid_t.node[5]);
	
	return 0;
}
#endif

int multi_profile_merge_global_settings(int type)  
{
	char src_nodeName[128]={0};
	char dst_nodeName[128]={0};
	int i = 0;
	char tmp[160] 			= {0}; 

	FILE *fp = NULL;	
	char *dst = NULL;

	wifi_attr_fun wlan_global_script_com_attr[64] = {0};
	int ret = 0;
	
	memset(wlan_global_script_com_attr, 0, 64 * sizeof(wifi_attr_fun));
	ret = wifimgr_lib_get_wlan_attr(2, wlan_global_script_com_attr);
	
	memset(src_nodeName,0,sizeof(src_nodeName));	
	memset(dst_nodeName,0,sizeof(dst_nodeName));
	
	if (type == WIFI_2_4G)    //to modify 11AC node 
	{
		if(wifimgr_lib_init(1) != 0)
		{
			tcdbg_printf("Create 11ac shared memory of data file fail.\n");
			return CFG2_RET_FAIL;
		}	
		
		//dst = WLAN_AC_PATH;
		snprintf(src_nodeName, sizeof(src_nodeName), WLAN_COMMON_NODE);	
		snprintf(dst_nodeName, sizeof(dst_nodeName), WLAN11AC_COMMON_NODE);		
	}	
	else         			//to modify 11n node               
	{		
		if(wifimgr_lib_init(0) != 0)
		{
			tcdbg_printf("Create 11ac shared memory of data file fail.\n");
			return CFG2_RET_FAIL;
		}	
		//dst = WLAN_PATH;	
		snprintf(src_nodeName, sizeof(src_nodeName), WLAN11AC_COMMON_NODE);	
		snprintf(dst_nodeName, sizeof(dst_nodeName), WLAN_COMMON_NODE);			
	}	

	for(i = 0;strlen(wlan_global_script_com_attr[i].attr_name) != 0;i++)	
	{					
		if(strcmp(wlan_global_script_com_attr[i].attr_name, "AutoChannelSelect") == 0)
		{
			if (type == WIFI_2_4G)
			{	
				if(wifimgr_lib_set_autoChannelSelect(1, -1, "3") != 0)
				{
					tcdbg_printf("write_wlan11ac_config:write %s to data file fail.\n", 
							wlan_global_script_com_attr[i].attr_name);
					return CFG2_RET_FAIL;
				}
			}
			else
			{						
				if(wifimgr_lib_set_autoChannelSelect(0, -1, "3") != 0)
				{
					tcdbg_printf("write_wlan11ac_config:write %s to data file fail.\n", 
							wlan_global_script_com_attr[i].attr_name);
					return CFG2_RET_FAIL;
				}
			}						
		}
		
		if(cfg_get_object_attr(src_nodeName, (char *)wlan_global_script_com_attr[i].attr_name,
														tmp, sizeof(tmp)) > 0)
		{
			if(strlen(tmp) > 0)
			{
				cfg_set_object_attr(dst_nodeName, (char *)wlan_global_script_com_attr[i].attr_name, tmp);
				if(wlan_global_script_com_attr[i].wifi_api != NULL)
				{
					if (type == WIFI_2_4G)
					{						
						if ((wlan_global_script_com_attr[i].wifi_api)(1, -1, tmp) != 0)
						{
							tcdbg_printf("write_wlan11ac_config:write %s to data file fail.\n", 
									wlan_global_script_com_attr[i].attr_name);
							return CFG2_RET_FAIL;
						}
					}
					else
					{						
						if ((wlan_global_script_com_attr[i].wifi_api)(0, -1, tmp) != 0)
						{
							tcdbg_printf("write_wlan11ac_config:write %s to data file fail.\n", 
									wlan_global_script_com_attr[i].attr_name);
							return CFG2_RET_FAIL;
						}
					}						
				}				
			}
		}
	}
	return ret;
}

int write_wlan_config(char* path, int Bssid_Num)
{
	int i = 0;
	int j = 0;
	char node_name[128] = {0}; 
	char tmp[160] 		= {0}; 
#ifdef TCSUPPORT_WPS_BTN_DUALBAND
	char tmp_s[160] 			= {0}; 
#endif
	char tmp2[160] 		= {0};
	char buf[160] 		= {0};
	char strWirelessMode[8] = {0};
	char strRate[32] 		= {0};	
	char encrytype[16] 		= {0};
#if defined(TCSUPPORT_WLAN_AX)
	char MuOfdmaDlEnable[5] = {0};
	char MuOfdmaUlEnable[5] = {0};
	char MuMimoDlEnable[5] = {0};
	char MuMimoUlEnable[5] = {0};
	char TWTSupportEnable[16] = {0};
#endif

	wifi_attr_fun wlan_common_default_parm[64] = {0};
	wifi_attr_fun wlan_entry_default_parm[32] = {0};
	wifi_attr_fun wlan_script_com_attr[64] = {0};
	int ret = 0;

	memset(wlan_common_default_parm, 0, 64 * sizeof(wifi_attr_fun));
	ret = wifimgr_lib_get_wlan_attr(0, wlan_common_default_parm);
	
	memset(wlan_entry_default_parm, 0, 32 * sizeof(wifi_attr_fun));
	wifimgr_lib_get_wlan_entry_attr(0, wlan_entry_default_parm);

	memset(wlan_script_com_attr, 0, 64 * sizeof(wifi_attr_fun));
	wifimgr_lib_get_wlan_script_com_attr_config(0, wlan_script_com_attr);
#if 0	
	if(wifimgr_lib_set_dbg_level("15") != 0)
		printf("open BSP wifi debug fail.\n");
	else
		printf("open BSP wifi debug success.\n");
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
			pre_sys_state_init();
#endif

	if(wifimgr_lib_init(0) != 0){
		printf("Create shared memory of data file fail.\n");
		return CFG2_RET_FAIL;
	}
	
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);

	if(cfg_get_object_attr(node_name, "Country", tmp, sizeof(tmp)) > 0){
		for(i = 0; i < NUM_COUNTRY; i++){	
			if(strcmp((char*)all_country[i].pCountryName,tmp)==0){
				if(wifimgr_lib_set_countryCode(0, -1, (char*)all_country[i].IsoName) != 0){
					printf("write_wlan_config:wifimgr_lib_set_countryCode fail.\n");
				}
				break;
			}	
		}
	}


	/*write cfg node value to RT2860AP.dat, instead of default value.  */
	for(i = 0; strlen(wlan_common_default_parm[i].attr_name) != 0; i++){	
		if(cfg_get_object_attr(node_name, wlan_common_default_parm[i].attr_name, tmp, 
																sizeof(tmp)) > 0){
			if (strcmp(wlan_common_default_parm[i].attr_name, "TxBurst") == 0){
				if(txBurst_or_not(Bssid_Num) == 1){
					if((wlan_common_default_parm[i].wifi_api)(0, -1, "1") < 0){
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
					}
				}else{
					if((wlan_common_default_parm[i].wifi_api)(0, -1, "0") < 0){
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
					}
				}	
			}
			else if ( 0 == strcmp(wlan_common_default_parm[i].attr_name, "WHNAT") )
			{
				if ( (wlan_common_default_parm[i].wifi_api)(0, -1, tmp) < 0 )
				{
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
				}
			}
			else{
				if((wlan_common_default_parm[i].wifi_api == NULL) ||
					((wlan_common_default_parm[i].wifi_api)(0, -1, tmp) != 0)){
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
				}
			}
		}
	}
	
	/*write common setting to RT2860AP.dat */
	if(cfg_get_object_attr(node_name, "WirelessMode", strWirelessMode, 
										sizeof(strWirelessMode)) > 0){
		if(wifimgr_lib_set_wirelessMode(0, -1, strWirelessMode) != 0){
			printf("write_wlan_config:wifimgr_lib_set_wirelessMode fail.\n");
		} 
		if((atoi(strWirelessMode) == PHY_MODE_N_ONLY)||(atoi(strWirelessMode) == PHY_MODE_GN_MIXED)
			||(atoi(strWirelessMode) == PHY_MODE_BGN_MIXED) || (atoi(strWirelessMode) == PHY_MODE_GNAX_MIXED)){
			cfg_set_object_attr(node_name, "11nMode", "1");
		}else{
			cfg_set_object_attr(node_name, "11nMode", "0");
		}
	}

	/*wlan_script_com_attr*/
	for(i = 0; strlen(wlan_script_com_attr[i].attr_name) != 0; i++){
		if(strcmp(wlan_script_com_attr[i].attr_name, "AutoChannelSelect") == 0){
			if(cfg_get_object_attr(node_name, "Channel", tmp, sizeof(tmp)) > 0){
				if(atoi(tmp) == 0){/*Auto Channel*/ 
					if(wifimgr_lib_set_autoChannelSelect(0, -1, "3") != 0){
						printf("write_wlan_config:wifimgr_lib_set_autoChannelSelect fail.\n");
					}	
				}else{
					if(wifimgr_lib_set_autoChannelSelect(0, -1, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_autoChannelSelect fail.\n");
					}
				}
			}
		}
		
#if 0
		if(strcmp(wlan_script_com_attr[i], "AutoChannelSkipList") == 0)
		{
			if(cfg_get_object_attr(node_name, "AutoChannelSkipList", tmp, sizeof(tmp)) 
																	== TCAPI_PROCESS_OK)
			{				
				sprintf(buf,"%s=%s\n", wlan_script_com_attr[i],tmp);
			}
		}
#endif

		else if(strcmp(wlan_script_com_attr[i].attr_name, "BasicRate") == 0){	
			switch(atoi(strWirelessMode)){
				case PHY_MODE_BG_MIXED:
				case PHY_MODE_BGN_MIXED:
				case PHY_MODE_N_ONLY:
				case PHY_MODE_GNAX_MIXED:
					if(wifimgr_lib_set_basicRate(0, -1, "15") != 0){
						printf("write_wlan_config:wifimgr_lib_set_basicRate fail.\n");
					}
					break;
				case PHY_MODE_B_ONLY:
					if(wifimgr_lib_set_basicRate(0, -1, "3") != 0){
						printf("write_wlan_config:wifimgr_lib_set_basicRate fail.\n");
					}
					break;
				case PHY_MODE_G_ONLY:
				case PHY_MODE_GN_MIXED:
					if(wifimgr_lib_set_basicRate(0, -1, "351") != 0){
						printf("write_wlan_config:wifimgr_lib_set_basicRate fail.\n");
					}
					break;
				default:
					printf("write_wlan_config:WLan wireless mode is wrong when set BasicRate!(3390,3090)\n");
					if(wifimgr_lib_commit(0, 1) != 0){
						printf("Save the data of shared memory to data file fail.\n");	
					}
					return -1;
			}
		}

		else if(strcmp(wlan_script_com_attr[i].attr_name, "HT_BW") == 0){
			if(Is11nWirelessMode()){
				if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name, 
																	tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_HT_BW(0, -1, tmp) != 0){
						printf("write_wlan_config:wifimgr_lib_set_HT_BW fail.\n");
					}
				}
			}
			else{
				if(wifimgr_lib_set_HT_BW(0, -1, "0") != 0){
					printf("write_wlan_config:wifimgr_lib_set_HT_BW fail.\n");
				}
			}
			
		}
		
		else if(strcmp(wlan_script_com_attr[i].attr_name, "HT_EXTCHA") == 0){
			cfg_set_object_attr(node_name, "HT_EXTCHA", "0");
			if(wifimgr_lib_set_HT_EXTCHA(0, -1, "0") != 0){
				printf("write_wlan_config:wifimgr_lib_set_HT_EXTCHA fail.\n");
			}
			if(Is11nWirelessMode()){
				if(cfg_get_object_attr(node_name, "HT_BW", tmp, sizeof(tmp)) >= 0){
					if(atoi(tmp) == 1){
						memset(tmp, 0, sizeof(tmp));
						if(cfg_get_object_attr(node_name, "Channel", tmp, sizeof(tmp)) > 0){
							if(atoi(tmp) <= 4){
								cfg_set_object_attr(node_name, "HT_EXTCHA", "1");
								if(wifimgr_lib_set_HT_EXTCHA(0, -1, "1") != 0){
									printf("write_wlan_config:wifimgr_lib_set_HT_EXTCHA fail.\n");
								}
							}
							else if(atoi(tmp) >= 5 && atoi(tmp) <= 7){
								cfg_get_object_attr(node_name, "HT_EXTCHA", tmp, sizeof(tmp));
								if(wifimgr_lib_set_HT_EXTCHA(0, -1, tmp) != 0){
									printf("write_wlan_config:wifimgr_lib_set_HT_EXTCHA fail.\n");
								}
							}
						}
					}
				}
			}
		}
		
		else if(strcmp(wlan_script_com_attr[i].attr_name, "HT_OpMode") == 0){		
			switch(atoi(strWirelessMode)){
				case PHY_MODE_BGN_MIXED:
				case PHY_MODE_GN_MIXED:
				case PHY_MODE_BG_MIXED:
				case PHY_MODE_B_ONLY:
				case PHY_MODE_GNAX_MIXED:
					if(wifimgr_lib_set_HT_OpMode(0, -1, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_HT_OpMode fail.\n");
					}
					break;
				case PHY_MODE_N_ONLY:
				case PHY_MODE_G_ONLY:
					if(wifimgr_lib_set_HT_OpMode(0, -1, "1") != 0){
						printf("write_wlan_config:wifimgr_lib_set_HT_OpMode fail.\n");
					}
					break;
				default:
					printf("write_wlan_config:WLan wireless mode is not right when set HT_OpMode!\n");
					return -1;
			}
		}

		
		else{
			if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name,
															tmp, sizeof(tmp)) > 0){
				if(strlen(tmp) > 0){
					if((wlan_script_com_attr[i].wifi_api == NULL) ||
					((wlan_script_com_attr[i].wifi_api)(0, -1, tmp) != 0)){
						printf("write_wlan_config:write %s to data file fail.\n", 
												wlan_script_com_attr[i].attr_name);
					}
				}
			}
		}
	}
#if defined(TCSUPPORT_CT_WLAN_NODE)
	if(cfg_get_object_attr(node_name, "TxPowerLevel", tmp, sizeof(tmp)) > 0){
		switch(atoi(tmp)){
		case TX_POWER_LEVLE_1:
			if(wifimgr_lib_set_txPower(0, -1, "75") != 0){//100
				printf("write_wlan_config:wifimgr_lib_set_txPower fail.\n");
			}
			break;
		case TX_POWER_LEVLE_2:
			if(wifimgr_lib_set_txPower(0, -1, "50") != 0){//75
				printf("write_wlan_config:wifimgr_lib_set_txPower fail.\n");
			}
			break;
		case TX_POWER_LEVLE_3:
			if(wifimgr_lib_set_txPower(0, -1, "25") != 0){//50
				printf("write_wlan_config:wifimgr_lib_set_txPower fail.\n");
			}
			break;
		case TX_POWER_LEVLE_4:
			if(wifimgr_lib_set_txPower(0, -1, "15") != 0){//25
				printf("write_wlan_config:wifimgr_lib_set_txPower fail.\n");
			}
			break;
		case TX_POWER_LEVLE_5:
			if(wifimgr_lib_set_txPower(0, -1, "9") != 0){//15
				printf("write_wlan_config:wifimgr_lib_set_txPower fail.\n");
			}
			break;
		case TX_POWER_LEVLE_6:
			if(wifimgr_lib_set_txPower(0, -1, "100") != 0){
				printf("write_wlan_config:wifimgr_lib_set_txPower fail.\n");
			}
			break;
		default:
			printf("write_wlan_config:the value of TxPowerLevel is wrong!\n");
			return -1;
		}
	}
#endif


	/*write entry default setting to RT2860AP.dat*/
	for(j=0; j < Bssid_Num; j++){
		memset(node_name, 0 ,sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, j+1);	
		memset(tmp,0,sizeof(tmp));
		for(i = 0; strlen(wlan_entry_default_parm[i].attr_name) != 0; i++){
			if(cfg_get_object_attr(node_name, wlan_entry_default_parm[i].attr_name, tmp, 
																	sizeof(tmp)) > 0){
				if((wlan_entry_default_parm[i].wifi_api)(0, j + 1, tmp) < 0){
						printf("set %s fail.\n", wlan_entry_default_parm[i].attr_name);
				}
			}
		}
	}
	/*write entry setting to RT2860AP.dat*/
	/*SSID=SSID-A;SSID-B;SSID-C;SSID-D*/
	for(j=0; j < Bssid_Num; j++){
		memset(node_name, 0, sizeof(node_name)),
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, j+1);	
		if(cfg_get_object_attr(node_name, "SSID", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_ssid(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_ssid fail.\n");
			}
		}
		if(wifimgr_lib_get_wlan_igmp_snooping_support() != 0 ){
			if(cfg_get_object_attr(node_name, "IgmpSnEnable", tmp, sizeof(tmp)) > 0){
				if(wifimgr_lib_set_igmpSnEnable(0, j+1, tmp) != 0){
					printf("write_wlan_config:wifimgr_lib_set_igmpSnEnable fail.\n");
				}
			}
		}
#if defined (TCSUPPORT_ECNT_MAP)
		if(cfg_get_object_attr(node_name, "RRMEnable", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_rrmEnable(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_rrmEnable fail.\n");
			}
		}	
		if(cfg_get_object_attr(node_name, "WNMEnable", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wnmEnable(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wnmEnable fail.\n");
			}
		}	
#endif	

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
		if(cfg_get_object_attr(node_name, "EnableSSID", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_enableSsid(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_enableSsid fail.\n");
			}
		}
		else{
			if(wifimgr_lib_set_enableSsid(0, j+1, "0") != 0){
				printf("write_wlan_config:wifimgr_lib_set_enableSsid fail.\n");
			}
		}
#endif

#if defined(TCSUPPORT_WLAN_8021X)
		if(cfg_get_object_attr(node_name, "RADIUS_Key", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_radiuskey(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_radiuskey fail.\n");
			}
		}
#endif
#if defined (TCSUPPORT_WLAN_DOT11R_FT)
		if(cfg_get_object_attr(node_name, "FtSupport", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_ftSupport(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_ftSupport fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "FtOtd", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_ftotd(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_ftotd fail.\n");
			}
		}
#endif

		if(cfg_get_object_attr(node_name, "HideSSID", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_hideSSID(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_hideSSID fail.\n");
			}
		}		
		if(cfg_get_object_attr(node_name, "AuthMode", tmp, sizeof(tmp)) > 0){
			if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
				if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
					if(wifimgr_lib_set_ieee8021x(0, j+1, "1") != 0){
						printf("write_wlan_config:wifimgr_lib_set_ieee8021x fail.\n");
					}
				}else{
					if(wifimgr_lib_set_ieee8021x(0, j+1, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_ieee8021x fail.\n");
					}
				}
#endif	
#if defined(TCSUPPORT_CT_WLAN_NODE)
				if(cfg_get_object_attr(node_name, "WEPAuthType", tmp, sizeof(tmp)) > 0){
					if(!strcmp(tmp, "OpenSystem")){
						if(wifimgr_lib_set_authMode(0, j+1, "OPEN") != 0){
							printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
						}
					}else if(!strcmp(tmp, "SharedKey")){
						if(wifimgr_lib_set_authMode(0, j+1, "SHARED") != 0){
							printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
						}
					}else{
						if(wifimgr_lib_set_authMode(0, j+1, "WEPAUTO") != 0){
							printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
						}
					}
				}
				else{
					if(wifimgr_lib_set_authMode(0, j+1, "WEPAUTO") != 0){
						printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
					}
				}					
#else 
				if(wifimgr_lib_set_authMode(0, j+1, "WEPAUTO") != 0){
					printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
				}
#endif
				if(wifimgr_lib_set_encrypType(0, j+1, "WEP") != 0){
					printf("write_wlan_config:wifimgr_lib_set_encrypType fail.\n");
				}
				if(cfg_get_object_attr(node_name, "DefaultKeyID", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_defaultKeyID(0, j+1, tmp) != 0){
						printf("write_wlan_config:wifimgr_lib_set_defaultKeyID fail.\n");
					}
				}
				
				for(i = 1; i <= 4; i++){/*4 key string*/
					sprintf(tmp2,"Key%dStr",i);
					if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) > 0){	
						if(strlen(tmp) == 10 || strlen(tmp) == 26||strlen(tmp) == 0){
							if(wifimgr_lib_set_keyType(0, j+1, i, "0") != 0){
								printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
							}
						}
						else{
							if(wifimgr_lib_set_keyType(0, j+1, i, "1") != 0){
								printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
							}
						}	
						
						if(wifimgr_lib_set_keyStr(0, j+1, i, tmp) != 0){
							printf("write_wlan_config:wifimgr_lib_set_keyStr fail.\n");
						}						
						
					}
				}
			}
			else if(!strcmp(tmp, "WPAPSK")||!strcmp(tmp, "WPA2PSK")||!strcmp(tmp, "WPAPSKWPA2PSK") /*WPAPSK or WPA2PSK or Mixed mode*/
#if defined(TCSUPPORT_WLAN_8021X)
				||!strcmp(tmp, "WPA")||!strcmp(tmp, "WPA2")
				||!strcmp(tmp, "WPA1WPA2")
#if defined(TCSUPPORT_WLAN_WPA3)
				|| !strcmp(tmp, "WPA3") || !strcmp(tmp, "WPA3-192Bit")
#endif
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
			||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
				){/*WPAPSK or WPA2PSK or Mixed mode*/	
#if defined(TCSUPPORT_WLAN_8021X)
				if(!strcmp(tmp, "WPA")||!strcmp(tmp, "WPA2")||!strcmp(tmp, "WPA1WPA2")
#if defined(TCSUPPORT_WLAN_WPA3)
				|| !strcmp(tmp, "WPA3") || !strcmp(tmp, "WPA3-192Bit")
#endif
				){/*Radius-WEP64, Radius-WEP128*/
					if(wifimgr_lib_set_ieee8021x(0, j+1, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_ieee8021x fail.\n");
					}
				}
#if defined(TCSUPPORT_WLAN_WPA3)
				if (!strcmp(tmp, "WPA3-192Bit"))
				{
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "WPA3-192");
				}
#endif
#endif
				if(wifimgr_lib_set_authMode(0, j+1, tmp) != 0){
					printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
				}

				if(cfg_get_object_attr(node_name, "WPAPSK", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_WPAPSK(0, j+1, tmp) != 0){
						printf("write_wlan_config:wifimgr_lib_set_WPAPSK fail.\n");
					}
				}
				if(cfg_get_object_attr(node_name, "EncrypType", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_encrypType(0, j+1, tmp) != 0){
						printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
					}
				}
				if(wifimgr_lib_set_defaultKeyID(0, j+1, "2") != 0){
					printf("write_wlan_config:wifimgr_lib_set_defaultKeyID fail.\n");
				}

				for(i = 1; i <= 4; i++){
					if(wifimgr_lib_set_keyStr(0, j+1, i, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_keyStr fail.\n");
					}
					
					if(wifimgr_lib_set_keyType(0, j+1, i, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
					}
				}				
			}
			else{/*OPEN*/
				if(wifimgr_lib_set_authMode(0, j+1, "OPEN") != 0){
					printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
				}
				if(wifimgr_lib_set_encrypType(0, j+1, "NONE") != 0){
					printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
				}				
				if(wifimgr_lib_set_defaultKeyID(0, j+1, "0") != 0){
					printf("write_wlan_config:wifimgr_lib_set_defaultKeyID fail.\n");
				}
				if(wifimgr_lib_set_WPAPSK(0, j+1, "0") != 0){
					printf("write_wlan_config:wifimgr_lib_set_WPAPSK fail.\n");
				}
				
				for(i = 1; i <= 4; i++){	
					if(wifimgr_lib_set_keyType(0, j+1, i, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
					}

					if(wifimgr_lib_set_keyStr(0, j+1, i, "0") != 0){
						printf("write_wlan_config:wifimgr_lib_set_keyStr fail.\n");
					}
				}
			}
		}
		
#if defined(TCSUPPORT_WLAN_8021X)
				/*RADIUS_Server*/
				memset(tmp,0,sizeof(tmp));
#if defined(TCSUPPORT_WLAN_8021X_EXT)
				if(cfg_get_object_attr(node_name, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
					if(strlen(tmp) > 0){
						hp = gethostbyname(tmp);
						if(hp != (struct hostent *) NULL){
							memcpy((char *) &RadiusAddr.sin_addr
						, (char *) hp->h_addr_list[0],sizeof(RadiusAddr.sin_addr));
							strcat(wlan_script_entry_attr_buf[radius_server]
						,inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
							strcat(wlan_script_entry_attr_buf[radius_server],";");
						}else{
							strcat(wlan_script_entry_attr_buf[radius_server]
								,"0.0.0.0;");
						}
					}
				}
#else
				if(cfg_get_object_attr(node_name, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
					if(strlen(tmp) > 0){
						if(wifimgr_lib_set_radiusserver(0, j+1, tmp)){
							printf("write_wlan_config:wifimgr_lib_set_radiusserver fail.\n");
						}
					}
				}
#endif
		/*RADIUS_Port*/
				memset(tmp,0,sizeof(tmp));
				if(cfg_get_object_attr(node_name, "RADIUS_Port", tmp, sizeof(tmp)) > 0){
					if(strlen(tmp) > 0){
						if(wifimgr_lib_set_radiusport(0, j+1, tmp)){
							printf("write_wlan11ac_config:wifimgr_lib_set_radiusport fail.\n");
						}
					}
				}
		/*RADIUS_Key*/
				memset(tmp,0,sizeof(tmp));
				if(cfg_get_object_attr(node_name, "BAK_RADIUS_Key", tmp, sizeof(tmp)) > 0 ){
					if(strlen(tmp) > 0){
						if(wifimgr_lib_set_radiuskey(0, j+1, tmp)){
							printf("write_wlan11ac_config:wifimgr_lib_set_radiuskey fail.\n");
						}
					}
				}
#endif
		
		if(cfg_get_object_attr(node_name, "AuthMode", tmp, sizeof(tmp)) > 0){
			strncpy(encrytype, tmp, sizeof(encrytype) - 1);
		}

		wifimgr_lib_get_version(0, tmp, sizeof(tmp));
		if (strncmp(tmp, "7915",4))
		{
			memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(node_name, "HT_MCS", tmp, sizeof(tmp)) > 0){
			if((strcmp(encrytype,"WPAPSK") == 0) &&(atoi(strWirelessMode) == 9) && 
																(atoi(tmp)> 7)){
				if(wifimgr_lib_set_HT_MCS(0, j+1, "33")){
					printf("write_wlan_config:wifimgr_lib_set_HT_MCS fail.\n");
				}
			}else{
				if(wifimgr_lib_set_HT_MCS(0, j+1, tmp)){
					printf("write_wlan_config:wifimgr_lib_set_HT_MCS fail.\n");
				}
			}
		}
		}
		if(Is11nWirelessMode()){
			if(wifimgr_lib_set_wmmCapable(0, j+1, "0")){
				printf("write_wlan_config:wifimgr_lib_set_wmmCapable fail.\n");
			}
		}else{
			if(cfg_get_object_attr(node_name, "WMM", tmp, sizeof(tmp)) > 0){
				if(wifimgr_lib_set_wmmCapable(0, j+1, tmp) != 0){
					printf("write_wlan_config:wifimgr_lib_set_wmmCapable fail.\n");
				}
			}
		}
		switch(atoi(strWirelessMode))
		{
			case PHY_MODE_BG_MIXED:
				if(cfg_get_object_attr(node_name, "HT_RATE", strRate, sizeof(strRate)) > 0){
					if(strcmp(strRate,GVWlanChannelList[0].cChannelRate) == 0
						|| strcmp(strRate,GVWlanChannelList[1].cChannelRate) == 0
						|| strcmp(strRate,GVWlanChannelList[2].cChannelRate) == 0
						|| strcmp(strRate,GVWlanChannelList[3].cChannelRate) == 0){	
						if((wifimgr_lib_set_fixedTxMode(type, j+1, "1")) != 0){/*CCK*/
							printf("write_wlan_config:wifimgr_lib_set_fixedTxMode fail.\n");
						}
					}
				   	else{
						if((wifimgr_lib_set_fixedTxMode(0, j+1, "2")) != 0){/*OFDM*/
							printf("write_wlan_config:wifimgr_lib_set_fixedTxMode fail.\n");
						}
					}
				}
				else{
					printf("WLan:get HT_RATE fail when wireless mode is BG!\n");		
					//return CFG2_RET_FAIL;
				}
				break;
			case PHY_MODE_B_ONLY:
				if((wifimgr_lib_set_fixedTxMode(0, j+1, "1")) != 0){/*CCK*/
					printf("write_wlan_config:wifimgr_lib_set_fixedTxMode fail.\n");
				}
				break;
			case PHY_MODE_G_ONLY:
				if((wifimgr_lib_set_fixedTxMode(0, j+1, "2")) != 0){/*OFDM*/
					printf("write_wlan_config:wifimgr_lib_set_fixedTxMode fail.\n");
				}
				break;
			default:
				if((wifimgr_lib_set_fixedTxMode(0, j+1, "0")) != 0){
					printf("write_wlan_config:wifimgr_lib_set_fixedTxMode fail.\n");
				}
				break;
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(node_name, "RekeyInterval", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_rekeyInterval(0, j+1, tmp)){
					printf("write_wlan_config:wifimgr_lib_set_rekeyInterval fail.\n");
				}
				if(wifimgr_lib_set_rekeyMethod(0, j+1, "TIME")){
					printf("write_wlan_config:wifimgr_lib_set_rekeyMethod fail.\n");
				}
			}
		}

#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
				memset(tmp,0,sizeof(tmp));
				if((cfg_get_object_attr(node_name, "MaxStaNum", tmp, sizeof(tmp)) > 0) 
					&& (strlen(tmp) > 0)){
					if(wifimgr_lib_set_MaxStaNum(WIFI_2_4G, j+1, tmp) != 0){
						printf("write_wlan_config:wifimgr_lib_set_maxStaNum fail.\n");
					}
				}
#endif

#ifdef WSC_AP_SUPPORT
		if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscConfMode(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscConfMode fail.\n");
			}
		}	
		if(cfg_get_object_attr(node_name, "WPSConfStatus", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscConfStatus(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscConfStatus fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "WPSKeyASCII", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscKeyASCII(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscKeyASCII fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "WscV2Support", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscV2Support(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscV2Support fail.\n");
			}
		}
		else{
			if(wifimgr_lib_set_wscV2Support(0, j+1, "0") != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscV2Support fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "WscMaxPinAttack", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscMaxPinAttack(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscMaxPinAttack fail.\n");
			}
        }
		if(cfg_get_object_attr(node_name, "WscSetupLockTime", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscSetupLockTime(0, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wscSetupLockTime fail.\n");
			}
        }		
	#ifdef TCSUPPORT_WPS_BTN_DUALBAND
		memset(tmp, 0, sizeof(tmp));
		memset(tmp_s, 0, sizeof(tmp_s));
		WscGenerateUUID(tmp,tmp_s);
		if(wifimgr_lib_set_WscUUIDE(0, j+1, tmp) != 0){
			tcdbg_printf("write_wlan_config:wifimgr_lib_set_WscUUIDE fail.\n");
		 }
		if(wifimgr_lib_set_WscUUIDStr(0, j+1, tmp_s) != 0){
			tcdbg_printf("write_wlan_config:wifimgr_lib_set_WscUUIDStr fail.\n");
		 }
	#endif
	#endif
	}

	/*AccessPolicy0=.... AccessPolicy1=....*/
	for(j = 0; j < Bssid_Num; j++){
		sprintf(node_name, WLAN_ENTRY_N_NODE, j+1);
		if(cfg_get_object_attr(node_name, "AccessPolicy", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_accessPolicy(0, j+1, tmp) != 0){
					printf("write_wlan_config:wifimgr_lib_set_accessPolicy fail.\n");
				}
			}
		}
	}
	
	/*AccessControlList0=MAC1;MAC2;.......*/
	for(j=0; j < Bssid_Num; j++){
		sprintf(node_name, WLAN_ENTRY_N_NODE, j+1);
		strcpy(buf, "");
		for(i = 0; i < WLAN_MAC_NUM; i++){
			sprintf(tmp2, "WLan_MAC%d", i);
			if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) > 0){
				if(strlen(tmp) > 0){
					strcat(buf,tmp);
					strcat(buf,";");
				}
			}
		}

		if(strlen(buf) > 0){
			buf[strlen(buf) - 1] = '\0';/*remove last ";"*/
			if(wifimgr_lib_set_accessControlList(0, j+1, buf) !=0) {
				printf("write_wlan_config:wifimgr_lib_set_accessControlList fail.\n");
			}
			//sprintf(tmp,"%s%d=%s\n","AccessControlList",j,buf);
			//fputs_escape(tmp,fp);
		}

	}
	
#ifdef WSC_AP_SUPPORT
	/*WscDefaultSSID1=....WscDefaultSSID2=.......*/
	for(j = 0; j < Bssid_Num; j++){
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, j+1);
		snprintf(tmp2, sizeof(tmp2), "%s%d", "WscDefaultSSID", j+1);
		if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_wscDefaultSSID(0, j+1, tmp) !=0){
					printf("write_wlan_config:wifimgr_lib_set_wscDefaultSSID fail.\n");
				}			
			}
		}
	}
#endif

#if defined(TCSUPPORT_WLAN_8021X)
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), LAN_ENTRY0_NODE);
		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(node_name, "IP", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				 if(wifimgr_lib_set_ownip(0, -1, tmp)){
					printf("write_wlan_config:wifimgr_lib_set_ownip fail.\n");
				 }
			}
		}
#endif

#ifdef TCSUPPORT_WLAN_WDS
		setWdsAttrToDataFile(WIFI_2_4G);
#endif

#if defined(TCSUPPORT_ECNT_MAP)
	if(0 != update_mesh_dat(WIFI_2_4G))
	{
		tcdbg_printf("update_mesh_dat fail.\n");
		return -1;
	}
	
	if(0 != config_itf_mac_dat(WIFI_2_4G))
	{
		tcdbg_printf("config_itf_mac_dat fail.\n");
		return -1;
	}
#else
	if(0 != setMacToDataFile(WIFI_2_4G))
	{
		tcdbg_printf("write_wlan_config:wifimgr_lib_set_mac fail.\n");
		return -1;
	}
#endif
	
	//need  sync to 5g
	//1.GET 5G parameter-->SET 5G parameter
	//2.wifimgr set to 5g
	//3.save 5G dat
	ret = multi_profile_merge_global_settings(WIFI_2_4G);
	if (ret > 0)
	{
		if((0 == wifimgr_lib_save_dataFile(WIFI_5G))&&(0 == wifimgr_lib_save_dataFile(WIFI_2_4G))){
			return 0;
		}else{
			return -1;
		}
	}
	else
	{
		if(0 == wifimgr_lib_save_dataFile(WIFI_2_4G)){
			return 0;
		}else{
			return -1;
		}
	}
}

#if defined(TCSUPPORT_WLAN_AC)
int write_wlan11ac_config(char* path, int Bssid_Num)
{
	int i = 0;
	int j = 0;
	char node_name[128] 	= {0}; 
	char tmp[160] 			= {0}; 
#ifdef TCSUPPORT_WPS_BTN_DUALBAND
	char tmp_s[160] 			= {0}; 
#endif
	char tmp2[160] 			= {0};
	char buf[160] 			= {0};
	char strWirelessMode[8] = {0};
	char encrytype[16] 		= {0};
#if defined(TCSUPPORT_WLAN_AX)
	char MuOfdmaDlEnable[5] = {0};
	char MuOfdmaUlEnable[5] = {0};
	char MuMimoDlEnable[5] = {0};
	char MuMimoUlEnable[5] = {0};
	char TWTSupportEnable[16] = {0};
#endif

	wifi_attr_fun wlan_common_default_parm[64] = {0};
	wifi_attr_fun wlan_entry_default_parm[32] = {0};
	wifi_attr_fun wlan_script_com_attr[64] = {0};
	int ret = 0;
	
	memset(wlan_common_default_parm, 0, 64 * sizeof(wifi_attr_fun));
	ret = wifimgr_lib_get_wlan_attr(1, wlan_common_default_parm);
	
	memset(wlan_entry_default_parm, 0, 32 * sizeof(wifi_attr_fun));
	wifimgr_lib_get_wlan_entry_attr(1, wlan_entry_default_parm);

	memset(wlan_script_com_attr, 0, 64 * sizeof(wifi_attr_fun));
	wifimgr_lib_get_wlan_script_com_attr_config(1, wlan_script_com_attr);
#if 0
	if(wifimgr_lib_set_dbg_level("15") != 0)
		printf("open BSP wifi debug fail.\n");
	else
		printf("open BSP wifi debug success.\n");
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		pre_sys_state_init();
#endif

	if(wifimgr_lib_init(1) != 0){
		printf("Create shared memory of data file fail.\n");
		return -1;
	}

	/*write common default setting to RT2860AP.dat*/
	memset(node_name, 0 , sizeof(node_name));
	snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
	
	if(cfg_get_object_attr(node_name, "Country", tmp, sizeof(tmp)) > 0){
		for(i = 0; i < NUM_COUNTRY; i++){	
			if(strcmp((char*)all_country[i].pCountryName,tmp)==0){
				if(wifimgr_lib_set_countryCode(1, -1, (char*)all_country[i].IsoName) != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_countryCode fail.\n");
				}
				break;
			}	
		}
	}	

	for(i = 0; strlen(wlan_common_default_parm[i].attr_name) != 0; i++){
		/*write cfg node value to RT2860AP.dat, instead of default value. Thus the value in cfg node and 
		  *  RT2860AP.dat can be  synchronized  */	
		if(cfg_get_object_attr(node_name, wlan_common_default_parm[i].attr_name, tmp, 
																sizeof(tmp)) > 0){
			if (strcmp(wlan_common_default_parm[i].attr_name, "TxBurst") == 0){
				if(txBurst_or_not(Bssid_Num) == 1){
					if((wlan_common_default_parm[i].wifi_api)(1, -1, "1") < 0){
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
					}
				}else{
					if((wlan_common_default_parm[i].wifi_api)(1, -1, "0") < 0){
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
					}
				}	
			}
			else if ( 0 == strcmp(wlan_common_default_parm[i].attr_name, "WHNAT") )
			{
				if ( (wlan_common_default_parm[i].wifi_api)(1, -1, tmp) < 0 )
				{
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
				}
			}
			else if ( 0 == strcmp(wlan_common_default_parm[i].attr_name, "SHORTGIFLAG") )
			{
				if ( (wlan_common_default_parm[i].wifi_api)(1, -1, tmp) < 0 )
				{
				printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
				}
			}
			else if ( 0 == strcmp(wlan_common_default_parm[i].attr_name, "WMMFLAG") )
			{
				
				if (atoi(tmp) == 1)
				{
					wifimgr_lib_set_bssAifsn(1,-1,"1;7;2;2");
					wifimgr_lib_set_bssCwmin(1,-1,"2;4;3;2");
					wifimgr_lib_set_bssCwmax(1,-1,"2;10;4;3");
				}
				else
					printf("do not set edca parm.\n");
			}
			else{
				if((wlan_common_default_parm[i].wifi_api != NULL)   &&
					(wlan_common_default_parm[i].wifi_api)(1, -1, tmp) != 0){
						printf("set %s fail.\n", wlan_common_default_parm[i].attr_name);
				}
			}
		}
	}
	
	/*write common setting to RT2860AP.dat */
	if(cfg_get_object_attr(node_name, "WirelessMode", strWirelessMode, 
										sizeof(strWirelessMode)) > 0){
		if(wifimgr_lib_set_wirelessMode(1, -1, strWirelessMode) != 0){
			printf("write_wlan11ac_config:wifimgr_lib_set_wirelessMode fail.\n");
		}

		if((atoi(strWirelessMode)==PHY_11VHT_N_A_MIXED) 
			|| (atoi(strWirelessMode)==PHY_11VHT_N_MIXED)
			|| (atoi(strWirelessMode)==PHY_MODE_11N_MIXED)
			|| (atoi(strWirelessMode)==PHY_MODE_11AN_MIXED)
#if defined(TCSUPPORT_WLAN_AX)
			|| (atoi(strWirelessMode)==PHY_MODE_N_AC_AX_MIX)
#endif
			){
			cfg_set_object_attr(node_name, "11nMode", "1");
		}else{
			cfg_set_object_attr(node_name, "11nMode", "0");
		}

		if((atoi(strWirelessMode)==PHY_11VHT_N_A_MIXED) 
			|| (atoi(strWirelessMode)==PHY_11VHT_N_MIXED)
#if defined(TCSUPPORT_WLAN_AX)
			|| (atoi(strWirelessMode)==PHY_MODE_N_AC_AX_MIX)
#endif
			){
			cfg_set_object_attr(node_name, "11acMode", "1");
		}else{
			cfg_set_object_attr(node_name, "11acMode", "0");
		}
	}

	/*wlan_script_com_attr*/
	for(i = 0; strlen(wlan_script_com_attr[i].attr_name) != 0; i++){
		if(strcmp(wlan_script_com_attr[i].attr_name, "AutoChannelSelect") == 0){
			if(cfg_get_object_attr(node_name, "Channel", tmp, sizeof(tmp)) > 0){
				if(atoi(tmp)==0){/*Auto Channel*/
					wifimgr_lib_get_CHANNEL_SELECT_DEF(tmp, sizeof(tmp));
					if(wifimgr_lib_set_autoChannelSelect(1, -1, tmp) != 0)
					{
						printf("write_wlan11ac_config:wifimgr_lib_set_channel fail.\n");
					}
					if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name, tmp, sizeof(tmp)) > 0){
						if(atoi(tmp) > 0 && atoi(tmp) < 4){
							if(wifimgr_lib_set_autoChannelSelect(1, -1, tmp) != 0){
								printf("write_wlan11ac_config:wifimgr_lib_set_autoChannelSelect fail.\n");
							}
						}
					}
				}else{
					if(wifimgr_lib_set_autoChannelSelect(1, -1, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_autoChannelSelect fail.\n");
					}
				}
			}
		}		
		else if(strcmp(wlan_script_com_attr[i].attr_name, "HT_BW") == 0){
			switch(atoi(strWirelessMode)){
				case PHY_11VHT_N_A_MIXED:
				case PHY_11VHT_N_MIXED:
				case PHY_MODE_11AN_MIXED:
#if defined(TCSUPPORT_WLAN_AX)
				case PHY_MODE_N_AC_AX_MIX:
#endif
					if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name,
																tmp, sizeof(tmp)) > 0){
						if(wifimgr_lib_set_HT_BW(1, -1, tmp) != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_HT_BW fail.\n");
						}
					}
					break;
				case PHY_MODE_11A_ONLY:
					if(wifimgr_lib_set_HT_BW(1, -1, "0") != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_HT_BW fail.\n");
					}
					break;
				default:
					printf("write_wlan11ac_config:WLan wireless"
						" mode is not right when set HT_BW!\n");
					return CFG2_RET_FAIL;
			}	
		}
		
		else if(strcmp(wlan_script_com_attr[i].attr_name, "VHT_BW") == 0){
			if(Is11acWirelessMode()){
				if(cfg_get_object_attr(node_name, "HT_BW", tmp, sizeof(tmp)) > 0){
					if (atoi(tmp) == 1){
						if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name,
																	tmp, sizeof(tmp)) > 0){
							if(wifimgr_lib_set_VHT_BW(1, -1, tmp) != 0){
								printf("write_wlan11ac_config:wifimgr_lib_set_VHT_BW fail.\n");
							}
						}
					}else{
						if(wifimgr_lib_set_VHT_BW(1, -1, "0") != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_VHT_BW fail.\n");
						}											
					}
				}
			}else{
				if(wifimgr_lib_set_VHT_BW(1, -1, "0") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_VHT_BW fail.\n");
				}
			}
		}

		else if(strcmp(wlan_script_com_attr[i].attr_name, "VHT_SGI") == 0){
			if(Is11acWirelessMode()){
				if(cfg_get_object_attr(node_name, "HT_BW", tmp, sizeof(tmp)) > 0){
					if (atoi(tmp) == 1) {
						if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name,
																	tmp, sizeof(tmp)) > 0){
							if(wifimgr_lib_set_VHT_SGI(1, -1, tmp) != 0){
								printf("write_wlan11ac_config:wifimgr_lib_set_VHT_SGI fail.\n");
							}
						}
					}
					else {
						if(wifimgr_lib_set_VHT_SGI(1, -1, "1") != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_VHT_SGI fail.\n");
						}											
					}
				}
			}else{
				if(wifimgr_lib_set_VHT_SGI(1, -1, "1") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_VHT_SGI fail.\n");
				}
			}
		}
		
		else if(strcmp(wlan_script_com_attr[i].attr_name, "HT_OpMode") == 0){		
			if(wifimgr_lib_set_HT_OpMode(1, -1, "0") != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_HT_OpMode fail.\n");
			}				
		}

		else{
			if(cfg_get_object_attr(node_name, (char *)wlan_script_com_attr[i].attr_name,
															tmp, sizeof(tmp)) > 0){
				if(strlen(tmp) > 0){
					if((wlan_script_com_attr[i].wifi_api != NULL) &&
					((wlan_script_com_attr[i].wifi_api)(1, -1, tmp) != 0)){
						printf("write_wlan11ac_config:write %s to data file fail.\n", 
												wlan_script_com_attr[i].attr_name);
					}
				}
			}
		}
	}
	printf("debug 4.\n");
/*write entry default setting to RT2860AP.dat*/
	for(j=0; j < Bssid_Num; j++){
		memset(node_name, 0 ,sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, j+1);	
		memset(tmp,0,sizeof(tmp));
		for(i = 0; strlen(wlan_entry_default_parm[i].attr_name) != 0; i++){
			if(cfg_get_object_attr(node_name, wlan_entry_default_parm[i].attr_name, tmp, 
																	sizeof(tmp)) > 0){
				if((wlan_entry_default_parm[i].wifi_api)(1, j+1, tmp) < 0){
						printf("set %s fail.\n", wlan_entry_default_parm[i].attr_name);
				}
			}
		}
	}
	/*write entry setting to RT2860AP.dat*/
	/*SSID=SSID-A;SSID-B;SSID-C;SSID-D*/
	for(j=0; j < Bssid_Num; j++){
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, j+1);	
		if(cfg_get_object_attr(node_name, "SSID", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_ssid(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_ssid fail.\n");
			}
		}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
		if(cfg_get_object_attr(node_name, "EnableSSID", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_enableSsid(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_enableSsid fail.\n");
			}
		}
		else{
			if(wifimgr_lib_set_enableSsid(1, j+1, "0") != 0){
				printf("write_wlan_config:wifimgr_lib_set_enableSsid fail.\n");
			}
		}
#endif	

		if(cfg_get_object_attr(node_name, "IgmpSnEnable", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_igmpSnEnable(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_igmpSnEnable fail.\n");
			}
		}

#if defined(TCSUPPORT_WLAN_8021X)
		if(cfg_get_object_attr(node_name, "RADIUS_Key", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_radiuskey(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_radiuskey fail.\n");
			}
		}
#endif

		if(cfg_get_object_attr(node_name, "HideSSID", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_hideSSID(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_hideSSID fail.\n");
			}
		}
		
#if defined (TCSUPPORT_WLAN_DOT11R_FT)
		if(cfg_get_object_attr(node_name, "FtSupport", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_ftSupport(1, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_ftSupport fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "FtOtd", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_ftotd(1, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_ftotd fail.\n");
			}
		}
#endif

#if defined (TCSUPPORT_WLAN_DOT11K_RRM) || defined (TCSUPPORT_ECNT_MAP)
				if(cfg_get_object_attr(node_name, "RRMEnable", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_rrmEnable(1, j+1, tmp) != 0){
						printf("write_wlan_config:wifimgr_lib_set_rrmEnable fail.\n");
					}
				}	
#endif	
#if defined (TCSUPPORT_ECNT_MAP)
		if(cfg_get_object_attr(node_name, "WNMEnable", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wnmEnable(1, j+1, tmp) != 0){
				printf("write_wlan_config:wifimgr_lib_set_wnmEnable fail.\n");
			}
		}	
#endif	
		if(cfg_get_object_attr(node_name, "AuthMode", tmp, sizeof(tmp)) > 0){
			if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
				if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
					if(wifimgr_lib_set_ieee8021x(1, j+1, "1") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_ieee8021x fail.\n");
					}
				}else{
					if(wifimgr_lib_set_ieee8021x(1, j+1, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_ieee8021x fail.\n");
					}
				}
#endif	
				if(cfg_get_object_attr(node_name, "WEPAuthType", tmp, sizeof(tmp)) > 0){				
					if(!strcmp(tmp, "OpenSystem")){
						if(wifimgr_lib_set_authMode(1, j+1, "OPEN") != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
						}
					}else if(!strcmp(tmp, "SharedKey")){
						if(wifimgr_lib_set_authMode(1, j+1, "SHARED") != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
						}
					}else{
						if(wifimgr_lib_set_authMode(1, j+1, "WEPAUTO") != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
						}
					}
				}
				else{
					if(wifimgr_lib_set_authMode(1, j+1, "WEPAUTO") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
					}
				}	
				if(wifimgr_lib_set_encrypType(1, j+1, "WEP") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_encrypType fail.\n");
				}
				if(cfg_get_object_attr(node_name, "DefaultKeyID", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_defaultKeyID(1, j+1, tmp) != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_defaultKeyID fail.\n");
					}
				}
				
				for(i=1; i<=4; i++){/*4 key string*/
					sprintf(tmp2,"Key%dStr",i);
					if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) > 0){
						if(strlen(tmp)==10 || strlen(tmp)==26||strlen(tmp)==0){
							if(wifimgr_lib_set_keyType(1, j+1, i, "0") != 0){
								printf("write_wlan11ac_config:wifimgr_lib_set_keyType fail.\n");
							}
						}
						else{
							if(wifimgr_lib_set_keyType(1, j+1, i, "1") != 0){
								printf("write_wlan11ac_config:wifimgr_lib_set_keyType fail.\n");
							}
						}	
						
						if(wifimgr_lib_set_keyStr(1, j+1, i, tmp) != 0){
							printf("write_wlan11ac_config:wifimgr_lib_set_keyStr fail.\n");
						}						
					}
				}

				#if 0
				if(cfg_get_object_attr(node_name, "KeyPassphrase", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_keyPassPhrase(type, j+1, tmp) != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_keyPassPhrase fail.\n");
					}
				}
				#endif

			}
			else if(!strcmp(tmp, "WPAPSK")||!strcmp(tmp, "WPA2PSK")||!strcmp(tmp, "WPAPSKWPA2PSK")
#if defined(TCSUPPORT_WLAN_8021X)
								||!strcmp(tmp, "WPA")||!strcmp(tmp, "WPA2")
								||!strcmp(tmp, "WPA1WPA2")
#if defined(TCSUPPORT_WLAN_WPA3)
								|| !strcmp(tmp, "WPA3") || !strcmp(tmp, "WPA3-192Bit")
#endif

#endif
#if defined(TCSUPPORT_WLAN_WPA3)
						||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
			){/*WPAPSK or WPA2PSK or Mixed mode*/	
#if defined(TCSUPPORT_WLAN_8021X)
				if(!strcmp(tmp, "WPA")||!strcmp(tmp, "WPA2")||!strcmp(tmp, "WPA1WPA2")
#if defined(TCSUPPORT_WLAN_WPA3)
				|| !strcmp(tmp, "WPA3") || !strcmp(tmp, "WPA3-192Bit")
#endif
				){/*Radius-WEP64, Radius-WEP128*/
					if(wifimgr_lib_set_ieee8021x(1, j+1, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_ieee8021x fail.\n");
					}
				}
#if defined(TCSUPPORT_WLAN_WPA3)
				if (!strcmp(tmp, "WPA3-192Bit"))
				{
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "WPA3-192");
				}
#endif
#endif
				if(wifimgr_lib_set_authMode(1, j+1, tmp) != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
				}

				if(cfg_get_object_attr(node_name, "WPAPSK", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_WPAPSK(1, j+1, tmp) != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_WPAPSK fail.\n");
					}
				}
				if(cfg_get_object_attr(node_name, "EncrypType", tmp, sizeof(tmp)) > 0){
					if(wifimgr_lib_set_encrypType(1, j+1, tmp) != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
					}
				}
				if(wifimgr_lib_set_defaultKeyID(1, j+1, "2") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_defaultKeyID fail.\n");
				}
				for(i=1; i<=4; i++){
					if(wifimgr_lib_set_keyStr(1, j+1, i, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_keyStr fail.\n");
					}
					
					if(wifimgr_lib_set_keyType(1, j+1, i, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_keyType fail.\n");
					}
				}				
			}
			else{/*OPEN*/
				if(wifimgr_lib_set_authMode(1, j+1, "OPEN") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
				}
				if(wifimgr_lib_set_encrypType(1, j+1, "NONE") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_authMode fail.\n");
				}						
				if(wifimgr_lib_set_defaultKeyID(1, j+1, "0") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_defaultKeyID fail.\n");
				}

				if(wifimgr_lib_set_WPAPSK(1, j+1, "0") != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_WPAPSK fail.\n");
				}
				
				for(i=1; i<=4; i++){	
					if(wifimgr_lib_set_keyType(1, j+1, i, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_keyType fail.\n");
					}

					if(wifimgr_lib_set_keyStr(1, j+1, i, "0") != 0){
						printf("write_wlan11ac_config:wifimgr_lib_set_keyStr fail.\n");
					}
				}
			}
		}

		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(node_name, "RekeyInterval", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_rekeyInterval(1, j+1, tmp)){
					printf("write_wlan11ac_config:wifimgr_lib_set_rekeyInterval fail.\n");
				}
				if(wifimgr_lib_set_rekeyMethod(1, j+1, "TIME")){
					printf("write_wlan11ac_config:wifimgr_lib_set_rekeyMethod fail.\n");
				}
			}
		}
		
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
		memset(tmp,0,sizeof(tmp));
		if((cfg_get_object_attr(node_name, "MaxStaNum", tmp, sizeof(tmp)) > 0) 
			&& (strlen(tmp) > 0)){
			if(wifimgr_lib_set_MaxStaNum(WIFI_5G, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_maxStaNum fail.\n");
			}
		}else{
			if(wifimgr_lib_set_MaxStaNum(WIFI_5G, j+1, "0") != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_maxStaNum fail.\n");
			}
		}
#endif

#if defined(TCSUPPORT_WLAN_8021X)
		/*RADIUS_Server*/
		memset(tmp,0,sizeof(tmp));
#if defined(TCSUPPORT_WLAN_8021X_EXT)
		if(cfg_get_object_attr(node_name, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				hp = gethostbyname(tmp);
				if(hp != (struct hostent *) NULL){
					memcpy((char *) &RadiusAddr.sin_addr
				, (char *) hp->h_addr_list[0],sizeof(RadiusAddr.sin_addr));
					strcat(wlan_script_entry_attr_buf[radius_server]
				,inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
					strcat(wlan_script_entry_attr_buf[radius_server],";");
				}else{
					strcat(wlan_script_entry_attr_buf[radius_server]
						,"0.0.0.0;");
				}
			}
		}
#else
		if(cfg_get_object_attr(node_name, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_radiusserver(1, j+1, tmp)){
					printf("write_wlan11ac_config:wifimgr_lib_set_radiusserver fail.\n");
				}
			}
		}
#endif
/*RADIUS_Port*/
		memset(tmp,0,sizeof(tmp));
		if(cfg_get_object_attr(node_name, "RADIUS_Port", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_radiusport(1, j+1, tmp)){
					printf("write_wlan11ac_config:wifimgr_lib_set_radiusport fail.\n");
				}
			}
		}
/*RADIUS_Key*/
		memset(tmp,0,sizeof(tmp));
		if(cfg_get_object_attr(node_name, "BAK_RADIUS_Key", tmp, sizeof(tmp)) > 0 ){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_radiuskey(1, j+1, tmp)){
					printf("write_wlan11ac_config:wifimgr_lib_set_radiuskey fail.\n");
				}
			}
		}
#if 0
#if defined(TCSUPPORT_WLAN_8021X_EXT)
/*BAK_RADIUS_Server*/
		memset(tmp,0,sizeof(tmp));
		if(getAttrValue(top, nodeName, "BAK_RADIUS_Server"
			, tmp) == TCAPI_PROCESS_OK){
			if(strlen(tmp) > 0){
				hp = gethostbyname(tmp);
				if(hp != (struct hostent *) NULL){
					memcpy((char *) &RadiusAddr.sin_addr
						, (char *) hp->h_addr_list[0]
						,sizeof(RadiusAddr.sin_addr));
					strcat(wlan_script_entry_attr_buf[bak_radius_server]
				,inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
					strcat(wlan_script_entry_attr_buf[bak_radius_server],";");
				}else{
					strcat(wlan_script_entry_attr_buf[bak_radius_server]
						,"0.0.0.0;");
				}
			}
		}
/*BAK_RADIUS_Port*/
		memset(tmp,0,sizeof(tmp));
		if(cfg_get_object_attr(node_name, "BAK_RADIUS_Port", tmp, sizeof(tmp) > 0)){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_bak_radius_port(type, j+1, tmp)){
					printf("write_wlan11ac_config:wifimgr_lib_set_radiusport fail.\n");
				}
			}
		}
		memset(tmp,0,sizeof(tmp));
		if(getAttrValue(top, nodeName, "BAK_RADIUS_Port", tmp) 
			== TCAPI_PROCESS_OK){
			if(strlen(tmp) > 0){
				strcat(wlan_script_entry_attr_buf[bak_radius_port],tmp);
				strcat(wlan_script_entry_attr_buf[bak_radius_port],";");
			}
		}
/*BAK_RADIUS_Key*/
		memset(tmp,0,sizeof(tmp));
		if(getAttrValue(top, nodeName, "BAK_RADIUS_Key", tmp) 
			== TCAPI_PROCESS_OK){
			if(strlen(tmp) > 0){
				strcat(wlan_script_entry_attr_buf[bak_radius_key],tmp);
				strcat(wlan_script_entry_attr_buf[bak_radius_key],";");
			}
		}
#endif
#endif
#endif
		if(cfg_get_object_attr(node_name, "AuthMode", tmp, sizeof(tmp)) > 0){
			strncpy(encrytype, tmp, sizeof(encrytype) - 1);
		}
		if(cfg_get_object_attr(node_name, "HT_MCS", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_HT_MCS(1, j+1, tmp)){
				printf("write_wlan11ac_config:wifimgr_lib_set_HT_MCS fail.\n");
			}
		}
		
		if(Is11nWirelessMode5G()){
			if(wifimgr_lib_set_wmmCapable(1, j+1, "1")){
				printf("write_wlan11ac_config:wifimgr_lib_set_wmmCapable fail.\n");
			}
		}else{
			if(cfg_get_object_attr(node_name, "WMM", tmp, sizeof(tmp)) > 0){
				if(wifimgr_lib_set_wmmCapable(1, j+1, tmp) != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_wmmCapable fail.\n");
				}
			}
		}
			
		if((wifimgr_lib_set_fixedTxMode(1, j+1, "0")) != 0){
			printf("write_wlan11ac_config:wifimgr_lib_set_fixedTxMode fail.\n");
		}

		
#ifdef WSC_AP_SUPPORT
		if(cfg_get_object_attr(node_name, "WPSConfMode", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscConfMode(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscConfMode fail.\n");
			}
		}	
		if(cfg_get_object_attr(node_name, "WPSConfStatus", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscConfStatus(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscConfStatus fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "WPSKeyASCII", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscKeyASCII(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscKeyASCII fail.\n");
			}
		}
		if(cfg_get_object_attr(node_name, "WscV2Support", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscV2Support(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscV2Support fail.\n");
			}
		}
		else{
			if(wifimgr_lib_set_wscV2Support(1, j+1, "0") != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscV2Support fail.\n");
			}
		}
		
		if(cfg_get_object_attr(node_name, "WscMaxPinAttack", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscMaxPinAttack(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscMaxPinAttack fail.\n");
			}
        }
		if(cfg_get_object_attr(node_name, "WscSetupLockTime", tmp, sizeof(tmp)) > 0){
			if(wifimgr_lib_set_wscSetupLockTime(1, j+1, tmp) != 0){
				printf("write_wlan11ac_config:wifimgr_lib_set_wscSetupLockTime fail.\n");
			}
        }		
		
#ifdef TCSUPPORT_WPS_BTN_DUALBAND

		memset(tmp, 0, sizeof(tmp));
		memset(tmp_s, 0, sizeof(tmp_s));
		WscGenerateUUID(tmp,tmp_s);
		if(wifimgr_lib_set_WscUUIDE(1, j+1, tmp) != 0){
			printf("write_wlan11ac_config:wifimgr_lib_set_WscUUIDE fail.\n");
		 }
		if(wifimgr_lib_set_WscUUIDStr(1, j+1, tmp_s) != 0){
			printf("write_wlan11ac_config:wifimgr_lib_set_WscUUIDStr fail.\n");
		 }
#endif
#endif
	}

	/*AccessPolicy0=.... AccessPolicy1=....*/
	for(j=0; j < Bssid_Num; j++){
		sprintf(node_name,WLAN11AC_ENTRY_N_NODE,j+1);
		if(cfg_get_object_attr(node_name, "AccessPolicy", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_accessPolicy(1, j+1, tmp) != 0){
					printf("write_wlan11ac_config:wifimgr_lib_set_accessPolicy fail.\n");
				}
			}
		}
	}
	
	/*AccessControlList0=MAC1;MAC2;.......*/
	for(j=0; j < Bssid_Num; j++){
		sprintf(node_name, WLAN11AC_ENTRY_N_NODE, j+1);
		strcpy(buf, "");
		for(i = 0; i < WLAN_MAC_NUM; i++){
			sprintf(tmp2, "WLan_MAC%d", i);
			if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) > 0){
				if(strlen(tmp) > 0){
					strcat(buf,tmp);
					strcat(buf,";");
				}
			}
		}
		if(strlen(buf) > 0){
			buf[strlen(buf) - 1] = '\0';/*remove last ";"*/
			if(wifimgr_lib_set_accessControlList(1, j+1, buf) !=0) {
				printf("write_wlan11ac_config:wifimgr_lib_set_accessControlList fail.\n");
			}
		}
	}

#ifdef WSC_AP_SUPPORT
	/*WscDefaultSSID1=....WscDefaultSSID2=.......*/
	for(j = 0; j < Bssid_Num; j++){
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, j+1);
		snprintf(tmp2, sizeof(tmp2), "%s%d", "WscDefaultSSID", j+1);
		if(cfg_get_object_attr(node_name, tmp2, tmp, sizeof(tmp)) > 0){
			if(strlen(tmp)>0){
				if(wifimgr_lib_set_wscDefaultSSID(1, j+1, tmp) !=0){
					printf("write_wlan11ac_config:wifimgr_lib_set_wscDefaultSSID fail.\n");
				}		
			}
		}
	}
#endif


#if defined(TCSUPPORT_WLAN_8021X)
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), LAN_ENTRY0_NODE);
	memset(tmp, 0, sizeof(tmp));
	if(cfg_get_object_attr(node_name, "IP", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			 if(wifimgr_lib_set_ownip(1, -1, tmp)){
				printf("write_wlan11ac_config:wifimgr_lib_set_ownip fail.\n");
			 }
		}
	}
#endif
#ifdef TCSUPPORT_WLAN_WDS
	setWdsAttrToDataFile(WIFI_5G);
#endif

#if defined(TCSUPPORT_ECNT_MAP)
	wifimgr_lib_get_version(0, tmp, sizeof(tmp));
	if(0 != update_mesh_dat(WIFI_5G))
	{
		tcdbg_printf("update_mesh_dat fail.\n");
		return -1;
	}
	if(strncmp(tmp, "7915D",5) && 0 != config_itf_mac_dat(WIFI_5G))
	{
		tcdbg_printf("config_itf_mac_dat fail.\n");
		return -1;
	}
#else
	if(strncmp(tmp, "7915D",5) && 0 != setMacToDataFile(WIFI_5G))
	{
		tcdbg_printf("write_wlan11ac_config:wifimgr_lib_set_mac fail.\n");
		return -1;
	}
#endif

	//need  sync to 2.4g
	//1.GET 5G parameter-->SET 2.4G parameter
	//2.wifimgr set tp 2.4g
	//3.save 2.4G dat
	ret = multi_profile_merge_global_settings(WIFI_5G);		

	if (ret > 0)
	{
		if((0 == wifimgr_lib_save_dataFile(WIFI_5G))&&(0 == wifimgr_lib_save_dataFile(WIFI_2_4G))){
			return 0;
		}else{
			return -1;
		}
	}
	else
	{
		if(0 == wifimgr_lib_save_dataFile(WIFI_5G)){
			return 0;
		}else{
			return -1;
		}
	}
}
#endif

void dowscd(void)
{
	char value[4]		= {0};
	char node_name[128] = {0};
	char lan_ip[32] 	= {0};
	char wscdpid[128]	= {0};
	char upnpd_stat[8]	= {0};
	char tmp[4]			= {0};
	int wlan_id 		= 0;
	int wlan11ac_id 		= 0;

	system("killall -9 wscd");
	snprintf(node_name, sizeof(node_name), UPNPD_ENTRY_NODE);
	if (cfg_get_object_attr(node_name, "Active", upnpd_stat, sizeof(upnpd_stat)) > 0){
		if(strcmp(upnpd_stat, "Yes") != 0){
			return;
		}	
	}	
	snprintf(node_name, sizeof(node_name), WEBCURSET_ENTRY_NODE);
	if(0 == type){
		if(cfg_get_object_attr(node_name, "wlan_id", tmp, sizeof(tmp)) > 0){
    		wlan_id = atoi(tmp);
		}else{
			return 0;
		}
	}else if(1 == type){
		if(cfg_get_object_attr(node_name, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
    		wlan11ac_id = atoi(tmp);
		}else{
			return 0;
		}
	}else{
		tcdbg_printf("The wifi type is invalid.\n");
		return 0;
	}
	
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "root.%s.Entry.%d", SVC_WLAN_NAME,wlan_id+1);
	if(cfg_get_object_attr(node_name, "WPSConfMode", value, sizeof(value)) > 0){
		if(atoi(value)!=0){
			/*mark this route, because when switch upnp on page, it will cause the lan pc can not ping to the CPE. evan.jiang_20120517*/
            memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), LAN_ENTRY0_NODE);
	        if(cfg_get_object_attr(node_name, "IP", lan_ip, sizeof(lan_ip)) > 0){
		    	memset(wscdpid, 0, sizeof(wscdpid));
	    		snprintf(wscdpid, sizeof(wscdpid), "/usr/bin/wscd -i %s%d -a %s -m 1 &", SVC_WLAN_RA_NAME, wlan_id, lan_ip);
				system_escape(wscdpid);	    	
	    	}
		}
	}

#if defined(TCSUPPORT_WLAN_AC)	
	memset(node_name, 0, sizeof(node_name));
	snprintf(node_name, sizeof(node_name), "root.%s.Entry.%d", SVC_WLAN11AC_NAME,wlan11ac_id+1);
	if(cfg_get_object_attr(node_name, "WPSConfMode", value, sizeof(value)) > 0){
		if(atoi(value)!=0){
            memset(node_name, 0, sizeof(node_name));
			snprintf(node_name, sizeof(node_name), LAN_ENTRY0_NODE);
	        if(cfg_get_object_attr(node_name, "IP", lan_ip, sizeof(lan_ip)) > 0){
		    	memset(wscdpid, 0, sizeof(wscdpid));
	    		snprintf(wscdpid, sizeof(wscdpid), "/usr/bin/wscd -i %s%d -a %s -m 1 &", SVC_WLAN11AC_RA_NAME,wlan11ac_id, lan_ip);  		
				system_escape(wscdpid);	 				
	    	}
		}
	}
#endif
	return;	
}

int reloadWifiModule(void)
{
	system("rmmod mt7603eap");
	system("insmod /lib/modules/mt7603eap.ko");

	return 0;
}
		
int reloadWifi(char* path, int preBssidNum, int newBssidNum)
{
	int i = 0;
	char tmp[32]       	= {0};
	char EnableSSID[4] 	= {0};
	char node_name[128] = {0};
	
	for(i = 0; i < preBssidNum; i++){
		snprintf(tmp, sizeof(tmp), "ifconfig ra%d down", i);
		system(tmp);
	}	
	reloadWifiModule();
		
	for(i=0; i<newBssidNum; i++){
#if defined(TCSUPPORT_CT_WLAN_NODE)
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, i+1);
		memset(EnableSSID, 0, sizeof(EnableSSID));
		if((cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, 
											sizeof(EnableSSID)) < 0)){
			continue;
		}
		if(!strcmp(EnableSSID, WLAN_SSID_ON)){
			snprintf(tmp, sizeof(tmp), "ifconfig ra%d up", i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl addif br0 ra%d", i);
			system(tmp);			
		}else{
			snprintf(tmp, sizeof(tmp), "ifconfig ra%d down", i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl addif br0 ra%d", i);
			system(tmp);			
		}
#else
        snprintf(tmp, sizeof(tmp), "ifconfig ra%d up", i);
		system(tmp);
		snprintf(tmp, sizeof(tmp), "brctl addif br0 ra%d", i);
		system(tmp);
		printf("add ra%d to br0\n",i);
#endif
	}

	return CFG2_RET_SUCCESS;
}

#ifdef WSC_AP_SUPPORT
int wpsAction(char* path, char* APOn, int action)
{
	char buf[128]		= {0};
	char tmp[32]		= {0};	
	char node_name[128] = {0};
	char item[128] 		= {0};
	int wpsstart 		= -1;

	if(WIFI_2_4G == type){
		wpsstart = iswpsstart;
		snprintf(item, sizeof(item), "wlanWPStimerRunning");
	}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
		wpsstart = iswps5gstart;
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
		wpsstart = iswpsstart;
#endif
#endif
#ifdef TCSUPPORT_WLAN_MULTI_WPS
		snprintf(item, sizeof(item), "wlanWPStimerRunning_0");
#else
		snprintf(item,  sizeof(item), "wlanWPStimerRunning");
#endif
	}else{
		printf("The wifi type is invalid.\n");
		return 0;
	}
	if(!strcmp(APOn, WLAN_APOFF)){
		if(WIFI_2_4G == type){
			snprintf(buf, sizeof(buf), "%d\n", action);
			wifimgr_lib_set_WPS_BUTTON(buf);
		}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
			snprintf(buf, sizeof(buf), "%d\n", action);
			wifimgr_lib_set_WPS_5G_BUTTON(buf);
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
			snprintf(buf, sizeof(buf), "%d\n", action);
			wifimgr_lib_set_WPS_BUTTON(buf);
#endif
#endif
		}
	
		snprintf(node_name, sizeof(node_name), INFO_WLAN_COMMON_NODE, svc_wlan_name);
		if(cfg_get_object_attr(node_name, item, tmp, sizeof(tmp)) > 0){
			if(1 == atoi(tmp)){
				cfg_set_object_attr(node_name, item, "0");
			}
		}	
	}else if(!strcmp(APOn, WLAN_APON)){
		if(wpsstart){	
			if(WIFI_2_4G == type){
				snprintf(buf, sizeof(buf), "%d\n", action);
				wifimgr_lib_set_WPS_BUTTON(buf);
				iswpsstart = 0;
			}else if(WIFI_5G == type){
#if defined(TCSUPPORT_WPS_5G_BTN)
				snprintf(buf, sizeof(buf), "%d\n", action);
				wifimgr_lib_set_WPS_5G_BUTTON(buf);
				iswps5gstart = 0;
#else
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
				snprintf(buf, sizeof(buf), "%d\n", action);
				wifimgr_lib_set_WPS_BUTTON(buf);
				iswpsstart = 0;
#endif
#endif
			}
			return 1;
		}
	}

	return 0;
}
#endif

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
void startWifiInterface(char* path, int BssidNum)
{
	int i 				= 0;	
	char tmp[32]		= {0};	
	char EnableSSID[4]	= {0};
	char node_name[128] = {0};
	char EnableSSID_shm[MAX_KEYWORD_LEN]	= {0};

	for(i = 0; i < BssidNum; i++){
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, i+1);
		if(cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, sizeof(EnableSSID)) > 0){
			if(1 == atoi(EnableSSID)){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig ra%d up", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl addif br0 ra%d", i);
				system(tmp);
			}
			else{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig ra%d down", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl delif br0 ra%d", i);
				system(tmp);	
			}
		}
		else{
			if(wifimgr_lib_get_enableSsid(0, i+1, EnableSSID_shm, MAX_KEYWORD_LEN) == 0){
				if(atoi(EnableSSID_shm) == 1){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "ifconfig ra%d down", i);
					system(tmp);
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "brctl delif br0 ra%d", i);
					system(tmp);					
					if(wifimgr_lib_set_enableSsid(0, i+1, "0") == 0){
						wifimgr_lib_save_dataFile(WIFI_2_4G);
					}		
				}	
			}	
		}
	}
	return;
}
#else
void startWifiInterface(char* path, int BssidNum)
{
	int i 				= 0;	
	char tmp[32]		= {0};	
	char EnableSSID[4]	= {0};
	char node_name[128] = {0};

	for(i = 0; i < BssidNum; i++){
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, i+1);
		if((cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, sizeof(EnableSSID)) > 0)
																&& (atoi(EnableSSID)==1)){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig ra%d up", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl addif br0 ra%d", i);
				system(tmp);		
		}
		else{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig ra%d down", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl delif br0 ra%d", i);
				system(tmp);
		}
	}
	return;
}
#endif

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
void start11acWifiInterface(char* path, int BssidNum)
{
	int i 				= 0;	
	char tmp[32]		= {0};	
	char EnableSSID[4]	= {0};
	char node_name[128] = {0};
	char EnableSSID_shm[MAX_KEYWORD_LEN]	= {0};

	for(i = 0; i < BssidNum; i++){
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, i+1);
		if(cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, sizeof(EnableSSID)) > 0){
			if(1 == atoi(EnableSSID)){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig rai%d up", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl addif br0 rai%d", i);
				system(tmp);
			}
			else{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig rai%d down", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl delif br0 rai%d", i);
				system(tmp);	
			}
		}
		else{
			if(wifimgr_lib_get_enableSsid(0, i+1, EnableSSID_shm, MAX_KEYWORD_LEN) == 0){
				if(atoi(EnableSSID_shm) == 1){
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "ifconfig rai%d down", i);
					system(tmp);
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "brctl delif br0 rai%d", i);
					system(tmp);					
					if(wifimgr_lib_set_enableSsid(0, i+1, "0") == 0){
						wifimgr_lib_save_dataFile(WIFI_5G);
					}		
				}	
			}	
		}
	}
	return;
}
#else
void start11acWifiInterface(char* path, int BssidNum)
{
	int i 				= 0;	
	char tmp[32]		= {0};	
	char EnableSSID[4]	= {0};
	char node_name[128] = {0};

	for(i = 0; i < BssidNum; i++){
		memset(node_name, 0, sizeof(node_name));
		snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, i+1);
		if((cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, sizeof(EnableSSID)) > 0)
																&& (atoi(EnableSSID)==1)){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig rai%d up", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl addif br0 rai%d", i);
				system(tmp);		
		}
		else{
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "ifconfig rai%d down", i);
				system(tmp);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "brctl delif br0 rai%d", i);
				system(tmp);
		}
	}
	return;
}
#endif



int getWlanSSIDEnableNum(int type, int BssidNum)
{
	int i				= 0;	
	char EnableSSID[4]	= {0};
	char node_name[128] = {0};
	int ret				= 0;
	
	for(i = 0; i < BssidNum; i++){
		memset(node_name, 0, sizeof(node_name));
		if(WIFI_2_4G == type){
			snprintf(node_name, sizeof(node_name), WLAN_ENTRY_N_NODE, i+1);
		}else if(WIFI_5G == type){
			snprintf(node_name, sizeof(node_name), WLAN11AC_ENTRY_N_NODE, i+1);
		}else{
			return -1;
		}
					
		if(cfg_get_object_attr(node_name, "EnableSSID", EnableSSID, 
											sizeof(EnableSSID)) >0){
			if (atoi(EnableSSID) == 1) {
				ret = (ret | (1 << i));
				if (type == WIFI_2_4G)
					g_wlan_switch_cfg.wifi_switch |= (1<<i);
				else
					g_wlan_switch_cfg.wifi_switch |= (1<<(i+16));
			}
			else {
				if (type == WIFI_2_4G)
					g_wlan_switch_cfg.wifi_switch &= ~(1<<i);
				else
					g_wlan_switch_cfg.wifi_switch &= ~(1<<(i+16));
			}
		}
	}

	return ret;
}

int checkWriteBinToFlash(void)
{
	char node_name[128]		= {0};
	char writebintoflash[4]	= {0};

	if(WIFI_2_4G == type){
		snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
		if(cfg_get_object_attr(node_name, "WriteBinToFlash", writebintoflash, 
											sizeof(writebintoflash)) > 0){
			if(atoi(writebintoflash) == 1){
				if(wifimgr_lib_set_wifi_calibration(WIFI_2_4G) != 0){
					printf("Write calibration infomation to flash fail.\n");
					return -1;
				}
				cfg_set_object_attr(node_name, "WriteBinToFlash", "0");
			}
		}
	}else if(WIFI_5G == type){
		snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
		if(cfg_get_object_attr(node_name, "WriteBinToFlash", writebintoflash, 
											sizeof(writebintoflash)) > 0){
			if(atoi(writebintoflash) == 1){
				if(wifimgr_lib_set_wifi_calibration(WIFI_5G) != 0){
					printf("Write calibration infomation to flash fail.\n");
					return -1;
				}
				cfg_set_object_attr(node_name, "WriteBinToFlash", "0");
			}
		}
	}else{
		printf("The wifi type is invalid.\n");
		return -1;
	}	
	
	return 0;
}

/*execute WLAN setup script*/
void executeWLANSetupScript(void)
{
	FILE *fp = NULL;
	int ret = 0;
	
	fp = fopen(svc_wlan_script_path, "r");
	if(fp != NULL){
		fclose(fp);
		ret = wifimgr_lib_execute_wlan_script(0, svc_wlan_script_path);
		fp = fopen(WLAN_MCS_SCRIPT_PATH, "r");
		if(fp != NULL){
			fclose(fp);
			ret = wifimgr_lib_execute_wlan_script(2, WLAN_MCS_SCRIPT_PATH);
			unlink(WLAN_MCS_SCRIPT_PATH);
		}
	}
	
	return;
}

#if defined(TCSUPPORT_WLAN_8021X)
void rtdot1xd_start(void)
{
	system("/userfs/bin/rtdot1xd");
	return;
}

void rtdot1xd_stop(void)
{
	system("killall -9 rtdot1xd");
	return;
}

#if defined(TCSUPPORT_WLAN_AC)	
void rtdot1xd_AC_start(void)
{
	system("/userfs/bin/rtdot1xd_AC");
	return;
}

void rtdot1xd_AC_stop(void)
{
	system("killall -9 rtdot1xd_AC");
	return;
}

#endif
#endif

#ifdef TCSUPPORT_WLAN_WDS
int write_wds_attr(int type, FILE* fp)
{
	char tmp[128] 		= {0};
	char tmp2[128] 		= {0};
	char nodeName[128]	= {0};
	int i 				= 0;

	if(WIFI_2_4G == type){
		snprintf(nodeName, sizeof(nodeName), "%s", WLAN_WDS_NODE);
	}else if(WIFI_5G == type){
		snprintf(nodeName, sizeof(nodeName), "%s", WLAN11AC_WDS_NODE);
	}else{
		return CFG2_RET_FAIL;
	}
	
	if(NULL != fp){
		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(nodeName, "WdsEnable", tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}
		memset(tmp2, 0, sizeof(tmp2));
		snprintf(tmp2, sizeof(tmp2), "WdsEnable=%s\n", tmp);
		fputs_escape(tmp2, fp);

		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(nodeName, "WdsEncrypType", tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}	
		memset(tmp2, 0, sizeof(tmp2));
		snprintf(tmp2, sizeof(tmp2), "WdsEncrypType=%s\n", tmp);
		fputs_escape(tmp2, fp);

		memset(tmp, 0, sizeof(tmp));
		if(cfg_get_object_attr(nodeName, "WdsKey", tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}
		memset(tmp2, 0, sizeof(tmp2));
		snprintf(tmp2, sizeof(tmp2), "WdsKey=%s\n", tmp);
		fputs_escape(tmp2, fp);

		for(i = 0; i < MAX_WDS_ENTRY; i++){
			memset(tmp2, 0, sizeof(tmp2));
			snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d", i);
			memset(tmp, 0, sizeof(tmp));
			if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) < 0){
				return CFG2_RET_FAIL;
			}	
			memset(tmp2, 0, sizeof(tmp2));
			snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d=%s\n", i, tmp);
			fputs_escape(tmp2, fp); 		
		}
	}
	else{
		return CFG2_RET_FAIL;
	}	

	return CFG2_RET_SUCCESS;
}

int wlan_wds_execute(int *p_isWDSReloadWIFI, int *p_WdsActive)
{
	int i 			= 0;
	char APOn[4]	= {0};
	char tmp[128]	= {0};
	char tmp2[128]	= {0};
	
	/*WdsEnable */
	if(cfg_get_object_attr(WLAN_WDS_NODE, "WdsEnable", tmp, sizeof(tmp)) < 0){
		return CFG2_RET_FAIL;
	}else{
		*p_WdsActive = atoi(tmp);
	}

	if(*p_WdsActive != pre_sys_state.WdsEnable || 
		(!strcmp(APOn, WLAN_APON) && (*p_WdsActive == 1))){
		*p_isWDSReloadWIFI = 1;
		pre_sys_state.WdsEnable = *p_WdsActive;
	}

	/*WdsEncrypType */
	if(cfg_get_object_attr(WLAN_WDS_NODE, "WdsEncrypType", tmp, sizeof(tmp)) < 0){
		return CFG2_RET_FAIL;
	}

	if(strcmp(tmp, pre_sys_state.WdsEncrypType) != 0){
		*p_isWDSReloadWIFI = 1;
		strncpy(pre_sys_state.WdsEncrypType, tmp, sizeof(pre_sys_state.WdsEncrypType) - 1);
	}

	/*WdsKey */
	if(cfg_get_object_attr(WLAN_WDS_NODE, "WdsKey", tmp, sizeof(tmp)) < 0){
		return CFG2_RET_FAIL;
	}

	if(strcmp(tmp, pre_sys_state.WdsKey) != 0){
		*p_isWDSReloadWIFI = 1;
		strncpy(pre_sys_state.WdsKey, tmp, sizeof(pre_sys_state.WdsKey) - 1);
	}

	/*Wds_MAC*/
	for(i = 0; i < MAX_WDS_ENTRY; i++){
		snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d", i);
		if(cfg_get_object_attr(WLAN_WDS_NODE, tmp2, tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}

		if(strcmp(tmp, pre_sys_state.Wds_MAC[i]) != 0){
			*p_isWDSReloadWIFI = 1;
			strcpy(pre_sys_state.Wds_MAC[i], tmp);
		}	
	}
	/*main ssid wep key change
	if wds is not enable ,main ssid wep key change ,not reload wifi module
	if wds enable,main ssid wep key change, reload wifi module*/
	if(*p_WdsActive != 0){/*wds enable*/
		if(cfg_get_object_attr(WLAN_ENTRY1_NODE, "AuthMode", tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}
		if(NULL != strstr(tmp, "WEP")){
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(WLAN_ENTRY1_NODE, tmp2, tmp, sizeof(tmp)) < 0){
					/*it will cause wifi unnormal, because the subsequent code will not be executed */
					continue;
				}
				if(strcmp(tmp, pre_sys_state.WepKeyStr[i]) != 0){
					*p_isWDSReloadWIFI = 1;
					strncpy(pre_sys_state.WepKeyStr[i], tmp, sizeof(pre_sys_state.WepKeyStr[i]) - 1);
				}	
			}
		}
	}

	return CFG2_RET_SUCCESS;
}

int wlan11ac_wds_execute(int *p_isWDSReloadWIFI, int *p_WdsActive)
{
	int i 			= 0;
	char APOn[4]	= {0};
	char tmp[128]	= {0};
	char tmp2[128]	= {0};
	
	/*WdsEnable */
	if(cfg_get_object_attr(WLAN11AC_WDS_NODE, "WdsEnable", tmp, sizeof(tmp)) < 0){
		return CFG2_RET_FAIL;
	}else{
		*p_WdsActive = atoi(tmp);
	}

	if(*p_WdsActive != pre_sys_state.WdsEnable_11ac || 
		(!strcmp(APOn,WLAN_APON) && (*p_WdsActive == 1))){
		*p_isWDSReloadWIFI = 1;
		pre_sys_state.WdsEnable = *p_WdsActive;
	}

	/*WdsEncrypType */
	if(cfg_get_object_attr(WLAN11AC_WDS_NODE, "WdsEncrypType", tmp, sizeof(tmp)) < 0){
		return CFG2_RET_FAIL;
	}

	if(strcmp(tmp, pre_sys_state.WdsEncrypType_11ac) != 0){
		*p_isWDSReloadWIFI = 1;
		strncpy(pre_sys_state.WdsEncrypType_11ac, tmp, sizeof(pre_sys_state.WdsEncrypType_11ac) - 1);
	}

	/*WdsKey */
	if(cfg_get_object_attr(WLAN11AC_WDS_NODE, "WdsKey", tmp, sizeof(tmp)) < 0){
		return CFG2_RET_FAIL;
	}

	if(strcmp(tmp, pre_sys_state.WdsKey_11ac) != 0){
		*p_isWDSReloadWIFI = 1;
		strncpy(pre_sys_state.WdsKey_11ac, tmp, sizeof(pre_sys_state.WdsKey_11ac) - 1);
	}

	/*Wds_MAC*/
	for(i = 0; i < MAX_WDS_ENTRY; i++){
		snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d", i);
		if(cfg_get_object_attr(WLAN11AC_WDS_NODE, tmp2, tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}

		if(strcmp(tmp, pre_sys_state.Wds_MAC_11ac[i]) != 0){
			*p_isWDSReloadWIFI = 1;
			strcpy(pre_sys_state.Wds_MAC_11ac[i], tmp);
		}	
	}
	/*main ssid wep key change
	if wds is not enable ,main ssid wep key change ,not reload wifi module
	if wds enable,main ssid wep key change, reload wifi module*/
	if(*p_WdsActive != 0){/*wds enable*/
		if(cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, "AuthMode", tmp, sizeof(tmp)) < 0){
			return CFG2_RET_FAIL;
		}
		if(NULL != strstr(tmp, "WEP")){
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, tmp2, tmp, sizeof(tmp)) < 0){
					/*it will cause wifi unnormal, because the subsequent code will not be executed */
					continue;
				}
				if(strcmp(tmp, pre_sys_state.WepKeyStr_11ac[i]) != 0){
					*p_isWDSReloadWIFI = 1;
					strncpy(pre_sys_state.WepKeyStr_11ac[i], tmp, sizeof(pre_sys_state.WepKeyStr_11ac[i]) - 1);
				}	
			}
		}
	}

	return CFG2_RET_SUCCESS;
}

#endif

#ifdef TCSUPPORT_WLAN
static void downWifiInterface(void)
{
	int i,flag = 0;
	char wifibuf[16] 	= {0};
	char wificmd[64] 	= {0};
	char nodeName[128]	= {0};

#ifdef TCSUPPORT_WLAN_WDS
	snprintf(nodeName, sizeof(nodeName), WLAN_WDS_NODE);
	cfg_get_object_attr(nodeName, "WdsEnable", wifibuf, sizeof(wifibuf));
	flag = atoi(wifibuf);
	if(flag){
		for(i = 0; i < MAX_WDS_ENTRY; i++){
			snprintf(wificmd, sizeof(wificmd), WIFI_DOWN_CMD_WDS, i);
			system(wificmd);
		}
	}
#endif
#ifdef 	TCSUPPORT_WLAN_AC
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_COMMON_NODE);
	cfg_get_object_attr(nodeName, "BssidNum", wifibuf, sizeof(wifibuf));
	flag = atoi(wifibuf);
	for(i = 0; i < flag; i++){
		snprintf(wificmd, sizeof(wificmd), WIFI_DOWN_CMD_RAI, i);
		system(wificmd);
	}

#ifdef TCSUPPORT_WLAN_WDS
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_WDS_NODE);
	cfg_get_object_attr(nodeName, "WdsEnable", wifibuf, sizeof(wifibuf));

	flag = atoi(wifibuf);
	if(flag){
		for(i = 0; i < MAX_WDS_ENTRY; i++){
			snprintf(wificmd, sizeof(wificmd), WIFI_DOWN_CMD_WDSI, i);
			system(wificmd);
		}
	}
#endif
#endif
	return;
}


static void upWifiInterface(void)
{
	int i,flag = 0;
	char wifibuf[16]	= {0};
	char wificmd[64]	= {0};
	char nodeName[128]	= {0};

#ifdef TCSUPPORT_WLAN_WDS
	snprintf(nodeName, sizeof(nodeName), WLAN_WDS_NODE);
	cfg_get_object_attr(nodeName, "WdsEnable", wifibuf, sizeof(wifibuf));
	flag = atoi(wifibuf);
	if(flag){
		for(i = 0; i < MAX_WDS_ENTRY; i++){
			snprintf(wificmd, sizeof(wificmd), WIFI_UP_CMD_WDS, i);
			system(wificmd);
		}
	}
#endif
#ifdef 	TCSUPPORT_WLAN_AC
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_COMMON_NODE);
	cfg_get_object_attr(nodeName, "BssidNum", wifibuf, sizeof(wifibuf));
	flag = atoi(wifibuf);
	for(i = 0; i < flag; i++){
		snprintf(wificmd, sizeof(wificmd), WIFI_UP_CMD_RAI, i);
		system(wificmd);
	}
#ifdef TCSUPPORT_WLAN_WDS
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_WDS_NODE);
	cfg_get_object_attr(nodeName, "WdsEnable", wifibuf, sizeof(wifibuf));
	flag = atoi(wifibuf);
	if(flag){
		for(i = 0; i < MAX_WDS_ENTRY; i++){
			snprintf(wificmd, sizeof(wificmd), WIFI_UP_CMD_WDSI, i);
			system(wificmd);
		}
	}
#endif
#endif
	return;
}

void check_wifi(void)
{
	char string[8] = {0};
	int reset = 0, onoff = 0, ret = 0;

	if(wifimgr_lib_get_AHB_STATUS(string, sizeof(string), &ret) != -1){
		sscanf(string, "%d %d", &onoff, &reset);
		if(reset==1 && onoff==1){
#if 0
#ifdef TCSUPPORT_SYSLOG
			do_wlan_alarm_syslog();
#endif
#endif
#ifdef PCIE_DEBUG
			fprintf(stderr,"%s\n", WIFI_RESET_CMD);
			system("echo cc reset >> /tmp/reset");
#endif
			/*first:down wifi interface*/
			downWifiInterface();
			/*second:do reset*/
			wifimgr_lib_set_AHB_STATUS("1 2\n");
			/*three:up wifi interface*/
			upWifiInterface();

			system(WIFI_RESET_CMD);
			/*Disable Reset*/
			wifimgr_lib_set_AHB_STATUS("1 0\n");
		}
	}
}


void checkWlanBtn(void)
{
	char string[4]		= {0};
	char strAPOn[4] 	= {0};
	char nodeName[128]	= {0};
	int ret, iWlanState, fd;
	char wlan_button_path[64] = {0};

	wifimgr_lib_get_WLAN_BUTTON_PATH(wlan_button_path, sizeof(wlan_button_path));
	ret = wifimgr_lib_get_button_state(WIFI_2_4G, wlan_button_path, string, sizeof(string));
	iWlanState = atoi(string);
	if(ret == 1 && iWlanState == 1)
	{
		snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
		cfg_get_object_attr(nodeName, "APOn", strAPOn, sizeof(strAPOn));
		if(atoi(strAPOn) == 1)
			cfg_set_object_attr(nodeName, "APOn", "0");
		else if(atoi(strAPOn) == 0 )
			cfg_set_object_attr(nodeName, "APOn", "1");
		cfg_commit_object(WLAN_NODE);
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WLAN11AC_COMMON_NODE);
		if(atoi(strAPOn) == 1)
			cfg_set_object_attr(nodeName, "APOn", "0");
		else if(atoi(strAPOn) == 0 )
			cfg_set_object_attr(nodeName, "APOn", "1");
		cfg_commit_object(WLAN11AC_NODE);
		wifimgr_lib_set_WLAN_WPS_BUTTON("0\n");
	}
	else if(ret == 2 && iWlanState >= 1){
		/*wait until unpress the button. shnwind 20110407*/
		while(iWlanState != 0){
			ret = wifimgr_lib_get_button_state(WIFI_2_4G, wlan_button_path, string, sizeof(string));
			if(ret > 0){
				iWlanState = atoi(string);
			}
			sleep(1);
		}
		snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
		cfg_get_object_attr(nodeName, "APOn", strAPOn, sizeof(strAPOn));
		if(atoi(strAPOn) == 1)
			cfg_set_object_attr(nodeName, "APOn", "0");
		else if(atoi(strAPOn) == 0 )
			cfg_set_object_attr(nodeName, "APOn", "1");
		cfg_commit_object(WLAN_NODE);
	}
	else
	{
		fprintf(stderr, "no file %s\n", wlan_button_path);
	}
	return;
}

#endif

#ifdef 	TCSUPPORT_WLAN_AC
void checkWlan11acBtn(void)
{
	char string[4]	= {0};
	char strAPOn[4] = {0};
	int ret, iWlanState, fd;
	char file[32] = {0};

	memset(file, 0, sizeof(file));
	wifimgr_lib_get_WLAN11AC_BUTTON_PATH(file, sizeof(file));
	ret = wifimgr_lib_get_button_state(WIFI_5G, file, string, sizeof(string));
	if(ret > 0)
	{
		iWlanState = atoi(string);	
		if(iWlanState >= 1){	
			/*wait until unpress the button.*/
			while(iWlanState != 0){
				if(wifimgr_lib_get_WLAN11AC_BUTTON(string, sizeof(string), &ret) != -1){
					if(ret){
						iWlanState = atoi(string);
					}
					sleep(1);
				}
			}
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", strAPOn, sizeof(strAPOn));
			if( atoi(strAPOn) == 1 ){
				cfg_set_object_attr(WLAN11AC_COMMON_NODE, "APOn", "0");
			}
			else if(atoi(strAPOn) == 0 ){
				cfg_set_object_attr(WLAN11AC_COMMON_NODE, "APOn", "1");
			}
			cfg_commit_object(WLAN11AC_NODE);
		}
	}
	else if(-1 == ret)
	{
		fprintf(stderr, "no file %s\n", file);
		return;
	}

	return;
}

#endif

#ifdef WSC_AP_SUPPORT
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
void clr_wps_flag(void)
{
	printf("enter clr_wps_flag\n");
	if(wps5g_done && wps24g_done) {
		wps5g_done = 0;
		wps24g_done = 0;
		wps_both_start = 0;
	}
}

static void set_wps_uuid(void)
{
	char cmd[128],*item[2]={"ra0","rai0"};
	int i;
	char cmd_value[64] = {0};
	
	for(i = 0; i < 2;i++){
		memset(cmd, 0, sizeof(cmd));
		memset(cmd_value, 0, sizeof(cmd_value));
		snprintf(cmd_value, sizeof(cmd_value), "WscUUID_E=%s\n", WPS_UUID_VALUE);
		wifimgr_lib_set_WIFI_CMD(item[i], 0, sizeof(cmd), cmd_value, cmd, 0);
		system(cmd);
	}
}
#endif

void check_wpsbtn(void)
{
	char string[4]		= {0};
	char tmp[4]			= {0};
	char nodeName[128] 	= {0};
	int ret;
#if defined(TCSUPPORT_WPS_BTN_NEWSPEC)
	char cmd[50]	= {0};
#endif
	char file[32] = {0};

	if(wifimgr_lib_get_WPS_BUTTON(string, sizeof(string), &ret) != -1){
		if(ret){
			if(1 == atoi(string)){
				snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
				cfg_get_object_attr(nodeName, "APOn", tmp, sizeof(tmp));
				if(!strcmp(tmp, "1")){
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), INFO_WLAN_NODE);
					cfg_set_object_attr(nodeName, "WPSActiveStatus", "1");
					cfg_get_object_attr(nodeName, "wlanWPStimerRunning", tmp, sizeof(tmp));
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
					cfg_commit_object(nodeName);
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
					wps_both_start = 1;
					wps5g_done = 0;
					wps24g_done = 0;
					set_wps_uuid();
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), INFO_WLAN11AC_NODE);
					cfg_set_object_attr(nodeName, "WPSActiveStatus", "1");
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY1_NODE);
					cfg_commit_object(nodeName);
					wifimgr_lib_set_WPS_BUTTON("400\n");
#endif
				}
				wifimgr_lib_set_WPS_BUTTON("0");
			}else if(2 == atoi(string)){
#if !defined(TCSUPPORT_CY)
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
				wps_both_start = 1;
#endif
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
				cfg_get_object_attr(nodeName, "APOn", tmp, sizeof(tmp));
				if(!strcmp(tmp,"1")){
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), INFO_WLAN_NODE);
					cfg_set_object_attr(nodeName, "WPSOOBActive", "1");
					cfg_get_object_attr(nodeName, "wlanWPStimerRunning", tmp, sizeof(tmp));
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
					cfg_commit_object(nodeName);
				}
#if defined(TCSUPPORT_WPS_BTN_DUALBAND)
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), WLAN11AC_COMMON_NODE);
				cfg_get_object_attr(nodeName, "APOn", tmp, sizeof(tmp));
				if(!strcmp(tmp,"1")){
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), INFO_WLAN11AC_NODE);
					cfg_set_object_attr(nodeName, "WPSOOBActive", "1");
					cfg_get_object_attr(nodeName, "wlanWPStimerRunning", tmp, sizeof(tmp));
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY1_NODE);
					cfg_commit_object(nodeName);

				}
#endif
			#endif
				wifimgr_lib_set_WPS_BUTTON("0");
			}

#if defined(TCSUPPORT_NP)
			else if(3 == atoi(string)){
#if defined(TCSUPPORT_ECNT_MAP)
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), MESH_DAT_NODE);
				cfg_get_object_attr(nodeName, "MapEnable", tmp, sizeof(tmp));
				if(!strcmp(tmp,"1")){
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), MESH_ACTION_NODE);
					cfg_set_object_attr(nodeName, "wifi_trigger_onboarding", "1");
					cfg_commit_object(nodeName);

				}
#endif
				wifimgr_lib_set_WPS_BUTTON("0");
			}
#endif
#if defined(TCSUPPORT_WPS_BTN_NEWSPEC)
			else if (4 == atoi(string)){
				memset(tmp, 0, sizeof(tmp));
				memset(nodeName, 0, sizeof(nodeName));
				snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
				cfg_get_object_attr(nodeName, "APOn", tmp, sizeof(tmp));
				if(!strcmp(tmp,"1")){
					memset(tmp, 0, sizeof(tmp));
					memset(nodeName, 0, sizeof(nodeName));
					snprintf(nodeName, sizeof(nodeName), INFO_WLAN_NODE);
					cfg_get_object_attr(nodeName, "wlanWPStimerRunning", tmp, sizeof(tmp));
					if (!atoi(tmp)){ /*start pbc*/
						cfg_set_object_attr(nodeName, "WPSActiveStatus", "1");
						memset(nodeName, 0, sizeof(nodeName));
						snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
						cfg_set_object_attr(nodeName, "WPSMode", "1");
						cfg_commit_object(nodeName);
					}else { /*stop last pbc connection and start a new connection*/
						cfg_set_object_attr(nodeName, "wlanWPStimerRunning", "0");
						wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "WscStop", cmd, 0);
						system(cmd);
						memset(cmd, 0, sizeof(cmd));
						snprintf(cmd, sizeof(cmd), "%d\n", WPSSTOP);
						wifimgr_lib_set_WPS_BUTTON(cmd);
						cfg_set_object_attr(nodeName, "WPSActiveStatus", "1");
						memset(nodeName, 0, sizeof(nodeName));
						snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
						cfg_set_object_attr(nodeName, "WPSMode", "1");
						cfg_commit_object(nodeName);
					}
				}
				wifimgr_lib_set_WPS_BUTTON("0");
			}
#endif	
		}
	}else{
		wifimgr_lib_get_WPS_BUTTON_PATH(file, sizeof(file));
		fprintf(stderr,"no file %s\n", file);
		return;
	}
	return;
}

#if defined(TCSUPPORT_WPS_5G_BTN)	
void check_wps5gbtn(void)
{
	char string[4]	= {0};
	char buf[32]	= {0};
	char tmp[4] 	= {0};
	int fd, ret;
#if defined(TCSUPPORT_WPS_BTN_NEWSPEC)
	char cmd[50]={0};
#endif
	char file[32]	= {0};

	wifimgr_lib_get_WPS_5G_BUTTON_PATH(file, sizeof(file));
	ret = wifimgr_lib_get_button_state(WIFI_5G, file, string, sizeof(string));
	if(ret > 0)
	{
		if(atoi(string) == 1){
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", tmp, sizeof(tmp));		
			if(!strcmp(tmp, "1")){
				cfg_set_object_attr(INFO_WLAN11AC_NODE, "WPSActiveStatus", "1");
				cfg_get_object_attr(INFO_WLAN11AC_NODE, "wlanWPStimerRunning", tmp, sizeof(tmp));
				cfg_set_object_attr(WLAN11AC_ENTRY1_NODE, "WPSMode", "1");
				cfg_commit_object(WLAN11AC_ENTRY1_NODE);
			}
			wifimgr_lib_set_WPS_5G_BUTTON("0");
		}else if(atoi(string) == 2){
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", tmp, sizeof(tmp));
			if(!strcmp(tmp, "1")){
				cfg_set_object_attr(INFO_WLAN11AC_NODE, "WPSOOBActive", "1");
				cfg_get_object_attr(INFO_WLAN11AC_NODE, "wlanWPStimerRunning", tmp, sizeof(tmp));
				cfg_commit_object(WLAN11AC_ENTRY1_NODE);
			}			
			wifimgr_lib_set_WPS_5G_BUTTON("0");
		}
#if defined(TCSUPPORT_WPS_BTN_NEWSPEC)
		else if (atoi(string) == 4) {
			memset(tmp, 0, sizeof(tmp));
			cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", tmp, sizeof(tmp));
			if(!strcmp(tmp,"1")) {
				memset(buf, 0, sizeof(buf));
				memset(tmp, 0, sizeof(tmp));
				cfg_get_object_attr(INFO_WLAN11AC_NODE, "wlanWPStimerRunning", tmp, sizeof(tmp));
				if (!atoi(tmp)) { /*start pbc*/
					cfg_set_object_attr(INFO_WLAN11AC_NODE, "WPSActiveStatus", "1");
					cfg_set_object_attr(WLAN11AC_ENTRY1_NODE, "WPSMode", "1");
					cfg_commit_object(WLAN11AC_ENTRY1_NODE);
				}
				else { /*stop last pbc connection and start a new connection*/
					cfg_set_object_attr(INFO_WLAN11AC_NODE, "wlanWPStimerRunning", "0");
					wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "WscStop", cmd, 0);
					system(cmd);
					memset(cmd, 0, sizeof(cmd));
					snprintf(cmd, sizeof(cmd), "%d\n", WPS5GSTOP);
					wifimgr_lib_set_WPS_5G_BUTTON(cmd);
					cfg_set_object_attr(INFO_WLAN11AC_NODE, "WPSActiveStatus", "1");
					cfg_set_object_attr(WLAN11AC_ENTRY1_NODE, "WPSMode", "1");
					cfg_commit_object(WLAN11AC_ENTRY1_NODE);
				}
			}
			wifimgr_lib_set_WPS_5G_BUTTON("0");
		}
#endif
	}
	else if(-1 == ret)
	{
		fprintf(stderr,"no file %s\n", file);
		return;
	}

	return;
}

#endif
#endif


#if defined(TCSUPPORT_CT_BUTTONDETECT)
/*to check wlan & wps button's click/double click/long click*/
void check_wlan_wps_button(void)
{
	int n = 0;
	/*args[0]:	1 :wlan button;  2: wps button;*/
	/*args[1]:	1:click; 2:double click; 3: long click.*/
	int args[2] = {0};

	args[0] = 0;
	n = wifimgr_lib_get_LED_BUTTON_STATUS(args);
	if(n == 0){		
		if(args[0] != 0){
#if defined(TCSUPPORT_CT_MIDWARE)	
			signalCwmpInfo(BUTTON_DETECT, args[0], args[1]);
#endif		
		}
	}

	return;
}	
#endif	

#if defined(TCSUPPORT_CT_JOYME)
void check_wpsstatus(void)
{
	char nodeName[128] 	= {0};
	char tmp[64] 		= {0};
	int i = 0, number = 0, k=0;
	char *ipbuf[MAX_LEN_OF_MAC_TABLE];
	static char ipbuf1[MAX_LEN_OF_MAC_TABLE][20];
	static int idle_number = 0,isconfigured = 0,time = 0;
	int isintable = 0;
	FILE *fp = NULL;

	for(i = 0; i < MAX_LEN_OF_MAC_TABLE; i++){
		ipbuf[i] = (char *)malloc(20);
		if(ipbuf[i] == NULL)
			goto FreeBUF;
		memset(ipbuf[i],0,sizeof(*ipbuf[i]));
	}
	snprintf(nodeName, sizeof(nodeName), INFO_WLAN_NODE);
	cfg_get_object_attr(nodeName, "wlanWPSStatus", tmp, sizeof(tmp));
	if(!strcmp(tmp, "Configured")){
		if(1 == isconfigured)
			goto FreeBUF;

		fp = fopen(WPS_ADD_IP_PATH, "w");
		if(fp == NULL)
			goto FreeBUF;
		memset(nodeName, 0, sizeof(nodeName));
		cfg_get_object_attr(nodeName, "NUM0", tmp, sizeof(tmp));
		number = atoi(tmp);
		for(i = 0; i < number; i++){
			memset(nodeName, 0, sizeof(nodeName));
#if defined(TCSUPPORT_CT_PON)
	 		snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, 1, i+1);
			cfg_get_object_attr(nodeName, "IP", tmp, sizeof(tmp));
#else
	 		snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_ENTRY_NODE, i+1);
			cfg_get_object_attr(nodeName, "IP0", tmp, sizeof(tmp));
#endif
			snprintf(ipbuf[i], sizeof(*ipbuf[i]), "%s", tmp);
			for(k = 0; k < idle_number; k++){
				if(!strcmp(ipbuf[i], ipbuf1[k])){
					isintable = 1;
					break;
				}
			}
			if(isintable == 0){
				fprintf(fp,"%s\n",ipbuf[i]);
			}
			isintable = 0;
			
		}
		isconfigured = 1;
	}
	else if(!strcmp(tmp, "In progress")){
		if(time++ < 2)
			goto FreeBUF;
		time = 0;
		isconfigured = 0;
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_COMMON_NODE);
		cfg_get_object_attr(nodeName, "NUM0", tmp, sizeof(tmp));
		idle_number = atoi(tmp);

		for(i = 0;i<idle_number;i++){
			memset(ipbuf1[i], 0, sizeof(ipbuf1[i]));
			memset(nodeName, 0, sizeof(nodeName));
#if defined(TCSUPPORT_CT_PON)
			snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, 1, i+1);
			cfg_get_object_attr(nodeName, "IP", tmp, sizeof(tmp));
#else
			snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_ENTRY_NODE, i+1);
			cfg_get_object_attr(nodeName, "IP0", tmp, sizeof(tmp));
#endif
			snprintf(ipbuf1[i], sizeof(ipbuf1[i]), "%s", tmp);
		}
		for(i = idle_number; i < MAX_LEN_OF_MAC_TABLE; i++)
			memset(ipbuf1[i], 0, sizeof(ipbuf1[i]));
	}

FreeBUF:
	for(i = 0; i < MAX_LEN_OF_MAC_TABLE; i++){
		if(NULL != ipbuf[i]){
			free(ipbuf[i]);
			ipbuf[i] = NULL;
		}
	}
	if(NULL != fp)
		fclose(fp);
	return;
}
#endif

#if defined(TCSUPPORT_CT_PON)
#ifdef WSC_AP_SUPPORT	
static int wpsStatusUpdate(void)
{	
	char nodeName[32] = {0};
	unsigned short output = 0;
			
	wifimgr_lib_get_WPS_STATUS(0, &output);
	snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
	if(!WscOOBSeted){
		if(output != 2){ 	
			/*wps not config*/
			cfg_set_object_attr(nodeName, "WPSConfStatus", "1");
			return 0;
		}
		cfg_set_object_attr(nodeName, "WPSConfStatus", "2");
	}else{
		cfg_set_object_attr(nodeName, "WPSConfStatus", "1");
		WscOOBSeted=0;
	}

	return 0;
}

static int wpsACLInfoUpdate(void)
{
	char nodeName[128] 		= {0};
	char wscV2Support[8] 	= {0};
	char accessPolicy[8] 	= {0};	
	RT_802_11_ACL * AclInfor= NULL;
	char AclList[WLAN_MAC_NUM][18];
	char WLan_MAC[11]		= {0};
	char WLan_MAC_Addr[18]	= {0};
	int i = 0, j = 0;
	int sameaddrstat = 0;
	snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
	cfg_get_object_attr(nodeName, "WscV2Support", wscV2Support, sizeof(wscV2Support));
	cfg_get_object_attr(nodeName, "AccessPolicy", accessPolicy, sizeof(accessPolicy));
	
	/*wscV2Support=1 means wps 2.0 being enabled, accessPolicy=1 means Access policy is "Allow"*/
	if(!strcmp(wscV2Support, "1") && !strcmp(accessPolicy, "1")){
		AclInfor = (RT_802_11_ACL *)malloc(sizeof(RT_802_11_ACL));
		if (!AclInfor)
			return -1;

		wifimgr_lib_get_WPS_ACL_INFO(0, 0, AclInfor);
		for (i = 0; i < WLAN_MAC_NUM; i++){
			sameaddrstat = 0;
			snprintf(AclList[i], sizeof(AclList[i]), "%02x:%02x:%02x:%02x:%02x:%02x",
					AclInfor->Entry[i].Addr[0],
					AclInfor->Entry[i].Addr[1],
					AclInfor->Entry[i].Addr[2],
					AclInfor->Entry[i].Addr[3],
					AclInfor->Entry[i].Addr[4],
					AclInfor->Entry[i].Addr[5]);
			if(strcmp(AclList[i], "00:00:00:00:00:00")){
				for (j = 0; j < WLAN_MAC_NUM; j++){
					snprintf(WLan_MAC, sizeof(WLan_MAC), "WLan_MAC%d", j); 
					cfg_get_object_attr(nodeName, WLan_MAC, WLan_MAC_Addr, sizeof(WLan_MAC_Addr));
					if(!strcmp(WLan_MAC_Addr, AclList[i])){
						sameaddrstat = 1;
						break;
					}
				}
				
				if(!sameaddrstat){
					for (j = 0; j < WLAN_MAC_NUM; j++){
						snprintf(WLan_MAC, sizeof(WLan_MAC), "WLan_MAC%d", j); 
						cfg_get_object_attr(nodeName, WLan_MAC, WLan_MAC_Addr, sizeof(WLan_MAC_Addr));
						if( (!strcmp(WLan_MAC_Addr,"00:00:00:00:00:00") 
							|| !strcmp(WLan_MAC_Addr,""))){
							cfg_set_object_attr(nodeName, WLan_MAC, AclList[i]);
						}
					}
				}
			}
		}
		if (AclInfor)
		{
			free(AclInfor);
			AclInfor = NULL;
		}
	}
	return 0;
}

#if defined(TCSUPPORT_WLAN_AC)
static int wps11acStatusUpdate(void)
{	
	char nodeName[128] = {0};
	unsigned short output = 0;
	
	wifimgr_lib_get_WPS_STATUS(1, &output);	
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY1_NODE);
	if(!WscOOBSeted_ac){
		if(output != 2){ 	
			/*wps not config*/
			cfg_set_object_attr(nodeName, "WPSConfStatus", "1");
			return 0;
		}
		cfg_set_object_attr(nodeName, "WPSConfStatus", "2");
	}else{
		cfg_set_object_attr(nodeName, "WPSConfStatus", "1");
		WscOOBSeted_ac = 0;
	}

	return 0;
}


static int wps11acACLInfoUpdate(void)
{
	char nodeName[128] 		= {0};
	char wscV2Support[8] 	= {0};
	char accessPolicy[8] 	= {0};	
	RT_802_11_ACL * AclInfor= NULL;
	char AclList[WLAN_MAC_NUM][18];
	char WLan_MAC[11]		= {0};
	char WLan_MAC_Addr[18]	= {0};
	int i = 0, j = 0;
	int sameaddrstat 	= 0;
	
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY1_NODE);
	cfg_get_object_attr(nodeName, "WscV2Support", wscV2Support, sizeof(wscV2Support));
	cfg_get_object_attr(nodeName, "AccessPolicy", accessPolicy, sizeof(accessPolicy));
	/*wscV2Support=1 means wps 2.0 being enabled, accessPolicy=1 means Access policy is "Allow"*/
	if(!strcmp(wscV2Support,"1") && !strcmp(accessPolicy,"1")){
		AclInfor = (RT_802_11_ACL *)malloc(sizeof(RT_802_11_ACL));
		if (!AclInfor)
			return -1;
	
		wifimgr_lib_get_WPS_ACL_INFO(1, 0, AclInfor);
		for (i=0;i<WLAN_MAC_NUM;i++){//i:wifi driver acl_entry num
			sameaddrstat = 0;
			/*tcdbg_printf("AclInfor->Entry[%d].Addr: %02x:%02x:%02x:%02x:%02x:%02x \n"
				,i,AclInfor->Entry[i].Addr[0],AclInfor->Entry[i].Addr[1],
				AclInfor->Entry[i].Addr[2],AclInfor->Entry[i].Addr[3],
				AclInfor->Entry[i].Addr[4],AclInfor->Entry[i].Addr[5]);
				*/
				
			snprintf(AclList[i], sizeof(AclList[i]), "%02x:%02x:%02x:%02x:%02x:%02x",
					AclInfor->Entry[i].Addr[0],
					AclInfor->Entry[i].Addr[1],
					AclInfor->Entry[i].Addr[2],
					AclInfor->Entry[i].Addr[3],
					AclInfor->Entry[i].Addr[4],
					AclInfor->Entry[i].Addr[5]);
			if(strcmp(AclList[i],"00:00:00:00:00:00")){
				for (j = 0; j < WLAN_MAC_NUM; j++){      
					snprintf(WLan_MAC, sizeof(WLan_MAC), "WLan_MAC%d", j); 
					cfg_get_object_attr(nodeName, WLan_MAC, WLan_MAC_Addr, sizeof(WLan_MAC_Addr));
					if(!strcmp(WLan_MAC_Addr,AclList[i])){
						sameaddrstat = 1;
						break;
					}
				}
				
				if(!sameaddrstat){
					for (j = 0; j < WLAN_MAC_NUM; j++){
						snprintf(WLan_MAC, sizeof(WLan_MAC), "WLan_MAC%d", j); 
						cfg_get_object_attr(nodeName, WLan_MAC, WLan_MAC_Addr, sizeof(WLan_MAC_Addr));
						if((!strcmp(WLan_MAC_Addr,"00:00:00:00:00:00") 
							|| !strcmp(WLan_MAC_Addr,""))){
							cfg_set_object_attr(nodeName, WLan_MAC, "");
							break;
						}
					}
				}
			}
			else{
				snprintf(WLan_MAC, sizeof(WLan_MAC), "WLan_MAC%d", i); 
				cfg_set_object_attr(nodeName, WLan_MAC, AclList[i]);
			}
		}
			
		if (AclInfor)
		{
			free(AclInfor);
			AclInfor = NULL;
		}
	}		
	return 0;
}
#endif

void wlanNodeUpdate(void)
{
	char nodeName[128] 	= {0};
	char confMode[2] 	= {0};

	snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY1_NODE);
	if(cfg_get_object_attr(nodeName, "WPSConfMode", confMode, sizeof(confMode)) <= 0){
		printf("wlanNodeUpdate: cfg_get_object_attr get WPSConfMode fail\n");
		return ;
	}
	if(!strcmp(confMode,"0")){
		/*no need update wps stauts*/
		return ;
	}
	wpsStatusUpdate();
	wpsACLInfoUpdate();

#if defined(TCSUPPORT_WLAN_AC)
	snprintf(nodeName, sizeof(nodeName), WLAN11AC_ENTRY1_NODE);
	if(cfg_get_object_attr(nodeName, "WPSConfMode", confMode, sizeof(confMode)) <= 0){
		printf("wlanNodeUpdate: cfg_get_object_attr get WPSConfMode fail\n");
		return ;
	}
	if(!strcmp(confMode,"0")){
		/*no need update wps stauts*/
		return ;
	}

	wps11acStatusUpdate();
	wps11acACLInfoUpdate();
#endif

	return ;
}
#endif	


#if defined(TCSUPPORT_CMCCV2)	
static int checkBlackList(char *mac)
{
	char nodeName[128] 		= {0};
	char ActiveMac[4] 		= {0};
	char FilterType[10] 	= {0};
	char strMac[20] 		= {0};
	char totalMacNum[4] 	= {0};
	char BlackOrWhite[10] 	= {0};
	int i = 0, maxNum = 0;
	
	if(cfg_get_object_attr(IPMACFILTER_COMMON_NODE, "ActiveMac", ActiveMac, sizeof(ActiveMac)) <= 0){
		return 0;
	}
	if(strcmp(ActiveMac, "1") != 0){
		return 0;
	}
	if(cfg_get_object_attr(IPMACFILTER_COMMON_NODE, "IpMacType", FilterType, sizeof(FilterType)) <= 0){
		return 0;
	}
	if(strcmp(FilterType, "Mac") != 0){
		return 0;
	}
	if(cfg_get_object_attr(IPMACFILTER_COMMON_NODE, "ListTypeMac", BlackOrWhite, sizeof(BlackOrWhite)) <= 0){
		return 0;	
	}
	if(strcmp(BlackOrWhite, "Black") != 0){
		return 0;
	}
	
	cfg_get_object_attr(IPMACFILTER_NODE, "mac_num", totalMacNum, sizeof(totalMacNum));
	maxNum = atoi(totalMacNum);
	for(i = 0; i < maxNum; i++){
		snprintf(nodeName, sizeof(nodeName), IPMACFILTER_ENTRY_N_NODE, i+1);
		if(cfg_get_object_attr(nodeName, "MacAddr", strMac, sizeof(strMac)) > 0){
			if(!strncasecmp(strMac, mac, sizeof(strMac))){
				return 1;
			}
		}
	}
	return 0;
}
#endif

static int notifyWLanHostMacs(mac_table_t *p_mac_tab_old, mac_table_t *p_mac_tab_new, int ssid_index)
{
	int idx = 0, dstidx = 0, is_found 	= 0, sendFlag = 0;
	int isMark[MAX_LEN_OF_MAC_TABLE] 	= {0};
	mac_entry_t *pCheckEntry = NULL, *pDstEntry = NULL;
#if defined(TCSUPPORT_CMCCV2)	
	char mac_s[20] = {0};
#endif
	int macTableChange = 0;
	int rssi_change = 0;
	static int update_rssi_cnt[MAX_DUALBAND_BSSID_NUM] = {0};
	
	if ( NULL == p_mac_tab_new || NULL == p_mac_tab_old )
		return -1;

	memset(isMark, 0, sizeof(isMark));
	for (idx = 0; idx < p_mac_tab_new->Num && idx < MAX_LEN_OF_MAC_TABLE; idx++){
		pCheckEntry = &(p_mac_tab_new->macEntry[idx]);
		if (NULL == pCheckEntry )
			continue;

		is_found = 0;
#if defined(TCSUPPORT_CMCCV2)
		sendFlag = 0;
#endif
		for (dstidx = 0; dstidx < p_mac_tab_old->Num && dstidx < MAX_LEN_OF_MAC_TABLE; dstidx ++){
			pDstEntry = &(p_mac_tab_old->macEntry[dstidx]);
			if (NULL == pDstEntry )
			{
				continue;
			}

			if (0 == memcmp(pCheckEntry->Addr, pDstEntry->Addr, MAC_ADDR_LENGTH)){
#ifdef TCSUPPORT_CMCC_WLAN_FORCEROAM
#if defined(TCSUPPORT_CT_PHONEAPP)
				if(pCheckEntry->AvgRssi0 != pDstEntry->AvgRssi0 || pCheckEntry->AvgRssi1 != pDstEntry->AvgRssi1){
					macTableChange |= 0x2;
				}				
#endif			
#endif
#if defined(TCSUPPORT_CMCCV2)
			if ((pCheckEntry->AvgRssi != pDstEntry->AvgRssi) || (pCheckEntry->DataRate!= pDstEntry->DataRate))
			{
				macTableChange |= 0x2;
			}
#endif
#if defined(TCSUPPORT_CT_JOYME2)
				//if(pCheckEntry->AvgRssi0 != pDstEntry->AvgRssi0 || pCheckEntry->AvgRssi1 != pDstEntry->AvgRssi1 || pCheckEntry->AvgRssi2 != pDstEntry->AvgRssi2)
				if (pCheckEntry->AvgRssi != pDstEntry->AvgRssi)
				{
					rssi_change = 1;
					macTableChange |= 0x2;
				}				
#endif		
				isMark[dstidx] = 1;
				is_found = 1;
#if defined(TCSUPPORT_CMCCV2)				
				if (pCheckEntry->PortSecured == 1 && pDstEntry->PortSecured == 2) {
					sendFlag = 1;
					macTableChange |= 0x1;
				}
#endif

				break;
			}
		}
#if defined(TCSUPPORT_CT_JOYME2)
		if (1 == rssi_change) {
			if (update_rssi_cnt[ssid_index] == 0 || update_rssi_cnt[ssid_index] > 5) {
			wifi_mac_entry_update(pCheckEntry, ssid_index);
				update_rssi_cnt[ssid_index] = 1;
			rssi_change = 0;
		}
			update_rssi_cnt[ssid_index]++;
		}
#endif
		/* check found. */
		if (0 == is_found){
			macTableChange |= 0x1;
#if defined(TCSUPPORT_CMCCV2)	
			/*WPA_802_1X_PORT_SECURED == 1 wifi connect success*/
			if (pCheckEntry->PortSecured == 1) {
			sendFlag = 1;
			}
#endif
		}
		
#if defined(TCSUPPORT_CMCCV2)
		if (sendFlag == 1) 
		{
			snprintf(mac_s, sizeof(mac_s)
				, "%02x:%02x:%02x:%02x:%02x:%02x"
				, pCheckEntry->Addr[0], pCheckEntry->Addr[1]
				, pCheckEntry->Addr[2], pCheckEntry->Addr[3]
				, pCheckEntry->Addr[4], pCheckEntry->Addr[5]);
			if(checkBlackList(mac_s))
			{
				sendEventMessage(5, mac_s, ssid_index); 
			}else{
				sendEventMessage(2, mac_s, ssid_index);
			}
		}
#endif
	
#if defined(TCSUPPORT_CT_JOYME2)
		if (0 == is_found)
			wifi_mac_entry_add(pCheckEntry, ssid_index);
#endif
	}

	/* check offline dev */
	for (dstidx = 0; dstidx < p_mac_tab_old->Num; dstidx ++){
		pDstEntry = &(p_mac_tab_old->macEntry[dstidx]);
		if (NULL == pDstEntry)
			continue;
		if (0 == isMark[dstidx]){
			macTableChange |= 0x1;		
#if defined(TCSUPPORT_CMCCV2)	
			snprintf(mac_s, sizeof(mac_s)
				, "%02x:%02x:%02x:%02x:%02x:%02x"
				, pDstEntry->Addr[0], pDstEntry->Addr[1]
				, pDstEntry->Addr[2], pDstEntry->Addr[3]
				, pDstEntry->Addr[4], pDstEntry->Addr[5]);
			if (pDstEntry->PortSecured == 1) 
			{
			    sendEventMessage(3, mac_s, ssid_index);
			}
			else if(pDstEntry->PortSecured == 2)
			{
			    sendEventMessage(5, mac_s, ssid_index);
			}
#endif		
#if defined(TCSUPPORT_CT_JOYME2)
			wifi_mac_entry_del(pDstEntry, ssid_index);
#endif
		}
	}

	return macTableChange;
}

static int getIpFromArp(char *macAddr, char *ip)
{
	FILE *fp 		= NULL;
	char cmd[64]	= {0};
	char buf[100]	= {0};
	char mactemp[18]= {0};
	int i	= 0;

	strncpy(mactemp, macAddr, sizeof(mactemp)-1);
	for(i = 0; i < 18; i ++){
	   mactemp[i] = tolower(mactemp[i]);
	}
	
	snprintf(cmd, sizeof(cmd), "/bin/cp /proc/net/arp %s", ARP_TABLE_PATH);
	system(cmd);

	fp = fopen(ARP_TABLE_PATH, "r");
	if( NULL == fp )
		return -1;
	
	while(fgets(buf, 100, fp) != NULL){		
	
		if((strstr(buf, macAddr) != NULL) ||(strstr(buf, mactemp) != NULL)){
			strncpy(ip, buf, 15); /*IP addr max length is 15.*/
			fclose(fp);
			return 1;
		}
		memset(buf,0,sizeof(buf));
	}
	fclose(fp);
	return 0;
}

unsigned int wifi_mab_ip_flag[MAX_DUALBAND_BSSID_NUM];
int wifi_mac_entry_update(mac_entry_t *mac_entry_new, int ssid_index)
{	
	char mac_entry_new_mac[20] = {0};
	char wifi_entry_mac[20] = {0};
	char node_name[64] = {0};
	char buf[20] = {0};
	int i = 0, j = 0;
	
	for (i = 0; i < MAX_STA_NUM; i++) {
		/* find empty entry */
		snprintf(node_name, sizeof(node_name), WIFIMACTAB_PVC_ENTRY_NODE, ssid_index + 1, i + 1);
	
		if(cfg_query_object(node_name, NULL, NULL) < 0)
			continue;
		memset(wifi_entry_mac, 0, sizeof(wifi_entry_mac));
		cfg_get_object_attr(node_name, "MAC", wifi_entry_mac, sizeof(wifi_entry_mac));
		snprintf(mac_entry_new_mac, sizeof(mac_entry_new_mac)
				, "%02x:%02x:%02x:%02x:%02x:%02x"
				, mac_entry_new->Addr[0], mac_entry_new->Addr[1]
				, mac_entry_new->Addr[2], mac_entry_new->Addr[3]
				, mac_entry_new->Addr[4], mac_entry_new->Addr[5]);

		for (j = 0; j < strlen(mac_entry_new_mac); j++)
		{
			if (mac_entry_new_mac[j] >= 'a' && mac_entry_new_mac[j] <= 'z')
			{
				mac_entry_new_mac[j] -= 32;
			}
		}
		if (0 == strcmp(wifi_entry_mac, mac_entry_new_mac))
			break;
	}
	
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2)
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi0);
	cfg_set_object_attr(node_name, "RSSIA", buf);
	
	memset(buf, 0, sizeof(buf) );
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi1);
	cfg_set_object_attr(node_name, "RSSIB", buf);
	
	memset(buf, 0, sizeof(buf) );
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi2);
	cfg_set_object_attr(node_name, "RSSIC", buf);
#endif

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
	memset(buf, 0, sizeof(buf) );
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi);
	cfg_set_object_attr(node_name, "RSSI", buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d",  mac_entry_new->DataRate);
	cfg_set_object_attr(node_name, "DataRate", buf);
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	char rdata[64] = {0};
	int ret, rssi = 0;
	memset(rdata, 0, sizeof(rdata));
#if 0
	rssi = get_rssi2(mac_entry_new->AvgRssi0, 
					 mac_entry_new->AvgRssi1, 
					 mac_entry_new->AvgRssi2);
#else
	rssi = mac_entry_new->AvgRssi;
#endif

#if defined(TCSUPPORT_CT_UBUS)	
		if(ENTRY_CAT_AP == mac_entry_new->EntryType)
			return 0;
#endif	

	snprintf(rdata, sizeof(rdata), "update;%d;%d;%s;%d;%d", ssid_index, i, wifi_entry_mac, rssi, mac_entry_new->DataRate);
#if 0//
	tcdbg_printf("\r\nwifi_mac_entry_update(): rss0=%d, rss1=%d, rssi2=%d, rssi=%d.", 
				mac_entry_new->AvgRssi0, 
				mac_entry_new->AvgRssi1,
				mac_entry_new->AvgRssi2,
				rssi);
#endif
	ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_UPDATESTAINFO, rdata, strlen(rdata) + 1);
#endif

	return 0;
	/* update rssi */	
}
static void cwmpInfo_wifi(void)
{	
	cwmp_msg_t message;
	memset(&message, 0, sizeof(message));
	message.cwmptype = WIFIMACTAB_REINIT;
	if(sendmegq(1, &message, 0) < 0){
		printf("\r\n[%s]send message to cwmp error!!",__FUNCTION__);
	}
}

int wifi_mac_entry_add(mac_entry_t *mac_entry_new, int ssid_index)
{	
	struct timespec time;
	char node_name[64] = {0};
	char mac_buf[32] = {0}, mac_val[32] = {0};
	char ip_addr[32] = {0}, buf[32] = {0};
	int i = 0, ret = 0;
	char rdata[64] = {0};
	char wpsRunning[32] = {0};
	char NUM[8] = {0}, pvc[8] = {0};
	int pvc_stanum = 0;
	
	for (i = 0; i < MAX_STA_NUM; i++) {
		/* find empty entry */
		snprintf(node_name, sizeof(node_name), WIFIMACTAB_PVC_ENTRY_NODE, ssid_index + 1, i + 1);
	
		if(cfg_query_object(node_name, NULL, NULL) < 0)
			break;
	}
	if (i == MAX_STA_NUM) {
		return -1;
	}

	/* create new entry and set new value */
	if (cfg_check_create_object(node_name) == -1){
		return -1;
	}
		
	
	snprintf(mac_val, sizeof(mac_val), "%02X:%02X:%02X:%02X:%02X:%02X", 
										mac_entry_new->Addr[0],
										mac_entry_new->Addr[1],
										mac_entry_new->Addr[2],
										mac_entry_new->Addr[3],
										mac_entry_new->Addr[4],
										mac_entry_new->Addr[5]);
	memset(ip_addr, 0, sizeof(ip_addr));
	getIpFromArp(mac_val, ip_addr);
	if (strlen(ip_addr) > 0) {
		wifi_mab_ip_flag[ssid_index] |= (1 << i);
	}
	
	/* set attr here */
	cfg_set_object_attr(node_name, "MAC", mac_val);
	cfg_set_object_attr(node_name, "IP", ip_addr);
	snprintf(NUM, sizeof(NUM), "NUM%d", ssid_index);
	if (0 > cfg_get_object_attr(WIFIMACTAB_COMMON_NODE, NUM, buf, sizeof(buf)))
	{
		cfg_set_object_attr(WIFIMACTAB_COMMON_NODE, NUM, "1");
	}
	else
	{
		pvc_stanum = atoi(buf);
		pvc_stanum += 1;
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%d", pvc_stanum);
		cfg_set_object_attr(WIFIMACTAB_COMMON_NODE, NUM, buf);
	}

#if defined(TCSUPPORT_CT_PHONEAPP)
	memset(buf, 0, sizeof(buf) );
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi0);
	cfg_set_object_attr(node_name, "RSSIA", buf);
	
	memset(buf, 0, sizeof(buf) );
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi1);
	cfg_set_object_attr(node_name, "RSSIB", buf);
	
	memset(buf, 0, sizeof(buf) );
	snprintf(buf, sizeof(buf), "%d", mac_entry_new->AvgRssi2);
	cfg_set_object_attr(node_name, "RSSIC", buf);
	
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d",  mac_entry_new->DataRate);
	cfg_set_object_attr(node_name, "DataRate", buf);
#endif
#endif	
#if defined(TCSUPPORT_CT_UBUS)
	if(ENTRY_CAT_AP == mac_entry_new->EntryType)
		return 0;
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	memset(buf, 0, sizeof(buf));
	clock_gettime(CLOCK_MONOTONIC, &time);
	snprintf(buf, sizeof(buf), "%ld", time.tv_sec);
	cfg_set_object_attr(node_name, "Monotonictime", buf);

	/* send event to dbus, msg format "cmd:ssidindex:index", cmd is add or del, index is entry index */
	//if (ssid_index == 0 || ssid_index == 8) {
		memset(rdata, 0, sizeof(rdata));
		snprintf(rdata, sizeof(rdata), "add;%d;%d;%s", ssid_index, i, mac_val);
		ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_UPDATESTAINFO, rdata, strlen(rdata) + 1);
		tcdbg_printf("\r\nwifi_mac_entry_add(): send msg to lanhost.");
	//}
		
	signalCwmpInfo(WIFIMACTAB_REINIT, 0, 0);
#endif
	return 0;
}

int wifi_mac_entry_del(mac_entry_t *mac_entry_old, int ssid_index)
{	
	char node_name[64] = {0};
	char mac_val_del[32], mac_val[32] = {0};
	int i;
	int ret = 0;
	char rdata[32] = {0}, buf[32] = {0};
	char NUM[8] = {0};
	int pvc_stanum = 0;
	unsigned long long one = 1;


	snprintf(mac_val_del, sizeof(mac_val_del), "%02X:%02X:%02X:%02X:%02X:%02X", 
										mac_entry_old->Addr[0],
										mac_entry_old->Addr[1],
										mac_entry_old->Addr[2],
										mac_entry_old->Addr[3],
										mac_entry_old->Addr[4],
										mac_entry_old->Addr[5]);
	for (i = 0; i < MAX_STA_NUM; i++) {
		snprintf(node_name, sizeof(node_name), WIFIMACTAB_PVC_ENTRY_NODE, ssid_index + 1, i + 1);
		if (cfg_get_object_attr(node_name, "MAC", mac_val, sizeof(mac_val)) == -1) {
			continue;
		}

		if (!strcmp(mac_val_del, mac_val)) {
			break;
		}
		}

	if (i == 64)
		return -1;

	cfg_delete_object(node_name);
	snprintf(NUM, sizeof(NUM), "NUM%d", ssid_index);
	cfg_get_object_attr(WIFIMACTAB_COMMON_NODE, NUM, buf, sizeof(buf));
	pvc_stanum = atoi(buf);
	pvc_stanum -= 1;
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d", pvc_stanum);
	cfg_set_object_attr(WIFIMACTAB_COMMON_NODE, NUM, buf);
	wifi_mab_ip_flag[ssid_index] &= ~(one << i);
#if defined(TCSUPPORT_CT_UBUS)	
	if(ENTRY_CAT_AP == mac_entry_old->EntryType)
		return 0;
#endif
#if defined(TCSUPPORT_CT_JOYME2)
	/* send msg to dbus */
	//if (ssid_index == 0 || ssid_index == 8) {
		memset(rdata, 0, sizeof(rdata));
		snprintf(rdata, sizeof(rdata), "del;%d;%d;%s", ssid_index, i, mac_val_del);
		ret = ecnt_event_send(ECNT_EVENT_WIFI, ECNT_EVENT_WIFI_UPDATESTAINFO, rdata, strlen(rdata) + 1);
	//}
	
	signalCwmpInfo(WIFIMACTAB_REINIT, 0, 0);
#endif
	return 0;
}

static int wifiMacTabEntryUpdate(mac_table_t* macTable, int wlanIndex)
{
	char nodeName[128]	= {0};
	char NUMAttr[8]		= {0};
	char ipAddr[16]		= {0};
	char macAddr[20]	= {0};
	char buf[64] 		= {0};
	int i;
	mac_entry_t* macEntry = NULL;
	int need_update_again = 0;
	int ret = -1;

	/*dele the old entries*/
	snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_NODE, wlanIndex + 1);
	cfg_delete_object(nodeName);
	for(i = 0; i < macTable->Num; i++){
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_PVC_ENTRY_NODE, wlanIndex+1, i+1);
		ret = cfg_check_create_object(nodeName);
		/*get mac addr and set it to node*/ 
		macEntry = &macTable->macEntry[i];
		snprintf( macAddr, sizeof(macAddr), "%02X:%02X:%02X:%02X:%02X:%02X", macEntry->Addr[0],\
															macEntry->Addr[1],\
															macEntry->Addr[2],\
															macEntry->Addr[3],\
															macEntry->Addr[4],\
															macEntry->Addr[5] );


		
		/*get ip addr and set it to node*/		
		memset(ipAddr, 0, sizeof(ipAddr));
		getIpFromArp( macAddr, ipAddr );
		if(strlen(ipAddr) == 0){
			need_update_again = 1;	/*don't get ip address, need update again*/
		}	
		/*set Mac*/
		cfg_set_object_attr(nodeName, "MAC", macAddr);
		cfg_set_object_attr(nodeName, "IP", ipAddr);
	
#if defined(TCSUPPORT_CT_PHONEAPP)
		/*set RSSIA,RSSIB,RSSIC*/
		memset(buf, 0, sizeof(buf) );
		snprintf(buf, sizeof(buf), "%d", macEntry->AvgRssi0);
		cfg_set_object_attr(nodeName, "RSSIA", buf);
		
		memset(buf, 0, sizeof(buf) );
		snprintf(buf, sizeof(buf), "%d", macEntry->AvgRssi1);
		cfg_set_object_attr(nodeName, "RSSIB", buf);
		
		memset(buf, 0, sizeof(buf) );
		snprintf(buf, sizeof(buf), "%d", macEntry->AvgRssi2);
		cfg_set_object_attr(nodeName, "RSSIC", buf);
		
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2)
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%d",  macEntry->DataRate);
		cfg_set_object_attr(nodeName, "DataRate", buf);
#endif
#endif 		
	}
	
	memset(NUMAttr, 0, sizeof(NUMAttr) );	
	memset(buf, 0, sizeof(buf) );
	snprintf(NUMAttr, sizeof(NUMAttr), "NUM%d", wlanIndex);	
	snprintf(buf, sizeof(buf), "%lu", macTable->Num);
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_COMMON_NODE);
	cfg_set_object_attr(nodeName, NUMAttr, buf);

	if(need_update_again){
		needUpdateFlag |= (1 << wlanIndex);
	}else{
		needUpdateFlag &= ~(1 << wlanIndex);
	}	
	
	return needUpdateFlag;
}

int get_rssi(char *nodeName)
{	
	char buf[32] = {0};
	
	if (nodeName == NULL)
		return 0;
	cfg_get_object_attr(nodeName, "RSSI", buf, sizeof(buf));
	return atoi(buf);

}

int get_rssi2(int rssia, int rssib, int rssic)
{	
	int rssi = 0;
	
	if (10 > rssib - rssia && 10 < rssic - rssia)
	{
		rssi = (rssia + rssib)/2;
	}
	else if (10 < rssib - rssia && 10 < rssic - rssia)
	{
		rssi = rssia;
	}
	else 
		rssi = (rssia + rssib + rssic) / 3;
	
	return rssi;
}

#if defined(TCSUPPORT_CT_UBUS)
void get_roam_config(int *roamenable, int *start_time, int *RSSIThreshold, int *startrssi, int *report_interval){
	char buf[32] = {0};
	char *pname[2] = { WLAN_ROAM_NODE, WLAN_ROAM11AC_NODE};
	int i = 0;

	for(i = 0; i < 2; i++){
		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "enable", buf, sizeof(buf)) > 0 ){
			if(0 == strcmp(buf, "yes"))
				roamenable[i] = 1;
			else
				roamenable[i] = 0;
		}

		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "starttime", buf, sizeof(buf)) > 0 ){
			start_time[i] = atoi(buf);	
		}
		
		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "startrssi", buf, sizeof(buf)) > 0 ){
			startrssi[i] = atoi(buf);
		}

		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "thresholdrssi", buf, sizeof(buf)) > 0 ){
			RSSIThreshold[i] = atoi(buf);
		}

		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "reportinterval", buf, sizeof(buf)) > 0 ){
			report_interval[i] = atoi(buf);
		}
	}
	return;
		}

static int check_rssi_notification(notify_time_t *pNotifyTime, int sta_rssi, int startrssi, int RSSIThreshold, int report_interval, int current_time)
{
	int alarm = 0;

	if ( NULL == pNotifyTime )
		return 0;

	/* sta rssi is bigger than starrssi, and AP has not informed notification, AP will start checking starssi */
	if ( 0 == pNotifyTime->start_inform && sta_rssi > startrssi )
		pNotifyTime->start_inform = 1;

	/* not alarm when AP does not start checking rssi */
	if ( 0 == pNotifyTime->start_inform )
		return 0;

	/* clear inform msg when sta rssi become bigger than RSSIThreshold once, and do not alarm with return 0 */
	if ( sta_rssi >= RSSIThreshold )
	{
		pNotifyTime->first_set = 0;
		pNotifyTime->notify_time= 0;
	}
	/* rssi is lower than RSSIThreshold, ready to inform */
	else
	{
		/* inform directly when AP firstly infrom rssi, and update some msg for it */
		if ( 0 == pNotifyTime->first_set )
		{
			pNotifyTime->first_set = 1;
			pNotifyTime->notify_time = current_time + report_interval;
			alarm = 1;
		}
		/* wait until reach next report_interval */
		else if( current_time >= pNotifyTime->notify_time )
		{
			pNotifyTime->notify_time = current_time + report_interval;
			alarm = 1;
		}
	}
	
	return alarm;
}
#else
void get_roam_config(int *roamenable, int *start_time, int *RSSIThreshold1, int *RSSIThreshold2){
	char buf[32] = {0};
	char *pname[2] = { WLAN_ROAM_NODE, WLAN_ROAM11AC_NODE};
	int i = 0;

	for(i = 0; i < 2; i++){
		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "Enable", buf, sizeof(buf)) > 0 ){
			roamenable[i] = atoi(buf);
		}

		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "Start_time", buf, sizeof(buf)) > 0 ){
			start_time[i] = atoi(buf);	
		}
		
		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "Threshold1_RSSI", buf, sizeof(buf)) > 0 ){
			RSSIThreshold1[i] = atoi(buf);
		}

		memset(buf, 0, sizeof(buf));
		if( cfg_get_object_attr(pname[i], "Threshold2_RSSI", buf, sizeof(buf)) > 0 ){
			RSSIThreshold2[i] = atoi(buf);
		}
	}
	return;
}
#endif

void check_wifi_mac_entry(void)
{	
	struct timespec time;
	int wifinode_time = 0, connect_time = 0, sta_rssi = 0;
#if	defined(TCSUPPORT_CT_UBUS)
	int current_time = 0, type = 0, cap = 0;
	char tmp[3] = {0};
	unsigned char stamac[MAC_ADDR_LENGTH] = {0};
#endif
	char RSSIAlarm [32] = {0}, rdata[64] = {0}, node_name[64] = {0};
	char mac[32] = {0}, mac_val[32] = {0};
	char ip_addr[32] = {0}, buf[32] = {0};
	char NUM[32] = {0};
	int sta_num = 0, i = 0, j = 0, k = 0;
	int roamenable[2] = {0}, start_time[2] = {0};
#if defined(TCSUPPORT_CT_UBUS)
	int RSSIThreshold[2] = {0}, startrssi[2] = {0}, report_interval[2] = {0}; 
#else
	int RSSIThreshold1[2] = {0}, RSSIThreshold2[2] = {0}; 
#endif
	int len = 0, idx = 0, cnt = 0, alarm = 0;
#if defined(TCSUPPORT_CT_UBUS)
	char band[32];
	char channel[32];
	char bssid[32];
	char connect_timeVal[32];
	char sta_rssival[32];
	char support11k[4];
	char support11v[4];
	wifiroamnotifi_msg_t msgdataV;
	char wlan_node[64] = {0};
#endif
	
#if defined(TCSUPPORT_CT_JOYME2)
#if defined(TCSUPPORT_CT_UBUS)
	get_roam_config(roamenable, start_time, RSSIThreshold, startrssi, report_interval);
#else
	get_roam_config(roamenable, start_time, RSSIThreshold1, RSSIThreshold2);
#endif
	clock_gettime(CLOCK_MONOTONIC, &time);
#endif

	for (i = 0; i < MAX_DUALBAND_BSSID_NUM; i++) {
#if defined(TCSUPPORT_CT_UBUS)
		memset(band, 0, sizeof(band));
		memset(channel, 0, sizeof(channel));
		memset(bssid, 0, sizeof(bssid));
		memset(wlan_node, 0, sizeof(wlan_node));
#endif 	
		/* check ssid enable or not */
		if (i < MAX_ECNT_WALN_PORT_NUM){ 
			snprintf(node_name, sizeof(node_name), "root.wlan.entry.%d", i + 1);
#if defined(TCSUPPORT_CT_UBUS)			
			strncpy(band, "2.4g", sizeof(band) - 1);
			snprintf(wlan_node, sizeof(wlan_node), "root.info.ra.%d", i + 1);
			cfg_get_object_attr(wlan_node, "hwaddr", bssid, sizeof(bssid));
#endif			
		}else{
			snprintf(node_name, sizeof(node_name), "root.wlan11ac.entry.%d", i - MAX_ECNT_WALN_PORT_NUM + 1);
#if defined(TCSUPPORT_CT_UBUS)			
			strncpy(band, "5g", sizeof(band) - 1);
			snprintf(wlan_node, sizeof(wlan_node), "root.info.rai.%d", i + 1);
			cfg_get_object_attr(wlan_node, "hwaddr", bssid, sizeof(bssid));
#endif
			}
		if ((cfg_get_object_attr(node_name, "EnableSSID", buf, sizeof(buf)) == -1) || (0 != strcmp(buf, "1")))
			continue;

		/* check if this entry exist */
		snprintf(node_name, sizeof(node_name), WIFIMACTAB_COMMON_NODE);
		snprintf(NUM, sizeof(NUM), "NUM%d", i);
		if (cfg_get_object_attr(node_name, NUM, buf, sizeof(buf)) == -1)
			continue;
		sta_num = atoi(buf);
		
		for (j = 0; j < sta_num; j++) {
			/* check if this entry exist */
			snprintf(node_name, sizeof(node_name), WIFIMACTAB_PVC_ENTRY_NODE, i + 1, j + 1);
			/* update entry IP */
			if ((wifi_mab_ip_flag[i] & (1 << j)) == 0) {
				if (cfg_get_object_attr(node_name, "MAC", mac_val, sizeof(mac_val)) == -1)
					continue;
				cfg_set_object_attr(node_name, "RSSIAlarm", "0");
				memset(ip_addr, 0, sizeof(ip_addr));
				getIpFromArp(mac_val, ip_addr);
				if (strlen(ip_addr) > 0) {
					cfg_set_object_attr(node_name, "IP", ip_addr);
					wifi_mab_ip_flag[i] |= (1 << j);
				}				
			}
			
#if defined(TCSUPPORT_CT_JOYME2)
			/* check RSSI Here */
			if ( (i == 0 && roamenable[0] == 1) || (i == 8 && roamenable[1] == 1) )
			{	
				if( 8 == i ) k = 1;
				else k = 0;
				memset(buf, 0, sizeof(buf));
				if (cfg_get_object_attr(node_name, "Monotonictime", buf, sizeof(buf)) < 0)
					continue;
				wifinode_time = atoi(buf);						
				connect_time = time.tv_sec - wifinode_time;

				if (connect_time > start_time[k])
				{	
					sta_rssi = get_rssi(node_name);
#if defined(TCSUPPORT_CT_UBUS)
					alarm = check_rssi_notification(&gNotifyTime[i][j], sta_rssi, startrssi[k], RSSIThreshold[k], report_interval[k], time.tv_sec);

					if( alarm == 1 )
#else
					alarm = 0;
					memset(RSSIAlarm, 0, sizeof(RSSIAlarm));
					cfg_get_object_attr(node_name, "RSSIAlarm", RSSIAlarm, sizeof(RSSIAlarm));
					if (sta_rssi < RSSIThreshold1[k] && 0 != strcmp(RSSIAlarm, "1") )
					{	
						alarm = 1;
						cfg_set_object_attr(node_name, "RSSIAlarm", "1");
					}
					else if (sta_rssi >= RSSIThreshold2[k] && 0 == strcmp(RSSIAlarm, "1"))
					{	
						alarm = 2;
						cfg_set_object_attr(node_name, "RSSIAlarm", "0");
					}
					
					if( alarm > 0 )
#endif
					{
						memset(mac, 0, sizeof(mac));
						cfg_get_object_attr(node_name, "MAC", mac, sizeof(mac));
#if !defined(TCSUPPORT_CT_UBUS)
						len = strlen(mac);
						for ( idx = 0; idx < len; idx ++ )
						{
							if ( ':' == mac[idx] )
								continue;
						
							if ( '\n' == mac[idx] || '\r' == mac[idx] )
								break;
							
							buf[cnt++] = mac[idx];
						}
						snprintf(rdata, sizeof(rdata), "%s;%s;%d;%d;%d", buf, (1 == alarm) ? "FALSE": "TRUE", sta_rssi, k, j);
						ecnt_event_send(ECNT_EVENT_WIFI,									
							ECNT_EVENT_WIFI_STARSSILOWALARM,						 			
							rdata,						 			
							strlen(rdata) + 1);	
#else
						/*change mac from string to HEX number*/
						for(idx = 0; idx < MAC_ADDR_LENGTH; idx++)
						{
							tmp[0] = mac[3*idx];
							tmp[1] = mac[3*idx+1];
							stamac[idx] = (unsigned char)strtoul(tmp, NULL, 16);
						}

						if(strcmp("2.4g",band)==0)
						{
							type = WIFI_2_4G;
						cfg_get_object_attr(INFO_WLAN_NODE, "CurrentChannel", channel, sizeof(channel));
						}
						else
						{
							type = WIFI_5G;
						cfg_get_object_attr(INFO_WLAN11AC_NODE, "CurrentChannel", channel, sizeof(channel));
						}
						
						memset(support11k, 0, sizeof(support11k));
						if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Sta11kCap(type, 1, stamac, &cap, MAX_KEYWORD_LEN) && cap == 1)
							snprintf(support11k, sizeof(support11k), "yes");
						else
							snprintf(support11k, sizeof(support11k), "no");
						
						memset(support11v, 0, sizeof(support11v));
						if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Sta11vCap(type, 1, stamac, &cap, MAX_KEYWORD_LEN) && cap == 1)
							snprintf(support11v, sizeof(support11v), "yes");
						else
							snprintf(support11v, sizeof(support11v), "no");
						
						memset(sta_rssival, 0, sizeof(sta_rssival));						
						memset(connect_timeVal, 0, sizeof(connect_timeVal));
						memset(&msgdataV, 0, sizeof(wifiroamnotifi_msg_t));	
						snprintf(sta_rssival, sizeof(sta_rssival), "%d",sta_rssi);
						snprintf(connect_timeVal, sizeof(connect_timeVal), "%d",connect_time);
						mac_rm_dot(mac, msgdataV.mac);
						string_toUper(msgdataV.mac);
						strncpy(msgdataV.connectime, connect_timeVal, sizeof(msgdataV.connectime) - 1);
						strncpy(msgdataV.rssi, sta_rssival, sizeof(msgdataV.rssi) - 1);
						strncpy(msgdataV.band, band, sizeof(msgdataV.band) - 1);
						strncpy(msgdataV.channel, channel, sizeof(msgdataV.channel) - 1);
						mac_rm_dot(bssid, msgdataV.bssid);
						string_toUper(msgdataV.bssid);

						strncpy(msgdataV.support11k, support11k, sizeof(msgdataV.support11k) - 1);
						strncpy(msgdataV.support11v, support11v, sizeof(msgdataV.support11v) - 1);

						ctc_notify2ctcapd(NOTIFY_WIFI_ROAM_STATUS, &msgdataV, sizeof(wifiroamnotifi_msg_t));	
#endif		
					}
				}
			}
#endif
		}
	}

	return;
}

void wifiMacTab_update(void)
{
	char nodeName[128] = {0};
	char buf[16] = {0};
	mac_table_t macTab;
	int macTableChg = 0;
	int i = 0, notify_flag = 0;
	int maxNum = 0;
	char BSSIDNum[4] = {0};

#if defined(TCSUPPORT_CT_JOYME2)
	snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_COMMON_NODE);
	if (cfg_get_object_attr(nodeName, "lanhostdone", buf, sizeof(buf)) == -1) {
		tcdbg_printf("\r\n[%s][%d]:lanhost service not ready00000", __FUNCTION__, __LINE__);
		return;
	}

	if (buf[0] != '1') {
		/*tcdbg_printf("\r\n[%s][%d]:lanhost service not ready1111, buf=[%s]", __FUNCTION__, __LINE__, buf);*/
		return;
	}
#endif
#if defined(TCSUPPORT_CT_JOYME4)
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
	cfg_obj_get_object_attr(nodeName, "BssidNum", 0, BSSIDNum, sizeof(BSSIDNum));
	maxNum = atoi(BSSIDNum);
#endif

	/*check whether wifi mac table change*/
	for (i = 0; i < MAX_DUALBAND_BSSID_NUM; i++){
		/* check if this ssid is enable or not */
		if (i < MAX_ECNT_WALN_PORT_NUM) 
			snprintf(nodeName, sizeof(nodeName), "root.wlan.entry.%d", i + 1);
		else
			snprintf(nodeName, sizeof(nodeName), "root.wlan11ac.entry.%d", i - MAX_ECNT_WALN_PORT_NUM + 1);
		if (cfg_get_object_attr(nodeName, "EnableSSID", buf, sizeof(buf)) == -1)
			continue;
		if (!strcmp(buf, "0"))
			continue;

		/*now we only support 4 ssid*/
		memset(&macTab, 0, sizeof(macTab));
		getMacEntryByIndex(&macTab, i,maxNum);

		macTableChg = notifyWLanHostMacs(&(gMacTable[i]), &macTab, i);

#if defined(TCSUPPORT_CT_JOYME2)
		if ( macTableChg ) 
		{
			memcpy((void*)&gMacTable[i], (void*)&macTab, sizeof(mac_table_t));
		}
#else
		if( macTableChg || needUpdateFlag & (1<<i)){
			memcpy((void*)&gMacTable[i], (void*)&macTab, sizeof(mac_table_t));
			wifiMacTabEntryUpdate(&macTab,i);
			/*macTableChg = 1;*/
			if( (macTableChg & 0x1) )
				notify_flag = 1;
		}	
#endif
	}

#ifdef TCSUPPORT_CT_PON
#if !defined(TCSUPPORT_CT_JOYME2)
	if (notify_flag == 1) {
		signalCwmpInfo(WIFIMACTAB_REINIT, 0, 0);
	}
#endif
#else
	snprintf(nodeName, sizeof(nodeName), WIFIMACTAB_COMMON_NODE);
	if(notify_flag == 1){
		cfg_set_object_attr(nodeName, "valueChanged", "1");
		cwmpInfo_wifi();
	}else  {
		/*it means wifi mac table not change*/
		cfg_set_object_attr(nodeName, "valueChanged", "0");
	}
#endif
	return ;
}
#endif

#ifdef TCSUPPORT_WLAN_VENDIE
void beaconVSIE_duration_check(void)
{
	char nodeName[128] 	= {0};
	char BeaconVSIE_Common[128]={0};
	int entryidx;
	char tmp[160]		= {0};
	char str[7]={0};
	int buf = -1;
	int Status = 3;
	int interfaceidx;
	int dbglevel=0;
	int needUpdate = 0;
	int interface_need_update[BEACONVSIE_INTERFACE_ENTRY_NUM]={0};
	struct timespec curTime;
	static EntryDuration EntryDur[BEACONVSIE_MAX_ENTRY_NUM]={0};
	int delta_sec = 0;
	long delta_usec = 0;
	
	if(access(ECNT_BEACONVSIE_DBG_ENABLE, F_OK) == 0)
	{
		dbglevel=1;
	}
	
	snprintf(BeaconVSIE_Common, sizeof(BeaconVSIE_Common),BEACONVSIE_COMMON_NODE);
	if (cfg_obj_get_object_attr(BeaconVSIE_Common, "Enable", 0, tmp, sizeof(tmp)) > 0)
	{
		if (strlen(tmp)== 0)
			return;
		if (strcmp(tmp, "1"))
			return;
	}
	else
		return;
	
	for(entryidx=0; entryidx < BEACONVSIE_MAX_ENTRY_NUM; entryidx++)
	{					

		//1.check if any entry is busy,if so, keep forward.
		memset(nodeName, 0, sizeof(nodeName));
		snprintf(nodeName, sizeof(nodeName),BEACONVSIE_ENTRY_N_NODE,entryidx+1); 
		if(cfg_get_object_attr(nodeName, "Status", tmp, sizeof(tmp)) > 0)
		{
			Status = atoi(tmp);
			if(Status != BEACONVSIE_ENTRY_BUSY)
			{//if Status is FINISHED or STOP, reset duration timer
				if(cfg_get_object_attr(nodeName, "Duration", tmp, sizeof(tmp)) > 0)
				{
					EntryDur[entryidx].SetDuration = atoi(tmp);
					clock_gettime(CLOCK_MONOTONIC,&curTime);	/*get cur time*/
					EntryDur[entryidx].StartBeaconTime.tv_sec = curTime.tv_sec; //StartBeaconTime, s
					EntryDur[entryidx].StartBeaconTime.tv_usec= curTime.tv_nsec/1000; //StartBeaconTime, us
				}
				continue;
			}
		}
		else
		{		
			continue;
		}

		if (cfg_obj_get_object_attr(BeaconVSIE_Common, "Action", 0, tmp, sizeof(tmp)) > 0)
		{  
			if (strlen(tmp)== 0)
				return;
			if (strcmp(tmp, "Start") && strcmp(tmp, "Stop"))
				return;
		}
		else
			return;

		clock_gettime(CLOCK_MONOTONIC,&curTime);	/*get cur time*/
		EntryDur[entryidx].CurrentBeaconTime.tv_sec = curTime.tv_sec; //CurrentBeaconTime, s
		EntryDur[entryidx].CurrentBeaconTime.tv_usec = curTime.tv_nsec/1000; //CurrentBeaconTime, us

		delta_sec = EntryDur[entryidx].CurrentBeaconTime.tv_sec - EntryDur[entryidx].StartBeaconTime.tv_sec;
		delta_usec = EntryDur[entryidx].CurrentBeaconTime.tv_usec- EntryDur[entryidx].StartBeaconTime.tv_usec;
		
		if(delta_sec < (EntryDur[entryidx].SetDuration-1))
		{
			if(dbglevel)
				tcdbg_printf("ECNT_BeaconVSIE_DBG: Entry.%d's duration has %3ds remained\n",entryidx+1,EntryDur[entryidx].SetDuration-delta_sec);
		}
		else if((delta_sec == EntryDur[entryidx].SetDuration-1) && (delta_usec < (1000000-Check_Period/2)))
		{
			if(dbglevel)
				tcdbg_printf("ECNT_BeaconVSIE_DBG: Entry.%d's duration has 1s remained\n",entryidx+1);
		}
		else if((delta_sec == EntryDur[entryidx].SetDuration  ) && (delta_usec < -Check_Period/2))
		{
			if(dbglevel)
				tcdbg_printf("ECNT_BeaconVSIE_DBG: Entry.%d's duration has 1s remained\n",entryidx+1);
		}
		else
		{
			if(dbglevel)
				tcdbg_printf("ECNT_BeaconVSIE_DBG: current timecmp:%d delta_sec:%3d, delta_usec:%9d\n",curTime.tv_sec,delta_sec,delta_usec);
			if(Status == BEACONVSIE_ENTRY_BUSY)
			{
				cfg_obj_set_object_attr(nodeName,"Status",0,_BEACONVSIE_ENTRY_FINISHED_);
				beacon_send_msg_2_dbus(entryidx,BEACONVSIE_ENTRY_FINISHED);
				needUpdate=1;
				if(dbglevel)
					tcdbg_printf("ECNT_BeaconVSIE_DBG: Entry.%d become finished,need update rightnow\n",entryidx+1);
				cfg_type_beaconvsie_update(nodeName);//UPDATE
			}
		}
	}
						
	return;
}
#endif


int doWlanRate(char *Rate, int nodeid)
{
	char nodeName[128] 	= {0};
	char tmp[160]		= {0};
	char tmp2[160]		= {0};
	char buf[160]		= {0};
	char Mcs[16]		= {0};
	char *cIdeaWM		= NULL; 
	int wlan_id = nodeid;
	int iWMIndex, iWMNumbers;
	FILE *fp			= NULL;
	int res = 0;
	char version[16] = {0};
	int switch_enhance = 0;

	wifimgr_lib_get_version(0, version, sizeof(version));
	if (strcmp(version, "7592") == 0)
		switch_enhance = 1;

	fp = fopen(WLAN_MCS_SCRIPT_PATH, "w");
	if(NULL == fp){
		return CFG2_RET_FAIL;
	}
		
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
 	if(cfg_get_object_attr(nodeName, "WirelessMode", tmp, sizeof(tmp)) > 0){
		switch(atoi(tmp)){
			case PHY_MODE_BG_MIXED:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HtOpMode", "1");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "BasicRate", "15");
				fputs(buf, fp);
				if(strcmp(Rate, GVWlanChannelList[0].cChannelRate) == 0
					   || strcmp(Rate,GVWlanChannelList[1].cChannelRate) == 0
					   || strcmp(Rate,GVWlanChannelList[2].cChannelRate) == 0
					   || strcmp(Rate,GVWlanChannelList[3].cChannelRate) == 0){	
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "FixedTxMode", "1"); /*CCK*/
				}
			   	else{
	         		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "FixedTxMode", "2"); /*OFDM*/
				}
				fputs(buf, fp);
				break;
				
			case PHY_MODE_B_ONLY:
        		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HtOpMode", "1");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "BasicRate", "3");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "FixedTxMode", "1");
				fputs(buf, fp);		
				break;
				
			case PHY_MODE_G_ONLY:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HtOpMode", "0");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "BasicRate", "351");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id,"FixedTxMode", "2");
				fputs(buf, fp);
				break;
				
			default:
				printf("doWlanRate:wireless mode is not right!");
				fclose(fp);
				return CFG2_RET_FAIL;
		}
	}

	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, wlan_id+1);
	iWMNumbers = sizeof(GVWlanChannelList)/sizeof(GVWlanChannelList[0]);
	for(iWMIndex = 0; iWMIndex < iWMNumbers ; iWMIndex++){
		cIdeaWM = GVWlanChannelList[iWMIndex].cChannelRate;          
		if(strcmp(Rate, cIdeaWM) == 0){	
			memset(Mcs, 0, sizeof(Mcs));
			snprintf(Mcs, sizeof(Mcs), "%d", GVWlanChannelList[iWMIndex].iRateCount);	
			cfg_set_object_attr(nodeName, "HT_MCS", Mcs);
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HtMcs", Mcs);
			fputs(buf, fp);
		}
	}
	
	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && defined(TCSUPPORT_CT_WLAN_JOYME3)
		if (switch_enhance == 1)
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
		else
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);	
#else
		if (switch_enhance == 1)
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
		else
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);	
#endif
		F_PUTS_NORMAL(buf,fp);
	}
  	fclose(fp);
 	res = chmod(WLAN_MCS_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

	return CFG2_RET_SUCCESS;
}

int	doWlanMcs(char *Mcs, int nodeid)
{
	char nodeName[128] 	= {0};
	char tmp[160]		= {0};
	char tmp2[160]		= {0};
	char buf[160]		= {0};
	int wlan_id = nodeid;
	FILE *fp			= NULL;
	int res = 0;
	char version[16] = {0};
	int switch_enhance = 0;

	wifimgr_lib_get_version(0, version, sizeof(version));
	if (strcmp(version, "7592") == 0)
		switch_enhance = 1;

	fp = fopen(WLAN_MCS_SCRIPT_PATH, "w");
	if(fp == NULL){
		return CFG2_RET_FAIL;
	}
		
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
 	if(cfg_get_object_attr(nodeName, "WirelessMode", tmp, sizeof(tmp)) > 0){
		switch(atoi(tmp)){
			case PHY_MODE_BGN_MIXED:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtOpMode", "0");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "BasicRate", "15");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "FixedTxMode", "0");
				fputs(buf, fp);
				break;
				
			case PHY_MODE_N_ONLY:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtOpMode", "1");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "BasicRate", "15");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "FixedTxMode", "0");
				fputs(buf, fp);
				break;
				
			case PHY_MODE_GN_MIXED:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtOpMode", "0");
				fputs(buf, fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "BasicRate", "351");
				fputs(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "FixedTxMode", "0");
				fputs(buf,fp);
				break;
			default:
				printf("doWlanMcs: wireless mode is not right!\n");
				fclose(fp);
				return CFG2_RET_FAIL;
		}
	}

	snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtMcs", Mcs);
	fputs_escape(buf,fp);
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_COMMON_NODE);
	cfg_get_object_attr(nodeName, "HT_BSSCoexistence", tmp, sizeof(tmp));	
	cfg_get_object_attr(nodeName, "HT_BW", tmp2, sizeof(tmp2));	
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && defined(TCSUPPORT_CT_WLAN_JOYME3)
	if (atoi(tmp) == 1 && atoi(tmp2) == 1 && (pre_sys_state.wlan_bw !=1 || pre_sys_state.wlan_bssconexist != 1)){
#else
	if (atoi(tmp) == 1 && atoi(tmp2) == 1){	
#endif
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AP2040Rescan", "1");	
		fputs(buf,fp);	
	}		

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), WLAN_ENTRY_N_NODE, wlan_id+1);
	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && defined(TCSUPPORT_CT_WLAN_JOYME3)
		if (switch_enhance == 1)
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
		else
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);	
#else
		if (switch_enhance == 1)
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
		else
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);	
#endif		
		F_PUTS_NORMAL(buf, fp);
	}
  	fclose(fp);
 	res = chmod(WLAN_MCS_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

  	return CFG2_RET_SUCCESS;
}

void pre_sys_state_init(void)
{
	char BssidPre[8]	= {0};
	char channel[8]		= {0};
	char whnat[8]		= {0};
	char wlan_wirelessmode[8]={0};
	char wlan_TxPower[8]={0};
	char wlan_gi[8]={0};
	char wlan_extcha[8]={0};
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	char bw[8]={0};
	char coexist[8]={0};
	char BeaconPeriod[8]	= {0};
	char  DtimPeriod[8] = {0};
	char  TxPower[8]	= {0};
	char HtTxstream[8]	= {0};
	char HtRxstream[8]	= {0};
	char HtBssCoex[8]	= {0};
	char WirelessMode[8]	= {0};
	char HtExchar[8]	= {0};
	char HTBw[8]	= {0};
	char HtGi[8]	= {0};
	char HtMcs[8]	= {0};
	char VhtBw[8]	= {0};
	char VhtGi[8]	= {0};
	char CountryRegion[8]	= {0};
	char CountryRegionABand[8]	= {0};
#if defined(TCSUPPORT_WLAN_AX)
	char MuOfdmaDlEnable[8] = {0};
	char MuOfdmaUlEnable[8] = {0};
	char MuMimoDlEnable[8] = {0};
	char MuMimoUlEnable[8] = {0};
	char TWTSupport[8] = {0};
	char SREnable[8] = {0};
	char SRMode[8] = {0};
	char SRSDEnable[8] = {0};
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
	char Ant_dis[8] = {0};
	char Ant_convert[8] = {0};
	char Ant_sel[8] = {0};
#endif
#endif
#endif

	get_profile_str(BSSIDKEYNAME, BssidPre, sizeof(BssidPre), NO_QMARKS, SVC_WLAN_APON_PATH);
	pre_sys_state.wlan_Bssid_num = atoi(BssidPre);			
	get_profile_str(CHANNELKEYNAME,channel, sizeof(channel), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_chann = atoi(channel);				
	get_profile_str(WHNATKEYNAME, whnat, sizeof(whnat), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_whnat = atoi(whnat);

	get_profile_str(WIRELESSMODENAME,wlan_wirelessmode, sizeof(wlan_wirelessmode), NO_QMARKS,WLAN_PATH);
	pre_sys_state.wlan_wirelessmode=atoi(wlan_wirelessmode);

	get_profile_str(TXPOWERNAME,wlan_TxPower, sizeof(wlan_TxPower), NO_QMARKS,WLAN_PATH);
	pre_sys_state.wlan_txPowerLevel=atoi(wlan_TxPower);

	get_profile_str(HTGINAME,wlan_gi, sizeof(wlan_gi), NO_QMARKS,WLAN_PATH);
	pre_sys_state.wlan_gi=atoi(wlan_gi);

	get_profile_str(HTEXCHARNAME,wlan_extcha, sizeof(wlan_extcha), NO_QMARKS,WLAN_PATH);
	pre_sys_state.wlan_extcha=atoi(wlan_extcha);

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	get_profile_str(HTBWNAME,bw, sizeof(bw), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_bw=atoi(bw);
	
	get_profile_str(HTBSSCOEXNAME,coexist, sizeof(coexist), NO_QMARKS,WLAN_PATH);
	pre_sys_state.wlan_bssconexist=atoi(coexist);

	memset(CountryRegion, 0, sizeof(CountryRegion));
	get_profile_str(COUNTRYREGION,CountryRegion, sizeof(CountryRegion), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_CountryRegion = atoi(CountryRegion);
#endif	

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	memset(BeaconPeriod, 0, sizeof(BeaconPeriod));
	memset(DtimPeriod, 0, sizeof(DtimPeriod));
	memset(TxPower, 0, sizeof(TxPower));
	memset(HtBssCoex, 0, sizeof(HtBssCoex));
	memset(WirelessMode, 0, sizeof(WirelessMode));
	memset(HtExchar, 0, sizeof(HtExchar));
	memset(HTBw, 0, sizeof(HTBw));
	memset(HtGi, 0, sizeof(HtGi));
	memset(HtMcs, 0, sizeof(HtMcs));
	get_profile_str(BEACONPERIODNAME,BeaconPeriod, sizeof(BeaconPeriod), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_BeaconPeriod = atoi(BeaconPeriod);	
	get_profile_str(DTIMPERIODNAME,DtimPeriod, sizeof(DtimPeriod), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_DtimPeriod = atoi(DtimPeriod);
	get_profile_str(TXPOWERNAME,TxPower, sizeof(TxPower), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_TxPower = atoi(TxPower);
	get_profile_str(HTBSSCOEXNAME,HtBssCoex, sizeof(HtBssCoex), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_HtBssCoex = atoi(HtBssCoex);

	get_profile_str(WIRELESSMODENAME,WirelessMode, sizeof(WirelessMode), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_WirelessMode = atoi(WirelessMode);
	get_profile_str(HTEXCHARNAME,HtExchar, sizeof(HtExchar), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_HtExchar = atoi(HtExchar);
	get_profile_str(HTBWNAME,HTBw, sizeof(HTBw), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_HTBw = atoi(HTBw);
	get_profile_str(HTGINAME,HtGi, sizeof(HtGi), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_HtGi = atoi(HtGi);
#endif

#ifdef TCSUPPORT_WLAN_AC
	memset(BssidPre, 0, sizeof(BssidPre));
	memset(channel, 0, sizeof(channel));
	get_profile_str(BSSIDKEYNAME, BssidPre, sizeof(BssidPre), NO_QMARKS, SVC_WLAN11AC_APON_PATH);
	pre_sys_state.wlan11ac_Bssid_num = atoi(BssidPre);
	get_profile_str(CHANNELKEYNAME, channel, sizeof(channel), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_chann = atoi(channel);
	memset(whnat, 0, sizeof(whnat));
	get_profile_str(WHNATKEYNAME, whnat, sizeof(whnat), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_whnat = atoi(whnat);
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
	memset(BeaconPeriod, 0, sizeof(BeaconPeriod));
	memset(DtimPeriod, 0, sizeof(DtimPeriod));
	memset(TxPower, 0, sizeof(TxPower));
	memset(HtTxstream, 0, sizeof(HtTxstream));
	memset(HtRxstream, 0, sizeof(HtRxstream));
	memset(HtBssCoex, 0, sizeof(HtBssCoex));
	memset(WirelessMode, 0, sizeof(WirelessMode));
	memset(HtExchar, 0, sizeof(HtExchar));
	memset(HTBw, 0, sizeof(HTBw));
	memset(HtGi, 0, sizeof(HtGi));
	memset(HtMcs, 0, sizeof(HtMcs));
	memset(CountryRegionABand, 0, sizeof(CountryRegionABand));
	get_profile_str(BEACONPERIODNAME,BeaconPeriod, sizeof(BeaconPeriod), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_BeaconPeriod = atoi(BeaconPeriod);	
	get_profile_str(DTIMPERIODNAME,DtimPeriod, sizeof(DtimPeriod), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_DtimPeriod = atoi(DtimPeriod);
	get_profile_str(TXPOWERNAME,TxPower, sizeof(TxPower), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_TxPower = atoi(TxPower);
	get_profile_str(HTBSSCOEXNAME,HtBssCoex, sizeof(HtBssCoex), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_HtBssCoex = atoi(HtBssCoex);

	get_profile_str(WIRELESSMODENAME,WirelessMode, sizeof(WirelessMode), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_WirelessMode = atoi(WirelessMode);
	get_profile_str(HTEXCHARNAME,HtExchar, sizeof(HtExchar), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_HtExchar = atoi(HtExchar);
	get_profile_str(HTBWNAME,HTBw, sizeof(HTBw), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_HTBw = atoi(HTBw);
	get_profile_str(HTGINAME,HtGi, sizeof(HtGi), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_HtGi = atoi(HtGi);
	get_profile_str(VHTBWNAME,VhtBw, sizeof(VhtBw), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_VhtBw = atoi(VhtBw);
	get_profile_str(VHTSGINAME,VhtGi, sizeof(VhtGi), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_VhtGi = atoi(VhtGi);
	
	get_profile_str(HTTXSTREAMNAME,HtTxstream, sizeof(HtTxstream), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_HtTxstream = atoi(HtTxstream);
	get_profile_str(HTRXSTREAMNAME,HtRxstream, sizeof(HtRxstream), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_HtRxstream = atoi(HtRxstream);
	get_profile_str(COUNTRYREGIONABAND,CountryRegionABand, sizeof(CountryRegionABand), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_CountryRegionABand = atoi(CountryRegionABand);

	memset(HtTxstream, 0, sizeof(HtTxstream));
	memset(HtRxstream, 0, sizeof(HtRxstream));
	get_profile_str(HTTXSTREAMNAME,HtTxstream, sizeof(HtTxstream), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_HtTxstream = atoi(HtTxstream);
	get_profile_str(HTRXSTREAMNAME,HtRxstream, sizeof(HtRxstream), NO_QMARKS, WLAN_PATH);
	pre_sys_state.wlan_HtRxstream = atoi(HtRxstream);

#if defined(TCSUPPORT_WLAN_AX)
	memset(MuOfdmaDlEnable, 0, sizeof(MuOfdmaDlEnable));
	memset(MuOfdmaUlEnable, 0, sizeof(MuOfdmaUlEnable));
	memset(MuMimoDlEnable, 0, sizeof(MuMimoDlEnable));
	memset(MuMimoUlEnable, 0, sizeof(MuMimoUlEnable));
	memset(TWTSupport, 0, sizeof(TWTSupport));
	memset(SREnable, 0, sizeof(SREnable));
	memset(SRMode, 0, sizeof(SRMode));
	memset(SRSDEnable, 0, sizeof(SRSDEnable));
	get_profile_str(MUOFDMADLENABLENAME, MuOfdmaDlEnable, sizeof(MuOfdmaDlEnable), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_MuOfdmaDlEnable = atoi(MuOfdmaDlEnable);
	get_profile_str(MUOFDMAULENABLENAME, MuOfdmaUlEnable, sizeof(MuOfdmaUlEnable), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_MuOfdmaUlEnable = atoi(MuOfdmaUlEnable);
	get_profile_str(MUMINODLENABLENAME, MuMimoDlEnable, sizeof(MuMimoDlEnable), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_MuMimoDlEnable = atoi(MuMimoDlEnable);
	get_profile_str(MUMINOULENABLENAME, MuMimoUlEnable, sizeof(MuMimoUlEnable), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_MuMimoUlEnable = atoi(MuMimoUlEnable);
	get_profile_str(TWTSUPPORTNAME, TWTSupport, sizeof(TWTSupport), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_TWTSupport = atoi(TWTSupport);
	get_profile_str(SRENABLENAME, SREnable, sizeof(SREnable), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_SREnable = atoi(SREnable);
	get_profile_str(SRMODENAME, SRMode, sizeof(SRMode), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_SRMode = atoi(SRMode);
	get_profile_str(SRSDENABLENAME, SRSDEnable, sizeof(SRSDEnable), NO_QMARKS, WLAN_AC_PATH);
	pre_sys_state.wlan11ac_SRSDEnable = atoi(SRSDEnable);
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
	get_profile_str(ANTDIS, Ant_dis, sizeof(Ant_dis), NO_QMARKS, SVC_WLAN11AC_APON_PATH);
	pre_sys_state.wlan11ac_ant_dis = atoi(Ant_dis);
	get_profile_str(ANTCONVERT, Ant_convert, sizeof(Ant_convert), NO_QMARKS, SVC_WLAN11AC_APON_PATH);
	pre_sys_state.wlan11ac_ant_convert = atoi(Ant_convert);
	get_profile_str(ANTSEL, Ant_sel, sizeof(Ant_sel), NO_QMARKS, SVC_WLAN11AC_APON_PATH);
	pre_sys_state.wlan11ac_ant_sel = atoi(Ant_sel);
#endif
#endif
#endif

#endif	
	return;
}

int setMacToDataFile(int type)
{
	char buf[160] 					= {0};
	unsigned char eth_mac_addr[6] = {0};
	unsigned char mac_addr[]		= {0x00,0xaa,0xbb,0x01,0x23,0x45};
	int ret = -1;

	ret = wifimgr_lib_get_ethaddr(eth_mac_addr);
	if((ret != 0) || ((eth_mac_addr[0] == 0) && (eth_mac_addr[1] == 0) 
	&& (eth_mac_addr[2] == 0) &&(eth_mac_addr[3] == 0)
	&& (eth_mac_addr[4] == 0) && (eth_mac_addr[5] == 0)) ){
		printf("MacAddress for wifi chip is NULL, force to default!!!\n");
	}
	else {	  
	 	memcpy(mac_addr, eth_mac_addr, 6);
#if defined(TCSUPPORT_NP)
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0],mac_addr[1],
								mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]+3);

		if(wifimgr_lib_set_ApCliMac_Manual_Flag(type, 1, "1") != 0){
			printf("write_wlan_config:wifimgr_lib_set_ApCliMac_Manual_Flag fail.\n");
		}
			
	
		if(wifimgr_lib_set_ApCliMac_Manual(type, 1, buf) != 0){
			printf("write_wlan_config:wifimgr_lib_set_ApCliMac_Manual fail.\n");
		}
#endif

	 	if(WIFI_5G == type){
#if defined(TCSUPPORT_UNIQUEMAC_ALL)
	 		mac_addr[5]=mac_addr[5]+0x10;
#endif
			ret = wifimgr_lib_use_global_mac(mac_addr);
	 	}
#if defined(TCSUPPORT_ANDLINK)
		else
		{
	 		mac_addr[5]=mac_addr[5]+0x01;
		}
#endif
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0],mac_addr[1],
								mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	if(wifimgr_lib_set_MacAddress(type, -1, buf) != 0){
	printf("setMacToDataFile:wifimgr_lib_set_MacAddress fail.\n");
	}
	return 0;
}


#ifdef TCSUPPORT_WLAN_WDS
void setWdsAttrToDataFile(int type)
{
	char tmp[32] 	= {0};
	char tmp2[32] 	= {0};
	char buf[160] 	= {0};
	int i 			= 0;
	char dstNode[40] = {0};
	if(WIFI_2_4G == type) {
		snprintf(dstNode,sizeof(dstNode),"%s",WLAN_WDS_NODE);
	}	
	else if(WIFI_5G == type) {
		snprintf(dstNode,sizeof(dstNode),"%s",WLAN11AC_WDS_NODE);
	}	
	else{
		return;	
	}
	/*WdsEnable*/
	if(cfg_get_object_attr(dstNode, "WdsEnable", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			if(wifimgr_lib_set_wdsenable(type, -1, tmp)){
				printf("write_wlan11ac_config:wifimgr_lib_set_wdsenable fail.\n");
			}
		}
	}

	/*WdsEncrypType*/
	if(cfg_get_object_attr(dstNode, "WdsEncrypType", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "%s;%s;%s;%s", tmp, tmp, tmp, tmp);
			if(wifimgr_lib_set_wdsencryptype(type, -1, buf)){
				printf("write_wl1an1ac_config:wifimgr_lib_set_wdsencryptype fail.\n");
			}
		}
	}
	
	/*WdsList*/
	memset(buf, 0, sizeof(buf));
	for(i = 0; i < MAX_WDS_ENTRY; i++){
		snprintf(tmp2, sizeof(tmp2), "Wds_MAC%d", i);
		if(cfg_get_object_attr(dstNode, tmp2, tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				strcat(buf, tmp);
				strcat(buf, ";");
			}else{
				strcat(buf, "00:00:00:00:00:00");
				strcat(buf, ";");
			}
		}	
		
		/*WdsKey*/
		if(cfg_get_object_attr(dstNode, "WdsKey", tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				if(wifimgr_lib_set_wdskey(type, i + 1, tmp)){
					printf("write_wlan11ac_config:wifimgr_lib_set_wdskey fail.\n");
				}		
			}
		}
	}

	buf[strlen(buf)-1] = '\0';
	if(wifimgr_lib_set_wdslist(type, -1, buf)){
		printf("write_wlan11ac_config:wifimgr_lib_set_wdslist fail.\n");
	}

	return ;
}
#endif

int write_wlan_ant_sh(void)
{
	char wlan_ant_dis[8]	= {0};
	char wlan_ant_convert[8]	= {0};
	char wlan_ant_sel[8]	= {0};
	char wlan11ac_ant_dis[8]	= {0};
	char wlan11ac_ant_convert[8]	= {0};
	char wlan11ac_ant_sel[8]	= {0};
	char cmd[200]			= {0};
	char cmdVal[64]			= {0};
  	FILE *fp				= NULL;
	int res = 0;

	fp = fopen(SVC_WLAN_ANT_PATH, "w");
	if(NULL == fp){
		return -1;
	}
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_dis", wlan_ant_dis, sizeof(wlan_ant_dis)) > 0){
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_dis=%d-%s\n", 0, wlan_ant_dis);
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	else
	{
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_dis=%d-1\n", 0);
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	fputs_escape(cmd,fp);
	memset(cmd, 0, sizeof(cmd));
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_convert", wlan_ant_convert, sizeof(wlan_ant_convert)) > 0){
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_convert=%d-%s\n", 0, wlan_ant_convert);
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmdVal, cmd, 0);

	}else{
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_convert=%d-1\n", 0);
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	fputs_escape(cmd,fp);
	memset(cmd, 0, sizeof(cmd));
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "ant_sel", wlan_ant_sel, sizeof(wlan_ant_sel)) > 0){
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_sel_ant=%d-%s\n", 0, wlan_ant_sel);
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmdVal, cmd, 0);
	}else{
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_sel_ant=%d-0\n", 0);
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	if(1 == atoi(wlan_ant_dis)){
		fputs_escape(cmd,fp);
	}


	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "ant_dis", wlan11ac_ant_dis, sizeof(wlan11ac_ant_dis)) > 0){
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_dis=%d-%s\n", 1, wlan11ac_ant_dis);
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	else
	{
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_dis=%d-1\n", 1);
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	fputs_escape(cmd,fp);
	memset(cmd, 0, sizeof(cmd));
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "ant_convert", wlan11ac_ant_convert, sizeof(wlan11ac_ant_convert)) > 0){
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_convert=%d-%s\n", 1, wlan11ac_ant_convert);
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmdVal, cmd, 0);
	}else{
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_convert=%d-1\n", 1);
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	fputs_escape(cmd,fp);
	memset(cmd, 0, sizeof(cmd));
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "ant_sel", wlan11ac_ant_sel, sizeof(wlan11ac_ant_sel)) > 0){
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_sel_ant=%d-%s\n", 1, wlan11ac_ant_sel);
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmdVal, cmd, 0);
	}else{
		snprintf(cmdVal, sizeof(cmdVal), "ant_div_sel_ant=%d-0\n", 1);
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmdVal, cmd, 0);
	}
	if(1 == atoi(wlan11ac_ant_dis)){
		fputs_escape(cmd,fp);
	}

	fclose(fp);
 	res = chmod(SVC_WLAN_ANT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

	return 0;
}

int write_wlan_exe_sh(int BssidNum)
{
  	char nodeName[128]		= {0};
	char tmp[256]			= {0};
	char tmp2[256]			= {0};
	char buf[600]			= {0};
	char wirelessmode[16]	= {0};
	char encrytype[16]		= {0};
  	FILE *fp				= NULL;
	int i, wlan_id;
	int res = 0;
#if defined(TCSUPPORT_NP_CMCC)
	int webset_id = 0;
#endif
	char wlan_script_com_attrs_other[][2][ATTR_SIZE]=
	{
		{"HT_RDG", 			"HtRdg"},
		{"HT_STBC", 		"HtStbc"},
		{"HT_AMSDU", 		"HtAmsdu"},
		{"HT_AutoBA", 		"HtAutoBa"},
		{"HT_BADecline", 	"BADecline"},
		{"HT_DisallowTKIP", "HtDisallowTKIP"},
		{"HT_TxStream", 	"HtTxStream"},
		{"HT_RxStream", 	"HtRxStream"},
		{"APSDCapable", 	"UAPSDCapable"},
		{"HT_BSSCoexistence","HtBssCoex"}, 
		{"", ""}
	};

	char wlan_script_com_attr[16][20] = {0};
	char version[16] = {0};
	int switch_enhance = 0;

	wifimgr_lib_get_version(0, version, sizeof(version));
	if (strcmp(version, "7592") == 0)
		switch_enhance = 1;

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
	int reset_flag = 0;
#endif



	memset(wlan_script_com_attr, 0, 16 * 20);
	res = wifimgr_lib_get_wlan_script_attr(wlan_script_com_attr);
  	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_id", tmp, sizeof(tmp)) > 0){
#if defined(TCSUPPORT_NP_CMCC)
		webset_id = atoi(tmp);
#else
    	wlan_id = atoi(tmp);
#endif
  	}else{
    	return -1;
 	}

	/*write common setting WLAN_exec.sh*/
	fp = fopen(SVC_WLAN_SCRIPT_PATH, "w");
	if(NULL == fp){
		return -1;
	}
#if defined(TCSUPPORT_NP_CMCC)
	for(wlan_id = 0; wlan_id < BssidNum; wlan_id++){
		snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN_ENTRY_NODE, wlan_id + 1);
		if(cfg_get_object_attr(WLAN_COMMON_NODE, "alink_type", tmp, sizeof(tmp)) > 0 && !strcmp(tmp, "1"))
		{
			if(cfg_get_object_attr(nodeName, "wlan_changed", tmp, sizeof(tmp)) <= 0 || !strcmp(tmp, "0"))
				continue;
		}
		else
		{
			if(webset_id != wlan_id)
				continue;
		}
#endif
	for(i = 0; strlen(wlan_script_com_attr[i]) != 0; i++){
	 	if(strcmp(wlan_script_com_attr[i], "Channel")==0){
			if(cfg_get_object_attr(WLAN_COMMON_NODE, wlan_script_com_attr[i], tmp, sizeof(tmp)) > 0){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(atoi(tmp) != 0 && pre_sys_state.wlan_chann != atoi(tmp) ){
#else
				if(atoi(tmp)){
#endif
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, wlan_script_com_attr[i], tmp);
					fputs_escape(buf,fp);
				}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(switch_enhance == 0){
					if (atoi(tmp) == 0 && pre_sys_state.wlan_chann != atoi(tmp) )
					{
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AutoChannelSel", "3");
						fputs_escape(buf,fp);
					}
				}
				else
					/*do nothing*/;
#else
				else if (atoi(tmp) == 0 && pre_sys_state.wlan_chann != atoi(tmp) )
				{
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AutoChannelSel", "3");
					fputs_escape(buf,fp);
				}
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(switch_enhance ==1)
					reset_flag = 1;
#endif
			
			}

		}else{
			if(cfg_get_object_attr(WLAN_COMMON_NODE, wlan_script_com_attr[i], tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, wlan_script_com_attr[i], tmp);
				fputs_escape(buf,fp);
			}
		}
	 }
#if defined(TCSUPPORT_CT_WLAN_NODE)
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "TxPowerLevel", tmp, sizeof(tmp)) > 0){
		switch(atoi(tmp)){
			case TX_POWER_LEVLE_1:
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(pre_sys_state.wlan_txPowerLevel != 75 && switch_enhance == 1){
					reset_flag = 1;
				}
#endif
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxPower", "75");//100
				fputs(buf,fp);
				break;
			case TX_POWER_LEVLE_2:
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(pre_sys_state.wlan_txPowerLevel != 50 && switch_enhance == 1){
					reset_flag = 1;
				}
#endif
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxPower", "50");//75
				fputs(buf,fp);
				break;
			case TX_POWER_LEVLE_3:
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(pre_sys_state.wlan_txPowerLevel != 25 && switch_enhance == 1){
					reset_flag = 1;
				}
#endif
				snprintf(buf,sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxPower", "25");//50
				fputs(buf,fp);
				break;
			case TX_POWER_LEVLE_4:
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(pre_sys_state.wlan_txPowerLevel != 15 && switch_enhance == 1){
					reset_flag = 1;
				}
#endif
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxPower", "15");//25
				fputs(buf,fp);
				break;
			case TX_POWER_LEVLE_5:
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(pre_sys_state.wlan_txPowerLevel != 9 && switch_enhance == 1){
					reset_flag = 1;
				}
#endif				
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxPower", "9");//15
				fputs(buf,fp);
				break;
			case TX_POWER_LEVLE_6:
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if(pre_sys_state.wlan_txPowerLevel != 100 && switch_enhance == 1){
					reset_flag = 1;
				}
#endif
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxPower", "100");//15
				fputs(buf,fp);
				break;
			default:
				tcdbg_printf("WLan:the valule of TxPowerLevel is not right!\n");
				fclose(fp);
				return -1;
			}
	}
#endif

	 for(i = 0; strlen(wlan_script_com_attrs_other[i][0]) != 0; i++){
		 if(cfg_get_object_attr(WLAN_COMMON_NODE, wlan_script_com_attrs_other[i][0], tmp, sizeof(tmp)) > 0){
			 snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, wlan_script_com_attrs_other[i][1], tmp);
			 fputs_escape(buf,fp);
		 }
	 }

	 if(cfg_get_object_attr(WLAN_COMMON_NODE, "WirelessMode", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "WirelessMode", tmp);
		fputs_escape(buf,fp);
		strncpy(wirelessmode, tmp, sizeof(wirelessmode) - 1);
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
		if((pre_sys_state.wlan_wirelessmode != atoi(tmp)) && switch_enhance == 1){
			reset_flag = 1;
		}
#endif

		switch(atoi(tmp)){
			case PHY_MODE_BG_MIXED:
			case PHY_MODE_BGN_MIXED:
			case PHY_MODE_N_ONLY:
			case PHY_MODE_GNAX_MIXED:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "BasicRate", "15");
				fputs(buf,fp);
				break;
			case PHY_MODE_B_ONLY:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "BasicRate", "3");
				fputs(buf,fp);
				break;
			case PHY_MODE_G_ONLY:
			case PHY_MODE_GN_MIXED:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "BasicRate", "351");
				fputs(buf,fp);
				break;
			default:
				tcdbg_printf("WLan: wireless mode is not right!(3390,3090)\n");
				fclose(fp);
				return FAIL;
		}
	}
	 
	if(Is11nWirelessMode()){
		if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BW", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtBw", tmp);
			fputs_escape(buf,fp);
		}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
		if((pre_sys_state.wlan_bw != atoi(tmp))  && switch_enhance == 1){
			reset_flag = 1;
		}
#endif

		if(atoi(tmp)==1){
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtExtcha", "0");
			if(cfg_get_object_attr(WLAN_COMMON_NODE, "Channel", tmp, sizeof(tmp)) > 0){
				if(atoi(tmp) <= 4 && (atoi(tmp) != 0)){
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtExtcha", "1");
				}
				else if(atoi(tmp) >= 5 && atoi(tmp) <= 7){
					cfg_get_object_attr(WLAN_COMMON_NODE, "HT_EXTCHA", tmp, sizeof(tmp));
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtExtcha", tmp);
				}
				else if(atoi(tmp) == 0){
					cfg_get_object_attr(INFO_WLAN_NODE, "CurrentChannel", tmp, sizeof(tmp));
					if(atoi(tmp) <= 4 && (atoi(tmp) != 0)){
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtExtcha", "1");
					}
					else if(atoi(tmp) >= 5 && atoi(tmp) <= 7){
						cfg_get_object_attr(WLAN_COMMON_NODE, "HT_EXTCHA", tmp, sizeof(tmp));
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtExtcha", tmp);
					}
				}
			}
			fputs(buf,fp);	
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
			cfg_get_object_attr(WLAN_COMMON_NODE, "HT_EXTCHA", tmp, sizeof(tmp));
			if((pre_sys_state.wlan_extcha != atoi(tmp)) && switch_enhance == 1){
				reset_flag = 1;
			}
#endif	
		}
		if(cfg_get_object_attr(WLAN_COMMON_NODE, "HT_GI", tmp, sizeof(tmp)) > 0){
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
			if((pre_sys_state.wlan_gi != atoi(tmp)) && switch_enhance == 1){
				reset_flag = 1;
			}
#endif
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtGi", tmp);
			fputs_escape(buf,fp);
		}
	}
	else{
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HtBw", "0");
		fputs(buf,fp);
	}

#if !defined(TCSUPPORT_NP_CMCC)
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN_ENTRY_NODE, wlan_id + 1);
#endif
	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		strncpy(encrytype, tmp, sizeof(encrytype) - 1);	
	}

	if(Is11nWirelessMode()){
		if(cfg_get_object_attr(nodeName, "HT_MCS", tmp, sizeof(tmp)) > 0){
			if(atoi(tmp)<=15){
				if((strcmp(encrytype, "WPAPSK") == 0) &&(atoi(wirelessmode) == 9) && (atoi(tmp) > 7)){
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtMcs", "33");
				}	
				else{
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtMcs", tmp);
				}	
			}else{
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtMcs", "33");
			}
				
			fputs_escape(buf,fp);
		}
	}else{
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "HtMcs", "33");
		fputs(buf,fp);
	}

	if(cfg_get_object_attr(nodeName, "HideSSID", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HideSSID", tmp);
		fputs_escape(buf,fp);
	}
	
	/*AuthMode*/
 	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
			if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "1");
				fputs(buf,fp);
			}else{
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif

#if defined(TCSUPPORT_CT_WLAN_NODE)
			if(cfg_get_object_attr(nodeName, "WEPAuthType", tmp, sizeof(tmp)) > 0){
				if (switch_enhance == 1)
				{
					if(!strcmp(tmp, "OpenSystem")){
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", "OPEN");
						fputs(buf,fp);
					}else if(!strcmp(tmp, "SharedKey")){
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", "SHARED");
						fputs(buf,fp);
					}else{
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", "WEPAUTO");
						fputs(buf,fp);
					}
				}
				else
				{
					if(!strcmp(tmp, "OpenSystem")){
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");
						fputs(buf,fp);
					}else if(!strcmp(tmp, "SharedKey")){
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "SHARED");
						fputs(buf,fp);
					}else{
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
						fputs(buf,fp);
					}
				}

			}
			else{
				if (switch_enhance == 1)
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", "WEPAUTO");
				else
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
				fputs(buf,fp);
			}
#else
			if (switch_enhance == 1)
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", "WEPAUTO");
			else
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");

			fputs(buf,fp);
#endif
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "EncrypType", "WEP");
			fputs(buf,fp);
			if(cfg_get_object_attr(nodeName, "DefaultKeyID", tmp, sizeof(tmp)) <= 0){
				strncpy(tmp, "1", sizeof(tmp));
			}
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", tmp);
			fputs(buf,fp);
			/*key1~key4*/
			for(i = 1;i <= KEY_NUM; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
					/*check strlen because the key maybe empty*/
					if(strlen(tmp) > 0){
						specialCharacterHandle(tmp);
						snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
						snprintf(tmp, sizeof(tmp), "Key%d", i);
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, tmp, tmp2);
						F_PUTS_NORMAL(buf,fp);
					}
				}
			}
    	}
		else if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK") /*WPAPSK or WPA2PSK or Mixed mode*/
#if defined(TCSUPPORT_WLAN_8021X)
						|| !strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
						||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
			){/*WPAPSK or WPA2PSK or Mixed mode*/
			if (switch_enhance == 1)
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", tmp);
			else
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", tmp);
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			wifimgr_lib_get_IEEE8021X_VAL(tmp2, sizeof(tmp2));
			if(!strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", tmp2);
				fputs(buf,fp);
			}else if (0 == strcmp(tmp2, "1")){
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif				

			if(cfg_get_object_attr(nodeName, "EncrypType", tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "EncrypType", tmp);
				fputs_escape(buf,fp);
			}

			if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
				if (switch_enhance == 1 && reset_flag == 0)
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
				else
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);					
#else
				if (switch_enhance == 1)
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
				else
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);	

#endif
				F_PUTS_NORMAL(buf,fp);
			}
			if(cfg_get_object_attr(nodeName, "WPAPSK", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WPAPSK", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", "2");/*WPAPSK and WPA2PSK use DefaultKeyID = 2*/
			fputs(buf,fp);
		}
		else{/*OPEN*/
			if (switch_enhance == 1)
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthModeSingle", "OPEN");
			else
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");				

			fputs(buf,fp);
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "EncrypType", "NONE");
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
			fputs(buf,fp);
#endif

		}
 	}
	
	/*AccessPolicy=....*/
	if(cfg_get_object_attr(nodeName, "AccessPolicy", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			snprintf(buf, sizeof(tmp), WLAN_SCRIPT_PREFIX, wlan_id, "AccessPolicy", tmp);
			fputs_escape(buf,fp);
		}
	}
	/*AccessControlList=MAC1;MAC2;.......*/
	strcpy(buf,"");
	for(i = 0; i < WLAN_MAC_NUM; i++){
		snprintf(tmp2, sizeof(tmp2), "%s%d", "WLan_MAC", i);
		if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
			if(strlen(tmp) > 0){
				strcat(buf,tmp);
				strcat(buf,";");
			}
		}
	}
	if(strlen(buf) > 0){
		buf[strlen(buf) - 1] ='\0';/*remove last ";"*/
		snprintf(tmp2, sizeof(tmp2), "\"%s\"", buf);/*add ""*/
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "ACLClearAll", "1");
		fputs(buf,fp);
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "ACLAddEntry", tmp2);
		fputs_escape(buf,fp);
	}

	if(!Is11nWirelessMode()){
		if(cfg_get_object_attr(nodeName, "WMM", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WmmCapable", tmp);
			fputs_escape(buf, fp);
			if(txBurst_or_not(BssidNum)){
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxBurst", "1");
			}
			else{
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxBurst", "0");
			}
			fputs(buf,fp);
		}
	}

#ifdef WSC_AP_SUPPORT
#ifdef TCSUPPORT_WLAN_MULTI_WPS
	cfg_set_object_attr(nodeName, "WPSConfStatus", "2");
	snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscConfStatus", "2");
	fputs(buf,fp);
	
	if(cfg_get_object_attr(nodeName, "WPSConfMode", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscConfMode", tmp);
		fputs_escape(buf,fp);
	}			
	if(cfg_get_object_attr(nodeName, "WscV2Support", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscV2Support", tmp);
		fputs_escape(buf,fp);
	}
	if(cfg_get_object_attr(nodeName, "WscMaxPinAttack", tmp, sizeof(tmp)) > 0){
        snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscMaxPinAttack", tmp);
        fputs_escape(buf,fp);
    }
	if(cfg_get_object_attr(nodeName, "WscSetupLockTime", tmp, sizeof(tmp)) > 0){
        snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscSetupLockTime", tmp);
        fputs_escape(buf,fp);
    }
#else
	/*now we just support main SSID*/
	if( 0 == wlan_id){
		cfg_set_object_attr(nodeName, "WPSConfStatus", "2");			
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscConfStatus", "2");
		
		fputs(buf,fp);
		if(cfg_get_object_attr(nodeName, "WPSConfMode", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscConfMode", tmp);
			fputs_escape(buf,fp);
		}			
		if(cfg_get_object_attr(nodeName, "WscV2Support", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscV2Support", tmp);
			fputs_escape(buf,fp);
		}
		if(cfg_get_object_attr(nodeName, "WscMaxPinAttack", tmp, sizeof(tmp)) > 0){
            snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscMaxPinAttack", tmp);
            fputs_escape(buf,fp);
        }
		if(cfg_get_object_attr(nodeName, "WscSetupLockTime", tmp, sizeof(tmp)) > 0){
            snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WscSetupLockTime", tmp);
            fputs_escape(buf,fp);
    	}
	}
#endif
#endif 

	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
		if (switch_enhance == 1 && reset_flag == 0)
			snprintf(buf, sizeof(tmp), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
		else
			snprintf(buf, sizeof(tmp), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
#else
		if (switch_enhance == 1)
			snprintf(buf, sizeof(tmp), WLAN_SCRIPT_PREFIX, wlan_id, "SSIDSingle", tmp2);
		else
			snprintf(buf, sizeof(tmp), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);

#endif
		F_PUTS_NORMAL(buf,fp);
	}
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	if(cfg_get_object_attr(nodeName, "MaxStaNum", tmp, sizeof(tmp)) > 0){
		wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(WIFI_2_4G, tmp2, sizeof(tmp2));
	    snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, tmp2, tmp);
		fputs_escape(buf,fp);
	}
#endif
	
/*Trigger AP resccan depends on the value of HT_BW and HT_BSSCoexistence*/
	cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BSSCoexistence", tmp, sizeof(tmp));
	cfg_get_object_attr(WLAN_COMMON_NODE, "HT_BW", tmp2, sizeof(tmp2));
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
	if (atoi(tmp) == 1 && atoi(tmp2) == 1 && (pre_sys_state.wlan_bw !=1 || pre_sys_state.wlan_bssconexist != 1))
#else
	if(atoi(tmp) == 1 && atoi(tmp2) == 1)
#endif
	{
		if (switch_enhance == 1)
			res = wifimgr_lib_set_AP2040Rescan(wlan_id, &(pre_sys_state.wlan_bw), &(pre_sys_state.wlan_bssconexist), fp);
	}


#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE)
	if (switch_enhance == 1){
		char chan[8] = {0};
		if(cfg_get_object_attr(WLAN_COMMON_NODE, "Channel", chan, sizeof(chan)) > 0){
			if(0 == atoi(chan) && atoi(chan) != pre_sys_state.wlan_chann)
			{
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id,"AutoChannelSel", "3");
				fputs(buf,fp);
			}
		}
	}
#endif

	if(isBoot){ 
		isBoot = 0;
	}
	else{
	/*used to set WIFi Tx Rate*/
	if(Is11nWirelessMode()){
		if(cfg_get_object_attr(nodeName, "HT_MCS", tmp, sizeof(tmp)) > 0){
			if((strcmp(encrytype,"WPAPSK") == 0) &&(atoi(wirelessmode) == 9) && (atoi(tmp)> 7)){
				strcpy(tmp,"33");
			}	
			doWlanMcs(tmp, wlan_id);
		}
	}
	else{
		if(cfg_get_object_attr(nodeName, "HT_RATE", tmp, sizeof(tmp)) > 0){
			doWlanRate(tmp, wlan_id);
		}
	}
	}
#if defined(TCSUPPORT_NP_CMCC)
		cfg_set_object_attr(nodeName, "wlan_changed", "0");
	}
	cfg_set_object_attr(WLAN_COMMON_NODE, "alink_type", "0");
#endif
	fclose(fp);
 	res = chmod(SVC_WLAN_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);
	
  	return 0;
}

#if defined(TCSUPPORT_WLAN_AC)	
int write_wlan11ac_exe_sh(int BssidNum)
{
	int i, wlan_id;
  	char nodeName[128]		= {0};
	char tmpName[128]		= {0};
	char tmp[256]			= {0}; 
	char tmp2[256]			= {0};
	char tmp3[32]			= {0};
	char buf[600]			= {0};
	char tmpChannel[8] 		= {0};
	char wirelessmode[16]	= {0}; 
	char encrytype[16]		= {0};
  	FILE *fp				= NULL;
	int res = 0;
#if defined(TCSUPPORT_WLAN_8021X_EXT)
	struct hostent *hp = NULL;
	struct sockaddr_in RadiusAddr;
#endif
#if defined(TCSUPPORT_NP_CMCC)
	int webset_id = 0;
#endif
	char wlan_script_com_attr[][ATTR_SIZE]=
	{
		{"CountryRegion"},
		{"CountryRegionABand"},
		{"Channel"},
		{"BeaconPeriod"},
		{"RTSThreshold"},
		{"FragThreshold"},
		{"DtimPeriod"},
		{"TxPower"},
		{"TxPreamble"},
#if defined(TCSUPPORT_TTNET)
		{"MaxStaNum"},
#endif
		{"IgmpSnEnable"},
		{"BGProtection"},
		{"TxPower"},
		{"TxPreamble"},
		{"ShortSlot"},
		{"TxBurst"},
		{"PktAggregate"},
		{"IEEE80211H"},
		{""}
	};
	
	char wlan_script_com_attrs_other[][2][ATTR_SIZE]=
	{
		{"HT_RDG", 				"HtRdg"},
		{"HT_STBC", 			"HtStbc"},
		{"HT_AMSDU", 			"HtAmsdu"},
		{"HT_AutoBA", 			"HtAutoBa"},
		{"HT_BADecline", 		"BADecline"},
		{"HT_DisallowTKIP", 	"HtDisallowTKIP"},
		{"HT_TxStream", 		"HtTxStream"},
		{"HT_RxStream", 		"HtRxStream"},
		{"APSDCapable", 		"UAPSDCapable"},
		{"HT_BSSCoexistence",	"HtBssCoex"}, 
		{"VHT_STBC", "VhtStbc"},
		{"VHT_BW_SIGNAL", "VhtBwSignal"},
		{"VHT_DisallowNonVHT", "VhtDisallowNonVHT"},

		{"", ""}
	};

  	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
#if defined(TCSUPPORT_NP_CMCC)
		webset_id = atoi(tmp);
#else
    	wlan_id = atoi(tmp);
#endif
  	}else{
    	return -1;
 	}

	/*write common setting WLAN_exec.sh*/
	fp = fopen(SVC_WLAN11AC_SCRIPT_PATH, "w");
	if(NULL == fp){
		return -1;
	}
#if defined(TCSUPPORT_NP_CMCC)
	for(wlan_id = 0; wlan_id < BssidNum; wlan_id++){
		snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN11AC_ENTRY_NODE, wlan_id + 1);
		if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "alink_type", tmp, sizeof(tmp)) > 0 && !strcmp(tmp, "1"))
		{
			if(cfg_get_object_attr(nodeName, "wlan_changed", tmp, sizeof(tmp)) <= 0 || !strcmp(tmp, "0"))
				continue;
		}
		else
		{
			if(webset_id != wlan_id)
				continue;
		}
#endif
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "Country", tmp ,sizeof(tmp)) > 0){
		for(i = 0; i < NUM_COUNTRY; i++){
			if(strcmp((char*)all_country[i].pCountryName,tmp) == 0){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "CountryCode", (char*)all_country[i].IsoName);
				fputs(buf,fp);
				break;
			}
		}
	}	
	for(i = 0; strlen(wlan_script_com_attr[i]) != 0; i++){
		if(strcmp(wlan_script_com_attr[i], "Channel") == 0){
			if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, wlan_script_com_attr[i], 
												tmpChannel, sizeof(tmpChannel)) > 0){
				if(atoi(tmpChannel)){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id
						, wlan_script_com_attr[i], tmpChannel);
					fputs_escape(buf,fp);
				}
			}
		}else{
			if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, wlan_script_com_attr[i], 
												tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id
					, wlan_script_com_attr[i], tmp);
				fputs_escape(buf,fp);
			}
		}
	}
	for(i = 0; strlen(wlan_script_com_attrs_other[i][0]) != 0; i++){
		if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, wlan_script_com_attrs_other[i][0], 
												tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id
					, wlan_script_com_attrs_other[i][1], tmp);
			fputs_escape(buf,fp);
		}
	}

	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "WirelessMode", 
												tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WirelessMode", tmp);
		fputs_escape(buf,fp);

		strncpy(wirelessmode, tmp, sizeof(wirelessmode) - 1);
	}

	if(Is11nWirelessMode5G()){
		if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "HT_BW", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtBw", tmp);
			fputs_escape(buf,fp);
		}
		if((atoi(tmp)==1) && (atoi(tmpChannel)!=0)){
			if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "HT_EXTCHA", tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtExtcha", tmp);
				fputs_escape(buf,fp);
			}
		}
		if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "HT_GI", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtGi", tmp);
			fputs_escape(buf,fp);
		}
		if(Is11acWirelessMode()){
			if (cfg_get_object_attr(WLAN11AC_COMMON_NODE, "HT_BW", tmp, sizeof(tmp)) > 0) {
				if (atoi(tmp) == 1) {
					if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "VHT_BW", tmp, sizeof(tmp)) > 0){
						snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "VhtBw", tmp);
						fputs_escape(buf,fp);
					}
					if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "VHT_SGI", tmp, sizeof(tmp)) > 0) {
						snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "VhtGi", tmp);
						fputs_escape(buf,fp);
					}
				}
				else {
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "VhtBw", "0");
					fputs(buf,fp);
				}
			}
		}
		else {
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "VhtBw", "0");
			fputs(buf,fp);
		}
	}
	else{
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtBw", "0");
		fputs(buf,fp);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "VhtBw", "0");
		fputs(buf,fp);
	}

#if !defined(TCSUPPORT_NP_CMCC)
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN11AC_ENTRY_NODE, wlan_id + 1);
#endif
	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		strncpy(encrytype, tmp, sizeof(encrytype) - 1);	
	}
	if(Is11nWirelessMode5G()){
		if(cfg_get_object_attr(nodeName, "HT_MCS", tmp, sizeof(tmp)) > 0){
			if(atoi(tmp)<=15){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtMcs", tmp);
			}else{
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtMcs", "33");
			}		
		}
	}else{
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HtMcs", "33");
		fputs(buf,fp);
	}
	/*write entry  setting to WLAN_exec.sh*/
	if(cfg_get_object_attr(nodeName, "HideSSID", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HideSSID", tmp);
		fputs_escape(buf,fp);
	}
	if(cfg_get_object_attr(nodeName, "NoForwarding", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "NoForwarding", tmp);
		fputs_escape(buf,fp);
	}
	/* rekeyinterval rekeymethod */
	if(cfg_get_object_attr(nodeName, "RekeyInterval", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RekeyInterval", tmp);
		fputs_escape(buf,fp);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RekeyMethod", "TIME");
		fputs(buf,fp);
	}

	/*AuthMode*/
 	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
			if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "1");
				fputs(buf,fp);
			}else{
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif
			if(cfg_get_object_attr(nodeName, "WEPAuthType", tmp, sizeof(tmp)) > 0){
				if(!strcmp(tmp, "OpenSystem")){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "AuthMode", "OPEN");
					fputs(buf,fp);
				}else if(!strcmp(tmp, "SharedKey")){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "AuthMode", "SHARED");
					fputs(buf,fp);
				}else{
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
					fputs(buf,fp);
				}
			}
			else{
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
				fputs(buf,fp);
			}

			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", "WEP");
			fputs(buf,fp);
			if(cfg_get_object_attr(nodeName, "DefaultKeyID", tmp, sizeof(tmp)) <= 0){
				strcpy(tmp,"1");
			}
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", tmp);
			fputs_escape(buf,fp);
			/*key1~key4*/
			for(i = 1;i <= KEY_NUM; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
					/*check strlen because the key maybe empty*/
					if(strlen(tmp) > 0){
						specialCharacterHandle(tmp);
						snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
						snprintf(tmp, sizeof(tmp), "Key%d", i);
						/*krammer change for bug 1352*/
						snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp, tmp2);
						F_PUTS_NORMAL(buf,fp);
					}
				}
			}
        }
		else if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK")
#if defined(TCSUPPORT_WLAN_8021X)
			|| !strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
								||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
		){/*WPAPSK or WPA2PSK or Mixed mode*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", tmp);
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			wifimgr_lib_get_IEEE8021X_VAL(tmp2, sizeof(tmp2));
			if(!strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", tmp2);
				fputs(buf,fp);
			}else if (0 == strcmp(tmp2, "1")){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif				
			if(cfg_get_object_attr(nodeName, "EncrypType", tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", tmp);
				fputs_escape(buf,fp);
			}

			if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			/*krammer add for bug 1242*/
			if(cfg_get_object_attr(nodeName, "WPAPSK", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WPAPSK", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			/*krammer change*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", "2");
			/*WPAPSK and WPA2PSK use DefaultKeyID = 2*/
			fputs(buf,fp);
			/*krammer change*/
		}
		else{/*OPEN*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");
			fputs(buf,fp);
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", "NONE");
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
			fputs(buf,fp);
#endif
		}
 	}

#ifdef TCSUPPORT_WLAN_PMF
	/*pmf configuretion*/
	if(cfg_get_object_attr(nodeName, "PMFMFPC", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PMFMFPC", tmp);
		fputs_escape(buf,fp);
	}

	if(cfg_get_object_attr(nodeName, "PMFMFPR", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PMFMFPR", tmp);
		fputs_escape(buf,fp);
	}

	if(cfg_get_object_attr(nodeName, "PMFSHA256", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PMFSHA256", tmp);
		fputs_escape(buf,fp);
	}
#endif

#if defined(TCSUPPORT_WLAN_8021X_EXT)
/*RADIUS_Server*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0 ){
			hp = gethostbyname(tmp);
			if(hp != (struct hostent *) NULL){
				memcpy((char *) &RadiusAddr.sin_addr,
					(char *) hp->h_addr_list[0]
					,sizeof(RadiusAddr.sin_addr));
				strcpy(tmp2,
				inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
				strcat(tmp2,";");
			}else{
				strcat(tmp2,"0.0.0.0;");
			}
		}
	}
	memset(tmp,0,sizeof(tmp));
	if(cfg_get_object_attr(nodeName, "BAK_RADIUS_Server", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0 ){
			hp = gethostbyname(tmp);
			if(hp != (struct hostent *) NULL){
				memcpy((char *) &RadiusAddr.sin_addr
					, (char *) hp->h_addr_list[0]
					,sizeof(RadiusAddr.sin_addr));
				strcat(tmp2,
				inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
				strcat(tmp2,";");
			}else{
				strcat(tmp2,"0.0.0.0;");
			}
		}
	}

	if(strlen(tmp2) > 0){
		tmp2[strlen(tmp2)-1]='\0';/*remove last ";"*/
		snprintf(tmp, sizeof(tmp), "\"%s\"",tmp2);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Server", tmp);
		fputs_escape(buf,fp);
	}

	/*RADIUS_Port*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Port", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcpy(tmp2,tmp);
			strcat(tmp2,";");
		}
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_get_object_attr(nodeName, "BAK_RADIUS_Port", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcat(tmp2,tmp);
			strcat(tmp2,";");
		}
	}

	if(strlen(tmp2) > 0){
		tmp2[strlen(tmp2)-1]='\0';/*remove last ";"*/
		snprintf(tmp, sizeof(tmp), "\"%s\"",tmp2);
		snprintf(buf, sizeof(tmp), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Port", tmp);
		fputs_escape(buf, fp);
	}

	/*RADIUS_Key*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Key", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcpy(tmp2,tmp);
			strcat(tmp2,";");
		}
	}

	memset(tmp,0,sizeof(tmp));
	if(cfg_get_object_attr(nodeName, "BAK_RADIUS_Key", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcat(tmp2,tmp);
			strcat(tmp2,";");
		}
	}
	if(strlen(tmp2) > 0){
		tmp2[strlen(tmp2)-1]='\0';/*remove last ";"*/
		snprintf(tmp, sizeof(tmp), "\"%s\"", tmp2);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Key", tmp);
		fputs_escape(buf,fp);
	}
#else
/*RADIUS_Server*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0 ){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Server", tmp);
			fputs_escape(buf,fp);
		}
	}

	/*RADIUS_Port*/
	memset(tmp,0,sizeof(tmp));
	memset(buf,0,sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Port", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Port", tmp);
			fputs_escape(buf,fp);
		}
	}

	/*RADIUS_Key*/
	memset(tmp, 0, sizeof(tmp));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Key", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Key", tmp);
			fputs_escape(buf,fp);
		}
	}
#endif
	/*AccessPolicy=....*/
	if(cfg_get_object_attr(nodeName, "AccessPolicy", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			snprintf(buf, sizeof(tmp), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AccessPolicy", tmp);
			fputs_escape(buf,fp);
		}
	}
	/*AccessControlList=MAC1;MAC2;.......*/

	strcpy(buf,"");
	for(i = 0; i < WLAN_MAC_NUM; i++){
		snprintf(tmp2, sizeof(tmp2), "%s%d", "WLan_MAC", i);
		if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
			if(strlen( tmp) > 0){
				strcat(buf,tmp);
				strcat(buf,";");
			}
		}
	}
	if(strlen(buf) > 0){
		buf[strlen(buf) - 1] ='\0';/*remove last ";"*/
		snprintf(tmp2, sizeof(tmp2), "\"%s\"",buf);/*add ""*/
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "ACLClearAll", "1");
		fputs(buf, fp);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "ACLAddEntry", tmp2);

		fputs_escape(buf,fp);
	}
	
	if(!Is11nWirelessMode5G()){
		if(cfg_get_object_attr(nodeName, "WMM", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WmmCapable", tmp);
			fputs_escape(buf, fp);

			res = txBurst_or_not_ac(BssidNum);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", res);
			res = wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), wlan_id, tmp, 1);
			if (0 == res)
				fputs(buf,fp);
		}
	}
	else {
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WmmCapable", "1");
		fputs(buf,fp);
		res = wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), wlan_id, tmp, 0);
		if (0 == res)
			fputs(buf,fp);
	}

#ifdef WSC_AP_SUPPORT
#ifndef TCSUPPORT_WLAN_MULTI_WPS
	if(wlan_id == 0)/*now we just support main SSID,xyyou*/
#endif
	{

		cfg_set_object_attr(nodeName, "WPSConfStatus", "2");			
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WscConfStatus", "2");									
		fputs(buf,fp);
		if(cfg_get_object_attr(nodeName, "WPSConfMode", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WscConfMode", tmp);
			fputs_escape(buf,fp);
		}			

		if(cfg_get_object_attr(nodeName, "WscV2Support", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WscV2Support", tmp);
			fputs_escape(buf,fp);
		}
		if(cfg_get_object_attr(nodeName, "WscMaxPinAttack", tmp, sizeof(tmp)) > 0){
	        snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX
				, wlan_id, "WscMaxPinAttack", tmp);
	        fputs_escape(buf,fp);
        }
		if(cfg_get_object_attr(nodeName, "WscSetupLockTime", tmp, sizeof(tmp)) > 0){
            snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX
				, wlan_id, "WscSetupLockTime", tmp);
            fputs_escape(buf,fp);
        }
	}
#endif 

	 if(cfg_get_object_attr(nodeName, "KeyPassphrase", tmp, sizeof(tmp)) > 0){
		sprintf(buf,WLAN_AC_SCRIPT_PREFIX , wlan_id,"KeyPassphrase", tmp);
		fputs_escape(buf,fp);
	 }	

	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
		F_PUTS_NORMAL(buf,fp);
	}
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	if(cfg_get_object_attr(nodeName, "MaxStaNum", tmp, sizeof(tmp)) > 0){
		wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(WIFI_5G, tmp2, sizeof(tmp2));
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp2, tmp);
		fputs_escape(buf,fp);
	}
#endif
#if defined(TCSUPPORT_NP_CMCC)
		cfg_set_object_attr(nodeName, "wlan_changed", "0");
	}
	cfg_set_object_attr(WLAN11AC_COMMON_NODE, "alink_type", "0");
#endif

  	fclose(fp);
 	res = chmod(SVC_WLAN11AC_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

  	return 0;
}
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2))
int write_wlan11ac_entry_attr_exe_sh(int BssidNum)
{
	int i, wlan_id;
  	char nodeName[128]		= {0};
	char tmp[160]			= {0}; 
	char tmp2[160]			= {0};
	char buf[160]			= {0};
	char tmpChannel[8] 		= {0};
	char wirelessmode[16]	= {0}; 
	char encrytype[16]		= {0};
  	FILE *fp				= NULL;
	int res = 0;
#if defined(TCSUPPORT_WLAN_8021X_EXT)
	struct hostent *hp = NULL;
	struct sockaddr_in RadiusAddr;
#endif
	char wlan_script_com_attr[][ATTR_SIZE]=
	{
		{"CountryRegion"},
		{"CountryRegionABand"},
		{"Channel"},
		{"BeaconPeriod"},
		{"RTSThreshold"},
		{"FragThreshold"},
		{"DtimPeriod"},
		{"TxPower"},
		{"TxPreamble"},
#if defined(TCSUPPORT_TTNET)
		{"MaxStaNum"},
#endif
		{"BGProtection"},
		{"TxPower"},
		{"TxPreamble"},
		{"ShortSlot"},
		{"TxBurst"},
		{"PktAggregate"},
		{"IEEE80211H"},
		{""}
	};
	
	char wlan_script_com_attrs_other[][2][ATTR_SIZE]=
	{
		{"HT_RDG", 				"HtRdg"},
		{"HT_STBC", 			"HtStbc"},
		{"HT_AMSDU", 			"HtAmsdu"},
		{"HT_AutoBA", 			"HtAutoBa"},
		{"HT_BADecline", 		"BADecline"},
		{"HT_DisallowTKIP", 	"HtDisallowTKIP"},
		{"HT_TxStream", 		"HtTxStream"},
		{"HT_RxStream", 		"HtRxStream"},
		{"APSDCapable", 		"UAPSDCapable"},
		{"HT_BSSCoexistence",	"HtBssCoex"}, 
		{"VHT_STBC", "VhtStbc"},
		{"VHT_BW_SIGNAL", "VhtBwSignal"},
		{"VHT_DisallowNonVHT", "VhtDisallowNonVHT"},

		{"", ""}
	};

  	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
    	wlan_id = atoi(tmp);
  	}else{
    	return -1;
 	}

	/*write Entry setting WLAN_exec.sh*/
	fp = fopen(WLAN11AC_ENTRY_ATTR_SCRIPT_PATH, "w");
	if(NULL == fp){
		return -1;
	}
	
	memset(nodeName,0,sizeof(nodeName));
	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN11AC_ENTRY_NODE, wlan_id + 1);


	/*write entry  setting to WLAN_exec.sh*/
	if(cfg_get_object_attr(nodeName, "HideSSID", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HideSSID", tmp);
		fputs_escape(buf,fp);
	}
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	if(cfg_get_object_attr(nodeName, "MaxStaNum", tmp, sizeof(tmp)) > 0){
		wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(WIFI_5G, tmp2, sizeof(tmp2));
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp2, tmp);
		fputs_escape(buf,fp);
	}
#endif

	/* rekeyinterval rekeymethod */
	if(cfg_get_object_attr(nodeName, "RekeyInterval", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RekeyInterval", tmp);
		fputs_escape(buf,fp);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RekeyMethod", "TIME");
		fputs(buf,fp);
	}

	/*AuthMode*/
 	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
			if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "1");
				fputs(buf,fp);
			}else{
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif
			if(cfg_get_object_attr(nodeName, "WEPAuthType", tmp, sizeof(tmp)) > 0){
				if(!strcmp(tmp, "OpenSystem")){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "AuthMode", "OPEN");
					fputs(buf,fp);
				}else if(!strcmp(tmp, "SharedKey")){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "AuthMode", "SHARED");
					fputs(buf,fp);
				}else{
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
					fputs(buf,fp);
				}
			}

			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", "WEP");
			fputs(buf,fp);
			if(cfg_get_object_attr(nodeName, "DefaultKeyID", tmp, sizeof(tmp)) <= 0){
				strcpy(tmp,"1");
			}
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", tmp);
			fputs_escape(buf,fp);
			/*key1~key4*/
			for(i = 1;i <= KEY_NUM; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
					/*check strlen because the key maybe empty*/
					if(strlen(tmp) > 0){
						specialCharacterHandle(tmp);
						snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
						snprintf(tmp, sizeof(tmp), "Key%d", i);
						/*krammer change for bug 1352*/
						snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp, tmp2);
						F_PUTS_NORMAL(buf,fp);
					}
				}
			}
        }
		else if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK")
#if defined(TCSUPPORT_WLAN_8021X)
			|| !strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#endif
		){/*WPAPSK or WPA2PSK or Mixed mode*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", tmp);
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			wifimgr_lib_get_IEEE8021X_VAL(tmp2, sizeof(tmp2));
			if(!strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", tmp2);
				fputs(buf,fp);
			}else if (0 == strcmp(tmp2, "1")){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif				
			if(cfg_get_object_attr(nodeName, "EncrypType", tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", tmp);
				fputs_escape(buf,fp);
			}

			if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			/*krammer add for bug 1242*/
			if(cfg_get_object_attr(nodeName, "WPAPSK", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WPAPSK", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			/*krammer change*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", "2");
			/*WPAPSK and WPA2PSK use DefaultKeyID = 2*/
			fputs(buf,fp);
			/*krammer change*/
		}
		else{/*OPEN*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");
			fputs(buf,fp);
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", "NONE");
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
			fputs(buf,fp);
#endif
		}
 	}


#if defined(TCSUPPORT_WLAN_8021X_EXT)
/*RADIUS_Server*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0 ){
			hp = gethostbyname(tmp);
			if(hp != (struct hostent *) NULL){
				memcpy((char *) &RadiusAddr.sin_addr,
					(char *) hp->h_addr_list[0]
					,sizeof(RadiusAddr.sin_addr));
				strcpy(tmp2,
				inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
				strcat(tmp2,";");
			}else{
				strcat(tmp2,"0.0.0.0;");
			}
		}
	}
	memset(tmp,0,sizeof(tmp));
	if(cfg_get_object_attr(nodeName, "BAK_RADIUS_Server", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0 ){
			hp = gethostbyname(tmp);
			if(hp != (struct hostent *) NULL){
				memcpy((char *) &RadiusAddr.sin_addr
					, (char *) hp->h_addr_list[0]
					,sizeof(RadiusAddr.sin_addr));
				strcat(tmp2,
				inet_ntoa(*(struct in_addr *) &RadiusAddr.sin_addr.s_addr));
				strcat(tmp2,";");
			}else{
				strcat(tmp2,"0.0.0.0;");
			}
		}
	}

	if(strlen(tmp2) > 0){
		tmp2[strlen(tmp2)-1]='\0';/*remove last ";"*/
		snprintf(tmp, sizeof(tmp), "\"%s\"",tmp2);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Server", tmp);
		fputs_escape(buf,fp);
	}

	/*RADIUS_Port*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Port", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcpy(tmp2,tmp);
			strcat(tmp2,";");
		}
	}

	memset(tmp, 0, sizeof(tmp));
	if(cfg_get_object_attr(nodeName, "BAK_RADIUS_Port", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcat(tmp2,tmp);
			strcat(tmp2,";");
		}
	}

	if(strlen(tmp2) > 0){
		tmp2[strlen(tmp2)-1]='\0';/*remove last ";"*/
		snprintf(tmp, sizeof(tmp), "\"%s\"",tmp2);
		snprintf(buf, sizeof(tmp), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Port", tmp);
		fputs_escape(buf, fp);
	}

	/*RADIUS_Key*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Key", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcpy(tmp2,tmp);
			strcat(tmp2,";");
		}
	}

	memset(tmp,0,sizeof(tmp));
	if(cfg_get_object_attr(nodeName, "BAK_RADIUS_Key", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			strcat(tmp2,tmp);
			strcat(tmp2,";");
		}
	}
	if(strlen(tmp2) > 0){
		tmp2[strlen(tmp2)-1]='\0';/*remove last ";"*/
		snprintf(tmp, sizeof(tmp), "\"%s\"", tmp2);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Key", tmp);
		fputs_escape(buf,fp);
	}
#else
/*RADIUS_Server*/
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Server", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0 ){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Server", tmp);
			fputs_escape(buf,fp);
		}
	}

	/*RADIUS_Port*/
	memset(tmp,0,sizeof(tmp));
	memset(buf,0,sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Port", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Port", tmp);
			fputs_escape(buf,fp);
		}
	}

	/*RADIUS_Key*/
	memset(tmp, 0, sizeof(tmp));
	memset(buf, 0, sizeof(buf));
	if(cfg_get_object_attr(nodeName, "RADIUS_Key", tmp, sizeof(tmp)) > 0){
		if(strlen(tmp) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "RADIUS_Key", tmp);
			fputs_escape(buf,fp);
		}
	}
#endif
	if(!Is11nWirelessMode5G()){
		if(cfg_get_object_attr(nodeName, "WMM", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WmmCapable", tmp);
			fputs_escape(buf, fp);

			res = txBurst_or_not_ac(BssidNum);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", res);
			res = wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), wlan_id, tmp, 1);
			if (0 == res)
				fputs(buf,fp);
		}
	}
	else {
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WmmCapable", "1");
		fputs(buf,fp);
		res = wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), wlan_id, tmp, 0);
		if (0 == res)
			fputs(buf,fp);
	}
	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "TxPower", tmp, sizeof(tmp)) > 0)
	{
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
		fputs_escape(buf,fp);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", tmp);
		fputs_escape(buf,fp);

	}
	
	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
		F_PUTS_NORMAL(buf,fp);
	}

  	fclose(fp);
 	res = chmod(WLAN11AC_ENTRY_ATTR_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

  	return 0;
}

int write_wlan_entry_7915D_sh(int BssidNum)
{
	char nodeName[128]		= {0};
	char tmp[160]			= {0};
	char tmp2[160]			= {0};
	char buf[200]			= {0};
	char wirelessmode[16]	= {0};
	char encrytype[16]		= {0};
  	FILE *fp				= NULL;
	int i, wlan_id;
	int res = 0;
	
	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_id", tmp, sizeof(tmp)) > 0){
		wlan_id = atoi(tmp);
	}else{
		return -1;
	}

	/*write common setting WLAN_exec.sh*/
	fp = fopen(SVC_WLAN_SCRIPT_PATH, "w");
	if(NULL == fp){
		return -1;
	}
	
	memset(nodeName, 0, sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN_ENTRY_NODE, wlan_id + 1);
	
	if(cfg_get_object_attr(nodeName, "HideSSID", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX , wlan_id, "HideSSID", tmp);
		fputs_escape(buf,fp);
	}

	/*AuthMode*/
 	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
			if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "1");
				fputs(buf,fp);
			}else{
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif

#if defined(TCSUPPORT_CT_WLAN_NODE)
			if(cfg_get_object_attr(nodeName, "WEPAuthType", tmp, sizeof(tmp)) > 0){
				if(!strcmp(tmp, "OpenSystem")){
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");
					fputs(buf,fp);
				}else if(!strcmp(tmp, "SharedKey")){
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "SHARED");
					fputs(buf,fp);
				}else{
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
					fputs(buf,fp);
				}
			}
			else{
					snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
					fputs(buf,fp);
			}
#else
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
			fputs(buf,fp);
#endif
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "EncrypType", "WEP");
			fputs(buf,fp);
			if(cfg_get_object_attr(nodeName, "DefaultKeyID", tmp, sizeof(tmp)) <= 0){
				strncpy(tmp, "1", sizeof(tmp));
			}
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", tmp);
			fputs(buf,fp);
			/*key1~key4*/
			for(i = 1;i <= KEY_NUM; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
					/*check strlen because the key maybe empty*/
					if(strlen(tmp) > 0){
						specialCharacterHandle(tmp);
						snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
						snprintf(tmp, sizeof(tmp), "Key%d", i);
						snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, tmp, tmp2);
						F_PUTS_NORMAL(buf,fp);
					}
				}
			}
    	}
		else if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK") /*WPAPSK or WPA2PSK or Mixed mode*/
#if defined(TCSUPPORT_WLAN_8021X)
						|| !strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
						||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
			){/*WPAPSK or WPA2PSK or Mixed mode*/
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", tmp);
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			wifimgr_lib_get_IEEE8021X_VAL(tmp2, sizeof(tmp2));
			if(!strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", tmp2);
				fputs(buf,fp);
			}else if (0 == strcmp(tmp2, "1")){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif
			if(cfg_get_object_attr(nodeName, "EncrypType", tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "EncrypType", tmp);
				fputs_escape(buf,fp);
			}

			if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			if(cfg_get_object_attr(nodeName, "WPAPSK", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WPAPSK", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", "2");/*WPAPSK and WPA2PSK use DefaultKeyID = 2*/
			fputs(buf,fp);
		}
		else{/*OPEN*/
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");
			fputs(buf,fp);
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "EncrypType", "NONE");
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
			fputs(buf,fp);
#endif

		}
 	}

#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	if(cfg_get_object_attr(nodeName, "MaxStaNum", tmp, sizeof(tmp)) > 0){
		wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(WIFI_2_4G, tmp2, sizeof(tmp2));
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp2, tmp);
		fputs_escape(buf,fp);
	}
#endif
	if(!Is11nWirelessMode()){
		if(cfg_get_object_attr(nodeName, "WMM", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "WmmCapable", tmp);
			fputs_escape(buf, fp);
			if(txBurst_or_not(BssidNum)){
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxBurst", "1");
			}
			else{
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "TxBurst", "0");
			}
			fputs_escape(buf,fp);
		}
	}
	if(cfg_get_object_attr(WLAN_COMMON_NODE, "TxPowerLevel", tmp, sizeof(tmp)) > 0){
		switch(atoi(tmp)){
			case 1:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
				fputs_escape(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", "75");
				fputs_escape(buf,fp);
				break;
			case 2:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
				fputs_escape(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", "50");
				fputs_escape(buf,fp);
				break;
			case 3:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
				fputs_escape(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", "25");
				fputs_escape(buf,fp);
				break;
			case 4:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
				fputs_escape(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", "15");
				fputs_escape(buf,fp);
				break;
			case 5:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
				fputs_escape(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", "9");
				fputs_escape(buf,fp);
				break;
			case 6:
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
				fputs_escape(buf,fp);
				snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", "100");
				fputs_escape(buf,fp);
				break;
			default:
				tcdbg_printf("WLan:the valule of TxPowerLevel is not right!\n");
				break;
		}
	}
	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
		snprintf(buf, sizeof(tmp), WLAN_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
		F_PUTS_NORMAL(buf,fp);
	}

	fclose(fp);
 	res = chmod(SVC_WLAN_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

}

int write_wlan11ac_entry_7915D_sh(int BssidNum)
{
	int i, wlan_id;
	char nodeName[128]		= {0};
	char tmp[160]			= {0}; 
	char tmp2[160]			= {0};
	char buf[160]			= {0};
	char tmpChannel[8]		= {0};
	char wirelessmode[16]	= {0}; 
	char encrytype[16]		= {0};
	FILE *fp				= NULL;
	int res = 0;

	if(cfg_get_object_attr(WEBCURSET_ENTRY_NODE, "wlan_ac_id", tmp, sizeof(tmp)) > 0){
    	wlan_id = atoi(tmp);
  	}else{
    	return -1;
 	}

	/*write Entry setting WLAN_exec.sh*/
	fp = fopen(WLAN11AC_ENTRY_ATTR_SCRIPT_PATH, "w");
	if(NULL == fp){
		return -1;
	}

	memset(nodeName,0,sizeof(nodeName));
	snprintf(nodeName, sizeof(nodeName), "%s.%d", WLAN11AC_ENTRY_NODE, wlan_id + 1);


	/*write entry  setting to WLAN_exec.sh*/
	if(cfg_get_object_attr(nodeName, "HideSSID", tmp, sizeof(tmp)) > 0){
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "HideSSID", tmp);
		fputs_escape(buf,fp);
	}

	/*AuthMode*/
 	if(cfg_get_object_attr(nodeName, "AuthMode", tmp, sizeof(tmp)) > 0){
		if(strstr(tmp, "WEP")){/*WEP64-bits or WEP128-bits*/
#if defined(TCSUPPORT_WLAN_8021X)
			if(strstr(tmp, "Radius")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "1");
				fputs(buf,fp);
			}else{
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif
			if(cfg_get_object_attr(nodeName, "WEPAuthType", tmp, sizeof(tmp)) > 0){
				if(!strcmp(tmp, "OpenSystem")){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "AuthMode", "OPEN");
					fputs(buf,fp);
				}else if(!strcmp(tmp, "SharedKey")){
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX , wlan_id, "AuthMode", "SHARED");
					fputs(buf,fp);
				}else{
					snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "WEPAUTO");
					fputs(buf,fp);
				}
			}

			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", "WEP");
			fputs(buf,fp);
			if(cfg_get_object_attr(nodeName, "DefaultKeyID", tmp, sizeof(tmp)) <= 0){
				strcpy(tmp,"1");
			}
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", tmp);
			fputs_escape(buf,fp);
			/*key1~key4*/
			for(i = 1;i <= KEY_NUM; i++){
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				if(cfg_get_object_attr(nodeName, tmp2, tmp, sizeof(tmp)) > 0){
					/*check strlen because the key maybe empty*/
					if(strlen(tmp) > 0){
						specialCharacterHandle(tmp);
						snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
						snprintf(tmp, sizeof(tmp), "Key%d", i);
						/*krammer change for bug 1352*/
						snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp, tmp2);
						F_PUTS_NORMAL(buf,fp);
					}
				}
			}
        }
		else if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK")
#if defined(TCSUPPORT_WLAN_8021X)
			|| !strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
								||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
		){/*WPAPSK or WPA2PSK or Mixed mode*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", tmp);
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			wifimgr_lib_get_IEEE8021X_VAL(tmp2, sizeof(tmp2));
			if(!strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")){/*Radius-WEP64, Radius-WEP128*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", tmp2);
				fputs(buf,fp);
			}else if (0 == strcmp(tmp2, "1")){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
				fputs(buf,fp);
			}
#endif
			if(cfg_get_object_attr(nodeName, "EncrypType", tmp, sizeof(tmp)) > 0){
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", tmp);
				fputs_escape(buf,fp);
			}

			if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			/*krammer add for bug 1242*/
			if(cfg_get_object_attr(nodeName, "WPAPSK", tmp, sizeof(tmp)) > 0){
				specialCharacterHandle(tmp);
				snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
				snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WPAPSK", tmp2);
				F_PUTS_NORMAL(buf,fp);
			}
			/*krammer change*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "DefaultKeyID", "2");
			/*WPAPSK and WPA2PSK use DefaultKeyID = 2*/
			fputs(buf,fp);
			/*krammer change*/
		}
		else{/*OPEN*/
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "AuthMode", "OPEN");
			fputs(buf,fp);
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "EncrypType", "NONE");
			fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_8021X)
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "IEEE8021X", "0");
			fputs(buf,fp);
#endif
		}
 	}

	
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
	if(cfg_get_object_attr(nodeName, "MaxStaNum", tmp, sizeof(tmp)) > 0){
		wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(WIFI_5G, tmp2, sizeof(tmp2));
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, tmp2, tmp);
		fputs_escape(buf,fp);
	}
#endif
	if(!Is11nWirelessMode5G()){
		if(cfg_get_object_attr(nodeName, "WMM", tmp, sizeof(tmp)) > 0){
			snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WmmCapable", tmp);
			fputs_escape(buf, fp);

			res = txBurst_or_not_ac(BssidNum);
			memset(tmp, 0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", res);
			res = wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), wlan_id, tmp, 1);
			if (0 == res)
				fputs(buf,fp);
		}
	}
	else {
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "WmmCapable", "1");
		fputs(buf,fp);
		res = wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), wlan_id, tmp, 0);
		if (0 == res)
			fputs(buf,fp);
	}

	if(cfg_get_object_attr(WLAN11AC_COMMON_NODE, "TxPower", tmp, sizeof(tmp)) > 0)
	{
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PercentageCtrl", "1");
		fputs_escape(buf,fp);
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "PowerDropCtrl", tmp);
		fputs_escape(buf,fp);
	}
	if(cfg_get_object_attr(nodeName, "SSID", tmp, sizeof(tmp)) > 0){
		specialCharacterHandle(tmp);
		snprintf(tmp2, sizeof(tmp2), "\"%s\"",tmp);/*need "..."*/
		snprintf(buf, sizeof(buf), WLAN_AC_SCRIPT_PREFIX, wlan_id, "SSID", tmp2);
		F_PUTS_NORMAL(buf,fp);
	}
	
	fclose(fp);
	res = chmod(WLAN11AC_ENTRY_ATTR_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);

}

#endif

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CUC)
/*for start cc*/
int guest_cc(void)
{
	struct timeval curTime;
	int i = 0;
	int flag = 0;
	char enable[8] = {0}, endtime[16] = {0};
	char guestNode[64]={0};
	long sec_time = 0, timelist = 0;
	static long times = 0;
	static long remain_time[GUESTSSIDINFO_NUM] = {-1, -1};

	times += 1;
	if (times % 10 != 0)
		return 0;

	memset(guestNode, 0, sizeof(guestNode));
	for (i=0; i < GUESTSSIDINFO_NUM; i++) {
		snprintf(guestNode, sizeof(guestNode), GUESTSSIDINFO_ENTRY_NODE, i+1);
		
		if (cfg_obj_get_object_attr(guestNode, "EnableSSID", 0, enable, sizeof(enable)) < 0 
			|| strncmp(enable, "1", sizeof(enable) - 1)) {
			continue;
		}
		if (cfg_obj_get_object_attr(guestNode, "EndTime", 0, endtime, sizeof(endtime)) < 0) {
			continue;
		}
		timelist = atol(endtime); 	/*get endtime*/
		if (timelist == 0) {  /*endtime = 0, endless*/
			continue;
		}
		gettimeofday(&curTime, NULL);	/*get cur time*/
		sec_time = curTime.tv_sec +curTime.tv_usec/1000000;
		
		/*if curtime and endtime are one year apart, recalculate the endtime*/
		if ((sec_time > timelist + GUESTSSIDINFO_MAX_TIME) || (timelist > sec_time + GUESTSSIDINFO_MAX_TIME)) {
				timelist= sec_time + remain_time[i] - 60;
				snprintf(endtime, sizeof(endtime), "%ld", timelist);
				cfg_set_object_attr(guestNode, "EndTime", endtime);
		}
		
		if (sec_time > timelist) {
			cfg_set_object_attr(guestNode, "EnableSSID", "2");
			flag = 1;
		}
		else {
			remain_time[i]= timelist - sec_time;
		}
	
	}

	if (flag == 1)
	{
		cfg_commit_object(GUESTSSIDINFO_NODE);
		cfg_evt_write_romfile_to_flash();
	}
	return 0;
}
#endif

#ifdef TCSUPPORT_WLAN_BNDSTRG
void Bndstrg_check_wlanload( ){
	char tmp[8]	= {0};
	
	if (cfg_get_object_attr(GLOBALSTATE_COMMON_NODE, "wlanLoad", tmp, sizeof(tmp)) > 0
		&& strcmp(tmp, "1") == 0){
		svc_wlan_Bndstrg_execute(WLAN_BNDSTRG_NODE);	
	}
	return;
	
}
int SameSSIDIndex( int *wlan_id, int *wlan11ac_id)
{
	char tmp1[4] = {0};
	char tmp2[4] = {0};
	char buf1[4] = {0};
	char buf2[4] = {0};
	
	if ( cfg_get_object_attr(WLAN_COMMON_NODE, "BssidNum", tmp1, sizeof(tmp1)) < 0 ) {
		return 0;
	}
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "SSIDIndex24", buf1, sizeof(buf1)) < 0 ) {
		return 0;
	}
	if ( atoi(buf1) > atoi(tmp1) ) {
		return 0;
	}
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "BssidNum", tmp2, sizeof(tmp2)) < 0 ) {
		return 0;
	}
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "SSIDIndex58", buf2, sizeof(buf2)) < 0 ) {
		return 0 ;
	}
	if ( atoi(buf2) > atoi(tmp2) ) {
		return 0;
	}
	if ( atoi(buf1) > 0 && atoi(buf2) > 0 ) {
		*wlan_id = atoi(buf1);
		*wlan11ac_id = atoi(buf2);
		return 1;
	}
	
	return 0;
}

void SetSameKey(char *nodenameSrc,char *nodenameDest,char *wlan_script_prefix,int wifi_id_dest,FILE *fp )
{
	char authmod[20]={0};
	char tmp1[160] = {0}, tmp2[160] = {0}, buf[160] = {0};
	int i = 0;
	
	if ( cfg_get_object_attr(nodenameSrc, "AuthMode", authmod, sizeof(authmod)) > 0 )		
		cfg_set_object_attr(nodenameDest,"AuthMode",authmod);

	if ( strstr(authmod, "WEP-") )
	{						/*WEP64-bits or WEP128-bits*/
		if ( cfg_get_object_attr(nodenameSrc, "WEPAuthType", tmp1, sizeof(tmp1)) > 0 )
		{
			if ( !strcmp(tmp1, "OpenSystem") )
			{
				snprintf(buf, sizeof(buf), wlan_script_prefix , wifi_id_dest, "AuthMode", "OPEN");	
	}
			else if ( !strcmp(tmp1, "SharedKey") )
			{
				snprintf(buf, sizeof(buf), wlan_script_prefix , wifi_id_dest, "AuthMode", "SHARED");
					} 
			else 
			{
				snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "AuthMode", "WEPAUTO");
					}
						fputs(buf,fp);
			cfg_set_object_attr(nodenameDest,"WEPAuthType",tmp1);		
				}

		snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "EncrypType", "WEP");
				fputs(buf,fp);
				
		if ( cfg_get_object_attr(nodenameSrc, "DefaultKeyID", tmp1, sizeof(tmp1)) > 0 )
		{
			cfg_set_object_attr(nodenameDest,"DefaultKeyID",tmp1);
			snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "DefaultKeyID", tmp1);
			fputs_escape(buf,fp);
				}
								
				/*key1~key4*/
		for ( i = 1; i <= KEY_NUM; i++ )
		{
					snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
			if ( cfg_get_object_attr(nodenameSrc, tmp2, tmp1, sizeof(tmp1)) > 0 )
			{
				cfg_set_object_attr(nodenameDest,tmp2,tmp1);

						/*check strlen because the key maybe empty*/
				if ( strlen(tmp1) > 0 )
				{
							specialCharacterHandle(tmp1);
							snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp1);		/*need "..."*/
							snprintf(tmp1, sizeof(tmp1), "Key%d", i);
							/*krammer change for bug 1352*/
					snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, tmp1, tmp2);
							F_PUTS_NORMAL(buf,fp);
						}
					}
				}
	    	}
	else if ( !strcmp(authmod, "WPAPSK") || !strcmp(authmod, "WPA2PSK") || !strcmp(authmod, "WPAPSKWPA2PSK") 
		#if defined(TCSUPPORT_WLAN_WPA3)
								||!strcmp(authmod, "WPA3PSK")||!strcmp(authmod, "WPA2PSKWPA3PSK")
		#endif

	) 	/*WPAPSK or WPA2PSK or Mixed mode*/
	{
		snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "AuthMode", authmod);
				fputs(buf,fp);
		if ( cfg_get_object_attr(nodenameSrc, "EncrypType", tmp1, sizeof(tmp1)) > 0 )
		{
			snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "EncrypType", tmp1);
					fputs_escape(buf,fp);
			cfg_set_object_attr(nodenameDest,"EncrypType",tmp1);
				}
				/*krammer add for bug 1242*/
		if ( cfg_get_object_attr(nodenameSrc, "WPAPSK", tmp1, sizeof(tmp1)) > 0 )
		{
					specialCharacterHandle(tmp1);
					snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp1);/*need "..."*/
			snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "WPAPSK", tmp2);
					F_PUTS_NORMAL(buf,fp);
			cfg_set_object_attr(nodenameDest,"WPAPSK",tmp1);
				}
				/*krammer change*/
		if ( cfg_get_object_attr(nodenameSrc, "DefaultKeyID", tmp1, sizeof(tmp1)) > 0 )
		{
			cfg_set_object_attr(nodenameDest,"DefaultKeyID",tmp1);
			snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "DefaultKeyID", tmp1);
			fputs_escape(buf,fp);
		}


			}
	else
	{
		snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "AuthMode", "OPEN");
				fputs(buf,fp);
		snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "EncrypType", "NONE");
		cfg_set_object_attr(nodenameDest,"EncrypType","NONE");
				fputs(buf,fp);		
			}
			
	return;

}
void SetSSID(char *nodenameSrc,char *nodenameDest,char *wlan_script_prefix,int wifi_id_dest,FILE *fp,int suffix_5g)
{
	char tmp1[160] = {0}, tmp2[160] = {0}, buf[160] = {0};

	if ( cfg_get_object_attr(nodenameSrc, "SSID", tmp1, sizeof(tmp1)) > 0 )
	{
			specialCharacterHandle(tmp1);
		snprintf(tmp2, sizeof(tmp2), tmp1);
		
		if(suffix_5g)
			snprintf(tmp2, sizeof(tmp2), "\"%s-5G\"", tmp1);
		
		snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "SSID", tmp2);
			F_PUTS_NORMAL(buf,fp);

		if(1 == suffix_5g)
			snprintf(tmp2, sizeof(tmp2), "%s-5G", tmp1);
		
		cfg_set_object_attr(nodenameDest,"SSID",tmp2);
	}
	return;

}
void SetHideSSID(char *nodenameSrc,char *nodenameDest,char *wlan_script_prefix,int wifi_id_dest,FILE *fp)
{
	char hidessidstr[160] = {0}, buf[160] = {0};

	if(cfg_get_object_attr(nodenameSrc, "HideSSID", hidessidstr, sizeof(hidessidstr)) > 0)
	{
		snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "HideSSID", hidessidstr);
		fputs_escape(buf,fp);
		cfg_set_object_attr(nodenameDest,"HideSSID",hidessidstr);
	}
	return;
}

void SetEnableSSID(char *nodenameSrc,char *nodenameDest,int wifi_id_dest, wifi_type type_dest)
{
	char enablessidstr[160] = {0}, buf[160] = {0};
	
	if(cfg_get_object_attr(nodenameSrc, "EnableSSID", enablessidstr, sizeof(enablessidstr)) > 0)
	{
		cfg_set_object_attr(nodenameDest,"EnableSSID",enablessidstr);
		if (WIFI_2_4G == type_dest)
		{
			if (0 == atoi(enablessidstr))
				snprintf(buf, sizeof(buf), "ifconfig ra%d down", wifi_id_dest);
			else
				snprintf(buf, sizeof(buf), "ifconfig ra%d up", wifi_id_dest);
		}
		else if (WIFI_5G == type_dest)
		{
			if (0 == atoi(enablessidstr))
				snprintf(buf, sizeof(buf), "ifconfig rai%d down", wifi_id_dest);
			else
				snprintf(buf, sizeof(buf), "ifconfig rai%d up", wifi_id_dest);
		}
		system(buf);
		cfg_commit_object(WLAN_BNDSTRG_NODE);
	}
	return;
}

void SetPowerLevel(char *nodenameSrc,char *nodenameDest,char *wlan_script_prefix,int wifi_id_dest,FILE *fp, wifi_type type_dest)
{
	char tmp1[160] = {0}, tmp2[160] = {0}, tmp3[160] = {0}, buf[160] = {0};

	if (WIFI_2_4G == type_dest)
		{
			if(cfg_get_object_attr(nodenameSrc, "TxPower", tmp1, sizeof(tmp1)) > 0)
			{
				switch(atoi(tmp1))
				{
					case 8:
						snprintf(tmp2, sizeof(tmp2), "1");
						snprintf(tmp3, sizeof(tmp3), "75");
						break;
					case 60:
						snprintf(tmp2, sizeof(tmp2), "2");
						snprintf(tmp3, sizeof(tmp3), "50");
						break;
					case 30:
						snprintf(tmp2, sizeof(tmp2), "3");
						snprintf(tmp3, sizeof(tmp3), "25");
						break;
					case 15:
						snprintf(tmp2, sizeof(tmp2), "4");
						snprintf(tmp3, sizeof(tmp3), "15");
						break;
					case 9:
						snprintf(tmp2, sizeof(tmp2), "5");
						snprintf(tmp3, sizeof(tmp3), "9");
						break;
					case 100:
						snprintf(tmp2, sizeof(tmp2), "6");
						snprintf(tmp3, sizeof(tmp3), "100");
						break;
					default:
						memset(tmp2, 0, sizeof(tmp2));
				}
				if ('\0' != tmp2[0])
				{
					snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "TxPower", tmp3);
					fputs_escape(buf,fp);
					cfg_set_object_attr(nodenameDest,"TxPowerLevel",tmp2);
				}
			}
		}
		else if (WIFI_5G == type_dest)
		{
			if(cfg_get_object_attr(nodenameSrc, "TxPowerLevel", tmp1, sizeof(tmp1)) > 0)
			{
				switch(atoi(tmp1))
				{
					case 1:
						snprintf(tmp2, sizeof(tmp2), "80");
						break;
					case 2:
						snprintf(tmp2, sizeof(tmp2), "60");
						break;
					case 3:
						snprintf(tmp2, sizeof(tmp2), "30");
						break;
					case 4:
						snprintf(tmp2, sizeof(tmp2), "15");
						break;
					case 5:
						snprintf(tmp2, sizeof(tmp2), "9");
						break;
					case 6:
						snprintf(tmp2, sizeof(tmp2), "100");
						break;
					default:
						memset(tmp2, 0, sizeof(tmp2));
						break;
				}
				
				if ('\0' != tmp2[0])
				{
					snprintf(buf, sizeof(buf), wlan_script_prefix, wifi_id_dest, "TxPower", tmp2);
					fputs_escape(buf,fp);
					cfg_set_object_attr(nodenameDest,"TxPower",tmp2);
				}
			}
		}
		
		if(cfg_get_object_attr(nodenameSrc, "PowerLevel", tmp1, sizeof(tmp1)) > 0)
		{
			cfg_set_object_attr(nodenameDest,"PowerLevel",tmp1);
		}
		
		if(cfg_get_object_attr(nodenameSrc, "PaMode", tmp1, sizeof(tmp1)) > 0)
		{
			cfg_set_object_attr(nodenameDest,"PaMode",tmp1);
		}
		return;
}

void SetSameSSIDandKey(char * action,char * wifitype)
{
	int i = 0, wlan_id = 0, wlan11ac_id = 0, end = 0;
	char tmp1[160] = {0}, tmp2[160] = {0}, buf[160] = {0};
	char authmod[20]={0};
	char nodenameDest[32] = {0}, nodenameSrc[32] = {0};
	FILE *fp  = NULL;
	char wlan_script_path[SVC_WLAN_PATH_SIZE]= {0};
	int wifi_id_dest=0;
	wifi_type type_dest = WIFI_2_4G; 
	char wlan_common_node_dest[SVC_WLAN_PATH_SIZE] = {0};
	char wlan_common_node_src[SVC_WLAN_PATH_SIZE] = {0};
	char wlan_script_prefix[64]= {0};
	int suffix_5g = 0;
	int ret = 0;

	if ( !SameSSIDIndex(&wlan_id, &wlan11ac_id) )
	{
		tcdbg_printf("SameSSIDIndex not same \n");
		return;	
	}
	
	if(!strcmp(wifitype,"wlan11ac") || !strcmp(action,"cancel"))
	{			
		wifi_id_dest = wlan11ac_id - 1;
		type_dest = WIFI_5G ;
		strncpy(wlan_script_path, SVC_WLAN11AC_SCRIPT_PATH,sizeof(wlan_script_path)-1);
		strncpy(wlan_script_prefix, WLAN_AC_SCRIPT_PREFIX,sizeof(wlan_script_prefix) - 1);
		snprintf(nodenameDest, sizeof(nodenameSrc), WLAN11AC_ENTRY_N_NODE, wlan_id);
		snprintf(nodenameSrc, sizeof(nodenameDest), WLAN_ENTRY_N_NODE, wlan11ac_id);		
		strncpy(wlan_common_node_dest, WLAN11AC_COMMON_NODE,sizeof(wlan_common_node_dest)-1);	
		strncpy(wlan_common_node_src, WLAN_COMMON_NODE,sizeof(wlan_common_node_src)-1);
	}
	else
	{	
		wifi_id_dest = wlan_id - 1;
		type_dest = WIFI_2_4G ;
		strncpy(wlan_script_path, SVC_WLAN_SCRIPT_PATH,sizeof(wlan_script_path)-1);
		strncpy(wlan_script_prefix, WLAN_SCRIPT_PREFIX,sizeof(wlan_script_prefix) - 1);
		snprintf(nodenameDest, sizeof(nodenameSrc), WLAN_ENTRY_N_NODE, wlan11ac_id);
		snprintf(nodenameSrc, sizeof(nodenameDest), WLAN11AC_ENTRY_N_NODE, wlan_id);
		strncpy(wlan_common_node_dest, WLAN_COMMON_NODE,sizeof(wlan_common_node_dest)-1);
		strncpy(wlan_common_node_src, WLAN11AC_COMMON_NODE,sizeof(wlan_common_node_src)-1);
	}	
	
	fp = fopen(wlan_script_path, "w");
	if ( NULL == fp ){
		return;
	}	

	if ( !strcmp(action,"set") ) 
	{
		SetSameKey(nodenameSrc,nodenameDest,wlan_script_prefix,wifi_id_dest,fp );
		SetHideSSID(nodenameSrc,nodenameDest,wlan_script_prefix,wifi_id_dest,fp);
		SetPowerLevel(wlan_common_node_src, wlan_common_node_dest, wlan_script_prefix,wifi_id_dest,fp, type_dest);
		SetEnableSSID(nodenameSrc, nodenameDest, wifi_id_dest, type_dest);
		SetSSID(nodenameSrc,nodenameDest,wlan_script_prefix,wifi_id_dest,fp,suffix_5g);
		cfg_set_object_attr(WLAN11AC_COMMON_NODE, "SameSSIDStatus", "1");
		end = 1;
	}
	else if ( !strcmp(action,"cancel") )
	{
		suffix_5g = 1;
		SetSSID(nodenameSrc,nodenameDest,wlan_script_prefix,wifi_id_dest,fp,suffix_5g);
		cfg_set_object_attr(WLAN11AC_COMMON_NODE, "SameSSIDStatus", "0");
		end = 1;
	}	
	
  	fclose(fp);
	
	if (end)
	{
	 	ret = chmod(wlan_script_path, S_IRUSR|S_IWUSR|S_IXUSR);
		if (ret < 0)
			tcdbg_printf("chmod error for %s\n", wlan_script_path);
		
		system(wlan_script_path);
	}	

	return;
}
void SetSameSSIDCommonExcute(char* path)
{
	char tmp[30] = {0};
	//tcdbg_printf("SameSSIDCommonExcute in\n");
	if ( cfg_get_object_attr(WLAN11AC_COMMON_NODE, "Action", tmp, sizeof(tmp)) < 0 ) {
		tcdbg_printf("no action attr \n");
		return;
	}
	SetSameSSIDandKey(tmp,"wlan11ac");		
}
int BndstrgParaCheck()
{
	char attrtmp[80] 	= {0};
	char attrtmp1[80] 	= {0};
	char APOn2G[4] 	= {0}; 
	char APOn5G[4] 	= {0}; 
	char KeyID[4] 	= {0}; 
	char Keystr[20] 	= {0};
	
	if (cfg_get_object_attr(WLAN_COMMON_NODE, "APOn", APOn2G, sizeof(APOn2G)) < 0) 		
		return 0;
	if (cfg_get_object_attr(WLAN11AC_COMMON_NODE, "APOn", APOn5G, sizeof(APOn5G)) < 0) 		
		return 0;
	if (!strcmp(APOn2G, WLAN_APON) && !strcmp(APOn5G, WLAN_APON)){

		memset(attrtmp,0,sizeof(attrtmp));
		memset(attrtmp1,0,sizeof(attrtmp1));
		if (cfg_get_object_attr(WLAN_ENTRY1_NODE, "SSID", attrtmp, sizeof(attrtmp))<0)
			return 0;
		if (cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, "SSID", attrtmp1, sizeof(attrtmp1))<0)
			return 0;
		if (!strcmp(attrtmp,attrtmp1)){
			
			memset(attrtmp,0,sizeof(attrtmp));
			memset(attrtmp1,0,sizeof(attrtmp1));
			
			if (cfg_get_object_attr(WLAN_ENTRY1_NODE, "AuthMode", attrtmp, sizeof(attrtmp)) < 0)
				return 0;
			if (cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, "AuthMode", attrtmp1, sizeof(attrtmp1))<0)
				return 0;
			
			if (!strcmp(attrtmp,attrtmp1))/*2G & 5G SSID same*/{
					if	(strstr(attrtmp,"WEP-"))/*wep mode*/{
							if (cfg_get_object_attr(WLAN_ENTRY1_NODE, "DefaultKeyID", KeyID, sizeof(KeyID)) <= 0)
								return 0;
							if (atoi(KeyID)>4 || atoi(KeyID)<1)
								return 0;
							snprintf(Keystr,sizeof(Keystr),"Key%dStr",atoi(KeyID));
							memset(attrtmp,0,sizeof(attrtmp));
							memset(attrtmp1,0,sizeof(attrtmp1));
							if (cfg_get_object_attr(WLAN_ENTRY1_NODE, Keystr, attrtmp, sizeof(attrtmp)) < 0)
								return 0;
							if (cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, Keystr, attrtmp1, sizeof(attrtmp1)) < 0)
								return 0;
							if (!strcmp(attrtmp,attrtmp1))
								return 1;
							else 
								return 0;
						}
					else if (!strcmp(attrtmp, "WPAPSK") || !strcmp(attrtmp, "WPA2PSK") || !strcmp(attrtmp, "WPAPSKWPA2PSK"))/*wpa mode*/{
							memset(attrtmp,0,sizeof(attrtmp));
							memset(attrtmp1,0,sizeof(attrtmp1));
							if (cfg_get_object_attr(WLAN_ENTRY1_NODE, "WPAPSK", attrtmp, sizeof(attrtmp)) < 0)
								return 0;
							if (cfg_get_object_attr(WLAN11AC_ENTRY1_NODE, "WPAPSK", attrtmp1, sizeof(attrtmp1)) < 0)
								return 0;	
							if (!strcmp(attrtmp,attrtmp1))
								return 1;
							else 
								return 0;
							
					}
					else/*open mode*/
						return 1;
				}
			}
		}
	return 0;
}
int BndstrgProceessExist()
{
	char buf[8];
	char bndstrg_pid_tmp[10];
	int bndstrg_pid;
	
	memset(bndstrg_pid_tmp, 0, sizeof(bndstrg_pid_tmp));
	FILE *fp = NULL;
#if defined(TCSUPPORT_CT_JOYME4)	
  	system("pidof bs20 > /tmp/bs20.pid");  
	fp = fopen("/tmp/bs20.pid", "r");
	if (fp != NULL) {
		fgets(bndstrg_pid_tmp, sizeof(bndstrg_pid_tmp), fp);
		fclose(fp);
		unlink("/tmp/bs20.pid");
		bndstrg_pid = atoi(bndstrg_pid_tmp);
	    if (bndstrg_pid != 0) {  /*if pid != 0, that's mean old igmpproxy process already exist*/
		return 1;
	    	}
	}	
#else
   	system("pidof bndstrg2 > /tmp/bndstrg2.pid");  
	fp = fopen("/tmp/bndstrg2.pid", "r");
	if (fp != NULL) {
		fgets(bndstrg_pid_tmp, sizeof(bndstrg_pid_tmp), fp);
		fclose(fp);
		unlink("/tmp/bndstrg2.pid");
		bndstrg_pid = atoi(bndstrg_pid_tmp);
	    if (bndstrg_pid != 0) {  /*if pid != 0, that's mean old igmpproxy process already exist*/
		return 1;
	    	}
	}
#endif
	return 0;
}
void KillBndStrgProcess()
{
	system("killall -15 bndstrg2");
}
void BndStrgSetParameter()
{
	char attrtmp[160] = {0};
	char cmd[128] = {0};
	int value = 0;
	char cmd_value[64] = {0};

	if (!BndstrgProceessExist()){
#if defined(TCSUPPORT_CT_JOYME4)
//assert wapp & bs20 are booted already
		tcdbg_printf("plz set node mesh_dat BS20Enable = 1 and commit mesh_dat\n");
		return;
#else
		memset(cmd, 0, sizeof(cmd));
		wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), "BndStrgEnable=1", cmd, 0);
		system(cmd);
		memset(cmd, 0, sizeof(cmd));
		wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), "BndStrgEnable=1", cmd, 0);
		system(cmd);
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/userfs/bin/bndstrg2 -i ra0 -i rai0");
		system(cmd);
#endif
	}
	else 
		tcdbg_printf("bndstrg2  process is open\n");
#if defined(TCSUPPORT_CT_JOYME4)
//enable bndstrg
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/userfs/bin/mapd_cli /tmp/mapd_ctrl setsteer 1");
		system(cmd);
#endif
	memset(attrtmp, 0, sizeof(attrtmp));
#if defined(TCSUPPORT_CT_JOYME4)
	if (cfg_get_object_attr(MESH_STEER_CFG_NODE, "SteeringDetectInterval", attrtmp, sizeof(attrtmp)) > 0){
#else
	if (cfg_get_object_attr(WLAN_BNDSTRG_NODE, "RSSICheckCount", attrtmp, sizeof(attrtmp)) > 0){
#endif
		value = atoi(attrtmp);
		if (value > 0 && value <100){
			memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_JOYME4)
			snprintf(cmd, sizeof(cmd), "/userfs/bin/mapd_cli /tmp/mapd_ctrl set SteeringDetectInterval %d",value);
#else
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"RSSICheckCount; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
#endif
			system(cmd);
		}
	}
	memset(attrtmp, 0, sizeof(attrtmp));
#if defined(TCSUPPORT_CT_JOYME4)
	if (cfg_get_object_attr(MESH_STEER_CFG_NODE, "RSSISteeringEdge_UG", attrtmp, sizeof(attrtmp)) > 0){
#else
	if (cfg_get_object_attr(WLAN_BNDSTRG_NODE, "RSSIHighUpSteer", attrtmp, sizeof(attrtmp)) > 0){
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		value = atoi(attrtmp) - 88;
#else
		value = atoi(attrtmp);
#endif
		if (value < 0 && value >-100){
			memset(cmd, 0, sizeof(cmd));
#if defined(TCSUPPORT_CT_JOYME4)
			snprintf(cmd, sizeof(cmd), "/userfs/bin/mapd_cli /tmp/mapd_ctrl set RSSIThreshold %d",value);
#else
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"RSSIHighUpSteer; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
#endif
			system(cmd);
		}
	}
	memset(attrtmp, 0, sizeof(attrtmp));
#if defined(TCSUPPORT_CT_JOYME4)
	if (cfg_get_object_attr(MESH_STEER_CFG_NODE, "RSSISteeringEdge_DG", attrtmp, sizeof(attrtmp)) > 0){
#else
	if (cfg_get_object_attr(WLAN_BNDSTRG_NODE, "RSSILowDownSteer", attrtmp, sizeof(attrtmp)) > 0){
#endif
#if defined(TCSUPPORT_CT_JOYME4)
		value = atoi(attrtmp) - 94;
#else
		value = atoi(attrtmp);
#endif
		if (value < 0 && value >-100){
#if defined(TCSUPPORT_CT_JOYME4)
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/userfs/bin/mapd_cli /tmp/mapd_ctrl set RSSIThreshold5G %d",value);
			system(cmd);
#else
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"RSSILowDownSteer; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
			tcdbg_printf("cmd=%s\n",cmd);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"RssiLow; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("ra", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
#endif
		}
	}
	memset(attrtmp, 0, sizeof(attrtmp));
	if (cfg_get_object_attr(WLAN_BNDSTRG_NODE, "LoadThres", attrtmp, sizeof(attrtmp)) > 0){
		value = atoi(attrtmp);
		if (value > 0 && value <100){
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"LoadThres; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
		}
	}
	memset(attrtmp, 0, sizeof(attrtmp));
	if (cfg_get_object_attr(WLAN_BNDSTRG_NODE, "IdleRxByteCount", attrtmp, sizeof(attrtmp)) > 0){
		value = atoi(attrtmp);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"IdleRxByteCount; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
	}
	memset(attrtmp, 0, sizeof(attrtmp));
	if (cfg_get_object_attr(WLAN_BNDSTRG_NODE, "IdleTxByteCount", attrtmp, sizeof(attrtmp)) > 0){
		value = atoi(attrtmp);
			memset(cmd, 0, sizeof(cmd));
			memset(cmd_value, 0, sizeof(cmd_value));
			snprintf(cmd_value, sizeof(cmd_value), "BndStrgParam=\"IdleTxByteCount; %d\"", value);
			wifimgr_lib_set_WIFI_CMD("rai", 0, sizeof(cmd), cmd_value, cmd, 0);
			system(cmd);
	}

}
#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
int np_check_radio_signal()
{
	char SSID_24G[64] = {0}, SSID_5G[64] = {0}, channel[8] = {0};
	char RSSI_24G[20] = {0}, RSSI_5G[20] = {0}, tmpbuf[20] = {0}, SwitchTime[10] = {0};
	char SignalDiff[10] = {0}, Signal_24G[16] = {0}, Signal_5G[16] = {0};
	int needSwitch = 0, curHigher = 0;
	static int checkTimes = 0, lastHigher = 0, scanRadio = 0, runtime = 0;
	
	cfg_obj_get_object_attr(APCLI_ENTRY0_NODE, "SSID", 0, SSID_24G, sizeof(SSID_24G));
	cfg_obj_get_object_attr(APCLI_ENTRY1_NODE, "SSID", 0, SSID_5G, sizeof(SSID_5G));

	if( 0 == runtime )
	{
		wifimgr_lib_set_SiteSurvery(0, SSID_24G);
		wifimgr_lib_set_SiteSurvery(1, SSID_5G);
		runtime++;
		return 0;
	}
	runtime = 0;
	np_wlan_apcli_get_sitesurvey(0, SSID_24G, channel, RSSI_24G, Signal_24G);
	np_wlan_apcli_get_sitesurvey(1, SSID_5G, channel, RSSI_5G, Signal_5G);

	cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, tmpbuf, sizeof(tmpbuf));
	if( cfg_obj_get_object_attr(APCLI_COMMON_NODE, "MaxRadioSwitchTime", 0, SwitchTime, sizeof(SwitchTime)) <= 0 )
		snprintf(SwitchTime, sizeof(SwitchTime), "60");
	
	if ( cfg_obj_get_object_attr(APCLI_COMMON_NODE, "SignalDiff", 0, SignalDiff, sizeof(SignalDiff)) <= 0)
		snprintf(SwitchTime, sizeof(SwitchTime), "10");
	
	if( 0 == atoi(tmpbuf) )
	{
		if( atoi(Signal_5G) - atoi(Signal_24G) >= atoi(SignalDiff) )
		{
			curHigher = 1;
			checkTimes++;
		}
	}
	else
	{
		if( atoi(Signal_24G) - atoi(Signal_5G) >= atoi(SignalDiff) )
		{
			curHigher = 1;
			checkTimes++;
		}
	}

	if( lastHigher != curHigher )
		checkTimes = 0;

	if( checkTimes > atoi(SwitchTime))
	{
		if( atoi(tmpbuf) )
		{
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, "0");
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "0");
			svc_wlan_apcli_start();
			needSwitch  = 1;
		}
		else
		{
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, "1");
			cfg_obj_set_object_attr(APCLI_COMMON_NODE, "noNeedScan", 0, "0");
			svc_wlan_apcli_start();
			needSwitch  = 2;
		}

		checkTimes = 0;
	}
	
	lastHigher= curHigher;
	
	return needSwitch;
}
#endif
#if defined(TCSUPPORT_ANDLINK)  || defined(TCSUPPORT_CT_UBUS)
double getTimeRate(struct timeval curTime,struct timeval lastTime){
	double rate = 0;
	double delay = 0;		/*usec*/

	if(curTime.tv_usec > lastTime.tv_usec){
		delay = (curTime.tv_sec-lastTime.tv_sec) + (double)(curTime.tv_usec-lastTime.tv_usec)/1000000;
	}else{
		delay = (curTime.tv_sec - 1 -lastTime.tv_sec) + (double)(curTime.tv_usec + 1000000 -lastTime.tv_usec)/1000000;
	}

	rate = 	1/delay;

	return rate;
}

int getUplinkRate(struct timeval lastTime, struct timeval curTime,unsigned long *oldRxBytes, unsigned long *oldTxBytes)
{
	char tmpbuf[64] = {0}, RxBytes[64] = {0}, TxBytes[64] = {0};
	double rxRate_rt = 0, txRate_rt = 0;
	double ajustRate = 1;
	double timeRate = 0;
	unsigned long iNewEthBytesUP = 0, iNewEthBytesDW = 0, totalRxBytes = 0, totalTxBytes = 0;
	
	timeRate = getTimeRate(curTime,lastTime);
	cfg_obj_get_object_attr(ALINKMGR_ENTRY_NODE, "apUplinkType", 0, tmpbuf, sizeof(tmpbuf));
	if( !strcmp(tmpbuf, "Wireless") )
	{
		cfg_obj_get_object_attr(APCLI_COMMON_NODE, "currentRadio", 0, tmpbuf, sizeof(tmpbuf));
		if( 0 == atoi(tmpbuf) )
		{	
			cfg_get_object_attr(INFO_APCLI0_NODE, "txbytes", TxBytes, sizeof(TxBytes));
			cfg_get_object_attr(INFO_APCLI0_NODE, "rxbytes", RxBytes, sizeof(RxBytes));
		}
		else
		{
			cfg_get_object_attr(INFO_APCLII0_NODE, "txbytes", TxBytes, sizeof(TxBytes));
			cfg_get_object_attr(INFO_APCLII0_NODE, "rxbytes", RxBytes, sizeof(RxBytes));
		}
	}
	else
	{
		macMT7530GetPortRxOctetsCnt(100, &iNewEthBytesUP);
		macMT7530GetPortTxOctetsCnt(100, &iNewEthBytesDW);
	}
	
	totalRxBytes = atoi(RxBytes) + iNewEthBytesUP;
	totalTxBytes = atoi(TxBytes) + iNewEthBytesDW;

	/* caculate rate */
	if(totalRxBytes < *oldRxBytes){
		rxRate_rt = (timeRate*ajustRate*(0xffffffff - *oldRxBytes + totalRxBytes)*8.0/1024)/1024;				/*mbps*/	
	}
	else{
		rxRate_rt = (timeRate*ajustRate*(totalRxBytes - *oldRxBytes)*8.0/1024)/1024;								/*mbps*/	
	}

	if(totalTxBytes < *oldTxBytes){
		txRate_rt = (timeRate*ajustRate*(0xffffffff - *oldTxBytes + totalTxBytes)*8.0/1024)/1024;				/*mbps*/	
	}
	else{
		txRate_rt = (timeRate*ajustRate*(totalTxBytes - *oldTxBytes)*8.0/1024)/1024;								/*mbps*/	
	}
	if( rxRate_rt == 0 )
		snprintf(tmpbuf, sizeof(tmpbuf), "0");
	else
		snprintf(tmpbuf, sizeof(tmpbuf), "%0.4f", rxRate_rt);
	cfg_obj_set_object_attr(APWANINFO_COMMON_NODE, "RxRate", 0, tmpbuf);
	if( txRate_rt == 0 )
		snprintf(tmpbuf, sizeof(tmpbuf), "0");
	else
		snprintf(tmpbuf, sizeof(tmpbuf), "%0.4f", txRate_rt);
	cfg_obj_set_object_attr(APWANINFO_COMMON_NODE, "TxRate", 0, tmpbuf);
	*oldRxBytes = totalRxBytes;
	*oldTxBytes = totalTxBytes;

	return 0;
}

#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
int get_Info_Ra0_Rate(struct timeval lastTime, struct timeval curTime,unsigned long *oldRxBytes, unsigned long *oldTxBytes,double *rxRatert ,double *txRatert)
{
	char RxBytes[64] = {0}, TxBytes[64] = {0};
	double rxRate_rt = 0, txRate_rt = 0;
	double ajustRate = 1;
	double timeRate = 0;
	unsigned long long totalRxBytes = 0, totalTxBytes = 0;
	
	timeRate = getTimeRate(curTime,lastTime);
	cfg_get_object_attr("root.info.ra.1", "rxbytes", RxBytes, sizeof(RxBytes));
	sscanf(RxBytes,"%llu",&totalRxBytes);
	cfg_get_object_attr("root.info.ra.1", "txbytes", TxBytes, sizeof(TxBytes));
	sscanf(TxBytes,"%llu",&totalTxBytes);

	/* caculate rate */
	if(totalRxBytes < *oldRxBytes){
		rxRate_rt = timeRate*ajustRate*(0xffffffff - *oldRxBytes + totalRxBytes)*8.0/1024;	/*kbps*/	
	}
	else{
		rxRate_rt = timeRate*ajustRate*(totalRxBytes - *oldRxBytes)*8.0/1024;			/*kbps*/	
	}
	
	if(totalTxBytes < *oldTxBytes){
		txRate_rt = timeRate*ajustRate*(0xffffffff - *oldTxBytes + totalTxBytes)*8.0/1024;	/*kbps*/	
	}
	else{
		txRate_rt = timeRate*ajustRate*(totalTxBytes - *oldTxBytes)*8.0/1024;			/*kbps*/	
	}
	
	*rxRatert = rxRate_rt;
	*txRatert = txRate_rt;
	*oldRxBytes = totalRxBytes;
	*oldTxBytes = totalTxBytes;

	return 0;
}
#endif

#ifdef TCSUPPORT_CT_WLAN_JOYME3
int ChenckMacTableConnect(char *mac,int* prssi){
	int i = 0;
	int j = 0;
	char buf[20]={0};

	for (i=0; i<MAX_DUALBAND_BSSID_NUM;i++){		
		for(j=0;  j<gMacTable[i].Num && j<MAX_LEN_OF_MAC_TABLE;j++){
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
					gMacTable[i].macEntry[j].Addr[0],
					gMacTable[i].macEntry[j].Addr[1],
					gMacTable[i].macEntry[j].Addr[2],
					gMacTable[i].macEntry[j].Addr[3],
					gMacTable[i].macEntry[j].Addr[4],
					gMacTable[i].macEntry[j].Addr[5]);
			if (!strcasecmp(mac,buf)){		
				*prssi = gMacTable[i].macEntry[j].AvgRssi;
				tcdbg_printf("ChenckMacTableConnect====rssi = %d\n",*prssi);
				return 1;				
				}
			}
		}
	return 0;
}

void SetMimodeInterface(char *channel,char * interfacebuf){
	int channelNum = atoi(channel);
	
	if (channelNum <= 14){/*2G*/
		strcpy(interfacebuf,"ra0");
	}
	else {
		strcpy(interfacebuf,"rai0");	
	}
	return ;
}
int GetStationNum(){
	char monitor[32] = {0};
	int stationNum = 0;

	if (cfg_get_object_attr(WLAN_MONITOR_NODE, "MonitorNum", monitor, sizeof(monitor)) < 0)
		stationNum = 0;
	
	stationNum = atoi(monitor);
	
	return stationNum;
}
int GetMixmodeNodeValue(char *mac,char *channel,int index){
	char tmp[32] = {0};
	
	memset(mac,0,MONITOR_STA_ADDR_LEN*sizeof(char));	
	memset(tmp,0,sizeof(tmp));
	snprintf(tmp,sizeof(tmp),"MAC%d",index);
	if (cfg_get_object_attr(WLAN_MONITOR_NODE, tmp, mac, MONITOR_STA_ADDR_LEN) < 0)
		return 0;
	
	memset(tmp,0,sizeof(tmp));
	snprintf(tmp,sizeof(tmp),"Channel%d",index);
	
	if (cfg_get_object_attr(WLAN_MONITOR_NODE, tmp, channel, 4) < 0){
		return 0;
	}

	return 1;
}	
void SendRssiValueMsg(int rssi,char *mac,char* interfacebuf,int index,char* errdest){
	char tmp[32] = {0};
	char rdata[128] = {0};
	int rfband = 0;
	int result = 0;
	int ret = 0;
#if !defined(TCSUPPORT_CT_JOYME4)
	if (rssi ==-127 || rssi==0){
		result=1;
		}
#endif
	memset(tmp,0,sizeof(tmp));
	snprintf(tmp,sizeof(tmp),"RSSI%d",index);
	cfg_set_object_attr(WLAN_MONITOR_NODE, tmp, itoa(rssi));

#if defined(TCSUPPORT_CT_JOYME4)
	if (rssi ==-127 || rssi == 0){
		rssi = -100;
		result=1;
	}
#endif
	if (!strcmp(interfacebuf,"rai0"))
		rfband = 1;
	else
		rfband = 0;
	
	snprintf(rdata,sizeof(rdata),"%d;%d;%s;%d;%s",rfband,result,mac,rssi,errdest);
	tcdbg_printf("SendRssiValueMsg rdata=%s\n",rdata);
	ret = ecnt_event_send(ECNT_EVENT_WIFI,ECNT_EVENT_WIFI_STARSSIQUERYRESULT,rdata,strlen(rdata)+1);
	return;
}
int GetMixMethod(char * interfacebuf)
{
	char node_name[128]	= {0};
	char rt_device[16]	= {0};
	int mode = MIXMODE;

	if (!strcmp(interfacebuf,"rai0")){
		snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);
		}
	else{
		snprintf(node_name, sizeof(node_name), WLAN_COMMON_NODE);
		}
	
	cfg_get_object_attr(node_name, "rt_device",  rt_device, sizeof(rt_device));

	if	(strstr(rt_device ,"7592") || strstr(rt_device,"7612")){
		mode = MIXMODE;
		}
	else{
		mode = AIRMONITOR;
		}
	
	tcdbg_printf("wlan_cfg GetDevice return MIXMODE =%d rt_deviceb=%s\n ",mode,rt_device);
	
	return mode;
}
void AirMonitorOnOff(int onoff,char * interfacebuf){
	char cmd[128] = {0};
	char cmd_value[64] = {0};
	
	memset(cmd_value,0,sizeof(cmd_value));
	snprintf(cmd_value, sizeof(cmd_value), "mnt_en=%d", onoff);
	wifimgr_lib_set_WIFI_CMD(interfacebuf, -1, sizeof(cmd), cmd_value, cmd, 0);
	system(cmd);
	
	return;	
}	
void AirMonitorSetRssi(char *mac,char * interfacebuf,int index,char *channel){
	char cmd[128] = {0};	
	char cmd_value[64] = {0};

	memset(cmd,0,sizeof(cmd));
	wifimgr_lib_set_WIFI_CMD(interfacebuf, -1, sizeof(cmd), "mnt_rule=1:1:0", cmd, 0);
	system(cmd);
	
	memset(cmd,0,sizeof(cmd));
	memset(cmd_value,0,sizeof(cmd_value));
	snprintf(cmd_value, sizeof(cmd_value), "mnt_sta%d=%s", index, mac);
	wifimgr_lib_set_WIFI_CMD(interfacebuf, -1, sizeof(cmd), cmd_value, cmd, 0);
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	memset(cmd_value,0,sizeof(cmd_value));
	snprintf(cmd_value, sizeof(cmd_value), "ch_sw=%s", channel);
	wifimgr_lib_set_WIFI_CMD(interfacebuf, -1, sizeof(cmd), cmd_value, cmd, 0);
	system(cmd);
	
	AirMonitorOnOff(MONITORON,interfacebuf);
	
	return;
	
}
int AirMonitorGetRssi(char *mac,char *interfacebuf)
{
	int i = 0;
	int rssi = 0;
	MNT_STA_ENTRY *pEntry_dump = NULL;
	unsigned char * pEntry = NULL;
	char Entry_mac[MONITOR_STA_ADDR_LEN]= {0};
	char tmp[32] = {0};
	char node_name[128] = {0};
	char rt_device[16] = {0};
	
	snprintf(node_name, sizeof(node_name), WLAN11AC_COMMON_NODE);	
	cfg_get_object_attr(node_name, "rt_device",  rt_device, sizeof(rt_device));

	pEntry = (unsigned char *)(malloc(MAX_NUM_OF_MONITOR_STA * sizeof(MNT_STA_ENTRY)) );
	if (!pEntry)
		goto end;
	memset(pEntry,0,MAX_NUM_OF_MONITOR_STA * sizeof(MNT_STA_ENTRY));

	if (wifimgr_lib_OidQueryInformation(OID_GET_AIR_MONITOR_RESULT, interfacebuf, pEntry, MAX_NUM_OF_MONITOR_STA * sizeof(MNT_STA_ENTRY)) < 0)
	{
		tcdbg_printf("AirMonitorGetRssi() ERROR:OID_GET_AIR_MONITOR_RESULT\n");
		free(pEntry);
			
	}
	else
	{
		pEntry_dump = (MNT_STA_ENTRY *)pEntry;
		for (i = 0;i < MAX_NUM_OF_MONITOR_STA ; i++)
		{
			if (pEntry_dump->bValid)
			{
				snprintf(Entry_mac,sizeof(Entry_mac),"%02x:%02x:%02x:%02x:%02x:%02x", pEntry_dump->addr[0],pEntry_dump->addr[1],pEntry_dump->addr[2],pEntry_dump->addr[3],pEntry_dump->addr[4],pEntry_dump->addr[5]);
				//snprintf(Entry_mac,sizeof(Entry_mac),"%02x:%02x:%02x:%02x:%02x:%02x",PRINT_MAC(pEntry_dump->addr));
				tcdbg_printf("Entry_mac=%s ;mac=%s\n",Entry_mac,mac);
				if(!strcasecmp(Entry_mac,mac)){
					/*because 7915D(2X2+2X2),but 7915(3x3,4x4) need modify in the future*/
					if(strstr(rt_device ,"7663") || strstr(rt_device ,"7915"))
						rssi = (pEntry_dump->RssiSample.AvgRssi[0] + pEntry_dump->RssiSample.AvgRssi[1]) >> 1;
					else
						rssi = (pEntry_dump->RssiSample.AvgRssi[0] + pEntry_dump->RssiSample.AvgRssi[1] + pEntry_dump->RssiSample.AvgRssi[2] + pEntry_dump->RssiSample.AvgRssi[3]) >> 2;
					tcdbg_printf("AirMonitorGetRssi same in Entry_mac=%s rssi=%d\n",Entry_mac,rssi);
					break;
				}			
			}
			pEntry_dump = (unsigned char *)(pEntry_dump) + sizeof(MNT_STA_ENTRY);
		}
		free(pEntry);
	}	
	end:
	AirMonitorOnOff(MONITOROFF,interfacebuf);
	return rssi;

}
void MixmodeSetRssi(char *mac,char * interfacebuf,int index,char *channel){
	struct _mix_peer_parameter mix_peer_info;
	char tmp[3] = {0};
	int i = 0;
	int channelNum = atoi(channel);
	
	if (channelNum <= 14)/*2G*/{
		mix_peer_info.bw = 0; 
	}
	else/*5G*/{
		mix_peer_info.bw = 1; 
	}
	tcdbg_printf("MixmodeSetRssi() interfacebuf=%s channelNum=%d mac=%s channel=%s\n",interfacebuf,channelNum,mac,channel);
	tmp[2] = 0;
	for(i = 0; i < 6; i++){
		tmp[0] = mac[3*i];
		tmp[1] = mac[3*i+1];
		mix_peer_info.mac_addr[i] = (unsigned char)strtoul(tmp, NULL, 16);
	}
	mix_peer_info.channel = channelNum;
	mix_peer_info.duration = 200;
	mix_peer_info.ch_offset = EXTCHA_BELOW;
	if (wifimgr_lib_OidQueryInformation(OID_GET_SET_TOGGLE | OID_SET_MIXMODE, interfacebuf, &mix_peer_info, sizeof(struct _mix_peer_parameter)) < 0){
		printf("MixmodeSetRssi() ERROR:OID_SET_MIXMODE\n");
		return FAIL;			
}
	else{
		printf("mixmode set entry with mac:");
		for(i = 0; i < 5; i++)    
    		printf("%02x:", mix_peer_info.mac_addr[i]);
		printf("%02x, ", mix_peer_info.mac_addr[5]);
		printf("channel:%d, bw = %d, ch_offset = %d, duration = %d\n", mix_peer_info.channel, mix_peer_info.bw, mix_peer_info.ch_offset, mix_peer_info.duration);
		return SUCCESS;
	}	
}
int MixmodeGetRssi(char *mac,char *interfacebuf)

{
	signed char rssi = 0;

	if (wifimgr_lib_OidQueryInformation(OID_MIXMODE_GET_RSSI, interfacebuf, &rssi, 1) < 0)
	{
		tcdbg_printf("MixmodeGetRssi ERROR:OID_MIXMODE_GET_RSSI\n");
	}
	else
	{
		tcdbg_printf("MixmodeGetRssi Station RSSI:%d \n", rssi);
	}
	return rssi;

}

#endif
