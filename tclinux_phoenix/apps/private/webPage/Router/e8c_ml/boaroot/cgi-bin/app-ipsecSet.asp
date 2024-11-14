<%
	if Request_form("add_num") <> "" then
		tcWebApi_set("WebCurSet_Entry","ipsec_id","add_num")
	end if
	if Request_form("editnum") <> "" then
		tcWebApi_set("WebCurSet_Entry","ipsec_id","editnum")
	end if

	if Request_form("EditFlag") = "1" then
		tcWebApi_unset("IPSEC_Entry")
		tcWebApi_set("IPSEC_Entry","Connection_Name","ConnectionName")
		tcWebApi_set("IPSEC_Entry","Gateway_IP","GatewayIP")
		tcWebApi_set("IPSEC_Entry","Local_Gateway_IP","LocalGatewayIP")
		tcWebApi_set("IPSEC_Entry","Local_SubnetOrIP","LocalSubnetOrIP")
		tcWebApi_set("IPSEC_Entry","Local_IP","LocalIP")
		tcWebApi_set("IPSEC_Entry","Local_SubnetMask","LocalSbunetMask")
		tcWebApi_set("IPSEC_Entry","Remote_SubnetOrIP","RemoteSubnetOrIP")
		tcWebApi_set("IPSEC_Entry","Remote_IP","RemoteIP")
		tcWebApi_set("IPSEC_Entry","Remote_SubnetMask","RemoteSbunetMask")
		tcWebApi_set("IPSEC_Entry","PreShared_Key","PreSharedKey")
		tcWebApi_set("IPSEC_Entry","Pfs_Enable","PfsEnable")
		tcWebApi_set("IPSEC_Entry","Phase1_Mode","Phase1Mode")
		tcWebApi_set("IPSEC_Entry","Phase1_EncryptAlgo","Phase1EncryptAlgo")
		tcWebApi_set("IPSEC_Entry","Phase1_AuthAlgo","Phase1AuthAlgo")
		tcWebApi_set("IPSEC_Entry","Phase1_DhGroup","Phase1DhGroup")
		tcWebApi_set("IPSEC_Entry","Phase1_KeyLifeTime","Phase1KeyLifeTime")
		tcWebApi_set("IPSEC_Entry","Phase1_KeyLifeUnit","Phase1KeyLifeUnit")
		tcWebApi_set("IPSEC_Entry","Phase2_EncryptAlgo","Phase2EncryptAlgo")
		tcWebApi_set("IPSEC_Entry","Phase2_AuthAlgo","Phase2AuthAlgo")
		tcWebApi_set("IPSEC_Entry","Phase2_PfsGroup","Phase2PfsGroup")
		tcWebApi_set("IPSEC_Entry","Phase2_KeyLifeTime","Phase2KeyLifeTime")
		tcWebApi_set("IPSEC_Entry","Phase2_KeyLifeUnit","Phase2KeyLifeUnit")
		tcWebApi_commit("IPSEC_Entry")
		tcWebApi_save()
	end if
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<HTML>
<HEAD>
<TITLE>VPN</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
	<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<META content="MSHTML 6.00.6000.16809" name=GENERATOR>	
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<SCRIPT language=javascript>
<%if Request_form("EditFlag") = "1" then%>
window.location.href = "/cgi-bin/app-ipsecList.asp";
<%end if%>

function doOnloadCheck()
{
}

function localSubnetCheck()
{
	if (document.forms[0].LocalSubnetOrIP.selectedIndex == 1)
		document.forms[0].LocalSbunetMask.value = "255.255.255.255";
}

function remoteSubnetCheck()
{
	if (document.forms[0].RemoteSubnetOrIP.selectedIndex == 1)
		document.forms[0].RemoteSbunetMask.value = "255.255.255.255";
}

function pfsEnableCheck()
{
	if (document.forms[0].PfsEnable.selectedIndex == 0)
		document.forms[0].Phase2PfsGroup.disabled = false;
	else{
		document.forms[0].Phase2PfsGroup.value = 0;
		document.forms[0].Phase2PfsGroup.disabled = true;
	}
}

function isValidIpAddrRoute(ip1,ip2,ip3,ip4) {
    if(ip1==0 || ip4==255 || ip1==127 )
        return false;

    return true;
}

