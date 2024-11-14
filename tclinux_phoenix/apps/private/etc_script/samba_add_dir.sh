#!/bin/sh

SAMBA_FILE=/etc/samba/smb.conf

if [ ! -n "$4" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <dir name> <access path> <browseable> <writable> [guest ok]"
	echo "Example: $0 temp /temp admin"
	exit 0
fi

if [ ! -n "$5" ]; then
	para5="No"
else
	para5="$5"
fi

echo "
["$1"]
path = "$2"
browseable = "$3"
writable = "$4"
create mask = 0664
directory mask = 0775
guest ok = "$para5"

[USB1_1]
path = "/tmp/mnt/usb1_1"
browseable = yes
writable = yes
create mask = 0664
directory mask = 0775

[USB1_2]
path = "/tmp/mnt/usb1_2"
browseable = yes
writable = yes
create mask = 0664
directory mask = 0775

[USB2_1]
path = "/tmp/mnt/usb2_1"
browseable = yes
writable = yes
create mask = 0664
directory mask = 0775

[USB2_2]
path = "/tmp/mnt/usb2_2"
browseable = yes
writable = yes
create mask = 0664
directory mask = 0775
" >> $SAMBA_FILE

