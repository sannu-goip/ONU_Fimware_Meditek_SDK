#!/bin/sh

Interface="eth0.4"
CONFFILE=/etc/port4.conf
echo $CONFFILE

if [ -f $CONFFILE ] ; then
	chmod 777 $CONFFILE
	. $CONFFILE
else
	exit 0
fi
MAC_CONFFILE=/etc/mac.conf
if [ -f $MAC_CONFFILE ]; then
	chmod 777 $MAC_CONFFILE
	. $MAC_CONFFILE
fi

if [ "$Active" != "Yes" ] ; then
	if [ "$LAN_MAC" != "" ]; then
		 /sbin/ifconfig $Interface hw ether $LAN_MAC
	fi
	/sbin/ifconfig $Interface 0.0.0.0 up
	exit 0
fi
if [ "$ConnectType" = "DynamicIP" ] ; then
	if [ "$WAN_MAC" != "" ]; then
		 /sbin/ifconfig $Interface hw ether $WAN_MAC
	fi
	/sbin/ifconfig $Interface 0.0.0.0
	if [ "$MTU" = "0" ] || [ "$MTU" = "" ] ; then
		/sbin/ifconfig $Interface mtu 1500
		iptables -A FORWARD -o $Interface -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
	else
		/sbin/ifconfig $Interface mtu $MTU
		iptables -A FORWARD -o $Interface -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
	fi
	if [ "$DEFAULTROUTE" = "Yes" ] ; then
		/sbin/udhcpc -i $Interface -s /usr/script/udhcpc.sh -p /var/run/udhcpc-$Interface.pid &
	else
		/sbin/udhcpc -i $Interface -s /usr/script/udhcpc_nodef.sh -p /var/run/udhcpc-$Interface.pid &
	fi
	if [ "$NATENABLE" = "Enable" ] ; then
		iptables -t nat -A POSTROUTING -o $Interface -j MASQUERADE
		#iptables -t nat -A PREROUTING -j VS_PRE$i
		#iptables -t nat -A PREROUTING -j DMZ_PRE$i
		if [ "$TCSUPPORT_PORT_TRIGGER" != "" ] ;then
		iptables -t nat -A PREROUTING -i $Interface -j PREROUTING_WAN
		iptables -t filter -A FORWARD -i $Interface -j FORWARD_WAN
		fi
	fi
	iptables -t raw -F
	/userfs/bin/dnsmasq &

elif [ "$ConnectType" = "StaticIP" ] ; then
	if [ "$WAN_MAC" != "" ]; then
		 /sbin/ifconfig $Interface hw ether $WAN_MAC
	fi
	/sbin/ifconfig $Interface $IPADDR netmask $NETMASK up
	if [ "$MTU" = "0" ] || [ "$MTU" = "" ] ; then
		/sbin/ifconfig $Interface mtu 1500
		iptables -A FORWARD -o $Interface -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
	else
		/sbin/ifconfig $Interface mtu $MTU
		iptables -A FORWARD -o $Interface -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
	fi
	if [ "$DEFAULTROUTE" = "Yes" ] ; then
		route add default gw $GATEWAY dev $Interface
	fi
	if [ "$NATENABLE" = "Enable" ] ; then
		iptables -t nat -A POSTROUTING -o $Interface -j MASQUERADE
		#iptables -t nat -A PREROUTING -j VS_PRE$i
		#iptables -t nat -A PREROUTING -j DMZ_PRE$i
	if [ "$TCSUPPORT_PORT_TRIGGER" != "" ] ;then
		iptables -t nat -A PREROUTING -i $Interface -j PREROUTING_WAN
		iptables -t filter -A FORWARD -i $Interface -j FORWARD_WAN
	fi
	fi
	iptables -t raw -F
	/userfs/bin/dnsmasq &

fi