function valDoValidateIPRoute(Address) {
    var address = Address.match("^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$");
    var digits;
    var i;
    var error=null;

    if(address == null) {
        error="not an IP address";
        return false;
    }
    else {
        digits = address[0].split(".");
        for(i=0; i < 4; i++) {
            if((Number(digits[i]) > 255 ) || (Number(digits[i]) < 0 ) || (Number(digits[0]) > 223) || (digits[i] == null)) {
              	error="Invalid IP address";
              	return false;
              	break;
            }
        }

        if((Number(digits[0])==0) && (Number(digits[1])==0) && (Number(digits[2])==0) && (Number(digits[3])==0))
        {
              	return false;
        }

        if(!isValidIpAddrRoute(digits[0],digits[1],digits[2],digits[3])) {
        	return false;
        }
    }
    return error;
}

function IPCheckRoute(address) {
    var message;
    if (address.value != "N/A") {
        message = valDoValidateIPRoute(address.value);
        if(message!=null) {
            address.focus();
            return false;
        }
   }
   return true;
}

function isValidIpAddr(ip1,ip2,ip3,ip4) {
    if(ip1==0 || ip4==0 || ip4==255 || ip1==127 )
        return false;

    return true;
}

function valDoValidateIP(Address) {
    var address = Address.match("^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$");
    var digits;
    var i;
    var error=null;

    if(address == null) {
        error="IP address is null";
        return false;
    }
    else {
        digits = address[0].split(".");
        for(i=0; i < 4; i++) {
            if((Number(digits[i]) > 255 ) || (Number(digits[i]) < 0 ) || (Number(digits[0]) > 223)) {
              	error="Invalid IP address";
              	return false;
              	break;
            }
        }

        if((Number(digits[0])==1) && (Number(digits[1])==0) && (Number(digits[2])==0) && (Number(digits[3])==0))
            return error;

        if(!isValidIpAddr(digits[0],digits[1],digits[2],digits[3])) {
        	return false;
        }
    }
    return error;
}


function IPCheck(address) {
    var message;
    if (address.value != "N/A") {
        message = valDoValidateIP(address.value);
        if(message!=null) {
     	    address.focus();
     	    return false;
        }
    }
    return true;
}

function stringExist(s)
{
  var len = s.length;

  if(len == 0)
    return false;

	return true;
}

function isNumeric(s)
{
  var len= s.length;
  var ch;
  if(len==0)
    return false;
  for( i=0; i< len; i++)
  {
    ch= s.charAt(i);
    if( ch > '9' || ch < '0')
    {
      return false;
    }
  }
  return true;
}



function SubnetCheckRouter(mask,IPAddr) {
	var v=mask.value;
	var addr = IPAddr.value.split(".");
	var digits = v.split(".");
	if ( !((v == "0.0.0.0")||
		(v == "128.0.0.0")||
		(v == "192.0.0.0")||
		(v == "224.0.0.0")||
		(v == "240.0.0.0")||
		(v == "248.0.0.0")||
		(v == "252.0.0.0")||
		(v == "254.0.0.0")||
		(v == "255.0.0.0")||
		(v == "255.128.0.0")||
		(v == "255.192.0.0")||
		(v == "255.224.0.0")||
		(v == "255.240.0.0")||
		(v == "255.248.0.0")||
		(v == "255.252.0.0")||
		(v == "255.254.0.0")||
		(v == "255.255.0.0")||
		(v == "255.255.128.0")||
		(v == "255.255.192.0")||
		(v == "255.255.224.0")||
		(v == "255.255.240.0")||
		(v == "255.255.252.0")||
		(v == "255.255.254.0")||
		(v == "255.255.255.0")||
		(v == "255.255.255.128")||
		(v == "255.255.255.192")||
		(v == "255.255.255.224")||
		(v == "255.255.255.240")||
		(v == "255.255.255.248")||
		(v == "255.255.255.252")||
		(v == "255.255.255.254")||
		(v == "255.255.255.255")) )
	{
		alert("Invalid subnet mask value!");mask.focus();
		v = "0.0.0.0";
		return false;
	}
	
	if((Number(addr[3])==0)&&(Number(digits[3])==255))
	{
		alert("Invalid subnet mask!");
		return false;
	}
	
	if(!((Number(addr[0]&digits[0])==Number(addr[0]))
		&&(Number(addr[1]&digits[1])==Number(addr[1]))
		&&(Number(addr[2]&digits[2])==Number(addr[2]))
		&&(Number(addr[3]&digits[3])==Number(addr[3]))))
	{
		alert("Wrong subnet mask!");
		return false;
	}
	
	return true;
}

