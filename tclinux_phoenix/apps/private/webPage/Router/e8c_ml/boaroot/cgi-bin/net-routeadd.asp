<%
if Request_Form("SaveFlag") = "1" then
	TCWebApi_set("WebCurSet_Entry","route_id","add_num")
	TCWebApi_set("Route_Entry","DST_IP","DestIPAddress")
	TCWebApi_set("Route_Entry","Sub_mask","DestSubnetMask")
	if Request_Form("gwflag") = "Yes" then
		TCWebApi_set("Route_Entry","Gateway","GatewayIPAddress")
	end if
	if Request_Form("ifflag") = "Yes" then
		TCWebApi_set("Route_Entry","Device","WanInterfaceFlag")
	end if
	TCWebApi_set("Route_Entry","metric","metricFlag")

	TCWebApi_commit("Route_Entry")
	TCWebApi_save() 
elseif Request_Form("SaveFlag") = "2" then
		TCWebApi_set("WebCurSet_Entry","wan_pvc","Interface")
end if
%>

<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" href='/JS/stylemain.css' type='text/css'>
<title><%tcWebApi_get("String_Entry","StaticRouteText","s")%></title>
<script language="javascript" src="/JS/menu.js"></script>
<script language="javascript" src="/JS/jquery.js"></script>
<script language="javascript" src="/JS/util.js"></script>
<script language="JavaScript" type="text/JavaScript">
<% if Request_Form("SaveFlag") = "1" then %>
	location.replace("/cgi-bin/net-routeset.asp");
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
<%tcWebApi_get("String_Entry","TitleWelcomeText","s")%>
</td>
<td width="50" valign="bottom"><a href="/cgi-bin/login.asp" id=logOut name=logOut target="_top" onClick="DoLogout()">
<span class="logout">
<%tcWebApi_get("String_Entry","TitleLogOutText","s")%>
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
<p align="right"><font color="#FFFFFF" face="<%tcWebApi_get("String_Entry","RomanText","s")%>"><b><font face="<%tcWebApi_get("String_Entry","RomanText","s")%>" size="6" color="#FFFFFF">
<input name="Selected_Menu" type="hidden" id="Selected_Menu" value="<%tcWebApi_get("String_Entry","ContentNetText","s")%>-><%tcWebApi_get("String_Entry","ContentRouteText","s")%>">
</font></b><span class="GatewayName"><%tcWebApi_get("String_Entry","TitleGateWayNameText","s")%>
<script language="javascript">
document.write(top.gateWayName);
</script>
</span></font></td>
<td height="33" width="211" bgcolor="#EF8218">
<p align="center" class="GatewayType"><%tcWebApi_get("String_Entry","TitleGateWayTypeText","s")%>
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

	var nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
	// num 0
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryName = vArrayStr.split(',');
	// num 1
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryValue = vArrayStr.split(',');
	// num 2~5
	<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "4") %>
	// num 6
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var Wan_IPVERSION = vArrayStr.split(',');
	
	vEntryName = SortUtil.UpdateWanIFName( vEntryName, nEntryNum );
	vEntryValue = SortUtil.SortMyArray( vEntryValue );
	Wan_IPVERSION = SortUtil.SortMyArray( Wan_IPVERSION );
	
var host = [["InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1","192.168.1.1","255.255.255.0"],null];
var IpAddress = host[0][1];
var SubnetMask = host[0][2];
function stWan(domain,wanName,CntType)
{
this.domain = domain;
var list = domain.split('.');
this.key = 	list[4];
this.wanName = wanName;
this.CntType = CntType;
this.LinkType = '';
}
function stDsl(domain,LinkType)
{
this.domain = domain;
var list = domain.split('.');
this.key = 	list[4];
this.LinkType = LinkType;
}
var WanPPP = new Array(new stWan("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.4.WANPPPConnection.2","1_TR069_INTERNET_R_0_35","IP_Routed"),null);
var WanIP = new Array(null);
var CntPvc = new Array(new stDsl("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.4.WANDSLLinkConfig","EoA"),null);
if (WanPPP.length > 1)
AssociateParam('WanPPP','CntPvc','LinkType');
if (WanIP.length > 1)
AssociateParam('WanIP','CntPvc','LinkType');
var Wan = new Array();
for (i = 0; i < WanIP.length-1; i++)
{
	Wan[i] = WanIP[i];
}
for (j = 0; j < WanPPP.length-1; i++,j++)
{
	Wan[i] =  WanPPP[j];
}

