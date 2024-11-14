<%
	If Request_Form("submitFlag") = "1" Then
		If Request_Form("PortTriggeringList_Cntr0") = "1" Then
			tcWebApi_unset("portTriggering_Entry0")
		End If
		If Request_Form("PortTriggeringList_Cntr1") = "1" Then
			tcWebApi_unset("portTriggering_Entry1")
		End If
		If Request_Form("PortTriggeringList_Cntr2") = "1" Then
			tcWebApi_unset("portTriggering_Entry2")
		End If
		If Request_Form("PortTriggeringList_Cntr3") = "1" Then
			tcWebApi_unset("portTriggering_Entry3")
		End If
		If Request_Form("PortTriggeringList_Cntr4") = "1" Then
			tcWebApi_unset("portTriggering_Entry4")
		End If
		If Request_Form("PortTriggeringList_Cntr5") = "1" Then
			tcWebApi_unset("portTriggering_Entry5")
		End If
		If Request_Form("PortTriggeringList_Cntr6") = "1" Then
			tcWebApi_unset("portTriggering_Entry6")
		End If
		If Request_Form("PortTriggeringList_Cntr7") = "1" Then
			tcWebApi_unset("portTriggering_Entry7")
		End If
		tcWebApi_commit("portTriggering")
	End If
%>
<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" href='/JS/stylemain.css' type='text/css'>
<title>PortTriggering</title>
<style type="text/css">
	STYLE2 {font-size: 12px}
</style>
<script language="javascript" src="/JS/menu.js"></script>
<script language="javascript" src="/JS/jquery.js"></script>
<script language="javascript" src="/JS/util.js"></script>
<script language="javascript">
function doSave() 
{
	var ObjectNameStr = "PortTriggeringList_Cntr0";
	active = 0;
	for(i = 0; i < 8; i++)
	{
		ObjectNameStr = "PortTriggeringList_Cntr" + i;
		if(1 == document.getElementById(ObjectNameStr).value)
		{
			active++;		
		}
	}
	
	if(!active)
	{
		alert("<%tcWebApi_get("String_Entry","NatPortTriggerlistJS1Text","s")%>");
		return true;
	}
	document.PORTTRIGGERINGList_form.submitFlag.value = 1;
	if( true == setEBooValueCookie(document.PORTTRIGGERINGList_form) )
		document.PORTTRIGGERINGList_form.submit();
}

function btnRtturnNAT()
{
	location.replace("app-natset.asp");
	return;
}
/******************For Multi-Language Strings Start*************************/
var	ary_strings = [			
				['porttriggerClear','<%tcWebApi_get("String_Entry","NatPortMappingDelText","s")%>'],
				['porttriggerApp','<%tcWebApi_get("String_Entry","NatPortTriggerApplicationText","s")%>'],
				['porttriggerRag','<%tcWebApi_get("String_Entry","NatPortTriggerRangeText","s")%>'],																	
				['porttriggerOpen','<%tcWebApi_get("String_Entry","NatPortTriggerOpeningRangeText","s")%>'],
				['porttriggerSPort','<%tcWebApi_get("String_Entry","NatPortTriggerStartPort0Text","s")%>'],
				['porttriggerEPort','<%tcWebApi_get("String_Entry","NatPortTriggerStartPort1Text","s")%>'],
				['porttriggerProtocol','<%tcWebApi_get("String_Entry","NatPortTriggerStartPort2Text","s")%>'],									
				['porttriggerAim','Aim Talk'],
				['porttriggerAsh',"Asheron's Call"],
				['porttriggerCal','Calista IPPhone'],
				['porttriggerDel','Delta Force'],
				['porttriggerICQ','ICQ'],
				['porttriggerNap',"Napster"],
				['porttriggerNet','Net2Phone'],
				['porttriggerQui','QuickTimeClient'],				
				['porttriggerRai','Rainbow 6 Game'],													
				['','']
			];
/******************End*************************/
</script>
</head>
<body topmargin="0" leftmargin="0" marginwidth="0" marginheight="0" alink="#000000" link="#000000" vlink="#000000" style="text-align: center" 
onLoad="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ConfigForm') != null)LoadFrame()"
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
<%tcWebApi_get("String_Entry","WelcomeSettingText","s")%> ?
</td>
<td width="50" valign="bottom"><a href="/cgi-bin/logout.cgi" target="_top" onClick="DoLogout()">
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

  <tr>
    <td valign="top"> 
    <table height="100%" border="0" cellpadding="0" cellspacing="0">
		  <tr>
			<td width="157" bgcolor="#EF8218" height="29"><p class="Item_L1"><%tcWebApi_get("String_Entry","NatPortTriggerSummaryText","s")%></p></td>
			<td width="7" bgcolor="#EF8218"></td>
			<td width="474">　</td>
			<td width="170"  rowspan="5" valign="top" background="/img/panel4.gif">
			  <table width="100%" border="0" cellspacing="0" cellpadding="20" height='100%'>
				<tr>
				   <td valign='top'><a href="../cgi-bin/help_content.asp#高级NAT配置" target="_blank"><img src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width="40" height="34" border="0"></a></td>
				</tr>
				<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
				<tr>
				  <td valign='bottom'><img src='/img/tybottom.jpg' height=76 width=112></td>
				</tr>
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
			<td>
   			   <FORM id=PORTTRIGGERINGList_form name=PORTTRIGGERINGList_form method="post">
		       <table width="100%" border="0" cellspacing="0" cellpadding="0">
		       <tr>
		         <td>
					<table width="470" border="0" height="250" align=center cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
						<tr>
							<td width="470" align=center>
							<iframe src="/cgi-bin/porttriggering_list.cgi" frameborder="0" width="100%" height="100%" align=left></iframe>
							</td>
						</tr>
					</table>
					<table  width="480" border="0" align=center cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
						<tr>
							<td align=center>
								<INPUT TYPE="BUTTON" NAME="SaveBtn" VALUE="<%tcWebApi_get("String_Entry","NatBtnSaveText","s")%>" onClick="doSave();">
								<INPUT TYPE="HIDDEN" NAME="submitFlag" id="submitFlag" VALUE="0">						
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr0" id="PortTriggeringList_Cntr0" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr1" id="PortTriggeringList_Cntr1" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr2" id="PortTriggeringList_Cntr2" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr3" id="PortTriggeringList_Cntr3" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr4" id="PortTriggeringList_Cntr4" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr5" id="PortTriggeringList_Cntr5" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr6" id="PortTriggeringList_Cntr6" VALUE="0">
								<INPUT TYPE="HIDDEN" NAME="PortTriggeringList_Cntr7" id="PortTriggeringList_Cntr7" VALUE="0">
							</td>
						</tr>
					</table>
		         </td>
		       </tr>
		       <tr>
		         <td>&nbsp;</td>
		       </tr>	
		       <tr>
				<td width="92" height="26" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				<input name="RtturnNAT" type="button" id="RtturnNAT" onClick="btnRtturnNAT()" value="<%tcWebApi_get("String_Entry","NatPortTriggerlistRtnText","s")%>"/></td>
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
<TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR>
</table>
</body>
</html>
