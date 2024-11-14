#!/bin/sh

if [ $# != 1 ] ; then
	echo "usage: $0 [PVCn]"
	exit 0
fi

i=$1
CONFFILE=/etc/isp$i.conf
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
	exit 0
fi

if [ $ISP != "2" ] && [ $ISP != "3" ] ; then
	exit 0
fi
 
if [ "$CONNECTION" != "Connect_Manually" ] ; then
	exit 0
fi
  #make sure the LCP echo will take no effect about ppp
	PPP_PARAM="unit $i user $USERNAME password $PASSWORD nodetach holdoff 4 maxfail 0 usepeerdns lcp-echo-interval 10 lcp-echo-failure 12"

if [ "$ISP" = "2" ] ; then #PPPoE
	PPP_PARAM="$PPP_PARAM plugin libpppoe.so" 
	if [ "$SRVNAME" != "" ] ; then
		PPP_PARAM="$PPP_PARAM rp_pppoe_service $SRVNAME" 
	fi
	PPP_PARAM="$PPP_PARAM nas$i" 
elif [ "$ISP" = "3" ] ; then #PPPoA
    if [ "$ENCAP" = "LLC" ] ; then
	PPP_PARAM="$PPP_PARAM plugin libpppoatm.so llc-encaps $VPI.$VCI"
    elif [ "$ENCAP" = "VC-Mux" ] ; then
	PPP_PARAM="$PPP_PARAM plugin libpppoatm.so vc-encaps $VPI.$VCI"
    fi
fi

    # PPPoE
    if [ "$ISP" = "2" ] ; then
	if [ "$AUTHEN" = "CHAP" ] ; then
		PPP_PARAM="$PPP_PARAM -pap"
	elif [ "$AUTHEN" = "PAP" ] ; then
		PPP_PARAM="$PPP_PARAM -chap"
	fi

	if [ $QOS = "ubr" ] ; then
		PCR_V="-p $PCR"
	elif [ $QOS = "cbr" ] || [ $QOS = "rt-vbr" ] || [ $QOS = "nrt-vbr" ] ; then
		PCR_V="-p $PCR -q $SCR -m $MBS"
	fi
    	if [ "$ENCAP" = "PPPoE LLC" ] ; then
		ENCAP_T="-e 0"
    	elif [ "$ENCAP" = "PPPoE VC-Mux" ] ; then
		ENCAP_T="-e 1"
	fi
	br2684ctl -c $i $ENCAP_T -t $QOS $PCR_V -a 0.$VPI.$VCI &
	echo $! > /var/run/nas$i.pid
	sleep 1
	if [ "$WAN_MAC" != "" ]; then
		 /sbin/ifconfig nas$i hw ether $WAN_MAC
	fi
	/sbin/ifconfig nas$i 0.0.0.0
    # PPPoA
    else
	PCR_V=""
	if [ $QOS = "ubr" ] ; then
		PCR_V="qos UBR qos_pcr $PCR"
	elif [ $QOS = "cbr" ] || [ $QOS = "rt-vbr" ] || [ $QOS = "nrt-vbr" ] ; then
		if [ $QOS = "cbr" ] ; then 
			PCR_V="qos CBR"
		elif [ $QOS = "rt-vbr" ] ; then
			PCR_V="qos VBR"
		elif [ $QOS = "nrt-vbr" ] ; then
			PCR_V="qos NRTVBR"
		fi
		PCR_V="$PCR_V qos_pcr $PCR qos_scr $SCR qos_mbs $MBS"
	fi
	PPP_PARAM="$PPP_PARAM $PCR_V"
    fi

	if [ "$DEFAULTROUTE" = "Yes" ] ; then
		PPP_PARAM="$PPP_PARAM defaultroute"
	fi
	if [ "$PPPGETIP" = "Dynamic" ] ; then
		PPP_PARAM="$PPP_PARAM noipdefault"
	else
		PPP_PARAM="$PPP_PARAM $IPADDR:$GATEWAY"
	fi
	if [ "$NETMASK" != "" ] ; then
		PPP_PARAM="$PPP_PARAM netmask $NETMASK"
	fi
	if [ "$MTU" != "0" ] && [ "$MTU" != "" ] ; then
		PPP_PARAM="$PPP_PARAM mtu $MTU mru $MTU"
	fi
	if [ "$IPVERSION" = "IPv4/IPv6" ] ; then
		if [ "$CONNECTION" = "Connect_on_Demand" ] ; then
			localIP="::11$i"
			remoteIP="::22$i"	
			PPP_PARAM="$PPP_PARAM ipv6 $localIP,$remoteIP "	
		else
			PPP_PARAM="$PPP_PARAM ipv6 , "
		fi
	elif [ "$IPVERSION" = "IPv6" ] ; then
		if [ "$CONNECTION" = "Connect_on_Demand" ] ; then
			localIP="::11$i"
			remoteIP="::22$i"
			PPP_PARAM="$PPP_PARAM ipv6 $localIP,$remoteIP noip"	
		else
			PPP_PARAM="$PPP_PARAM ipv6 , noip"
		fi		
	fi

	pppd $PPP_PARAM &
	echo $! > /var/run/ppp$i.pid
	echo "pppd $PPP_PARAM &" > /var/tmp/ppp$i.conf
	
	if [ "$LASTmss" != "0" ] ; then
		iptables -D FORWARD -o ppp$i -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss $LASTmss
	fi
	
	if [ "$MSS" = "0" ] || [ "$MSS" = "" ] ; then
		iptables -A FORWARD -o ppp$i -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
	else
		iptables -A FORWARD -o ppp$i -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss $MSS
	fi
	if [ "$IPVERSION" != "IPv6" ] ; then
	if [ "$NATENABLE" = "Enable" ] ; then
#		iptables -t nat -A POSTROUTING -j ADDRMAP_POS$i
	if [ "$TCSUPPORT_MULTI_NAT" != "" ] ;then	
		iptables -t nat -A ADDRMAP_POS -j ADDRMAP_POS$i
	fi
		iptables -t nat -A POSTROUTING -o ppp$i -j MASQUERADE
		iptables -t nat -A PREROUTING -j VS_PRE$i
		iptables -t nat -A PREROUTING -j DMZ_PRE$i
#		iptables -t nat -A PREROUTING -j ADDRMAP_PRE$i
	if [ "$TCSUPPORT_MULTI_NAT" != "" ] ;then	
		iptables -t filter -A ADDRMAP_FORWARD -j ADDRMAP_FORWARD$i	
	fi
	fi
	fi
	WAN_IF=ppp$i

/usr/script/ether_mac.sh
if [ $BridgeInterface = "Yes" ] ; then
	/userfs/bin/pppoe-relay -C br0 -S nas$i &
fi
