<%
if Request_Form("PortFilterType") <> "" then
	TCWebApi_set("WebCurSet_Entry","ipfilter_id","PortFilterType")
end if

if Request_Form("Save_Flag") = "1" then 
	TCWebApi_set("WebCurSet_Entry","ipfilter_id","curNum")
	TCWebApi_set("IpMacFilter_Common","IpMacType","IpMacType_Flag")
	TCWebApi_set("IpMacFilter_Common","Action","Actionflag")
	TCWebApi_set("IpMacFilter_Entry","Interface","WanInterfaceFlag")	
	TCWebApi_set("IpMacFilter_Entry","Direction","Direction_Flag")
	TCWebApi_set("IpMacFilter_Entry","Active","EnableIP_Flag")
	TCWebApi_set("IpMacFilter_Entry","RuleType","RuleType_Flag")
	TCWebApi_set("IpMacFilter_Entry","IPName","Name")
	TCWebApi_set("IpMacFilter_Entry","Protocol","Protocol")
	
	TCWebApi_set("IpMacFilter_Entry","SrcIPAddr","SourceIPStart")
	TCWebApi_set("IpMacFilter_Entry","SrcIPMask","SourceIpMask")
	TCWebApi_set("IpMacFilter_Entry","SrcPort","SourcePortStart")

	TCWebApi_set("IpMacFilter_Entry","DesIPAddr","DestIPStart")
	TCWebApi_set("IpMacFilter_Entry","DesIPMask","DestIpMask")
	TCWebApi_set("IpMacFilter_Entry","DesPort","DestPortStart")
	if TCWebApi_get("WebCustom_Entry","isCMCCSupported","h" ) = "Yes" then
		TCWebApi_set("IpMacFilter_Entry","SrcIPEndAddr","SourceIPEnd")
		TCWebApi_set("IpMacFilter_Entry","DesIPEndAddr","DestIPEnd")
		TCWebApi_set("IpMacFilter_Entry","SrcPortEnd","SourcePortEnd")
		TCWebApi_set("IpMacFilter_Entry","DesPortEnd","DestPortEnd")
	end if
	TCWebApi_commit("IpMacFilter_Entry")
	TCWebApi_save()
	
end if
%>
<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" href="/JS/stylemain.css" type='text/css'>
<title><%tcWebApi_get("String_Entry","PortFilter29Text","s")%></title>

<script language="javascript" src="/JS/menu.js"></script>
<script language="javascript" src="/JS/jquery.js"></script>
<script language="javascript" src="/JS/util.js"></script>
<SCRIPT language=javascript src="/JS/json2.js"></SCRIPT>
<script language="JavaScript" type="text/JavaScript">
<% if Request_Form("Save_Flag") = "1" then %>
	location.replace("/cgi-bin/sec-portfilter.asp");
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
          <td width="358" align="right" valign="bottom"> <table border="0" cellpadding="0" cellspacing="0" id="table8">
              <tr> 
                <td align="right" valign="bottom"> <span class="curUserName"> 
                  &nbsp; </span> </td>
                <td width="120" align="center" valign="bottom" class="welcom"> 
                  <%tcWebApi_get("String_Entry","WelcomeSettingText","s")%> </td>
                <td width="50" valign="bottom"><a href="/cgi-bin/login.asp" id=logOut name=logOut target="_top" onClick="DoLogout()"> 
                  <span class="logout"> <%tcWebApi_get("String_Entry","LogoutText","s")%> </span> </a> </td>
              </tr>
            </table></td>
        </tr>
      </table>
      <table border="0" cellpadding="0" cellspacing="0" width="808" height="100" id="table2">
        <tr> 
          <td width="163" rowspan="3" align="center" bgcolor="#EF8218" class="LocationDisplay" id="LocationDisplay"></td>
          <td height="33" width="434" bgcolor="#427594"> <p align="right"><font color="#FFFFFF" face="黑体"><b><font face="黑体" size="6" color="#FFFFFF"> 
              <input name="Selected_Menu" type="hidden" id="Selected_Menu" value="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>-><%tcWebApi_get("String_Entry","ContentPortFilterText","s")%>">
              </font></b><span class="GatewayName"><%tcWebApi_get("String_Entry","GatewayNameText","s")%> 
              <script language="javascript">
