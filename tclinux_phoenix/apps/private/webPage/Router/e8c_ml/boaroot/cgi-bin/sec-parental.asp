<%
if Request_Form("Commit_Flag") = "1" then
	TCWebApi_set("Parental_Common","Active","rightFlag")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="2" then
	TCWebApi_set("Parental_Entry","Active","rightFlag")
	TCWebApi_set("Parental_Entry","Policy","policyVal")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="3" then
	TCWebApi_set("Parental_Entry","DurationActive","rightFlag")
	TCWebApi_set("Parental_Entry","DurationPolicy","policyVal")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="4" then
	TCWebApi_set("WebCurSet_Entry","parental_id","ParentalID_Flag")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="5" then
	TCWebApi_set("Parental_Common","AddIndx","addIdx")
	TCWebApi_set("Parental_Entry","MAC","MACval")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="6" then
	TCWebApi_set("Parental_Common","AddIndx","addIdx")
	TCWebApi_set("Parental_Entry","URL","urlitem")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="7" then
	TCWebApi_set("Parental_Common","AddIndx","addIdx")
	TCWebApi_set("Parental_Entry","StartTime","startTime")
	TCWebApi_set("Parental_Entry","EndTime","endTime")
	TCWebApi_set("Parental_Entry","RepeatDay","repeatDay")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="8" then
	TCWebApi_set("Parental_Common","DelIdx","delnum")
	TCWebApi_set("Parental_Entry","DelFlag","delFlag")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="9" then
	TCWebApi_set("Parental_Entry","Policy","policyVal")
	TCWebApi_commit("Parental")
	TCWebApi_save()
elseif Request_Form("Commit_Flag") ="10" then
	TCWebApi_set("Parental_Entry","DurationPolicy","policyVal")
	TCWebApi_commit("Parental")
	TCWebApi_save()
end if	
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","ContentParentalText","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<script language="JavaScript" type="text/JavaScript">
var	vMacList = new Array(4);
var	vUrlList = new Array(10);
var vStarTime = new Array(4);
var vEndTime = new Array(4);
var vRepeatDay = new Array(4);
var url_right = "<% tcWebApi_get("Parental_Entry","Policy","s") %>";
var duration_right = "<% tcWebApi_get("Parental_Entry","DurationPolicy","s") %>";
vMacList[0] = "<% tcWebApi_get("Parental_Entry","MAC0","s") %>";
vMacList[1] = "<% tcWebApi_get("Parental_Entry","MAC1","s") %>";
vMacList[2] = "<% tcWebApi_get("Parental_Entry","MAC2","s") %>";
vMacList[3] = "<% tcWebApi_get("Parental_Entry","MAC3","s") %>";

vUrlList[0] = "<% tcWebApi_get("Parental_Entry","URL0","s") %>";
vUrlList[1] = "<% tcWebApi_get("Parental_Entry","URL1","s") %>";
vUrlList[2] = "<% tcWebApi_get("Parental_Entry","URL2","s") %>";
vUrlList[3] = "<% tcWebApi_get("Parental_Entry","URL3","s") %>";
vUrlList[4] = "<% tcWebApi_get("Parental_Entry","URL4","s") %>";
vUrlList[5] = "<% tcWebApi_get("Parental_Entry","URL5","s") %>";
vUrlList[6] = "<% tcWebApi_get("Parental_Entry","URL6","s") %>";
vUrlList[7] = "<% tcWebApi_get("Parental_Entry","URL7","s") %>";
vUrlList[8] = "<% tcWebApi_get("Parental_Entry","URL8","s") %>";
vUrlList[9] = "<% tcWebApi_get("Parental_Entry","URL9","s") %>";

