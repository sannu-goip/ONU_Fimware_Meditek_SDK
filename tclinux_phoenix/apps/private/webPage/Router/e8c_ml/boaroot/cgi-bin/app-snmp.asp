<%
If Request_Form("Snmpflag")="1" then 
	TCWebApi_set("Snmpd_Entry","Active","SNMP_active")
	TCWebApi_set("Snmpd_Entry","rocommunity","SNMP_get")
	TCWebApi_set("Snmpd_Entry","rwcommunity","SNMP_set")
	
	if Request_Form("SnmpFullflag")="Yes" then 
		if  Request_Form("startTrapflag") <> "Yes" then 
			TCWebApi_set("Snmpd_Entry","sysName","SNMP_sysName")
			TCWebApi_set("Snmpd_Entry","sysContact","SNMP_sysContact")
			TCWebApi_set("Snmpd_Entry","sysLocation","SNMP_sysLocation")
		end if
		TCWebApi_set("Snmpd_Entry","trap2sink","SNMP_trapManagerIP")
		TCWebApi_set("Snmpd_Entry","trapsink","SNMP_trapManagerIP")
		if Request_Form("SnmpIPv6flag")="Yes" then
			TCWebApi_set("Snmpd_Entry","trap2sink_ipv6","SNMP_trapManagerIPv6")
		end if	
	end If

	if Request_Form("trustIPflag") = "Yes" then
		TCWebApi_set("Snmpd_Entry", "trustStartIP", "SNMP_trustStartIP")
		TCWebApi_set("Snmpd_Entry", "trustEndIP", "SNMP_trustEndIP")
		if Request_Form("SnmpIPv6flag") = "Yes" then
			TCWebApi_set("Snmpd_Entry", "trustStartIPv6", "SNMP_trustStartIPv6")
			TCWebApi_set("Snmpd_Entry", "trustEndIPv6", "SNMP_trustEndIPv6")
		end if
	end if
			
	if Request_Form("Snmpv3flag")="Yes" then
		TCWebApi_set("Snmpd_Entry","V3Enable","SNMPv3_enable")
		if Request_Form("SNMPv3_enable")="Yes" then
			TCWebApi_set("Snmpd_Entry","v3Name","SNMPv3_User")
			TCWebApi_set("Snmpd_Entry","access","Access_Permissions")
			TCWebApi_set("Snmpd_Entry","authProto","Auth_Proto")
			TCWebApi_set("Snmpd_Entry","authPasswd","Auth_Passwd")
			TCWebApi_set("Snmpd_Entry","privProto","Privacy_Proto")
			TCWebApi_set("Snmpd_Entry","privPasswd","Privacy_Passwd")
		end if
	end if
				
	tcWebApi_commit("Snmpd_Entry")
	TCWebApi_save()
End If
%>

<HTML>
<HEAD>
<TITLE>SNMP</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<META content="MSHTML 6.00.6000.16809" name=GENERATOR>
<SCRIPT language=JavaScript type=text/javascript>
function LoadFrame()
{	
	snmpShow();
}

function isValidIpAddrSnmp(ip1,ip2,ip3,ip4)
{
	if(ip1==0 || ip4==0 || ip4==255 || ip1==127 )		
		return false;	
	
	return true;	
}

function inValidIPAddrSnmp(Address)
{
  var address = Address.match("^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$");
  var digits;
  var i;

  if(address == null) { 
  	alert('<%tcWebApi_get("String_Entry","SNMPJS15Text","s")%>' + Address);
  	return true;
  }
  
  digits = address[0].split(".");
  for(i=0; i < 4; i++)
  {
  	if((Number(digits[i]) > 255 ) || (Number(digits[i]) < 0 ) || (Number(digits[0]) > 223) || (digits[i] == null))
  	{ 
  		alert('<%tcWebApi_get("String_Entry","SNMPJS15Text","s")%>' + Address);
  		return true;
  	}
  }
  if(!isValidIpAddrSnmp(digits[0],digits[1],digits[2],digits[3],false))
  {
		alert('<%tcWebApi_get("String_Entry","SNMPJS15Text","s")%>' + Address);
		return true;
  }
  return false;
}

function inValidIPv6AddrSnmp(Address1)
{
	var regExp = /^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$/;
	
	var Address = trim(Address1);
	var address = Address.match(regExp);
	if(address == null){		
		alert('<%tcWebApi_get("String_Entry","SNMPJS17Text","s")%>' + Address);
 		return true;
	}	
	return false;
}

