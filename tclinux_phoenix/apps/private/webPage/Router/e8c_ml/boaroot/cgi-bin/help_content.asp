<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv=Content-Script-Type content=text/javascript>
<script language="javascript" src="/JS/util.js"></script>
<link rel="stylesheet" href="/JS/stylemain.css" type="text/css">
<script language="JavaScript" type="text/javascript">
var sptUserName = '0'; 
var sysUserName = '1';
var curUserName = "<%tcWebApi_get("WebCurSet_Entry","CurrentAccess","s")%>";
var isCZGDSupport = "<%tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "s")%>";
function FormLoad()
{
with (document.forms[0])
{
	manageEquip_LOG.style.display = "none";
	
if (curUserName == sysUserName)
{
statusEquip.style.display = "";
statusNetwork.style.display = "";
if(isCZGDSupport == 'Yes')
	getElement('statusNetwork_1').innerText = '<%tcWebApi_get("String_Entry","HelpShowMessage1Text","s")%>';
else
getElement('statusNetwork_1').innerText = '<%tcWebApi_get("String_Entry","HelpShowMessage2Text","s")%>';
statusUser.style.display = "";
getElement('statusUser_1').innerText ='<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %><%tcWebApi_get("String_Entry","HelpShowMessage3Text","s")%><% end if %><%tcWebApi_get("String_Entry","HelpShowMessage4Text","s")%><% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) = "Yes" then %><%tcWebApi_get("String_Entry","HelpShowMessage5Text","s")%><% end if %>。<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %><%tcWebApi_get("String_Entry","HelpShowMessage6Text","s")%><% end if %><%tcWebApi_get("String_Entry","HelpShowMessage7Text","s")%><% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) = "Yes" then %><%tcWebApi_get("String_Entry","HelpShowMessage8Text","s")%> <% end if %>';
statusACS.style.display = "none";
statusACS_1.style.display = "none";
<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) <> "Yes" then %>
ntwk.style.display = "none";
<% end if %>
var network = getElementByName("ntwk_cfgWan");
for (i = 0; i < network.length; i++)
{
network[i].style.display = "none";
}
ntwk_cfgBind.style.display = "none";
ntwk_cfgBind_1.style.display = "none";
if(isCZGDSupport != 'Yes')
{
ntwk_cfgDHCP.style.display = "none";
ntwk_cfgDHCP_1.style.display = "none";
}
ntwk_cfgWLAN.style.display = "";
ntwk_cfgWLAN_2.style.display = "none";
ntwk_cfgWLAN_3.style.display = "none";
ntwk_cfgWLAN_4.style.display = "none";
ntwk_cfgWLAN_5.style.display = "none";
ntwk_cfgWLAN_7.style.display = "none";
ntwk_cfgWLAN_8.style.display = "none";
ntwk_cfgWLAN_9.style.display = "none";
ntwk_cfgRemoteManage.style.display = "none";
ntwk_cfgRemoteManage_1.style.display = "none";
ntwk_cfgRemoteManage_2.style.display = "none";
ntwk_cfgRemoteManage_3.style.display = "none";
ntwk_cfgRemoteManage_4.style.display = "none";
<% tcWebApi_set("dynDisp_Entry", "CurPage", "2")%>
var v_cfgRemote = '<% tcWebApi_get("dynDisp_Entry","CurMaskBit4","s") %>';
if ( v_cfgRemote == '1' )
{
	var item_idx = 1;
	<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %> 
	item_idx ++;
	<% end if %>
	if(isCZGDSupport == 'Yes')
		item_idx ++;
	ntwk.style.display = '';
	ntwk_cfgRemoteManage.style.display = '';
	ntwk_cfgRM01.innerHTML = item_idx + '.<%tcWebApi_get("String_Entry","HelpRemoteManagementText","s")%> <a name=<%tcWebApi_get("String_Entry","HelpRemoteManagementText","s")%> id="<%tcWebApi_get("String_Entry","HelpRemoteManagementText","s")%>"></a>';
	ntwk_cfgRemoteManage_3.style.display = '';
	ntwk_cfgRM03.innerHTML = '&nbsp;(1).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%>';
	ntwk_cfgRemoteManage_4.style.display = '';
	ntwk_cfgRM04.innerHTML = '<ul><li><% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %><%tcWebApi_get("String_Entry","HelpBroadbandcodeText","s")%><% else %><%tcWebApi_get("String_Entry","HelpOLTText","s")%><% end if %></li></ul>';
}
ntwk_cfgQoS.style.display = "none";
ntwk_cfgQoS_1.style.display = "none";
ntwk_cfgSNTP.style.display = "none";
ntwk_cfgSNTP_1.style.display = "none";
ntwk_cfgSNTP_2.style.display = "none";
ntwk_cfgSNTP_3.style.display = "none";
ntwk_cfgSNTP_4.style.display = "none";
ntwk_cfgRoute.style.display = "none";
ntwk_cfgRoute_1.style.display = "none";
ntwk_cfgRoute_2.style.display = "none";
ntwk_cfgRoute_3.style.display = "none";
ntwk_cfgRoute_4.style.display = "none";
<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported","h") = "Yes" then %>
ntwk_dslimit.style.display = "none";
ntwk_dslimit_1.style.display = "none";
ntwk_dslimit_2.style.display = "none";
<% end if %>
securityURL.style.display = "";
securityFirewall.style.display = "";
getElement('securityFirewall_1').innerHTML ='<%tcWebApi_get("String_Entry","HelpFirewallSetupText","s")%>';
securityMAC.style.display = "";
securityMACLimit.style.display = "";
securityPort.style.display = "none";
securityPort_1.style.display = "none";
securityPort_2.style.display = "none";
securityPort_3.style.display = "none";
securityPort_4.style.display = "none";
securityACL.style.display = "none";
securityACL_1.style.display = "none";
securityACL_2.style.display = "none";
securityACL_3.style.display = "none";
securityACL_4.style.display = "none";
securityParentCtrl.style.display = "none";
securityParentCtrl_1.style.display = "none";
securityParentCtrl_2.style.display = "none";
securityParentCtrl_3.style.display = "none";
securityParentCtrl_4.style.display = "none";
appDDNS.style.display = "none";
appDDNS_1.style.display = "none";
appDDNS_2.style.display = "none";
appDDNS_3.style.display = "none";
appDDNS_4.style.display = "none";
if(isCZGDSupport != 'Yes')
{
appAdnat.style.display = "none";
appAdnat_1.style.display = "none";
appAdnat_2.style.display = "none";
appAdnat_3.style.display = "none";
appAdnat_4.style.display = "none";
}
else
{
	appAdnat_2_alg.style.display = "none";
	appAdnat_2_alg_s.style.display = "none";
	appAdnat_2_dmz.style.display = "none";
	appAdnat_2_dmz_s.style.display = "none";
	appAdnat_4_alg.style.display = "none";
	appAdnat_4_dmz.style.display = "none";
}
appUPNP.style.display = "none";
appUPNP_1.style.display = "none";
appUPNP_2.style.display = "none";
appUPNP_3.style.display = "none";
appUPNP_4.style.display = "none";
appVoip.style.display = "none";
appVoip_1.style.display = "none";
appVoip_2.style.display = "none";
appVoip_3.style.display = "none";
appVoip_4.style.display = "none";
appIGMP.style.display = "none";
appIGMP_1.style.display = "none";
appIGMP_2.style.display = "none";
appIGMP_3.style.display = "none";
appIGMP_4.style.display = "none";
<% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) <> "Yes" then %>
apptitle.style.display = "none";
appDaily.style.display = "none";
appDaily_1.style.display = "none";
appDaily_3.style.display = "none";
<% end if %>
appDaily_5.style.display = "none";
appDaily_6.style.display = "none";
appSamba.style.display = "none";
appSamba_1.style.display = "none";
appSamba_2.style.display = "none";
appSamba_3.style.display = "none";
appSamba_4.style.display = "none";
appSNMP.style.display = "none";
appSNMP_1.style.display = "none";
appSNMP_2.style.display = "none";
appSNMP_3.style.display = "none";
appSNMP_4.style.display = "none";
appEasyMesh.style.display = "none";
appEasyMesh_1.style.display = "none";
appEasyMesh_2.style.display = "none";
appEasyMesh_3.style.display = "none";
appEasyMesh_4.style.display = "none";
appEasyMesh_5.style.display = "none";
appEasyMesh_6.style.display = "none";
appEasyMesh_7.style.display = "none";
appEasyMesh_8.style.display = "none";
manageUser.style.display = "";
manageUser_1.style.display = "none";
manageUser_2.style.display = "none";
manageUser_3.style.display = "none";
manageEquip_LOG.style.display = "none";
<% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) = "Yes" then %>
manageEquip_USB.style.display = "none";
<% end if %>
if(isCZGDSupport != 'Yes')
{
manageEquip_2.style.display = "none";
}
if(isCZGDSupport == 'Yes')
{
manageSyslog_1_1.style.display = "none";
manageSyslog_1_3.style.display = "none";	
}
else
{
manageSyslog.style.display = "none";
manageSyslog_1.style.display = "none";
}
manageUpgrade.style.display = "";
manageUpgrade_1.style.display = "none";
netDiag.style.display = "none";
networkDiag.style.display = "none";
networkDiag_1.style.display = "none";
<% if TCWebApi_get("WebCustom_Entry","isCTJOYMESupported","h" ) = "Yes" then %>
serviceDiag.style.display = "none";
serviceDiag_1.style.display = "none";
<% end if %>
<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) <> "Yes" then %>
	getElement("secuiText").innerHTML="<%tcWebApi_get("String_Entry","Help2SecurityText","s")%>";     
	<% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) <> "Yes" then %>
		getElement("manText").innerHTML="<%tcWebApi_get("String_Entry","Help3ManagementText","s")%>";
	<%else%>
		getElement("appText").innerHTML="<%tcWebApi_get("String_Entry","Help3ApplicationText","s")%>";
		getElement("manText").innerHTML="<%tcWebApi_get("String_Entry","Help4ManagementText","s")%>";
	<% end if %>
