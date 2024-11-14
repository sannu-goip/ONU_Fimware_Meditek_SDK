#!/bin/bash

export PATH=$PATH:/opt/trendchip/mipsel-linux-uclibc-4.6.3-kernel3.18/usr/bin/
STRIP=mipsel-buildroot-linux-uclibc-strip
SCAN_DIR1=./lib
SCAN_DIR2=./bin
SCAN_DIR3=./sbin
SCAN_DIR4=./usr/bin
SCAN_DIR5=./usr/sbin
SCAN_DIR6=./userfs/bin

function strip_old()
{
		for DIR in ${SCAN_DIR1} ${SCAN_DIR2} ${SCAN_DIR3} ${SCAN_DIR4} ${SCAN_DIR5} ${SCAN_DIR6}
		do 
		    for FILE in $(ls $DIR)
		    do
		    		if [ -f ${DIR}/${FILE} ]
		    		then
		        	 echo "STRIP $DIR/$FILE"
			     	   $STRIP ${DIR}/${FILE}
			     	 fi
		    done
		done
}

function strip_file()
{
    echo "STRIP ${1}"
	  $STRIP ${1}
}

function strip_dir()
{
	if [ -L $1 ]
	then
		return
	fi
	if [ -f $1 ]
	then
		strip_file $1
	fi
	if [ -d $1 ]
	then
		for FILE in $(ls $1)
		do
			strip_dir $1/${FILE}
		done
	fi
}

# strip_dir ${SCAN_DIR1}
# strip_dir ${SCAN_DIR2}
# strip_dir ${SCAN_DIR3}
# strip_dir ${SCAN_DIR4}
# strip_dir ${SCAN_DIR5}
# strip_dir ${SCAN_DIR6}

strip_old