/*
 *	Wireless Tools
 *
 *		Jean II - HPLB 97->99 - HPL 99->07
 *
 * Main code for "iwconfig". This is the generic tool for most
 * manipulations...
 * You need to link this code against "iwlib.c" and "-lm".
 *
 * This file is released under the GPL license.
 *     Copyright (c) 1997-2007 Jean Tourrilhes <jt@hpl.hp.com>
 */

#include "iwlib.h"		/* Header */
#if 0

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include "wireless.h"
//#include <linux/if.h>

#define IFNAMSIZ	16
#define SIOCIWFIRSTPRIV      0x8BE0

struct  iw_point
{
	void	*pointer;       /* Pointer to the data  (in user space) */
	unsigned short	length;         /* number of fields or size in bytes */
	unsigned short	flags;          /* Optional params */
};

union   iwreq_data
{
	/* Config - generic */
	char            name[IFNAMSIZ];
	struct iw_point	data;
};

struct  iwreq
{
	char    ifr_name[IFNAMSIZ];    /* if name, e.g. "eth0" */

        /* Data part (defined just above) */
        union   iwreq_data      u;
};
#endif



//#define MAC_ADDR_LENGTH                 		6
typedef unsigned char			UINT8;
typedef unsigned short			UINT16;
typedef unsigned int			UINT32;
typedef unsigned long long		UINT64;
typedef int						INT32;
typedef short					INT16;
typedef long long 				INT64;
typedef unsigned char			UCHAR;
typedef unsigned short			USHORT;
typedef unsigned int			UINT;
typedef unsigned long			ULONG;
typedef signed char				CHAR;
typedef signed short			SHORT;
typedef signed int				INT;
typedef signed long				LONG;
typedef signed long long		LONGLONG;	
typedef unsigned long long		ULONGLONG;
typedef unsigned char			BOOLEAN;


#define MAC_ADDR_LEN					6
#define MAX_NUMBER_OF_MAC				75
#define MAX_LEN_OF_BSS_TABLE             		64
#define NDIS_802_11_LENGTH_SSID         32
#define MAX_LENGTH_OF_SUPPORT_RATES		12	/* 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54 */
#define	RT_OID_DEVICE_NAME						0x0607
#define	RT_OID_VERSION_INFO						0x0608
#define	OID_802_11_BSSID_LIST						0x0609
#define	OID_802_3_CURRENT_ADDRESS					0x060A

#define RT_PRIV_IOCTL							(SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_GET_MAC_TABLE					(SIOCIWFIRSTPRIV + 0x0F)
#define OID_802_11_GET_CURRENT_CHANNEL_FALSE_CCA_AVG			0x0690	
#define OID_802_11_GET_CURRENT_CHANNEL_CST_TIME_AVG			0x0691
#define	OID_802_11_GET_CURRENT_CHANNEL_BUSY_TIME_AVG			0x0692
#define OID_802_11_GET_CURRENT_CHANNEL_STATS 				0x0693
#define OID_802_11_GET_CURRENT_CHANNEL_AP_ACTIVITY_AVG  		0x0694
#define OID_802_11_STA_STATISTICS					0x0695
#define OID_802_11_ASSOLIST						0x0534
#define OID_802_11_GET_CURRENT_CHANNEL_AP_TABLE				0x0696
#define OID_802_11_GET_ACCESS_CATEGORY_TRAFFIC_STATUS			0x0697
#define OID_802_11_GET_SCAN_RESULTS					0x0698
#define OID_802_11_GET_RADIO_STATS_COUNT            0X0699
#define OID_802_11_MBSS_STATISTICS                                      0x069a


typedef struct GNU_PACKED_NDIS_802_11_SSID {
	UINT SsidLength;	/* length of SSID field below, in bytes; */
	/* this can be zero. */
	UCHAR Ssid[NDIS_802_11_LENGTH_SSID];	/* SSID information field */
} NDIS_802_11_SSID, *PNDIS_802_11_SSID;

