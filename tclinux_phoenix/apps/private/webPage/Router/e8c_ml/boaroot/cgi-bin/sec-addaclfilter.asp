<%
if Request_Form("Save_Flag") = "1" then 
	TCWebApi_set("WebCurSet_Entry","acl_id","curNum")
	TCWebApi_set("ACL_Common","Action","Actionflag")
	TCWebApi_set("ACL_Entry","Activate","EnableAcl_Flag")
	TCWebApi_set("ACL_Entry","ACLName","AclRuleName")
	TCWebApi_set("ACL_Entry","ScrIPAddrBegin","ScrIPAddrBegin")
	TCWebApi_set("ACL_Entry","ScrIPAddrEnd","ScrIPAddrEnd")
	TCWebAPI_set("ACL_Entry","Interface","Interface")
	TCWebAPI_set("ACL_Entry","Application","Application")
	
	TCWebApi_commit("ACL_Entry")
	TCWebApi_save()
end if
%>
<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" href='/JS/stylemain.css' type='text/css'>
<title><%tcWebApi_get("String_Entry","MACFilter17Text","s")%></title>
<script language="javascript" src="/JS/menu.js"></script>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<script language="javascript" src="/JS/util.js"></script>
<script language="JavaScript" type="text/JavaScript">
<% if Request_Form("Save_Flag") = "1" then %>
	location.replace("/cgi-bin/sec-aclfilter.asp");
<%end if%>
</script>
</head>
<body topmargin="0" leftmargin="0" marginwidth="0" marginheight="0" alink="#000000" link="#000000" vlink="#000000" style="text-align: center" onLoad="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ConfigForm') != null)LoadFrame()"
onunload="DoUnload()">
<table width="808" height="100%" border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
<td height="1">
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
<tr>
<td>&nbsp;</td>
<td width="358" align="right" valign="bottom">
<table border="0" cellpadding="0" cellspacing="0" id="table8">
<tr>
<td align="right" valign="bottom">
<span class="curUserName">
&nbsp;
</span>
</td>
<td width="120" align="center" valign="bottom" class="welcom">
<%tcWebApi_get("String_Entry","WelcomeSettingText","s")%>
</td>
<td width="50" valign="bottom"><a href="/cgi-bin/login.asp" id=logOut name=logOut target="_top" onClick="DoLogout()">
<span class="logout">
<%tcWebApi_get("String_Entry","LogoutText","s")%>
</span>
</a>
</td>
</tr>
</table></td>
</tr>
</table>
<table border="0" cellpadding="0" cellspacing="0" width="808" height="100" id="table2">
<tr>
<td width="163" rowspan="3" align="center" bgcolor="#EF8218" class="LocationDisplay" id="LocationDisplay"></td>
<td height="33" width="434" bgcolor="#427594">
<p align="right"><font color="#FFFFFF" face="黑体"><b><font face="黑体" size="6" color="#FFFFFF">
<input name="Selected_Menu" type="hidden" id="Selected_Menu" value="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>-><%tcWebApi_get("String_Entry","ContentAclFilterText","s")%>">
</font></b><span class="GatewayName"><%tcWebApi_get("String_Entry","GatewayNameText","s")%>
<script language="javascript">
document.write(top.gateWayName);
</script>
</span></font></td>
<td height="33" width="211" bgcolor="#EF8218">
<p align="center" class="GatewayType"><%tcWebApi_get("String_Entry","ModeText","s")%>
<script language="javascript">
document.write(top.ModelName);
</script></td>
</tr>
<tr>
<td height="43" colspan="2"  bgcolor="#EF8218" valign="bottom" id="MenuArea_L1">&nbsp;</td>
</tr>
<tr>
<td colspan="2" height="24" bgcolor="#427594" id="MenuArea_L2"></td>
</tr>
</table>
<script language="javascript">
MakeMenu(getElById ('Selected_Menu').value);
</script>
<table border="0" cellpadding="0" cellspacing="0" width="808" height="15" id="table3">
<tr>
<td height="15">
<img height=15 src="/img/panel1.gif" width=164 border=0>
</td>
<td>
<img height=15 src="/img/panel2.gif" width=645 border=0>
</td>
</tr>
</table>
</td>
</tr>
<tr>
<td valign="top">
<script language="JavaScript" type="text/javascript">
var AclFilterRuleIndex = 0;
var AclFilterRule = new Array();