var routecount = 16;
function stStaticRoute(domain,DestIPAddress)
{
	this.domain = domain;
	this.DestIPAddress = DestIPAddress;
}
function getStaticRouteInfo()
{
	var	nCurTemp = 0;
	var	vDestIPAddress = new Array(routecount);
	var	vcurLinks = new Array(routecount);
	
	vDestIPAddress[0] = "<% tcWebApi_get("Route_Entry0","DST_IP","s") %>";
	vDestIPAddress[1] = "<% tcWebApi_get("Route_Entry1","DST_IP","s") %>";
	vDestIPAddress[2] = "<% tcWebApi_get("Route_Entry2","DST_IP","s") %>";
	vDestIPAddress[3] = "<% tcWebApi_get("Route_Entry3","DST_IP","s") %>";
	vDestIPAddress[4] = "<% tcWebApi_get("Route_Entry4","DST_IP","s") %>";
	vDestIPAddress[5] = "<% tcWebApi_get("Route_Entry5","DST_IP","s") %>";
	vDestIPAddress[6] = "<% tcWebApi_get("Route_Entry6","DST_IP","s") %>";
	vDestIPAddress[7] = "<% tcWebApi_get("Route_Entry7","DST_IP","s") %>";
	vDestIPAddress[8] = "<% tcWebApi_get("Route_Entry8","DST_IP","s") %>";
	vDestIPAddress[9] = "<% tcWebApi_get("Route_Entry9","DST_IP","s") %>";
	vDestIPAddress[10] = "<% tcWebApi_get("Route_Entry10","DST_IP","s") %>";
	vDestIPAddress[11] = "<% tcWebApi_get("Route_Entry11","DST_IP","s") %>";
	vDestIPAddress[12] = "<% tcWebApi_get("Route_Entry12","DST_IP","s") %>";
	vDestIPAddress[13] = "<% tcWebApi_get("Route_Entry13","DST_IP","s") %>";
	vDestIPAddress[14] = "<% tcWebApi_get("Route_Entry14","DST_IP","s") %>";
	vDestIPAddress[15] = "<% tcWebApi_get("Route_Entry15","DST_IP","s") %>";
	
	for(var i=0; i<routecount; i++)
	{
		if(vDestIPAddress[i] != "N/A")
			vcurLinks[nCurTemp++] = new stStaticRoute(i, vDestIPAddress[i]);
	}
	var	vObjRet = new Array(nCurTemp+1);
	for(var m=0; m<nCurTemp; m++)
	{
		vObjRet[m] = vcurLinks[m];
	}
	vObjRet[nCurTemp] = null;
	return vObjRet;
}
var StaticRouteInfo = getStaticRouteInfo();