document.write(top.gateWayName);
</script>
              </span></font></td>
          <td height="33" width="211" bgcolor="#EF8218"> <p align="center" class="GatewayType"><%tcWebApi_get("String_Entry","ModeText","s")%> 
              <script language="javascript">
document.write(top.ModelName);
</script>
          </td>
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
</script> <table border="0" cellpadding="0" cellspacing="0" width="808" height="15" id="table3">
        <tr> 
          <td height="15"> <img height=15 src="/img/panel1.gif" width=164 border=0> 
          </td>
          <td> <img height=15 src="/img/panel2.gif" width=645 border=0> </td>
        </tr>
      </table></td>
  </tr>
  <tr> 
    <td valign="top"> <table height="100%" border="0" cellpadding="0" cellspacing="0">
        <tr> 
          <td width="157" bgcolor="#EF8218" height="30"><p class="Item_L1"><%tcWebApi_get("String_Entry","PortFilter1Text","s")%></p></td>
          <td width="7" bgcolor="#EF8218">　</td>
          <td width="474">　</td>
          <td width="170" rowspan="5" valign="top" background="/img/panel4.gif"><table width="100%" border="0" cellspacing="0" cellpadding="20" height='100%'>
              <tr> 
                <td valign='top'><a href="/cgi-bin/help_content.asp#端口过滤" target="_blank"><img src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width="40" height="34" border="0"></a></td>
              </tr>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
            </table>
            　 　 　</td>
        </tr>
        <tr> 
          <td width="157" height="10" valign="top" bgcolor="#E7E7E7"></td>
          <td width="7" background="/img/panel3.gif">　</td>
          <td></td>
        </tr>
        <tr> 
          <td width="157" height="30" valign="top" bgcolor="#E7E7E7"><p class="Item_L2"></p></td>
          <td width="7" background="/img/panel3.gif">　</td>
          <td> <table width="100%" border="0" cellspacing="0" cellpadding="0">
              <tr> 
                <td width="10">&nbsp;</td>
                <td> <form name="ConfigForm" id="ConfigForm" action="/cgi-bin/sec-addportfilter.asp" method="post">
                    <p> 
                      <input type="hidden" id="PortFilterType" name="PortFilterType">
                      <input type="hidden" id="IpMacType_Flag" name="IpMacType_Flag" value="IpUp">
                      <input type="hidden" id="Save_Flag" name="Save_Flag" value="0">
                      <input type="hidden" id="curNum" name="curNum" value="<% tcWebApi_get("WebCurSet_Entry","ipfilter_id","s") %>">
                      <input type="hidden" id="Direction_Flag" name="Direction_Flag" value="<% tcWebApi_get("IpMacFilter_Entry","Direction","s") %>">
                      <input type="hidden" id="EnableIP_Flag" name="EnableIP_Flag" value="Yes">
                      <input type="hidden" id="RuleType_Flag" name="RuleType_Flag" value="IP">
                      <input type="hidden" id="Current_Flag" name="Current_Flag" value="0">
                      <input type="hidden" id="WanInterfaceFlag" name="WanInterfaceFlag" value="br0">
                      <input type="hidden" id="IpType_Flag" name="IpType_Flag" value="IpUp">
                      <input type="hidden" id="Actionflag" name="Actionflag" value="Add">
                      <script language="JavaScript" type="text/javascript">
var DomainName = document.ConfigForm.curNum.value;
/*if (top.preAddDomain != '')
{
DomainName = top.preAddDomain;
top.preAddDomain = '';
}
else
{
DomainName = 'InternetGatewayDevice.X_ATP_Security.IpFilterOut';
}*/
var portFilterInIndex = 0;
var portFilterOutIndex = 0;
var portFilterIn = new Array()
var portFilterOut = new Array()
function stWanInfo(domain,Name,Enable,NATEnabled,ConnectionType,CntStatus,IPv6Enabled)
{
this.domain = domain;
this.Name = Name;
this.Enable = Enable;
this.NATEnabled = NATEnabled;
this.ConnectionType = ConnectionType;
this.CntStatus = CntStatus;
this.IPv6Enabled = IPv6Enabled;
}
var WanIP  = new Array(new stWanInfo("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1","1_TR069_R_8_46","1","0","IP_Routed","PendingDisconnect","0"),null);
var WanPPP = new Array(null);
var WanInfo = new Array();
var i;
for (i = 0; i < WanIP.length-1; i++)
{
WanInfo[i] = WanIP[i];
}
for (j = 0; j < WanPPP.length-1; i++,j++)
{
WanInfo[i] =  WanPPP[j];
}
var FliterType;

