<%
If request_Form("VoIP_H248_flag")="1" then
	tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "1")
	if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h")= "Yes" then
		if tcWebApi_get("VoIPBasic_Entry0", "Enable", "h") = "Yes"  then
			if tcWebApi_get("VoIPSysParam_Entry0", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
				if tcWebApi_get("VoIPSysParam_Entry0", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
				end if
			end if
		end if
		if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
			if tcWebApi_get("VoIPBasic_Entry1", "Enable", "h") = "Yes"  then
				if tcWebApi_get("VoIPSysParam_Entry1", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
					if tcWebApi_get("VoIPSysParam_Entry1", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
						tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
					end if
				end if
			end if
		end if
		if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
			if tcWebApi_get("VoIPBasic_Entry2", "Enable", "h") = "Yes"  then
				if tcWebApi_get("VoIPSysParam_Entry2", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
					if tcWebApi_get("VoIPSysParam_Entry2", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
						tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
					end if
				end if
			end if
		end if
		if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
			if tcWebApi_get("VoIPBasic_Entry3", "Enable", "h") = "Yes"  then
				if tcWebApi_get("VoIPSysParam_Entry3", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
					if tcWebApi_get("VoIPSysParam_Entry3", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
						tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
					end if
				end if
			end if
		end if
	end if
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("InfoVoIPH248_Entry0", "lineInfoStatus", "h") <> "N/A" then
			if tcWebApi_get("InfoVoIPH248_Entry0", "lineInfoStatus", "h") <> "Idle"  then
				tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
			end if
		end if
	end if
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("InfoVoIPH248_Entry1", "lineInfoStatus", "h") <> "N/A" then
			if tcWebApi_get("InfoVoIPH248_Entry1", "lineInfoStatus", "h") <> "Idle"  then
				tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
			end if
		end if
	end if
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("InfoVoIPH248_Entry2", "lineInfoStatus", "h") <> "N/A" then
			if tcWebApi_get("InfoVoIPH248_Entry2", "lineInfoStatus", "h") <> "Idle"  then
				tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
			end if
		end if
	end if
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("InfoVoIPH248_Entry3", "lineInfoStatus", "h") <> "N/A" then
			if tcWebApi_get("InfoVoIPH248_Entry3", "lineInfoStatus", "h") <> "Idle"  then
				tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "0")
			end if
		end if
	end if
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		tcWebApi_set("VoIPCodecs_PVC0_Entry0", "priority", "codec0")
		tcWebApi_set("VoIPCodecs_PVC1_Entry0", "priority", "codec0")
		tcWebApi_set("VoIPCodecs_PVC2_Entry0", "priority", "codec0")
		tcWebApi_set("VoIPCodecs_PVC3_Entry0", "priority", "codec0")
		tcWebApi_set("VoIPCodecs_PVC0_Entry1", "priority", "codec1")
		tcWebApi_set("VoIPCodecs_PVC1_Entry1", "priority", "codec1")
		tcWebApi_set("VoIPCodecs_PVC2_Entry1", "priority", "codec1")
		tcWebApi_set("VoIPCodecs_PVC3_Entry1", "priority", "codec1")
		tcWebApi_set("VoIPCodecs_PVC0_Entry2", "priority", "codec2")
		tcWebApi_set("VoIPCodecs_PVC1_Entry2", "priority", "codec2")
		tcWebApi_set("VoIPCodecs_PVC2_Entry2", "priority", "codec2")
		tcWebApi_set("VoIPCodecs_PVC3_Entry2", "priority", "codec2")
		tcWebApi_set("VoIPCodecs_PVC0_Entry3", "priority", "codec3")
		tcWebApi_set("VoIPCodecs_PVC1_Entry3", "priority", "codec3")
		tcWebApi_set("VoIPCodecs_PVC2_Entry3", "priority", "codec3")
		tcWebApi_set("VoIPCodecs_PVC3_Entry3", "priority", "codec3")
		tcWebApi_set("VoIPCodecs_PVC0_Entry4", "priority", "codec4")
		tcWebApi_set("VoIPCodecs_PVC1_Entry4", "priority", "codec4")
		tcWebApi_set("VoIPCodecs_PVC2_Entry4", "priority", "codec4")
		tcWebApi_set("VoIPCodecs_PVC3_Entry4", "priority", "codec4")
		
		tcWebApi_set("VoIPCodecs_PVC0", "SIPPacketizationTime", "voicePtime")
		tcWebApi_set("VoIPAdvanced_Entry0", "VoiceVolumeSpeak", "mymgtxgain")
		tcWebApi_set("VoIPAdvanced_Entry0", "VoiceVolumeListen", "mymgrxtain")
		tcWebApi_set("VoIPMedia_Entry0", "VAD", "mymgsliencenble")
		tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "mymgechocancel")
		tcWebApi_set("VoIPAdvanced_Entry0", "RFC2833Redundance", "mymg2833enable")
		
		tcWebApi_set("VoIPAdvanced_Entry1", "VoiceVolumeSpeak", "mymgtxgain")
		tcWebApi_set("VoIPAdvanced_Entry1", "VoiceVolumeListen", "mymgrxtain")
		tcWebApi_set("VoIPMedia_Entry1", "VAD", "mymgsliencenble")
		tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "mymgechocancel")
		tcWebApi_set("VoIPAdvanced_Entry1", "RFC2833Redundance", "mymg2833enable")
		tcWebApi_set("VoIPCodecs_PVC1", "SIPPacketizationTime", "voicePtime")
		
		tcWebApi_set("VoIPAdvanced_Entry2", "VoiceVolumeSpeak", "mymgtxgain")
		tcWebApi_set("VoIPAdvanced_Entry2", "VoiceVolumeListen", "mymgrxtain")
		tcWebApi_set("VoIPMedia_Entry2", "VAD", "mymgsliencenble")
		tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "mymgechocance2")
		tcWebApi_set("VoIPAdvanced_Entry2", "RFC2833Redundance", "mymg2833enable")
		tcWebApi_set("VoIPCodecs_PVC2", "SIPPacketizationTime", "voicePtime")
		
		tcWebApi_set("VoIPAdvanced_Entry3", "VoiceVolumeSpeak", "mymgtxgain")
		tcWebApi_set("VoIPAdvanced_Entry3", "VoiceVolumeListen", "mymgrxtain")
		tcWebApi_set("VoIPMedia_Entry3", "VAD", "mymgsliencenble")
		tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "mymgechocance3")
		tcWebApi_set("VoIPAdvanced_Entry3", "RFC2833Redundance", "mymg2833enable")
		tcWebApi_set("VoIPCodecs_PVC3", "SIPPacketizationTime", "voicePtime")
		
		tcWebApi_set("VoIPH248_Common", "PolReverse", "megacopolreverseflag")
		tcWebApi_set("VoIPMedia_Common", "FaxCodec", "megacofaxmode")
		tcWebApi_set("VoIPAdvanced_Common", "FaxCtrlMode", "megacofaxcontrol")
		tcWebApi_set("VoIPAdvanced_Common", "FaxPassThruCodec", "megacofaxpassthru")
		tcWebApi_set("VoIPAdvanced_Common", "FaxRedundancy", "megacofaxredundancyflag")
		tcWebApi_set("VoIPAdvanced_Common", "FaxRedundancyPT", "megacofaxredundancypt")
		tcWebApi_set("VoIPAdvanced_Common", "DTMFTransportMode", "megacodtmfrelay")
		if request_Form("megacodtmfrelay") = "RFC2833" then
			tcWebAPI_set("VoIPBasic_Common", "TelephoneEventPayloadType", "megacorfc2833pt")
		end if
		tcWebApi_set("VoIPAdvanced_Common", "MaxStartDelay", "megacomaxstartupdelay")
		tcWebApi_set("VoIPDigitMap_Entry", "StartDigitTimer", "megacodigitmapstarttime")
		tcWebApi_set("VoIPDigitMap_Entry", "InterDigitTimerShort", "megacodigitmapshorttime")
		tcWebApi_set("VoIPDigitMap_Entry", "InterDigitTimerLong", "megacodigitmapLongtime")
		tcWebApi_set("VoIPH248_Common", "NoAnswerTimer", "megaconoanswertimer")
		tcWebApi_set("VoIPDigitMap_Entry", "BusyToneTimer", "megacobusytonetimer")
		tcWebApi_set("VoIPDigitMap_Entry", "HangingReminderToneTimer", "megacohangrmdtonetimer")
		tcWebApi_set("VoIPDigitMap_Entry", "HangingReminderToneTimer2", "megacohangrmdtonetimer2")
		tcWebApi_set("VoIPH248_Common", "HeartbeatMode", "megacoheartbeatenable")
		tcWebApi_set("VoIPH248_Common", "HeartbeatSendMode", "megacohetbtSendMode")
		tcWebApi_set("VoIPH248_Common", "HeartBeatTimer", "megacoHeartBeatTimer")
		tcWebApi_set("VoIPH248_Common", "HeartBeatRetransTimes", "megacoHeartBeatRetransTimes")
		tcWebApi_set("VoIPH248_Common", "SmartSendReg", "cancellineregflag")
		tcWebApi_set("VoIPH248_Common", "NumberMatchMode", "DivDigitMatchMethod")
		tcWebApi_set("VoIPH248_Common", "NumberSmartMatch", "digitmapswitchflag")
		tcWebApi_set("VoIPH248_Common", "RingVoltage", "megacoringingvoltage")
		tcWebApi_set("VoIPAdvanced_Common", "FlashMaxTime", "megacoFlashMaxTime")
		tcWebApi_set("VoIPAdvanced_Common", "FlashMinTime", "megacoFlashMinTime")
		tcWebApi_set("VoIPAdvanced_Common", "ReleaseMinTime", "megacoReleaseMinTime")
		tcWebApi_set("VoIPH248_Common", "ComingCallerIDMode", "megacocalleridmode")
		tcWebApi_set("VoIPH248_Common", "JitterBufferFixedMode", "megacojbfixedmodeflag")
		tcWebApi_set("VoIPH248_Common", "JitterBufferLen", "megacojitterbuflen")
		tcWebApi_set("VoIPMedia_Common", "SC_ACCT_MEDIA_G723_RATE", "codecRate")
		tcWebApi_commit("VoIPCodecs")
		tcWebApi_commit("VoIPAdvanced")
		tcWebApi_commit("VoIPMedia_Common")
		tcWebApi_commit("VoIPDigitMap")
		tcWebApi_commit("VoIPH248_Common")
		tcWebApi_commit("VoIPBasic_Common")
		tcWebApi_save()
	end if
end if
%>
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","VoipStep1Text","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language="javascript" type = "text/javascript">
	var voipinfo3 = '<%tcWebApi_get("String_Entry","VoipStep2Text","s")%>';
<%
If request_Form("VoIP_H248_flag")="1" then
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") <> "1"  then
		asp_write("alert(voipinfo3);")
	end if
end if
%>
function LoadFrame()
{
	if ( "RFC2833" == "<%tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "s") %>")
	{
		document.getElementById("trRFC2833PT").style.display = "";
	}
	else
	{
		document.getElementById("trRFC2833PT").style.display = "none";
	}

	<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported", "h") = "Yes"  then %>
		document.getElementById("megacohangrmdtonetimer2").style.display = "";
	<%else%>
		document.getElementById("megacohangrmdtonetimer2").style.display = "none";
	<%end if%>

}


function checkValueValid()
{
	if((limitVolumeNum(document.getElementById("in_send_out_val"))==false) 
		|| (limitVolumeNum(document.getElementById("in_incept1_val"))==false)
		|| (limitNum(document.getElementById("in_fax_redundancy_pt_val_1"))==false)
		|| (limitTimerNum(document.getElementById("in_on_hook_release_val_1"))==false)
		|| (limitTimerNum(document.getElementById("megacomaxstartupdelay"))==false) 
		|| (limitTimerNum(document.getElementById("megacodigitmapstarttime"))==false)
		|| (limitTimerNum(document.getElementById("megacodigitmapshorttime"))==false)			
		|| (limitTimerNum(document.getElementById("megacodigitmapLongtime"))==false)
		|| (limitTimerNum(document.getElementById("megaconoanswertimer"))==false)
		|| (limitTimerNum(document.getElementById("megacobusytonetimer"))==false)
		|| (limitTimerNum(document.getElementById("megacohangrmdtonetimer"))==false)
		|| (limitTimerNum(document.getElementById("megacoHeartBeatTimer"))==false))
		return false;
<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported", "h") = "Yes"  then %>
	if(limitTimerNum(document.getElementById("megacohangrmdtonetimer2"))==false)
		return false;
<%end if%>
	
	if((limitTimerNum(document.getElementById("megacoFlashMaxTime"))==false) 
		|| (limitTimerNum(document.getElementById("megacoFlashMinTime"))==false)
		|| (limitNum(document.getElementById("megacoHeartBeatRetransTimes"))==false) 
		|| (limitNum(document.getElementById("megacorfc2833pt"))==false)
		|| (limitNum(document.getElementById("megacojitterbuflen"))==false))
		return false;
}

function applyClickBasic()
{
	var loc = "app-VoIP248.asp";
	var code = "location='" + loc + " '";
	eval(code);
}
function VoiceApplyBasic()
{
	with(document.ConfigForm)
	{		
		if(checkValueValid()==false)
			return;

		var temp = parseInt(mymgtxgain.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep145Text","s")%>');
			return;
		}

		var temp = parseInt(mymgrxtain.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep146Text","s")%>');
			return;
		}
		
		var temp = parseInt(megacofaxredundancypt.value);
		if (temp < 96 || temp > 127)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep149Text","s")%>');
			return;
		}

		var temp = parseInt(megacoReleaseMinTime.value);
		if (temp < 300 || temp > 1199)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep158Text","s")%>');
			return;
		}		
		var temp2 = parseInt(megacoFlashMaxTime.value);
		var temp3 = parseInt (megacoFlashMinTime.value);

		if (temp2 <= temp3)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep162Text","s")%>');
			return;
		}
		if (temp < temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep163Text","s")%>');
			return;
		}
		
		var temp = parseInt(megacorfc2833pt.value);
		if (temp < 96 || temp > 127)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep147Text","s")%>');
			return;
		}		
		var codecOld = "";
		var codecNew = "";
		var zz = 0;
		for (k = 0; k < 5; k++) 
		{
			codecOld = codecOld + "," + codecNew;
			codecNew = eval( "codec" + k + ".value" );
			codecList = codecOld.split(",");
			for (var i = 0;i < codecList.length;i++)
			{
				if (codecNew != 0 && codecNew == codecList[i])
				{
					//alert("Codec priority is same, please config again!");
					alert("<%tcWebApi_get("String_Entry","VoipStep143Text","s")%>");
					return;
				}
			}
			if(codecNew == 0)
			{
				zz++;
			}
		}
		if(zz == 5)
		{
			alert("<%tcWebApi_get("String_Entry","VoipStep144Text","s")%>");
			return;
		}
		if (megacopolreverse.checked)
		{
			megacopolreverseflag.value = "1";
		}
		if (megacofaxredundancy.checked)
		{
			megacofaxredundancyflag.value = "1";
		}
		if (cancellinereg.checked)
		{
			cancellineregflag.value = "1";
		}
		if (digitmapswitch.checked)
		{
			digitmapswitchflag.value = "1";
		}
		if (megacojbfixedmode.checked)
		{
			megacojbfixedmodeflag.value = "1";
		}
		VoIP_H248_flag.value = "1";
		if( true == setEBooValueCookie(document.ConfigForm) )
			submit();
	}
}
function isIntegerNeg(value)
{
	if (/^([1-9][0-9]*)|^0|(^-[1-9][0-9]*)$/.test(value))
	{
		return true;
	}
	else
	{
		return false;
	}
}
function isInteger(value)
{
	if (/^(\|)?\d+$/.test(value))
	{
		return true;
	}
	else
	{
		return false;
	}
}

