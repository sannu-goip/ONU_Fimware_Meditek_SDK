<%
If request_Form("VoIP_h248_flag")="1" then
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
		if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then
			tcWebApi_set("VoIPBasic_Common", "SIPProtocol", "VoIPMode")
			tcWebApi_commit("VoIPBasic")
		End if
		tcWebApi_set("VoIPH248_Common", "DeviceIDType", "mgidselect")
		tcWebApi_set("VoIPH248_Common", "DeviceID", "mymgname")
		tcWebApi_set("VoIPH248_Common", "MediaGatewayPort", "mymgport")
		tcWebApi_set("VoIPH248_Common", "MediaGatewayControler", "mymgcip")
		tcWebApi_set("VoIPH248_Common", "MediaGatewayControlerPort", "mymgcport")
		tcWebApi_set("VoIPH248_Common", "SBMediaGatewayControler", "mybkmgcip")
		tcWebApi_set("VoIPH248_Common", "SBMediaGatewayControlerPort", "mybkmgcport")
		tcWebApi_set("VoIPH248_Common", "RTPPrefix", "mgRTPPrefix")
		tcWebApi_set("VoIPH248_Common", "EphemeralTermIDAddLen", "mgEpTermIDLen")
		tcWebApi_set("VoIPH248_Common", "EphemeralTermIDUniform", "mgEpTermIDUniform")
		tcWebApi_set("VoIPH248_Common", "EphemeralTermIDStart", "mgEpTermIDStart")
		tcWebApi_set("VoIPH248_Common", "EphemeralTermIDNum", "mgEpTermIDNum")
		tcWebApi_set("VoIPH248_Common", "MegacoSignalDSCP", "mymgtos")
		tcWebApi_set("VoIPH248_Common", "MegacoRTPDSCP", "mymgtos2")
		tcWebApi_set("VoIPH248_Entry0", "PhysicalTermID", "mymgterm0")
		tcWebApi_set("VoIPH248_Common", "PhysicalTermIDPrefix", "termidPrefix")
		tcWebApi_set("VoIPH248_Common", "PhysicalTermIDConfigMethod", "termidMethod")
		tcWebApi_set("VoIPH248_Common", "PhysicalTermIDStart", "termidStart")
			
		tcWebApi_set("VoIPH248_Entry1", "PhysicalTermID", "mymgterm1")
		tcWebApi_set("VoIPH248_Entry2", "PhysicalTermID", "mymgterm2")
		tcWebApi_set("VoIPH248_Entry3", "PhysicalTermID", "mymgterm3")
		tcWebApi_commit("VoIPH248_Common")
		tcWebApi_commit("VoIPH248")
		tcWebApi_save()
	end if
end if
%>
<HTML><HEAD><TITLE><%tcWebApi_get("String_Entry","VoipStep1Text","s")%></TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
	href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<style>
div#perLineTablePlaceholder2{
	width: 100%;
	}
table#perLineTable1 {
	width: 100%;
	table-layout: fixed;
	}
table#perLineTable1 td{
	overflow: hidden;
	} 
</style>
<SCRIPT language="javascript" type = "text/javascript">
	var voipinfo1 = '<%tcWebApi_get("String_Entry","VoipStep2Text","s")%>';
<%
If request_Form("VoIP_h248_flag")="1" then
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") <> "1"  then
		asp_write("alert(voipinfo1);")
	end if
end if
%>
function LoadFrame()
{
	with (document.ConfigForm)
	{

		generatePerLineTable1();
		
		if ("0" == document.getElementById("termid_method").selectedIndex){
			document.getElementById("termidPrefix_table").style.display = "";
			document.getElementById("termidStart_table").style.display = "";
			document.getElementById("perLineTablePlaceholder2").style.display = "none";
		}
		else{
			document.getElementById("termidPrefix_table").style.display = "none";
			document.getElementById("termidStart_table").style.display = "none";
			document.getElementById("perLineTablePlaceholder2").style.display = "";
		}
	}
	if ( "0" == document.getElementById("mgidtype").selectedIndex){
		document.getElementById("in_h_domain1_val").style.display = "none";
	}
	else{
		document.getElementById("in_h_domain1_val").style.display = "";
	}
}