//macfilter:40 + upPortFilter:100
if (DomainName <= 139)
{
	FliterType = 'out';//up filter
}
else
{
	FliterType = 'in';//down filter
}
function LoadFrame()
{
	var typeFLAG = GetQueryString('typeFlag');
	document.ConfigForm.Save_Flag.value = "0";
	if (FliterType== "out")
	{
		document.ConfigForm.Current_Flag.value = "out";
		setDisplay('interface',0);
		ChangeInface();		
	}
	else if (FliterType== "in")
	{
		document.ConfigForm.Current_Flag.value = "in";
		setDisplay('interface',1);
		ChangeInface();
	}
	if ( typeFLAG )
		document.ConfigForm.IpMacType_Flag.value = typeFLAG;
}

function checkForm()
{
	//var	IPNames = new Array(8);
	var vnumber = 0;
	/*IPNames[0] = "<% tcWebApi_get("IpMacFilter_Entry0","IPName","s") %>";
	IPNames[1] = "<% tcWebApi_get("IpMacFilter_Entry1","IPName","s") %>";
	IPNames[2] = "<% tcWebApi_get("IpMacFilter_Entry2","IPName","s") %>";
	IPNames[3] = "<% tcWebApi_get("IpMacFilter_Entry3","IPName","s") %>";
	IPNames[4] = "<% tcWebApi_get("IpMacFilter_Entry4","IPName","s") %>";
	IPNames[5] = "<% tcWebApi_get("IpMacFilter_Entry5","IPName","s") %>";
	IPNames[6] = "<% tcWebApi_get("IpMacFilter_Entry6","IPName","s") %>";
	IPNames[7] = "<% tcWebApi_get("IpMacFilter_Entry7","IPName","s") %>";*/
	
	if(document.ConfigForm.Current_Flag.value == "out")
	{
		/*
		if(parseInt(vnumber)>=40)
		{
			alert("<%tcWebApi_get("String_Entry","PortFilter30Text","s")%>");
			return false;	
		}

		if (document.ConfigForm.Name.value == IPNames[0] || document.ConfigForm.Name.value == IPNames[1] || document.ConfigForm.Name.value == IPNames[2] || document.ConfigForm.Name.value == IPNames[3] )
		{
			 alert("<%tcWebApi_get("String_Entry","PortFilter31Text","s")%>");
			 return false;			 	
		}*/
		
		if (document.ConfigForm.Protocol.value == "" && document.ConfigForm.SourceIPStart.value == "" && document.ConfigForm.SourceIpMask.value == "" && document.ConfigForm.SourcePortStart.value == "" &&
			document.ConfigForm.DestIPStart.value == "" && document.ConfigForm.DestIpMask.value == "" && document.ConfigForm.DestPortStart.value == "")
		{
			 alert("<%tcWebApi_get("String_Entry","PortFilter32Text","s")%>");
			 return false;			 	
		}
	}
		
	if(document.ConfigForm.Current_Flag.value == "in")
	{
		/*
		if(parseInt(vnumber)>=40)
		{
			alert("<%tcWebApi_get("String_Entry","PortFilter30Text","s")%>");
			return false;		
		}
	
		if (document.ConfigForm.Name.value == IPNames[4] || document.ConfigForm.Name.value == IPNames[5] || document.ConfigForm.Name.value == IPNames[6] || document.ConfigForm.Name.value == IPNames[7] )
		{
			 alert("<%tcWebApi_get("String_Entry","PortFilter31Text","s")%>");
			 return false;			 	
		}*/
		
		if (document.ConfigForm.Protocol.value == "" && document.ConfigForm.SourceIPStart.value == "" && document.ConfigForm.SourceIpMask.value == "" && document.ConfigForm.SourcePortStart.value == "" &&
			document.ConfigForm.DestPortStart.value == "")
		{
			 alert("<%tcWebApi_get("String_Entry","PortFilter33Text","s")%>");
			 return false;			 	
		}
		
		if (document.ConfigForm.WanPath.value.length == 0)
		{
			alert('<%tcWebApi_get("String_Entry","PortFilter34Text","s")%>');
			return false;
		}	
	}
	
	if (document.ConfigForm.Name.value == '' || document.ConfigForm.Name.value.length > 31 	)
	{
		alert("<%tcWebApi_get("String_Entry","PortFilter35Text","s")%>");
		return false;
	}
	if (isValidName(document.ConfigForm.Name.value) == false)
	{
		alert("<%tcWebApi_get("String_Entry","PortFilter35Text","s")%>");
		return false;
	}
	
	if (document.ConfigForm.SourceIPStart.value != ""  && document.ConfigForm.SourceIPStart.value != "0.0.0.0"  && (isAbcIpAddress(document.ConfigForm.SourceIPStart.value) == false))
	{
		if (isGlobalIpv6Address(document.ConfigForm.SourceIPStart.value) == false)
		{
			alert('<%tcWebApi_get("String_Entry","PortFilter36Text","s")%> "' + document.ConfigForm.SourceIPStart.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>');
			return false;
		}
	}
<% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>	
	if (document.ConfigForm.SourceIPStart.value == "" && document.ConfigForm.SourceIPEnd.value != "" )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter38Text","s")%>';
		alert(msg);
		return false;
	}
	if (document.ConfigForm.SourceIPEnd.value != ""  && document.ConfigForm.SourceIPEnd.value != "0.0.0.0"  && (isAbcIpAddress(document.ConfigForm.SourceIPEnd.value) == false))
	{
		if (isGlobalIpv6Address(document.ConfigForm.SourceIPEnd.value) == false)
		{
			alert('<%tcWebApi_get("String_Entry","PortFilter39Text","s")%> "' + document.ConfigForm.SourceIPEnd.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>');
			return false;
		}
	}
<% end if %>	
	if (document.ConfigForm.SourceIpMask.value != "" && document.ConfigForm.SourceIpMask.value != "0.0.0.0" && (isValidSubnetMask(document.ConfigForm.SourceIpMask.value) == false))
	{
			alert('<%tcWebApi_get("String_Entry","PortFilter40Text","s")%>"' + document.ConfigForm.SourceIpMask.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>');
			return false;
	}
	
	if (document.ConfigForm.DestIPStart.value != ""  && document.ConfigForm.DestIPStart.value != "0.0.0.0"  && (isAbcIpAddress(document.ConfigForm.DestIPStart.value) == false))
	{
		if (isGlobalIpv6Address(document.ConfigForm.DestIPStart.value) == false)
		{
			alert('<%tcWebApi_get("String_Entry","PortFilter41Text","s")%> "' + document.ConfigForm.DestIPStart.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>');
			return false;
		}
	}
<% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>	
	if (document.ConfigForm.DestIPStart.value == "" && document.ConfigForm.DestIPEnd.value != "" )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter42Text","s")%>';
		alert(msg);
		return false;
	}
	if (document.ConfigForm.DestIPEnd.value != ""  && document.ConfigForm.DestIPEnd.value != "0.0.0.0"  && (isAbcIpAddress(document.ConfigForm.DestIPEnd.value) == false))
	{
		if (isGlobalIpv6Address(document.ConfigForm.DestIPEnd.value) == false)
		{
			alert('<%tcWebApi_get("String_Entry","PortFilter43Text","s")%> "' + document.ConfigForm.DestIPEnd.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>');
			return false;
		}
	}
<% end if %>		
	if (document.ConfigForm.DestIpMask.value != "" && document.ConfigForm.DestIpMask.value != "0.0.0.0" && (isValidSubnetMask(document.ConfigForm.DestIpMask.value) == false))
	{
		alert('<%tcWebApi_get("String_Entry","PortFilter44Text","s")%>"' + document.ConfigForm.DestIpMask.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>');
		return false;
	}
	
	if (document.ConfigForm.SourceIPStart.value == "" && document.ConfigForm.SourceIpMask.value != "" )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter45Text","s")%>';
		alert(msg);
		return false;
	}
	
	if (document.ConfigForm.DestIPStart.value == "" && document.ConfigForm.DestIpMask.value != "" )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter46Text","s")%>';
		alert(msg);
		return false;
	}
	
	if (document.ConfigForm.SourcePortStart.value != "" || document.ConfigForm.DestPortStart.value != "")
	{
			if (document.ConfigForm.Protocol.selectedIndex == 0 || document.ConfigForm.Protocol.selectedIndex == 4)
			{		
				msg = '<%tcWebApi_get("String_Entry","PortFilter47Text","s")%>';
				alert(msg);
				return false;
			}
	}
	
	if (document.ConfigForm.SourcePortStart.value != "" && document.ConfigForm.SourcePortStart.value != "0" && isValidPort(document.ConfigForm.SourcePortStart.value) == false )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter7Text","s")%> "' + document.ConfigForm.SourcePortStart.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>';
		alert(msg);
		return false;
	}
<% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>	
	if (document.ConfigForm.SourcePortStart.value == "" && document.ConfigForm.SourcePortEnd.value != "" )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter48Text","s")%>';
		alert(msg);
		return false;
	}
	if (document.ConfigForm.SourcePortEnd.value != "" && document.ConfigForm.SourcePortEnd.value != "0" && isValidPort(document.ConfigForm.SourcePortEnd.value) == false )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter49Text","s")%> "' + document.ConfigForm.SourcePortEnd.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>';
		alert(msg);
		return false;
	}
	if(document.ConfigForm.SourcePortStart.value != "" && document.ConfigForm.SourcePortEnd.value != "" && parseInt(document.ConfigForm.SourcePortEnd.value) < parseInt(document.ConfigForm.SourcePortStart.value)){
		msg = '<%tcWebApi_get("String_Entry","PortFilter50Text","s")%>';
		alert(msg);
		return false;		
	}
