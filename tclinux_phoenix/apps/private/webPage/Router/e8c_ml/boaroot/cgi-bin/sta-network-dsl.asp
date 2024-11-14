<%
	If Request_Form("Saveflag")="1" then
		tcWebApi_set("DeviceInfo_PVC","DipNasName","DipNasName")
		If Request_Form("Dipflag")="1" then
			tcWebApi_set("DeviceInfo_PVC","DispBtnType","Dipflag")
			tcWebApi_commit("DeviceInfo_PVC")
		elseif Request_Form("Dipflag")="2" then
			tcWebApi_set("DeviceInfo_PVC","DispBtnType","Dipflag")
			tcWebApi_commit("DeviceInfo_PVC")
		elseif Request_Form("Dipflag")="3" then
			tcWebApi_set("DeviceInfo_PVC","DispBtnType","Dipflag")
			tcWebApi_set("DeviceInfo_PVC","IP6DispBtnType","Dipflag")
			tcWebApi_commit("DeviceInfo_PVC")
		elseif Request_Form("Dipflag")="4" then
			tcWebApi_set("DeviceInfo_PVC","DispBtnType","Dipflag")
			tcWebApi_set("DeviceInfo_PVC","IP6DispBtnType","Dipflag")
			tcWebApi_commit("DeviceInfo_PVC")
		END If 
	END If
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","NetInfoNSInText","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<style type='text/css'>
.cus_help {
	width:170px;	
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
    <FORM name=ConfigForm action="/cgi-bin/sta-network-dsl.asp" method="post">
    	<input type="hidden" id=Dipflag name=Dipflag value="0">
    	<input type="hidden" id=Saveflag name=Saveflag value="0">
    	<input type="hidden" id=DipNasName name=DipNasName value="0">
    </FORM>
      <TABLE height=90 cellSpacing=0 cellPadding=0 width=808 
      background=/img/framelogo.jpg border=0>
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
                  href="/cgi-bin/logout.cgi" id=logOut name=logOut target=_top><SPAN 
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
            value="<%tcWebApi_get("String_Entry","ContentStatusText","s")%>-><%tcWebApi_get("String_Entry","ContentNetInfoText","s")%>" name=Sta_Net> </FONT></B><SPAN 
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
      <SCRIPT language=JavaScript src="/JS/printtable.js"></SCRIPT>

      <SCRIPT language=JavaScript type=text/javascript>
	 	var userState = 1;
	 	if (curUserName == sptUserName)
			userState = 0;
	 	var para = 0;
		var pvc_counts=<% tcWebApi_get("WanInfo_Common","CycleNum","s") %>;
		//get all value
		// num 0
		var vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_WanName = vArrayStr.split(',');
		// num 1
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_WanNas = vArrayStr.split(',');
		// num 2
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		// num 3
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		Wan_Actives = vArrayStr.split(',');
		// num 4
		// num 5		
		<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "2") %>
		// num 6		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_IPVERSION = vArrayStr.split(',');
		// num 7		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_Status4 = vArrayStr.split(',');
		// num 8		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_IP4 = vArrayStr.split(',');
		// num 9		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_ENCAP = vArrayStr.split(',');
		// num 10		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_GateWay4 = vArrayStr.split(',');
		// num 11		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_DNS4 = vArrayStr.split(',');
		// num 12		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_SecDNS4 = vArrayStr.split(',');
		// num 13		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_Status6 = vArrayStr.split(',');
		// num 14		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_IP6 = vArrayStr.split(',');
		// num 15		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_GateWay6 = vArrayStr.split(',');
		// num 16		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_DNS6 = vArrayStr.split(',');
		// num 17		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_SecDNS6 = vArrayStr.split(',');
		// num 18		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_PD6 = vArrayStr.split(',');
		// num 19		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_NetMask4 = vArrayStr.split(',');
		// num 20		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_VPI = vArrayStr.split(',');
		// num 21		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_VCI = vArrayStr.split(',');
		// num 22		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_Connection = vArrayStr.split(',');
		// num 23		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_VidPRI = vArrayStr.split(',');
		// num 24		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_MAC = vArrayStr.split(',');
		// num 25
		<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "1") %>
		// num 26
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_ObtMode = vArrayStr.split(',');
		// num 27
		<% tcWebApi_constSet("WanInfo_Common", "CycleJump", "1") %>
		// num 28
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_TransMode = vArrayStr.split(',');

		SortUtil.SetReplaceStrflag(1);
		Wan_WanName = SortUtil.UpdateWanIFName( Wan_WanName, pvc_counts );
		Wan_WanNas = SortUtil.SortMyArray( Wan_WanNas );
		Wan_Actives = SortUtil.SortMyArray( Wan_Actives );
		Wan_IPVERSION = SortUtil.SortMyArray( Wan_IPVERSION );
		Wan_Status4 = SortUtil.SortMyArray( Wan_Status4 );
		Wan_IP4 = SortUtil.SortMyArray( Wan_IP4 );
		Wan_ENCAP = SortUtil.SortMyArray( Wan_ENCAP );
		Wan_GateWay4 = SortUtil.SortMyArray( Wan_GateWay4 );
		Wan_DNS4 = SortUtil.SortMyArray( Wan_DNS4 );
		Wan_SecDNS4 = SortUtil.SortMyArray( Wan_SecDNS4 );
		Wan_Status6 = SortUtil.SortMyArray( Wan_Status6 );
		Wan_IP6 = SortUtil.SortMyArray( Wan_IP6 );
		Wan_GateWay6 = SortUtil.SortMyArray( Wan_GateWay6 );
		Wan_DNS6 = SortUtil.SortMyArray( Wan_DNS6 );
		Wan_SecDNS6 = SortUtil.SortMyArray( Wan_SecDNS6 );
		Wan_PD6 = SortUtil.SortMyArray( Wan_PD6 );
		Wan_NetMask4 = SortUtil.SortMyArray( Wan_NetMask4 );
		Wan_VPI = SortUtil.SortMyArray( Wan_VPI );
		Wan_VCI = SortUtil.SortMyArray( Wan_VCI );
		Wan_Connection = SortUtil.SortMyArray( Wan_Connection );
		Wan_VidPRI = SortUtil.SortMyArray( Wan_VidPRI );
		Wan_MAC = SortUtil.SortMyArray( Wan_MAC );
		Wan_ObtMode = SortUtil.SortMyArray( Wan_ObtMode );
		Wan_TransMode = SortUtil.SortMyArray( Wan_TransMode );

		function LoadFrame()
		{
			return;
		}

		var EthernetState = '<% tcWebApi_get("WanInfo_Common","EthernetState","s") %>';

		function renewrelease(ip, nasIndex)
		{  
			 document.ConfigForm.Dipflag.value = ip;
			 document.ConfigForm.Saveflag.value = 1;
			 document.ConfigForm.DipNasName.value = Wan_WanNas[nasIndex];
			 if( true == setEBooValueCookie(document.ConfigForm) )
			 	document.ConfigForm.submit();  
		}
		
		function ListCreate_info1(para)
		{
				var index = 1;
				for(var i=0;i<pvc_counts;i++)
				{
					if(Wan_Actives[i] != "N/A")
					{
						if(para == 6)
						{

							if(Wan_IPVERSION[i] == "IPv6" || Wan_IPVERSION[i] == "IPv4/IPv6")
							{

								document.write('<tr align="middle">');
								document.write('<td>' + Wan_WanName[i] + '</td>');
								var isIPGWUp = 0;
								if ( 0 != Wan_IP6[i].length && '-' != Wan_IP6[i]
									&& 0 != Wan_GateWay6[i].length && '-' != Wan_GateWay6[i] )
									isIPGWUp = 1;
								if(Wan_WanName[i].indexOf('_B_') >= 0)
									isIPGWUp = 1;
<% if TCWebApi_get("Wan_Common","TransMode","h" ) = "Ethernet" then %>
								if(EthernetState == "up"){
<% else %>
								if(DslInfo.status == "up"){
<% end if %>
										if(Wan_Status6[i] == "up" && (1 == isIPGWUp))
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectedText","s")%>&nbsp;</td>');
										else if(Wan_Actives[i]=="Yes")											
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectingText","s")%>&nbsp;</td>');
										else
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
									}
									else
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
									
									document.write('<td>' + Wan_IP6[i] + '&nbsp;</td>');
									document.write('<td>' + Wan_GateWay6[i] + '&nbsp;</td>');
									if((DslInfo.status == "up") && ('PPPoE' != Wan_ObtMode[i]))
									{
										document.write('<td align="middle" valign="middle"><INPUT id=btnReleaseIP name=btnReleaseIP onclick="renewrelease(4, '+ i +')" type=button value=ReleaseIP></td>');
										document.write('<td align="middle" valign="middle"><INPUT id=btnRenewIP name=btnRenewIP onclick="renewrelease(3, '+ i +')" type=button value=RenewIP></td>');
									}
									else
									{
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
									}
								document.write('</tr>');
								index = index + 1;
							}
							else 
							{
								if(Wan_IPVERSION[i] == "IPv4")
								{
									index = index + 1;
								}
							}
						}
						else 
						{
							if(para == 4)
							{
								if(Wan_IPVERSION[i] == "IPv4" || Wan_IPVERSION[i] == "IPv4/IPv6")
								{
									document.write('<tr align="middle">');
									document.write('<td>' + Wan_WanName[i] + '</td>');
<% if TCWebApi_get("Wan_Common","TransMode","h" ) = "Ethernet" then %>
									if(EthernetState == "up"){
<% else %>
									if(DslInfo.status == "up"){
<% end if %>
										if(Wan_Status4[i] == "up")
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectedText","s")%>&nbsp;</td>');
										else if(Wan_Actives[i]=="Yes")											
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectingText","s")%>&nbsp;</td>');
										else
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
									}
									else
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
									
									document.write('<td>' + Wan_IP4[i] + '&nbsp;</td>');
									document.write('<td>' + Wan_NetMask4[i] + '&nbsp;</td>');
									if((DslInfo.status == "up") && ('PPPoE' != Wan_ObtMode[i]))
									{
										document.write('<td align="middle" valign="middle"><INPUT id=btnReleaseIP name=btnReleaseIP onclick="renewrelease(2, '+ i +')" type=button value=ReleaseIP></td>');
										document.write('<td align="middle" valign="middle"><INPUT id=btnRenewIP name=btnRenewIP onclick="renewrelease(1, '+ i +')" type=button value=RenewIP></td>');
									}
									else
									{
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
									}

									document.write('</tr>');
									index = index + 1;
								}
								else 
								{
									if(Wan_IPVERSION[i] == "IPv6")
									{
										index = index + 1;
									}
								}
							}
						}
					}
				}
			}
			
		function ListCreate_info2(para)
		{
				var index = 1;
				var encap;
				var destaddr;
				for(var i=0;i<pvc_counts;i++)
				{
					if(Wan_Actives[i] != "N/A")
					{
						if ( 1 == Wan_TransMode[i] )
						{
							encap = Wan_ENCAP[i];
							destaddr = Wan_VPI[i] + '/' + Wan_VCI[i];
						}
						else
						{
							destaddr = encap = '-';
						}

						if(para == 6)
						{
							if(Wan_IPVERSION[i] == "IPv6" || Wan_IPVERSION[i] == "IPv4/IPv6")
							{
								document.write('<tr align="middle">');
								document.write('<td>' + Wan_WanName[i] + '</td>');
								if ( 'Connect_Manually' == Wan_Connection[i] )
									document.write('<td> <%tcWebApi_get("String_Entry","NetInfoManualText","s")%> </td>');
								else
									document.write('<td> <%tcWebApi_get("String_Entry","NetInfoAutomaticText","s")%> </td>');
								document.write('<td>' + destaddr + '</td>');
								document.write('<td>' + encap + '&nbsp;</td>');
								document.write('</tr>');
								index = index + 1;
							}
							else 
							{
								if(Wan_IPVERSION[i] == "IPv4")
								{
									index = index + 1;
								}
							}
						}
						else 
						{
							if(para == 4)
							{
								if(Wan_IPVERSION[i] == "IPv4" || Wan_IPVERSION[i] == "IPv4/IPv6")
								{
									document.write('<tr align="middle">');
									document.write('<td>' + Wan_WanName[i] + '</td>');
									if ( 'Connect_Manually' == Wan_Connection[i] )
										document.write('<td> <%tcWebApi_get("String_Entry","NetInfoManualText","s")%> </td>');
									else
										document.write('<td> <%tcWebApi_get("String_Entry","NetInfoAutomaticText","s")%> </td>');
									document.write('<td>' + destaddr + '</td>');
									document.write('<td>' + encap + '&nbsp;</td>');
									document.write('</tr>');
									index = index + 1;
								}
								else 
								{
									if(Wan_IPVERSION[i] == "IPv6")
									{
										index = index + 1;
									}
								}
							}
						}
					}
				}
			}
						
		function ListCreate_info2p(para)
		{
			var obtmode;
			for( var i=0; i<pvc_counts; i++ )
			{
				if( Wan_Actives[i] != 'N/A' )
				{
					obtmode = ( 0 == Wan_ObtMode[i].length ) ? '-' : Wan_ObtMode[i];
					if( para == 6 )
					{
						if( Wan_IPVERSION[i] == 'IPv4')
							continue;
					}
					else if ( para == 4 )
					{
						if( Wan_IPVERSION[i] == 'IPv6')
							continue;	
					}
					else
							return;

					document.write('<tr align="middle">');
					document.write('<td>' + Wan_WanName[i] + '</td>');
					document.write('<td>' + obtmode + '&nbsp;</td>');
					document.write('<td>' + Wan_VidPRI[i] + '</td>');
					document.write('<td>' + Wan_MAC[i] + '&nbsp;</td>');
					document.write('</tr>');
				}
			}
		}
		
		function ListCreate_info3(para)
		{
				var index = 1;
				for(var i=0;i<pvc_counts;i++)
				{
					if(Wan_Actives[i] != "N/A")
					{
						if(para == 6)
						{
							if(Wan_IPVERSION[i] == "IPv6" || Wan_IPVERSION[i] == "IPv4/IPv6")
							{
								document.write('<tr align="middle">');
								document.write('<td>' + Wan_WanName[i] + '</td>');
								document.write('<td>' + Wan_DNS6[i] + '&nbsp;</td>');
								document.write('<td>' + Wan_SecDNS6[i] + '&nbsp;</td>');
								document.write('<td>' + Wan_PD6[i] + '&nbsp;</td>');

								document.write('</tr>');
								index = index + 1;
							}
							else 
							{
								if(Wan_IPVERSION[i] == "IPv4")
								{
									index = index + 1;
								}
							}
						}
						else 
						{
							if(para == 4)
							{
								if(Wan_IPVERSION[i] == "IPv4" || Wan_IPVERSION[i] == "IPv4/IPv6")
								{
									document.write('<tr align="middle">');
									document.write('<td>' + Wan_WanName[i] + '</td>');
									document.write('<td>' + Wan_GateWay4[i] + '&nbsp;</td>');
									document.write('<td>' + Wan_DNS4[i] + '&nbsp;</td>');
									document.write('<td>' + Wan_SecDNS4[i] + '&nbsp;</td>');

									document.write('</tr>');
									index = index + 1;
								}
								else 
								{
									if(Wan_IPVERSION[i] == "IPv6")
									{
										index = index + 1;
									}
								}
							}
						}
					}
				}
			}

<% if tcWebApi_get("WebCustom_Entry", "isCYSupported", "h") = "Yes"  then %>
function stDsl(domain,Modulation,status,UpCurrRate,DownCurrRate,UpMaxCurrRate,DownMaxCurrRate,UpMargin,DownMargin,UpDepth,DownDepth,Encoding,UpAttenuation,DownAttenuation,UpPower,DownPower,DataPath)
<% else %>
function stDsl(domain,Modulation,status,UpCurrRate,DownCurrRate,UpMargin,DownMargin,UpDepth,DownDepth,Encoding,UpAttenuation,DownAttenuation,UpPower,DownPower,DataPath)
<% end if %>
{
	this.domain = domain;
	this.status = status;
	this.Encoding = Encoding;
	
	if(this.status == "up")
	{
		this.Modulation = Modulation;	 
		this.UpCurrRate = UpCurrRate; 
		this.DownCurrRate = DownCurrRate; 
<% if tcWebApi_get("WebCustom_Entry", "isCYSupported", "h") = "Yes"  then %>
		this.UpMaxCurrRate = UpMaxCurrRate; 
		this.DownMaxCurrRate = DownMaxCurrRate; 
<% end if %>
		this.UpMargin = UpMargin; 
		this.DownMargin = DownMargin;
	 	this.UpDepth = UpDepth; 
		this.DownDepth = DownDepth;	
		this.UpAttenuation = UpAttenuation;
		this.DownAttenuation = DownAttenuation;
		this.UpPower = UpPower;
		this.DownPower = DownPower;
		this.DataPath = DataPath;
	}
	else
	{
		this.Modulation = "-";	 
		this.UpCurrRate = "-"; 
		this.DownCurrRate = "-"; 
<% if tcWebApi_get("WebCustom_Entry", "isCYSupported", "h") = "Yes"  then %>
		this.UpMaxCurrRate = "-"; 
		this.DownMaxCurrRate = "-";
<% end if %>
		this.UpMargin = "-"; 
		this.DownMargin = "-";
	 	this.UpDepth = "-"; 
		this.DownDepth = "-";	
		this.UpAttenuation = "-";
		this.DownAttenuation = "-";
		this.UpPower = "-";
		this.DownPower = "-";
		this.DataPath = "-";
	}
}
<% if tcWebApi_get("WebCustom_Entry", "isCYSupported", "h") = "Yes"  then %>
var DSLCfg = new Array(new stDsl("0","<% tcWebApi_get("Info_Adsl","Opmode","s") %>","<% tcWebApi_get("Info_Adsl","lineState","s") %>","<% tcWebApi_get("Info_Adsl","DataRateUp","s") %>","<% tcWebApi_get("Info_Adsl","DataRateDown","s") %>","<% tcWebApi_get("Info_Adsl","AttainUp","s") %>","<% tcWebApi_get("Info_Adsl","AttainDown","s") %>","<% tcWebApi_get("Info_Adsl","SNRMarginUp","s") %>","<% tcWebApi_get("Info_Adsl","SNRMarginDown","s") %>","<% tcWebApi_get("Info_Adsl","InterleaveDepth","s") %>","<% tcWebApi_get("Info_Adsl","InterleaveDepthDown","s") %>","<% tcWebApi_get("Adsl_Entry","MODULATIONTYPE","s") %>","<% tcWebApi_get("Info_Adsl","AttenUp","s") %>","<% tcWebApi_get("Info_Adsl","AttenDown","s") %>","<% tcWebApi_get("Info_Adsl","PowerUp","s") %>","<% tcWebApi_get("Info_Adsl","PowerDown","s") %>","<% tcWebApi_get("Info_Adsl","mtenStandard","s") %>"));
<% else %>
var DSLCfg = new Array(new stDsl("0","<% tcWebApi_get("Info_Adsl","Opmode","s") %>","<% tcWebApi_get("Info_Adsl","lineState","s") %>","<% tcWebApi_get("Info_Adsl","DataRateUp","s") %>","<% tcWebApi_get("Info_Adsl","DataRateDown","s") %>","<% tcWebApi_get("Info_Adsl","SNRMarginUp","s") %>","<% tcWebApi_get("Info_Adsl","SNRMarginDown","s") %>","<% tcWebApi_get("Info_Adsl","InterleaveDepth","s") %>","<% tcWebApi_get("Info_Adsl","InterleaveDepthDown","s") %>","<% tcWebApi_get("Adsl_Entry","MODULATIONTYPE","s") %>","<% tcWebApi_get("Info_Adsl","AttenUp","s") %>","<% tcWebApi_get("Info_Adsl","AttenDown","s") %>","<% tcWebApi_get("Info_Adsl","PowerUp","s") %>","<% tcWebApi_get("Info_Adsl","PowerDown","s") %>","<% tcWebApi_get("Info_Adsl","mtenStandard","s") %>"));
<% end if %>
var DslInfo = DSLCfg[0];

</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NetInfoIPv4ConnectionText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top background=/img/panel4.gif class=cus_help
          rowSpan=12>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0>
              <TBODY>
              <TR>
                <TD><A 
                  href="/cgi-bin/help_content.asp#网络侧信息" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
                  width=40 border=0></A></TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2>&nbsp;</P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=3 width='100%'  border=1>
                    <TBODY>
                    <TR align=middle>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoStatusText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoIPAddressText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoSubnetMaskText","s")%></TD>
                      <TD class=table_title align=middle>ReleaseIP</TD>
                      <TD class=table_title align=middle>RenewIP</TD></TR>
                    <SCRIPT language=JavaScript type=text/javascript>
						ListCreate_info1(4);
					</SCRIPT>
                    </TBODY></TABLE><BR>
                  <TABLE id=tabOptionv4 cellSpacing=0 cellPadding=3 width='100%' 
                  border=1 name="tabOptionv4">
                    <TBODY>
                    <TR align=middle>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionText","s")%></TD>
                      <TD class=table_title id=secPVC align=middle>VPI/VCI</TD>
                      <TD class=table_title id=secEncap align=middle><%tcWebApi_get("String_Entry","PackagingInformationText","s")%></TD></TR>
                    <SCRIPT language=JavaScript type=text/javascript>
						ListCreate_info2(4);
					</SCRIPT>
                    </TBODY></TABLE><BR>
                    <TABLE cellSpacing=0 cellPadding=3 width='100%' border=1>
                    <TBODY>
                    <TR align=middle>
                    	<TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                    	<TD class=table_title align=middle><%tcWebApi_get("String_Entry","DevInfoIPAddrAWaText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoVLANPText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoMACAddressText","s")%></TD>
                      </TR>
							        <SCRIPT language=JavaScript type=text/javascript>
											ListCreate_info2p(4);
											</SCRIPT>
                  </TBODY></TABLE><BR>
                  <TABLE cellSpacing=0 cellPadding=3 border=1 width='100%' >
                    <TBODY>
                    <TR>
                      <TD class=table_title align=middle width=180><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle width=208><%tcWebApi_get("String_Entry","NetInfoDeGaText","s")%></TD>
                      <TD class=table_title align=middle width=208><%TCWEBAPI_GET("String_Entry","NetInfoPrDNSText","s")%></TD>
                      <TD class=table_title align=middle width=208><%TCWEBAPI_GET("String_Entry","NetInfoSeDNSText","s")%></TD></TR>
                    <SCRIPT language=javascript>
						ListCreate_info3(4);
					</SCRIPT>
                    </TBODY></TABLE></TD>
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
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NetInfoIPv6CoText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>         
          <TD width=474>　</TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2>&nbsp;</P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=3 width='100%' border=1>
                    <TBODY>
                    <TR align=middle>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoIPv6StatusText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoIPv6AddressText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoIPv6DeGText","s")%></TD>
                      <TD class=table_title align=middle>ReleaseIP</TD>
                      <TD class=table_title align=middle>RenewIP</TD></TR>
                    <SCRIPT language=JavaScript type=text/javascript>
						ListCreate_info1(6);
					</SCRIPT>
                    </TBODY></TABLE><BR>
                  <TABLE id=tabOptionv6 cellSpacing=0 cellPadding=3 width='100%'
                  border=1 name="tabOptionv6">
                    <TBODY>
                    <TR align=middle>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionText","s")%></TD>
                      <TD class=table_title id=secPVC align=middle>VPI/VCI</TD>
                      <TD class=table_title id=secEncap align=middle><%tcWebApi_get("String_Entry","PackagingInformationText","s")%></TD></TR>
                    <SCRIPT language=JavaScript type=text/javascript>
						ListCreate_info2(6);
					</SCRIPT>
                    </TBODY></TABLE><BR>
                    <TABLE cellSpacing=0 cellPadding=3 width='100%' border=1>
                    <TBODY>
                    <TR align=middle>
                    	<TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                    	<TD class=table_title align=middle><%tcWebApi_get("String_Entry","DevInfoIPAddrAWaText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoVLANPText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoMACAddressText","s")%></TD>
                      </TR>
							        <SCRIPT language=JavaScript type=text/javascript>
											ListCreate_info2p(6);
											</SCRIPT>
                  </TBODY></TABLE><BR>
                  <TABLE cellSpacing=0 cellPadding=3 border=1 width='100%'>
                    <TBODY>
                    <TR>
                      <TD class=table_title align=middle width=180><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle 
width=208><%tcWebApi_get("String_Entry","NetInfoIPv6PrDnsText","s")%></TD>
                      <TD class=table_title align=middle 
width=208><%tcWebApi_get("String_Entry","NetInfoIPv6SeDnsText","s")%></TD>
                      <TD class=table_title align=middle width=208><%tcWebApi_get("String_Entry","NetInfoGetprefixText","s")%></TD></TR>
                    <SCRIPT language=javascript>
						ListCreate_info3(6);
					</SCRIPT>
                    </TBODY></TABLE></TD>
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
        <TR>
<% if tcwebApi_get("Wan_Common","TransMode","h") <> "Ethernet" then %>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%TCWEBAPI_GET("String_Entry","NetInfoPONLiCText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
<% else %>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
<% end if %>
          <TD width=474>　</TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2>&nbsp;</P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <DIV id=secDslInfo>
<% if tcwebApi_get("Wan_Common","TransMode","h") <> "Ethernet" then %>
                  <TABLE cellSpacing=0 cellPadding=3 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title width=208><%tcWebApi_get("String_Entry","DevInfoDSLCurrentSyncStatusText","s")%></TD>
                                <TD width=180> <script language=JavaScript type=text/javascript>
						if ( DslInfo.status == "down")
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoFreeText","s")%>");
						}
						else if( DslInfo.status == "initializing")
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoHandShakeText","s")%>");
						}
						else if( DslInfo.status == "EstablishingLink")
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoTrainingText","s")%>");
						}
						else if( DslInfo.status == "up")
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoActivationText","s")%>");
						}
						else
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoActivatingText","s")%>");
						}
						</script>
                                  &nbsp;</TD>
                              </TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoConStatText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						if(DslInfo.status == "up")
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoActivationText","s")%>"));
						}
						else
						{
							document.write("<%tcWebApi_get("String_Entry","DevInfoActivatingText","s")%>");
						}
						</SCRIPT>
                        &nbsp;</TD></TR></TBODY></TABLE></DIV>
                  <DIV id=secDslStat><BR>
                  <TABLE cellSpacing=0 cellPadding=3 border=1>
                    <SCRIPT language=javascript>
					document.write("<tr>");
					document.write("<td class='table_title' width='200'><%tcWebApi_get("String_Entry","DevInfoLineSpeedUpText","s")%></td>");
					document.write("<td width='180'>" + DslInfo.UpCurrRate + "</td>");
					document.write("</tr>" );
					document.write("<tr>");
					document.write("<td class='table_title'><%tcWebApi_get("String_Entry","DevInfoLineSpeedDownText","s")%></td>");
					document.write("<td>" + DslInfo.DownCurrRate + "</td>");
					document.write("</tr>");
