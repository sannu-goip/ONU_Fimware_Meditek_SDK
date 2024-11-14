<%
if Request_Form("Acl_Flag") = "1" then
	TCWebApi_set("ACL_Common","Action","Actionflag")
	TCWebApi_set("ACL_Common","DeleteIndex","delnum")
	TCWebApi_commit("ACL_Entry")
	TCWebApi_save()
elseif Request_Form("Acl_Flag") ="2" then
	TCWebApi_set("ACL_Common","Action","Actionflag")
	TCWebApi_set("ACL_Common","Activate","EnACLFilter_Flag")
	TCWebApi_commit("ACL_Entry")
	TCWebApi_save()
end if	
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","ACLFiltertTitleText","s")%></TITLE>
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
            color=#ffffff size=6><INPUT id=Selected_Menu type=hidden value="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>-><%tcWebApi_get("String_Entry","ContentAclFilterText","s")%>" name="Selected_Menu"> </FONT></B><SPAN 
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
      <SCRIPT language=JavaScript src="/JS/printbox.js"></SCRIPT>
	  
      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","ACLFiltertTitleText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
          rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#ACL过滤" 
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
            <FORM name="ConfigForm" action="/cgi-bin/sec-aclfilter.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
	        	<TD><P><font color=red><%tcWebApi_get("String_Entry","WanSetting1IPV6","s")%></font></P><BR></TD>
	          </TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <P><%tcWebApi_get("String_Entry","ACLFilterEnableText","s")%> <INPUT id=isFilter onclick="selFilter(this)" type="checkbox" name="isFilter" <%if tcWebApi_get("ACL_Common","Activate","h") = "Yes" then asp_Write("checked") end if%>></P>
                          <input type="hidden" id="Acl_Flag" name="Acl_Flag" value="0">
                          <input type="hidden" id="delnum" name="delnum" value="">
                          <input type="hidden" id="editnum" name="editnum" value="">
                          <input type="hidden" id="EnACLFilter_Flag" name="EnACLFilter_Flag" value="<% tcWebApi_get("ACL_Common","Activate","s") %>">
                          <input type="hidden" id="acl_num" name="acl_num" value="<% tcWebApi_get("ACL_Common","acl_num","s") %>">
                          <input type="hidden" id="Actionflag" name="Actionflag" value="Del">
                          <script language="JavaScript" type="text/JavaScript">
                          
var enableFilter = "<% tcWebApi_get("ACL_Common","Activate","s") %>";

function addClick()
{
	if(parseInt(document.ConfigForm.acl_num.value) >= 16)
	{
		alert('<%tcWebApi_get("String_Entry","ACLFilterMaxEntryText","s")%>')
		return;
	}
	location.replace('sec-addaclfilter.asp');
}
function removeClick(rml)
{
	document.ConfigForm.Actionflag.value = "Del";
	document.ConfigForm.Acl_Flag.value = "1";
	if(true == setEBooValueCookie(document.ConfigForm))
		document.ConfigForm.submit();
}

function LoadFrame()
{
	document.ConfigForm.delnum.value = "";
	document.ConfigForm.Acl_Flag.value = "0";
	var FilterInfo = getElById("FilterInfo");
	if (enableFilter == "Yes")
	{
		var FilterOption = getElById("isFilter");
		FilterOption.checked = true;
	}
}

function selFilter(filter)
{
	if (filter.checked)
	{
		getElement('FilterInfo').style.display = "";
		if (enableFilter == "No")
		{
			with (getElById('ConfigForm'))
			{
				btnAdd.disabled = true;
				btnRemove.disabled = true;
			}
		}
	}
	else
	{
		FilterInfo.style.display = "none";
	}
}

function SubmitForm()
{
	var Form = document.ConfigForm;
	Form.Actionflag.value = "Add";
	Form.Acl_Flag.value = "2";
	if(Form.isFilter.checked)
		Form.EnACLFilter_Flag.value = "Yes";
	else Form.EnACLFilter_Flag.value = "No";

	if(true == setEBooValueCookie(Form))
		Form.submit();
}

function stAclFilter(EntryNum,Name,ScrBeginIP,ScrEndIP,Interface,Application, Enable)
{
	this.EntryNum = EntryNum;
	this.Name = Name;
	this.ScrBeginIP = ScrBeginIP;
	this.ScrEndIP = ScrEndIP;
	this.Interface = Interface;
	this.Application = Application;
	this.Enable = Enable;
}

