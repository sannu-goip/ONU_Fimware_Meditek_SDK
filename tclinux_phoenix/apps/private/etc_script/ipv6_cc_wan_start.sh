#!/bin/sh
/usr/bin/ip6tables -D FORWARD -i br0 -j DROP
/usr/bin/ip6tables -A FORWARD -i br0 -j DROP
sleep 6
/userfs/bin/dhcp6c -c /etc/dhcp6c_$1.conf $1  -p  /var/run/dhcp6c_$1.pid  -v /var/run/dhcp6c-$1.vt -u /var/run/dhcp6c-$1.pt &
