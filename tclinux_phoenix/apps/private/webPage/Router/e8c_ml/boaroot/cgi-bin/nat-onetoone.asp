<%
if Request_Form("delFlag") = "1" then

	if Request_Form("hSrouteFlag0") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry0")
	end if
	if Request_Form("hSrouteFlag1") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry1")
	end if
	if Request_Form("hSrouteFlag2") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry2")
	end if
	if Request_Form("hSrouteFlag3") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry3")
	end if
	if Request_Form("hSrouteFlag4") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry4")
	end if
	if Request_Form("hSrouteFlag5") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry5")
	end if
	if Request_Form("hSrouteFlag6") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry6")
	end if
	if Request_Form("hSrouteFlag7") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry7")
	end if	
	
	if Request_Form("hSrouteFlag8") <> "17" then 
	tcWebApi_constSet("StaticNat_Common","Flag","delete")
	TCWebApi_unset("StaticNat_Entry8")
	end if	

	if Request_Form("hSrouteFlag9") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry9")
	end if	

	if Request_Form("hSrouteFlag10") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry10")
	end if	

	if Request_Form("hSrouteFlag11") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry11")
	end if	

	if Request_Form("hSrouteFlag12") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry12")
	end if	

	if Request_Form("hSrouteFlag13") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry13")
	end if	

	if Request_Form("hSrouteFlag14") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry14")
	end if	

	if Request_Form("hSrouteFlag15") <> "17" then 
		tcWebApi_constSet("StaticNat_Common","Flag","delete")
		TCWebApi_unset("StaticNat_Entry15")
	end if	

	TCWebApi_save()
end if

%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>one-to-one</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>

<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('OnetoOneForm') != null)LoadFrame()" 
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
            value="网络->one-to-one" name=Selected_Menu> </FONT></B><SPAN 
            class=GatewayName>网关名称:
            <SCRIPT 
            language=javascript>
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
      <SCRIPT language=JavaScript src="/JS/printbox.js"></SCRIPT>

      <SCRIPT language=JavaScript type=text/javascript>

	