var ipsecCount = 8;
function getIpsecLocalIPs()
{
	var	localIPAddress = new Array(ipsecCount);
	
	localIPAddress[0] = "<% tcWebApi_get("IPSEC_Entry0","Local_IP","s") %>";
	localIPAddress[1] = "<% tcWebApi_get("IPSEC_Entry1","Local_IP","s") %>";
	localIPAddress[2] = "<% tcWebApi_get("IPSEC_Entry2","Local_IP","s") %>";
	localIPAddress[3] = "<% tcWebApi_get("IPSEC_Entry3","Local_IP","s") %>";
	localIPAddress[4] = "<% tcWebApi_get("IPSEC_Entry4","Local_IP","s") %>";
	localIPAddress[5] = "<% tcWebApi_get("IPSEC_Entry5","Local_IP","s") %>";
	localIPAddress[6] = "<% tcWebApi_get("IPSEC_Entry6","Local_IP","s") %>";
	localIPAddress[7] = "<% tcWebApi_get("IPSEC_Entry7","Local_IP","s") %>";
	
	return localIPAddress;
}

function getIpsecRemoteIPs()
{
	var	remoteIPAddress = new Array(ipsecCount);

	remoteIPAddress[0] = "<% tcWebApi_get("IPSEC_Entry0","Remote_IP","s") %>";
	remoteIPAddress[1] = "<% tcWebApi_get("IPSEC_Entry1","Remote_IP","s") %>";
	remoteIPAddress[2] = "<% tcWebApi_get("IPSEC_Entry2","Remote_IP","s") %>";
	remoteIPAddress[3] = "<% tcWebApi_get("IPSEC_Entry3","Remote_IP","s") %>";
	remoteIPAddress[4] = "<% tcWebApi_get("IPSEC_Entry4","Remote_IP","s") %>";
	remoteIPAddress[5] = "<% tcWebApi_get("IPSEC_Entry5","Remote_IP","s") %>";
	remoteIPAddress[6] = "<% tcWebApi_get("IPSEC_Entry6","Remote_IP","s") %>";
	remoteIPAddress[7] = "<% tcWebApi_get("IPSEC_Entry7","Remote_IP","s") %>";
	
	return remoteIPAddress;
}

var ipsecLocalIPs = getIpsecLocalIPs();
var ipsecRemoteIPs = getIpsecRemoteIPs();

