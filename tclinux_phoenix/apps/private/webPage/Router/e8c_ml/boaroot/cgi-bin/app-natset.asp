<%		  
if Request_Form("alg_Flag") = "1" then
	TCWebApi_set("ALGSwitch_Entry","L2TPSW","L2TPSW_Flag")
	TCWebApi_set("ALGSwitch_Entry","IPSECSW","IPSecSW_Flag")
	TCWebApi_set("ALGSwitch_Entry","H323SW","H323SW_Flag")
	TCWebApi_set("ALGSwitch_Entry","RTSPSW","RTSPSW_Flag")
	TCWebApi_set("ALGSwitch_Entry","SIPSW","SIPSW_Flag")	
	TCWebApi_set("ALGSwitch_Entry","FTPSW","FTPSW_Flag")
	TCWebApi_set("ALGSwitch_Entry","PPTPSW","PPTPSW_Flag")
	TCWebApi_commit("ALGSwitch_Entry")	
	tcWebApi_save()
end if

if Request_Form("DMZ_Flag") = "1" then
	TCWebApi_set("WebCurSet_Entry","nat_pvc","DMZEntryCurIndex")
	TCWebApi_set("WebCurSet_Entry","dmz_ifidx","DMZEntryCurIndex")
	TCWebApi_commit("WebCurSet_Entry")
	TCWebApi_set("Dmz_Entry","IFName","DMZInterface")
	TCWebApi_set("Dmz_Entry","Active","EnableDMZ_Flag")
	
	if Request_Form("isAutoDMZSupported") = "1" then
		TCWebApi_set("Dmz_Common","UpdateDMZ_Active","AutoDMZ_Flag")
		TCWebApi_set("Dmz_Entry","AutoDMZ_Flag","AutoDMZ_Flag")
	end if
	
	TCWebApi_set("Dmz_Entry","DMZ_IP","DMZHostIPAddress")
	TCWebApi_commit("Dmz_Entry")
	tcWebApi_save()
end if

if Request_Form("VirServer_Flag") = "1" then
if tcWebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then
	TCWebApi_set("WebCurSet_Entry","nat_pvc","VirServEntryCurIndex")
	TCWebApi_set("WebCurSet_Entry","nat_ifidx","VirServEntryCurIndex")
end if
	TCWebApi_set("WebCurSet_Entry","virServ_id","VirServCurIndex")
	TCWebApi_commit("WebCurSet_Entry")
if tcWebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then
	TCWebApi_set("VirServer_Entry","IFName","PortMappingInterface")
	TCWebApi_unset("VirServer_Entry_Entry")
	TCWebApi_commit("VirServer_Entry_Entry")	
	TCWebApi_set("VirServer_Entry_Entry","Active","EnableVirServer_Flag")
	TCWebApi_set("VirServer_Entry_Entry","Protocal","PortMappingProtocol")
	TCWebApi_set("VirServer_Entry_Entry","SrcIP","RemoteHost")
	TCWebApi_set("VirServer_Entry_Entry","STARTPORT","ExternalPort")
	TCWebApi_set("VirServer_Entry_Entry","ENDPORT","InternalPort")
	TCWebApi_set("VirServer_Entry_Entry","LOCALIP","InternalClient")
	TCWebApi_set("VirServer_Entry_Entry","MapName","PortMappingDescription")	
	TCWebApi_commit("VirServer_Entry_Entry")
	tcWebApi_save()
else
	TCWebApi_set("VirServer_Entry","Active","EnableVirServer_Flag")
	TCWebApi_set("VirServer_Entry","Protocal","PortMappingProtocol")
	TCWebApi_set("VirServer_Entry","SrcIP","RemoteHost")
	TCWebApi_set("VirServer_Entry","STARTPORT","ExternalPort")
	TCWebApi_set("VirServer_Entry","INTPORT","InternalPort")
	TCWebApi_set("VirServer_Entry","LOCALIP","InternalClient")
	TCWebApi_set("VirServer_Entry","MapName","PortMappingDescription")	
	TCWebApi_commit("VirServer_Entry")
	tcWebApi_save()
end if
elseif Request_Form("VirServer_Flag") = "2" then
if tcWebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") <> "Yes" then
	TCWebApi_set("WebCurSet_Entry","nat_pvc","VirServEntryCurIndex")	
	TCWebApi_set("WebCurSet_Entry","nat_ifidx","VirServEntryCurIndex")
	if Request_Form("hVirServEntry0") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry0")
			TCWebApi_unset("VirServer_Entry_Entry")		
	end if
	if Request_Form("hVirServEntry1") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry1")
			TCWebApi_unset("VirServer_Entry_Entry")	
	end if
	if Request_Form("hVirServEntry2") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry2")
			TCWebApi_unset("VirServer_Entry_Entry")		
	end if
	if Request_Form("hVirServEntry3") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry3")
			TCWebApi_unset("VirServer_Entry_Entry")		
	end if
	if Request_Form("hVirServEntry4") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry4")
			TCWebApi_unset("VirServer_Entry_Entry")
	end if
	if Request_Form("hVirServEntry5") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry5")
			TCWebApi_unset("VirServer_Entry_Entry")
	end if
	if Request_Form("hVirServEntry6") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry6")
			TCWebApi_unset("VirServer_Entry_Entry")		
	end if
	if Request_Form("hVirServEntry7") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry7")
			TCWebApi_unset("VirServer_Entry_Entry")
	end if	
	if Request_Form("hVirServEntry8") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry8")
			TCWebApi_unset("VirServer_Entry_Entry")	
	end if	
	if Request_Form("hVirServEntry9") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry9")
			TCWebApi_unset("VirServer_Entry_Entry")		
	end if	
else
	TCWebApi_set("WebCurSet_Entry","nat_ifidx","VirServEntryCurIndex")
	if Request_Form("hVirServEntry0") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry0")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry1") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry1")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry2") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry2")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry3") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry3")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry4") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry4")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry5") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry5")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry6") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry6")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if
	if Request_Form("hVirServEntry7") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry7")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if	
	if Request_Form("hVirServEntry8") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry8")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if	
	if Request_Form("hVirServEntry9") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry9")
		TCWebApi_set("VirServer_Entry","vDelEntry","vDelEntry")
			TCWebApi_unset("VirServer_Entry")		
	end if	
end if
	tcWebApi_save()
elseif Request_Form("VirServer_Flag") = "3" then
	TCWebApi_set("WebCurSet_Entry","nat_pvc","VirServEntryCurIndex")
	TCWebApi_set("WebCurSet_Entry","nat_ifidx","VirServEntryCurIndex")
	TCWebApi_commit("WebCurSet_Entry")
elseif Request_Form("VirServer_Flag") = "4" then
if tcWebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then
	TCWebApi_set("WebCurSet_Entry","nat_ifidx","VirServEntryCurIndex")
	if Request_Form("hVirServEntry0") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry0")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry0")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry1") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry1")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry1")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry2") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry2")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry2")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry3") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry3")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry3")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry4") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry4")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry4")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry5") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry5")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry5")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry6") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry6")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry6")
			TCWebApi_commit("VirServer_Entry")
	end if
	if Request_Form("hVirServEntry7") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry7")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry7")
			TCWebApi_commit("VirServer_Entry")
	end if	
	if Request_Form("hVirServEntry8") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry8")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry8")
			TCWebApi_commit("VirServer_Entry")
	end if	
	if Request_Form("hVirServEntry9") <> "99" then
		TCWebApi_set("WebCurSet_Entry","virServ_id","hVirServEntry9")
			TCWebApi_set("VirServer_Entry","Active","vEnableEntry9")
			TCWebApi_commit("VirServer_Entry")
	end if	
end if
	tcWebApi_save()
end if

