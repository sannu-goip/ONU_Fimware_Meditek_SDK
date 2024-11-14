<%
if Request_Form("Port_Flag") = "5" then
	TCWebApi_set("IpMacFilter_Common","IpMacType","IpMacType_Flag")
	TCWebApi_set("IpMacFilter_Common","Action","Actionflag")
	TCWebApi_set("IpMacFilter_Common","ActivePortOut","EnPortFilterOut_Flag")
	TCWebApi_set("IpMacFilter_Common","ListTypeIpUp","PortListType_Up_Flag")
	TCWebApi_commit("IpMacFilter_Entry")
	TCWebApi_save()
elseif Request_Form("Port_Flag") = "6" then
	TCWebApi_set("IpMacFilter_Common","IpMacType","IpMacType_Flag")
	TCWebApi_set("IpMacFilter_Common","Action","Actionflag")
	TCWebApi_set("IpMacFilter_Common","ActivePortIn","EnPortFilterIn_Flag")
	TCWebApi_set("IpMacFilter_Common","ListTypeIpDown","PortListType_Down_Flag")
	TCWebApi_commit("IpMacFilter_Entry")
	TCWebApi_save()
	
elseif Request_Form("Port_Flag") = "2" then
	if Request_Form("delnum") <> "" then
		TCWebApi_set("IpMacFilter_Common","Action","Actionflag")
		TCWebApi_set("IpMacFilter_Common","IpMacType","IpMacType_Flag")
		TCWebApi_set("IpMacFilter_Common","DeleteIndex","delnum")
	end if
	TCWebApi_commit("IpMacFilter_Entry")
	TCWebApi_save()
elseif Request_Form("Port_Flag") = "3" then
	TCWebApi_set("IpMacFilter_Common","Action","Actionflag")
	TCWebApi_set("IpMacFilter_Common","IpMacType","IpMacType_Flag")
	TCWebApi_set("IpMacFilter_Common","ListTypeIpUp","PortListType_Up_Flag")
	TCWebApi_commit("IpMacFilter_Entry")
	TCWebApi_save()
elseif Request_Form("Port_Flag") = "4" then
	TCWebApi_set("IpMacFilter_Common","Action","Actionflag")
	TCWebApi_set("IpMacFilter_Common","IpMacType","IpMacType_Flag")
	TCWebApi_set("IpMacFilter_Common","ListTypeIpDown","PortListType_Down_Flag")
	TCWebApi_commit("IpMacFilter_Entry")
	TCWebApi_save()		
end if
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","PortFilter1Text","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/json2.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ConfigForm') != null)LoadFrame()" 
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
            color=#ffffff size=6><INPUT id=Selected_Menu type=hidden 
            value="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>-><%tcWebApi_get("String_Entry","ContentPortFilterText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
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
      <SCRIPT language=JavaScript type=text/javascript>
var portFilter = [
["InternetGatewayDevice.X_ATP_Security","<% tcWebApi_get("IpMacFilter_Common","ActivePortOut","s")%>","<% tcWebApi_get("IpMacFilter_Common","ListTypeIpUp","s")%>","<% tcWebApi_get("IpMacFilter_Common","ListTypeIpDown","s")%>","<% tcWebApi_get("IpMacFilter_Common","ActivePortIn","s")%>"],
null
]
var enblFilterOut = portFilter[0][1];
var ModeUp = portFilter[0][2];
var ModeDown = portFilter[0][3];
var enblFilterIn = portFilter[0][4];

function stWanInfo(domain,Name)
{
this.domain = domain;
this.Name = Name;
}
var WanIP  = new Array(null);
var WanPPP = new Array(new stWanInfo("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.4.WANPPPConnection.2","1_TR069_INTERNET_R_0_35"),null);
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
function ShowSrcAddr(val,arrMsgInfo)
{
return val + '-' + arrMsgInfo[i][10];
}
function ShowSrcPort(val,arrMsgInfo)
{
return val + '-' + arrMsgInfo[i][11];
}
function ShowDestAddr(val,arrMsgInfo)
{
return val + '-' + arrMsgInfo[i][12];
}
function ShowDestPort(val,arrMsgInfo)
{
return val + '-' + arrMsgInfo[i++][13];
}
function ShowIface(val,arrMsgInfo)

