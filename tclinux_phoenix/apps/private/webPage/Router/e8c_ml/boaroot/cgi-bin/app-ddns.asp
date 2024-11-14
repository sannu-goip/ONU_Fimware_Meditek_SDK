<%
if Request_Form("DDNS_Flag") = "0" then
	TCWebApi_set("WebCurSet_Entry","ddns_id","Curddns_Flag")
elseif Request_Form("DDNS_Flag") = "1" then
	TCWebApi_set("WebCurSet_Entry","ddns_id","Curddns_Flag")
 			TCWebApi_set("Ddns_Common","useOray","useOray_Flag")
 		if Request_Form("DDNS_ORAY_Flag")  = "1" then
			TCWebApi_set("Ddns_Entry0","Active","EnableDDNS_Flag")
			TCWebApi_set("Ddns_Entry0","DDNS_Name","ddns_Servername")
			TCWebApi_set("Ddns_Entry0","SERVERNAME","DDNSServer")
			TCWebApi_set("Ddns_Entry0","SERVICEPORT","ServicePort")
			TCWebApi_set("Ddns_Entry0","DDNS_Interface","Interface")
			TCWebApi_set("Ddns_Entry0","MYHOST","DDNSHostName")
			TCWebApi_set("Ddns_Entry0","DDNS_Domain","DDNSDomainName")
			TCWebApi_set("Ddns_Entry0","USERNAME","DDNSUsername")
			TCWebApi_set("Ddns_Entry0","PASSWORD","DDNSPassword")	
		elseif Request_Form("DDNS_ORAY_Flag") = "0" then
	TCWebApi_set("Ddns_Entry","Active","EnableDDNS_Flag")
	TCWebApi_set("Ddns_Entry","DDNS_Name","ddns_Servername")
	TCWebApi_set("Ddns_Entry","DDNS_Protocal","DDNSProtocol")
	TCWebApi_set("Ddns_Entry","SERVERNAME","DDNSServer")
	TCWebApi_set("Ddns_Entry","SERVICEPORT","ServicePort")
	TCWebApi_set("Ddns_Entry","DDNS_Interface","Interface")
	TCWebApi_set("Ddns_Entry","MYHOST","DDNSHostName")
	TCWebApi_set("Ddns_Entry","DDNS_Domain","DDNSDomainName")
	TCWebApi_set("Ddns_Entry","USERNAME","DDNSUsername")
	TCWebApi_set("Ddns_Entry","PASSWORD","DDNSPassword")	
	TCWebApi_set("Ddns_Entry","WILDCARD","Enable_Wildcard")
		end if
	TCWebApi_commit("Ddns_Entry")
	TCWebApi_save()