var acl_list;
function doEdit(i)
{
	location.replace('sec-addaclfilter.asp?AclEditNumber=' + i);
}

function doDel(i)
{
	var f = document.ConfigForm;
	var tempstr;
	var rml = document.getElementsByName('rml');
	if (rml == null)
		return;
	if(rml.length>0)
	{
		for(var n=0;n<(rml.length);n++)
		{
			if(rml[n].value == i)
			{
				if(rml[n].checked)
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
                          <BR>
                  <DIV id=FilterInfo>
                  <SCRIPT language=JavaScript type=text/javascript>
if (enableFilter == "No")
{
getElById("FilterInfo").style.display = "none";
}
</SCRIPT>
                  <BR><BR>
				  <% if tcWebApi_get("ACL_Common","Activate","h") = "Yes" then %>
				  <TABLE cellSpacing=0 cellPadding=3 width="100%" border=1 class='table_data'>
				    <TBODY>
						<TR align="center">
							<TD class="table_title" width="35%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterNameTitleText","s")%></STRONG></TD>
							<TD class="table_title" width="30%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterScrBeginIPTitleText","s")%></STRONG></TD>
							<TD class="table_title" width="20%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterScrEndIPTitleText","s")%></STRONG></TD>
							<TD class="table_title" width="20%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterInterfaceTitleText","s")%></STRONG></TD>
							<TD class="table_title" width="20%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterAppTitleText","s")%></STRONG></TD>
							<TD class="table_title" width="20%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterEnableText","s")%></STRONG></TD>
							<TD class="table_title" width="15%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterEditText","s")%></STRONG></TD>
							<TD class="table_title" width="20%" align=middle ><STRONG><%tcWebApi_get("String_Entry","ACLFilterDeleteTitleText","s")%></STRONG></TD>
						</TR>
	                  	<SCRIPT language=JavaScript type=text/javascript>
	                  		var aclvalue = '<% tcWebApi_JsonHook("get_aclfilter_info", "action", "area") %>';
							var rsp = JSON.parse(aclvalue);
							acl_list = rsp.data;
							for(var i = 0; i < acl_list.length; i++)
							{
								document.write('<TR align="center">');
								document.write('<TD align="center" width="35%">' + acl_list[i]['ACLName'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="30%">' + acl_list[i]['ScrIPAddrBegin'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="30%">' + acl_list[i]['ScrIPAddrEnd'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="20%">' + acl_list[i]['Interface'] + '&nbsp;</TD>');
								document.write('<TD align="center" width="20%">' + acl_list[i]['Application'] + '&nbsp;</TD>');
								if(acl_list[i]['Active'] == "Yes")
									document.write('<TD align="center" width="20%">' + '<%tcWebApi_get("String_Entry","ACLFilterEnableStatusText","s")%>' + '&nbsp;</TD>');
								else
									document.write('<TD align="center" width="20%">' + '<%tcWebApi_get("String_Entry","ACLFilterDisableStatusText","s")%>' + '&nbsp;</TD>');
								document.write('<TD width="15%"><input type="button" id="edit" name="edit" onclick="doEdit(' + acl_list[i]['Index'] + ')" value="<%tcWebApi_get("String_Entry","ACLFilterEditText","s")%>"></TD>');
								document.write('<TD width="20%"><input type="checkbox" id="rml" name="rml" onclick="doDel(' + acl_list[i]['Index'] + ')" value="' + acl_list[i]['Index'] + '"></TD>');
								document.write('</TR>');	
							}
	                  	</SCRIPT>
					</TBODY> 
					</TABLE>
                  <% end if %>
                  <P></P><BR><INPUT id=btnAdd name=btnAdd onclick="addClick()" type=button value=<%tcWebApi_get("String_Entry","ACLFilterAddText","s")%>> 
<INPUT id=btnRemove name=btnRemove onclick="removeClick(this.form.rml)" type=button value=<%tcWebApi_get("String_Entry","ACLFilterDeleteText","s")%>> 
                  </DIV></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD></TD>
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
            <P align=center><IMG id=btnOK onclick=SubmitForm() height=23 
            src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width=80 border=0>&nbsp;&nbsp;<IMG 
            id=btnCancel onclick="RefreshPage()" height=23 src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" 
            width=80 border=0> </P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