<% end if %>
	if (document.ConfigForm.DestPortStart.value != "" && document.ConfigForm.DestPortStart.value != "0" && isValidPort(document.ConfigForm.DestPortStart.value) == false )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter9Text","s")%> "' + document.ConfigForm.DestPortStart.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>';
		alert(msg);
		return false;
	}
<% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>	
	if (document.ConfigForm.DestPortStart.value == "" && document.ConfigForm.DestPortEnd.value != "" )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter51Text","s")%>';
		alert(msg);
		return false;
	}
	if (document.ConfigForm.DestPortEnd.value != "" && document.ConfigForm.DestPortEnd.value != "0" && isValidPort(document.ConfigForm.DestPortEnd.value) == false )
	{
		msg = '<%tcWebApi_get("String_Entry","PortFilter52Text","s")%> "' + document.ConfigForm.DestPortEnd.value + '" <%tcWebApi_get("String_Entry","PortFilter37Text","s")%>';
		alert(msg);
		return false;
	}
	if(document.ConfigForm.DestPortStart.value != "" && document.ConfigForm.DestPortEnd.value != "" && parseInt(document.ConfigForm.DestPortEnd.value) < parseInt(document.ConfigForm.DestPortStart.value)){
		msg = '<%tcWebApi_get("String_Entry","PortFilter53Text","s")%>';
		alert(msg);
		return false;		
	}