typedef struct _CURRENT_CHANNEL_STATS{
	unsigned int	SamplePeriod;                 
	unsigned int	FalseCCACount;                               
 	unsigned int  	ChannelApActivity;       
 	unsigned int 	EdCcaBusyTime;                              
 	unsigned int  	ChannelBusyTime;                           
} CURRENT_CHANNEL_STATISTICS , *PCURRENT_CHANNEL_STATISTICS; 

typedef struct __RT_MBSS_STAT_ENTRY{
	// DATA counter
	unsigned int RxCount;
	unsigned int TxCount;
	unsigned int ReceivedByteCount;
	unsigned int TransmittedByteCount;
	unsigned int RxErrorCount;
	unsigned int RxDropCount;
	unsigned int TxErrorCount;
	unsigned int TxDropCount;
	unsigned int UnicastPktsRx;
	unsigned int UnicastPktsTx;
	unsigned int MulticastPktsRx;
	unsigned int MulticastPktsTx;
	unsigned int BroadcastPktsRx;
	unsigned int BroadcastPktsTx;
	unsigned int TxRetriedPktCount;
	unsigned int ChannelUseTime;
	// MGMT counter
	unsigned int MGMTRxCount;
	unsigned int MGMTTxCount;
	unsigned int MGMTReceivedByteCount;
	unsigned int MGMTTransmittedByteCount;
	unsigned int MGMTRxErrorCount;
	unsigned int MGMTRxDropCount;
	unsigned int MGMTTxErrorCount;
	unsigned int MGMTTxDropCount;
} RT_MBSS_STAT_ENTRY, *PRT_MBSS_STAT_ENTRY;

typedef struct  __RT_STA_STAT_ENTRY
{
	unsigned char  ApIdx;
    	unsigned char  Addr[MAC_ADDR_LEN];
	unsigned int   RxCount;
	unsigned int   TxCount;
	unsigned int   ReceivedByteCount;
	unsigned int   TransmittedByteCount;
	unsigned int   RxErrorCount;
	unsigned int   RxDropCount;
	unsigned int   TxErrorCount;
	unsigned int   TxDropCount;
	unsigned int   TxRetriedPktCount;
	unsigned int   ChannelUseTime;
} RT_STA_STAT_ENTRY, *PRT_STA_STAT_ENTRY;

typedef struct  __RT_STA_STATISTICS_TABLE
{
	unsigned int Num;
	RT_STA_STAT_ENTRY	STAEntry[MAX_NUMBER_OF_MAC];
} RT_STA_STATISTICS_TABLE, *PRT_STA_STATISTICS_TABLE;


typedef struct  __RT_MBSS_STATISTICS_TABLE{
//	unsigned int 			TotalBeaconSentCount;
//    unsigned int            TotalTxCount;
//    unsigned int            TotalRxCount;
	unsigned int			 Num;
	RT_MBSS_STAT_ENTRY	MbssEntry[8];
} RT_MBSS_STATISTICS_TABLE, *PRT_MBSS_STATISTICS_TABLE;

typedef union _MACHTTRANSMIT_SETTING {
	struct {
		unsigned short  MCS:6;	/* MCS */
		unsigned short  ldpc:1;
		unsigned short  BW:2;	/*channel bandwidth 20MHz or 40 MHz */
		unsigned short  ShortGI:1;
		unsigned short  STBC:1;	/*SPACE */
		unsigned short  eTxBF:1;
		unsigned short  iTxBF:1;
		unsigned short  MODE:3;	/* Use definition MODE_xxx. */
	} field;
	unsigned short word;
} MACHTTRANSMIT_SETTING, *PMACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
	unsigned char  ApIdx;
	unsigned char  Addr[MAC_ADDR_LEN];
	unsigned char  Aid;
	unsigned char  Psm;		/* 0:PWR_ACTIVE, 1:PWR_SAVE */
	unsigned char  MimoPs;		/* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	char AvgRssi0;
	char AvgRssi1;
	char AvgRssi2;
	unsigned int  ConnectedTime;
	MACHTTRANSMIT_SETTING TxRate;
	unsigned int AvgSnr;
	unsigned int  LastRxRate;
//	int StreamSnr[3];
//	int SoundingRespSnr[3];				/* RTMP_RBUS_SUPPORT */
} RT_802_11_MAC_ENTRY, *PRT_802_11_MAC_ENTRY;

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long Num;
	RT_802_11_MAC_ENTRY Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;

