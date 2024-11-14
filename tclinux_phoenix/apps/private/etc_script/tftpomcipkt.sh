#!/bin/sh

cd 
cd /tmp/
index=$(cat /tmp/currIndex.txt)
echo $index
thr=5
i=1
if [ $index -gt $thr ]; then
	startindex=`expr $index - 4`
	echo $startindex
	while [ $startindex -le $index ] 
	do 
		cat /tmp/omci[0]*$startindex.pcapng >> omci.pcapng
		startindex=`expr $startindex + 1`
	done 
elif [ $index -le $thr ]; then
	while [ $i -le $index ]
	do 
		echo "less than"
		echo $i
		cat /tmp/omci[0]*$i.pcapng >> omci.pcapng
		i=`expr $i + 1`
	done 
else
	echo "end up"
fi
echo "tftp to PC"
tftp -pr omci.pcapng $1
cd  /tmp/
chmod 777 /tmp/omci.pcapng
rm -rf /tmp/omci.pcapng
echo "finsh!"

