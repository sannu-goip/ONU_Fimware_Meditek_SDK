// JavaScript Document
var curUserName = parent.curUser;
if (typeof(curUserName) == "undefined")
	curUserName = '0';
var vPageMap = parent.pageMap;
if(parent.voipType!= null)
	var VoipType = parent.voipType;
else
	var VoipType = "SIP";
if (typeof(vPageMap) == "undefined")
	top.window.location.href="/cgi-bin/content.asp";
var sysUserName = '1';
var sptUserName = '0';
var ctcqdUserName = 'ctcqd';
var bandRegName = 'regAcc';
var usrUserName = 0;
var iCount = 0;
var IsIPv6Support = parent.IPv6Support;
if (typeof(IsIPv6Support) == "undefined")
	IsIPv6Support = 'No';
var isIpsecSupport = parent.ipsecSupport;
if (typeof(isIpsecSupport) == "undefined")
	isIpsecSupport = 'No';
var MenuArray = new Array();

var Menu_Status = top.Content_Status;
var Menu_DeviceInfo = top.Content_DeviceInfo;
var Menu_NetInfo = top.Content_NetInfo;
var Menu_UserInfo = top.Content_UserInfo;
var Menu_VoIPInfo = top.Content_VoIPInfo;
var Menu_CWMPInfo = top.Content_CWMPInfo;

var Menu_Net = top.Content_Net;
var Menu_WAN = top.Content_WAN;
var Menu_Binding = top.Content_Binding;
var Menu_LAN = top.Content_LAN;
var Menu_WLAN = top.Content_WLAN;
var Menu_WLAN5G = top.Content_WLAN5G;
var Menu_CWMP = top.Content_CWMP;
var Menu_Iphone = top.Content_Iphone;
var Menu_Time = top.Content_Time;
var Menu_Qos = top.Content_Qos;
var Menu_Route = top.Content_Route;
var Menu_QosBand = top.Content_QosBand;

var Menu_Security=top.Content_Security;
var Menu_WANSet=top.Content_WANSet;
var Menu_Firewall=top.Content_Firewall;
var Menu_MACFilter=top.Content_MACFilter;
var Menu_PortFilter=top.Content_PortFilter;
var Menu_AclFilter=top.Content_AclFilter;
var Menu_Parental=top.Content_Parental;

var Menu_APP = top.Content_APP;
var Menu_DDNS = top.Content_DDNS;
var Menu_NAT = top.Content_NAT;
var Menu_UPNP = top.Content_UPNP;
var Menu_VoIPSet = top.Content_VoIPSet;
var Menu_IGMPOrMLD = top.Content_IGMPOrMLD;
var Menu_IGMP = top.Content_IGMP;
var Menu_DailyAPP = top.Content_DailyAPP;
var Menu_SAMBA = top.Content_SAMBA;
var Menu_VPN = top.Content_VPN;
var Menu_SNMP = top.Content_SNMP;

var Menu_Management = top.Content_Management;
var Menu_UserManagement = top.Content_UserManagement;
var Menu_DeviceManagement = top.Content_DeviceManagement;
var Menu_LogManagement = top.Content_LogManagement;
var Menu_Upgrade = top.Content_Upgrade;
var Menu_Maintain = top.Content_Maintain;

var Menu_Diagnose = top.Content_Diagnose;
var Menu_InternetDiagnose = top.Content_InternetDiagnose;
var Menu_Help = top.Content_Help;
var Menu_UseHelp = top.Content_UseHelp;