if Request_Form("PortTriggering_SubmitFlag") = "1" Then
	TCWebApi_set("GUITemp_Entry0","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry0","TSPortTmp","PortTriggering_StartPort_0")
	TCWebApi_set("GUITemp_Entry0","TEPortTmp","PortTriggering_EndPort_0")
	TCWebApi_set("GUITemp_Entry0","TProtocolTmp","PortTriggering_Protocol_0")
	TCWebApi_set("GUITemp_Entry0","OSPortTmp","PortOpening_StartPort_0")
	TCWebApi_set("GUITemp_Entry0","OEPortTmp","PortOpening_EndPort_0")
	TCWebApi_set("GUITemp_Entry0","OProtocolTmp","PortOpening_Protocol_0")
	
	TCWebApi_set("GUITemp_Entry1","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry1","TSPortTmp","PortTriggering_StartPort_1")
	TCWebApi_set("GUITemp_Entry1","TEPortTmp","PortTriggering_EndPort_1")
	TCWebApi_set("GUITemp_Entry1","TProtocolTmp","PortTriggering_Protocol_1")
	TCWebApi_set("GUITemp_Entry1","OSPortTmp","PortOpening_StartPort_1")
	TCWebApi_set("GUITemp_Entry1","OEPortTmp","PortOpening_EndPort_1")
	TCWebApi_set("GUITemp_Entry1","OProtocolTmp","PortOpening_Protocol_1")
	
	TCWebApi_set("GUITemp_Entry2","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry2","TSPortTmp","PortTriggering_StartPort_2")
	TCWebApi_set("GUITemp_Entry2","TEPortTmp","PortTriggering_EndPort_2")
	TCWebApi_set("GUITemp_Entry2","TProtocolTmp","PortTriggering_Protocol_2")
	TCWebApi_set("GUITemp_Entry2","OSPortTmp","PortOpening_StartPort_2")
	TCWebApi_set("GUITemp_Entry2","OEPortTmp","PortOpening_EndPort_2")
	TCWebApi_set("GUITemp_Entry2","OProtocolTmp","PortOpening_Protocol_2")
	
	TCWebApi_set("GUITemp_Entry3","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry3","TSPortTmp","PortTriggering_StartPort_3")
	TCWebApi_set("GUITemp_Entry3","TEPortTmp","PortTriggering_EndPort_3")
	TCWebApi_set("GUITemp_Entry3","TProtocolTmp","PortTriggering_Protocol_3")
	TCWebApi_set("GUITemp_Entry3","OSPortTmp","PortOpening_StartPort_3")
	TCWebApi_set("GUITemp_Entry3","OEPortTmp","PortOpening_EndPort_3")
	TCWebApi_set("GUITemp_Entry3","OProtocolTmp","PortOpening_Protocol_3")
	
	TCWebApi_set("GUITemp_Entry4","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry4","TSPortTmp","PortTriggering_StartPort_4")
	TCWebApi_set("GUITemp_Entry4","TEPortTmp","PortTriggering_EndPort_4")
	TCWebApi_set("GUITemp_Entry4","TProtocolTmp","PortTriggering_Protocol_4")
	TCWebApi_set("GUITemp_Entry4","OSPortTmp","PortOpening_StartPort_4")
	TCWebApi_set("GUITemp_Entry4","OEPortTmp","PortOpening_EndPort_4")
	TCWebApi_set("GUITemp_Entry4","OProtocolTmp","PortOpening_Protocol_4")
	
	TCWebApi_set("GUITemp_Entry5","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry5","TSPortTmp","PortTriggering_StartPort_5")
	TCWebApi_set("GUITemp_Entry5","TEPortTmp","PortTriggering_EndPort_5")
	TCWebApi_set("GUITemp_Entry5","TProtocolTmp","PortTriggering_Protocol_5")
	TCWebApi_set("GUITemp_Entry5","OSPortTmp","PortOpening_StartPort_5")
	TCWebApi_set("GUITemp_Entry5","OEPortTmp","PortOpening_EndPort_5")
	TCWebApi_set("GUITemp_Entry5","OProtocolTmp","PortOpening_Protocol_5")
	
	TCWebApi_set("GUITemp_Entry6","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry6","TSPortTmp","PortTriggering_StartPort_6")
	TCWebApi_set("GUITemp_Entry6","TEPortTmp","PortTriggering_EndPort_6")
	TCWebApi_set("GUITemp_Entry6","TProtocolTmp","PortTriggering_Protocol_6")
	TCWebApi_set("GUITemp_Entry6","OSPortTmp","PortOpening_StartPort_6")
	TCWebApi_set("GUITemp_Entry6","OEPortTmp","PortOpening_EndPort_6")
	TCWebApi_set("GUITemp_Entry6","OProtocolTmp","PortOpening_Protocol_6")
	
	TCWebApi_set("GUITemp_Entry7","NameTmp","PortTriggering_Applications")
	TCWebApi_set("GUITemp_Entry7","TSPortTmp","PortTriggering_StartPort_7")
	TCWebApi_set("GUITemp_Entry7","TEPortTmp","PortTriggering_EndPort_7")
	TCWebApi_set("GUITemp_Entry7","TProtocolTmp","PortTriggering_Protocol_7")
	TCWebApi_set("GUITemp_Entry7","OSPortTmp","PortOpening_StartPort_7")
	TCWebApi_set("GUITemp_Entry7","OEPortTmp","PortOpening_EndPort_7")
	TCWebApi_set("GUITemp_Entry7","OProtocolTmp","PortOpening_Protocol_7")
	
	TCWebApi_commit("portTriggering")
	
	TCWebApi_unset("GUITemp_Entry0")
	TCWebApi_unset("GUITemp_Entry1")
	TCWebApi_unset("GUITemp_Entry2")
	TCWebApi_unset("GUITemp_Entry3")
	TCWebApi_unset("GUITemp_Entry4")
	TCWebApi_unset("GUITemp_Entry5")
	TCWebApi_unset("GUITemp_Entry6")
	TCWebApi_unset("GUITemp_Entry7")
  	TCWebApi_save()
end if

