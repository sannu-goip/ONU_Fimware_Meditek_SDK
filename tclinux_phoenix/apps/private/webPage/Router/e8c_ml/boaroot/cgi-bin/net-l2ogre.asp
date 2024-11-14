
<%
if Request_Form("Save_Flag")="1" then
	TCWebApi_set("L2oGRE_Common","Active","ActiveChange_Flag")
	TCWebApi_set("L2oGRE_Entry0","RemoteIP","remoteIpAddress")
	TcWebApi_set("L2oGRE_Entry0","CurIFIndex","CurL2ogreIF_Flag") 
	TcWebApi_set("L2oGRE_Entry0","Interface","CurL2ogreIFName_Flag") 

	TCWebApi_set("L2oGRE_Entry0","ttl","L2ogreTTL")
	
	TCWebApi_set("L2oGRE_Entry0","LAN1","LAN1Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","LAN2","LAN2Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","LAN3","LAN3Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","LAN4","LAN4Change_Flag")
	
	TCWebApi_set("L2oGRE_Entry0","SSID1","SSID1Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","SSID2","SSID2Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","SSID3","SSID3Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","SSID4","SSID4Change_Flag")

	TCWebApi_set("L2oGRE_Entry0","SSID5","SSID5Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","SSID6","SSID6Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","SSID7","SSID7Change_Flag")
	TCWebApi_set("L2oGRE_Entry0","SSID8","SSID8Change_Flag")

	TCWebApi_commit("L2oGRE")
	TCWebApi_save()
	
end If

if Request_Form("Save_Flag")="2" then
	TcWebApi_set("L2oGRE_Entry0","CurIFIndex","CurL2ogreIF_Flag") 
	TcWebApi_set("L2oGRE_Entry0","Interface","CurL2ogreIFName_Flag") 
	TCWebApi_commit("L2oGRE")
	TCWebApi_save()
end if
%>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>L2oGRE配置</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>

<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<style>
DIV {
	float: left;
}
</style>
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
                <TD class=welcom vAlign=bottom align=middle width=120>欢迎您！ </TD>
                <TD vAlign=bottom width=50><A onclick=DoLogout() 
                  href="/cgi-bin/logout.cgi" target=_top><SPAN 
                  class=logout>退出 
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
            value="网络->L2oGRE配置" name=Selected_Menu> </FONT></B><SPAN 
            class=GatewayName>网关名称:
<SCRIPT language=javascript>
document.write(top.gateWayName);
</SCRIPT>
             </SPAN></FONT></P></TD>
          <TD width=211 bgColor=#ef8218 height=33>
            <P class=GatewayType align=center>型号:
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
	  
function LoadFrame()
{
	resetEBooValueCookie();
	var vForm = document.ConfigForm;
	var selObj = getElById("L2oGREInterface");
	var curSetId = selObj.options[selObj.selectedIndex].id;
	var curSetValue = selObj.options[selObj.selectedIndex].value;
	var curSetId2 = "<% tcWebApi_get("L2oGRE_Entry0","CurIFIndex","s") %>";
	if(curSetId != curSetId2)
	{
		vForm.CurL2ogreIF_Flag.value = curSetId;
		vForm.CurL2ogreIFName_Flag.value = curSetValue;
		vForm.Save_Flag.value = "2";
		vForm.submit();	
	}

	vForm.Save_Flag.value = "0";	
}
	
function CheckTTLVlaue(TTL)
{
	var str = /^\+?[1-9][0-9]*$/;
	if(str.test(TTL) &&  1<= TTL  && TTL <= 255)
	{
		return true;
	}
	else
	{
		alert("TTL 无效(1-255)."); 
		return false;
	}	
}

function SubmitForm()
{
	var vForm = document.ConfigForm;
	var remoteIP = vForm.remoteIpAddress.value;
	var ttl = vForm.L2ogreTTL.value;
	if(CheckRemoteIP(remoteIP) == false || CheckTTLVlaue(ttl) == false)
	{
		return false;	
	}
	else
	{
	vForm.Save_Flag.value = "1";
	vForm.submit();	
}

}

