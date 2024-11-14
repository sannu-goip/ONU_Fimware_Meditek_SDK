<%
if Request_Form("Samba_Flag")="1" then 
	TCWebApi_set("Samba_Entry0","Active","Samba_active")
	if TCWebAPI_get("Samba_Entry0", "Active", "h") = "Yes" then
	TCWebAPI_set("Samba_Entry0","username","smbusername")
	TCWebAPI_set("Samba_Entry0","Passwd","smbpwd")
	TCWebAPI_set("Samba_Entry0","WorkGroup","WorkGroup")
	TCWebAPI_set("Samba_Entry0","NetBiosName","NetBiosName")
	end if
	TCWebApi_save()
	TCWebAPI_commit("Samba")	
end If
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","SambaSettingText","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>


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
            value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>-><%tcWebApi_get("String_Entry","ContentSAMBAText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
            class=GatewayName><%tcWebApi_get("String_Entry","GatewayNameText","s")%> 
<SCRIPT language=javascript>
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
		function OnOK()
		{
			if(document.forms[0].Samba_active[0].checked){
				with (document.forms[0])
				{
						if(isValidNameEx(smbpwd.value) == false )
						{
							alert("<%tcWebApi_get("String_Entry","SambaPwdErrorText","s")%>");
							smbpwd.focus();
							return false;
						}
						if(isValidNameEx(WorkGroup.value) == false )
						{
							WorkGroup.focus();
							alert("<%tcWebApi_get("String_Entry","SambaJS2Text","s")%>");
							return false;
						}
						if(isValidNameEx(NetBiosName.value) == false )
						{
							NetBiosName.focus();
							alert("<%tcWebApi_get("String_Entry","SambaJS3Text","s")%>");
							return false;
						}
						if((smbpwd.value.length == 0)||(smbpwd.value.length > 15))
						{
							alert("<%tcWebApi_get("String_Entry","SambaJS4Text","s")%>");
							smbpwd.focus();
							return false;
						}
						if((WorkGroup.value.length == 0)||(WorkGroup.value.length > 15))
						{
							alert("<%tcWebApi_get("String_Entry","SambaJS5Text","s")%>");
							WorkGroup.focus();
							return false;
						}
						if((NetBiosName.value.length == 0)||(NetBiosName.value.length > 15))
						{
							alert("<%tcWebApi_get("String_Entry","SambaJS6Text","s")%>");
							NetBiosName.focus();
							return false;
						}
						if (confirmSambasmbpwd.value != smbpwd.value) 
						{
				        	alert("<%tcWebApi_get("String_Entry","SambaJS7Text","s")%>");
				        	return false;
				    }	
				}
			}
			document.forms[0].Samba_Flag.value = "1";
			if( true == setEBooValueCookie(document.forms[0]) )
				document.forms[0].submit();
		}
function isValidNameEx(name) {
   var i = 0;	
   
   for ( i = 0; i < name.length; i++ ) {
      if ( isNameUnsafeEx(name.charAt(i)) == true ){
				return false;
			}
   }

   return true;
}
function isNameUnsafeEx(compareChar)
{	
   if ( compareChar.charCodeAt(0) > 32
        && compareChar.charCodeAt(0) < 127)
      return false; // found no unsafe chars, return false
   else
      return true;
}
function sambaOff(off){
	if(off){
  document.ConfigForm.smbusername.disabled = true;
	document.ConfigForm.smbpwd.disabled = true;
	document.ConfigForm.confirmSambasmbpwd.disabled = true;
	document.ConfigForm.WorkGroup.disabled = true;
	document.ConfigForm.NetBiosName.disabled = true;	
	}
	else{
	document.ConfigForm.smbusername.disabled = false;
	document.ConfigForm.smbpwd.disabled = false;
	document.ConfigForm.confirmSambasmbpwd.disabled = false;
	document.ConfigForm.WorkGroup.disabled = false;
	document.ConfigForm.NetBiosName.disabled = false;		
	}
}	
function LoadFrame()
{
<% if TCWebApi_get("Samba_Entry0","Active","h") = "Yes" then %>
     sambaOff(0);
<% else %>
     sambaOff(1);
<% end if %> 
}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","SambaSettingText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474></TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#Samba" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0></A></TD></TR>
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
            <FORM name=ConfigForm id=ConfigForm action="/cgi-bin/app-samba.asp" method="post">
						<input type="hidden" name="EnableSAMBA_Flag" value="<% tcWebApi_get("Samba_Entry0","Active","s") %>">
						 	<input type="hidden" name="Samba_Flag" value="0">
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    	<TR>
				        	<TD><P><font color=red><%tcWebApi_get("String_Entry","WanSetting1IPV6","s")%></font></P><BR></TD>
				        </TR>
                    	<TR>
                      <TD width=80>Samba:</TD>
                      <TD>
                      	<INPUT type="radio" onClick="sambaOff(0);" name="Samba_active" VALUE="Yes" <% If TCWebApi_get("Samba_Entry0","Active","h") = "Yes" then asp_Write("checked") end if %> ><%tcWebApi_get("String_Entry","SambaActivatedText","s")%>
                      <INPUT onclick="sambaOff(1);" value="No" type="radio" name="Samba_active" <% If TCWebApi_get("Samba_Entry0","Active","h") = "No" then asp_Write("checked") end if %>  > <%tcWebApi_get("String_Entry","SambaDeactivatedText","s")%></TD>
                      </TR>
                    <TR >
                      <TD><%tcWebApi_get("String_Entry","SambaUsernameText","s")%></TD>
                      <TD>
                      	<INPUT id=smbusername maxLength=35 size=15 name=smbusername value="<% If TCWebApi_get("Samba_Entry0","username","h") <> "N/A" then TCWebApi_get("Samba_Entry0","username","s") end if%>" >
                      </TD>
                     </TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","SambaNewPasswordText","s")%></TD>
                      <TD>
                      	<INPUT id=smbpwd type=password maxLength=35 size=15 name=smbpwd value="<% If TCWebApi_get("Samba_Entry0","Passwd","h") <> "N/A" then TCWebApi_get("Samba_Entry0","Passwd","s") end if%>" >
                       </TD>
                     </TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","SambaConfirmPasswordText","s")%></TD>
                      <TD>
                      	<INPUT id=confirmSambasmbpwd type=password maxLength=35 size=15 name=confirmSambasmbpwd value="<% If TCWebApi_get("Samba_Entry0","Passwd","h") <> "N/A" then TCWebApi_get("Samba_Entry0","Passwd","s") end if%>" >
                       </TD>
                     </TR>
                     <TR >
                      <TD><%tcWebApi_get("String_Entry","SambaWorkGroupText","s")%></TD>
                      <TD>
                      	<INPUT id=WorkGroup maxLength=35 size=15 name=WorkGroup value="<% If TCWebApi_get("Samba_Entry0","WorkGroup","h") <> "N/A" then TCWebApi_get("Samba_Entry0","WorkGroup","s") end if%>" >
                      </TD>
                     </TR>
                     <TR >
                      <TD><%tcWebApi_get("String_Entry","SambaNetBIOSNameText","s")%></TD>
                      <TD>
                      	<INPUT id=NetBiosName maxLength=35 size=15 name=NetBiosName value="<% If TCWebApi_get("Samba_Entry0","NetBiosName","h") <> "N/A" then TCWebApi_get("Samba_Entry0","NetBiosName","s") end if%>" >
                      </TD>
                     </TR>
                   </TBODY></TABLE><BR></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD><DIV id=stResult style="FONT-SIZE: 13px"></DIV></TD>
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
            <P align=center><IMG id="btnOK" onclick="OnOK()" height="23"
            src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width="80" border="0">&nbsp;&nbsp;<IMG 
            id="btnCancel" onclick="RefreshPage()" height="23" src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" 
            width="80" border="0"> </P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR>
        </TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
