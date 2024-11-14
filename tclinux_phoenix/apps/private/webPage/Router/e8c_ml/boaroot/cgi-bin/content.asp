<% tcWebApi_constSet("WebCurSet_Entry", "isLockState", "0") %>
<html>
<head>
<META http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>content</title>
<script language="JavaScript" type="text/javascript">
<%
	if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
	   if tcWebApi_get("SimCard_Entry", "cpeFlag", "h") = "1"  then
		   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "0"  then%>
		   	top.window.location.href="/cgi-bin/InsertSimcardMsg.cgi";
		   <%end if
	   end if
	end if
%>

var VirServerModifyMappingIndex = -1;
var VirServerModifyEntryIndex = -1;
var AclFilterEditRule = new Array();
var AclEditNumber = -1;
var ssidIdx = 0;
var lanDevIndex = 1;
var QoSCurInterface = '';
var DDNSProvider = '';
var curUser = "<%tcWebApi_get("WebCurSet_Entry","CurrentAccess","s")%>";
var ripIndex = "";
var previousPage = "";
var gateWayName = "<%tcWebApi_get("DeviceInfo","GateWay","s")%>";
<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
var ModelName_dev = "<%tcWebApi_get("DeviceInfo_devParaStatic","ModelName","s")%>";
<% if tcwebApi_get("WebCustom_Entry","isCTSFUC9Supported","h") = "Yes" then %>
var ModelName = "SFU";
<% else %>
var ModelName = "E8-B-GPON";
<% end if %>
<% else %>
var ModelName = "<%tcWebApi_get("DeviceInfo_devParaStatic","ModelName","s")%>";
<% end if %> 
var IPv6Support = "<%tcWebApi_get("WebCustom_Entry","isIPv6Supported","s")%>";
var ipsecSupport = "<%tcWebApi_get("WebCustom_Entry","isIpsecSupported","s")%>";
var pageMap =  new Array();//pageMap[8][9]
var voipType = "<%tcWebApi_get("VoIPBasic_Common", "SIPProtocol","s")%>";

var Content_Status="<%tcWebApi_get("String_Entry","ContentStatusText","s")%>";
var Content_DeviceInfo="<%tcWebApi_get("String_Entry","ContentDeviceInfoText","s")%>";
var Content_NetInfo="<%tcWebApi_get("String_Entry","ContentNetInfoText","s")%>";
var Content_UserInfo="<%tcWebApi_get("String_Entry","ContentUserInfoText","s")%>";
var Content_VoIPInfo="<%tcWebApi_get("String_Entry","ContentVoIPInfoText","s")%>";
var Content_CWMPInfo="<%tcWebApi_get("String_Entry","ContentCWMPStatusText","s")%>";

var Content_Net="<%tcWebApi_get("String_Entry","ContentNetText","s")%>";
var Content_WAN="<%tcWebApi_get("String_Entry","ContentWANText","s")%>";
var Content_Binding="<%tcWebApi_get("String_Entry","ContentBindingText","s")%>";
var Content_LAN="<%tcWebApi_get("String_Entry","ContentLANText","s")%>";
var Content_WLAN="<%tcWebApi_get("String_Entry","ContentWLANText","s")%>";
var Content_WLAN5G="<%tcWebApi_get("String_Entry","ContentWLAN5GText","s")%>";
var Content_CWMP="<%tcWebApi_get("String_Entry","ContentCWMPText","s")%>";
var Content_Iphone="<%tcWebApi_get("String_Entry","ContentIphoneText","s")%>";
var Content_Qos="<%tcWebApi_get("String_Entry","ContentQosText","s")%>";
var Content_Time="<%tcWebApi_get("String_Entry","ContentTimeText","s")%>";
var Content_Route="<%tcWebApi_get("String_Entry","ContentRouteText","s")%>";
var Content_QosBand="<%tcWebApi_get("String_Entry","ContentQoSBandWidthText","s")%>";