function ipCheck() 
{
	var IP = document.forms[0].SNMP_trustStartIP.value;
	
	if ( document.forms[0].SNMP_trustStartIP.value == "" && document.forms[0].SNMP_trustEndIP.value == "" ) 
	{
		alert('<%tcWebApi_get("String_Entry","SNMPJS16Text","s")%>');		
		return true;
	}

	if(inValidIPAddrSnmp(IP))
		return true;

	if(document.forms[0].SNMP_trustStartIP!=null)
	{	
		var IP_SRC_END = document.forms[0].SNMP_trustEndIP.value;		
		if((IP_SRC_END != "255.255.255.255") && (inValidIPAddrSnmp(IP_SRC_END)))
			return true;
	
		var startIp = IP.split(".");
		var endIp = IP_SRC_END.split(".");
		var stIP = parseInt(startIp[0])<<24 | parseInt(startIp[1])<<16 | parseInt(startIp[2])<<8 | parseInt(startIp[3]);
		var edIP = parseInt(endIp[0])<<24 | parseInt(endIp[1])<<16 | parseInt(endIp[2])<<8 | parseInt(endIp[3]);
		stIP = stIP >>> 0;
		edIP = edIP >>> 0;									
		if(stIP > edIP)
		{	                 
				alert("<%tcWebApi_get("String_Entry","SNMPJS3Text","s")%>");
			return true;
		}				
	}
	
	return false;	
}		

function ipv6Check() 
{ 
	var startIP = document.forms[0].SNMP_trustStartIPv6.value;
	var endIP = document.forms[0].SNMP_trustEndIPv6.value;

	if ( startIP == "" && endIP == "" ) 
	{
		alert('<%tcWebApi_get("String_Entry","SNMPJS18Text","s")%>');		
		return true;
	}

	if(inValidIPv6AddrSnmp(startIP) || inValidIPv6AddrSnmp(endIP))
		return true;
	
	var startFullIP = getFullIpv6Address(startIP);
	var endFullIP = getFullIpv6Address(endIP);
	var startIntFullIP = startFullIP.split(":");
	var endIntFullIP = endFullIP.split(":");
	var index;
	var ret=false;
	if(startIntFullIP.length != 8) 
	{
			alert("<%tcWebApi_get("String_Entry","SNMPJS11Text","s")%>"+startIntFullIP.length);
		return true;
	}
	if(endIntFullIP.length != 8)  
	{
			alert("<%tcWebApi_get("String_Entry","SNMPJS12Text","s")%>"+endIntFullIP.length);
		return true;
	}
	for(index =0 ;index < startIntFullIP.length ;index++)
	{
		if(parseInt(startIntFullIP[index],16) < parseInt(endIntFullIP[index],16))
		{
			ret = false; 
			break;
		}
		else if(parseInt(startIntFullIP[index],16) > parseInt(endIntFullIP[index],16)) 
		{
				alert("<%tcWebApi_get("String_Entry","SNMPJS13Text","s")%>");
			ret = true; 
			break;
		}
	}
	
	return ret;
}

