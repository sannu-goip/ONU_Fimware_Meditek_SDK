<%
if Request_Form("Save_Flag") = "1" then
		if Request_Form("Action_Flag") = "1" then
			if Request_Form("MapEnable") = "1" then
				if Request_Form("trigger_uplink_ap_selection_Flag") = "1" then
						TCWebApi_constSet("Mesh_action", "trigger_uplink_ap_selection", "1")
						TCWebApi_commit("Mesh_action")
				end if
				
				if Request_Form("trigger_channel_planning_Flag") = "1" then
						TCWebApi_constSet("Mesh_action", "trigger_channel_planning", "1")
						TCWebApi_Set("Mesh_actionpara", "channel_planning_channel", "channel_inp")
						TCWebApi_commit("Mesh_action")
				end if
				
				if Request_Form("trigger_channel_force_switching_Flag") = "1" then
						TCWebApi_constSet("Mesh_action", "trigger_channel_force_switching", "1")
						TCWebApi_Set("Mesh_actionpara", "channel_force_al_mac", "targel_al_mac_inp")
						TCWebApi_Set("Mesh_actionpara", "channel_force_2g_channel", "_2g_channel_inp")
						TCWebApi_Set("Mesh_actionpara", "channel_force_5gl_channel", "_5g_channel_inp")
						TCWebApi_constSet("Mesh_actionpara", "channel_force_5gh_channel", "0")
						TCWebApi_commit("Mesh_action")
				end if

				if Request_Form("trigger_mandate_steering_on_agent_Flag") = "1" then
						TCWebApi_Set("Mesh_actionpara", "mandate_steer_sta_mac", "sta_mac_inp")
						TCWebApi_Set("Mesh_actionpara", "mandate_steer_target_bssid", "target_bssid_inp")

						TCWebApi_constSet("Mesh_action", "trigger_mandate_steering", "1")
						TCWebApi_commit("Mesh_action")
				end if
				
				if Request_Form("trigger_back_haul_steering_on_agent_Flag") = "1" then
						TCWebApi_Set("Mesh_actionpara", "steer_back_haul_mac", "back_haul_mac_inp")
						TCWebApi_Set("Mesh_actionpara", "steer_back_haul_target_bssid", "back_haul_target_bssid_inp")
						
						TCWebApi_constSet("Mesh_action", "trigger_backhaul_steering", "1")
						TCWebApi_commit("Mesh_action")
				end if
				
				if Request_Form("trigger_wps_fh_agent_Flag") = "1" then
						TCWebApi_Set("Mesh_actionpara", "wps_front_haul_mac", "fh_mac_inp")
						
						TCWebApi_constSet("Mesh_action", "trigger_wps_fh_agent", "1")
						TCWebApi_commit("Mesh_action")
				end if
				if Request_Form("apply_bh_priority_Flag") = "1" then
						TCWebApi_Set("Mesh_mapdcfg", "BhPriority2G", "BhPriority2G")
						TCWebApi_Set("Mesh_mapdcfg", "BhPriority5GL", "BhPriority5GL")
						TCWebApi_Set("Mesh_mapdcfg", "BhPriority5GH", "BhPriority5GL")
						TCWebApi_constSet("Mesh_action", "apply_bh_priority", "1")						
						TCWebApi_commit("Mesh_action")
				end if
				if Request_Form("trigger_channel_planning_r2_Flag") = "1" then
						TCWebApi_constSet("Mesh_action","trigger_channel_planning_r2","1") 
						TCWebApi_constSet("Mesh_actionpara","trigger_channel_planning_r2","1") 
						TCWebApi_commit("Mesh_action")
				end if
				if Request_Form("trigger_channel_planning_r2_Flag") = "2" then
						TCWebApi_constSet("Mesh_action","trigger_channel_planning_r2","2") 
						TCWebApi_constSet("Mesh_actionpara","trigger_channel_planning_r2","2") 
						TCWebApi_commit("Mesh_action")
				end if
				if Request_Form("trigger_channel_r2_scan__Flag") = "1" then
						TCWebApi_constSet("Mesh_action","trigger_ChPlanning_by_almac","1")
						TCWebApi_Set("Mesh_actionpara","trigger_ChPlanning_by_almac","_mac") 
						TCWebApi_commit("Mesh_action")
				end if
				TCWebApi_save()
			end if			
	end if