var Content_Security="<%tcWebApi_get("String_Entry","ContentSecurityText","s")%>";
var Content_WANSet="<%tcWebApi_get("String_Entry","ContentWANSetText","s")%>";
var Content_Firewall="<%tcWebApi_get("String_Entry","ContentFirewallText","s")%>";
var Content_MACFilter="<%tcWebApi_get("String_Entry","ContentMACFilterText","s")%>";
var Content_PortFilter="<%tcWebApi_get("String_Entry","ContentPortFilterText","s")%>";
var Content_AclFilter="<%tcWebApi_get("String_Entry","ContentAclFilterText","s")%>";
var Content_Parental="<%tcWebApi_get("String_Entry","ContentParentalText","s")%>";

var Content_APP="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>";
var Content_DDNS="<%tcWebApi_get("String_Entry","ContentDDNSText","s")%>";
var Content_NAT="<%tcWebApi_get("String_Entry","ContentAdvancedNATText","s")%>";
var Content_UPNP="<%tcWebApi_get("String_Entry","ContentUPNPText","s")%>";
var Content_VoIPSet="<%tcWebApi_get("String_Entry","ContentVoIPSetText","s")%>";
var Content_IGMPOrMLD="<%tcWebApi_get("String_Entry","ContentIGMPMLDText","s")%>";
var Content_IGMP="<%tcWebApi_get("String_Entry","ContentIGMPText","s")%>";
var Content_DailyAPP="<%tcWebApi_get("String_Entry","ContentDailyAPPText","s")%>";
var Content_SAMBA="<%tcWebApi_get("String_Entry","ContentSAMBAText","s")%>";
var Content_VPN="<%tcWebApi_get("String_Entry","ContentVPNText","s")%>";
var Content_SNMP="<%tcWebApi_get("String_Entry","ContentSNMPText","s")%>";

var Content_Management="<%tcWebApi_get("String_Entry","ContentManageText","s")%>";
var Content_UserManagement="<%tcWebApi_get("String_Entry","ContentUserManagementText","s")%>";
var Content_DeviceManagement="<%tcWebApi_get("String_Entry","ContentDeviceManagementText","s")%>";
var Content_LogManagement="<%tcWebApi_get("String_Entry","ContentLogManagementText","s")%>";
var Content_Upgrade="<%tcWebApi_get("String_Entry","ContentUpgradeText","s")%>";
var Content_Maintain="<%tcWebApi_get("String_Entry","ContentMaintainText","s")%>";