function CheckCommunityVlaue(COM)
{
	if(COM.indexOf(" ") == -1)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

function SNMPsave() 
{
	var ipversion = ipv4OnlyOrIpv6OnlyCheck();
	
	if ( document.SNMP_form.SNMP_active[1].checked )
	{
		document.SNMP_form.Snmpflag.value = 1;
		if( true == setEBooValueCookie(document.SNMP_form) )
		document.SNMP_form.submit();
		return true;
	}
	
	if(document.SNMP_form.SNMP_get.value.length <= 0 
		|| document.SNMP_form.SNMP_set.value.length <= 0)
	{
		alert("<%tcWebApi_get("String_Entry","SNMPJS2Text","s")%>");
		return false;
	}

	if(document.SNMP_form.SnmpFullflag.value == "Yes")
	{  
		if (document.SNMP_form.startTrapflag.value != "Yes") 
		{
				if(document.SNMP_form.SNMP_sysName.value.length <= 0 
					|| document.SNMP_form.SNMP_sysContact.value.length <= 0
					||document.SNMP_form.SNMP_sysLocation.value.length <= 0)
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS2Text","s")%>");
				return false;
			}
		}

		if(ipversion==1 && (SNMP_form.SNMP_trapManagerIP.value=="0.0.0.0" || SNMP_form.SNMP_trapManagerIP.value==""))
		{
			// WAN ipv6 only  
		}
		else
		{
			if(inValidIPAddrSnmp(SNMP_form.SNMP_trapManagerIP.value))
			{
				return false;
			}
		}
		if(document.SNMP_form.SnmpIPv6flag.value == "Yes")
		{
			if(ipversion!=1 && (SNMP_form.SNMP_trapManagerIPv6.value == "::" || SNMP_form.SNMP_trapManagerIPv6.value == ""))
			{
				// WAN not ipv6 only
			}
			else
			{
				if(SNMP_form.SNMP_trapManagerIPv6.value == "::")
				{
					alert("<%tcWebApi_get("String_Entry","SNMPJS14Text","s")%>");
					return false;
				}
				if(inValidIPv6AddrSnmp(SNMP_form.SNMP_trapManagerIPv6.value))
					return false;
			}
		}
	}
  
	if (SNMP_form.trustIPflag.value == "Yes")
	{  
		if(ipversion ==1 && (document.forms[0].SNMP_trustStartIP.value=="0.0.0.0" 
			|| document.forms[0].SNMP_trustStartIP.value=="" ) && (document.forms[0].SNMP_trustEndIP.value == "0.0.0.0" 
			|| document.forms[0].SNMP_trustEndIP.value == ""))
		{
			//WAN ipv6 only
		}
		else
		{	
			if(ipCheck())
				return;
		}
	
		if (SNMP_form.SnmpIPv6flag.value == "Yes")  
		{
			if(ipversion!=1 && document.SNMP_form.SNMP_trustStartIPv6.value=="" && document.SNMP_form.SNMP_trustEndIPv6.value=="")
			{
				//WAN ipv4 only
			}
			else
			{
				if(ipv6Check())
					return;		
			}
		}
	}
	
	if(quotationCheck(document.SNMP_form.SNMP_get, 29) ) 
		return false;
		
	if(quotationCheck(document.forms[0].SNMP_set, 29) ) 
		return false;
	
	if(document.SNMP_form.Snmpv3flag.value == "Yes")
	{
		if(document.SNMP_form.SNMPv3_enable[0].checked)
		{
			if(document.SNMP_form.SNMPv3_User.value.length <= 0 )
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS4Text","s")%>");
				return false;
			}
			if(document.SNMP_form.SNMPv3_User.value.length > 31 )
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS5Text","s")%>");
				return false;
			}
			if(document.SNMP_form.Auth_Passwd.value.length < 8 )
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS6Text","s")%>");
				return false;
			}
			if(document.SNMP_form.Auth_Passwd.value.length > 31 )
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS7Text","s")%>");
				return false;
			}
			if(PasswdCheck(document.SNMP_form.Auth_Passwd))
				return false;
				
			if(document.SNMP_form.Privacy_Passwd.value.length < 8 )
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS8Text","s")%>");
				return false;
			}
			if(document.SNMP_form.Privacy_Passwd.value.length > 31 )
			{
				alert("<%tcWebApi_get("String_Entry","SNMPJS9Text","s")%>");
				return false;
			}
			if(PasswdCheck(document.SNMP_form.Privacy_Passwd))
				return false;
		}
	}
	
	document.SNMP_form.Snmpflag.value = 1;
	if( true == setEBooValueCookie(document.SNMP_form) )
		document.SNMP_form.submit();
}

function doNonSympolCheck(c)
{
	if ((c >= "0")&&(c <= "9"))
	{
		return 1;
	}
	else if ((c >= "A")&&(c <= "Z"))
	{
		return 1;
	}
	else if ((c >= "a")&&(c <= "z"))
	{
		return 1;
	}

	return -1;
}

function PasswdCheck(object)
{
	var len = object.value.length;
	var c;
	var i = 0;
	for(i=0; i<len; i++)
	{
		var c = object.value.charAt(i);
		if(doNonSympolCheck(c) == -1)
		{
			alert("<%tcWebApi_get("String_Entry","SNMPJS10Text","s")%>" + c);
			return true;
		}
	}
	
	return false;
}

function quotationCheck(object, limit_len) 
{
	var len = object.value.length;
	var c;
	var i, j = 0;
	for (i = 0; i < len; i++)
	{
		var c = object.value.charAt(i);
		if (c == '"')
		{
			j += 6;
		}
		else
			j++;
	}
	if(j > limit_len)
	{
		alert('<%tcWebApi_get("String_Entry","SNMPJS1Text","s")%>'); 	  								    	    	   		
		return true;
	}	
	
	return false;
}

