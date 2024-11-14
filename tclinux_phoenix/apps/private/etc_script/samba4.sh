#!/bin/sh

SAMBA_FILE=/etc/samba/smb.conf

if [ ! -n "$3" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <netbios_name> <workgroup> <guest> [map_to_guest]"
	echo "Example: $0 ECONET admin"
	exit 0
fi

if [ ! -n "$5" ]; then
	BADUSER="/ /"
else
	BADUSER="$4 $5"
fi

NETBIOS_NAME="$1"
WORKGROUP="$2"
GUEST="$3"

echo "[global]
netbios name = $NETBIOS_NAME
server string = Samba Server
workgroup = $WORKGROUP
smb ports = 445
security = user
guest account = $GUEST
log file = /var/log.samba
socket options = TCP_NODELAY SO_RCVBUF=262144 SO_SNDBUF=131072
passdb backend = smbpasswd
encrypt passwords = yes
use sendfile = yes
use spne go = no
client use spnego = no
disable spoolss = yes
smb passwd file = /etc/samba/smbpasswd
host msdfs = no
strict allocate = No
os level = 20
log level = 1
max log size = 50
max protocol = NT1
null passwords = yes
mangling method = hash
dos charset = ASCII
unix charset = UTF8
display charset = UTF8
fstype = Samba
map to guest = $BADUSER

[USB1_1]
path = /tmp/mnt/usb1_1
browseable = Yes
writable = Yes
create mask = 0664
directory mask = 0775

[USB1_2]
path = /tmp/mnt/usb1_2
browseable = Yes
writable = Yes
create mask = 0664
directory mask = 0775

[USB2_1]
path = /tmp/mnt/usb2_1
browseable = Yes
writable = Yes
create mask = 0664
directory mask = 0775

[USB2_2]
path = /tmp/mnt/usb2_2
browseable = Yes
writable = Yes
create mask = 0664
directory mask = 0775" > $SAMBA_FILE