<% end if %>
}
}
}
</script>
</head>
<body onLoad="FormLoad()">
<form name=test>
<table width="100%" border="0" cellspacing="0" cellpadding="0" id=tab name=tab>
<tr><td class="topHelpTitle" colspan="2"><%tcWebApi_get("String_Entry","HelpHText","s")%>&nbsp;<%tcWebApi_get("String_Entry","HelpEText","s")%>&nbsp;<%tcWebApi_get("String_Entry","HelpLText","s")%>&nbsp;<%tcWebApi_get("String_Entry","HelpPText","s")%></td></tr>
<tr><td colspan="2">&nbsp;</td></tr>
<tr><td class="tabHelpdata" colspan=2 bgcolor=#CCCCCC><%tcWebApi_get("String_Entry","Help1StatusText","s")%><a name=<%tcWebApi_get("String_Entry","HelpStatusText","s")%>></a></td></tr>
<tr name=statusEquip id=statusEquip >
<td width=20>&nbsp;</td>
<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpDeviceInfoText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDeviceInfoText","s")%>></a></td>
</tr>
<tr>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpDerviceTypeText","s")%></li>
</ul>
</td>
</tr>
<tr name=statusNetwork id=statusNetwork >
<td width=20>&nbsp;</td>
<td class="tabdataleft">2.<%tcWebApi_get("String_Entry","HelpWANText","s")%><a name=<%tcWebApi_get("String_Entry","HelpWANText","s")%> id="<%tcWebApi_get("String_Entry","HelpWANText","s")%>"></a></td>
</tr>
<tr>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id=statusNetwork_1><%tcWebApi_get("String_Entry","HelpShowMessage2Text","s")%></li>
</ul>
</td>
</tr>
<tr name=statusUser id=statusUser>
<td width=20>&nbsp;</td>
<td class="tabdataleft">3.<%tcWebApi_get("String_Entry","HelpLANText","s")%><a name=<%tcWebApi_get("String_Entry","HelpLANText","s")%> id="<%tcWebApi_get("String_Entry","HelpLANText","s")%>"></a></td>
</tr>
<tr>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id=statusUser_1><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %><%tcWebApi_get("String_Entry","HelpShowMessage9Text","s")%><% else %><%tcWebApi_get("String_Entry","HelpShowMessage10Text","s")%><% end if %></li>
</ul>
</td>
</tr>
<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") <> "Yes" then %>
<tr name=statusVoip id=statusVoip>
<td width=20>&nbsp;</td>
<td class="tabdataleft">4.<%tcWebApi_get("String_Entry","HelpVoIPText","s")%><a name=<%tcWebApi_get("String_Entry","HelpVoIPText","s")%> id="<%tcWebApi_get("String_Entry","HelpVoIPText","s")%>"></a></td>
</tr>
<tr>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id=statusVoip_1><%tcWebApi_get("String_Entry","HelpShowMessage11Text","s")%></li>
</ul>
</td>
</tr>
<% end if %>
<tr name=statusACS id=statusACS>
<td width=20>&nbsp;</td>
<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") = "Yes" then %>
<td class="tabdataleft">4.<%tcWebApi_get("String_Entry","HelpRemoteStatusText","s")%><a name=<%tcWebApi_get("String_Entry","HelpRemoteStatusText","s")%> id="<%tcWebApi_get("String_Entry","HelpRemoteStatusText","s")%>"></a></td>
<% else %>
<td class="tabdataleft">5.<%tcWebApi_get("String_Entry","HelpRemoteStatusText","s")%><a name=<%tcWebApi_get("String_Entry","HelpRemoteStatusText","s")%> id="<%tcWebApi_get("String_Entry","HelpRemoteStatusText","s")%>"></a></td>
<% end if %> 
</tr>
<tr>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id=statusACS_1><%tcWebApi_get("String_Entry","HelpShowMessage12Text","s")%></li>
</ul>
</td>
</tr>
<tr name=ntwk id=ntwk>
<td class="tabHelpdata" colspan=2 bgcolor=#CCCCCC><%tcWebApi_get("String_Entry","Help2InternetText","s")%><a name=<%tcWebApi_get("String_Entry","HelpNetworkText","s")%>></a></td>
</tr>
<tr name=ntwk_cfgWan id=ntwk_cfgWan>
<td>&nbsp;</td>
<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpBroadbandText","s")%><a name=<%tcWebApi_get("String_Entry","HelpBroadbandText","s")%>></a></td>
</tr>
<tr name=ntwk_cfgWan id=ntwk_cfgWan>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=ntwk_cfgWan id=ntwk_cfgWan>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpInternetConnText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgWan id=ntwk_cfgWan>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=ntwk_cfgWan id=ntwk_cfgWan>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupBroadText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpWANConnText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpEnablePVCText","s")%></li>
<li id=ntwk_cfgWan_1><%tcWebApi_get("String_Entry","HelpBidingText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpModeRouteText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpConnIPAndPPPText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpIPProtocolText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpPPPoEText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpDHCPText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpStaticText","s")%></li>
<li id=ntwk_cfgWan_2><%tcWebApi_get("String_Entry","HelpSupportText","s")%>802.1q、802.1p、NAT、IGMP Proxy。</li>
<li id=ntwk_cfgWan_3><%tcWebApi_get("String_Entry","HelpServiceTypeText","s")%>:UBR Without PCR、UBR With PCR、CBR、Non-Real Time VBR、Realtime VBR。</li>
<li><%tcWebApi_get("String_Entry","HelpIPv6Text","s")%></li>
</ul>
</ul></td>
</tr>
<tr name=ntwk_cfgBind id=ntwk_cfgBind>
<td>&nbsp;</td>
<td class="tabdataleft">2.<%tcWebApi_get("String_Entry","HelpBindConfigurationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpBindConfigurationText","s")%> id="<%tcWebApi_get("String_Entry","HelpBindConfigurationText","s")%>"></a></td>
</tr>
<tr name=ntwk_cfgBind_1 id=ntwk_cfgBind_1>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpBindText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgDHCP id=ntwk_cfgDHCP>
<td>&nbsp;</td>
<script language="JavaScript" type="text/javascript">
if((curUserName == sysUserName) && (isCZGDSupport == 'Yes'))
document.write( '<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpLANConfigurationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpLANConfigurationText","s")%>id="<%tcWebApi_get("String_Entry","HelpLANConfigurationText","s")%>"></a></td>');
else
document.write( '<td class="tabdataleft">3.<%tcWebApi_get("String_Entry","HelpLANConfigurationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpLANConfigurationText","s")%> id="<%tcWebApi_get("String_Entry","HelpLANConfigurationText","s")%>"></a></td>');
</script>
</tr>
<tr name=ntwk_cfgDHCP_1 id=ntwk_cfgDHCP_1>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpIPv4Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpIPv6configureText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgWLAN id=ntwk_cfgWLAN <% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) <> "Yes" then %>style="display:none"<% end if %>>
<td>&nbsp; </td>
<script language="JavaScript" type="text/javascript">
if (curUserName == sptUserName)
{
<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>
document.write( '<td class="tabdataleft" >4.<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%> id="<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%>"></a></td>');
<% end if %>
}
else if(curUserName == sysUserName)
{
<% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>
if(isCZGDSupport == 'Yes')
document.write( '<td class="tabdataleft" >2.<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%> id="<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%>"></a></td>');
else
document.write( '<td class="tabdataleft" >1.<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%> id="<%tcWebApi_get("String_Entry","HelpWLANConfigurationText","s")%>"></a></td>');
<% end if %>
}
</script>
</tr>
<tr <% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) <> "Yes" then %>style="display:none"<% end if %>>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li id=ntwk_cfgWLAN_1><%tcWebApi_get("String_Entry","HelpEnableWLANText","s")%></li>
<li id=ntwk_cfgWLAN_2><%tcWebApi_get("String_Entry","HelpModeChooseText","s")%><i><b>802.11b</b></i>,<i><b>802.11g</b></i>,<i><b>802.11n</b></i>,<i><b>802.11b/g<%tcWebApi_get("String_Entry","HelpMixedText","s")%></b></i><%tcWebApi_get("String_Entry","HelpAndText","s")%><i><b>802.11b/g/n<%tcWebApi_get("String_Entry","HelpMixedText","s")%></b></i><%tcWebApi_get("String_Entry","HelpModeText","s")%></li>
<li id=ntwk_cfgWLAN_3><%tcWebApi_get("String_Entry","HelpChannelSelectText","s")%></li>
<li id=ntwk_cfgWLAN_4><%tcWebApi_get("String_Entry","HelpTransmissionText","s")%></li>
<li id=ntwk_cfgWLAN_5><%tcWebApi_get("String_Entry","HelpSSIDIndexText","s")%></li>
<li id=ntwk_cfgWLAN_6><%tcWebApi_get("String_Entry","HelpSSIDSetText","s")%></li>
<li id=ntwk_cfgWLAN_7><%tcWebApi_get("String_Entry","HelpRateText","s")%></li>
<li id=ntwk_cfgWLAN_8><%tcWebApi_get("String_Entry","HelpBandwithText","s")%></li>
<li id=ntwk_cfgWLAN_9><%tcWebApi_get("String_Entry","HelpGuardIntervalText","s")%></li>
<li id=ntwk_cfgWLAN_10><%tcWebApi_get("String_Entry","HelpSSIDEnableText","s")%></li>
<li id=ntwk_cfgWLAN_11><%tcWebApi_get("String_Entry","HelpBroadcastText","s")%></li>
<li id=ntwk_cfgWLAN_12><%tcWebApi_get("String_Entry","HelpSecurityConfiureText","s")%></li>
<li id=ntwk_cfgWLAN_13><%tcWebApi_get("String_Entry","HelpWPAKeyText","s")%></li>
<li id=ntwk_cfgWLAN_14><%tcWebApi_get("String_Entry","HelpWPAEncrytionText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgRemoteManage id=ntwk_cfgRemoteManage>
<td>&nbsp;</td>
<td class="tabdataleft" name='ntwk_cfgRM01' id='ntwk_cfgRM01'><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>5<% else %>4<% end if%>.<%tcWebApi_get("String_Entry","HelpRemoteManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpRemoteManagementText","s")%> id="<%tcWebApi_get("String_Entry","HelpRemoteManagementText","s")%>"></a></td>
</tr>
<tr name=ntwk_cfgRemoteManage_1 id=ntwk_cfgRemoteManage_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=ntwk_cfgRemoteManage_2 id=ntwk_cfgRemoteManage_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpITMSServerText","s")%></li>
<li><% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %><%tcWebApi_get("String_Entry","HelpBroadbandcodeText","s")%><% else %><%tcWebApi_get("String_Entry","HelpOLTText","s")%><% end if %></li>
</ul></td>
</tr>
<tr name=ntwk_cfgRemoteManage_3 id=ntwk_cfgRemoteManage_3>
<td>&nbsp;</td>
<td class="tabdataleft" name='ntwk_cfgRM03' id='ntwk_cfgRM03'>&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=ntwk_cfgRemoteManage_4 id=ntwk_cfgRemoteManage_4>
<td>&nbsp;</td>
<td class="tabdataleft" name='ntwk_cfgRM04' id='ntwk_cfgRM04'><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupURLText","s")%></li>
<li><% if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then %><%tcWebApi_get("String_Entry","HelpBroadbandcodeText","s")%><% else %><%tcWebApi_get("String_Entry","HelpOLTText","s")%><% end if %></li>
</ul></td>
</tr>
<tr name=ntwk_cfgQoS id=ntwk_cfgQoS>
<td>&nbsp;</td>
<td class="tabdataleft"><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>6<% else %>5<% end if%>.QoS<a name=QoS id="QoS"></a></td>
</tr>
<tr name=ntwk_cfgQoS_1 id=ntwk_cfgQoS_1>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpQosSetupText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgSNTP id=ntwk_cfgSNTP>
<td>&nbsp;</td>
<td class="tabdataleft"><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>7<% else %>6<% end if%>.<%tcWebApi_get("String_Entry","HelpTimeManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpTimeManagementText","s")%> id="<%tcWebApi_get("String_Entry","HelpTimeManagementText","s")%>"></a></td>
</tr>
<tr name=ntwk_cfgSNTP_1 id=ntwk_cfgSNTP_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=ntwk_cfgSNTP_2 id=ntwk_cfgSNTP_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgSNTP_3 id=ntwk_cfgSNTP_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=ntwk_cfgSNTP_4 id=ntwk_cfgSNTP_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul><li>SNTP </li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpSNTPSetupText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpServerSetupdemainText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpTimezoneText","s")%></li>
</ul>
</ul></td>
</tr>
<tr name=ntwk_cfgRoute id=ntwk_cfgRoute>
<td>&nbsp;</td>
<td class="tabdataleft"><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>8<% else %>7<% end if%>.<%tcWebApi_get("String_Entry","HelpRouteSetupText","s")%><a name=<%tcWebApi_get("String_Entry","HelpRouteSetupText","s")%> id="<%tcWebApi_get("String_Entry","HelpRouteSetupText","s")%>"></a></td>
</tr>
<tr name=ntwk_cfgRoute_1 id=ntwk_cfgRoute_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=ntwk_cfgRoute_2 id=ntwk_cfgRoute_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpStaticRouteText","s")%></li>
</ul></td>
</tr>
<tr name=ntwk_cfgRoute_3 id=ntwk_cfgRoute_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=ntwk_cfgRoute_4 id=ntwk_cfgRoute_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpStaticRouteText","s")%></li>
</ul></td>
</tr>
<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported","h") = "Yes" then %>
<tr name=ntwk_dslimit id=ntwk_dslimit>
<td>&nbsp;</td>
<td class="tabdataleft"><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>9<% else %>8<% end if%>.<%tcWebApi_get("String_Entry","HelpQosManagementText","s")%><a name='<%tcWebApi_get("String_Entry","HelpQosManagementText","s")%>' id='<%tcWebApi_get("String_Entry","HelpQosManagementText","s")%>'></a></td>
</tr>
<tr name=ntwk_dslimit_1 id=ntwk_dslimit_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpSupportUplinkText","s")%></td>
</tr>
<tr name=ntwk_dslimit_2 id=ntwk_dslimit_2>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpSupportUserText","s")%></td>
</tr>
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isStaticNatSupported","h") = "Yes" then %>
<tr name=ntwk_cfgRoute id=ntwk_cfgRoute>
<td>&nbsp;</td>
<td class="tabdataleft"><% if TCWebApi_get("WebCustom_Entry","isWLanSupported","h" ) = "Yes" then %>9<% else %>8<% end if%>.One to One<a name='One to One' id="One to One"></a></td>
</tr>
<tr name=ntwk_cfgRoute_1 id=ntwk_cfgRoute_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=ntwk_cfgRoute_2 id=ntwk_cfgRoute_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li>One to One NAT</li>
</ul></td>
</tr>
<tr name=ntwk_cfgRoute_3 id=ntwk_cfgRoute_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=ntwk_cfgRoute_4 id=ntwk_cfgRoute_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li>One to One NAT:<%tcWebApi_get("String_Entry","HelpSetupLocalIPText","s")%></li>
</ul></td>
</tr>
<% end if %>
<tr>
<td class="tabHelpdata" colspan=2 bgcolor=#CCCCCC id="secuiText"><%tcWebApi_get("String_Entry","Help3SecurityText","s")%><a name=<%tcWebApi_get("String_Entry","HelpSecurityText","s")%>></a></td>
</tr>
<tr name=securityURL id=securityURL>
<td>&nbsp;</td>
<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpWWWSetupText","s")%><a name=<%tcWebApi_get("String_Entry","HelpWWWSetupText","s")%> id="<%tcWebApi_get("String_Entry","HelpWWWSetupText","s")%>"></a></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpCurrentFilterText","s")%></li>
</ul></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpWWWFilterText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpFilterModeText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpURLText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpFilterListText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpWillFilterText","s")%></li>
</ul></td>
</tr>
<tr name=securityFirewall id=securityFirewall>
<td>&nbsp;</td>
<td class="tabdataleft">2.<%tcWebApi_get("String_Entry","HelpFirewallText","s")%><a name=<%tcWebApi_get("String_Entry","HelpFirewallText","s")%>></a></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li id=securityFirewall_1><%tcWebApi_get("String_Entry","HelpFirewallSetupLevelText","s")%></li>
</ul></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSecurityLevelText","s")%></li>
</ul></td>
</tr>
<tr name=securityMAC id=securityMAC>
<td>&nbsp;</td>
<td class="tabdataleft">3.<%tcWebApi_get("String_Entry","HelpMACFilterText","s")%><a name=<%tcWebApi_get("String_Entry","HelpMACFilterText","s")%>></a></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpCurrentFilterText","s")%></li>
</ul></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpActiveWWWText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpFilterModeText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpMACAddressText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpFilterListMACText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpWillFilterMACText","s")%></li>
</ul></td>
</tr>
<tr name=securityPort id=securityPort>
<td>&nbsp;</td>
<td class="tabdataleft">4.<%tcWebApi_get("String_Entry","HelpPortFilterText","s")%><a name=<%tcWebApi_get("String_Entry","HelpPortFilterText","s")%>></a></td>
</tr>
<tr name=securityPort_1 id=securityPort_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=securityPort_2 id=securityPort_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpCurrentFilterText","s")%></li>
</ul></td>
</tr>
<tr name=securityPort_3 id=securityPort_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=securityPort_4 id=securityPort_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpWWWFilterText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpFilterModeText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpAddOneFilterText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpFilterListShowText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpWillFilterURLText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpRuleText","s")%></li>
</ul></td>
</tr>
<% if tcWebApi_get("WebCustom_Entry", "isMacDupDetect","h") = "Yes" then %>
<tr name=securityMACLimit id=securityMACLimit>
<td>&nbsp;</td>
<td class="tabdataleft">4.MacDup Detect<a name='MacDup Detect'></a></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
</ul></td>
</tr>
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isMacDupDetect","h") = "Yes" then %>
<tr name=securityACL id=securityACL>
<td>&nbsp;</td>
<td class="tabdataleft">5.<%tcWebApi_get("String_Entry","HelpACLFilter","s")%><a name=<%tcWebApi_get("String_Entry","HelpACLFilter","s")%>></a></td>
</tr>
<tr name=securityACL_1 id=securityACL_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=securityACL_2 id=securityACL_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
</ul></td>
</tr>
<tr name=securityACL_3 id=securityACL_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=securityACL_4 id=securityACL_4>
<td>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpACLFileterActiveText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterModeText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterActionText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpACLFilterListText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterSetText","s")%></li>
</ul></td>
</tr>
<tr name=securityParentCtrl id=securityParentCtrl>
<td>&nbsp;</td>
<td class="tabdataleft">6.<%tcWebApi_get("String_Entry","HelpParentCtrl","s")%><a name=<%tcWebApi_get("String_Entry","HelpParentCtrl","s")%>></a></td>
</tr>
<tr name=securityParentCtrl_1 id=securityParentCtrl_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=securityParentCtrl_2 id=securityParentCtrl_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTemplateText","s")%></li>
<li>MAC</li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlURLActiveText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTimerActiveText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTimerModeText","s")%></li>
</ul></td>
</tr>
<tr name=securityParentCtrl_3 id=securityParentCtrl_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=securityParentCtrl_4 id=securityParentCtrl_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlActiveText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTimerModeSetText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlActionText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlListText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlSetText","s")%></li>
</ul></td>
</tr>
<% else %>
<tr name=securityACL id=securityACL>
<td>&nbsp;</td>
<td class="tabdataleft">5.<%tcWebApi_get("String_Entry","HelpACLFilter","s")%><a name=<%tcWebApi_get("String_Entry","HelpACLFilter","s")%>></a></td>
</tr>
<tr name=securityACL_1 id=securityACL_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=securityACL_2 id=securityACL_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
</ul></td>
</tr>
<tr name=securityACL_3 id=securityACL_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=securityACL_4 id=securityACL_4>
<td>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpACLFileterActiveText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterModeText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterActionText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpACLFilterListText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpACLFilterSetText","s")%></li>
</ul></td>
</tr>
<tr name=securityParentCtrl id=securityParentCtrl>
<td>&nbsp;</td>
<td class="tabdataleft">6.<%tcWebApi_get("String_Entry","HelpParentCtrl","s")%><a name=<%tcWebApi_get("String_Entry","HelpParentCtrl","s")%>></a></td>
</tr>
<tr name=securityParentCtrl_1 id=securityParentCtrl_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=securityParentCtrl_2 id=securityParentCtrl_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTemplateText","s")%></li>
<li>MAC</li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlURLActiveText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTimerActiveText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTimerModeText","s")%></li>
</ul></td>
</tr>
<tr name=securityParentCtrl_3 id=securityParentCtrl_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=securityParentCtrl_4 id=securityParentCtrl_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlActiveText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlTimerModeSetText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlActionText","s")%></li>
</ul>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlListText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpParentCtrlSetText","s")%></li>
</ul></td>
</tr>
<% end if %>

