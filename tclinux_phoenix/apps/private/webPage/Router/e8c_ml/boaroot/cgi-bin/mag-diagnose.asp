<!--[if IE]>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<!--[else]>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<![endif]-->
<%
If Request_Form("btnflag") = "1" Then
	tcWebApi_constSet("Diagnostic_HW","status","1")
	TCWebApi_commit("Diagnostic_HW")
End If
%>
<HTML>
<HEAD>
<TITLE>维护</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT> 
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT>
var hwstatus = "<%TCWebApi_get("Diagnostic_HW","status","s")%>";

function doFault()
{
	setText("btnflag", "1");
	if ( true == setEBooValueCookie(document.diagnoseForm) )
	{
		$('#diagnoseForm').submit();
	}
}

function LoadFrame()
{
	if ( hwstatus == "1" )
	{
		$('#btnfault').attr('disabled', true);
		setTimeout(function() {
			$('#faultfrm').attr('src', $('#faultfrm').attr('src'));
			$('#btnfault').attr('disabled', false);
		}, 30000);
	}
	else
	{
		$('#faultfrm').css('display', 'none');
	}
}
</SCRIPT>
</HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 leftMargin=0 topMargin=0 	marginheight="0" marginwidth="0"
	onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('diagnoseForm') != null)LoadFrame()">
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
		<TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%></TD>
		<TD vAlign=bottom width=50>
			<A onclick=DoLogout() href="/cgi-bin/login.asp" id=logOut name=logOut target=_top>
			<SPAN class=logout>
			<%tcWebApi_get("String_Entry","LogoutText","s")%> 
			</SPAN>
			</A>
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
		<P align=right>
		<FONT face=黑体 color=#ffffff>
		<B>
		<FONT face=黑体 color=#ffffff size=6>
		<INPUT id=Selected_Menu type=hidden 
		value="<%tcWebApi_get("String_Entry","ContentManageText","s")%>-><%tcWebApi_get("String_Entry","ContentMaintainText","s")%>" 
		name=Selected_Menu>
		</FONT>
		</B>
		<SPAN class=GatewayName><%tcWebApi_get("String_Entry","GatewayNameText","s")%>
		<SCRIPT language=javascript>
		document.write(top.gateWayName);
		</SCRIPT>
		</SPAN>
		</FONT>
		</P>
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
	MakeMenu(getElById ('Selected_Menu').value);
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
	<TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
	<TBODY>
	<TR>
	<TD width=157 bgColor=#ef8218 height=30>
	<P class=Item_L1><%tcWebApi_get("String_Entry","ContentMaintainText","s")%></P></TD>
	<TD width=7 bgColor=#ef8218></TD>
	<TD width=474></TD>
	<TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
		<TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
		<TBODY>
		<TR>
		<TD valign='top'>
			<A href="/cgi-bin/help_content.asp#维护" target=_blank>
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
	<TD width=7 background=/img/panel3.gif></TD>
	<TD></TD>
	</TR>
	<TR>
	<TD vAlign=top width=157 bgColor=#e7e7e7 height=30><P class=Item_L2></P></TD>
	<TD width=7 background=/img/panel3.gif></TD>
	<TD>
		<FORM id=diagnoseForm name=diagnoseForm action="/cgi-bin/mag-diagnose.asp" method="post">
		<INPUT type="hidden" name="btnflag" value="0">
		<TABLE cellSpacing=10 cellPadding=0 width="100%" border=0>
		<TBODY>
		<TR>
		<TD width=10>&nbsp;</TD>
		<TD align=center>
		<P>
			<TABLE cellSpacing=3 cellPadding=3 border=0 style="display:none;">
			<TBODY>
			<TR>
			<TD>
			<P align=center>
				<A id=MainTainence name=MainTainence href="/cgi-bin/maintainreport.cgi"><%tcWebApi_get("String_Entry","ManageEndreportText","s")%></A>
				<BR>
			</P>
			</TD>
			</TR>
			</TBODY>
			</TABLE>
			<TABLE cellSpacing=3 cellPadding=3 border=0>
			<TBODY>
			<TR>
			<TD>
			<P align=center>
				<INPUT type="button" value='<%tcWebApi_get("String_Entry","ManagementHWdetectionText","s")%>' id="btnfault" onclick="doFault();">
				<BR>
				<iframe src="/cgi-bin/hwdetection.cgi" id="faultfrm" width="380" height="200" frameborder="no" border="0" marginwidth="0" marginheight="0" scrolling="no"></iframe>
			</P>
			</TD>
			</TR>
			</TBODY>
			</TABLE>
		</P>
		</TD>
		<TD width=10>&nbsp;</TD>
		</TR>
		<TR>
		<TD width=10>&nbsp;</TD>
		<TD></TD>
		<TD width=10>&nbsp;</TD>
		</TR>
		</TBODY>
		</TABLE>
		</FORM>
	</TD>
	</TR>
	<TR>
	<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
	<TD width=7 background=/img/panel3.gif></TD>
	<TD></TD>
	</TR>
	<TR>
	<TD vAlign=top width=157 bgColor=#e7e7e7></TD>
	<TD width=7 background=/img/panel3.gif></TD>
	<TD></TD>
	</TR>
	</TBODY>
	</TABLE>
</TD>
</TR>
<TR>
<TD height=1>
	<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 border=0>
	<TBODY>
	<TR>
	<TD width=162 bgColor=#ef8218></TD>
	<TD width=278 bgColor=#427594></TD>
	<TD width=196 bgColor=#427594>
		<P align=center>
		<!--IMG id=btnOK onclick="RefreshPage()" height=23 src="/img/ok.gif" width=80 border=0>&nbsp;&nbsp;
		<IMG id=btnCancel onclick="RefreshPage()" height=23 src="/img/cancel.gif" width=80 border=0-->
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