{
for (k = 0; k < WanInfo.length; k++)
{
if (WanInfo[k].domain == arrMsgInfo[i-1][14])
{
return WanInfo[k].Name;
}
}
}
function ShowEnable(val,arrMsgInfo)
{
if (val == 1)
{
return '<%tcWebApi_get("String_Entry","PortFilter2Text","s")%>';
}
else
{
return '<%tcWebApi_get("String_Entry","PortFilter3Text","s")%>';
}
}
var FilterOut = new Array(
new Array("html/security/portfilter.asp", "FilterOut", "", "18%", "10%", "14%", "12%", "12%", "12%", "12%","10%",""),
new Array("html/security/addportfilter.asp","", "InternetGatewayDevice.X_ATP_Security.IpFilterOut", "<%tcWebApi_get("String_Entry","PortFilter4Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter5Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter6Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter7Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter8Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter9Text","s")%>","<%tcWebApi_get("String_Entry","PortFilter10Text","s")%>"),
new Array("", "", "", "", "", "ShowSrcAddr", "ShowSrcPort", "ShowDestAddr", "ShowDestPort","ShowEnable"),

new Array()
);
var FilterIn = new Array(
new Array("html/security/portfilter.asp", "FilterIn", "", "10%", "10%", "14%", "12%", "12%", "12%", "10%", "8%","12%",""),
new Array("html/security/addportfilter.asp","", "InternetGatewayDevice.X_ATP_Security.IpFilterIn", "<%tcWebApi_get("String_Entry","PortFilter11Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter5Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter6Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter7Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter8Text","s")%>", "<%tcWebApi_get("String_Entry","PortFilter9Text","s")%>","<%tcWebApi_get("String_Entry","PortFilter10Text","s")%>","<%tcWebApi_get("String_Entry","PortFilter12Text","s")%>"),
new Array("", "", "", "", "", "ShowSrcAddr", "ShowSrcPort", "ShowDestAddr", "ShowDestPort","ShowEnable","ShowIface"),

new Array()
);
var stateUp,stateDown;
function LoadFrame()
{
	document.ConfigForm.delnum.value = "";
	document.ConfigForm.Port_Flag.value = "0";
	var FilterMode = getElById("FilterMode");
	var FilterModeDown = getElById("FilterModeDown");
	var FilterInfo = getElById("FilterOutInfo");
	if (ModeUp == "black")
	{
		FilterMode[0].checked = true;
		stateUp = false;
	}
	else
	{
		FilterMode[1].checked = true;
		stateUp = true;
	}
	
	if (ModeDown == "black")
	{
		FilterModeDown[0].checked = true;
		stateDown = false;
	}
	else
	{
		FilterModeDown[1].checked = true;
		stateDown = true;
	}
	
	setCheck('enblFilterOut',enblFilterOut);
	setCheck('enblFilterIn',enblFilterIn);
}