/*function WriteOption()
{
for (i = 0; i < Wan.length; i++)
{
if (Wan[i].CntType == 'IP_Routed' && (Wan[i].wanName.indexOf("TR069") == -1))
{
document.write('<option value="' + Wan[i].domain + '"' + ' id="routewan_' + i + '">'
+ Wan[i].wanName + '</option>');
}
}
}*/
function btnClear()
{
with ( document.forms[0] )
{
DestIPAddress.value = '';
DestSubnetMask.value = '';
GatewayIPAddress.value = '';
}
}
function LoadFrame()
{
	document.ConfigForm.SaveFlag.value = "0";
}
function getMostRightPosOf1(str)
{
for (i = str.length - 1; i >= 0; i--)
{
if (str.charAt(i) == '1')
{
break;
}
}
return i;
}
function getBinaryString(str)
{
var numArr = [128, 64, 32, 16, 8, 4, 2, 1];
var addrParts = str.split('.');
if (addrParts.length < 3)
{
return "00000000000000000000000000000000";
}
var binstr = '';
for (i = 0; i < 4; i++)
{
var num = parseInt(addrParts[i])
for ( j = 0; j < numArr.length; j++ )
{
if ( (num & numArr[j]) != 0 )
{
binstr += '1';
}
else
{
binstr += '0';
}
}
}
return binstr;
}
function isMatchedIpAndMask(ip, mask)
{
var locIp = getBinaryString(ip);
var locMask = getBinaryString(mask);
if (locIp.length != locMask.length)
{
return false;
}
var most_right_pos_1 = getMostRightPosOf1(locMask);
if (most_right_pos_1 == -1)
{
if (loaIp == '00000000000000000000000000000000')
{
return true;
}
else
{
return false;
}
}
for (i = most_right_pos_1 + 1; i < locIp.length; i++)
{
if (locIp.charAt(i) != '0')
{
return false;
}
}
return true;
}
function CheckForm(Form)
{
	for (i = 0; i < StaticRouteInfo.length - 1; i++)
	{
		if (StaticRouteInfo[i].DestIPAddress == getValue('DestIPAddress'))
		{
			alert('<%tcWebApi_get("String_Entry","IPAddressUsedText","s")%>');
			return false;
		}
	}
	with ( document.forms[0] )
{
	var DestIp = DestIPAddress.value;
	var index1 = IpAddress.lastIndexOf('.');
	var index2 = DestIp.lastIndexOf('.');
	if ( isValidIpAddress(DestIp) == false
	|| isDeIpAddress(DestIp) == true
	|| isBroadcastIpAddress(DestIp) == true
	|| isLoopIpAddress(DestIp) == true )
	{
		alert('<%tcWebApi_get("String_Entry","DestinationText","s")%> "' + DestIPAddress.value + '" <%tcWebApi_get("String_Entry","IsInvalidText","s")%>');
		return false;
	}
	if (IpAddress == DestIp
	|| (DestIp.substr(index1 + 1) == '0' && IpAddress.substr(0,index1) == DestIp.substr(0,index2)))
	{
		alert('<%tcWebApi_get("String_Entry","DestinationIPInvalidText","s")%>');
		return false;
	}
//Form.addParameter('DestIPAddress',DestIPAddress.value);
	if ( isValidSubnetMask(DestSubnetMask.value) == false &&  DestSubnetMask.value != '255.255.255.255')
	{
		alert('<%tcWebApi_get("String_Entry","SubnetMaskText","s")%>"' + DestSubnetMask.value + '" <%tcWebApi_get("String_Entry","IsInvalidText","s")%>');
		return false;
	}
//Form.addParameter('DestSubnetMask',DestSubnetMask.value);
	if (isMatchedIpAndMask(DestIPAddress.value, DestSubnetMask.value) == false)
	{
		alert('<%tcWebApi_get("String_Entry","DestinationIPAddressText","s")%> "' + DestIPAddress.value + '"<%tcWebApi_get("String_Entry","AndSubnetMaskText","s")%>"' + DestSubnetMask.value + '" <%tcWebApi_get("String_Entry","NotMatchText","s")%>');
		return false;
	}
	if (gwStr.checked == false && ifStr.checked == false)
	{
		alert('<%tcWebApi_get("String_Entry","SelectUseText","s")%>');
		return false;
	}
	if (gwStr.checked == true)
	{
		if ( isAbcIpAddress(GatewayIPAddress.value) == false )
		{
			alert('<%tcWebApi_get("String_Entry","DefaultGatewayText","s")%>"' + GatewayIPAddress.value + '" <%tcWebApi_get("String_Entry","IsInvalidText","s")%>');
			return false;
		}
//Form.addParameter('GatewayIPAddress',GatewayIPAddress.value);
	}
	else
	{
//Form.addParameter('GatewayIPAddress','0.0.0.0');
	}
	
	if (ifStr.checked == true)
	{
		var idx = Interface.selectedIndex;
		var WANId = Interface.options[idx].id;
		if ((idx != Interface.length - 1) && (Interface.value.indexOf('nas') > -1) && (gwStr.checked == false))
		{
			alert('<%tcWebApi_get("String_Entry","IPoESelectText","s")%>');
			return false;
		}
//Form.addParameter('Interface',getSelectVal('Interface'));
	}
	else
	{
	}
}
//Form.endPrefix();
return true;
}
function SubmitForm()
{
//var Form = new webSubmitForm();
if (true == CheckForm(document.ConfigForm))
{
/*Form.setAction("addcfg.cgi?x=InternetGatewayDevice.Layer3Forwarding.Forwarding"
+ "&RequestFile=html/network/route.asp");
Form.submit();*/
var vForm = document.ConfigForm;
if(vForm.user_def_num.value >= 16)
{
	alert("<%tcWebApi_get("String_Entry","Define16RoutesText","s")%>");
	return;
}

	if(vForm.gwStr.checked)
		vForm.gwflag.value = "Yes";
	else vForm.gwflag.value = "No";
	
	if(vForm.ifStr.checked)
		vForm.ifflag.value = "Yes";
	else vForm.ifflag.value = "No";
	
	vForm.WanInterfaceFlag.value = vForm.Interface.value;
	vForm.SaveFlag.value = "1";
	if( true == setEBooValueCookie(vForm) )
		vForm.submit();
}

}
function Cancle()
{
location.replace("/cgi-bin/net-routeset.asp");
}
</script>
<table height="100%" border="0" cellpadding="0" cellspacing="0">
<tr>
<td width="157" bgcolor="#EF8218" height="30"><p class="Item_L1"><%tcWebApi_get("String_Entry","StaticRouteText","s")%></p></td>
<td width="7" bgcolor="#EF8218">　</td>
<td width="474">　</td>
<td width="170" rowspan="5" valign="top" background="/img/panel4.gif"><table width="100%" border="0" cellspacing="0" cellpadding="20" height='100%'>
<tr>
<td valign='top'><a href="../cgi-bin/help_content.asp#<%tcWebApi_get("String_Entry","RouteText","s")%>" target="_blank"><img src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width="40" height="34" border="0"></a></td>
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
<form name="ConfigForm" action="/cgi-bin/net-routeadd.asp" method="post">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="10">&nbsp;</td>
<td><p> <br>
                      <%tcWebApi_get("String_Entry","InputAddressText","s")%><br>
                      <input type="hidden" id="add_num" name="add_num" value="<% tcWebApi_get("Route","add_num","s") %>">
                      <input type="hidden" id="user_def_num" name="user_def_num" value="<% tcWebApi_get("Route","User_def_num","s") %>">
                      <input type="hidden" id="WanInterfaceFlag" name="WanInterfaceFlag" value="PVC0">
                      <input type="hidden" id="metricFlag" name="metricFlag" value="0">
                      <input type="hidden" id="gwflag" name="gwflag" value="No">
                      <input type="hidden" id="ifflag" name="ifflag" value="No">
                      <script language="JavaScript" type="text/JavaScript">
	