function ipv4OnlyOrIpv6OnlyCheck() 
{
	var i = 0;
	var j = 0;
	var idx = 0;
	var form=document.SNMP_form;
	
	var nEntryNum = "<% tcWebApi_get("WanInfo_Common","CycleNum","s") %>";
	// num 0
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var Wan_WanName = vArrayStr.split(',');

	<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "5") %>
	// num 6		
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var Wan_IPVERSION = vArrayStr.split(',');
	
	for ( idx = 0; idx < nEntryNum; idx ++ )
	{
		if ( Wan_WanName[idx].indexOf('INTERNET') < 0 )
			continue;
		
		if ( 'IPv4/IPv6' == Wan_IPVERSION[idx] )
			return 2;
			
		if ( 'IPv4' == Wan_IPVERSION[idx] )
			i ++;
		if ( 'IPv6' == Wan_IPVERSION[idx] )
			j ++;
	}

	if(i!=0 && j==0)
		return 0;

	if(j!=0 && i==0)
		return 1;
		
	if(i==1 && j==1)
		return 2;

	return 0;
}
 
function snmpOff(off)
{
	if(off)
	{
		document.SNMP_form.SNMP_get.disabled = true;
		document.SNMP_form.SNMP_set.disabled = true;
		if(document.SNMP_form.SnmpFullflag.value == "Yes")
		{
			if(document.SNMP_form.startTrapflag.value != "Yes")
			{
				document.SNMP_form.SNMP_sysName.disabled = true;
				document.SNMP_form.SNMP_sysContact.disabled = true;
				document.SNMP_form.SNMP_sysLocation.disabled = true;
			}
			document.SNMP_form.SNMP_trapManagerIP.disabled = true;
			if(document.SNMP_form.SnmpIPv6flag.value == "Yes")
			{
				document.SNMP_form.SNMP_trapManagerIPv6.disabled = true;
			}
		}
		if(document.SNMP_form.trustIPflag.value == "Yes") 
		{
			document.SNMP_form.SNMP_trustStartIP.disabled = true;
			document.SNMP_form.SNMP_trustEndIP.disabled = true;
			if(document.SNMP_form.SnmpIPv6flag.value == "Yes")
			{ 
				document.SNMP_form.SNMP_trustStartIPv6.disabled = true;
				document.SNMP_form.SNMP_trustEndIPv6.disabled = true; 
			}
		}
		if(document.SNMP_form.Snmpv3flag.value == "Yes")
		{
			document.SNMP_form.SNMPv3_enable[0].disabled = true;
			document.SNMP_form.SNMPv3_enable[1].disabled = true;
			document.SNMP_form.SNMPv3_User.disabled = true;
			document.SNMP_form.Access_Permissions.disabled = true;
			document.SNMP_form.Auth_Proto.disabled = true;
			document.SNMP_form.Auth_Passwd.disabled = true;
			document.SNMP_form.Privacy_Proto.disabled = true;
			document.SNMP_form.Privacy_Passwd.disabled = true;
		}					
	}
	else
	{
		document.SNMP_form.SNMP_get.disabled = false;
		document.SNMP_form.SNMP_set.disabled = false;
		if(document.SNMP_form.SnmpFullflag.value == "Yes")
		{
			if(document.SNMP_form.startTrapflag.value != "Yes") 
			{
				document.SNMP_form.SNMP_sysName.disabled = false;
				document.SNMP_form.SNMP_sysContact.disabled = false;
				document.SNMP_form.SNMP_sysLocation.disabled = false;
			}
			document.SNMP_form.SNMP_trapManagerIP.disabled = false;
			if(document.SNMP_form.SnmpIPv6flag.value == "Yes")
			{
				document.SNMP_form.SNMP_trapManagerIPv6.disabled = false;
			}
		}
		if(document.SNMP_form.trustIPflag.value == "Yes") 
		{
			document.SNMP_form.SNMP_trustStartIP.disabled = false;
			document.SNMP_form.SNMP_trustEndIP.disabled = false;
			if(document.SNMP_form.SnmpIPv6flag.value == "Yes") 
			{
				document.SNMP_form.SNMP_trustStartIPv6.disabled = false;
				document.SNMP_form.SNMP_trustEndIPv6.disabled = false;
			}
		}
		if(document.SNMP_form.Snmpv3flag.value == "Yes")
		{
			document.SNMP_form.SNMPv3_enable[0].disabled = false;
			document.SNMP_form.SNMPv3_enable[1].disabled = false;
			document.SNMP_form.SNMPv3_User.disabled = false;
			document.SNMP_form.Access_Permissions.disabled = false;
			document.SNMP_form.Auth_Proto.disabled = false;
			document.SNMP_form.Auth_Passwd.disabled = false;
			document.SNMP_form.Privacy_Proto.disabled = false;
			document.SNMP_form.Privacy_Passwd.disabled = false;
		}						
	}
}