function selFilter(filter,enable,divinfo)
{
	var Form = document.ConfigForm;
	var enableFilter = filter.checked;
	if(0 == enable){
		if (enableFilter == false)
		{
			if(!confirm("<%tcWebApi_get("String_Entry","PortFilter13Text","s")%>"))
			{
				filter.checked = true;
				Form.Port_Flag.value = "0";
				return;
			}
			Form.Port_Flag.value = "5";
			Form.IpMacType_Flag.value = "IpUp";
			setDisplay(divinfo,enableFilter);
		}
		else
		{
			if(!confirm("<%tcWebApi_get("String_Entry","PortFilter14Text","s")%>"))
			{
				filter.checked = false;
				Form.Port_Flag.value = "0";
				return;
			}
			Form.Port_Flag.value = "5";
			Form.IpMacType_Flag.value = "IpUp";
			setDisplay(divinfo,enableFilter);
		}
	}
	else if(1 == enable)
	{
		if (enableFilter == false)
		{
			if(!confirm("<%tcWebApi_get("String_Entry","PortFilter15Text","s")%>"))
			{
				filter.checked = true;
				Form.Port_Flag.value = "0";
				return;
			}
			Form.Port_Flag.value = "6";
			Form.IpMacType_Flag.value = "IpDown";
			setDisplay(divinfo,enableFilter);
		}
		else
		{
			if(!confirm("<%tcWebApi_get("String_Entry","PortFilter16Text","s")%>"))
			{
				filter.checked = false;
				Form.Port_Flag.value = "0";
				return;
			}
			Form.Port_Flag.value = "6";
			Form.IpMacType_Flag.value = "IpDown";
			setDisplay(divinfo,enableFilter);
		}
	}
	Form.Actionflag.value = "Mod";
	btnSubmit();
	
}
function ChangeMode()
{
	var FilterMode = getElById("FilterMode");
	var isFilter = getElById("enblFilterOut");
	if (FilterMode[0].checked == true && stateUp)
	{
		if (confirm("<%tcWebApi_get("String_Entry","PortFilter17Text","s")%>"))
		{
			stateUp = false;
			document.ConfigForm.PortListType_Up_Flag.value="black";
			document.ConfigForm.IpMacType_Flag.value = "IpUp";
			document.ConfigForm.Actionflag.value = "Mod";
			document.ConfigForm.Port_Flag.value = "3";
			if( true == setEBooValueCookie(document.ConfigForm) )
				document.ConfigForm.submit();
		}
		else
		{
			FilterMode[0].checked = false;
			FilterMode[1].checked = true;
			return;
		}
	}
	else if (FilterMode[1].checked == true && !stateUp)
	{
		if (confirm("<%tcWebApi_get("String_Entry","PortFilter18Text","s")%>"))
		{
			stateUp = true;
			document.ConfigForm.PortListType_Up_Flag.value="white";
			document.ConfigForm.IpMacType_Flag.value = "IpUp";
			document.ConfigForm.Actionflag.value = "Mod";
			document.ConfigForm.Port_Flag.value = "3";
			if( true == setEBooValueCookie(document.ConfigForm) )
				document.ConfigForm.submit();
		}
		else
		{
			FilterMode[0].checked = true;
			FilterMode[1].checked = false;
			return;
		}
	}
}