function checkMark(value)
{
	if (/^(\|)?\d+$/.test(value))
	{
		return true;
	}
	else
	{
		return false;
	}
}
function limitNum(o)
{
	if (o.value != "")
	{
		if (!isInteger(o.value)|| parseInt(o.value) < 0)
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep140Text","s")%>');
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}		
		else
			return true;
	}
}

function limitTimerNum(o)
{
	if (o.value != "")
	{
		if (!checkMark(o.value))
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep140Text","s")%>');
			window.setTimeout( function(){ o.focus(); }, 0);			
			return false;
		}		
		else
			return true;
	}
}
function limitVolumeNum(o)
{
	if (o.value != "")
	{
		if (!isIntegerNeg(o.value))
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep140Text","s")%>'); 
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}
		else
			return true;
	}
}

function dtmfRelayChange(o)
{
	var idx = o.selectedIndex;
	var val = o.options[idx].value;
	if ( val == "RFC2833" )
	{
		document.getElementById("trRFC2833PT").style.display = "";
	}
	else{
		document.getElementById("trRFC2833PT").style.display = "none";
	}
}
</SCRIPT>
<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
	<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
	leftMargin=0 topMargin=0 
	onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ConfigForm') != null)LoadFrame()" 
	onunload=DoUnload() marginheight="0" marginwidth="0">
	<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center border=0>
		<TBODY>
			<TR>
				<TD height=1>
					<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
						<TABLE height=80 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
					<% else %>
						<% if tcwebApi_get("WebCustom_Entry","isCTPONTYLOGOSupported","h") = "Yes" then %>
							<TABLE height=80 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
						<% else %>
							<TABLE height=90 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
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
                  href="/cgi-bin/logout.cgi" id=logOut name=logOut target=_top><SPAN 
											  class=logout><%tcWebApi_get("String_Entry","LogoutText","s")%> 
					</SPAN></A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
					<TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
						<TBODY>
							<TR>
								<TD class=LocationDisplay id=LocationDisplay align=middle width=163 bgColor=#ef8218 rowSpan=3></TD>
								<TD width=434 bgColor=#427594 height=33>
									<P align=right><FONT face=黑体 color=#ffffff><B><FONT face=黑体 
									color=#ffffff size=6><INPUT id=Selected_Menu type=hidden 
									value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>-><%tcWebApi_get("String_Entry","ContentVoIPSetText","s")%>" name=Selected_Menu> </FONT></B><SPAN 
									class=GatewayName><%tcWebApi_get("String_Entry","GatewayNameText","s")%>
									<SCRIPT language=javascript>
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
									<TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24></TD>
					</TR></TBODY></TABLE>
					<SCRIPT language=javascript>
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
									<P class=Item_L1><%tcWebApi_get("String_Entry","VoipStep83Text","s")%></P></TD>
								<TD width=7 bgColor=#ef8218></TD>
								<TD width=474></TD>
								<TD vAlign=top width=170 background=/img/panel4.gif 
									rowSpan=13>
								<TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
									<TBODY>
							<TR>
								<TD valign='top'><A 
									href="/cgi-bin/help_content.asp#宽带电话设置" 
									target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
									width=40 border=0> </A></TD></TR>
							<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
								<TR>
									<TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
							<% end if %>
			</TBODY></TABLE></TD></TR>
			<TR>
				<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
				<TD width=7 background=/img/panel3.gif></TD>
				<TD></TD></TR>
			<TR>
				<TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
					<P class=Item_L2></P></TD>
				<TD width=7 background=/img/panel3.gif></TD>
				<TD>
					<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
						<TBODY>
							<TR>
								<TD width=10>&nbsp;</TD>
								<TD>
									<FORM id=ConfigForm name="ConfigForm" action="/cgi-bin/app-VoIP248_Adv.asp" method="post">
										<DIV id=pptp>
											<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
												<TBODY>
													<TR>
														<TD>
															<input type='button' id=applyClickBas name=applyClickBas style="font-size:100%;width:170;height:30" onClick="applyClickBasic()" value='<%tcWebApi_get("String_Entry","VoipStep85Text","s")%>'>
														</TD>
													</TR>
													<TR>
														<TD height="10"></TD>
													</TR>
													<TR>
														<TD class=table1_head>
															<B>
																<%tcWebApi_get("String_Entry","VoipStep84Text","s")%>
															</B>
														</TD>
													</TR>
													<TR>
														<TD height="10"></TD>
													</TR>
												</TBODY>
											</TABLE>
											<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
												<TBODY>
													<TR>
														<TD class=prompt>
															<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
																<TBODY>
																	<TR>
																		<TD style="PADDING-LEFT: 10px" width="100%"></TD>
																	</TR>
																</TBODY>
															</TABLE>
														</TD>
													</TR>
												</TBODY>
											</TABLE>
											<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
												<TBODY>
													<TR>
														<TD height=5></TD>
													</TR>
												</TBODY>
											</TABLE>
											<TABLE border=1 cellSpacing=1 cellPadding=0 width="98%">
												<TBODY>
													<TR>
														<td class="table1_right" colspan="2" ><strong id='in_h_resource1'><%tcWebApi_get("String_Entry","VoipStep256Text","s")%></strong></td>
													</TR>
													<TR>
														<td class="table1_right" colspan="2" ><%tcWebApi_get("String_Entry","VoipStep257Text","s")%></td>
													</TR>
													<TR>
														<td class="table1_left" width="35%"align="left">G.722</td>
														<td class="table1_right" width="65%">
															<select name="codec0" id="g722">
																<option value="0" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "0" then asp_Write("selected") end if %>>0</option>
																<option value="1" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "1" then asp_Write("selected") end if %>>1</option>
																<option value="2" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "2" then asp_Write("selected") end if %>>2</option>
																<option value="3" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "3" then asp_Write("selected") end if %>>3</option>
																<option value="4" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "4" then asp_Write("selected") end if %>>4</option>
																<option value="5" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "5" then asp_Write("selected") end if %>>5</option>
																<option value="6" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "6" then asp_Write("selected") end if %>>6</option>
																<option value="7" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") = "7" then asp_Write("selected") end if %>>7</option>
															</select>
														</td>
													</TR>
													<TR>
														<td class="table1_left">G.711A-law</td>
														<td class="table1_right">
															<select name="codec1" id="g711a">
																<option value="0" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "0" then asp_Write("selected") end if %>>0</option>
																<option value="1" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "1" then asp_Write("selected") end if %>>1</option>
																<option value="2" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "2" then asp_Write("selected") end if %>>2</option>
																<option value="3" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "3" then asp_Write("selected") end if %>>3</option>
																<option value="4" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "4" then asp_Write("selected") end if %>>4</option>
																<option value="5" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "5" then asp_Write("selected") end if %>>5</option>
																<option value="6" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "6" then asp_Write("selected") end if %>>6</option>
																<option value="7" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") = "7" then asp_Write("selected") end if %>>7</option>
															</select>
														</td>
													</TR>
													<TR>
														<td class="table1_left">G.711U-law</td>
														<td class="table1_right">
															<select name="codec2" id="g711u">
																<option value="0" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "0" then asp_Write("selected") end if %>>0</option>
																<option value="1" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "1" then asp_Write("selected") end if %>>1</option>
																<option value="2" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "2" then asp_Write("selected") end if %>>2</option>
																<option value="3" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "3" then asp_Write("selected") end if %>>3</option>
																<option value="4" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "4" then asp_Write("selected") end if %>>4</option>
																<option value="5" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "5" then asp_Write("selected") end if %>>5</option>
																<option value="6" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "6" then asp_Write("selected") end if %>>6</option>
																<option value="7" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") = "7" then asp_Write("selected") end if %>>7</option>
															</select>
														</td>
													</TR>
													<TR>
														<td class="table1_left">G.729</td>
														<td class="table1_right">
															<select name="codec3" id="g729">
																<option value="0" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "0" then asp_Write("selected") end if %>>0</option>
																<option value="1" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "1" then asp_Write("selected") end if %>>1</option>
																<option value="2" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "2" then asp_Write("selected") end if %>>2</option>
																<option value="3" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "3" then asp_Write("selected") end if %>>3</option>
																<option value="4" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "4" then asp_Write("selected") end if %>>4</option>
																<option value="5" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "5" then asp_Write("selected") end if %>>5</option>
																<option value="6" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "6" then asp_Write("selected") end if %>>6</option>
																<option value="7" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") = "7" then asp_Write("selected") end if %>>7</option>
															</select>
														</td>
													</TR>
													<TR>
														<td class="table1_left">G.723</td>
														<td class="table1_right">
															<select name="codec4" id="g723">
																<option value="0" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "0" then asp_Write("selected") end if %>>0</option>
																<option value="1" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "1" then asp_Write("selected") end if %>>1</option>
																<option value="2" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "2" then asp_Write("selected") end if %>>2</option>
																<option value="3" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "3" then asp_Write("selected") end if %>>3</option>
																<option value="4" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "4" then asp_Write("selected") end if %>>4</option>
																<option value="5" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "5" then asp_Write("selected") end if %>>5</option>
																<option value="6" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "6" then asp_Write("selected") end if %>>6</option>
																<option value="7" <%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") = "7" then asp_Write("selected") end if %>>7</option>
															</select>
														</td>
													</TR>
													<TR>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep258Text","s")%></td>
														<td class="table1_right">
															<select name="voicePtime" id="voicePtime">
																<option value="10" <%if tcWebApi_get("VoIPCodecs_PVC0", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>10</option>
																<option value="20" <%if tcWebApi_get("VoIPCodecs_PVC0", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>20</option>
																<option value="30" <%if tcWebApi_get("VoIPCodecs_PVC0", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>30</option>
															</select>
														</td>
													</TR>
													<TR>
														<td class="table1_right" colspan="2" ><%tcWebApi_get("String_Entry","VoipStep259Text","s")%> </td>
													</TR>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep260Text","s")%></td>
														<td class="table1_right">
															<input name="mymgtxgain" type="text" id="in_send_out_val" onchange="limitVolumeNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Entry0", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry0", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>">
															(-14-6db)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep261Text","s")%></td>
														<td class="table1_right">
															<input name="mymgrxtain" type="text" id="in_incept1_val" onchange="limitVolumeNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Entry0", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry0", "VoiceVolumeListen", "s") else asp_Write("") end if %>">
															(-14-6db)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep262Text","s")%> </td>
														<td class="table1_right" valign="middle">
															<table border="0" cellpadding="0" cellspacing="0">
																<tr>
																<td align="left"><input name="mymgsliencenble" type="radio" id="loudspeaker0" value="0" <%if tcWebApi_get("VoIPMedia_Entry0", "VAD", "h") <> "1" then asp_Write("checked") end if %>></td>
																<td align="left"><%tcWebApi_get("String_Entry","VoipStep245Text","s")%> &nbsp;&nbsp;&nbsp;</td>
																<td align="left"><input name="mymgsliencenble" type="radio" id="loudspeaker1" value="1" <%if tcWebApi_get("VoIPMedia_Entry0", "VAD", "h") = "1" then asp_Write("checked") end if %>></td>
																<td align="left"><%tcWebApi_get("String_Entry","VoipStep250Text","s")%> </td>
																</tr>
															</table>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep263Text","s")%> </td>
														<td class="table1_right" valign="middle">
															<table height="25" border="0" cellpadding="0" cellspacing="0">
																<tr>
																	<td align="left"><input name="mymgechocancel" type="radio" id="aftersound0" value="No"  <%if tcWebApi_get("VoIPMedia_Common", "EchoCancellationEnable", "h") <> "Yes" then asp_Write("checked") end if %>></td>
																	<td align="left"><%tcWebApi_get("String_Entry","VoipStep245Text","s")%> &nbsp;&nbsp;&nbsp;</td>
																	<td align="left"><input name="mymgechocancel" type="radio" id="aftersound1" value="Yes"  <%if tcWebApi_get("VoIPMedia_Common", "EchoCancellationEnable", "h") = "Yes" then asp_Write("checked") end if %>></td>
																	<td align="left"><%tcWebApi_get("String_Entry","VoipStep250Text","s")%> </td>
																</tr>
															</table>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep264Text","s")%></td>
														<td class="table1_right">
															<table border="0" cellpadding="0" cellspacing="0">
																<tr>
																	<td align="left"><input name="mymg2833enable" type="radio" id="rfc2833_0" value="0"  <%if tcWebApi_get("VoIPAdvanced_Entry0", "RFC2833Redundance", "h") <> "1" then asp_Write("checked") end if %>></td>
																	<td align="left" ><%tcWebApi_get("String_Entry","VoipStep245Text","s")%> &nbsp;&nbsp;&nbsp;</td>
																	<td align="left"><input name="mymg2833enable" type="radio" id="rfc2833_1" value="1"  <%if tcWebApi_get("VoIPAdvanced_Entry0", "RFC2833Redundance", "h") = "1" then asp_Write("checked") end if %>></td>
																	<td align="left" ><%tcWebApi_get("String_Entry","VoipStep250Text","s")%> &nbsp;&nbsp;&nbsp;</td>
																</tr>
															</table>
														</td>
													</tr>
													<tr>
														<td class="table1_right" colspan='2' ><strong id='in_h_resource1'><%tcWebApi_get("String_Entry","VoipStep265Text","s")%></strong></td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep266Text","s")%> </td>
														<td class="table1_right">
															<input id="megacopolreverse" name="megacopolreverse" type="checkbox" <%if tcWebApi_get("VoIPH248_Common", "PolReverse", "h") = "1" then asp_Write("checked") end if %>>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep267Text","s")%> </td>
														<td class="table1_right">
															<select id="megacofaxmode" name="megacofaxmode" >
																<option value="0" <%if tcWebApi_get("VoIPMedia_Common", "FaxCodec", "h") = "0" then asp_Write("selected") end if %>>T.30</option>
																<option value="1" <%if tcWebApi_get("VoIPMedia_Common", "FaxCodec", "h") = "1" then asp_Write("selected") end if %>>T38</option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep268Text","s")%></td>
														<td class="table1_right">
															<select id="megacofaxcontrol" name="megacofaxcontrol">
																<option value="all" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxCtrlMode", "h") = "all" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep269Text","s")%></option>
																<option value="other" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxCtrlMode", "h") = "other" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep270Text","s")%></option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep271Text","s")%></td>
														<td>
															<select id="megacofaxpassthru" name="megacofaxpassthru" size="1">
																<option value="PCMU" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxPassThruCodec", "h") = "PCMU" then asp_Write("selected") end if %>>PCMU</option>
																<option value="PCMA" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxPassThruCodec", "h") = "PCMA" then asp_Write("selected") end if %>>PCMA</option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep272Text","s")%></td>
														<td class="table1_right">
															<input id="megacofaxredundancy" name="megacofaxredundancy" type="checkbox" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxRedundancy", "h") = "1" then asp_Write("checked") end if %>>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep273Text","s")%> </td>
														<td class="table1_right">
															<input name="megacofaxredundancypt" type="text" id="in_fax_redundancy_pt_val_1" onchange="limitNum(this)"  value="<%if tcWebApi_get("VoIPAdvanced_Common", "FaxRedundancyPT", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "FaxRedundancyPT", "s") else asp_Write("") end if %>">
																&nbsp;(96-127)
														</td>
													</tr>
													<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported", "h") <> "Yes"  then %>
														<tr>
															<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep274Text","s")%></td>
															<td class="table1_right">
																<select id="megacodtmfrelay" name="megacodtmfrelay" onchange="dtmfRelayChange(this)">
																	<option value="InBand" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "InBand" then asp_Write("selected") end if %>>Inband</option>
																	<option value="RFC2833" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "RFC2833" then asp_Write("selected") end if %>>RFC2833</option>
																</select></td>
														</tr>
													<%else%>
														<tr>
															<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep275Text","s")%></td>
															<td class="table1_right">
															<select id="megacodtmfrelay" name="megacodtmfrelay" onchange="dtmfRelayChange(this)">
																<option value="InBand" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "InBand" then asp_Write("selected") end if %>>Inband</option>
																<option value="RFC2833" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "RFC2833" then asp_Write("selected") end if %>>RFC2833</option>
															</select></td>
														</tr>
														<tr>
															<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep276Text","s")%></td>
															<td class="table1_right">
																<%tcWebApi_get("String_Entry","VoipStep277Text","s")%>
															</td>
														</tr>
													<%end if%>
													<tr id="trRFC2833PT">
														<td class="table1_left">Telephone Event Payload Type:</td>
														<td class="table1_right">
															<input type='text' size=20 maxlength=20 id='megacorfc2833pt' name='megacorfc2833pt' onchange='limitNum(this)' value="<%if tcWebApi_get("VoIPBasic_Common", "TelephoneEventPayloadType", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "TelephoneEventPayloadType", "s") else asp_Write("") end if %>">
															&nbsp;(96-127)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep278Text","s")%> </td>
														<td class="table1_right">
															<input id="megacomaxstartupdelay" name="megacomaxstartupdelay" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "MaxStartDelay", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "MaxStartDelay", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported", "h") = "Yes"  then %>
															<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep279Text","s")%> </td>
														<%else%>  
															<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep280Text","s")%> </td>
														<%end if%>
														<td class="table1_right">
															<input id="megacodigitmapstarttime" name="megacodigitmapstarttime" type="text" onchange="limitTimerNum(this)" id="in_digitMapStartTime_val_1" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "StartDigitTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "StartDigitTimer", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep281Text","s")%> </td>
														<td class="table1_right">
															<input id="megacodigitmapshorttime" name="megacodigitmapshorttime" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "InterDigitTimerShort", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "InterDigitTimerShort", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep282Text","s")%> </td>
														<td class="table1_right">
															<input id="megacodigitmapLongtime" name="megacodigitmapLongtime" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "InterDigitTimerLong", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "InterDigitTimerLong", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep283Text","s")%> </td>
														<td class="table1_right">
															<input id="megaconoanswertimer" name="megaconoanswertimer" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "NoAnswerTimer", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "NoAnswerTimer", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep284Text","s")%> </td>
														<td class="table1_right">
															<input id="megacobusytonetimer" name="megacobusytonetimer" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "BusyToneTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "BusyToneTimer", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
													<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported", "h") = "Yes"  then %>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep285Text","s")%> </td>
														<td class="table1_right">
															<input id="megacohangrmdtonetimer" name="megacohangrmdtonetimer" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "HangingReminderToneTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "HangingReminderToneTimer", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep286Text","s")%> </td>
														<td class="table1_right">
															<input id="megacohangrmdtonetimer2" name="megacohangrmdtonetimer2" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "HangingReminderToneTimer2", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "HangingReminderToneTimer2", "s") else asp_Write("") end if %>">
													<%else%>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep287Text","s")%> </td>
														<td class="table1_right">
															<input id="megacohangrmdtonetimer" name="megacohangrmdtonetimer" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "HangingReminderToneTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "HangingReminderToneTimer", "s") else asp_Write("") end if %>">
													<%end if%>
													&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep288Text","s")%> </td>
														<td class="table1_right">
															<select id="megacoheartbeatenable" name="megacoheartbeatenable">
																<option value="0"  <%if tcWebApi_get("VoIPH248_Common", "HeartbeatMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep289Text","s")%></option>
																<option value="1"  <%if tcWebApi_get("VoIPH248_Common", "HeartbeatMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep290Text","s")%></option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep291Text","s")%> </td>
														<td class="table1_right">
															<select id="megacohetbtSendMode" name="megacohetbtSendMode">
																<option value="0" <%if tcWebApi_get("VoIPH248_Common", "HeartbeatSendMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep292Text","s")%></option>
																<option value="1" <%if tcWebApi_get("VoIPH248_Common", "HeartbeatSendMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep293Text","s")%> </option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep294Text","s")%> </td>
														<td class="table1_right">
															<input id="megacoHeartBeatTimer" name="megacoHeartBeatTimer" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "HeartBeatTimer", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "HeartBeatTimer", "s") else asp_Write("") end if %>">
															&nbsp;(s)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep295Text","s")%> </td>
														<td class="table1_right">
															<input id="megacoHeartBeatRetransTimes" name="megacoHeartBeatRetransTimes" type="text" onchange="limitNum(this)"  value="<%if tcWebApi_get("VoIPH248_Common", "HeartBeatRetransTimes", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "HeartBeatRetransTimes", "s") else asp_Write("") end if %>">
															&nbsp;
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep296Text","s")%></td>
														<td class="table1_right">
															<input id="cancellinereg" name="cancellinereg" type="checkbox" <%if tcWebApi_get("VoIPH248_Common", "SmartSendReg", "h") = "1" then asp_Write("checked") end if %>>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep297Text","s")%></td>
														<td>
															<select id="DivDigitMatchMethod" name="DivDigitMatchMethod" size="1">
																<option value="0" <%if tcWebApi_get("VoIPH248_Common", "NumberMatchMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep298Text","s")%></option>
																<option value="1" <%if tcWebApi_get("VoIPH248_Common", "NumberMatchMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep299Text","s")%></option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep300Text","s")%></td>
														<td class="table1_right">
															<input id="digitmapswitch" name="digitmapswitch" type="checkbox" <%if tcWebApi_get("VoIPH248_Common", "NumberSmartMatch", "h") = "1" then asp_Write("checked") end if %>>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep301Text","s")%> </td>
														<td class="table1_right">
															<select id="megacoringingvoltage" name="megacoringingvoltage">
																<option value="10" <%if tcWebApi_get("VoIPH248_Common", "RingVoltage", "h") = "10" then asp_Write("selected") end if %>>10 V</option>
																<option value="20" <%if tcWebApi_get("VoIPH248_Common", "RingVoltage", "h") = "20" then asp_Write("selected") end if %>>20 V</option>
																<option value="30" <%if tcWebApi_get("VoIPH248_Common", "RingVoltage", "h") = "30" then asp_Write("selected") end if %>>30 V</option>
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep302Text","s")%> </td>
														<td class="table1_right">
															<input id="megacoFlashMaxTime" name="megacoFlashMaxTime" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "FlashMaxTime", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "FlashMaxTime", "s") else asp_Write("") end if %>">
															&nbsp;(ms)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep303Text","s")%> </td>
														<td class="table1_right">
															<input id="megacoFlashMinTime" name="megacoFlashMinTime" type="text" onchange="limitTimerNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "FlashMinTime", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "FlashMinTime", "s") else asp_Write("") end if %>">
															&nbsp;(ms)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep304Text","s")%> </td>
														<td class="table1_right">
															<input name="megacoReleaseMinTime" type="text" id="in_on_hook_release_val_1" onchange="limitTimerNum(this)"  value="<%if tcWebApi_get("VoIPAdvanced_Common", "ReleaseMinTime", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "ReleaseMinTime", "s") else asp_Write("") end if %>">
															&nbsp;(300-1199ms)
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep305Text","s")%> </td>
														<td class="table1_right">
															<select id="megacocalleridmode" name="megacocalleridmode">
																<option value="0" <%if tcWebApi_get("VoIPH248_Common", "ComingCallerIDMode", "h") = "0" then asp_Write("selected") end if %>>FSK</option>
																<option value="1" <%if tcWebApi_get("VoIPH248_Common", "ComingCallerIDMode", "h") = "1" then asp_Write("selected") end if %>>DTMF</option>                                        
															</select>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep306Text","s")%></td>
														<td class="table1_right">
															<input id="megacojbfixedmode" name="megacojbfixedmode" type="checkbox" <%if tcWebApi_get("VoIPH248_Common", "JitterBufferFixedMode", "h") = "1" then asp_Write("checked") end if %>>
														</td>
													</tr>
													<tr>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep307Text","s")%> </td>
														<td class="table1_right">
															<input id="megacojitterbuflen" name="megacojitterbuflen" type="text" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "JitterBufferLen", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "JitterBufferLen", "s") else asp_Write("") end if %>">
														</td>
													</tr>
													<TR>
														<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep253Text","s")%></td>
														<td class="table1_right">
															<select name="codecRate" size="1">
																<option value="0" <%if tcWebApi_get("VoIPMedia_Common", "SC_ACCT_MEDIA_G723_RATE", "h") = "0" then asp_Write("selected") end if %>>5.3</option>
																<option value="1" <%if tcWebApi_get("VoIPMedia_Common", "SC_ACCT_MEDIA_G723_RATE", "h") = "1" then asp_Write("selected") end if %>>6.3</option>
														</td>
													</TR>
													<% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported", "h") = "Yes"  then %>
														<tr>
															<td class="table1_left"><%tcWebApi_get("String_Entry","VoipStep308Text","s")%> </td>
															<td class="table1_right">
																<input type="checkbox" id="callsync" name="callsync" checked>
															</td>
														</tr>
													<%end if%>
													</TBODY>
												</TABLE>
												<TABLE border=0 cellSpacing=1 cellPadding=0 width="98%">
													<tr>
														<td height="5">
														</td>
													</tr>
													<tr>
														<td>
															<input type="hidden" id="VoIP_H248_flag" name="VoIP_H248_flag" value="0">
															<input type="hidden" id="megacopolreverseflag" name="megacopolreverseflag" value="0">
															<input type="hidden" id="megacofaxredundancyflag" name="megacofaxredundancyflag" value="0">
															<input type="hidden" id="cancellineregflag" name="cancellineregflag" value="0">
															<input type="hidden" id="digitmapswitchflag" name="digitmapswitchflag" value="0">
															<input type="hidden" id="megacojbfixedmodeflag" name="megacojbfixedmodeflag" value="0">
														</td>
													</tr>
													<tr>
														<td align="center">
															<input name="bt_apply" type="button" id="VoiceApplyButton"  value="<%tcWebApi_get("String_Entry","VoipStep65Text","s")%>"  onClick="VoiceApplyBasic()">
														</td>
													</tr>
												</TABLE>
										</DIV>
									</FORM>
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
				<TD>&nbsp;</TD></TR>
			<TR>
				<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
				<TD width=7 background=/img/panel3.gif></TD>
				<TD></TD></TR>
			<TR>
				<TD vAlign=top width=157 bgColor=#e7e7e7></TD>
				<TD width=7 background=/img/panel3.gif></TD>
				<TD></TD></TR>
				</TBODY></TABLE></TD></TR>
				<TR>
				<TD height=1>
				<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
				border=0><TBODY>
					<TR>
						<TD width=162 bgColor=#ef8218></TD>
						<TD width=278 bgColor=#427594></TD>
						<TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
						<TD width=170 bgColor=#313031></TD>
					</TR></TBODY>
				</TABLE>
			</TD></TR>
</TBODY></TABLE></BODY></HTML>