function snmpv3Off(off)
{
	if(off)
	{
		setDisplay('snmpv3_L1', 0);
		setDisplay('snmpv3_L2', 0);
	}
	else
	{
		setDisplay('snmpv3_L1', 1);
		setDisplay('snmpv3_L2', 1);
	}
}

function snmpShow()
{	
	if(document.SNMP_form.SNMP_active[0].checked) 
		snmpOff(0); 
	else 
		snmpOff(1);
	
	if(document.SNMP_form.Snmpv3flag.value == "Yes")
	{
		if(document.SNMP_form.SNMPv3_enable[0].checked)
			snmpv3Off(0);
		else
			snmpv3Off(1);
	}	
}
</SCRIPT>
</HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 leftMargin=0 topMargin=0 
onLoad="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('SNMP_form') != null)LoadFrame()" 
onunload=DoUnload() marginheight="0" marginwidth="0">
<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center border=0>
<TBODY>
<TR>
	<TD height=1>
<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
		<TABLE height=80 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
<% else %>
<% if tcwebApi_get("WebCustom_Entry","isCTPONTYLOGOSupported","h") = "Yes" then %>
		<TABLE height=80 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
<% else %>
		<TABLE height=90 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
<% end if %> 
<% end if %> 
		<TBODY>
		<TR>
			<TD>&nbsp;</TD>
			<TD vAlign=bottom align=right width=358>
				<TABLE id=table8 cellSpacing=0 cellPadding=0 border=0>
				<TBODY>
				<TR>
					<TD vAlign=bottom align=right><SPAN class=curUserName>&nbsp;</SPAN></TD>
					<TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%> </TD>
					<TD vAlign=bottom width=50>
						<A onclick=DoLogout() href="/cgi-bin/logout.cgi" target=_top><SPAN class=logout><%tcWebApi_get("String_Entry","LogoutText","s")%> </SPAN></A>
					</TD>
				</TR>
				</TBODY>
				</TABLE>
			</TD>
		</TR>
		</TBODY>
		</TABLE>
		<TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
		<TBODY>
		<TR>
			<TD class=LocationDisplay id=LocationDisplay align=middle width=163 bgColor=#ef8218 rowSpan=3></TD>
			<TD width=434 bgColor=#427594 height=33>
				<P align=right><FONT face=菜悶 color=#ffffff><B><FONT face=菜悶 color=#ffffff size=6>
				<INPUT id=Selected_Menu type=hidden value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>-><%tcWebApi_get("String_Entry","ContentSNMPText","s")%>" name=Selected_Menu> </FONT></B>
				<SPAN class=GatewayName><%tcWebApi_get("String_Entry","GatewayNameText","s")%>
				<SCRIPT language=javascript>
				document.write(top.gateWayName);
				</SCRIPT>
				</SPAN></FONT></P>
			</TD>
			<TD width=211 bgColor=#ef8218 height=33>
				<P class=GatewayType align=center><%tcWebApi_get("String_Entry","ModeText","s")%>
				<SCRIPT language=javascript>
				document.write(top.ModelName);
				</SCRIPT>
				</P>
			</TD>
		</TR>
		<TR><TD id=MenuArea_L1 vAlign=bottom bgColor=#ef8218 colSpan=2 height=43>&nbsp;</TD></TR>
		<TR><TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24></TD></TR>
		</TBODY>
		</TABLE>
<SCRIPT language=javascript>
MakeMenu(getElById('Selected_Menu').value);
</SCRIPT>
		<TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 border=0>
		<TBODY>
		<TR>
			<TD height=15><IMG height=15 src="/img/panel1.gif" width=164 border=0></TD>
			<TD><IMG height=15 src="/img/panel2.gif" width=645 border=0></TD>
		</TR>
		</TBODY>
		</TABLE>
	</TD>
