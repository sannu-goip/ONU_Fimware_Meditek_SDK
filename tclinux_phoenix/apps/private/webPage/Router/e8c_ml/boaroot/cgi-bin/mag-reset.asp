<%
if Request_Form("rebootflag") = "1" then
	TCWebApi_set("System_Entry","reboot_type","restoreFlag") 
	TCWebApi_set("Account_Entry0","Logged","login_init")
	TCWebApi_commit("System_Entry")
end if
if Request_Form("defaultflag") = "1" then
	TCWebApi_set("System_Entry","reboot_type","restoreflag2") 
	TCWebApi_set("Account_Entry0","Logged","login_init")
	TCWebApi_commit("System_Entry")
end if	
%>
<%
	if request_Form("backupflg") = "1" then
		tcWebApi_set("usbRestore_Entry", "dev", "ClDevType")
		tcWebApi_set("usbRestore_Entry", "autorestore", "autorestoreflag")
		tcWebApi_set("WebCurSet_Entry", "opt", "opt")
		tcWebApi_Commit("WebCurSet_Entry")
		tcWebApi_Commit("usbRestore_Entry")
		tcWebApi_Save()
	elseif request_Form("backupflg") = "2" then
		tcWebApi_set("WebCurSet_Entry", "opt", "opt")
		tcWebApi_Commit("WebCurSet_Entry")
		tcWebApi_Commit("usbRestore_Entry")
	elseif request_Form("backupflg") = "3" then
		tcWebApi_set("WebCurSet_Entry", "opt", "opt")
		tcWebApi_set("usbRestore_Entry", "autorestore", "autorestoreflag")
		tcWebApi_Commit("WebCurSet_Entry")
		tcWebApi_Commit("usbRestore_Entry")
		tcWebApi_Save()
	end if
%>		

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<SCRIPT language=JavaScript type=text/javascript>
<% if Request_Form("rebootflag") = "1" then %>
<% if Request_Form("isCUCSupport") = "1" then %>
	setTimeout("doRefresh()",80*1000);
<% else %>
	setTimeout("doRefresh()",35*1000);
<% end if %>
	document.writeln("<%tcWebApi_get("String_Entry","DevManage1Text","s")%>");
<% end if %>
<% if Request_Form("defaultflag") = "1" then %>
<% if Request_Form("isCUCSupport") = "1" then %>
	setTimeout("doRefresh()",80*1000);
<% else %>
	setTimeout("doRefresh()",35*1000);
<% end if %>
	document.writeln("<%tcWebApi_get("String_Entry","DevManage2Text","s")%>");