vStarTime[0] = "<% tcWebApi_get("Parental_Entry","StartTime0","s") %>";
vStarTime[1] = "<% tcWebApi_get("Parental_Entry","StartTime1","s") %>";
vStarTime[2] = "<% tcWebApi_get("Parental_Entry","StartTime2","s") %>";
vStarTime[3] = "<% tcWebApi_get("Parental_Entry","StartTime3","s") %>";
vEndTime[0] = "<% tcWebApi_get("Parental_Entry","EndTime0","s") %>";
vEndTime[1] = "<% tcWebApi_get("Parental_Entry","EndTime1","s") %>";
vEndTime[2] = "<% tcWebApi_get("Parental_Entry","EndTime2","s") %>";
vEndTime[3] = "<% tcWebApi_get("Parental_Entry","EndTime3","s") %>";
vRepeatDay[0] = "<% tcWebApi_get("Parental_Entry","RepeatDay0","s") %>";
vRepeatDay[1] = "<% tcWebApi_get("Parental_Entry","RepeatDay1","s") %>";
vRepeatDay[2] = "<% tcWebApi_get("Parental_Entry","RepeatDay2","s") %>";
vRepeatDay[3] = "<% tcWebApi_get("Parental_Entry","RepeatDay3","s") %>";

function LoadFrame()
{	
	var enbl_p = "<% tcWebApi_get("Parental_Common","Active","s") %>";
	var enbl_u = "<% tcWebApi_get("Parental_Entry","Active","s") %>";
	var enbl_d = "<% tcWebApi_get("Parental_Entry","DurationActive","s") %>";
	var tmp_id = "<% tcWebApi_get("WebCurSet_Entry","parental_id","s") %>";

	//setCheck('isFilter',enbl_p);
	if(enbl_p == "Yes"){
		setDisplay("Parental",1);				
	}else{
		setDisplay("Parental",0);
	}
	if(tmp_id == "0"){
		setDisplay("only_macFilter",0);
	}else{
		setDisplay("only_macFilter",1);
	}
	if(enbl_u == "Yes"){
		setDisplay("url_right",1);				
	}else{
		setDisplay("url_right",0);
	}
	if(enbl_d == "Yes"){
		setDisplay("d_right",1);				
	}else{
		setDisplay("d_right",0);
	}
}
function ParentalIDChange(val)
{
	document.ParentalForm.Commit_Flag.value = "4";
	document.ParentalForm.ParentalID_Flag.value = val;
	if( true == setEBooValueCookie(document.ParentalForm))
		document.ParentalForm.submit();
}
function get_weekDay()
{
	var tempstr = "";
	var rml = document.getElementsByName("d_w");
	if (rml == null)
		return -1;
	if(rml.length>0)
	{
		for(var n=0;n<(rml.length);n++)
		{		
				if(rml[n].checked)
				{
					tempstr =tempstr+ rml[n].value;				
				}			
		}
		if(tempstr.length < 1)
			return -1;
		return tempstr;		
	}
}
function selRight(filter)
{
	var id_name;
	if(filter.id == "isFilter"){
		id_name = "Parental";
		document.ParentalForm.Commit_Flag.value = "1";
	}else if(filter.id == "URLRight")
	{
		id_name = "url_right";
		if(url_right == "N/A"){
			url_right = "Black";
		}
		document.ParentalForm.policyVal.value = url_right;
		document.ParentalForm.Commit_Flag.value = "2";
	}else if(filter.id == "DRight")
	{
		if(duration_right == "N/A"){
			duration_right = "Black";
		}
		id_name = "d_right";
		document.ParentalForm.policyVal.value = duration_right;
		document.ParentalForm.Commit_Flag.value = "3";
	}else{
		return ;
	}	
		if (filter.checked)
		{
			setDisplay(id_name,1);
			document.ParentalForm.rightFlag.value = "Yes";
		}
		else
		{
			setDisplay(id_name,0);
			document.ParentalForm.rightFlag.value = "No";
		}
		if( true == setEBooValueCookie(document.ParentalForm))
			document.ParentalForm.submit();
}
function doHexCheck(c)
{
  if ((c >= "0")&&(c <= "9"))
  {
    return 1;
  }
  else if ((c >= "A")&&(c <= "F"))
  {
    return 1;
  }
  else if ((c >= "a")&&(c <= "f"))
  {
    return 1;
  }

  return -1;
}
function doMACcheck(val)
{
  var szAddr = val;
  var len = szAddr.length;
	var i = 0;

  if ( len == 0 )
  {
		alert("<%tcWebApi_get("String_Entry","ParentalInvalidMacText","s")%>");
    return -1;
  }

	for(i = 0; i < vMacList.length; i++)
  {
		if(val == vMacList[i])
    {
			alert( val + "<%tcWebApi_get("String_Entry","ParentalIsExistText","s")%>");
        return -1;
      }
    }

  if ( len == 17 )
  {
    var i = 2;
    var c0 = szAddr.charAt(0);
    var c1 = szAddr.charAt(1);
		var strArray =val.split(":"); 
		var j = 0 ,sum = 0;
		for(j = 0; j< strArray.length; j++){
			if(strArray[j] == "00")
				sum ++;
		}
    if ( (doHexCheck(c0) < 0) || (doHexCheck(c1) < 0) || sum == 6 )
    {
			alert("<%tcWebApi_get("String_Entry","ParentalInvalidMacText","s")%>");
			return -1;
    }
    
    i = 2;
    while ( i < len )
    {
      var c0 = szAddr.charAt(i);
      var c1 = szAddr.charAt(i+1);
      var c2 = szAddr.charAt(i+2);  
      if ( (c0 != ":") || (doHexCheck(c1) < 0) || (doHexCheck(c2) < 0) )
      {
		alert("<%tcWebApi_get("String_Entry","ParentalInvalidMacText","s")%>");        	
				return -1;
      }
      i = i + 3;
    }
    return 0; 
  }
  else
  {
		alert("<%tcWebApi_get("String_Entry","ParentalInvalidMacText","s")%>");     	
		return -1;
  }
}