</TR>
<TR>
	<TD vAlign=top>	
	<DIV style="height: 100%;">
		<FORM name="SNMP_form" ACTION="/cgi-bin/app-snmp.asp" METHOD="POST"> 

		<INPUT TYPE="HIDDEN" NAME="Snmpflag" VALUE="0">
		<INPUT TYPE="HIDDEN" NAME="Snmpv3flag" VALUE="<%tcWebApi_get("WebCustom_Entry","isSNMPV3Supported","s")%>">
		<INPUT TYPE="HIDDEN" NAME="SnmpIPv6flag" VALUE="<%tcWebApi_get("WebCustom_Entry","isSNMPIPv6Supported","s")%>">
		<INPUT TYPE="HIDDEN" NAME="SnmpFullflag" VALUE="<%tcWebApi_get("WebCustom_Entry","isSNMPFullSupported","s")%>">
		<INPUT TYPE="HIDDEN" NAME="startTrapflag" VALUE="<%tcWebApi_get("WebCustom_Entry","isStartTrap","s")%>">
		<INPUT TYPE="HIDDEN" NAME="trustIPflag" VALUE="<%tcWebApi_get("WebCustom_Entry","isSnmpTrustIp","s")%>">
		
		<TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
		<TBODY>
		<TR>
			<TD width=157 bgColor=#ef8218 height=30><P class=Item_L1>SNMP</P></TD>
			<TD width=7 bgColor=#ef8218>　</TD>
			<TD width=474>　</TD>
			<TD vAlign=top width=170 background=/img/panel4.gif rowSpan=8>
				<TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
				<TBODY>
				<TR>
					<TD valign='top'>
						<A href="/cgi-bin/help_content.asp#SNMP" target=_blank>
						<IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 border=0>
						</A>
					</TD>
				</TR>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
				<TR><TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
				</TBODY>
				</TABLE>　 　 　
			</TD>
		</TR>
		<TR>
			<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
			<TD width=7 background=/img/panel3.gif>　</TD>
			<TD></TD>
		</TR>
		<TR vAlign="top" height="100%">
			<TD vAlign=top width=157 bgColor=#e7e7e7 height=30><P class=Item_L2></P></TD>
			<TD width=7 background=/img/panel3.gif>　</TD>
			<TD>
				<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
				<TBODY>
				<TR>
					<TD width=10>&nbsp;</TD>
					<TD>
						<TABLE cellSpacing=0 cellPadding=0 width=380 border=0>
						<TBODY>
						<TR>
							<TD width="130"><font color="#000000"><%tcWebApi_get("String_Entry","SNMPText","s")%>:</font></TD>
							<TD>
								<INPUT TYPE="RADIO" NAME="SNMP_active"  VALUE="Yes"  onClick="snmpOff(0)" <% If TCWebApi_get("Snmpd_Entry","Active","h") = "Yes" then asp_Write("checked") end if%> ><font color="#000000"><%tcWebApi_get("String_Entry","SNMPActivatedText","s")%></font>
								<INPUT TYPE="RADIO" NAME="SNMP_active"  VALUE="No"  onClick="snmpOff(1)" <% If TCWebApi_get("Snmpd_Entry","Active","h") = "No" then asp_Write("checked") end if%> ><font color="#000000"><%tcWebApi_get("String_Entry","SNMPDeactivatedText","s")%></font>
							</TD>
						</TR>
<% if tcWebApi_get("WebCustom_Entry","isSNMPV3Supported","h") = "Yes" then %>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSnmpV3Text","s")%>:</font></TD>
							<TD>
								<INPUT TYPE="RADIO" NAME="SNMPv3_enable"  VALUE="Yes"  onClick="snmpv3Off(0)" <% If TCWebApi_get("Snmpd_Entry","V3Enable","h") = "Yes" then asp_Write("checked") end if%> ><font color="#000000"><%tcWebApi_get("String_Entry","SNMPEnableText","s")%></font> 
						        <INPUT TYPE="RADIO" NAME="SNMPv3_enable"  VALUE="No"  onClick="snmpv3Off(1)" <% If TCWebApi_get("Snmpd_Entry","V3Enable","h") = "No" then asp_Write("checked") end if If TCWebApi_get("Snmpd_Entry","V3Enable","h") = "N/A" then asp_Write("checked") end if%> ><font color="#000000"><%tcWebApi_get("String_Entry","SNMPDisableText","s")%></font>
							</TD>
						</TR>
