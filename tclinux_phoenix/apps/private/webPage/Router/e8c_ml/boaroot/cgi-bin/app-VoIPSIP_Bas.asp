<%
If request_Form("VoIP_basic_flag")="1" then
	tcWebApi_constSet("WebCurSet_Entry", "VoIPIdle", "1")
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
		tcWebApi_set("VoIPBasic_Common", "SIPProtocol", "sipMode")
		tcWebApi_set("VoIPAdvanced_Common", "VoIPRegion", "localeName")
		tcWebApi_set("VoIPAdvanced_Common","CurIFName","ifName")
		tcWebApi_set("VoIPAdvanced_Common","CurIFIndex","curSetIndex")
		tcWebApi_set("VoIPAdvanced_Common", "SIPDomain", "dareSipDomain")
		tcWebApi_set("VoIPBasic_Common", "RegistrarServer", "regAddr")
		tcWebApi_set("VoIPBasic_Common", "RegistrarServerPort", "regPort")
		tcWebApi_set("VoIPBasic_Common", "SBRegistrarServer", "regAddr2")
		tcWebApi_set("VoIPBasic_Common", "SBRegistrarServerPort", "regPort2")	
		tcWebApi_set("VoIPBasic_Entry0", "Enable", "lineEnabled0")	
		tcWebApi_set("VoIPBasic_Entry0", "SIPAuthenticationName", "authName0")	
		tcWebApi_set("VoIPBasic_Entry0", "SIPDisplayName", "authName0")	
		tcWebApi_set("VoIPBasic_Common", "SIPProxyAddr", "proxyAddr")
		tcWebApi_set("VoIPBasic_Common", "SIPProxyPort", "proxyPort")
		tcWebApi_set("VoIPBasic_Common", "SBSIPProxyAddr", "proxyAddr2")
		tcWebApi_set("VoIPBasic_Common", "SBSIPProxyPort", "proxyPort2")
		if tcWebApi_get("WebCustom_Entry", "isCTPONC9Supported", "h") = "Yes"  then
			tcWebApi_set("VoIPBasic_Common", "SIPOutboundProxyEnable", "obProxyEn_flag")
		end if
		tcWebApi_set("VoIPBasic_Common", "SIPOutboundProxyAddr", "obProxyAddr")
		tcWebApi_set("VoIPBasic_Common", "SIPOutboundProxyPort", "obProxyPort")
		tcWebApi_set("VoIPBasic_Common", "SBOutboundProxyAddr", "obProxyAddr2")
		tcWebApi_set("VoIPBasic_Common", "SBOutboundProxyPort", "obProxyPort2")
		
		tcWebApi_set("VoIPBasic_Entry0", "SIPPassword", "password0")	
		
		tcWebApi_set("VoIPBasic_Entry1", "Enable", "lineEnabled1")
		tcWebApi_set("VoIPBasic_Entry1", "SIPAuthenticationName", "authName1")
		tcWebApi_set("VoIPBasic_Entry1", "SIPDisplayName", "authName1")
		tcWebApi_set("VoIPBasic_Entry1", "SIPPassword", "password1")
			
		tcWebApi_set("VoIPBasic_Entry2", "Enable", "lineEnabled2")
		tcWebApi_set("VoIPBasic_Entry2", "SIPAuthenticationName", "authName2")
		tcWebApi_set("VoIPBasic_Entry2", "SIPDisplayName", "authName2")
		tcWebApi_set("VoIPBasic_Entry2", "SIPPassword", "password2")
	
		tcWebApi_set("VoIPBasic_Entry3", "Enable", "lineEnabled3")
		tcWebApi_set("VoIPBasic_Entry3", "SIPAuthenticationName", "authName3")
		tcWebApi_set("VoIPBasic_Entry3", "SIPDisplayName", "authName3")
		tcWebApi_set("VoIPBasic_Entry3", "SIPPassword", "password3")
			
		if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "IMSSIP" then
		tcWebApi_set("VoIPBasic_Entry0", "SIPDisplayName", "extension0")
		tcWebApi_set("VoIPBasic_Entry0", "SIPDisplayNumber", "dispName0")
		If tcWebApi_get("WebCustom_Entry", "isCTSCSupported", "h") <> "Yes"  then
			tcWebApi_constSet("VoIPAdvanced_Common", "SubscribeType", "1")
		End if

		tcWebApi_set("VoIPBasic_Entry1", "SIPDisplayName", "extension1")
		tcWebApi_set("VoIPBasic_Entry1", "SIPDisplayNumber", "dispName1")
	
		tcWebApi_set("VoIPBasic_Entry2", "SIPDisplayName", "extension2")
		tcWebApi_set("VoIPBasic_Entry2", "SIPDisplayNumber", "dispName2")

		tcWebApi_set("VoIPBasic_Entry3", "SIPDisplayName", "extension3")
		tcWebApi_set("VoIPBasic_Entry3", "SIPDisplayNumber", "dispName3")

		end if	
		if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "SIP" then
			If tcWebApi_get("WebCustom_Entry", "isCTSCSupported", "h") <> "Yes"  then
				tcWebApi_constSet("VoIPAdvanced_Common", "SubscribeType", "0")
			End If
		end if	
		tcWebApi_commit("VoIPBasic")
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
div#perLineTablePlaceholder1 {
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
<SCRIPT language=javascript>
	var voipinfo = '<%tcWebApi_get("String_Entry","VoipStep2Text","s")%>';