end if
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>EasyMesh</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/json2.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 leftMargin=0 topMargin=0 
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
								<TD vAlign=bottom align=right><SPAN class=curUserName>&nbsp; </SPAN></TD>
								<TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%> </TD>
								<TD vAlign=bottom width=50>
									<A onclick=DoLogout() href="/cgi-bin/login.asp" target=_top><SPAN class=logout><%tcWebApi_get("String_Entry","LogoutText","s")%> </SPAN></A>
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
					<TD class=LocationDisplay id=LocationDisplay align=middle width=163 bgColor=#ef8218 rowSpan=3></TD>
					<TD width=434 bgColor=#427594 height=33>
						<P align=right>
							<FONT face=<%tcWebApi_get("String_Entry","RomanText","s")%> color=#ffffff>
								<B><FONT face=<%tcWebApi_get("String_Entry","RomanText","s")%> color=#ffffff size=6>
									<INPUT id=Selected_Menu type=hidden value="<%tcWebApi_get("String_Entry","ContentAPPText","s")%>->EasyMesh" name=Selected_Menu>
								</FONT></B>
								<SPAN class=GatewayName><%tcWebApi_get("String_Entry","GatewayNameText","s")%>
									<SCRIPT language=javascript>
										document.write(top.gateWayName);
									</SCRIPT>
								</SPAN>
							</FONT>
						</P>
					</TD>
					<TD width=211 bgColor=#ef8218 height=33>
						<P class=GatewayType align=center><%tcWebApi_get("String_Entry","ModeText","s")%>
							<SCRIPT language=javascript>
								document.write(top.ModelName);
							</SCRIPT>
						</P>
					</TD>
				</TR>
				<TR>
					<TD id=MenuArea_L1 vAlign=bottom bgColor=#ef8218 colSpan=2 height=43>&nbsp;</TD>
				</TR>
				<TR>
					<TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24></TD>
				</TR>
				</TBODY>
			</TABLE>
			<SCRIPT language=javascript>
				MakeMenu(getElById ('Selected_Menu').value);
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

			<TABLE height=100% cellSpacing=0 cellPadding=0 border=0>
				<TBODY>	
				<TR>
					<TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
						<ul class="cbi-tabmenu">
							<li class="cbi-tab-disabled" id="map-cfg-tab-basic">
								<a href="/cgi-bin/wifi_multi_ap_basic.asp"><%tcWebApi_get("String_Entry","EasyMeshBasicTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-adv">
								<a href="/cgi-bin/wifi_multi_ap_advanced.asp"><%tcWebApi_get("String_Entry","EasyMeshAdvancedTest","s")%></a>
							</li>
							<li class="cbi-tab" id="map-cfg-tab-action">
								<a href="/cgi-bin/wifi_multi_ap_action.asp"><%tcWebApi_get("String_Entry","EasyMeshActionTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-status">
								<a href="/cgi-bin/wifi_multi_ap_status.asp"><%tcWebApi_get("String_Entry","EasyMeshStatusTest","s")%></a>
							</li>
						</ul>
					</TD>
					<TD vAlign=top width=7 background=/img/panel3.gif>
						<ul class="cbi-tabmenu">
							<li class="cbi-tab-disabled" id="map-cfg-tab-basic-pel">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-adv-pel">&nbsp;</li>
							<li class="cbi-tab" id="map-cfg-tab-action-pel">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-status-pel">&nbsp;</li>
						</ul>
					</TD>
					<TD vAlign=top width=474 rowspan="2">
						<FORM style="DISPLAY: none" name=ConfigForm></FORM>
						<div id="maincontent" class="container">
								<div id="l1datNot">
									<% if tcWebApi_get("globalstate_common", "wlanLoad", "h") = "0" then %>
									<fieldset class="cbi-section">
						            	<legend class="alert-message error">l1profile.dat file is not available!</legend>
						            </fieldset>
						      <% end if %>
								</div>
								<div id="l1datYes">

								<% if tcWebApi_get("WLan_Common", "MultiSSIDConfigEnable", "h") = "No" then %>
						        <div id="isMapSupportedNo">
						            <fieldset class="cbi-section">
						                <legend class="alert-message error"><%tcWebApi_get("String_Entry","EasyMeshBasic13Test","s")%></legend>
						            </fieldset>
						        </div>
						      	<% else %>
						      	<div id="isMapSupportedYes">

						      		<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "N/A" then %>
							            <div id="isMapCfgSupportedNo">
							                <fieldset class="cbi-section">
							                    <legend class="alert-message error"><%tcWebApi_get("String_Entry","EasyMeshBasic14Test","s")%></legend>
							                </fieldset>
							            </div>
						            <% else %>
						            	<div id="isMapCfgSupportedYes">
						            		<div id="map_cfgs_loading_div" style="display: none">
						            			<div class="alert-message" id="BusyMsg" style="display:none">
						            				<img src="/img/loading.gif" alt="" style="vertical-align:middle" />
						            				<big><strong><%tcWebApi_get("String_Entry","EasyMeshBasic15Test","s")%></strong></big>
						            			</div>
						            		</div>

						            		<div class="alert-message" id="LOADING_MULTI_AP" style="display:none;">
							                    <img src="/img/loading.gif" alt="" style="vertical-align:middle" />
							                    <big><strong><%tcWebApi_get("String_Entry","EasyMeshBasic16Test","s")%></strong></big>
							                </div>
							                <div id="MULTI_AP_SETTINGS">
							                    <input type="hidden" name="__activeTab" id="__activeTab" value="action">
							                    <form method="post" name="cbi" action="/cgi-bin/wifi_multi_ap_action.asp" autocomplete="off">
									                    <input type="hidden" name="MapEnable" value="<% tcWebApi_get("Mesh_dat","MapEnable","s") %>" />
							                    		<input type="hidden" name="SaveAll_Flag" value="0" />
							                    		<input type="hidden" name="Save_Flag" value="0" />
							                    		<input type="hidden" name="Action_Flag" value="0" />
							                    		<input type="hidden" name="trigger_channel_force_switching_Flag" value="0" />
							                    		<input type="hidden" name="trigger_channel_planning_Flag" value="0" />
							                    		<input type="hidden" name="trigger_uplink_ap_selection_Flag" value="0" />
							                    		<input type="hidden" name="trigger_mandate_steering_on_agent_Flag" value="0" />
							                    		<input type="hidden" name="trigger_back_haul_steering_on_agent_Flag" value="0" />
							                    		<input type="hidden" name="trigger_wps_fh_agent_Flag" value="0" />
														<input type="hidden" name="apply_bh_priority_Flag" value="0" />
														
														<input type="hidden" name="_wlan_type" id="_wlan_type" value="">
														<input type="hidden" name="trigger_channel_planning_r2_Flag" id="trigger_channel_planning_r2_Flag" value="0" />
														<input type="hidden" name="_mac" id="_mac" value="">
														<input type="hidden" name="trigger_channel_r2_scan__Flag"    id="trigger_channel_r2_scan__Flag"value="0" />
														<input type="hidden" name="channel_r2_Mac_scan_result_Flag"  id="channel_r2_Mac_scan_result_Flag"value="0" />
														
							                    		<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
							                    		<table class="cbi-section-table" id="map-cfg-action" style="display:none">
							                        	 <tbody id="map-cfg-action-uplink-ap-selection" style="display:none">
							                        	  <tr>
							                        	  	 <td><%tcWebApi_get("String_Entry","EasyMeshAction19Text","s")%></td>
							                        	  	 <td colspan="2">
							                        	  	 	<input type="button" id="uplink-ap-selection-btn" value="<%tcWebApi_get("String_Entry","EasyMeshAction20Text","s")%>" onclick="triggerUplinkApSelection()">
							                        	  	 </td>
							                          	  </tr>
							                        	 </tbody>
							                        	 <tbody id="map-cfg-action-controller-atc" style="display:none">
							                        	 	<% if tcWebApi_get("WebCustom_Entry","isNewEasyMeshSupport","h") = "Yes"  then %>
							                        	 		<tr>
							                        	 			<td rowspan="2" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction12Text","s")%></td>
						                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction13Text","s")%></td>
						                                  <td>
						                                      <input type="text" id="channel-planning-channel" name="channel_inp" />
						                                  </td>
							                        	 		</tr>
							                              <tr>
							                                  <td colspan="2">							                                  
							                                      <input type="button" id="channel-planning-apply" value="<%tcWebApi_get("String_Entry","EasyMeshAction14Text","s")%>" onClick="triggerChannelPlanning()" />
							                                  </td>
							                              </tr>
							                              
														  <% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
							                              <tr>
							                        	 			<td rowspan="2" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction21Text","s")%></td>
						                                  <td>
						                                  	 <select name="wlanType" size="1" id="add_wlan_type_select" onchange="">
												                            <option value="1">2.4G</option>
												                            <option value="2">5G</option>
												                       	 </select>
						                                  </td>
						                                  <td>						                                      	
							                                  	<input type="button" id="trigger_channel_planning_r2_button" style="padding:5px" value="<%tcWebApi_get("String_Entry","EasyMeshAction22Text","s")%>" onClick="triggerChannelR2WlanTypeButton()"></>
																<a id="auto" href="#Mesh_scan_info_div"></a>
						                                  </td>
							                        	 		</tr>							                        	 		
							                              <tr>
							                              	 	<td>
							                                      <input type="button" id="display_channel_scan_result_button" style="margin:5px" value="<%tcWebApi_get("String_Entry","EasyMeshAction23Text","s")%>" onClick="triggerChannelR2WlanTypeScanResult()" />
							                                  </td>
							                                  <td>
							                                      <input type="button" id="display_channel_planning_score_button" style="margin:5px"  value="<%tcWebApi_get("String_Entry","EasyMeshAction24Text","s")%>" onClick="triggerChannelR2WlanTypeScore()" />
							                                  </td>
							                              </tr>
							                             
							                               <tr>
							                        	 			<td rowspan="2" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction25Text","s")%></td>
						                                  <td>
						                                  	 <select name="add_Mac_select" size="1" id="add_Mac_select" onchange="">
												             </select>
						                                  </td>
						                                  <td>						                                      	
							                                  	<input type="button" id="trigger_channel_planning_r2_mac_button" style="margin:5px" value="<%tcWebApi_get("String_Entry","EasyMeshAction26Text","s")%>" onClick="triggerChannelR2MacTypeScanBt()"></>
						                                  </td>
							                        	 		</tr>							                        	 		
							                              <tr>
							                              	 	<td colspan="2">
							                                      <input type="button" id="channel_r2_mac_scan_result_button" style="margin:5px" value="<%tcWebApi_get("String_Entry","EasyMeshAction27Text","s")%>" onClick="triggerChannelR2MacTypeScanResult()"></>
																</td>							                                 
							                              </tr>
							                              <% end if %>
														  <% if tcWebApi_get("Mesh_Mapdcfg", "DivergentChPlanning", "h") = "1" then %>
							                        	 		<tr>
							                        	 			<td rowspan="4" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction15Text","s")%></td>
						                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction16Text","s")%></td>
						                                  <td>
						                                      <input type="text" id="channel-force-switching-targel-al-mac" name="targel_al_mac_inp" />
						                                  </td>
							                        	 		</tr>
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction17Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="channel-force-switching-2g-channel" name="_2g_channel_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction18Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="channel-force-switching-5g-channel" name="_5g_channel_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="channel-force-switching-apply" value="<%tcWebApi_get("String_Entry","EasyMeshAction14Text","s")%>" onClick="triggerChannelForceSwitching()" />
							                                  </td>
							                              </tr>
														  <% end if %>
							                            <% end if %>
							                              <tr>
							                                  <td rowspan="3" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction1Text","s")%></td>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction2Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="mandate-steering-agent-sta-mac" name="sta_mac_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction3Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="mandate-steering-agent-target-bssid" name="target_bssid_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="mandate-steering-agent-apply" value="<%tcWebApi_get("String_Entry","EasyMeshAction4Text","s")%>" onClick="triggerMandateSteeringOnAgent()" />
							                                  </td>
							                              </tr>
							                              
							                              <tr>
							                                  <td rowspan="3" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction5Text","s")%></td>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction6Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="back-haul-steering-mac" name="back_haul_mac_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction3Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="back-haul-steering-target-bssid" name="back_haul_target_bssid_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="back-haul-steering-apply" value="<%tcWebApi_get("String_Entry","EasyMeshAction7Text","s")%>" onClick="triggerBackHaulSteering()" />
							                                  </td>
							                              </tr>
							                              
							                              </tbody>
							                              <tbody id="map-cfg-action-trigger-wpsfhagent" style="display:none">
							                              <tr>
							                                  <td rowspan="2" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction8Text","s")%></td>
							
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAction9Text","s")%></td>
							                                  <td>
							                                      <input type="text" id="trigger-wps-fh-agent-al-mac" name="fh_mac_inp" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="trigger-wps-fh-agent-apply" value="<%tcWebApi_get("String_Entry","EasyMeshAction8Text","s")%>" onClick="triggerWpsFhAgent()" />
							                                  </td>
							                              </tr>
							                          </tbody>
							                          <tbody id="map-cfg-basic-wireless-bh-preference" style="display:none">
					                                  <tr>
					                                  		<td rowspan="4" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAction10Text","s")%></td>
					                                      <td>
					                                          <span>2G</span>
					                                      </td>
					                                      <td>
					                                          <select name="BhPriority2G" size="1">
					                                          			<option value="0" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority2G", "h") = "0" then %> selected="selected" <% end if %>>Disable</option>
				                		          				            <option value="1" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority2G", "h") = "1" then %> selected="selected" <% end if %>>High</option>
				                		          				            <option value="2" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority2G", "h") = "2" then %> selected="selected" <% end if %>>Medium</option>
				                		          				            <option value="3" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority2G", "h") = "3" then %> selected="selected" <% end if %>>Low</option>
					                                          </select>
					                                      </td>
					                                  </tr>
					                                  <tr>
					                                      <td>
					                                          <span>5G</span>
					                                      </td>
					                                      <td>
					                                          <select name="BhPriority5GL" size="1">
					                                          			<option value="0" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority5GL", "h") = "0" then %> selected="selected" <% end if %>>Disable</option>
				                		          				            <option value="1" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority5GL", "h") = "1" then %> selected="selected" <% end if %>>High</option>
				                		          				            <option value="2" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority5GL", "h") = "2" then %> selected="selected" <% end if %>>Medium</option>
				                		          				            <option value="3" <% if tcWebApi_get("Mesh_mapdcfg", "BhPriority5GL", "h") = "3" then %> selected="selected" <% end if %>>Low</option>
					                                          </select>
					                                      </td>
					                                  </tr>
					                                  <tr>
					                                      <td colspan="2">
					                                        <input type="button" id="apply-bh-priority-btn" onClick="applyBhPriority()" class="cbi-section-table-button" value="<%tcWebApi_get("String_Entry","EasyMeshAction11Text","s")%>" />
							                                  </td>
							                              </tr>
							                          </tbody>
							                        </table>
							                    <% end if %><!-- Mesh_mapdcfg.MapEnable=1  -->
							                    </form>
							                </div>
							
							                <SCRIPT language=JavaScript type=text/javascript>
																	
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																	var get_device_role_timer_id;
																	<% end if %>
																	var mesh_get_ch_scan_dump="";
																	var mesh_get_ch_score_dump="";
																	var mesh_scan_or_score_flag="";
																	var max_cellspan = 2;
																	function JSO_Only_Status(staType)
																	{
																			var ret = ' { ' + 
																				'"status": "' + staType + '"' + 
																				' } ';
																			return ret;
																	}
																	
																	function JSO_get_apply_status()
																	{
																			var staType = 'DONE';
																			var webstate = "<% tcWebApi_get("Mesh_action","map_action_complete","s") %>";
																			
																			if(1 == webstate)
																			{
																					staType = 'ON_PROGRESS';
																			}
																			else
																			{
																					staType = 'DONE';
																			}
																			
																			var ret = JSO_Only_Status(staType);
																			return ret;
																	}
																	
																	function JSO_get_device_role()
																	{
																			var ret = '';
																			var mapDevRole = "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																	<% if mapDevRole <> "N/A" then %>
																			ret = '{ ';
																			ret = ret +	'"mapDevRole": "' + mapDevRole + '"';
																			ret = ret + ' }';
																	<% end if %>
																			return ret;
																	}
																	
																	function getX_Response(type)
																	{
																			var X_Res='';
																			try
																			{
																					switch(type)
																					{
																							case "get_apply_status":
																								X_Res = JSO_get_apply_status();
																								break;
																							case "get_device_role":
																								X_Res = JSO_get_device_role();
																								break;
																							default:
																								break;
																					}
																			}
																			catch(e)
																			{
																					X_Res='';
																			}
																			
																			return X_Res;
																	}
																	
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																	
																	function clearAllTimers()
																	{
																	    clearTimeout(get_device_role_timer_id);
																	    get_device_role_timer_id = null;
																	    clearTimeout(get_apply_status);
																	}
																	
																	function getDeviceRole()
																	{
																		var XHR_type = 'get_device_role';
																		var x_response = getX_Response(XHR_type);
																	    {
																	        try
																	        {
																	            var devRoleObj = JSON.parse(x_response);
																	            var devRole = parseInt(devRoleObj.mapDevRole);
																	            if(isNaN(devRole))
																	            {
																	                console.log("Incorrect Device Role",devRole);
																	                devRole = 0;
																	            }
																	            switch(devRole)
																	            {
																	                case 0: // Auto
																	                    document.getElementById("map-cfg-tab-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "none";
																	                    document.getElementById("map-cfg-action").style.display = "";
																	                    document.getElementById("map-cfg-action-uplink-ap-selection").style.display = "";
																	                    document.getElementById("map-cfg-action-controller-atc").style.display = "none";
																	                    document.getElementById("map-cfg-action-trigger-wpsfhagent").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																					        		document.getElementById("map-cfg-basic-wireless-bh-preference").style.display = "";
																	                		break;
																	                case 1: // Controller
																	                    document.getElementById("map-cfg-tab-adv").style.display = "";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-action").style.display = "";
																	                    document.getElementById("map-cfg-action-uplink-ap-selection").style.display = "none";
																	                    document.getElementById("map-cfg-action-controller-atc").style.display = "";
																	                    document.getElementById("map-cfg-action-trigger-wpsfhagent").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																					        		document.getElementById("map-cfg-basic-wireless-bh-preference").style.display = "none";
																	                		break;
																	                case 2: // Agent
																	                    document.getElementById("map-cfg-tab-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "none";
																	                    document.getElementById("map-cfg-action").style.display = "";
																	                    document.getElementById("map-cfg-action-uplink-ap-selection").style.display = "";
																	                    document.getElementById("map-cfg-action-controller-atc").style.display = "none";
																	                    document.getElementById("map-cfg-action-trigger-wpsfhagent").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																					        		document.getElementById("map-cfg-basic-wireless-bh-preference").style.display = "";
																	                		break;
																	                default:
																	                    console.log("Invalid device role received: ",devRole);
																	                		break;
																	            }
																	        }
																	        catch(e)
																	        {
																	            console.log("Incorrect response received for get device role request!");
																	            clearTimeout(get_device_role_timer_id);
																	            get_device_role_timer_id = null;
																	            if(document.getElementById("__activeTab").value == "basic")
																	            {
																	                get_device_role_timer_id = setTimeout(getDeviceRole, 5000);
																	            }
																	        }
																	    }
																	}
																	
																	function triggerMandateSteeringOnAgent()
																	{
																		clickButton();
																	    var sta_mac_inp = document.getElementById("mandate-steering-agent-sta-mac");
																	    var mac_reg_exp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
																	    if (!mac_reg_exp.test(sta_mac_inp.value)) 
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshActionAlert1Text","s")%> (XX:XX:XX:XX:XX:XX)");
																	        sta_mac_inp.focus();
																	        sta_mac_inp.select();
																	        return;
																	    }
																	    var target_bssid_inp = document.getElementById("mandate-steering-agent-target-bssid");
																	    if (!mac_reg_exp.test(target_bssid_inp.value)) 
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshActionAlert2Text","s")%> (XX:XX:XX:XX:XX:XX)");
																	        target_bssid_inp.focus();
																	        target_bssid_inp.select();
																	        return;
																	    }
																	    
																	    setText('trigger_mandate_steering_on_agent_Flag', 1);
																		setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	function triggerBackHaulSteering()
																	{
																		clickButton();
																	    var back_haul_mac_inp = document.getElementById("back-haul-steering-mac");
																	    var mac_reg_exp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
																	    if (!mac_reg_exp.test(back_haul_mac_inp.value)) 
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshActionAlert3Text","s")%>");
																	        back_haul_mac_inp.focus();
																	        back_haul_mac_inp.select();
																	        return;
																	    }
																	    var back_haul_target_bssid_inp = document.getElementById("back-haul-steering-target-bssid");
																	    if (!mac_reg_exp.test(back_haul_target_bssid_inp.value)) 
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshActionAlert2Text","s")%> (XX:XX:XX:XX:XX:XX)");
																	        back_haul_target_bssid_inp.focus();
																	        back_haul_target_bssid_inp.select();
																	        return;
																	    }
																	    
																	    setText('trigger_back_haul_steering_on_agent_Flag', 1);
																		setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	function triggerWpsFhAgent()
																	{
																		clickButton();
																	    var fh_mac_inp = document.getElementById("trigger-wps-fh-agent-al-mac");
																	    var mac_reg_exp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
																	    if (!mac_reg_exp.test(fh_mac_inp.value)) 
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshActionAlert4Text","s")%> (XX:XX:XX:XX:XX:XX)");
																	        fh_mac_inp.focus();
																	        fh_mac_inp.select();
																	        return;
																	    }
																	    
																	    setText('trigger_wps_fh_agent_Flag', 1);
																		setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	function triggerUplinkApSelection()
																	{	clickButton();
																	 	setText('trigger_uplink_ap_selection_Flag', 1);
																	    setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	function triggerChannelPlanning()
																	{	
																		clickButton();
																	 	setText('trigger_channel_planning_Flag', 1);
																	    setText('Action_Flag', 1);
																	    formSubmit(0);																		
																	}
																	
																	function triggerChannelForceSwitching()
																	{
																		clickButton();
																		var al_mac_inp = document.getElementById("channel-force-switching-targel-al-mac");
																    var mac_reg_exp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
																    if (!mac_reg_exp.test(al_mac_inp.value)) 
																    {
																        alert("<%tcWebApi_get("String_Entry","EasyMeshActionAlert2Text","s")%> (XX:XX:XX:XX:XX:XX)");
																        al_mac_inp.focus();
																        al_mac_inp.select();
																        return;
																    }
																	    
																		setText('trigger_channel_force_switching_Flag', 1);
																	    setText('Action_Flag', 1);
																	    formSubmit(0);	
																	}
																	
																	function applyBhPriority()
																	{		clickButton();
																			setText('apply_bh_priority_Flag', 1);
																			setText('Action_Flag', 1);
																			formSubmit(0);
																	}
																	
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */

																	function checkWebStatus()
																	{
																		location.href = '/cgi-bin/wifi_multi_ap_action.asp';
																	}
																	
																	function get_apply_status_cb(rsp)
																	{
																		clearTimeout(checkWebStatus);
																	    try
																	    {
																	        var r = JSON.parse(rsp);
																	    }
																	    catch(e)
																	    {
																	        return;
																	    }
																	    if(r.status == "ON_PROGRESS")
																	    {
																	        var altmsg ="Device is applying the saved settings now!\n" +
																	            "It is recommended to wait until all the saved settings are applied.\n";
																	        //alert(altmsg);
																	        
																	        document.getElementById('map_cfgs_loading_div').style.display = '';
																	        document.getElementById('BusyMsg').style.display = '';
																	        document.getElementById('LOADING_MULTI_AP').style.display = 'none';
																	        document.getElementById('MULTI_AP_SETTINGS').style.display = 'none';
																	        setTimeout(checkWebStatus, 5000);
																	    }
																	}
																	
																	function get_apply_status()
																	{
																			var XHR_type = 'get_apply_status';
																			var x_response = getX_Response(XHR_type);
																			{
																					get_apply_status_cb(x_response);
																			}
																	}
																	
																	function LoadMapFrame ()
																	{
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																		var deviceRole = "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																	    if(deviceRole == "")
																	    {
																	    	deviceRole = "0";
																	    }

																			getDeviceRole();
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */
																	}
																	
																	<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
																	/*trigger_channel_planning_r2_button*/
																	function triggerChannelR2WlanTypeButton()
																	{
																		clickButton();
																		var wlanTypeSel = document.getElementById('add_wlan_type_select').value;
																		if(wlanTypeSel == "2")
																		{
																			setText('trigger_channel_planning_r2_Flag', 2);	
																			setText('_wlan_type', 2);
																			setText('Action_Flag', 1);																			
																			formSubmit(1);																			
																		}
																		else
																		{
																			setText('trigger_channel_planning_r2_Flag', 1);	
																			setText('_wlan_type', 1);
																			setText('Action_Flag', 1);
																			formSubmit(1);
																		}
																	}
																	/*trigger_channel_planning_r2_button*/
																	function triggerChannelR2WlanTypeScanResult()
																	{	
																		
																		get_run_time_topology("1");		
																		autoClickA();
																	}
																	/*display_channel_scan_result_button*/
																	function triggerChannelR2WlanTypeScore()
																	{																			
																		get_run_time_topology("2");	
																		autoClickA();																			
																			
																	}
																	/*display_channel_planning_score_button*/
																	function triggerChannelR2MacTypeScanBt()
																	{	
																		clickButton();
																		var MacSel = document.getElementById('add_Mac_select').value;
																		
																		if((MacSel).toString() != "")
																		{																				
																			var MacSelTostring = (MacSel).toString();
																			setText('_mac', MacSelTostring);	
																			setText('trigger_channel_r2_scan__Flag', 1);
																			setText('Action_Flag', 1);
																			formSubmit(1);																			
																		}		
																	}
																	/*channel_r2_mac_scan_result_button*/
																	function triggerChannelR2MacTypeScanResult()
																	{
																		get_run_time_topology("1");
																		autoClickA();		
																	}																	
																	function JSO_get_ch_scan_dump()
																	{
																			mesh_get_ch_scan_dump = "<%tcWebApi_MeshJsonHook("mesh_get_ch_scan_dump") %>";
																			
																			mesh_get_ch_scan_dump = mesh_get_ch_scan_dump.replace(/\'/g,'\\\\u0022');
																			mesh_get_ch_scan_dump = mesh_get_ch_scan_dump.replace(/\\\\u0022Pass-phrase\\\\u0022:/g,'\\\\u0022WPAPSK\\\\u0022:');
																			
																	}
																	
																	function JSO_get_ch_score_dump()
																	{
																			mesh_get_ch_score_dump = "<%tcWebApi_MeshJsonHook("mesh_get_ch_score_dump")%>";
																			mesh_get_ch_score_dump = mesh_get_ch_score_dump.replace(/\'/g,'\\\\u0022');
																			mesh_get_ch_score_dump = mesh_get_ch_score_dump.replace(/\\\\u0022Pass-phrase\\\\u0022:/g,'\\\\u0022WPAPSK\\\\u0022:');																		
																	}																	
																	function get_run_time_topology(value)
																	{
																		document.getElementById("Mesh_scan_info_div").style.display = "none";																		
																		if (value == "1")
																		{
																			var x_response = mesh_get_ch_scan_dump; //JSO_get_ch_scan_dump();
																			mesh_scan_or_score_flag="1";																			
																		}
																		else if  (value == "2")
																		{
																			var x_response = mesh_get_ch_score_dump;//JSO_get_ch_score_dump();
																			mesh_scan_or_score_flag="2";
																		}		
																		get_run_time_topology_cb(x_response);																		
																	}
																	function get_run_time_topology_cb(rsp)
																	{
																		try
																		{
																			var r = JSON.parse(rsp);
																			if(r.status == "SUCCESS")
																			{	
																				if(mesh_scan_or_score_flag == "2"){
																					var tmp_jsTopoInfo = r.channelScoreResult;
																				}
																				else{
																					var tmp_jsTopoInfo = r.channelScanResult;
																				}	
																				tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\u0022/g, '\"');
																				tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\\\u0022/g, '\"');
																				var jsTopoInfo = JSON.parse(tmp_jsTopoInfo);
																				document.getElementById("Mesh_scan_info_div").style.display = "";	
																				document.getElementById("Mesh_scan_info_div").innerHTML = "";
																				draw_topology_table(jsTopoInfo);
																			}
																			else
																			{
																				console.log("Failed to get Topology Info!\nStatus: ",r.status);
																			}
																			//setTimeout(get_run_time_topology_submit, 15000);
																		}
																		catch(e)
																		{
																			console.log("Incorrect response! Failed to get Topology Info!",e.name,e.message);
																			setText('Action_Flag', 1);
																			formSubmit(1);
																			//setTimeout(get_run_time_topology_submit, 15000);
																		}
																	}
																	function draw_topology_table(jsTopoInfo)
																	{
																		var tree_info = [];
																		var tree_info_count = 0;
																		var topoInfoArr = "";
																		if (mesh_scan_or_score_flag == '1')
																		{
																			topoInfoArr = jsTopoInfo['Channel Scan Information'];	
																		}
																		else if ( mesh_scan_or_score_flag == '2' )
																		{
																			topoInfoArr = jsTopoInfo['Channel Planning Score Information'];	
																		}
																		else 
																		{
																			topoInfoArr = "";
																		}
																		
																		if(!(topoInfoArr instanceof Array))
																		{
																			console.log("Incorrect TopologyInfo: Value of topology information is not an Array!");
																			return;
																		}
																		
																		if (mesh_scan_or_score_flag == '1'){
																			for(var idx_1905=0; idx_1905 < topoInfoArr.length; idx_1905++)
																			{
																				var obj_1905 = topoInfoArr[idx_1905];
																				if(!obj_1905.hasOwnProperty('AL MAC') || ("00:00:00:00:00:00" == obj_1905['AL MAC']))
																				{
																						continue;
																				}
																				obj_1905 = prep_bssinfo(obj_1905);
																				
																				var dev1905tbl = document.createElement("TABLE");
																				dev1905tbl.setAttribute("class", "cbi-section-table");
																				var dev1905fset = document.createElement("div");
																				dev1905fset.setAttribute("class", "table1_head");
																				var dev1905legend = document.createElement("p");
																				var dev1905a = document.createElement("A");
																				var dev1905text = document.createTextNode("1905 Device - " + (idx_1905 + 1));
																				dev1905a.setAttribute("href", "#");
																				dev1905a.appendChild(dev1905text);
																				dev1905legend.appendChild(dev1905a);
																				dev1905fset.appendChild(dev1905legend);
																				document.getElementById("Mesh_scan_info_div").appendChild(dev1905fset);
																				
																				set_max_cellspan(obj_1905);
																				obj_loop(dev1905tbl, null, obj_1905);																				
																				document.getElementById("Mesh_scan_info_div").appendChild(dev1905tbl);
																				tree_info_count++;
																			}
																		}	
																		else if (mesh_scan_or_score_flag == '2'){
																			for(var idx_1905=0; idx_1905 < topoInfoArr.length; idx_1905++)
																			{
																				var obj_1905 = topoInfoArr[idx_1905];
																				obj_1905 = prep_bssinfo(obj_1905);
																				
																				var dev1905tbl = document.createElement("TABLE");
																				dev1905tbl.setAttribute("class", "cbi-section-table");
																				var dev1905fset = document.createElement("div");
																				dev1905fset.setAttribute("class", "table1_head");
																				var dev1905legend = document.createElement("p");
																				var dev1905a = document.createElement("A");
																				var dev1905text = document.createTextNode("1905 Device - " + (idx_1905 + 1));
																				dev1905a.setAttribute("href", "#");
																				dev1905a.appendChild(dev1905text);
																				dev1905legend.appendChild(dev1905a);
																				dev1905fset.appendChild(dev1905legend);
																				document.getElementById("Mesh_scan_info_div").appendChild(dev1905fset);																				
																				set_max_cellspan(obj_1905);
																				obj_loop(dev1905tbl, null, obj_1905);																				
																				document.getElementById("Mesh_scan_info_div").appendChild(dev1905tbl);
																			}
																		}
																		else
																		{
																		}																		
																	}
																	function prep_bssinfo(dev1905Obj)
																	{
																		var tag_dev1905Obj = dev1905Obj;
																		if (mesh_scan_or_score_flag == '1'){
																			for(var radIdx=0; radIdx < tag_dev1905Obj['Radio Info'].length; radIdx++)
																			{
																				for(var bssIdx=0; bssIdx < tag_dev1905Obj['Radio Info'][radIdx]['Channel scan result'].length; bssIdx++)
																				{
																				}
																			}
																		}
																		else if ( mesh_scan_or_score_flag == '2')
																		{
																			/*
																			for(var radIdx=0; radIdx < tag_dev1905Obj['Radio Info'].length; radIdx++)
																			{
																				for(var bssIdx=0; bssIdx < tag_dev1905Obj['Radio Info'][radIdx].length; bssIdx++)
																				{
																				}
																			}
																			*/
																		}
																		
																		return tag_dev1905Obj;
																	}
																	function set_max_cellspan(o)
																	{
																		for(var k in o)
																		{
																			var v = o[k];
																			if (v instanceof Array)
																			{
																				max_cellspan++;
																				for(var a_idx=0; a_idx < v.length; a_idx++)
																				{
																					if(((typeof v[a_idx]) == "object") && v[a_idx] != null)
																					{
																						set_max_cellspan(v[a_idx]);
																					}
																					else
																					{
																						console.log("set_max_cellspan: Incorrect Topology: Array element is not an object!",v[a_idx]);
																					}
																				}
																			}
																			else if(((typeof v) == "object") && v != null)
																			{
																				set_max_cellspan(v);
																			}
																		}
																	}
																	
																	function set_rowspan(o)
																	{
																		var rowspan = 0;
																		for(var k in o)
																		{
																			var v = o[k];
																			if (v instanceof Array)
																			{
																				for(var a_idx=0; a_idx < v.length; a_idx++)
																				{
																					if(((typeof v[a_idx]) == "object") && v[a_idx] != null)
																					{
																						rowspan = rowspan + set_rowspan(v[a_idx]);
																					}
																					else
																					{
																						console.log("set_rowspan: Incorrect Topology: Array element is not an object!",v[a_idx]);
																					}
																				}
																			}
																			else if(((typeof v) == "object") && v != null)
																			{
																				rowspan = rowspan + set_rowspan(v);
																			}
																			else
																			{
																				rowspan++;
																			}
																		}
																		return rowspan;
																	}
																	
																	function obj_loop(table_id, row, o)
																	{
																		var cell;
																		for(var k in o)
																		{
																			var v = o[k];
																			if (v instanceof Array)
																			{
																				arr_loop(table_id, k, v);
																			}
																			else if(((typeof v) == "object") && v != null)
																			{
																				obj_loop(table_id, row, v);
																			}
																			else
																			{
																				if(row == null)
																				{
																					row = table_id.insertRow(-1);
																				}
																				cell = row.insertCell(-1);
																				cell.innerHTML = k;
																				cell = row.insertCell(-1);
																				cell.colSpan = max_cellspan - row.cells.length + 1;
																				cell.innerHTML = v;
																				row = null;
																			}
																		}
																	}
																	
																	function arr_loop(table_id, k, a)
																	{
																		var row, cell;
																		for(var a_idx=0; a_idx < a.length; a_idx++)
																		{
																			var v = a[a_idx];
																			if(((typeof v) == "object") && v != null)
																			{
																				row = table_id.insertRow(-1);
																				cell = row.insertCell(-1);
																				cell.rowSpan = set_rowspan(v);
																				cell.style.verticalAlign  = "middle";
																				cell.innerHTML = "<strong>" + k + ' - ' + (a_idx + 1) + "</strong>";
																				obj_loop(table_id, row, v);
																			}
																			else
																			{
																				console.log("Incorrect Topology: Array element is not an object!",a);
																			}
																		}
																	}
																	function prep_tree_info(dev1905Obj)
																	{
																		var node = {};
																		node["alMac"] = dev1905Obj['AL MAC'];
																		node["hopCount"] = parseInt(dev1905Obj['number of radios']);
																		node["staInfo"] = {};
																	
																		for(var radIdx=0; radIdx < dev1905Obj['Radio Info'].length; radIdx++)
																		{
																			for(var bssIdx=0; bssIdx < dev1905Obj['Radio Info'][radIdx]['Channel scan result'].length; bssIdx++)
																			{
																				if(dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx].hasOwnProperty('Channel Number'))
																				{
																					node["staInfo"][bssIdx]['Channel Number'].push(tag_dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx]['Channel Number']);
																				}
																				if(dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx].hasOwnProperty('Channel Util'))
																				{
																					node["staInfo"][bssIdx]['Channel Util'].push(tag_dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx]['Channel Util']);
																				}
																				if(dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx].hasOwnProperty('Channel Neighbour'))
																				{
																					node["staInfo"][bssIdx]['Channel Neighbour'].push(tag_dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx]['Channel Neighbour']);
																				}
																				if(dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx].hasOwnProperty('CHannel Condition'))
																				{
																					node["staInfo"][bssIdx]['CHannel Condition'].push(tag_dev1905Obj['Radio Info'][radIdx]['Channel scan result'][bssIdx]['CHannel Condition']);
																				}
																			}
																		}
																		return node;
																	}
																	//mac_list
																	function JSO_get_runtime_topology()
																	{
																			var ret = "<% tcWebApi_MeshJsonHook("mesh_get_run_time_topology") %>";
																			ret = ret.replace(/\'/g,'\\\\u0022');
																			ret = ret.replace(/\\\\u0022Pass-phrase\\\\u0022:/g,'\\\\u0022WPAPSK\\\\u0022:');
																			get_mac_list_cb(ret);
																	}
																	function get_mac_list_cb(rsp)
																	{
																		try
																		{
																			var r = JSON.parse(rsp);
																			if(r.status == "SUCCESS")
																			{
																					var tmp_jsTopoInfo = r.luaTopologyInfo;
																					tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\u0022/g, '\"');
																					tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\\\u0022/g, '\"');
																				var jsTopoInfo = JSON.parse(tmp_jsTopoInfo);																			
																				draw_mac_list_table(jsTopoInfo);
																			}
																			else
																			{
																				console.log("Failed to get Topology Info!\nStatus: ",r.status);
																			}
																		}
																		catch(e)
																		{
																			console.log("Incorrect response! Failed to get Topology Info!",e.name,e.message);
																		}
																	}
																	 function draw_mac_list_table(jsTopoInfo)
																	{
																		var mac_List_info = [];
																		var tree_info_count = 0;
																		var topoInfoArr = jsTopoInfo['topology information'];
																		if(!(topoInfoArr instanceof Array))
																		{
																			console.log("Incorrect TopologyInfo: Value of topology information is not an Array!");
																			return;
																		}
																		
																		for(var idx_1905=0; idx_1905 < topoInfoArr.length; idx_1905++)
																		{
																			var obj_1905 = topoInfoArr[idx_1905];
																			if(!obj_1905.hasOwnProperty('AL MAC') || ("00:00:00:00:00:00" == obj_1905['AL MAC']))
																			{
																					continue;
																			}
																			mac_List_info.push(topoInfoArr[idx_1905]['AL MAC']);
																			tree_info_count++;
																		}
																		add_mac_list_select(mac_List_info,tree_info_count)																		
																	}
																	//add_mac list select
																	function add_mac_list_select(mac_List_info, tree_info_count)
																	{	
																		var selid =document.getElementById("add_Mac_select");
																		for(var i =0; i < tree_info_count; i++){
																			selid.options[i] = new Option(mac_List_info[i],mac_List_info[i]);
																		}																		
																	}
																	
																	
																	function autoClickA(){																	
																		Ink = document.getElementById("auto");
																		Ink.click();
																	}																
																	
																	JSO_get_ch_scan_dump();
																	JSO_get_ch_score_dump();
																	JSO_get_runtime_topology();
																	clickButtonDisplay();
																	<% end if %>
																	function clickButton(){			
																		<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
																			document.getElementById("Mesh_scan_info_div").style.display = "none";	
																		<% end if %>
																	}		
																	function clickButtonDisplay(){																	
																		<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
																			<% if tcWebApi_get("Mesh_action", "R2_ch_scan_status", "h") = "0" then %>
																				<% if tcWebApi_get("Mesh_action", "R1_ch_scan_status", "h") = "0" then %>
																					$("#trigger_channel_planning_r2_button").attr('disabled',false);
																					$("#trigger_channel_planning_r2_mac_button").attr('disabled',false);
																				<% end if %>
																			<% else %>
																				$("#trigger_channel_planning_r2_button").attr('disabled',true);
																				$("#trigger_channel_planning_r2_mac_button").attr('disabled',true);		
																			<% end if %>	
																			setTimeout(get_multi_ap_action_submit, 30000);
																		<% end if %>
																	}
																	function get_multi_ap_action_submit()
																	{
																		location.href = '/cgi-bin/wifi_multi_ap_action.asp';
																	}
							                </script>
							      </div><!-- isMapCfgSupportedYes -->
							      <% end if %>
									</div><!-- isMapSupportedYes -->
									<% end if %>
								</div><!-- l1datYes -->
								
							<div id="Mesh_scan_info_div" style="display:none"></div>
							
						</div>
					</TD>
			    <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
			    	<TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
			    		<TBODY>
			     		<TR>
				    		<TD valign='top'>
			    				<A href="/cgi-bin/help_content.asp" target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 border=0></A>
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
					<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
					<TD width=7 background=/img/panel3.gif>��</TD>
					<TD></TD>
				</TR>
				<TR>
					<TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
						<P class=Item_L2></P>
					</TD>
					<TD width=7 background=/img/panel3.gif>��</TD>
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
					<TD width=162 bgColor=#ef8218>��</TD>
					<TD width=278 bgColor=#427594>��</TD>
					<TD width=196 bgColor=#427594>
						<P align=center id="cbi-btnsave_cancel-pel" style="display: none">
							<IMG id="btnOK" onclick="btnSave()" height=23 src="/img/ok.gif" width=80 border=0>&nbsp;&nbsp;
							<IMG id="btnCancel" height=23 src="/img/cancel.gif" onclick="RefreshPage()" width=80 border=0>
						</P>
					</TD>
					<TD width=170 bgColor=#313031>��</TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
	</TR>
	</TBODY>