function L2ogreClickEnble(ctl)
{
	var vForm = document.ConfigForm;
		if (ctl.checked == true)
		{
			switch(ctl.name)
			{
				case "enableL2ogre" : 
					vForm.ActiveChange_Flag.value  = "1";
					break;
				case "enableL2ogreLAN1" : 
					vForm.LAN1Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreLAN2" : 
					vForm.LAN2Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreLAN3" : 
					vForm.LAN3Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreLAN4" : 
					vForm.LAN4Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID1" : 
					vForm.SSID1Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID2" : 
					vForm.SSID2Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID3" : 
					vForm.SSID3Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID4" : 
					vForm.SSID4Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID5" : 
					vForm.SSID5Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID6" : 
					vForm.SSID6Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID7" : 
					vForm.SSID7Change_Flag.value  = "Yes";
					break;
				case "enableL2ogreSSID8" : 
					vForm.SSID8Change_Flag.value  = "Yes";
					break;
				default:
					break;			
			}
		}
		else
		{
			switch(ctl.name)
			{
				case "enableL2ogre" : 
					vForm.ActiveChange_Flag.value  = "0";
					break;
				case "enableL2ogreLAN1" : 
					vForm.LAN1Change_Flag.value  = "No";
					break;
				case "enableL2ogreLAN2" : 
					vForm.LAN2Change_Flag.value  = "No";
					break;
				case "enableL2ogreLAN3" : 
					vForm.LAN3Change_Flag.value  = "No";
					break;
				case "enableL2ogreLAN4" : 
					vForm.LAN4Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID1" : 
					vForm.SSID1Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID2" : 
					vForm.SSID2Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID3" : 
					vForm.SSID3Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID4" : 
					vForm.SSID4Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID5" : 
					vForm.SSID5Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID6" : 
					vForm.SSID6Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID7" : 
					vForm.SSID7Change_Flag.value  = "No";
					break;
				case "enableL2ogreSSID8" : 
					vForm.SSID8Change_Flag.value  = "No";
					break;
				default:
					break;
			}	
		}	
}

function CheckRemoteIP(lanIp)
{
	var lanIpParts = lanIp.split('.');
	
	if (isValidIpAddress(lanIp) == false
		|| lanIpParts[3] == 0
		|| lanIpParts[3] == 255
		|| isDeIpAddress(lanIp) == true
		|| isBroadcastIpAddress(lanIp) == true
		|| isLoopIpAddress(lanIp) == true )
		{
			alert('地址 "' + lanIp + '" 是无效IP地址。');
		return false;
	}

	return true;
	
}

function WriteWanNameSelected()
{
	var i=0;
	var nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryName = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryValue = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryIndex = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var WANEnNAT = vArrayStr.split(',');
	var selObj = getElById("L2oGREInterface");
	var CurL2ogreIF_index = document.ConfigForm.CurL2ogreIF_Flag.value;
	
	vEntryName = SortUtil.UpdateWanIFName( vEntryName, nEntryNum );
	
	vEntryValue = SortUtil.SortMyArray( vEntryValue );
	vEntryIndex = SortUtil.SortMyArray( vEntryIndex );

	for (i = 0; i < nEntryNum; i++)
	{
		if( ( vEntryName[i].indexOf('_INTERNET_') >= 0 || vEntryName[i].indexOf('OTHER') >= 0 )
			 && vEntryName[i].indexOf('_B_') < 0 )
		{
			if(vEntryIndex[i] == CurL2ogreIF_index)
			{
				document.writeln("<option id=" + vEntryIndex[i] + " " + "value=" + vEntryValue[i] +" selected>" + vEntryName[i] + "</option>");
			}
			else
			{
				document.writeln("<option id=" + vEntryIndex[i] + " " + "value=" + vEntryValue[i] +">" + vEntryName[i] + "</option>");
			}
		}
	}
}