function WriteWanNameSelected()
{
	for (var i = 0; i < nEntryNum; i++)
	{
		if((vEntryName[i].indexOf('_INTERNET_')<0) || (vEntryName[i].indexOf('_B_')>-1)
		|| 'IPv6' == Wan_IPVERSION[i] )
				continue;

		document.write('<option value=' + vEntryValue[i] + '>' + vEntryName[i] + '</option>');
	}
}
function ChangeInterface()
{
	//document.ConfigForm.SaveFlag.value = "2";
	//document.ConfigForm.submit();
}
					  </script>
                      <br>
                      <br>
                    </p>
<table border="0" cellpadding="0" cellspacing="0">
<tr>
<td width="180"><%tcWebApi_get("String_Entry","DestinationIPAddressText","s")%></td>
<td><input type='text' id='DestIPAddress' name='DestIPAddress' maxlength='15' value="<%if TCWebApi_get("Route_Entry","DST_IP","h") <> "N/A" then TCWebApi_get("Route_Entry","DST_IP","s") else asp_Write("0.0.0.0") end if %>"></td>
</tr>
<tr>
<td><%tcWebApi_get("String_Entry","SubnetMaskText","s")%></td>
<td><input type='text' id='DestSubnetMask' name='DestSubnetMask' maxlength='15' value="<%if TCWebApi_get("Route_Entry","Sub_mask","h") <> "N/A" then TCWebApi_get("Route_Entry","Sub_mask","s") else asp_Write("0.0.0.0") end if %>"></td>
</tr>
</table>
<br>
<table>
<tr>
<td><input type="checkbox" id='gwStr' name='gwStr' <%if TCWebApi_get("Route_Entry","Gateway","h") <> "N/A" then asp_Write("checked") end if%>>
&nbsp;<%tcWebApi_get("String_Entry","UseGatewayText","s")%>IP<%tcWebApi_get("String_Entry","AddressText","s")%>&nbsp;&nbsp;</td>
<td><input type='text' maxlength='15' id='GatewayIPAddress' name='GatewayIPAddress' value="<%if TCWebApi_get("Route_Entry","Gateway","h") <> "N/A" then TCWebApi_get("Route_Entry","Gateway","s") end if%>"></td>
</tr>
<tr>
<td><input type="checkbox" id='ifStr' name='ifStr' <%if TcWebApi_get("Route_Entry","Device","h") <> "N/A" then asp_Write("checked")  end if%>>
&nbsp;<%tcWebApi_get("String_Entry","UseInterfaceText","s")%></td>
                        <td><select name='Interface'  id="select" style="width:153px" onChange="ChangeInterface()">
                            <script language="javascript">
WriteWanNameSelected();
document.writeln("  <option value='br0'> LAN/br0");
</script>
                          </select></td>
</tr>
</table>
<br>
<center>
</center>
<p><br>
<br>
</p></td>
<td width="10">&nbsp;</td>
</tr>
<tr>
<td width="10">&nbsp;</td>
<td><input type="hidden" name="SaveFlag" value="0"></td>
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
<p align="center"><img src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width="80" height="23" border="0" id="btnOK" onClick="SubmitForm()">&nbsp;&nbsp;<img src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" width="80" height="23" border="0" id="btnCancel" onClick="Cancle()"> </td>
<td width="170" bgcolor="#313031">　</td>
</tr>
</table>
</td>
</tr>
</table>
</body>
</html>
