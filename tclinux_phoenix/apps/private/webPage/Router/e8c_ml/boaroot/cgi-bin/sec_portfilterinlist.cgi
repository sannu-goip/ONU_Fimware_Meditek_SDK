#!/bin/sh

echo "Content-Type: text/html"
echo ""

echo "<html><head><meta http-equiv=Content-Script-Type content=text/javascript><meta http-equiv=Content-Style-Type
content=text/css><meta http-equiv=Content-Type content=\"text/html; charset=gb2312\"><link rel=\"stylesheet\"
href=\"/JS/stylemain.css\" type=\"text/css\"></head><SCRIPT language=javascript src="/JS/util.js"></SCRIPT><script language=\"JavaScript\">
function formatstr(sourcestr, searchstr)
{
	var restr;
	var offset = sourcestr.indexOf(searchstr);
	if(offset == -1)
		return null;
	var lenstr = searchstr.toString();	
	restr = sourcestr.substring(0, offset) + sourcestr.substring((offset+parseInt(lenstr.length)+1), sourcestr.length);
	return restr;
}
function doDel(i)
{
	var f = parent.document.ConfigForm;
	var tempstr;
	var rmlin = document.getElementsByName('rmlin');
	if (rmlin == null)
		return;
	if(rmlin.length>0)
	{
		for(var n=0;n<(rmlin.length);n++)
		{
			if(rmlin[n].value == i){
				if(rmlin[n].checked)
				{
					tempstr = i + \",\";
					f.delnum.value = f.delnum.value + tempstr;
					
				}
				else
				{
					f.delnum.value = formatstr(f.delnum.value,i);
				}
				break;
			}
		}
	}
}
function stPortFilterIn(domain,Activate,Interface,IPName,Protocol,SrcIPAddr,SrcIPMask,SrcPort,DesIPAddr,DesIPMask,DesPort)
{
this.domain = domain;
this.Activate = Activate;
this.Interface = Interface;
this.IPName = IPName;
this.Protocol = Protocol;
this.SrcIPAddr = SrcIPAddr;
this.SrcIPMask = SrcIPMask;
this.SrcPort = SrcPort;
this.DesIPAddr = DesIPAddr;
this.DesIPMask = DesIPMask;
this.DesPort = DesPort;
}
</script><body topmargin=\"10\" leftmargin=\"0\"><table width=\"450\" align=center cellpadding=\"0\" cellspacing=\"0\">"

F1Str=`/userfs/bin/tcapi get String_Entry PortFilter4Text`
P1Str=`/userfs/bin/tcapi get String_Entry PortFilter5Text`
Ss1Str=`/userfs/bin/tcapi get String_Entry PortFilter6Text`
SP1Str=`/userfs/bin/tcapi get String_Entry PortFilter7Text`
DA1Str=`/userfs/bin/tcapi get String_Entry PortFilter8Text`
DP1Str=`/userfs/bin/tcapi get String_Entry PortFilter9Text`
Enable2Str=`/userfs/bin/tcapi get String_Entry PortFilter10Text`
Interface1Str=`/userfs/bin/tcapi get String_Entry PortFilter12Text`
Delete1Str=`/userfs/bin/tcapi get String_Entry PortFilter27Text`
Enable1Str=`/userfs/bin/tcapi get String_Entry PortFilter2Text`
Disable1Str=`/userfs/bin/tcapi get String_Entry PortFilter3Text`


echo "<tr><td><table width=\"450\" border=\"1\" align=center cellpadding=\"3\" cellspacing=\"0\" bordercolor=\"#CCCCCC\"
bgcolor=\"#FFFFFF\"><tr><TD class=table_title align=middle width=\"10%\"><STRONG>$F1Str</STRONG></TD><td width=\"10%\" align=center
class=\"table_title\"><strong>$P1Str</strong></td><TD class=table_title align=middle
width=\"14%\"><STRONG>$Ss1Str</STRONG></TD><td width=\"12%\" align=center
class=\"table_title\"><strong>$SP1Str</strong></td><td width=\"12%\" align=center
class=\"table_title\"><strong>$DA1Str</strong></td><td width=\"12%\" align=center
class=\"table_title\"><strong>$DP1Str</strong></td><td width=\"10%\" align=center
class=\"table_title\"><strong>$Enable2Str</strong></td><td width=\"8%\" align=center
class=\"table_title\"><strong>$Interface1Str</strong></td><td width=\"12%\" align=center
class=\"table_title\"><strong>$Delete1Str</strong></td></TR></tr>"