%>
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","NatConfigText","s")%></TITLE>
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
            value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>-><%tcWebApi_get("String_Entry","ContentAdvancedNATText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
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
      <SCRIPT language=JavaScript type=text/javascript>
	var VirServEntryIndex = 0;
	var VirSerIdleEntryIndex = 0;
	var VirSerIdleFlag = 0;
	var WanInfo = new Array();
	var nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryName = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryValue = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vEntryIndex = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vActive = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
	var vNATENABLE = vArrayStr.split(',');
	
	/*sort*/
	vEntryName = SortUtil.UpdateWanIFName( vEntryName, nEntryNum );
	vEntryValue = SortUtil.SortMyArray( vEntryValue );
	vEntryIndex = SortUtil.SortMyArray( vEntryIndex );
	vActive = SortUtil.SortMyArray( vActive );
	vNATENABLE = SortUtil.SortMyArray( vNATENABLE );
	
	nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","DmzCycleValue","s") %>";
	var vDMZActive = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","DmzCycleValue","s") %>";
	var vDMZIP = vArrayStr.split(',');
	vArrayStr = "<% tcWebApi_get("WanInfo_Common","AutoDMZ_Flag","s") %>";
	var vDMZAuto = vArrayStr.split(',');
	
	/*sort*/
	vDMZActive = SortUtil.SortMyArray( vDMZActive );
	vDMZIP = SortUtil.SortMyArray( vDMZIP );
	vDMZAuto = SortUtil.SortMyArray( vDMZAuto );
		
	for (i = 0; i < nEntryNum; i++)
	{
		WanInfo[i] = new stWanInfo(vEntryValue[i],vEntryName[i],vActive[i],vNATENABLE[i],vEntryName[i],vEntryName[i],vDMZActive[i],vDMZIP[i],vDMZAuto[i]);
	}
	
function stAlg(domain,L2TPEnable,IPSECEnable,H323Enable,RTSPEnable,SIPEnable,FTPEnable,PPTPEnable)
{
	this.domain = domain;
	if(L2TPEnable == "on")
		this.L2TPEnable = 1;
	else
		this.L2TPEnable = 0;
	if(IPSECEnable == "on")
		this.IPSECEnable = 1;
	else
		this.IPSECEnable = 0;
	if(H323Enable == "on")
		this.H323Enable = 1;
	else
		this.H323Enable = 0;
	if(RTSPEnable == "on")
		this.RTSPEnable = 1;
	else
		this.RTSPEnable = 0;
	if(SIPEnable == "on")
		this.SIPEnable = 1;
	else
		this.SIPEnable = 0;
	if(FTPEnable == "on")
		this.FTPEnable = 1;
	else
		this.FTPEnable = 0;	
	if(PPTPEnable == "on")
		this.PPTPEnable = 1;
	else
		this.PPTPEnable = 0;	
}
var algArray = 
new Array
(
	new stAlg("domain","<% tcWebApi_get("ALGSwitch_Entry","L2TPSW","s") %>","<% tcWebApi_get("ALGSwitch_Entry","IPSECSW","s") %>","<% tcWebApi_get("ALGSwitch_Entry","H323SW","s") %>","<% tcWebApi_get("ALGSwitch_Entry","RTSPSW","s") %>","<% tcWebApi_get("ALGSwitch_Entry","SIPSW","s") %>","<% tcWebApi_get("ALGSwitch_Entry","FTPSW","s") %>","<% tcWebApi_get("ALGSwitch_Entry","PPTPSW","s") %>"),
	null
);
var alg = algArray[0];
var WanIndex = -1;
function stWanInfo(domain,Name,Enable,NATEnabled,ConnectionType,ServiceList,DMZEnable,DMZIP,DMZAuto)
{
	this.domain = domain;
	this.key = this.domain;
	this.Name = Name;
	this.ConnectionType = ConnectionType;
	this.ServiceList = ServiceList;
	if(DMZAuto == "1")
		this.AutoDMZ_Flag = 1;
	else
		this.AutoDMZ_Flag = 0;
	if(DMZEnable == "Yes")
		this.DMZEnable = 1;
	else
		this.DMZEnable = 0;
	this.DMZIP = DMZIP;
	if(Enable == "Yes")
		this.Enable = 1;
	else
		this.Enable = 0;
	if(NATEnabled == "Enable")
		this.NATEnabled = 1;
	else
		this.NATEnabled = 0;		
	this.CntStatus = '';
}

function stPortMap(domain,ProtMapEnabled,Protocol,RemoteHost,ExPort,InPort,InClient,Description)
{
	domain = ""+domain;	
	this.domain = domain;
	this.key = this.domain;
	this.ProtMapEnabled = ProtMapEnabled;
	this.Protocol = Protocol;
	this.RemoteHost = RemoteHost;
	this.ExPort = ExPort;
	this.InPort = InPort;
	this.InClient = InClient;
	this.Description = Description;
	var index = domain.lastIndexOf('PortMapping');
	this.interface = domain.substr(0,index - 1);
	this.interface = domain;
	this.NATEnabled = '';
	this.Enable = '';
}

var PortMapping = new Array();
var indexPortMapping = 0;
var MAX_VIRSERVER_NUM = 10;

nEntryNum = <% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatActive = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatProtocal = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatSrcIP = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatSTARTPORT = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatENDPORT = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatLOCALIP = vArrayStr.split(',');
vArrayStr = "<% tcWebApi_get("WanInfo_Common","VirServerCycleValue","s") %>";
var vNatMapName = vArrayStr.split(',');

for (j = 0; j < MAX_VIRSERVER_NUM; j++)
{
	if(vNatActive[j] == "Yes" || vNatActive[j] == "No")
	{
		PortMapping[indexPortMapping] = new stPortMap(j,vNatActive[j],vNatProtocal[j],vNatSrcIP[j],vNatSTARTPORT[j],vNatENDPORT[j],vNatLOCALIP[j],vNatMapName[j]);
		indexPortMapping++;
	}
	else if(0 == VirSerIdleFlag){
		VirSerIdleEntryIndex = j;
		VirSerIdleFlag = 1;
	}
}

function hidesipPortInfo(hide)
{
	var status = 'visible';
	if ( hide == 1 )
	status = 'hidden';
	if (document.getElementById)  
	{
		document.getElementById('sipproxyport').style.visibility = status;
	}
	else
	{
		if (document.layers == false) 
		getElement('sipproxyport').style.visibility = status;
	}
}
function hideSiproxdInfo(hide) {
	var status = 'visible';
	if ( hide == 1 )
	status = 'hidden';
	if (document.getElementById)  
	{
		document.getElementById('sipproxdInfo').style.visibility = status;
		document.getElementById('applybutton').style.visibility = status;
	}
	else {
		if (document.layers) { 
			if ( hide == 1 )
				form1.Siproxd.value = '';
			else
				form1.Siproxd.value = enblSiproxd;
		}
		else  
		{
			getElement('sipproxdInfo').style.visibility = status;
			getElement('applybutton').style.visibility = status;
		}
	}
}
function dmzClick()
{
	if(getCheckVal('DMZEnable') == 0)
	{
		setText('DMZHostIPAddress', "");
		<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
		setCheck('AutoDMZEnable',0);
		document.dmzForm.AutoDMZEnable.disabled = true;
		<%End If%>
		document.dmzForm.DMZHostIPAddress.disabled = true;
	}
	else
	{	
		<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
		document.dmzForm.AutoDMZEnable.disabled = false;
		<%End If%>
		document.dmzForm.DMZHostIPAddress.disabled = false;
	}
}
function AutodmzClick()
{
	if(getCheckVal('AutoDMZEnable') == 0)
	{
		document.dmzForm.DMZHostIPAddress.disabled = false;
	}
	else
	{
		document.dmzForm.DMZHostIPAddress.disabled = true;
	}
}

function isOptionValid(portValue)
{
	if(isPlusInteger(portValue) == false)
	{
		alert('<%tcWebApi_get("String_Entry","NatPortIncorrectText","s")%>');
		return false;
	}
	if(isValidPort(portValue) == false)
	{
		alert('<%tcWebApi_get("String_Entry","NatPortIncorrectText","s")%>');
		return false;
	}
	return true;
}
function CheckForm(type)
{
	switch (type)
	{
	case 1:
		break;
	case 2:
		return CheckDMZ();
		break;
	case 3:
		return CheckPortMapping();
		break;
	case 4:
		with(getElById('PortMappingForm'))
		{
			var rml = getElById('rml');
			if (rml == null)
			{
				return false;
			}
		}
	}
	return true;
}
function CheckAlg()
{
	for (var i = 0; i < WanInfo.length; i++)
	{
		if (getSelectVal('SIPWanPath') == WanInfo[i].key)
		{
			break;
		}
	}
	if (isOptionValid(getValue('SIPPort')) == false)
	{
		alert('<%tcWebApi_get("String_Entry","NatSipPortIncorrectText","s")%>');
		return false;
	}
	if (getCheckVal('SIPEnable') == 1 && WanIndex > -1)
	{
		if (WanInfo[WanIndex].NATEnabled == 0 
		|| WanInfo[WanIndex].ConnectionType.indexOf('_B_')>-1)		
		{
			if (confirm('<%tcWebApi_get("String_Entry","NatSipCfgAttentionText","s")%>') == false)
			return false;
		}
	}
	return true;
}
function CheckDMZ()
{
	var i = 0;
	var bWANinterface = false;
	for (i = 0; i < WanInfo.length; i++)
	{
		if((WanInfo[i].ServiceList.indexOf('_TR069_')<0) || (WanInfo[i].ServiceList.indexOf('_INTERNET_')>-1))
		{
			bWANinterface = true;
		}
	}
	if (bWANinterface == false)
	{
		alert('<%tcWebApi_get("String_Entry","NatWanAddText","s")%>');
		return false;
	}
	with (getElById('dmzForm'))
	{
		<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
		if (getCheckVal('AutoDMZEnable') == 1)
			return true;
		<%End If%>
		if (getCheckVal('DMZEnable') == 1)
		{
			if (getValue('DMZHostIPAddress') == "")
			{
				alert('<%tcWebApi_get("String_Entry","NatDmzAddrJS0Text","s")%>');
				return false;
			}
			else if ((isAbcIpAddress(getValue('DMZHostIPAddress'))== false)||(isBroadcastIpAddress(getValue('DMZHostIPAddress'))==true)
			||(isNtwkSgmtIpAddress(getValue('DMZHostIPAddress'))==true))
			{
				alert('<%tcWebApi_get("String_Entry","NatDmzAddrJS1Text","s")%>');
				return false;
			}
		}
		else if (getCheckVal('DMZEnable') == 0 && getValue('DMZHostIPAddress') != "")
		{
			if ((isAbcIpAddress(getValue('DMZHostIPAddress'))== false)||(isBroadcastIpAddress(getValue('DMZHostIPAddress'))==true)
			||(isNtwkSgmtIpAddress(getValue('DMZHostIPAddress'))==true))
			{
				alert('<%tcWebApi_get("String_Entry","NatDmzAddrJS1Text","s")%>');
				return false;
			}
		}
	}
	return true;
}
function CheckPortMapping()
{
	if(VirServEntryIndex >= 10)
	{
		alert('<%tcWebApi_get("String_Entry","NatVirServerPortText","s")%>')
		return false;
	}
	if(PortMapping.length >= 32)
	{
		alert('<%tcWebApi_get("String_Entry","NatVirServerRuleText","s")%>')
		return false;
	}
	if (WanInfo.length == 0)
	{
		alert('<%tcWebApi_get("String_Entry","NatWanAddText","s")%>');
		return false;
	}
	with (getElById('PortMappingForm') )
	{
		if (RemoteHost.value != "")
		{
			if (isValidIpAddress(RemoteHost.value) == false)
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS0Text","s")%>');
				return false;
			}
			else if ((isLoopIpAddress(getValue('RemoteHost'))==true)||(isBroadcastIpAddress(getValue('RemoteHost'))==true)
			||(isNtwkSgmtIpAddress(getValue('RemoteHost'))==true))
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS1Text","s")%>');
				return false;
			}
		}
		if (ExternalPort.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS2Text","s")%>' );
			return false;
		}
		if (InternalPort.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS3Text","s")%>' );
			return false;
		}
		if (InternalClient.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS4Text","s")%>' );
			return false;
		}
		if (PortMappingDescription.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS5Text","s")%>' );
			return false;
		}
		if (isAbcIpAddress(InternalClient.value) == false ||(isBroadcastIpAddress(getValue('InternalClient'))==true)
		||(isNtwkSgmtIpAddress(getValue('InternalClient'))==true))
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS4Text","s")%>');
			return false;
		}
		if (isValidPort(ExternalPort.value) == false )
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS2Text","s")%>' );
			return false;
		}
		if (isValidPort(InternalPort.value) == false )
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS3Text","s")%>' );
			return false;
		}
		if (isValidName(PortMappingDescription.value) == false)
		{
			alert('<%tcWebApi_get("String_Entry","NatPortMappingJS6Text","s")%>');
			return false;
		}
		for (i = 0; i < PortMapping.length; i++)
		{
			if (PortMapping[i].Description == PortMappingDescription.value)
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS7Text","s")%>');
				return false;
			}
			if (PortMapping[i].Protocol == PortMappingProtocol.value
			&& PortMapping[i].ExPort == ExternalPort.value
			&& PortMapping[i].InPort == InternalPort.value
			&& PortMapping[i].RemoteHost == RemoteHost.value
			&& PortMapping[i].InClient == InternalClient.value)
			{
				alert('<%tcWebApi_get("String_Entry","NatPortMappingJS8Text","s")%>');
				return false;
			}
		}
		/*
		if ( "21" == getValue("ExternalPort"))
		{
			return confirm('<%tcWebApi_get("String_Entry","NatPortMappingJS9Text","s")%>');
		}
		else if ( "23" == getValue("ExternalPort") )
		{
			return confirm('<%tcWebApi_get("String_Entry","NatPortMappingJS10Text","s")%>');
		}
		else if ( "80" == getValue("ExternalPort") )
		{
			return confirm('<%tcWebApi_get("String_Entry","NatPortMappingJS11Text","s")%>');
		}
		*/
	}
	return true;
}
function RemovePortMapping(Form, CheckTag)
{
	var rml = getElById(CheckTag);
	var i;
	var cnt = 0;
	if (rml.length > 0)
	{
		cnt = 0;
		for (i = 0; i < rml.length; i++)
		{
			if (rml[i].checked == true)
			{
				cnt++;
				/*Form.addParameter(rml[i].value,'');*/
				var ctlName = "hVirServEntry"+i;
				setText(ctlName,rml[i].value);
			}
		}
		
		if (0 == cnt)
		{
			alert('<%tcWebApi_get("String_Entry","NatVirSerMappingError1Text","s")%>');
			return false;
		}
	}
	else if (rml.checked == true)
	{
		/*Form.addParameter(rml.value,'');*/
		var ctlName = "hVirServEntry0";
		setText(ctlName,rml.value);		
	}
}
function ModifyPortMappingEnable(Form, CheckTag, flag)
{
	var rml = getElById(CheckTag);
	var i, cnt;
	var enable;
	var ctlName;

	if (rml == null)
	{
		alert('<%tcWebApi_get("String_Entry","NatVirSerMappingErrorText","s")%>');
		return false;
	}
	
	if (1 == flag)
		enable = "Yes";
	else
		enable = "No";
	
	if (rml.length > 0)
	{
		cnt = 0;
		for (i = 0; i < rml.length; i++)
		{
			if (rml[i].checked == true)
			{
				cnt++;
				ctlName = "hVirServEntry"+i;
				setText(ctlName,rml[i].value);
				ctlName = "vEnableEntry"+i;
				setText(ctlName,enable);
			}
		}

		if (0 == cnt)
		{
			alert('<%tcWebApi_get("String_Entry","NatVirSerMappingError1Text","s")%>');
			return false;
		}
	}
	else if (rml.checked == true)
	{
		ctlName = "hVirServEntry0";
		setText(ctlName,rml.value);
		ctlName = "vEnableEntry0";
		setText(ctlName,enable);
	}
	else
	{
		alert('<%tcWebApi_get("String_Entry","NatVirSerMappingError1Text","s")%>');
		return false;
	}
	return true;
}

