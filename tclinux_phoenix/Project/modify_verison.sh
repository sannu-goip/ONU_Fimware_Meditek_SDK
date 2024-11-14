#!/bin/sh

FILE_PATH=$1

if [[ ${FILE_PATH} =~ "testversion" ]];then
		echo $2 > $1
fi
if [[ ${FILE_PATH} =~ "upgrade_framever" ]];then
		echo $2 > $1
fi
if [ -f $1 ] ; then
	if [[ ${FILE_PATH} =~ "filesystem" ]];then
		sed -i "s/^.*/$2/g" $1
	fi
	if [[ ${FILE_PATH} =~ "deviceParaStatic" ]];then
		sed -i "s/^CustomerSWVersion=.*/CustomerSWVersion=$2/g" $1
	fi
fi



