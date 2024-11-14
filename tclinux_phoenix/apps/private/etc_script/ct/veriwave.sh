echo 100000 > /proc/net/skbmgr_driver_max_skb
echo 80000 > /proc/net/skbmgr_limit
echo 80000 > /proc/net/skbmgr_4k_limit
echo 30000 > /proc/net/skbmgr_hot_list_len
echo 7 > /proc/irq/25/smp_affinity
echo 7 > /proc/irq/24/smp_affinity
echo 8 > /proc/irq/22/smp_affinity
hw_nat -N 1
hw_nat -U 300 300 300 300
if [ $1 == "2" ]
then
echo 0 0 2 > /proc/tc3162/lan_tx
iwpriv ra0 set pse_dbg=0
iwpriv ra0 set ShortcutQMax=73000
iwpriv ra0 set ShortcutMaxSkb=73000
iwpriv ra0 set ShortcutQSize=128
iwpriv ra0 set BeaconPeriod=500
iwpriv ra0 set ShortcutFlag=1
iwpriv ra0 set autoShortcutFlag=0
iwpriv ra0 set pse_dbg=0
else
echo  0 1 2 > /proc/tc3162/lan_tx
iwpriv rai0 set ShortcutMaxSkb=63000
iwpriv rai0 set BeaconPeriod=1000
iwpriv rai0 set ShortcutQSize=1024
iwpriv rai0 set ShortcutQMax=63000
iwpriv rai0 set autoShortcutFlag=0
iwpriv rai0 set txDetectFlag=0
iwpriv rai0 set ShortcutFlag=1
iwpriv rai0 set TxBurst=1
iwpriv rai0 set HtRdg=1
iwpriv rai0 mac 1300=0007315e
fi
killall -9 java
killall jvm_monitor
killall bundle_monitor
killall bandwidth
killall mobile-manager
killall usb_auto_mount
killall tr69
killall tcwdog
