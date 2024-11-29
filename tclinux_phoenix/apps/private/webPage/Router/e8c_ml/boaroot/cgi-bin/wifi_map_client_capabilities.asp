<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>EasyMesh Client Capabilities</TITLE>
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
						<p class="cbi-tab" id="p-item-title" ><%tcWebApi_get("String_Entry","EasyMeshClientCapabilitiesText","s")%></p>
					</TD>
					<TD vAlign=top width=7 background="/img/panel3.gif" height=30>
						<p class="cbi-tab" id="p-item-title-pel" ></p>
					</TD>
					<TD vAlign=top width=474 rowspan="2">
						<div id="maincontent" class="container">

							<div id="cli_db_display" style="overflow-x: scroll; width:465"></div>

							<script type="text/javascript">
									function JSO_get_client_capabilities()
									{
											var ret = "<% tcWebApi_MeshJsonHook("mesh_get_client_capabilities") %>";
											return ret;
									}
									
									function draw_cli_db_table(g_cli_db)
									{
									    var disp_cli_idx = 1;
									    var cli_db = g_cli_db['cli_db'];
									    var cdb_tbl = document.createElement("TABLE");
									    cdb_tbl.setAttribute("class", "cbi-section-table-scroll");
									    var row, cell, th, th_txt;
									    row = cdb_tbl.insertRow(-1);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("No.");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("MAC");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("BSSID");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Capability");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Phy Mode");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Max. BW 0");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Max. BW 1");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Spatial Streams");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Supported Bands");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    th = document.createElement("TH");
									    th_txt = document.createTextNode("Supported Channels");
									    th.appendChild(th_txt);
									    row.appendChild(th);
									    
									    for(var cli_idx = 0; cli_idx < parseInt(g_cli_db['num_clis']); cli_idx++)
									    {
									        if(cli_db[cli_idx]['BSSID'] == '00:00:00:00:00:00')
									        {
									            continue;
									        }
									        row = cdb_tbl.insertRow(-1);
									        cell = row.insertCell(-1);
									        cell.innerHTML = disp_cli_idx++;
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['MAC'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['BSSID'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Capability'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Phy Mode'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Max. Bandwidth 0'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Max. Bandwidth 1'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Spatial Stream'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Known Band'];
									        cell = row.insertCell(-1);
									        cell.innerHTML = cli_db[cli_idx]['Known Channels'];
									    }
									    if(cdb_tbl.rows.length == 1)
									    {
									        row = cdb_tbl.insertRow(-1);
									        cell = row.insertCell(-1);
									        cell.colSpan = cdb_tbl.rows[0].cells.length;
									        cell.style.textAlign = "center";
									        cell.innerHTML = "Client DB is empty!";
									    }
									    document.getElementById("cli_db_display").appendChild(cdb_tbl);
									}
									
									function checkWebStatus()
									{
											location.href = '/cgi-bin/wifi_map_client_capabilities.asp';
									}
									
									function get_client_db_cb(rsp)
									{
									    try
									    {
									        var r = JSON.parse(rsp);
									        if(r.status == "SUCCESS")
									        {
									            document.getElementById("cli_db_display").innerHTML = "";
									            draw_cli_db_table(r);
									        }
									        else
									        {
									            console.log("Failed to get Client DB!\nStatus: ",r.status);
									        }
									        setTimeout(checkWebStatus, 10000);
									    }
									    catch(e)
									    {
									        console.log("Incorrect response! Failed to get Client DB!",e.name,e.message);
									        setTimeout(checkWebStatus, 10000);
									    }
									}
									
									function get_client_db()
									{
											document.getElementById("cli_db_display").innerHTML = "";
									    var x_response = JSO_get_client_capabilities();
									    get_client_db_cb(x_response);
									}
									
									function  LoadFrame()
									{
									
									}
									
									get_client_db();
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