<tr id="apptitle">
<td class="tabHelpdata" colspan=2 bgcolor=#CCCCCC id="appText"><%tcWebApi_get("String_Entry","Help4ApplicationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpApplicationText","s")%>></a></td>
</tr>
<tr name=appDDNS id=appDDNS>
<td width=20>&nbsp;</td>
<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpDDNSSetupText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDDNSSetupText","s")%> id="<%tcWebApi_get("String_Entry","HelpDDNSSetupText","s")%>"></a></td>
</tr>
<tr name=appDDNS_1 id=appDDNS_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appDDNS_2 id=appDDNS_2>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpDynamicText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpDDNSSetupText","s")%></li>
</ul>
</ul>	 		</td>
</tr>
<tr name=appDDNS_3 id=appDDNS_3>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appDDNS_4 id=appDDNS_4>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpEnableDDNSText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpDDNSSetupParametersText","s")%></li>
</ul>	 	</td>
</tr>
<tr name=appAdnat id=appAdnat>
<td width=20>&nbsp;</td>
<script language="JavaScript" type="text/javascript">
if((curUserName == sysUserName) && (isCZGDSupport == 'Yes'))
document.write( '<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpAdvancedNATText","s")%><a name=<%tcWebApi_get("String_Entry","HelpAdvancedNATText","s")%> id="<%tcWebApi_get("String_Entry","HelpAdvancedNATText","s")%>"></a></td>');
else
document.write( '<td class="tabdataleft">2.<%tcWebApi_get("String_Entry","HelpAdvancedNATText","s")%><a name=<%tcWebApi_get("String_Entry","HelpAdvancedNATText","s")%> id="<%tcWebApi_get("String_Entry","HelpAdvancedNATText","s")%>"></a></td>');
</script>
</tr>
<tr name=appAdnat_1 id=appAdnat_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appAdnat_2 id=appAdnat_2>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id='appAdnat_2_alg'>ALG<%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul id='appAdnat_2_alg_s'>
<li>L2TP ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>IPSec ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>H.323 ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>RTSP ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>SIP ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>FTP ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>PPTP ALG<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
</ul>
<li id='appAdnat_2_dmz'>DMZ<%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul type="circle" id='appAdnat_2_dmz_s'>
<li><%tcWebApi_get("String_Entry","HelpDMZStatusText","s")%></li>
</ul>
<%if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes"  then%>
<li><%tcWebApi_get("String_Entry","HelpDMZSetupText","s")%></li>
<ul type="circle">
<li><%tcWebApi_get("String_Entry","HelpDMZStatusText","s")%></li>
</ul>
<% end if %>
</ul>	 	</td>
</tr>
<tr name=appAdnat_3 id=appAdnat_3>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appAdnat_4 id=appAdnat_4>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id='appAdnat_4_alg'><%tcWebApi_get("String_Entry","HelpALGActivateText","s")%></li>
<li id='appAdnat_4_dmz'><%tcWebApi_get("String_Entry","HelpDMZSAetupCanText","s")%></li>
<%if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes"  then%>
<li><%tcWebApi_get("String_Entry","HelpVirtualText","s")%></li>
<% end if %>
</ul>	 	</td>
</tr>
<tr name=appUPNP id=appUPNP>
<td width=20>&nbsp;</td>
<td class="tabdataleft">3.<%tcWebApi_get("String_Entry","HelpUPNPSetupText","s")%><a name=<%tcWebApi_get("String_Entry","HelpUPNPSetupText","s")%> id="<%tcWebApi_get("String_Entry","HelpUPNPSetupText","s")%>"></a></td>
</tr>
<tr name=appUPNP_1 id=appUPNP_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appUPNP_2 id=appUPNP_2>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpDisplayUPnPText","s")%></li>
</ul>	 	</td>
</tr>
<tr name=appUPNP_3 id=appUPNP_3>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appUPNP_4 id=appUPNP_4>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpSetupUPnPText","s")%></li>
</ul>	 	</td>
</tr>
<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") <> "Yes" then %>
<tr name=appVoip id=appVoip>
<td width=20>&nbsp;</td>
<td class="tabdataleft">4.<%tcWebApi_get("String_Entry","HelpVoIPSetupText","s")%><a name=<%tcWebApi_get("String_Entry","HelpVoIPSetupText","s")%> id="<%tcWebApi_get("String_Entry","HelpVoIPSetupText","s")%>"></a></td>
</tr>
<tr name=appVoip_1 id=appVoip_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appVoip_2 id=appVoip_2>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpBasicVoIPText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpAdvanceVoIPText","s")%></li>
</ul>	 	</td>
</tr>
<tr name=appVoip_3 id=appVoip_3>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appVoip_4 id=appVoip_4>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpModifySIPText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpOutboundText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpOutboundNumberText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpBackupAddressText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpBackupNumberText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpMainAddressText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpMainNumberText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpBackupserverAddressText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpBackupPortNumberText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpLocalPortText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpDigMapText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpDigMapModeText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpCountryAndRegionText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpSignalingText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpMediaportText","s")%></li>
</ul>	 	</td>
</tr>
<% end if %>

