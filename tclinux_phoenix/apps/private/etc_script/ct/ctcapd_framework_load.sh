#!/bin/sh

#Mount ubi system
/userfs/bin/ubiattach /dev/ubi_ctrl -m 10
/bin/mount -t ubifs ubi0:ubifs0 /ctcap
if [ "$?" != "0" ]; then
echo "mount ubi system failed!"
/userfs/bin/ubidetach /dev/ubi_ctrl -m 10
/userfs/bin/ubiformat /dev/mtd10 -y
/userfs/bin/ubiattach /dev/ubi_ctrl -m 10
/userfs/bin/ubimkvol /dev/ubi0 -s 20MiB -N ubifs0
/bin/mount -t ubifs ubi0:ubifs0 /ctcap
/bin/mkdir -p /ctcap/apps
/bin/mkdir -p /ctcap/bin
fi