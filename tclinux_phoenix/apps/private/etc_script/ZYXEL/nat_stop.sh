#!/bin/sh

if [ $# != 1 ] ; then
	echo "usage: $0 [PVCn]"
	exit 0
fi

i=$1

#iptables -t nat -D POSTROUTING -o nas$i -j MASQUERADE
#iptables -t nat -D POSTROUTING -o ppp$i -j MASQUERADE
#iptables -t nat -D POSTROUTING -j ADDRMAP_POS$i
#iptables -t nat -D PREROUTING -j VS_PRE$i
#iptables -t nat -D PREROUTING -j DMZ_PRE$i
#iptables -t nat -D PREROUTING -j ADDRMAP_PRE$i
# flush nat rule
#iptables -t nat -F VS_PRE$i
#iptables -t nat -F DMZ_PRE$i
#iptables -t nat -F ADDRMAP_PRE$i
#iptables -t nat -F ADDRMAP_POS$i

