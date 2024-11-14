#!/bin/sh

killall -9 jvm_monitor 
killall -9 java
killall -9 cvm
killall -9 bench_monitor
rm -f /tmp/jiton
echo dis_ddmi > /proc/pon_phy/debug
#echo 1 > /tmp/methodlist
echo 1 > /tmp/jiton
echo 1 > /tmp/cpuson
cp /usr/script/methodlist /tmp/methodlist

BUNDLENAME=`/userfs/bin/tcapi get CMCCInfo_Entry3 benchname`
BUNDLENAME=`echo $strA | grep ["com.chinamobile.smartgateway."]`
if [ "$BUNDLENAME" = "" ] ;then
	BUNDLENAME=com.chinamobile.smartgateway.jembenchtest
fi
echo 1 > /tmp/buildin_list
echo $BUNDLENAME >> /tmp/buildin_list
/userfs/bin/bench_monitor &

i=0;
while [ $i -lt 10 ];
do
	PID=`pidof java`
	echo "pid=$PID"
	if test -z $PID
	then
	echo "can test"
	i=20
	else
	echo "wait"
	sleep 2
	i=`expr $i + 1`;
killall -9 java
	fi
done
echo "start test now"
rm -rf /tmp/felix-cache
su -p CMCCAdmin && export LD_LIBRARY_PATH=/usr/local/jre/lib/mips && cd /usr/osgi/felix-framework && taskset 0xf /usr/local/jre/bin/evm -XsplitVerify=false -Djava.library.path=/lib -Djava.security.policy=all.policy -Dorg.osgi.framework.security=osgi -jar ./bin/felix.jar &


#/userfs/bin/jvm_monitor &