function ChangeModeDown()
{
	var FilterMode = getElById("FilterModeDown");
	var isFilter = getElById("enblFilterIn");
	if (FilterMode[0].checked == true && stateDown)
	{
		if (confirm("<%tcWebApi_get("String_Entry","PortFilter17Text","s")%>"))
		{
			stateDown = false;
			document.ConfigForm.PortListType_Down_Flag.value="black";
			document.ConfigForm.IpMacType_Flag.value = "IpDown";
			document.ConfigForm.Actionflag.value = "Mod";
			document.ConfigForm.Port_Flag.value = "4";
			if( true == setEBooValueCookie(document.ConfigForm) )
				document.ConfigForm.submit();
		}
		else
		{
			FilterMode[0].checked = false;
			FilterMode[1].checked = true;
			return;
		}
	}
	else if (FilterMode[1].checked == true && !stateDown)
	{
		if (confirm("<%tcWebApi_get("String_Entry","PortFilter18Text","s")%>"))
		{
			stateDown = true;
			document.ConfigForm.PortListType_Down_Flag.value="white";
			document.ConfigForm.IpMacType_Flag.value = "IpDown";
			document.ConfigForm.Actionflag.value = "Mod";
			document.ConfigForm.Port_Flag.value = "4";
			if( true == setEBooValueCookie(document.ConfigForm) )
				document.ConfigForm.submit();
		}
		else
		{
			FilterMode[0].checked = true;
			FilterMode[1].checked = false;
			return;
		}
	}
}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","PortFilter1Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
          rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#端口过滤" 
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
            <FORM name="ConfigForm" action="/cgi-bin/sec-portfilter.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <DIV><font color=red><%tcWebApi_get("String_Entry","WanSetting1IPV6","s")%></font></DIV><BR>
                  <DIV><%tcWebApi_get("String_Entry","PortFilter19Text","s")%></DIV>
                          <input type="hidden" id="Port_Flag" name="Port_Flag" value="0">
						  <input type="hidden" id="PortListType_Up_Flag" name="PortListType_Up_Flag" value="<% tcWebApi_get("IpMacFilter_Common","ListTypeIpUp","s") %>">
						  <input type="hidden" id="PortListType_Down_Flag" name="PortListType_Down_Flag" value="<% tcWebApi_get("IpMacFilter_Common","ListTypeIpDown","s") %>">
						  <input type="hidden" id="EnPortFilterOut_Flag" name="EnPortFilterOut_Flag" value="<% tcWebApi_get("IpMacFilter_Common","ActivePortOut","s") %>">
						  <input type="hidden" id="EnPortFilterIn_Flag" name="EnPortFilterIn_Flag" value="<% tcWebApi_get("IpMacFilter_Common","ActivePortIn","s") %>">
                          <input type="hidden" id="delnum" name="delnum" value="">
                          <input type="hidden" id="Actionflag" name="Actionflag" value="Mod">
						  <input type="hidden" id="IpMacType_Flag" name="IpMacType_Flag" value="IpUp">
                          <input type="hidden" id="ipup_num" name="ipup_num" value="<% tcWebApi_get("IpMacFilter","ipup_num","s") %>">
                          <input type="hidden" id="ipdown_num" name="ipdown_num" value="<% tcWebApi_get("IpMacFilter","ipdown_num","s") %>"> 
                          <SCRIPT language=JavaScript src="/JS/printbox.js"></SCRIPT>
						  <script language="JavaScript" type="text/JavaScript">