function interfaceChange()
{
	with (document.forms[0])
	{		
		var vForm = document.ConfigForm;
		var selObj = getElById("L2oGREInterface");
		var curSetId = selObj.options[selObj.selectedIndex].id;
		var curSetValue = selObj.options[selObj.selectedIndex].value;
		vForm.CurL2ogreIF_Flag.value = curSetId;
		vForm.CurL2ogreIFName_Flag.value = curSetValue;
		vForm.Save_Flag.value = "1";
	}
	document.forms[0].submit();
}

</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1>L2oGRE配置</P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474> </TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=8>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#L2oGRE配置" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0></A></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
              </TBODY></TABLE></TD></TR>

	<FORM name=ConfigForm action="/cgi-bin/net-l2ogre.asp" method="post">

	<input type="hidden" name="Save_Flag" value="0">
	<input type="hidden" name="ActiveChange_Flag" value="<% tcWebApi_get("L2oGRE_Common","Active","s") %>">
	<input type="hidden" name="CurL2ogreIF_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","CurIFIndex","s") %>">
	<input type="hidden" name="CurL2ogreIFName_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","Interface","s") %>">
	
	<input type="hidden" name="LAN1Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","LAN1","s") %>">
	<input type="hidden" name="LAN2Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","LAN2","s") %>">
	<input type="hidden" name="LAN3Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","LAN3","s") %>">
	<input type="hidden" name="LAN4Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","LAN4","s") %>">
	<input type="hidden" name="SSID1Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID1","s") %>">
	<input type="hidden" name="SSID2Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID2","s") %>">
	<input type="hidden" name="SSID3Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID3","s") %>">
	<input type="hidden" name="SSID4Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID4","s") %>">
	<input type="hidden" name="SSID5Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID5","s") %>">
	<input type="hidden" name="SSID6Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID6","s") %>">
	<input type="hidden" name="SSID7Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID7","s") %>">
	<input type="hidden" name="SSID8Change_Flag" value="<% tcWebApi_get("L2oGRE_Entry0","SSID8","s") %>">
	
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>
		<TD height="24">
			<TABLE  style="width:100%;cellSpacing:0;border:0"  >
          			<TBODY>
          			<TR>
          				<TD style="width:20%">&nbsp;启动L2oGRE:</TD>
					<TD><INPUT id = enableL2ogre onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogre" <%if tcWebApi_get("L2oGRE_Common","Active","h") = "1" then asp_Write("checked") end if%>></TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
		</TD>
	</TR>
          
	           
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>　
		<TD height="24">		  
			<TABLE  style="width:100%;cellSpacing:0;border:0"  >
          			<TBODY>
          			<TR>
          				<TD style="width:20%">&nbsp;目的网络地址:</TD>
					<TD><INPUT maxLength=16 size= 12  onchange=CheckRemoteIP(this.value) 
						name="remoteIpAddress" value="<% if tcWebApi_get("L2oGRE_Entry0","RemoteIP","h" ) <> "N/A" then tcWebApi_get("L2oGRE_Entry0","RemoteIP","s" ) end if %>"></TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
		</TD>
	</TR>
       
          
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>
		<TD height="24">				  
			<TABLE  style="width:100%;cellSpacing:0;border:0" >
	          		<TBODY>
	          		<TR>
	      				<TD style="width:20%">&nbsp;选取连接:</TD>
					<TD>
						<SELECT size=1 name="L2oGREInterface" onchange=interfaceChange()> 
							<script language="JavaScript" type="text/javascript"> 
								WriteWanNameSelected(); 
							</script>
						</SELECT></TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
		</TD>
	</TR>
      
          
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>
		<TD height="24">			  
			<TABLE  style="width:100%;cellSpacing:0;border:0" >
	          		<TBODY>
	          		<TR>
					<TD style="width:20%">&nbsp;TTL:</TD>
					<TD><INPUT maxLength=3 size=12 onchange=CheckTTLVlaue(this.value) 
					name="L2ogreTTL" value="<% if tcWebApi_get("L2oGRE_Entry0","ttl","h" ) <> "N/A" then tcWebApi_get("L2oGRE_Entry0","ttl","s" ) end if %>"></TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
		</TD>
	</TR>


	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>
		<TD height="24">				  
			<TABLE  style="width:100%;cellSpacing:0;border:0" >
	          		<TBODY>
	          		<TR>
					<TD style="width:20%">&nbsp;绑定选项:</TD>
					<TD>
						<INPUT id = enableL2ogreLAN1 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreLAN1" <%if tcWebApi_get("L2oGRE_Entry0","LAN1","h") = "Yes" then asp_Write("checked") end if%>>LAN1
						<INPUT id = enableL2ogreLAN2 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreLAN2" <%if tcWebApi_get("L2oGRE_Entry0","LAN2","h") = "Yes" then asp_Write("checked") end if%>>LAN2
						<INPUT id = enableL2ogreLAN3 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreLAN3" <%if tcWebApi_get("L2oGRE_Entry0","LAN3","h") = "Yes" then asp_Write("checked") end if%>>LAN3
						<INPUT id = enableL2ogreLAN4 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreLAN4" <%if tcWebApi_get("L2oGRE_Entry0","LAN4","h") = "Yes" then asp_Write("checked") end if%>>LAN4
					</TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
		</TD>
	</TR>
	