var nCount = 16;
function CheckRoutesActives()
{
	var	nCurTemp = 0;
	var	vLocalIPAddress = new Array(nCount);
	var	vWanIPAddress = new Array(nCount);
	var	vInterface = new Array(nCount);
	var	vNetMask = new Array(nCount);
	
	var	vcurLinks = new Array(nCount+3);
	
	var	vTemp;
	var	RouteNameTemp;	


	vLocalIPAddress[0] = "<% tcWebApi_get("StaticNat_Entry0","LocalIP","s") %>";	
	vLocalIPAddress[1] = "<% tcWebApi_get("StaticNat_Entry1","LocalIP","s") %>";	
	vLocalIPAddress[2] = "<% tcWebApi_get("StaticNat_Entry2","LocalIP","s") %>";	
	vLocalIPAddress[3] = "<% tcWebApi_get("StaticNat_Entry3","LocalIP","s") %>";	
	vLocalIPAddress[4] = "<% tcWebApi_get("StaticNat_Entry4","LocalIP","s") %>";	
	vLocalIPAddress[5] = "<% tcWebApi_get("StaticNat_Entry5","LocalIP","s") %>";	
	vLocalIPAddress[6] = "<% tcWebApi_get("StaticNat_Entry6","LocalIP","s") %>";	
	vLocalIPAddress[7] = "<% tcWebApi_get("StaticNat_Entry7","LocalIP","s") %>";	
	vLocalIPAddress[8] = "<% tcWebApi_get("StaticNat_Entry8","LocalIP","s") %>";	
	vLocalIPAddress[9] = "<% tcWebApi_get("StaticNat_Entry9","LocalIP","s") %>";	
	vLocalIPAddress[10] = "<% tcWebApi_get("StaticNat_Entry10","LocalIP","s") %>";	
	vLocalIPAddress[11] = "<% tcWebApi_get("StaticNat_Entry11","LocalIP","s") %>";	
	vLocalIPAddress[12] = "<% tcWebApi_get("StaticNat_Entry12","LocalIP","s") %>";	
	vLocalIPAddress[13] = "<% tcWebApi_get("StaticNat_Entry13","LocalIP","s") %>";	
	vLocalIPAddress[14] = "<% tcWebApi_get("StaticNat_Entry14","LocalIP","s") %>";	
	vLocalIPAddress[15] = "<% tcWebApi_get("StaticNat_Entry15","LocalIP","s") %>";	
	
	vWanIPAddress[0] = "<% tcWebApi_get("StaticNat_Entry0","WanIP","s") %>";
	vWanIPAddress[1] = "<% tcWebApi_get("StaticNat_Entry1","WanIP","s") %>";
	vWanIPAddress[2] = "<% tcWebApi_get("StaticNat_Entry2","WanIP","s") %>";
	vWanIPAddress[3] = "<% tcWebApi_get("StaticNat_Entry3","WanIP","s") %>";
	vWanIPAddress[4] = "<% tcWebApi_get("StaticNat_Entry4","WanIP","s") %>";
	vWanIPAddress[5] = "<% tcWebApi_get("StaticNat_Entry5","WanIP","s") %>";
	vWanIPAddress[6] = "<% tcWebApi_get("StaticNat_Entry6","WanIP","s") %>";
	vWanIPAddress[7] = "<% tcWebApi_get("StaticNat_Entry7","WanIP","s") %>";
	vWanIPAddress[8] = "<% tcWebApi_get("StaticNat_Entry8","WanIP","s") %>";
	vWanIPAddress[9] = "<% tcWebApi_get("StaticNat_Entry9","WanIP","s") %>";
	vWanIPAddress[10] = "<% tcWebApi_get("StaticNat_Entry10","WanIP","s") %>";
	vWanIPAddress[11] = "<% tcWebApi_get("StaticNat_Entry11","WanIP","s") %>";
	vWanIPAddress[12] = "<% tcWebApi_get("StaticNat_Entry12","WanIP","s") %>";
	vWanIPAddress[13] = "<% tcWebApi_get("StaticNat_Entry13","WanIP","s") %>";
	vWanIPAddress[14] = "<% tcWebApi_get("StaticNat_Entry14","WanIP","s") %>";
	vWanIPAddress[15] = "<% tcWebApi_get("StaticNat_Entry15","WanIP","s") %>";
	
	vInterface[0] = "<% tcWebApi_get("StaticNat_Entry0","Interface","s") %>";
	vInterface[1] = "<% tcWebApi_get("StaticNat_Entry1","Interface","s") %>";
	vInterface[2] = "<% tcWebApi_get("StaticNat_Entry2","Interface","s") %>";
	vInterface[3] = "<% tcWebApi_get("StaticNat_Entry3","Interface","s") %>";
	vInterface[4] = "<% tcWebApi_get("StaticNat_Entry4","Interface","s") %>";
	vInterface[5] = "<% tcWebApi_get("StaticNat_Entry5","Interface","s") %>";
	vInterface[6] = "<% tcWebApi_get("StaticNat_Entry6","Interface","s") %>";
	vInterface[7] = "<% tcWebApi_get("StaticNat_Entry7","Interface","s") %>";
	vInterface[8] = "<% tcWebApi_get("StaticNat_Entry8","Interface","s") %>";
	vInterface[9] = "<% tcWebApi_get("StaticNat_Entry9","Interface","s") %>";
	vInterface[10] = "<% tcWebApi_get("StaticNat_Entry10","Interface","s") %>";
	vInterface[11] = "<% tcWebApi_get("StaticNat_Entry11","Interface","s") %>";
	vInterface[12] = "<% tcWebApi_get("StaticNat_Entry12","Interface","s") %>";
	vInterface[13] = "<% tcWebApi_get("StaticNat_Entry13","Interface","s") %>";
	vInterface[14] = "<% tcWebApi_get("StaticNat_Entry14","Interface","s") %>";
	vInterface[15] = "<% tcWebApi_get("StaticNat_Entry15","Interface","s") %>";
			
	vcurLinks[nCurTemp++] = new Array("nat-onetoone.asp", "StaticRoute", "", "20%", "20%", "20%", "10%", "10%");
	vcurLinks[nCurTemp++] = new Array("cgi-bin/nat-onetooneadd.asp","", "InternetGatewayDevice.Layer3Forwarding.Forwarding", "local地址", "wan地址", "连接地址" );
	vcurLinks[nCurTemp++] = new Array("", "", "", "", "", "", "","");
	
	for(var i=0; i<nCount; i++)
	{
		if(vLocalIPAddress[i] != "N/A")
			vcurLinks[nCurTemp++] = new Array("","", i, vLocalIPAddress[i], vWanIPAddress[i], vInterface[i], vNetMask[i],"","");
	}
	var	vObjRet = new Array(nCurTemp+1);
	for(var m=0; m<nCurTemp; m++)
	{
		vObjRet[m] = vcurLinks[m];
	}
	vObjRet[nCurTemp] = null;
	return vObjRet;
}
var StaticRoute = CheckRoutesActives();
function CheckForm()
{
if (getElById('sel_Interface').length == 0)
{
alert('当前没有WAN连接，请新建新的WAN连接后one to one配置！');
return false;
}
return true;
}