function ModifyVirServerClick(CheckTag)
{
	var rml = getElById(CheckTag);
	var i, cnt;
	
	if (rml == null)
	{
		alert('<%tcWebApi_get("String_Entry","NatVirSerMappingErrorText","s")%>');
		return false;
	}
		
	if (rml.length > 0)
	{
		cnt = 0;
		for (i = 0; i < rml.length; i++)
		{
			top.VirServerModifyInde = -1;
			if (rml[i].checked == true)
			{
				cnt++;
				if( cnt > 1)
				{
					alert('<%tcWebApi_get("String_Entry","NatPortMappingJS12Text","s")%>');
					return false;
				}
				top.VirServerModifyMappingIndex = i;
				top.VirServerModifyEntryIndex = rml[i].value;
				
			}
		}

		if (cnt == 0)
		{
			alert('<%tcWebApi_get("String_Entry","NatVirSerMappingError1Text","s")%>');
			return false;
		}
	}
	else if (rml.checked == true)
	{
		top.VirServerModifyMappingIndex = 0;
		top.VirServerModifyEntryIndex = rml.value;
	}
	else
	{
		alert('<%tcWebApi_get("String_Entry","NatVirSerMappingError1Text","s")%>');
		return false;
	}

	return true;
}

function LoadFrame()
{
<% if tcWebApi_get("WebCustom_Entry", "isProtTriggeringSupported", "h") = "Yes"  then %>
	ruleSetting(document.PORTTRIGGERING_form.PortTriggering_App_idx.selectedIndex);
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isCTFJSupported", "h") = "Yes"  then %>
<% if tcwebApi_get("WebCurSet_Entry","CurrentAccess","h") = "0" then %>
	setCheck('L2TPEnable',alg.L2TPEnable);
	setCheck('IPSECEnable',alg.IPSECEnable);
	setCheck('H323Enable',alg.H323Enable);
	setCheck('RTSPEnable',alg.RTSPEnable);
	setCheck('SIPEnable',alg.SIPEnable);
	setCheck('FTPEnable',alg.FTPEnable);
	setCheck('PPTPEnable',alg.PPTPEnable);
<% end if %>
<% else %>	
	setCheck('L2TPEnable',alg.L2TPEnable);
	setCheck('IPSECEnable',alg.IPSECEnable);
	setCheck('H323Enable',alg.H323Enable);
	setCheck('RTSPEnable',alg.RTSPEnable);
	setCheck('SIPEnable',alg.SIPEnable);
	setCheck('FTPEnable',alg.FTPEnable);
	setCheck('PPTPEnable',alg.PPTPEnable);
<% end if %>
	ShowDMZ();

	setSelect('PortMappingInterface',getEntryValue(<% tcWebApi_get("WebCurSet_Entry","nat_ifidx","s") %>));
	if(document.ConfigForm.isVirServerSupported.value == 1){	
	     ShowPortMapping();
        }
}
function ShowPortMapping()
{
<% if tcWebApi_get("WebCustom_Entry", "isCTJOYME2Supported", "h") = "Yes"  then %>
	setDisable('btn1',0);
<% else %>
	var Interface = getElById('PortMappingInterface');
	var index = Interface.selectedIndex;
	setDisable('btn1',1);
	if (index >0)
	{
		var PortMpId = Interface.options[index].id;
		index = PortMpId.substr(PortMpId.indexOf('_') + 1);
		if (Interface.length > 1)
		{
			if (WanInfo[index].Enable == 0 || WanInfo[index].NATEnabled == 0)
			{
				setDisable('btn1',1);
			}
			else
			{
				setDisable('btn1',0);
			}
		}
<% end if %>
		var html = '<table border="1" cellpadding="0" cellspacing="0" width="460">'
		+ '<tr align="middle">'
		+  '<td width="9%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingProtocolText","s")%></td>'
<% if tcWebApi_get("WebCustom_Entry", "isCTJOYME2Supported", "h") = "Yes"  then %>
		+  '<td width="16%" class="table_title"><%tcWebApi_get("String_Entry","NatVirSerTableExternalPortText","s")%></td>'
		+  '<td width="16%" class="table_title"><%tcWebApi_get("String_Entry","NatVirSerTableInternalPortText","s")%></td>'
<% else %>
		+  '<td width="13%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingSrcIPText","s")%></td>'
		+  '<td width="16%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingSPortNumText","s")%></td>'
		+  '<td width="16%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingEndPortNumText","s")%></td>'
<% end if %>
		+  '<td width="13%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingInterServText","s")%></td>'
		+  '<td width="15%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingNameText","s")%></td>'
		+  '<td width="10%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingEnbaleText","s")%></td>'
<% if tcWebApi_get("WebCustom_Entry", "isCTJOYME2Supported", "h") = "Yes"  then %>
		+  '<td width="10%" class="table_title"><%tcWebApi_get("String_Entry","NatVirSerTableCheckText","s")%></td>'
<% else %>
		+  '<td width="10%" class="table_title"><%tcWebApi_get("String_Entry","NatPortMappingDelText","s")%></td>'
<% end if %>
		+ '</tr>'
		VirServEntryIndex = 0;
		for (i = 0; i < PortMapping.length; i++)
		{
			/*if (PortMapping[i].domain.indexOf(getSelectVal('PortMappingInterface'))	> -1)*/
			{
				html += '<TR align="middle">'
				html +=  '<TD align="center">' + getDisplayText(PortMapping[i].Protocol,10) + '&nbsp;</TD>';
<% if tcWebApi_get("WebCustom_Entry", "isCTJOYME2Supported", "h") <> "Yes"  then %>
				html += '<TD align="center">' + getDisplayText(PortMapping[i].RemoteHost,8) + '&nbsp;</TD>';
<% end if %>
				html += '<TD align="center">' + getDisplayText(PortMapping[i].ExPort,5) + '&nbsp;</TD>';
				html += '<TD align="center">' + getDisplayText(PortMapping[i].InPort,5) + '&nbsp;</TD>';
				html += '<TD align="center">' + getDisplayText(PortMapping[i].InClient,10) + '&nbsp;</TD>';
				html += '<TD align="center">' + getDisplayText(PortMapping[i].Description,10) + '&nbsp;</TD>';
				if (PortMapping[i].ProtMapEnabled == "Yes")
				{
					html += '<TD align="center">' + '<%tcWebApi_get("String_Entry","NatPortMappingOpenText","s")%>' + '&nbsp;</TD>';
				}
				else
				{
					html += '<TD align="center">' + '<%tcWebApi_get("String_Entry","NatPortMappingCloseText","s")%>' + '&nbsp;</TD>';
				}
				html += '<TD align="center"><input type="checkbox" name="rml" value="'
				+ PortMapping[i].domain + '"></TD>';
				html += '</TR>';
				VirServEntryIndex++;
			}
		}
		html += '</table>'
		getElById('portMappingInfo').innerHTML = html;
<% if tcWebApi_get("WebCustom_Entry", "isCTJOYME2Supported", "h") <> "Yes"  then %>
	}
<% end if %>
}
TOTAL_APP = 14;
FIRST_APP = "<%tcWebApi_get("String_Entry","NatSelectText","s")%>";
var v = new Array(TOTAL_APP);
v[0] = new cV("Domain Name Server",1);
v[0].e[0] = new iVe("53", "53", "2", "53", "53");
v[1] = new cV("FTP Server",1);
v[1].e[0] = new iVe("21", "21", "1", "21", "21");
v[2] = new cV("IPSEC",1);
v[2].e[0] = new iVe("500", "500", "2", "500", "500");
v[3] = new cV("Mail POP3",1);
v[3].e[0] = new iVe("110", "110", "1", "110", "110");
v[4] = new cV("Mail SMTP",1);
v[4].e[0] = new iVe("25", "25", "1", "25", "25");
v[5] = new cV("PPTP",1);
v[5].e[0] = new iVe("1723", "1723", "1", "1723", "1723");
v[6] = new cV("Real Player 8 Plus",1);
v[6].e[0] = new iVe("7070", "7070", "2", "7070", "7070");
v[7] = new cV("Secure Shell Server",1);
v[7].e[0] = new iVe("22", "22", "1", "22", "22");
v[8] = new cV("Secure Web Server HTTPS",1);
v[8].e[0] = new iVe("443", "443", "1", "443", "443");
v[9] = new cV("SNMP",1);
v[9].e[0] = new iVe("161", "161", "2", "161", "161");
v[10] = new cV("SNMP Trap",1);
v[10].e[0] = new iVe("162", "162", "2", "162", "162");
v[11] = new cV("Telnet Server",1);
v[11].e[0] = new iVe("23", "23", "1", "23", "23");
v[12] = new cV("TFTP",1);
v[12].e[0] = new iVe("69", "69", "2", "69", "69");
v[13] = new cV("Web Server HTTP",1);
v[13].e[0] = new iVe("80", "80", "1", "80", "80");
function cV(name, entryNum)
{
	this.name = name;
	this.eNum = entryNum;
	this.e = new Array(5);
}
function iVe(eStart, eEnd, proto, iStart, iEnd)
{
	this.eStart = eStart;
	this.eEnd = eEnd;
	this.proto = proto;
	this.iStart = iStart;
	this.iEnd = iEnd;
}
function clearAll()
{
}
function trimspace(str)
{
	var strTemp = new String(str);
	while (-1 != strTemp.search(" "))
	{
		strTemp = strTemp.replace(" ", "");
	}
	return strTemp;
}
function appSelect(sName)
{
	clearAll();
	with (getElById('PortMappingForm'))
	{
		if (sName == FIRST_APP)
		{
			return;
		}
		for(i = 0; i < TOTAL_APP; i++)
		{
			if(v[i].name == sName)
			{
				switch (v[i].e[0].proto)
				{
				case '0':
					setSelect('PortMappingProtocol','TCP/UDP');
					break;
				case '1':
					setSelect('PortMappingProtocol','TCP');
					break;
				case '2':
					setSelect('PortMappingProtocol','UDP');
					break;
				}
				RemoteHost.value = "";
				ExternalPort.value = v[i].e[0].eStart;
<% if tcWebApi_get("WebCustom_Entry", "isCTJOYME2Supported", "h") <> "Yes"  then %>
				InternalPort.value = v[i].e[0].iStart;
<% end if %>
				PortMappingDescription.value = trimspace(sName);
			}
		}
	}
}
function radioClick()
{
	if (document.PortMappingForm.radiosrv[0].checked == true)
	{
		with(document.PortMappingForm)
		{
			constsrvName.disabled = true;
		}
	}
	else
	{
		with(document.PortMappingForm)
		{
			constsrvName.disabled = false;
		}
	}
}

