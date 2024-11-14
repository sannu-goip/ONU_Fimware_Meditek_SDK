<%
if Request_Form("Save_Flag") = "1" then
	if Request_Form("webwait_Flag") = "1" then
		TCWebApi_constSet("Mesh_action", "map_action_complete", "1")
	else
		TCWebApi_constSet("Mesh_action", "map_action_complete", "0")
	end if

	if Request_Form("Edit_Flag") = "1" then		
		if Request_Form("_edit_removeNodeIndex") = "1" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry0", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry0", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry0", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry0", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry0", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry0", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry0", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry0", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry0", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry0", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry0")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "2" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry1", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry1", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry1", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry1", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry1", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry1", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry1", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry1", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry1", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry1", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry1")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "3" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry2", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry2", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry2", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry2", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry2", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry2", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry2", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry2", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry2", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry2", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry2")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "4" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry3", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry3", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry3", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry3", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry3", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry3", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry3", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry3", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry3", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry3", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry3")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "5" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry4", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry4", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry4", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry4", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry4", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry4", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry4", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry4", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry4", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry4", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry4")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "6" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry5", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry5", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry5", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry5", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry5", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry5", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry5", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry5", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry5", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry5", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry5")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "7" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry6", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry6", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry6", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry6", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry6", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry6", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry6", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry6", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry6", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry6", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry6")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "8" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry7", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry7", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry7", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry7", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry7", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry7", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry7", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry7", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry7", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry7", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry7")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "9" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry8", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry8", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry8", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry8", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry8", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry8", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry8", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry8", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry8", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry8", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry8")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "10" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry9", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry9", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry9", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry9", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry9", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry9", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry9", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry9", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry9", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry9", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry9")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "11" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry10", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry10", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry10", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry10", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry10", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry10", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry10", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry10", "DefaultPCP", "_defaultPcp")				
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry10", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry10", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry10")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "12" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry11", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry11", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry11", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry11", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry11", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry11", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry11", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry11", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry11", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry11", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry11")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "13" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry12", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry12", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry12", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry12", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry12", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry12", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry12", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry12", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry12", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry12", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry12")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "14" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry13", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry13", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry13", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry13", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry13", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry13", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry13", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry13", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry13", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry13", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry13")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "15" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry14", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry14", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry14", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry14", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry14", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry14", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry14", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry14", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry14", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry14", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry14")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "16" then
			if Request_Form("_band") = "8x" then
				TCWebApi_Set("WLan_Entry15", "SSID", "ssid")
				TCWebApi_Set("WLan_Entry15", "AuthMode", "_authMode")
				TCWebApi_Set("WLan_Entry15", "EncrypType", "_encType")
				TCWebApi_Set("WLan_Entry15", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan_Entry15", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio2gbssinfo_entry15", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry15", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio2gbssinfo_entry15", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio2gbssinfo_entry15", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio2gbssinfo_entry15", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio2gbssinfo_entry15")
				end if
			end if
		end if
		
		if Request_Form("_edit_removeNodeIndex") = "1" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry0", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry0", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry0", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry0", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry0", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry0", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry0", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry0", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry0", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry0", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry0")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "2" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry1", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry1", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry1", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry1", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry1", "HideSSID", "isHidden")
				
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry1", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry1", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry1", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry1", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry1", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry1")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "3" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry2", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry2", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry2", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry2", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry2", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry2", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry2", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry2", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry2", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry2", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry2")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "4" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry3", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry3", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry3", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry3", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry3", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry3", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry3", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry3", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry3", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry3", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry3")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "5" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry4", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry4", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry4", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry4", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry4", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry4", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry4", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry4", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry4", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry4", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry4")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "6" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry5", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry5", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry5", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry5", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry5", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry5", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry5", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry5", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry5", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry5", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry5")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "7" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry6", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry6", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry6", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry6", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry6", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry6", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry6", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry6", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry6", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry6", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry6")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "8" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry7", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry7", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry7", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry7", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry7", "HideSSID", "isHidden")				
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry7", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry7", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry7", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry7", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry7", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry7")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "9" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry8", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry8", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry8", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry8", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry8", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry8", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry8", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry8", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry8", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry8", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry8")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "10" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry9", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry9", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry9", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry9", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry9", "HideSSID", "isHidden")				
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry9", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry9", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry9", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry9", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry9", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry9")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "11" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry10", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry10", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry10", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry10", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry10", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry10", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry10", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry10", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry10", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry10", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry10")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "12" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry11", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry11", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry11", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry11", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry11", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry11", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry11", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry11", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry11", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry11", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry11")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "13" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry12", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry12", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry12", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry12", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry12", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry12", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry12", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry12", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry12", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry12", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry12")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "14" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry13", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry13", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry13", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry13", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry13", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry13", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry13", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry13", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry13", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry13", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry13")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "15" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry14", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry14", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry14", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry14", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry14", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry14", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry14", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry14", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry14", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry14", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry14")
				end if
			end if
		end if
		if Request_Form("_edit_removeNodeIndex") = "16" then
			if Request_Form("_band") = "11x" then
				TCWebApi_Set("WLan11ac_Entry15", "SSID", "ssid")
				TCWebApi_Set("WLan11ac_Entry15", "AuthMode", "_authMode")
				TCWebApi_Set("WLan11ac_Entry15", "EncrypType", "_encType")
				TCWebApi_Set("WLan11ac_Entry15", "WPAPSK", "passPhrase")
				TCWebApi_Set("WLan11ac_Entry15", "HideSSID", "isHidden")
				if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then
					TCWebApi_Set("Mesh_radio5glbssinfo_entry15", "VlanID", "_fhVlanId")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry15", "PrimaryVlan", "_primaryVlan")
					TCWebApi_Set("Mesh_radio5glbssinfo_entry15", "DefaultPCP", "_defaultPcp")
				end if
				TCWebApi_Set("Mesh_radio5glbssinfo_entry15", "BackHaul", "isBhBssSupported")
				TCWebApi_Set("Mesh_radio5glbssinfo_entry15", "FrontHaul", "isFhBssSupported")
				if Request_Form("restore_mesh") = "1" then		
					TCWebApi_commit("Mesh_dat")
				else
					TCWebApi_commit("Mesh_radio5glbssinfo_entry15")
				end if
			end if
		end if
		
	end if
	if Request_Form("Close_Flag") = "1" then	
		TCWebApi_constSet("Mesh_dat", "MapEnable", "0")
		TCWebApi_constSet("Mesh_common", "ReinitWifiFlag", "0")
		TCWebApi_save()
		TCWebApi_commit("Mesh_dat")
	end if
	
	TCWebApi_save()
end if
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>EasyMesh BSS Configuration</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<LINK href="/JS/stylemain.css" type=text/css rel=stylesheet>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/json2.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR>

<SCRIPT language=javascript>

function get_encryption_val2str(encryptionCode)
{
		switch(encryptionCode)
		{
				case "0x0001"://NONE
				    return "NONE";
				case "0x0004"://TKIP
				    return "TKIP";
				case "0x0008"://AES
				    return "AES";
				case "0x000c"://TKIPAES
				    return "TKIPAES";
				default:
						return "";
		}
}

function get_encryption_str2val(encryptionCode)
{
		switch(encryptionCode)
		{
				case "NONE"://NONE
				case "0x0001":
				    return "0x0001";
				case "TKIP"://TKIP
				case "0x0004":
				    return "0x0004";
				case "AES"://AES
				case "0x0008":
				    return "0x0008";
				case "TKIPAES"://TKIPAES
				case "0x000c":
				    return "0x000c";
				default:
						return "";
		}
}

function get_auth_mode_val2str(authModeCode)
{
		switch(authModeCode)
		{
				case "0x0001"://OPEN
				    return "OPEN";
				case "0x0002"://WPAPSK
				    return "WPAPSK";
				case "0x0020"://WPA2 - Personal
				    return "WPA2PSK";
				case "0x0022"://WPAPSKWPA2PSK
				    return "WPAPSKWPA2PSK";
				case "0x0030":
				    return "WPA3PSK";
				case "0x0031":
				    return "WPA2PSKWPA3PSK";
				default:
						return "";
		}
}

function get_auth_mode_str2val(authModeCode)
{
		switch(authModeCode)
		{
			case "OPEN"://OPEN
			case "0x0001":
				return "0x0001";
			case "WPAPSK"://WPAPSK
			case "0x0002":
				return "0x0002";
			case "WPA2PSK"://WPA2 - Personal
			case "WPA2 - Personal":
			case "0x0020":
				return "0x0020";
			case "WPAPSKWPA2PSK"://WPAPSKWPA2PSK
			case "0x0022":
				return "0x0022";				    
			case "WPA3-SAE":
			case "WPA3SAE"://WPA3SAE
			case "WPA3PSK"://WPA3SAE				
			case "0x0030":
				return "0x0030";
			case "WPA3-SAE Transition"://WPA3SAETransition
			case "WPA3SAETransition":	
			case "WPA2PSKWPA3PSK":				
			case "0x0031":
				return "0x0031";
			default:
					return "";	
		}
}
</SCRIPT>