var firstNULLmac = -1;
var firstNULLurl = -1;
var firstNULLduration = -1;

function parseTime(val)
{
	var h = parseInt(val/3600);
	var m = parseInt(val%3600/60);
	var str;
	if(h < 10)
		str = "0"+h;
	else
		str = h;
	if(m < 10)
		str = str + ":0" + m;
	else
		str = str + ":" + m;
	return str;
}

function doDel_p(dobj)
{
	var tempstr = "";
	var rml = document.getElementsByName(dobj.name);
	if (rml == null)
		return;
	if(rml.length>0)
	{
		for(var n=0;n<(rml.length);n++)
		{		
				if(rml[n].checked)
				{
					tempstr =tempstr+ rml[n].value + ",";				
				}			
		}
		document.ParentalForm.delnum.value = tempstr;		
	}
}

function isValidUrlName(url)
{
	var i=0;
	var invalidArray = new Array();
	invalidArray[i++] = "www";
	invalidArray[i++] = "com";
	invalidArray[i++] = "org";
	invalidArray[i++] = "net";
	invalidArray[i++] = "edu";
	invalidArray[i++] = "www.";
	invalidArray[i++] = ".com";
	invalidArray[i++] = ".org";
	invalidArray[i++] = ".net";
	invalidArray[i++] = ".edu";
	if (isValidAscii(url) != '')
	{
		return false;
	}

	if (isValidUrl(url) == false)	
	{
		return false;
	}
	
	for (i = 0; i < url.length; i++)
		{
		if (url.charAt(i) == '\\')
		{
			return false;
		}
	}
	if (url == "")
	{
		return false;
	}
	if (url.length < 3)
	{
		return false;
	}
	for(j=0; j< invalidArray.length; j++)
	{
		if (url == invalidArray[j])
		{
			return false;
		}
	}
	return true;
}