function setALGSwitch()
{
	var Form = document.algForm;
	if(Form.L2TPEnable.checked)
		Form.L2TPSW_Flag.value = "on";
	else 
		Form.L2TPSW_Flag.value = "off";
	
	if(Form.IPSECEnable.checked)
		Form.IPSecSW_Flag.value = "on";
	else 
		Form.IPSecSW_Flag.value = "off";
	
	if(Form.H323Enable.checked)
		Form.H323SW_Flag.value = "on";
	else 
		Form.H323SW_Flag.value = "off";
	
	if(Form.RTSPEnable.checked)
		Form.RTSPSW_Flag.value = "on";
	else 
		Form.RTSPSW_Flag.value = "off";
	
	if(Form.SIPEnable.checked)
		Form.SIPSW_Flag.value = "on";
	else 
		Form.SIPSW_Flag.value = "off";
	
	if(Form.FTPEnable.checked)
		Form.FTPSW_Flag.value = "on";
	else 
		Form.FTPSW_Flag.value = "off";
	
	if(Form.PPTPEnable.checked)
		Form.PPTPSW_Flag.value = "on";
	else 
		Form.PPTPSW_Flag.value = "off";
}
function getEntryIndex(ctl)
{
	for (i = 0; i < nEntryNum; i++)
	{
		if (getSelectVal(ctl) == vEntryValue[i])
			return vEntryIndex[i];
	}
	return 999;
}
function getEntryValue(idx)
{
	for (i = 0; i < nEntryNum; i++)
	{
		if (idx == vEntryIndex[i])
			return vEntryValue[i];
	}
	return 999;
}
function btnSubmit(iMode)
{
	if(CheckForm(iMode) == false)
		return;
	if(iMode == 1)/*alg*/
	{
		setALGSwitch();
		document.algForm.alg_Flag.value = "1";
		if( true == setEBooValueCookie(document.algForm) )
			document.algForm.submit();
	}
	else if(iMode == 2)/*dmz*/
	{
		if(getCheckVal('DMZEnable') == 0)
			document.dmzForm.EnableDMZ_Flag.value = "No";
		else 
			document.dmzForm.EnableDMZ_Flag.value = "Yes";
		
		<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>	
		if(getCheckVal('AutoDMZEnable') == 0)
			document.dmzForm.AutoDMZ_Flag.value = "0";
		else 
			document.dmzForm.AutoDMZ_Flag.value = "1";
		<%End If%>	
		document.dmzForm.DMZ_Flag.value = "1";
		document.dmzForm.DMZEntryCurIndex.value = getEntryIndex('DMZInterface');
		if( true == setEBooValueCookie(document.dmzForm) )
			document.dmzForm.submit();
	}
	else if(iMode == 3)/*add*/
	{
		document.PortMappingForm.VirServCurIndex.value = VirSerIdleEntryIndex;
		document.PortMappingForm.VirServer_Flag.value = "1";
		document.PortMappingForm.VirServEntryCurIndex.value = getEntryIndex('PortMappingInterface');
		if( true == setEBooValueCookie(document.PortMappingForm) )
			document.PortMappingForm.submit();
	}
	else if(iMode == 4)/*del*/
	{
		if(false == RemovePortMapping(PortMappingForm,'rml'))
			return;
		document.PortMappingForm.VirServCurIndex.value = VirServEntryIndex;
		document.PortMappingForm.VirServer_Flag.value = "2";
		document.PortMappingForm.VirServEntryCurIndex.value = getEntryIndex('PortMappingInterface');
		if( true == setEBooValueCookie(document.PortMappingForm) )
			document.PortMappingForm.submit();
	}
	else if(iMode == 5)/*changeIF*/
	{
		document.PortMappingForm.VirServer_Flag.value = "3";
		document.PortMappingForm.VirServEntryCurIndex.value = getEntryIndex('PortMappingInterface');
		if( true == setEBooValueCookie(document.PortMappingForm) )
			document.PortMappingForm.submit();
	}	
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %>
	else if(iMode == 6)//open
	{
		if ( false == ModifyPortMappingEnable(PortMappingForm, 'rml', 1) )
			return;
		document.PortMappingForm.VirServCurIndex.value = VirServEntryIndex;
		document.PortMappingForm.VirServer_Flag.value = "4";
		document.PortMappingForm.VirServEntryCurIndex.value = getEntryIndex('PortMappingInterface');
		if( true == setEBooValueCookie(document.PortMappingForm) )
			document.PortMappingForm.submit();
	}
	else if(iMode == 7)//close
	{
		if ( false == ModifyPortMappingEnable(PortMappingForm, 'rml', 0) )
			return;
		document.PortMappingForm.VirServCurIndex.value = VirServEntryIndex;
		document.PortMappingForm.VirServer_Flag.value = "4";
		document.PortMappingForm.VirServEntryCurIndex.value = getEntryIndex('PortMappingInterface');
		if( true == setEBooValueCookie(document.PortMappingForm) )
			document.PortMappingForm.submit();
	}
	else if(iMode == 8)//modify
			{
		if ( false == ModifyVirServerClick('rml') )
		{
		return;
	}
	location.replace('app-modifyVirServer.asp');
}
<% end if %>
}

function ShowDMZ()
{
	var Interface = getElById('DMZInterface');
	var index = Interface.selectedIndex;
	if (index >=0)
	{
		var DMZId = Interface.options[index].id;
		index = DMZId.substr(DMZId.indexOf('_') + 1);
		if ((index < 0) || (index >= WanInfo.length))
		{
			return;
		}
		setCheck('DMZEnable',WanInfo[index].DMZEnable);
		<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
		setCheck('AutoDMZEnable',WanInfo[index].AutoDMZ_Flag);
		<%End If%>
		setText('DMZHostIPAddress',WanInfo[index].DMZIP);
		if (WanInfo[index].Enable == 0 || WanInfo[index].NATEnabled == 0)
		{
			setDisable('DMZEnable',1);
			<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
			setDisable('AutoDMZEnable',1);
			<%End If%>
			setDisable('DMZHostIPAddress',1);	
		}
		else
		{
			setDisable('DMZEnable',0);
			<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
			setDisable('AutoDMZEnable',0);
			<%End If%>
			setDisable('DMZHostIPAddress',0);
			
			if(WanInfo[index].DMZEnable)
			{	
				document.dmzForm.DMZHostIPAddress.disabled = false;
				<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
				document.dmzForm.AutoDMZEnable.disabled = false;
				if(WanInfo[index].AutoDMZ_Flag)
				{
					document.dmzForm.DMZHostIPAddress.disabled = true;
				}
				else
				{
					document.dmzForm.DMZHostIPAddress.disabled = false;
				}
				<%End If%>
			}
			else
			{
				setText('DMZHostIPAddress', "");
				<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
				document.dmzForm.AutoDMZEnable.disabled = true;
				<%End If%>
				document.dmzForm.DMZHostIPAddress.disabled = true;
			}			
		}
	}
}
function clearAll()
{
}

