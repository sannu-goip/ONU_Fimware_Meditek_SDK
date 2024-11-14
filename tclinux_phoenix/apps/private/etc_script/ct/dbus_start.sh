#!/bin/sh

#Start dbus
mv /etc/dbus /var/run/
mkdir -p /var/lib/dbus
/usr/sbin/dbus-uuidgen > /var/lib/dbus/machine-id
export LD_LIBRARY_PATH=/usr/lib/glib-2.0/:/usr/lib
taskset 0x8 /usr/sbin/dbus-daemon --system
#/usr/sbin/dbus-daemon --system
/userfs/bin/ctc_igd1_dbus &> /dev/null && pidof ctc_igd1_dbus > /var/run/ctc_igd1_dbus.pid

while [ ! -e "/tmp/dbus_start_finish" ];do
echo "-------***dbus_start_finish not exist***--------"
	sleep 1
done

while [ ! -e "/tmp/usb_auto_mount_finish" ];do
echo "-------***usb_auto_mount_finish not exist***--------"
	sleep 1
done

#Start saf service
is_start=`/userfs/bin/tcapi get globalstate_common is_avalanche_start`
if [ "$is_start" != "Yes" ]
then
/userfs/bin/saf service 8 9 10 
fi
