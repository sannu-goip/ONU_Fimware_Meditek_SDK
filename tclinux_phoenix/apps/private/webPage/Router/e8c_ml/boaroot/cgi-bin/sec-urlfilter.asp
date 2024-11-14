<%
if Request_Form("Save_Flag") = "1" then
	TCWebApi_set("UrlFilter_Common","Action","Actionflag")
	TCWebApi_set("WebCurSet_Entry","url_filter_id","add_num")
	TCWebApi_set("UrlFilter_Entry","Activate","EnableUrlFilterFlag")
	TCWebApi_set("UrlFilter_Entry","URL","urlitem")
	
	TCWebApi_commit("UrlFilter")
	TCWebApi_save()
elseif Request_Form("Save_Flag") ="2" then
	TCWebApi_set("UrlFilter_Common","Action","Actionflag")
	TCWebApi_set("UrlFilter_Common","Activate","EnableUrlFilterFlag")
	TCWebApi_commit("UrlFilter")
	TCWebApi_save()
elseif Request_Form("Save_Flag") = "3" then
	TCWebApi_set("UrlFilter_Common","Action","Actionflag")
	TCWebApi_set("UrlFilter_Common","DeleteIndex","delnum")
	TCWebApi_commit("UrlFilter")
	TCWebApi_save()
elseif Request_Form("Save_Flag") = "4" then
	TCWebApi_set("UrlFilter_Common","Action","Actionflag")
	TCWebApi_set("UrlFilter_Common","Filter_Policy","FilterPolicy")
	TCWebApi_commit("UrlFilter")
	TCWebApi_save()
end if
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","WanSetting1Text","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/json2.js"></SCRIPT>
<META content="MSHTML 6.00.6000.16809" name=GENERATOR>


</HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('UrlFilterForm') != null)LoadFrame()" 
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
                  href="/cgi-bin/login.asp" id="logOut" name="logOut" target=_top><SPAN 
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
            value="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>-><%tcWebApi_get("String_Entry","ContentWANSetText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
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
      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","WanSetting1Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
          rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#广域网访问设置" 
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
            <FORM name="UrlFilterForm" action="/cgi-bin/sec-urlfilter.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
	        	<TD><P><font color=red><%tcWebApi_get("String_Entry","WanSetting1IPV6","s")%></font></P></TD>
	          </TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                        
                        <TD> <P> 
                            <input type="hidden" id="Save_Flag" name="Save_Flag" value="0">
                            <input type="hidden" id="Actionflag" name="Actionflag" value="Del">
                            <input type="hidden" id="EnableUrlFilterFlag" name="EnableUrlFilterFlag" value="<% tcWebApi_get("UrlFilter_Common","Activate","s") %>">
                            <input type="hidden" id="delnum" name="delnum" value="">
                            <input type="hidden" id="add_num" name="add_num" value="<% tcWebApi_get("UrlFilter","add_num","s") %>">
                            <input type="hidden" id="Url_num" name="Url_num" value="<% tcWebApi_get("UrlFilter","Url_num","s") %>">
                            <script language="JavaScript" type="text/JavaScript">