</HEAD>
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
						<p class="cbi-tab" id="p-item-title" ><%tcWebApi_get("String_Entry","EasyMeshBSS20Test","s")%></p>
					</TD>
					<TD vAlign=top width=7 background="/img/panel3.gif" height=30>
						<p class="cbi-tab" id="p-item-title-pel" ></p>
					</TD>
					<TD vAlign=top width=474 rowspan="2">
						<div id="maincontent" name="maincontent" class="container">
							<input type="hidden" name="webwait_Flag" id="webwait_Flag" value="0">

							<div class="alert-message" id="bss_cfgs_loading_div" style="display:none">
							    <img src="/img/loading.gif" alt="" style="vertical-align:middle" />
							    <big><strong><%tcWebApi_get("String_Entry","EasyMeshBSS12Test","s")%></strong></big>
							</div>
							
							<div width=470 id="bss_cfgs_display">
								<div class="alert-message" id="bss_cfgs_backhaulcheck_div" style="display:none">
									<strong><span style="color:red;"><%tcWebApi_get("String_Entry","EasyMeshBSS13Test","s")%></span><%tcWebApi_get("String_Entry","EasyMeshBSS14Test","s")%> <span style="color:red;"><%tcWebApi_get("String_Entry","EasyMeshBSS15Test","s")%></span><%tcWebApi_get("String_Entry","EasyMeshBSS16Test","s")%></strong></br>&nbsp;
								</div>
							    <form method="post" name="cbi" id="add_bss_cfgs_form" action="/cgi-bin/wifi_map_bss_cfg_renew.asp" autocomplete="off" style="display:none">
							    		<input type="hidden" name="Save_Flag" id="Save_Flag" value="0">
							    		<input type="hidden" name="Add_Flag" id="Add_Flag" value="0">
							    		<input type="hidden" name="Edit_Flag" id="Edit_Flag" value="0">
							    		<input type="hidden" name="Remove_Flag" id="Remove_Flag" value="0">
							    		<input type="hidden" name="Close_Flag" id="Close_Flag" value="0">

							    		<input type="hidden" name="_BssInfoCount" value="0">
							    		<input type="hidden" name="_alMac" value="">
							    		<input type="hidden" name="_band" value="">
							    		<input type="hidden" name="_ssid" value="">
							    		<input type="hidden" name="_authMode" value="">
							    		<input type="hidden" name="_encType" value="">
							    		<input type="hidden" name="_passPhrase" value="">
							    		<input type="hidden" name="_isBhBssSupported" value="">
							    		<input type="hidden" name="_isFhBssSupported" value="">
							    		<input type="hidden" name="_isHidden_ssid" value="">
							    		
							    		<input type="hidden" name="_fhVlanId" id="edit_fh_vlan_id_inp" value="">
										<input type="hidden" name="_primaryVlan" id="edit_primary_vlan_inp" value="0">
										<input type="hidden" name="_defaultPcp" id="edit_default_pcp_inp" value="">	
							    										
							    		<input type="hidden" name="_edit_removeNodeIndex" value="">
							    		
							        <input type="hidden" name="__IS_EDIT" id="edit_bss_hid_inp" value="0" />
							        <input type="hidden" name="__EDIT_SSID" id="edit_bss_ssid_hid_inp" value="" />
							        <input type="hidden" name="__EDIT_AUTH_MODE" id="edit_bss_auth_mode_hid_inp" value="" />
							        <input type="hidden" name="__EDIT_ENCRYPTION_TYPE" id="edit_bss_enc_type_hid_inp" value="" />
							        <input type="hidden" name="__EDIT_PASS_PHRASE" id="edit_bss_pass_phrase_hid_inp" value="" />
							        <input type="hidden" name="__EDIT_BH_SUPPORT" id="edit_bss_bh_support_hid_inp" value="" />
							        <input type="hidden" name="__EDIT_FH_SUPPORT" id="edit_bss_fh_support_hid_inp" value="" />
							        <input type="hidden" name="__EDIT_IS_SSID_HIDDEN" id="edit_bss_is_ssid_hidden_hid_inp" value="" />
							        <input type="hidden" name="isBhBssSupported" id="add_bss_bh_hid_inp" value="0" />
							        <input type="hidden" name="isFhBssSupported" id="add_bss_fh_hid_inp" value="0" />
							        <input type="hidden" name="isHidden" id="add_bss_is_ssid_hidden_hid_inp" value="0" />
							        <input type="hidden" name="__EDIT_REMOVE_BSSINDEX" id="edit_remove_bss_bssindex_hid_inp" value="">
							        <input type="hidden" name="restore_mesh" id="restore_mesh" value="0">
							        
							        
							        <table class="cbi-section-table" id="add_bss_cfgs_tbl">
							        	<tbody id="add_bss_cfgs_ALMAC_tbody" style="display:none">	
							            <tr>
							                <td>AL-MAC</td>
							                <td>
							                    <input type="text" name="alMac" id="add_bss_almac_inp_text" />
							                </td>
							                <td></td>
							            </tr>
							          </tbody>
							            <tr>
							                <td><%tcWebApi_get("String_Entry","EasyMeshBSS9Test","s")%></td>
							                <td>
							                		<div id="add_bss_band_div" style="display: none">
							                    <select name="band" size="1" id="add_bss_band_sel">
							                        <option value="8x">2G</option>
							                        <option value="11x">5GL</option>
							                        <option value="12x">5GH</option>
							                    </select>
							                  	</div>
							                  	<div id="add_bss_band_val_div"></div>
							                    <input type="hidden" name="add_bss_band_sel_hid" id="add_bss_band_sel_hid" value="" />
							                </td>
							                <td></td>
							            </tr>
							            <tbody id="add_bss_cfgs_fields_tbody" style="display:none">
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS2Test","s")%></td>
							                    <td>
							                        <input type="text" name="ssid" id="add_bss_ssid_inp_text" />
							                        <input type="hidden" name="add_bss_ssid_inp_text_hid" id="add_bss_ssid_inp_text_hid" value="" />
							                    </td>
							                    <td></td>
							                </tr>
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS3Test","s")%></td>
							                    <td>
							                        <select name="authMode" size="1" id="add_bss_auth_mode_sel" onchange="AuthModeOnChange(this.value)">
							                            <option value="0x0001">OPEN</option>
							                            <option value="0x0002">WPAPSK</option>
							                            <option value="0x0020">WPA2 - Personal</option>
							                            <option value="0x0022">WPAPSKWPA2PSK</option>
							                            <option value="0x0030">WPA3-SAE</option>
							                            <option value="0x0031">WPA3 Transition</option>		
							                        </select>
							                    </td>
							                    <td></td>
							                </tr>
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS4Test","s")%></td>
							                    <td>
							                        <select name="encType" size="1" id="add_bss_enc_type_sel" onchange="EncryptionTypeOnChange(this.value)">
							                            <option value="0x0001">NONE</option>
							                            <option value="0x0004">TKIP</option>
							                            <option value="0x0008">AES</option>
							                            <option value="0x000c">TKIPAES</option>
							                        </select>
							                    </td>
							                    <td></td>
							                </tr>
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS5Test","s")%></td>
							                    <td>
							                        <input type="text" name="passPhrase" id="add_bss_pass_phrase_inp_text" />
							                    </td>
							                    <td></td>
							                </tr>
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS6Test","s")%></td>
							                    <td>
							                        <input type="checkbox" id="add_bss_is_ssid_hidden_cbox" />
							                    </td>
							                    <td></td>
							                </tr>
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS7Test","s")%></td>
							                    <td>
							                        <input type="checkbox" id="add_bss_bh_cbox" onClick="validate_bss_backhaul()" />
							                    </td>
							                    <td></td>
							                </tr>
							                <tr>
							                    <td><%tcWebApi_get("String_Entry","EasyMeshBSS8Test","s")%></td>
							                    <td>
							                        <input type="checkbox" id="add_bss_fh_cbox" onClick="validate_bss_fonthaul()" />
							                    </td>
							                    <td></td>
							                </tr>
											
											<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
											<tr>
													<td><%tcWebApi_get("String_Entry","EasyMeshBSS22Test","s")%></td>
													<td>
															<input type="text" name="fhVlanId" id="add_fh_vlan_id" value="" onkeyup="value=value.replace(/[^\d]/g,'')"/>																		
													</td>
													<td>
															<p>(vaild values:3-4094,disabled value:4095)</p>
													</td>
											</tr>
											<tr>
													<td><%tcWebApi_get("String_Entry","EasyMeshBSS23Test","s")%></td>
													<td>
															<input type="checkbox" name="primaryVlan" id="add_primary_vlan" onClick="validate_primary_Vlan()" />
													</td>
													<td></td>
											</tr>
											<tr>										
													<td><%tcWebApi_get("String_Entry","EasyMeshBSS24Test","s")%></td>
													<td>
															<input type="text" name="defaultPcp" id="add_default_pcp" value="" onkeyup="value=value.replace(/[^\d]/g,'')"/>
													</td>
													<td>
															<p>(vaild values:0-7)</p>
													</td>
											</tr>
											<% end if %>
							            </tbody>
							            <tbody>
							            		<tr colspan="3">
							                		<strong>
														<span style="color:red;"><%tcWebApi_get("String_Entry","EasyMeshBSS13Test","s")%></span> <%tcWebApi_get("String_Entry","EasyMeshBSS17Test","s")%> <span style="color:red;"><%tcWebApi_get("String_Entry","EasyMeshBSS18Test","s")%></span> <%tcWebApi_get("String_Entry","EasyMeshBSS19Test","s")%> <span style="color:red;">OPEN</span> or <span style="color:red;">WPA2 - Personal</span>  or <span style="color:red;">WPA3 SAE</span>  or <span style="color:red;">WPA3 Transition</span><span> (ssid is not fronthaul) </span>,
														<%tcWebApi_get("String_Entry","EasyMeshBSS4Test","s")%> <span style="color:red;"><%tcWebApi_get("String_Entry","EasyMeshBSS18Test","s")%></span> <%tcWebApi_get("String_Entry","EasyMeshBSS19Test","s")%> <span style="color:red;">NONE</span> or <span style="color:red;">AES</span>
								        			</strong>
							                </tr>
							            </tbody>
							        </table>
							        <div id="add_bss_cfgs_validate_almac_band_div" style="display:none">
							            <input type="button" value="Validate AL-MAC & Band" onclick="addBssPreConditionCheck()" />
							        </div>
							        <div id="add_bss_cfgs_form_submit_div" style="display:none">
							            <input value="<%tcWebApi_get("String_Entry","EasyMeshBSS10Test","s")%>" type="button" onclick="bss_Save()" />
							            <input value="<%tcWebApi_get("String_Entry","EasyMeshBSS11Test","s")%>" type="button" onclick="bss_Reset()" />
							        </div>
							    </form>
							
							    <div class="alert-message" id='no_bss_cfg_msg_div' style="display:none">
							        <strong>No BSS Configuration has been added yet!</strong>
							    </div>
							
							    <div width="470" id='cfg_bss_info_hdr_fset' style='display:none'>
							        <legend>
							            <B><%tcWebApi_get("String_Entry","EasyMeshBSS1Test","s")%></B>
							        </legend>
							        <div width="470" id="bss_overview_div">
							            <!-- JavaScript will create table with BSS Information -->
							        </div>
							    </div>
							
							</div>
							
							<script type="text/javascript">

									var cfgsJs_global;
									var isBackHaul = 0;
									var macRegExp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
									var oldBackHaul = -1;
									var oldFrontHaul = -1;

									function JSO_Only_Status(staType)
      						{
      								var ret = ' { ' + 
																		'"status": "' + staType + '"' + 
																' } ';
      								return ret;
      						}

      						function validate_band(band)
      						{
      								var ret = 0;
      								switch(band)
      								{
      										case "8x":
													case "11x":
													case "12x":
														ret = 1;
														break;
													default:
														ret = 0;
														break;
      								}
      								return ret;
      						}
      						
      						function validate_easymshe_bss(cfgsJs, alMac, band)
      						{
      								var numBss = 0;
      								var ret = '';

      								if(cfgsJs[alMac] == null)
      								{
      										ret = '{ "status": "SUCCESS" }';
      								}
      								else if(cfgsJs[alMac][band] == null)
      								{
      										ret = '{ "status": "SUCCESS" }';
      								}
      								else
      								{
      										numBss = 0;
      										for(var bssIndex in cfgsJs[alMac][band])
      										{
      												numBss++;
      										}
      										if(numBss >= 16)
      										{
      												ret = '{ "status": "No more BSS could be added!" }';
      										}
      										else
      										{
      												ret = '{ "status": "SUCCESS" }';
      										}
      								}

      								return ret;
      						}

      						function JSO_validate_add_easymesh_bss_req(alMac, band)
      						{
      								var ret = '';
      								//var macRegExp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
      								if (!macRegExp.test(alMac))
      								{
      										ret = '{ ';
      										ret = ret + '"status": "Invalid AL-MAC: '+ alMac + ' !"';
      										ret = ret + ' }';
      								}
      								else if(0 == validate_band(band))
      								{
      										ret = '{ ';
      										ret = ret + '"status": "Invalid Band: '+ band + ' !"';
      										ret = ret + ' }';
      								}
      								else
      								{
      										alMac = alMac.toUpperCase();
      										if(alMac == "FF:FF:FF:FF:FF:FF")
      										{
      												ret = validate_easymshe_bss(cfgsJs_global['wildCardAlMacCfgs'], alMac, band);
      										}
      										else
      										{
      												ret = validate_easymshe_bss(cfgsJs_global['distinctAlMacCfgs'], alMac, band);
      										}
      								}

      								return ret;
      						}

      						function JSO_remove_easymesh_bss_cfg_req()
      						{
      								var staType = 'SUCCESS';
											var ret = JSO_Only_Status(staType);
      								return ret;
      						}

      						function getX_Response(type)
      						{
      								var X_Res='';
      								try
      								{
      									switch(type)
      									{
      											case "remove_easymesh_bss_cfg_req":
      												X_Res = JSO_remove_easymesh_bss_cfg_req();
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

									function bss_Reset()
      						{
      								document.getElementById("add_bss_almac_inp_text").value = "FF:FF:FF:FF:FF:FF";
											var bandSel = document.getElementById("add_bss_band_sel");
											var idx = document.getElementById("add_bss_band_sel_hid").value;
											bandSel.options[idx].selected = true;
											document.getElementById("add_bss_ssid_inp_text").value = document.getElementById("add_bss_ssid_inp_text_hid").value;
											var authModeSel = document.getElementById("add_bss_auth_mode_sel");
											authModeSel.options[0].selected = true;
											var endTypeSel = document.getElementById("add_bss_enc_type_sel");
											endTypeSel.options[0].selected = true;
											document.getElementById("add_bss_pass_phrase_inp_text").value = "";
											document.getElementById("add_bss_is_ssid_hidden_cbox").checked = false;
											document.getElementById("add_bss_bh_cbox").checked = false;
											document.getElementById("add_bss_fh_cbox").checked = false;
      						}
      						
							    function checkInjection(str)
							    {
							        for (var i=0; i < str.length; i++) 
							        {
							            if ( str.charAt(i) == '\r' || str.charAt(i) == '\n' )
							            {
							                return false;
							            }
							        }
							        return true;
							    }

								function get_backhaul_bss_count(bandList)
								{
									var i, tmpIndex, backhaul_bss_count = 0;
									
									for(i = 0; i < 8; i++)
									{
										tmpIndex = i + 1;
										if(bandList[tmpIndex] != null)
										{
											if(bandList[tmpIndex]['isBhBssSupported'] != null)
											{
												if(1 == bandList[tmpIndex]['isBhBssSupported'])
												{
													backhaul_bss_count++;
												}
											}
										}
									}
									return backhaul_bss_count;
								}

								function checkBackHaulBssCount()
								{
									var checkRet = 0;
									var tmpMac = "FF:FF:FF:FF:FF:FF";
									var tmpband, backhaul_bss_count_8x, backhaul_bss_count_11x, tmpIndex, i;
									
									if(cfgsJs_global['wildCardAlMacCfgs'] != null)
									{
										if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac] != null)
										{
											tmpband = "8x";
											backhaul_bss_count_8x = 0;
											if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac][tmpband] != null)
											{
												backhaul_bss_count_8x = get_backhaul_bss_count(cfgsJs_global['wildCardAlMacCfgs'][tmpMac][tmpband]);
											}
											else
											{
												/*band not exist*/
												backhaul_bss_count_8x = -1;
											}

											tmpband = "11x";
											backhaul_bss_count_11x = 0;
											if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac][tmpband] != null)
											{
												backhaul_bss_count_11x = get_backhaul_bss_count(cfgsJs_global['wildCardAlMacCfgs'][tmpMac][tmpband]);
											}
											else
											{
												/*band not exist*/
												backhaul_bss_count_11x = -1;
											}

											if( ((-1 != backhaul_bss_count_8x) && (1 != backhaul_bss_count_8x))
												|| ((-1 != backhaul_bss_count_11x) && (1 != backhaul_bss_count_11x)) )
											{
												checkRet = 0;
											}
											else
											{
												checkRet = 1;
											}
										}
										else
										{
											checkRet = 0;
										}
									}
									else
									{
										checkRet = 0;
									}
									return checkRet;
								}
								function checkAuthModeCss()
								{
									var authModeSel = document.getElementById('add_bss_auth_mode_sel');
									var encTypeSel = document.getElementById('add_bss_enc_type_sel');
									var bhBox = document.getElementById('add_bss_bh_cbox').checked;
									var fhBox = document.getElementById('add_bss_fh_cbox').checked;		
									if (1 == bhBox)
									{
										if ( encTypeSel.value != '0x0008' && !(encTypeSel.value == '0x0001' && authModeSel.value  == '0x0001') )
										{	
											for (var idx=0; idx < encTypeSel.length; idx++) 
											{
												if (encTypeSel.options[idx].value == '0x0008')
												{
													encTypeSel.options[idx].selected = true;
													encTypeSel.options[idx].disabled = false;
												}
												else 
												{
													encTypeSel.options[idx].selected = false;
												}												
											}									
										}
										for (var idx=0; idx < authModeSel.length; idx++) 
										{
											/* OPEN, WPA2 - Personal,WPA3-SAE,WPA3-SAE Transition*/
											if (authModeSel.options[idx].value == '0x0001' || authModeSel.options[idx].value == '0x0020' || authModeSel.options[idx].value == '0x0030' || authModeSel.options[idx].value == '0x0031' )
											{
												authModeSel.options[idx].disabled = false;
											}
											else
											{
												authModeSel.options[idx].disabled = true;
											}
								  		}
										for (var idx=0; idx < encTypeSel.length; idx++) 
										{				                    
											// NONE, AES
											if ((encTypeSel.options[idx].value == '0x0001' && authModeSel.value  == '0x0001') || (authModeSel.value != '0x0001' && encTypeSel.options[idx].value == '0x0008'))
											{
												encTypeSel.options[idx].disabled = false;
											}
											else
											{
												encTypeSel.options[idx].disabled = true;
											}
										}							        		
									}
									if(1 == fhBox)
									{	
										if ( authModeSel.value == '0x0030')
										{	
											for (var idx=0; idx < authModeSel.length; idx++) 
											{
												if (authModeSel.options[idx].value == '0x0001')
												{
													authModeSel.options[idx].selected = true;
													authModeSel.options[idx].disabled = false;
												}												
											}
											AuthModeOnChange("0x0001");										
										}
										if ( encTypeSel.value != '0x0008' && !(encTypeSel.value == '0x0001' && authModeSel.value  == '0x0001') )
										{	
											for (var idx=0; idx < encTypeSel.length; idx++) 
											{
												if (encTypeSel.options[idx].value == '0x0008')
												{
													encTypeSel.options[idx].selected = true;
													encTypeSel.options[idx].disabled = false;
												}	
												else 
												{
													encTypeSel.options[idx].selected = false;
												}												
											}									
										}
										for (var idx=0; idx < authModeSel.length; idx++) 
										{
											/* OPEN, WPA2 - Personal,WPA3-SAE Transition*/
											if (authModeSel.options[idx].value == '0x0001' || authModeSel.options[idx].value == '0x0020' || authModeSel.options[idx].value == '0x0031' )
											{
												authModeSel.options[idx].disabled = false;
											}
											else
											{
												authModeSel.options[idx].disabled = true;
											}
								 		}
										for (var idx=0; idx < encTypeSel.length; idx++) 
										{				                    
											/* NONE, AES*/
											if ((encTypeSel.options[idx].value == '0x0001' && authModeSel.value == '0x0001') || (authModeSel.value != '0x0001' && encTypeSel.options[idx].value == '0x0008'))
											{
												encTypeSel.options[idx].disabled = false;
											}
											else
											{
												encTypeSel.options[idx].disabled = true;
											}
										}							        		
									}
									if( 0 == fhBox && 0 == bhBox )
									{
										if ( authModeSel.value == '0x0001' )
										{											
											for (var idx=0; idx < encTypeSel.length; idx++) 
											{
												// OPEN
												if (encTypeSel.options[idx].value == '0x0001')
												{
													encTypeSel.options[idx].selected = true;
													encTypeSel.options[idx].disabled = false;
												}
												else
												{
													encTypeSel.options[idx].selected = false;
													encTypeSel.options[idx].disabled = true;
												}
											}	
										}
										else
										{
											for (var idx=0; idx < encTypeSel.length; idx++) 
											{												
												// !OPEN
												if (encTypeSel.options[idx].value != '0x0001')
												{													
													encTypeSel.options[idx].disabled = false;
												}
												else
												{
													encTypeSel.options[idx].selected = false;
													encTypeSel.options[idx].disabled = true;
												}												
											}	
										}
										for (var idx=0; idx < authModeSel.length; idx++) 
										{
											authModeSel.options[idx].disabled = false;											
										}
										
									}
									if ( authModeSel.value == '0x0030' || authModeSel.value == '0x0031' )
									{
										for (var idx=0; idx < encTypeSel.length; idx++) 
										{
											// AES
											if (encTypeSel.options[idx].value == '0x0008')
											{
												encTypeSel.options[idx].selected = true;
												encTypeSel.options[idx].disabled = false;
											}
											else
											{
												encTypeSel.options[idx].selected = false;
												encTypeSel.options[idx].disabled = true;
											}
										}									
									}
									EncryptionTypeOnChange(encTypeSel.value);									
								}

								function validate_bss_backhaul()
								{
									var tmpMac = "FF:FF:FF:FF:FF:FF";
									var tmpband = document.getElementById('add_bss_band_sel').value;
									var bss_bh_ctl = document.getElementById('add_bss_bh_cbox');
									var curIndex = document.getElementById('edit_remove_bss_bssindex_hid_inp').value;
									var tmpIndex = 0, i = 0;
									var bandList;
																
									if(bss_bh_ctl.checked)
									{
										if(cfgsJs_global['wildCardAlMacCfgs'] != null)
										{
											if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac] != null)
											{
												if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac][tmpband] != null)
												{
													bandList = cfgsJs_global['wildCardAlMacCfgs'][tmpMac][tmpband];
													for(i = 0; i < 8; i++)
													{
														tmpIndex = i + 1;
														if(curIndex == tmpIndex)
														{
															continue;
														}
														
														if(bandList[tmpIndex] != null)
														{
															if(bandList[tmpIndex]['isBhBssSupported'] != null)
															{
																if(1 == bandList[tmpIndex]['isBhBssSupported'])
																{
																	bss_bh_ctl.checked = false;
																	alert("<%tcWebApi_get("String_Entry","EasyMeshBSSAlert1Test","s")%>");
																	return;
																}
															}
														}
													}
												}
											}
										}										
									}	
									checkAuthModeCss();
								}		
								function validate_bss_fonthaul()
								{					
									checkAuthModeCss();
								}	
								function validate_primary_Vlan()
								{									
									var tmpMac = "FF:FF:FF:FF:FF:FF";
									var tmpband = document.getElementById('add_bss_band_sel').value;
									var primary_vlan_ctl = document.getElementById('add_primary_vlan');
									var default_pcp_ctl = document.getElementById('add_default_pcp');
									var curIndex = document.getElementById('edit_remove_bss_bssindex_hid_inp').value;		
									var tmpIndex = 0, i = 0;
									var bandList;
									var trueCount = 0;
									if(primary_vlan_ctl.checked)
									{
										$("#add_default_pcp").attr('disabled',false);
										if(cfgsJs_global['wildCardAlMacCfgs'] != null)
										{																				
											if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac].hasOwnProperty('8x'))
											{	
												bandList=cfgsJs_global['wildCardAlMacCfgs'][tmpMac]['8x']
												for(var bssIdx in bandList)
												{
													var bssInfo = bandList[bssIdx];
													if ((bssInfo['primaryVlan']).toString() == "1" )
													{
														if (tmpband == "8x" && curIndex == bssIdx)
														{
															continue;
														}
														else
														{
															trueCount++;	
														}				
													}													
												}											
											}
											if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac].hasOwnProperty('11x'))
											{
												bandList=cfgsJs_global['wildCardAlMacCfgs'][tmpMac]['11x']
												for(var bssIdx in bandList)
												{
													var bssInfo = bandList[bssIdx];
													if ((bssInfo['primaryVlan']).toString() == "1" )
													{
														if (tmpband == "11x" && curIndex == bssIdx)
														{
															continue;
														}
														else
														{
															trueCount++;	
														}														
													}													
												}
											}
											if (trueCount >0)
											{
												alert("<%tcWebApi_get("String_Entry","EasyMeshBSSAlert15Test","s")%>");
											}				
										}
									}
									else{
										alert("<%tcWebApi_get("String_Entry","EasyMeshBSSAlert20Test","s")%>");
										$("#add_fh_vlan_id").val("4095");
										$("#add_default_pcp").val("");
										$("#add_default_pcp").attr('disabled',true);										
									}
								}						
									function isValidWPAPskKey(val)
									{
										var ret = false;
										var len = val.length;
										var maxSize = 64;
										var minSize = 8;
										if ( len >= minSize && len < maxSize )
										{
											if(val.match(/[^\x00-\xff]/g))
									   	    {
												ret = false;
											 } else {
									   	        for ( i = 0; i < len; i++)
													if (val.charAt(i) == ','){
													break;
													}
												if ( i == len)
													ret = true;
									   	    }
										} else if ( len == maxSize ) {
											for ( i = 0; i < maxSize; i++ )
											if ( isHexaDigit(val.charAt(i)) == false )
											break;
											if ( i == maxSize )
											ret = true;
										} else
											ret = false;
										return ret;
									}
									function checkNotNumber(value)
									{
										if (parseFloat(value).toString() == "NaN")
										{
											return true;	
										}
										else
										{
											return false;
										}
									}
							    function validate_security()
							    {
							        var authModeSel = document.getElementById('add_bss_auth_mode_sel');
							        var encTypeSel = document.getElementById('add_bss_enc_type_sel');
							        var passPhraseId = document.getElementById('add_bss_pass_phrase_inp_text');
							        var passPhrase = passPhraseId.value;
									
									var tmpMac = "FF:FF:FF:FF:FF:FF";
									var tmpband = document.getElementById('add_bss_band_sel').value;
									var curIndex = document.getElementById('edit_remove_bss_bssindex_hid_inp').value;		
									var bandList;
									var trueCount = 0;
									
									<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
										var fhVlanId = document.getElementById('add_fh_vlan_id'); 
										var primaryVlan = document.getElementById('add_primary_vlan').checked;
										var defaultPcp = document.getElementById('add_default_pcp');								
									<% end if %>
									
									var bhBox = document.getElementById('add_bss_bh_cbox').checked;
									var fhBox = document.getElementById('add_bss_fh_cbox').checked;
											
							        if(authModeSel.value == '0x0001' && encTypeSel.value == '0x0001')
							        {
							            //AUTH_MODE = OPEN and ENCRYPTION_TYPE = NONE
							            passPhraseId.value = '';
							        }
							        else if((authModeSel.value == '0x0002' || authModeSel.value == '0x0020' || authModeSel.value == '0x0022'|| authModeSel.value == '0x0030'|| authModeSel.value == '0x0031') && 
							        (encTypeSel.value == '0x0004' || encTypeSel.value == '0x0008' || encTypeSel.value == '0x000c'))
							        {
							            //AUTH_MODE = WPA2-Personal and ENCRYPTION_TYPE = AES
							            if (passPhrase.length == 0)
							            {
							                alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert2Test","s")%>\n' +
							                    '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert3Test","s")%>\n' +
							                    '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							                passPhraseId.select();
							                passPhraseId.focus();
							                return false;
							            }
							            if (passPhrase.length < 8)
							            {
							                alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert5Test","s")%>\n' +
							                    '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert3Test","s")%>\n' +
							                    '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							                passPhraseId.select();
							                passPhraseId.focus();
							                return false;
							            }
							            if (isValidWPAPskKey(passPhrase) == false )
													{
														alert('WPA  <%tcWebApi_get("String_Entry","PreliminaryKeyText","s")%>');
														passPhraseId.select();
							              passPhraseId.focus();
														return false;
													}
							            var nonHexPat = /[^A-Fa-f0-9]/;
							            if(nonHexPat.test(passPhrase))
							            {
							                // ASCII
							                if (passPhrase.length > 63) 
							                {
							                    alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert6Test","s")%>\n' +
							                        '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert3Test","s")%>\n' +
							                        '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							                    passPhraseId.select();
							                    passPhraseId.focus();
							                    return false;
							                }
							            }
							            else
							            {
							                // Hexadecimal
							                if (passPhrase.length > 64 )
							                {
							                    alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert6Test","s")%>\n' +
							                        '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert3Test","s")%>\n' +
							                        '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							                    passPhraseId.select();
							                    passPhraseId.focus();
							                    return false;
							                }
							            }
							            if(checkInjection(passPhrase) == false)
							            {
							                alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert7Test","s")%>\n' +
							                   '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert3Test","s")%>\n' +
							                   '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							                passPhraseId.select();
							                passPhraseId.focus();
							                return false;
							            }
							        }
									/*
							        else
							        {
							            alert('Invalid security settings!\n'+
							                'Please select valid Authentication Mode and Encryption Type.');
							            authModeSel.focus();
							            return false;
							        }
							        */										
							        if(bhBox && fhBox && !(authModeSel.value == '0x0001' || authModeSel.value == '0x0020' || authModeSel.value == '0x0031'))
							        {
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert18Test","s")%>\n');
							            return false;
							        }		
							        if(bhBox && !fhBox && !(authModeSel.value == '0x0001' || authModeSel.value == '0x0020' || authModeSel.value == '0x0030' || authModeSel.value == '0x0031'))
							        {						
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert17Test","s")%>\n');
							            return false;
							        }			
							         if(!bhBox && fhBox && !(authModeSel.value == '0x0001' || authModeSel.value == '0x0020' || authModeSel.value == '0x0031'))
							        {    
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert18Test","s")%>\n');
							            return false;
							        }	
									if((bhBox || fhBox) && encTypeSel.value != '0x0008' && !(encTypeSel.value == '0x0001' && authModeSel.value == '0x0001') )
							        {    
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert19Test","s")%>\n');
							            return false;
							        }									
							        	
									<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
										if (checkNotNumber(fhVlanId.value) || (parseInt(fhVlanId.value)< 3 || parseInt(fhVlanId.value) > 4095))
										{
											if ((fhVlanId.value).toString() != "")
											{
												alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert13Test","s")%>\n');
												return false;
											}		        		
										}
										else
										{
											if (parseInt(fhVlanId.value) != 4095 && !primaryVlan)
											{
												if(cfgsJs_global['wildCardAlMacCfgs'] != null)
												{																				
													if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac].hasOwnProperty('8x'))
													{	
														bandList=cfgsJs_global['wildCardAlMacCfgs'][tmpMac]['8x']
														for(var bssIdx in bandList)
														{
															var bssInfo = bandList[bssIdx];
															if ((bssInfo['primaryVlan']).toString() == "1" )
															{
																if (tmpband == "8x" && curIndex == bssIdx)
																{
																	continue;
																}
																else
																{
																	trueCount++;	
																}				
															}													
														}											
													}
													if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac].hasOwnProperty('11x'))
													{
														bandList=cfgsJs_global['wildCardAlMacCfgs'][tmpMac]['11x']
														for(var bssIdx in bandList)
														{
															var bssInfo = bandList[bssIdx];
															if ((bssInfo['primaryVlan']).toString() == "1" )
															{
																if (tmpband == "11x" && curIndex == bssIdx)
																{
																	continue;
																}
																else
																{
																	trueCount++;	
																}														
															}													
														}
													}
													if (trueCount == 0)
													{
														alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert21Test","s")%>\n');
														return false;
													}				
												}
											}
										}
										if (checkNotNumber(defaultPcp.value) || (parseInt(defaultPcp.value)  < 0 || parseInt(defaultPcp.value) > 7))
										{
											if ((defaultPcp.value).toString() != "")
											{
												alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert14Test","s")%>\n');
												return false;
											}								             
										}				
									<% end if %>
							        return true;
							    }

							    function validate_ssid()
							    {
							        var ssidInp = document.getElementById("add_bss_ssid_inp_text");
							        var ssid = ssidInp.value;
							        if(ssid.length == 0)
							        {
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert8Test","s")%>\n' +
							                '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert9Test","s")%>\n' +
							                '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							            ssidInp.select();
							            ssidInp.focus();
							            return false;
							        }
							        if (isValidAscii(ssid) != '')
							        {
							        	alert("SSID<%tcWebApi_get("String_Entry","InvalidCharacterText","s")%> '" + isValidAscii(ssid) + "'");
												ssidInp.select();
							          ssidInp.focus();
												return false;
							        }
							        if(ssid.length > 32)
							        {
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert10Test","s")%>\n' +
							                '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert9Test","s")%>\n' +
							                '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							            ssidInp.select();
							            ssidInp.focus();
							            return false;
							        }
							        if(checkInjection(ssid) == false)
							        {
							            alert('<%tcWebApi_get("String_Entry","EasyMeshBSSAlert11Test","s")%>\n' +
							                '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert9Test","s")%>\n' +
							                '<%tcWebApi_get("String_Entry","EasyMeshBSSAlert4Test","s")%>');
							            ssidInp.select();
							            ssidInp.focus();
							            return false;
							        }
							        return true;
							    }

							    function validate_al_mac()
							    {
							        var alMacInp = document.getElementById('add_bss_almac_inp_text');
							        //var macRegExp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
							        if (!macRegExp.test(alMacInp.value)) 
							        {
							            alert("<%tcWebApi_get("String_Entry","EasyMeshBSSAlert12Test","s")%>");
							            alMacInp.focus();
							            alMacInp.select();
							            return false;
							        }
							        return true;
							    }

							    function validate_all()
							    {
							        if (validate_al_mac() &&
							            validate_ssid() &&
							            validate_security())
							        {
							            var fhChbox = document.getElementById('add_bss_fh_cbox');
							            var fhHdInp = document.getElementById('add_bss_fh_hid_inp');
							            var bhChbox = document.getElementById('add_bss_bh_cbox');
							            var bhHdInp = document.getElementById('add_bss_bh_hid_inp');
							            
							            var isSsidHiddenChbox = document.getElementById('add_bss_is_ssid_hidden_cbox');
							            var isSsidHiddenHdInp = document.getElementById('add_bss_is_ssid_hidden_hid_inp');
							            fhChbox.checked ? fhHdInp.value = "1" : fhHdInp.value = "0";
							            bhChbox.checked ? bhHdInp.value = "1" : bhHdInp.value = "0";
							            if ((-1 != oldBackHaul && oldBackHaul != bhHdInp.value) || (-1 != oldFrontHaul && oldFrontHaul != fhHdInp.value))
							            {
							            	document.getElementById('restore_mesh').value = 1;
							            }
							            	
										<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
											var primaryVlanChbox = document.getElementById('add_primary_vlan');
											primaryVlanChbox.checked ? primaryVlanChbox.value = "1" : primaryVlanChbox.value = "0";										
										<% end if %>										
							            isSsidHiddenChbox.checked ? isSsidHiddenHdInp.value = "hidden-Y" : isSsidHiddenHdInp.value = "hidden-N";
							            document.getElementById('bss_cfgs_display').style.display = 'none';
							            document.getElementById('bss_cfgs_loading_div').style.display = '';
							            return true;
							        }
							        return false;
							    }

							    function AuthModeOnChange(authModeCode)
							    {
							        var encTypeSel = document.getElementById('add_bss_enc_type_sel');
									isBackHaul = document.getElementById('add_bss_bh_cbox').checked ? "1" : "0";
							        isFontHaul = document.getElementById('add_bss_fh_cbox').checked ? "1" : "0";
							        switch(authModeCode)
							        {
							            case "0x0001":  // OPEN
							                for (var idx=0; idx < encTypeSel.length; idx++) 
							                {
							                    // NONE
							                    if (encTypeSel.options[idx].value == '0x0001')
							                    {
							                        encTypeSel.options[idx].selected = true;
							                        encTypeSel.options[idx].disabled = false;
							                    }
							                    else
							                    {
							                        encTypeSel.options[idx].selected = false;
							                        encTypeSel.options[idx].disabled = true;
							                    }
							                }
							                break;
							            case "0x0002": // WPAPSK
							            case "0x0020": // WPA2 - Personal
							            case "0x0022": // WPAPSKWPA2PSK
							                for (var idx=0; idx < encTypeSel.length; idx++) 
							                {
							                    // NONE
							                    if (encTypeSel.options[idx].value == '0x0001')
							                    {
							                        encTypeSel.options[idx].selected = false;
							                        encTypeSel.options[idx].disabled = true;
							                    }
							                    else if (( 1 == isBackHaul || 1 == isFontHaul) && (encTypeSel.options[idx].value == '0x0004' || encTypeSel.options[idx].value == '0x000c'))
							                    {
							                        encTypeSel.options[idx].selected = false;
							                        encTypeSel.options[idx].disabled = true;
							                    }
							                    else
							                    {
													if (( 1 == isBackHaul || 1 == isFontHaul))
													{
														encTypeSel.options[idx].selected = true;
													}							                        
							                        encTypeSel.options[idx].disabled = false;
							                    }
							                }
							                break;
										case "0x0030": // WPA3SAE
										case "0x0031": // WPA3SAETransition
							                for (var idx=0; idx < encTypeSel.length; idx++) 
							                {
							                    // NONE
							                    if (encTypeSel.options[idx].value == '0x0008')
							                    {
							                        encTypeSel.options[idx].selected = true;
							                        encTypeSel.options[idx].disabled = false;
							                    }
							                    else
							                    {
							                        encTypeSel.options[idx].selected = false;
							                        encTypeSel.options[idx].disabled = true;
							                    }
							                }
							                break;
							            default:
							                console.log("AuthModeOnChange: unknown Authentication Code: ",authModeCode);
							        }
							        EncryptionTypeOnChange(encTypeSel.value);
							    }

							    function EncryptionTypeOnChange(encType)
							    {
							        var passPhraseInp = document.getElementById('add_bss_pass_phrase_inp_text');
							        switch(encType)
							        {
							            case "0x0001": // NONE
							                passPhraseInp.parentNode.parentNode.style.display = 'none';
							                break;
							            case "0x0004": // TKIP
							            case "0x0008": // AES
							            case "0x000c": // TKIPAES
							                passPhraseInp.parentNode.parentNode.style.display = '';
							                break;
							            default:
							                return "Unknown Encryption Code: " + encType;
							        }
							    }

							    function addNewBssCfgs()
							    {
							        document.getElementById('add_bss_cfgs_form').style.display = '';
							        document.getElementById('add_bss_cfgs_fields_tbody').style.display = 'none';
							        var alMacInp = document.getElementById('add_bss_almac_inp_text');
							        alMacInp.value = '';
							        alMacInp.readOnly = false;
							        var bandSel = document.getElementById('add_bss_band_sel');
							        for (var idx=0; idx < bandSel.length; idx++) 
							        {
							            bandSel.options[idx].disabled = false;
							        }
							        bandSel.selectedIndex = 0;
							        document.getElementById('add_bss_cfgs_validate_almac_band_div').style.display = '';
							        document.getElementById('add_bss_cfgs_form_submit_div').style.display = 'none';
							    }

							    function addBssPreConditionCheck()
							    {
							        var bandSel = document.getElementById('add_bss_band_sel');
							        var alMacInp = document.getElementById('add_bss_almac_inp_text');
							        if (!validate_al_mac()) 
							        {
							            return;
							        }
							        addBssCfg(alMacInp.value,bandSel.value);
							    }

							    function addBssCfg(alMac,band)
							    {
							        /* Value of __IS_EDIT should be set to 1 only by onClick Edit button Action. */
							        document.getElementById('edit_bss_hid_inp').value = "0";
							        document.getElementById('edit_bss_ssid_hid_inp').value = '';
							        document.getElementById('edit_bss_auth_mode_hid_inp').value = '';
							        document.getElementById('edit_bss_enc_type_hid_inp').value = '';
							        document.getElementById('edit_bss_pass_phrase_hid_inp').value = '';
							        document.getElementById('edit_bss_bh_support_hid_inp').value = '';
							        document.getElementById('edit_bss_fh_support_hid_inp').value = '';
							        document.getElementById('edit_bss_is_ssid_hidden_hid_inp').value = '';
							        document.getElementById('bss_cfgs_display').style.display = 'none';
							        document.getElementById('bss_cfgs_loading_div').style.display = '';
							        document.getElementById('Add_Flag').value = 1;
							        document.getElementById('Edit_Flag').value = 0;
							        document.getElementById('Remove_Flag').value = 0;

											var x_response = JSO_validate_add_easymesh_bss_req(alMac,band);
					            {
							            try
							            {
							                var r = JSON.parse(x_response);
							                if(r['status'] == 'SUCCESS')
							                {
							                    document.getElementById('add_bss_cfgs_form').style.display = '';
							                    document.getElementById('add_bss_cfgs_fields_tbody').style.display = '';
							                    var alMacInp = document.getElementById('add_bss_almac_inp_text');
							                    alMacInp.value = alMac;
							                    alMacInp.readOnly = true;
							                    var bandSel = document.getElementById('add_bss_band_sel');
							                    for (var idx=0; idx < bandSel.length; idx++) 
							                    {
							                        if (bandSel.options[idx].value == band)
							                        {
							                            bandSel.options[idx].selected = true;
							                            bandSel.options[idx].disabled = false;
							                        }
							                        else
							                        {
							                            bandSel.options[idx].selected = false;
							                            bandSel.options[idx].disabled = true;
							                        }
							                    }
							                    document.getElementById('add_bss_ssid_inp_text').value = '';
							                    document.getElementById('add_bss_auth_mode_sel').selectedIndex = 0;
							                    document.getElementById('add_bss_enc_type_sel').selectedIndex = 0;
							                    document.getElementById('add_bss_pass_phrase_inp_text').value = '';
							                    document.getElementById('add_bss_bh_cbox').checked = false;
							                    document.getElementById('add_bss_fh_cbox').checked = false;
							                    document.getElementById('add_bss_is_ssid_hidden_cbox').checked = false;
							                    AuthModeOnChange(document.getElementById('add_bss_auth_mode_sel').value);
							                    document.getElementById('add_bss_cfgs_validate_almac_band_div').style.display = 'none';
							                    document.getElementById('add_bss_cfgs_form_submit_div').style.display = '';
							                }
							                else
							                {
							                    alert('No more BSS could be added in ' +
							                        (band == '8x' ? '2G' : band == '11x' ? '5GL' : '5GH') +
							                        ' band of Device with AL-MAC ' + alMac + ' !\nStatus: ' + r['status']);
							                    location.reload(true);
							                }
							            }
							            catch(e)
							            {
							                alert("Failed to add a new BSS!\nERROR: Incorrect response received!");
							                console.log("Failed to add a new BSS!\nERROR: [" + e.name + "] " + e.message);
							                location.reload(true);
							            }
							            finally
							            {
							                document.getElementById('bss_cfgs_display').style.display = '';
							                document.getElementById('bss_cfgs_loading_div').style.display = 'none';
							            }
							        }
							    }

							    function editBssCfg(alMac,band,bssInfoGrpObj,btnIdStr)
							    {
							        var bssInfo;
							        var bssIdx;
							        if(typeof(btnIdStr) == 'string')
							        {
							            var bssIdxArr = btnIdStr.match(/\d{1,}$/);
							            if(bssIdxArr)
							            {
							                bssIdx = bssIdxArr[0];
							            }
							        }
							        if(typeof(bssIdx) == 'string' && typeof(bssInfoGrpObj) == 'object')
							        {
							            bssInfo = bssInfoGrpObj[bssIdx];
							        }
							        if(typeof(bssInfo) != 'object' ||
							            !bssInfo.hasOwnProperty('ssid') ||
							            !bssInfo.hasOwnProperty('authMode') ||
							            !bssInfo.hasOwnProperty('encType') ||
							            !bssInfo.hasOwnProperty('passPhrase') ||
							            !bssInfo.hasOwnProperty('isBhBssSupported') ||
							            !bssInfo.hasOwnProperty('isFhBssSupported') ||
							            !bssInfo.hasOwnProperty('isHidden'))
							        {
							            alert('BSS could not be edited!\nIt does not have all required information to edit!');
							            location.reload(true);
							            return;
							        }
							        isBackHaul = parseInt(bssInfo['isBhBssSupported']);
							        isFontHaul = parseInt(bssInfo['isFhBssSupported']);
									document.getElementById('add_bss_bh_cbox').checked = (parseInt(bssInfo['isBhBssSupported']) == 1) ? true : false;
							        document.getElementById('add_bss_fh_cbox').checked = (parseInt(bssInfo['isFhBssSupported']) == 1) ? true : false;
							        document.getElementById('add_bss_cfgs_form').style.display = '';
							        document.getElementById('add_bss_cfgs_fields_tbody').style.display = '';
							        var alMacInp = document.getElementById('add_bss_almac_inp_text');
							        alMacInp.value = alMac;
							        alMacInp.readOnly = true;
							        alMacInp.disabled = true;
							        var bandSel = document.getElementById('add_bss_band_sel');
							        for (var idx=0; idx < bandSel.length; idx++) 
							        {
							            if (bandSel.options[idx].value == band)
							            {
							                bandSel.options[idx].selected = true;
							                bandSel.options[idx].disabled = false;
							                document.getElementById('add_bss_band_sel_hid').value = idx;
							            }
							            else
							            {
							                bandSel.options[idx].selected = false;
							                bandSel.options[idx].disabled = true;
							            }
							        }
							        bandSel.disabled = true;
							        
							        var bandValDiv= document.getElementById('add_bss_band_val_div');
							        switch(band)
							        {
							        		case "8x":
							        			bandValDiv.innerHTML = "2G";
							        			break;
							        		case "11x":
							        		case "12x":
							        			bandValDiv.innerHTML = "5G";
							        			break;
							        		default:
							        			bandValDiv.innerHTML = "";
							        			break;
							        }
							        
							        document.getElementById('add_bss_ssid_inp_text').value = bssInfo['ssid'];
							        document.getElementById('add_bss_ssid_inp_text_hid').value = bssInfo['ssid'];
							        var authModeSel = document.getElementById('add_bss_auth_mode_sel');
							        for (var idx=0; idx < authModeSel.length; idx++) 
							        {
							            if (authModeSel.options[idx].value == bssInfo['authMode'])
							            {
							                authModeSel.options[idx].selected = true;
							            }
							            authModeSel.options[idx].disabled = false;
							        }
							        AuthModeOnChange(authModeSel.value);
							        var encTypeSel = document.getElementById('add_bss_enc_type_sel');
							        for (var idx=0; idx < encTypeSel.length; idx++) 
							        {
					        				if (encTypeSel.options[idx].value == bssInfo['encType'])
					            		{
					                		encTypeSel.options[idx].selected = true;
					                		encTypeSel.options[idx].disabled = false;
						            	}
							        }
							        EncryptionTypeOnChange(encTypeSel.value);
							        // Populate Pass-phrase field only when EncryptionType is not NONE.
							        if(encTypeSel.value != '0x0001')
							        {
							            document.getElementById('add_bss_pass_phrase_inp_text').value = bssInfo['passPhrase'];
							        }
							         if (parseInt(bssInfo['isBhBssSupported']) == 1)
							        {
							        	
							        }
							        if (1 == isBackHaul)
							        {
										for (var idx=0; idx < authModeSel.length; idx++) 
										{
											// OPEN, WPA2 - Personal
											if (authModeSel.options[idx].value == '0x0001' || authModeSel.options[idx].value == '0x0020' || authModeSel.options[idx].value == '0x0030' || authModeSel.options[idx].value == '0x0031' )
											{
												authModeSel.options[idx].disabled = false;
											}
											else
											{
												authModeSel.options[idx].disabled = true;
											}
										}
										for (var idx=0; idx < encTypeSel.length; idx++) 
										{				                    
											// NONE, AES
											if ((encTypeSel.options[idx].value == '0x0001' && bssInfo['authMode'] == '0x0001') || encTypeSel.options[idx].value == '0x0008')
											{
												encTypeSel.options[idx].disabled = false;
											}
											else
											{
												encTypeSel.options[idx].disabled = true;
											}
										}							        		
							        }
							        if(1 == isFontHaul)
							        {
										for (var idx=0; idx < authModeSel.length; idx++) 
										{
											// OPEN, WPA2 - Personal
											if (authModeSel.options[idx].value == '0x0001' || authModeSel.options[idx].value == '0x0020' || authModeSel.options[idx].value == '0x0031' )
											{
												authModeSel.options[idx].disabled = false;
											}
											else
											{
												authModeSel.options[idx].disabled = true;
											}
										}
										for (var idx=0; idx < encTypeSel.length; idx++) 
										{				                    
											// NONE, AES
											if ((encTypeSel.options[idx].value == '0x0001' && bssInfo['authMode'] == '0x0001') || encTypeSel.options[idx].value == '0x0008')
											{
												encTypeSel.options[idx].disabled = false;
											}
											else
											{
												encTypeSel.options[idx].disabled = true;
											}
										}							        		
									}
							        document.getElementById('add_bss_is_ssid_hidden_cbox').checked = (bssInfo['isHidden'] == 'hidden-Y') ? true : false;							       
							       	oldBackHaul = parseInt(bssInfo['isBhBssSupported']);
							        oldFrontHaul = parseInt(bssInfo['isFhBssSupported']);
							        <% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
										if ( bssInfo['primaryVlan'] == "1" || bssInfo['primaryVlan'] == "Enable" )
										{
											 document.getElementById('edit_primary_vlan_inp').checked = true;
											 document.getElementById('add_primary_vlan').checked = true;
											 $("#add_default_pcp").attr('disabled',false);
										}
										else
										{
											document.getElementById('edit_primary_vlan_inp').checked = false;
											document.getElementById('add_primary_vlan').checked = false;
											$("#add_default_pcp").val("");
											$("#add_default_pcp").attr('disabled',true);	
										}
									<% end if %>
									
							        /* Value of __IS_EDIT should be set to 1 here. */
							        document.getElementById('edit_bss_hid_inp').value = "1";
							        document.getElementById('edit_bss_ssid_hid_inp').value = bssInfo['ssid'];
							        document.getElementById('edit_bss_auth_mode_hid_inp').value = bssInfo['authMode'];
							        document.getElementById('edit_bss_enc_type_hid_inp').value = bssInfo['encType'];
							        document.getElementById('edit_bss_pass_phrase_hid_inp').value = bssInfo['passPhrase'];
							        document.getElementById('edit_bss_is_ssid_hidden_hid_inp').value = bssInfo['isHidden'];
							        document.getElementById('edit_bss_bh_support_hid_inp').value = bssInfo['isBhBssSupported'];
							        document.getElementById('edit_bss_fh_support_hid_inp').value = bssInfo['isFhBssSupported'];
							        document.getElementById('edit_remove_bss_bssindex_hid_inp').value = bssInfo['NodeIndex'];						        
							        <% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
										document.getElementById('edit_fh_vlan_id_inp').value = bssInfo['fhVlanId']; 
										document.getElementById('edit_default_pcp_inp').value = bssInfo['defaultPcp'];									
										document.getElementById('add_fh_vlan_id').value = bssInfo['fhVlanId']; 	
										if(document.getElementById('add_primary_vlan').checked)
										{
											document.getElementById('add_default_pcp').value = bssInfo['defaultPcp'];	
											$("#add_default_pcp").attr('disabled',false);
										}
										else
										{
											$("#add_default_pcp").val("");
											$("#add_default_pcp").attr('disabled',true);
										}
																		
									<% end if %>
							        document.getElementById('add_bss_cfgs_validate_almac_band_div').style.display = 'none';
							        document.getElementById('add_bss_cfgs_form_submit_div').style.display = '';
							        document.getElementById('Add_Flag').value = 0;
							        document.getElementById('Edit_Flag').value = 1;
							        document.getElementById('Remove_Flag').value = 0;
							        window.scrollTo(0, 0);
							    }

							    function removeBssCfg(alMac,band,bssInfoGrpObj,btnIdStr)
							    {
							        var bssInfo;
							        var bssIdx;
							        if(typeof(btnIdStr) == 'string')
							        {
							            var bssIdxArr = btnIdStr.match(/\d{1,}$/);
							            if(bssIdxArr)
							            {
							                bssIdx = bssIdxArr[0];
							            }
							        }
							        if(typeof(bssIdx) == 'string' && typeof(bssInfoGrpObj) == 'object')
							        {
							            bssInfo = bssInfoGrpObj[bssIdx];
							        }
							        if(!alMac ||
							            !band ||
							            !bssInfo ||
							            !bssInfo['ssid'] ||
							            !bssInfo['authMode'] ||
							            !bssInfo['encType'] ||
							            !bssInfo['passPhrase'] ||
							            !bssInfo['isBhBssSupported'] ||
							            !bssInfo['isFhBssSupported'] ||
							            !bssInfo['isHidden'])
							        {
							            console.log('Failed to remove BSS!\nERROR: BSSINFO is incorrect!\nbssInfo=',bssInfo);
							            alert('Failed to remove BSS!\nERROR: BSSINFO is incorrect!');
							            return;
							        }
							        document.getElementById('bss_cfgs_display').style.display = 'none';
							        document.getElementById('bss_cfgs_loading_div').style.display = '';

							        document.cbi._alMac.value = alMac;
							        document.cbi._band.value = band;
							        document.cbi._ssid.value = bssInfo['ssid'];
							        document.cbi._authMode.value = bssInfo['authMode'];
							        document.cbi._encType.value = bssInfo['encType'];
							        document.cbi._passPhrase.value = bssInfo['passPhrase'];
							        document.cbi._isBhBssSupported.value = bssInfo['isBhBssSupported'];
							        document.cbi._isFhBssSupported.value = bssInfo['isFhBssSupported'];
							        document.cbi._isHidden_ssid.value = bssInfo['isHidden'];
							        <% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
										document.cbi._fhVlanId.value = bssInfo['fhVlanId'];
										document.cbi._primaryVlan.value = bssInfo['primaryVlan'];
										document.cbi._defaultPcp.value = bssInfo['defaultPcp'];
									<% end if %>
							        document.cbi.defaultPcp.value = bssInfo['defaultPcp'];
							        document.cbi._edit_removeNodeIndex.value = bssInfo['NodeIndex'];
							        
							        document.cbi.Add_Flag.value = 0;
							        document.cbi.Edit_Flag.value = 0;
							        document.cbi.Remove_Flag.value = 1;
							        
							        if(SetCtlValue())
							        {
							        		formSubmit();
							        }
							        else
							        {
							        		alert("Failed to remove BSS!\nError: BSS is not exist !\n");
							        		document.getElementById('bss_cfgs_display').style.display = '';
							        		document.getElementById('bss_cfgs_loading_div').style.display = 'none';
							        }
							    }

							    function get_auth_mode_str(authModeCode)
							    {
							        switch(authModeCode)
							        {
							            case "0x0001":
							                return "OPEN";
									    case "0x0002":
									        return "WPAPSK";
							            case "0x0020":
							                return "WPA2 - Personal";
									    case "0x0022":
									        return "WPAPSKWPA2PSK";
										 case "0x0030":
							                return "WPA3-SAE";
									    case "0x0031":
									        return "WPA3 Transition";														
							            default:
							                return "Unknown Authentication Code: " + authModeCode;
							        }
							    }

							    function get_encryption_str(encryptionCode)
							    {
							        switch(encryptionCode)
							        {
							            case "0x0001":
							                return "NONE";
							            case "0x0004":
							                return "TKIP";
							            case "0x0008":
							                return "AES";
							            case "0x000c":
							                return "TKIPAES";
							            default:
							                return "Unknown Encryption Code: " + encryptionCode;
							        }
							    }

									function get_band_str(bandCode)
							    {
							        switch(bandCode)
							        {
							            case "8x":
							                return "2G";
							            case "11x":
							            case "12x":
							                return "5G";
							            default:
							                return "Unknown Band Code: " + bandCode;
							        }
							    }
							    
							    function get_band_str2(bandCode)
							    {
							        switch(bandCode)
							        {
							            case "8x":
							                return "2G";
							            case "11x":
							                return "5GL";
							            case "12x":
							                return "5GH";
							            default:
							                return "Unknown Band Code: " + bandCode;
							        }
							    }

							    function set_rowspan_auth_mode(o)
							    {
							        var rowspan = 0;
							        for(var k in o)
							        {
							            var v = o[k];
							            if(((typeof v) == "object") && v != null)
							            {
							                rowspan = rowspan + set_rowspan_auth_mode(v);
							            }
							            else
							            {
							                /* Pass-phrase row is not required for OPEN/NONE Security. */
							                if(k != 'passPhrase' || !(o['authMode'] == '0x0001' && o['encType'] == '0x0001'))
							                {
							                		if((k != 'NodeIndex'))
							                		{
							                				rowspan++;
							                		}
							                }
							            }
							        }
							        return rowspan;
							    }

							    function draw_bssinfo_cells(alMacTbl, row, alMac, band, bssInfoGrpObj)
							    {
							        var cell,s,t,i;
							        for(var bssIdx in bssInfoGrpObj)
							        {
							            var bssInfo = bssInfoGrpObj[bssIdx];									
										var bssRowSpan = set_rowspan_auth_mode(bssInfo);										
							            /* BSS-Idx */
							            cell = row.insertCell(-1);
							            cell.style.verticalAlign  = "middle";
							            cell.rowSpan = bssRowSpan;
							            s = document.createElement('STRONG');
							            t = document.createTextNode(get_band_str(band) +' : BSS-' + bssIdx);
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* SSID Key */
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS2Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* SSID Value */
							            cell = row.insertCell(-1);
							            t = document.createTextNode(bssInfo['ssid']);
							            cell.appendChild(t);

							            /* Action Button : Edit BSS */
							            cell = row.insertCell(-1);
							            cell.style.verticalAlign  = "middle";
							            cell.rowSpan = bssRowSpan;
							            i = document.createElement('INPUT');
							            i.setAttribute("style", "width: auto");
							            i.setAttribute("type", "button");
							            i.setAttribute("value", "<%tcWebApi_get("String_Entry","EasyMeshBSS21Test","s")%>");
							            i.setAttribute("id", "edit-bss-" + alMac + '-' + band + '-' + bssIdx);
							            if (document.addEventListener) 
							            {
							                // For all major browsers, except IE 8 and earlier
							                i.addEventListener("click", function(e){
							                	e = e || event;
							                	editBssCfg(alMac,band,bssInfoGrpObj,e.target.id);
							                });
							            } else if (document.attachEvent) {
							                // For IE 8 and earlier versions
							                i.attachEvent("onclick", function(e){
							                	e = e || event;
							                	editBssCfg(alMac,band,bssInfoGrpObj,e.srcElement.id);
							                });
							            }
							            cell.appendChild(i);

							            /* Action Button : Remove BSS */
							            /*
							            cell = row.insertCell(-1);
							            cell.style.verticalAlign  = "middle";
							            cell.rowSpan = bssRowSpan;
							            i = document.createElement('INPUT');
							            i.setAttribute("style", "width: auto");
							            i.setAttribute("type", "button");
							            i.setAttribute("value", "Remove BSS");
							            i.setAttribute("id", "remove-bss-" + alMac + '-' + band + '-' + bssIdx);
							            if (document.addEventListener) 
							            {
							                // For all major browsers, except IE 8 and earlier
							                i.addEventListener("click", function(){removeBssCfg(alMac,band,bssInfoGrpObj,this.id);});
							            } 
							            else if (document.attachEvent) 
							            {
							                // For IE 8 and earlier versions
							                i.attachEvent("onclick", function(){removeBssCfg(alMac,band,bssInfoGrpObj,this.id);});
							            }
							            cell.appendChild(i);
							            */

							            /* AuthMode Key */
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS3Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* AuthMode Value */
							            cell = row.insertCell(-1);
							            t = document.createTextNode(get_auth_mode_str(bssInfo['authMode']));
							            cell.appendChild(t);

							            /* Encryption Type Key */
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS4Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* Encryption Type Value */
							            cell = row.insertCell(-1);
							            t = document.createTextNode(get_encryption_str(bssInfo['encType']));
							            cell.appendChild(t);

							            /* Pass-phrase row will not be inserted for OPEN/NONE Security. */
							            if (!(bssInfo['authMode'] == '0x0001' && bssInfo['encType'] == '0x0001'))
							            {
							                /* Pass-phrase Key */
							                row = alMacTbl.insertRow(-1);
							                cell = row.insertCell(-1);
							                s = document.createElement('STRONG');
							                //t = document.createTextNode('Pass-phrase');
							                t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS5Test","s")%>');
							                s.appendChild(t);
							                cell.appendChild(s);

							                /* Pass-phrase Value */
							                cell = row.insertCell(-1);
							                t = document.createTextNode(bssInfo['passPhrase']);
							                cell.appendChild(t);
							            }

							            /* Hidden SSID Key */
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS6Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* Hidden SSID Value */
							            cell = row.insertCell(-1);
							            t = document.createTextNode(bssInfo['isHidden'] == 'hidden-Y' ? '<%tcWebApi_get("String_Entry","EasyMeshBasic2Test","s")%>' : '<%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>');
							            cell.appendChild(t);

							            /* BH BSS Support Key */
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS7Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* BH BSS Support Value */
							            cell = row.insertCell(-1);
							            if (((bssInfo['authMode'] == '0x0001') || (bssInfo['authMode'] == '0x0002') || (bssInfo['authMode'] == '0x0020') || (bssInfo['authMode'] == '0x0022') || (bssInfo['authMode'] == '0x0030') || (bssInfo['authMode'] == '0x0031') ) &&
							             ((bssInfo['encType'] == '0x0001') || (bssInfo['encType'] == '0x0004') || (bssInfo['encType'] == '0x0008') || (bssInfo['encType'] == '0x000c')))
							            	t = document.createTextNode(parseInt(bssInfo['isBhBssSupported']) == 1 ? '<%tcWebApi_get("String_Entry","EasyMeshBasic2Test","s")%>' : '<%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>');
							            else
							            	t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>');
							            cell.appendChild(t);

							            /* FH BSS Support Key */
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS8Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* FH BSS Support Value */
							            cell = row.insertCell(-1);
							            if (((bssInfo['authMode'] == '0x0001') || (bssInfo['authMode'] == '0x0002') || (bssInfo['authMode'] == '0x0020') || (bssInfo['authMode'] == '0x0022') || (bssInfo['authMode'] == '0x0030') || (bssInfo['authMode'] == '0x0031') ) &&
							             ((bssInfo['encType'] == '0x0001') || (bssInfo['encType'] == '0x0004') || (bssInfo['encType'] == '0x0008') || (bssInfo['encType'] == '0x000c')))
							            	t = document.createTextNode(parseInt(bssInfo['isFhBssSupported']) == 1 ? '<%tcWebApi_get("String_Entry","EasyMeshBasic2Test","s")%>' : '<%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>');
							            else
							            	t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBasic3Test","s")%>');
							            	
							            cell.appendChild(t);
							            row = alMacTbl.insertRow(-1);
										
							            <% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
							             /* FH Vlan ID Key */
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS22Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* FH Vlan ID  Value *///EasyMeshBSS22Test
							            cell = row.insertCell(-1);
							            t = document.createTextNode(bssInfo['fhVlanId']);
							            cell.appendChild(t);
							            
							            
							             /* primary vlan Key *///EasyMeshBSS23Test
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS23Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* primary vlan Value *///EasyMeshBSS24Test
							           	cell = row.insertCell(-1);
							           	if(bssInfo['primaryVlan'] == '1')
							           	{
							           		t = document.createTextNode("Enable");
							           	}
							           	else
							           	{
							           		t = document.createTextNode("Disable");
							           	}		            
							            cell.appendChild(t);
							            
							             /* default pcp Key */
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            s = document.createElement('STRONG');
							            t = document.createTextNode('<%tcWebApi_get("String_Entry","EasyMeshBSS24Test","s")%>');
							            s.appendChild(t);
							            cell.appendChild(s);

							            /* default pcp Value */
							            cell = row.insertCell(-1);
							            t = document.createTextNode(bssInfo['defaultPcp'] == "" ? "N/A" : bssInfo['defaultPcp']);
							            cell.appendChild(t);							            
							            row = alMacTbl.insertRow(-1);
										<% end if %>
							        }
							        /* Action Button : Add BSS */
							        //cell = row.insertCell(-1);
							        /* (1)BSS-Idx (2)BSS-Key (3)BSS-Value (4)Edit (5)Remove
							         *  First cell is Band and the below Add BSS button will be placed at second cell with colspan of 5.
							         */
							        /*
							        cell.colSpan = 5;
							        i = document.createElement('INPUT');
							        i.setAttribute("class", "cbi-button cbi-button-add");
							        i.setAttribute("type", "button");
							        i.setAttribute("value", "Add BSS");
							        if (document.addEventListener) {
							            // For all major browsers, except IE 8 and earlier
							            i.addEventListener("click", function(){addBssCfg(alMac,band);});
							        } else if (document.attachEvent) {
							            // For IE 8 and earlier versions
							            i.attachEvent("onclick", function(){addBssCfg(alMac,band);});
							        }
							        cell.appendChild(i);
							        */
							    }

							    function draw_easymesh_bss_overview_table(alMac, alMacInfo)
							    {
							        var cfgBssInfoHdrFset = document.getElementById('cfg_bss_info_hdr_fset');
							        var bssCfgOverviewDiv = document.getElementById('bss_overview_div');
							        var alMacTbl = document.createElement("TABLE");
							        alMacTbl.setAttribute("class", "cbi-section-table");
							        var row = alMacTbl.insertRow(-1);
							        var cell = row.insertCell(-1);
							        /* (1)Band (2)BSS-Idx (3)BSS-Key (4)BSS-Value (5)Edit (6)Remove */
							        cell.colSpan = 6;
							        cell.style.textAlign = "center";
							        var b = document.createElement('BIG');
							        var s = document.createElement('STRONG');
							        var t = document.createTextNode('AL-MAC: ' + alMac);
							        s.appendChild(t);
							        b.appendChild(s);
							        cell.appendChild(b);
							        /* 2G Band */
							        if(alMacInfo.hasOwnProperty('8x'))
							        {
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            cell.style.verticalAlign  = "middle";
							            cell.rowSpan = set_rowspan_auth_mode(alMacInfo['8x']) + 1; // One more row is required for 'Add New BSS' button.
							            s = document.createElement('STRONG');
							            t = document.createTextNode('2G');
							            s.appendChild(t);
							            cell.appendChild(s);
							            draw_bssinfo_cells(alMacTbl, row, alMac, '8x', alMacInfo['8x']);
							        }
							        /* 5GL Band */
							        if(alMacInfo.hasOwnProperty('11x'))
							        {
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            cell.style.verticalAlign  = "middle";
							            cell.rowSpan = set_rowspan_auth_mode(alMacInfo['11x']) + 1; // One more row is required for 'Add New BSS' button.
							            s = document.createElement('STRONG');
							            //t = document.createTextNode('5GL');
							            t = document.createTextNode('5G');
							            s.appendChild(t);
							            cell.appendChild(s);
							            draw_bssinfo_cells(alMacTbl, row, alMac, '11x', alMacInfo['11x']);
							        }
							        /* 5GH Band */
							        /*
							        if(alMacInfo.hasOwnProperty('12x'))
							        {
							            row = alMacTbl.insertRow(-1);
							            cell = row.insertCell(-1);
							            cell.style.verticalAlign  = "middle";
							            cell.rowSpan = set_rowspan_auth_mode(alMacInfo['12x']) + 1; // One more row is required for 'Add New BSS' button.
							            s = document.createElement('STRONG');
							            t = document.createTextNode('5GH');
							            s.appendChild(t);
							            cell.appendChild(s);
							            draw_bssinfo_cells(alMacTbl, row, alMac, '12x', alMacInfo['12x']);
							        }
							        */
							        bssCfgOverviewDiv.appendChild(alMacTbl);
							        cfgBssInfoHdrFset.style.display = '';
							        cfgBssInfoHdrFset.setAttribute("style", "width: 470; padding: 0");
							    }

									function htmlCharacterEscaping(str)
									{
										if(str.length == 0)
											return "";
											
										s = str.toString().replace(/&lt;/g, '<');
										s = s.toString().replace(/&gt;/g, '>');
										s = s.toString().replace(/&amp;/g, '&');
										s = s.toString().replace(/&quot;/g, '"');
										s = s.toString().replace(/&sqm;/g, "'");
										return s;
									}
									
							    function get_easymesh_bss_overview_info()
							    {
							        var cfgsJs = {};
							        cfgsJs['distinctAlMacCfgs'] = {};
							        cfgsJs['wildCardAlMacCfgs'] = {};
							        var tmpMac,tmpband,i,j,k,tmpIndex,tmpBssCount,tmpNodeIndex,tmpBssInfoIndex;
											var tmpbssEnable="",tmpalMac="",tmpband="",tmpssid="",tmpauthMode="",tmpencType="",tmppassPhrase="",tmpisBhBssSupported="",tmpisFhBssSupported="",tmpisHidden_ssid="",tmpNodeIndex="",tmpFhVlanId="",tmpprimaryVlan="",tmpDefaultPcp="";
											var al_mac_2g = "<% tcWebApi_get("Mesh_radio2gbssinfo","AL-MAC","s") %>";
											var al_mac_5gl = "<% tcWebApi_get("Mesh_radio5glbssinfo","AL-MAC","s") %>";
											var al_mac_5gh = "<% tcWebApi_get("Mesh_radio5ghbssinfo","AL-MAC","s") %>";
											var band_2g = "8x";
											var band_5gl = "11x";
											var band_5gh = "12x";
											
											tmpBssCount = 0;
											tmpBssInfoIndex = 0;
							       	{
							       			tmpBssInfoIndex=1;
													var vNonMapIface = "<% tcWebApi_get("mesh_common","NonMapIface","s") %>";
													var IfaceSearchResult = -1;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra0");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry0","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry0","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry0","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry0","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry0","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry0","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry0","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry0","HideSSID","s") %>";	
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;		
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry0","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry0","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = ("<% tcWebApi_get("Mesh_radio2gbssinfo_entry0","DefaultPCP","s") %>" == "N/A" ? "" : "<% tcWebApi_get("Mesh_radio2gbssinfo_entry0","DefaultPCP","s") %>" ) ;
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
																													
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra1");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry1","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
															tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry1","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry1","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry1","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry1","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry1","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry1","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry1","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry1","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry1","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry1","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra2");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry2","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry2","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry2","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry2","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry2","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry2","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry2","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry2","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry2","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry2","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry2","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra3");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry3","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry3","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry3","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry3","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry3","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry3","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry3","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry3","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry3","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry3","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry3","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra4");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry4","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry4","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry4","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry4","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry4","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry4","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry4","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry4","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry4","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry4","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry4","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra5");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry5","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry5","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry5","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry5","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry5","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry5","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry5","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry5","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry5","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry5","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry5","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra6");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry6","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry6","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry6","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry6","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry6","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry6","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry6","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry6","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry6","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry6","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry6","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra7");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry7","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry7","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry7","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry7","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry7","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry7","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry7","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry7","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry7","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry7","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry7","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra8");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry8","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry8","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry8","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry8","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry8","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry8","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry8","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry8","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry8","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry8","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry8","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra9");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry9","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry9","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry9","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry9","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry9","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry9","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry9","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry9","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry9","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry9","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry9","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra10");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry10","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry10","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry10","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry10","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry10","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry10","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry10","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry10","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry10","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry10","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry10","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra11");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry11","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry11","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry11","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry11","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry11","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry11","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry11","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry11","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry11","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry11","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry11","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra12");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry12","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry12","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry12","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry12","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry12","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry12","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry12","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry12","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry12","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry12","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry12","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra13");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry13","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry13","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry13","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry13","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry13","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry13","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry13","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry13","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry13","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry13","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry13","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra14");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry14","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry14","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry14","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry14","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry14","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry14","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry14","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry14","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry14","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry14","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry14","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("ra15");
													}
													var vArrayStr = "<% tcWebApi_get("WLan_Entry15","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_2g;
															tmpband = tmpband + ',' + band_2g;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry15","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan_Entry15","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry15","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry15","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry15","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry15","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan_Entry15","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry15","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry15","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio2gbssinfo_entry15","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													
													tmpBssInfoIndex=1;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai0");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry0","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry0","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry0","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry0","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry0","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry0","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry0","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry0","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry0","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry0","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry0","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai1");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry1","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry1","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry1","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry1","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry1","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry1","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry1","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry1","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry1","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry1","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry1","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai2");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry2","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry2","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry2","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry2","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry2","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry2","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry2","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry2","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry2","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry2","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry2","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai3");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry3","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry3","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry3","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry3","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry3","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry3","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry3","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry3","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry3","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry3","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry3","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai4");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry4","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry4","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry4","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry4","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry4","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry4","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry4","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry4","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry4","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry4","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry4","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai5");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry5","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry5","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry5","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry5","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry5","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry5","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry5","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry5","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry5","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry5","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry5","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai6");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry6","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry6","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry6","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry6","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry6","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry6","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry6","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry6","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry6","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry6","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry6","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai7");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry7","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry7","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry7","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry7","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry7","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry7","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry7","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry7","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry7","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry7","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry7","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai8");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry8","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry8","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry8","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry8","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry8","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry8","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry8","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry8","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry8","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry8","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry8","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai9");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry9","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry9","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry9","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry9","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry9","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry9","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry9","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry9","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry9","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry9","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry9","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai10");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry10","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry10","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry10","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry10","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry10","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry10","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry10","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry10","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry10","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry10","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry10","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai11");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry11","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry11","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry11","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry11","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry11","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry11","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry11","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry11","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;															
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry11","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry11","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry11","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai12");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry12","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry12","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry12","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry12","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry12","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry12","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry12","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry12","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry12","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry12","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry12","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai13");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry13","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry13","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry13","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry13","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry13","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry13","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry13","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry13","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry13","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry13","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry13","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai14");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry14","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry14","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry14","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry14","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry14","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry14","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry14","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry14","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry14","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry14","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry14","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
													tmpBssInfoIndex++;
													if(vNonMapIface != "N/A"){
														IfaceSearchResult = vNonMapIface.indexOf("rai15");
													}
													var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry15","EnableSSID","s") %>";
													if((vArrayStr != "N/A") && (parseInt(vArrayStr) > 0) && (IfaceSearchResult == -1))
													{
							       					tmpbssEnable = tmpbssEnable + ',' + vArrayStr;
							        				tmpalMac = tmpalMac + ',' + al_mac_5gl;
															tmpband = tmpband + ',' + band_5gl;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry15","SSID","s") %>";
															tmpssid = tmpssid + '/&split;' + htmlCharacterEscaping(vArrayStr);
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry15","AuthMode","s") %>";
															tmpauthMode = tmpauthMode + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry15","EncrypType","s") %>";
															tmpencType = tmpencType + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry15","WPAPSK","s") %>";
															tmppassPhrase = tmppassPhrase + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry15","BackHaul","s") %>";
															tmpisBhBssSupported = tmpisBhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry15","FrontHaul","s") %>";
															tmpisFhBssSupported = tmpisFhBssSupported + ',' + vArrayStr;
															var vArrayStr = "<% tcWebApi_get("WLan11ac_Entry15","HideSSID","s") %>";
															tmpisHidden_ssid = tmpisHidden_ssid + ',' + vArrayStr;
															
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry15","VlanID","s") %>";
															tmpFhVlanId = tmpFhVlanId + ',' + vArrayStr;	
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry15","PrimaryVlan","s") %>";
															tmpprimaryVlan = tmpprimaryVlan + ',' + (vArrayStr == "1" ? "1" : "0");
															var vArrayStr = "<% tcWebApi_get("Mesh_radio5glbssinfo_entry15","DefaultPCP","s") %>";
															tmpDefaultPcp = tmpDefaultPcp + ',' + vArrayStr;
															
															tmpNodeIndex = tmpNodeIndex + ',' + tmpBssInfoIndex;
															tmpBssCount++;
													}
							       	}

											/*bssEnable*/
											var vbssEnable = tmpbssEnable.split(',');
							        /*alMac*/
											var valMac = tmpalMac.split(',');
											/*band*/
											var vband = tmpband.split(',');
											/*ssid*/
											var vssid = tmpssid.split('/&split;');
											/*authMode*/
											var vauthMode = tmpauthMode.split(',');
											/*encType*/
											var vencType = tmpencType.split(',');
											/*passPhrase*/
											var vpassPhrase = tmppassPhrase.split(',');
											/*isBhBssSupported*/
											var visBhBssSupported = tmpisBhBssSupported.split(',');
											/*isFhBssSupported*/
											var visFhBssSupported = tmpisFhBssSupported.split(',');
											/*isHidden_ssid*/
											var visHidden_ssid = tmpisHidden_ssid.split(',');
											/*NodeIndex*/
											var vNodeIndex = tmpNodeIndex.split(',');
											
											var vfhVlanId = tmpFhVlanId.split(',');
											var vprimaryVlan = tmpprimaryVlan.split(',');
											var vdefaultPcp = tmpDefaultPcp.split(',');

											for(i=0;i<vbssEnable.length;i++)
											{
													tmpMac = valMac[i];
													tmpband = vband[i];

      										if (macRegExp.test(tmpMac))
													{
															tmpMac = tmpMac.toUpperCase();
															if(tmpMac == "FF:FF:FF:FF:FF:FF")
															{
																	/* wildCardAlMacCfgs */
																	if(cfgsJs['wildCardAlMacCfgs'][tmpMac] == null)
																	{
																			cfgsJs['wildCardAlMacCfgs'][tmpMac] = {};
																	}
																	if(cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband] == null)
																	{
																			cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband] = {};
																	}
																	for(j=0; j<tmpBssCount; j++)
																	{
																			tmpIndex = j + 1;
																			if(cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex] == null)
																			{
																					var tb_authMode = "";
																					var tb_encType = "";
																					var tb_isHidden = "hidden-N";

																					tb_authMode = get_auth_mode_str2val(vauthMode[i]);
																					tb_encType = get_encryption_str2val(vencType[i]);
																					if('0x0001' == tb_authMode)
																					{
																							tb_encType = '0x0001';
																							vpassPhrase[i] = '';
																					}

																					if(("hidden-Y" == visHidden_ssid[i]) || ("1" == visHidden_ssid[i]))
																					{
																							tb_isHidden = "hidden-Y";
																					}
																					
																					if((null == vprimaryVlan[i]) || ("N/A" == vprimaryVlan[i]) || ("" == vprimaryVlan[i]))
																					{
																							vprimaryVlan[i] = "0";
																					}
																					if(("Enable" == vprimaryVlan[i]) || ("1" == vprimaryVlan[i]))
																					{
																							vprimaryVlan[i] = "1";
																					}
																					if((NaN == (vdefaultPcp[i]).toString()) ||("NaN" == (vdefaultPcp[i]).toString()) || ("N/A" == vdefaultPcp[i]) || ("" == vdefaultPcp[i]))
																					{
																							vdefaultPcp[i] = "";
																					}
																					
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex] = {};
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['ssid'] = vssid[i];
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['authMode'] = tb_authMode;
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['encType'] = tb_encType;
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['passPhrase'] = vpassPhrase[i];
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['isFhBssSupported'] = visFhBssSupported[i];
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['isBhBssSupported'] = visBhBssSupported[i];
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['isHidden'] = tb_isHidden;
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['NodeIndex'] = vNodeIndex[i];
																					<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['fhVlanId'] = vfhVlanId[i];
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['primaryVlan'] = vprimaryVlan[i];
																					cfgsJs['wildCardAlMacCfgs'][tmpMac][tmpband][tmpIndex]['defaultPcp'] = vdefaultPcp[i];
																					<%end if %>
																					break;
																			}
																	}
															}
															else
															{
																	continue;/* only support wildCardAlMacCfgs */

																	/* distinctAlMacCfgs */
																	if(cfgsJs['distinctAlMacCfgs'][tmpMac] == null)
																	{
																			cfgsJs['distinctAlMacCfgs'][tmpMac] = {};
																	}
																	if(cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband] == null)
																	{
																			cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband] = {};
																	}
																	for(j=0; j<tmpBssCount; j++)
																	{
																			tmpIndex = j + 1;
																			if(cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex] == null)
																			{
																					var tb_authMode = "";
																					var tb_encType = "";
																					var tb_isHidden = "hidden-N";

																		tb_authMode = get_auth_mode_str2val(vauthMode[i]);
																		tb_encType = get_encryption_str2val(vencType[i]);
																		if('0x0001' == tb_authMode)
																		{
																			tb_encType = '0x0001';
																			vpassPhrase[i] = ''
																		}

																		if(("hidden-Y" == visHidden_ssid[i]) || ("1" == visHidden_ssid[i]))
																		{
																			tb_isHidden = "hidden-Y";
																		}
																		if((null == vprimaryVlan[i]) || ("N/A" == vprimaryVlan[i]) || ("" == vprimaryVlan[i]))
																		{
																			vprimaryVlan[i] = "0";
																		}
																		if(("Enable" == vprimaryVlan[i]) || ("1" == vprimaryVlan[i]))
																		{
																			vprimaryVlan[i] = "1";
																		}

																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex] = {};
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['ssid'] = vssid[i];
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['authMode'] = tb_authMode;
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['encType'] = tb_encType;
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['passPhrase'] = vpassPhrase[i];
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['isFhBssSupported'] = visFhBssSupported[i];
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['isBhBssSupported'] = visBhBssSupported[i];
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['isHidden'] = tb_isHidden;
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['NodeIndex'] = vNodeIndex[i];
																		<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['fhVlanId'] = vfhVlanId[i];
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['primaryVlan'] = vprimaryVlan[i];
																		cfgsJs['distinctAlMacCfgs'][tmpMac][tmpband][tmpIndex]['defaultPcp'] = vdefaultPcp[i];
																		<%end if %>
																		break;
																	}
															}
													}
											}
									}

							        return cfgsJs;
							    }

							    function gen_easymesh_bss_overview_table()
							    {
							        var cfgsJs = get_easymesh_bss_overview_info();
							        cfgsJs_global = cfgsJs;
							        var isCfgJsEmpty = true;
							        if(cfgsJs.hasOwnProperty('distinctAlMacCfgs'))
							        {
							            for(var alMac in cfgsJs['distinctAlMacCfgs'])
							            {
							                draw_easymesh_bss_overview_table(alMac, cfgsJs['distinctAlMacCfgs'][alMac]);
							                isCfgJsEmpty = false;
							            }
							        }
							        if(cfgsJs.hasOwnProperty('wildCardAlMacCfgs'))
							        {
							            for(var alMac in cfgsJs['wildCardAlMacCfgs'])
							            {
							                draw_easymesh_bss_overview_table(alMac, cfgsJs['wildCardAlMacCfgs'][alMac]);
							                isCfgJsEmpty = false;
							            }
							        }
							        if(isCfgJsEmpty)
							        {
							            document.getElementById('cfg_bss_info_hdr_fset').style.display = 'none';
							            document.getElementById('no_bss_cfg_msg_div').style.display = '';
							        }
							    }
							    
							    function checkWebStatus()
									{
											location.href = '/cgi-bin/wifi_map_bss_cfg_renew.asp';
									}

							    function webStateCheck()
							    {
							    	var checkBackHaulBssFlag;
							    	var webstate = "<% tcWebApi_get("Mesh_action", "map_action_complete","s") %>";
							    	if(1 == parseInt(webstate))
							    	{
							    			document.getElementById('bss_cfgs_loading_div').style.display = '';
							    			document.getElementById('bss_cfgs_display').style.display = 'none';
											document.getElementById('bss_cfgs_backhaulcheck_div').style.display = 'none';
							    			setTimeout(checkWebStatus, 5000);
							    	}
							    	else
							    	{
							    		 	clearTimeout(checkWebStatus);
							    			gen_easymesh_bss_overview_table();
							    			document.getElementById('bss_cfgs_loading_div').style.display = 'none';
							    			document.getElementById('bss_cfgs_display').style.display = '';
											checkBackHaulBssFlag = checkBackHaulBssCount();
											if(0 == checkBackHaulBssFlag)
											{
								    			document.getElementById('bss_cfgs_backhaulcheck_div').style.display = '';
											}
											else
											{
												document.getElementById('bss_cfgs_backhaulcheck_div').style.display = 'none';
											}
							    	}
							    }

							    webStateCheck();

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
						<P align=center></P>
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
function formSubmit()
{
		if(document.cbi != null)
		{
				if(1 == getValue('Save_Flag'))
				{
					setText('webwait_Flag', 1);
					if( true == setEBooValueCookie(document.cbi) )
					{
						document.cbi.submit();	
					}
					else
					{
						setText('webwait_Flag', 0);
					}
				}
		}
}