<% end if %>
	if(document.ConfigForm.Current_Flag.value == "in")
	{
      	var portInvalue = '<% tcWebApi_JsonHook("get_portfilterIn_info", "action", "area") %>'
		var rsp = JSON.parse(portInvalue);
		var portIn_list = rsp.data;
		for(var i=0;i<portIn_list.length;i++)
		{
			/*
			if (document.ConfigForm.Name.value == portFilterIn[i].IPName)
			{
				alert("<%tcWebApi_get("String_Entry","PortFilter54Text","s")%>");
				return false;
			}*/
			if (document.ConfigForm.Protocol.value == portIn_list[i]['Protocol'] &&
			document.ConfigForm.SourceIPStart.value == portIn_list[i]['SrcIPAddr'] &&
			document.ConfigForm.SourceIpMask.value == portIn_list[i]['SrcIPMask'] &&
			document.ConfigForm.SourcePortStart.value == portIn_list[i]['SrcPort'] &&
			document.ConfigForm.DestIPStart.value == portIn_list[i]['DesIPAddr'] &&
			document.ConfigForm.DestIpMask.value == portIn_list[i]['DesIPMask'] &&
			document.ConfigForm.DestPortStart.value == portIn_list[i]['DesPort'] &&
			document.ConfigForm.WanPath.value == portIn_list[i]['Interface'])
			{
				alert("<%tcWebApi_get("String_Entry","PortFilter55Text","s")%>");
				return false;
			}
		}
	}
	else	//out
	{
      	var portOutvalue = '<% tcWebApi_JsonHook("get_portfilterOut_info", "action", "area") %>'
		var rsp = JSON.parse(portOutvalue);
		var portOut_list = rsp.data;
		for(var i=0;i<portOut_list.length;i++)
		{
			/*
			if (document.ConfigForm.Name.value == portFilterOut[i].IPName)
			{
				alert("<%tcWebApi_get("String_Entry","PortFilter54Text","s")%>");
				return false;
			}*/
			if (document.ConfigForm.Protocol.value == portOut_list[i]['Protocol'] &&
			document.ConfigForm.SourceIPStart.value == portOut_list[i]['SrcIPAddr'] &&
			document.ConfigForm.SourceIpMask.value == portOut_list[i]['SrcIPMask'] &&
			document.ConfigForm.SourcePortStart.value == portOut_list[i]['SrcPort'] &&
			document.ConfigForm.DestIPStart.value == portOut_list[i]['DesIPAddr'] &&
			document.ConfigForm.DestIpMask.value == portOut_list[i]['DesIPMask'] &&
			document.ConfigForm.DestPortStart.value == portOut_list[i]['DesPort'])		
			{
				alert("<%tcWebApi_get("String_Entry","PortFilter55Text","s")%>");
				return false;
			}
		}
	}
	
	return true;
}