function doSubmit(index) {
	var currIpsecConnStr = "<% tcWebApi_get("WebCurSet_Entry","ipsec_id","s") %>";
	var cruuIpsecConnNum = parseInt(currIpsecConnStr);

	document.forms[0].EditFlag.value = index;
	
	if(index)
	{
		var localIP = document.forms[0].LocalIP;
		var remoteIP = document.forms[0].RemoteIP;
		var localMask = document.forms[0].LocalSbunetMask;
		var remoteMask = document.forms[0].RemoteSbunetMask;

		
		for (i = 0; i < ipsecCount; i++)
		{
			if (i == cruuIpsecConnNum)
				continue;
			
			if (ipsecLocalIPs[i] == localIP.value)
			{
				for (j = 0; j < ipsecCount; j++)
				{
					if (j == cruuIpsecConnNum)
						continue;

					if (ipsecRemoteIPs[j] == remoteIP.value)
					{
						alert('The same Local & Remote IP Address have been added!');
						return false;
					}
				}
			}
		}

		if (!stringExist(document.forms[0].ConnectionName.value))
		{
			alert("Please insert the Connection Name");
			document.forms[0].ConnectionName.focus();
			return false;
		}
		
		if(!IPCheckRoute(document.forms[0].LocalIP))
		{
			alert('The Local IP is invalid!');
			return false;
		}
		if(!IPCheckRoute(document.forms[0].RemoteIP))
		{
			alert('The Remote IP is invalid!');
			return false;
		}

		if(!IPCheck(document.forms[0].GatewayIP))
		{
			alert('The Remote Gateway IP is invalid!');
			return false;
		}

		if(!IPCheck(document.forms[0].LocalGatewayIP))
		{
			alert('The Local Gateway IP is invalid!');
			return false;
		}
	
		if(!SubnetCheckRouter(localMask,localIP))
		{ 
			alert('The Local Subnet Mask is invalid!');
			return false;
		}
		if(!SubnetCheckRouter(remoteMask,remoteIP))
		{ 
			alert('The Remote Subnet Mask is invalid!');
			return false;
		}
		if((localIP.value != "0.0.0.0") && (localMask.value == "0.0.0.0"))
		{
			alert("Invalid local subnet mask!");
			document.forms[0].LocalSbunetMask.focus();
			return false;
		}
		if((remoteIP.value != "0.0.0.0") && (remoteMask.value == "0.0.0.0"))
		{
			alert("Invalid remote subnet mask!");
			document.forms[0].RemoteSbunetMask.focus();
			return false;
		}

		if (
			(document.forms[0].LocalSubnetOrIP.selectedIndex == 1) && 
			(localMask.value != "255.255.255.255")
		)
		{
			alert("local subnet mask should be 255.255.255.255!");
			document.forms[0].LocalSbunetMask.focus();
			return false;			
		}

		if (
			(document.forms[0].RemoteSubnetOrIP.selectedIndex == 1) && 
			(remoteMask.value != "255.255.255.255")
		)
		{
			alert("remote subnet mask should be 255.255.255.255!");
			document.forms[0].RemoteSbunetMask.focus();
			return false;			
		}

		if (!isNumeric(document.forms[0].Phase1KeyLifeTime.value))
		{
			alert("Key Life Time for phase 1 is not a number");
			document.forms[0].Phase1KeyLifeTime.focus();
			return false;
		}
		if (!isNumeric(document.forms[0].Phase2KeyLifeTime.value))
		{
			alert("Key Life Time for phase 2 is not a number");
			document.forms[0].Phase2KeyLifeTime.focus();
			return false;
		}
	}

	if( true == setEBooValueCookie(document.forms[0]) )
		document.forms[0].submit();	
}
</SCRIPT>
</HEAD>

<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 leftMargin=0 topMargin=0 
	onLoad="doOnloadCheck();" marginheight="0" marginwidth="0">
<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center border=0>
  <TBODY>
  <TR>
    <TD height=1>
	<% if tcWebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
      <TABLE height=80 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
	<% else %>
	<% if tcWebApi_get("WebCustom_Entry","isCTPONTYLOGOSupported","h") = "Yes" then %>
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
                <TD class=welcom vAlign=bottom align=middle width=120>Welcome! </TD>
                <TD vAlign=bottom width=50>
                  <A onclick=DoLogout() href="/cgi-bin/logout.cgi" id=logOut name=logOut target=_top>
                  <SPAN class=logout>Log out</SPAN></A>
                </TD></TR>
              </TBODY>
            </TABLE></TD>
        </TR>
        </TBODY>
      </TABLE>
      <TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
        <TBODY>
        <TR>
          <TD class=LocationDisplay id=LocationDisplay align=middle width=163 
          	bgColor=#ef8218 rowSpan=3></TD>
          <TD width=434 bgColor=#427594 height=33>
            <P align=right>
            <FONT face=菜悶 color=#ffffff>
            <B><FONT face=菜悶 color=#ffffff size=6>
            	<INPUT id=Selected_Menu type=hidden value="APP->VPN" name=Selected_Menu> 
            </FONT></B>
            <SPAN class=GatewayName>Gateway Name:
            <SCRIPT language=javascript>
				document.write(top.gateWayName);
			</SCRIPT></SPAN>
			</FONT></P>
		  </TD>
          <TD width=211 bgColor=#ef8218 height=33>
            <P class=GatewayType align=center>Gateway Type:
          <SCRIPT language=javascript>
				document.write(top.ModelName);
			</SCRIPT>
            </P></TD>
        </TR>
        <TR>
          <TD id=MenuArea_L1 vAlign=bottom bgColor=#ef8218 colSpan=2 
          	height=43>&nbsp;</TD></TR>
        <TR>
          <TD id=MenuArea_L2 bgColor=#427594 colSpan=2 
      		height=24></TD>
      	</TR></TBODY>
      </TABLE>
	 
      
      <SCRIPT language=javascript>
		MakeMenu(getElById ('Selected_Menu').value);
	  </SCRIPT>

      <TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 border=0>
        <TBODY>
        <TR>
          <TD height=15><IMG height=15 src="/img/panel1.gif" width=164 border=0> </TD>
          <TD><IMG height=15 src="/img/panel2.gif" width=645 border=0></TD></TR>
        </TBODY>
      </TABLE>
    </TD>
  </TR>
  <TR>
    <TD vAlign=top>
      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","ipsecVPNSettingText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'>
                  <A href="/cgi-bin/help_content.asp#IPSEC" 
                  	target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  	width=40 border=0></A></TD>
              </TR>
			<% if tcWebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
			<% end if %>
              </TBODY>
            </TABLE>　 　 　</TD>
        </TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>

