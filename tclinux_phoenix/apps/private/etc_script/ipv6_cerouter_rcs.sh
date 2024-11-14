#!/bin/sh

MAC_CONFFILE=/etc/mac.conf
if [ -f $MAC_CONFFILE ]; then
	chmod 777 $MAC_CONFFILE
	. $MAC_CONFFILE
	mac1=`echo $LAN_MAC | cut -d ':' -f 1`
	mac2=`echo $LAN_MAC | cut -d ':' -f 2`
	mac3=`echo $LAN_MAC | cut -d ':' -f 3`
	mac4=`echo $LAN_MAC | cut -d ':' -f 4`
	mac5=`echo $LAN_MAC | cut -d ':' -f 5`
	mac6=`echo $LAN_MAC | cut -d ':' -f 6`
	mac1=$((0x${mac1}^0x2))
	mac1=`echo ${mac1} | awk '{printf("%x\n",$0)}'`
	/usr/bin/ip -6 addr add fe80::${mac1}${mac2}:${mac3}FF:FE${mac4}:${mac5}${mac6}/64 dev br0
fi
echo 1 > /proc/tc3162/ipv6_defrag
#rmmod hw_nat
