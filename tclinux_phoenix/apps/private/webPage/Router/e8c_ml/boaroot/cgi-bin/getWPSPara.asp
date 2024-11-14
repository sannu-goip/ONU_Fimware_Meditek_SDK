{
<% if tcwebApi_get("WebCustom_Entry","isMultiSupported","h") = "Yes" then %>
 "wpsTimerRunningData" : "<% tcWebApi_getbywifiid("Info_WLan", "wlanWPStimerRunning_", "wlan_id") %>"
 ,"wlanWPSStatus" : "<% tcWebApi_getbywifiid("Info_WLan", "wlanWPSStatus_", "wlan_id") %>"
 ,"wpsTimerRunningData_5G" : "<% tcWebApi_getbywifiid("Info_WLan11ac", "wlanWPStimerRunning_", "wlan_ac_id") %>"
 ,"wlanWPSStatus_5G" : "<% tcWebApi_getbywifiid("Info_WLan11ac", "wlanWPSStatus_", "wlan_ac_id") %>"
<% end if %>
}