<% if tcWebApi_get("WebCustom_Entry", "isWLanSupported","h") = "Yes" then %>
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>
			<TD height="24">	
				<TABLE  style="width:100%;cellSpacing:0;border:0" >
		          		<TBODY>
		          		<TR>
						<TD style="width:20%"></TD>
						<TD>
							<DIV id = selL2ogreSSID1 >
								<INPUT id = enableL2ogreSSID1 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID1" <%if tcWebApi_get("L2oGRE_Entry0","SSID1","h") = "Yes" then asp_Write("checked") end if%>>SSID1</DIV>
							<DIV id = selL2ogreSSID2 >
								<INPUT id = enableL2ogreSSID2 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID2" <%if tcWebApi_get("L2oGRE_Entry0","SSID2","h") = "Yes" then asp_Write("checked") end if%>>SSID2</DIV>
							<DIV id =selL2ogreSSID3 >
								<INPUT id = enableL2ogreSSID3 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID3" <%if tcWebApi_get("L2oGRE_Entry0","SSID3","h") = "Yes" then asp_Write("checked") end if%>>SSID3</DIV>
							<DIV id =selL2ogreSSID4 >
								<INPUT id = enableL2ogreSSID4 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID4" <%if tcWebApi_get("L2oGRE_Entry0","SSID4","h") = "Yes" then asp_Write("checked") end if%>>SSID4</DIV>
	       				 </TD>
					</TR>
					</TBODY>
				</TABLE>
			</TD>
		</TD>
	</TR>
<% end if %>

<% if tcWebApi_get("WebCustom_Entry", "isWLanACSupported","h") = "Yes" then %>	
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
		<TD width=7 background=/img/panel3.gif>
			<TD height="24">	
				<TABLE  style="width:100%;cellSpacing:0;border:0" >
		          		<TBODY>
		          		<TR>
						<TD style="width:20%"></TD>
						<TD>
							<DIV id = selL2ogreSSID5 >
								<INPUT id = enableL2ogreSSID5 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID5" <%if tcWebApi_get("L2oGRE_Entry0","SSID5","h") = "Yes" then asp_Write("checked") end if%>>SSID5</DIV>
							<DIV id = selL2ogreSSID6>
								<INPUT id = enableL2ogreSSID6 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID6" <%if tcWebApi_get("L2oGRE_Entry0","SSID6","h") = "Yes" then asp_Write("checked") end if%>>SSID6</DIV>
							<DIV id = selL2ogreSSID7>
								<INPUT id = enableL2ogreSSID7 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID7" <%if tcWebApi_get("L2oGRE_Entry0","SSID7","h") = "Yes" then asp_Write("checked") end if%>>SSID7</DIV>
							<DIV id = selL2ogreSSID8>
								<INPUT id = enableL2ogreSSID8 onclick=L2ogreClickEnble(this) type=checkbox  name="enableL2ogreSSID8" <%if tcWebApi_get("L2oGRE_Entry0","SSID8","h") = "Yes" then asp_Write("checked") end if%>>SSID8</DIV>
			 			</TD>
					</TR>
					</TBODY>
				</TABLE>
			</TD>
		</TD>
	</TR>
