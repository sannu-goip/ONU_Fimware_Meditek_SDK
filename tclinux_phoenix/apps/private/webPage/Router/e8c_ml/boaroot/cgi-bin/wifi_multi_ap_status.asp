<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>EasyMesh</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/json2.js"></SCRIPT>

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
							<li class="cbi-tab-disabled" id="map-cfg-tab-action">
								<a href="/cgi-bin/wifi_multi_ap_action.asp"><%tcWebApi_get("String_Entry","EasyMeshActionTest","s")%></a>
							</li>
							<li class="cbi-tab" id="map-cfg-tab-status">
								<a href="/cgi-bin/wifi_multi_ap_status.asp"><%tcWebApi_get("String_Entry","EasyMeshStatusTest","s")%></a>
							</li>
						</ul>
					</TD>
					<TD vAlign=top width=7 background=/img/panel3.gif>
						<ul class="cbi-tabmenu">
							<li class="cbi-tab-disabled" id="map-cfg-tab-basic-pel">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-adv-pel">&nbsp;</li>
							<li class="cbi-tab-disabled" id="map-cfg-tab-action-pel">&nbsp;</li>
							<li class="cbi-tab" id="map-cfg-tab-status-pel">&nbsp;</li>
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
							                    <input type="hidden" name="__activeTab" id="__activeTab" value="status">
							                    	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
							                    		<table class="cbi-section-table" id="map-cfg-status" style="display:none">
							                    		<TBODY>
							                        		<tbody id="map-cfg-status-details">
							                        			<tr>
							                              		<td>AL_MAC</td>
							                              		<td>
							                                  		<span id="al_mac"></span>
							                              		</td>
							                              		<td></td>
							                          		</tr>
							                          		<tbody id="map-cfg-status-sta-steering-progress"  style="display: none">
							                          		<tr>
							                          			<td><%tcWebApi_get("String_Entry","EasyMeshStatus1Text","s")%></td>
							                          			<td>
							                          				<span id="sta-steering-progress"></span>
							                          			</td>
							                          			<td></td>
							                          		</tr>
							                          		<tr>
							                          			<td><%tcWebApi_get("String_Entry","EasyMeshStatus2Text","s")%></td>
							                          			<td>
							                          				<ul id="sta-bh-inf-ul"></ul>
							                          			</td>
							                          			<td></td>
							                          		</tr>
							                          		</tbody>
							                            	<tr>
							                            	    <td style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshStatus3Text","s")%></td>
							                            	    <td>
							                            	        <ul id="ap-bh-inf-list-ul"></ul>
							                            	    </td>
							                            	    <td></td>
							                            	</tr>
							                            	<tr>
							                            	    <td style="vertical-align:middle"><%tcWebApi_get("String_Entry","EasyMeshStatus4Text","s")%></td>
							                            	    <td>
							                            	        <ul id="ap-fh-inf-list-ul"></ul>
							                            	    </td>
							                            	    <td></td>
							                            	</tr>
							                            	<tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshStatus5Text","s")%></td>
							                                  <td colspan="2">
							                                      <input type="button" id="display-runtime-topology-btn" value="<%tcWebApi_get("String_Entry","EasyMeshStatus10Text","s")%>" onClick="displayRuntimeTopology()" />
							                                  </td>
							                              </tr>
							                            	<tr>
							                            	    <td><%tcWebApi_get("String_Entry","EasyMeshStatus6Text","s")%></td>
							                            	    <td colspan="2">
							                            	        <input type="button" id="ap-db-btn" value="<%tcWebApi_get("String_Entry","EasyMeshStatus11Text","s")%>" onClick="displayApCapabilities()" />
							                            	    </td>
							                            	</tr>
							                            	<tr>
							                            	    <td><%tcWebApi_get("String_Entry","EasyMeshStatus7Text","s")%></td>
							                            	    <td colspan="2">
							                            	        <input type="button" id="client-db-btn" value="<%tcWebApi_get("String_Entry","EasyMeshStatus12Text","s")%>" onClick="displayClientCapabilities()" />
							                            	    </td>
							                            	</tr>
							                            	<tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshStatus8Text","s")%></td>
							                                  <td colspan="2">
							                                      <input type="button" id="disp-fh-status-bss" value="<%tcWebApi_get("String_Entry","EasyMeshStatus13Text","s")%>" onClick="dispFhStatusBss()" />
							                                  </td>
							                              </tr>
							                              <tr>
							                                  <td><%tcWebApi_get("String_Entry","EasyMeshStatus9Text","s")%></td>
							                                  <td colspan="2">
							                                      <input type="button" id="disp-bh-link-metrics-ctrler" value="<%tcWebApi_get("String_Entry","EasyMeshStatus14Text","s")%>" onClick="dispBhLinkMetricsCtrler()" />
							                                  </td>
							                              </tr>
							                           </tbody>
							                           </TBODY>
							                        </table>
							                    <% end if %><!-- Mesh_mapdcfg.MapEnable=1  -->
							                </div>
							
							                <SCRIPT language=JavaScript type=text/javascript>
																	
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
																	
																	function JSO_get_sta_steering_progress()
																	{
																		<% if tcWebApi_get("Mesh_Common","DeviceRole","h") <> "1"  then %>
																		var ret = "<% tcWebApi_MeshJsonHook("mesh_get_sta_steering_progress") %>";
																		<% end if %>
																		return ret;
																	}
																	
																	function JSO_get_sta_bh_interface()
																	{
																		<% if tcWebApi_get("Mesh_Common","DeviceRole","h") <> "1"  then %>
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
																				case "get_sta_steering_progress":
																					X_Res = JSO_get_sta_steering_progress();
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
																	
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																	function clearAllTimers()
																	{
																	    clearTimeout(get_device_role_timer_id);
																	    get_device_role_timer_id = null;
																	    clearTimeout(get_al_mac_timer_id);
																	    get_al_mac_timer_id = null;
																	  <% if tcWebApi_get("Mesh_common", "DeviceRole", "h") <> "1" then %>
																	  	clearTimeout(get_sta_steering_progress_timer_id);
																	  	get_sta_steering_progress_timer_id = null;
																	  	clearTimeout(get_bh_connection_status_timer_id);
																	  	get_bh_connection_status_timer_id = null;
																	  <% end if %>
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
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                    document.getElementById("map-cfg-status").style.display = "";
																	                    document.getElementById("map-cfg-status-sta-steering-progress").style.display = "";
																	                		break;
																	                case 1: // Controller
																	                    document.getElementById("map-cfg-tab-adv").style.display = "";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                    document.getElementById("map-cfg-status").style.display = "";
																	                    document.getElementById("map-cfg-status-sta-steering-progress").style.display = "none";
																	                		break;
																	                case 2: // Agent
																	                    document.getElementById("map-cfg-tab-adv").style.display = "none";
																	                    document.getElementById("map-cfg-tab-adv-pel").style.display = "none";
																	                    document.getElementById("map-cfg-tab-action").style.display = "";
																	                    document.getElementById("map-cfg-tab-action-pel").style.display = "";
																	                    document.getElementById("map-cfg-tab-status").style.display = "";
																	                    document.getElementById("map-cfg-tab-status-pel").style.display = "";
																	                    document.getElementById("map-cfg-status").style.display = "";
																	                    document.getElementById("map-cfg-status-sta-steering-progress").style.display = "";
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
																	
																	function getAlMac()
																	{
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
																	                document.getElementById("al_mac").innerHTML = r['al_mac'];
																	            }
																	            else
																	            {
																	                console.log("Incorrect AL-MAC received!",r['status'],r['al_mac']);
																					if(document.getElementById("__activeTab").value == "status")
																	                {
																	                    get_al_mac_timer_id = setTimeout(getAlMac,1000);
																	                }
																	            }
																	        }
																	        catch(e)
																	        {
																	            console.log("Incorrect response received for get AL-MAC request!");
																				if(document.getElementById("__activeTab").value == "status")
																	            {
																	                get_al_mac_timer_id = setTimeout(getAlMac,1000);
																	            }
																	        }
																	    }
																	}
																	
																	function displayRuntimeTopology()
																	{
																			var dev_role = "<% tcWebApi_get("Mesh_dat","MapEnable","s") %>";
																			if(dev_role != "0" && dev_role != "1")
																			{
																					alert("Device Role is not configured yet!\nPlease configure the Device Role.");
																					return;
																			}
																			location.href = '/cgi-bin/wifi_map_runtime_topology.asp';
																	}

																	function dispFhStatusBss()
																	{
																			location.href = '/cgi-bin/wifi_map_bssinfo.asp';
																	}

																	function dispBhLinkMetricsCtrler()
																	{
							    										location.href = '/cgi-bin/wifi_map_bh_link_metrics.asp';
																	}

																	function displayClientCapabilities()
																	{
							    										location.href = '/cgi-bin/wifi_map_client_capabilities.asp';
																	}

																	function displayApCapabilities()
																	{
							    										location.href = '/cgi-bin/wifi_map_ap_capabilities.asp';
																	}

																	function get_sta_steering_progress()
																	{
																		var XHR_type = 'get_sta_steering_progress';
																		var x_response = getX_Response(XHR_type);
																		{
																			try
																			{
																				var rsp = JSON.parse(x_response);
																				if(rsp.status == "SUCCESS")
																				{
																					document.getElementById("sta-steering-progress").innerHTML = rsp['sta_steering_info'];
																				}
																				else
																				{
																					console.log("Failed to get STA Steering Progress!\nStatus: ",rsp.status);
																				}
																				if(document.getElementById("__activeTab").value == "status")
																				{
																					get_sta_steering_progress_timer_id = setTimeout(checkWebStatus, 5000);
																				}
																			}
																			catch(e)
																			{
																				console.log("Incorrect response!\nFailed to get STA Steering Progress!");
																				if(document.getElementById("__activeTab").value == "status")
																				{
																					get_sta_steering_progress_timer_id = setTimeout(checkWebStatus, 5000);
																				}
																			}
																		}
																	}
																	
																	function get_sta_bh_interface()
																	{
																		var devRole =  "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																		var staBhInfUl = document.getElementById("sta-bh-inf-ul");
																		staBhInfUl.innerHTML = "";
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
																						staBhInfUl.parentNode.parentNode.style.display = "";
																						var sta_bh_inf_arr = rsp.staBhInfStr.split(";");
																						for(var idx=0; idx < sta_bh_inf_arr.length; idx++)
																						{
																							if(sta_bh_inf_arr[idx] == "")
																							{
																								continue;
																							}
																							var l = document.createElement("LI");
																							var t = document.createTextNode(sta_bh_inf_arr[idx]);
																							l.appendChild(t);
																							staBhInfUl.appendChild(l);
																						}
																					}
																					else
																					{
																						console.log("Failed to get STA BH Interface!\nStatus: ",rsp.status);
																						if(document.getElementById("__activeTab").value == "status")
																						{
																							setTimeout(checkWebStatus, 5000);
																						}
																					}
																				}
																				catch(e)
																				{
																					console.log("Incorrect response!\nFailed to get STA BH Interface!");
																					if(document.getElementById("__activeTab").value == "status")
																					{
																						setTimeout(checkWebStatus, 5000);
																					}
																				}
																			}
																		}
																		else
																		{
																			staBhInfUl.parentNode.parentNode.style.display = "none";
																		}
																	}
																	
																	function get_ap_bh_inf_list()
																	{
																			var apBhInfList = document.getElementById("ap-bh-inf-list-ul");
																			apBhInfList.innerHTML = "";
																			
																			var x_response = "<% tcWebApi_MeshJsonHook("mesh_get_ap_bh_inf_list") %>";
																			{
																					try
																					{
																							var rsp = JSON.parse(x_response);
																							if(rsp.status == "SUCCESS")
																							{
																									var ap_bh_inf_list_arr = rsp.apBhInfListStr.split(";");
																									for(var idx=0; idx < ap_bh_inf_list_arr.length; idx++)
																									{
																											if(ap_bh_inf_list_arr[idx] == "")
																											{
																													continue;
																											}
																											var l = document.createElement("LI");
																											var t = document.createTextNode(ap_bh_inf_list_arr[idx]);
																											l.appendChild(t);
																											apBhInfList.appendChild(l);
																									}
																							}
																							else
																							{
																									console.log("Failed to get AP BH Interface list!\nStatus: ",rsp.status);
																									if(document.getElementById("__activeTab").value == "status")
																									{
																											setTimeout(checkWebStatus, 5000);
																									}
																							}
																					}
																					catch(e)
																					{
																							console.log("Incorrect response!\nFailed to get AP BH Interface list!");
																							if(document.getElementById("__activeTab").value == "status")
																							{
																									setTimeout(checkWebStatus, 5000);
																							}
																					}
																			}
																	}
																	
																	function get_ap_fh_inf_list()
																	{
																	    var apFhInfList = document.getElementById("ap-fh-inf-list-ul");
																	    apFhInfList.innerHTML = "";
																	    
																	    var x_response = "<% tcWebApi_MeshJsonHook("mesh_get_ap_fh_inf_list") %>";
																	    {
																	        try
																	        {
																	            var rsp = JSON.parse(x_response);
																	            if(rsp.status == "SUCCESS")
																	            {
																	                var ap_fh_inf_list_arr = rsp.apFhInfListStr.split(";");
																	                for(var idx=0; idx < ap_fh_inf_list_arr.length; idx++)
																	                {
																	                    if(ap_fh_inf_list_arr[idx] == "")
																	                    {
																	                        continue;
																	                    }
																	                    var l = document.createElement("LI");
																	                    var t = document.createTextNode(ap_fh_inf_list_arr[idx]);
																	                    l.appendChild(t);
																	                    apFhInfList.appendChild(l);
																	                }
																	            }
																	            else
																	            {
																	                console.log("Failed to get AP FH Interface list!\nStatus: ",rsp.status);
																	                if(document.getElementById("__activeTab").value == "status")
																	                {
																	                    setTimeout(checkWebStatus, 5000);
																	                }
																	            }
																	        }
																	        catch(e)
																	        {
																	            console.log("Incorrect response!\nFailed to get AP FH Interface list!");
																	            if(document.getElementById("__activeTab").value == "status")
																	            {
																	            		setTimeout(checkWebStatus, 5000);
																	            }
																	        }
																	    }
																	}
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */
																	
																	function hi_status()
																	{
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																		clearAllTimers();
																		getAlMac();
																	<% if tcWebApi_get("Mesh_common", "DeviceRole", "h") <> "1" then %>
																		get_sta_steering_progress();
																		get_sta_bh_interface();
																	<% end if %>
																		get_ap_bh_inf_list();
																		get_ap_fh_inf_list();
																	<% end if %>/* Mesh_mapdcfg.MapEnable=1 */
																	}
																	
																	function checkWebStatus()
																	{
																			location.href = '/cgi-bin/wifi_multi_ap_status.asp';
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
																	    get_apply_status();
																	<% if tcWebApi_get("Mesh_dat", "MapEnable", "h") = "1" then %>
																		var deviceRole = "<% tcWebApi_get("Mesh_common","DeviceRole","s") %>";
																	    if(deviceRole == "")
																	    {
																		    deviceRole = "0";
																	    }

																			getDeviceRole();
																		
																		hi_status();
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
						<P align=center id="cbi-btnsave_cancel-pel" style="display: none">
							<IMG id="btnOK" height=23 src="/img/ok.gif" width=80 border=0>&nbsp;&nbsp;
							<IMG id="btnCancel" height=23 src="/img/cancel.gif" width=80 border=0>
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
