#!/bin/sh

killall -9 java
killall -9 jvm_monitor 

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
su -p CMCCAdmin && export USER=admin && cd /usr/osgi/felix-framework && taskset 0x8 /usr/local/jre/bin/java -jvm -Djava.security.policy=all.policy -Dorg.osgi.framework.security="osgi" -Djava.library.path=/lib/ -jar bin/felix.jar &

#/userfs/bin/jvm_monitor &