<% end if %>
function doRefresh()
{
	//alert("<%tcWebApi_get("String_Entry","DevManage3Text","s")%>"); 
 	window.top.location.href = "../";    
}
</SCRIPT>
<%if tcWebApi_get("System_Entry","reboot_type","h") = "0" then%>
<HEAD><TITLE><%tcWebApi_get("String_Entry","DevManage4Text","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<SCRIPT language=JavaScript src="/JS/printtable.js"></SCRIPT>
<% if TCWebApi_get("WebCustom_Entry","isCT2PORTSSupported","h" ) <> "Yes" then %>
<% if TCWebApi_get("WebCustom_Entry","isCT1PORTSupported","h" ) <> "Yes" then %>
<SCRIPT language=JavaScript type=text/javascript src="/cgi-bin/showusb.cgi"></SCRIPT>
<% end if %>
<% end if %>
<SCRIPT language=JavaScript type=text/javascript>

function usbRestore()
{
	if(document.USBForm.usbRestoreFlag.checked == true){
		document.USBForm.autorestoreflag.value = "1";
	}else{
		document.USBForm.autorestoreflag.value = "0";
	}

	document.USBForm.backupflg.value = "3";
	document.USBForm.opt.value = "0";
	if( true == setEBooValueCookie(document.USBForm) )
		document.USBForm.submit();
}

function usbConfResult()
{
	document.USBForm.backupflg.value = "0";
	
	if(document.USBForm.autorestoreflag.value =="1")
	{
		document.USBForm.usbRestoreFlag.checked = true;
	}else
	{
		document.USBForm.usbRestoreFlag.checked = false;
}

	BackupEn();
	
	var str, commit;
	
	commit = false;
	
	if (document.USBForm.rstresult.value == "1")
{
		str = '<%tcWebApi_get("String_Entry","DevManage5Text","s")%>';
		alert(str);
		commit = true;
	}
	else if (document.USBForm.rstresult.value == "2")
	{
		str = '<%tcWebApi_get("String_Entry","DevManage6Text","s")%>';
		alert(str);
		commit = true;
	}
	else if(document.USBForm.rstresult.value == "3")
	{
		str = '<%tcWebApi_get("String_Entry","DevManage7Text","s")%>';
		alert(str);
		commit = true;
	}
	else if(document.USBForm.rstresult.value == "4")
	{
		var bchoise = 1;
		bchoise=confirm("<%tcWebApi_get("String_Entry","DevManage8Text","s")%>");
		if(!bchoise)
		{
			commit = true;
		}
		else
		{
			document.USBForm.opt.value = "2";
			document.USBForm.backupflg.value = "2";
			if( true == setEBooValueCookie(document.USBForm) )
				document.USBForm.submit();
		}
		}
	else if(document.USBForm.rstresult.value == "7")
	{
		str = '<%tcWebApi_get("String_Entry","DevManage9Text","s")%>';
		alert(str);
		commit = true;
	}
	else if(document.USBForm.rstresult.value == "8")
	{
		str = '<%tcWebApi_get("String_Entry","DevManage10Text","s")%>';
		alert(str);
		commit = true;
	}

	if(commit){
		document.USBForm.opt.value = "3";
		document.USBForm.backupflg.value = "2";
		if( true == setEBooValueCookie(document.USBForm) )
			document.USBForm.submit();
	}
}
function SetUserType()
{
	var curUserType = top.curUser;
	//current user check 
	if (curUserType == '0') 
	{
		getElement('usbconf').style.display = "none";
		getElement('clearlog').style.display = "none";
	}
	else if (curUserType == '1') 
	{
		getElement('usbconf').style.display = "none";
		getElement('clearlog').style.display = "none";
		getElement('restoredefault0').style.display = "none";
		getElement('restoredefault1').style.display = "none";
		getElement('restoredefault2').style.display = "none";
	}
}
function BackupEn()
{
	if(document.USBForm.ClDevType.value == "N/A")
	{
		document.USBForm.usbConfBtn.disabled = true;
	}
	else if (document.USBForm.ClDevType.value == "<%tcWebApi_get("String_Entry","DevManage11Text","s")%>")
	{
		document.USBForm.usbConfBtn.disabled = true;
	}
	else
	{
		document.USBForm.usbConfBtn.disabled = false;
	}
}
</SCRIPT>
      <SCRIPT language=JavaScript type=text/javascript>
function LoadFrame()
{
	document.ResetForm.rebootflag.value = "0";
	<% if TCWebApi_get("WebCustom_Entry","isCT2PORTSSupported","h" ) <> "Yes" then %>
	<% if TCWebApi_get("WebCustom_Entry","isCT1PORTSupported","h" ) <> "Yes" then %>
	usbConfResult();
	<% end if %>
	<% end if %>
}

function backupToUSB()
{
	
	document.USBForm.backupflg.value = "1";
	document.USBForm.opt.value = "1";
	if( true == setEBooValueCookie(document.USBForm) )
		document.USBForm.submit();
}

function doRefresh()
{
	var mydate = new Date();
	mydate.setTime(mydate.getTime() - 1);
	document.cookie = "SESSIONID=; expires=" + mydate.toGMTString(); 
 	window.top.location.href = "../";    
}

function ReDefaultboot()
{
	if(checkVoipIdle() == false)
	{
		return false;
	}
	
	<% if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes" then %>	
	if(!confirm("<%tcWebApi_get("String_Entry","DevManage12Text","s")%>"))
	<%else%>
	if(confirm("<%tcWebApi_get("String_Entry","DevManage13Text","s")%>"))
	<%end if%>
	{
		document.DefaultForm.defaultflag.value = "1";
		document.DefaultForm.restoreflag2.value = "2";
		if( true == setEBooValueCookie(document.DefaultForm) )
			document.DefaultForm.submit();
		getElById('buttondefaultbak').disabled = true;
		setTimeout("doRefresh()",30*1000);
	}
	//十秒钟
}

function Reboot()
{
	if(checkVoipIdle() == false)
	{
		return false;
	}
	<% if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes" then %>	
	if(!confirm("<%tcWebApi_get("String_Entry","DevManage14Text","s")%>"))
	<%else%>
	if(confirm("<%tcWebApi_get("String_Entry","DevManage15Text","s")%>"))
	<%end if%>
	{
		document.ResetForm.rebootflag.value = "1";
		document.ResetForm.restoreFlag.value = "1";
		if( true == setEBooValueCookie(document.ResetForm) )
			document.ResetForm.submit();
		getElById('btnReboot').disabled = true;
		setTimeout("doRefresh()",30*1000);
	}
}

</SCRIPT>
<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('USBForm') != null)LoadFrame()" 
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
                  href="/cgi-bin/login.asp" id=logOut name=logOut target=_top><SPAN 
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
            value="<%tcWebApi_get("String_Entry","ContentManageText","s")%>-><%tcWebApi_get("String_Entry","ContentDeviceManagementText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
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
          <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","DevManage16Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
              <TD width=474>&nbsp; </TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
            rowSpan=15><TABLE cellSpacing=0 cellPadding=20 width="100%" 