<tr name=appIGMP id=appIGMP>
<td>&nbsp;</td>
<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") = "Yes" then %>
<td class="tabdataleft">
<% if tcwebApi_get("WebCustom_Entry","isIPv6Supported","h") ="Yes" then %>
	4.<%tcWebApi_get("String_Entry","HelpIGMPSetupText","s")%>
<% else %>
	4.<%tcWebApi_get("String_Entry","HelpIGMPSetText","s")%>
<% end if %>
	<a name=<%tcWebApi_get("String_Entry","HelpIGMPSetupText","s")%>id="<%tcWebApi_get("String_Entry","HelpIGMPSetupText","s")%>"></a></td>
<% else %>
<td class="tabdataleft">
<% if tcwebApi_get("WebCustom_Entry","isIPv6Supported","h") ="Yes" then %>
	5.<%tcWebApi_get("String_Entry","HelpIGMPSetupText","s")%>
<% else %>
	5.<%tcWebApi_get("String_Entry","HelpIGMPSetText","s")%>
<% end if %>
	<a name=<%tcWebApi_get("String_Entry","HelpIGMPSetupText","s")%> id="<%tcWebApi_get("String_Entry","HelpIGMPSetupText","s")%>"></a></td>
<% end if %> 
</tr>
<tr name=appIGMP_1 id=appIGMP_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appIGMP_2 id=appIGMP_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<% if tcwebApi_get("WebCustom_Entry","isIPv6Supported","h") ="Yes" then %>
<li>IGMP/MLD Snooping<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>IGMP/MLD Proxy<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<% else %>
<li>IGMP Snooping<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<li>IGMP Proxy<%tcWebApi_get("String_Entry","HelpStatusText","s")%></li>
<% end if %>
</ul></td>
</tr>
<tr name=appIGMP_3 id=appIGMP_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appIGMP_4 id=appIGMP_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<% if tcwebApi_get("WebCustom_Entry","isIPv6Supported","h") ="Yes" then %>
<li><%tcWebApi_get("String_Entry","HelpIGMP1Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpIGMP2Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpIGMP3Text","s")%></li>
<% else %>
<li><%tcWebApi_get("String_Entry","HelpIGMP4Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpIGMP5Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpIGMP6Text","s")%></li>
<% end if %>
</ul></td>
</tr>
<tr name=appDaily id=appDaily>
<td>&nbsp;</td>
<script language="JavaScript" type="text/javascript">
if (curUserName == sptUserName)
{
<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") = "Yes" then %>
document.write( '<td class="tabdataleft" >5.<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%> id="<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%>"></a></td>');
<% else %>
document.write( '<td class="tabdataleft" >6.<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%> id="<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%>"></a></td>');
<% end if %> 
}
else if(curUserName == sysUserName)
{
if(isCZGDSupport == 'Yes')
document.write( '<td class="tabdataleft" >2.<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%>id="<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%>"></a></td>');
else
document.write( '<td class="tabdataleft" >1.<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%> id="<%tcWebApi_get("String_Entry","HelpDailyApplicationText","s")%>"></a></td>');
}
</script>
</tr>
<tr name=appDaily_1 id=appDaily_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appDaily_2 id=appDaily_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) = "Yes" then %>
<li><%tcWebApi_get("String_Entry","HelpFTPText","s")%></li>
<% end if %>
<li id=appDaily_5>IPTV<%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
</ul></td>
</tr>
<tr name=appDaily_3 id=appDaily_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appDaily_4 id=appDaily_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) = "Yes" then %>
<li><%tcWebApi_get("String_Entry","HelpAllowUserText","s")%></li>
<% end if %>
<li id=appDaily_6><%tcWebApi_get("String_Entry","HelpWANportText","s")%></li>
</ul></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"></td>
</tr>