var ApplicationName = ["-" , "AimTalk" , "Asheron's Call" , "CalistaIPPhone" , "DeltaForce" , "ICQ" , "Napster" , "Net2Phone" , "QuickTimeClient" , "RainBow 6 Game" ];
var defaultArray = [
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var AimTalkArray = [
					[1 , 4099 ,4099 , 1 , 5191 , 5191 , 1],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var ACallArray = [
					[1 , 9000 ,9013 , 2 , 9000 , 9013 , 2],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var CalistaIPPhoneArray = [
					[1 , 5190 ,5190 , 1 , 3000 , 3000 , 1],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var DeltaForceArray = [
					[1 , 3568 ,3568 , 2 , 3100 , 3999 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var ICQArray = [
					[1 , 4000 ,4000 , 2 , 20000 , 20059 , 1],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var NapsterArray = [
					[1 , 6699 ,6699 , 1 , 6699 , 6699 , 1],
					[1 , 6699 ,6699 , 1 , 6697 , 6697 , 1],
					[1 , 6699 ,6699 , 1 , 4444 , 4444 , 1],
					[1 , 6699 ,6699 , 1 , 5555 , 5555 , 1],
					[1 , 6699 ,6699 , 1 , 6666 , 6666 , 1],
					[1 , 6699 ,6699 , 1 , 7777 , 7777 , 1],
					[1 , 6699 ,6699 , 1 , 8888 , 8888 , 1],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var Net2PhoneArray = [
					[1 , 6801 ,6801 , 2 , 6801 , 6801 , 2],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var QuickTimeArray = [
					[1 , 554 ,554 , 1 , 6990 , 32000 , 1],
					[1 , 554 ,554 , 1 , 6970 , 7000 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
var RainBow6GameArray = [
					[1 , 2346 ,2346 , 1 , 2346 , 2438 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0],
					[0 , 0 ,0 , 0 , 0 , 0 , 0]					
];
function RuleTableSetting(ApplicationArray)
{
	var i = 0;
	for(i = 0; i < 8; i++)
	{
		if(1 == ApplicationArray[i][0])
		{
			document.PORTTRIGGERING_form.elements[i*7+3].checked = true;	
		}
		else
		{
			document.PORTTRIGGERING_form.elements[i*7+3].checked = false;
		}

		document.PORTTRIGGERING_form.elements[i*7+4].value = ApplicationArray[i][1];
		document.PORTTRIGGERING_form.elements[i*7+5].value = ApplicationArray[i][2];
		document.PORTTRIGGERING_form.elements[i*7+6].selectedIndex = ApplicationArray[i][3];
		document.PORTTRIGGERING_form.elements[i*7+7].value = ApplicationArray[i][4];	
		document.PORTTRIGGERING_form.elements[i*7+8].value = ApplicationArray[i][5];
		document.PORTTRIGGERING_form.elements[i*7+9].selectedIndex = ApplicationArray[i][6];
	}
}

function ruleSetting(selectIndex)
{
	document.PORTTRIGGERING_form.PortTriggering_Applications.value = ApplicationName[selectIndex];
	switch(selectIndex)
	{
		case 0:
			RuleTableSetting(defaultArray);
			break;
		case 1:
			RuleTableSetting(AimTalkArray);
			break;
		case 2:
			RuleTableSetting(ACallArray);
			break;
		case 3:
			RuleTableSetting(CalistaIPPhoneArray);
			break;
		case 4:
			RuleTableSetting(DeltaForceArray);
			break;
		case 5:
			RuleTableSetting(ICQArray);
			break;
		case 6:
			RuleTableSetting(NapsterArray);
			break;
		case 7:
			RuleTableSetting(Net2PhoneArray);
			break;
		case 8:
			RuleTableSetting(QuickTimeArray);
			break;
		case 9:
			RuleTableSetting(RainBow6GameArray);
			break;
	}
}

function doAppChange(selectIndex) 
{
	ruleSetting(selectIndex);
}

function chkRuleLimit()
{

	var free_rule = 0;
	var input_rule = 0;
	
<%if tcWebApi_get("portTriggering_Entry0","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry1","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry2","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry3","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry4","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry5","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry6","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>

<%if tcWebApi_get("portTriggering_Entry7","TSPort","h") = "N/A" then%>
	free_rule += 1;
<%end if%>


	for(i = 0; i < 8; i++)
	{
		if(document.PORTTRIGGERING_form.elements[i*7+3].checked)
			input_rule++;
	}

	if (free_rule >= input_rule)
		return false;

	return true;
}

function chkPortRange()
{
	var number = /^[0-9]/;

	active = 0;
	for(i = 0; i < 8; i++)
	{
		if(document.PORTTRIGGERING_form.elements[i*7+3].checked)
		{
			active++;
			if(isNaN(document.PORTTRIGGERING_form.elements[i*7+4].value)|| 
			isNaN(document.PORTTRIGGERING_form.elements[i*7+5].value)||
			isNaN(document.PORTTRIGGERING_form.elements[i*7+7].value)|| 
			isNaN(document.PORTTRIGGERING_form.elements[i*7+8].value))
			{
				alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS7Text","s")%>");return true;
			}	
		}
		else
			continue;
		
		port1 = parseInt(document.PORTTRIGGERING_form.elements[i*7+4].value);
		port2 = parseInt(document.PORTTRIGGERING_form.elements[i*7+5].value);
		port3 = parseInt(document.PORTTRIGGERING_form.elements[i*7+7].value);
		port4 = parseInt(document.PORTTRIGGERING_form.elements[i*7+8].value);
		
		if (port1 > 65535 || port1 < 1 || port2 > 65535 || port2 < 1 ||
		port3 > 65535 || port3 < 1 || port4 > 65535 || port4 < 1)
		{
			alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS0Text","s")%>");return true;
		}
		if (port2 < port1 || port4 < port3)
		{
			alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS1Text","s")%>");return true;
		}
		else if((document.PORTTRIGGERING_form.elements[i*7+4].value.match(number) == null) ||
				(document.PORTTRIGGERING_form.elements[i*7+5].value.match(number) == null) ||
				(document.PORTTRIGGERING_form.elements[i*7+7].value.match(number) == null) ||
				(document.PORTTRIGGERING_form.elements[i*7+8].value.match(number) == null))
		{
			alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS7Text","s")%>");return true;
		}
	}
	
	if(!active)
	{
		alert("<%tcWebApi_get("String_Entry","NatPortTriggerlistJS0Text","s")%>");
		return true;
	}
	
	if(active > document.PORTTRIGGERING_form.PortTriggering_CanUseNumFlag.value)
	{
		alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS3Text","s")%>");
		return true;
	}
	return false;
}

function quotationCheck(object, limit_len) 
{
	var len = object.value.length;
	var c;
	var i, j = 0;
	if(!len)
	{
		alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS4Text","s")%>");
		return true;
	}
	for (i = 0; i < len; i++)
	{
		var c = object.value.charAt(i);
		if (c == '"')
		{
			j += 6;
		}
		else
		{
			j++;	
		}

	}
	
	if (j > limit_len)
	{
		alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS5Text","s")%>");return true;
	}
	return false;
}

function doSave() 
{
	if (chkPortRange())
		return;
	 if (chkRuleLimit())
	 {
	 	alert("<%tcWebApi_get("String_Entry","NatPortTriggerJS6Text","s")%>");
	 	return;
	 }	
	if (quotationCheck(document.PORTTRIGGERING_form.PortTriggering_Applications, 15) )
		return;
	document.PORTTRIGGERING_form.PortTriggering_SubmitFlag.value = 1;

	document.PORTTRIGGERING_form.PortTriggering_App_idx.disabled = true;
	if( true == setEBooValueCookie(document.PORTTRIGGERING_form) )
	{			
		document.PORTTRIGGERING_form.submit();
	}
}

function doLoad() 
{
	ruleSetting(document.PORTTRIGGERING_form.PortTriggering_App_idx.selectedIndex);
	return;
}

function onClickPortTriggeringSummary(filename)
		{				
			location.replace(filename);
		}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY id="alg_dmz">  
        <%if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes"  then %>
        <SCRIPT language=JavaScript type=text/javascript>
					if (curUserName != sptUserName)
						getElement("alg_dmz").style.display = "none";
				</SCRIPT>
        <% end if %>      	 
<% if tcWebApi_get("WebCustom_Entry", "isCTFJSupported", "h") = "Yes"  then %>
<% if tcwebApi_get("WebCurSet_Entry","CurrentAccess","h") = "0" then %>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NatAlgCfgText","s")%> </P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
            rowSpan=20><TABLE cellSpacing=0 cellPadding=20 width="100%" 
border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#高级NAT配置" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0> </A></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
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
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <FORM id=algForm name="algForm" action="/cgi-bin/app-natset.asp" method="post">
                  <DIV id=pptp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=L2TPEnable name=L2TPEnable> <%tcWebApi_get("String_Entry","NatEnbL2TPText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=ipsec>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=IPSECEnable name=IPSECEnable> <%tcWebApi_get("String_Entry","NatEnbIPSecText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=h323>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=H323Enable name=H323Enable> <%tcWebApi_get("String_Entry","NatEnbH323Text","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=rtsp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=RTSPEnable name=RTSPEnable> <%tcWebApi_get("String_Entry","NatEnbRTSPText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=sipproxdInfo>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=SIPEnable name=SIPEnable> <%tcWebApi_get("String_Entry","NatEnbSIPText","s")%> 
                  ALG</TD></TR><BR></TBODY></TABLE></DIV>
                      <input type="hidden" id="L2TPSW_Flag" name="L2TPSW_Flag" value="0">
                      <input type="hidden" id="IPSecSW_Flag" name="IPSecSW_Flag" value="0">
                      <input type="hidden" id="H323SW_Flag" name="H323SW_Flag" value="0">
                      <input type="hidden" id="RTSPSW_Flag" name="RTSPSW_Flag" value="0">
                      <input type="hidden" id="SIPSW_Flag" name="SIPSW_Flag" value="0">
                      <input type="hidden" id="FTPSW_Flag" name="FTPSW_Flag" value="0">
                      <input type="hidden" id="alg_Flag" name="alg_Flag" value="0">
                      <input type="hidden" id="PPTPSW_Flag" name="PPTPSW_Flag" value="0">
                  <DIV id=ftp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=FTPEnable name=FTPEnable> <%tcWebApi_get("String_Entry","NatEnbFTPText","s")%> 
                    ALG</TD>
                      </TR><BR></TBODY></TABLE>
                      </DIV>
                  <DIV id=pptp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=PPTPEnable name=PPTPEnable> <%tcWebApi_get("String_Entry","NatEnbPPTPText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                    </FORM>
                  <CENTER>
                  <DIV id=applybutton><INPUT onclick="btnSubmit(1)" type=button id=button3 value="<%tcWebApi_get("String_Entry","NatBtnSaveText","s")%>" name=button3> 
                  </DIV></CENTER>
                  <P></P></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
<% end if %>
<% else %>	
		<TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NatAlgCfgText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif 
            rowSpan=20><TABLE cellSpacing=0 cellPadding=20 width="100%" 
border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#高级NAT配置" 
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
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <FORM id=algForm name="algForm" action="/cgi-bin/app-natset.asp" method="post">
                  <DIV id=pptp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=L2TPEnable name=L2TPEnable> <%tcWebApi_get("String_Entry","NatEnbL2TPText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=ipsec>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=IPSECEnable name=IPSECEnable> <%tcWebApi_get("String_Entry","NatEnbIPSecText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=h323>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox name=H323Enable> <%tcWebApi_get("String_Entry","NatEnbH323Text","s")%>
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=rtsp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=RTSPEnable name=RTSPEnable> <%tcWebApi_get("String_Entry","NatEnbRTSPText","s")%>
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                  <DIV id=sipproxdInfo>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=SIPEnable name=SIPEnable> <%tcWebApi_get("String_Entry","NatEnbSIPText","s")%>
                  ALG</TD></TR><BR></TBODY></TABLE></DIV>
                      <input type="hidden" id="L2TPSW_Flag" name="L2TPSW_Flag" value="0">
                      <input type="hidden" id="IPSecSW_Flag" name="IPSecSW_Flag" value="0">
                      <input type="hidden" id="H323SW_Flag" name="H323SW_Flag" value="0">
                      <input type="hidden" id="RTSPSW_Flag" name="RTSPSW_Flag" value="0">
                      <input type="hidden" id="SIPSW_Flag" name="SIPSW_Flag" value="0">
                      <input type="hidden" id="FTPSW_Flag" name="FTPSW_Flag" value="0">
                      <input type="hidden" id="alg_Flag" name="alg_Flag" value="0">
                      <input type="hidden" id="PPTPSW_Flag" name="PPTPSW_Flag" value="0">
                  <DIV id=ftp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=FTPEnable name=FTPEnable> <%tcWebApi_get("String_Entry","NatEnbFTPText","s")%> 
                    ALG</TD>
                      </TR><BR></TBODY></TABLE>
                      </DIV>
                  <DIV id=pptp>
                  <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD><INPUT type=checkbox id=PPTPEnable name=PPTPEnable> <%tcWebApi_get("String_Entry","NatEnbPPTPText","s")%> 
                    ALG</TD></TR><BR></TBODY></TABLE></DIV>
                    </FORM>
                  <CENTER>
                  <DIV id=applybutton><INPUT onclick="btnSubmit(1)" type=button value="<%tcWebApi_get("String_Entry","NatBtnSaveText","s")%>" name=button3> 
                  </DIV></CENTER>
                  <P></P></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
<% end if %>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NatDmzCfgText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD></TR>
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
                  <FORM id=dmzForm name="dmzForm" action="/cgi-bin/app-natset.asp" method="post">
                      <%tcWebApi_get("String_Entry","NatDmzInputText","s")%> 
                      <input type="hidden" id="EnableDMZ_Flag" name="EnableDMZ_Flag" value="0">
                      <input type="hidden" id="DMZ_Flag" name="DMZ_Flag" value="0">
                      <input type="hidden" id="DMZEntryCurIndex" name="DMZEntryCurIndex" value="0">
					  <input type="hidden" id="AutoDMZ_Flag" name="AutoDMZ_Flag" value="0">
					  <input type="hidden" id="isAutoDMZSupported" name="isAutoDMZSupported" value="<%if tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" then asp_write("1") else asp_write("0") end if%>">
                      <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatInterFaceNameText","s")%> </TD>
                      <TD><SELECT id=DMZInterface onchange=ShowDMZ() 
                        name=DMZInterface maxlength="15"> 
			<SCRIPT language=JavaScript type=text/javascript>
			var dmzidx = <% tcWebApi_get("WebCurSet_Entry","dmz_ifidx","s") %>;
			var sel = '';
			for (i = 0; i < nEntryNum; i++)
			{
				if ( i == 0 && dmzidx == 999 )
					sel = 'selected';
				else
					sel = ( dmzidx == vEntryIndex[i] ) ? 'selected' : '';
				<% if tcwebApi_get("WebCustom_Entry","isCTPONCZGDSupported","h") = "Yes" then %>
				if((vEntryName[i].indexOf('_TR069_')<0 && vEntryName[i].indexOf('_INTERNET_')>-1) && (vEntryName[i].indexOf('_R_')>-1))
				<%else%>
				if((vEntryName[i].indexOf('_TR069_')<0) || (vEntryName[i].indexOf('_INTERNET_')>-1))
				<%end if%>
				{
					document.write('<option value=' + vEntryValue[i] + ' id=dmz_' + i + '  ' + sel + '>'
					+ vEntryName[i] + '</option>');
				}
			}
			</SCRIPT>
                        </SELECT> </TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatDmzEnableText","s")%> </TD>
                      <TD><INPUT onclick=dmzClick() type=checkbox 
                        id=DMZEnable name=DMZEnable></TD></TR>
					<%If tcWebApi_get("Dmz_Common","isAutoDMZSupported","h") = "Yes" Then%>
						<TR>
                            <TD width=120>AUTO DMZ：</TD>
                            <TD><input onClick=AutodmzClick() type=checkbox 
                        id=AutoDMZEnable name=AutoDMZEnable > 
                            </TD>
                          </TR>
					<%End If%>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatDmzLanIpText","s")%> </TD>
                      <TD><INPUT maxLength=15 
                    id=DMZHostIPAddress name=DMZHostIPAddress></TD></TR></TBODY></TABLE></FORM><BR>
                  <CENTER><INPUT onclick="btnSubmit(2)" type=button id=button value="<%tcWebApi_get("String_Entry","NatBtnSaveText","s")%>" name=button> 
                  </CENTER>
                  <P></P></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
         <%if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes"  then %> 	
         </TBODY>        
         <TBODY>
         <% end if %> 		
<% if tcWebApi_get("Info_Ether","isVirServerSupported","h")="Yes" then %>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NatVirSerCfgText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
           <%if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes"  then %>          
           <TD vAlign=top width=170 background=/img/panel4.gif 
            rowSpan=20><TABLE id='nat_help' cellSpacing=0 cellPadding=20 width="100%" 
border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#高级NAT配置" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0> </A></TD></TR>
              </TBODY></TABLE></TD>
              <SCRIPT language=JavaScript type=text/javascript>
								if (curUserName == sptUserName)
									getElement("nat_help").style.display = "none";
							</SCRIPT>           
           <% end if%>          
          </TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <FORM id=PortMappingForm name="PortMappingForm" action="/cgi-bin/app-natset.asp" method="post">
                  <DIV id=natpm>
	                <input type="hidden" id="VirServer_Flag" name="VirServer_Flag" value="0"> 
	                <input type="hidden" id="EnableVirServer_Flag" name="EnableVirServer_Flag" value="Yes">
	                <input type="hidden" id="VirServEntryCurIndex" name="VirServEntryCurIndex" value="0">
	                <input type="hidden" id="VirServCurIndex" name="VirServCurIndex" value="0">
	                <input type="hidden" id="hVirServEntry0" name="hVirServEntry0" value="99">
	                <input type="hidden" id="hVirServEntry1" name="hVirServEntry1" value="99">
	                <input type="hidden" id="hVirServEntry2" name="hVirServEntry2" value="99">
	                <input type="hidden" id="hVirServEntry3" name="hVirServEntry3" value="99">
	                <input type="hidden" id="hVirServEntry4" name="hVirServEntry4" value="99">
	                <input type="hidden" id="hVirServEntry5" name="hVirServEntry5" value="99">
	                <input type="hidden" id="hVirServEntry6" name="hVirServEntry6" value="99">
	                <input type="hidden" id="hVirServEntry7" name="hVirServEntry7" value="99">
	                <input type="hidden" id="hVirServEntry8" name="hVirServEntry8" value="99">
	                <input type="hidden" id="hVirServEntry9" name="hVirServEntry9" value="99">	                
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %>
 									<input type="hidden" id="vDelEntry" name="vDelEntry" value="0">
	                <input type="hidden" id="vEnableEntry0" name="vEnableEntry0" value="N/A">
	                <input type="hidden" id="vEnableEntry1" name="vEnableEntry1" value="N/A">
	                <input type="hidden" id="vEnableEntry2" name="vEnableEntry2" value="N/A">
	                <input type="hidden" id="vEnableEntry3" name="vEnableEntry3" value="N/A">
	                <input type="hidden" id="vEnableEntry4" name="vEnableEntry4" value="N/A">
	                <input type="hidden" id="vEnableEntry5" name="vEnableEntry5" value="N/A">
	                <input type="hidden" id="vEnableEntry6" name="vEnableEntry6" value="N/A">
	                <input type="hidden" id="vEnableEntry7" name="vEnableEntry7" value="N/A">
	                <input type="hidden" id="vEnableEntry8" name="vEnableEntry8" value="N/A">
	                <input type="hidden" id="vEnableEntry9" name="vEnableEntry9" value="N/A">
<%end if%>
                 <TABLE cellSpacing=0 cellPadding=0 border=0>
                    <TBODY>
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %>
                    <TR style="DISPLAY: none">
<%else%>				
                    <TR>
<%end if%>
                              <TD width=120><%tcWebApi_get("String_Entry","NatInterFaceNameText","s")%> </TD>
                      <TD><SELECT onchange=btnSubmit(5) size=1 
                        id=PortMappingInterface name=PortMappingInterface> 
			<SCRIPT language=JavaScript type=text/javascript>
			document.write('<option value="999">' + '' + '</option>');
			for (i = 0; i < nEntryNum; i++)
			{
				<% if tcwebApi_get("WebCustom_Entry","isCTPONCZGDSupported","h") = "Yes" then %>
				if((vEntryName[i].indexOf('_TR069_')<0 && vEntryName[i].indexOf('_INTERNET_')>-1) && (vEntryName[i].indexOf('_R_')>-1))
				<%else%>				
				if((vEntryName[i].indexOf('_TR069_')<0 || vEntryName[i].indexOf('_INTERNET_')>-1) && (vEntryName[i].indexOf('_R_')>-1))
				<%end if%>
				{
					document.write('<option value=' + vEntryValue[i] + ' id=portmp_' + i + '>' + vEntryName[i] + '</option>');
				}
			}
			</SCRIPT>
                        </SELECT> </TD></TR>
										<TR><TD><INPUT onclick=radioClick() type=radio CHECKED 
                        id=radiosrv name=radiosrv>&nbsp;<%tcWebApi_get("String_Entry","NatVirSerCustomizeText","s")%>&nbsp; </TD><TD>&nbsp;</TD></TR>
                    <TR><TD><INPUT onclick=radioClick() type=radio 
                        id=radiosrv name=radiosrv>&nbsp;<%tcWebApi_get("String_Entry","NatVirSerSelAppText","s")%> &nbsp; </TD><TD>
                        <SELECT disabled onchange=appSelect(this.value) 
                        size=1 id=constsrvName name=constsrvName> <OPTION value=FIRST_APP 
                          selected><%tcWebApi_get("String_Entry","NatSelectText","s")%></OPTION> <OPTION 
                          value="Domain Name Server">Domain Name Server
                          </OPTION> <OPTION value="FTP Server">FTP 
                          Server</OPTION> <OPTION value=IPSEC>IPSEC</OPTION> 
                          <OPTION value="Mail POP3">Mail POP3</OPTION> 
                          <OPTION value="Mail SMTP">Mail SMTP</OPTION> 
                          <OPTION value=PPTP>PPTP</OPTION> <OPTION 
                          value="Real Player 8 Plus">Real Player 8 Plus</OPTION> 
                          <OPTION value="Secure Shell Server">Secure Shell 
                          Server</OPTION> <OPTION 
                          value="Secure Web Server HTTPS">Secure Web Server 
                          HTTPS</OPTION> <OPTION value=SNMP>SNMP</OPTION> 
                          <OPTION value="SNMP Trap">SNMP Trap</OPTION> <OPTION 
                          value="Telnet Server">Telnet Server</OPTION> <OPTION 
                          value=TFTP>TFTP</OPTION> <OPTION 
                          value="Web Server HTTP">Web Server 
                        HTTP</OPTION></SELECT> 
              			</TD></TR>
                    <TR style="DISPLAY: none">
                      <TD width=120><%tcWebApi_get("String_Entry","NatDmzEnableText","s")%></TD>
                      <TD><SELECT size=1 id=PortMappingEnabled name=PortMappingEnabled> <OPTION 
                          value=1 selected><%tcWebApi_get("String_Entry","NatPortMappingEnbaleText","s")%></OPTION> <OPTION 
                          value=0><%tcWebApi_get("String_Entry","NatVirSerDisableText","s")%></OPTION></SELECT> </TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerProtocolText","s")%> </TD>
                      <TD><SELECT size=1 id=PortMappingProtocol name=PortMappingProtocol> <OPTION 
                          value=TCP selected>TCP</OPTION> <OPTION 
                          value=UDP>UDP</OPTION> <OPTION 
                          value=TCP/UDP>TCP/UDP</OPTION></SELECT> </TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %>
                    <TR style="DISPLAY: none">
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerExternalSerText","s")%>：</TD>
                      <TD><INPUT maxLength=15 id=RemoteHost name=RemoteHost></TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerTableExternalPortText","s")%>：</TD>
                      <TD><INPUT maxLength=5 id=ExternalPort name=ExternalPort></TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerTableInternalPortText","s")%>：</TD>
                      <TD><INPUT maxLength=5 id=InternalPort name=InternalPort></TD></TR>
<%else%>				
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerSrcIPText","s")%> </TD>
                      <TD><INPUT maxLength=15 id=RemoteHost name=RemoteHost></TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerSPortNumText","s")%> </TD>
                      <TD><INPUT maxLength=5 id=ExternalPort name=ExternalPort></TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerEndPortNumText","s")%> </TD>
                      <TD><INPUT maxLength=5 id=InternalPort name=InternalPort></TD></TR>
<%end if%>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerInternalSerText","s")%> </TD>
                      <TD><INPUT maxLength=32 id=InternalClient name=InternalClient></TD></TR>
                    <TR>
                      <TD width=120><%tcWebApi_get("String_Entry","NatVirSerMappingNameText","s")%> </TD>
                      <TD><INPUT maxLength=31 id=PortMappingDescription
                    name=PortMappingDescription></TD></TR></TBODY></TABLE></DIV></FORM><BR><LEFT><INPUT onclick=btnSubmit(3) type=button id=btn1 value="<%tcWebApi_get("String_Entry","NatAddText","s")%>" name=btn1> 
                  </LEFT><BR><BR><BR>
                  <DIV id=portMappingInfo></DIV><BR><LEFT>
                  <INPUT onclick=btnSubmit(4) id=btnDel type=button value="<%tcWebApi_get("String_Entry","NatPortMappingDelText","s")%>" name=btnDel> 
<% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %>
                  <INPUT onclick=btnSubmit(6) id=btnOpen name=btnOpen type=button value="<%tcWebApi_get("String_Entry","NatPortMappingOpenText","s")%>"> 
                  <INPUT onclick=btnSubmit(7) id=btnClose name=btnClose type=button value="<%tcWebApi_get("String_Entry","NatVirSerBtnCloseText","s")%>"> 
                  <INPUT onclick=btnSubmit(8) id=btnModify name=btnModify type=button value="<%tcWebApi_get("String_Entry","NatPortMappingModifyText","s")%>">
<%end if%>                
                  </LEFT></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <HR>
                </TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
<%end if%>
<% if tcWebApi_get("WebCustom_Entry","isProtTriggeringSupported","h")="Yes" then %>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD>
        </TR>
 	    <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NatPortTriggerText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
        </TR> 
    <FORM id="PORTTRIGGERING_form" name="PORTTRIGGERING_form" action="/cgi-bin/app-natset.asp" autocomplete="off" method="post">
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD align=middle class=tabdata>
          <FONT color="black"><%tcWebApi_get("String_Entry","NatPortTriggerCommnetText","s")%></FONT>: <% TCWebAPI_get("portTriggering_setting", "canUseNum", "s") %></TD>
        </TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD align=middle>
				<TABLE cellSpacing=0 cellPadding=0 bgColor=#ffffff>
					<TBODY>
						<TR>
							<TD class=tabdata align=middle>
								<FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerApplicationText","s")%></FONT>&nbsp;:&nbsp; 
								<INPUT maxLength=15 size=15 name=PortTriggering_Applications>
								<SELECT onchange=doAppChange(this.selectedIndex) size=1 id=PortTriggering_App_idx name=PortTriggering_App_idx>
								<OPTION selected><%tcWebApi_get("String_Entry","NatSelectText","s")%></OPTION>
								<OPTION>Aim Talk</OPTION>
								<OPTION>Asheron's Call</OPTION>
								<OPTION>Calista IPPhone</OPTION>
								<OPTION>Delta Force</OPTION>
								<OPTION>ICQ</OPTION>
								<OPTION>Napster</OPTION>
								<OPTION>Net2Phone</OPTION>
								<OPTION>QuickTimeClient</OPTION>
								<OPTION>Rainbow 6 Game</OPTION>
								</SELECT>
							</TD>
						</TR>
					</TBODY>
				</TABLE>
		  </TD>
        </TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif></TD>
		  <TD align=middle><INPUT type=hidden value=8 id=PortTriggering_SubmitFlag name=PortTriggering_SubmitFlag>
		  	<TABLE borderColor=#cccccc cellSpacing=0 cellPadding=0 bgColor=#ffffff border=1>
          	<TBODY>
				<TR height=30>
					<TD class=tabdata align=middle><%tcWebApi_get("String_Entry","NatPortTriggerActiveText","s")%></TD>
					<TD class=tabdata align=middle colSpan=3><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerRangeText","s")%></FONT></TD>
					<TD class=tabdata align=middle colSpan=3><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerOpeningRangeText","s")%></FONT></TD>
				</TR>
				<TR height=30>
					<TD class=tabdata align=middle>&nbsp;</TD>
					<TD class=tabdata align=middle><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerStartPort0Text","s")%></FONT></TD>
					<TD class=tabdata align=middle><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerStartPort1Text","s")%></FONT></TD>
					<TD class=tabdata align=middle><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerStartPort2Text","s")%></FONT></TD>
					<TD class=tabdata align=middle><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerStartPort0Text","s")%></FONT></TD>
					<TD class=tabdata align=middle><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerStartPort1Text","s")%></FONT></TD>
					<TD class=tabdata align=middle><FONT color=#000000><%tcWebApi_get("String_Entry","NatPortTriggerStartPort2Text","s")%></FONT></TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_0>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_0>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_0>
					</TD>
					<TD align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_0>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_0>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_0>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_0>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_1>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_1>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_1>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_1>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_1>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_1>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_1>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_2>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_2>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_2>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_2>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_2>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_2>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_2>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_3>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_3>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_3>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_3>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_3>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_3>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_3>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_4>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_4>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_4>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_4>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_4>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_4>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_4>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_5>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_5>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_5>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_5>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_5>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_5>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_5>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_6>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_6>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_6>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_6>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_6>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_6>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_6>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
				<TR>
					<TD class=tabdata align=middle>
						<INPUT type=checkbox name=PortTriggeringActive_7>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_StartPort_7>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortTriggering_EndPort_7>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortTriggering_Protocol_7>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_StartPort_7>
					</TD>
					<TD class=tabdata align=middle>
						<INPUT maxLength=5 size=3 value=0 name=PortOpening_EndPort_7>
					</TD>
					<TD class=tabdata align=middle>
						<SELECT size=1 name=PortOpening_Protocol_7>
							<OPTION selected value="TCP/UDP">TCP/UDP
							<OPTION value="TCP">TCP
							<OPTION value="UDP">UDP
							</OPTION>
						</SELECT>
					</TD>
				</TR>
			</TBODY>
			</TABLE>
        </TD>
    </TR>
    <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD>
    </TR>
    <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
		  <TD align=middle>				
			<INPUT onclick=onClickPortTriggeringSummary("/cgi-bin/adv_nat_porttriggering_list.asp"); type=button value="<%tcWebApi_get("String_Entry","NatPortTriggerSummaryText","s")%>" name=PortTriggeringSummary>
	      </TD>   
	</TR>
	<TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD>
    </TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
		  <TD align=middle>
			<INPUT onclick=doSave(); type=button value="<%tcWebApi_get("String_Entry","NatBtnSaveText","s")%>" name=SaveBtn>
			<input type="reset" name="Reset" value=<%tcWebApi_get("String_Entry","NatBtnCancelText","s")%>>
			<input type=hidden  id=PortTriggering_CanUseNumFlag value="<%if TCWebAPI_get("portTriggering_setting", "canUseNum", "h") <> "N/A" then TCWebAPI_staticGet("portTriggering_setting", "canUseNum", "s") else asp_Write("8") end if %>" name=PortTriggering_CanUseNumFlag>
		  </TD>
    </TR>
  </FORM>
<% end if %>
 </TBODY>
</TABLE>
<FORM style="DISPLAY: none" id=ConfigForm name=ConfigForm action="">
<input type="hidden" id="isVirServerSupported" name="isVirServerSupported" value="<%if tcWebApi_get("Info_Ether","isVirServerSupported","h") = "Yes" then asp_write("1") else asp_write("0") end if%>">
</FORM></TD>
</TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