//QS->0
//Sta->1
if(vPageMap[1][0] == '1'){
	if(vPageMap[1][1] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Status, "/cgi-bin/sta-device.asp", "");
	else if(vPageMap[1][2] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Status, "/cgi-bin/sta-network.asp", "");
	else if(vPageMap[1][3] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Status, "/cgi-bin/sta-user.asp", "");
	else if(vPageMap[1][4] == '1')
		if(VoipType == 'H.248')
			MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Status, "/cgi-bin/sta-VoIP248.asp", "");
		else
			MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Status, "/cgi-bin/sta-VoIP.asp", "");
	else if(vPageMap[1][5] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Status, "/cgi-bin/sta-acs.asp", "");							
}
if(vPageMap[1][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_DeviceInfo, "/cgi-bin/sta-device.asp", "");
if(vPageMap[1][2] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_NetInfo, "/cgi-bin/sta-network.asp", "");
if(vPageMap[1][3] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_UserInfo, "/cgi-bin/sta-user.asp", "");
if(vPageMap[1][4] == '1')
    if(VoipType == 'H.248')
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_VoIPInfo, "/cgi-bin/sta-VoIP248.asp", "");
	else
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_VoIPInfo, "/cgi-bin/sta-VoIP.asp", "");
if(vPageMap[1][5] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_CWMPInfo, "/cgi-bin/sta-acs.asp", "");
//Net->2
if(vPageMap[2][0] == '1'){
	if(vPageMap[2][1] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-wanset.asp", "");
	else if(vPageMap[2][2] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-binding.asp", "");
	else if(vPageMap[2][3] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-dhcp.asp", "");
	else if(vPageMap[2][4] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-wlan.asp", "");
	else if(vPageMap[2][5] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-tr069.asp", "");
	else if(vPageMap[2][9] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-phoneapp.asp", "");
	else if(vPageMap[2][6] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-qos.asp", "");
	else if(vPageMap[2][7] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-time.asp", "");
	else if(vPageMap[2][8] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/net-routeset.asp", "");	
	else if(vPageMap[2][10] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Net, "/cgi-bin/qos-dslimit.asp", "");								
}
if(vPageMap[2][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_WAN, "/cgi-bin/net-wanset.asp", "");
if(vPageMap[2][2] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Binding, "/cgi-bin/net-binding.asp", "");
if(vPageMap[2][3] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_LAN, "/cgi-bin/net-dhcp.asp", "");
if(vPageMap[2][4] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_WLAN, "/cgi-bin/net-wlan.asp", "");
if(vPageMap[2][11] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_WLAN5G, "/cgi-bin/net-wlan11ac.asp", "");
if(vPageMap[2][5] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_CWMP, "/cgi-bin/net-tr069.asp", "");
if(vPageMap[2][9] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Iphone, "/cgi-bin/net-phoneapp.asp", "");
if(vPageMap[2][6] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Qos, "/cgi-bin/net-qos.asp", "");
if(vPageMap[2][7] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Time, "/cgi-bin/net-time.asp", "");
if(vPageMap[2][8] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Route, "/cgi-bin/net-routeset.asp", "");
if(vPageMap[2][10] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_QosBand, "/cgi-bin/qos-dslimit.asp", "");
//Sec->3
if(vPageMap[3][0] == '1'){
	if(vPageMap[3][1] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Security, "/cgi-bin/sec-urlfilter.asp", "");
	else if(vPageMap[3][2] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Security, "/cgi-bin/sec-firewall.asp", "");
	else if(vPageMap[3][3] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Security, "/cgi-bin/sec-macfilter.asp", "");
	else if(vPageMap[3][4] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Security, "/cgi-bin/sec-portfilter.asp", "");				
	else if(vPageMap[3][5] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Security, "/cgi-bin/sec-aclfilter.asp", "");				
	else if(vPageMap[3][6] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Security, "/cgi-bin/sec-parental.asp", "");			
}
if(vPageMap[3][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_WANSet, "/cgi-bin/sec-urlfilter.asp", "");
if(vPageMap[3][2] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Firewall, "/cgi-bin/sec-firewall.asp", "");
if(vPageMap[3][3] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_MACFilter, "/cgi-bin/sec-macfilter.asp", "");
if(vPageMap[3][4] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_PortFilter, "/cgi-bin/sec-portfilter.asp", "");
if(vPageMap[3][5] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_AclFilter, "/cgi-bin/sec-aclfilter.asp", "");
if(vPageMap[3][6] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Parental, "/cgi-bin/sec-parental.asp", "");
//App->4
if(vPageMap[4][0] == '1'){
	if(vPageMap[4][1] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-ddns.asp", "");
	else if(vPageMap[4][2] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-natset.asp", "");
	else if(vPageMap[4][3] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-upnp.asp", "");
	else if(vPageMap[4][4] == '1')
		if(VoipType == 'H.248')
			MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-VoIP248.asp", "");
		else
			MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-VoIP.asp", "");
	else if(vPageMap[4][5] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-igmpset.asp", "");
	else if(vPageMap[4][6] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-daily.asp", "");						
	else if(vPageMap[4][7] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-samba.asp", "");								
	else if(vPageMap[4][8] == '1') {
		if(isIpsecSupport == 'Yes')
			MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-ipsecList.asp", "");
	}		
	else if(vPageMap[4][9] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/app-snmp.asp", "");	
	else if(vPageMap[4][10] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_APP, "/cgi-bin/wifi_multi_ap_basic.asp", "");
}
if(vPageMap[4][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_DDNS, "/cgi-bin/app-ddns.asp", "");
if(vPageMap[4][2] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_NAT, "/cgi-bin/app-natset.asp", "");
if(vPageMap[4][3] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_UPNP, "/cgi-bin/app-upnp.asp", "");
if(vPageMap[4][4] == '1')
	if(VoipType == 'H.248')
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_VoIPSet, "/cgi-bin/app-VoIP248.asp", "");
	else
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_VoIPSet, "/cgi-bin/app-VoIP.asp", "");
if(vPageMap[4][5] == '1')
{
	if(IsIPv6Support == 'Yes')
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_IGMPOrMLD, "/cgi-bin/app-igmpset.asp", "");
	else
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_IGMP, "/cgi-bin/app-igmpset.asp", "");
}
if(vPageMap[4][6] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_DailyAPP, "/cgi-bin/app-daily.asp", "");
	if(vPageMap[4][7] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_SAMBA, "/cgi-bin/app-samba.asp", "");
if(vPageMap[4][8] == '1') {
	if(isIpsecSupport == 'Yes')
		MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_VPN, "/cgi-bin/app-ipsecList.asp", "");
}
if(vPageMap[4][9] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_SNMP, "/cgi-bin/app-snmp.asp", "");
if(vPageMap[4][10] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, "EasyMesh", "/cgi-bin/wifi_multi_ap_basic.asp", "");
//Mag->5
if(vPageMap[5][0] == '1'){
	if(vPageMap[5][1] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Management, "/cgi-bin/mag-account.asp", "");
	else if(vPageMap[5][2] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Management, "/cgi-bin/mag-reset.asp", "");
	else if(vPageMap[5][3] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Management, "/cgi-bin/mag-syslogmanage.asp", "");		
	else if(vPageMap[5][4] == '1')
		MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Management, "/cgi-bin/upgrade.asp", "");	
}
if(vPageMap[5][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_UserManagement, "/cgi-bin/mag-account.asp", "");
if(vPageMap[5][2] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_DeviceManagement, "/cgi-bin/mag-reset.asp", "");
if(vPageMap[5][3] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_LogManagement, "/cgi-bin/mag-syslogmanage.asp", "");
if(vPageMap[5][4] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Upgrade, "/cgi-bin/upgrade.asp", "");
if(vPageMap[5][5] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_Maintain, "/cgi-bin/mag-diagnose.asp", "");	
//Diag->6
if(vPageMap[6][0] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Diagnose, "/cgi-bin/diag-quickdiagnose.asp", "");
if(vPageMap[6][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_InternetDiagnose, "/cgi-bin/diag-quickdiagnose.asp", "");
//Help->7
if(vPageMap[7][0] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(1, Menu_Help, "/cgi-bin/help.asp", "");
if(vPageMap[7][1] == '1')
	MenuArray[iCount++] = new MenuNodeConstruction(2, Menu_UseHelp, "/cgi-bin/help.asp", "");

MenuArray[iCount++] = new MenuNodeConstruction(0, "", "", "");

function MenuNodeConstruction(Level, Text, Link, Target)
{
    this.Level = Level;
    this.Text = Text;
    this.Link = Link;
    this.Target = Target;
}

function MakeMenu(Selected_Menu)
{
	var Menu = Selected_Menu.split("->");
	MakeMenu_L2(Menu[1], MakeMenu_L1(Menu[0]));
}

function MakeMenu_L1(Menu_Text)
{
	var Menu_L2_Start;
	var Code = '<table border="0" cellpadding="0" cellspacing="0" height="43px">';
	
	for (iCount = 0; MenuArray[iCount].Level != 0; iCount++)
	{
		if (MenuArray[iCount].Level == 1)
		{
			if (MenuArray[iCount].Text != Menu_Text)
			{
				Code += '<td height="15px"><img src="/img/back_button.jpg"></td>';
			}
			else
			{
				Menu_L2_Start = iCount + 1;
				Code += '<td height="15px"><img src="/img/selected_button.jpg"></td>';
			}
		}
	}
	Code += '</tr><tr>';
	for (iCount = 0; MenuArray[iCount].Level != 0; iCount++)
	{
		var str = 'LoadPage(\'' + iCount + '\')';
		if (MenuArray[iCount].Level == 1)
		{
			var s = MenuArray[iCount].Link;
			var x = s.lastIndexOf("/");
			var y = s.indexOf(".asp");
			var ss = s.substring(x+1,y);
			if (MenuArray[iCount].Text != Menu_Text)
			{
				Code += '<td bgcolor="#EF8218" height="30px"><a href="javascript:' + str + '" target="' + MenuArray[iCount].Target + '" id=' + ss + '  name=' + ss + ' class="Menu_L1_Link"><p align="center">' + MenuArray[iCount].Text + '</p></a></td>';
			}
			else
			{
				Code += '<td bgcolor="#427594" height="30px"><a href="javascript:' + str + '" target="' + MenuArray[iCount].Target + '" id=' + ss + '  name=' + ss + ' class="Menu_L1_Active"><p align="center">' + MenuArray[iCount].Text + '</p></a></td>';
			}
		}
	}
	Code += '</tr></table>';
	getElement('MenuArea_L1').innerHTML = Code;
	return Menu_L2_Start;
}

function LoadPage(strIndex)
{
	var index = parseInt(strIndex);
	location = MenuArray[index].Link;
}

function MakeMenu_L2(Menu_Text, Start)
{
	var Code = '<table border="0" cellpadding="0" cellspacing="0" height="15px"><tr><td width="10px"><td width="7px" class="Menu_L2_Link"><p>|</p></td>';
	for (var iCount = Start; (MenuArray[iCount].Level != 0) && (MenuArray[iCount].Level != 1); iCount++)
	{
		var str = 'LoadPage(\'' + iCount + '\')';
		if (MenuArray[iCount].Level == 2)
		{
			var s = MenuArray[iCount].Link;
			var x = s.lastIndexOf("/");
			var y = s.indexOf(".asp");
			var ss = s.substring(x+1,y);
			if (MenuArray[iCount].Text != Menu_Text)
			{				
				Code += '<td height="30px"><a href="javascript:' + str + '" target="' + MenuArray[iCount].Target + '" id=' + ss + '  name=' + ss + ' class="Menu_L2_Link"><p align="center"> ' + MenuArray[iCount].Text + ' </p></a></td>';
			}
			else
			{
				Code += '<td height="30px"><a href="javascript:' + str + '" target="' + MenuArray[iCount].Target + '" id=' + ss + '  name=' + ss + ' class="Menu_L2_Active"><p align="center"> ' + MenuArray[iCount].Text + ' </p></a></td>';
			}
			Code += '<td width="7px" class="Menu_L2_Link"><p>|</p></td>';
		}
	}
	Code += '</tr></table>';
	getElement('MenuArea_L2').innerHTML = Code;
}
 
function DisplayLocation(Selected_Menu)
{
	var Menu = Selected_Menu.split("->");
	getElement('LocationDisplay').innerHTML = Menu[0];
}
