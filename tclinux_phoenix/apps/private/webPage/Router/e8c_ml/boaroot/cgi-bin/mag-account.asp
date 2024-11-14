<%
if Request_Form("accountflg")="1" then
	if tcWebApi_get("WebCurSet_Entry","CurrentAccess","h") = "1" then
		if Request_Form("oldPassword") = tcWebApi_get("Account_Entry1","web_passwd","h") then
			TCWebApi_set("WebCurSet_Entry","CurPage","1")
			TCWebApi_set("Account_Entry1","web_passwd","cfmPassword")
			TCWebApi_commit("Account_Entry")
			TCWebApi_save()
		else
			TCWebApi_set("WebCurSet_Entry","CurPage","2")
		end if
	else
		TCWebApi_set("WebCurSet_Entry","CurPage","1")
		TCWebApi_set("Account_Entry1","web_passwd","cfmPassword")
		TCWebApi_commit("Account_Entry")
		TCWebApi_save()
	end If
else
	TCWebApi_set("WebCurSet_Entry","CurPage","0")
end If
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","UserManage1Text","s")%></TITLE>
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
                <TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%></TD>
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
            value="<%tcWebApi_get("String_Entry","ContentManageText","s")%>-><%tcWebApi_get("String_Entry","ContentUserManagementText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
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
var signal = "<% tcWebApi_get("WebCurSet_Entry","CurPage","s") %>";

function LoadFrame()
{	
		with (getElById('ConfigForm'))
		{
			accountflg.value = "0";
			if((0 != parseInt(signal)) && (signal != "N/A"))
			{
				var istate = parseInt(signal);
				if(1 == istate)
				{
					if (curUserName == sptUserName){
						var tdResultElement = getElement('stResult');
						tdResultElement.innerHTML = '<P><strong><font color="#0000FF" size="2"><%tcWebApi_get("String_Entry","UserManage2Text","s")%></font></strong></P>';
					}
					else{
						alert("<%tcWebApi_get("String_Entry","UserManage3Text","s")%>");
						window.location.href="/cgi-bin/logout.cgi";
					}
				}
				else{
					window.location.href="/cgi-bin/ErrMsg.asp";
				}
			}
		}
}
	
	function CheckForm()
	{
		if(document.ConfigForm.newPassword.value.length > 31)
		{
			alert("<%tcWebApi_get("String_Entry","UserManage4Text","s")%>");
			return false;
		}
		if (document.ConfigForm.newPassword.value == "")
		{
			alert("<%tcWebApi_get("String_Entry","UserManage5Text","s")%>");
			return false;
		}
		if (document.ConfigForm.cfmPassword.value != document.ConfigForm.newPassword.value)
		{
			alert("<%tcWebApi_get("String_Entry","UserManage6Text","s")%>");
			return false;
		}
		return true;
	}
	
	function OnOK()
	{
		if (CheckForm())
		{
			document.ConfigForm.accountflg.value = "1";
			if( true == setEBooValueCookie(document.ConfigForm) )
				document.ConfigForm.submit();
		}
	}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","UserManage1Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474></TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#用户管理" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0></A></TD></TR>
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
            <FORM name=ConfigForm id=ConfigForm action="/cgi-bin/mag-account.asp" method="post">
			<INPUT TYPE="HIDDEN" ID="name0" NAME="name0" VALUE="<%TCWebApi_get("Account_Entry0","username","s")%>">
			<INPUT TYPE="HIDDEN" ID="name1" NAME="name1" VALUE="<%TCWebApi_get("Account_Entry1","username","s")%>">
			<INPUT TYPE="HIDDEN" ID="name2" NAME="name2" VALUE="<%TCWebApi_get("Account_Entry2","username","s")%>">
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD><p>
						<strong>
							<font color="red" size="2">It is recommended to change your default password for this device for security and safety reasons.</font>
						</strong>
						</p>
					</TD>
				  </TR>
				  <TR height="30px">
					<TD width=10>&nbsp;</TD>
					<TD></TD>
				  </TR>
				  <TR>
					<TD width=10>&nbsp;</TD>
					<TD><p>
							<font size="2">1:The password must contain at least 8 characters.</font>
						</p>
					</TD>
				  </TR>
				  <TR>
					<TD width=10>&nbsp;</TD>
					<TD><p>
							<font size="2">2:The password must Input Max 31 characters.</font>
						</p>
					</TD>
				  </TR>
				  <TR>
					<TD width=10>&nbsp;</TD>
					<TD><p>
							<font size="2">3:The password must be alphanumeric or a combination of alphanumeric and symbol.</font>
						</p>
					</TD>
				  </TR>
				  <TR height="30px">
					<TD width=10>&nbsp;</TD>
					<TD></TD>
				  </TR>
              <TR>
                <TD width=10>&nbsp;</TD>

                <TD>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR id=secUsername>
                      <TD width=91 height="26"><%tcWebApi_get("String_Entry","UserManage7Text","s")%></TD>
                      <TD width=169>
					  <SELECT id=oldUsername style="WIDTH: 115px"  size=1 name=oldUsername> 
                        	<SCRIPT language=JavaScript type=text/javascript>
							/*if(document.ConfigForm.name0.value != "")
							{
								document.write('<option value="' + document.ConfigForm.name0.value + '" >' + document.ConfigForm.name0.value);
							}*/
							if(document.ConfigForm.name1.value != "")
							{
								document.write('<option value="' + document.ConfigForm.name1.value + '" selected>' + document.ConfigForm.name1.value);
							}
							/*if(document.ConfigForm.name2.value != "")
							{
								document.write('<option value="' + document.ConfigForm.name2.value + '" >' + document.ConfigForm.name2.value);
							}*/
							</SCRIPT>
                        </SELECT></TD></TR>
                    <TR id=trNewUsername name=trNewUsername >
                      <TD width=89><%tcWebApi_get("String_Entry","UserManage8Text","s")%></TD>
                      <TD width=169><LABEL><INPUT id=newUsername 
                        style="WIDTH: 115px" maxLength=35 size=15 
                        name=newUsername> </LABEL></TD></TR>
                    <TR id=TroldPassword name=TroldPassword >
                      <TD><%tcWebApi_get("String_Entry","UserManage9Text","s")%></TD>
                      <TD><INPUT id=oldPassword type=password maxLength=35 
                        size=15 name=oldPassword></TD></TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","UserManage10Text","s")%></TD>
                      <TD><INPUT id=newPassword type=password maxLength=35 
                        size=15 name=newPassword>
					<script language="JavaScript" type="text/JavaScript">
					setDisplay("trNewUsername", 0);
					if (curUserName == sptUserName)
					{
						setDisplay("TroldPassword", 0);
					}
					</script></TD></TR>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","UserManage11Text","s")%></TD>
                      <TD><INPUT id=cfmPassword type=password maxLength=35 
                        size=15 name=cfmPassword></TD></TR></TBODY></TABLE><BR></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD><DIV id=stResult style="FONT-SIZE: 13px"></DIV></TD>
				<INPUT TYPE="HIDDEN" ID="accountflg" NAME="accountflg" VALUE="0">
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
