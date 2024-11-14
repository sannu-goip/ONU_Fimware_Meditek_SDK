<%
if Request_Form("SaveFlag") = "1" then
	if Request_Form("FlagEntry") = "0" then
	TCWebApi_set("StaticNat_Entry0","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry0","Interface","wanName")
	TCWebApi_set("StaticNat_Entry0","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry0")
	end if
	
	if Request_Form("FlagEntry") = "1" then
	TCWebApi_set("StaticNat_Entry1","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry1","Interface","wanName")
	TCWebApi_set("StaticNat_Entry1","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry1")

	end if

	if Request_Form("FlagEntry") = "2" then
	TCWebApi_set("StaticNat_Entry2","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry2","Interface","wanName")
	TCWebApi_set("StaticNat_Entry2","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry2")

	end if

	if Request_Form("FlagEntry") = "3" then
	TCWebApi_set("StaticNat_Entry3","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry3","Interface","wanName")
	TCWebApi_set("StaticNat_Entry3","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry3")

	end if

	if Request_Form("FlagEntry") = "4" then
	TCWebApi_set("StaticNat_Entry4","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry4","Interface","wanName")
	TCWebApi_set("StaticNat_Entry4","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry4")

	end if

	if Request_Form("FlagEntry") = "5" then
	TCWebApi_set("StaticNat_Entry5","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry5","Interface","wanName")
	TCWebApi_set("StaticNat_Entry5","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry5")

	end if

	if Request_Form("FlagEntry") = "6" then
	TCWebApi_set("StaticNat_Entry6","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry6","Interface","wanName")
	TCWebApi_set("StaticNat_Entry6","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry6")

	end if

	if Request_Form("FlagEntry") = "7" then
	TCWebApi_set("StaticNat_Entry7","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry7","Interface","wanName")
	TCWebApi_set("StaticNat_Entry7","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry7")

	end if

	if Request_Form("FlagEntry") = "8" then
	TCWebApi_set("StaticNat_Entry8","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry8","Interface","wanName")
	TCWebApi_set("StaticNat_Entry8","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry8")

	end if

	if Request_Form("FlagEntry") = "9" then
	TCWebApi_set("StaticNat_Entry9","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry9","Interface","wanName")
	TCWebApi_set("StaticNat_Entry9","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry9")

	end if

	if Request_Form("FlagEntry") = "10" then
	TCWebApi_set("StaticNat_Entry10","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry10","Interface","wanName")
	TCWebApi_set("StaticNat_Entry10","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry10")

	end if

	if Request_Form("FlagEntry") = "11" then
	TCWebApi_set("StaticNat_Entry11","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry11","Interface","wanName")
	TCWebApi_set("StaticNat_Entry11","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry11")

	end if

	if Request_Form("FlagEntry") = "12" then
	TCWebApi_set("StaticNat_Entry12","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry12","Interface","wanName")
	TCWebApi_set("StaticNat_Entry12","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry12")

	end if

	if Request_Form("FlagEntry") = "13" then
	TCWebApi_set("StaticNat_Entry13","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry13","Interface","wanName")
	TCWebApi_set("StaticNat_Entry13","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry13")

	end if

	if Request_Form("FlagEntry") = "14" then
	TCWebApi_set("StaticNat_Entry14","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry14","Interface","wanName")
	TCWebApi_set("StaticNat_Entry14","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry14")

	end if

	if Request_Form("FlagEntry") = "15" then
	TCWebApi_set("StaticNat_Entry15","LocalIP","LocalIP")
	TCWebApi_set("StaticNat_Entry15","Interface","wanName")
	TCWebApi_set("StaticNat_Entry15","WanIP","WanIP")
	tcWebApi_constSet("StaticNat_Common","Flag","add")
	TCWebApi_commit("StaticNat_Entry15")

	end if

	TCWebApi_save() 
end if
%>

