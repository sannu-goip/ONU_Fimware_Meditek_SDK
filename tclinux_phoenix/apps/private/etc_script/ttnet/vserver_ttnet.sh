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

EXT_SPORT=$STARTPORT
EXT_EPORT=$ENDPORT
INT_IP=$LOCALIP
PROTOCOL=$PROTOCOL

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

if [ "$SERVER_IP" = "0.0.0.0" ] ; then
	if [ "$PROTOCOL" = "TCP" ] ; then
		iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP:$INTPORT	
	elif [ "$PROTOCOL" = "UDP" ] ; then
		iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP:$INTPORT
	else
		iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP:$INTPORT
		iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -i $WAN_IF -j DNAT --to $INT_IP:$INTPORT
	fi
	
else
	
	if [ "$PROTOCOL" = "TCP" ] ; then
		iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -d $SERVER_IP -s 0/0  -j DNAT --to $INT_IP:$INTPORT
	elif [ "$PROTOCOL" = "UDP" ] ; then
		iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -d $SERVER_IP -s 0/0  -j DNAT --to $INT_IP:$INTPORT
	else
		iptables -t nat $OPT VS_PRE$PVC -p tcp --dport $EXT_SPORT:$EXT_EPORT -d $SERVER_IP -s 0/0  -j DNAT --to $INT_IP:$INTPORT
		iptables -t nat $OPT VS_PRE$PVC -p udp --dport $EXT_SPORT:$EXT_EPORT -d $SERVER_IP -s 0/0  -j DNAT --to $INT_IP:$INTPORT
	fi

fi