function addClick(iType)
{
	var Form = document.ConfigForm;
	var FilterType;
	var typeFlag = '&typeFlag=';

	if(iType == 0)
	{
		FilterType = "40";
		if(parseInt(Form.ipup_num.value) >= 100)
		{
			alert("<%tcWebApi_get("String_Entry","PortFilter20Text","s")%>");
			return false;
		}
		typeFlag += 'IpUp';
	}
	else
	{
		FilterType = 140;
		if(parseInt(Form.ipdown_num.value) >= 100)
		{
			alert("<%tcWebApi_get("String_Entry","PortFilter21Text","s")%>");
			return false;
		}
		typeFlag += 'IpDown';
	}
	window.location = '/cgi-bin/sec-addportfilter.asp?PortFilterType='+FilterType+typeFlag;
}
function removeClick(iType)
{
	document.ConfigForm.Port_Flag.value = "2";
	if(0 == iType)
	{
		document.ConfigForm.IpMacType_Flag.value = "IpUp";
	}
	else
	{
		document.ConfigForm.IpMacType_Flag.value = "IpDown";
	}
	document.ConfigForm.Actionflag.value = "Del";
	if( true == setEBooValueCookie(document.ConfigForm) )
		document.ConfigForm.submit();
}
function	btnSubmit()
{
	var Form = document.ConfigForm;
	if(Form.enblFilterOut.checked)
	{
		Form.EnPortFilterOut_Flag.value = "1";
		if (Form.FilterMode[0].checked)
			Form.PortListType_Up_Flag.value = "black";
		else if (Form.FilterMode[1].checked)
			Form.PortListType_Up_Flag.value = "white";
	}
	else
	{
		Form.EnPortFilterOut_Flag.value = "0";
	}
	if(Form.enblFilterIn.checked)
	{
		Form.EnPortFilterIn_Flag.value = "1";
		if (Form.FilterModeDown[0].checked)
			Form.PortListType_Down_Flag.value = "black";
		else if (Form.FilterModeDown[1].checked)
			Form.PortListType_Down_Flag.value = "white";		
	}
	else
	{
		Form.EnPortFilterIn_Flag.value = "0";		
	}
	if( true == setEBooValueCookie(Form) )
		Form.submit();
}
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
function doDelOut(i)
{
	var f = document.ConfigForm;
	var tempstr;
	var rmlout = document.getElementsByName('rmlout');
	if (rmlout == null)
		return;
	if(rmlout.length>0)
	{
		for(var n=0;n<(rmlout.length);n++)
		{
			if(rmlout[n].value == i){
				if(rmlout[n].checked)
				{
					tempstr = i + ",";
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
function doDelIn(i)
{
	var f = document.ConfigForm;
	var tempstr;
	var rmlin = document.getElementsByName('rmlin');
	if (rmlin == null)
		return;
	if(rmlin.length>0)
	{
		for(var n=0;n<(rmlin.length);n++)
		{
			if(rmlin[n].value == i){
				if(rmlin[n].checked)
				{
					tempstr = i + ",";
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
</script>
                          <DIV><%tcWebApi_get("String_Entry","PortFilter22Text","s")%><INPUT id=enblFilterOut onclick="selFilter(this, 0,'FilterOutInfo')" type=checkbox name="enblFilterOut" <%if tcWebApi_get("IpMacFilter_Common","ActivePortOut","h") = "1" then asp_Write("checked") end if%>></DIV><BR>
                  <DIV id=FilterOutInfo><%tcWebApi_get("String_Entry","PortFilter23Text","s")%>
				  <INPUT id=FilterMode onclick="ChangeMode()" type=radio name="FilterMode" <%if tcWebApi_get("IpMacFilter_Common","ListTypeIpUp","h") = "black" then asp_Write("checked") end if%>><%tcWebApi_get("String_Entry","PortFilter24Text","s")%> 
				  <INPUT id=FilterMode onclick="ChangeMode()" type=radio name="FilterMode" <%if tcWebApi_get("IpMacFilter_Common","ListTypeIpUp","h") = "white" then asp_Write("checked") end if%>><%tcWebApi_get("String_Entry","PortFilter25Text","s")%> <BR><BR>
                  <SCRIPT language=JavaScript type=text/javascript>
				  	setDisplay("FilterOutInfo",enblFilterOut);
				  </SCRIPT>
                            <% if tcWebApi_get("IpMacFilter_Common","ActivePortOut","h") = "1" then %>
					<table border="1" align=center cellpadding="3" cellspacing="0" bordercolor="#CCCCCC" bgcolor="#FFFFFF">
						<TBODY>
							<tr>
								<TD align="middle" width="14%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter4Text","s")%></strong></TD>
								<TD align="middle" width="10%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter5Text","s")%></strong></TD>
								<TD align="middle" width="14%" class='table_title'><STRONG><%tcWebApi_get("String_Entry","PortFilter6Text","s")%></STRONG></TD>
								<TD align="middle" width="12%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter7Text","s")%></strong></TD>
								<TD align="middle" width="12%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter8Text","s")%></strong></TD>
								<TD align="middle" width="16%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter9Text","s")%></strong></TD>
								<TD align="middle" width="12%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter10Text","s")%></strong></TD>
								<TD align="middle" width="10%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter27Text","s")%></strong></TD>
							</tr>
							<SCRIPT language=JavaScript type=text/javascript>
									               
							 var portOutvalue = '<% tcWebApi_JsonHook("get_portfilterOut_info", "action", "area") %>'
							var rsp = JSON.parse(portOutvalue);
							var portOut_list = rsp.data;
							for(var i = 0; i < portOut_list.length; i++)
							{
								if( portOut_list[i]['Protocol']== "All" || portOut_list[i]['Protocol']== "ALL" )
									portOut_list[i]['Protocol'] = "ANY";
								document.write('<TR align="center">');
								document.write('<TD align="center" width="14%">' + portOut_list[i]['IPName'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="10%">' + portOut_list[i]['Protocol'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="14%">' + portOut_list[i]['SrcIPAddr'] + '/' + portOut_list[i]['SrcIPMask'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="12%">' + portOut_list[i]['SrcPort'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="14%">' + portOut_list[i]['DesIPAddr'] + '/' + portOut_list[i]['DesIPMask'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="16%">' + portOut_list[i]['DesPort'] + '&nbsp;</TD>');
								if(portOut_list[i]['Active'] == "Yes")
									document.write('<TD align="center" width="12%">' + '<%tcWebApi_get("String_Entry","PortFilter2Text","s")%>' + '&nbsp;</TD>');
								else
									document.write('<TD align="center" width="10%">' + '<%tcWebApi_get("String_Entry","PortFilter3Text","s")%>' + '&nbsp;</TD>');
								document.write('<TD width="10%"><input type="checkbox" id="rmlout" name="rmlout" onclick="doDelOut(' + portOut_list[i]['Index'] + ')" value="' + portOut_list[i]['Index'] + '"></TD>');
								document.write('</TR>');	
							}			          
					</SCRIPT>
					</TBODY> 
					</TABLE>  
                            <% end if %>
                            <BR>
                            <BR>
				  <INPUT id=btnAdd name=btnAdd onclick="addClick(0)" type=button value=<%tcWebApi_get("String_Entry","PortFilter26Text","s")%>> 
				  <INPUT id=btnRemove name=btnRemove onclick="removeClick(0)" type=button value=<%tcWebApi_get("String_Entry","PortFilter27Text","s")%>>
                          </DIV>                            
						  <HR>

                  <DIV><%tcWebApi_get("String_Entry","PortFilter28Text","s")%></DIV><BR>
                  <DIV><%tcWebApi_get("String_Entry","PortFilter22Text","s")%><INPUT id=enblFilterIn  onclick="selFilter(this, 1,'FilterInInfo')" type=checkbox name="enblFilterIn" <%if tcWebApi_get("IpMacFilter_Common","ActivePortIn","h") = "1" then asp_Write("checked") end if%>> </DIV><BR>
                  <DIV id=FilterInInfo><%tcWebApi_get("String_Entry","PortFilter23Text","s")%>
				  <INPUT id=FilterModeDown onclick="ChangeModeDown()" type=radio name="FilterModeDown" <%if tcWebApi_get("IpMacFilter_Common","ListTypeIpDown","h") = "black" then asp_Write("checked") end if%>><%tcWebApi_get("String_Entry","PortFilter24Text","s")%> 
				  <INPUT id=FilterModeDown onclick="ChangeModeDown()" type=radio name="FilterModeDown" <%if tcWebApi_get("IpMacFilter_Common","ListTypeIpDown","h") = "white" then asp_Write("checked") end if%>><%tcWebApi_get("String_Entry","PortFilter25Text","s")%> <BR><BR>
                  <SCRIPT language=JavaScript type=text/javascript>
					  setDisplay("FilterInInfo",enblFilterIn);
				  </SCRIPT>
                            <% if tcWebApi_get("IpMacFilter_Common","ActivePortIn","h") = "1" then %>
				<table border="1" align=center cellpadding="3" cellspacing="0" bordercolor="#CCCCCC" bgcolor="#FFFFFF">
					<TBODY>
						<tr>
							<TD align="middle" width="10%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter4Text","s")%></strong></TD>
							<TD align="middle" width="10%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter5Text","s")%></strong></TD>
							<TD align="middle" width="14%" class='table_title'><STRONG><%tcWebApi_get("String_Entry","PortFilter6Text","s")%></STRONG></TD>
							<TD align="middle" width="12%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter7Text","s")%></strong></TD>
							<TD align="middle" width="12%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter8Text","s")%></strong></TD>
							<TD align="middle" width="12%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter9Text","s")%></strong></TD>
							<TD align="middle" width="10%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter10Text","s")%></strong></TD>
							<TD align="center" width="10%" class='table_title'><%tcWebApi_get("String_Entry","PortFilter12Text","s")%></TD>
							<TD align="middle" width="10%" class='table_title'><strong><%tcWebApi_get("String_Entry","PortFilter27Text","s")%></strong></TD>
						</tr>
						<SCRIPT language=JavaScript type=text/javascript>
		                  		var portInvalue = '<% tcWebApi_JsonHook("get_portfilterIn_info", "action", "area") %>'
								var rsp = JSON.parse(portInvalue);
								var portIn_list = rsp.data;
								for(var i = 0; i < portIn_list.length; i++)
								{
									if( portIn_list[i]['Protocol'] == "All" || portIn_list[i]['Protocol'] == "ALL" )
										portIn_list[i]['Protocol'] = "ANY";									
									document.write('<TR align="center">');
									document.write('<TD align="center" width="10%">' + portIn_list[i]['IPName'] + '&nbsp;</TD>');
									document.write('<TD align="center" width="10%">' + portIn_list[i]['Protocol'] + '&nbsp;</TD>');
									document.write('<TD align="center" width="14%">' + portIn_list[i]['SrcIPAddr'] + '/' + portIn_list[i]['SrcIPMask'] + '&nbsp;</TD>');
									document.write('<TD align="center" width="12%">' + portIn_list[i]['SrcPort'] + '&nbsp;</TD>');
									document.write('<TD align="center" width="12%">' + portIn_list[i]['DesIPAddr'] + '/' + portIn_list[i]['DesIPMask'] + '&nbsp;</TD>');
									document.write('<TD align="center" width="12%">' + portIn_list[i]['DesPort'] + '&nbsp;</TD>');
									if(portIn_list[i]['Active'] == "Yes")
										document.write('<TD align="center" width="10%">' + '<%tcWebApi_get("String_Entry","PortFilter2Text","s")%>' + '&nbsp;</TD>');
									else
										document.write('<TD align="center" width="10%">' + '<%tcWebApi_get("String_Entry","PortFilter3Text","s")%>' + '&nbsp;</TD>');
									document.write('<TD align="center" width="12%">' + portIn_list[i]['Interface'] + '&nbsp;</TD>');
									document.write('<TD width="10%"><input type="checkbox" id="rmlin" name="rmlin" onclick="doDelIn(' + portIn_list[i]['Index'] + ')" value="' + portIn_list[i]['Index'] + '"></TD>');
									document.write('</TR>');	
								}
						</SCRIPT>
				</TBODY> 
			</TABLE>  
					<% end if %>
                            <p><BR>
                              <BR>
                              <INPUT id=downLink_btnAdd name=downLink_btnAdd onclick="addClick(1)" type=button value=<%tcWebApi_get("String_Entry","PortFilter26Text","s")%>>
                              <INPUT id=downLink_btnRemove name=downLink_btnRemove onclick="removeClick(1)" type=button value=<%tcWebApi_get("String_Entry","PortFilter27Text","s")%>>
                            </p>
                          </DIV>							
						  
                        </TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD> </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></FORM></TD></TR>
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
          <TD width=196 bgColor=#427594>
            <P align=center><IMG id=btnOK onclick="btnSubmit()" height=23 
            src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width=80 border=0>&nbsp;&nbsp;<IMG 
            id=btnCancel onclick="RefreshPage()" height=23 src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" 
            width=80 border=0> </P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
