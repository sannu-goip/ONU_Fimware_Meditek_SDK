#!/bin/sh
PROFILE_CFG=/userfs/profile.cfg
if [ -f $PROFILE_CFG ] ; then
    chmod 777 $PROFILE_CFG
    . $PROFILE_CFG
fi

if [ "$TCSUPPORT_CT_JOYME" != "" ] ;then
/bin/mkdir -m 0777 -p /var/osgi
fi

if [ "$TCSUPPORT_CT_JOYME" != "" ] ;then
insmod /lib/modules/plugin_netlimit.ko
fi
if [ "$TCSUPPORT_CT_JOYME2" != "" ] ;then
if [ "$TCSUPPORT_CFG_NG" = "" ] ;then
insmod /lib/modules/info_utility.ko
fi
fi

if [ "$TCSUPPORT_CT_JOYME_BANDWIDTH" != "" ] ;then
insmod /lib/modules/bandwidth.ko
/userfs/bin/tcapi commit BandWidth 
/userfs/bin/tcapi commit MaxBandWidth 
fi

if [ -f /tmp/dev_register ] ; then
registerVal=`cat /tmp/dev_register`
if [ "$registerVal" = "1" ];then
		exit
else
    /bin/rm -f /tmp/dev_register
    echo "register sleep Type1"
    sleep 60
    echo "register sleep Type1 end"
fi
fi

if [ "$TCSUPPORT_CT_OPENJDK" != "" ] ;then
SQUASHFS_MTDBLOCK="/dev/mtdblock8"
YAFFS_MTDBLOCK="/dev/mtdblock9"
mount -t squashfs $SQUASHFS_MTDBLOCK /usr/local/
else
YAFFS_MTDBLOCK="/dev/mtdblock8"
fi
#mount yaffs2 or jffs2
if [ "$TCSUPPORT_CT_JOYME" != "" ] ;then
#insmod /lib/modules/nlk_msg.ko
insmod /lib/modules/usbhost/nls_utf8.ko
#YAFFS_MTDBLOCK="/dev/mtdblock8"
if [ "$TCSUPPORT_SQUASHFS_ADD_YAFFS" != "" ] ;then
mount -t yaffs2 $YAFFS_MTDBLOCK /usr/osgi/ -o tags-ecc-off
/bin/mkdir -m 0777 -p /usr/osgi/cgroups
mount -t cgroup -o cpu,memory cpu_memory /usr/osgi/cgroups
if [ "$?" != "0" ] ;then
echo "erase yaffs2 and mount again!"
#mtd erase yaffs
mount -t yaffs2 $YAFFS_MTDBLOCK /usr/osgi/ -o tags-ecc-off
/bin/mkdir -m 0777 -p /usr/osgi/cgroups
mount -t cgroup -o cpu,memory cpu_memory /usr/osgi/cgroups
else
echo "mount yaffs2 success!"
fi
else
mount -t jffs2 -o rw $YAFFS_MTDBLOCK /usr/osgi/
if [ "$?" != "0" ] ;then
echo "erase jffs2 and mount again!"
#/userfs/bin/mtd erase jffs2
mount -t jffs2 -o rw $YAFFS_MTDBLOCK /usr/osgi/
else
echo "mount jffs2 success!"
fi
/bin/mkdir -m 0777 -p /usr/osgi/cgroups
mount -t cgroup -o cpu,memory cpu_memory /usr/osgi/cgroups
fi
fi

if [ -f /tmp/dev_register ] ; then
registerVal=`cat /tmp/dev_register`
if [ "$registerVal" = "1" ];then
		exit
else
    /bin/rm -f /tmp/dev_register
    echo "register sleep Type2"
    sleep 60
    echo "register sleep Type2 end"
fi
fi

if [ "$TCSUPPORT_CT_JOYME" != "" ] ;then
/userfs/bin/usb_monitor &
/userfs/bin/plugin_monitor & 
if [ "$TCSUPPORT_CT_JOYME_BANDWIDTH" != "" ] ;then
/userfs/bin/bandwidth & 
fi
fi

if [ "$TCSUPPORT_CMCCV2" != "" ] ;then
/userfs/bin/tcapi commit ApiPermission
fi

if [ "$TCSUPPORT_CT_JOYME" != "" ] ;then
/userfs/bin/bundle_monitor &
cd /usr/osgi/felix-framework
if [ "$TCSUPPORT_CT_OPENJDK" != "" ] ;then
/usr/script/jamvm.sh
else
../bin/jamvm -Xms32M -Xmx64M -jar bin/felix.jar &
fi
fi

if [ "$TCSUPPORT_CMCCV2" != "" ] ;then
/userfs/bin/mobile-manager &
/userfs/bin/jvm_monitor &
fi