var Content_Diagnose="<%tcWebApi_get("String_Entry","ContentDiagnoseText","s")%>";
var Content_InternetDiagnose="<%tcWebApi_get("String_Entry","ContentInternetDiagnoseText","s")%>";
var Content_Help="<%tcWebApi_get("String_Entry","ContentHelpText","s")%>";
var Content_UseHelp="<%tcWebApi_get("String_Entry","ContentUseHelpText","s")%>";
var getWlanWpsTimerCount = 0;
var maxWlanWpsTimerCount = 10;
for (i = 0; i < 8; i++)
{
	pageMap[i] = new Array();
}
<% tcWebApi_set("dynDisp_Entry", "CurPage", "0")%>
pageMap[0][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit0","s") %>";
pageMap[1][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit1","s") %>";
pageMap[2][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit2","s") %>";
pageMap[3][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit3","s") %>";
pageMap[4][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit4","s") %>";
pageMap[5][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit5","s") %>";
pageMap[6][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit6","s") %>";
pageMap[7][0] = "<% tcWebApi_get("dynDisp_Entry","MainMaskBit7","s") %>";
pageMap[0][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[0][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[0][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
pageMap[0][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
pageMap[0][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
pageMap[0][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
pageMap[0][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[0][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% tcWebApi_set("dynDisp_Entry", "CurPage", "1")%>
pageMap[1][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[1][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[1][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
pageMap[1][4] = 0;
   <%else%>
pageMap[1][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
   <% end if
else%>
pageMap[1][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
<% end if
else%>
pageMap[1][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
<% end if %>
pageMap[1][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
pageMap[1][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
pageMap[1][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[1][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% tcWebApi_set("dynDisp_Entry", "CurPage", "2")%>
pageMap[2][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[2][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[2][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isWLanSupported","h") = "Yes" then%>
pageMap[2][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
<% else %>
pageMap[2][4] = 0;
<% end if %>
pageMap[2][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
pageMap[2][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
pageMap[2][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[2][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isCTPHONEAPPSupported","h") = "Yes" then%>
pageMap[2][9] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit8","s") %>";
<% else %>
pageMap[2][9] = 0;
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported","h") = "Yes" then%>
pageMap[2][10] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit9","s") %>";
<% else %>
pageMap[2][10] = 0;
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isWLanACSupported","h") = "Yes" then %>
pageMap[2][11] = 1;
<% else %>
pageMap[2][11] = 0;
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isStaticNatSupported","h") = "Yes" then %>
pageMap[2][12] = 1;
<% else %>
pageMap[2][12] = 0;
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isL2ogreWebSupported","h") = "Yes" then %>
pageMap[2][13] = 1;
<% else %>
pageMap[2][13] = 0;	
<% end if %>
<% tcWebApi_set("dynDisp_Entry", "CurPage", "3")%>
pageMap[3][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[3][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[3][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
pageMap[3][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
pageMap[3][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isParentalSupported","h") = "Yes" then %>
pageMap[3][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
<% else %>
pageMap[3][6] = 0;
<% end if %>
pageMap[3][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[3][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isMacDupDetect","h") = "Yes" then %>
pageMap[3][9] = 1;
<% else %>
pageMap[3][9] = 0;
<% end if %>
<% tcWebApi_set("dynDisp_Entry", "CurPage", "4")%>
pageMap[4][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[4][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[4][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
pageMap[4][4] = 0;
   <%else%>
pageMap[4][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
   <% end if
else%>
pageMap[4][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
<% end if
else%>
pageMap[4][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
<% end if %>
pageMap[4][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
pageMap[4][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isSambaWebSupported","h") = "Yes" then %>
pageMap[4][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
<% else %>
pageMap[4][7] = 0;
<% end if %>
pageMap[4][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isSnmpWebSupported","h") = "Yes" then %>
pageMap[4][9] = 1;	
<% else %>
pageMap[4][9] = 0;	
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isEasyMeshSupport","h") = "Yes" then %>
pageMap[4][10] = 1;
<% else %>
pageMap[4][10] = 0;
<% end if %>
<% tcWebApi_set("dynDisp_Entry", "CurPage", "5")%>
pageMap[5][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[5][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[5][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
pageMap[5][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
<% if tcWebApi_get("WebCustom_Entry", "isUBUSSupported","h") = "Yes" then %>
pageMap[5][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
<% else %>
pageMap[5][5] = 0;
<% end if %>
pageMap[5][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
pageMap[5][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[5][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% tcWebApi_set("dynDisp_Entry", "CurPage", "6")%>
pageMap[6][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[6][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[6][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
pageMap[6][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
pageMap[6][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
pageMap[6][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
pageMap[6][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[6][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
<% tcWebApi_set("dynDisp_Entry", "CurPage", "7")%>
pageMap[7][1] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit0","s") %>";
pageMap[7][2] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit1","s") %>";
pageMap[7][3] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit2","s") %>";
pageMap[7][4] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit3","s") %>";
pageMap[7][5] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>";
pageMap[7][6] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit5","s") %>";
pageMap[7][7] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit6","s") %>";
pageMap[7][8] = "<% tcWebApi_get("dynDisp_Entry","CurMaskBit7","s") %>";
</script>
<frameset rows='0,*' frameborder='0' border='0'>
<frame src='/cgi-bin/refresh.asp' name='refreshfrm' frameborder='no' border='0' scrolling='no' target='_self' marginwidth='0' marginheight='0' noresize>
<frame src='/cgi-bin/sta-device.asp' name='contentfrm' frameborder='no' border='0' target='_self' marginwidth='0' marginheight='0' >
</frameset>
<noframes>The information on the page makes use of frame technology. For best results, use the new version of Browser.</noframes>
</head>
</body>
</html>
