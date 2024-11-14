<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0038)http://192.168.1.1/html/status/acs.asp -->
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","CwmpStatusRMStText","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>

<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR>
<style>
	TABLE {
		FONT-SIZE: 12px;
		FONT-FAMILY: "system-ui";
	 }
</style>

</HEAD>
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
            value="<%tcWebApi_get("String_Entry","ContentStatusText","s")%>-><%tcWebApi_get("String_Entry","ContentCWMPStatusText","s")%>" name=Sta_ACS> </FONT></B><SPAN 
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
      <SCRIPT language=JavaScript src="/JS/printtable.js"></SCRIPT>

      <SCRIPT language=JavaScript type=text/javascript>

var resultinfo = "<%TCWebApi_get("Diagnostic_PVC","ReportResult","s")%>";
var autoreportinfo = "<%TCWebApi_get("Diagnostic_PVC","AutoReport","s")%>";
var itmsconn = "<%TCWebApi_get("Diagnostic_PVC","ItmsConn","s")%>";
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD class=Item_L1 width=157 bgColor=#ef8218 height=30>
            <P><%tcWebApi_get("String_Entry","CwmpStatusIBuText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=7>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#远程管理状态" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 
                  border=0></A></TD></TR>
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
            <P class=Item_L2>&nbsp;</P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE id=report_info name=report_info cellSpacing=0 cellPadding=3 width=434 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title width=171><%tcWebApi_get("String_Entry","CwmpStatusPRISText","s")%></TD>
                      <TD width=245>
                        <SCRIPT language=javascript>
						var result = "<%TCWebApi_get("Cwmp_Entry","Inform_Status","s")%>";
						switch(result)
						{
							case "0":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusUnReText","s")%>");
								break;
							case "1":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusUnReNOText","s")%>");
								break;
							case "2":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusUnReMText","s")%>");
								break;
							case "3":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusUnNoMText","s")%>");
								break;
							case "4":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusUnNoACSText","s")%>");
								break;
							case "5":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusUnACSDoText","s")%>");
								break;
							case "6":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusReNoReText","s")%>");
								break;
							case "7":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusRePInText","s")%>"); 
								break;
							case "8":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusReSuccText","s")%>");
								break;
							 default:
								document.writeln("N/A");
								break;			
						}						
						</SCRIPT>
                      </TD></TR></TBODY></TABLE><BR><BR>
                  <TABLE id=itms_info name=itms_info cellSpacing=0 cellPadding=3 width=434 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title width=172><%tcWebApi_get("String_Entry","CwmpStatusAcITMSText","s")%></TD>
                      <TD width=244>
                        <SCRIPT language=javascript>
						var result = "<%TCWebApi_get("Cwmp_Entry","AcsConnStatus","s")%>";
						switch(result)
						{
							case "0":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusNRRCText","s")%>");
								break;
							case "1":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusThRCText","s")%>");
								break;
							case "2":
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusTRConText","s")%>");
								break;
							 default:
								document.writeln("<%tcWebApi_get("String_Entry","CwmpStatusNRRCText","s")%>");
								break;			
						}						
						</SCRIPT>
                      </TD></TR></TBODY></TABLE></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD class=Item_L1 width=157 bgColor=#ef8218 height=30>
            <P><%tcWebApi_get("String_Entry","CwmpStatusBCDSText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2>&nbsp;</P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE id=conf_info name=conf_info cellSpacing=0 cellPadding=3 width=434 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title width=172><%tcWebApi_get("String_Entry","CwmpStatusBuCDText","s")%></TD>
                      <TD width=244>
                        <SCRIPT language=javascript>
						var rstatus = "<%tcWebApi_get("deviceAccount_Entry","registerStatus","s") %>";
						var rresult = "<%tcWebApi_get("deviceAccount_Entry","registerResult","s") %>";
							if(rstatus=="0" && rresult == "99")
							{
								document.write('<%tcWebApi_get("String_Entry","CwmpStatusNoBcoText","s")%>');
							}
							else if(rstatus=="0" && rresult == "0")
							{
								document.write('<%tcWebApi_get("String_Entry","CwmpStatusAcReText","s")%>');
							}
							else if(rstatus=="0" && rresult == "1")
							{
								document.write('<%tcWebApi_get("String_Entry","CwmpStatusBCSuText","s")%>');
							}
							else if(rstatus=="5" && rresult == "0")
							{
								document.write('<%tcWebApi_get("String_Entry","CwmpStatusBCSuText","s")%>');
							}
							else if(rstatus=="0" && rresult == "2")
							{
								document.write('<%tcWebApi_get("String_Entry","CwmpStatusBCFaText","s")%>');
							}
							else
							{
								document.write('<%tcWebApi_get("String_Entry","CwmpStatusNoBcoText","s")%>');
							}
						</SCRIPT>
                      </TD></TR></TBODY></TABLE></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>&nbsp;</TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR></TBODY></TABLE>
      <SCRIPT language=JavaScript type=text/javascript>
var curUserType = '0';  
if (curUserType == '1')
{
tabOption.style.display = "none";
secDslStat.style.display = "none";
}
</SCRIPT>
    </TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594>
            <P align=center>&nbsp;</P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