<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" href='/JS/stylemain.css' type='text/css'>
<title>one-to-one</title>
<script language="javascript" src="/JS/menu.js"></script>
<script language="javascript" src="/JS/util.js"></script>
<script language="JavaScript" type="text/JavaScript">
<% if Request_Form("SaveFlag") = "1" then %>
	location.replace("/cgi-bin/nat-onetoone.asp");
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
欢迎您！
</td>
<td width="50" valign="bottom"><a href="/cgi-bin/logout.cgi" target="_top" onClick="DoLogout()">
<span class="logout">
退出
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
<input name="Selected_Menu" type="hidden" id="Selected_Menu" value="网络->one-to-one">
</font></b><span class="GatewayName">网关名称:
<script language="javascript">
document.write(top.gateWayName);
</script>
</span></font></td>
<td height="33" width="211" bgcolor="#EF8218">
<p align="center" class="GatewayType">型号:
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
var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var vEntryName = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var vEntryValue = vArrayStr.split(',');
<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "4") %>
vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var Wan_IPVERSION = vArrayStr.split(',');
<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "1") %>
var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var vEntryjIp = vArrayStr.split(',');


vEntryName = SortUtil.UpdateWanIFName( vEntryName, nEntryNum );
vEntryValue = SortUtil.SortMyArray( vEntryValue );
Wan_IPVERSION = SortUtil.SortMyArray( Wan_IPVERSION );
vEntryjIp = SortUtil.SortMyArray( vEntryjIp );

