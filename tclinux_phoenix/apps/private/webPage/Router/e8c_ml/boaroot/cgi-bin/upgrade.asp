<%
if Request_Form("postflag") = "1" then
		tcWebApi_constSet("WebCustom_Entry","web_upgrade","1")
		TCWebApi_set("System_Entry","upgrade_fw","HTML_HEADER_TYPE")
		TCWebApi_commit("System_Entry")
end if	
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<%if tcWebApi_get("System_Entry","upgrade_fw_status","h") <> "SUCCESS" then%>
<HEAD><TITLE>FW Upgrade</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 leftMargin=0 topMargin=0 onload="if(getElById('ConfigForm') != null)LoadFrame()" onunload=DoUnload() marginheight="0" marginwidth="0">
<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center border=0>
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
                <TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","TitleWelcomeText","s")%> </TD>
                <TD vAlign=bottom width=50><A onclick=DoLogout() 
                  href="/cgi-bin/login.asp" target=_top><SPAN 
                  class=logout><%tcWebApi_get("String_Entry","TitleLogOutText","s")%> 
      </SPAN></A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
      <TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
        <TBODY>
        <TR>
          <TD class=LocationDisplay id=LocationDisplay align=middle width=163 
          bgColor=#ef8218 rowSpan=3></TD>
          <TD width=434 bgColor=#427594 height=33>
            <P align=right><FONT face=<%tcWebApi_get("String_Entry","RomanText","s")%>  color=#ffffff><B><FONT face=<%tcWebApi_get("String_Entry","RomanText","s")%>  
            color=#ffffff size=6><INPUT id=Selected_Menu type=hidden 
      value="<%tcWebApi_get("String_Entry","ContentManageText","s")%>-><%tcWebApi_get("String_Entry","ContentUpgradeText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
            class=GatewayName><%tcWebApi_get("String_Entry","TitleGateWayNameText","s")%> 
<SCRIPT language=javascript>
document.write(top.gateWayName);
</SCRIPT>
             </SPAN></FONT></P></TD>
          <TD width=211 bgColor=#ef8218 height=33>
 <P class=GatewayType align=center><%tcWebApi_get("String_Entry","TitleGateWayTypeText","s")%> 
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
      <SCRIPT language=javascript>
			MakeMenu(getElById('Selected_Menu').value);
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

function Upgrade()
{
	this.UpgradeStatus = '<% tcWebApi_get("System_Entry","upgrade_fw_status","s")%>';
}
var up = new Upgrade();

function LoadFrame()
{
	resetEBooValueCookie();
}

function btnCommit()
{
	var form=document.ConfigForm;
	var string3 = form.FW_UploadFile.value.search(/romfile/);
	var string4 = form.FW_UploadFile.value.search(/tclinux/);
<% if tcwebApi_get("WebCustom_Entry","isAllinoneUploadSupported","h") ="Yes" then %>
	var string5 = form.FW_UploadFile.value.search(/tclinux_allinone/);
<%end if %>

	if(checkVoipIdle() == false)
	{
		return false;
	}
	
	if (form.FW_UploadFile.value=="") {
		alert("<%tcWebApi_get("String_Entry","ManageUpgradeFileText","s")%> ");
	}
	else {
		if (((form.upload_type[0].checked) && (string3 >= 0))
<% if tcwebApi_get("WebCustom_Entry","isAllinoneUploadSupported","h") ="Yes" then %>
				|| ((form.upload_type[1].checked) && (string4 >= 0 && string5<0))
				|| ((form.upload_type[2].checked) && (string5 >= 0))
<% else %>
				 || ((form.upload_type[1].checked) && (string4 >= 0))
<%end if %>
		) {
				document.getElementById("warn").innerText = "<%tcWebApi_get("String_Entry","ManagementUpgradeWaitText","s")%> ";
			
			form.postflag.value = "1";
			form.submit();
		}
		else
			alert("<%tcWebApi_get("String_Entry","ManagementUpgradeErrorText","s")%>");
	}
}
function backup_settings()
{
	var cfg = '/romfile.cfg';
	var code = 'location.assign("' + cfg + '")';
	eval(code);
}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","ManagementUpgrade1Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474></TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><!--A 
                  href="/cgi-bin/help_content.asp#<%tcWebApi_get("String_Entry","ManagementUpgrade2Text","s")%>" 
                  target=_blank><IMG height=34 src="/img/help.gif" 
                  width=40 border=0></A--></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
              </TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <FORM ENCTYPE="multipart/form-data" name="ConfigForm" method="post">
              <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <tr>
                  <td width="170">&nbsp;</td>
                  <td width="90">Upgrade Reset: </td>
                  <td width="200">
                    <input id="reset_flag" type="checkbox" name="reset_flag" onclick="EnableReset()">
                      <input id="goip_reset_flag" name="goip_reset_flag" type="hidden" value="0">
                  </td>
                </tr>
              <TR>
                <TD width=12>&nbsp;</TD>
                <TD width="447">
					<INPUT TYPE="HIDDEN" NAME="postflag" VALUE="1">
					<INPUT TYPE="HIDDEN" NAME="HTML_HEADER_TYPE" VALUE="2">
               	</TD>
                <TD width=10>&nbsp;</TD></TR>
			  <TR>
                <TD width=12>&nbsp;</TD>
                <TD align="center" width="447">
						<input name="upload_type" type="radio" value="1" CHECKED>romfile
						<input name="upload_type" type="radio" value="4">tclinux.bin