function generatePerLineTable1()
{
	if (document.body.innerHTML)
	{
		var tableHtmlOutput = "<tr>";
		var lineNumber = parseInt('<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>');
		var len=lineNumber + 1;
		tableHtmlOutput += "<td>";
		tableHtmlOutput += "<table id='perLineTable1'  border='1' cellpadding='0'  bordercolor='#C0C0C0' >";
		tableHtmlOutput += "<tr align='left'>";
		tableHtmlOutput += "<td height='25' colspan="+len+"><strong id='in_h_resource1'><%tcWebApi_get("String_Entry","VoipStep66Text","s")%></strong></td>";
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr align='center'>";
		tableHtmlOutput += "<td bgcolor='eeeeee'><%tcWebApi_get("String_Entry","VoipStep67Text","s")%></td>"
	
	
		var data = "<td height='25' bgcolor='#EEEEEE' id=\'in_h_line1\'><%tcWebApi_get("String_Entry","VoipStep68Text","s")%></td>";
		tableHtmlOutput += data;

		if (lineNumber > 1)
		{
			var data = "<td  bgcolor='#EEEEEE' id=\'in_h_line2\'><%tcWebApi_get("String_Entry","VoipStep69Text","s")%></td>";
			tableHtmlOutput += data;  
		}
		else
		{
			var data = "<td  style=display:none  bgcolor='#EEEEEE' id=\'in_h_line2\'><%tcWebApi_get("String_Entry","VoipStep69Text","s")%></td>";
			tableHtmlOutput += data;
		}
		if (lineNumber > 2)
		{
			var data = "<td  bgcolor='#EEEEEE' id=\'in_h_line3\'><%tcWebApi_get("String_Entry","VoipStep70Text","s")%></td>";
			tableHtmlOutput += data;
		}
		else
		{
			var data = "<td style=display:none bgcolor='#EEEEEE' id=\'in_h_line3\'><%tcWebApi_get("String_Entry","VoipStep70Text","s")%></td>";
			tableHtmlOutput += data;
		}
		if (lineNumber > 3)
		{
			var data = "<td  bgcolor='#EEEEEE' id=\'in_h_line4\'><%tcWebApi_get("String_Entry","VoipStep71Text","s")%></td>";
			tableHtmlOutput += data;
		}
		else
		{
			var data = "<td style=display:none  bgcolor='#EEEEEE' id=\'in_h_line4\'><%tcWebApi_get("String_Entry","VoipStep71Text","s")%></td>";
			tableHtmlOutput += data;
		}
	
		tableHtmlOutput += "</tr>";
		tableHtmlOutput += "<tr align='center'>";
		tableHtmlOutput += "<td height='25' bgcolor='eeeeee' id='in_h_term1'><%tcWebApi_get("String_Entry","VoipStep72Text","s")%></td>";
		
		var data = "<input name=\'mymgterm0\' type='text' id=\'in_h_term1_val0\' value=\'<%if tcWebApi_get("VoIPH248_Entry0", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry0", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
		tableHtmlOutput += "<td height='25'>" + data + "</td>";
		if (lineNumber > 1)
		{
			var data = "<input name=\'mymgterm1\' type='text' id=\'in_h_term1_val1\' value=\'<%if tcWebApi_get("VoIPH248_Entry1", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry1", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td height='25'>" + data + "</td>";
		}
		else
		{
			var data = "<input name=\'mymgterm1\'  type='text' id=\'in_h_term1_val1\' value=\'<%if tcWebApi_get("VoIPH248_Entry1", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry1", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td style=display:none height='25'>" + data + "</td>"; 
		}
		if (lineNumber > 2)
		{
			var data = "<input name=\'mymgterm2\' type='text' id=\'in_h_term1_val2\' value=\'<%if tcWebApi_get("VoIPH248_Entry2", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry2", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td  height='25'>" + data + "</td>";
		}
		else
		{
			 var data = "<input name=\'mymgterm2\'  type='text' id=\'in_h_term1_val2\' value=\'<%if tcWebApi_get("VoIPH248_Entry2", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry2", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
			 tableHtmlOutput += "<td style=display:none height='25'>" + data + "</td>"; 
		}
		if (lineNumber > 3)
		{
			 var data = "<input name=\'mymgterm3\' type='text' id=\'in_h_term1_val3\' value=\'<%if tcWebApi_get("VoIPH248_Entry3", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry3", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
			 tableHtmlOutput += "<td height='25'>" + data + "</td>";
		}
		else
		{
			var data = "<input name=\'mymgterm3\' type='text' id=\'in_h_term1_val3\' value=\'<%if tcWebApi_get("VoIPH248_Entry3", "PhysicalTermID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Entry3", "PhysicalTermID", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td style=display:none  height='25'>" + data + "</td>";  
		}
		tableHtmlOutput += "</tr>";
		tableHtmlOutput += "</table>";
		tableHtmlOutput += "</td>";
		tableHtmlOutput += "</tr>";  
		
		document.getElementById("perLineTablePlaceholder2").innerHTML = tableHtmlOutput;
	}
	else
	{
		alert("Cannot create per line table in this browser.");
	}
}
	