function checkUrl(val)
{
	var i = 0;
	if(val.length == 0)
	{
		alert("<%tcWebApi_get("String_Entry","ParentalUrlIsNullText","s")%>");
		return -1;
	}
	for(i = 0; i < vUrlList.length; i++)
	{
		if(val.toLowerCase() == vUrlList[i].toLowerCase())
		{
			alert( val + "<%tcWebApi_get("String_Entry","ParentalIsExistText","s")%>");
			return -1;
		}
	}
	if (val.length > 31)
		{
			alert("<%tcWebApi_get("String_Entry","WanSetting3Text","s")%>");
			return -1;
		}
		if(isValidUrlName(val) == false)
		{
			alert("<%tcWebApi_get("String_Entry","WanSetting4Text","s")%>");
			return -1;
		}
		
	return 0;
}
function formatRepeatDay(val)
{
	var i = 0;
	var bit = 0;
	var str = "";
	for(i = 1; i < 8; i++)
	{
		if(i == 7)
			bit = 0;
		else
			bit = i;
		if(parseInt(val) & 1 << parseInt(bit))
		{
			if(str.length == 0)
			 str = i;
			else
			 str = str + "," + i;
		}
	}
	return str;
}
function CreateList(num,str)
{
	var i = 0;
	var list_idx = 1;
	for (i = 0; i < num; i++)
	{	
		if(str == "macList")
		{
			if(vMacList[i] == "N/A" || vMacList[i].length <= 0)
			{
				if(firstNULLmac == -1)
					firstNULLmac = i;
				continue;
			}
			document.write('<tr><td align=middle width=60>'+list_idx+'</td>');
			document.write('<td width=180>'+vMacList[i]+'</td>');
			document.write('<td align=middle width=60><input type="checkbox" name="rmmac"  onclick="doDel_p(this);" value='+i+'></td></tr>');
			list_idx ++;	
		}else if(str == "url"){
			if(vUrlList[i] == "N/A" || vUrlList[i].length <= 0)
			{ 
				if( firstNULLurl == -1)
						firstNULLurl = i;	
					continue;		
			}
			document.write('<tr><td align=middle width=60>'+list_idx+'</td>');
			document.write('<td width=180>'+vUrlList[i]+'</td>');
			document.write('<td align=middle width=60><input type="checkbox" name="rmurl" onclick="doDel_p(this);" value='+i+'></td></tr>');
			list_idx ++;	
		}else if(str == "duration"){
			if(vStarTime[i] == "N/A" || vStarTime[i].length <= 0) {
				if( firstNULLduration == -1)
						firstNULLduration = i;
					continue;
			}			
			document.write('<tr><td align=middle width=60>'+list_idx+'</td>');
			document.write('<td width=150>'+parseTime(vStarTime[i])+'</td>');
			document.write('<td width=150>'+parseTime(vEndTime[i])+'</td>');
			document.write('<td width=150>'+formatRepeatDay(vRepeatDay[i])+'</td>');
			document.write('<td align=middle width=60><input type="checkbox" name="rmdr" onclick="doDel_p(this);" value='+i+'></td></tr>');	
			list_idx ++;		
		}	
	}
}
function getSec(t_value)
{
	var timeval = new Array(2);

	if(t_value.split(":").length != 2)
	{
		return -1;
	}
	
	timeval[0] = t_value.split(":")[0];
	timeval[1] = t_value.split(":")[1];

	if(timeval[1].length != 2 || timeval[0] >= 24 || timeval[1] >59)	
	{
		return -1;
	}
	
	return timeval[0]*3600 + timeval[1]*60;
}
function btnAdd(addType)
{
 if(addType.id == "macAdd")
 {
 	if(firstNULLmac == -1){
 		alert("<%tcWebApi_get("String_Entry","ParentalMaxMacNumText","s")%>");		
 		return;
	}
	if(doMACcheck(document.ParentalForm.macitem.value) == -1)
		return ;
 	document.ParentalForm.Commit_Flag.value = "5";
 	document.ParentalForm.MACval.value = document.ParentalForm.macitem.value;
 	document.ParentalForm.addIdx.value = firstNULLmac;
 }else if(addType.id == "urlAdd"){
 	 	if(firstNULLurl == -1)
 	 	{
	 		alert("<%tcWebApi_get("String_Entry","ParentalMaxUrlNumText","s")%>");	
	 		return;
		}
		if(checkUrl(document.ParentalForm.urlitem.value) == -1)
			return;
	document.ParentalForm.addIdx.value = firstNULLurl;
	document.ParentalForm.urlVal.value = document.ParentalForm.urlitem.value;
	document.ParentalForm.Commit_Flag.value = "6";
 	}else if(addType.id == "durationAdd"){
	 	if(firstNULLduration == -1){
	 		alert("<%tcWebApi_get("String_Entry","ParentalMaxTimeDurationText","s")%>");
	 		return;
	 }
	 var sT = getSec(document.ParentalForm.ds_item.value);
	 var eT = getSec(document.ParentalForm.de_item.value);
	 var wT = get_weekDay();
	 
	 if(sT < 0 || eT < 0)
	 {
		alert("<%tcWebApi_get("String_Entry","ParentalInvalidTimeText","s")%>");
	 	return;
	 }
	 
	 if(sT >= eT)
	 {
		alert("<%tcWebApi_get("String_Entry","ParentalStartEndTimeText","s")%>");
	 	return;
	 }
	
	 if(wT < 0)
	 {
		alert("<%tcWebApi_get("String_Entry","ParentalcheckWeekText","s")%>");
	 	return;
	 }
	 
	 document.ParentalForm.Commit_Flag.value = "7";
	 document.ParentalForm.addIdx.value = firstNULLduration;
	 document.ParentalForm.startTime.value = sT;
	 document.ParentalForm.endTime.value = eT;
	 document.ParentalForm.repeatDay.value = wT;
	}
	if( true == setEBooValueCookie(document.ParentalForm))
		document.ParentalForm.submit();
}
function btnRemove(delType)
{
	document.ParentalForm.Commit_Flag.value = "8";
	document.ParentalForm.delFlag.value = delType.id;
	if( true == setEBooValueCookie(document.ParentalForm))
		document.ParentalForm.submit();
}
function ChangePolicy(policyType)
{
	if(policyType.name == "u_policy"){
		document.ParentalForm.Commit_Flag.value = "9";
	}
	else if(policyType.name == "d_policy"){
		document.ParentalForm.Commit_Flag.value = "10";
	}else{
		return;
	}
	document.ParentalForm.policyVal.value = policyType.value;
	if( true == setEBooValueCookie(document.ParentalForm))
		document.ParentalForm.submit();
}
</script>
<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ParentalForm') != null)LoadFrame()" 
onunload=DoUnload() marginheight="0" marginwidth="0">
<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center 
border=0>
  <TBODY>
  <TR>
    <TD height=1>
