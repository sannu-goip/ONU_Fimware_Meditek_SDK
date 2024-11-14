#!/bin/sh

SAMBA_FILE=/etc/samba/smb.conf

if [ ! -n "$5" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <netbios_name> <workgroup> <guest> <map_to_guest>"
	echo "Example: $0 ECONET admin"
	exit 0
fi

NETBIOS_NAME="$1"
WORKGROUP="$2"
GUEST="$3"
BADUSER="$4 $5"

echo "[global]
netbios name = $NETBIOS_NAME
server string = Samba Server
workgroup = $WORKGROUP
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
bind interfaces only = yes
interfaces = br0
map to guest = $BADUSER" > $SAMBA_FILE
