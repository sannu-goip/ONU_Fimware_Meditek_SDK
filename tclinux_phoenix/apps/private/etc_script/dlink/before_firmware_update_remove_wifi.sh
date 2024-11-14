#!/bin/sh

WLAN_MODULE_RT5390=/lib/modules/rt5390ap.ko
WLAN_MODULE_RT3390=/lib/modules/rt3390ap.ko
WLAN_MODULE_RT3090=/lib/modules/rt3090ap.ko
WLAN_NODULE_RT2561=/lib/modules/rt61ap.ko

/sbin/ifconfig ra0 down 2>/dev/null
/sbin/ifconfig ra1 down 2>/dev/null
/sbin/ifconfig ra2 down 2>/dev/null
/sbin/ifconfig ra3 down 2>/dev/null

if [ -f $WLAN_MODULE_RT5390 ] ; then

	echo "rmmod rt5390ap"
	/sbin/rmmod rt5390ap 2>/dev/null
fi

if [ -f $WLAN_MODULE_RT3390 ] ; then
	echo "rmmod rt3390ap"
	/sbin/rmmod rt3390ap 2>/dev/null
fi

if [ -f $WLAN_MODULE_RT3090 ] ; then
	echo "rmmod rt3090ap"
	/sbin/rmmod rt3090ap 2>/dev/null
fi

if [ -f $WLAN_MODULE_RT2561 ] ; then
	echo "rmmod rt2561ap"
	/sbin/rmmod rt61ap 2>/dev/null
fi

/usr/bin/killall -9 tr69 2>/dev/null
/usr/bin/killall -9 udhcpd 2>/dev/null
/usr/bin/killall -9 radvd 2>/dev/null
#/usr/bin/killall -9 br2684ctl 2>/dev/null   marked for wan upgrade
/usr/bin/killall -9 wscd 2>/dev/null
/usr/bin/killall -9 dnsmasq 2>/dev/null
#/usr/bin/killall -9 pppd 2>/dev/null    marked for wan upgrade
/usr/bin/killall -9 arp 2>/dev/null
/usr/bin/killall -9 dropbear 2>/dev/null
/usr/bin/killall -9 utelnetd 2>/dev/null
/usr/bin/killall -9 tftpd 2>/dev/null
	

echo "3" > /proc/sys/vm/drop_caches