<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
      <TABLE height=80 cellSpacing=0 cellPadding=0 width=808 
      background=/img/framelogo.jpg border=0>
<% else %>
<% if tcwebApi_get("WebCustom_Entry","isCTPONTYLOGOSupported","h") = "Yes" then %>
      <TABLE height=80 cellSpacing=0 cellPadding=0 width=808 
      background=/img/framelogo.jpg border=0>
<% else %>
      <TABLE height=90 cellSpacing=0 cellPadding=0 width=808 
      background=/img/framelogo.jpg border=0>
<% end if %> 
<% end if %>
        <TBODY>
        <TR>
          <TD>&nbsp;</TD>
          <TD vAlign=bottom align=right width=358>
            <TABLE id=table8 cellSpacing=0 cellPadding=0 border=0>
              <TBODY>
              <TR>
                <TD vAlign=bottom align=right><SPAN class=curUserName>&nbsp; 
                  </SPAN></TD>
                <TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%> </TD>
                <TD vAlign=bottom width=50><A onclick=DoLogout() 
                  href="/cgi-bin/login.asp" id=logOut name=logOut target=_top><SPAN 
                  class=logout><%tcWebApi_get("String_Entry","LogoutText","s")%> 
      </SPAN></A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
      <TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 
border=0>
        <TBODY>
        <TR>
          <TD class=LocationDisplay id=LocationDisplay align=middle width=163 
          bgColor=#ef8218 rowSpan=3></TD>
          <TD width=434 bgColor=#427594 height=33>
            <P align=right><FONT face=黑体 color=#ffffff><B><FONT face=黑体 
            color=#ffffff size=6><INPUT id=Selected_Menu type=hidden value="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>-><%tcWebApi_get("String_Entry","ContentParentalText","s")%>" name="Selected_Menu"> </FONT></B><SPAN 
            class=GatewayName><%tcWebApi_get("String_Entry","GatewayNameText","s")%>
            <SCRIPT 
            language=javascript>
document.write(top.gateWayName);
</SCRIPT>
             </SPAN></FONT></P></TD>
          <TD width=211 bgColor=#ef8218 height=33>
            <P class=GatewayType align=center><%tcWebApi_get("String_Entry","ModeText","s")%>
            <SCRIPT language=javascript>
document.write(top.ModelName);
</SCRIPT>
             </P></TD></TR>
        <TR>
          <TD id=MenuArea_L1 vAlign=bottom bgColor=#ef8218 colSpan=2 
          height=43>&nbsp;</TD></TR>
        <TR>
          <TD id=MenuArea_L2 bgColor=#427594 colSpan=2 
      height=24></TD></TR></TBODY></TABLE>
      <SCRIPT 
      language=javascript>
