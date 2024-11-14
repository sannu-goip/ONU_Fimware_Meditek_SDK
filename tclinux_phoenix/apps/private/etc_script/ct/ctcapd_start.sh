#!/bin/sh

#Start ctcapd
/usr/bin/ctcapd &> /dev/null && pidof ctcapd > /var/run/ctcapd.pid

while [ ! -e "/tmp/ctcapd_start_finish" ];do
	sleep 1
done

#Start appd, elinkclt
sleep 1
/bin/init.ctc &
