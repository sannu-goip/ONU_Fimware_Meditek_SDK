<%
if Request_form("delnum") <> "" then
		tcWebApi_set("WebCurSet_Entry","ipsec_id","delnum")
		tcWebApi_unset("IPSEC_Entry")
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
function totalConnCheck()
{
	if(document.IpsecTable_form.TotalConnNum.value >= 8)
	{
		alert("Users can only configure 8 IPSEC connections!\n");
		return false;
	}
	
	window.location='/cgi-bin/app-ipsecSet.asp?add_num='+document.IpsecTable_form.add_num.value
	return true;
}

function doDelete(i)
{
 	document.IpsecTable_form.delnum.value=i;
	if( true == setEBooValueCookie(document.IpsecTable_form) )
		document.IpsecTable_form.submit();
}

function doedit(i)
{
 	document.IpsecTable_form.action="/cgi-bin/app-ipsecSet.asp";
	document.IpsecTable_form.editnum.value=i;
	if( true == setEBooValueCookie(document.IpsecTable_form) )
		document.IpsecTable_form.submit();
}
</SCRIPT>
</HEAD>

<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 leftMargin=0 topMargin=0 
	onload="DisplayLocation(getElement('Selected_Menu').value);" marginheight="0" marginwidth="0">

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
                <TD class=welcom vAlign=bottom align=middle width=120>Welcome!  </TD>
                <TD vAlign=bottom width=50>
                  <A onclick=DoLogout() href="/cgi-bin/logout.cgi" id=logOut name=logOut target=_top>
                  <SPAN class=logout>Log out</SPAN></A></TD>
              </TR></TBODY></TABLE></TD>
 		</TR>
 		</TBODY>
 	  </TABLE>
 		
      <TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
        <TBODY>
        <TR>
          <TD class=LocationDisplay id=LocationDisplay align=middle width=163 bgColor=#ef8218 rowSpan=3>APP</TD>
          <TD width=434 bgColor=#427594 height=33>
            <P align=right>
            <FONT face=黑体 color=#ffffff>
            <B><FONT face=黑体 color=#ffffff size=6><INPUT id=Selected_Menu type=hidden 
              value="APP->VPN" name=Selected_Menu></FONT></B>
              <SPAN class=GatewayName>Gateway Name:
              <SCRIPT language=javascript>
				document.write(top.gateWayName);
			  </SCRIPT></SPAN>
            </FONT></P></TD>
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
          <TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24>
          </TD></TR>
        </TBODY>
      </TABLE>
      
      <SCRIPT language=javascript>
		MakeMenu(getElById ('Selected_Menu').value);
	  </SCRIPT>

      <TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 border=0>
        <TBODY>
        <TR>
          <TD height=15><IMG height=15 src="/img/panel1.gif" width=164 border=0> </TD>
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
            <P class=Item_L1><%tcWebApi_get("String_Entry","ipsecListText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'>
                  <A href="/cgi-bin/help_content.asp#IPSEC" 
                    target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                    width=40 border=0></A>
                </TD></TR>
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
<FORM METHOD="POST" ACTION="/cgi-bin/app-ipsecList.asp" name="IpsecTable_form">
	<INPUT TYPE="HIDDEN" NAME="delnum">
	<INPUT TYPE="HIDDEN" NAME="editnum">
	<INPUT TYPE="HIDDEN" NAME="TotalConnNum" value="<%tcWebApi_get("IPSEC","Total_Conn_Num","s")%>">
	<INPUT TYPE="HIDDEN" NAME="add_num" value="<%tcWebApi_get("IPSEC","add_num","s")%>">

<DIV>
<table width="100%" border="1" align=center cellpadding="3" cellspacing="0" bordercolor="#CCCCCC"bgcolor="#FFFFFF">
  <TBODY>
  <tr>
	<td width="4%" align=center class="table_title"><strong>#</strong></TD>
	<td width="20%" align=center class="table_title"><strong><%tcWebApi_get("String_Entry","ipsecConnectionNameText","s")%></strong></TD>
	<td width="20%" align=center class="table_title"><strong><%tcWebApi_get("String_Entry","ipsecRemoteGatewayText","s")%></strong></td>
	<td width="20%" align=center class="table_title"><strong><%tcWebApi_get("String_Entry","ipsecLocalAddressText","s")%></strong></TD>
	<td width="20%" align=center class="table_title"><strong><%tcWebApi_get("String_Entry","ipsecRemoteAddressText","s")%></strong></td>
	<td width="8%" align=center class="table_title"><strong><%tcWebApi_get("String_Entry","ipsecEditText","s")%></strong></td>
	<td width="8%" align=center class="table_title"><strong><%tcWebApi_get("String_Entry","ipsecDropText","s")%></strong></td>
  </tr>
<%if tcWebApi_get("IPSEC","entry0_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>0</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry0","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry0","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry0","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry0","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(0)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(0)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry1_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>1</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry1","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry1","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry1","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry1","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(1)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(1)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry2_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>2</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry2","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry2","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry2","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry2","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(2)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(2)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry3_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>3</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry3","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry3","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry3","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry3","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(3)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(3)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry4_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>4</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry4","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry4","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry4","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry4","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(4)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(4)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry5_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>5</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry5","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry5","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry5","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry5","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(5)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(5)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry6_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>6</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry6","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry6","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry6","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry6","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(6)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(6)></td>
  </tr>
<%end if%>
<%if tcWebApi_get("IPSEC","entry7_en","h") <> "N/A" then%>
  <tr>
	<td width="4%" align=center class=tabdata>7</td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry7","Connection_Name","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry7","Gateway_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry7","Local_IP","s") %></td>
	<td width="20%" align=center class=tabdata><%tcWebApi_staticGet("IPSEC_Entry7","Remote_IP","s") %></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/pen.gif onmouseover=this.style.cursor='hand' onClick=doedit(7)></td>
	<td width="8%" align=center class=tabdata><IMG src=/img/cross.gif onmouseover=this.style.cursor='hand' onClick=doDelete(7)></td>
  </tr>
<%end if%>

  </TBODY>
</table>
<!--
<BR><BR>
<LEFT><input name="Add New Connection" type="button" value="<%tcWebApi_get("String_Entry","ipsecAddNewConnectionText","s")%>" onClick="totalConnCheck()"></LEFT>
-->
</div>
</form>
<!-- IPSEC form end -->
				  
                </TD>
                <TD width=10>&nbsp;</TD>
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
		  		
		  </TD>
          <TD width=170 bgColor=#313031>　</TD>
        </TR>
        </TBODY>
      </TABLE>
    </TD>
  </TR>

  </TBODY>
</TABLE>
</BODY>
</HTML>