var routecount = 16;
var StaStaticNat_Entry = 0;
function stStaticRoute(domain,DestIPAddress)
{
	this.domain = domain;
	this.DestIPAddress = DestIPAddress;
}
function getStaticRouteInfo()
{
	var	nCurTemp = 0;
	var	vLocalIPAddress = new Array(routecount);
	var	vcurLinks = new Array(routecount);

	vLocalIPAddress[0] = "<% tcWebApi_get("StaticNat_Entry0","LocalIP","s") %>";
	vLocalIPAddress[1] = "<% tcWebApi_get("StaticNat_Entry1","LocalIP","s") %>";
	vLocalIPAddress[2] = "<% tcWebApi_get("StaticNat_Entry2","LocalIP","s") %>";
	vLocalIPAddress[3] = "<% tcWebApi_get("StaticNat_Entry3","LocalIP","s") %>";
	vLocalIPAddress[4] = "<% tcWebApi_get("StaticNat_Entry4","LocalIP","s") %>";
	vLocalIPAddress[5] = "<% tcWebApi_get("StaticNat_Entry5","LocalIP","s") %>";
	vLocalIPAddress[6] = "<% tcWebApi_get("StaticNat_Entry6","LocalIP","s") %>";
	vLocalIPAddress[7] = "<% tcWebApi_get("StaticNat_Entry7","LocalIP","s") %>";
	vLocalIPAddress[8] = "<% tcWebApi_get("StaticNat_Entry8","LocalIP","s") %>";
	vLocalIPAddress[9] = "<% tcWebApi_get("StaticNat_Entry9","LocalIP","s") %>";
	vLocalIPAddress[10] = "<% tcWebApi_get("StaticNat_Entry10","LocalIP","s") %>";
	vLocalIPAddress[11] = "<% tcWebApi_get("StaticNat_Entry11","LocalIP","s") %>";
	vLocalIPAddress[12] = "<% tcWebApi_get("StaticNat_Entry12","LocalIP","s") %>";
	vLocalIPAddress[13] = "<% tcWebApi_get("StaticNat_Entry13","LocalIP","s") %>";
	vLocalIPAddress[14] = "<% tcWebApi_get("StaticNat_Entry14","LocalIP","s") %>";
	vLocalIPAddress[15] = "<% tcWebApi_get("StaticNat_Entry15","LocalIP","s") %>";

	for(var i=0; i<routecount; i++)
	{
		if(vLocalIPAddress[i] != "N/A")
		{
			vcurLinks[nCurTemp++] = new stStaticRoute(i, vLocalIPAddress[i]);
		}
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

function getonetooneentry()
{
	var	nCurTemp = 0;
	var	vLocalIPAddress = new Array(routecount);
	var	vcurLinks = new Array(routecount);

	vLocalIPAddress[0] = "<% tcWebApi_get("StaticNat_Entry0","LocalIP","s") %>";
	vLocalIPAddress[1] = "<% tcWebApi_get("StaticNat_Entry1","LocalIP","s") %>";
	vLocalIPAddress[2] = "<% tcWebApi_get("StaticNat_Entry2","LocalIP","s") %>";
	vLocalIPAddress[3] = "<% tcWebApi_get("StaticNat_Entry3","LocalIP","s") %>";
	vLocalIPAddress[4] = "<% tcWebApi_get("StaticNat_Entry4","LocalIP","s") %>";
	vLocalIPAddress[5] = "<% tcWebApi_get("StaticNat_Entry5","LocalIP","s") %>";
	vLocalIPAddress[6] = "<% tcWebApi_get("StaticNat_Entry6","LocalIP","s") %>";
	vLocalIPAddress[7] = "<% tcWebApi_get("StaticNat_Entry7","LocalIP","s") %>";
	vLocalIPAddress[8] = "<% tcWebApi_get("StaticNat_Entry8","LocalIP","s") %>";
	vLocalIPAddress[9] = "<% tcWebApi_get("StaticNat_Entry9","LocalIP","s") %>";
	vLocalIPAddress[10] = "<% tcWebApi_get("StaticNat_Entry10","LocalIP","s") %>";
	vLocalIPAddress[11] = "<% tcWebApi_get("StaticNat_Entry11","LocalIP","s") %>";
	vLocalIPAddress[12] = "<% tcWebApi_get("StaticNat_Entry12","LocalIP","s") %>";
	vLocalIPAddress[13] = "<% tcWebApi_get("StaticNat_Entry13","LocalIP","s") %>";
	vLocalIPAddress[14] = "<% tcWebApi_get("StaticNat_Entry14","LocalIP","s") %>";
	vLocalIPAddress[15] = "<% tcWebApi_get("StaticNat_Entry15","LocalIP","s") %>";
			
	for(var i=0; i<routecount; i++)
	{

		if(vLocalIPAddress[i] == "N/A")
		{
			StaStaticNat_Entry = i;
			return true;
		}
	}
	alert('只能配置16条one to one规则！');
	return;
}

function LoadFrame()
{
	resetEBooValueCookie();
	document.ConfigForm.SaveFlag.value = "0";
}

function CheckForm(Form)
{
	var	vLocalIPAddress = new Array(routecount);
	var	vWanIPAddress = new Array(routecount);
	var LocalIPaddress = document.ConfigForm.LocalIP.value;
  	var LocalIPaddrParts = LocalIPaddress.split('.');
	var WanIPaddress = document.ConfigForm.WanIP.value;
  	var WanIPaddrParts = WanIPaddress.split('.');
	var WanIP = "";
	var i = 0;
	var LocalIP = "<% tcWebApi_get("Lan_Entry0","IP","s") %>";
  	var LocalIPParts = LocalIP.split('.');

	if ( isValidIpAddress(LocalIPaddress) == false
	|| isDeIpAddress(LocalIPaddress) == true
	|| isBroadcastIpAddress(LocalIPaddress) == true
	|| isLoopIpAddress(LocalIPaddress) == true 
	|| LocalIPaddrParts[3] == 0 )
	{
		alert('私网IP "' + LocalIPaddress + '" 是无效的IP地址。');
		return;
	}
	else if ( isValidIpAddress(WanIPaddress) == false
	|| isDeIpAddress(WanIPaddress) == true
	|| isBroadcastIpAddress(WanIPaddress) == true
	|| isLoopIpAddress(WanIPaddress) == true 
	|| WanIPaddrParts[3] == 0 )
	{
		alert('Wan IP "' + WanIPaddress + '" 是无效的IP地址。');
		return;
	}
	else if(LocalIPaddrParts[0] != LocalIPParts[0] || LocalIPaddrParts[1] != LocalIPParts[1] || LocalIPaddrParts[2] != LocalIPParts[2])
	{
		alert('私网IP必须与Lan IP同网段');
		return;
	}
	else 
	{
		for(i = 0; i < 16; i++)
		{
			if(vEntryName[i] == document.ConfigForm.wanId.value)
			{
				WanIP = vEntryjIp[i];
				document.ConfigForm.wanName.value = vEntryValue[i];
				if((WanIP != "") && (WanIP != "N/A"))
				{
					var WanIPParts = WanIP.split('.');
					if(WanIPaddrParts[0] != WanIPParts[0] || WanIPaddrParts[1] != WanIPParts[1] || WanIPaddrParts[2] != WanIPParts[2])
					{
						alert('Wan IP必须与Wan连接的Wan IP同网段');
						return;
					}
				}
			}
		}
	}

	vLocalIPAddress[0] = "<% tcWebApi_get("StaticNat_Entry0","LocalIP","s") %>";
	vLocalIPAddress[1] = "<% tcWebApi_get("StaticNat_Entry1","LocalIP","s") %>";
	vLocalIPAddress[2] = "<% tcWebApi_get("StaticNat_Entry2","LocalIP","s") %>";
	vLocalIPAddress[3] = "<% tcWebApi_get("StaticNat_Entry3","LocalIP","s") %>";
	vLocalIPAddress[4] = "<% tcWebApi_get("StaticNat_Entry4","LocalIP","s") %>";
	vLocalIPAddress[5] = "<% tcWebApi_get("StaticNat_Entry5","LocalIP","s") %>";
	vLocalIPAddress[6] = "<% tcWebApi_get("StaticNat_Entry6","LocalIP","s") %>";
	vLocalIPAddress[7] = "<% tcWebApi_get("StaticNat_Entry7","LocalIP","s") %>";
	vLocalIPAddress[8] = "<% tcWebApi_get("StaticNat_Entry8","LocalIP","s") %>";
	vLocalIPAddress[9] = "<% tcWebApi_get("StaticNat_Entry9","LocalIP","s") %>";
	vLocalIPAddress[10] = "<% tcWebApi_get("StaticNat_Entry10","LocalIP","s") %>";
	vLocalIPAddress[11] = "<% tcWebApi_get("StaticNat_Entry11","LocalIP","s") %>";
	vLocalIPAddress[12] = "<% tcWebApi_get("StaticNat_Entry12","LocalIP","s") %>";
	vLocalIPAddress[13] = "<% tcWebApi_get("StaticNat_Entry13","LocalIP","s") %>";
	vLocalIPAddress[14] = "<% tcWebApi_get("StaticNat_Entry14","LocalIP","s") %>";
	vLocalIPAddress[15] = "<% tcWebApi_get("StaticNat_Entry15","LocalIP","s") %>";
	
	vWanIPAddress[0] = "<% tcWebApi_get("StaticNat_Entry0","WanIP","s") %>";
	vWanIPAddress[1] = "<% tcWebApi_get("StaticNat_Entry1","WanIP","s") %>";
	vWanIPAddress[2] = "<% tcWebApi_get("StaticNat_Entry2","WanIP","s") %>";
	vWanIPAddress[3] = "<% tcWebApi_get("StaticNat_Entry3","WanIP","s") %>";
	vWanIPAddress[4] = "<% tcWebApi_get("StaticNat_Entry4","WanIP","s") %>";
	vWanIPAddress[5] = "<% tcWebApi_get("StaticNat_Entry5","WanIP","s") %>";
	vWanIPAddress[6] = "<% tcWebApi_get("StaticNat_Entry6","WanIP","s") %>";
	vWanIPAddress[7] = "<% tcWebApi_get("StaticNat_Entry7","WanIP","s") %>";
	vWanIPAddress[8] = "<% tcWebApi_get("StaticNat_Entry8","WanIP","s") %>";
	vWanIPAddress[9] = "<% tcWebApi_get("StaticNat_Entry9","WanIP","s") %>";
	vWanIPAddress[10] = "<% tcWebApi_get("StaticNat_Entry10","WanIP","s") %>";
	vWanIPAddress[11] = "<% tcWebApi_get("StaticNat_Entry11","WanIP","s") %>";
	vWanIPAddress[12] = "<% tcWebApi_get("StaticNat_Entry12","WanIP","s") %>";
	vWanIPAddress[13] = "<% tcWebApi_get("StaticNat_Entry13","WanIP","s") %>";
	vWanIPAddress[14] = "<% tcWebApi_get("StaticNat_Entry14","WanIP","s") %>";
	vWanIPAddress[15] = "<% tcWebApi_get("StaticNat_Entry15","WanIP","s") %>";

	for(i = 0; i < routecount; i++)
	{
		if(vLocalIPAddress[i] == document.ConfigForm.LocalIP.value)
		{
			alert('私网IP不能相同');
			return;
		}
		else if(vWanIPAddress[i] == document.ConfigForm.WanIP.value)
		{
			alert('Wan IP不能相同');
			return;
		}
	}
	return true;
}
function SubmitForm()
{
	var WanIndex = "<% tcWebApi_get("WanInfo_Common","DefRouteIndexv4","s") %>";
	for(var i=0; i< j; i++)
	{
		if ( document.ConfigForm.curSetIndex.value == WanNameObjs[i].IfaceDomain )
		{
			document.ConfigForm.wanId.value = WanNameObjs[i].IfaceName;
		}
	}

	if (true == CheckForm(document.ConfigForm))
	{
		var vForm = document.ConfigForm;
		if(true == getonetooneentry())
		{
			if(document.ConfigForm.wanId.value != "")
			{
				if(WanIndex != document.ConfigForm.curSetIndex.value)
				{
					alert('该连接不是默认路由，请配置端口绑定');
				}
				vForm.SaveFlag.value = "1";
				vForm.FlagEntry.value = StaStaticNat_Entry;
				vForm.submit();
			}
			else
			{
				alert('请选择Wan连接');
			}
		}
	}

}
function Cancle()
{
	location.replace("/cgi-bin/nat-onetoone.asp");
}

var WanNameObjs;
vArrayStr = "<% tcWebApi_get("WanInfo_Common","ValidIFIndex","s") %>";
var vEntryIndex = vArrayStr.split(',');
var CycleV = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
var IFIdxArray = CycleV.split(',');
function WanNameObject(IFDomain, IFName, IFIdx)
{
	this.IfaceDomain = IFDomain;
	this.IfaceName = IFName;
	this.IfaceIndex = IFIdx;
}
function WanIndexConstruction(domain,WanName)
{
	this.domain = domain;
	this.WanName = WanName;
}
function CheckWansActives()
{
	var	nCurTemp = 0;
	var	vcurLinks = new Array(nEntryNum);

	for(var i=0; i<nEntryNum; i++)
	{	
		vcurLinks[nCurTemp++] = new WanIndexConstruction(vEntryIndex[i], vEntryName[i]);
	}
	
	var	vObjRet = new Array(nCurTemp+1);
	for(var m=0; m<nCurTemp; m++)
	{
		vObjRet[m] = vcurLinks[m];
	}
	vObjRet[nCurTemp] = null;
	return vObjRet;
}
var	CurWan = CheckWansActives();
var WanIDNums = CurWan;
var nlength = WanIDNums.length-1;
var j = 0;

function WriteWanNameSelected()
{
	var i = 0;
	var isSel = 0;
	var gotopts = 0;
    var str = 0;
	
	if(nlength == 1) 
		document.ConfigForm.curSetIndex.value = WanIDNums[0].domain;
	
	WanNameObjs = new Array(nlength)
	for( i=0; i< nlength; i++)
	{
		str = WanIDNums[i].WanName;
		if((( str.indexOf('OTHER') >= 0 ) || ( str.indexOf('INTERNET') >= 0 )) && ( str.indexOf('_R_') >= 0 ))
		{
			WanNameObjs[j] = new WanNameObject(WanIDNums[i].domain, WanIDNums[i].WanName, IFIdxArray[i]);
			j++;
		}
	}
	WanNameObjs.sort(WanNameSort);
	
	with (getElById('wanIndex'))
	{
		document.ConfigForm.curSetIndex.value = WanNameObjs[0].IfaceDomain;
		for( i=0; i< j; i++)
		{
			var opt = new Option(WanNameObjs[i].IfaceName, WanNameObjs[i].IfaceDomain);
			if ( document.ConfigForm.curSetIndex.value == WanNameObjs[i].IfaceDomain )
			{
				opt.selected = true
				isSel = i;
			}
			options.add ( opt );
			gotopts = 1;
		}
		if ( gotopts )
		{
			options[isSel].setAttribute('selected', 'true');
		}
	}
}
function WanIdChange()
{
	document.ConfigForm.curSetIndex.value = getSelectVal('wanIndex');
}

</script>
<table height="100%" border="0" cellpadding="0" cellspacing="0">
<tr>
<td width="157" bgcolor="#EF8218" height="30"><p class="Item_L1">One-to-One NAT</p></td>
<td width="7" bgcolor="#EF8218">　</td>
<td width="474">　</td>
<td width="170" rowspan="5" valign="top" background="/img/panel4.gif"><table width="100%" border="0" cellspacing="0" cellpadding="20" height='100%'>
<tr>
<td valign='top'><a href="../cgi-bin/help_content.asp#One to One" target="_blank"><img src="/img/help.gif" width="40" height="34" border="0"></a></td>
</tr>
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
<form name="ConfigForm" action="/cgi-bin/nat-onetooneadd.asp" method="post">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="10">&nbsp;</td>
<td>
<table border="0" cellpadding="0" cellspacing="0">
<tr>
<td width="100">私网IP:</td>
<td><input type='text' name='LocalIP' maxlength='15' value="<%asp_Write("") %>">
	<input type="hidden" name="FlagEntry" value="0">
	</td>
</tr>
<tr>
<td>选取连接:</td>
 <td><LABEL> 
              <select onChange=WanIdChange() name=wanIndex id='wanIndex' >
              </select>
              <input type="hidden" name="curSetIndex" value="<% tcWebApi_get("WanInfo_Common","CurIFIndex","s") %>">
                <script language=JavaScript type=text/javascript>
  WriteWanNameSelected();
</script>
              <input type="hidden" name="Wan_Flag" value="0">
              <input type="hidden" name="EnCAPFlag" value="PPPoE">
              <input type="hidden" name="PPPGetIpFlag" value="Dynamic">
              <input type="hidden" name="ConnectionFlag" value="<% tcWebApi_get("WanInfo_WanIF","CONNECTION","s") %>">
              <input type="hidden" name="Enable_Flag" value="Yes">
              <input type="hidden" name="Disable_Flag" value="No">
              <input type="hidden" name="afterdeleteFlag" value="0">
              <input type="hidden" name="OperatorStyle" value="Add">
              <input type="hidden" name="dhcpv6pdflag" value="Yes">
              <input type="hidden" name="pppManualStatus_Flag" value="<% tcWebApi_get("WanInfo_WanIF","PPPManualStatus","s") %>">
              <input type="hidden" name="wanName" value="">
              <input type="hidden" name="wanId" value="">
               </LABEL></td>
</tr>
<tr>
<td>Wan IP:</td>
<td><input type='text' name='WanIP' maxlength='15' value="<%asp_Write("")%>"></td>
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
<p align="center"><img src="/img/ok.gif" width="80" height="23" border="0" id="btnOK" onClick="SubmitForm()">&nbsp;&nbsp;<img src="/img/cancel.gif" width="80" height="23" border="0" id="btnCancel" onClick="Cancle()"> </td>
<td width="170" bgcolor="#313031">　</td>
</tr>
</table>
</td>
</tr>
</table>
</body>
</html>