elseif Request_Form("DDNS_Flag") = "2" then
	if Request_Form("hDdnsEntry0") <> "-1" then
		TCWebApi_unset("Ddns_Entry0")
	end if
	
	if Request_Form("hDdnsEntry1") <> "-1" then
		TCWebApi_unset("Ddns_Entry1")
	end if
	
	if Request_Form("hDdnsEntry2") <> "-1" then
		TCWebApi_unset("Ddns_Entry2")
	end if

	if Request_Form("hDdnsEntry3") <> "-1" then
		TCWebApi_unset("Ddns_Entry3")
	end if

	if Request_Form("hDdnsEntry4") <> "-1" then
		TCWebApi_unset("Ddns_Entry4")
	end if

	if Request_Form("hDdnsEntry5") <> "-1" then
		TCWebApi_unset("Ddns_Entry5")
	end if

	if Request_Form("hDdnsEntry6") <> "-1" then
		TCWebApi_unset("Ddns_Entry6")
	end if

	if Request_Form("hDdnsEntry7") <> "-1" then
		TCWebApi_unset("Ddns_Entry7")
	end if
	if Request_Form("hDdnsEntry8") <> "-1" then
		TCWebApi_unset("Ddns_Entry8")
	end if
	
	if Request_Form("hDdnsEntry9") <> "-1" then
		TCWebApi_unset("Ddns_Entry9")
	end if
	
	if Request_Form("hDdnsEntry10") <> "-1" then
		TCWebApi_unset("Ddns_Entry10")
	end if

	if Request_Form("hDdnsEntry11") <> "-1" then
		TCWebApi_unset("Ddns_Entry11")
	end if

	if Request_Form("hDdnsEntry12") <> "-1" then
		TCWebApi_unset("Ddns_Entry12")
	end if

	if Request_Form("hDdnsEntry13") <> "-1" then
		TCWebApi_unset("Ddns_Entry13")
	end if

	if Request_Form("hDdnsEntry14") <> "-1" then
		TCWebApi_unset("Ddns_Entry14")
	end if

	if Request_Form("hDdnsEntry15") <> "-1" then
		TCWebApi_unset("Ddns_Entry15")
	end if
	if Request_Form("hDdnsEntry16") <> "-1" then
		TCWebApi_unset("Ddns_Entry16")
	end if
	
	if Request_Form("hDdnsEntry17") <> "-1" then
		TCWebApi_unset("Ddns_Entry17")
	end if
	
	if Request_Form("hDdnsEntry18") <> "-1" then
		TCWebApi_unset("Ddns_Entry18")
	end if

	if Request_Form("hDdnsEntry19") <> "-1" then
		TCWebApi_unset("Ddns_Entry19")
	end if

	if Request_Form("hDdnsEntry20") <> "-1" then
		TCWebApi_unset("Ddns_Entry20")
	end if

	if Request_Form("hDdnsEntry21") <> "-1" then
		TCWebApi_unset("Ddns_Entry21")
	end if

	if Request_Form("hDdnsEntry22") <> "-1" then
		TCWebApi_unset("Ddns_Entry22")
	end if

	if Request_Form("hDdnsEntry23") <> "-1" then
		TCWebApi_unset("Ddns_Entry23")
	end if
		if Request_Form("hDdnsEntry24") <> "-1" then
		TCWebApi_unset("Ddns_Entry24")
	end if
	
	if Request_Form("hDdnsEntry25") <> "-1" then
		TCWebApi_unset("Ddns_Entry25")
	end if
	
	if Request_Form("hDdnsEntry26") <> "-1" then
		TCWebApi_unset("Ddns_Entry26")
	end if

	if Request_Form("hDdnsEntry27") <> "-1" then
		TCWebApi_unset("Ddns_Entry27")
	end if

	if Request_Form("hDdnsEntry28") <> "-1" then
		TCWebApi_unset("Ddns_Entry28")
	end if

	if Request_Form("hDdnsEntry29") <> "-1" then
		TCWebApi_unset("Ddns_Entry29")
	end if

	if Request_Form("hDdnsEntry30") <> "-1" then
		TCWebApi_unset("Ddns_Entry30")
	end if

	if Request_Form("hDdnsEntry31") <> "-1" then
		TCWebApi_unset("Ddns_Entry31")
	end if
		if Request_Form("hDdnsEntry32") <> "-1" then
		TCWebApi_unset("Ddns_Entry32")
	end if
	
	if Request_Form("hDdnsEntry33") <> "-1" then
		TCWebApi_unset("Ddns_Entry33")
	end if
	
	if Request_Form("hDdnsEntry34") <> "-1" then
		TCWebApi_unset("Ddns_Entry34")
	end if

	if Request_Form("hDdnsEntry35") <> "-1" then
		TCWebApi_unset("Ddns_Entry35")
	end if

	if Request_Form("hDdnsEntry36") <> "-1" then
		TCWebApi_unset("Ddns_Entry36")
	end if

	if Request_Form("hDdnsEntry37") <> "-1" then
		TCWebApi_unset("Ddns_Entry37")
	end if

	if Request_Form("hDdnsEntry38") <> "-1" then
		TCWebApi_unset("Ddns_Entry38")
	end if

	if Request_Form("hDdnsEntry39") <> "-1" then
		TCWebApi_unset("Ddns_Entry39")
	end if
		if Request_Form("hDdnsEntry40") <> "-1" then
		TCWebApi_unset("Ddns_Entry40")
	end if
	
	if Request_Form("hDdnsEntry41") <> "-1" then
		TCWebApi_unset("Ddns_Entry41")
	end if
	
	if Request_Form("hDdnsEntry42") <> "-1" then
		TCWebApi_unset("Ddns_Entry42")
	end if

	if Request_Form("hDdnsEntry43") <> "-1" then
		TCWebApi_unset("Ddns_Entry43")
	end if

	if Request_Form("hDdnsEntry44") <> "-1" then
		TCWebApi_unset("Ddns_Entry44")
	end if

	if Request_Form("hDdnsEntry45") <> "-1" then
		TCWebApi_unset("Ddns_Entry45")
	end if

	if Request_Form("hDdnsEntry46") <> "-1" then
		TCWebApi_unset("Ddns_Entry46")
	end if

	if Request_Form("hDdnsEntry47") <> "-1" then
		TCWebApi_unset("Ddns_Entry47")
	end if
		if Request_Form("hDdnsEntry48") <> "-1" then
		TCWebApi_unset("Ddns_Entry48")
	end if
	
	if Request_Form("hDdnsEntry49") <> "-1" then
		TCWebApi_unset("Ddns_Entry49")
	end if
	
	if Request_Form("hDdnsEntry50") <> "-1" then
		TCWebApi_unset("Ddns_Entry50")
	end if

	if Request_Form("hDdnsEntry51") <> "-1" then
		TCWebApi_unset("Ddns_Entry51")
	end if

	if Request_Form("hDdnsEntry52") <> "-1" then
		TCWebApi_unset("Ddns_Entry52")
	end if

	if Request_Form("hDdnsEntry53") <> "-1" then
		TCWebApi_unset("Ddns_Entry53")
	end if

	if Request_Form("hDdnsEntry54") <> "-1" then
		TCWebApi_unset("Ddns_Entry54")
	end if

	if Request_Form("hDdnsEntry55") <> "-1" then
		TCWebApi_unset("Ddns_Entry55")
	end if
		if Request_Form("hDdnsEntry56") <> "-1" then
		TCWebApi_unset("Ddns_Entry56")
	end if
	
	if Request_Form("hDdnsEntry57") <> "-1" then
		TCWebApi_unset("Ddns_Entry57")
	end if
	
	if Request_Form("hDdnsEntry58") <> "-1" then
		TCWebApi_unset("Ddns_Entry58")
	end if

	if Request_Form("hDdnsEntry59") <> "-1" then
		TCWebApi_unset("Ddns_Entry59")
	end if

	if Request_Form("hDdnsEntry60") <> "-1" then
		TCWebApi_unset("Ddns_Entry60")
	end if

	if Request_Form("hDdnsEntry61") <> "-1" then
		TCWebApi_unset("Ddns_Entry61")
	end if

	if Request_Form("hDdnsEntry62") <> "-1" then
		TCWebApi_unset("Ddns_Entry62")
	end if

	if Request_Form("hDdnsEntry63") <> "-1" then
		TCWebApi_unset("Ddns_Entry63")
	end if

	TCWebApi_commit("Ddns_Entry")
	TCWebApi_save()
