<%
if Request_Form("Save_Flag") = "1" then
		TCWebApi_constSet("Mesh_action", "trigger_de_dump", "1")
		TCWebApi_Set("Mesh_actionpara", "trigger_de_dump", "_Mac_value")	
		TCWebApi_Set("Mesh_actionpara", "show_al_mac", "show_al_mac")	
		TCWebApi_commit("Mesh_action")
		TCWebApi_constSet("Mesh_action", "trigger_de_dump_is_open", "1")
else
		TCWebApi_constSet("Mesh_action", "trigger_de_dump_is_open", "0")
	
end if
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>EasyMesh Run-time Topology Display</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/vis.js"></SCRIPT>
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
						<p class="cbi-tab" id="p-item-title" ><%tcWebApi_get("String_Entry","EasyMeshRunTime6Text","s")%></p>
					</TD>
					<TD vAlign=top width=7 background="/img/panel3.gif" height=30>
						<p class="cbi-tab" id="p-item-title-pel" ></p>
					</TD>
    			<TD vAlign=top width=474 rowspan="2">
						<div id="maincontent" class="container">
							
							<div class="alert-message" id="getTopoInfoMsgDiv">
			    			<img src="/img/loading.gif" alt="" style="vertical-align:middle" />
			    			<big><strong>Retrieving EasyMesh Run-time Topology Information!</strong></big>
							</div>
							<div class="alert-message error" style="display:none">
			    			<span><strong id="topoErrMsgDiv"></strong></span>
							</div>
							<div id="display_tree_vis" style="display:none"></div>
							<button id="toggle_disp_tree_vis_detail_btn" onclick="toggle_disp_tree_vis_detail()" disabled="disabled"><%tcWebApi_get("String_Entry","EasyMeshRunTime1Text","s")%></button>
							
							<div id="display_all_1905_dev_info_div" style="display:none"></div>							
							<button id="toggle_all_1905_dev_info_btn" onclick="toggle_disp_dev_table(this)" disabled="disabled"><%tcWebApi_get("String_Entry","EasyMeshRunTime2Text","s")%></button>
							<div id="display_all_1905_channel_info_div" style="display:none"></div>
							<form method="post" name="cbi" id="add_runtime_topology_form" action="/cgi-bin/wifi_map_runtime_topology.asp" autocomplete="off" style="display:none">
									<input type="hidden" name="Save_Flag" id="Save_Flag" value="0" />
									<input type="hidden" name="_Mac_value" id="_Mac_value" value="" />
									<input type="hidden" name="show_al_mac" id="show_al_mac" value="" />
									<div name="div_runtime_topology" id="div_runtime_topology" style="display:none"></div>
							</form>
							
							<script type="text/javascript">
									
									var max_cellspan = 2;
									var al_mac = '<% tcWebApi_get("Mesh_actionpara","show_al_mac","s") %>';
									
									if(typeof String.prototype.startsWith != 'function')
									{
											String.prototype.startsWith = function(prefix)
											{
													return this.slice(0, prefix.length) == prefix;
											}
									}
									
									function toggle_disp_tree_vis_detail()
									{
											close_channel_div();
											window.open("/cgi-bin/wifi_map_get_run_time_topology_detail.asp","runtime_topology_detail","toolbar=no,menubar=no,height=400, width=800,location=0,left=200,top=200");
									}

									function JSO_get_runtime_topology()
									{
											var ret = "<% tcWebApi_MeshJsonHook("mesh_get_run_time_topology") %>";
											ret = ret.replace(/\'/g,'\\\\u0022');
											ret = ret.replace(/\\\\u0022Pass-phrase\\\\u0022:/g,'\\\\u0022WPAPSK\\\\u0022:');
											return ret;
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
											if(k == "Device role")
											{
												v = get_dev_role_string(v);
											}
											if(k == "Map version")
											{
												v = get_version_string(v);
											}
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
									var show_al_mac_now, al_mac_now;
									
									show_al_mac_now = '<% tcWebApi_get("Mesh_actionpara","show_al_mac","s") %>';
									al_mac_now = '<% tcWebApi_get("Mesh_Mapcfg","AL-MAC","s") %>';
									
									if (show_al_mac_now == al_mac_now && k == "Bss Info")
									{
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
									} else {
										//show Agent on Controller.
										for(var a_idx=a.length - 1; a_idx >= 0; a_idx--)
										{
											var v = a[a_idx];
											if(((typeof v) == "object") && v != null)
											{
												row = table_id.insertRow(-1);
												cell = row.insertCell(-1);
												cell.rowSpan = set_rowspan(v);
												cell.style.verticalAlign  = "middle";
												cell.innerHTML = "<strong>" + k + ' - ' + (a.length - a_idx) + "</strong>";
												obj_loop(table_id, row, v);
											}
											else
											{
												console.log("Incorrect Topology: Array element is not an object!",a);
											}
										}
									}
							    }

							    function prep_bssinfo(dev1905Obj)
							    {
							    		var tag_dev1905Obj = dev1905Obj;
							        for(var radIdx=0; radIdx < tag_dev1905Obj['Radio Info'].length; radIdx++)
							        {
							            for(var bssIdx=0; bssIdx < tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'].length; bssIdx++)
							            {
							            		var Security_val = '';
							            		if(tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx].hasOwnProperty('Security'))
							            		{
							            				Security_val = tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]['Security'];
							            				if(Security_val == '0001')
							            				{
							            						if(tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx].hasOwnProperty('Encryption'))
							            						{
							            								tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]['Encryption'] = '0001';
							            						}
							            						if(tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx].hasOwnProperty('WPAPSK'))
							            						{
							            								tag_dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]['WPAPSK'] = '';
							            						}
							            				}
							            		}
							            }
							        }
							        return tag_dev1905Obj;
							    }
							    
							    function draw_topology_table(jsTopoInfo)
							    {
							        var tree_info = [];
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
							            document.getElementById("display_all_1905_dev_info_div").appendChild(dev1905fset);
							            
							            set_max_cellspan(obj_1905);
							            obj_loop(dev1905tbl, null, obj_1905);
							            tree_info[tree_info_count] = prep_tree_info(obj_1905);
							            
							            document.getElementById("display_all_1905_dev_info_div").appendChild(dev1905tbl);
							            tree_info_count++;
							        }
							        tree_info.sort(function(a, b){return a.hopCount - b.hopCount});
							        disp_topology_vis(tree_info);
							    }
							    <% if tcWebApi_get("WebCustom_Entry","isNewEasyMeshSupport","h") = "Yes"  then %>
							    function prep_tree_info(dev1905Obj)
							    {
							        var node = {};
							        node["devRole"] = get_dev_role_string(dev1905Obj['Device role']);
							        node["alMac"] = dev1905Obj['AL MAC'];
							        node["hopCount"] = parseInt(dev1905Obj['Distance from controller']);
							        node['UplinkAlMac'] = dev1905Obj['Upstream 1905 device'];
							        node["wifistaInfo"] = {};
							        node["ethstaInfo"] = {};
							        node["mapVersion"] = get_version_string(dev1905Obj['Map version']);
							        for(var radIdx=0; radIdx < dev1905Obj['Radio Info'].length; radIdx++)
							        {
							            for(var bssIdx=0; bssIdx < dev1905Obj['Radio Info'][radIdx]['BSSINFO'].length; bssIdx++)
							            {
							                if(dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx].hasOwnProperty('connected sta info'))
							                {
							                    for(var staIdx=0; staIdx < dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]["connected sta info"].length; staIdx++)
							                    {
							                        var ssid = dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]['SSID'];
																			var staMac = dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]["connected sta info"][staIdx]['STA MAC address'];
																			var bh_sta = dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]["connected sta info"][staIdx]['BH STA'];
																			if (bh_sta == 'Yes')
																			{
																				console.log("it is bh sta  staMac = ", staMac);
																				continue;
																			}

																			if (node["wifistaInfo"].hasOwnProperty(ssid))
																			{
																				node["wifistaInfo"][ssid].push(staMac);
																			}
																			else
																			{
																				node["wifistaInfo"][ssid] = Array(staMac);
																			}
							                    }
							                }
							            }
							        }
							        
							        for(var cliIdex = 0; cliIdex < dev1905Obj['Other Clients Info'].length; cliIdex++)
							        {
							        		if (dev1905Obj['Other Clients Info'][cliIdex].hasOwnProperty('Client Address'))
							        		{
							        			var cliAdd = dev1905Obj['Other Clients Info'][cliIdex]['Client Address'];
							        			var medium = dev1905Obj['Other Clients Info'][cliIdex]['Medium'];
							        			if (node["ethstaInfo"].hasOwnProperty(medium))
														{
															node["ethstaInfo"][medium].push(cliAdd);
														}
														else
														{
															node["ethstaInfo"][medium] = Array(cliAdd);
														}
							        		}
							        }
							        return node;
							    }
							    <% else %>
							    function prep_tree_info(dev1905Obj)
							    {
							        var node = {};
							        node["devRole"] = get_dev_role_string(dev1905Obj['Device role']);
							        node["alMac"] = dev1905Obj['AL MAC'];
							        node["hopCount"] = parseInt(dev1905Obj['Distance from controller']);
							        node['UplinkAlMac'] = dev1905Obj['Upstream 1905 device'];
							        node["staInfo"] = {};
									node["mapVersion"] = get_version_string(dev1905Obj['Map version']);
							        for(var radIdx=0; radIdx < dev1905Obj['Radio Info'].length; radIdx++)
							        {
							            for(var bssIdx=0; bssIdx < dev1905Obj['Radio Info'][radIdx]['BSSINFO'].length; bssIdx++)
							            {
							                if(dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx].hasOwnProperty('connected sta info'))
							                {
							                    for(var staIdx=0; staIdx < dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]["connected sta info"].length; staIdx++)
							                    {
							                        var ssid = dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]['SSID'];
													var staMac = dev1905Obj['Radio Info'][radIdx]['BSSINFO'][bssIdx]["connected sta info"][staIdx]['STA MAC address'];
													if (node["staInfo"].hasOwnProperty(ssid))
													{
														node["staInfo"][ssid].push(staMac);
													}
													else
													{
														node["staInfo"][ssid] = Array(staMac);
													}
							                    }
							                }
							            }
							        }
							        return node;
							    }
							    <% end if %>
							    function get_uplink_tree_info(tree_info, mac)
							    {
							        for(var nIdx=0; nIdx < tree_info.length; nIdx++)
							        {
							            if(tree_info[nIdx]['alMac'] == mac)
							            {
							                return '"' + tree_info[nIdx]['devRole'] + '\n' + "AL_MAC" + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"';
							            }
							        }
							        return "";
							    }
							    <% if tcWebApi_get("WebCustom_Entry","isNewEasyMeshSupport","h") = "Yes"  then %>
							    function disp_topology_vis(tree_info)
							    {
							        var div = document.getElementById("display_tree_vis");
							        div.style.display = "";
							        var topoErrMsg = document.getElementById("topoErrMsgDiv");
							        topoErrMsg.innerHTML = "";
							        var str = "";
							        var numStaPerDev = 0;
							        console.log("TREE INFO VIS = ",tree_info);
							        for(var nIdx=0; nIdx < tree_info.length; nIdx++)
							       	{
							            if (tree_info[nIdx]['devRole'] == "Agent")
							            {
							                var upLinkNodeStr = get_uplink_tree_info(tree_info, tree_info[nIdx]['UplinkAlMac']);
							                if(upLinkNodeStr != "")
							                {
							                    str += '"' + tree_info[nIdx]['devRole'] + '\n' + "AL_MAC" + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + '->' + upLinkNodeStr + " ";
							                }
							                else
							                {
							                    console.log("INFO: AGENT WITH NO UP-LINK MAC = ",tree_info[nIdx]['alMac']);
							                    topoErrMsg.innerHTML += "<br />ERROR: Agent with AL-MAC " + tree_info[nIdx]['alMac'] +" has no upstream 1905 device!";
							                    topoErrMsg.style.display = "";
							                }
							            }
							            for(var ssid in tree_info[nIdx]['wifistaInfo'])
							            {
							                var staIdx;
							                for(var staIdx=0; staIdx < tree_info[nIdx]['wifistaInfo'][ssid].length; staIdx++)
							                {
							                    var mac1stOctet = parseInt(tree_info[nIdx]['wifistaInfo'][ssid][staIdx].substring(0,2),16);
												if ( tree_info[nIdx]['wifistaInfo'][ssid][staIdx] == "00:00:00:00:00:00" )
												{
													continue;
												}
							                    str += '"STA\nMAC - ' + tree_info[nIdx]['wifistaInfo'][ssid][staIdx] + '\nSSID - ' + ssid + '"' + '->' + '"' + tree_info[nIdx]['devRole'] + '\n' + "AL_MAC" + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + " ";
							                    numStaPerDev += 1;
							                }
							            }
							            
							            for(var medium in tree_info[nIdx]['ethstaInfo'])
							            {
							                var ethstaIdx;
							                for(var ethstaIdx=0; ethstaIdx < tree_info[nIdx]['ethstaInfo'][medium].length; ethstaIdx++)
							                {
							                    var mac1stOctet = parseInt(tree_info[nIdx]['ethstaInfo'][medium][ethstaIdx].substring(0,2),16);
							                    if(mac1stOctet & 0x02)
							                    {
							                        continue;
							                    }
							                    str += '"STA\nMAC - ' + tree_info[nIdx]['ethstaInfo'][medium][ethstaIdx] + '\nMedium - ' + medium + '"' + '->' + '"' + tree_info[nIdx]['devRole'] + '\n' + "AL_MAC" + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + " ";
							                    numStaPerDev += 1;
							                }
							            }
							            
							            if (numStaPerDev == 0 && tree_info.length == 1 &&
							                (tree_info[nIdx]['devRole'] == "Controller" ||
							                tree_info[nIdx]['devRole'] == "Agent"))
							            {
							                str += '"' + tree_info[nIdx]['devRole'] + '\n' + 'AL_MAC' + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + " ";
							            }
							        }
							        console.log("TREE INFO str = ",str);
							        
							        var NewDOTstring = "dinetwork{"+str+"}";
							        
							        var parsedData = vis.network.convertDot(NewDOTstring);
							        
							        for(var count=0;count<parsedData.nodes.length;count++)
							        {
							            console.log("NODE LABEL = ",parsedData.nodes[count].label);
							            if(parsedData.nodes[count].label.startsWith('Agent'))
							            {
							                parsedData.nodes[count].color='Orange';
							            }
							            else if(parsedData.nodes[count].label.startsWith('STA'))
							            {
							                parsedData.nodes[count].color='Green';
							            }
							            else
							            {
							                parsedData.nodes[count].shape='box';
							            }
							        }
							        
							        var data = {
							          nodes: parsedData.nodes,
							          edges: parsedData.edges
							        }
							        var options = parsedData.options;
							        
							        // create a network
							        var network = new vis.Network(div, data, options);
									
									<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
									// append click action
							        network.on('click', function(parsedData){
							        		var DeviceRole = '<%tcWebApi_get("Mesh_common","DeviceRole","s")%>';
							        		for(var i=0;i<parsedData.nodes.length;i++){
							        			if(parsedData.note == parsedData.nodes[i].id)
							        				/*console.log(parsedData.nodes[i])*/
							        				var clickNode = parsedData.nodes[i];
							        				var al_mac = '<%tcWebApi_get("Mesh_mapcfg","AL-MAC","s")%>';
							        				al_mac = al_mac.toLocaleLowerCase();
							        				if(clickNode.startsWith('STA') || (DeviceRole == 2 && !clickNode.includes(al_mac) || clickNode.includes("R1")))
							        				{
							        					return;
							        				}
							        				else
							        				{
							        					var str = clickNode;
							        					var arr = str.split("AL_MAC - ");
							        					/*console.log(arr);*/
							        					var arr2 = arr[1].split("\n");
														setText("_Mac_value",arr2);
														setText('show_al_mac', arr2[0]);	
														formSubmit(1);
							        				}						        					
							        		}        	
							        })
									<% end if %>
							        network.setSize('440px','300px');
							    }
							    <% else %>
							    function disp_topology_vis(tree_info)
							    {
							        var div = document.getElementById("display_tree_vis");
							        div.style.display = "";
							        var topoErrMsg = document.getElementById("topoErrMsgDiv");
							        topoErrMsg.innerHTML = "";
							        var str = "";
							        var numStaPerDev = 0;
							        console.log("TREE INFO VIS = ",tree_info);
							        for(var nIdx=0; nIdx < tree_info.length; nIdx++)
							       	{
							            if (tree_info[nIdx]['devRole'] == "Agent")
							            {
							                var upLinkNodeStr = get_uplink_tree_info(tree_info, tree_info[nIdx]['UplinkAlMac']);
							                if(upLinkNodeStr != "")
							                {
							                    str += '"' + tree_info[nIdx]['devRole'] + '\n' + "AL_MAC" + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + '->' + upLinkNodeStr + " ";
							                }
							                else
							                {
							                    console.log("INFO: AGENT WITH NO UP-LINK MAC = ",tree_info[nIdx]['alMac']);
							                    topoErrMsg.innerHTML += "<br />ERROR: Agent with AL-MAC " + tree_info[nIdx]['alMac'] +" has no upstream 1905 device!";
							                    topoErrMsg.style.display = "";
							                }
							            }
							            for(var ssid in tree_info[nIdx]['staInfo'])
							            {
							                var staIdx;
							                for(var staIdx=0; staIdx < tree_info[nIdx]['staInfo'][ssid].length; staIdx++)
							                {
							                    var mac1stOctet = parseInt(tree_info[nIdx]['staInfo'][ssid][staIdx].substring(0,2),16);
							                    str += '"STA\nMAC - ' + tree_info[nIdx]['staInfo'][ssid][staIdx] + '\nSSID - ' + ssid + '"' + '->' + '"' + tree_info[nIdx]['devRole'] + '\n' + "AL_MAC" + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + " ";
							                    numStaPerDev += 1;
							                }
							            }
							    
							            if (numStaPerDev == 0 && tree_info.length == 1 &&
							                (tree_info[nIdx]['devRole'] == "Controller" ||
							                tree_info[nIdx]['devRole'] == "Agent"))
							            {
							                str += '"' + tree_info[nIdx]['devRole'] + '\n' + 'AL_MAC' + ' - ' + tree_info[nIdx]['alMac'] + '\n' + "MAP_Version" + ' - ' + tree_info[nIdx]['mapVersion'] + '"' + " ";
							            }
							        }
							        console.log("TREE INFO str = ",str);
							        
							        var NewDOTstring = "dinetwork{"+str+"}";
							        
							        var parsedData = vis.network.convertDot(NewDOTstring);
							        
							        for(var count=0;count<parsedData.nodes.length;count++)
							        {
							            console.log("NODE LABEL = ",parsedData.nodes[count].label);
							            if(parsedData.nodes[count].label.startsWith('Agent'))
							            {
							                parsedData.nodes[count].color='Orange';
							            }
							            else if(parsedData.nodes[count].label.startsWith('STA'))
							            {
							                parsedData.nodes[count].color='Green';
							            }
							            else
							            {
							                parsedData.nodes[count].shape='box';
							            }
							        }
							        
							        var data = {
							          nodes: parsedData.nodes,
							          edges: parsedData.edges
							        }
							        var options = parsedData.options;
							        
							        // create a network
							        var network = new vis.Network(div, data, options);
							        network.setSize('440px','300px');
							    }
							    <% end if %>
							    function toggle_disp_dev_table(btn)
							    {	
									close_channel_div();
							        var div = document.getElementById("display_all_1905_dev_info_div");
							        var btn_str = btn.innerHTML;
							        if(btn_str.startsWith('<%tcWebApi_get("String_Entry","EasyMeshRunTime5Text","s")%>'))
							        {
							            btn.innerHTML = '<%tcWebApi_get("String_Entry","EasyMeshRunTime3Text","s")%>';
							            div.style.display = "";
							            if(div.children.length == 0)
							            {
							                div.innerHTML = "";
							                var b = document.createElement("BIG");
							                var s = document.createElement("STRONG");
							                s.setAttribute("class", "alert-message");
							                var t = document.createTextNode("<%tcWebApi_get("String_Entry","EasyMeshRunTime4Text","s")%>");
							                s.appendChild(t);
							                b.appendChild(s);
							                div.appendChild(b);
							            }
							        }
							        else
							        {
							            btn.innerHTML = '<%tcWebApi_get("String_Entry","EasyMeshRunTime2Text","s")%>';
							            div.style.display = "none";
							        }
							    }
							    
							    function get_dev_role_string(devRole)
							    {
							        switch(parseInt(devRole))
							        {
							            case 1:
							                return 'Controller';
							            case 2:
							                return 'Agent';
							            default:
							                return 'Not Configured';
							        }
							    }
							    function get_version_string(version)
							    {
							        switch(version)
							        {
							            case "01":
							                return 'R1';
							            case "02":
							                return 'R2';
							            default:
							                return 'Not Configured';
							        }
							    }
							    function get_run_time_topology_submit()
							    {
							    		location.href = '/cgi-bin/wifi_map_runtime_topology.asp';
							    }
							    
							    function get_run_time_topology_cb(rsp)
							    {
							        try
							        {
							            var r = JSON.parse(rsp);
							            if(r.status == "SUCCESS")
							            {
							            		var tmp_jsTopoInfo = r.luaTopologyInfo;
							            		tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\u0022/g, '\"');
							            		tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\\\u0022/g, '\"');
         											tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/&sqm;/g, "'");
         											tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/&lt;/g, "&amp;lt");
         											tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/&rt;/g, "&amp;rt");
							                var jsTopoInfo = JSON.parse(tmp_jsTopoInfo);
							                document.getElementById("display_all_1905_dev_info_div").innerHTML = "";
							                document.getElementById("display_tree_vis").innerHTML = "";
							                draw_topology_table(jsTopoInfo);
							            }
							            else
							            {
							                console.log("Failed to get Topology Info!\nStatus: ",r.status);
							            }
							            setTimeout(get_run_time_topology_submit, 30000);
							        }
							        catch(e)
							        {
							            console.log("Incorrect response! Failed to get Topology Info!",e.name,e.message);
							            setTimeout(get_run_time_topology_submit, 30000);
							        }
							    }
							    
									function get_run_time_topology()
							    {
							        document.getElementById("topoErrMsgDiv").style.display = "none";
							        document.getElementById("display_all_1905_dev_info_div").style.display = "none";
							        document.getElementById("display_tree_vis").style.display = "none";
							        document.getElementById("toggle_disp_tree_vis_detail_btn").disabled = true;
							        document.getElementById("toggle_all_1905_dev_info_btn").disabled = true;
							        document.getElementById("toggle_all_1905_dev_info_btn").innerHTML = '<%tcWebApi_get("String_Entry","EasyMeshRunTime2Text","s")%>';
							        document.getElementById("getTopoInfoMsgDiv").style.display = "";
							        
							        var x_response = JSO_get_runtime_topology();
							        document.getElementById('getTopoInfoMsgDiv').style.display = 'none';
							        get_run_time_topology_cb(x_response);
							        document.getElementById("toggle_disp_tree_vis_detail_btn").disabled = false;
							        document.getElementById("toggle_all_1905_dev_info_btn").disabled = false;
							    }
							    
							    function  LoadFrame()
								{
										
								}
									
								get_run_time_topology();
								get_channel_score();
								open_check() 
								function JSO_get_channel_topology()
								{
										var ret = "<% tcWebApi_MeshJsonHook("mesh_get_de_dump") %>";
										ret = ret.replace(/\'/g,'\\\\u0022');
										ret = ret.replace(/\\\\u0022Pass-phrase\\\\u0022:/g,'\\\\u0022WPAPSK\\\\u0022:');
										return ret;
								}
								function get_channel_score()
							    {
							        document.getElementById("display_all_1905_channel_info_div").style.display = "none";
							        
							        var x_response = JSO_get_channel_topology();
							        document.getElementById('getTopoInfoMsgDiv').style.display = 'none';
							        get_run_time_channel_cb(x_response);
							    }
								function get_run_time_channel_cb(rsp)
							    {
							        try
							        {
							            var r = JSON.parse(rsp);
							            if(r.status == "SUCCESS")
							            {
											var tmp_jsTopoInfo = r.deDumpResult;
											tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\u0022/g, '\"');
											tmp_jsTopoInfo = tmp_jsTopoInfo.replace(/\\\\u0022/g, '\"');
							                var jsTopoInfo = JSON.parse(tmp_jsTopoInfo);
							                document.getElementById("display_all_1905_channel_info_div").innerHTML = "";
							                draw_channel_table(jsTopoInfo);
							            }
							            else
							            {
							                console.log("Failed to get Topology Info!\nStatus: ",r.status);
							            }
							            setTimeout(get_run_time_topology_submit, 15000);
							        }
							        catch(e)
							        {
							            console.log("Incorrect response! Failed to get Topology Info!",e.name,e.message);
							            setTimeout(get_run_time_topology_submit, 15000);
							        }
							    }
								function draw_channel_table(jsTopoInfo)
							    {
							        var tree_info = [];
							        var tree_info_count = 0;
							        var topoInfoArr = jsTopoInfo['Data Elements Information'];
							        if(!(topoInfoArr instanceof Array))
							        {
							            console.log("Incorrect TopologyInfo: Value of topology information is not an Array!");
							            return;
							        }
							        
							        for(var idx_1905=0; idx_1905 < topoInfoArr.length; idx_1905++)
							        {
							            var obj_1905 = topoInfoArr[idx_1905];
							            if(!obj_1905.hasOwnProperty('AL MAC') || ("00:00:00:00:00:00" == obj_1905['AL MAC']) || (al_mac != obj_1905['AL MAC']))
							            {
							            		continue;
							            }
							            //obj_1905 = prep_bssinfo(obj_1905);
							            
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
							            document.getElementById("display_all_1905_channel_info_div").appendChild(dev1905fset);							            
							            set_max_cellspan(obj_1905);
							            obj_loop(dev1905tbl, null, obj_1905);							            
							            document.getElementById("display_all_1905_channel_info_div").appendChild(dev1905tbl);
							            tree_info_count++;
							        }
							    }
								function open_check()
								{
									var open_check = <%TCWebApi_get("Mesh_action", "trigger_de_dump_is_open","s")%>
									if(open_check == "1")
									{
										document.getElementById("display_all_1905_channel_info_div").style.display = "";
									}
									else
									{
										document.getElementById("display_all_1905_channel_info_div").style.display = "none";
									}									
								}
								function close_channel_div()
								{
									document.getElementById("display_all_1905_channel_info_div").style.display = "none";																		
								}
								
							</script>
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
						<P align=center></P>
					</TD>
					<TD width=170 bgColor=#313031>　</TD>
				</TR>
				</TBODY>
			</TABLE>
		</TD>
	</TR>
	<SCRIPT language=JavaScript type=text/javascript>
		function formSubmit(saveAll)
		{
			if(document.cbi != null)
			{						
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
		</SCRIPT>
	</TBODY>
</TABLE>
</BODY>
</HTML>