</TABLE>
<SCRIPT language=JavaScript type=text/javascript>
function formSubmit(saveAll)
{
		if(document.cbi != null)
		{
				if(saveAll)
				{
						setText('SaveAll_Flag', 1);
				}
				else
				{
						setText('SaveAll_Flag', 0);
				}
				setText('Save_Flag', 1);
				if( true == setEBooValueCookie(document.cbi) )
				{
					document.cbi.submit();
				}
				else
				{
					setText('Save_Flag', 0);
				}
		}
}

function btnSave()
{
		if(document.cbi != null)
		{
				formSubmit(1);
		}
}

function LoadFrame ()
{
		var isl1datSupported = 1;

<% if tcWebApi_get("globalstate_common", "wlanLoad", "h") = "0" then %>
		isl1datSupported = 0;
<% end if %>

		if(isl1datSupported)
		{
				setDisplay('l1datNot',0);
				setDisplay('l1datYes',1);

<% if tcWebApi_get("WLan_Common", "MultiSSIDConfigEnable", "h") = "Yes" then if tcWebApi_get("Mesh_dat", "MapEnable", "h") <> "N/A" then %>
				LoadMapFrame();
<% end if end if %>
		}
		else
		{
				setDisplay('l1datNot',1);
				setDisplay('l1datYes',0);
		}
}
</SCRIPT>
</BODY>
</HTML>