typedef struct _BSS_ENTRY_TABLE
{
	unsigned char   Bssid[MAC_ADDR_LEN];
	unsigned short  SsidLen;
	unsigned char 	Ssid[33];
	unsigned char 	Channel;
	unsigned char	ChannelWidth;
	unsigned char	ExtChannel;
	char		RSSI;
	unsigned char	SNR;
	unsigned char	PhyMode;
	unsigned char	NumSpatialStream;
}BSS_ENTRY_TABLE, *PBSS_ENTRY_TABLE;

typedef struct _BEACON_TABLE
{
	unsigned char Num;
	BSS_ENTRY_TABLE BssTable[MAX_LEN_OF_BSS_TABLE];
} BEACON_TABLE, *PBEACON_TABLE;

typedef struct _SCAN_RESULTS
{
	unsigned int	ch_busy_time;
	unsigned int	cca_err_cnt;
	unsigned int	num_ap;
	unsigned int	ch_non_wifi_intf;
//	signed int      Avg_NF;
	BSS_ENTRY_TABLE BssTable[MAX_LEN_OF_BSS_TABLE];
}SCAN_RESULTS, *PSCAN_RESULTS;

typedef struct _RADIO_STATS_COUNTER
{
    unsigned int TotalBeaconSentCount;
    unsigned int TotalTxCount;
    unsigned int TotalRxCount;
    unsigned int TxDataCount;
    unsigned int RxDataCount;
    unsigned int TxRetriedPktCount;
    unsigned int TxRetriedCount;
  //  unsigned int TxProbRspCount;
//    unsigned int RxBeaconCount;
  //  unsigned int RxProbReqCount;
}RADIO_STATS_COUNTER, *PRADIO_STATS_COUNTER;
typedef struct _AVG_VALUE
{
	unsigned int	value;
}AVG_VALUE, *PAVG_VALUE;