<tr name=appIPSEC id=appIPSEC>
<td>&nbsp;</td>
<script language="JavaScript" type="text/javascript">
if (curUserName == sptUserName)
{
<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") = "Yes" then %>
document.write( '<td class="tabdataleft" >6.VPN(IPSEC)<a name=IPSEC id="IPSEC"></a></td>');
<% else %>
document.write( '<td class="tabdataleft" >7.VPN(IPSEC)<a name=IPSEC id="IPSEC"></a></td>');
<% end if %> 
}
else if(curUserName == sysUserName)
{
if(isCZGDSupport == 'Yes')
document.write( '<td class="tabdataleft" >2.VPN(IPSEC)<a name=IPSEC id="IPSEC"></a></td>');
else
document.write( '<td class="tabdataleft" >1.VPN(IPSEC)<a name=IPSEC id="IPSEC"></a></td>');
}
</script>
</tr>
<tr name=appIPSEC_1 id=appIPSEC_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appIPSEC_2 id=appIPSEC_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li id=appIPSEC_5>VPN(IPSEC) settings: please check <%tcWebApi_get("String_Entry","HelpApplicationText","s")%>->VPN webpage</li>
</ul></td>
</tr>
<tr name=appIPSEC_3 id=appIPSEC_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appIPSEC_4 id=appIPSEC_4>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li id=appIPSEC_6>VPN(IPSEC) operation: please go to <%tcWebApi_get("String_Entry","HelpApplicationText","s")%>->VPN webpage</li>
</ul></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"></td>
</tr>

