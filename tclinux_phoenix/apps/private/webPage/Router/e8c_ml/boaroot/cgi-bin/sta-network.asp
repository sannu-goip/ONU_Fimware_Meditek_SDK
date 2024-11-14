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

<META content="MSHTML 6.00.6000.16809" name=GENERATOR>
<style>
	TABLE {
		FONT-SIZE: 12px;
		FONT-FAMILY: "system-ui";
	 }
</style>

</HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ConfigForm') != null)LoadFrame()" 
onunload=DoUnload() marginheight="0" marginwidth="0">
<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center 
border=0>
  <TBODY>
  <TR>
    <TD height=1>
    <FORM name=ConfigForm action="/cgi-bin/sta-network.asp" method="post">
    	<input type="hidden" id=Dipflag name=Dipflag value="0">
    	<input type="hidden" id=Saveflag name=Saveflag value="0">
    	<input type="hidden" id=DipNasName name=DipNasName value="0">
    </FORM>
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
            <P align=right><FONT face=菜悶 color=#ffffff><B><FONT face=菜悶 
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
<% if tcwebApi_get("WebCustom_Entry","isCTSFUC9Supported","h") = "Yes" then %>		
		pvc_counts = 1;
<% end if %>
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
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var WANEnNAT = vArrayStr.split(',');
		// num 5		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
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
		// num 21		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		// num 22		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_Connection = vArrayStr.split(',');
		// num 23		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_VidPRI = vArrayStr.split(',');
		// num 24		
		vArrayStr = "<% tcWebApi_get("WanInfo_Common","CycleValue","s") %>";
		var Wan_MAC = vArrayStr.split(',');

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
		Wan_Connection = SortUtil.SortMyArray( Wan_Connection );
		Wan_VidPRI = SortUtil.SortMyArray( Wan_VidPRI );
		Wan_MAC = SortUtil.SortMyArray( Wan_MAC );

		function LoadFrame()
		{
			return;
		}

		function transTemperature(temperature)
		{
			var temp = Number(temperature);
			if (temp >= Math.pow(2, 15)){
				<% if tcwebApi_get("WebCustom_Entry","isCTPONGDTemperatureSupported","h") = "Yes" then %>	
					return -((Math.pow(2, 16)-temp)/256).toFixed(1);
				<% else %>
					return -Math.round((Math.pow(2, 16)-temp)/256);
				<% end if %>
			}else{
				<% if tcwebApi_get("WebCustom_Entry","isCTPONGDTemperatureSupported","h") = "Yes" then %>	
					return (temp/256).toFixed(1);
				<% else %>
					return Math.round(temp/256);
				<% end if %>
			}
		}			
			
		function PonInfoClass()
		{
			var SupplyVoltage = '<%tcWebApi_get("Info_PonPhy", "SupplyVoltage", "s")%>';
			var TxBiasCurrent = '<%tcWebApi_get("Info_PonPhy", "TxBiasCurrent", "s")%>';
			var Temperature = '<%tcWebApi_get("Info_PonPhy", "Temperature", "s")%>';
			var TxPower = '<%tcWebApi_get("Info_PonPhy", "TxPower", "s")%>';
			var RxPower = '<%tcWebApi_get("Info_PonPhy", "RxPower", "s")%>';
			this.LinkSta					= '<% tcWebApi_get("XPON_LinkCfg","LinkSta","s") %>';
			this.trafficstate			= '<% tcWebApi_get("XPON_Common","trafficStatus","s") %>';
			this.fecState					= '<% tcWebApi_get("Info_PonPhy","FecStatus","s") %>';
			this.ponType					= '<% tcWebApi_get("XPON_Common","xponMode","s") %>';
			this.loidStatus				= 'init';
			this.PonState 				= 'down';
			this.FECEnable 				= '-';
			this.WarnInfo 				= 'disconnect';
			this.PonSendPkt 			= '<% tcWebApi_get("Info_PonWanStats","TxFrameCnt","s") %>';
			this.PonRecvPkt 			= '<% tcWebApi_get("Info_PonWanStats","RxFrameCnt","s") %>';
			this.SendPower 				= '-';
			this.RecvPower 				= '-';
			<% if SupplyVoltage <> "N/A" then %>	
			this.WorkVoltage 			= (Number(SupplyVoltage)/10);
			<% else %>
			this.WorkVoltage 			= '-';
			<% end if %>
			<% if TxBiasCurrent <> "N/A" then %>	
			this.WorkElectric 		= (Number(TxBiasCurrent)*2/1000);
			<% else %>
			this.WorkElectric 		= '-';
			<% end if %>
			<% if Temperature <> "N/A" then %>	
			this.WorkTemperature 	= transTemperature(Temperature);
			<% else %>
			this.WorkTemperature 	= '-';
			<% end if %>
			this.EncryptEnable 		= '-';
			this.encryptState			= '<% tcWebApi_get("WebCurSet_Entry","EPONEncryption","s") %>';
			this.GemPortInfo			= '<% tcWebApi_get("GPON_GEMPort","EncryptionStateALL","s") %>';
			this.DispGemPortState = DispGemPortState;
			this.phyStatus				=	'<% tcWebApi_get("XPON_Common","phyStatus","s") %>';

			if ( 'GPON' != this.ponType && 'EPON' != this.ponType )
				this.ponType = 'GPON';
			if ( 'GPON' == this.ponType )
			{
				this.loidAuth					= '<% tcWebApi_get("GPON_LOIDAuth","AuthStatus","s") %>';
				this.loidV1						= '<% tcWebApi_get("GPON_LOIDAuth","LOID","s") %>';
			}
			else
			{
				this.loidAuth					= '<% tcWebApi_get("EPON_LOIDAuth","AuthStatus","s") %>';
				this.loidV1						= '<% tcWebApi_get("EPON_LOIDAuth","LOID0","s") %>';
			}

			if ( 'up' == this.trafficstate )
			{
				this.PonState 				= 'up';
				this.WarnInfo 				= 'no info';
				this.loidStatus				= 'up';
			}
			else
			{
				if ( '0' != this.LinkSta && '2' == this.loidAuth && 0 != this.loidV1.length )
					this.loidStatus			= 'error';
			}
			
			if( '0' != this.LinkSta)
			{
				<% if TxPower <> "N/A" then %>	
				this.SendPower 				= (Math.round(Math.log((Number(TxPower))/10000)/(Math.log(10))*100)/10);
				<% else %>
				this.SendPower 				= '-';
				<% end if %>
				<% if RxPower <> "N/A" then %>	
				this.RecvPower 				= (Math.round(Math.log((Number(RxPower))/10000)/(Math.log(10))*100)/10);
				<% else %>
				this.RecvPower 				= '-';
				<% end if %>
				this.FECEnable 				= ( '1' == this.fecState ) ? 'enabled' : 'disabled';

				if ( '2' == this.LinkSta )
					this.EncryptEnable	= ( '1' == this.encryptState ) ? 'enabled' : 'disabled';
			}
			
			function DispGemPortState()
			{
				var gemportList;
				var gemportInfo;
				var gemportState;
				var i = 0;

				if ( this.GemPortInfo.indexOf('#') >=0 )
					this.GemPortInfo = this.GemPortInfo.substring(0, this.GemPortInfo.length - 1);
				else
				{
					document.write('<%tcWebApi_get("String_Entry","NetInfoDisableText","s")%>');
					return -1;
				}

				gemportList = this.GemPortInfo.split('#');
				for (i = 0; i < gemportList.length; i ++)
				{
					gemportInfo = gemportList[i].split(':');
					if ( 2 != gemportInfo.length )
						continue;

					gemportState = 'GEMPORT' + gemportInfo[0] + ':' + ( ( 1 == gemportInfo[1] ) ? '<%tcWebApi_get("String_Entry","NetInfoEnableText","s")%>' : '<%tcWebApi_get("String_Entry","NetInfoDisableText","s")%>' )
												+ '; '
					document.write(gemportState);
				}

				return 0;
			}
		}
		
		var PonInfo = new PonInfoClass();

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
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
					if( Wan_WanName[i].indexOf('TR069') >= 0 )
					{
<% end if
end if
end if%>
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
								if(EthernetState == "up")
<% elseif TCWebApi_get("Wan_Common","TransMode","h" ) = "ActiveEtherWan" then %>
								if(EthernetState == "up")
<% else %>
								if(PonInfo.PonState == "up")
<% end if %>
								{
									if ('up' == Wan_Status6[i] && (1 == isIPGWUp))
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectedText","s")%>&nbsp;</td>');
<% if TCWebApi_get("WebCustom_Entry","isPPPoEOnDemandSupported","h" ) = "Yes" then %>
										else if(Wan_Actives[i]=="Yes"
													&& ( Wan_WanName[i].indexOf('_B_') >= 0
															||'PPPoE' != Wan_ENCAP[i]
															|| ('PPPoE' == Wan_ENCAP[i] && ('Connect_Keep_Alive' == Wan_Connection[i] || '-' == Wan_Connection[i]))
															)
														)
<% else %>
										else if(Wan_Actives[i]=="Yes")	
<% end if %>
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectingText","s")%>&nbsp;</td>');
										else
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
										
											document.write('<td>' + Wan_IP6[i] + '&nbsp;</td>');
											document.write('<td>' + Wan_GateWay6[i] + '&nbsp;</td>');
											if('DHCP' == Wan_ENCAP[i])
											{
												document.write('<td align="middle" valign="middle"><INPUT id=btnReleaseIP name=btnReleaseIP onclick="renewrelease(4, '+ i +')" type=button value=ReleaseIP></td>');
												document.write('<td align="middle" valign="middle"><INPUT id=btnRenewIP name=btnRenewIP onclick="renewrelease(3, '+ i +')" type=button value=RenewIP></td>');
											}
									}
									else
									{
										document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
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
								if(EthernetState == "up")
<% elseif TCWebApi_get("Wan_Common","TransMode","h" ) = "ActiveEtherWan" then %>
								if(EthernetState == "up")
<% else %>
								if(PonInfo.PonState == "up")
<% end if %>
								{
										if ('up' == Wan_Status4[i])
												document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectedText","s")%>&nbsp;</td>');
<% if TCWebApi_get("WebCustom_Entry","isPPPoEOnDemandSupported","h" ) = "Yes" then %>
										else if(Wan_Actives[i]=="Yes"
													&& ( Wan_WanName[i].indexOf('_B_') >= 0
																	||'PPPoE' != Wan_ENCAP[i]
																	|| ('PPPoE' == Wan_ENCAP[i] && ('Connect_Keep_Alive' == Wan_Connection[i] || '-' == Wan_Connection[i]))
																	)
														)
<% else %>
										else if(Wan_Actives[i]=="Yes")	
<% end if %>
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoConnectingText","s")%>&nbsp;</td>');
										else
											document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
										
											document.write('<td>' + Wan_IP4[i] + '&nbsp;</td>');
											document.write('<td>' + Wan_NetMask4[i] + '&nbsp;</td>');
											if('DHCP' == Wan_ENCAP[i])
											{
												document.write('<td align="middle" valign="middle"><INPUT id=btnReleaseIP name=btnReleaseIP onclick="renewrelease(2, '+ i +')" type=button value=ReleaseIP></td>');
												document.write('<td align="middle" valign="middle"><INPUT id=btnRenewIP name=btnRenewIP onclick="renewrelease(1, '+ i +')" type=button value=RenewIP></td>');
											}
									}
									else
									{
										document.write('<td><%tcWebApi_get("String_Entry","NetInfoUnconnectedText","s")%>&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
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
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
						}
<% end if
end if
end if%>
					}
				}
			}
			
		function ListCreate_info2(para)
		{
				var index = 1;
				for(var i=0;i<pvc_counts;i++)
				{
					if(Wan_Actives[i] != "N/A")
					{
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
					if( Wan_WanName[i].indexOf('TR069') >= 0 )
					{
<% end if
end if
end if%>
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
								document.write('<td>' + Wan_ENCAP[i] + '&nbsp;</td>');
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
									document.write('<td>' + Wan_ENCAP[i] + '&nbsp;</td>');
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
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
						}
<% end if
end if
end if%>
					}
				}
			}
			
		function ListCreate_info2p(para)
		{
			for( var i=0; i<pvc_counts; i++ )
			{
				if( Wan_Actives[i] != 'N/A' )
				{
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
				if( Wan_WanName[i].indexOf('TR069') >= 0 )
				{
<% end if
end if
end if%>
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
					document.write('<td>' + Wan_VidPRI[i] + '</td>');
					document.write('<td>' + Wan_MAC[i] + '&nbsp;</td>');
					document.write('</tr>');
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
				}
<% end if
end if
end if%>
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
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
					if( Wan_WanName[i].indexOf('TR069') >= 0 )
					{
<% end if
end if
end if%>
						if(para == 6)
						{
							if(Wan_IPVERSION[i] == "IPv6" || Wan_IPVERSION[i] == "IPv4/IPv6")
							{
<% if TCWebApi_get("Wan_Common","TransMode","h" ) = "Ethernet" then %>
								if(EthernetState == "up")
<% elseif TCWebApi_get("Wan_Common","TransMode","h" ) = "ActiveEtherWan" then %>
								if(EthernetState == "up")
<% else %>
								if(PonInfo.PonState == "up")
<% end if %>
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
									document.write('<tr align="middle">');
									document.write('<td>' + Wan_WanName[i] + '</td>');
									document.write('<td>' + '-' + '&nbsp;</td>');
									document.write('<td>' + '-' + '&nbsp;</td>');
									document.write('<td>' + '-' + '&nbsp;</td>');

									document.write('</tr>');
									index = index + 1;
								}
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
<% if TCWebApi_get("Wan_Common","TransMode","h" ) = "Ethernet" then %>
								if(EthernetState == "up")
<% elseif TCWebApi_get("Wan_Common","TransMode","h" ) = "ActiveEtherWan" then %>
								if(EthernetState == "up")
<% else %>
									if(PonInfo.PonState == "up")
<% end if %>
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
										document.write('<tr align="middle">');
										document.write('<td>' + Wan_WanName[i] + '</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('<td>' + '-' + '&nbsp;</td>');
										document.write('</tr>');
										index = index + 1;
									}
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
<% if tcWebApi_get("WebCustom_Entry", "isCTSIMCARDSEPARATIONSupported", "h") = "Yes"  then
   if tcWebApi_get("SimCard_Entry", "cardExist", "h") = "1"  then
   if tcWebApi_get("SimCard_Entry", "simAuthRet", "h") = "0"  then %>
						}
<% end if
end if
end if%>
					}
				}
			}
</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1><%tcWebApi_get("String_Entry","NetInfoIPv4ConnectionText","s")%></P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") ="Yes" then %>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=14>
<% else %>
          <TD vAlign=top width=170 background=/img/panel4.gif 
          rowSpan=16>
<%end if %>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#利大迦佚連" 
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
            <P class=Item_L2>&nbsp;</P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE id=network_info name=network_info cellSpacing=0 cellPadding=3 width=406 border=1>
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
                  <TABLE id=tabOptionv4 cellSpacing=0 cellPadding=3 width=406 
                  border=1 name="tabOptionv4">
                    <TBODY>
                    <TR align=middle>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title id=secPVC align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionText","s")%></TD>
                      <TD class=table_title id=secEncap 
align=middle><%TCWEBAPI_GET("String_Entry","NetInfoWOfGIPText","s")%></TD></TR>
                    <SCRIPT language=JavaScript type=text/javascript>
						ListCreate_info2(4);
					</SCRIPT>
                    </TBODY></TABLE><BR>
                  <TABLE id=ipv4_vlan_info name=ipv4_vlan_info cellSpacing=0 cellPadding=3 width=406 border=1>
                    <TBODY>
                    <TR align=middle>
                    	<TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoVLANPText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoMACAddressText","s")%></TD>
                      </TR>
							        <SCRIPT language=JavaScript type=text/javascript>
											ListCreate_info2p(4);
											</SCRIPT>
                  </TBODY></TABLE><BR>
                  <TABLE id=dns_info name=dns_info cellSpacing=0 cellPadding=3 border=1>
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
<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") <>"Yes" then %> 
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
                  <TABLE id=ipv6_info name=ipv6_info cellSpacing=0 cellPadding=3 width=406 border=1>
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
                  <TABLE id=tabOptionv6 cellSpacing=0 cellPadding=3 width=406 
                  border=1 name="tabOptionv6">
                    <TBODY>
                    <TR align=middle>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title id=secPVC align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionText","s")%></TD>
                      <TD class=table_title id=secEncap 
align=middle><%TCWEBAPI_GET("String_Entry","NetInfoWOfGIPText","s")%></TD></TR>
                    <SCRIPT language=JavaScript type=text/javascript>
						ListCreate_info2(6);
					</SCRIPT>
                    </TBODY></TABLE><BR>
                    
                  <TABLE id=ipv6_vlan_info name=ipv6_vlan_info cellSpacing=0 cellPadding=3 width=406 border=1>
                    <TBODY>
                    <TR align=middle>
                    	<TD class=table_title align=middle><%tcWebApi_get("String_Entry","NetInfoConnectionNameText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoVLANPText","s")%></TD>
                      <TD class=table_title align=middle><%TCWEBAPI_GET("String_Entry","NetInfoMACAddressText","s")%></TD>
                      </TR>
							        <SCRIPT language=JavaScript type=text/javascript>
											ListCreate_info2p(6);
											</SCRIPT>
                  </TBODY></TABLE><BR>
                  <TABLE id=ipv6_dns_info name=ipv6_dns_info cellSpacing=0 cellPadding=3 border=1>
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
<% end if %>
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
                  <DIV id=secPonInfo1>
<% if tcwebApi_get("Wan_Common","TransMode","h") <> "Ethernet" then %>
                  <TABLE id=pon_info name=pon_info cellSpacing=0 cellPadding=3 width="100%" border=1>
                    <TBODY>
                    <TR>
                    	<TD class='table_title' colspan='2'><%tcWebApi_get("String_Entry","NetInfoNeLiCIText","s")%></TD>
                    </TR>
                    <TR>
                      <TD class=table_title width=160><%TCWEBAPI_GET("String_Entry","NetInfoPONLICOSText","s")%></TD>
                       <TD > 
                        <script language=JavaScript type=text/javascript>
												if( PonInfo.PonState == "up")
													document.write("<%tcWebApi_get("String_Entry","NetInfoOnlineText","s")%>");
												else if ( PonInfo.PonState == "connecting")
													document.write("<%tcWebApi_get("String_Entry","NetInfoConnectingText","s")%>");
												else
													document.write("<%tcWebApi_get("String_Entry","NetInfoOfflineText","s")%>");
												</script>
                    		&nbsp;</TD>
                    </TR>
				 <TR>
                      <TD class=table_title width=160>PON Mode:</TD>
                       <TD > 
                        <script language=JavaScript type=text/javascript>
												if(( PonInfo.PonState == "up") || ( PonInfo.PonState == "connecting"))

													document.write("EPON");

												else
													document.write("Offline");
												</script>
                    		&nbsp;</TD>
                    </TR>
<% if tcwebApi_get("WebCustom_Entry","isCTPONJSSupported","h") = "Yes" then %>
                    <TR>
                      <TD class=table_title width=160><%TCWEBAPI_GET("String_Entry","NetInfoECoStaText","s")%></TD>
                       <TD > 
                        <script language=JavaScript type=text/javascript>
												if( PonInfo.phyStatus.indexOf("up") > -1 && PonInfo.trafficstate == "up")
													document.write("<%tcWebApi_get("String_Entry","NetInfoRegCerText","s")%>");
												else if ( PonInfo.phyStatus.indexOf("up") > -1 && PonInfo.trafficstate == "down")
													document.write("<%tcWebApi_get("String_Entry","NetInfoRegUncerText","s")%>");
												else
													document.write("<%tcWebApi_get("String_Entry","NetInfoUnreUnceText","s")%>");
												</script>
                    		&nbsp;</TD>
                    </TR>
<% end if %>
<% if tcwebApi_get("WebCustom_Entry","isCTPONNMGSupported","h") = "Yes" then %>
                    <TR>
                      <TD class=table_title width=160>
                      	<script language=JavaScript type=text/javascript>
                      	document.write(PonInfo.ponType + '<%tcWebApi_get("String_Entry","NetInfoStatus1Text","s")%>');
                      	</script>
                      </TD>
                       <TD > 
                        <script language=JavaScript type=text/javascript>
												if( PonInfo.loidStatus == 'up')
													document.write("<%tcWebApi_get("String_Entry","NetInfoAuSuText","s")%>");
												else if ( PonInfo.loidStatus == 'error')
													document.write("<%TCWEBAPI_GET("String_Entry","NetInfoLOIDEorNText","s")%>");
												else
													document.write("<%tcWebApi_get("String_Entry","NetInfoInitStaText","s")%>");
												</script>
                    		&nbsp;</TD>
                    </TR>
<% end if %>
<% if tcWebApi_get("WebCustom_Entry", "isCTPONCZGDSupported", "h") = "Yes"  then %> 
										</TBODY>
										<TBODY id="StaNetPonAdvInfo"> 			
         							<SCRIPT language=JavaScript type=text/javascript>
											if (curUserName != sptUserName)
											{
												getElement("StaNetPonAdvInfo").style.display = "none";									
											}
											</SCRIPT>	
<% end if %>
<% if tcWebApi_get("WebCurSet_Entry","CurrentAccess","h") = "0" then %>
                    <TR>
                      <TD class=table_title><%TCWEBAPI_GET("String_Entry","NetInfoFECEnText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
                        if ( '-' == PonInfo.FECEnable )
                        	document.write(PonInfo.FECEnable);
                        else
                        {
													if(PonInfo.FECEnable == "disabled")
														document.write("<%tcWebApi_get("String_Entry","NetInfoDisable11Text","s")%>");
													else
														document.write("<%tcWebApi_get("String_Entry","NetInfoEnable11Text","s")%>");
												}
											</SCRIPT>
                        &nbsp;</TD>
                      </TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoEnModText","s")%></TD>
                      <TD>
                      	<SCRIPT language=JavaScript type=text/javascript>
                      	if ( '1' == PonInfo.LinkSta )
                      	{
                      		PonInfo.DispGemPortState();
                      	}
                      	else
                      	{
	                        if ( '-' == PonInfo.EncryptEnable )
	                        	document.write(PonInfo.EncryptEnable);
	                        else
	                        {
														if(PonInfo.EncryptEnable == "disabled")
															document.write("<%tcWebApi_get("String_Entry","NetInfoDisableText","s")%>");
														else
															document.write("<%tcWebApi_get("String_Entry","NetInfoEnableText","s")%>");
													}
												}
                        </SCRIPT>
                        &nbsp;</TD>
                    </TR>
                    <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoAlaInText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
                        if(PonInfo.WarnInfo == "disconnect")
                                document.write("<%tcWebApi_get("String_Entry","NetInfoConIntText","s")%>");
			else
				document.write("<%tcWebApi_get("String_Entry","NetInfoNAlaText","s")%>");
			</SCRIPT>
                        &nbsp;</TD>
                      </TR>
                    <TR>
                    	<TD class='table_title' colspan='2'><%tcWebApi_get("String_Entry","NetInfoLiPerStaText","s")%></TD>
                    </TR>
                    <TR>
                      <TD class=table_title><%TCWEBAPI_GET("String_Entry","NetInfoPaNuOfText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.PonSendPkt);
												</SCRIPT>
                        &nbsp;</TD>
                     </TR>
                    <TR>
                      <TD class=table_title><%TCWEBAPI_GET("String_Entry","NetInfoPaNOPPText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.PonRecvPkt);
												</SCRIPT>
                        &nbsp;</TD>
                     </TR>
	                    <TR>
	                    	<TD class='table_title' colspan='2'><%tcWebApi_get("String_Entry","NetInfoLiMInText","s")%></TD>
	                    </TR>
                     <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoTranLPOText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.SendPower);
												</SCRIPT>dBm
                        &nbsp;</TD>
                     </TR>
                     <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoReceLoText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.RecvPower);
												</SCRIPT>dBm
                        &nbsp;</TD>
                     </TR>
                     <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoWoVOlText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.WorkVoltage);
												</SCRIPT>mV
                        &nbsp;</TD>
                     </TR>
                     <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoWorkCurText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.WorkElectric);
												</SCRIPT>mA
                        &nbsp;</TD>
                     </TR>
                     <TR>
                      <TD class=table_title><%tcWebApi_get("String_Entry","NetInfoWorkiTemText","s")%></TD>
                      <TD>
                        <SCRIPT language=JavaScript type=text/javascript>
												document.write(PonInfo.WorkTemperature);
												</SCRIPT>≧
                        &nbsp;</TD>
                     </TR>
<% end if %>
                      </TBODY>
                     </TABLE>
<% end if %>
                  </DIV>
                  </TD>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>&nbsp;</TD>
                <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR></TBODY></TABLE>
    </TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>
          <TD width=196 bgColor=#427594>
            <P align=center></P></TD>
          <TD width=170 
bgColor=#313031>　</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