<% end if %>

	<script language="JavaScript" type="text/JavaScript">
		<% if tcWebApi_get("WebCustom_Entry", "isWLanSupported","h") = "Yes" then %>										  
			var validSSID = "<% tcWebApi_get("WLan_Common","BssidNum","s") %>";	
			if(validSSID == "1")
			{
				setDisplay('selL2ogreSSID1', 1);
				setDisplay('selL2ogreSSID2', 0);
				setDisplay('selL2ogreSSID3', 0);
				setDisplay('selL2ogreSSID4', 0);
			}
			else if(validSSID == "2")
			{
				setDisplay('selL2ogreSSID1', 1);
				setDisplay('selL2ogreSSID2', 1);
				setDisplay('selL2ogreSSID3', 0);
				setDisplay('selL2ogreSSID4', 0);
			}
			else if(validSSID == "3")
			{
				setDisplay('selL2ogreSSID1', 1);
				setDisplay('selL2ogreSSID2', 1);
				setDisplay('selL2ogreSSID3', 1);
				setDisplay('selL2ogreSSID4', 0);
			}
			else if(validSSID == "4")
			{
				setDisplay('selL2ogreSSID1', 1);
				setDisplay('selL2ogreSSID2', 1);
				setDisplay('selL2ogreSSID3', 1);
				setDisplay('selL2ogreSSID4', 1);
			}	
		<% end if %>		
			
		<% if tcWebApi_get("WebCustom_Entry", "isWLanACSupported","h") = "Yes" then %>										  
			var validacSSID = "<% tcWebApi_get("WLan11ac_Common","BssidNum","s") %>";	
			if(validacSSID == "1")
			{
				setDisplay('selL2ogreSSID5', 1);
				setDisplay('selL2ogreSSID6', 0);
				setDisplay('selL2ogreSSID7', 0);
				setDisplay('selL2ogreSSID8', 0);
			}
			else if(validacSSID == "2")
			{
				setDisplay('selL2ogreSSID5', 1);
				setDisplay('selL2ogreSSID6', 1);
				setDisplay('selL2ogreSSID7', 0);
				setDisplay('selL2ogreSSID8', 0);
			}
			else if(validacSSID == "3")
			{
				setDisplay('selL2ogreSSID5', 1);
				setDisplay('selL2ogreSSID6', 1);
				setDisplay('selL2ogreSSID7', 1);
				setDisplay('selL2ogreSSID8', 0);
			}
			else if(validacSSID == "4")
			{
				setDisplay('selL2ogreSSID5', 1);
				setDisplay('selL2ogreSSID6', 1);
				setDisplay('selL2ogreSSID7', 1);
				setDisplay('selL2ogreSSID8', 1);
			}	
		<% end if %>		
	</script>

          
	<TR>
		<TD vAlign=top width=157 bgColor=#e7e7e7 ></TD>
		<TD width=7 background=/img/panel3.gif>　</TD>          
	</TR>
	</FORM>
	</TBODY></TABLE></TD></TR>
          
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594>
            <P align=center><IMG id="btnOK" onclick="SubmitForm()" height="23"
            src="/img/ok.gif" width="80" border="0">&nbsp;&nbsp;<IMG 
            id="btnCancel" onclick="RefreshPage()" height="23" src="/img/cancel.gif" 
            width="80" border="0"> </P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR>
        </TABLE></TD></TR></TBODY></TABLE></BODY></HTML>