function applyClickAdvanced()
{
	var loc = "app-VoIP248_Adv.asp";
	var code = "location='" + loc + " '";
	eval(code);
}


function isValidName(name) 
{
	var i = 0;
	var unsafeString = "\"<>%\\^[]`\+\$\,='#&:;*/{} \t";
	for ( i = 0; i < name.length; i++ )
	{
		for( j = 0; j < unsafeString.length; j++)
			if ( (name.charAt(i)) == unsafeString.charAt(j) )
			return false;
	}
	for (k = 0; k < name.length; k++)
	{
		if (name.charAt(k) == '.')
		{
			break;
		}
	}
	if (k >= name.length - 1 || k == 0)
		return false;
}
function isValidIPV6(str)
{
	var pattern = new RegExp("^((([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:)|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}(:[0-9A-Fa-f]{1,4}){1,2})|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){1,3})|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){1,4})|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){1,5})|([0-9A-Fa-f]{1,4}:(:[0-9A-Fa-f]{1,4}){1,6})|(:(:[0-9A-Fa-f]{1,4}){1,7})|(([0-9A-Fa-f]{1,4}:){6}(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){5}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|([0-9A-Fa-f]{1,4}:(:[0-9A-Fa-f]{1,4}){0,4}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3})|(:(:[0-9A-Fa-f]{1,4}){0,5}:(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3}))$");
	return pattern.test(str);
}