<%
	If request_Form("VoIP_basic_flag")="1" then
		if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") <> "1"  then
			asp_write("alert(voipinfo);")
		end if
	end if
%>
var voiptype=document.location.search.toString();
var lineNumber = parseInt('<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>');
<% if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "H.248"  then %>
	if(voiptype.slice(10) != "SIP" && voiptype.slice(10) != "IMSSIP")
		location.replace("/cgi-bin/app-VoIP248.asp");
<% end if %>
var sm = "<%if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "SIP" then asp_Write("1") else asp_Write("0") end if %>";
function LoadFrame() {

	generatePerLineTable();
	
	with ( document.ConfigForm )
	{
		<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
			if(voiptype != "")
			{
				selectOption();
			}
		<% end if %>
		enableEditBoxes();
		<% if tcWebApi_get("WebCustom_Entry", "isCTSCSupported", "h") = "Yes" then%>
			extension0.disabled=true;
			dispName0.disabled=true;
			authName0.disabled=true;
			password0.disabled=true;
	
			extension1.disabled=true;
			dispName1.disabled=true;
			authName1.disabled=true;
			password1.disabled=true;
				
			extension2.disabled=true;
			dispName2.disabled=true;
			authName2.disabled=true;
			password2.disabled=true;
		
			extension3.disabled=true;
			dispName3.disabled=true;
			authName3.disabled=true;
			password3.disabled=true;

		<% end if %>
	}

	document.getElementById("regStatus0").value = showRegisterState(1);
	
	document.getElementById("regStatus1").value = showRegisterState(2);
	
	document.getElementById("regStatus2").value = showRegisterState(3);
	
	document.getElementById("regStatus3").value = showRegisterState(4);
	
}
function showRegisterState(port)
{
	var Status = '';
	var LineEnable='';
	var RegFailReason = '';
	
	if ( 1 == port ){
		Status = "<% tcWebAPI_get("InfoVoIP_Entry0", "Status", "s") %>";
		LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry0", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "Enable", "s") else asp_Write("") end if %>';
		RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry0", "RegFailReason", "s") %>";
	}else if ( 2 == port ){
		
		Status = "<% tcWebAPI_get("InfoVoIP_Entry1", "Status", "s") %>";
		LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry1", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "Enable", "s") else asp_Write("") end if %>';
		RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry1", "RegFailReason", "s") %>";
	}

	else if ( 3 == port ){
		Status = "<% tcWebAPI_get("InfoVoIP_Entry2", "Status", "s") %>";
		LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry2", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "Enable", "s") else asp_Write("") end if %>';
		RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry2", "RegFailReason", "s") %>";
	}

	else if ( 4 == port ){
		Status = "<% tcWebAPI_get("InfoVoIP_Entry3", "Status", "s") %>";
		LineEnable = '<%if tcWebAPI_get("VoIPBasic_Entry3", "Enable", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "Enable", "s") else asp_Write("") end if %>';
		RegFailReason = "<% tcWebAPI_get("InfoVoIP_Entry3", "RegFailReason", "s") %>";
	}
	else
		return;
	if(LineEnable == 'No'){
			return '<%tcWebApi_get("String_Entry","VoipStep3Text","s")%>';
	};
	if ( Status == 'Up' )
			return '<%tcWebApi_get("String_Entry","VoipStep4Text","s")%>';
	else if ( Status == 'Initializing' )
			return '<%tcWebApi_get("String_Entry","VoipStep5Text","s")%>';
	else if ( Status == 'Registering' )
			return '<%tcWebApi_get("String_Entry","VoipStep6Text","s")%>';
	else if ( Status == 'Unregistering' )
			return '<%tcWebApi_get("String_Entry","VoipStep7Text","s")%>';
	else if ( Status == 'Quiescent' )
			return '<%tcWebApi_get("String_Entry","VoipStep8Text","s")%>';
	else if ( Status == 'Disabled' )
			return '<%tcWebApi_get("String_Entry","VoipStep3Text","s")%>';
	else if ( Status == 'Error' ){
		switch ( parseInt(RegFailReason) ){
			case 2:
			case 3:
				voiptatus = '<%tcWebApi_get("String_Entry","VoipStep10Text","s")%>';
				break;
			case 4:
				voiptatus = '<%tcWebApi_get("String_Entry","VoipStep11Text","s")%>';
				break;
			case 6:
				voiptatus = '<%tcWebApi_get("String_Entry","VoipStep12Text","s")%>';
				break;
			default:
				voiptatus = '<%tcWebApi_get("String_Entry","VoipStep13Text","s")%>';
				break;
		}
		return voiptatus;
		}
	else if ( Status == 'Testing' )
			return '<%tcWebApi_get("String_Entry","VoipStep14Text","s")%>';
	else
			return '--';
}
	