i=0
Activate="N/A"
Interface="N/A"
IPName="N/A"
Protocol="N/A"
SrcIPAddr="N/A"
SrcIPMask="N/A"
SrcPort="N/A"
DesIPAddr="N/A"
DesIPMask="N/A"
DesPort="N/A"
SrcIPEndAddr="N/A"
DesIPEndAddr="N/A"
SrcPortEnd="N/A"
DesPortEnd="N/A"
isCMCCSupported=`/userfs/bin/tcapi get WebCustom_Entry isCMCCSupported`
cat /tmp/ipdownvalid | while read LINE
do
	k=`expr $i % 11`
	if [ "$isCMCCSupported" = "Yes" ] 
	then
		k=`expr $i % 15`	
	fi	
	if [ $k = 0 ]
	then
		iEntry=$LINE
	elif [ $k = 1 ]
	then
		Activate=$LINE
		if [ "$Activate" = "Yes" ] 
  		then
  			Activate="$Enable1Str"
		elif [ "$Activate" = "No" ] 
		then
			Activate="$Disable1Str"
		else  Activate="Wrong setting"
		fi
	elif [ $k = 2 ]
	then
		Interface=$LINE
	elif [ $k = 3 ]
	then
		IPName=$LINE
	elif [ $k = 4 ]
	then
		Protocol=$LINE
	elif [ $k = 5 ]
	then
		SrcIPAddr=$LINE
	elif [ $k = 6 ]
	then
		SrcIPMask=$LINE
	elif [ $k = 7 ]
	then
		SrcPort=$LINE
	elif [ $k = 8 ]
	then
		DesIPAddr=$LINE
	elif [ $k = 9 ]
	then
		DesIPMask=$LINE
	elif [ $k = 10 ]
	then
		DesPort=$LINE
		if [ "$isCMCCSupported" = "Yes" ] 
		then
			echo ""
		else
			echo "<script language=\"javascript\">
			if(parent.portFilterInIndex != null)
			{
				parent.portFilterIn[parent.portFilterInIndex] = new stPortFilterIn(\"domain\", \"$Activate\", \"$Interface\", \"$IPName\", \"$Protocol\", \"$SrcIPAddr\", \"$SrcIPMask\", \"$SrcPort\", \"$DesIPAddr\", \"$DesIPMask\", \"$DesPort\");
				parent.portFilterInIndex++;
			}
			</script>
			<tr align=\"middle\"><td width=\"10%\">$IPName</td><td width=\"10%\">$Protocol</td><td width=\"14%\">$SrcIPAddr "/" $SrcIPMask</td><td
			width=\"12%\">$SrcPort</td><td width=\"12%\">$DesIPAddr "/" $DesIPMask</td><td width=\"12%\">$DesPort</td><td
			width=\"10%\">$Activate</td><td width=\"8%\">$Interface</td><td width=\"12%\"><input type=\"checkbox\" name=\"rmlin\" onclick=\"doDel($iEntry);\" value=\"$iEntry\"></td></tr>"
		fi
	elif [ $k = 11 ]
	then
		srcIPEndAddr=$LINE
	elif [ $k = 12 ]
	then
		DesIPEndAddr=$LINE
	elif [ $k = 13 ]
	then
		SrcPortEnd=$LINE
	elif [ $k = 14 ]
	then
		DesPortEnd=$LINE
			echo "<script language=\"javascript\">
			if(parent.portFilterInIndex != null)
			{
				parent.portFilterIn[parent.portFilterInIndex] = new stPortFilterIn(\"domain\", \"$Activate\", \"$Interface\", \"$IPName\", \"$Protocol\", \"$SrcIPAddr\", \"$SrcIPMask\", \"$SrcPort\", \"$DesIPAddr\", \"$DesIPMask\", \"$DesPort\");
				parent.portFilterInIndex++;
			}
			</script>
			<tr align=\"middle\"><td width=\"10%\">$IPName</td><td width=\"10%\">$Protocol</td><td width=\"14%\">$SrcIPAddr "-" $srcIPEndAddr</td><td
			width=\"12%\">$SrcPort ":" $SrcPortEnd</td><td width=\"12%\">$DesIPAddr "-" $DesIPEndAddr</td><td width=\"12%\">$DesPort ":" $DesPortEnd</td><td
			width=\"10%\">$Activate</td><td width=\"8%\">$Interface</td><td width=\"12%\"><input type=\"checkbox\" name=\"rmlin\" onclick=\"doDel($iEntry);\" value=\"$iEntry\"></td></tr>"		
	fi
	i=`expr $i + 1`
done
rm -f /tmp/ipdownvalid
echo "</table></td></tr></table></body></html>"