<% if tcwebApi_get("WebCustom_Entry","isAllinoneUploadSupported","h") ="Yes" then %>
						<input name="upload_type" type="radio" value="5">tclinux_allinone
<%end if %>
               	</TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=12>&nbsp;</TD>
                <TD>
					<%tcWebApi_get("String_Entry","ManageUpgradePathText","s")%>
				</TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=12>&nbsp;</TD>
                <TD align="center">
					<INPUT TYPE="FILE" NAME="FW_UploadFile" SIZE="30" MAXLENGTH="128">
				</TD>
                <TD width=10>&nbsp;</TD></TR>
			 <TR>
                <TD width=12>&nbsp;</TD>
                <TD align="center">
					<input type="button" value="ROMFILE BACKUP" onClick='backup_settings()'>
				</TD>
                <TD width=10>&nbsp;</TD></TR>
			 <TR>
                <TD width=12>&nbsp;</TD>
                <TD align="center">
					
				</TD>
                <TD width=10>&nbsp;</TD></TR>
			  <TR>
                <TD width=12>&nbsp;</TD>
                <TD width="447">
			            <label ID="warn" SIZE="55" MAXLENGTH="55" VALUE="" class="InputTextWarning">
                           <script language=javascript>
			    	if(up.UpgradeStatus == "NONE") 
                              	{
                                   document.getElementById("warn").innerText="";
                              	}                                             
			     	else if(up.UpgradeStatus == "FAIL")  
                             	{
                                   document.getElementById("warn").innerText="<%tcWebApi_get("String_Entry","ManageUpgradeFailText","s")%>";
                              	}                                                    
                           </script>
               	</TD>
                <TD width=10>&nbsp;</TD></TR>
			 <TR>
                <TD width=12>&nbsp;</TD>
                <TD align="center">
					
				</TD>
                <TD width=10>&nbsp;</TD></TR>
			 <TR>
                <TD width=12>&nbsp;</TD>
                <TD>
					<font color="#FF0000"><%tcWebApi_get("String_Entry","ManageUpgradeNoteText","s")%></font>
				</TD>
                <TD width=10>&nbsp;</TD></TR>
				</TBODY></TABLE></FORM></TD></TR>
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
            <P align=center>
			<IMG id="btnOK" onclick="btnCommit()" height="23"src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width="80" border="0">&nbsp;&nbsp;
			<IMG id="btnCancel" onclick="RefreshPage()" height="23" src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" width="80" border="0">
			</P></TD>
          <TD width=170 bgColor=#313031>　</TD></TR>
        </TABLE></TD></TR></TBODY></TABLE></BODY>
<%else%>
<% if tcwebApi_get("WebCustom_Entry","isAllinoneUploadSupported","h") ="Yes" then %>
<%if tcWebApi_get("System_Entry","upgrade_fw_type","h") = "5" then%>
<HEAD></HEAD>
<BODY><font color=red><%tcWebApi_get("String_Entry","ManageUpgradesucessText","s")%></font></BODY>
<%else%>
<HEAD><meta http-equiv="Refresh" content="115"; url="upgrade.asp"></HEAD>
<BODY><font color=red><%tcWebApi_get("String_Entry","ManageUpgradeFilesucessText","s")%></font></BODY>
<%end if%>
<%else%>
<HEAD><meta http-equiv="Refresh" content="115"; url="upgrade.asp"></HEAD>
<BODY><font color=red><%tcWebApi_get("String_Entry","ManageUpgradeFilesucessText","s")%></font></BODY>
<%end if%>
<%end if%>
</HTML>