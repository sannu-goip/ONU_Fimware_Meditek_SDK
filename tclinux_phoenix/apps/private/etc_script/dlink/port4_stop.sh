#!/bin/sh

Interface="eth0.4"
iptables -D FORWARD -o $Interface -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

iptables -t nat -D POSTROUTING -o $Interface -j MASQUERADE
iptables -F PKT_FLT

if [ -f /var/run/udhcpc-$Interface.pid ] ; then
	#shnwind modify ->use SIGTERM to send release
	kill -SIGTERM `cat /var/run/udhcpc-$Interface.pid`
fi
/sbin/ifconfig $Interface down
#brctl addif br0 $Interface