function SetCtlValue()
{
		var macRegExp = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		/*BssInfoCount, alMac, band, ssid, authMode, encType, passPhrase, isBhBssSupported, isFhBssSupported*/
		var vBssInfoCount = "<% tcWebApi_get("Mesh_common","BssInfoCount","s") %>";

		if((0 == getValue('Add_Flag')) && (1 == getValue('Edit_Flag')) && (0 == getValue('Remove_Flag')))
		{
			var _edit_removeNodeIndex = document.cbi.__EDIT_REMOVE_BSSINDEX.value;
			var tmpNodeIndex = parseInt(_edit_removeNodeIndex);
			if((tmpNodeIndex < 1) || (tmpNodeIndex > 48))
			{
				setText('Edit_Flag', 0);
				setText('Save_Flag', 0);
				return 0;
			}
			else
			{
				var isSsidHiddenChbox = document.cbi.add_bss_is_ssid_hidden_cbox;
				var band_val = document.cbi.add_bss_band_sel.value;
				var authMode_val = document.cbi.authMode.value;
				var encType_val = document.cbi.encType.value;

												
				authMode_val = get_auth_mode_val2str(authMode_val);
				encType_val = get_encryption_val2str(encType_val);					
				if(isSsidHiddenChbox.checked)
				{
					setText('isHidden', "1");
				}
				else
				{
					setText('isHidden', "0");
				}
				setText('_band', band_val);
				setText('_authMode', authMode_val);
				setText('_encType', encType_val);		
				<% if tcWebApi_get("Mesh_Mapcfg", "map_ver", "h") = "R2" then %>
				var fhVlanId_val = document.cbi.fhVlanId.value;
				var primaryVlan_val = document.cbi.primaryVlan.value;
				var defaultPcp_val = document.cbi.defaultPcp.value;
				setText('_fhVlanId',(fhVlanId_val).toString() == "" ? "4095" : parseInt(fhVlanId_val).toString());
				setText('_primaryVlan', parseInt(primaryVlan_val).toString());
				setText('_defaultPcp', defaultPcp_val == "" ? defaultPcp_val : parseInt(defaultPcp_val).toString());
				<% end if %>												
				setText('_edit_removeNodeIndex', tmpNodeIndex);
				setText('Save_Flag', 1);
				return 1;
			}						
		}
		
}