function VoiceApplyBasic()
{
	with (document.ConfigForm)
	{

		if (mymgname.value != "")
		{
			if (mgidselect.selectedIndex == "2")
			{
				if (isValidMacAddress(mymgname.value) == false)
				{
					alert('"' + mymgname.value + '"<%tcWebApi_get("String_Entry","VoipStep73Text","s")%>');
					return;
				}
			}
		}


		if((limitNum(document.getElementById("in_h_eptermidlen_val"))==false) 
		|| (limitNum(document.getElementById("in_h_eptermidstart_val"))==false)
		|| (limitNum(document.getElementById("in_h_eptermidnum_val"))==false)
		|| (limitNum(document.getElementById("in_h_tos1_val"))==false)
		|| (limitNum(document.getElementById("in_h_tos2_val"))==false))
			return;
		
		if (mgRTPPrefix.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep74Text","s")%>');
			return;
		}
			
		var temp = parseInt(mymgtos.value);
		if (temp < 0 || temp > 63)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep75Text","s")%>');
			return;
		}
		
		var temp = parseInt(mymgtos2.value);
		if (temp < 0 || temp > 63)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep76Text","s")%>');
			return;
		}
		
		if (mymgport.value != "")
		{
			if(checkPort(document.getElementById("in_h_mg1_val"))==false)
				return;
		}
		
		if (mymgcip.value != "")
		{
			if(mymgcip.value.match(":"))
			{
				var result = isValidIPV6(mymgcip.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStepseripText","s")%>'+'"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
				}
			}else if (isValidName(mymgcip.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStepseripText","s")%>'+'"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
			
		}else
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep78Text","s")%>');				
			return;

		}
	
		if (mybkmgcip.value != "")
		{	
			if(mybkmgcip.value.match(":"))
			{
				var result = isValidIPV6(mybkmgcip.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStepseripText","s")%>'+'"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
				}
			}else if (isValidName(mybkmgcip.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStepsecseripText","s")%>'+'"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		


		if (mymgcport.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep77Text","s")%>');
			return;
		}
		else
		{
			if(checkPort(document.getElementById("in_h_standbynum1_val"))==false)			
				return;
		}

		if (mybkmgcport.value != "")
		{
			if(checkPort(document.getElementById("in_h_standbynum1_val"))==false)			
				return;
		}
		
		VoIP_h248_flag.value = 1;
		var voipType = "H.248";
		<% if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") <> "H.248"  then %>
			location.replace("/cgi-bin/app-VoIP.asp");
		<% end if %>
		if( true == setEBooValueCookie(document.ConfigForm) )
			submit();
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
function limitNum(o)
{
	if (o.value != "")
	{
		if (!isInteger(o.value) )
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep81Text","s")%>.');
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}
		else
			return true;
	}
}
function isValidPort(port)
{
	if (!isInteger(port) || port < 0 || port > 65535)
	{
		return false;
	}
	
	return true;
}
function checkPort(o)
{
	if (o.value != "")
	{
		if (!isInteger(o.value) || !isValidPort(o.value))
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep82Text","s")%>'); 
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}
		else
		{
			return true;
		}
	}
}

function mgIdSelectChange(o){
	var idx = o.selectedIndex;
	var val = o.options[idx].value;

	if ( val == "0" ){
		document.getElementById("in_h_domain1_val").style.display = "none";
	}
	else{
		document.getElementById("in_h_domain1_val").style.display = "";
	}
}

function termidMethodSelectChange(o){
	var idx = o.selectedIndex;
	var val = o.options[idx].value;

	if ( val == "0" ){
		document.getElementById("termidPrefix_table").style.display = "";
		document.getElementById("termidStart_table").style.display = "";
		
		document.getElementById("perLineTablePlaceholder2").style.display = "none";
	}
	else{
		document.getElementById("termidPrefix_table").style.display = "none";
		document.getElementById("termidStart_table").style.display = "none";
		
		document.getElementById("perLineTablePlaceholder2").style.display = "";
	}
}

<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
function voipTypeChg(){
	with (document.ConfigForm)
	{
		if(VoIPMode.options[VoIPMode.selectedIndex].value != "H.248")
		{
			location.replace('/cgi-bin/app-VoIP.asp'+'?voiptype='+VoIPMode.options[VoIPMode.selectedIndex].value);
		}
	}
}
<%end if%>

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
											<TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","WelcomeSettingText","s")%> </TD>
											<TD vAlign=bottom width=50><A onclick=DoLogout() 
											href="/cgi-bin/logout.cgi" id=logOut name=logOut target=_top><SPAN 
											class=logout><%tcWebApi_get("String_Entry","LogoutText","s")%> 
						</SPAN></A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
		<TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 border=0>
			<TBODY>
				<TR>
					<TD class=LocationDisplay id=LocationDisplay align=middle width=163 
						bgColor=#ef8218 rowSpan=3></TD>
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
					<TD id=MenuArea_L2 bgColor=#427594 colSpan=2 
		height=24></TD></TR></TBODY></TABLE>
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
								<TD width=7 bgColor=#ef8218> </TD>
								<TD width=474> </TD>
								<TD vAlign=top width=170 background=/img/panel4.gif rowSpan=13>
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
								<TD width=7 background=/img/panel3.gif>　</TD>
							<TD></TD></TR>
							<TR>
								<TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
								<P class=Item_L2></P></TD>
								<TD width=7 background=/img/panel3.gif>　</TD>
								<TD>
									<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
										<TBODY>
											<TR>
												<TD width=10>&nbsp;</TD>
												<TD>
													<FORM id=ConfigForm name="ConfigForm" action="/cgi-bin/app-VoIP248.asp" method="post">
														<DIV id=pptp>
															<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
																<TBODY>
																	<TR>
																		<TD>
																			<input type='button' id=applyClickAdv name=applyClickAdv style="font-size:100%;width:170;height:30" onClick="applyClickAdvanced()" value='<%tcWebApi_get("String_Entry","VoipStep84Text","s")%>'>
																		</TD>
																	</TR>
																	<TR>
																		<TD height="10">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_head>
																		<B>
																			<%tcWebApi_get("String_Entry","VoipStep85Text","s")%>
																		</B>
																		</TD>
																	</TR>
																	<TR>
																		<TD height="10">
																		</TD>
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
															<TABLE class=table1_bg border=0 cellSpacing=1 cellPadding=0 width="98%">
																<TBODY>
																	<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
																		<TR>
																			<TD class="table1_left" width="100"><%tcWebApi_get("String_Entry","VoipStep46Text","s")%></td>
																			<TD class=table1_right>
																				<select onchange="voipTypeChg()" id="VoIPMode" name="VoIPMode">
																					<option value="IMSSIP">IMSSIP</option>
																					<option value="SIP" >SIP</option>
																					<option value="H.248" selected>H.248</option>
																				</select>
																				<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
																					<%tcWebApi_get("String_Entry","VoipStep47Text","s")%>
																				<%end if%>
																			 </TD>
																		</TR>
																	<% end if %>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep86Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																				<select name="mgregselect" id="mg_regist" onchange="" disabled=true>
																					<option value="0" <%if tcWebApi_get("VoIPH248_Common", "DeviceRegType", "h") = "0" then asp_Write("selected") end if %>>ROOT</option>
																					<option value="1" <%if tcWebApi_get("VoIPH248_Common", "DeviceRegType", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep87Text","s")%></option>
																				</select>
																		</TD>
																	</TR> 
																	<TR>
																		<TD class=table1_left width="35%" align=left>
																			<%tcWebApi_get("String_Entry","VoipStep88Text","s")%>
																		</TD>
																		<TD class=table1_right width="65%" colSpan=2 align=left>
																				<select name="mgidselect" id="mgidtype" onchange="mgIdSelectChange(this)">
																					<option value="0" <%if tcWebApi_get("VoIPH248_Common", "DeviceIDType", "h") = "0" then asp_Write("selected") end if %>>IP</option>
																					<option value="1" <%if tcWebApi_get("VoIPH248_Common", "DeviceIDType", "h") = "1" then asp_Write("selected") end if %>>Domain name</option> 
																					<option value="2" <%if tcWebApi_get("VoIPH248_Common", "DeviceIDType", "h") = "2" then asp_Write("selected") end if %>>MAC</option>
																				</select>
																				<input name="mymgname" type="text" id="in_h_domain1_val" value="<%if tcWebApi_get("VoIPH248_Common", "DeviceID", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "DeviceID", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep89Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>  
																			<input name="mymgport" type="text" id="in_h_mg1_val" onchange="checkPort(this)" value="<%if tcWebApi_get("VoIPH248_Common", "MediaGatewayPort", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "MediaGatewayPort", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep90Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mymgcip" type="text" id="in_h_servaddr1_val" value="<%if tcWebApi_get("VoIPH248_Common", "MediaGatewayControler", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "MediaGatewayControler", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep91Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mymgcport" type="text" id="in_h_servnum1_val" onchange="checkPort(this)" title=an_tos2 value="<%if tcWebApi_get("VoIPH248_Common", "MediaGatewayControlerPort", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "MediaGatewayControlerPort", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep92Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mybkmgcip" type="text" id="in_h_standbyaddr1_val" value="<%if tcWebApi_get("VoIPH248_Common", "SBMediaGatewayControler", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "SBMediaGatewayControler", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep93Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mybkmgcport" type="text" id="in_h_standbynum1_val" onchange="checkPort(this)" value="<%if tcWebApi_get("VoIPH248_Common", "SBMediaGatewayControlerPort", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "SBMediaGatewayControlerPort", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep94Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mgRTPPrefix" type="text" id="in_h_rtpprefix_val" value="<%if tcWebApi_get("VoIPH248_Common", "RTPPrefix", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "RTPPrefix", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep95Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mgEpTermIDLen" type="text" id="in_h_eptermidlen_val" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "EphemeralTermIDAddLen", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "EphemeralTermIDAddLen", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep96Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mgEpTermIDStart" type="text" id="in_h_eptermidstart_val" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "EphemeralTermIDStart", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "EphemeralTermIDStart", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep97Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mgEpTermIDNum" type="text" id="in_h_eptermidnum_val" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "EphemeralTermIDNum", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "EphemeralTermIDNum", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left width="35%" align=left>
																			<%tcWebApi_get("String_Entry","VoipStep98Text","s")%>
																		</TD>
																		<TD class=table1_right width="65%" colSpan=2 align=left>
																			<select name="mgEpTermIDUniform" id="mg_uniform" >
																				<option value="0" <%if tcWebApi_get("VoIPH248_Common", "EphemeralTermIDUniform", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep99Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPH248_Common", "EphemeralTermIDUniform", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep100Text","s")%></option>
																			</select>
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep101Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mymgtos" type="text" id="in_h_tos1_val" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "MegacoSignalDSCP", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "MegacoSignalDSCP", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep102Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="mymgtos2" type="text" id="in_h_tos2_val" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPH248_Common", "MegacoRTPDSCP", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "MegacoRTPDSCP", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_left width="35%" align=left>
																			<%tcWebApi_get("String_Entry","VoipStep103Text","s")%>
																		</TD>
																		<TD class=table1_right width="65%" colSpan=2 align=left>
																			<select name="termidMethod" id="termid_method" onchange="termidMethodSelectChange(this)">
																				<option value="0" <%if tcWebApi_get("VoIPH248_Common", "PhysicalTermIDConfigMethod", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep104Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPH248_Common", "PhysicalTermIDConfigMethod", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep105Text","s")%></option>
																			</select>
																			
																		</TD>
																	</TR>
																	<TR id="termidPrefix_table">
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep106Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="termidPrefix" type="text" id="in_h_termidPrefix_val" value="<%if tcWebApi_get("VoIPH248_Common", "PhysicalTermIDPrefix", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "PhysicalTermIDPrefix", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR id="termidStart_table">
																		<TD class=table1_left align=left>
																			<%tcWebApi_get("String_Entry","VoipStep107Text","s")%>
																		</TD>
																		<TD class=table1_right colSpan=2 align=left>
																			<input name="termidStart" type="text" id="in_h_termidStart_val" value="<%if tcWebApi_get("VoIPH248_Common", "PhysicalTermIDStart", "h") <> "N/A" then tcWebAPI_get("VoIPH248_Common", "PhysicalTermIDStart", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																</TBODY>
															</TABLE>
															<TABLE border=0 cellSpacing=1 cellPadding=0 width="98%">
																<tr>
																	<td height="5">
																	</td>
																</tr>
																<tr>
																	<td><div class="tableWrapper" id="perLineTablePlaceholder2" ></div></td>
																</tr> 
																<tr>
																<td height="5">
																</td>
																</tr>
																<tr>
																	<td align="center">
																		<input name="VoIP_h248_flag" type="hidden" value="0">
																		<input name="bt_apply" type="button"  id="VoiceApplyButton"  value="<%tcWebApi_get("String_Entry","VoipStep65Text","s")%>" onClick="VoiceApplyBasic()">
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
								<TD width=7 background=/img/panel3.gif>　</TD>
							<TD>&nbsp;</TD></TR>
							<TR>
								<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
								<TD width=7 background=/img/panel3.gif>　</TD>
							<TD></TD></TR>
							<TR>
								<TD vAlign=top width=157 bgColor=#e7e7e7></TD>
								<TD width=7 background=/img/panel3.gif>	</TD>
					<TD></TD></TR></TBODY></TABLE>
			</TD></TR>
			<TR>
				<TD height=1>
					<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
					border=0><TBODY>
					<TR>
						<TD width=162 bgColor=#ef8218>	</TD>
						<TD width=278 bgColor=#427594>	</TD>
						<TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
						<TD width=170 bgColor=#313031>	</TD>
					</TR>
</TBODY></TABLE></TD></TR></TBODY></TABLE></BODY></HTML>
