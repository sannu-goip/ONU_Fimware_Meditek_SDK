<%
if Request_Form("VirServer_Flag") = "1" then
	TCWebApi_set("WebCurSet_Entry","virServ_id","VirServCurIndex")
	TCWebApi_commit("WebCurSet_Entry")
if tcWebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then
	TCWebApi_set("VirServer_Entry_Entry","Protocal","PortMappingProtocol")
	TCWebApi_set("VirServer_Entry_Entry","SrcIP","RemoteHost")
	TCWebApi_set("VirServer_Entry_Entry","STARTPORT","ExternalPort")
	TCWebApi_set("VirServer_Entry_Entry","ENDPORT","InternalPort")
	TCWebApi_set("VirServer_Entry_Entry","LOCALIP","InternalClient")
	TCWebApi_set("VirServer_Entry_Entry","MapName","PortMappingDescription")	
	TCWebApi_commit("VirServer_Entry_Entry")
	tcWebApi_save()
else
	TCWebApi_set("VirServer_Entry","Protocal","PortMappingProtocol")
	TCWebApi_set("VirServer_Entry","STARTPORT","ExternalPort")
	TCWebApi_set("VirServer_Entry","INTPORT","InternalPort")
	TCWebApi_set("VirServer_Entry","LOCALIP","InternalClient")
	TCWebApi_set("VirServer_Entry","MapName","PortMappingDescription")	
	TCWebApi_commit("VirServer_Entry")
	tcWebApi_save()
end if
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
<% if Request_Form("VirServer_Flag") = "1" then %>
	location.replace("/cgi-bin/app-natset.asp");
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
<td width="50" valign="bottom"><a href="/cgi-bin/logout.cgi" id=logOut name=logOut target="_top" onClick="DoLogout()">
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
<input name="Selected_Menu" type="hidden" id="Selected_Menu" value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>-><%tcWebApi_get("String_Entry","ContentAdvancedNATText","s")%>">
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

<table height="100%" border="0" cellpadding="0" cellspacing="0">
<tr>
<td width="157" bgcolor="#EF8218" height="30"><p class="Item_L1"><%tcWebApi_get("String_Entry","NatVirSerCfgText","s")%></p></td>
<td width="7" bgcolor="#EF8218">　</td>
<td width="474">　</td>
<td width="170" rowspan="5" valign="top" background="/img/panel4.gif"><table width="100%" border="0" cellspacing="0" cellpadding="20" height='100%'>
<tr>
<td valign='top'><a href="/cgi-bin/help_content.asp#高级NAT配置" target="_blank"><img src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width="40" height="34" border="0"></a></td>
</tr>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
	<TR><TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
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
<form name="ConfigForm" action="/cgi-bin/app-natset.asp" method="post">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="10">&nbsp;</td>
<td>
<br>

<input type="hidden" id="VirServer_Flag" name="VirServer_Flag" value="0"> 
<input type="hidden" id="VirServCurIndex" name="VirServCurIndex" value="0">

<script language="JavaScript" type="text/JavaScript">

var j = 0;
var PortMapping = new Array();
var indexPortMapping = 0;
var VirSerIdleFlag = 0;
var VirSerIdleEntryIndex = 0;
var MAX_VIRSERVER_NUM = 10;

var nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
var vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatActive = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatProtocal = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatSrcIP = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatSTARTPORT = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatENDPORT = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatLOCALIP = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatMapName = vArrayStr.split(',');

for (j = 0; j < MAX_VIRSERVER_NUM; j++)
{
	if(vNatActive[j] == "Yes" || vNatActive[j] == "No")
	{
		PortMapping[indexPortMapping] = new stPortMap(j,vNatActive[j],vNatProtocal[j],vNatSrcIP[j],vNatSTARTPORT[j],vNatENDPORT[j],vNatLOCALIP[j],vNatMapName[j]);
		indexPortMapping++;
	}
	else if(0 == VirSerIdleFlag){
		VirSerIdleEntryIndex = j;
		VirSerIdleFlag = 1;
	}
}

function stPortMap(domain,ProtMapEnabled,Protocol,RemoteHost,ExPort,InPort,InClient,Description)
{
	domain = ""+domain;	
	this.domain = domain;
	this.key = this.domain;
	this.ProtMapEnabled = ProtMapEnabled;
	this.Protocol = Protocol;
	this.RemoteHost = RemoteHost;
	this.ExPort = ExPort;
	this.InPort = InPort;
	this.InClient = InClient;
	this.Description = Description;
	var index = domain.lastIndexOf('PortMapping');
	this.interface = domain.substr(0,index - 1);
	this.interface = domain;
	this.NATEnabled = '';
	this.Enable = '';
}