elseif Request_Form("DDNS_Flag") = "3" then
	TCWebApi_set("WebCurSet_Entry","ddns_id","Curddns_Flag")
	TCWebApi_set("Ddns_Common","CurIndex","Curddns_Flag")
	TCWebApi_set("Ddns_Common","useOray","useOray_Flag")
	TCWebApi_commit("Ddns")
end if
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","DdnsConfText","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>

<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('DdnsForm') != null)LoadFrame()" 
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
                <TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%></TD>
                <TD vAlign=bottom width=50><A onclick=DoLogout() 
                  href="/cgi-bin/login.asp" id=logOut name=logOut target=_top><SPAN 
                  class=logout><%tcWebApi_get("String_Entry","LogoutText","s")%> 
      </SPAN></A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
      <TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 
border=0>
        <TBODY>
        <TR>
          <TD class=LocationDisplay id=LocationDisplay align=middle width=163 
          bgColor=#ef8218 rowSpan=3></TD>
          <TD width=434 bgColor=#427594 height=33>
            <P align=right><FONT face=∫⁄ÃÂ color=#ffffff><B><FONT face=∫⁄ÃÂ 
            color=#ffffff size=6><INPUT id=Selected_Menu type=hidden 
            value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>-><%tcWebApi_get("String_Entry","ContentDDNSText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
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
          <TD height=15><IMG height=15 src="/img/panel1.gif" width=164 
            border=0> </TD>
          <TD><IMG height=15 src="/img/panel2.gif" width=645 border=0> 
          </TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD vAlign=top>
      <SCRIPT language=JavaScript>
var IfaceNumber = 0;
var temp = 'InternetGatewayDevice.WANDevice.1.WANConnectionDevice.';
var prefixLength = temp.length;
function stWan(domain,Name,ServiceList)
{
this.domain = domain;
this.key = domain.substr(prefixLength);
this.InterfaceName = Name;
this.ServiceList = ServiceList;
}
var WanPPP = new Array(new stWan("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.4.WANPPPConnection.2","1_TR069_INTERNET_R_0_35","TR069_INTERNET"),null);
var WanIP = new Array(null);
var Wan = new Array();
for (j = 0; j < WanIP.length - 1; j++)
{
Wan[j] = WanIP[j];
}
for (k = 0; k < WanPPP.length - 1; k++,j++)
{
Wan[j] = WanPPP[k];
}
function stDDNS(domain,Enable,Provider,Username,Password,Port,Protocol,Server,DomainName,HostName)
{
this.domain = domain;
this.key = domain.substr(prefixLength);
this.Enable = Enable;
this.Provider = Provider;
this.Username = Username;
this.Password = Password;
this.Port = Port;
this.Protocol = Protocol;
this.Server = Server;
this.DomainName = DomainName;
this.InterfaceName = '';
this.HostName = HostName;
}
var WanPPPDDNS = 
new Array
(
null
);
var WanIPDDNS = 
new Array
(
null
);
var DDNS = new Array();
for (j = 0; j < WanPPPDDNS.length - 1; j++)
{
DDNS[j] = WanPPPDDNS[j];
}
for (k = 0; k < WanIPDDNS.length - 1; k++,j++)
{
DDNS[j] = WanIPDDNS[k];
}
AssociateParam('DDNS','Wan','InterfaceName');
function WriteOption()
{
var k;
for (k = 0; k < Wan.length; k++)
{
if (Wan[k].ServiceList != 'TR069')
{
document.write('<option value="' + Wan[k].domain + '">'
+ Wan[k].InterfaceName + '</option>');
}
}
}
function isTValidName(name) {
var i = 0;
var unsafeString = "\"<>%\\^[]`\+\$\,='#&:;*/{} \t";
for ( i = 0; i < name.length; i++ )
{
for( j = 0; j < unsafeString.length; j++)
if ( (name.charAt(i)) == unsafeString.charAt(j) )
return false;
}
return true;
}
function isValidHostName(name) {
var i = 0;
for ( i = 0; i < name.length; i++ )
{
if ((name.charAt(i) >= 'a' && name.charAt(i) <= 'z')
||(name.charAt(i) >= 'A' && name.charAt(i) <= 'Z')
||(name.charAt(i) >= '1' && name.charAt(i) <= '9')
||(name.charAt(i) == '-')
)
{
continue;
}
else
{
return false;
}
}
return true;
}
function btnAdd(place)
{
var loc = place + '?action=add';
with ( document.forms[0] )
{
loc += '&service=' + ispname[ispname.selectedIndex].value;
loc += '&iface=' + ifc[ifc.selectedIndex].value;
loc += '&hostname=' + domainname.value;
loc += '&username=' + username.value;
loc += '&password=' + password.value;
loc += '&domainname=' + domainname.value;
if (ispname.selectedIndex == 1)
{
loc += '&protocol=' + infoprotocol[infoprotocol.selectedIndex].value;
loc += '&serveraddr=' + infoserveraddress.value;
}
else
{
loc += '&protocol=' + 'GNUDip.http';
loc += '&serveraddr=' + 'ns.eagleeyes.com.cn';
}
}
var code = 'location="' + loc + '"';
eval(code);
}
function btnRemove()
{
	document.DdnsForm.DDNS_Flag.value = "2";
	if( true == setEBooValueCookie(document.DdnsForm) )
		document.DdnsForm.submit();
/*var rml = getElById('rml');
if (rml == null)
return;
var SubmitForm = new webSubmitForm();
with (document.forms[0])
{
if (rml.length > 0)
{
var i;
for (i = 0; i < rml.length; i++)
{
if (rml[i].checked == true)
{
SubmitForm.addParameter(rml[i].value,'');
}
}
}
else if (rml.checked == true)
{
SubmitForm.addParameter(rml.value,'');
}
}
SubmitForm.setAction('del.cgi?RequestFile=html/application/ddns.asp');
SubmitForm.submit();*/
}
function LoadFrame()
{
if (typeof(top.DDNSProvider) != 'undefined' && top.DDNSProvider != '')
setSelect('DDNSProviderType',top.DDNSProvider);
ISPChange();

	if ( 0 == IfaceNumber )
		setDisable('Add_Button', 1);
//document.DdnsForm.DDNS_Flag.value = "0";
}
function FindString(sourceStr, subStr)
{
	var i,j,k;
	var len;
	len = sourceStr.length;
	for(i = 0; i < len; i++)
	{
		if ( (i + subStr.length) > len )
		{
			return false;
		}
		if (sourceStr.charAt(i) != subStr.charAt(0) )
		{
			continue;
		}
		else
		{
			for (j = 0, k = i; j < subStr.length, k< (i + subStr.length); j++, k++ )
			{
				if ( (i + subStr.length) > len )
				{
					return false;
				}
				if( sourceStr.charAt(k) != subStr.charAt(j) )
				{
					break;
				}
			}
			if(k == ( i + subStr.length))
			{
				break;
			}
		}
	}
	return true;
}
function CheckForm()
{
if(DDNS.length >= 32)
{
alert('<%tcWebApi_get("String_Entry","DdnsJS0Text","s")%>');
return false;
}
with (getElById('DdnsForm'))
{
if(Interface.value == "")
{
alert('<%tcWebApi_get("String_Entry","DdnsJS1Text","s")%>');
return false;
}
var selObj = getElById("Interface");
var temp = selObj.options[selObj.selectedIndex].innerHTML;
if(FindString(temp , "_B_") ==  true)
{
alert('<%tcWebApi_get("String_Entry","DdnsJS2Text","s")%>');
return false;
}
if (DDNSHostName.value == "")
{
alert('<%tcWebApi_get("String_Entry","DdnsJS3Text","s")%>');
return false;
}
if (isValidHostName(DDNSHostName.value) == false)
{
alert('<%tcWebApi_get("String_Entry","DdnsJS4Text","s")%>');
return false;
}
if (DDNSDomainName.value == "")
{
alert('<%tcWebApi_get("String_Entry","DdnsJS5Text","s")%>');
return false;
}
if (isTValidName(DDNSDomainName.value) == false)
{
alert('<%tcWebApi_get("String_Entry","DdnsJS6Text","s")%>');
return false;
}
for (j = 0; j < DDNSDomainName.value.length; j++)
{
var dm = DDNSDomainName.value;
if (dm.charAt(j) == '.')
{
break;
}
}
if (j >= DDNSDomainName.value.length - 1 || j == 0)
{
alert('<%tcWebApi_get("String_Entry","DdnsJS6Text","s")%>');
return false;
}
for (i = 0; i < DDNS.length; i++)
{
if ( (DDNSDomainName.value == DDNS[i].DomainName) &&
(DDNSHostName.value == DDNS[i].HostName) )
{
alert('<%tcWebApi_get("String_Entry","DdnsJS7Text","s")%>');
return false;
}
}
if (DDNSUsername.value == "")
{
alert('<%tcWebApi_get("String_Entry","DdnsJS8Text","s")%>');
return false;
}
if (isTValidName(DDNSUsername.value) == false)
{
alert('<%tcWebApi_get("String_Entry","DdnsJS9Text","s")%>');
return false;
}
if (DDNSPassword.value == "")
{
alert('<%tcWebApi_get("String_Entry","DdnsJS10Text","s")%>');
return false;
}
if(useOray_Flag.value != "Yes")
{
if(DDNSServer.value == "")
{
alert('<%tcWebApi_get("String_Entry","DdnsJS11Text","s")%>');
return false;
}
}
if (isValidPort(ServicePort.value) == false)
{
alert(ServicePort.value + '<%tcWebApi_get("String_Entry","DdnsJS12Text","s")%>');
return false;
}
}
return true;
}
function AddSubmitParam(SubmitForm)
{
var ProviderType = getSelectVal('DDNSProviderType');
if (ProviderType == 'Chinatelecom')
{
setText('DDNSProvider','chinatelecom');
}
setDisable('DDNSProviderType',1);
setDisable('Interface',1);
SubmitForm.addForm('DdnsForm','x');
var DomainPrefix = getSelectVal('Interface') + '.X_CT-COM_DDNSConfiguration';
SubmitForm.setAction('addcfg.cgi?x=' + DomainPrefix
+ '&RequestFile=html/application/ddns.asp');
top.DDNSProvider = ProviderType;
}

function ISPChange()
{
with (document.forms[0])
{
		setDisplay('tr_ProviderName',0);
		if (DDNSProviderType.value == 'Oray')
{
			document.DdnsForm.useOray_Flag.value = "Yes" ;
			DDNS_ORAY_Flag.value = 1;
			document.DdnsForm.ddns_Servername.value = "ORAY" ;
			document.DdnsForm.ServicePort.value = "6060" ;
			document.DdnsForm.DDNSProtocol.value = "" ;
			document.DdnsForm.DDNSServer.value = "" ;
			document.DdnsForm.Enable_Wildcard.value = "" ;
			setDisplay('ispinfoprotocol',0);
			setDisplay('tr_DDNSServer',0);
			setDisplay('tr_wanInterface',0);	
			setDisplay('tr_DDNSWildCard',0);	
}
else
{
			document.DdnsForm.useOray_Flag.value = "No" ;
			document.DdnsForm.DDNSProtocol.options[0].selected= true ;
			document.DdnsForm.DDNSServer.options[0].selected= true ;
			document.DdnsForm.Enable_Wildcard.value = "No" ;
			DDNS_ORAY_Flag.value = 0;
			setDisplay('ispinfoprotocol',1);
			setDisplay('tr_DDNSServer',1);
			setDisplay('tr_wanInterface',1);	
			setDisplay('tr_DDNSWildCard',1);	
}
}
}
function interfaceChange()
{
	var curProviderMode ="<% tcWebApi_get("Ddns_Common", "useOray", "s") %>";
	var ddnsEntrylist1 ="<% tcWebApi_get("Ddns_Common","Entrylist1","s") %>";
     	var ddnsEntrylist2 ="<% tcWebApi_get("Ddns_Common","Entrylist2","s") %>";
		
	with (document.forms[0])
	{		
		if(useOray_Flag.value == "No"  &&  curProviderMode == "Yes")
		{	
     			if(ddnsEntrylist1 != ""  || ddnsEntrylist2 != "")
			{
				alert('<%tcWebApi_get("String_Entry","DdnsJS15Text","s")%>');
				return;
			}
		}
		
		var selObj = getElById("Interface");
		var temp = selObj.options[selObj.selectedIndex].id;
		Curddns_Flag.value = temp;
		DDNS_Flag.value = "3";
	}
	if( true == setEBooValueCookie(document.forms[0]) )
		document.forms[0].submit();
}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1>DDNS≈‰÷√</P></TD>
          <TD width=7 bgColor=#ef8218>°°</TD>
          <TD width=474>°°</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#DDNS≈‰÷√" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0></A></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
              </TBODY></TABLE>°° °° °°</TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>°°</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>°°</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <FORM name="DdnsForm" action="/cgi-bin/app-ddns.asp" method="post">
                          <input type="hidden" id="DDNS_Flag" name="DDNS_Flag" value="0">
                          <input type="hidden" id="DDNS_ORAY_Flag" name="DDNS_ORAY_Flag" value="0">
                          <input type="hidden" id="ddns_Servername" name="ddns_Servername" value="<% tcWebApi_get("Ddns_Entry","DDNS_Name","s") %>">
                          <input type="hidden" id="EnableDDNS_Flag" name="EnableDDNS_Flag" value="<% tcWebApi_get("Ddns_Entry","Active","s") %>">
                          <input type="hidden" id="Curddns_Flag" name="Curddns_Flag" value="<% tcWebApi_get("Ddns_Common","CurIndex","s") %>">
                          <input type="hidden" id="useOray_Flag" name="useOray_Flag" value="<% tcWebApi_get("Ddns_Common","useOray","s") %>">
                          <input type="hidden" id="delnum" name="delnum">
						  
                          <input type="hidden" id="hDdnsEntry0" name="hDdnsEntry0" value="-1">
                          <input type="hidden" id="hDdnsEntry1" name="hDdnsEntry1" value="-1">
                          <input type="hidden" id="hDdnsEntry2" name="hDdnsEntry2" value="-1">
                          <input type="hidden" id="hDdnsEntry3" name="hDdnsEntry3" value="-1">
                          <input type="hidden" id="hDdnsEntry4" name="hDdnsEntry4" value="-1">
                          <input type="hidden" id="hDdnsEntry5" name="hDdnsEntry5" value="-1">
                          <input type="hidden" id="hDdnsEntry6" name="hDdnsEntry6" value="-1">
                          <input type="hidden" id="hDdnsEntry7" name="hDdnsEntry7" value="-1">   						  
                          <input type="hidden" id="hDdnsEntry8" name="hDdnsEntry8" value="-1">
                          <input type="hidden" id="hDdnsEntry9" name="hDdnsEntry9" value="-1">
                          <input type="hidden" id="hDdnsEntry10" name="hDdnsEntry10" value="-1">
                          <input type="hidden" id="hDdnsEntry11" name="hDdnsEntry11" value="-1">
                          <input type="hidden" id="hDdnsEntry12" name="hDdnsEntry12" value="-1">
                          <input type="hidden" id="hDdnsEntry13" name="hDdnsEntry13" value="-1">
                          <input type="hidden" id="hDdnsEntry14" name="hDdnsEntry14" value="-1">
                          <input type="hidden" id="hDdnsEntry15" name="hDdnsEntry15" value="-1"> 					  						  
                          <input type="hidden" id="hDdnsEntry16" name="hDdnsEntry16" value="-1">
                          <input type="hidden" id="hDdnsEntry17" name="hDdnsEntry17" value="-1">
                          <input type="hidden" id="hDdnsEntry18" name="hDdnsEntry18" value="-1">
                          <input type="hidden" id="hDdnsEntry19" name="hDdnsEntry19" value="-1">
                          <input type="hidden" id="hDdnsEntry20" name="hDdnsEntry20" value="-1">
                          <input type="hidden" id="hDdnsEntry21" name="hDdnsEntry21" value="-1">
                          <input type="hidden" id="hDdnsEntry22" name="hDdnsEntry22" value="-1">
                          <input type="hidden" id="hDdnsEntry23" name="hDdnsEntry23" value="-1"> 					  						  
                          <input type="hidden" id="hDdnsEntry24" name="hDdnsEntry24" value="-1">
                          <input type="hidden" id="hDdnsEntry25" name="hDdnsEntry25" value="-1">
                          <input type="hidden" id="hDdnsEntry26" name="hDdnsEntry26" value="-1">
                          <input type="hidden" id="hDdnsEntry27" name="hDdnsEntry27" value="-1">
                          <input type="hidden" id="hDdnsEntry28" name="hDdnsEntry28" value="-1">
                          <input type="hidden" id="hDdnsEntry29" name="hDdnsEntry29" value="-1">
                          <input type="hidden" id="hDdnsEntry30" name="hDdnsEntry30" value="-1">
                          <input type="hidden" id="hDdnsEntry31" name="hDdnsEntry31" value="-1"> 			  						  
                          <input type="hidden" id="hDdnsEntry32" name="hDdnsEntry32" value="-1">
                          <input type="hidden" id="hDdnsEntry33" name="hDdnsEntry33" value="-1">
                          <input type="hidden" id="hDdnsEntry34" name="hDdnsEntry34" value="-1">
                          <input type="hidden" id="hDdnsEntry35" name="hDdnsEntry35" value="-1">
                          <input type="hidden" id="hDdnsEntry36" name="hDdnsEntry36" value="-1">
                          <input type="hidden" id="hDdnsEntry37" name="hDdnsEntry37" value="-1">
                          <input type="hidden" id="hDdnsEntry38" name="hDdnsEntry38" value="-1">
                          <input type="hidden" id="hDdnsEntry39" name="hDdnsEntry39" value="-1"> 			  						  
                          <input type="hidden" id="hDdnsEntry40" name="hDdnsEntry40" value="-1">
                          <input type="hidden" id="hDdnsEntry41" name="hDdnsEntry41" value="-1">
                          <input type="hidden" id="hDdnsEntry42" name="hDdnsEntry42" value="-1">
                          <input type="hidden" id="hDdnsEntry43" name="hDdnsEntry43" value="-1">
                          <input type="hidden" id="hDdnsEntry44" name="hDdnsEntry44" value="-1">
                          <input type="hidden" id="hDdnsEntry45" name="hDdnsEntry45" value="-1">
                          <input type="hidden" id="hDdnsEntry46" name="hDdnsEntry46" value="-1">
                          <input type="hidden" id="hDdnsEntry47" name="hDdnsEntry47" value="-1"> 					  						  
                          <input type="hidden" id="hDdnsEntry48" name="hDdnsEntry48" value="-1">
                          <input type="hidden" id="hDdnsEntry49" name="hDdnsEntry49" value="-1">
                          <input type="hidden" id="hDdnsEntry50" name="hDdnsEntry50" value="-1">
                          <input type="hidden" id="hDdnsEntry51" name="hDdnsEntry51" value="-1">
                          <input type="hidden" id="hDdnsEntry52" name="hDdnsEntry52" value="-1">
                          <input type="hidden" id="hDdnsEntry53" name="hDdnsEntry53" value="-1">
                          <input type="hidden" id="hDdnsEntry54" name="hDdnsEntry54" value="-1">
                          <input type="hidden" id="hDdnsEntry55" name="hDdnsEntry55" value="-1"> 			  						  
                          <input type="hidden" id="hDdnsEntry56" name="hDdnsEntry56" value="-1">
                          <input type="hidden" id="hDdnsEntry57" name="hDdnsEntry57" value="-1">
                          <input type="hidden" id="hDdnsEntry58" name="hDdnsEntry58" value="-1">
                          <input type="hidden" id="hDdnsEntry59" name="hDdnsEntry59" value="-1">
                          <input type="hidden" id="hDdnsEntry60" name="hDdnsEntry60" value="-1">
                          <input type="hidden" id="hDdnsEntry61" name="hDdnsEntry61" value="-1">
                          <input type="hidden" id="hDdnsEntry62" name="hDdnsEntry62" value="-1">
                          <input type="hidden" id="hDdnsEntry63" name="hDdnsEntry63" value="-1"> 
						  
                          <script language="JavaScript" type="text/JavaScript">

function	checkDdnsIdleEntry()
{
	var	nCurTemp = 0;
	var	DdnsActives = new Array(8);
	DdnsActives[0] = "<% tcWebApi_get("Ddns_Entry0","Active","s") %>";
	DdnsActives[1] = "<% tcWebApi_get("Ddns_Entry1","Active","s") %>";
	DdnsActives[2] = "<% tcWebApi_get("Ddns_Entry2","Active","s") %>";
	DdnsActives[3] = "<% tcWebApi_get("Ddns_Entry3","Active","s") %>";
	DdnsActives[4] = "<% tcWebApi_get("Ddns_Entry4","Active","s") %>";
	DdnsActives[5] = "<% tcWebApi_get("Ddns_Entry5","Active","s") %>";
	DdnsActives[6] = "<% tcWebApi_get("Ddns_Entry6","Active","s") %>";
	DdnsActives[7] = "<% tcWebApi_get("Ddns_Entry7","Active","s") %>";
	
	for(var i=0; i<8; i++)
	{
		if((DdnsActives[i] == "Yes") || (DdnsActives[i] == "No"))
		{
			nCurTemp++;
		}
		else break;
	}
	return nCurTemp;
}

function isValidDDNSProvider()
{
	var ddns_Provider = document.forms[0].DDNSProvider.value;
	var len = ddns_Provider.length;
	if(len!=0)
	{
			for ( var j = 0 ; j < len ; j++ )
			{
				var ch = ddns_Provider.charCodeAt(j)
				if(ch < 0 || ch > 255)
				{
					return false;
				}
			}
	}
	return true;
}

function btnSubmit()
{
	var curProviderMode ="<% tcWebApi_get("Ddns_Common", "useOray", "s") %>";
	var ddnsEntrylist1 ="<% tcWebApi_get("Ddns_Common","Entrylist1","s") %>";
     	var ddnsEntrylist2 ="<% tcWebApi_get("Ddns_Common","Entrylist2","s") %>";
				
	with (document.forms[0])
	{
		if (useOray_Flag.value == "Yes"  &&  curProviderMode == "No")
		{	
     			if(ddnsEntrylist1 != ""  || ddnsEntrylist2 != "")
			{
				alert('<%tcWebApi_get("String_Entry","DdnsJS14Text","s")%>');
				return;
			}
		}
		else if(useOray_Flag.value == "No"  &&  curProviderMode == "Yes")
		{
			if(ddnsEntrylist1 != "" || ddnsEntrylist2 != "")
			{
				alert('<%tcWebApi_get("String_Entry","DdnsJS15Text","s")%>');
				return;
			}
		}
		
		if(useOray_Flag.value != "Yes")
		{
		if (DDNSProviderType.value == 'Chinatelecom')
			ddns_Servername.value = "China Telecom";
		else
		{
			if(isValidDDNSProvider()==false)
			{
				alert('<%tcWebApi_get("String_Entry","DdnsJS13Text","s")%>');
				return;
			}
			ddns_Servername.value = DDNSProvider.value;
		}
		}
		
		if(DDNSCfgEnabled.checked)
			EnableDDNS_Flag.value = "Yes";
		else EnableDDNS_Flag.value = "No";
		var selObj = getElById("Interface");
		var temp = selObj.options[selObj.selectedIndex].id;
		Curddns_Flag.value = temp;
		DDNS_Flag.value = "1";
	}
	if(CheckForm()== true)
	{
		if( true == setEBooValueCookie(document.forms[0]) )
			document.forms[0].submit();
	}
}

function WriteWanNameSelected()
{
	var nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryName = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryValue = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryIndex = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var WANEnNAT = vArrayStr.split(',');
	var selObj = getElById("Interface");
	var Curddns_index = document.DdnsForm.Curddns_Flag.value;

	vEntryName = SortUtil.UpdateWanIFName( vEntryName, nEntryNum );
	
	vEntryValue = SortUtil.SortMyArray( vEntryValue );
	vEntryIndex = SortUtil.SortMyArray( vEntryIndex );

	for (i = 0; i < nEntryNum; i++)
	{
		if( ( vEntryName[i].indexOf('_INTERNET_') >= 0 || vEntryName[i].indexOf('OTHER') >= 0 )
			 && vEntryName[i].indexOf('_B_') < 0 )
		{
			IfaceNumber ++;
			if(vEntryIndex[i] == Curddns_index)
			{
				document.writeln("<option id=" + vEntryIndex[i] + " " + "value=" + vEntryValue[i] +" selected>" + vEntryName[i] + "</option>");
			}
			else
			{
				document.writeln("<option id=" + vEntryIndex[i] + " " + "value=" + vEntryValue[i] +">" + vEntryName[i] + "</option>");
			}
		}
	}
}

	  </script>
                          <BR>
                  <DIV id=ddns>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","DDNSServiceProviderText","s")%></TD>
                      <TD><SELECT onchange=ISPChange() size=1 id="DDNSProviderType" name="DDNSProviderType"> 
					  <OPTION value="Oray" <% if tcWebApi_get("Ddns_Common","useOray","h") = "Yes" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","DDNSOrayText","s")%></OPTION>
					  <OPTION value="Chinatelecom" <%if tcWebApi_get("Ddns_Common","useOray","h") = "No" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","DDNSChinaTelecomText","s")%></OPTION> 
					  </SELECT> </TD></TR>
                    <TR id=tr_ProviderName>
                      <TD width=120><%tcWebApi_get("String_Entry","DDNSServiceNameText","s")%></TD>
                      <TD><INPUT maxLength=63 id="DDNSProvider" name="DDNSProvider" value="<%if TCWebApi_get("Ddns_Entry","DDNS_Name","h" ) <> "N/A" then TCWebApi_get("Ddns_Entry","DDNS_Name","s" ) end if %>"> </TD></TR>
                    <TR id=ispinfoprotocol>
                      <TD><%tcWebApi_get("String_Entry","DDNSProtocolTypeText","s")%></TD>
                      <TD><SELECT id="DDNSProtocol" name="DDNSProtocol"> 
                          <OPTION value="GNUDip.http"
                        selected>GNUDip.http</OPTION></SELECT> </TD></TR>
			<TR id=tr_DDNSServer>
                      <TD><%tcWebApi_get("String_Entry","DDNSServiceAddrText","s")%></TD>
				<TD>
					<SELECT NAME="DDNSServer" id="DDNSServer">
						 <option value="www.dyndns.org" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.dyndns.org" then asp_Write("selected") end if %>>www.dyndns.org
						 <option value="www.justlinux.com" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.justlinux.com" then asp_Write("selected") end if %>>www.justlinux.com
						 <option value="www.dhs.org" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.dhs.org" then asp_Write("selected") end if %>>www.dhs.org
						 <option value="www.ods.org" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.ods.org" then asp_Write("selected") end if %>>www.ods.org
						 <option value="www.gnudip.cheapnet.net" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.gnudip.cheapnet.net" then asp_Write("selected") end if %>>www.gnudip.cheapnet.net
						 <option value="www.tzo.com" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.tzo.com" then asp_Write("selected") end if %>>www.tzo.com
						 <option value="www.easydns.com" <% if tcWebApi_get("Ddns_Entry","SERVERNAME","h") = "www.easydns.com" then asp_Write("selected") end if %>>www.easydns.com
					</SELECT>
				</TD>
			</TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","DDNSServicePortText","s")%></TD>
                      <TD><INPUT maxLength=63 id="ServicePort" name="ServicePort" value="<%if TCWebApi_get("Ddns_Entry","SERVICEPORT","h" ) <> "N/A" then TCWebApi_get("Ddns_Entry","SERVICEPORT","s" ) end if %>"> </TD></TR>
                    <TR id=tr_wanInterface>
                      <TD width=120><%tcWebApi_get("String_Entry","DDNSWanInterfaceText","s")%></TD>
                      <TD><SELECT size=1 id="Interface" name="Interface" onchange=interfaceChange()> 
                              <script language="JavaScript" type="text/javascript">
								WriteWanNameSelected();
							  
							</script>
                        </SELECT> </TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","DDNSHostNameText","s")%></TD>
                      <TD><INPUT maxLength=32 id="DDNSHostName" name="DDNSHostName" value="<%if TCWebApi_get("Ddns_Entry","MYHOST","h" ) <> "N/A" then TCWebApi_get("Ddns_Entry","MYHOST","s" ) end if %>"></TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","DDNSDomainNameText","s")%></TD>
                      <TD><INPUT maxLength=32 id="DDNSDomainName" name="DDNSDomainName" value="<%if TCWebApi_get("Ddns_Entry","DDNS_Domain","h" ) <> "N/A" then TCWebApi_get("Ddns_Entry","DDNS_Domain","s" ) end if %>"></TD></TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","DDNSUserNameText","s")%></TD>
                      <TD><INPUT maxLength=32 id="DDNSUsername" name="DDNSUsername" value="<%if TCWebApi_get("Ddns_Entry","USERNAME","h" ) <> "N/A" then TCWebApi_get("Ddns_Entry","USERNAME","s" ) end if %>"></TD></TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","DDNSPasswordText","s")%></TD>
                      <TD><INPUT  type=DDNSPassword maxLength=32 id="DDNSPassword" name="DDNSPassword" value="<%if TCWebApi_get("Ddns_Entry","PASSWORD","h" ) <> "N/A" then TCWebApi_get("Ddns_Entry","PASSWORD","s" ) end if %>"> </TD></TR>
                   <TR id=tr_DDNSWildCard>
                      <TD><%tcWebApi_get("String_Entry","DDNSWildCardText","s")%></TD>
                      <TD>
                      <INPUT TYPE="RADIO" NAME="Enable_Wildcard" VALUE="Yes" <% If TCWebApi_get("Ddns_Entry","WILDCARD","h") = "Yes" then asp_Write("checked") end if%> ><font color="#000000">Yes</font>
                      <INPUT TYPE="RADIO" NAME="Enable_Wildcard" VALUE="No" <% If TCWebApi_get("Ddns_Entry","WILDCARD","h") = "N/A" then asp_Write("checked") end if%> ><font color="#000000">No</font> </TD></TR>
                   <TR>
                      <TD><%tcWebApi_get("String_Entry","DDNSEnableText","s")%></TD>
                      <TD><INPUT type=checkbox maxLength=32 <%if tcWebApi_get("WebCustom_Entry","isCTSCSupported","h") <> "Yes" then asp_Write("checked") end if%> name="DDNSCfgEnabled"> 
                  </TD></TR></TBODY></TABLE></DIV></FORM><LEFT><INPUT id='Add_Button' onclick="btnSubmit()" type=button value=<%tcWebApi_get("String_Entry","DDNSAddText","s")%> name='Add_Button'> 
                  </LEFT><BR><BR>
                  <DIV id=ddnsInfo>
					<iframe src="/cgi-bin/app_ddnslist.cgi" frameborder="0" width="100%" ></iframe>
					<BR><BR><BR>
					<LEFT><INPUT id=Remove_Button onclick="btnRemove()" type=button value=<%tcWebApi_get("String_Entry","DDNSDeleteText","s")%> name=button> 
                  </LEFT></DIV>
                  <P></P></TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>°°</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7></TD>
          <TD width=7 background=/img/panel3.gif>°°</TD>
          <TD></TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>°°</TD>
          <TD width=278 bgColor=#427594>°°</TD>
          <TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
          <TD width=170 
bgColor=#313031>°°</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