<% if tcWebApi_get("WebCustom_Entry", "isCYSupported", "h") = "Yes"  then %>
					document.write("<tr>");
					document.write("<td class='table_title'><%tcWebApi_get("String_Entry","DevInfoMaxReachableRateUpText","s")%></td>");
					document.write("<td>" + DslInfo.UpMaxCurrRate + "</td>");
					document.write("</tr>");
					document.write("<tr>");
					document.write("<td class='table_title'><%tcWebApi_get("String_Entry","DevInfoMaxReachableRateDownText","s")%></td>");
					document.write("<td>" + DslInfo.DownMaxCurrRate + "</td>");
					document.write("</tr>");
<% end if%>
					</SCRIPT>

                    <TBODY>
                    <TR>
                      <TD class=table_title width=208><%tcWebApi_get("String_Entry","DevInfoNoiseMarginUpText","s")%>&nbsp;</TD>
                      <TD width=180>
                        <SCRIPT language=JavaScript type=text/javascript>
						document.write(DslInfo.UpMargin);
						</SCRIPT>
                        &nbsp;</TD></TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoNoiseMarginDownText","s")%>&nbsp;</TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						document.write(DslInfo.DownMargin);
						</SCRIPT>
					</TD></TR>
						<SCRIPT language=javascript>
						var mode = DslInfo.Modulation;
						if (mode != "")
						{
						document.writeln("<tr><td class='table_title'><nobreak><%tcWebApi_get("String_Entry","DevInfoInterleavingDepthUpText","s")%></nobreak></td>");
						document.writeln("<td>" + DslInfo.UpDepth + "&nbsp;</td></tr>");
						document.writeln("<tr><td class='table_title'><nobreak><%tcWebApi_get("String_Entry","DevInfoInterleaveDepthDownText","s")%></nobreak></td>");
						document.writeln("<td>" + DslInfo.DownDepth + "&nbsp;</td></tr>");
						}
						</SCRIPT>

                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoLiprotocolText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						if ( DslInfo.Encoding == "Auto Sync-Up")
						{
							document.write("Auto Sync-Up");
						}
						else if( DslInfo.Encoding == "ADSL2+")
						{
							document.write("ADSL2+");
						}
						else if( DslInfo.Encoding == "ADSL2")
						{
							document.write("ADSL2");
						}
						else if( DslInfo.Encoding == "G.DMT")
						{
							document.write("G.DMT");
						}
						else if( DslInfo.Encoding == "T1.413")
						{
							document.write("T1.413");
						}
						else if( DslInfo.Encoding == "G.lite")
						{
							document.write("G.lite");
						}
						else
						{
							document.write("未知");
						}
						</SCRIPT>
                        &nbsp;</TD></TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoLineAttenuationUpText","s")%>&nbsp;</TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						document.write(DslInfo.UpAttenuation);
						</SCRIPT>
                        &nbsp;</TD></TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoLineAttenuationDownText","s")%>&nbsp;</TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						document.write(DslInfo.DownAttenuation);
						</SCRIPT>
                        &nbsp;</TD></TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoOutputPowerUpText","s")%>&nbsp;</TD>
                                <TD> <script language=JavaScript type=text/javascript>
						document.write(DslInfo.UpPower);
						</script>
                                  &nbsp;</TD>
                              </TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoOutputPowerDownText","s")%>&nbsp;</TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						document.write(DslInfo.DownPower);
						</SCRIPT>
                        &nbsp;</TD></TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","DevInfoChannelTypeText","s")%>&nbsp;</TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
						document.write(DslInfo.DataPath);
						</SCRIPT>
                        &nbsp;</TD></TR></TBODY></TABLE>
<% end if %>
		</DIV></TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>&nbsp;</TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR></TBODY></TABLE>
	  <SCRIPT language=JavaScript type=text/javascript>
	  if ( userState )
		secDslStat.style.display = "none";
	  </SCRIPT>
    </TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD bgColor=#427594>　</TD>
          <TD bgColor=#427594>
            <P align=center>&nbsp;</P></TD>
          <TD  class=cus_help
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
