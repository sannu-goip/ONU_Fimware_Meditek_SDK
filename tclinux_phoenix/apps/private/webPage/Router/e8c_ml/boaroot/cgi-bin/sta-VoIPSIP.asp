<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
	
	<HEAD>
		<TITLE>
			<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP1Text","s")%>
		</TITLE>
		<META http-equiv=Content-Language content=zh-cn>
		<META content="MSHTML 6.00.6000.16809" name=GENERATOR>
		<META http-equiv=Content-Type content="text/html; charset=gb2312">
		<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
		<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
		<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
		<SCRIPT language=JavaScript src="/JS/printtable.js"></SCRIPT>
		<SCRIPT language=JavaScript type=text/javascript>
		<% if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "H.248"  then %>
			location.replace("/cgi-bin/sta-VoIP248.asp");
		<% end if %>
		function VoipInfoClass()
		{
			this.Reg1Status = '<%if tcWebAPI_get("InfoVoIP_Entry0", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry0", "Status", "s") else asp_Write("") end if %>';
			this.Line1Status = '<%if tcWebApi_get("VoIPSysParam_Entry0", "SC_LINE_INFO_STATUS", "h") <> "N/A" then tcWebAPI_get("VoIPSysParam_Entry0", "SC_LINE_INFO_STATUS", "s") else asp_Write("") end if %>';
			
			this.Reg2Status = '<%if tcWebAPI_get("InfoVoIP_Entry1", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry1", "Status", "s") else asp_Write("") end if %>';
			this.Line2Status = '<%if tcWebApi_get("VoIPSysParam_Entry1", "SC_LINE_INFO_STATUS", "h") <> "N/A" then tcWebAPI_get("VoIPSysParam_Entry1", "SC_LINE_INFO_STATUS", "s") else asp_Write("") end if %>';
			
			this.Reg3Status = '<%if tcWebAPI_get("InfoVoIP_Entry2", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry2", "Status", "s") else asp_Write("") end if %>';
			this.Line3Status = '<%if tcWebApi_get("VoIPSysParam_Entry2", "SC_LINE_INFO_STATUS", "h") <> "N/A" then tcWebAPI_get("VoIPSysParam_Entry2", "SC_LINE_INFO_STATUS", "s") else asp_Write("") end if %>';
			
			this.Reg4Status = '<%if tcWebAPI_get("InfoVoIP_Entry3", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry3", "Status", "s") else asp_Write("") end if %>';
			this.Line4Status = '<%if tcWebApi_get("VoIPSysParam_Entry3", "SC_LINE_INFO_STATUS", "h") <> "N/A" then tcWebAPI_get("VoIPSysParam_Entry3", "SC_LINE_INFO_STATUS", "s") else asp_Write("") end if %>';
			

			var line1number='<%if tcWebApi_get("VoIPBasic_Common","SIPProtocol", "h") = "SIP" then asp_Write("") elseif tcWebAPI_get("VoIPBasic_Entry0", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "SIPDisplayName", "s") else asp_Write("") end if %>';
			if(line1number == ''){
				var tmp='<%if tcWebAPI_get("VoIPBasic_Entry0", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "SIPAuthenticationName", "s") else asp_Write("") end if %>';
				if(tmp.indexOf('@') >= 0)
					line1number=tmp.substr(0,tmp.indexOf('@'));
				else
					line1number=tmp;
			}
			this.Line1Number = line1number;

			var line2number = '<%if tcWebApi_get("VoIPBasic_Common","SIPProtocol", "h") = "SIP" then asp_Write("") elseif tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayName", "s") else asp_Write("") end if %>';
			if(line2number == ''){
				var tmp2='<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPAuthenticationName", "s") else asp_Write("") end if %>';
				if(tmp2.indexOf('@') >= 0)
					line2number=tmp2.substr(0,tmp2.indexOf('@'));
				else
					line2number=tmp2;
			}
			this.Line2Number = line2number;

			var line3number = '<%if tcWebApi_get("VoIPBasic_Common","SIPProtocol", "h") = "SIP" then asp_Write("") elseif tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayName", "s") else asp_Write("") end if %>';
			if(line3number == ''){
				var tmp3='<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPAuthenticationName", "s") else asp_Write("") end if %>';
				if(tmp3.indexOf('@') >= 0)
					line3number=tmp3.substr(0,tmp3.indexOf('@'));
				else
					line3number=tmp3;
			}
			this.Line3Number = line3number;

			var line4number = '<%if tcWebApi_get("VoIPBasic_Common","SIPProtocol", "h") = "SIP" then asp_Write("") elseif tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayName", "s") else asp_Write("") end if %>';
			if(line4number == ''){
				var tmp4='<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPAuthenticationName", "s") else asp_Write("") end if %>';
				if(tmp4.indexOf('@') >= 0)
					line4number=tmp4.substr(0,tmp4.indexOf('@'));
				else
					line4number=tmp4;
			}
			this.Line4Number = line4number;
			
			this.showRegisterState = showRegisterState;
			this.showVoipNumber = showVoipNumber;
			this.showLineState = showLineState;
		}

		function showRegisterState(port)
		{
			var Status = '';
			var LineEnable='';

			if ( 1 == port ){
				Status = this.Reg1Status;
				LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry0", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "Enable", "s") else asp_Write("") end if %>';
				RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry0", "RegFailReason", "s") %>";

			}
			else if ( 2 == port ){
				Status = this.Reg2Status;
				LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry1", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "Enable", "s") else asp_Write("") end if %>';
				RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry1", "RegFailReason", "s") %>";
			}
			else if ( 3 == port ){
				Status = this.Reg3Status;
				LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry2", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "Enable", "s") else asp_Write("") end if %>';
				RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry2", "RegFailReason", "s") %>";
			}
			else if ( 4 == port ){
				Status = this.Reg3Status;
				LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry3", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "Enable", "s") else asp_Write("") end if %>';
				RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry3", "RegFailReason", "s") %>";
			}
			else
				return;
						if(LineEnable == 'No'){
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP2Text","s")%>');
							return;
						}
						if ( Status == 'Up' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP3Text","s")%>');
						else if ( Status == 'Initializing' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP4Text","s")%>');
						else if ( Status == 'Registering' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP5Text","s")%>');
						else if ( Status == 'Unregistering' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP6Text","s")%>');
						else if ( Status == 'Quiescent' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP7Text","s")%>');
						else if ( Status == 'Disabled' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP2Text","s")%>');
						else if ( Status == 'Error' ){
							switch ( parseInt(RegFailReason) ){
								case 2:
								case 3:
									voiptatus = '<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP9Text","s")%>';
									break;
								case 4:
									voiptatus = '<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP10Text","s")%>';
									break;
								case 6:
									voiptatus = '<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP11Text","s")%>';
									break;
								default:
									voiptatus = '<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP12Text","s")%>';
									break;
							}
							document.write(voiptatus);
							}
						else if ( Status == 'Testing' )
							document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP13Text","s")%>');
						else
							document.write('--');
		}

		function showVoipNumber(port)
		{
			var Number = '';
			var DigNum = '';
			var k;
			
			if ( 1 == port )
				Number = this.Line1Number
			else if ( 2 == port )
				Number = this.Line2Number
			else if ( 3 == port )
				Number = this.Line3Number
			else if ( 4 == port )
				Number = this.Line4Number
			else
				return;
			
			k = Number.split('@');
			DigNum = k[0];
			
			document.write(DigNum);
		}

		function showLineState(port)
		{
			var lineStatus = '';
			
			if ( 1 == port ){
				lineStatus = this.Line1Status;
			}
			else if ( 2 == port ){
				lineStatus = this.Line2Status;
			}
			else if ( 3 == port ){
				lineStatus = this.Line3Status;
			}
			else if ( 4 == port ){
				lineStatus = this.Line4Status;
			}
			else
				return;
			if ( lineStatus == 'Idle' )
				document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP14Text","s")%>');
			else if ( lineStatus == 'Ringing' )
				document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP15Text","s")%>');
			else if ( lineStatus == 'Dialing' )
				document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP16Text","s")%>');
			else if ( lineStatus == 'Ringback' )
				document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP17Text","s")%>');
			else if ( lineStatus == 'Connect' )
				document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP18Text","s")%>');
			else if ( lineStatus == 'Disconnect' )
				document.write('<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP19Text","s")%>');	
			else
				document.write('--');
		}	
		var VoipInfo = new VoipInfoClass();

	
		function checkLineNum()
		{
			var i, j, k;
			var lineNumber = parseInt(<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>);
			var table = document.getElementById("perLineTablePlaceholder");
			for(i = 0; i < lineNumber; i++)
			{
				var tabletr = table.getElementsByTagName("tr");
				if(lineNumber > 0)
				{
					for(j = 0; j < tabletr.length; j++)
					{
						var tabletd = tabletr[j].getElementsByTagName("td");
						for(k = lineNumber + 1; k < tabletd.length; k++)
						{
							tabletd[k].style.display = "none";
						}
					}
				}
			}
		}
		</SCRIPT>
	</HEAD>

	<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000
		leftMargin=0 topMargin=0 onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('perLineTablePlaceholder') != null)checkLineNum()"
		onunload=DoUnload() marginheight="0" marginwidth="0">
		<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center border=0>
			<TBODY>
				<TR>
					<TD height=1>
						<% if tcwebApi_get("WebCustom_Entry","isCYE8SFUSupported","h") = "Yes" then %>
							<TABLE height=80 cellSpacing=0 cellPadding=0 width=808 background=/img/framelogo.jpg border=0>
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
										<TD>
											&nbsp;
										</TD>
										<TD vAlign=bottom align=right width=358>
											<TABLE id=table8 cellSpacing=0 cellPadding=0 border=0>
												<TBODY>
													<TR>
														<TD vAlign=bottom align=right>
															<SPAN class=curUserName>
																&nbsp;
															</SPAN>
														</TD>
														<TD class=welcom vAlign=bottom align=middle width=120>
															<%tcWebApi_get("String_Entry","WelcomeSettingText","s")%>
														</TD>
														<TD vAlign=bottom width=50>
															<A onclick=DoLogout() href="/cgi-bin/logout.cgi" id=logOut name=logOut target=_top>
																<SPAN class=logout>
																	<%tcWebApi_get("String_Entry","LogoutText","s")%>
																</SPAN>
															</A>
														</TD>
													</TR>
												</TBODY>
											</TABLE>
										</TD>
									</TR>
								</TBODY>
						</TABLE>
						<TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
							<TBODY>
								<TR>
									<TD class=LocationDisplay id=LocationDisplay align=middle width=163 bgColor=#ef8218
									rowSpan=3>
									</TD>
									<TD width=434 bgColor=#427594 height=33>
										<P align=right>
											<FONT face=黑体 color=#ffffff>
												<B>
													<FONT face=黑体 color=#ffffff size=6>
														<INPUT id=Selected_Menu type=hidden value="<%tcWebApi_get("String_Entry","ContentStatusText","s")%>-><%tcWebApi_get("String_Entry","ContentVoIPInfoText","s")%>" name=Selected_Menu>
													</FONT>
												</B>
												<SPAN class=GatewayName>
													<%tcWebApi_get("String_Entry","GatewayNameText","s")%>
													<SCRIPT language=javascript>
														document.write(top.gateWayName);
													</SCRIPT>
												</SPAN>
											</FONT>
										</P>
									</TD>
									<TD width=211 bgColor=#ef8218 height=33>
										<P class=GatewayType align=center>
											<%tcWebApi_get("String_Entry","ModeText","s")%>
											<SCRIPT language=javascript>
												document.write(top.ModelName);
											</SCRIPT>
										</P>
									</TD>
								</TR>
								<TR>
									<TD id=MenuArea_L1 vAlign=bottom bgColor=#ef8218 colSpan=2 height=43>
										&nbsp;
									</TD>
								</TR>
								<TR>
									<TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24>
									</TD>
								</TR>
							</TBODY>
						</TABLE>
						<SCRIPT language=javascript>
							MakeMenu(getElById('Selected_Menu').value);
						</SCRIPT>
						<TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 border=0>
							<TBODY>
								<TR>
									<TD height=15>
										<IMG height=15 src="/img/panel1.gif" width=164 border=0>
									</TD>
									<TD>
										<IMG height=15 src="/img/panel2.gif" width=645 border=0>
									</TD>
								</TR>
							</TBODY>
						</TABLE>
					</TD>
				</TR>
				<TR>
					<TD vAlign=top>
						<TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
							<TBODY>
								<TR>
									<TD class=Item_L1 width=157 bgColor=#ef8218 height=30>
										<P>
											<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP20Text","s")%>
										</P>
									</TD>
									<TD width=7 bgColor=#ef8218>
										　
									</TD>
									<TD width=474>
										　
									</TD>
									<TD vAlign=top width=170 background=/img/panel4.gif rowSpan=4>
										<TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
											<TBODY>
												<TR>
													<TD valign='top'>
														<A href="/cgi-bin/help_content.asp#宽带语音信息" target=_blank>
															<IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 border=0>
														</A>
													</TD>
												</TR>
												<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
													<TR>
														<TD valign='bottom'>
															<IMG src='/img/tybottom.jpg' height=76 width=112>
														</TD>
													</TR>
												<% end if %>
											</TBODY>
										</TABLE>
									</TD>
								</TR>
								<TR>
									<TD vAlign=top width=157 bgColor=#e7e7e7 height=10>
									</TD>
									<TD width=7 background=/img/panel3.gif>
										　
									</TD>
									<TD>
									</TD>
								</TR>
								<TR>
									<TD class=Item_L2 vAlign=top width=157 bgColor=#e7e7e7 height=30>
										<P>
										</P>
									</TD>
									<TD width=7 background=/img/panel3.gif>
										　
									</TD>
									<TD vAlign=top>
										<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
											<TBODY>
												<TR>
													<TD width=10>
														&nbsp;
													</TD>
													<TD>
														<TABLE id=voipSIP_info name=voipSIP_info cellSpacing=0 cellPadding=3 width="100%" border=1>
															<TBODY>
																<TR>
																	<TD class=table_title>
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP21Text","s")%>
																	</TD>
																	<TD>
																		<%if tcWebApi_get("VoIPBasic_Common", "RegistrarServer", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "RegistrarServer", "s") else asp_Write("--") end if %>
																			&nbsp;
																	</TD>
																</TR>

																<TR>
																	<TD class=table_title>
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP22Text","s")%>
																	</TD>
																	<TD>
																		<%if tcWebApi_get("VoIPBasic_Common", "RegistrarServerPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "RegistrarServerPort", "s") else asp_Write("--") end if %>
																			&nbsp;
																	</TD>
																</TR>
																<TR>
																	<TD class=table_title>
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP23Text","s")%>
																	</TD>
																	<TD>
																		<%if tcWebApi_get("VoIPBasic_Common", "SBRegistrarServer", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBRegistrarServer", "s") else asp_Write("--") end if %>
																			&nbsp;
																	</TD>
																</TR>
																<TR>
																	<TD class=table_title>
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP24Text","s")%>
																	</TD>
																	<TD>
																		<%if tcWebApi_get("VoIPBasic_Common", "SBRegistrarServerPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBRegistrarServerPort", "s") else asp_Write("--") end if %>
																			&nbsp;
																	</TD>
																</TR>
															</TBODY>
														</TABLE>
														<BR>
														<BR>
														<TABLE id="perLineTablePlaceholder" style="table-layout: fixed;word-wrap: break-word;" cellSpacing=0 cellPadding=3 width="100%" border=1>
															<TBODY>
																<TR>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP25Text","s")%>
																	</TD>
																	<TD class=table_title  width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP26Text","s")%>
																	</TD>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP27Text","s")%>
																	</TD>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP28Text","s")%>
																	</TD>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP29Text","s")%>
																	</TD>
																</TR>
																<TR>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP30Text","s")%>
																	</TD>
																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showVoipNumber(1);
																		</script>
																			&nbsp;
																	</TD>

																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showVoipNumber(2);
																		</script>
																			&nbsp;
																	</TD>
																		
																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showVoipNumber(3);
																		</script>
																			&nbsp;
																	</TD>

																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showVoipNumber(4);
																		</script>
																			&nbsp;
																	</TD>
																</TR>
																<TR>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP31Text","s")%>
																	</TD>
																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showRegisterState(1);
																		</script>
																			&nbsp;
																	</TD>

																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showRegisterState(2);
																		</script>
																			&nbsp;
																	</TD>

																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showRegisterState(3);
																		</script>
																			&nbsp;
																	</TD>
																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showRegisterState(4);
																		</script>
																			&nbsp;
																		</TD>
																</TR>
																<TR>
																	<TD class=table_title width="20%">
																		<%tcWebApi_get("String_Entry","VoipInfoVoIPSIP32Text","s")%>
																	</TD>
																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showLineState(1);
																		</script>
																			&nbsp;
																	</TD>

																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showLineState(2);
																		</script>
																			&nbsp;
																	</TD>

																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showLineState(3);
																		</script>
																			&nbsp;
																	</TD>
																	<TD>
																		<script language=JavaScript type=text/javascript>
																			VoipInfo.showLineState(4);
																		</script>
																			&nbsp;
																	</TD>
																</TR>
															</TBODY>
														</TABLE>
													</TD>
													<TD width=10>
														&nbsp;
													</TD>
												</TR>
												<TR>
													<TD width=10>
														&nbsp;
													</TD>
													<TD>
													</TD>
													<TD width=10>
														&nbsp;
													</TD>
												</TR>
											</TBODY>
										</TABLE>
									</TD>
								</TR>
								<TR>
									<TD vAlign=top width=157 bgColor=#e7e7e7>
									</TD>
									<TD width=7 background=/img/panel3.gif>
									</TD>
									<TD>
									</TD>
								</TR>
							</TBODY>
						</TABLE>
					</TD>
				</TR>
				<TR>
					<TD height=1>
						<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 border=0>
							<TBODY>
								<TR>
									<TD width=162 bgColor=#ef8218>
										　
									</TD>
									<TD width=278 bgColor=#427594>
										　
									</TD>
									<TD width=196 bgColor=#427594>
										<P align=center>
										</P>
									</TD>
									<TD width=170 bgColor=#313031>
										　
									</TD>
								</TR>
							</TBODY>
						</TABLE>
					</TD>
				</TR>
			</TBODY>
		</TABLE>
	</BODY>

</HTML>