</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0 id="mainTableSet">
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1>One-to-One NAT</P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
            rowSpan=10 id="TRSpan"><TABLE cellSpacing=0 cellPadding=20 width="100%" 
border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#One to One" 
                  target=_blank><IMG height=34 src="/img/help.gif" 
                  width=40 border=0></A></TD></TR>
              </TBODY></TABLE>　 　　　 　　　</TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <DIV action="" name="ConfigForm">
			<FORM id="StaticRouteForm" name="StaticRouteForm" action="/cgi-bin/nat-onetoone.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <P>
				  
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
printInstNoEditBox('StaticRoute',400);
</SCRIPT>
                        <input type="hidden" name="del_num">
						<input type="hidden" name="hSrouteFlag0" value="17">
                        <input type="hidden" name="hSrouteFlag1" value="17">
                        <input type="hidden" name="hSrouteFlag2" value="17">
                        <input type="hidden" name="hSrouteFlag3" value="17">
                        <input type="hidden" name="hSrouteFlag4" value="17">
                        <input type="hidden" name="hSrouteFlag5" value="17">
                        <input type="hidden" name="hSrouteFlag6" value="17">
                        <input type="hidden" name="hSrouteFlag7" value="17">
                        <input type="hidden" name="hSrouteFlag8" value="17">
						<input type="hidden" name="hSrouteFlag9" value="17">
						<input type="hidden" name="hSrouteFlag10" value="17">
						<input type="hidden" name="hSrouteFlag11" value="17">
						<input type="hidden" name="hSrouteFlag12" value="17">
						<input type="hidden" name="hSrouteFlag13" value="17">
						<input type="hidden" name="hSrouteFlag14" value="17">
						<input type="hidden" name="hSrouteFlag15" value="17">
                        <input type="hidden" name="delFlag" value="0">
                         <BR><BR></TD>
                      <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></P></TD>
                <TD width=10>&nbsp;</TD></TR>
                </TBODY></TABLE></FORM></DIV></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        
        <TR id="DynaRouteInfo2">
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
   </TABLE></TD></TR>
        <TR>

          <TD vAlign=top width=157 bgColor=#e7e7e7></TD>
          <TD width=8 background=/img/panel3.gif></TD>
            <TD width=474>　
              <script language="JavaScript" type="text/JavaScript">
				getElById("DynaRouteInfo2").style.display = "none";
			  </script></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <FORM id="OnetoOne" name="OnetoOne" action="/cgi-bin/nat-onetoone.asp" method="post">
          
				  	<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0 style="display:none;">
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <P>
                  <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD>
                        <TABLE cellSpacing=1 cellPadding=3 width=400 border=1>
                          <TBODY>
                          <TR align=middle>
                            <TD class=table_title align=middle>local地址</TD>
                            <TD class=table_title align=middle>wan地址</TD>
                            <TD class=table_title align=middle>连接地址</TD>
                            <TD class=table_title align=middle>删除</TD></TR>
                         <SCRIPT language=JavaScript type=text/javascript>
</SCRIPT>
                          </TBODY></TABLE>
                                <LEFT><INPUT id=btnAdd onclick="AddStaticRt()" type=button value=添加> 
                        &nbsp; <INPUT id="btnRemove" onclick="removeClick(this.form.rml)" type=button value=删除> 
                        </LEFT></TD></TR></TBODY></TABLE></P></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></FORM></TD></TR>
        <DIV></DIV>
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
            <P align=center><IMG id=btnOK onclick="" height=23  src="/img/ok.gif" width=80 border=0>&nbsp;&nbsp;<IMG 
            id=btnCancel onclick="RefreshPage()" height=23 src="/img/cancel.gif" 
            width=80 border=0> </P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>



