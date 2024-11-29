<%
if Request_Form("Save_Flag") = "1" then
	if Request_Form("SaveAll_Flag") = "1" then

		if Request_Form("MapEnable") = "1" then
			TCWebApi_constSet("Mesh_dat", "MapEnable","1")
			TCWebApi_Set("Mesh_common", "SteerEnable", "SteerEnable")
			TCWebApi_Set("Mesh_common", "DeviceRole", "DeviceRole")

			if Request_Form("DeviceRole") = "0" then
				TCWebApi_constSet("Mesh_mapdcfg", "DhcpCtl", "1")
				TCWebApi_Set("Mesh_mapdcfg", "BhPriority2G", "BhPriority2G")
				TCWebApi_Set("Mesh_mapdcfg", "BhPriority5GL", "BhPriority5GL")
				TCWebApi_Set("Mesh_mapdcfg", "BhPriority5GH", "BhPriority5GL")
				if tcwebApi_get("WebCustom_Entry", "isUBUSSupported", "h") = "Yes" then
					if tcwebApi_get("Wan_Common", "WorkMode", "h") = "repeater" then
						TCWebApi_constSet("Wan_Common", "WorkMode", "router")
						TCWebApi_commit("Wan_common")
					end if
				end if
			elseif Request_Form("DeviceRole") = "2" then
				TCWebApi_constSet("Mesh_mapdcfg", "DhcpCtl", "1")
				if tcwebApi_get("WebCustom_Entry", "isUBUSSupported", "h") = "Yes" then
					if tcwebApi_get("Wan_Common", "WorkMode", "h") <> "bridge" then
						TCWebApi_constSet("Wan_Common", "WorkMode", "bridge")
						TCWebApi_commit("Wan_common")
					end if
				end if
			else
				TCWebApi_constSet("Mesh_mapdcfg", "DhcpCtl", "0")
				if tcwebApi_get("WebCustom_Entry", "isUBUSSupported", "h") = "Yes" then
					if tcwebApi_get("Wan_Common", "WorkMode", "h") = "repeater" then
						TCWebApi_constSet("Wan_Common", "WorkMode", "router")
						TCWebApi_commit("Wan_common")
					end if
				end if
			end if
			if Request_Form("Change_Flag") = "1" then
				TCWebApi_constSet("Mesh_common", "ReinitWifiFlag","1")
	
				TCWebApi_commit("Mesh_dat")
				if Request_Form("MeshTriggerFlag") = "1" then
					TCWebApi_commit("Mesh_common")
				end if
			end if
			
		else
			TCWebApi_constSet("Mesh_dat", "MapEnable","0")
			TCWebApi_commit("Mesh_dat")
		end if

	else
		if Request_Form("Action_Flag") = "1" then

			if Request_Form("resetToDefaultEasyMesh_Flag") = "1" then
				TCWebApi_constSet("Mesh_action", "load_default_setting", "1")
				TCWebApi_commit("Mesh_action")
			end if
			
			if Request_Form("MapEnable") = "1" then
				if Request_Form("wifi_trigger_onboarding_Flag") = "1" then
					if Request_Form("ether_trigger_onboarding_Flag") = "0" then	
						TCWebApi_constSet("Mesh_action", "wifi_trigger_onboarding", "1")
						TCWebApi_constSet("Mesh_action", "ether_trigger_onboarding", "0")
						TCWebApi_commit("Mesh_action")
					end if
				end if
				if Request_Form("wifi_trigger_onboarding_Flag") = "0" then
					if Request_Form("ether_trigger_onboarding_Flag") = "1" then	
						TCWebApi_constSet("Mesh_action", "wifi_trigger_onboarding", "0")
						TCWebApi_constSet("Mesh_action", "ether_trigger_onboarding", "1")
						TCWebApi_commit("Mesh_action")
					end if
				end if
				
			end if

		end if
	end if

	TCWebApi_save()
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
							<li class="cbi-tab" id="map-cfg-tab-basic">
								<a href="wifi_multi_ap_basic.asp"><%tcWebApi_get("String_Entry","EasyMeshBasicTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-adv" style="display:none">
								<a href="wifi_multi_ap_advanced.asp"><%tcWebApi_get("String_Entry","EasyMeshAdvancedTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-action" style="display:none">
								<a href="wifi_multi_ap_action.asp"><%tcWebApi_get("String_Entry","EasyMeshActionTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-status" style="display:none">
								<a href="wifi_multi_ap_status.asp"><%tcWebApi_get("String_Entry","EasyMeshStatusTest","s")%></a>
							</li>
						</ul>
					</TD>
					<TD vAlign=top width=7 background=/img/panel3.gif>
						<ul class="cbi-tabmenu">
							<li class="cbi-tab" id="map-cfg-tab-basic-pel">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-adv-pel" style="display:none">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-action-pel" style="display:none">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-status-pel" style="display:none">&nbsp;</li>
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
							                <div id="map_validate_controller_settings_error_div" style="display: none">
							                	<div id="map_validate_controller_rssith_error_msg" style="display: none" class="alert-message error"><big><strong>RSSI Threshold is incorrect!</big></strong></div>
							                	<div id="map_validate_controller_channelth_error_msg" style="display: none" class="alert-message error"><big><strong>2G/5G Channel Utilization Threshold is incorrect!</big></strong></div>
							                	<div id="map_validate_controller_error_msg" class="alert-message error"><big><strong>Please enter an integer number !</big></strong></div>
							                </div>
							                <div id="MULTI_AP_SETTINGS">
							                    <input type="hidden" name="__activeTab" id="__activeTab" value="basic">
							                    <form method="post" name="cbi" action="/cgi-bin/wifi_multi_ap_basic.asp" onsubmit="return validate_all()" autocomplete="off">
							                    		<input type="hidden" name="SaveAll_Flag" value="0" />
							                    		<input type="hidden" name="Save_Flag" value="0" />
							                    		<input type="hidden" name="Action_Flag" value="0" />
							                    		<input type="hidden" name="resetToDefaultEasyMesh_Flag" value="0" />
							                    		<input type="hidden" name="wifi_trigger_onboarding_Flag" value="0" />
							                    		<input type="hidden" name="ether_trigger_onboarding_Flag" value="0" />
														<input type="hidden" name="MeshTriggerFlag" value="<% tcWebApi_get("Mesh_Common","MeshTriggerFlag","s") %>" />
														<input type="hidden" name="Change_Flag" value="0" />

							                    	<table class="cbi-section-table" id="map-cfg-basic">
							                    	<TBODY>
							                    		<tbody id="map-cfg-basic-meshenable-setting">
															<tr>
																<td><%tcWebApi_get("String_Entry","EasyMeshBasic1Test","s")%></td>
																<td colspan="2">
																	<input type="radio" style="width: auto" name="MapEnable" id="MapEnableOn" value="1" onchange="MapEnableClick(1)" onClick="this.blur();" <% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %> checked="checked"<% end if %> /> <%tcWebApi_get("String_Entry","EasyMeshBasic2Test","s")%>
																	<input type="radio" style="width: auto" name="MapEnable" id="MapEnableOff" value="0" onchange="MapEnableClick(0)" onClick="this.blur();" <% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "0" then %> checked="checked"<% end if %> /> <%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>
																</td>
															</tr>															
							                    		</tbody>

							                    	<div id="appliedMapEnableDiffNA_MapEnableYes_basic">
							                    		<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
							                    		<tbody id="map-cfg-basic-devicerole-setting">
															<% if tcWebApi_get("WebCustom_Entry", "isEasyMeshR2Supported", "h") = "Yes" then %>
															<tr>
																<td><%tcWebApi_get("String_Entry","EasyMeshBasic20Test","s")%></td>
																<td colspan="2">
																	<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
																	<span> <strong> <%tcWebApi_get("String_Entry","EasyMeshBasic22Test","s")%> </strong></span>
																	<% elseif tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R1" then %>	
																	<span> <strong> <%tcWebApi_get("String_Entry","EasyMeshBasic21Test","s")%> </strong></span>
																	<% else %>
																	<span> <strong> <%tcWebApi_get("String_Entry","EasyMeshBasic21Test","s")%> </strong></span>
																	<% end if%>
																</td>
															</tr>
															<% end if %>
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshBasic11Test","s")%></td>
							                                  <td>
							                                      <select name="DeviceRole" id="DeviceRole" size="1" onchange="deviceRoleOnChange(this.value)">
							                                          <option value="1">Controller</option>
							                                          <option value="2">Agent</option>
							                                          <option value="0">Auto</option>
							                                      </select>
							                                  </td>
							                                  <td></td>
							                              </tr>
							                            </tbody>
							                            <tbody id="map-cfg-basic-detail-setting">
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshBasic4Test","s")%></td>
							                                  <td>
							                                     <span><strong id="current-dev-role">Not configured</strong></span>
							                                  </td>
							                                  <td></td>
							                              </tr>
							                              <tbody id="map-cfg-back-haul-connection-status-settings" style="display:none">
							                              <tr>
							                              		<td><%tcWebApi_get("String_Entry","EasyMeshBasic12Test","s")%></td>
							                              		<td>
							                              				<span><strong id="back-haul-connection-status"><%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%></strong></span>
							                              		</td>
							                              		<td></td>
							                              </tr>
							                              </tbody>
								                          	<tbody id="map-cfg-basic-dev-role-configured-settings" style="display:none">
								                              <tr>
								                              	  <td><%tcWebApi_get("String_Entry","EasyMeshBasic10Test","s")%></td>
								                                  <td colspan="2">
							                		          		        <input type="radio" style="width: auto" name="SteerEnable" id="SteerEnable" value="1" <% if tcWebApi_get("Mesh_common", "SteerEnable", "h") = "1" then %> checked="checked"<% end if %> /> <%tcWebApi_get("String_Entry","EasyMeshBasic2Test","s")%>
							                		          		        <input type="radio" style="width: auto" name="SteerEnable" id="SteerEnable" value="0" <% if tcWebApi_get("Mesh_common", "SteerEnable", "h") <> "1" then %> checked="checked"<% end if %> /> <%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>
								                                  </td>
								                              </tr>
								                          </tbody>							                              
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshBasic5Test","s")%></td>
							                                  <td colspan="2">
							                                      <input type="button" value="<%tcWebApi_get("String_Entry","EasyMeshBasic6Test","s")%>" id="resetMap" onClick="resetToDefaultEasyMesh()" />
							                                  </td>
							                              </tr>
							                              <tbody id="map-cfg-basic-ap-onboarding">
							                              <tr>
							                                  <td rowspan="2" style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshBasic7Test","s")%></td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="multi-ap-on-board-btn" value="<%tcWebApi_get("String_Entry","EasyMeshBasic8Test","s")%>" onClick="triggerMultiApOnBoarding()" />
							                                      <span><%tcWebApi_get("String_Entry","EasyMeshBasic9Test","s")%></span>
							                                  </td>
							                              </tr>
							                              </tbody>
							                              <tr id="current-dev-role-during-not-configured-msg">
							                              	  <td class="alert-message" colspan="3">
							                                     <strong>
							                                          <%tcWebApi_get("String_Entry","EasyMeshBasic17Test","s")%>
							                                     </strong>
							                                 </td>
							                              </tr>
							                            </tbody>
							                          </TBODY>
							                        <% end if %>
							                        </div><!-- appliedMapEnableDiffNA_MapEnableYes_basic -->
							                        </table>
							                    </form>
							                </div>
							
							                <SCRIPT language=JavaScript type=text/javascript>
																	
																	var webReloadFlag = 0;
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																	var get_device_role_timer_id;
																	<% end if %>
																	
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
																			var webstate = "<% tcWebApi_get("Mesh_common","ReinitWifiFlag","s") %>";
																			
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
																	
																	function JSO_get_al_mac()
																	{
																		var ret = '';
																		var al_mac = '';
																		
																		al_mac = "<% tcWebApi_get("Mesh_mapcfg","AL-MAC","s") %>";
																		<% if al_mac <> "N/A" then %>
																		ret = ' { ';
																		ret = ret + '"status": "SUCCESS",';
																		ret = ret + '"al_mac": "' + al_mac + '"';
																		ret = ret + ' } ';
																		<% end if %>
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
																	
																	function JSO_get_runtime_topology()
																	{
																		var ret = "<% tcWebApi_MeshJsonHook("mesh_get_run_time_topology") %>";
																		ret = ret.replace(/\'/g,'\\\\u0022');
																		ret = ret.replace(/\\\\u0022Pass-phrase\\\\u0022:/g,'\\\\u0022WPAPSK\\\\u0022:');
																		return ret;
																	}
																	function JSO_get_sta_bh_interface()
																	{
																		<% if tcWebApi_get("Mesh_Common","DeviceRole","h") = "2"  then %>
																		var ret = "<% tcWebApi_MeshJsonHook("mesh_get_sta_bh_interface") %>";
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
																							case "get_al_mac":
																								X_Res = JSO_get_al_mac();
																								break;
																							case "get_runtime_topology":
																								X_Res = JSO_get_runtime_topology();
																								break;
																							case "get_sta_bh_interface":
																								X_Res = JSO_get_sta_bh_interface();
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
																	
																	function get_sta_bh_interface()
																	{
																		var devRole =  "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																		if((devRole == "Agent") || ("2" == devRole))
																		{
																			var XHR_type = 'get_sta_bh_interface';
																		    var x_response = getX_Response(XHR_type);
																			{
																				try
																				{
																					var rsp = JSON.parse(x_response);
																					if(rsp.status == "SUCCESS")
																					{
																						var sta_bh_inf_arr = rsp.staBhInfStr.split(";");
																						for(var idx=0; idx < sta_bh_inf_arr.length; idx++)
																						{
																							if(sta_bh_inf_arr[idx] == "")
																							{
																								continue;
																							}
																							console.log("conn interface", sta_bh_inf_arr[idx]);
																							return sta_bh_inf_arr[idx];
																						}
																					}
																					else
																					{
																						console.log("Failed to get STA BH Interface!\nStatus: ",rsp.status);
																					}
																				}
																				catch(e)
																				{
																					console.log("Incorrect response!\nFailed to get STA BH Interface!");
																				}
																			}
																		}
																		else
																		{
																			staBhInfUl.parentNode.parentNode.style.display = "none";
																		}
																	}
																	
																	function CheckWifi()
																	{
																			var isWifiSupport = "<% tcWebApi_get("Mesh_common","isMeshEncrypTypeAuthModeSupport","s") %>";
																			if(isWifiSupport == "No")
																			{
																					alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert13Test","s")%>");
																					return 1;
																			}
																			var isWifi2gEnable = "<% tcWebApi_get("Wlan_Common","APOn","s") %>";
																			var isWifi5gEnable = "<% tcWebApi_get("Wlan11ac_Common","APOn","s") %>";
																			if(0 == isWifi2gEnable || 0 == isWifi5gEnable)
																			{
																					alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert14Test","s")%>");
																					return 1;
																			}
																	}
																	
																	function MapEnableClick(_val)
																	{
																			if(_val)
																			{
																					if ( 1 == CheckWifi() )
																					{
																						var mapEnableRadio1 = document.getElementById("MapEnableOn");
																						var mapEnableRadio2 = document.getElementById("MapEnableOff");
																						mapEnableRadio1.checked = false;
																						mapEnableRadio2.checked = true;
																						return;
																					}
																					document.getElementById("map-cfg-basic-detail-setting").style.display = "";
																					document.getElementById("map-cfg-basic-ap-onboarding").style.display = "";
																					document.getElementById("map-cfg-tab-adv").style.display = "";
																					document.getElementById("map-cfg-tab-adv-pel").style.display = "";
																					document.getElementById("map-cfg-tab-action").style.display = "";
																					document.getElementById("map-cfg-tab-action-pel").style.display = "";
																					document.getElementById("map-cfg-tab-status").style.display = "";
																					document.getElementById("map-cfg-tab-status-pel").style.display = "";
																					
																					var devRole = document.getElementById("current-dev-role").innerHTML;
																					switch(devRole)
																	      	{
																	        		case "Not Configured": // Auto
																	        		    document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "";
																							document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "";
																							document.getElementById("map-cfg-tab-adv").style.display = "none";
																							document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																	        				break;
																	        		case "Controller": // Controller
																	        		    document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "";
																							document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "none";
																	        				break;
																	        		case "Agent": // Agent
																	        		    document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "";
																							document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "";
																							document.getElementById("map-cfg-tab-adv").style.display = "none";
																							document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																	        				break;
																	        		default:
																	        				break;
																	      	}
																			}
																			else
																			{
																					document.getElementById("map-cfg-basic-detail-setting").style.display = "";
																					document.getElementById("map-cfg-basic-ap-onboarding").style.display = "none";
																					document.getElementById("map-cfg-tab-adv").style.display = "none";
																					document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																					document.getElementById("map-cfg-tab-action").style.display = "none";
																					document.getElementById("map-cfg-tab-action-pel").style.display = "none";
																					document.getElementById("map-cfg-tab-status").style.display = "none";
																					document.getElementById("map-cfg-tab-status-pel").style.display = "none";
																					
																					document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "none";
																					document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "none";
																			}
																	}
																	
																	function validate_all()
																	{
																	    var devRole = document.getElementById("DeviceRole");
																	    var mapEnableRadio = document.getElementById("MapEnableOn");
																	    if(devRole.type == "radio" && (!(mapEnableRadio.checked)))
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert1Test","s")%>");
																	        return false;
																	    }
																	    
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																			if(devRole.type != "select-one")
																			{
																			    alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert2Test","s")%>");
																			    return false;
																			}
																			var curDevRole = document.getElementById("current-dev-role").innerHTML;
																			if (devRole.selectedIndex == 0)
																			{
																			    if (curDevRole == "Not configured")
																			    {
																			        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert3Test","s")%>\n" +
																			              "<%tcWebApi_get("String_Entry","EasyMeshBasicAlert4Test","s")%>\n" +
																			              "<%tcWebApi_get("String_Entry","EasyMeshBasicAlert5Test","s")%>");
																			        return false;
																			    }
																			    if(curDevRole == "Controller")
																			    {
																			        devRole.selectedIndex = 0;
																			    }
																			    else if(curDevRole == "Agent")
																			    {
																			        devRole.selectedIndex = 1;
																			    }
																			    else
																			    {
																			        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert6Test","s")%>\n");
																			        return false;
																			    }
																			}
																			if(curDevRole == "Controller")
																			{
																			    if(!validateControllerSettings())
																			    {
																			        return false;
																			    }
																			}
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */

																		document.getElementById("LOADING_MULTI_AP").style.display = "";
																	    document.getElementById("MULTI_AP_SETTINGS").style.display = "none";
																	    return true;
																	}
																	
																	function resetToDefaultEasyMesh()
																	{
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																			clearAllTimers();
																	<% end if %>
																			setText('resetToDefaultEasyMesh_Flag', 1);
																			setText('Action_Flag', 1);							                        
																	    	formSubmit(0);
																	}
																	
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																	function validateControllerSettings()
																	{
																	    if (!validateApSteerRssiTh() ||
																	        !validateChannelUtilizationTh())
																	    {
																	        return false;
																	    }
																	    return true;
																	}
																	
																	function validateApSteerRssiTh()
																	{
																	    var apSteerRssiTh = "<% tcWebApi_get("Mesh_mapdcfg","APSteerRssiTh","s") %>";
																	    if (apSteerRssiTh == "" ||
																	        isNaN(apSteerRssiTh) ||
																	        !/^(0|-[1-9]|-[1-9][0-9])$/.test(apSteerRssiTh) ||
																	        parseInt(apSteerRssiTh) > 0 ||
																	        parseInt(apSteerRssiTh) < -90)
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert7Test","s")%>\n<%tcWebApi_get("String_Entry","EasyMeshBasicAlert8Test","s")%>");
																	    }
																	    return true;
																	}
																	
																	function validateChannelUtilizationTh()
																	{
																	    var channelUtilTh2G = "<% tcWebApi_get("Mesh_steercfg","CUOverloadTh_2G","s") %>";
																	    var channelUtilTh5GL = "<% tcWebApi_get("Mesh_steercfg","CUOverloadTh_5G_L","s") %>";
																	    var channelUtilTh5GH = channelUtilTh5GL;
																	    
																	    if (channelUtilTh2G == "" ||
																	        isNaN(channelUtilTh2G) ||
																	        !/^(0|[1-9]|[1-9][0-9]|100)$/.test(channelUtilTh2G) ||
																	        parseInt(channelUtilTh2G) < 0 ||
																	        parseInt(channelUtilTh2G) > 100)
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert9Test","s")%>\n<%tcWebApi_get("String_Entry","EasyMeshBasicAlert11Test","s")%>");
																	    }
																	    if (channelUtilTh5GL == "" ||
																	        isNaN(channelUtilTh5GL) ||
																	        !/^(0|[1-9]|[1-9][0-9]|100)$/.test(channelUtilTh5GL) ||
																	        parseInt(channelUtilTh5GL) < 0 ||
																	        parseInt(channelUtilTh5GL) > 100)
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert10Test","s")%>\n<%tcWebApi_get("String_Entry","EasyMeshBasicAlert11Test","s")%>");
																	    }
																	    return true;
																	}

																	function set_controller_validate_msg()
																	{
																		var apSteerRssiTh = "<% tcWebApi_get("Mesh_mapdcfg","APSteerRssiTh","s") %>";
																		var channelUtilTh2G = "<% tcWebApi_get("Mesh_steercfg","CUOverloadTh_2G","s") %>";
																		var channelUtilTh5GL = "<% tcWebApi_get("Mesh_steercfg","CUOverloadTh_5G_L","s") %>";
																		var setting_error_msg_div = document.getElementById("map_validate_controller_settings_error_div");
																		var rssi_error_msg_div = document.getElementById("map_validate_controller_rssith_error_msg");
																		var radio_error_msg_div = document.getElementById("map_validate_controller_channelth_error_msg");

																		setting_error_msg_div.style.display = "none";
																		rssi_error_msg_div.style.display = "none";
																		radio_error_msg_div.style.display = "none";

																		if (apSteerRssiTh == "" ||
																	        isNaN(apSteerRssiTh) ||
																	        !/^(0|-[1-9]|-[1-9][0-9])$/.test(apSteerRssiTh) ||
																	        parseInt(apSteerRssiTh) > 0 ||
																	        parseInt(apSteerRssiTh) < -90)
																	    {
																			setting_error_msg_div.style.display = "";
																			rssi_error_msg_div.style.display = "";
																	    }
																		if (channelUtilTh2G == "" ||
																	        isNaN(channelUtilTh2G) ||
																	        !/^(0|[1-9]|[1-9][0-9]|100)$/.test(channelUtilTh2G) ||
																	        parseInt(channelUtilTh2G) < 0 ||
																	        parseInt(channelUtilTh2G) > 100)
																	    {
																			setting_error_msg_div.style.display = "";
																			radio_error_msg_div.style.display = "";
																	    }
																	    if (channelUtilTh5GL == "" ||
																	        isNaN(channelUtilTh5GL) ||
																	        !/^(0|[1-9]|[1-9][0-9]|100)$/.test(channelUtilTh5GL) ||
																	        parseInt(channelUtilTh5GL) < 0 ||
																	        parseInt(channelUtilTh5GL) > 100)
																	    {
																	    	setting_error_msg_div.style.display = "";
																			radio_error_msg_div.style.display = "";
																	    }
																	}
																	
																	function clearAllTimers()
																	{
																	    clearTimeout(get_device_role_timer_id);
																	    get_device_role_timer_id = null;
																	    clearTimeout(get_apply_status);
																	}
																	
																	function update_displayed_bh_connect_status(obj_1905)
																	{
																		var bhConStatus = document.getElementById("back-haul-connection-status");
																		if(!obj_1905.hasOwnProperty('BH Info'))
																		{
																			console.log("Topology does not have BH Info Property!",obj_1905);
																			bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																			return;
																		}

																		console.log("Topology BH Info:  ! ", obj_1905['BH Info']);
																		if(obj_1905['BH Info'].length == 0)
																		{
																			console.log("Topology does not have Backhaul Medium Type Property! ", obj_1905['BH Info'].length);
																			bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																			return;
																		}

																		var bhInfo = obj_1905['BH Info'][0];
																		console.log("update_displayed_bh_connect_status ['Backhaul Medium Type']:  ", bhInfo['Backhaul Medium Type']);
																		if ( bhInfo['Backhaul Medium Type'] == "Ethernet")
																		{
																			bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic18Test","s")%>)";
																		}
																		else if (bhInfo['Backhaul Medium Type'] == "11AX 2G/5G")
																		{
																				var conn_interface = get_sta_bh_interface();
																				if (conn_interface == "apcli0")
																					bhConStatus.innerHTML = "2.4G";
																				else if (conn_interface == "apclii0")
																					bhConStatus.innerHTML = "5G";
																				else
																					bhConStatus.innerHTML = bhInfo['Backhaul Medium Type'];
																		}
																		else
																		{
																			bhConStatus.innerHTML = bhInfo['Backhaul Medium Type'];
																		}
																	}
																	
																	function find_own_topo_info_by_role(devRole, jsTopoInfo, alMac)
																	{
																		var tree_info = [];
																		var topoInfoArr = jsTopoInfo['topology information'];
																		var bhConStatus = document.getElementById("back-haul-connection-status");

																		if(!(topoInfoArr instanceof Array))
																		{
																			console.log("Incorrect TopologyInfo: Value of topology information is not an Array!");
																			bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																			return;
																		}

																		for(var idx_1905=0; idx_1905 < topoInfoArr.length; idx_1905++)
																		{ 
																			var obj_1905 = topoInfoArr[idx_1905];
																			if (obj_1905.hasOwnProperty('AL MAC') && (obj_1905['AL MAC'].toUpperCase() == alMac.toUpperCase()))
																			{
																				update_displayed_bh_connect_status(obj_1905);
																				return;
																			}
																		}

																		bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																		console.log(alMac," AL-MAC NOT FOUND in Topology! Topology: ",jsTopoInfo);
																	}
																	
																	function get_al_mac_by_role(devRole, jsTopoInfo)
																	{
																		var bhConStatus = document.getElementById("back-haul-connection-status");
																		var XHR_type = '';
																		
																		XHR_type = 'get_al_mac';
																		var x_response = getX_Response(XHR_type);
																		{
																			try
																			{
																				var r = JSON.parse(x_response);
																				var mac_reg_exp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
																				if (r['status'] == "SUCCESS" && mac_reg_exp.test(r['al_mac'])) 
																				{
																					find_own_topo_info_by_role(devRole, jsTopoInfo, r['al_mac']);
																				}
																				else
																				{
																					console.log("Incorrect AL-MAC received!",r['status'],r['al_mac']);
																					bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																				}
																			}
																			catch(e)
																			{
																				console.log("Incorrect response received for get AL-MAC request!",e.name,e.message); 
																				bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																			}
																		}
																	}
																	
																	function get_bh_connection_status()
																	{
																		var devRole = document.getElementById("current-dev-role").innerHTML;
																		var bhConStatus = document.getElementById("back-haul-connection-status");
																		if(devRole == "Agent")
																		{
																			var XHR_type = 'get_runtime_topology';
																			var x_response = getX_Response(XHR_type);
																			{
																				try
																				{
																					var r = JSON.parse(x_response);
																					if(r.status == "SUCCESS")
																					{
																						console.log("get_bh_connection_status get topo ");
																						var tmp_jsTopoInfo = r.luaTopologyInfo;
																		            			tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\u0022/g, '\"');
																		            			tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\\\u0022/g, '\"');
																		                		var jsTopoInfo = JSON.parse(tmp_jsTopoInfo);
																						get_al_mac_by_role(devRole, jsTopoInfo);
																					}
																					else
																					{
																						console.log("get_bh_connection_status() Failed to get Topology Info!\nStatus: ",r.status);
																						bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																					}
																				}
																				catch(e)
																				{
																					console.log("get_bh_connection_status() Incorrect response! Failed to get Topology Info!",e.name,e.message);
																					bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																				}
																			}
																		}
																		else
																		{
																			bhConStatus.innerHTML = "<%tcWebApi_get("String_Entry","EasyMeshBasic19Test","s")%>";
																			console.log("get_bh_connection_status not agent ");
																		}
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
																	                    document.getElementById("current-dev-role-during-not-configured-msg").style.display = "none";
																	                    document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "";
																	                    document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "";
																	                    document.getElementById("map-cfg-tab-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                    document.getElementById("current-dev-role").innerHTML = "Not Configured";
																	                    clearTimeout(get_device_role_timer_id);
																	                    get_device_role_timer_id = null;
																	                    if(document.getElementById("__activeTab").value == "basic")
																	                    {
																	                        get_device_role_timer_id = setTimeout(getDeviceRole, 1000);
																	                    }
																	                		break;
																	                case 1: // Controller
																	                    document.getElementById("current-dev-role-during-not-configured-msg").style.display = "none";
																	                    document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "";
																	                    document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "none";
																	                    document.getElementById("map-cfg-tab-adv").style.display = "";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                    document.getElementById("current-dev-role").innerHTML = "Controller";
																	                		break;
																	                case 2: // Agent
																	                    document.getElementById("current-dev-role-during-not-configured-msg").style.display = "none";
																	                    document.getElementById("map-cfg-basic-dev-role-configured-settings").style.display = "";
																	                    document.getElementById("map-cfg-back-haul-connection-status-settings").style.display = "";
																	                    document.getElementById("map-cfg-tab-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                    document.getElementById("current-dev-role").innerHTML = "Agent";
																	                    get_bh_connection_status();
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
																	
																	function deviceRoleOnChange(devRole)
																	{
																	    devRole = parseInt(devRole);
																	    switch(devRole)
																	    {
																	        case 1: // Controller
																	            document.getElementById("DeviceRole").selectedIndex = 0;
																	            clearTimeout(get_device_role_timer_id);
																	            get_device_role_timer_id = null;
																	        		break;
																	        case 2: // Agent
																	            document.getElementById("DeviceRole").selectedIndex = 1;
																	            clearTimeout(get_device_role_timer_id);
																	            get_device_role_timer_id = null;
																	        		break;
																	        case 0: // Auto
																	            document.getElementById("DeviceRole").selectedIndex = 2;
																	            clearTimeout(get_device_role_timer_id);
																	            get_device_role_timer_id = null;
																	        		break;
																	        default:
																	            console.log("Invalid device role selected: ",devRole);
																	        		break;
																	    }
																	}
																	
																	function triggerMultiApOnBoarding()
																	{
																			/*Only Wi-Fi On-boarding*/
																	    var bh_val = 1;
																	    if(0 == bh_val)
																	    {
																	    	setText('wifi_trigger_onboarding_Flag', 0);
																	    	setText('ether_trigger_onboarding_Flag', 1);
																	    }
																	    else
																	    {
																	    	setText('wifi_trigger_onboarding_Flag', 1);
																	    	setText('ether_trigger_onboarding_Flag', 0);
																	    }
																	    setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */
																	
																	function checkWebStatus()
																	{
																			
																			location.href = '/cgi-bin/wifi_multi_ap_basic.asp';
																	}
																	
																	function get_apply_status_cb(rsp)
																	{
																			clearTimeout(checkWebStatus);
																			webReloadFlag = 0;
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
																	        document.getElementById('map-cfg-tab-basic').style.display = 'none';
																	        document.getElementById('map-cfg-tab-adv').style.display = 'none';
																	        document.getElementById('map-cfg-tab-action').style.display = 'none';
																	        document.getElementById('map-cfg-tab-status').style.display = 'none';
																	        document.getElementById('map-cfg-tab-basic-pel').style.display = 'none';
																	        document.getElementById('map-cfg-tab-adv-pel').style.display = 'none';
																	        document.getElementById('map-cfg-tab-action-pel').style.display = 'none';
																	        document.getElementById('map-cfg-tab-status-pel').style.display = 'none';
																	        setTimeout(checkWebStatus, 5000);
																	        webReloadFlag = 1;
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
																			get_apply_status();
																			if(webReloadFlag)
																			{
																				return;
																			}
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																		var deviceRole = "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																	    if(deviceRole == "")
																	    {
																	    		deviceRoleOnChange("0");
																	    }
																	    else
																	    {
																	    		deviceRoleOnChange(deviceRole);
																	    			getDeviceRole();
																					if(deviceRole == "1")
																					{
																						set_controller_validate_msg();
																					}
																	    		}
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */
																	}
							                </script>
							      </div><!-- isMapCfgSupportedYes -->
							      <% end if %>
									</div><!-- isMapSupportedYes -->
									<% end if %>
								</div><!-- l1datYes -->
						</div>
					</TD>
			    <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
			    	<TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
			    		<TBODY>
			     		<TR>
				    		<TD valign='top'>
			    				<A href="/cgi-bin/help_content.asp#EasyMesh" target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 border=0></A>
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
					<TD width=7 background=/img/panel3.gif>　</TD>
					<TD></TD>
				</TR>
				<TR>
					<TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
						<P class=Item_L2></P>
					</TD>
					<TD width=7 background=/img/panel3.gif>　</TD>
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
					<TD width=162 bgColor=#ef8218>　</TD>
					<TD width=278 bgColor=#427594>　</TD>
					<TD width=196 bgColor=#427594>
						<P align=center id="cbi-btnsave_cancel-pel">
							<IMG id="btnOK" onclick="btnSave()" height=23 src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width=80 border=0>&nbsp;&nbsp;
							<IMG id="btnCancel" height=23 src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" onclick="RefreshPage()" width=80 border=0>
						</P>
					</TD>
					<TD width=170 bgColor=#313031>　</TD>
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
			var isAllBHFHClosed = "<% tcWebApi_get("Mesh_common","isMeshAllBHOrFHClosedStatus","s") %>";
			if(isAllBHFHClosed == "Yes")
			{
				var DeviceEnable=<% tcWebApi_get("Mesh_dat", "MapEnable", "s") %>;		
				meshEnable = $('input[name=MapEnable]:checked').val();
				if ( (meshEnable != DeviceEnable) && (1 == meshEnable) )
				{
					alert("All bh and fh are closed, Please open them !")	
				}						
			}																
		
<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
				var DeviceRole=<% tcWebApi_get("Mesh_common","DeviceRole","s") %>;
				var devRole = document.getElementById("DeviceRole");
				if ( (devRole.value != DeviceRole) )
				{
					setText('Change_Flag', 1);
				}
<% else %>
				setText('Change_Flag', 1);
<% end if %>
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
