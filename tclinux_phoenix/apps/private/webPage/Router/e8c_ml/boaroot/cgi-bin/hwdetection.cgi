#!/bin/sh

echo -e "Content-Type: text/html\n"
echo "<html><head><meta http-equiv=Content-Script-Typecontent=text/javascript>
<meta http-equiv=Content-Style-Typecontent=text/css>
<meta http-equiv=Content-Type content=\"text/html;charset=gb2312\"></head><body>"

LanguageSwitch=`/userfs/bin/tcapi get LanguageSwitch_Entry Type`
if [ $LanguageSwitch = 1 ]; then
	interfacestr="接口"
	statusstr="状态"
	successstr="正常"
	failstr="异常"
	schedulestr="正在执行硬件故障检测,请等待检测结果..."
	testresultstr="硬件故障检测结果"
else
	interfacestr="Interface"
	statusstr="Status"
	successstr="Success"
	failstr="Fail"
	schedulestr="Hardware failure detection is in progress, please wait for the result..."
	testresultstr="Hardware failure detection result"
fi

hwstatus=`/userfs/bin/tcapi get Diagnostic_HW status`
if [ $hwstatus = 1 ]; then
	echo "<table cellSpacing=0 cellPadding=0 width='100%' border=0 style='text-align:center'><tbody>
	<tr><td colspan='2'><font size='2'>$schedulestr</font></td></tr></tbody></table>"
elif [ $hwstatus = 2 ]; then
	echo "<table cellSpacing=0 cellPadding=0 width='100%' border=1><tbody>
	<tr><td align=center colspan='2'><font size='2'>$testresultstr</font></td></tr>
	<tr><td align=center><font size='2'>$interfacestr</font></td><td align=center><font size='2'>$statusstr</font></td></tr>"
	i=1
	j=0
	cat /tmp/interface_status | while read LINE
	do
		if [ $i -lt 5 ]; then
			if [ $i = 1 ]; then
				echo "<tr><td><font size='2'>LAN</font></td><td><font size='2'>"
			fi	
			if [ "$LINE" = "$i=On" ]; then
				echo "LAN$i&nbsp$successstr"
			else
				echo"LAN$i&nbsp$failstr"
			fi
			if [ $i -lt 4 ]; then
				echo ",&nbsp"
				if [ $i = 2 ]; then
					echo "<br />"
				fi
			fi
			if [ $i = 4 ]; then
				echo "</font></td></tr>"
			fi
		elif [ $i -lt 9 ]; then
			j=`expr $i - 4`	
			if [ $i = 5 ]; then
				echo "<tr><td><font size='2'>WLAN2.4G</font></td><td><font size='2'>"
			fi	
			if [ "$LINE" = "$i=On" ]; then
				echo "SSID$j&nbsp$successstr"
			else
				echo "SSID$j&nbsp$failstr" 
			fi
			if [ $i -lt 8 ]; then
				echo ",&nbsp"
				if [ $j = 2 ]; then
					echo "<br />"
				fi
			fi
			if [ $i = 8 ]; then
				echo "</font></td></tr>"
			fi
		elif [ $i -lt 13 ]; then
			j=`expr $i - 8`	
			if [ $i = 9 ]; then
				echo "<tr><td><font size='2'>WLAN5G</td><td><font size='2'>"
			fi	
			if [ "$LINE" = "$i=On" ]; then
				echo "SSIDAC$j&nbsp$successstr"
			else
				echo "SSIDAC$j&nbsp$failstr"
			fi
			if [ $i -lt 12 ]; then
				echo ",&nbsp"
				if [ $j = 2 ]; then
					echo "<br />"
				fi
			fi
			if [ $i = 12 ]; then
				echo "</font></td></tr>"
			fi
		elif [ $i -lt 14 ]; then
			echo "<tr><td><font size='2'>USB</td><td><font size='2'>"
			if [ "$LINE" = "$i=On" ]; then
				echo "$successstr"
			else
				echo "$failstr"
			fi
			echo "</font></td></tr>"		
		else
			echo ""	
		fi
		i=`expr $i + 1`	
	done
	echo "</tbody></table>"
/userfs/bin/tcapi set Diagnostic_HW status 0	
else
	echo ""
fi

if [ $hwstatus = 1 ]; then
/userfs/bin/tcapi set Diagnostic_HW status 2
fi

echo "</body></html>"