function LoadFrame()
{
	var enable = "<% tcWebApi_get("String_Entry", "ACLFilterEnableStatusText","s") %>"; 
	with (getElById('ConfigForm'))
	{	
		Save_Flag.value = "0";
		
        var aclvalue = '<% tcWebApi_JsonHook("get_aclfilter_entry_info", "AclEditNumber", "area") %>';
		var rsp = JSON.parse(aclvalue);
		acl_list = rsp.data;

		if(acl_list[0]['Index'] != -1)
		{
			if( acl_list[0]['Active'] == "Yes")
			{
				enableAcl.checked = true;
			}
			else
			{
				enableAcl.value = false;
			}
			AclRuleName.value = acl_list[0]['ACLName'];
			ScrIPAddrBegin.value = acl_list[0]['ScrIPAddrBegin'];
			ScrIPAddrEnd.value = acl_list[0]['ScrIPAddrEnd'];
			Interface.value = acl_list[0]['Interface'];
			Application.value = acl_list[0]['Application'];	
			curNum.value = acl_list[0]['Index'];
		}
	}
}

function cmpIpAddress(address1,address2)
{
	var Lnum = 0;
	var Snum = 0;
	var addrParts1 = address1.split('.');
	var addrParts2 = address2.split('.');
	for (var i = 0; i <= 3; i++)
	{
		Lnum = parseInt(addrParts1[i]);
		Snum = parseInt(addrParts2[i]);
		if (Lnum < Snum)
		{
			return false;
		}
	}
	
	return true;
}

function CheckForm(type)
{
	with (getElById('ConfigForm'))
	{
		var BeginIP = getValue('ScrIPAddrBegin');
		var EndIP = getValue('ScrIPAddrEnd');
		var RuleName = getValue('AclRuleName');

		if (RuleName == '' || BeginIP == '' || EndIP == '')
		{
			alert("<%tcWebApi_get("String_Entry","InputNotEmptyText","s")%>");
			return false;
		}
				
		if (isValidName(RuleName) == false)
		{
			alert("<%tcWebApi_get("String_Entry","ACLFilterNameErrorText","s")%>");
			return false;
		}
		
		if (BeginIP != "0.0.0.0" && isAbcIpAddress(BeginIP) == false)
		{
			alert("<%tcWebApi_get("String_Entry","ACLFilterBeginIPErrorText","s")%>");
			return false;
		}
		
		if (EndIP != "0.0.0.0" && isAbcIpAddress(EndIP) == false)
		{
			alert("<%tcWebApi_get("String_Entry","ACLFilterEndIPErrorText","s")%>");
			return false;
		}
		
		if (cmpIpAddress(EndIP ,BeginIP) == false)
		{
			alert("<%tcWebApi_get("String_Entry","ACLFilterLargerIPText","s")%>");
			return false;
		}
	}
	
	return true;
}

</script>
<script language="javascript" for="document" event="onkeydown">
if(event.keyCode==13)
{
SubmitForm();
return false;
}
</script>
<table height="100%" border="0" cellpadding="0" cellspacing="0">
<tr>
<td width="157" bgcolor="#EF8218" height="30"><p class="Item_L1"><%tcWebApi_get("String_Entry","ACLFiltertTitleText","s")%></p></td>
<td width="7" bgcolor="#EF8218">　</td>
<td width="474">　</td>
<td width="170" rowspan="5" valign="top" background="/img/panel4.gif"><table width="100%" border="0" cellspacing="0" cellpadding="20" height='100%'>
<tr>
<td valign='top'><a href="/cgi-bin/help_content.asp#ACL过滤" target="_blank"><img src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width="40" height="34" border="0"></a></td>
</tr>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
</table>　      </td>
</tr>
<tr>
<td width="157" height="10" valign="top" bgcolor="#E7E7E7"></td>
<td width="7" background="/img/panel3.gif">　</td>
<td></td>
</tr>
<tr>
<td width="157" height="30" valign="top" bgcolor="#E7E7E7"><p class="Item_L2"></p></td>
<td width="7" background="/img/panel3.gif">　</td>
<td>
<form name="ConfigForm" action="/cgi-bin/sec-addaclfilter.asp" method="post">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="10">&nbsp;</td>
<td>
<br>

                    <input type="hidden" id="Save_Flag" name="Save_Flag" value="0">
                    <input type="hidden" id="EnableAcl_Flag" name="EnableAcl_Flag" value="No">
                    <input type="hidden" id="curNum" name="curNum" value="<% tcWebApi_get("ACL_Common","add_aclnum","s") %>">
                    <input type="hidden" id="Actionflag" name="Actionflag" value="Add">
                    <script language="JavaScript" type="text/JavaScript">

function btnSubmit()
{
	var Form = document.ConfigForm;
	enableFilter(Form.enableAcl);
	if(CheckForm(0) == false)
		return;
	Form.Save_Flag.value = "1";
	if(true == setEBooValueCookie(document.ConfigForm))
		Form.submit();
}	

function enableFilter(filter)
{
	
	with (getElById('ConfigForm'))
	{
		if (filter.checked)
		{
			EnableAcl_Flag.value = "Yes";	
		}
		else
		{
			EnableAcl_Flag.value = "No";
		}
	}
}