<tr name=appSamba id=appSamba>
<td>&nbsp;</td>
<td class="tabdataleft">8.Samba<a name=Samba id="Samba"></a></td>
</tr>
<tr name=appSamba_1 id=appSamba_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appSamba_2 id=appSamba_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSambaInformation","s")%></li>
</ul>
</td>
</tr>
<tr name=appSamba_3 id=appSamba_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appSamba_4 id=appSamba_4>
<td>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpappSambaActive","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpappSambaUserName","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSambaNewPassword","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSambaSurePassword","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpappSambaGroup","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSambaNetBios","s")%></li>
</ul>
</ul>
</td>
</tr>

<tr name=appSNMP id=appSNMP>
<td>&nbsp;</td>
<td class="tabdataleft">9.SNMP<a name=SNMP id="SNMP"></a></td>
</tr>
<tr name=appSNMP_1 id=appSNMP_1>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).<%tcWebApi_get("String_Entry","HelpStatusdiaplayText","s")%></td>
</tr>
<tr name=appSNMP_2 id=appSNMP_2>
<td>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpappSNMPActiveInfoText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSNMPv3ActiveInfoText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSNMPInformationText","s")%></li>
</ul>
</td>
</tr>
<tr name=appSNMP_3 id=appSNMP_3>
<td>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).<%tcWebApi_get("String_Entry","HelpOperatingText","s")%></td>
</tr>
<tr name=appSNMP_4 id=appSNMP_4>
<td>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpSetupText","s")%></li>
<ul>
<li><%tcWebApi_get("String_Entry","HelpappSNMPActiveText","s")%> </li>
<li><%tcWebApi_get("String_Entry","HelpappSNMPv3ActiveText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappSNMPSetText","s")%></li>
</ul>
</ul>
</td>
</tr>
<tr name=appEasyMesh id=appEasyMesh>
<td width=20>&nbsp;</td>
<td class="tabdataleft">10.EasyMesh<a name=EasyMesh id="EasyMesh"></a></td>
</tr>
<tr name=appEasyMesh_1 id=appEasyMesh_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(1).Bassic</td>
</tr>
<tr name=appEasyMesh_2 id=appEasyMesh_2>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpActivateText","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshBassic1Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshBassic2Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshBassic3Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshBassic4Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshBassic5Text","s")%></li>
</ul>	 	
</td>
</tr>
<tr name=appEasyMesh_3 id=appEasyMesh_3>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(2).Advanced</td>
</tr>
<tr name=appEasyMesh_4 idappEasyMesh_4>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpappMeshAdvanced1Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshAdvanced2Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshAdvanced3Text","s")%></li>
</ul>	 	
</td>
</tr>
<tr name=appEasyMesh_5 id=appEasyMesh_5>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(3).Action</td>
</tr>
<tr name=appEasyMesh_6 id=appEasyMesh_6>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpappMeshAction1Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshAction2Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshAction3Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshAction4Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshAction5Text","s")%></li>
</ul>	 	
</td>
</tr>
<tr name=appEasyMesh_7 id=appEasyMesh_7>
<td width=20>&nbsp;</td>
<td class="tabdataleft">&nbsp;(4).Status</td>
</tr>
<tr name=appEasyMesh_8 id=appEasyMesh_8>
<td width=20>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus1Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus2Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus3Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus4Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus5Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus6Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus7Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpappMeshStatus8Text","s")%></li>
</ul>	 	
</td>
</tr>
<tr>
<script language="JavaScript" type="text/JavaScript">
if (curUserName == sysUserName)
	document.write('<td id=manText class="tabHelpdata" colspan=2 bgcolor=#CCCCCC><%tcWebApi_get("String_Entry","Help5ManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpManagementText","s")%>></a></td>');
