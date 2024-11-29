<%
if Request_Form("Save_Flag") = "1" then
	if Request_Form("SaveAll_Flag") = "1" then

		if Request_Form("MapEnable") = "1" then
			TCWebApi_constSet("Mesh_dat", "MapEnable","1")

			TCWebApi_Set("Mesh_steercfg", "CUOverloadTh_2G", "CUOverloadTh_2G");
			TCWebApi_Set("Mesh_steercfg", "CUOverloadTh_5G_L", "CUOverloadTh_5G_L")
			TCWebApi_Set("Mesh_steercfg", "CUOverloadTh_5G_H", "CUOverloadTh_5G_L")

			TCWebApi_Set("Mesh_mapdcfg", "APSteerRssiTh", "APSteerRssiTh")
			TCWebApi_constSet("Mesh_common", "ReinitWifiFlag","1")

			TCWebApi_commit("Mesh_dat")
			if Request_Form("MeshTriggerFlag") = "1" then
				TCWebApi_commit("Mesh_common")
			end if
		else
			TCWebApi_constSet("Mesh_dat", "MapEnable","0")

			TCWebApi_commit("Mesh_dat")
		end if

	else
		if Request_Form("Action_Flag") = "1" then			
			if Request_Form("MapEnable") = "1" then
				if Request_Form("apply_channel_utilization_th_Flag") = "1" then
						TCWebApi_Set("Mesh_steercfg", "CUOverloadTh_2G", "CUOverloadTh_2G");
						TCWebApi_Set("Mesh_steercfg", "CUOverloadTh_5G_L", "CUOverloadTh_5G_L")
						TCWebApi_Set("Mesh_steercfg", "CUOverloadTh_5G_H", "CUOverloadTh_5G_L")
						TCWebApi_commit("Mesh_steercfg")
				end if

				if Request_Form("apply_ap_steer_rssi_th_Flag") = "1" then
						TCWebApi_Set("Mesh_mapdcfg", "APSteerRssiTh", "APSteerRssiTh")
						TCWebApi_commit("Mesh_mapdcfg")
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
							<li class="cbi-tab-disabled" id="map-cfg-tab-basic">
								<a href="wifi_multi_ap_basic.asp"><%tcWebApi_get("String_Entry","EasyMeshBasicTest","s")%></a>
							</li>
							<li class="cbi-tab" id="map-cfg-tab-adv">
								<a href="wifi_multi_ap_advanced.asp"><%tcWebApi_get("String_Entry","EasyMeshAdvancedTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-action">
								<a href="wifi_multi_ap_action.asp"><%tcWebApi_get("String_Entry","EasyMeshActionTest","s")%></a>
							</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-status">
								<a href="/cgi-bin/wifi_multi_ap_status.asp"><%tcWebApi_get("String_Entry","EasyMeshStatusTest","s")%></a>
							</li>
						</ul>
					</TD>
					<TD vAlign=top width=7 background=/img/panel3.gif>
						<ul class="cbi-tabmenu">
							<li class="cbi-tab-disabled" id="map-cfg-tab-basic-pel">&nbsp;</li>
							<li class="cbi-tab" id="map-cfg-tab-adv-pel">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-action-pel">&nbsp;</li>
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

						            		<div id="map_validate_controller_settings_error_div" style="display: none">
															<div id="map_validate_controller_rssith_error_msg" style="display: none" class="alert-message error"><big><strong>RSSI Threshold is incorrect!</big></strong></div>
															<div id="map_validate_controller_channelth_error_msg" style="display: none" class="alert-message error"><big><strong>2G/5G Channel Utilization Threshold is incorrect!</big></strong></div>
															<div id="map_validate_controller_error_msg" class="alert-message error"><big><strong>Please enter an integer number !</big></strong></div>
														</div>
							                <div id="MULTI_AP_SETTINGS">
							                    <input type="hidden" name="__activeTab" id="__activeTab" value="adv">
							                    <form method="post" name="cbi" action="/cgi-bin/wifi_multi_ap_advanced.asp" onsubmit="return validate_all()" autocomplete="off">
									                    <input type="hidden" name="MapEnable" value="<% tcWebApi_get("Mesh_dat","MapEnable","s") %>" />
							                    		<input type="hidden" name="SaveAll_Flag" value="0" />
							                    		<input type="hidden" name="Save_Flag" value="0" />
							                    		<input type="hidden" name="Action_Flag" value="0" />
							                    		<input type="hidden" name="apply_channel_utilization_th_Flag" value="0" />
							                    		<input type="hidden" name="apply_ap_steer_rssi_th_Flag" value="0" />
											                    		<input type="hidden" name="MeshTriggerFlag" value="<% tcWebApi_get("Mesh_Common","MeshTriggerFlag","s") %>" />
							                    		
							                    <% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
							                        <table class="cbi-section-table" id="map-cfg-adv" style="display:none">
							                          <tbody id="map-cfg-adv-controller-settings">
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshAdvanced1Test","s")%></td>
							                                  <td>
							                                      <input type="button" value="<%tcWebApi_get("String_Entry","EasyMeshAdvanced2Test","s")%>" onClick="configBss()" />
							                                  </td>
							                                  <td></td>
							                              </tr>
							                              <tr>
							                                  <td rowspan="3"  style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAdvanced3Test","s")%></td>
							                                  <td>
							                                      <span>2G</span>
							                                  </td>
							                                  <td>
							                                  		<input type="text" id="CUOverloadTh_2G" name="CUOverloadTh_2G" value="<%if tcWebApi_get("Mesh_steercfg","CUOverloadTh_2G","h" ) <> "N/A" then tcWebApi_get("Mesh_steercfg","CUOverloadTh_2G","s") end if %>" />
							                                      <span><%tcWebApi_get("String_Entry","EasyMeshAdvanced4Test","s")%></span>
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td>
							                                      <span>5G</span>
							                                  </td>
							                                  <td>
							                                  		<input type="text" id="CUOverloadTh_5G_L" name="CUOverloadTh_5G_L" value="<%if tcWebApi_get("Mesh_steercfg","CUOverloadTh_5G_L","h" ) <> "N/A" then tcWebApi_get("Mesh_steercfg","CUOverloadTh_5G_L","s") end if %>" />
							                                      <span><%tcWebApi_get("String_Entry","EasyMeshAdvanced4Test","s")%></span>
							                                      <input type="hidden" id="CUOverloadTh_5G_H" name="CUOverloadTh_5G_H" value="<%if tcWebApi_get("Mesh_steercfg","CUOverloadTh_5G_H","h" ) <> "N/A" then tcWebApi_get("Mesh_steercfg","CUOverloadTh_5G_H","s") end if %>" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="apply-channel-utilization-th-btn" value="<%tcWebApi_get("String_Entry","EasyMeshAdvanced5Test","s")%>" onClick="applyChannelUtilizationTh()" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td rowspan="2"  style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshAdvanced6Test","s")%></td>
							                                  <td>
							                                  		<input type="text" id="APSteerRssiTh" name="APSteerRssiTh" value="<%if tcWebApi_get("Mesh_mapdcfg","APSteerRssiTh","h" ) <> "N/A" then tcWebApi_get("Mesh_mapdcfg","APSteerRssiTh","s") end if %>" />
							                                      <span><%tcWebApi_get("String_Entry","EasyMeshAdvanced7Test","s")%></span>
							                                  </td>
							                                  <td></td>
							                              </tr>
							                              <tr>
							                                  <td colspan="2">
							                                      <input type="button" id="apply-rssi-th-btn" value="<%tcWebApi_get("String_Entry","EasyMeshAdvanced8Test","s")%>" onClick="applyRssiTh()" />
							                                  </td>
							                              </tr>
							                          </tbody>
							                        </table>
							                    <% end if %><!-- Mesh_mapdcfg.MapEnable=1  -->
							                    </form>
							                </div>
							
							                <SCRIPT language=JavaScript type=text/javascript>
							                		var webReloadFlag = 0;
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																	var get_device_role_timer_id;
																	var get_al_mac_timer_id;
																	var get_sta_steering_progress_timer_id;
																	var get_bh_connection_status_timer_id;
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
																	
																	function validate_all()
																	{
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																		var curDevRole = "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																		curDevRole = parseInt(curDevRole);

																		switch(curDevRole)
																		{
																			case 1: // Controller
																				break;
																			case 2: // Agent
																				break;
																			default:
																				return false;
																		}

																		if(curDevRole == 1)
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
																	    var apSteerRssiTh = document.getElementById("APSteerRssiTh").value;
																	    if (apSteerRssiTh == "" ||
																	        isNaN(apSteerRssiTh) ||
																	        !/^(0|-[1-9]|-[1-9][0-9])$/.test(apSteerRssiTh) ||
																	        parseInt(apSteerRssiTh) > 0 ||
																	        parseInt(apSteerRssiTh) < -90)
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert7Test","s")%>\n<%tcWebApi_get("String_Entry","EasyMeshBasicAlert8Test","s")%>");
																	        document.getElementById("APSteerRssiTh").focus();
																	        document.getElementById("APSteerRssiTh").select();
																	        return false;
																	    }
																	    return true;
																	}
																	
																	function validateChannelUtilizationTh()
																	{
																	    var channelUtilTh2G = document.getElementById("CUOverloadTh_2G").value;
																	    var channelUtilTh5GL = document.getElementById("CUOverloadTh_5G_L").value;
																	    var channelUtilTh5GH = channelUtilTh5GL;
																	    
																	    if (channelUtilTh2G == "" ||
																	        isNaN(channelUtilTh2G) ||
																	        !/^(0|[1-9]|[1-9][0-9]|100)$/.test(channelUtilTh2G) ||
																	        parseInt(channelUtilTh2G) < 0 ||
																	        parseInt(channelUtilTh2G) > 100)
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert9Test","s")%>\n<%tcWebApi_get("String_Entry","EasyMeshBasicAlert11Test","s")%>");
																	        document.getElementById("CUOverloadTh_2G").focus();
																	        document.getElementById("CUOverloadTh_2G").select();
																	        return false;
																	    }
																	    if (channelUtilTh5GL == "" ||
																	        isNaN(channelUtilTh5GL) ||
																	        !/^(0|[1-9]|[1-9][0-9]|100)$/.test(channelUtilTh5GL) ||
																	        parseInt(channelUtilTh5GL) < 0 ||
																	        parseInt(channelUtilTh5GL) > 100)
																	    {
																	        alert("<%tcWebApi_get("String_Entry","EasyMeshBasicAlert10Test","s")%>\n<%tcWebApi_get("String_Entry","EasyMeshBasicAlert11Test","s")%>");
																	        document.getElementById("CUOverloadTh_5G_L").focus();
																	        document.getElementById("CUOverloadTh_5G_L").select();
																	        return false;
																	    }
																	    
																	    return true;
																	}

																	function set_controller_validate_msg()
																	{
																		var apSteerRssiTh = document.getElementById("APSteerRssiTh").value;
																		var channelUtilTh2G = document.getElementById("CUOverloadTh_2G").value;
																	    var channelUtilTh5GL = document.getElementById("CUOverloadTh_5G_L").value;

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
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																	                    document.getElementById("map-cfg-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "none";
																	                    document.getElementById("map-cfg-tab-status").style.display = "none";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "none";
																	                    clearTimeout(get_device_role_timer_id);
																	                    get_device_role_timer_id = null;
																	                    if(document.getElementById("__activeTab").value == "basic")
																	                    {
																	                        get_device_role_timer_id = setTimeout(getDeviceRole, 1000);
																	                    }
																	                		break;
																	                case 1: // Controller
																	                    document.getElementById("map-cfg-tab-adv").style.display = "";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "";
																	                    document.getElementById("map-cfg-adv").style.display = "";
																	                    document.getElementById("map-cfg-adv-controller-settings").style.display = "";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                		break;
																	                case 2: // Agent
																	                    document.getElementById("map-cfg-tab-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																	                    document.getElementById("map-cfg-adv").style.display = "";
																	                    document.getElementById("map-cfg-adv-controller-settings").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
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
																	
																	function applyChannelUtilizationTh()
																	{
																	    if(!validateChannelUtilizationTh())
																	    {
																	        return;
																	    }
																	    
																	    setText('apply_channel_utilization_th_Flag', 1);
																	    setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	function applyRssiTh()
																	{
																	    if(!validateApSteerRssiTh())
																	    {
																	        return;
																	    }
																	    
																	    setText('apply_ap_steer_rssi_th_Flag', 1);
																	    setText('Action_Flag', 1);
																	    formSubmit(0);
																	}
																	
																	function configBss()
																	{
																	    location.href = '/cgi-bin/wifi_map_bss_cfg_renew.asp';
																	}
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */
																	
																	function checkWebStatus()
																	{
																		location.href = '/cgi-bin/wifi_multi_ap_advanced.asp';
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
																	        webReloadFlag = 1;
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
																			get_apply_status();
																			if(webReloadFlag)
																			{
																				return;
																			}
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																			var deviceRole = "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																	    if(deviceRole == "")
																	    {
																	    		deviceRole = "0";
																	    }
																	    
																	    if(deviceRole != "0")
																	    {
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