function LoadFrame()
{		
	with (getElById('ConfigForm'))
	{	
		PortMappingProtocol.value = PortMapping[top.VirServerModifyMappingIndex].Protocol;
		ExternalPort.value = PortMapping[top.VirServerModifyMappingIndex].ExPort;
		InternalPort.value = PortMapping[top.VirServerModifyMappingIndex].InPort;
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then %>
		RemoteHost.value = PortMapping[top.VirServerModifyMappingIndex].RemoteHost;
<% end if %>
		InternalClient.value = PortMapping[top.VirServerModifyMappingIndex].InClient;
		PortMappingDescription.value = PortMapping[top.VirServerModifyMappingIndex].Description;
	}
}

function CheckForm()
{
	with (getElById('ConfigForm') )
	{

<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then %>
		if (RemoteHost.value != "")
		{
			if (isValidIpAddress(RemoteHost.value) == false)
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS0Text","s")%>');
				return false;
			}
			else if ((isLoopIpAddress(getValue('RemoteHost'))==true)||(isBroadcastIpAddress(getValue('RemoteHost'))==true)
			||(isNtwkSgmtIpAddress(getValue('RemoteHost'))==true))
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS1Text","s")%>');
				return false;
			}
		}
<% end if %>
		if (ExternalPort.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS2Text","s")%>' );
			return false;
		}
		if (InternalPort.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS3Text","s")%>' );
			return false;
		}
		if (InternalClient.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS4Text","s")%>' );
			return false;
		}
		if (PortMappingDescription.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS5Text","s")%>' );
			return false;
		}
		if (isAbcIpAddress(InternalClient.value) == false ||(isBroadcastIpAddress(getValue('InternalClient'))==true)
		||(isNtwkSgmtIpAddress(getValue('InternalClient'))==true))
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS4Text","s")%>');
			return false;
		}
		if (isValidPort(ExternalPort.value) == false )
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS2Text","s")%>' );
			return false;
		}
		if (isValidPort(InternalPort.value) == false )
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS3Text","s")%>' );
			return false;
		}
		if (isValidName(PortMappingDescription.value) == false)
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS6Text","s")%>');
			return false;
		}
		for (i = 0; i < PortMapping.length; i++)
		{
			if ( PortMapping[i].Description == PortMappingDescription.value 
			&& PortMapping[i].Protocol == PortMappingProtocol.value
			&& PortMapping[i].ExPort == ExternalPort.value
			&& PortMapping[i].InPort == InternalPort.value
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then %>
			&& PortMapping[i].RemoteHost == RemoteHost.value
<% end if %>
			&& PortMapping[i].InClient == InternalClient.value)
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS8Text","s")%>');
				return false;
			}
		}
	}
	return true;
}

function btnSubmit()
{
	var Form = document.ConfigForm;
	if(CheckForm() == false)
	{
		return;
	}
	Form.VirServer_Flag.value = "1";
	Form.VirServCurIndex.value = top.VirServerModifyEntryIndex;
	if(true == setEBooValueCookie(document.ConfigForm))
		Form.submit();
}	

</script>

<table border="0" cellpadding="0" cellspacing="0">

	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerProtocolText","s")%> </TD>
		<TD>
			<SELECT size=1 id=PortMappingProtocol name=PortMappingProtocol>
			<OPTION value=TCP selected>TCP</OPTION> 
			<OPTION value=UDP>UDP</OPTION>
			<OPTION value=TCP/UDP>TCP/UDP</OPTION>
			</SELECT>
		</TD>
	</TR>
	
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %>	
	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerTableExternalPortText","s")%>：</TD>
		<TD><INPUT maxLength=5 id=ExternalPort name=ExternalPort></TD>
	</TR>
	
	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerTableInternalPortText","s")%>：</TD>
		<TD><INPUT maxLength=5 id=InternalPort name=InternalPort></TD>
	</TR>
<% else %>				
	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerSrcIPText","s")%> </TD>
		<TD><INPUT maxLength=15 id=RemoteHost name=RemoteHost></TD>
	</TR>
	
	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerSPortNumText","s")%> </TD>
		<TD><INPUT maxLength=5 id=ExternalPort name=ExternalPort></TD>
	</TR>
	
	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerEndPortNumText","s")%> </TD>
		<TD><INPUT maxLength=5 id=InternalPort name=InternalPort></TD>
	</TR>
<% end if %>

	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerInternalSerText","s")%> </TD>
		<TD><INPUT maxLength=32 id=InternalClient name=InternalClient></TD>
	</TR>
	
	<TR>
		<TD width=120><%tcWebApi_get("String_Entry","NatVirSerMappingNameText","s")%> </TD>
		<TD><INPUT maxLength=31 id=PortMappingDescription name=PortMappingDescription></TD>
	</TR>

<tr>
<td style="display:none"><br>
<%tcWebApi_get("String_Entry","ACLFilterEnableText","s")%>
<input type='checkbox' id="Enable" name="Enable" checked></td>
</tr>
</table>
<br><br>
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