else
	document.write('<td id=manText class="tabHelpdata" colspan=2 bgcolor=#CCCCCC><%tcWebApi_get("String_Entry","Help5ManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpManagementText","s")%>></a></td>');
</script>
</tr>
<tr name=manageUser id=manageUser>
<td>&nbsp;</td>
<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpUserManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpUserManagementText","s")%> id="<%tcWebApi_get("String_Entry","HelpUserManagementText","s")%>"></a></td>
</tr>
<tr name=manageUser_1 id=manageUser_1>
<td>&nbsp;</td>
<td class="tabdataleft" >&nbsp;(1)<%tcWebApi_get("String_Entry","HelpAdminAccountText","s")%></td>
</tr>
<tr name=manageUser_2 id=manageUser_2>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpPasswordSetupText","s")%></li>
</ul></td>
</tr>
<tr name=manageUser_3 id=manageUser_3>
<td>&nbsp;</td>
      <td class="tabdataleft">&nbsp;(2)<%tcWebApi_get("String_Entry","HelpUseraccountText","s")%></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
          <li><%tcWebApi_get("String_Entry","HelpPasswordaccountText","s")%></li>
        </ul></td>
</tr>
<tr name=manageEquip id=manageEquip>
<td width=20>&nbsp;</td>
<td class="tabdataleft">2.<%tcWebApi_get("String_Entry","HelpDeviceManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDeviceManagementText","s")%> id="<%tcWebApi_get("String_Entry","HelpDeviceManagementText","s")%>"></a></td>
</tr>
<tr>
<td>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpRebootDeviceText","s")%></li>
<% if TCWebApi_get("WebCustom_Entry","isCTUsbSupported","h" ) = "Yes" then %>
<li id=manageEquip_USB><%tcWebApi_get("String_Entry","HelpUSBbackupText","s")%></li>
<% end if %>
<li id=manageEquip_LOG style="display:none"><%tcWebApi_get("String_Entry","HelpCleanLogText","s")%></li>
<li id=manageEquip_2><%tcWebApi_get("String_Entry","HelpresetdefaultText","s")%></li>
</ul></td>
</tr>
<tr name=manageSyslog id=manageSyslog>
<td>&nbsp;</td>
<td class="tabdataleft">3.<%tcWebApi_get("String_Entry","HelpLogManagementText","s")%><a name=<%tcWebApi_get("String_Entry","HelpLogManagementText","s")%> id="<%tcWebApi_get("String_Entry","HelpLogManagementText","s")%>"></a></td>
</tr>
<tr name=manageSyslog_1 id=manageSyslog_1>
<td>&nbsp;</td>
<td class="tabdataleft"><ul><li id="manageSyslog_1_1"><%tcWebApi_get("String_Entry","HelpLogLevelText","s")%></li>
<li id="manageSyslog_1_2"><%tcWebApi_get("String_Entry","HelpLogShowText","s")%></li>
<li id="manageSyslog_1_3"><%tcWebApi_get("String_Entry","HelpMaintcainText","s")%></li>
</ul></td>
</tr>
<tr name=manageUpgrade id=manageUpgrade>
<td>&nbsp;</td>
<td class="tabdataleft">4.<%tcWebApi_get("String_Entry","HelpUgradeText","s")%><a name=<%tcWebApi_get("String_Entry","HelpUgradeText","s")%> id="<%tcWebApi_get("String_Entry","HelpUgradeText","s")%>"></a></td>
</tr>
<tr name=manageUpgrade_1 id=manageUpgrade_1>
<td>&nbsp;</td>
<td class="tabdataleft">
<ul>
<li id="manageSyslog_1_1"><%tcWebApi_get("String_Entry","HelpUgradeTypeText","s")%></li>
<li id="manageSyslog_1_2"><%tcWebApi_get("String_Entry","HelpUgradeFileText","s")%></li>
<li id="manageSyslog_1_3"><%tcWebApi_get("String_Entry","HelpUgradeBackUPText","s")%></li>
</ul>
</td>
</tr>
<tr name=netDiag id=netDiag>
<td id=diagText class="tabHelpdata" colspan=2 bgcolor=#CCCCCC><%tcWebApi_get("String_Entry","Help6DiagnoseText","s")%><a name=<%tcWebApi_get("String_Entry","HelpDiagnoseText","s")%>></a></td>
</tr>
<tr name=networkDiag id=networkDiag>
<td width=20>&nbsp;</td>
<td class="tabdataleft">1.<%tcWebApi_get("String_Entry","HelpNetworkDiagnoseText","s")%><a name=<%tcWebApi_get("String_Entry","HelpNetworkDiagnoseText","s")%> id="<%tcWebApi_get("String_Entry","HelpNetworkDiagnoseText","s")%>"></a></td>
</tr>
<tr name=networkDiag_1 id=networkDiag_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpTest1Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpTest2Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpInforreportText","s")%></li>
</ul>	 	</td>
</tr>
<% if tcwebApi_get("WebCustom_Entry","isCTJOYMESupported","h") = "Yes" then %>
<tr name=serviceDiag id=serviceDiag>
<td width=20>&nbsp;</td>
<td class="tabdataleft">2.<%tcWebApi_get("String_Entry","HelpBusinessDiagnosisText","s")%><a name=<%tcWebApi_get("String_Entry","HelpBusinessDiagnosisText","s")%> id="<%tcWebApi_get("String_Entry","HelpBusinessDiagnosisText","s")%>"></a></td>
</tr>
<tr name=serviceDiag_1 id=serviceDiag_1>
<td width=20>&nbsp;</td>
<td class="tabdataleft"><ul>
<li><%tcWebApi_get("String_Entry","HelpTest3Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpTest4Text","s")%></li>
<li><%tcWebApi_get("String_Entry","HelpTest5Text","s")%></li>
</ul>	 	</td>
</tr>
<% end if %>
</table>




</form>
</body>
</html>
