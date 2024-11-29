<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>EasyMesh AP Capabilities</TITLE>
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
						<p class="cbi-tab" id="p-item-title" ><%tcWebApi_get("String_Entry","EasyMeshApCapabilitiesText","s")%></p>
					</TD>
					<TD vAlign=top width=7 background="/img/panel3.gif" height=30>
						<p class="cbi-tab" id="p-item-title-pel" ></p>
					</TD>
					<TD vAlign=top width=474 rowspan="2">
						<div id="maincontent" class="container">
								<div width=470 id="ap_db_display"></div>
								<script type="text/javascript">
										
										var max_cellspan = 2;
										
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
							    
										function draw_ap_db_table(jsTopoInfo)
										{
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
										      	
										      	var ap_db_tbl = document.createElement("TABLE");
										        ap_db_tbl.setAttribute("height", "100%");
										        ap_db_tbl.setAttribute("cellSpacing", "0");
										        ap_db_tbl.setAttribute("cellPadding", "0");
										        ap_db_tbl.setAttribute("border", "1");
										        
										        var ap_db = {};
										        ap_db["Radio Info"] = topoInfoArr[idx_1905]['Radio Info'];
										        
										        var dev1905div = document.createElement("div");
										        dev1905div.setAttribute("class", "table1_head");
										        var dev1905fb = document.createElement("b");
										        var dev1905a = document.createElement("A");
										        var dev1905text = document.createTextNode("Radio Info of 1905 Device - " + (idx_1905 + 1));
										        dev1905a.setAttribute("href", "#");
										        dev1905a.appendChild(dev1905text);
										        dev1905fb.appendChild(dev1905a);
										        dev1905div.appendChild(dev1905fb);
										        document.getElementById("ap_db_display").appendChild(dev1905div);
										        
										        set_max_cellspan(ap_db);
										        obj_loop(ap_db_tbl, null, ap_db);
										        
										        document.getElementById("ap_db_display").appendChild(ap_db_tbl);
										    }
										}
										
										function checkWebStatus()
										{
												location.href = '/cgi-bin/wifi_map_ap_capabilities.asp';
										}
										
										function get_ap_db_cb(rsp)
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
										            document.getElementById("ap_db_display").innerHTML = "";
										            draw_ap_db_table(jsTopoInfo);
										        }
										        else
										        {
										            console.log("Failed to get AP DB!\nStatus: ",r.status);
										        }
										        setTimeout(checkWebStatus, 10000);
										    }
										    catch(e)
										    {
										        console.log("Incorrect response! Failed to get AP DB!",e.name,e.message);
										        setTimeout(checkWebStatus, 10000);
										    }
										}
										
										function get_ap_db()
										{
										  	document.getElementById("ap_db_display").innerHTML = "";
										  	var x_response = JSO_get_runtime_topology();
										  	get_ap_db_cb(x_response);
										}
										
										function LoadFrame()
										{
										
										}
										
										get_ap_db();
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
	</TBODY>
</TABLE>
</BODY>
</HTML>