function generatePerLineTable()
{
	// Generate the table障碍
	if (document.body.innerHTML) {
		var tableHtmlOutput = "<table id='perLineTable1' border='1'><tbody id='perLineTbody1'>";
		var lineNumber = parseInt('<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>');
		// Enable row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td class='hd'><%tcWebApi_get("String_Entry","VoipStep15Text","s")%> <\/td>"
		var selectOptions0 = Array();
		var selectOptions1 = Array();
		var selectOptions2 = Array();
		var selectOptions3 = Array();
		//var pFlag = "off";
		selectOptions0[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry0", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
		//pFlag = "on";
		selectOptions0[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry0", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
		var data = "<select name='lineEnabled0' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions0.join("") + "<\/select>";
		tableHtmlOutput += "<td align=center >" + data + "</td>";

		if (lineNumber > 1){
			//var pFlag = "off";
			selectOptions1[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry1", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
			//pFlag = "on";
			selectOptions1[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry1", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
			var data = "<select name='lineEnabled1' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions1.join("") + "<\/select>";
			tableHtmlOutput += "<td align=center >" + data + "</td>";
		}
		else {
			//var pFlag = "off";
			selectOptions1[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry1", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
			//pFlag = "on";
			selectOptions1[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry1", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
			var data = "<select name='lineEnabled1' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions1.join("") + "<\/select>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";
		}
		
		if (lineNumber > 2){
			//var pFlag = "off";
			selectOptions2[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry2", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
			//pFlag = "on";
			selectOptions2[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry2", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
			var data = "<select name='lineEnabled2' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions2.join("") + "<\/select>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			//var pFlag = "off";
			selectOptions2[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry2", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
			//pFlag = "on";
			selectOptions2[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry2", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
			var data = "<select name='lineEnabled2' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions2.join("") + "<\/select>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		
		if (lineNumber > 3){
			//var pFlag = "off";
			selectOptions3[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry3", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
			//pFlag = "on";
			selectOptions3[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry3", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
			var data = "<select name='lineEnabled3' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions3.join("") + "<\/select>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			//var pFlag = "off";
			selectOptions3[0] = "<option value=\'No\' <%if tcWebAPI_get("VoIPBasic_Entry3", "Enable", "h") = "No" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep16Text","s")%>";
			//pFlag = "on";
			selectOptions3[1] = "<option value=\'Yes\' <%if tcWebAPI_get("VoIPBasic_Entry3", "Enable", "h") = "Yes" then  asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep17Text","s")%>";
			var data = "<select name='lineEnabled3' <% if tcwebApi_get("WebCustom_Entry","isCTSCSupported","h") = "Yes" then %>disabled<% end if %>>" + selectOptions3.join("") + "<\/select>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		tableHtmlOutput += "</tr>";
		
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td class='hd'><%tcWebApi_get("String_Entry","VoipStep18Text","s")%><\/td>"
		var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus0\' id=\'regStatus0\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry0", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry0", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
		tableHtmlOutput += "<td align=center>" + data + "</td>";
		
		if (lineNumber > 1){
			var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus1\' id=\'regStatus1\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry1", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry1", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
			tableHtmlOutput += "<td align=center >" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus1\' id=\'regStatus1\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry1", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry1", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";	
		}
		
		if (lineNumber > 2){
			var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus2\' id=\'regStatus2\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry2", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry2", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus2\' id=\'regStatus2\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry2", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry2", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";	
		}
		
		if (lineNumber > 3){
			var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus3\' id=\'regStatus3\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry3", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry3", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=40 disabled=true name=\'regStatus3\' id=\'regStatus3\' value=\'<%if tcWebAPI_get("InfoVoIP_Entry3", "Status", "h") <> "N/A" then tcWebAPI_get("InfoVoIP_Entry3", "Status", "s") else tcWebApi_get("String_Entry","VoipStep7Text","s") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";	
		}
		tableHtmlOutput += "</tr>";
		
		//telephone number
		tableHtmlOutput += "<tr ID=\"edit_telnumber\">";
		tableHtmlOutput += "<td class='hd'><%tcWebApi_get("String_Entry","VoipStep19Text","s")%><\/td>"
		
		var data = "<input type='text' size=20 maxlength=40 name=\'extension0\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry0", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
		tableHtmlOutput += "<td align=center>" + data + "</td>";
		
		if (lineNumber > 1 ){
			var data = "<input type='text' size=20 maxlength=40 name=\'extension1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=40 name=\'extension1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center 	style=display:none >" + data + "</td>";
		}
		
		if (lineNumber > 2 ){
			var data = "<input type='text' size=20 maxlength=40 name=\'extension2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=40 name=\'extension2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}

		if (lineNumber > 3 ){
			var data = "<input type='text' size=20 maxlength=40 name=\'extension3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=40 name=\'extension3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";
		}
		tableHtmlOutput += "</tr>";
		
		//display name
		tableHtmlOutput += "<tr ID=\"edit_displayname\">";
		tableHtmlOutput += "<td class='hd'><%tcWebApi_get("String_Entry","VoipStep20Text","s")%><\/td>"
		var data = "<input type='text' size=20 maxlength=128 name=\'dispName0\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry0", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
		tableHtmlOutput += "<td align=center>" + data + "</td>";
		
		if (lineNumber >1 ){
			var data = "<input type='text' size=20 maxlength=128 name=\'dispName1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=128 name=\'dispName1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";
		}
		
		if (lineNumber >2 ){
			var data = "<input type='text' size=20 maxlength=128 name=\'dispName2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=128 name=\'dispName2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		
		if (lineNumber >2 ){
			var data = "<input type='text' size=20 maxlength=128 name=\'dispName3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		} 
		else{
			var data = "<input type='text' size=20 maxlength=128 name=\'dispName3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayNumber", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPDisplayNumber", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		tableHtmlOutput += "</tr>";
		
		//auth name
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td class='hd'><%tcWebApi_get("String_Entry","VoipStep21Text","s")%><\/td>"
		
		var data = "<input type='text' size=20 maxlength=128 name=\'authName0\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry0", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
		tableHtmlOutput += "<td align=center>" + data + "</td>";
		
		if (lineNumber > 1){
			var data = "<input type='text' size=20 maxlength=128 name=\'authName1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=128 name=\'authName1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";
		}
		 
		if (lineNumber > 2){
			var data = "<input type='text' size=20 maxlength=128 name=\'authName2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		} 
		else{
			var data = "<input type='text' size=20 maxlength=128 name=\'authName2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		
		if (lineNumber > 3){
			var data = "<input type='text' size=20 maxlength=128 name=\'authName3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='text' size=20 maxlength=128 name=\'authName3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPAuthenticationName", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPAuthenticationName", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Password row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td class='hd'><%tcWebApi_get("String_Entry","VoipStep22Text","s")%><\/td>"
		
		var data = "<input type='Password' size=20 maxlength=128 name=\'password0\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry0", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry0", "SIPPassword", "s") else asp_Write("") end if %>\'>";
		tableHtmlOutput += "<td align=center>" + data + "</td>";
		
		if (lineNumber > 1){
			var data = "<input type='Password' size=20 maxlength=128 name=\'password1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPPassword", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}else{
			var data = "<input type='Password' size=20 maxlength=128 name=\'password1\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry1", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry1", "SIPPassword", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";
		}
		
		if (lineNumber > 2){
			var data = "<input type='Password' size=20 maxlength=128 name=\'password2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPPassword", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='Password' size=20 maxlength=128 name=\'password2\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry2", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry2", "SIPPassword", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none >" + data + "</td>";
		}

		if (lineNumber > 3){
			var data = "<input type='Password' size=20 maxlength=128 name=\'password3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPPassword", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center>" + data + "</td>";
		}
		else{
			var data = "<input type='Password' size=20 maxlength=128 name=\'password3\' value=\'<%if tcWebAPI_get("VoIPBasic_Entry3", "SIPPassword", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Entry3", "SIPPassword", "s") else asp_Write("") end if %>\'>";
			tableHtmlOutput += "<td align=center style=display:none>" + data + "</td>";
		}
		tableHtmlOutput += "</tr>";
		
		//End of table
		tableHtmlOutput += "</tbody></table>";
		
		document.getElementById("perLineTablePlaceholder1").innerHTML = tableHtmlOutput;
		//document.getElementById("perLineTable1").style.width = 100 + "px";
		}else {
			alert("Cannot create per line table in this browser.");
		}
}

function enableEditBoxes()
{
	if ( sm == "1" )
	{
		document.getElementById('edit_dareSipDomain').style.display = "none";
		document.getElementById('edit_telnumber').style.display = "none";
		document.getElementById('edit_displayname').style.display = "none";
	}
	else
	{
		document.getElementById('edit_dareSipDomain').style.display = "";
		document.getElementById('edit_telnumber').style.display = "";
		document.getElementById('edit_displayname').style.display = "";
	}
}	
function applyClickAdvanced()
{
	var loc = "app-VoIP_Adv.asp";
	var code = "location='" + loc + " '";
	eval(code);
}

function proClick(obj) {
	var idx = obj.selectedIndex;
	var val = obj.options[idx].value;
	if ( val == "SIP" )
	{
		sm = '1';
	}
<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
	else if ( val == "H.248" )
	{
		location.replace("/cgi-bin/app-VoIP248.asp?isLocation=1");
	}
<% end if %>
	else
	{
		sm = '0';
	}
	
	enableEditBoxes();
}

function curIndexUpdate()
{
	var pvc_counts = <% tcWebApi_get("WanInfo_Common", "CycleNum", "s") %>;
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common", "CycleValue", "s") %>";
	var indexArrayStr = "<% tcWebApi_get("WanInfo_Common", "ValidIFIndex", "s") %>";
	var Wan_WanName = vArrayStr.split(',');
	var Voip_WanIndex = indexArrayStr.split(',');

	for (var i = 0;i < pvc_counts;i++)
	{
		if(Wan_WanName[i]==document.ConfigForm.ifName.value) 
		{
			document.ConfigForm.curSetIndex.value = Voip_WanIndex[i]; 
		}
	}
}

function printInterfaceListBox()
{
	var pvc_counts = <% tcWebApi_get("WanInfo_Common", "CycleNum", "s") %>;
	var vArrayStr = "<% tcWebApi_get("WanInfo_Common", "CycleValue", "s") %>";
	var Voip_WanName = "<% tcWebApi_get("VoIPAdvanced_Common", "CurIFName", "s") %>";
	var Wan_WanName = vArrayStr.split(',');

	for (var i = 0;i < pvc_counts;i++)
	{
		if (Wan_WanName[i].indexOf('_VOICE_') > -1 && Wan_WanName[i].indexOf('_R_') > -1)
		{
			if(Wan_WanName[i]==Voip_WanName) 
			{
				document.writeln("<option value=\"" + Wan_WanName[i] + "\" selected>"+Wan_WanName[i]+"");
			}
			else
				document.writeln("<option value=\"" + Wan_WanName[i] + "\">"+Wan_WanName[i]+"");
		}
	}

	curIndexUpdate();
}

function printLocaleListBox()
{
	//var locList = ',,,,,,,,,,,,,,,,,,,,,,,,';	
	document.writeln("<option value=\"AUS-AUSTRAILIA\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "AUS-AUSTRAILIA" then  asp_Write("selected") end if %>> AUS-AUSTRAILIA" );
	document.writeln("<option value=\"BEL-BELIGIUM\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "BEL-BELIGIUM" then  asp_Write("selected") end if %>> BEL-BELIGIUM" );
	document.writeln("<option value=\"BRA-BRAZIL\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "BRA-BRAZIL" then  asp_Write("selected") end if %>> BRA-BRAZIL" );
	document.writeln("<option value=\"CHL-CHILE\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "CHL-CHILE" then  asp_Write("selected") end if %>> CHL-CHILE" );
	document.writeln("<option value=\"CHN-CHINA\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "CHN-CHINA" then  asp_Write("selected") end if %>> CHN-CHINA" );
	document.writeln("<option value=\"CZH-CZECH\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "CZH-CZECH" then  asp_Write("selected") end if %>> CZH-CZECH" );
	document.writeln("<option value=\"DNK-DENMARK\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "DNK-DENMARK" then  asp_Write("selected") end if %>> DNK-DENMARK" );
	document.writeln("<option value=\"ETS-ETSI\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "ETS-ETSI" then  asp_Write("selected") end if %>> ETS-ETSI" );
	document.writeln("<option value=\"FIN-FINLAND\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "FIN-FINLAND" then  asp_Write("selected") end if %>> FIN-FINLAND" );
	document.writeln("<option value=\"FRA-FRANCE\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "FRA-FRANCE" then  asp_Write("selected") end if %>> FRA-FRANCE" );
	document.writeln("<option value=\"DEU-GERMANY\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "DEU-GERMANY" then  asp_Write("selected") end if %>> DEU-GERMANY" );
	document.writeln("<option value=\"HUN-HUNGARY\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "HUN-HUNGARY" then  asp_Write("selected") end if %>> HUN-HUNGARY" );
	document.writeln("<option value=\"IND-INDIA\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "IND-INDIA" then  asp_Write("selected") end if %>> IND-INDIA" );
	document.writeln("<option value=\"ITA-ITALY\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "ITA-ITALY" then  asp_Write("selected") end if %>> ITA-ITALY" );
	document.writeln("<option value=\"JPN-JAPAN\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "JPN-JAPAN" then  asp_Write("selected") end if %>> JPN-JAPAN" );
	document.writeln("<option value=\"RUS-RUSSIA\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "RUS-RUSSIA" then  asp_Write("selected") end if %>> RUS-RUSSIA" );
	document.writeln("<option value=\"NLD-NETHERLANDS\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "NLD-NETHERLANDS" then  asp_Write("selected") end if %>> NLD-NETHERLANDS" );
	document.writeln("<option value=\"NZL-NEWZEALAND\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "NZL-NEWZEALAND" then  asp_Write("selected") end if %>> NZL-NEWZEALAND" );
	document.writeln("<option value=\"USA-NORTHAMERICA\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "USA-NORTHAMERICA" then  asp_Write("selected") end if %>> USA-NORTHAMERICA" );
	document.writeln("<option value=\"ESP-SPAIN\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "ESP-SPAIN" then  asp_Write("selected") end if %>> ESP-SPAIN" );
	document.writeln("<option value=\"SWE-SWEDEN\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "SWE-SWEDEN" then  asp_Write("selected") end if %>> SWE-SWEDEN" );
	document.writeln("<option value=\"NOR-NORWAY\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "NOR-NORWAY" then  asp_Write("selected") end if %>> NOR-NORWAY" );
	document.writeln("<option value=\"CHE-SWITZERLAND\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "CHE-SWITZERLAND" then  asp_Write("selected") end if %>> CHE-SWITZERLAND" );
	document.writeln("<option value=\"T57-TR57\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "T57-TR57" then  asp_Write("selected") end if %>> T57-TR57" );
	document.writeln("<option value=\"GBR-UK\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "GBR-UK" then  asp_Write("selected") end if %>> GBR-UK" );
	document.writeln("<option value=\"TWN-TAIWAN\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "TWN-TAIWAN" then  asp_Write("selected") end if %>> TWN-TAIWAN" );
	document.writeln("<option value=\"POL-POLAND\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "POL-POLAND" then  asp_Write("selected") end if %>> POL-POLAND" );
	document.writeln("<option value=\"SRI-SRILANKA\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "SRI-SRILANKA" then  asp_Write("selected") end if %>> SRI-SRILANKA" );
	document.writeln("<option value=\"MEX-MEXICO\" <%if TCWebAPI_get("VoIPAdvanced_Common", "VoIPRegion", "h") = "MEX-MEXICO" then  asp_Write("selected") end if %>> MEX-MEXICO" );
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
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep23Text","s")%>');
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}
		else
		{
			return true;
		}
	}
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

function btnApplySip() 
{
	with ( document.ConfigForm )
	{
		if(regAddr.value != "")
		{
			if(regAddr.value.match(":"))
			{
				var result = isValidIPV6(regAddr.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStep24Text","s")%>' + regAddr.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
			    }
			}else if (isValidName(regAddr.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStep24Text","s")%>' + regAddr.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		
		else
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep32Text","s")%>');
			return;
		}
	
		if (regAddr2.value != "")
		{	
			if(regAddr2.value.match(":"))
			{
				var result = isValidIPV6(regAddr2.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStep26Text","s")%>' + regAddr2.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
			    }
			}else if (isValidName(regAddr2.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStep26Text","s")%>' + regAddr2.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		
		
		
		if (proxyAddr.value != "")
		{
			if(proxyAddr.value.match(":"))
			{
				var result = isValidIPV6(proxyAddr.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStep27Text","s")%>' + proxyAddr.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
			    }
			}else if (isValidName(proxyAddr.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStep27Text","s")%>' + proxyAddr.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		
		else
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep36Text","s")%>');
			return;
		}
		
		if (proxyAddr2.value != "")
		{
			if(proxyAddr2.value.match(":"))
			{
				var result = isValidIPV6(proxyAddr2.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStep28Text","s")%>' + proxyAddr2.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
			    }
			}else if (isValidName(proxyAddr2.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStep28Text","s")%>' + proxyAddr2.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		

		
		if (obProxyAddr.value != "")
		{
			if(obProxyAddr.value.match(":"))
			{
				var result = isValidIPV6(obProxyAddr.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStep29Text","s")%>' + obProxyAddr.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
			    }
			}else if (isValidName(obProxyAddr.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStep29Text","s")%>' + obProxyAddr.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		


		if (obProxyAddr2.value != "")
		{
			if(obProxyAddr2.value.match(":"))
			{
				var result = isValidIPV6(obProxyAddr2.value);
				if(result != true){
				    alert('"<%tcWebApi_get("String_Entry","VoipStep30Text","s")%>' + obProxyAddr2.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				    return;
				}
			}else if (isValidName(obProxyAddr2.value) == false)
			{
				alert('"<%tcWebApi_get("String_Entry","VoipStep30Text","s")%>' + obProxyAddr2.value + '"<%tcWebApi_get("String_Entry","VoipStep25Text","s")%>');
				return;
			}
		}		
		

		if (regPort.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep31Text","s")%>');
			return;
		}
		else
		{
			if(checkPort(document.getElementById("regPort"))==false)			
				return;
		}

	

		if (regPort2.value != "")
		{
			if(checkPort(document.getElementById("regPort2"))==false)			
			return;
		}

	

		if (proxyPort.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep35Text","s")%>');
			return;
		}
		else
		{
			if(checkPort(document.getElementById("proxyPort"))==false)				
			return;
		}

	

		if (proxyPort2.value != "")
		{
			if(checkPort(document.getElementById("proxyPort2"))==false)				
			return;
		}


		if (obProxyPort.value != "")
		{
			if(checkPort(document.getElementById("obProxyPort"))==false)			
			return;
		}

		if (obProxyPort2.value != "")
		{
			if(checkPort(document.getElementById("obProxyPort2"))==false)				
			return;
		}

	
		<% if tcWebApi_get("WebCustom_Entry", "isCTPONC9Supported", "h") = "Yes"  then %>
			if (obProxyEn.checked)
			{
				obProxyEn_flag.value = "Yes";
			}
		<% end if %>
		VoIP_basic_flag.value = 1;
		parent.voipType = "SIP";
		if( true == setEBooValueCookie(document.ConfigForm) )
			submit();
	}
}
<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
	function selectOption()
	{
		with (document.ConfigForm)
		{
			for(i = 0;i<3;i++)
			{
				if(sipMode.options[i].value == voiptype.slice(10))
				{
					sipMode.options[i].selected = true;
				}
			}
	
			if ( voiptype.slice(10) == "SIP" )
			{
				sm = '1';
			}
			else if(voiptype.slice(10) == "IMSSIP")
			{
				sm = '0';
			}
		}
	}
<% end if %>
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
														href="/cgi-bin/login.asp" id=logOut name=logOut target=_top><SPAN 
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
										<TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24></TD>
									</TR>
							</TBODY></TABLE>
							<SCRIPT language=javascript>
								MakeMenu(getElById ('Selected_Menu').value);
							</SCRIPT>

							<TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 
							border=0><TBODY>
								<TR>
									<TD height=15><IMG height=15 src="/img/panel1.gif" width=164 border=0> </TD>
									<TD><IMG height=15 src="/img/panel2.gif" width=645 border=0> 
					</TD></TR></TBODY></TABLE></TD></TR>
					<TR>
						<TD vAlign=top>
						<TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
							<TBODY>
								<TR>
									<TD width=157 bgColor=#ef8218 height=30>
										<P class=Item_L1><%tcWebApi_get("String_Entry","VoipStep1Text","s")%></P></TD>
												<TD width=7 bgColor=#ef8218>　</TD>
												<TD width=474>　</TD>
												<TD vAlign=top width=170 background=/img/panel4.gif 
													rowSpan=13><TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
													<TBODY>
														<TR>
															<TD valign='top'><A 
																href="/cgi-bin/help_content.asp#宽带电话设置" 
																target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" 
															width=40 border=0> </A></TD>
														</TR>
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
											<FORM id=ConfigForm name="ConfigForm" action="/cgi-bin/app-VoIP.asp" method="post">
												<DIV id=pptp>
													<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
														<TBODY>
															<TR>
																<TD>
																	<input type='button' id=applyClickAdv name=applyClickAdv style="font-size:100%;width:170;height:30" onClick="applyClickAdvanced()" value='<%tcWebApi_get("String_Entry","VoipStep43Text","s")%>'>
																</TD>
															</TR>
															<TR>
																<TD height="10">
																	
																</TD>
															</TR>
															<TR>
																<TD class=table1_head>
																	<B>
																	<%tcWebApi_get("String_Entry","VoipStep44Text","s")%>
																	</B>
																</TD>
															</TR>
															<TR>
																<TD height="10">
																	
																</TD>
															</TR>
															<TR>
																<TD>
																	<%tcWebApi_get("String_Entry","VoipStep45Text","s")%>
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
													<TABLE border=0 cellSpacing=1 cellPadding=0 width="98%">
														<TBODY>
															<TR>
																<TD width="35%" align=left>
																	<%tcWebApi_get("String_Entry","VoipStep46Text","s")%>
																</TD>
																<TD width="65%" colSpan=2 align=left>
																	<select name="sipMode" id="mg_regist" onChange="proClick(this)">
																		<option value="IMSSIP" <%if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "IMSSIP" then asp_Write("selected") end if %>>IMSSIP</option>
																		<option value="SIP" <%if tcWebApi_get("VoIPBasic_Common", "SIPProtocol", "h") = "SIP" then asp_Write("selected") end if %>>SIP</option>
																		<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
																			<option value="H.248" >H.248</option>
																		<%end if%>
																	</select>
																	<% if tcWebApi_get("WebCustom_Entry", "isCTCUCSIPH248Supported", "h") = "Yes"  then %>
																		<%tcWebApi_get("String_Entry","VoipStep47Text","s")%>
																	<%end if%>
																</TD>
															</TR>
															<TR>
																<TD align=left>
																	<%tcWebApi_get("String_Entry","VoipStep48Text","s")%>
																</TD>
																<TD align=left>
																	<TABLE>
																		<TR>
																			<TD>
																				<input type="hidden" id="curSetIndex" name="curSetIndex" value="<%if tcWebApi_get("VoIPAdvanced_Common", "CurIFIndex", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "CurIFIndex", "s") else asp_Write("") end if %>">
																					<select id="ifName" name="ifName" onChange="curIndexUpdate()" size="1" >
																						<script TYPE="text/javascript">
																							printInterfaceListBox()
																						</script>
																					</select>
																			</TD>
																			<TD>
																				<b><%tcWebApi_get("String_Entry","VoipStep49Text","s")%></b>
																			</TD>
																		</TR>
																	</TABLE>
																</TD>
															</TR>
															<TR>
																<TD align=left>
																	<%tcWebApi_get("String_Entry","VoipStep50Text","s")%>
																</TD>
																<TD align=left>
																	<TABLE>
																		<TR>
																			<TD>
																				<select id="localeName" name="localeName" size="1">
																					<script TYPE="text/javascript">
																						printLocaleListBox()
																					</script>
																				</select>
																			</TD>
																			<TD>
																				<b><%tcWebApi_get("String_Entry","VoipStep49Text","s")%></b>
																			</TD>
																		</TR>
																	</TABLE>
																</TD>
															</TR>
															<tr ID="edit_dareSipDomain">
																<td width="35%"><%tcWebApi_get("String_Entry","VoipStep51Text","s")%></td>
																<td width="65%"><input type='text' id='dareSipDomain' name='dareSipDomain' VALUE="<%if tcWebApi_get("VoIPAdvanced_Common", "SIPDomain", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "SIPDomain", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr>
																<td><%tcWebApi_get("String_Entry","VoipStep52Text","s")%></td>
																<td><input type='text' id='regAddr' name='regAddr' VALUE="<%if tcWebApi_get("VoIPBasic_Common", "RegistrarServer", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "RegistrarServer", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr>
																<td><%tcWebApi_get("String_Entry","VoipStep53Text","s")%></td>
																<td><input type='text' id='regPort' name='regPort' onchange="checkPort(this)" VALUE="<%if tcWebApi_get("VoIPBasic_Common", "RegistrarServerPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "RegistrarServerPort", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_proxyAddr">
																<td><%tcWebApi_get("String_Entry","VoipStep54Text","s")%></td>
																<td><input type='text' id='proxyAddr' name='proxyAddr' VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SIPProxyAddr", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SIPProxyAddr", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_proxyPort">
																<td><%tcWebApi_get("String_Entry","VoipStep55Text","s")%></td>
														<td><input type='text' id='proxyPort' name='proxyPort' onchange="checkPort(this)" VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SIPProxyPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SIPProxyPort", "s") else asp_Write("") end if %>"></td>
															</tr>
															<% if tcWebApi_get("WebCustom_Entry", "isCTPONC9Supported", "h") = "Yes"  then %>
																<tr ID="edit_obProxyEn">
																	<td><%tcWebApi_get("String_Entry","VoipStep56Text","s")%></td>
																	<td>
																<input type='checkbox' id='obProxyEn' name='obProxyEn' value='YES' <%if tcWebApi_get("VoIPBasic_Common", "SIPOutboundProxyEnable", "h") = "Yes" then asp_Write("checked") end if %>>
																	</td>
																</tr>
															<% end if %>
															<tr ID="edit_obProxyAddr">
																<td><%tcWebApi_get("String_Entry","VoipStep57Text","s")%></td>
														<td><input type='text' id='obProxyAddr' name='obProxyAddr' VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SIPOutboundProxyAddr", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SIPOutboundProxyAddr", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_obProxyPort">
																<td><%tcWebApi_get("String_Entry","VoipStep58Text","s")%></td>
																<td><input type='text' id='obProxyPort' name='obProxyPort' onchange="checkPort(this)" VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SIPOutboundProxyPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SIPOutboundProxyPort", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_regAddr2">
																<td><%tcWebApi_get("String_Entry","VoipStep59Text","s")%></td>
																<td><input type='text' id='regAddr2' name='regAddr2' VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SBRegistrarServer", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBRegistrarServer", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_regPort2">
																<td><%tcWebApi_get("String_Entry","VoipStep60Text","s")%></td>
																<td><input type='text' id='regPort2' name='regPort2' onchange="checkPort(this)" VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SBRegistrarServerPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBRegistrarServerPort", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_proxyAddr2">
																<td><%tcWebApi_get("String_Entry","VoipStep61Text","s")%></td>
																<td><input type='text' id='proxyAddr2' name='proxyAddr2' VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SBSIPProxyAddr", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBSIPProxyAddr", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_proxyPort2">
																<td><%tcWebApi_get("String_Entry","VoipStep62Text","s")%></td>
																<td><input type='text' id='proxyPort2' name='proxyPort2' onchange="checkPort(this)" VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SBSIPProxyPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBSIPProxyPort", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_obProxyAddr2">
																<td><%tcWebApi_get("String_Entry","VoipStep63Text","s")%></td>
																<td><input type='text' id='obProxyAddr2' name='obProxyAddr2' VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SBOutboundProxyAddr", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBOutboundProxyAddr", "s") else asp_Write("") end if %>"></td>
															</tr>
															<tr ID="edit_obProxyPort2">
																<td><%tcWebApi_get("String_Entry","VoipStep64Text","s")%></td>
																<td><input type='text' id='obProxyPort2' name='obProxyPort2' onchange="checkPort(this)" VALUE="<%if tcWebApi_get("VoIPBasic_Common", "SBOutboundProxyPort", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SBOutboundProxyPort", "s") else asp_Write("") end if %>"></td>
															</tr>
														</TBODY>
													</TABLE>
													<BR>
													<div id="perLineTablePlaceholder1">
													
													</div>
													<TABLE border=0 cellSpacing=1 cellPadding=0 width="98%">
														<tr>
															<td height="10">
															</td>
														</tr>
														<tr>
															<td align="center">
															 	<input type='hidden' id="VoIP_basic_flag" name="VoIP_basic_flag" value="0">
																<% if tcWebApi_get("WebCustom_Entry", "isCTPONC9Supported", "h") = "Yes"  then %>
																	<input type='hidden' id="obProxyEn_flag" name="obProxyEn_flag" value="No">
																<% end if %>
															</td>
														</tr>
														<tr>
															<td align="center">
																<input type='button' onClick="btnApplySip()" value='<%tcWebApi_get("String_Entry","VoipStep65Text","s")%>' ID="ApplyButton" name="ApplyButton">
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
						<TD width=7 background=/img/panel3.gif>　</TD>
						<TD></TD></TR></TBODY></TABLE>
					</TD></TR>
					<TR>
						<TD height=1>
						<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
						border=0><TBODY>
							<TR>
								<TD width=162 bgColor=#ef8218>　</TD>
								<TD width=278 bgColor=#427594>　</TD>
								<TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
								<TD width=170 bgColor=#313031>　</TD>
							</TR>
						</TBODY></TABLE></TD></TR>
</TBODY></TABLE></BODY></HTML>