border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#设备管理" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0> </A></TD></TR>
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
		  <FORM id="ResetForm" name="ResetForm" action="/cgi-bin/mag-reset.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD>&nbsp;</TD>
                      <TD width=10>&nbsp;</TD></TR></TBODY></TABLE>
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=10>&nbsp;</TD>
                      <TD align=middle><INPUT type="button" value="    <%tcWebApi_get("String_Entry","DevManage16Text","s")%>    "  name="btnReboot" onclick="Reboot();" >
                                  <input type="hidden" name="rebootflag" value="0"> 
                                  <input type="HIDDEN" name="restoreFlag" value="0">
				<INPUT type="hidden" NAME="isCUCSupport" value="<%if tcwebApi_get("Info_WLan","isCUCSupport","h")="Yes" then asp_write("1") else asp_write("0") end if%>">
				  
                                </TD>
                      <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></FORM></TD></TR>
				<% if tcWebApi_get("WebCurSet_Entry","CurrentAccess","h") = "0" then %>
				<% if TCWebApi_get("WebCustom_Entry","isCT2PORTSSupported","h" ) <> "Yes" then %>
				<% if TCWebApi_get("WebCustom_Entry","isCT1PORTSupported","h" ) <> "Yes" then %>
        <DIV id=usbconf>
        <TR id=usbconf0>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","DevManage17Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD></TR>
        <TR id=usbconf1>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR id=usbconf2>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
		  <FORM id="USBForm" name="USBForm" action="/cgi-bin/mag-reset.asp" method="post">	
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=10>&nbsp;</TD>
			<INPUT TYPE="HIDDEN" NAME="autorestoreflag" VALUE="<%TCWebApi_get("usbRestore_Entry","autorestore","s")%>">
			<INPUT TYPE="HIDDEN" NAME="backupflg" VALUE="0">
			<INPUT TYPE="HIDDEN" NAME="rstresult" VALUE="<%TCWebApi_get("WebCurSet_Entry","rstresult","s")%>">
			<INPUT TYPE="HIDDEN" NAME="opt" VALUE="0">
                      <TD align=middle><%tcWebApi_get("String_Entry","DevManage18Text","s")%> <SELECT id=ClDevType onChange="BackupEn()"
                        name=ClDevType> 
                        <SCRIPT language=JavaScript type=text/javascript>
				var i;
				for(i=0;i < usbvalue.length;i++)
				{
					var t1 =usbvalue[i].split("usb");  
					var t2 =t1[1].split("_");
					document.write("<option value=\""+usbvalue[i]+"\">USB"+t2[0]+"-"+t2[1]+"&nbsp;<%tcWebApi_get("String_Entry","DevManage19Text","s")%></option>");
				}
					if(usbvalue.length == 0)
						document.write("<option value=\"N/A\"><%tcWebApi_get("String_Entry","DevManage11Text","s")%></option>");
			</SCRIPT>
                        </SELECT> &nbsp; <INPUT id="usbConfBtn" onclick="backupToUSB()" type="button" value="<%tcWebApi_get("String_Entry","DevManage21Text","s")%>" name="usbConfBtn"> 
                        <BR><BR>
                        <CENTER><%tcWebApi_get("String_Entry","DevManage22Text","s")%> 
                        	<INPUT id="usbRestoreFlag" onclick="usbRestore()" type="checkbox" name="usbRestoreFlag"> 
                        </CENTER>                       
                                <input type="HIDDEN" name="login_init" value="0">
	 			<INPUT TYPE="HIDDEN" NAME="restoreFlag" VALUE="0">		                                
                                <input type="HIDDEN" name="postflag" value="0"> 
                                <input type="HIDDEN" name="HTML_HEADER_TYPE" value="2"> 
                                <input type="HIDDEN" name="usertype0" value="<%TCWebApi_get("Account_Entry0","usertype0","s")%>"> 
                                <input type="HIDDEN" name="usertype1" value="<%TCWebApi_get("Account_Entry1","usertype1","s")%>"> 
                                <input type="HIDDEN" name="usertype2" value="<%TCWebApi_get("Account_Entry2","usertype2","s")%>"> 
                                <input type="HIDDEN" name="name0" value="<%TCWebApi_get("Account_Entry0","username","s")%>"> 
                                <input type="HIDDEN" name="name1" value="<%TCWebApi_get("Account_Entry1","username","s")%>"> 
                                <input type="HIDDEN" name="name2" value="<%TCWebApi_get("Account_Entry2","username","s")%>">                            
                                <SCRIPT language=javascript>
						{
							BackupEn();
						}
						</SCRIPT>
                      </TD>
                      <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></FORM></TD></TR></DIV>
        <% end if %>
        <% end if %>
        <DIV></DIV>
        <DIV id=clearlog>
        <TR id=clearlog0  style="display:none">
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","DevManage23Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD></TR>
        <TR id=clearlog1  style="display:none">
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR id=clearlog2  style="display:none">
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=10>&nbsp;</TD>
                      <TD align=middle><INPUT onclick='if(confirm("<%tcWebApi_get("String_Entry","DevManage24Text","s")%>")) {location.replace("clearsyslog.cgi");}' type=button value="    <%tcWebApi_get("String_Entry","DevManage23Text","s")%>    " name=button> 
                      </TD>
                      <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></DIV>
		<% end if %>
        <TR id=restoredefault0 name="restoredefault0">
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","DevManage26Text","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD></TR>
        <TR id=restoredefault1 name="restoredefault1">
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR id=restoredefault2 name="restoredefault2">
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
		  <FORM id="DefaultForm" name="DefaultForm" action="/cgi-bin/mag-reset.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=10>&nbsp;</TD>
                              <TD align=middle><input type="button" value="  <%tcWebApi_get("String_Entry","DevManage26Text","s")%>  "  id="buttondefaultbak" name="buttondefaultbak"  onClick="ReDefaultboot();" >
                                <input type="hidden" id="defaultflag" name="defaultflag" value="0">
                                <input type="hidden" id="restoreflag2" name="restoreflag2" value="0">
				<INPUT type="hidden" id="isCUCSupport" NAME="isCUCSupport" value="<%if tcwebApi_get("Info_WLan","isCUCSupport","h")="Yes" then asp_write("1") else asp_write("0") end if%>">
                            </TD>
                      <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD>

                <TD width=10>&nbsp;</TD></TR>

        </TBODY></TABLE></FORM></TD></TR>
        <SCRIPT language=JavaScript type=text/javascript>
				if (curUserName != sptUserName)
				{
					<% if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") <> "Yes" then %>		
					getElement('restoredefault0').style.display = "none";
					getElement('restoredefault1').style.display = "none";
					getElement('restoredefault2').style.display = "none";
		<% end if %>
				}
				</SCRIPT>			

        <TR>

          <TD vAlign=top width=157 bgColor=#e7e7e7 height="100%"></TD>

          <TD width=7 background=/img/panel3.gif>　</TD>

          <TD><script language="JavaScript" type="text/JavaScript">
if ( getElement('usbconf') )
	getElement('usbconf').style.display = "none";
if ( getElement('clearlog') )
	getElement('clearlog').style.display = "none";

</script></TD></TR>

        <TR>

          <TD vAlign=top width=157 bgColor=#e7e7e7 height="100%"></TD>

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



            <P align=center><!--IMG id=btnOK onclick="RefreshPage()" height=23 



            src="/img/ok.gif" width=80 border=0>&nbsp;&nbsp;<IMG 



            id=btnCancel onclick="RefreshPage()" height=23 src="/img/cancel.gif" 



            width=80 border=0--> </P></TD>



          <TD width=170 



bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>



</BODY>

<% end if %>

</HTML>