function btnSaveIP()
{
	var Form = document.ConfigForm;	

	if (checkForm() != false)
	{
		if(Form.Current_Flag.value == "in")
		{
		Form.curNum.value = "<% tcWebApi_get("IpMacFilter","add_ipdownnum","s") %>";
		Form.IpType_Flag.value = "IpDown";
		Form.Direction_Flag.value = "Incoming";
		Form.WanInterfaceFlag.value =  Form.WanPath.value;
		}
		else
		{
		Form.curNum.value = "<% tcWebApi_get("IpMacFilter","add_ipupnum","s") %>";
		Form.Direction_Flag.value = "Outgoing";
		Form.IpType_Flag.value = "IpUp";
		//Form.WanInterfaceFlag.value =  Form.WanPath.value;		
		}
		Form.Save_Flag.value = "1";
		if( true == setEBooValueCookie(Form) )
			Form.submit();
	}
}
function Cancel()
{
	location.replace('/cgi-bin/sec-portfilter.asp');
}
</script>
                      <script language="JavaScript" type="text/JavaScript">
var nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var vEntryName = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var vEntryValue = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var WANEnNAT = vArrayStr.split(',');

		vEntryName = SortUtil.UpdateWanIFName( vEntryName, nEntryNum );

		vEntryValue = SortUtil.SortMyArray( vEntryValue );
		WANEnNAT = SortUtil.SortMyArray( WANEnNAT );

