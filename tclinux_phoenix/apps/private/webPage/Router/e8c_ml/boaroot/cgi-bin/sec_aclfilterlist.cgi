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
	var rml = document.getElementsByName('rml');
	if (rml == null)
		return;
	if(rml.length>0)
	{
		for(var n=0;n<(rml.length);n++)
		{
			if(rml[n].value == i){
				if(rml[n].checked)
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

function stAclFilter(EntryNum,Name,ScrBeginIP,ScrEndIP,Interface,Application, Enable)
{
	this.EntryNum = EntryNum;
	this.Name = Name;
	this.ScrBeginIP = ScrBeginIP;
	this.ScrEndIP = ScrEndIP;
	this.Interface = Interface;
	this.Application = Application;
	this.Enable = Enable;
}

function doEdit(EntryNum,Name,ScrBeginIP,ScrEndIP,Interface,Application, Enable)
{
		top.AclEditNumber = EntryNum;
		top.AclFilterEditRule[0] = new stAclFilter(EntryNum, Name, ScrBeginIP, ScrEndIP, Interface, Application, Enable);
		parent.location.replace('sec-addaclfilter.asp');			
}

</script>
<body topmargin=\"10\" leftmargin=\"0\"><table width=\"100%\" align=center cellpadding=\"0\" cellspacing=\"0\">"

RuleNameStr=`/userfs/bin/tcapi get String_Entry ACLFilterNameTitleText`
ScrBeginIPStr=`/userfs/bin/tcapi get String_Entry ACLFilterScrBeginIPTitleText`
ScrEndIPStr=`/userfs/bin/tcapi get String_Entry ACLFilterScrEndIPTitleText`
InterfaceStr=`/userfs/bin/tcapi get String_Entry ACLFilterInterfaceTitleText`
AppStr=`/userfs/bin/tcapi get String_Entry ACLFilterAppTitleText`
EnableTitleStr=`/userfs/bin/tcapi get String_Entry ACLFilterEnableText`
EnableStr=`/userfs/bin/tcapi get String_Entry ACLFilterEnableStatusText`
DisableStr=`/userfs/bin/tcapi get String_Entry ACLFilterDisableStatusText`
DeleteStr=`/userfs/bin/tcapi get String_Entry ACLFilterDeleteTitleText`
EditStr=`/userfs/bin/tcapi get String_Entry ACLFilterEditText`

echo "<tr><td><table width=\"100%\" border=\"1\" align=center cellpadding=\"3\" cellspacing=\"0\" bordercolor=\"#CCCCCC\"
bgcolor=\"#FFFFFF\"><tr>
<TD class=\"table_title\" width=\"35%\" align=middle ><STRONG>$RuleNameStr</STRONG></TD>
<td class=\"table_title\" width=\"30%\" align=center ><strong><FONT color=#000000>$ScrBeginIPStr</FONT></strong></td>
<TD class=\"table_title\" width=\"20%\" align=middle ><STRONG><FONT color=#000000>$ScrEndIPStr</FONT></STRONG></TD>
<TD class=\"table_title\" width=\"20%\" align=middle ><STRONG><FONT color=#000000>$InterfaceStr</FONT></STRONG></TD>
<TD class=\"table_title\" width=\"20%\" align=middle ><STRONG><FONT color=#000000>$AppStr</FONT></STRONG></TD>
<TD class=\"table_title\" width=\"20%\" align=middle ><STRONG><FONT color=#000000>$EnableTitleStr</FONT></STRONG></TD>
<TD class=\"table_title\" width=\"20%\" align=middle ><STRONG><FONT color=#000000>$EditStr</FONT></STRONG></TD>
<td class=\"table_title\" width=\"15%\" align=center ><strong><FONT color=#000000>$DeleteStr</FONT></strong></td>
</TR></tr>"

i=0
iEntry=0
AclRuleName="N/A"
ScrBeginIP="N/A"
ScrEndIP="N/A"
Interface="N/A"
Application="N/A"
Activate="N/A"

cat /tmp/aclvalid | while read LINE
do
	k=`expr $i % 7`
	if [ $k = 0 ]
	then
		iEntry=$LINE
	elif [ $k = 1 ]
	then
		AclRuleName=$LINE
	elif [ $k = 2 ]
	then
		ScrBeginIP=$LINE
	elif [ $k = 3 ]
	then
		ScrEndIP=$LINE
	elif [ $k = 4 ]
	then
		Interface=$LINE
	elif [ $k = 5 ]
	then
		Application=$LINE
	elif [ $k = 6 ]
	then
		Activate=$LINE
		if [ "$Activate" = "Yes" ] 
  		then
  			Activate="$EnableStr"
		elif [ "$Activate" = "No" ] 
		then
			Activate="$DisableStr"
		else  Activate="Wrong setting"
		fi
		echo "<script language=\"javascript\">
		if(parent.AclFilterRule != null)
		{
			parent.AclFilterRule[parent.AclFilterRuleIndex] = new stAclFilter(\"$iEntry\", \"$AclRuleName\", \"$ScrBeginIP\", \"$ScrEndIP\", \"$Interface\", \"$Application\", \"$Activate\");
			parent.AclFilterRuleIndex++;
		}
		</script>
		<tr align=\"middle\">
		<td width=\"35%\">$AclRuleName</td>
		<td width=\"30%\">$ScrBeginIP</td>
		<td width=\"20%\">$ScrEndIP</td>
		<td width=\"20%\">$Interface</td>
		<td width=\"20%\">$Application</td>
		<td width=\"20%\">$Activate</td>
		<td width=\"15%\"><input type=\"button\" id=\"edit\" name=\"edit\" onclick='doEdit(\"$iEntry\",\"$AclRuleName\",\"$ScrBeginIP\",\"$ScrEndIP\",\"$Interface\",\"$Application\",\"$Activate\");' value=\"$EditStr\"></td>
		<td width=\"15%\"><input type=\"checkbox\" id=\"rml\" name=\"rml\" onclick=\"doDel($iEntry);\" value=\"$iEntry\"></td>
		</tr>"
	fi
	i=`expr $i + 1`
done
rm -f /tmp/aclvalid
echo "</table></td></tr></table></body></html>"