var urlfltpolicy = "<% tcWebApi_get("UrlFilter_Common","Filter_Policy","s") %>";
var enbl = "<% tcWebApi_get("UrlFilter_Common","Activate","s") %>";
var urllistIndex = 0;
var urllist = new Array();
var state = enbl;
function isValidUrlName(url)
{
var i=0;
var invalidArray = new Array();
invalidArray[i++] = "www";
invalidArray[i++] = "com";
invalidArray[i++] = "org";
invalidArray[i++] = "net";
invalidArray[i++] = "edu";
invalidArray[i++] = "www.";
invalidArray[i++] = ".com";
invalidArray[i++] = ".org";
invalidArray[i++] = ".net";
invalidArray[i++] = ".edu";
if (isValidAscii(url) != '')
{
return false;
}

if (isValidName(url) == false)	
{
	return false;
}

for (i = 0; i < url.length; i++)
{
if (url.charAt(i) == '\\')
{
return false;
}
}
if (url == "")
{
return false;
}
if (url.length < 3)
{
return false;
}
for(j=0; j< invalidArray.length; j++)
{
if (url == invalidArray[j])
{
return false;
}
}
return true;
}
function btnAdd()
{
  	var urlvalue = '<% tcWebApi_JsonHook("get_urlfilter_info", "action", "area") %>'
	var rsp = JSON.parse(urlvalue);
	var url_list = rsp.data;
//var SubmitForm = new webSubmitForm();
	with ( document.forms[0] )
	{
		if(Url_num.value >= 100)
		{
			alert("<%tcWebApi_get("String_Entry","WanSetting2Text","s")%>")
			return;
		}
		if (urlitem.value.length > 31)
		{
			alert("<%tcWebApi_get("String_Entry","WanSetting3Text","s")%>")
			return;
		}
		if(isValidUrlName(urlitem.value) == false)
		{
			alert("<%tcWebApi_get("String_Entry","WanSetting4Text","s")%>")
			return;
		}
		var str = urlitem.value;
		var i;
		if (-1 != (i = str.indexOf("http")))
		{
			if(-1 != str.indexOf("https"))
				str = str.substring(i+8);
			else str = str.substring(i+7);
		}
		urlitem.value = str;
		for(k = 0; k < url_list.length; k++)
		{
			if(url_list[k]['URL'] == str)
			{
				alert("<%tcWebApi_get("String_Entry","WanSetting5Text","s")%>");
				return;
			}
		}
//SubmitForm.addParameter('x.Url',str);
		
	}
//SubmitForm.setAction('addcfg.cgi?x=InternetGatewayDevice.X_ATP_Security.UrlFilter&'
//+ 'RequestFile=html/security/urlfilter.asp');
//SubmitForm.submit();
	var	vForm = document.forms[0];
	vForm.Actionflag.value = "Add";
	vForm.Save_Flag.value = "1";
	if( true == setEBooValueCookie(vForm) )
		vForm.submit();

}
function btnRemove(rml)
{
	/*if (rml == null)
	{
		alert("rml is null")
		return;
	}
	//var SubmitForm = new webSubmitForm();
	with (document.forms[0])
	{
		if (rml.length > 0)
		{
			var i;
			for (i = 0; i < rml.length; i++)
			{
				if (rml[i].checked == true)
				{
					//SubmitForm.addParameter(rml[i].value,'');
					curUrlfilter.value = rml[i].value - 1;
				}
			}
		}
		else if (rml.checked == true)
		{
			//SubmitForm.addParameter(rml.value,'');
			curUrlfilter.value = rml[i].value - 1;
		}
	}*/
	//SubmitForm.setAction('del.cgi?RequestFile=html/security/urlfilter.asp');
	//SubmitForm.submit();
	
	document.UrlFilterForm.Actionflag.value = "Del";
	document.UrlFilterForm.Save_Flag.value = "3";
	if( true == setEBooValueCookie(document.UrlFilterForm) )
		document.UrlFilterForm.submit();
}
function LoadFrame()
{
	var vForm = document.UrlFilterForm;
	vForm.Save_Flag.value = "0";
	vForm.delnum.value = "";
	setCheck('enableFilter',enbl);
//setRadio('FilterPolicy',urlfltpolicy);
}
function SubmitForm()
{
/*var SubmitForm = new webSubmitForm();
if (state == "0" && getCheckVal("enableFilter") == 1)
{
SubmitForm.addParameter('InternetGatewayDevice.X_ATP_Security.X_ATP_UrlFilterEnabled',1);
}
if (state == "1" && getCheckVal("enableFilter") == 0)
{
SubmitForm.addParameter('InternetGatewayDevice.X_ATP_Security.X_ATP_UrlFilterEnabled',0);
}
SubmitForm.setAction('set.cgi?RequestFile=html/security/urlfilter.asp');
SubmitForm.submit();*/
	var vForm = document.UrlFilterForm;
	vForm.Save_Flag.value = "2";
	document.UrlFilterForm.Actionflag.value = "Status";
	if(vForm.enableFilter.checked)
		vForm.EnableUrlFilterFlag.value = "1";
	else vForm.EnableUrlFilterFlag.value = "0";
	if( true == setEBooValueCookie(vForm) )
		vForm.submit();
	
}
function cliEnableBox(checkBox)
{
	if (checkBox.checked == true)
	{
		if (document.UrlFilterForm.EnableUrlFilterFlag.value == "1")
			setDisplay("Filter",1);
	}
	else
	{
		setDisplay("Filter",0);
	}
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
function doDel(i)
{
	var f = document.UrlFilterForm;
	var tempstr;
	var rml = document.getElementsByName('rml');
	if (rml == null)
		return;
	if(rml.length>0)
	{
		for(var n=0;n<(rml.length);n++)
		{
			if(rml[n].value == i){
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
function ChangePolicy()
{
	if (urlfltpolicy == getRadioVal('FilterPolicy'))
		return;
	//var Form = new webSubmitForm();
	var FilterMode = getElById("FilterPolicy");
	if (FilterMode[0].checked == true)
	{
		if (confirm("<%tcWebApi_get("String_Entry","WanSetting6Text","s")%>"))
		{
			//state = false;
			//Form.addParameter('InternetGatewayDevice.X_ATP_Security.X_ATP_UrlFilterPolicy',0);
				document.UrlFilterForm.Actionflag.value = "Filter";
				document.UrlFilterForm.Save_Flag.value = "4";
				if( true == setEBooValueCookie(document.UrlFilterForm) )
					document.UrlFilterForm.submit();
		}
		else
		{
			FilterMode[0].checked = false;
			FilterMode[1].checked = true;
			return;
		}
	}
	else if (FilterMode[1].checked == true )
	{
		if (confirm("<%tcWebApi_get("String_Entry","WanSetting7Text","s")%>"))
		{
			//state = true;
			//Form.addParameter('InternetGatewayDevice.X_ATP_Security.X_ATP_UrlFilterPolicy',1);
				document.UrlFilterForm.Actionflag.value = "Filter";
				document.UrlFilterForm.Save_Flag.value = "4";
				if( true == setEBooValueCookie(document.UrlFilterForm) )
					document.UrlFilterForm.submit();
		}
		else
		{
			FilterMode[0].checked = true;
			FilterMode[1].checked = false;
			return;
		}
	}
	//Form.setAction('set.cgi?RequestFile=html/security/urlfilter.asp');
	//Form.submit();
}

							</script>
                            <BR>
                          <TABLE>
                    <TBODY>
                    <TR>
                      <TD><%tcWebApi_get("String_Entry","WanSetting8Text","s")%></TD>
                      <TD>&nbsp;&nbsp; <INPUT id=enableFilter onclick="cliEnableBox(this)" type=checkbox name="enableFilter" <%if tcWebApi_get("UrlFilter_Common","Activate","h") = "1" then asp_Write("checked") end if%>> </TD></TR></TBODY></TABLE>
                  <DIV id=Filter><%tcWebApi_get("String_Entry","WanSetting9Text","s")%><BR><%tcWebApi_get("String_Entry","WanSetting10Text","s")%><BR><%tcWebApi_get("String_Entry","WanSetting11Text","s")%><BR>
                            <%tcWebApi_get("String_Entry","WanSetting12Text","s")%><BR>
                  <SCRIPT language=JavaScript type=text/javascript>
				  setDisplay('Filter',enbl);
if(urlfltpolicy == "0")
{
document.write('<%tcWebApi_get("String_Entry","WanSetting13Text","s")%><B><%tcWebApi_get("String_Entry","WanSetting14Text","s")%></B>\n');
}
else
{
document.write('<%tcWebApi_get("String_Entry","WanSetting13Text","s")%><B><%tcWebApi_get("String_Entry","WanSetting16Text","s")%></B><BR>\n');
document.write('<font color="#FF0000"><%tcWebApi_get("String_Entry","WanSetting17Text","s")%></font>');
}
</SCRIPT>
                  <BR><BR><%tcWebApi_get("String_Entry","WanSetting18Text","s")%><INPUT id=FilterPolicy onclick="ChangePolicy()" type=radio value="0" name="FilterPolicy" <%if tcWebApi_get("UrlFilter_Common","Filter_Policy","h") = "0" then asp_Write("checked")  end if%>><%tcWebApi_get("String_Entry","WanSetting14Text","s")%> 
				  <INPUT id=FilterPolicy onclick="ChangePolicy()" type=radio value="1" name="FilterPolicy" <%if tcWebApi_get("UrlFilter_Common","Filter_Policy","h") = "1" then asp_Write("checked") end if%>><%tcWebApi_get("String_Entry","WanSetting16Text","s")%> 
				  <BR><BR><BR>
                  <DIV id=dnsdomain>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD width=60>URL:</TD>
                      <TD width=100>
					  <INPUT onkeydown="if(event.keyCode==13)return false" maxLength=31 size=30 name="urlitem">
					  </TD>
                      <TD align=middle width=70><INPUT onclick="btnAdd()" id="buttonAdd" name="buttonAdd" type=button value=" <%tcWebApi_get("String_Entry","WanSetting19Text","s")%> "> 
                      </TD></TR></TBODY></TABLE></DIV><BR><BR>
					<% if tcWebApi_get("UrlFilter_Common","Activate","h") = "1" then %>
                  <DIV id=ddnsInfo>
					<TABLE cellSpacing=0 cellPadding=3 width="100%" border=1 class='table_data'>
				    <TBODY>
						<TR>
						<TD class="table_title" align=middle width=60><STRONG><%tcWebApi_get("String_Entry","WanSetting21Text","s")%></STRONG></TD>
						<TD class="table_title" align=middle width=180><strong><FONT>URL</FONT></strong></TD>
						<TD class="table_title" align=middle width=60><STRONG><FONT><%tcWebApi_get("String_Entry","WanSetting22Text","s")%></FONT></STRONG></TD>
						</TR>
		                  	<SCRIPT language=JavaScript type=text/javascript>
		                  		var urlvalue = '<% tcWebApi_JsonHook("get_urlfilter_info", "action", "area") %>'
								var rsp = JSON.parse(urlvalue);
								var url_list = rsp.data;
								for(var i = 0; i < url_list.length; i++)
								{
									var webIndex = parseInt(url_list[i]['Index']) + 1;
									document.write('<TR align="center">');
									document.write('<TD align="center" width=60>' + webIndex + '&nbsp;</TD>');
									document.write('<TD align="center" width=180>' + url_list[i]['URL'] + '&nbsp;</TD>');
									document.write('<TD width=60><input type="checkbox" id="rml" name="rml" onclick="doDel(' + url_list[i]['Index'] + ')" value="' + url_list[i]['Index'] + '"></TD>');
									document.write('</TR>');	
								}
		                  	</SCRIPT>
					</TBODY> 
					</TABLE>   
                  </DIV>
					<% end if %>
					<BR><INPUT onclick='btnRemove(this.form.rml)' id="buttonRemove" name="buttonRemove" type=button value=" <%tcWebApi_get("String_Entry","WanSetting20Text","s")%> "> 
                  </DIV>
                  </TD>
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
          <TD width=7 background="/img/panel3.gif">　</TD>
        <TD></TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594>
            <P align=center><IMG id=btnOK onclick="SubmitForm()" height=23 
            src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width=80 border=0>&nbsp;&nbsp;<IMG 
            id=btnCancel onclick="RefreshPage()" height=23 src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" 
            width=80 border=0> </P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