function ChangeInface()
{
	var index = getElById('WanPath').selectedIndex;
	if (index > -1)
	{
		for (i = 0; i < WANEnNAT.length-1; i++)
		{
			if (vEntryValue[i] == getSelectVal('WanPath'))
			{
				break;
			}
		}
		if (WANEnNAT[i] == "Disabled")
		{
			getElById('NatState').innerHTML = '<%tcWebApi_get("String_Entry","PortFilter3Text","s")%>';
		}
		else
		{
			getElById('NatState').innerHTML = '<%tcWebApi_get("String_Entry","PortFilter2Text","s")%>';
		}
	}
}
</script>
                      <br>
                      <b><%tcWebApi_get("String_Entry","PortFilter56Text","s")%></b><br>
                      <br>
                    <table border="0" cellpadding="0" cellspacing="0">
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter57Text","s")%></td>
                        <td><input type='text' size="18" id="Name" name="Name" maxLength=31></td>
                      </tr>
                      <tr style="display:none"> 
                        <td><%tcWebApi_get("String_Entry","PortFilter22Text","s")%></td>
                        <td> <input type='checkbox' size="18" id="Enable" name="Enable" checked maxLength=16></td>
                      </tr>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter58Text","s")%></td>
                        <td><select id='Protocol' name='Protocol' size="1">
                            <option value=""> 
                            <option value="TCP/UDP">TCP/UDP 
                            <option value="TCP">TCP 
                            <option value="UDP">UDP 
                            <option value="ICMP">ICMP </select></td>
                      </tr>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter59Text","s")%></td>
                        <td><input type='text' size="20" id="SourceIPStart" name="SourceIPStart" maxLength=15></td>
                      </tr>
                      <tr <% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>style="display:none"<% end if %>> 
                        <td><%tcWebApi_get("String_Entry","PortFilter60Text","s")%></td>
                        <td><input type='text' size="20" id="SourceIpMask" name="SourceIpMask" maxLength=15></td>
                      </tr>
                      <% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter61Text","s")%></td>
                        <td><input type='text' size="20" id="SourceIPEnd" name="SourceIPEnd" maxLength=15></td>
                      </tr>                      
                      <% end if %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter62Text","s")%></td>
                        <td> <input type='text' size="20" id="SourcePortStart" name="SourcePortStart" maxLength=11></td>
                      </tr>
                      <% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter63Text","s")%></td>
                        <td><input type='text' size="20" id="SourcePortEnd" name="SourcePortEnd" maxLength=15></td>
                      </tr>                      
                      <% end if %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter64Text","s")%></td>
                        <td> <input type='text' size="20" id="DestIPStart" name="DestIPStart" maxLength=15></td>
                      </tr>
                      <tr <% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>style="display:none"<% end if %>> 
                        <td><%tcWebApi_get("String_Entry","PortFilter65Text","s")%></td>
                        <td><input type='text' size="20" id="DestIpMask" name="DestIpMask" maxLength=15></td>
                      </tr>
                      <% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter66Text","s")%></td>
                        <td><input type='text' size="20" id="DestIPEnd" name="DestIPEnd" maxLength=15></td>
                      </tr>                      
                      <% end if %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter67Text","s")%></td>
                        <td><input type='text' size="20" id="DestPortStart" name="DestPortStart" maxLength=11></td>
                      </tr>
                      <% if tcwebApi_get("WebCustom_Entry","isCMCCSupported","h") = "Yes" then %>
                      <tr> 
                        <td><%tcWebApi_get("String_Entry","PortFilter68Text","s")%></td>
                        <td><input type='text' size="20" id="DestPortEnd" name="DestPortEnd" maxLength=15></td>
                      </tr>                      
                      <% end if %>
                    </table>
                    <div id='interface' style="display:none;"> 
                      <table width="400"  border="0" cellpadding="0" cellspacing="0">
                        <tr> 
                          <td width="79"><%tcWebApi_get("String_Entry","PortFilter69Text","s")%></td>
                          <td width="321"> <select id="WanPath" name="WanPath" size="1" onChange="ChangeInface()">
                              <script language="JavaScript" type="text/javascript">
							for (i = 0; i < nEntryNum; i++)
							{
								document.write('<option value=' + vEntryValue[i] + '>' + vEntryName[i] + '</option>');
							}
							  
							</script>
                            </select></td>
                        </tr>
                        <tr> 
                          <td  width="79" height="23"><%tcWebApi_get("String_Entry","PortFilter70Text","s")%></td>
                          <td><div id="NatState">&nbsp;</div></td>
                        </tr>
                      </table>
                    </div>
                    <br></p>
                    </form>
                            </td>
                <td width="10">&nbsp;</td>
              </tr>
              <tr> 
                <td width="10">&nbsp;</td>
                <td></td>
                <td width="10">&nbsp;</td>
              </tr>
            </table></td>
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
      </table></form>
      </td>
  </tr>
  <tr> 
    <td height="1"> <table border="0" cellpadding="0" cellspacing="0" width="808" height="35" id="table7">
        <tr> 
          <td width="162" bgcolor="#EF8218">　</td>
          <td width="278" bgcolor="#427594">　</td>
          <td width="196" bgcolor="#427594"> <p align="center"><img src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width="80" height="23" border="0" id="btnOK" onClick="btnSaveIP()">&nbsp;&nbsp;<a href="#"><img src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" width="80" height="23" border="0" id="btnCancel" onClick="RefreshPage()"> 
          </td>
          <td width="170" bgcolor="#313031">　</td>
        </tr>
      </table></td>
  </tr>
</table>
</body>
</html>
