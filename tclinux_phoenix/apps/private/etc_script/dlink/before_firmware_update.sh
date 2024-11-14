#!/bin/sh

/usr/bin/killall -9 tr69 2>/dev/null
/usr/bin/killall -9 udhcpd 2>/dev/null
/usr/bin/killall -9 radvd 2>/dev/null
/usr/bin/killall -9 br2684ctl 2>/dev/null
/usr/bin/killall -9 wscd 2>/dev/null
/usr/bin/killall -9 dnsmasq 2>/dev/null
/usr/bin/killall -9 pppd 2>/dev/null
/usr/bin/killall -9 arp 2>/dev/null
/usr/bin/killall -9 dropbear 2>/dev/null
/usr/bin/killall -9 utelnetd 2>/dev/null
/usr/bin/killall -9 tftpd 2>/dev/null
	

echo "3" > /proc/sys/vm/drop_caches