<!-- IPSEC form start -->
<FORM name="ipsecConnection_form" action="/cgi-bin/app-ipsecSet.asp" method="post">
	<INPUT TYPE="HIDDEN" NAME="editnum">
	<INPUT TYPE="HIDDEN" NAME="add_num">
	<INPUT type="hidden" value="0" name="EditFlag">
          <BR>
  <DIV>
  <TABLE cellSpacing=0 cellPadding=0 border=0>
    <TBODY>

	<TR>
      <TD><%tcWebApi_get("String_Entry","ipsecConnectNameText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Connection_Name","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Connection_Name","s") else asp_Write("") end if %>" name="ConnectionName"> </TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecLocalGatewayIPText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Local_Gateway_IP","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Local_Gateway_IP","s")  else asp_Write("0.0.0.0")  end if%>" name="LocalGatewayIP"></TD></TR>
	  
		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecRemoteGatewayIPText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Gateway_IP","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Gateway_IP","s")  else asp_Write("0.0.0.0")  end if%>" name="GatewayIP"></TD></TR>

		<TR>
	  <TD>&nbsp;</TD>
	  <TD></TD></TR>
	  
		<TR>
	  <TD width=120><%tcWebApi_get("String_Entry","ipsecLocalAccessRangeText","s")%></TD>
	  <TD><SELECT NAME="LocalSubnetOrIP" SIZE="1" onChange="localSubnetCheck()">
	  <option value="Subnet" <% if tcWebApi_staticGet("IPSEC_Entry","Local_SubnetOrIP","h") = "Subnet" then asp_Write("selected") end if %>>Subnet
	  <option value="SingleIP" <% if tcWebApi_staticGet("IPSEC_Entry","Local_SubnetOrIP","h") = "SingleIP" then asp_Write("selected") end if %>>Single IP
	  	  </SELECT> </TD><TD><%tcWebApi_get("String_Entry","ipsecRemoteAccessRangeInfoText","s")%></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecLocalIPAddressText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Local_IP","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Local_IP","s")  else asp_Write("0.0.0.0")  end if%>" name="LocalIP"></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecIPSubnetmaskText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Local_SubnetMask","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Local_SubnetMask","s")  else asp_Write("0.0.0.0")  end if%>" name="LocalSbunetMask"></TD></TR>

		<TR>
	  <TD>&nbsp;</TD>
	  <TD></TD></TR>

		<TR>
	  <TD width=120><%tcWebApi_get("String_Entry","ipsecRemoteAccessRangeText","s")%></TD>
	  <TD><SELECT NAME="RemoteSubnetOrIP" SIZE="1" onChange="remoteSubnetCheck()">
	  <option value="Subnet" <% if tcWebApi_staticGet("IPSEC_Entry","Remote_SubnetOrIP","h") = "Subnet" then asp_Write("selected") end if %>>Subnet
	  <option value="SingleIP" <% if tcWebApi_staticGet("IPSEC_Entry","Remote_SubnetOrIP","h") = "SingleIP" then asp_Write("selected") end if %>>Single IP
		  </SELECT> </TD><TD><%tcWebApi_get("String_Entry","ipsecLocalAccessRangeInfoText","s")%></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecRemoteIPAddressText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Remote_IP","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Remote_IP","s")  else asp_Write("0.0.0.0")  end if%>" name="RemoteIP"></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecIPSubnetmaskText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Remote_SubnetMask","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Remote_SubnetMask","s")  else asp_Write("0.0.0.0")  end if%>" name="RemoteSbunetMask"></TD></TR>

		<TR>
	  <TD>&nbsp;</TD>
	  <TD></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecPreSharedKeyText","s")%></TD>
	  <TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","PreShared_Key","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","PreShared_Key","s")  else asp_Write("")  end if%>" name="PreSharedKey"></TD></TR>

		<TR>
	  <TD width=120><%tcWebApi_get("String_Entry","ipsecPerfectForwardSecrecyText","s")%></TD>
	  <TD><SELECT NAME="PfsEnable" SIZE="1" onChange="pfsEnableCheck()">
	  <option value="Enable" <% if tcWebApi_staticGet("IPSEC_Entry","Pfs_Enable","h") = "Enable" then asp_Write("selected") end if %>>Enable
	  <option value="Disable" <% if tcWebApi_staticGet("IPSEC_Entry","Pfs_Enable","h") = "Disable" then asp_Write("selected") end if %>>Disable
	  </SELECT></TD></TR>

		<TR>
	  <TD>&nbsp;</TD>
	  <TD></TD></TR>

		<TR>
	  <TD><strong><%tcWebApi_get("String_Entry","ipsecKeyExchangePhase1Text","s")%></strong></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecExchangeModeText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase1Mode" SIZE="1">
	  <option value="0" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_Mode","h") = "0" then asp_Write("selected") end if %>>Main
	  <option value="1" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_Mode","h") = "1" then asp_Write("selected") end if %>>Aggressive
	  <option value="2" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_Mode","h") = "2" then asp_Write("selected") end if %>>Base
		</SELECT></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecEncryptionAlgorithmText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase1EncryptAlgo" SIZE="1">
	  <option value="des" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_EncryptAlgo","h") = "des" then asp_Write("selected") end if %>>DES
	  <option value="3des" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_EncryptAlgo","h") = "3des" then asp_Write("selected") end if %>>3DES
	  <option value="aes 128" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_EncryptAlgo","h") = "aes 128" then asp_Write("selected") end if %>>AES-128
	  <option value="aes 192" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_EncryptAlgo","h") = "aes 192" then asp_Write("selected") end if %>>AES-192
	  <option value="aes 256" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_EncryptAlgo","h") = "aes 256" then asp_Write("selected") end if %>>AES-256
		</SELECT></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecAuthAlgorithmText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase1AuthAlgo" SIZE="1">
		<option value="md5" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_AuthAlgo","h") = "md5" then asp_Write("selected") end if %>>MD5
		<option value="sha1" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_AuthAlgo","h") = "sha1" then asp_Write("selected") end if %>>SHA1
		<option value="sha256" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_AuthAlgo","h") = "sha256" then asp_Write("selected") end if %>>SHA256
		</SELECT></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecDiffieHellmanGroupText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase1DhGroup" SIZE="1">
		<option value="1" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_DhGroup","h") = "1" then asp_Write("selected") end if %>>768bit
		<option value="2" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_DhGroup","h") = "2" then asp_Write("selected") end if %>>1024bit
		<option value="5" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_DhGroup","h") = "5" then asp_Write("selected") end if %>>1536bit
		<option value="14" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_DhGroup","h") = "14" then asp_Write("selected") end if %>>2048bit
		<option value="15" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_DhGroup","h") = "15" then asp_Write("selected") end if %>>3072bit
		<option value="16" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_DhGroup","h") = "16" then asp_Write("selected") end if %>>4096bit
		</SELECT></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecKeyLifeTimeText","s")%></TD>
			<TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Phase1_KeyLifeTime","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Phase1_KeyLifeTime","s")  else asp_Write("")  end if%>" name="Phase1KeyLifeTime"></TD>
			<TD><SELECT NAME="Phase1KeyLifeUnit" SIZE="1">
		<option value="secs" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_KeyLifeUnit","h") = "secs" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","ipsecSecondsText","s")%>
		<option value="mins" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_KeyLifeUnit","h") = "mins" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","ipsecMinutesText","s")%>
		<option value="hours" <% if tcWebApi_staticGet("IPSEC_Entry","Phase1_KeyLifeUnit","h") = "hours" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","ipsecHoursText","s")%>
			</SELECT></TD>
		</TR>

		<TR>
	  <TD>&nbsp;</TD>
	  <TD></TD></TR>

		<TR>
	  <TD><strong><%tcWebApi_get("String_Entry","ipsecKeyExchangePhase2Text","s")%></strong></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecEncryptionAlgorithmText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase2EncryptAlgo" SIZE="1">
		<option value="0" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_EncryptAlgo","h") = "0" then asp_Write("selected") end if %>>DES
		<option value="1" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_EncryptAlgo","h") = "1" then asp_Write("selected") end if %>>3DES
		<option value="2" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_EncryptAlgo","h") = "2" then asp_Write("selected") end if %>>AES-128
		<option value="3" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_EncryptAlgo","h") = "3" then asp_Write("selected") end if %>>AES-192
		<option value="4" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_EncryptAlgo","h") = "4" then asp_Write("selected") end if %>>AES-256
		</SELECT></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecAuthAlgorithmText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase2AuthAlgo" SIZE="1">
		<option value="0" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_AuthAlgo","h") = "0" then asp_Write("selected") end if %>>MD5
		<option value="1" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_AuthAlgo","h") = "1" then asp_Write("selected") end if %>>SHA1
		<option value="2" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_AuthAlgo","h") = "2" then asp_Write("selected") end if %>>SHA256
		</SELECT></TD></TR>
		
		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecDiffieHellmanGroupText","s")%></TD>
	  <TD width=120><SELECT NAME="Phase2PfsGroup" SIZE="1">
		<option value="1" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_PfsGroup","h") = "1" then asp_Write("selected") end if %>>768bit
		<option value="2" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_PfsGroup","h") = "2" then asp_Write("selected") end if %>>1024bit
		<option value="5" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_PfsGroup","h") = "5" then asp_Write("selected") end if %>>1536bit
		<option value="14" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_PfsGroup","h") = "14" then asp_Write("selected") end if %>>2048bit
		<option value="15" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_PfsGroup","h") = "15" then asp_Write("selected") end if %>>3072bit
		<option value="16" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_PfsGroup","h") = "16" then asp_Write("selected") end if %>>4096bit
		</SELECT></TD></TR>

		<TR>
	  <TD><%tcWebApi_get("String_Entry","ipsecKeyLifeTimeText","s")%></TD>
		  	<TD><INPUT maxLength=16 size=16 value="<%If tcWebApi_get("IPSEC_Entry","Phase2_KeyLifeTime","h") <> "N/A" then tcWebApi_get("IPSEC_Entry","Phase2_KeyLifeTime","s")  else asp_Write("")  end if%>" name="Phase2KeyLifeTime"></TD>
			<TD><SELECT NAME="Phase2KeyLifeUnit" SIZE="1">
		<option value="secs" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_KeyLifeUnit","h") = "secs" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","ipsecSecondsText","s")%>
		<option value="mins" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_KeyLifeUnit","h") = "mins" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","ipsecMinutesText","s")%>
		<option value="hours" <% if tcWebApi_staticGet("IPSEC_Entry","Phase2_KeyLifeUnit","h") = "hours" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","ipsecHoursText","s")%>
			</SELECT></TD>
		</TR>

  </TBODY></TABLE></DIV>
 </FORM>
<!-- IPSEC form end -->

                </TD>
              </TR>
              </TBODY>
            </TABLE>
          </TD>
        </TR>
				  
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        </TBODY>
      </TABLE>
    </TD>
  </TR>
  
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 border=0>
        <TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594>
		  <!-- InstanceEndEditable -->
			<INPUT onClick="doSubmit(1);" type=button value="<%tcWebApi_get("String_Entry","ipsecSaveText","s")%>" name="IpsecSubmit">&nbsp;&nbsp;  
			<INPUT type="button" value="<%tcWebApi_get("String_Entry","ipsecBackText","s")%>" name="IpsecBack" onClick="javascript:window.location='/cgi-bin/app-ipsecList.asp'" >&nbsp;&nbsp; 		  
		  </TD>
          <TD width=170 bgColor=#313031>　</TD>
		</TR></TBODY>
	  </TABLE>
	</TD>
  </TR>
  </TBODY>
</TABLE>
</BODY>
</HTML>