</script>

<table border="0" cellpadding="0" cellspacing="0">

<tr>
	<td><%tcWebApi_get("String_Entry","ACLFilterEnableText","s")%></td>
	<td><INPUT name="enableAcl" id="enableAcl" onclick="enableFilter(this)" type="checkbox"  <%if tcWebApi_get("ACL_Entry","Activate","h") = "Yes" then asp_Write("checked") end if%>></td>
</tr>

<tr>
	<td><%tcWebApi_get("String_Entry","ACLFilterNameText","s")%></td>
	<td><input type='text' name="AclRuleName" id="AclRuleName" maxlength='31'></td>
</tr>

<tr>
	<td><%tcWebApi_get("String_Entry","ACLFilterScrIPAddrBeginText","s")%></td>
	<td><input type='text' name="ScrIPAddrBegin" id="ScrIPAddrBegin" maxlength='64'></td>
	<td width=200><font color=red><%tcWebApi_get("String_Entry","ACLFilterNoteText","s")%></font></td>
</tr>

<tr>
	<td><%tcWebApi_get("String_Entry","ACLFilterScrIPAddrEndText","s")%></td>
	<td><input type='text' name="ScrIPAddrEnd" id="ScrIPAddrEnd" maxlength='64'></td>
</tr>

<tr>
	<td><%tcWebApi_get("String_Entry","ACLFilterInterfaceText","s")%></td>
	<td>
		<SELECT NAME="Interface" id="Interface">
		 <option value="Wan" <% if tcWebApi_get("ACL_Entry","Interface","h") = "Wan" then asp_Write("selected") end if %>>Wan
		 <option value="Lan" <% if tcWebApi_get("ACL_Entry","Interface","h") = "Lan" then asp_Write("selected") end if %>>Lan
		 <option value="Both" <% if tcWebApi_get("ACL_Entry","Interface","h") = "Both" then asp_Write("selected") end if %>>Both
		</SELECT>
	<td>
</tr>

<tr>
	<td><%tcWebApi_get("String_Entry","ACLFilterAppText","s")%></td>
	<td>
		<SELECT NAME="Application" id="Application">
		 <option value="ALL" <% if tcWebApi_get("ACL_Entry","Application","h") = "ALL" then asp_Write("selected") end if %>>ALL
		 <option value="PING" <% if tcWebApi_get("ACL_Entry","Application","h") = "PING" then asp_Write("selected") end if %>>PING
		 <option value="FTP" <% if tcWebApi_get("ACL_Entry","Application","h") = "FTP" then asp_Write("selected") end if %>>FTP
		 <option value="SSH" >SSH</option>
		 <option value="WEB" <% if tcWebApi_get("ACL_Entry","Application","h") = "WEB" then asp_Write("selected") end if %>>WEB
		 <option value="TELNET" <% if tcWebApi_get("ACL_Entry","Application","h") = "TELNET" then asp_Write("selected") end if %>>TELNET
		 <option value="SNMP" <% if tcWebApi_get("ACL_Entry","Application","h") = "SNMP" then asp_Write("selected") end if %>>SNMP
		</SELECT>
	<td>
</tr>

<tr>
<td style="display:none"><br>
<%tcWebApi_get("String_Entry","ACLFilterEnableText","s")%>
<input type='checkbox' id="Enable" name="Enable" checked></td>
</tr>
</table>
</td>
<td width="10">&nbsp;</td>
</tr>
<tr>
<td width="10">&nbsp;</td>
<td></td>
<td width="10">&nbsp;</td>
</tr>
</table>
</form>
</td>
</tr>
<tr>
<td width="157" height="10" valign="top" bgcolor="#E7E7E7"></td>
<td width="7" background="/img/panel3.gif">　</td>
<td></td>
</tr>
<tr>
<td width="157" height="" valign="top" bgcolor="#E7E7E7"></td>
<td width="7" background="/img/panel3.gif">　</td>
<td></td>
</tr>
</table>
</td>
</tr>
<tr>
<td height="1">
<table border="0" cellpadding="0" cellspacing="0" width="808" height="35" id="table7">
<tr>
<td width="162" bgcolor="#EF8218">　</td>
<td width="278" bgcolor="#427594">　</td>
<td width="196" bgcolor="#427594">
<p align="center"><img src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width="80" height="23" border="0" id="btnOK" onClick="btnSubmit()">&nbsp;&nbsp;<img src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" width="80" height="23" border="0" id="btnCancel" onClick="RefreshPage()"> </td>
<td width="170" bgcolor="#313031">　</td>
</tr>
</table>
</td>
</tr>
</table>
</body>
</html>
