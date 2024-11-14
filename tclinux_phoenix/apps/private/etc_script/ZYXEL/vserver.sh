#!/bin/sh

if [ $# != 3 ] ; then
        echo "usage: $0 [add/del] [PVC#] [Rule#]"
        exit 0
fi

if [ $1 = "add" ] ; then
        OPT=-I
elif [ $1 = "del" ] ; then
        OPT=-D
else
        echo "usage: $0 [add/del] [PVC#] [Rule#]"
        exit 0
fi

PVC=$2
RULE=$3

VS_CONF=/etc/nat_pvc$PVC/vserver$RULE
if [ -f $VS_CONF ] ; then
        chmod +x $VS_CONF
        . $VS_CONF
else
	exit 0
fi

if [ "$ACTIVE" = "No" ] ; then
	exit 0
fi

EXT_SPORT=$STARTPORT
EXT_EPORT=$ENDPORT
INT_IP=$LOCALIP

INT_SPORT=$LOCAL_SPORT
INT_EPORT=$LOCAL_EPORT

# get ipmode (Single or Multiple)
IPMODE_CONF=/etc/nat_pvc$PVC/ipmode
if [ -f $IPMODE_CONF ] ; then
        chmod +x $IPMODE_CONF
        . $IPMODE_CONF
fi
SERVER_IP="0.0.0.0"
if [ "$IPMODE" = "Multiple" ] ; then
	SERVERIP_CONF=/etc/nat_pvc$PVC/server_ip
	if [ -f $SERVERIP_CONF ] ; then
        	chmod +x $SERVERIP_CONF
        	. $SERVERIP_CONF
	fi
fi

# default wan_if
WAN_IF=nas$PVC
CONFFILE=/etc/isp$PVC.conf
if [ -f $CONFFILE ] ; then
        chmod +x $CONFFILE
        . $CONFFILE
fi
# PPPoE/PPPoA
if [ "$ISP" = "2" ] ; then
        WAN_IF=ppp$PVC
fi

# judge bypassTriangleRoute
PROFILE_CFG=/userfs/profile.cfg
if [ -f $PROFILE_CFG ] ; then
    . $PROFILE_CFG
fi

if [ "$TCSUPPORT_OBM_FW" != "" ] ;then
	FIREWALL_CONF=/etc/firewall.conf
	if [ -f $FIREWALL_CONF ] ; then
        chmod +x $FIREWALL_CONF
        . $FIREWALL_CONF
	fi

	LAN_CONF=/etc/lan.conf
	if [ -f $LAN_CONF ] ; then
        chmod +x $LAN_CONF
        . $LAN_CONF
	fi
	LAN_IP=$IP
fi
if [ "$SERVER_IP" = "0.0.0.0" ] ; then
	if [ -n "$INT_SPORT" ] ; then
	iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP:$INT_SPORT-$INT_EPORT

	iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP:$INT_SPORT-$INT_EPORT
	if [ "$TCSUPPORT_OBM_FW" != "" ] ;then
		if [ "$BypassTriRoute" = "1" -a  "$OPT" = "-I" ] || [ "$OPT" = "-D" ] ;then
			iptables -t nat $OPT VS_PRE$PVC -p tcp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP:$INT_SPORT-$INT_EPORT 2>/dev/null
			iptables -t nat $OPT VS_PRE$PVC -p udp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP:$INT_SPORT-$INT_EPORT 2>/dev/null
		fi
	fi
	else
	iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP

	iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP
	if [ "$TCSUPPORT_OBM_FW" != "" ] ;then
		if [ "$BypassTriRoute" = "1" -a  "$OPT" = "-I" ] || [ "$OPT" = "-D" ] ;then
			iptables -t nat $OPT VS_PRE$PVC -p udp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP 2>/dev/null
			iptables -t nat $OPT VS_PRE$PVC -p tcp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP 2>/dev/null
		fi
	fi
	fi
	if [ "$TCSUPPORT_OBM_FW" != "" ] ;then
		if [ "$BypassTriRoute" = "1" -a  "$OPT" = "-I" ] || [ "$OPT" = "-D" ] ;then
		iptables -t nat $OPT POSTROUTING -p tcp -d $INT_IP --dport $EXT_SPORT:$EXT_EPORT -j SNAT --to $LAN_IP 2>/dev/null
		iptables -t nat $OPT POSTROUTING -p udp -d $INT_IP --dport $EXT_SPORT:$EXT_EPORT -j SNAT --to $LAN_IP 2>/dev/null
		fi
	fi
	if [ "$TCSUPPORT_OBM_FW" != "" ] ;then
		if [ "$BypassTriRoute" = "0" ] ;then
			if [ -n "$INT_SPORT" ] ; then
			iptables -t nat -D VS_PRE$PVC -p tcp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP:$INT_SPORT-$INT_EPORT 2>/dev/null
			iptables -t nat -D VS_PRE$PVC -p udp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP:$INT_SPORT-$INT_EPORT 2>/dev/null
			else
			iptables -t nat -D VS_PRE$PVC -p udp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP 2>/dev/null
			iptables -t nat -D VS_PRE$PVC -p tcp -i br0 -d ! $LAN_IP --dport $EXT_SPORT:$EXT_EPORT -j DNAT --to $INT_IP 2>/dev/null
			fi
			iptables -t nat -D POSTROUTING -p tcp -d $INT_IP --dport $EXT_SPORT:$EXT_EPORT -j SNAT --to $LAN_IP 2>/dev/null
			iptables -t nat -D POSTROUTING -p udp -d $INT_IP --dport $EXT_SPORT:$EXT_EPORT -j SNAT --to $LAN_IP 2>/dev/null
		fi
	fi
else

iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -d $SERVER_IP -s 0/0  -j DNAT --to $INT_IP

iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -d $SERVER_IP -s 0/0  -j DNAT --to $INT_IP

fi