<% end if %>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPGetCommunityText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_get" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","rocommunity","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","rocommunity","s") end if%>" ></TD>
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSetCommunityText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_set" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","rwcommunity","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","rwcommunity","s") end if%>" ></TD>
						</TR>
<% if tcWebApi_get("WebCustom_Entry","isSnmpTrustIp","h") = "Yes" then %>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPTrustStartIPText","s")%>:</font></TD>
						    <TD><INPUT TYPE="TEXT" NAME="SNMP_trustStartIP" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","trustStartIP","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","trustStartIP","s") else asp_Write("0.0.0.0") end if%>" ></TD>
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPTrustEndIPText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_trustEndIP" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","trustEndIP","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","trustEndIP","s") else asp_Write("0.0.0.0") end if%>" ></TD>
						</TR>
<% if tcWebApi_get("WebCustom_Entry","isSNMPIPv6Supported","h") = "Yes" then %>	 
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPTrustStartIPv6Text","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_trustStartIPv6" SIZE="20" MAXLENGTH="45" VALUE="<% If TCWebApi_get("Snmpd_Entry","trustStartIPv6","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","trustStartIPv6","s") else asp_Write("") end if%>" ></TD>
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPTrustEndIPv6Text","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_trustEndIPv6" SIZE="20" MAXLENGTH="45" VALUE="<% If TCWebApi_get("Snmpd_Entry","trustEndIPv6","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","trustEndIPv6","s") else asp_Write("") end if%>" ></TD>
						</TR>
<%end if%>
<%end if%>
<% if tcWebApi_get("WebCustom_Entry","isSNMPFullSupported","h") = "Yes" then %>
<% if tcWebApi_get("WebCustom_Entry","isStartTrap","h") <> "Yes" then %>	
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSystemNameText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_sysName" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","sysName","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","sysName","s") end if%>" ></TD>	
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSystemContactText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_sysContact" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","sysContact","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","sysContact","s") end if%>" ></TD>
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSystemLocationText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_sysLocation" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","sysLocation","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","sysLocation","s") end if%>" ></TD> 
						</TR>
<%end if %>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSystemManagerIPText","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_trapManagerIP" SIZE="20" MAXLENGTH="15" VALUE="<% If TCWebApi_get("Snmpd_Entry","trap2sink","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","trap2sink","s") else asp_Write("0.0.0.0") end if%>" ></TD>	
						</TR>
<% if tcWebApi_get("WebCustom_Entry","isSNMPIPv6Supported","h") = "Yes" then %>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPSystemManagerIPv6Text","s")%>:</font></TD>
							<TD><INPUT TYPE="TEXT" NAME="SNMP_trapManagerIPv6" SIZE="20" MAXLENGTH="50" VALUE="<% If TCWebApi_get("Snmpd_Entry","trap2sink_ipv6","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","trap2sink_ipv6","s") else asp_Write("::") end if%>" ></TD>	
						</TR>
<% end if %>
<% end if %>
						</TBODY>
						</TABLE>
						</FORM>
					 </TD>
				</TR>
				</TBODY>
				</TABLE>
			</TD>
		</TR>