MakeMenu(getElById ('Selected_Menu').value);
</SCRIPT>

      <TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD height=15><IMG height=15 src="/img/panel1.gif" 
            width=164 border=0> </TD>
          <TD><IMG height=15 src="/img/panel2.gif" width=645 
            border=0> </TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD vAlign=top>
      <SCRIPT language=JavaScript src="/JS/printbox.js"></SCRIPT>
	  
      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","ContentParentalText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
          rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#家长控制" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0></A></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
                  </TBODY></TABLE>　 </TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <FORM name="ParentalForm" action="/cgi-bin/sec-parental.asp" method="post"> 
            	 <input type="hidden" id="rightFlag" name="rightFlag" value="0">
            	 <input type="hidden" id="Commit_Flag" name="Commit_Flag" value="0">
            	 <input type="hidden" id="MACval" name="MACval" value="0">
            	 <input type="hidden" id="startTime" name="startTime" value="0">
            	 <input type="hidden" id="endTime" name="endTime" value="0">
            	 <input type="hidden" id="repeatDay" name="repeatDay" value="1">
            	 <input type="hidden" id="ParentalID_Flag" name="ParentalID_Flag" value="0">
            	 <input type="hidden" id="urlVal" name="urlVal" value="0">
            	 <input type="hidden" id="delnum" name="delnum" value="0">
            	 <input type="hidden" id="addIdx" name="addIdx" value="0">
            	 <input type="hidden" id="delFlag" name="delFlag" value="0">
            	 <input type="hidden" id="policyVal" name="policyVal" value="0">
            	<DIV style="margin-left:5%">
            		<DIV style="margin-right:5%">
            	<DIV>      
            	<%tcWebApi_get("String_Entry","ParentalEnableText","s")%>&nbsp;&nbsp;<INPUT id=isFilter onclick="selRight(this)" type="checkbox" name="isFilter" <%if tcWebApi_get("Parental_Common","Active","h") = "Yes" then asp_Write("checked") end if%>>  
            	</DIV>           	       	
            	<DIV id=Parental name=Parental style="display:none">
            		<br/> 
            		<hr/> 
            		<br/> 
            		<DIV>
            		&nbsp;<%tcWebApi_get("String_Entry","ParentalTipsText","s")%> &nbsp;
            	</DIV>          		
            	<br/> 
            	<%tcWebApi_get("String_Entry","ParentalIDText","s")%><select onChange=ParentalIDChange(this.options[this.options.selectedIndex].value) name=ParentalID id='ParentalID' >
            				<option value="0" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "0" then asp_Write("selected") end if%>>1</option>
										<option value="1" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "1" then asp_Write("selected") end if%>>2</option>
										<option value="2" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "2" then asp_Write("selected") end if%>>3</option>
										<option value="3" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "3" then asp_Write("selected") end if%>>4</option>
										<option value="4" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "4" then asp_Write("selected") end if%>>5</option>
										<option value="5" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "5" then asp_Write("selected") end if%>>6</option>
										<option value="6" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "6" then asp_Write("selected") end if%>>7</option>
										<option value="7" <%if tcWebApi_get("WebCurSet_Entry","parental_id","h") = "7" then asp_Write("selected") end if%>>8</option>
										</select>         				
							<br/>	
							<hr/>				
							<br/>				
            	<DIV>
            		<%tcWebApi_get("String_Entry","ParentalMACText","s")%>:&nbsp;&nbsp; <INPUT onkeydown="if(event.keyCode==13)return false" maxLength=31 size=30 name="macitem" id="macitem"> 
            		   	<INPUT onclick="btnAdd(this)" id="macAdd" name="macAdd" type=button value=" <%tcWebApi_get("String_Entry","ParentalADDText","s")%> ">           		
            	</DIV>
            	<br/>
            		<TABLE width="90%" border="1" align=center cellpadding="3" cellspacing="0" bordercolor="#CCCCCC" bgcolor="#FFFFFF">
            			<TR>
            				<TD class=table_title align=middle ><STRONG><%tcWebApi_get("String_Entry","ParentalListIdxText","s")%></STRONG></TD>
      						<TD width=180 align=center class="table_title"><strong><FONT color="#000000"><%tcWebApi_get("String_Entry","ParentalMACText","s")%></FONT></strong></TD>
      						<TD class=table_title align=middle ><STRONG><FONT color=#000000><%tcWebApi_get("String_Entry","ParentalDELText","s")%></FONT></STRONG></TD>  
            		</TR>
            			<script language=JavaScript type=text/javascript>
							 	 			CreateList(4,"macList")
										</script>							
            	</TABLE>
            	<br/>
            	<DIV style="margin-left:5%">
									<INPUT onclick='btnRemove(this)' id="delMAC" name="delMAC" type=button value=" <%tcWebApi_get("String_Entry","ParentalDELText","s")%> "> 
							</DIV>
							<DIV id=only_macFilter style="display:none">
            	<br/>
            	<hr/>
            	<br/>
            	
            	<DIV>
            		<P><%tcWebApi_get("String_Entry","ParentalURLRightText","s")%> <INPUT id=URLRight onclick="selRight(this)" type="checkbox" name="URLRight" <%if tcWebApi_get("Parental_Entry","Active","h") = "Yes" then asp_Write("checked") end if%>></P>
            	</DIV>
            		   <br/>       		
            		<DIV id=url_right>
            			<DIV>
            		<%tcWebApi_get("String_Entry","ParentalURLPolicyText","s")%> <font color="#000000"><%tcWebApi_get("String_Entry","ParentalBlackText","s")%></font><INPUT onclick="ChangePolicy(this)" TYPE="radio"  NAME="u_policy"  VALUE="Black" <%if tcWebApi_get("Parental_Entry","Policy","h") = "Black" then asp_Write("checked")  end if%>>
										<font color="#000000"><%tcWebApi_get("String_Entry","ParentalWhiteText","s")%></font><INPUT onclick="ChangePolicy(this)" TYPE="radio" NAME="u_policy"  VALUE="White" <%if tcWebApi_get("Parental_Entry","Policy","h") = "White" then asp_Write("checked")  end if%>>          
            		</DIV>
            		<br/>
            		<DIV>
            			<%tcWebApi_get("String_Entry","ParentalURLText","s")%>:&nbsp;&nbsp;<INPUT onkeydown="if(event.keyCode==13)return false" maxLength=31 size=30 name="urlitem" id="urlitem">
            				<INPUT onclick="btnAdd(this)" id="urlAdd" name="urlAdd" type=button value=" <%tcWebApi_get("String_Entry","ParentalADDText","s")%> "> 
            		</DIV>
            		<br/> 
            		<TABLE width="90%" border="1" align=center cellpadding="3" cellspacing="0" bordercolor="#CCCCCC" bgcolor="#FFFFFF">
            		<TR> 		     	
      						<TD class=table_title align=middle ><STRONG><%tcWebApi_get("String_Entry","ParentalListIdxText","s")%></STRONG></TD>
      						<TD width=180 align=center class="table_title"><strong><FONT color="#000000"><%tcWebApi_get("String_Entry","ParentalURLText","s")%></FONT></strong></TD>
      						<TD class=table_title align=middle ><STRONG><FONT color=#000000><%tcWebApi_get("String_Entry","ParentalDELText","s")%></FONT></STRONG></TD>      					          		 								
            		</TR>
            		<script language=JavaScript type=text/javascript>
							 	 			CreateList(10,"url")
										</script>
            		</TABLE>
            		<br/>
            		<DIV style="margin-left:5%">
									<INPUT onclick='btnRemove(this)' id="delURL" name="delURL" type=button value=" <%tcWebApi_get("String_Entry","ParentalDELText","s")%> "> 
								</DIV>
         			</DIV>
            		<br/>
            		<hr/>
            		<br/>
            		<DIV>           		
            			<P><%tcWebApi_get("String_Entry","ParentalDRightText","s")%><INPUT id=DRight onclick="selRight(this)" type="checkbox" name="DRight" <%if tcWebApi_get("Parental_Entry","DurationActive","h") = "Yes" then asp_Write("checked") end if%>></P>          	
            		</DIV>
            		<br/>
            		<DIV id=d_right>
            			<DIV>
            		<%tcWebApi_get("String_Entry","ParentalDPolicyText","s")%><font color="#000000"><%tcWebApi_get("String_Entry","ParentalBlackText","s")%></font><INPUT onclick="ChangePolicy(this)"  TYPE="radio"  NAME="d_policy"  VALUE="Black" <%if tcWebApi_get("Parental_Entry","DurationPolicy","h") = "Black" then asp_Write("checked")  end if%>>
										<font color="#000000"><%tcWebApi_get("String_Entry","ParentalWhiteText","s")%></font><INPUT onclick="ChangePolicy(this)"  TYPE="radio"  NAME="d_policy"  VALUE="White" <%if tcWebApi_get("Parental_Entry","DurationPolicy","h") = "White" then asp_Write("checked")  end if%>>
									</DIV>
									<br/>
								<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0 >
										<TR>
										<TD><%tcWebApi_get("String_Entry","ParentalStartText","s")%>:&nbsp;&nbsp;<INPUT onkeydown="if(event.keyCode==13)return false" maxLength=5  name="ds_item" id="ds_item" value=""><br/></TD>
										</TR>
										<br/>
										<TR>
										<TD><%tcWebApi_get("String_Entry","ParentalEndText","s")%>:&nbsp;&nbsp;<INPUT onkeydown="if(event.keyCode==13)return false" maxLength=5  name="de_item" id="de_item" value=""><br/></TD>
									</TR>
									<br/>
									<TR>									
										<TD >
											<%tcWebApi_get("String_Entry","ParentalSunText","s")%><input type="checkbox" name="d_w"  value="7">
											<%tcWebApi_get("String_Entry","ParentalMonText","s")%><input type="checkbox" name="d_w"  value="1">
											<%tcWebApi_get("String_Entry","ParentalTuesText","s")%><input type="checkbox" name="d_w"  value="2">  
											<%tcWebApi_get("String_Entry","ParentalWedText","s")%><input type="checkbox" name="d_w"  value="3">  
											</TD>
									</TR> 
									<TR>									
										<TD > 
											<%tcWebApi_get("String_Entry","ParentalThurText","s")%><input type="checkbox" name="d_w"  value="4">
											<%tcWebApi_get("String_Entry","ParentalFriText","s")%><input type="checkbox" name="d_w"  value="5">  
											<%tcWebApi_get("String_Entry","ParentalSatText","s")%><input type="checkbox" name="d_w"  value="6">  								  											  
										</TD>
									</TR>
									<TR>
										<TD align=left><INPUT onclick="btnAdd(this)" id="durationAdd" name="durationAdd" type=button value=" <%tcWebApi_get("String_Entry","ParentalADDText","s")%> "> <TD>
								</TR>
								</TABLE>
								<br/>
								<TABLE width="90%" border="1" align=center cellpadding="3" cellspacing="0" bordercolor="#CCCCCC" bgcolor="#FFFFFF">
            		<TR>
            		<TD class=table_title align=middle width="10%"><STRONG><%tcWebApi_get("String_Entry","ParentalListIdxText","s")%></STRONG></TD>
      						<TD width="26%" align=center class="table_title"><strong><FONT color="#000000"><%tcWebApi_get("String_Entry","ParentalStartText","s")%></FONT></strong></TD>
      						<TD width="26%" align=center class="table_title"><strong><FONT color="#000000"><%tcWebApi_get("String_Entry","ParentalEndText","s")%></FONT></strong></TD>
      						<TD width="26%" align=center class="table_title"><strong><FONT color="#000000"><%tcWebApi_get("String_Entry","ParentalWeekDayText","s")%></FONT></strong></TD>
      						<TD class=table_title align=middle width="10%"><STRONG><FONT color=#000000><%tcWebApi_get("String_Entry","ParentalDELText","s")%></FONT></STRONG></TD>      			
            		</TR>
            		   	<script language=JavaScript type=text/javascript>
							 	 			CreateList(4,"duration")
										</script>
            		</TABLE>
            		<br/>
            		<DIV style="margin-left:5%">
									<INPUT onclick='btnRemove(this)' id="delTime" name="delTime" type=button value=" <%tcWebApi_get("String_Entry","ParentalDELText","s")%> "> 
								</DIV>   
            	</DIV>
            	</DIV>                 
            </DIV></DIV></DIV></FORM></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594>	</TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
