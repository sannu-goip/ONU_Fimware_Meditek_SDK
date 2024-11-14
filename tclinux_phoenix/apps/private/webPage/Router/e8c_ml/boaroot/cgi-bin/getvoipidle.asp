{"voipIdle" :
<% 
	tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "1")
	
	if tcWebApi_get("VoIPBasic_Entry0", "Enable", "h") = "Yes"  then
		if tcWebApi_get("VoIPSysParam_Entry0", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
			if tcWebApi_get("VoIPSysParam_Entry0", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
				tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
			end if
		end if
	end if
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("VoIPBasic_Entry1", "Enable", "h") = "Yes"  then
			if tcWebApi_get("VoIPSysParam_Entry1", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
				if tcWebApi_get("VoIPSysParam_Entry1", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
				end if
			end if
		end if
	end if
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("VoIPBasic_Entry2", "Enable", "h") = "Yes"  then
			if tcWebApi_get("VoIPSysParam_Entry2", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
				if tcWebApi_get("VoIPSysParam_Entry2", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
				end if
			end if
		end if
	end if
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("VoIPBasic_Entry3", "Enable", "h") = "Yes"  then
			if tcWebApi_get("VoIPSysParam_Entry3", "SC_LINE_INFO_STATUS", "h") <> "N/A"  then
				if tcWebApi_get("VoIPSysParam_Entry3", "SC_LINE_INFO_STATUS", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
				end if
			end if
		end if
	end if	
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
		if tcWebApi_get("InfoVoIPH248_Entry0", "lineInfoStatus", "h") <> "N/A" then
			if tcWebApi_get("InfoVoIPH248_Entry0", "lineInfoStatus", "h") <> "Idle"  then
				tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
			end if
		end if
	end if
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
			if tcWebApi_get("InfoVoIPH248_Entry1", "lineInfoStatus", "h") <> "N/A" then
				if tcWebApi_get("InfoVoIPH248_Entry1", "lineInfoStatus", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
				end if
		end if
	end if
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then		
			if tcWebApi_get("InfoVoIPH248_Entry2", "lineInfoStatus", "h") <> "N/A" then
				if tcWebApi_get("InfoVoIPH248_Entry2", "lineInfoStatus", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
				end if
			end if
	end if
	
	if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") = "1"  then
			if tcWebApi_get("InfoVoIPH248_Entry3", "lineInfoStatus", "h") <> "N/A" then
				if tcWebApi_get("InfoVoIPH248_Entry3", "lineInfoStatus", "h") <> "Idle"  then
					tcWebApi_constSet("WebCurSet_Entry", "IdleStatus_flag", "0")
				end if
		end if
	end if

	if tcWebApi_get("WebCurSet_Entry", "IdleStatus_flag", "h") = "1"  then
			asp_write("1");
	else
			asp_write("0");
	end if

%>
}