<% if tcWebApi_get("WebCustom_Entry","isSNMPV3Supported","h") = "Yes" then %>	
		<TR id="snmpv3_L1">
			<TD width=157 bgColor=#ef8218 height=30><P class=Item_L1>SNMPv3</P></TD>
			<TD width=7 bgColor=#ef8218>　</TD>
			<TD width=474>　</TD>
		</TR>
		<TR>
			<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
			<TD width=7 background=/img/panel3.gif>　</TD>
			<TD></TD>
		</TR>
		<TR vAlign="top" height="100%" id="snmpv3_L2">
			<TD vAlign=top width=157 bgColor=#e7e7e7 height=30><P class=Item_L2></P></TD>
			<TD width=7 background=/img/panel3.gif>　</TD>
			<TD>
				<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
				<TBODY>
				<TR>
					<TD width=10>&nbsp;</TD>
					<TD> 
						<TABLE cellSpacing=0 cellPadding=0 width=380 border=0>
						<TBODY>				
						<TR>
							<TD width="130"><font color="#000000"><%tcWebApi_get("String_Entry","SNMPUserNameText","s")%>:</font></TD>
				        	<TD><INPUT TYPE="TEXT" NAME="SNMPv3_User" SIZE="20" MAXLENGTH="60" VALUE="<% If TCWebApi_get("Snmpd_Entry","v3Name","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","v3Name","s") end if%>" ></TD>	
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPAccessPermissText","s")%>:</font></TD>
					      	<TD>
						      	<SELECT NAME="Access_Permissions" SIZE="1" >
								<option <% if tcWebApi_get("Snmpd_Entry","access","h") = "RO" then asp_Write("selected") end if %>>RO
								<option <% if tcWebApi_get("Snmpd_Entry","access","h") = "RW" then asp_Write("selected") end if %>>RW
								</SELECT>
							</TD>	
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPAuthProtocolText","s")%>:</font></TD>
					      	<TD>
						      	<SELECT NAME="Auth_Proto" SIZE="1" >
								<option <% if tcWebApi_get("Snmpd_Entry","authProto","h") = "MD5" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","SNMPAuthProOp0Text","s")%>
								<option <% if tcWebApi_get("Snmpd_Entry","authProto","h") = "SHA" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","SNMPAuthProOp1Text","s")%>
								</SELECT>
							</TD>	
						</TR>
						<TR>
							<TD> <font color="#000000"><%tcWebApi_get("String_Entry","SNMPAuthPasswdText","s")%>:</font></TD>
					        <TD><INPUT TYPE="TEXT" NAME="Auth_Passwd" SIZE="20" MAXLENGTH="60" VALUE="<% If TCWebApi_get("Snmpd_Entry","authPasswd","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","authPasswd","s") end if%>" ><%tcWebApi_get("String_Entry","SNMPAuthInfoText","s")%></TD>	
						</TR>
						<TR>
							<TD><font color="#000000"><%tcWebApi_get("String_Entry","SNMPPrivacyProText","s")%>:</font></TD>
					      	<TD>
						      	<SELECT NAME="Privacy_Proto" SIZE="1" >
								<option <% if tcWebApi_get("Snmpd_Entry","privProto","h") = "DES" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","SNMPPrivacyProOp0Text","s")%>
								<option <% if tcWebApi_get("Snmpd_Entry","privProto","h") = "AES" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","SNMPPrivacyProOp1Text","s")%>
								</SELECT>
							</TD>	
						</TR>
						<TR>
							<TD> <font color="#000000"><%tcWebApi_get("String_Entry","SNMPPrivacyPasswdText","s")%>:</font></TD>
					        <TD><INPUT TYPE="TEXT" NAME="Privacy_Passwd" SIZE="20" MAXLENGTH="60" VALUE="<% If TCWebApi_get("Snmpd_Entry","privPasswd","h") <> "N/A" then TCWebApi_get("Snmpd_Entry","privPasswd","s") end if%>" ><%tcWebApi_get("String_Entry","SNMPAuthInfoText","s")%> </TD>	
						</TR>	
						</TBODY>
						</TABLE>
					</TD>
				</TR>
				</TBODY>
				</TABLE>
			</TD>
		</TR>
		<TR>
			<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
			<TD width=7 background=/img/panel3.gif>　</TD>
			<TD></TD>
		</TR>
		<TR>
			<TD vAlign=top width=157 bgColor=#e7e7e7></TD>
			<TD width=7 background=/img/panel3.gif>　</TD>
			<TD></TD>
		</TR>
<% end if %>
		</TBODY>
		</TABLE>	
		</FORM>	
	</DIV>	
	</TD>
</TR>
<TR>
	<TD height=1>
		<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 align="center" border=0>
		<TBODY>
		<TR>
			<TD width=162 bgColor=#ef8218></TD>
			<TD width=278 bgColor=#427594></TD>
			<TD width=196 bgColor=#427594>
				<P align=center>
				<IMG id="btnOK" onclick="SNMPsave()" height="23" src="/img/ok.gif" width="80" border="0">&nbsp;&nbsp;
				<IMG id="btnCancel" onclick="RefreshPage()" height="23" src="/img/cancel.gif" width="80" border="0">
				</P>
			</TD>
			<TD width=170 bgColor=#313031></TD>
		</TR>
		</TBODY>
		</TABLE>
	</TD>
</TR>
</TBODY>
</TABLE>
</BODY>
</HTML>