function checkBHAndFH(){
	var tmpMac = "FF:FF:FF:FF:FF:FF";
	var tmpband = document.getElementById('add_bss_band_sel').value;
	var bss_bh_ctl = document.getElementById('add_bss_bh_cbox');
	var bss_fh_ctl = document.getElementById('add_bss_fh_cbox');
	var curIndex = document.getElementById('edit_remove_bss_bssindex_hid_inp').value;
	console.log("curIndex = "+curIndex)
	var tmpIndex = 0, i = 0;
	var isNeedCloseMesh = 1;
	var bandList;
	console.log(cfgsJs_global['wildCardAlMacCfgs'][tmpMac]);
	if(cfgsJs_global['wildCardAlMacCfgs'] != null)
		{
			console.log(1)
			if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac] != null)
			{
				console.log(2)
				if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac]["8x"] != null)
				{
					console.log(3)
					bandList = cfgsJs_global['wildCardAlMacCfgs'][tmpMac]["8x"];
					for(i = 0; i < 8; i++)
					{
						tmpIndex = i + 1;
						//document.getElementById("add_bss_bh_cbox").checked = false
						if(curIndex == tmpIndex && tmpband == "8x")
						{
							if(bss_bh_ctl.checked == true || bss_fh_ctl.checked == true){
								isNeedCloseMesh = 0;
								break;
							} else {
								continue;
							}
							
						}
						
						if(bandList[tmpIndex] != null)
						{
							if(bandList[tmpIndex]['isBhBssSupported'] != null && bandList[tmpIndex]['isFhBssSupported'] != null )
							{
								if(1 == bandList[tmpIndex]['isBhBssSupported'] || 1 == bandList[tmpIndex]['isFhBssSupported'] )
								{
									isNeedCloseMesh = 0;
									break;
								}
							}
						}
					}
				}
				
				if(cfgsJs_global['wildCardAlMacCfgs'][tmpMac]["11x"] != null)
				{
					console.log(4)
					bandList = cfgsJs_global['wildCardAlMacCfgs'][tmpMac]["11x"];
					for(i = 0; i < 8; i++)
					{
						tmpIndex = i + 1;
						if(curIndex == tmpIndex && tmpband == "11x")
						{
							if(bss_bh_ctl.checked == true || bss_fh_ctl.checked == true){
								isNeedCloseMesh = 0;
								break;
							} else {
								continue;
							}
							
						}
						
						if(bandList[tmpIndex] != null)
						{
							if(bandList[tmpIndex]['isBhBssSupported'] != null && bandList[tmpIndex]['isFhBssSupported'] != null )
							{
								if(1 == bandList[tmpIndex]['isBhBssSupported'] || 1 == bandList[tmpIndex]['isFhBssSupported'] )
								{
									isNeedCloseMesh = 0;
									break;
								}
								
							}
						}
					}
				}
			}
		}
		console.log(isNeedCloseMesh)
		if (isNeedCloseMesh) {
			setText('Close_Flag', 1);
			alert("All bh and fh have be closed, mesh will be closed!");
		}
}

function bss_Save()
{
	checkBHAndFH();
	
	if(document.cbi != null)
	{
				if((0 == getValue('Edit_Flag')) && (0 == getValue('Remove_Flag')))
				{
					setText('Add_Flag', 1);
				}
				if(validate_all())
				{
					SetCtlValue();
					formSubmit();
				}
	}
}

</SCRIPT>
</BODY>
</HTML>