typedef struct _STREAMING_STATUS
{
	unsigned int status;
}STREAMING_STATUS, *PSTREAMING_STATUS;
typedef unsigned char NDIS_802_11_MAC_ADDRESS[6];
typedef enum _NDIS_802_11_NETWORK_TYPE {
	Ndis802_11FH,
	Ndis802_11DS,
	Ndis802_11OFDM5,
	Ndis802_11OFDM24,
	Ndis802_11Automode,
	Ndis802_11OFDM5_N,
	Ndis802_11OFDM24_N,
	Ndis802_11OFDM5_AC,
	Ndis802_11NetworkTypeMax	/* not a real type, defined as an upper bound */
} NDIS_802_11_NETWORK_TYPE, *PNDIS_802_11_NETWORK_TYPE;
typedef enum _NDIS_802_11_AUTHENTICATION_MODE {
	Ndis802_11AuthModeOpen,
	Ndis802_11AuthModeShared,
	Ndis802_11AuthModeAutoSwitch,
	Ndis802_11AuthModeWPA,
	Ndis802_11AuthModeWPAPSK,
	Ndis802_11AuthModeWPANone,
	Ndis802_11AuthModeWPA2,
	Ndis802_11AuthModeWPA2PSK,
	Ndis802_11AuthModeWPA1WPA2,
	Ndis802_11AuthModeWPA1PSKWPA2PSK,
	Ndis802_11AuthModeMax	/* Not a real mode, defined as upper bound */
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;
/* Also aliased typedef to new name */
typedef enum _NDIS_802_11_WEP_STATUS {
    Ndis802_11WEPEnabled,
    Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
    Ndis802_11WEPDisabled,
    Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
    Ndis802_11WEPKeyAbsent,
    Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
    Ndis802_11WEPNotSupported,
    Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
    Ndis802_11TKIPEnable,
    Ndis802_11Encryption2Enabled = Ndis802_11TKIPEnable,
    Ndis802_11Encryption2KeyAbsent,
    Ndis802_11AESEnable,
    Ndis802_11Encryption3Enabled = Ndis802_11AESEnable,
    Ndis802_11CCMP256Enable,
    Ndis802_11GCMP128Enable,
    Ndis802_11GCMP256Enable,
    Ndis802_11Encryption3KeyAbsent,
    Ndis802_11TKIPAESMix,
    Ndis802_11Encryption4Enabled = Ndis802_11TKIPAESMix,    /* TKIP or AES mix */
    Ndis802_11Encryption4KeyAbsent,
    Ndis802_11GroupWEP40Enabled,
    Ndis802_11GroupWEP104Enabled,
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS, NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;

typedef struct _DCC_CH
{
	UINT8	ChannelNo;
	UINT32 	FalseCCA;
	UINT32	chanbusytime;
} DCC_CH, *PDCC_CH;

typedef struct _WLAN_BSSID_LIST
{   
   NDIS_802_11_MAC_ADDRESS        	Bssid;         // BSSID
   UCHAR 							WpsAP; 		/* 0x00: not support WPS, 0x01: support normal WPS, 0x02: support Ralink auto WPS, 0x04: support Samsung WAC */
   USHORT							WscDPIDFromWpsAP;   
   UCHAR							Privacy;    // WEP encryption requirement
   UINT                    			Signal;       // receive signal strength in dBm
   NDIS_802_11_NETWORK_TYPE    		wireless_mode;
   NDIS_802_11_AUTHENTICATION_MODE	AuthMode;	
   NDIS_802_11_WEP_STATUS			Cipher;				// Unicast Encryption Algorithm extract from VAR_IE
   UINT8 							Channel;
   UINT8							ExtChannel;
   UCHAR   							BssType;
   UCHAR							SupportedRates[MAX_LENGTH_OF_SUPPORT_RATES];
   BOOLEAN     						bQbssLoadValid;                     // 1: variable contains valid value
   USHORT      						QbssLoadStaNum;
   UCHAR       						QbssLoadChannelUtilization;
   UCHAR  							Snr[4];
   UINT8							NumSpatialStream;
   CHAR								rssi[4];
   UCHAR  							vendorOUI0[3];
   UCHAR  							vendorOUI1[3];
   UINT8						   	ChannelWidth;
   NDIS_802_11_SSID					Ssid;       // SSID
} WLAN_BSSID_LIST, *PWLAN_BSSID_LIST;

typedef struct  _BSSID_LIST_RESULTS
{
   UINT           					NumberOfItems;      // in list below, at least 1
   WLAN_BSSID_LIST 					BssidTable[MAX_LEN_OF_BSS_TABLE];
   DCC_CH							DccCh[26]; //anand to handle 5g and 2g both channels
}BSSID_LIST_RESULTS, *PBSSID_LIST_RESULTS;

#ifndef os_malloc
#define os_malloc(s) malloc((s))
#endif

main(int	argc,
     char **	argv)
{
	int skfd;		/* generic raw socket desc.	*/
	int subcmd;
	CURRENT_CHANNEL_STATISTICS ChannelStats;
	RT_MBSS_STATISTICS_TABLE mbss_stat;
	RT_STA_STATISTICS_TABLE Sta_stat;
	RT_802_11_MAC_TABLE MacTable;
	char *buf = NULL;
//	BEACON_TABLE beaconTable;
//	SCAN_RESULTS scanResult;

	
	int AvgValue, i;
	struct iwreq	wrq;
	u_char	buffer[8192];	/* Only that big in v25 and later */

	/* Create a channel to the NET kernel. */
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0)
	{
		perror("socket");
		return(-1);
	}

	if (argc != 3)
	{
		printf("Usage: %s wlan0 subcmd\n", argv[0]);
		return -1;
	}

	subcmd = atoi(argv[2]);
	strncpy(wrq.ifr_name, argv[1], IFNAMSIZ-1);

	wrq.u.data.flags = subcmd;
	wrq.u.data.pointer = buffer;
	wrq.u.data.length = sizeof(buffer);
	printf("Before: iface %s, subcmd %d, buffer %p, length %d\n",
			wrq.ifr_name, wrq.u.data.flags, wrq.u.data.pointer, wrq.u.data.length);

	if(subcmd == 9999)
	{
	 	if (ioctl(skfd,RTPRIV_IOCTL_GET_MAC_TABLE, &wrq) < 0)
        	{
                	perror("ioctl");
                	return -1;
        	}
	}
	else
	{
		if (ioctl(skfd, RT_PRIV_IOCTL, &wrq) < 0)
		{
			perror("ioctl");
			return -1;
		}	
		//printf("ERR NO = %d\n", errno);
	}
	close(skfd);
//	printf("\n iwprivget done ");
	switch(subcmd) 
	{
		case OID_802_11_GET_CURRENT_CHANNEL_STATS:
			ChannelStats = *((PCURRENT_CHANNEL_STATISTICS)wrq.u.data.pointer);
		  //	printf(" After: iface %s, subcmd %d, length %d\n",
                  //      wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
			printf(" TotalDuration: %u  FalseCCACount: %u ChannelAPActivity: %u  EDCccaBusyTime: %u ChannlBusyTime: %u \n", ChannelStats.SamplePeriod, ChannelStats.FalseCCACount, ChannelStats.ChannelApActivity, ChannelStats.EdCcaBusyTime, ChannelStats.ChannelBusyTime);
			//free(wrq.u.data.pointer);
			break;
		case OID_802_11_GET_CURRENT_CHANNEL_FALSE_CCA_AVG:
			{
				AVG_VALUE avgValue;
				avgValue = *((PAVG_VALUE)wrq.u.data.pointer);
		 	 //	printf(" After: iface %s, subcmd %d, length %d\n",
                  	//      wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
				printf("AVG FALSE CCA: %u \n", avgValue.value);
			}
			break;

		case OID_802_11_GET_CURRENT_CHANNEL_CST_TIME_AVG:
			AvgValue = *((int*)wrq.u.data.pointer);
                    //    printf(" After: iface %s, subcmd %d, length %d\n",
                    //    wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
                        printf("CCA BUSY TIME: %u \n", AvgValue);
                        break;
			
		case OID_802_11_GET_CURRENT_CHANNEL_BUSY_TIME_AVG:
			AvgValue = *((int*)wrq.u.data.pointer);
                     //   printf(" After: iface %s, subcmd %d, length %d\n",
                    //    wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
                        printf("Channel BUSY TIME: %u \n", AvgValue);
                        break;
		case OID_802_11_GET_CURRENT_CHANNEL_AP_ACTIVITY_AVG:
			AvgValue = *((int*)wrq.u.data.pointer);
                     //   printf(" After: iface %s, subcmd %d, length %d\n",
                     //   wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
                        printf("AVG AP Activity TIME: %u \n", AvgValue);
                        break;
		case OID_802_11_MBSS_STATISTICS:
			mbss_stat = *((PRT_MBSS_STATISTICS_TABLE)wrq.u.data.pointer);
		//	printf(" After: iface %s, subcmd %d, length %d\n",
                 //       wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
	//		printf(" TotalBeaconSentCount %u \n",mbss_stat.TotalBeaconSentCount); 
      //      printf(" TotalTxCount %u \n", mbss_stat.TotalTxCount);
       //     printf(" TotalRxCount %u \n", mbss_stat.TotalRxCount);
			for(i=0; i < mbss_stat.Num; i++)
			{
				printf("BSS %d RxCount: %u  \n",i+1, mbss_stat.MbssEntry[i].RxCount);
				printf("BSS %d TxCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TxCount);
				printf("BSS %d ReceivedByteCount: %u  \n",i+1, mbss_stat.MbssEntry[i].ReceivedByteCount);
				printf("BSS %d TransmittedByteCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TransmittedByteCount);
				printf("BSS %d RxErrorCount: %u  \n",i+1, mbss_stat.MbssEntry[i].RxErrorCount);
				printf("BSS %d RxDropCount: %u  \n",i+1, mbss_stat.MbssEntry[i].RxDropCount);
				printf("BSS %d TxErrorCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TxErrorCount);
				printf("BSS %d TxDropCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TxDropCount);
				printf("BSS %d TxRetriedPktCount: %u \n",i+1, mbss_stat.MbssEntry[i].TxRetriedPktCount);
				printf("BSS %d UnicastPktsRx: %u \n",i+1, mbss_stat.MbssEntry[i].UnicastPktsRx);
				printf("BSS %d UnicastPktsTx: %u \n",i+1, mbss_stat.MbssEntry[i].UnicastPktsTx);
				printf("BSS %d MulticastPktsRx: %u \n",i+1, mbss_stat.MbssEntry[i].MulticastPktsRx);
				printf("BSS %d MulticastPktsTx: %u \n",i+1, mbss_stat.MbssEntry[i].MulticastPktsTx);
				printf("BSS %d BroadcastPktsRx: %u \n",i+1, mbss_stat.MbssEntry[i].BroadcastPktsRx);
				printf("BSS %d BroadcastPktsTx: %u \n",i+1, mbss_stat.MbssEntry[i].BroadcastPktsTx);
				printf("BSS %d ManagementTXcount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTxCount);
				printf("BSS %d ManagementRxcount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTRxCount);
				printf("BSS %d MGMTReceivedByteCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTReceivedByteCount);
				printf("BSS %d MGMTTransmittedByteCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTransmittedByteCount);
				printf("BSS %d MGMTRxErrorCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTRxErrorCount);
				printf("BSS %d MGMTTxErrorCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTxErrorCount);
				printf("BSS %d MGMTRxDropCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTRxDropCount);
				printf("BSS %d MGMTTxDropCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTxDropCount);	
				printf("BSS %d ChannelUseTime: %u \n",i+1, mbss_stat.MbssEntry[i].ChannelUseTime);
			
			//	printf("BSS %d FifoTxCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoTxCnt);
			//	printf("BSS %d FifoFailCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoFailCnt);
			//	printf("BSS %d FifoSucCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoSucCnt);
			//	printf("BSS %d FifoRetryCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoRetryCnt);
			//	printf("BSS %d FifoRetriedPktCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoRetriedPktCnt);
			}
			break;
        case OID_802_11_GET_RADIO_STATS_COUNT:
             {
                 RADIO_STATS_COUNTER radiostatscounter;
                 radiostatscounter = *((PRADIO_STATS_COUNTER)wrq.u.data.pointer);
                 printf("TotalBeaconCount %u \n", radiostatscounter.TotalBeaconSentCount);
                 printf("TotalTxCount %u \n", radiostatscounter.TotalTxCount);
                 printf("TotalRxCount %u \n", radiostatscounter.TotalRxCount);
                 printf("TxDataCount %u \n", radiostatscounter.TxDataCount);
                 printf("RxDataCount %u \n", radiostatscounter.RxDataCount);
                 printf("TxRetriedPktCount %u \n", radiostatscounter.TxRetriedPktCount);
                 printf("TxRetriedCount %u \n", radiostatscounter.TxRetriedCount);
           //      printf("RxBeaconCount %u \n", radiostatscounter.RxBeaconCount);
            //     printf("RxProbReqCount %u \n", radiostatscounter.RxProbReqCount);           
             }
             break;
		case OID_802_11_STA_STATISTICS:
			Sta_stat = *((PRT_STA_STATISTICS_TABLE)wrq.u.data.pointer);
		//	printf(" After: iface %s, subcmd %d, length %d\n",
                 //       wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
			for(i = 0; i < Sta_stat.Num; i++)
			{
				printf("APIDX: %u \n", Sta_stat.STAEntry[i].ApIdx);
				printf("MAC: %x:%x:%x:%x:%x:%x \n", Sta_stat.STAEntry[i].Addr[0], Sta_stat.STAEntry[i].Addr[1], Sta_stat.STAEntry[i].Addr[2], Sta_stat.STAEntry[i].Addr[3], Sta_stat.STAEntry[i].Addr[4], Sta_stat.STAEntry[i].Addr[5]);
				printf("RxCount: %u \n", Sta_stat.STAEntry[i].RxCount);
				printf("TxCount: %u \n", Sta_stat.STAEntry[i].TxCount);
				printf("ReceivedByteCount: %u \n", Sta_stat.STAEntry[i].ReceivedByteCount);
				printf("TransmittedByteCount: %u \n", Sta_stat.STAEntry[i].TransmittedByteCount);
				printf("RxErrorCount: %u \n", Sta_stat.STAEntry[i].RxErrorCount);
				printf("RxDropCount: %u \n", Sta_stat.STAEntry[i].RxDropCount);
				printf("TxErrorCount: %u \n", Sta_stat.STAEntry[i].TxErrorCount);
                                printf("TxDropCount: %u \n", Sta_stat.STAEntry[i].TxDropCount);
				printf("TxRetriedPktCount: %u \n", Sta_stat.STAEntry[i].TxRetriedPktCount);
				printf("ChannelUseTime: %u \n", Sta_stat.STAEntry[i].ChannelUseTime);
			}
			break;
		case OID_802_11_ASSOLIST:
			buf = os_malloc(wrq.u.data.length);
			memcpy(buf,wrq.u.data.pointer,wrq.u.data.length);
			printf("\n%s\n",(buf));
			free(buf);
			buf=NULL;
		break;
		 case 9999:
			MacTable = *((PRT_802_11_MAC_TABLE)wrq.u.data.pointer);
			for(i=0; i < MacTable.Num; i++)
			{
				printf("APIDX: %u \n", MacTable.Entry[i].ApIdx);
				printf("Mac: %x:%x:%x:%x:%x;%x \n", MacTable.Entry[i].Addr[0],MacTable.Entry[i].Addr[1], MacTable.Entry[i].Addr[2], MacTable.Entry[i].Addr[3], MacTable.Entry[i].Addr[4], MacTable.Entry[i].Addr[5]);
				printf("Aid: %u \n", MacTable.Entry[i].Aid);
				printf("Psm: %u \n", MacTable.Entry[i].Psm);
				printf("MimoPs: %u \n", MacTable.Entry[i].MimoPs);
				printf("AvgRssi0: %d \n", MacTable.Entry[i].AvgRssi0);
				printf("AvgRssi1: %d \n", MacTable.Entry[i].AvgRssi1);
				printf("AvgSnr: %u \n", MacTable.Entry[i].AvgSnr);
				printf("ConnectedTime: %u \n", MacTable.Entry[i].ConnectedTime);
				printf("MCS: %u \n", MacTable.Entry[i].TxRate.field.MCS);
				printf("BW: %u \n", MacTable.Entry[i].TxRate.field.BW);
				printf("GI: %u \n", MacTable.Entry[i].TxRate.field.ShortGI);
				printf("STBC: %u \n", MacTable.Entry[i].TxRate.field.STBC);
			//	printf("rsv: %x \n", MacTable.Entry[i].TxRate.field.rsv);
				printf("MODE: %u \n", MacTable.Entry[i].TxRate.field.MODE);
				//printf("Word: %u \n", MacTable.Entry[i].TxRate.word);
				printf("LastRxRate: %u \n", MacTable.Entry[i].LastRxRate);
	
			}
			break;
		 
		case	OID_802_11_GET_CURRENT_CHANNEL_AP_TABLE:
			
			{
				BEACON_TABLE beaconTable;
				beaconTable = *((PBEACON_TABLE)wrq.u.data.pointer);
				
				printf("BSSID             SSIDLEN SSID                               CHANNEL CW ExCH RSSI SNR PHYMOD Nss \n");
				for(i=0; i < beaconTable.Num; i++)
				{
				//	printf(" BSSID			SSIDLEN	SSID				CHANNEL CW ExCH	RSSI  SNR PHYMOD  Nss");
					printf("%02x:%02x:%02x:%02x:%02x:%02x ", beaconTable.BssTable[i].Bssid[0],  beaconTable.BssTable[i].Bssid[1],  beaconTable.BssTable[i].Bssid[2],  beaconTable.BssTable[i].Bssid[3],  beaconTable.BssTable[i].Bssid[4],  beaconTable.BssTable[i].Bssid[5]);
					printf(" %6u ", beaconTable.BssTable[i].SsidLen);
					printf(" %33s ", beaconTable.BssTable[i].Ssid);
					printf(" %6u ", beaconTable.BssTable[i].Channel);
					printf("%2u ", beaconTable.BssTable[i].ChannelWidth);
					printf(" %3u ", beaconTable.BssTable[i].ExtChannel);
					printf(" %3d ", beaconTable.BssTable[i].RSSI);
					printf(" %2u ", beaconTable.BssTable[i].SNR);
					printf(" %5u ", beaconTable.BssTable[i].PhyMode);
					printf(" %2u  \n", beaconTable.BssTable[i].NumSpatialStream);
				}	
			} 
			break;

		case	OID_802_11_GET_SCAN_RESULTS:
			{
				SCAN_RESULTS scanResult;
				//printf("\n anand 12");
				scanResult = *((PSCAN_RESULTS)wrq.u.data.pointer);
				printf("Ch_BUSY_Time: %u \n", scanResult.ch_busy_time);
				printf("CCA Error Count: %u \n", scanResult.cca_err_cnt);
				printf("Non wifi Intf: %u \n", scanResult.ch_non_wifi_intf);
				
//				printf("Noise Floor: %d \n",scanResult.Avg_NF);
				printf("AP NUM: %u \n\n\n",scanResult.num_ap);
				printf("BSSID             SSIDLEN SSID                               CHANNEL CW ExCH RSSI SNR PHYMOD Nss \n");
				for(i=0; i < scanResult.num_ap; i++)
                                {
                                        printf("%02x:%02x:%02x:%02x:%02x:%02x ", scanResult.BssTable[i].Bssid[0], scanResult.BssTable[i].Bssid[1],  scanResult.BssTable[i].Bssid[2],  scanResult.BssTable[i].Bssid[3], scanResult.BssTable[i].Bssid[4],  scanResult.BssTable[i].Bssid[5]);
                                        printf(" %6u ", scanResult.BssTable[i].SsidLen);
                                        printf(" %33s ", scanResult.BssTable[i].Ssid);
                                        printf(" %6u ", scanResult.BssTable[i].Channel);
                                        printf(" %2u ", scanResult.BssTable[i].ChannelWidth);
                                        printf(" %3u ", scanResult.BssTable[i].ExtChannel);
                                        printf(" %3d ", scanResult.BssTable[i].RSSI);
                                        printf(" %2u ", scanResult.BssTable[i].SNR);
                                        printf(" %5u ",scanResult.BssTable[i].PhyMode);
                                        printf(" %2u  \n", scanResult.BssTable[i].NumSpatialStream);
                                } 
			}
			break;
		
		case 	OID_802_11_GET_ACCESS_CATEGORY_TRAFFIC_STATUS:
			{
				STREAMING_STATUS streamingStatus;
				streamingStatus = *((PSTREAMING_STATUS)wrq.u.data.pointer);
				printf("Streaming Status is: %u \n", streamingStatus.status);	
				
			}
			break;
		default:
			break;
				
	}
//	free(wrq.u.data.pointer);

//	printf(" After: iface %s, subcmd %d, buffer %p, length %d\n",
//			wrq.ifr_name, wrq.u.data.flags, wrq.u.data.pointer, wrq.u.data.length);

	return 0;
}
