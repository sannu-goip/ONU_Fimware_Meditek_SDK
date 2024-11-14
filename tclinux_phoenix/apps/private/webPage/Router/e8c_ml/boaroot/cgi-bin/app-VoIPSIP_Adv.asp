<%
If request_Form("VoIP_SIP_flag")="1" then
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
		tcWebApi_set("VoIPAdvanced_Common", "RegistrationExpire", "regExpTmr")
		tcWebApi_set("VoIPAdvanced_Common", "MaxStartDelay", "maxStartDelay")
		tcWebApi_set("VoIPAdvanced_Common", "RegisterRetryInterval", "regRetryInt")
	
	if Request_Form("isMultiDigitMap") = "1" then
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap1", "DigitMap1")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap2", "DigitMap2")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap3", "DigitMap3")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap4", "DigitMap4")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap5", "DigitMap5")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap6", "DigitMap6")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap7", "DigitMap7")
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMap8", "DigitMap8")
	else
		tcWebApi_set("VoIPAdvanced_Common", "DialPlan", "dialPlan")
	end if
		tcWebApi_set("VoIPDigitMap_Entry", "StartDigitTimer", "megacodmstarttime")
		tcWebApi_set("VoIPDigitMap_Entry", "InterDigitTimerShort", "megacodmshorttime")
		tcWebApi_set("VoIPDigitMap_Entry", "InterDigitTimerLong", "megacodmlongtime")
		tcWebApi_set("VoIPDigitMap_Entry", "InterDigitTimerT", "megacodmttime")
		tcWebApi_set("VoIPDigitMap_Entry", "BusyToneTimer", "busytonetimer")
		tcWebApi_set("VoIPDigitMap_Entry", "HangingReminderToneTimer", "hangingremindertimer")
		tcWebApi_set("VoIPDigitMap_Entry", "NoAnswerTimer", "noanswertimer")
		tcWebApi_set("VoIPDigitMap_Entry", "TMode", "dmtmode")
		tcWebApi_set("VoIPAdvanced_Common", "DTMFTransportMode", "dtmfRelay")
	
	if request_Form("dtmfRelay") = "RFC2833" then
		tcWebApi_set("VoIPBasic_Common", "TelephoneEventPayloadType", "rfc2833pt")
	end if
	tcWebApi_set("VoIPAdvanced_Common", "FlashOffTransSetting", "hookFlashRelay")
	tcWebApi_set("VoIPBasic_Common", "HeartbeatSwitch", "natKeepAliveMessageFlag")
	tcWebApi_set("VoIPBasic_Common", "HeartbeatCycle", "natKeepAliveTime")
	tcWebApi_set("VoIPAdvanced_Common", "OptionsSericeMode", "optionssericemode")
	tcWebApi_set("VoIPAdvanced_Common", "FaxCtrlMode", "faxctrlmode")
	tcWebApi_set("VoIPAdvanced_Common", "rtpRedundantEnable", "siprtpredundancyflag")
	tcWebApi_set("VoIPAdvanced_Common", "rtpRedundantPT", "siprtpredundancypt")
	If tcWebApi_get("WebCustom_Entry", "isCTPONC9Supported", "h") = "Yes" then
		tcWebApi_set("VoIPAdvanced_Common", "FromToHeadURI", "fromtoheadurl")
	End If
	tcWebApi_set("VoIPAdvanced_Common", "FaxPassThruCodec", "faxpassthru")
	tcWebApi_set("VoIPAdvanced_Common", "HotLineMode", "DivHotLineType")
	tcWebApi_set("VoIPAdvanced_Common", "RegResponsePortEnable", "DivIsRegisterWithPort")
	tcWebApi_set("VoIPAdvanced_Common", "MsgWithUserEnable", "DivIsMsgWithUser")
	tcWebApi_set("VoIPAdvanced_Common", "HotLineActNum", "DivHotLineActiveCode")
	tcWebApi_set("VoIPAdvanced_Common", "CallWaitType", "DivCWType")
	tcWebApi_set("VoIPAdvanced_Common", "NoRegDialToneEnable", "DivDialtone")
	tcWebApi_set("VoIPAdvanced_Common", "PolarityReversalMode", "DivRevPolarityType")
	tcWebApi_set("VoIPAdvanced_Common", "VoiceCodecPriorityCtrl", "DivCodecSelectFlag")
	tcWebApi_set("VoIPAdvanced_Common", "NumberMatchMode", "DivDigitMatchMethod")
	tcWebApi_set("VoIPAdvanced_Common", "ActLocalSupplementService", "DivSupplementServiceType")
	tcWebApi_set("VoIPAdvanced_Common", "EndCharHandleMode", "EndCharacterHandleMode")
	tcWebApi_set("VoIPAdvanced_Common", "SupportSpecCharTransfer", "changeCharEnableFlag")
	tcWebApi_set("VoIPAdvanced_Common", "SupReferSpecCharTrans", "changeReferCharEnableFlag")
	tcWebApi_set("VoIPMedia_Common", "FaxCodec", "t38EnableFlag")
	tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "echoCancelEnableFlag")
	tcWebApi_set("VoIPMedia_Common", "EchoCancellationLowSpeedFax", "EchoCancellationLowSpeedFax")
	tcWebApi_set("VoIPMedia_Entry0", "VAD", "comfortnoiseEnableFlag")
	tcWebApi_set("VoIPMedia_Entry0", "SilenceCompressionEnable", "vadEnableFlag")
	
	tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "echoCancelEnableFlag")
	tcWebApi_set("VoIPMedia_Entry1", "VAD", "comfortnoiseEnableFlag")
	tcWebApi_set("VoIPMedia_Entry1", "SilenceCompressionEnable", "vadEnableFlag")
	
	
	tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "echoCancelEnableFlag")
	tcWebApi_set("VoIPMedia_Entry2", "VAD", "comfortnoiseEnableFlag")
	tcWebApi_set("VoIPMedia_Entry2", "SilenceCompressionEnable", "vadEnableFlag")
	
	
	tcWebApi_set("VoIPMedia_Common", "EchoCancellationEnable", "echoCancelEnableFlag")
	tcWebApi_set("VoIPMedia_Entry3", "VAD", "comfortnoiseEnableFlag")
	tcWebApi_set("VoIPMedia_Entry3", "SilenceCompressionEnable", "vadEnableFlag")
	
	tcWebApi_set("VoIPBasic_Common", "PRACKEnable", "PrackEnableFlag")
	tcWebApi_set("VoIPBasic_Common", "LogOffMsgSendEnable", "sendUnregisterFlag")
	tcWebApi_set("VoIPAdvanced_Common", "CallSynEnAutoSwitch", "autoSynPhoneDateSwitchFlag")
	tcWebApi_set("VoIPAdvanced_Common", "SyncSysTimeEnable", "syncSystemDateFlag")
	tcWebApi_set("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_TYPE", "AJBEnableFlag")
	tcWebApi_set("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_LEN", "JBMaxLen")
	tcWebApi_set("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_FLAG", "sessiontimerEnableFlag")
	if request_Form("sessiontimerEnableFlag") = "1" then
		tcWebApi_set("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_TIMER", "sessionTimer")
		tcWebApi_set("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_MIN_EXP", "sessionMinSE")
		tcWebApi_set("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_REFRESHER", "sessionRefresher")
		tcWebApi_set("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_METHOD", "sessionMethod")
	end if
	tcWebApi_set("VoIPBasic_Common", "VoIP8021PMarkEnable", "VoIP8021pEnableFlag")
	if request_Form("VoIP8021pEnableFlag") = "1" then
		tcWebApi_set("VoIPBasic_Common", "SIP8021PMark", "VoIP8021PSIP")
		tcWebApi_set("VoIPBasic_Common", "RTP8021PMark", "VoIP8021PRTP")
	end if
	tcWebApi_set("VoIPBasic_Common", "SIPDSCPMark", "dscpmark")
	tcWebApi_set("VoIPBasic_Common", "RTPDSCPMark", "mymgtos")
	tcWebApi_set("VoIPCodecs_PVC0_Entry0", "priority", "codecList0")
	tcWebApi_set("VoIPCodecs_PVC1_Entry0", "priority", "codecList1")
	tcWebApi_set("VoIPCodecs_PVC2_Entry0", "priority", "codecList2")
	tcWebApi_set("VoIPCodecs_PVC3_Entry0", "priority", "codecList3")
	tcWebApi_set("VoIPCodecs_PVC0_Entry1", "priority", "codecList4")
	tcWebApi_set("VoIPCodecs_PVC1_Entry1", "priority", "codecList5")
	tcWebApi_set("VoIPCodecs_PVC2_Entry1", "priority", "codecList6")
	tcWebApi_set("VoIPCodecs_PVC3_Entry1", "priority", "codecList7")
	tcWebApi_set("VoIPCodecs_PVC0_Entry2", "priority", "codecList8")
	tcWebApi_set("VoIPCodecs_PVC1_Entry2", "priority", "codecList9")
	tcWebApi_set("VoIPCodecs_PVC2_Entry2", "priority", "codecList10")
	tcWebApi_set("VoIPCodecs_PVC3_Entry2", "priority", "codecList11")
	tcWebApi_set("VoIPCodecs_PVC0_Entry3", "priority", "codecList12")
	tcWebApi_set("VoIPCodecs_PVC1_Entry3", "priority", "codecList13")
	tcWebApi_set("VoIPCodecs_PVC2_Entry3", "priority", "codecList14")
	tcWebApi_set("VoIPCodecs_PVC3_Entry3", "priority", "codecList15")
	tcWebApi_set("VoIPCodecs_PVC0_Entry4", "priority", "codecList16")
	tcWebApi_set("VoIPCodecs_PVC1_Entry4", "priority", "codecList17")
	tcWebApi_set("VoIPCodecs_PVC2_Entry4", "priority", "codecList18")
	tcWebApi_set("VoIPCodecs_PVC3_Entry4", "priority", "codecList19")
	tcWebApi_set("VoIPCodecs_PVC0", "SIPPacketizationTime", "voicePtime0")
	tcWebApi_set("VoIPAdvanced_Entry0", "VoiceVolumeSpeak", "ingressGain0")
	tcWebApi_set("VoIPAdvanced_Entry0", "VoiceVolumeListen", "egressGain0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "HotLineEnable", "warmline0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "HotLineNumber", "warmlinenum0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "HotLineDelayTime", "warmlinetimeout0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "SIPCallWaitingEnable", "callwait0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "SIPCallTransfer", "transfer0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "ForwardNumber", "fwdNum0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "MTKSIPUCFNumber", "fwdNum0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "MTKSIPBCFNumber", "fwdNum0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "MTKSIPNCFNumber", "fwdNum0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "MTKUCFEnable", "fwdAll0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "MTKBCFEnable", "fwdBusy0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "MTKNCFEnable", "fwdNoAns0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "NoAnswerNCFWaitTime", "fwdNoAnsTime0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "SIP3wayConf", "threepartyservice0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "SIP3wayConfServerMix", "threepartyservermix0Flag")
	tcWebApi_set("VoIPCallCtrl_Entry0", "SIP3wayConfServAddr", "threepartyserviceadd0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "ComingCallDisplayMode", "calleridmode0")
	tcWebApi_set("VoIPAdvanced_Entry0", "SubscribeType", "DivSubscribeType0")
	tcWebApi_set("VoIPAdvanced_Entry0", "SubscribeExpire", "DivSubscribeExpire0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "HookMinInterval", "minhookinterval0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "HookMaxInterval", "maxhookinterval0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "HookReleaseMin", "minhookrelease0")
	tcWebApi_set("VoIPCallCtrl_Entry0", "SyncCallerTimeEnable", "synphonedate0Flag")
	
	tcWebApi_set("VoIPCodecs_PVC1", "SIPPacketizationTime", "voicePtime1")
	tcWebApi_set("VoIPAdvanced_Entry1", "VoiceVolumeSpeak", "ingressGain1")
	tcWebApi_set("VoIPAdvanced_Entry1", "VoiceVolumeListen", "egressGain1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "HotLineEnable", "warmline1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "HotLineNumber", "warmlinenum1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "HotLineDelayTime", "warmlinetimeout1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "SIPCallWaitingEnable", "callwait1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "SIPCallTransfer", "transfer1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "ForwardNumber", "fwdNum1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "MTKSIPUCFNumber", "fwdNum1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "MTKSIPBCFNumber", "fwdNum1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "MTKSIPNCFNumber", "fwdNum1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "MTKUCFEnable", "fwdAll1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "MTKBCFEnable", "fwdBusy1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "MTKNCFEnable", "fwdNoAns1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "NoAnswerNCFWaitTime", "fwdNoAnsTime1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "SIP3wayConf", "threepartyservice1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "SIP3wayConfServerMix", "threepartyservermix1Flag")
	tcWebApi_set("VoIPCallCtrl_Entry1", "SIP3wayConfServAddr", "threepartyserviceadd1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "ComingCallDisplayMode", "calleridmode1")
	tcWebApi_set("VoIPAdvanced_Entry1", "SubscribeType", "DivSubscribeType1")
	tcWebApi_set("VoIPAdvanced_Entry1", "SubscribeExpire", "DivSubscribeExpire1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "HookMinInterval", "minhookinterval1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "HookMaxInterval", "maxhookinterval1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "HookReleaseMin", "minhookrelease1")
	tcWebApi_set("VoIPCallCtrl_Entry1", "SyncCallerTimeEnable", "synphonedate1Flag")
	
	tcWebApi_set("VoIPCodecs_PVC2", "SIPPacketizationTime", "voicePtime2")
	tcWebApi_set("VoIPAdvanced_Entry2", "VoiceVolumeSpeak", "ingressGain2")
	tcWebApi_set("VoIPAdvanced_Entry2", "VoiceVolumeListen", "egressGain2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "HotLineEnable", "warmline2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "HotLineNumber", "warmlinenum2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "HotLineDelayTime", "warmlinetimeout2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "SIPCallWaitingEnable", "callwait2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "SIPCallTransfer", "transfer2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "ForwardNumber", "fwdNum2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "MTKSIPUCFNumber", "fwdNum2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "MTKSIPBCFNumber", "fwdNum2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "MTKSIPNCFNumber", "fwdNum2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "MTKUCFEnable", "fwdAll2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "MTKBCFEnable", "fwdBusy2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "MTKNCFEnable", "fwdNoAns2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "NoAnswerNCFWaitTime", "fwdNoAnsTime2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "SIP3wayConf", "threepartyservice2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "SIP3wayConfServerMix", "threepartyservermix2Flag")
	tcWebApi_set("VoIPCallCtrl_Entry2", "SIP3wayConfServAddr", "threepartyserviceadd2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "ComingCallDisplayMode", "calleridmode2")
	tcWebApi_set("VoIPAdvanced_Entry2", "SubscribeType", "DivSubscribeType2")
	tcWebApi_set("VoIPAdvanced_Entry2", "SubscribeExpire", "DivSubscribeExpire2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "HookMinInterval", "minhookinterval2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "HookMaxInterval", "maxhookinterval2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "HookReleaseMin", "minhookrelease2")
	tcWebApi_set("VoIPCallCtrl_Entry2", "SyncCallerTimeEnable", "synphonedate2Flag")

	tcWebApi_set("VoIPCodecs_PVC3", "SIPPacketizationTime", "voicePtime3")
	tcWebApi_set("VoIPAdvanced_Entry3", "VoiceVolumeSpeak", "ingressGain3")
	tcWebApi_set("VoIPAdvanced_Entry3", "VoiceVolumeListen", "egressGain3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "HotLineEnable", "warmline3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "HotLineNumber", "warmlinenum3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "HotLineDelayTime", "warmlinetimeout3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "SIPCallWaitingEnable", "callwait3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "SIPCallTransfer", "transfer3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "ForwardNumber", "fwdNum3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "MTKSIPUCFNumber", "fwdNum3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "MTKSIPBCFNumber", "fwdNum3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "MTKSIPNCFNumber", "fwdNum3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "MTKUCFEnable", "fwdAll3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "MTKBCFEnable", "fwdBusy3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "MTKNCFEnable", "fwdNoAns3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "NoAnswerNCFWaitTime", "fwdNoAnsTime3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "SIP3wayConf", "threepartyservice3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "SIP3wayConfServerMix", "threepartyservermix3Flag")
	tcWebApi_set("VoIPCallCtrl_Entry3", "SIP3wayConfServAddr", "threepartyserviceadd3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "ComingCallDisplayMode", "calleridmode3")
	tcWebApi_set("VoIPAdvanced_Entry3", "SubscribeType", "DivSubscribeType3")
	tcWebApi_set("VoIPAdvanced_Entry3", "SubscribeExpire", "DivSubscribeExpire3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "HookMinInterval", "minhookinterval3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "HookMaxInterval", "maxhookinterval3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "HookReleaseMin", "minhookrelease3")
	tcWebApi_set("VoIPCallCtrl_Entry3", "SyncCallerTimeEnable", "synphonedate3Flag")

	tcWebApi_set("VoIPDigitMap_Entry", "DigitMapEnable", "DigitMapEnableFlag")
	
	tcWebApi_set("VoIPDigitMap_Entry", "DigitMapSpecialEnable", "DigitMapSpecialEnableFlag")
	if request_Form("DigitMapSpecialEnableFlag") = "1" then 
		tcWebApi_set("VoIPDigitMap_Entry", "DigitMapSpecial", "ipt_DigitMapSpecial")
	end if
	tcWebApi_set("VoIPDigitMap_Entry", "PBXPrefixEnable", "PBXPrefixEnableFlag")
	if request_Form("PBXPrefixEnableFlag") = "1" then 
		tcWebApi_set("VoIPDigitMap_Entry", "PBXPrefix", "ipt_PBXPrefix")
	end if 
	tcWebApi_set("VoIPDigitMap_Entry", "PBXDialSecondaryEnable", "PBXDialSecondaryEnableFlag")
	tcWebApi_set("VoIPMedia_Common", "SC_ACCT_MEDIA_G723_RATE", "codecRate")
	tcWebApi_commit("VoIPBasic_Common")
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
table#perLineTable2 {
	width: 100%;
	table-layout: fixed;
}
table#perLineTable2 td{
	overflow: hidden;
}
table#perLineTable3 {
	width: 100%;
	table-layout: fixed;
}
table#perLineTable3 td{
	overflow: hidden;
}
table#Table_DigitMap {
	width: 100%;
	table-layout: fixed;
}
table#Table_DigitMap td{
	overflow: hidden;
}
</style>

<script language="javascript">
	var voipinfo2 = '<%tcWebApi_get("String_Entry","VoipStep2Text","s")%>';
<%
	If request_Form("VoIP_SIP_flag")="1" then
		if tcWebApi_get("WebCurSet_Entry", "VoIPIdle", "h") <> "1"  then
			asp_write("alert(voipinfo2);")
		end if
	end if
%>
var session = "0";
<% if tcWebApi_get("WebCustom_Entry","isMultiDigitMap","h") = "Yes" then %>
function getDigitMapValue()
{
	if(document.getElementById("DigitMap1").value.length < 512){
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value;
	}
	else if(document.getElementById("DigitMap2").value.length < 512){
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value + document.getElementById("DigitMap2").value;
	}else if(document.getElementById("DigitMap3").value.length < 512){
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value + 
													document.getElementById("DigitMap2").value +
													document.getElementById("DigitMap3").value;
	}else if(document.getElementById("DigitMap4").value.length < 512){
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value + 
													document.getElementById("DigitMap2").value +
													document.getElementById("DigitMap3").value +
													document.getElementById("DigitMap4").value;
	}else if(document.getElementById("DigitMap5").value.length < 512){
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value + 
													document.getElementById("DigitMap2").value +
													document.getElementById("DigitMap3").value +
													document.getElementById("DigitMap4").value +
													document.getElementById("DigitMap5").value;
	}else if(document.getElementById("DigitMap6").value.length < 512){
		document .getElementById("dialPlan").value = document.getElementById("DigitMap1").value + 
													document.getElementById("DigitMap2").value +
													document.getElementById("DigitMap3").value +
													document.getElementById("DigitMap4").value + 
													document.getElementById("DigitMap5").value +
													document.getElementById("DigitMap6").value;
	}else if(document.getElementById("DigitMap7").value.length < 512){
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value + 
													document.getElementById("DigitMap2").value +
													document.getElementById("DigitMap3").value +
													document.getElementById("DigitMap4").value +
													document.getElementById("DigitMap5").value + 
													document.getElementById("DigitMap6").value +
													document.getElementById("DigitMap7").value;
	}else{
		document.getElementById("dialPlan").value = document.getElementById("DigitMap1").value + 
													document.getElementById("DigitMap2").value +
													document.getElementById("DigitMap3").value +
													document.getElementById("DigitMap4").value +
													document.getElementById("DigitMap5").value + 
													document.getElementById("DigitMap6").value + 
													document.getElementById("DigitMap7").value +
													document.getElementById("DigitMap8").value;
	}

}
function setDigitMapValue()
{
	var len = document.getElementById("dialPlan").value.length;
	var num = len/512;

	if(len > 4096){
		alert("<%tcWebApi_get("String_Entry","VoipStep108Text","s")%>");
		return -1;
	}
	
	if(num < 1){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value;
		document.getElementById("DigitMap2").value = "";
	}else if(num < 2){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512);
		document.getElementById("DigitMap3").value = "";
	}else if(num < 3){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512,512*2);
		document.getElementById("DigitMap3").value = document.getElementById("dialPlan").value.substring(512*2);
		document.getElementById("DigitMap4").value = "";
	}else if(num < 4){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512,512*2);
		document.getElementById("DigitMap3").value = document.getElementById("dialPlan").value.substring(512*2,512*3);
		document.getElementById("DigitMap4").value = document.getElementById("dialPlan").value.substring(512*3);
		document.getElementById("DigitMap5").value = "";
	}else if(num < 5){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512,512*2);
		document.getElementById("DigitMap3").value = document.getElementById("dialPlan").value.substring(512*2,512*3);
		document.getElementById("DigitMap4").value = document.getElementById("dialPlan").value.substring(512*3,512*4);
		document.getElementById("DigitMap5").value = document.getElementById("dialPlan").value.substring(512*4);
		document.getElementById("DigitMap6").value = "";
	}else if(num < 6){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512,512*2);
		document.getElementById("DigitMap3").value = document.getElementById("dialPlan").value.substring(512*2,512*3);
		document.getElementById("DigitMap4").value = document.getElementById("dialPlan").value.substring(512*3,512*4);
		document.getElementById("DigitMap5").value = document.getElementById("dialPlan").value.substring(512*4,512*5);
		document.getElementById("DigitMap6").value = document.getElementById("dialPlan").value.substring(512*5);
		document.getElementById("DigitMap7").value = "";
	}else if(num < 7){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512,512*2);
		document.getElementById("DigitMap3").value = document.getElementById("dialPlan").value.substring(512*2,512*3);
		document.getElementById("DigitMap4").value = document.getElementById("dialPlan").value.substring(512*3,512*4);
		document.getElementById("DigitMap5").value = document.getElementById("dialPlan").value.substring(512*4,512*5);
		document.getElementById("DigitMap6").value = document.getElementById("dialPlan").value.substring(512*5,512*6);
		document.getElementById("DigitMap7").value = document.getElementById("dialPlan").value.substring(512*6);
		document.getElementById("DigitMap8").value = "";
	}else if(num <= 8){
		document.getElementById("DigitMap1").value = document.getElementById("dialPlan").value.substring(0,512);
		document.getElementById("DigitMap2").value = document.getElementById("dialPlan").value.substring(512,512*2);
		document.getElementById("DigitMap3").value = document.getElementById("dialPlan").value.substring(512*2,512*3);
		document.getElementById("DigitMap4").value = document.getElementById("dialPlan").value.substring(512*3,512*4);
		document.getElementById("DigitMap5").value = document.getElementById("dialPlan").value.substring(512*4,512*5);
		document.getElementById("DigitMap6").value = document.getElementById("dialPlan").value.substring(512*5,512*6);
		document.getElementById("DigitMap7").value = document.getElementById("dialPlan").value.substring(512*6,512*7);
		document.getElementById("DigitMap8").value = document.getElementById("dialPlan").value.substring(512*7);
	}

	 return 0;
}
<%end if%>

function LoadFrame()
{
	with ( document.ConfigForm ) 
	{
	generatePerLineTable();
	CheckPriority();
	if("2" != "<%tcWebApi_get("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_TYPE", "s") %>")
	{
		document.getElementById("JB_LEN").style.display = "";
	}
	else
	{
		document.getElementById("JB_LEN").style.display = "none";
	}

	if ("1" == "<%tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_FLAG", "s") %>")
	{
		document.getElementById("sesstimer").style.display = "";
		document.getElementById("sessminse").style.display = "";
		document.getElementById("sessrefresher").style.display = "";
		document.getElementById("sessmethod").style.display = "";
	}
	else
	{
		document.getElementById("sesstimer").style.display = "none";
		document.getElementById("sessminse").style.display = "none";
		document.getElementById("sessrefresher").style.display = "none";
		document.getElementById("sessmethod").style.display = "none";
	}
	
	if ("1" == "<%tcWebApi_get("VoIPBasic_Common", "VoIP8021PMarkEnable", "s") %>")
	{
		document.getElementById("8021psip").style.display = "";
		document.getElementById("8021prtp").style.display = "";
	}
	else
	{
		document.getElementById("8021psip").style.display = "none";
		document.getElementById("8021prtp").style.display = "none";
	}
	
	if ( "RFC2833" == "<%tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "s") %>"){
		document.getElementById("trRFC2833PT").style.display = "";
	}
	else{
		document.getElementById("trRFC2833PT").style.display = "none";
	}
	}
	<% if tcWebApi_get("WebCustom_Entry","isMultiDigitMap","h") = "Yes" then %>
		getDigitMapValue();
	<%end if%>
	CheckDigitMapEnable(getElById('cb_DigitMapEnable'));
	getElById('txt_DigitMap').value=getElById('dialPlan').value;

	getElById('txt_DigitMapSpecial').value=getElById('ipt_DigitMapSpecial').value;
	CheckDigitMapSpecialEnable(getElById('cb_DigitMapSpecialEnable'));
	CheckPBXPrefixEnable(getElById('cb_PBXPrefixEnable'));
	<% if tcWebApi_get("WebCustom_Entry", "isCTSCSupported", "h") = "Yes"  then %>
		<% if tcwebApi_get("WanInfo_Common","NoGUIAccessLimit","h" ) <> "1" then %>
			getElById('txt_DigitMap').disabled = true;
		<% end if %>
	<%end if%>
}

function doCheckSelect(obj,gid){
	if (obj.checked)
	{
		var sid="callwait" + gid;
		setCheck(sid,0);
		sid="fwdBusy" + gid;
		setCheck(sid,0);
		sid="fwdNoAns" + gid;
		setCheck(sid,0);
		sid="fwdAll" + gid;
		setCheck(sid,0);
		obj.checked = true;
	}
}

function generatePerLineTable() {
	// Generate the table
	if (document.body.innerHTML) {
		var tableHtmlOutput = "<table id='perLineTable1' border='1'><tbody id='perLineTbody1'>";
		var lineNumber = parseInt('<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>');

		// Line row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td >Line    <\/td>"
		tableHtmlOutput += "<td align=center>1</td>";
		if (lineNumber > 1 ){
			tableHtmlOutput += "<td align=center>2</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>2</td>";
		}
		
		if (lineNumber > 2 ){
			tableHtmlOutput += "<td align=center>3</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>3</td>";
		}
		if (lineNumber > 3 ){
			tableHtmlOutput += "<td align=center>4</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>4</td>";
		}
		
		tableHtmlOutput += "</tr>";

		// Codec rows
		tableHtmlOutput += "<tr><td ><% tcWebApi_get("VoIPCodecs_PVC0_Entry0", "codec", "s") %><\/td>"
		var selectOptions0 = Array();
		selectOptions0[0] = "<option value=" + 0 + " >" + "0";
		selectOptions0[1] = "<option value=" + 1 + " >" + "1";
		selectOptions0[2] = "<option value=" + 2 + " >" + "2";
		selectOptions0[3] = "<option value=" + 3 + " >" + "3";
		selectOptions0[4] = "<option value=" + 4 + " >" + "4";
		selectOptions0[5] = "<option value=" + 5 + " >" + "5";
		selectOptions0[6] = "<option value=" + 6 + " >" + "6";
		selectOptions0[7] = "<option value=" + 7 + " >" + "7";
		tableHtmlOutput += "<td align=center  width=180><select name='codecList0'>" + selectOptions0.join("") + "<\/select></td>";
		
		if (lineNumber > 1 ){
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=" + 0 + " >" + "0";
			selectOptions1[1] = "<option value=" + 1 + " >" + "1";
			selectOptions1[2] = "<option value=" + 2 + " >" + "2";
			selectOptions1[3] = "<option value=" + 3 + " >" + "3";
			selectOptions1[4] = "<option value=" + 4 + " >" + "4";
			selectOptions1[5] = "<option value=" + 5 + " >" + "5";
			selectOptions1[6] = "<option value=" + 6 + " >" + "6";
			selectOptions1[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  width=180><select name='codecList1'>" + selectOptions1.join("") + "<\/select></td>"; 
		}
		else{
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=" + 0 + " >" + "0";
			selectOptions1[1] = "<option value=" + 1 + " >" + "1";
			selectOptions1[2] = "<option value=" + 2 + " >" + "2";
			selectOptions1[3] = "<option value=" + 3 + " >" + "3";
			selectOptions1[4] = "<option value=" + 4 + " >" + "4";
			selectOptions1[5] = "<option value=" + 5 + " >" + "5";
			selectOptions1[6] = "<option value=" + 6 + " >" + "6";
			selectOptions1[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none width=180><select name='codecList1'>" + selectOptions1.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 2 ){
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=" + 0 + " >" + "0";
			selectOptions2[1] = "<option value=" + 1 + " >" + "1";
			selectOptions2[2] = "<option value=" + 2 + " >" + "2";
			selectOptions2[3] = "<option value=" + 3 + " >" + "3";
			selectOptions2[4] = "<option value=" + 4 + " >" + "4";
			selectOptions2[5] = "<option value=" + 5 + " >" + "5";
			selectOptions2[6] = "<option value=" + 6 + " >" + "6";
			selectOptions2[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  width=180><select name='codecList2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		else{
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=" + 0 + " >" + "0";
			selectOptions2[1] = "<option value=" + 1 + " >" + "1";
			selectOptions2[2] = "<option value=" + 2 + " >" + "2";
			selectOptions2[3] = "<option value=" + 3 + " >" + "3";
			selectOptions2[4] = "<option value=" + 4 + " >" + "4";
			selectOptions2[5] = "<option value=" + 5 + " >" + "5";
			selectOptions2[6] = "<option value=" + 6 + " >" + "6";
			selectOptions2[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none width=180><select name='codecList2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 3 ){
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=" + 0 + " >" + "0";
			selectOptions3[1] = "<option value=" + 1 + " >" + "1";
			selectOptions3[2] = "<option value=" + 2 + " >" + "2";
			selectOptions3[3] = "<option value=" + 3 + " >" + "3";
			selectOptions3[4] = "<option value=" + 4 + " >" + "4";
			selectOptions3[5] = "<option value=" + 5 + " >" + "5";
			selectOptions3[6] = "<option value=" + 6 + " >" + "6";
			selectOptions3[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  width=180><select name='codecList3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		else{
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=" + 0 + " >" + "0";
			selectOptions3[1] = "<option value=" + 1 + " >" + "1";
			selectOptions3[2] = "<option value=" + 2 + " >" + "2";
			selectOptions3[3] = "<option value=" + 3 + " >" + "3";
			selectOptions3[4] = "<option value=" + 4 + " >" + "4";
			selectOptions3[5] = "<option value=" + 5 + " >" + "5";
			selectOptions3[6] = "<option value=" + 6 + " >" + "6";
			selectOptions3[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none width=180><select name='codecList3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr><td ><% tcWebApi_get("VoIPCodecs_PVC0_Entry1", "codec", "s") %><\/td>"
		var selectOptions4 = Array();
		selectOptions4[0] = "<option value=" + 0 + " >" + "0";
		selectOptions4[1] = "<option value=" + 1 + " >" + "1";
		selectOptions4[2] = "<option value=" + 2 + " >" + "2";
		selectOptions4[3] = "<option value=" + 3 + " >" + "3";
		selectOptions4[4] = "<option value=" + 4 + " >" + "4";
		selectOptions4[5] = "<option value=" + 5 + " >" + "5";
		selectOptions4[6] = "<option value=" + 6 + " >" + "6";
		selectOptions4[7] = "<option value=" + 7 + " >" + "7";
		tableHtmlOutput += "<td align=center  ><select name='codecList4'>" + selectOptions4.join("") + "<\/select></td>";
		
		if (lineNumber > 1 ){
			var selectOptions5 = Array();
			selectOptions5[0] = "<option value=" + 0 + " >" + "0";
			selectOptions5[1] = "<option value=" + 1 + " >" + "1";
			selectOptions5[2] = "<option value=" + 2 + " >" + "2";
			selectOptions5[3] = "<option value=" + 3 + " >" + "3";
			selectOptions5[4] = "<option value=" + 4 + " >" + "4";
			selectOptions5[5] = "<option value=" + 5 + " >" + "5";
			selectOptions5[6] = "<option value=" + 6 + " >" + "6";
			selectOptions5[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList5'>" + selectOptions5.join("") + "<\/select></td>";
		}
		else{
			var selectOptions5 = Array();
			selectOptions5[0] = "<option value=" + 0 + " >" + "0";
			selectOptions5[1] = "<option value=" + 1 + " >" + "1";
			selectOptions5[2] = "<option value=" + 2 + " >" + "2";
			selectOptions5[3] = "<option value=" + 3 + " >" + "3";
			selectOptions5[4] = "<option value=" + 4 + " >" + "4";
			selectOptions5[5] = "<option value=" + 5 + " >" + "5";
			selectOptions5[6] = "<option value=" + 6 + " >" + "6";
			selectOptions5[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList5'>" + selectOptions5.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 2 ){
			var selectOptions6 = Array();
			selectOptions6[0] = "<option value=" + 0 + " >" + "0";
			selectOptions6[1] = "<option value=" + 1 + " >" + "1";
			selectOptions6[2] = "<option value=" + 2 + " >" + "2";
			selectOptions6[3] = "<option value=" + 3 + " >" + "3";
			selectOptions6[4] = "<option value=" + 4 + " >" + "4";
			selectOptions6[5] = "<option value=" + 5 + " >" + "5";
			selectOptions6[6] = "<option value=" + 6 + " >" + "6";
			selectOptions6[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList6'>" + selectOptions6.join("") + "<\/select></td>";
		}
		else{
			var selectOptions6 = Array();
			selectOptions6[0] = "<option value=" + 0 + " >" + "0";
			selectOptions6[1] = "<option value=" + 1 + " >" + "1";
			selectOptions6[2] = "<option value=" + 2 + " >" + "2";
			selectOptions6[3] = "<option value=" + 3 + " >" + "3";
			selectOptions6[4] = "<option value=" + 4 + " >" + "4";
			selectOptions6[5] = "<option value=" + 5 + " >" + "5";
			selectOptions6[6] = "<option value=" + 6 + " >" + "6";
			selectOptions6[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList6'>" + selectOptions6.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 3 ){
			var selectOptions7 = Array();
			selectOptions7[0] = "<option value=" + 0 + " >" + "0";
			selectOptions7[1] = "<option value=" + 1 + " >" + "1";
			selectOptions7[2] = "<option value=" + 2 + " >" + "2";
			selectOptions7[3] = "<option value=" + 3 + " >" + "3";
			selectOptions7[4] = "<option value=" + 4 + " >" + "4";
			selectOptions7[5] = "<option value=" + 5 + " >" + "5";
			selectOptions7[6] = "<option value=" + 6 + " >" + "6";
			selectOptions7[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList7'>" + selectOptions7.join("") + "<\/select></td>";
		}
		else{
			var selectOptions7 = Array();
			selectOptions7[0] = "<option value=" + 0 + " >" + "0";
			selectOptions7[1] = "<option value=" + 1 + " >" + "1";
			selectOptions7[2] = "<option value=" + 2 + " >" + "2";
			selectOptions7[3] = "<option value=" + 3 + " >" + "3";
			selectOptions7[4] = "<option value=" + 4 + " >" + "4";
			selectOptions7[5] = "<option value=" + 5 + " >" + "5";
			selectOptions7[6] = "<option value=" + 6 + " >" + "6";
			selectOptions7[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList7'>" + selectOptions7.join("") + "<\/select></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr><td ><% tcWebApi_get("VoIPCodecs_PVC0_Entry2", "codec", "s") %><\/td>"
		var selectOptions8 = Array();
		selectOptions8[0] = "<option value=" + 0 + " >" + "0";
		selectOptions8[1] = "<option value=" + 1 + " >" + "1";
		selectOptions8[2] = "<option value=" + 2 + " >" + "2";
		selectOptions8[3] = "<option value=" + 3 + " >" + "3";
		selectOptions8[4] = "<option value=" + 4 + " >" + "4";
		selectOptions8[5] = "<option value=" + 5 + " >" + "5";
		selectOptions8[6] = "<option value=" + 6 + " >" + "6";
		selectOptions8[7] = "<option value=" + 7 + " >" + "7";
		tableHtmlOutput += "<td align=center  ><select name='codecList8'>" + selectOptions8.join("") + "<\/select></td>";
		
		if (lineNumber > 1 ){
			var selectOptions9 = Array();
			selectOptions9[0] = "<option value=" + 0 + " >" + "0";
			selectOptions9[1] = "<option value=" + 1 + " >" + "1";
			selectOptions9[2] = "<option value=" + 2 + " >" + "2";
			selectOptions9[3] = "<option value=" + 3 + " >" + "3";
			selectOptions9[4] = "<option value=" + 4 + " >" + "4";
			selectOptions9[5] = "<option value=" + 5 + " >" + "5";
			selectOptions9[6] = "<option value=" + 6 + " >" + "6";
			selectOptions9[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList9'>" + selectOptions9.join("") + "<\/select></td>";
		}
		else{
			var selectOptions9 = Array();
			selectOptions9[0] = "<option value=" + 0 + " >" + "0";
			selectOptions9[1] = "<option value=" + 1 + " >" + "1";
			selectOptions9[2] = "<option value=" + 2 + " >" + "2";
			selectOptions9[3] = "<option value=" + 3 + " >" + "3";
			selectOptions9[4] = "<option value=" + 4 + " >" + "4";
			selectOptions9[5] = "<option value=" + 5 + " >" + "5";
			selectOptions9[6] = "<option value=" + 6 + " >" + "6";
			selectOptions9[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none  ><select name='codecList9'>" + selectOptions9.join("") + "<\/select></td>";
		}
		
		
		if (lineNumber > 2 ){
			var selectOptions10 = Array();
			selectOptions10[0] = "<option value=" + 0 + " >" + "0";
			selectOptions10[1] = "<option value=" + 1 + " >" + "1";
			selectOptions10[2] = "<option value=" + 2 + " >" + "2";
			selectOptions10[3] = "<option value=" + 3 + " >" + "3";
			selectOptions10[4] = "<option value=" + 4 + " >" + "4";
			selectOptions10[5] = "<option value=" + 5 + " >" + "5";
			selectOptions10[6] = "<option value=" + 6 + " >" + "6";
			selectOptions10[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList10'>" + selectOptions10.join("") + "<\/select></td>";
		}
		else{
			var selectOptions10 = Array();
			selectOptions10[0] = "<option value=" + 0 + " >" + "0";
			selectOptions10[1] = "<option value=" + 1 + " >" + "1";
			selectOptions10[2] = "<option value=" + 2 + " >" + "2";
			selectOptions10[3] = "<option value=" + 3 + " >" + "3";
			selectOptions10[4] = "<option value=" + 4 + " >" + "4";
			selectOptions10[5] = "<option value=" + 5 + " >" + "5";
			selectOptions10[6] = "<option value=" + 6 + " >" + "6";
			selectOptions10[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  style=display:none><select name='codecList10'>" + selectOptions10.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 3 ){
			var selectOptions11 = Array();
			selectOptions11[0] = "<option value=" + 0 + " >" + "0";
			selectOptions11[1] = "<option value=" + 1 + " >" + "1";
			selectOptions11[2] = "<option value=" + 2 + " >" + "2";
			selectOptions11[3] = "<option value=" + 3 + " >" + "3";
			selectOptions11[4] = "<option value=" + 4 + " >" + "4";
			selectOptions11[5] = "<option value=" + 5 + " >" + "5";
			selectOptions11[6] = "<option value=" + 6 + " >" + "6";
			selectOptions11[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList11'>" + selectOptions11.join("") + "<\/select></td>";
		}
		else{
			var selectOptions11 = Array();
			selectOptions11[0] = "<option value=" + 0 + " >" + "0";
			selectOptions11[1] = "<option value=" + 1 + " >" + "1";
			selectOptions11[2] = "<option value=" + 2 + " >" + "2";
			selectOptions11[3] = "<option value=" + 3 + " >" + "3";
			selectOptions11[4] = "<option value=" + 4 + " >" + "4";
			selectOptions11[5] = "<option value=" + 5 + " >" + "5";
			selectOptions11[6] = "<option value=" + 6 + " >" + "6";
			selectOptions11[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList11'>" + selectOptions11.join("") + "<\/select></td>";
		}
		tableHtmlOutput += "</tr>"; 
		
		tableHtmlOutput += "<tr><td ><% tcWebApi_get("VoIPCodecs_PVC0_Entry3", "codec", "s") %><\/td>"
		var selectOptions12 = Array();
		selectOptions12[0] = "<option value=" + 0 + " >" + "0";
		selectOptions12[1] = "<option value=" + 1 + " >" + "1";
		selectOptions12[2] = "<option value=" + 2 + " >" + "2";
		selectOptions12[3] = "<option value=" + 3 + " >" + "3";
		selectOptions12[4] = "<option value=" + 4 + " >" + "4";
		selectOptions12[5] = "<option value=" + 5 + " >" + "5";
		selectOptions12[6] = "<option value=" + 6 + " >" + "6";
		selectOptions12[7] = "<option value=" + 7 + " >" + "7";
		tableHtmlOutput += "<td align=center  ><select name='codecList12'>" + selectOptions12.join("") + "<\/select></td>";
		
		if (lineNumber > 1 ){
			var selectOptions13 = Array();
			selectOptions13[0] = "<option value=" + 0 + " >" + "0";
			selectOptions13[1] = "<option value=" + 1 + " >" + "1";
			selectOptions13[2] = "<option value=" + 2 + " >" + "2";
			selectOptions13[3] = "<option value=" + 3 + " >" + "3";
			selectOptions13[4] = "<option value=" + 4 + " >" + "4";
			selectOptions13[5] = "<option value=" + 5 + " >" + "5";
			selectOptions13[6] = "<option value=" + 6 + " >" + "6";
			selectOptions13[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList13'>" + selectOptions13.join("") + "<\/select></td>";
		}
		else{
			var selectOptions13 = Array();
			selectOptions13[0] = "<option value=" + 0 + " >" + "0";
			selectOptions13[1] = "<option value=" + 1 + " >" + "1";
			selectOptions13[2] = "<option value=" + 2 + " >" + "2";
			selectOptions13[3] = "<option value=" + 3 + " >" + "3";
			selectOptions13[4] = "<option value=" + 4 + " >" + "4";
			selectOptions13[5] = "<option value=" + 5 + " >" + "5";
			selectOptions13[6] = "<option value=" + 6 + " >" + "6";
			selectOptions13[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList13'>" + selectOptions13.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 2 ){
			var selectOptions14 = Array();
			selectOptions14[0] = "<option value=" + 0 + " >" + "0";
			selectOptions14[1] = "<option value=" + 1 + " >" + "1";
			selectOptions14[2] = "<option value=" + 2 + " >" + "2";
			selectOptions14[3] = "<option value=" + 3 + " >" + "3";
			selectOptions14[4] = "<option value=" + 4 + " >" + "4";
			selectOptions14[5] = "<option value=" + 5 + " >" + "5";
			selectOptions14[6] = "<option value=" + 6 + " >" + "6";
			selectOptions14[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList14'>" + selectOptions14.join("") + "<\/select></td>";
		}
		else{
			var selectOptions14 = Array();
			selectOptions14[0] = "<option value=" + 0 + " >" + "0";
			selectOptions14[1] = "<option value=" + 1 + " >" + "1";
			selectOptions14[2] = "<option value=" + 2 + " >" + "2";
			selectOptions14[3] = "<option value=" + 3 + " >" + "3";
			selectOptions14[4] = "<option value=" + 4 + " >" + "4";
			selectOptions14[5] = "<option value=" + 5 + " >" + "5";
			selectOptions14[6] = "<option value=" + 6 + " >" + "6";
			selectOptions14[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList14'>" + selectOptions14.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 3 ){
			var selectOptions15 = Array();
			selectOptions15[0] = "<option value=" + 0 + " >" + "0";
			selectOptions15[1] = "<option value=" + 1 + " >" + "1";
			selectOptions15[2] = "<option value=" + 2 + " >" + "2";
			selectOptions15[3] = "<option value=" + 3 + " >" + "3";
			selectOptions15[4] = "<option value=" + 4 + " >" + "4";
			selectOptions15[5] = "<option value=" + 5 + " >" + "5";
			selectOptions15[6] = "<option value=" + 6 + " >" + "6";
			selectOptions15[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList15'>" + selectOptions15.join("") + "<\/select></td>";
		}
		else{
			var selectOptions15 = Array();
			selectOptions15[0] = "<option value=" + 0 + " >" + "0";
			selectOptions15[1] = "<option value=" + 1 + " >" + "1";
			selectOptions15[2] = "<option value=" + 2 + " >" + "2";
			selectOptions15[3] = "<option value=" + 3 + " >" + "3";
			selectOptions15[4] = "<option value=" + 4 + " >" + "4";
			selectOptions15[5] = "<option value=" + 5 + " >" + "5";
			selectOptions15[6] = "<option value=" + 6 + " >" + "6";
			selectOptions15[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList15'>" + selectOptions15.join("") + "<\/select></td>";
		}
		
		tableHtmlOutput += "</tr>";
		
		
		tableHtmlOutput += "<tr><td ><% tcWebApi_get("VoIPCodecs_PVC0_Entry4", "codec", "s") %><\/td>"
		
		var selectOptions16= Array();
		selectOptions16[0] = "<option value=" + 0 + " >" + "0";
		selectOptions16[1] = "<option value=" + 1 + " >" + "1";
		selectOptions16[2] = "<option value=" + 2 + " >" + "2";
		selectOptions16[3] = "<option value=" + 3 + " >" + "3";
		selectOptions16[4] = "<option value=" + 4 + " >" + "4";
		selectOptions16[5] = "<option value=" + 5 + " >" + "5";
		selectOptions16[6] = "<option value=" + 6 + " >" + "6";
		selectOptions16[7] = "<option value=" + 7 + " >" + "7";
		tableHtmlOutput += "<td align=center  ><select name='codecList16'>" + selectOptions16.join("") + "<\/select></td>";
		
		if (lineNumber > 1)
		{
			var selectOptions17 = Array();
			selectOptions17[0] = "<option value=" + 0 + " >" + "0";
			selectOptions17[1] = "<option value=" + 1 + " >" + "1";
			selectOptions17[2] = "<option value=" + 2 + " >" + "2";
			selectOptions17[3] = "<option value=" + 3 + " >" + "3";
			selectOptions17[4] = "<option value=" + 4 + " >" + "4";
			selectOptions17[5] = "<option value=" + 5 + " >" + "5";
			selectOptions17[6] = "<option value=" + 6 + " >" + "6";
			selectOptions17[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList17'>" + selectOptions17.join("") + "<\/select></td>";
		}
		else{
			var selectOptions17 = Array();
			selectOptions17[0] = "<option value=" + 0 + " >" + "0";
			selectOptions17[1] = "<option value=" + 1 + " >" + "1";
			selectOptions17[2] = "<option value=" + 2 + " >" + "2";
			selectOptions17[3] = "<option value=" + 3 + " >" + "3";
			selectOptions17[4] = "<option value=" + 4 + " >" + "4";
			selectOptions17[5] = "<option value=" + 5 + " >" + "5";
			selectOptions17[6] = "<option value=" + 6 + " >" + "6";
			selectOptions17[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none ><select name='codecList17'>" + selectOptions17.join("") + "<\/select></td>";
		}
			
		if (lineNumber > 2)
		{
			var selectOptions18 = Array();
			selectOptions18[0] = "<option value=" + 0 + " >" + "0";
			selectOptions18[1] = "<option value=" + 1 + " >" + "1";
			selectOptions18[2] = "<option value=" + 2 + " >" + "2";
			selectOptions18[3] = "<option value=" + 3 + " >" + "3";
			selectOptions18[4] = "<option value=" + 4 + " >" + "4";
			selectOptions18[5] = "<option value=" + 5 + " >" + "5";
			selectOptions18[6] = "<option value=" + 6 + " >" + "6";
			selectOptions18[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList18'>" + selectOptions18.join("") + "<\/select></td>";
		}
		else{
			var selectOptions18 = Array();
			selectOptions18[0] = "<option value=" + 0 + " >" + "0";
			selectOptions18[1] = "<option value=" + 1 + " >" + "1";
			selectOptions18[2] = "<option value=" + 2 + " >" + "2";
			selectOptions18[3] = "<option value=" + 3 + " >" + "3";
			selectOptions18[4] = "<option value=" + 4 + " >" + "4";
			selectOptions18[5] = "<option value=" + 5 + " >" + "5";
			selectOptions18[6] = "<option value=" + 6 + " >" + "6";
			selectOptions18[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none  ><select name='codecList18'>" + selectOptions18.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 3)
		{
			var selectOptions19 = Array();
			selectOptions19[0] = "<option value=" + 0 + " >" + "0";
			selectOptions19[1] = "<option value=" + 1 + " >" + "1";
			selectOptions19[2] = "<option value=" + 2 + " >" + "2";
			selectOptions19[3] = "<option value=" + 3 + " >" + "3";
			selectOptions19[4] = "<option value=" + 4 + " >" + "4";
			selectOptions19[5] = "<option value=" + 5 + " >" + "5";
			selectOptions19[6] = "<option value=" + 6 + " >" + "6";
			selectOptions19[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center  ><select name='codecList19'>" + selectOptions19.join("") + "<\/select></td>";
		}
		else{
			var selectOptions19 = Array();
			selectOptions19[0] = "<option value=" + 0 + " >" + "0";
			selectOptions19[1] = "<option value=" + 1 + " >" + "1";
			selectOptions19[2] = "<option value=" + 2 + " >" + "2";
			selectOptions19[3] = "<option value=" + 3 + " >" + "3";
			selectOptions19[4] = "<option value=" + 4 + " >" + "4";
			selectOptions19[5] = "<option value=" + 5 + " >" + "5";
			selectOptions19[6] = "<option value=" + 6 + " >" + "6";
			selectOptions19[7] = "<option value=" + 7 + " >" + "7";
			tableHtmlOutput += "<td align=center style=display:none  ><select name='codecList19'>" + selectOptions19.join("") + "<\/select></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Ptime row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep109Text","s")%> <\/td>"
		var selectOptions0 = Array();
		selectOptions0[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC0", "SIPPacketizationTime", "h") = "10" then  asp_Write("selected") end if %>>" + 10;
		selectOptions0[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC0", "SIPPacketizationTime", "h") = "20" then  asp_Write("selected") end if %>>" + 20;
		selectOptions0[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC0", "SIPPacketizationTime", "h") = "30" then  asp_Write("selected") end if %>>" + 30;
		tableHtmlOutput += "<td align=center><select name='voicePtime0'>" + selectOptions0.join("") + "<\/select></td>";
		
		if (lineNumber > 1 ){
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC1", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>" + 10;
			selectOptions1[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC1", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>" + 20;
			selectOptions1[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC1", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>" + 30;
			tableHtmlOutput += "<td align=center><select name='voicePtime1'>" + selectOptions1.join("") + "<\/select></td>";
		}
		else{
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC1", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>" + 10;
			selectOptions1[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC1", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>" + 20;
			selectOptions1[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC1", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>" + 30;
			tableHtmlOutput += "<td align=center style=display:none><select name='voicePtime1'>" + selectOptions1.join("") + "<\/select></td>"; 
		}
		
		if (lineNumber > 2 ){
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC2", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>" + 10;
			selectOptions2[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC2", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>" + 20;
			selectOptions2[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC2", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>" + 30;
			tableHtmlOutput += "<td align=center><select name='voicePtime2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		else{
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC2", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>" + 10;
			selectOptions2[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC2", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>" + 20;
			selectOptions2[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC2", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>" + 30;
			tableHtmlOutput += "<td align=center style=display:none><select name='voicePtime2'>" + selectOptions2.join("") + "<\/select></td>"; 
		}
		
		if (lineNumber > 3 ){
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC3", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>" + 10;
			selectOptions3[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC3", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>" + 20;
			selectOptions3[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC3", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>" + 30;
			tableHtmlOutput += "<td align=center><select name='voicePtime3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		else{
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=10 <%if tcWebApi_get("VoIPCodecs_PVC3", "SIPPacketizationTime", "h") = "10" then asp_Write("selected") end if %>>" + 10;
			selectOptions3[1] = "<option value=20 <%if tcWebApi_get("VoIPCodecs_PVC3", "SIPPacketizationTime", "h") = "20" then asp_Write("selected") end if %>>" + 20;
			selectOptions3[2] = "<option value=30 <%if tcWebApi_get("VoIPCodecs_PVC3", "SIPPacketizationTime", "h") = "30" then asp_Write("selected") end if %>>" + 30;
			tableHtmlOutput += "<td align=center style=display:none><select name='voicePtime3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		
		tableHtmlOutput += "</tr>";
		
		//End of table
		tableHtmlOutput += "</tbody></table>";

		//////////////////////////////////////////
		//       new table for volume
		//////////////////////////////////////////
		tableHtmlOutput += "<br>";
		tableHtmlOutput += "<b><%tcWebApi_get("String_Entry","VoipStep110Text","s")%></b><br>";
		tableHtmlOutput += "<table id='perLineTable2' border='1'><tbody id='perLineTbody2'>";
		
		// Line row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td >Line            <\/td>"
		tableHtmlOutput += "<td align=center>1</td>";
		if (lineNumber > 1 ){
			tableHtmlOutput += "<td align=center>2</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>2</td>";
		}
		if (lineNumber > 2 ){
			tableHtmlOutput += "<td align=center>3</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>3</td>";
		}
		if (lineNumber > 3 ){
			tableHtmlOutput += "<td align=center>4</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>4</td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Volume row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep111Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'ingressGain0\' name=\'ingressGain0\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry0", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry0", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1 ){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'ingressGain1\' name=\'ingressGain1\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry1", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry1", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'ingressGain1\' name=\'ingressGain1\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry1", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry1", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>";
		}
		
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'ingressGain2\' name=\'ingressGain2\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry2", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry2", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'ingressGain2\' name=\'ingressGain2\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry2", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry2", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>";
		}
		
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'ingressGain3\' name=\'ingressGain3\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry3", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry3", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'ingressGain3\' name=\'ingressGain3\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry3", "VoiceVolumeSpeak", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry3", "VoiceVolumeSpeak", "s") else asp_Write("") end if %>\'></td>"; 
		}
		
		tableHtmlOutput += "</tr>";
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep112Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'egressGain0\' name=\'egressGain0\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry0", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry0", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'egressGain1\' name=\'egressGain1\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry1", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry1", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
		tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'egressGain1\' name=\'egressGain1\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry1", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry1", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		}
		
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'egressGain2\' name=\'egressGain2\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry2", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry2", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'egressGain2\' name=\'egressGain2\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry2", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry2", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'egressGain3\' id=\'egressGain3\' name=\'egressGain3\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry3", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry3", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'egressGain3\' name=\'egressGain3\' onchange=\'limitVolumeNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry3", "VoiceVolumeListen", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry3", "VoiceVolumeListen", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		//End of table
		tableHtmlOutput += "</tbody></table>";
		
		//////////////////////////////////////////
		//   new table for complement service
		//////////////////////////////////////////
		tableHtmlOutput += "<br>";
		tableHtmlOutput += "<b><%tcWebApi_get("String_Entry","VoipStep113Text","s")%></b><br>";
		tableHtmlOutput += "<table id='perLineTable3' border='1'><tbody id='perLineTbody3'>";
		
		// Line row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td >Line            <\/td>"
		tableHtmlOutput += "<td align=center>1</td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center>2</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>2</td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center>3</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>3</td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center>4</td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none>4</td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Warm Line row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep114Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'warmline0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'warmline1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'warmline1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'warmline2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'warmline2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 3){
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'warmline3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		else{
		tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'warmline3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "HotLineEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Warm Line Number row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep115Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 name=\'warmlinenum0\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 name=\'warmlinenum1\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 name=\'warmlinenum1\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		} 
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 name=\'warmlinenum2\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 name=\'warmlinenum2\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		} 
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center ><input type='text' size=20 maxlength=20 name=\'warmlinenum3\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none ><input type='text' size=20 maxlength=20 name=\'warmlinenum3\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HotLineNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HotLineNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Warm Line Timeout row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep116Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'warmlinetimeout0\' name=\'warmlinetimeout0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'warmlinetimeout1\' name=\'warmlinetimeout1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none ><input type='text' size=20 maxlength=20 id=\'warmlinetimeout1\' name=\'warmlinetimeout1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'warmlinetimeout2\' name=\'warmlinetimeout2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'warmlinetimeout2\' name=\'warmlinetimeout2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'warmlinetimeout3\' name=\'warmlinetimeout3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'warmlinetimeout3\' name=\'warmlinetimeout3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HotLineDelayTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HotLineDelayTime", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Call waiting row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep117Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'callwait0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,0)'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'callwait1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'callwait1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'callwait2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'callwait2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'callwait3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'callwait3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIPCallWaitingEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		/*zhangcg: 20110612*/
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep118Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'transfer0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'transfer1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'transfer1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'transfer2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'transfer2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'transfer3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'transfer3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIPCallTransfer", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Call forwarding number row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep119Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNum0\' name=\'fwdNum0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNum1\' name=\'fwdNum1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'fwdNum1\' name=\'fwdNum1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNum2\' name=\'fwdNum2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'fwdNum2\' name=\'fwdNum2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNum3\' name=\'fwdNum3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'fwdNum3\' name=\'fwdNum3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "ForwardNumber", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "ForwardNumber", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Call forwarding unconditionally row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep120Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdAll0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,0)'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdAll1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdAll1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdAll2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdAll2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdAll3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdAll3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "MTKUCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Call forwarding on "busy" row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep121Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdBusy0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,0)'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdBusy1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdBusy1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdBusy2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdBusy2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdBusy3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdBusy3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "MTKBCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Call forwarding on "no answer" row
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep122Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdNoAns0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,0)'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdNoAns1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdNoAns1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,1)'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdNoAns2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdNoAns2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,2)'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'fwdNoAns3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'fwdNoAns3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "MTKNCFEnable", "h") = "1" then asp_Write("checked") end if %> onclick='doCheckSelect(this,3)'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		// Call forwarding on "no answer time"
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep123Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime0\' name=\'fwdNoAnsTime0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime1\' name=\'fwdNoAnsTime1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime1\' name=\'fwdNoAnsTime1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime2\' name=\'fwdNoAnsTime2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime2\' name=\'fwdNoAnsTime2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime3\' name=\'fwdNoAnsTime3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'fwdNoAnsTime3\' name=\'fwdNoAnsTime3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "NoAnswerNCFWaitTime", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "NoAnswerNCFWaitTime", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep124Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservice0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservice1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'threepartyservice1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservice2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'threepartyservice2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservice3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'threepartyservice3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIP3wayConf", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep125Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservermix0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservermix1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'threepartyservermix1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservermix2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'threepartyservermix2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'threepartyservermix3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'threepartyservermix3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIP3wayConfServerMix", "h") = "Yes" then asp_Write("checked") end if %>></td>";
		}	
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep126Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd0\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd1\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd1\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd2\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd2\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd3\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=64 name=\'threepartyserviceadd3\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "SIP3wayConfServAddr", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "SIP3wayConfServAddr", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>"; 
		
		//caller id mode
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep127Text","s")%> <\/td>"
		var selectOptions0 = Array();
		selectOptions0[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry0", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
		selectOptions0[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry0", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK";
		tableHtmlOutput += "<td align=center><select name='calleridmode0'>" + selectOptions0.join("") + "<\/select></td>";

		if (lineNumber > 1){
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry1", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
			selectOptions1[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry1", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK";
			tableHtmlOutput += "<td align=center><select name='calleridmode1'>" + selectOptions1.join("") + "<\/select></td>";
		}
		else{
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry1", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
			selectOptions1[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry1", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK";
			tableHtmlOutput += "<td align=center style=display:none><select name='calleridmode1'>" + selectOptions1.join("") + "<\/select></td>";
		}
		if (lineNumber > 2){
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry2", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
			selectOptions2[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry2", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK";
			tableHtmlOutput += "<td align=center><select name='calleridmode2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		else{
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry2", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
			selectOptions2[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry2", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK";
			tableHtmlOutput += "<td align=center style=display:none><select name='calleridmode2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		if (lineNumber > 3){
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry3", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
			selectOptions3[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry3", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK"; 
			tableHtmlOutput += "<td align=center><select name='calleridmode3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		else{
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPCallCtrl_Entry3", "ComingCallDisplayMode", "h") = "0" then asp_Write("selected") end if %>>" + "DTMF";
			selectOptions3[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPCallCtrl_Entry3", "ComingCallDisplayMode", "h") = "1" then asp_Write("selected") end if %>>" + "FSK";
			tableHtmlOutput += "<td align=center style=display:none><select name='calleridmode3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		tableHtmlOutput += "</tr>";
		
		//subscribe type
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep128Text","s")%> <\/td>"
		var selectOptions0 = Array();
		selectOptions0[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry0", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
		selectOptions0[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry0", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
		selectOptions0[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry0", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
		selectOptions0[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry0", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
		tableHtmlOutput += "<td align=center><select name='DivSubscribeType0'>" + selectOptions0.join("") + "<\/select></td>";

		if (lineNumber > 1){
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
			selectOptions1[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
			selectOptions1[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
			selectOptions1[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
			tableHtmlOutput += "<td align=center><select name='DivSubscribeType1'>" + selectOptions1.join("") + "<\/select></td>";
		}
		else{
			var selectOptions1 = Array();
			selectOptions1[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
			selectOptions1[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
			selectOptions1[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
			selectOptions1[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
			tableHtmlOutput += "<td align=center style=display:none><select name='DivSubscribeType1'>" + selectOptions1.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 2){  
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
			selectOptions2[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
			selectOptions2[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
			selectOptions2[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
			tableHtmlOutput += "<td align=center><select name='DivSubscribeType2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		else{
			var selectOptions2 = Array();
			selectOptions2[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
			selectOptions2[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
			selectOptions2[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
			selectOptions2[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
			tableHtmlOutput += "<td align=center style=display:none><select name='DivSubscribeType2'>" + selectOptions2.join("") + "<\/select></td>";
		}
		
		if (lineNumber > 3){
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
			selectOptions3[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
			selectOptions3[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
			selectOptions3[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
			tableHtmlOutput += "<td align=center><select name='DivSubscribeType3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		else{
			var selectOptions3 = Array();
			selectOptions3[0] = "<option value=" + 0 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "0" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep309Text","s")%>";
			selectOptions3[1] = "<option value=" + 1 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "1" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep310Text","s")%>";
			selectOptions3[2] = "<option value=" + 2 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "2" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep311Text","s")%>";
			selectOptions3[3] = "<option value=" + 3 + " <%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeType", "h") = "3" then asp_Write("selected") end if %>>" + "<%tcWebApi_get("String_Entry","VoipStep312Text","s")%>";
			tableHtmlOutput += "<td align=center style=display:none><select name='DivSubscribeType3'>" + selectOptions3.join("") + "<\/select></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep129Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire0\' name=\'DivSubscribeExpire0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry0", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry0", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire1\' name=\'DivSubscribeExpire1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry1", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire1\' name=\'DivSubscribeExpire1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry1", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry1", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire2\' name=\'DivSubscribeExpire2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry2", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire2\' name=\'DivSubscribeExpire2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry2", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry2", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire3\' name=\'DivSubscribeExpire3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry3", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'DivSubscribeExpire3\' name=\'DivSubscribeExpire3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPAdvanced_Entry3", "SubscribeExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Entry3", "SubscribeExpire", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		
		//flash hook interval 
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep130Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookinterval0\' name=\'minhookinterval0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookinterval1\' name=\'minhookinterval1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'minhookinterval1\' name=\'minhookinterval1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookinterval2\' name=\'minhookinterval2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'minhookinterval2\' name=\'minhookinterval2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookinterval3\' name=\'minhookinterval3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'minhookinterval3\' name=\'minhookinterval3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HookMinInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HookMinInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep131Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'maxhookinterval0\' name=\'maxhookinterval0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'maxhookinterval1\' name=\'maxhookinterval1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'maxhookinterval1\' name=\'maxhookinterval1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'maxhookinterval2\' name=\'maxhookinterval2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'maxhookinterval2\' name=\'maxhookinterval2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		} 
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'maxhookinterval3\' name=\'maxhookinterval3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'maxhookinterval3\' name=\'maxhookinterval3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HookMaxInterval", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HookMaxInterval", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep132Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookrelease0\' name=\'minhookrelease0\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry0", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry0", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookrelease1\' name=\'minhookrelease1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'minhookrelease1\' name=\'minhookrelease1\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry1", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry1", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 2){
				tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookrelease2\' name=\'minhookrelease2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'minhookrelease2\' name=\'minhookrelease2\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry2", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry2", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		}
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='text' size=20 maxlength=20 id=\'minhookrelease3\' name=\'minhookrelease3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='text' size=20 maxlength=20 id=\'minhookrelease3\' name=\'minhookrelease3\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPCallCtrl_Entry3", "HookReleaseMin", "h") <> "N/A" then tcWebAPI_get("VoIPCallCtrl_Entry3", "HookReleaseMin", "s") else asp_Write("") end if %>\'></td>";
		}
		tableHtmlOutput += "</tr>";
		//Synchronize phone date time
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep133Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'synphonedate0\' <%if tcWebApi_get("VoIPCallCtrl_Entry0", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		if (lineNumber > 1){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'synphonedate1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'synphonedate1\' <%if tcWebApi_get("VoIPCallCtrl_Entry1", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		
		if (lineNumber > 2){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'synphonedate2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'synphonedate2\' <%if tcWebApi_get("VoIPCallCtrl_Entry2", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		
		if (lineNumber > 3){
			tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'synphonedate3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		else{
			tableHtmlOutput += "<td align=center style=display:none><input type='checkbox' size=10 name=\'synphonedate3\' <%if tcWebApi_get("VoIPCallCtrl_Entry3", "SyncCallerTimeEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		}
		
		tableHtmlOutput += "</tr>";
		
		//End of table
		tableHtmlOutput += "</tbody></table>";
		
		//////////////////////////////////////////
		//       new table for digitmap
		//////////////////////////////////////////
		tableHtmlOutput += "<br>";
		tableHtmlOutput += "<b><%tcWebApi_get("String_Entry","VoipStep134Text","s")%></b><br>";
		tableHtmlOutput += "<table id='Table_DigitMap' border='1'><tbody id='Tbody_DigitMap'>";
		
		tableHtmlOutput += "<tr>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep135Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 id=\'cb_DigitMapEnable\' name=\'cb_DigitMapEnable\' <%if tcWebApi_get("VoIPDigitMap_Entry", "DigitMapEnable", "h") = "1" then asp_Write("checked") end if %> onclick='CheckDigitMapEnable(this)'></td>";
		tableHtmlOutput += "</tr>";
		
		tableHtmlOutput += "<tr id='sec_DigitMap'>";
		tableHtmlOutput += "<td align=center colSpan=2><textarea id=\'txt_DigitMap\' name=\'txt_DigitMap\' rows=4 maxlength=4096 wrap='OFF' style='WIDTH:420px;'></textarea></td>";
		tableHtmlOutput += "</tr>"; 

		tableHtmlOutput += "<tr>";      
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep136Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 id=\'cb_DigitMapSpecialEnable\' name=\'cb_DigitMapSpecialEnable\' <%if tcWebApi_get("VoIPDigitMap_Entry", "DigitMapSpecialEnable", "h") = "1" then asp_Write("checked") end if %> onclick='CheckDigitMapSpecialEnable(this)'></td>";
		tableHtmlOutput += "</tr>";

		tableHtmlOutput += "<tr id='sec_DigitMapSpecial'>";
		tableHtmlOutput += "<td align=center colSpan=2><textarea id=\'txt_DigitMapSpecial\' name=\'txt_DigitMapSpecial\' rows=1 maxlength=512 wrap='OFF' style='WIDTH:420px;'></textarea></td>";
		tableHtmlOutput += "</tr>";

		tableHtmlOutput += "<tr style='display:none'>";
		tableHtmlOutput += "<td align=center colSpan=2><input type='text' value='<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMapSpecial", "s")%>' name='ipt_DigitMapSpecial' id='ipt_DigitMapSpecial'></td>";
		tableHtmlOutput += "</tr>";

		tableHtmlOutput += "<tr>";      
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep137Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 id=\'cb_PBXPrefixEnable\' name=\'cb_PBXPrefixEnable\' <%if tcWebApi_get("VoIPDigitMap_Entry", "PBXPrefixEnable", "h") = "1" then asp_Write("checked") end if %> onclick='CheckPBXPrefixEnable(this)'></td>";
		tableHtmlOutput += "</tr>";

		tableHtmlOutput += "<tr id='sec_PBXPrefix'>";
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep138Text","s")%><\/td>"    
		tableHtmlOutput += "<td align=center><input type='text' size=12 maxlength=8 id=\'ipt_PBXPrefix\' name=\'ipt_PBXPrefix\' onchange=\'limitNum(this)\' value=\'<%if tcWebApi_get("VoIPDigitMap_Entry", "PBXPrefix", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "PBXPrefix", "s") else asp_Write("") end if %>\'></td>";
		tableHtmlOutput += "</tr>";

		tableHtmlOutput += "<tr>";      
		tableHtmlOutput += "<td ><%tcWebApi_get("String_Entry","VoipStep139Text","s")%><\/td>"
		tableHtmlOutput += "<td align=center><input type='checkbox' size=10 name=\'cb_PBXDialSecondaryEnable\' <%if tcWebApi_get("VoIPDigitMap_Entry", "PBXDialSecondaryEnable", "h") = "1" then asp_Write("checked") end if %>></td>";
		tableHtmlOutput += "</tr>";

		//End of table
		tableHtmlOutput += "</tbody></table>";

		document.getElementById("perLineTablePlaceholder1").innerHTML = tableHtmlOutput;
	}
	else
	{
		alert("Cannot create per line table in this browser.");
	}
}

function CheckDigitMapEnable(cbCtl)
{
	if (cbCtl.checked == true)
	{
	setDisplay('sec_DigitMap',1);
	}
	else
	{
	setDisplay('sec_DigitMap',0);
	}
}


function CheckDigitMapSpecialEnable(cbCtl)
{
	if (cbCtl.checked == true)
	{
	setDisplay('sec_DigitMapSpecial',1);
	}
	else
	{
	setDisplay('sec_DigitMapSpecial',0);
	}
}

function CheckPBXPrefixEnable(cbCtl)
{
	if (cbCtl.checked == true)
	{
	setDisplay('sec_PBXPrefix',1);
	}
	else
	{
	setDisplay('sec_PBXPrefix',0);
	}
}


function CheckPriority()
{
	getElementByName("codecList0").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC0_Entry0", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC0_Entry0", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList4").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC0_Entry1", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC0_Entry1", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList8").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC0_Entry2", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC0_Entry2", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList12").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC0_Entry3", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC0_Entry3", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList16").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC0_Entry4", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC0_Entry4", "priority", "s") else asp_Write("0") end if %>";

	
	getElementByName("codecList1").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC1_Entry0", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC1_Entry0", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList5").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC1_Entry1", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC1_Entry1", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList9").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC1_Entry2", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC1_Entry2", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList13").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC1_Entry3", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC1_Entry3", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList17").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC1_Entry4", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC1_Entry4", "priority", "s") else asp_Write("0") end if %>";

	
	
	getElementByName("codecList2").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC2_Entry0", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC2_Entry0", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList6").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC2_Entry1", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC2_Entry1", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList10").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC2_Entry2", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC2_Entry2", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList14").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC2_Entry3", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC2_Entry3", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList18").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC2_Entry4", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC2_Entry4", "priority", "s") else asp_Write("0") end if %>";

	
	
	getElementByName("codecList3").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC3_Entry0", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC3_Entry0", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList7").selectedIndex =  "<%if tcWebApi_get("VoIPCodecs_PVC3_Entry1", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC3_Entry1", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList11").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC3_Entry2", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC3_Entry2", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList15").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC3_Entry3", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC3_Entry3", "priority", "s") else asp_Write("0") end if %>";
	getElementByName("codecList19").selectedIndex =   "<%if tcWebApi_get("VoIPCodecs_PVC3_Entry4", "priority", "h") <> "N/A" then tcWebAPI_get("VoIPCodecs_PVC3_Entry4", "priority", "s") else asp_Write("0") end if %>";

}

function applyClickBasic()
{
	var loc = "app-VoIP.asp";
	var code = "location='" + loc + " '";
	eval(code);
}

function isIntegerNeg(value)
{
	if ((/^([1-9][0-9]*)|(^-[1-9][0-9]*)$/.test(value)) || value == "0")
	{
		return true;
	}
	else
	{
		return false;
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
		if (!isInteger(o.value)|| parseInt(o.value) < 0)
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep140Text","s")%>');
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}
		else
			return true;
	}
}

function limitVolumeNum(o)
{
	if (o.value != "")
	{
		if (!isIntegerNeg(o.value)||o.value.indexOf('.') > 0)
		{
			alert('"' + o.value + '"' + '<%tcWebApi_get("String_Entry","VoipStep140Text","s")%>');
			window.setTimeout( function(){ o.focus(); }, 0);
			return false;
		}
		else
			return true;
	}
}

function dtmfRelayChange(o){
	var idx = o.selectedIndex;
	var val = o.options[idx].value;
	if ( val == "RFC2833" ){
	document.getElementById("trRFC2833PT").style.display = "";
	}
	else{
	document.getElementById("trRFC2833PT").style.display = "none";
	}
}

function AJBClick(cb){
	if(cb.checked == true )
	{
		document.getElementById("JB_LEN").style.display = "";
	}
	else
	{
		document.getElementById("JB_LEN").style.display = "none";
	}
}
function sessionClick(cb){
	if(cb.checked == true )
	{
	document.getElementById("sesstimer").style.display = "";
	document.getElementById("sessminse").style.display = "";
	document.getElementById("sessrefresher").style.display = "";
	document.getElementById("sessmethod").style.display = "";
	}
	else
	{
	document.getElementById("sesstimer").style.display = "none";
	document.getElementById("sessminse").style.display = "none";
	document.getElementById("sessrefresher").style.display = "none";
	document.getElementById("sessmethod").style.display = "none";
	}
}

function Priority8021PClick(cb){
	if(cb.checked == true )
	{
	document.getElementById("8021psip").style.display = "";
	document.getElementById("8021prtp").style.display = "";
	}
	else
	{
		document.getElementById("8021psip").style.display = "none";
		document.getElementById("8021prtp").style.display = "none";
	}
}

function checkValueValid()
{
	var lineNumber = parseInt(<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>);

	if((limitNum(document.getElementById("regExpTmr"))==false) 
	|| (limitNum(document.getElementById("maxStartDelay"))==false)
	|| (limitNum(document.getElementById("regRetryInt"))==false)
	|| (limitNum(document.getElementById("megacodmstarttime"))==false)
	|| (limitNum(document.getElementById("megacodmshorttime"))==false)
	|| (limitNum(document.getElementById("megacodmlongtime"))==false)
	|| (limitNum(document.getElementById("megacodmttime"))==false)
	|| (limitNum(document.getElementById("busytonetimer"))==false)
	|| (limitNum(document.getElementById("hangingremindertimer"))==false)
	|| (limitNum(document.getElementById("noanswertimer"))==false)
	|| (limitNum(document.getElementById("rfc2833pt"))==false)
	|| (limitNum(document.getElementById("natKeepAliveTime"))==false)
	|| (limitNum(document.getElementById("sip_rtp_redundancy_pt_val_1"))==false)
	|| (limitNum(document.getElementById("VoIP8021PSIP"))==false)
	|| (limitNum(document.getElementById("VoIP8021PRTP"))==false)
	|| (limitNum(document.getElementById("sessionTimer"))==false)
	|| (limitNum(document.getElementById("sessionMinSE"))==false)
	|| (limitNum(document.getElementById("dscpmark"))==false)
	|| (limitNum(document.getElementById("in_h_tos_val"))==false)
	|| (limitNum(document.getElementById("JBMaxLen"))==false))
		return false;
	
	if((limitVolumeNum(document.getElementById("ingressGain0"))==false) 
		|| (limitVolumeNum(document.getElementById("egressGain0"))==false)) 
		return false;


	if((limitNum(document.getElementById("warmlinetimeout0"))==false) 
	|| (limitNum(document.getElementById("fwdNum0"))==false)
	|| (limitNum(document.getElementById("fwdNoAnsTime0"))==false)
	|| (limitNum(document.getElementById("DivSubscribeExpire0"))==false)
	|| (limitNum(document.getElementById("minhookinterval0"))==false)
	|| (limitNum(document.getElementById("maxhookinterval0"))==false)
	|| (limitNum(document.getElementById("minhookrelease0"))==false)
	|| (limitNum(document.getElementById("ipt_PBXPrefix"))==false))
		return false;

	if(lineNumber > 1)
	{
		if((limitVolumeNum(document.getElementById("ingressGain1"))==false) 
			|| (limitVolumeNum(document.getElementById("egressGain1"))==false)) 
			return false;
		
		if((limitNum(document.getElementById("warmlinetimeout1"))==false) 
			|| (limitNum(document.getElementById("warmlinetimeout2"))==false)
			|| (limitNum(document.getElementById("fwdNum1"))==false)
			|| (limitNum(document.getElementById("fwdNoAnsTime1"))==false)
			|| (limitNum(document.getElementById("DivSubscribeExpire1"))==false)
			|| (limitNum(document.getElementById("minhookinterval1"))==false)
			|| (limitNum(document.getElementById("maxhookinterval1"))==false)
			|| (limitNum(document.getElementById("minhookrelease1"))==false))
				return false;
		
	}
	else if(lineNumber > 2)
	{
		if((limitVolumeNum(document.getElementById("ingressGain1"))==false) 
			|| (limitVolumeNum(document.getElementById("ingressGain2"))==false)
			|| (limitVolumeNum(document.getElementById("egressGain1"))==false)
			|| (limitVolumeNum(document.getElementById("egressGain2"))==false))
			return false;

		if((limitNum(document.getElementById("warmlinetimeout1"))==false) 
			|| (limitNum(document.getElementById("warmlinetimeout2"))==false)
			|| (limitNum(document.getElementById("fwdNum1"))==false)
			|| (limitNum(document.getElementById("fwdNum2"))==false)
			|| (limitNum(document.getElementById("fwdNoAnsTime1"))==false)
			|| (limitNum(document.getElementById("fwdNoAnsTime2"))==false)
			|| (limitNum(document.getElementById("DivSubscribeExpire1"))==false)
			|| (limitNum(document.getElementById("DivSubscribeExpire2"))==false)
			|| (limitNum(document.getElementById("minhookinterval1"))==false)
			|| (limitNum(document.getElementById("minhookinterval2"))==false)
			|| (limitNum(document.getElementById("maxhookinterval1"))==false)
			|| (limitNum(document.getElementById("maxhookinterval2"))==false)
			|| (limitNum(document.getElementById("minhookrelease1"))==false)
			|| (limitNum(document.getElementById("minhookrelease2"))==false))
				return false;
	}
	else if(lineNumber > 3)
	{
		if((limitVolumeNum(document.getElementById("ingressGain1"))==false) 
			|| (limitVolumeNum(document.getElementById("ingressGain2"))==false)				
			|| (limitVolumeNum(document.getElementById("ingressGain3"))==false)
			|| (limitVolumeNum(document.getElementById("egressGain1"))==false)
			|| (limitVolumeNum(document.getElementById("egressGain2"))==false)
			|| (limitVolumeNum(document.getElementById("egressGain3"))==false))
			return false;

		if((limitNum(document.getElementById("warmlinetimeout1"))==false) 
			|| (limitNum(document.getElementById("warmlinetimeout2"))==false)
			|| (limitNum(document.getElementById("warmlinetimeout3"))==false)
			|| (limitNum(document.getElementById("fwdNum1"))==false)
			|| (limitNum(document.getElementById("fwdNum2"))==false)
			|| (limitNum(document.getElementById("fwdNum3"))==false)
			|| (limitNum(document.getElementById("fwdNoAnsTime1"))==false)
			|| (limitNum(document.getElementById("fwdNoAnsTime2"))==false)
			|| (limitNum(document.getElementById("fwdNoAnsTime3"))==false)
			|| (limitNum(document.getElementById("DivSubscribeExpire1"))==false)
			|| (limitNum(document.getElementById("DivSubscribeExpire2"))==false)
			|| (limitNum(document.getElementById("DivSubscribeExpire3"))==false)
			|| (limitNum(document.getElementById("minhookinterval1"))==false)
			|| (limitNum(document.getElementById("minhookinterval2"))==false)
			|| (limitNum(document.getElementById("minhookinterval3"))==false)
			|| (limitNum(document.getElementById("maxhookinterval1"))==false)
			|| (limitNum(document.getElementById("maxhookinterval2"))==false)
			|| (limitNum(document.getElementById("maxhookinterval3"))==false)
			|| (limitNum(document.getElementById("minhookrelease1"))==false)
			|| (limitNum(document.getElementById("minhookrelease2"))==false)
			|| (limitNum(document.getElementById("minhookrelease3"))==false))
				return false;
	}
}

function btnApplySip()
{
	with (document.ConfigForm)
	{
		var iCw = 0;
		var iCfnum = 0;
		var iCf = 0;
		var i = 0;
		var checkMarkTest;
		var regDigitMap = /[^0-9\[\]\-\|\.\*#xXABCDTLStlsEF]/g;
		var lineNumber = parseInt(<% tcwebApi_get("VoIPBasic_Common","VoIPLineNumber","s") %>);

		if (cb_DigitMapSpecialEnable.checked)
		{
			if(regDigitMap.test(getElById('txt_DigitMapSpecial').value)){
				alert("<%tcWebApi_get("String_Entry","VoipStep141Text","s")%>");
				return;
		}
		}
		if (cb_DigitMapEnable.checked){   
			if(regDigitMap.test(getElById('txt_DigitMap').value)){
				alert("<%tcWebApi_get("String_Entry","VoipStep142Text","s")%>");
				return;
			}
		getElById('dialPlan').value=getElById('txt_DigitMap').value;
		}
		<% if tcWebApi_get("WebCustom_Entry","isMultiDigitMap","h") = "Yes" then %>
			var ret = -1;
			ret = setDigitMapValue();
			if(ret < 0)
			return;
		<%end if%>
		for (i = 0; i < lineNumber; i++)
		{
			iCw = 0;
			iCfnum = 0;
			iCf = 0;
			
			checkMarkTest = eval("callwait" + i + ".checked == true");
			if ( checkMarkTest )
			{
				iCw = 1;
			}
			checkMarkTest = eval("fwdBusy" + i + ".checked == true");
			if ( checkMarkTest )
			{
				iCf++;
			}
			checkMarkTest = eval("fwdNoAns" + i + ".checked == true");
			if ( checkMarkTest )
			{
				iCf++; 
			}
			checkMarkTest = eval("fwdAll" + i + ".checked == true");
			if ( checkMarkTest )
			{
				iCf++;
			}
			checkMarkTest = eval("fwdNum" + i + ".value != ''");
			if ( checkMarkTest )
			{
				iCfnum = 1;
			}
			if ((iCw + iCf) > 1) {
				alert("Call feature can only choose one!");
				return;
			}
		
			if ((iCf == 1) && (iCfnum == 0)) {
				alert("Call forward number is invalid!");
				return;
			}
	
			var codecOld = "";
			var codecNew = "";
			var k = 0;
			var m = 0;
			var mm = 0;
			var zz = 0;
			for (k = i; k < 20; k+=4) 
			{
				codecOld = codecOld + "," + codecNew;
				codecNew = eval( "codecList" + k + ".value" );
				codecList = codecOld.split(",");
				for (var j = 0;j < codecList.length;j++)
				{
					if (codecNew != 0 && codecNew == codecList[j])
					{
						//alert("Codec priority is same, please config again!");
						alert("<%tcWebApi_get("String_Entry","VoipStep143Text","s")%>");
						return;
					}
				}
				if(codecNew == 0)
				{
					zz++;
				}
			}
			if(zz == 5)
			{
			alert("<%tcWebApi_get("String_Entry","VoipStep144Text","s")%>");
			return;
			}
		}
		
		if(checkValueValid()==false)
			return;
		
		var temp = parseInt(ingressGain0.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep145Text","s")%>');
			return;
		}
	
		var temp = parseInt(ingressGain1.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep145Text","s")%>');
			return;
		}
	
		var temp = parseInt(ingressGain2.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep145Text","s")%>');
			return;
		}
	
		var temp = parseInt(ingressGain3.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep145Text","s")%>');
			return;
		}
		
		var temp = parseInt(egressGain0.value);
		if (temp < -14 || temp > 6)
		{
		alert('<%tcWebApi_get("String_Entry","VoipStep146Text","s")%>');
		return;
		}
	
		var temp = parseInt(egressGain1.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep146Text","s")%>');
			return;
		}
	
		var temp = parseInt(egressGain2.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep146Text","s")%>');
			return;
		}
	
		var temp = parseInt(egressGain3.value);
		if (temp < -14 || temp > 6)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep146Text","s")%>');
			return;
		}
	
		var tempRfc2833Pt = parseInt(rfc2833pt.value);
		if (tempRfc2833Pt < 96 || tempRfc2833Pt > 127)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep147Text","s")%>');
			return;
		}
		var temp = parseInt(JBMaxLen.value);
		if (temp < 1 || temp > 1000)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep148Text","s")%>');
			return;
		}
		
		var tempRtpPt = parseInt(siprtpredundancypt.value);
		if (tempRtpPt < 96 || tempRtpPt > 127)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep149Text","s")%>');
			return;
		}
	
		if(tempRfc2833Pt == tempRtpPt)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep150Text","s")%>');
			return;		
		}
	
		var temp = parseInt(VoIP8021PSIP.value);
		if (temp < 0 || temp > 7)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep151Text","s")%>');
			return;
		}
	
		var temp = parseInt(VoIP8021PRTP.value);
		if (temp < 0 || temp > 7)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep152Text","s")%>');
			return;
		}
		
		var temp1 = parseInt(sessionTimer.value);
		if (temp1 < 90)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep153Text","s")%>');
			return;
		}
		
		var temp2 = parseInt(sessionMinSE.value);
		if (temp2 < 90)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep154Text","s")%>');
			return;
		}
		
		if (temp1 < temp2)
		{
			alert ('<%tcWebApi_get("String_Entry","VoipStep155Text","s")%>');
			return;
		}
	
		var temp = parseInt (dscpmark.value);
		if (temp < 0 || temp > 63)
		{
			alert ('<%tcWebApi_get("String_Entry","VoipStep156Text","s")%>');
			return;
		}
	
		var temp = parseInt (mymgtos.value);
		if (temp < 0 || temp > 63)
		{
			alert ('<%tcWebApi_get("String_Entry","VoipStep157Text","s")%>');
			return;
		}
	
		var temp1 = parseInt (minhookinterval0.value);
		var temp2 = parseInt (maxhookinterval0.value);
		var temp3 = parseInt (minhookrelease0.value);
		if (temp3 < 300 || temp3 > 1199)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep158Text","s")%>');
			return;
		} 
		if (temp1 >= temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep159Text","s")%>');
			return;
		}
		if (temp3 < temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep160Text","s")%>');
			return;
		}
	
		var temp1 = parseInt (minhookinterval1.value);
		var temp2 = parseInt (maxhookinterval1.value);
		var temp3 = parseInt (minhookrelease1.value);
		if (temp3 < 300 || temp3 > 1199)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep158Text","s")%>');
			return;
		}
		if (temp1 >= temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep159Text","s")%>');
			return;
		}
		if (temp3 < temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep160Text","s")%>');
			return;
		}
		
		var temp1 = parseInt (minhookinterval2.value);
		var temp2 = parseInt (maxhookinterval2.value);
		var temp3 = parseInt (minhookrelease2.value);
		if (temp3 < 300 || temp3 > 1199)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep158Text","s")%>');
			return;
		} 
		if (temp1 >= temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep159Text","s")%>');
			return;
		}
		if (temp3 < temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep160Text","s")%>');
			return;
		}
		
		var temp1 = parseInt (minhookinterval3.value);
		var temp2 = parseInt (maxhookinterval3.value);
		var temp3 = parseInt (minhookrelease3.value);
		if (temp3 < 300 || temp3 > 1199)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep158Text","s")%>');
			return;
		}
		if (temp1 >= temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep159Text","s")%>');
			return;
		}
		if (temp3 < temp2)
		{
			alert('<%tcWebApi_get("String_Entry","VoipStep160Text","s")%>');
			return;
		}
		if (AJBEnable.checked)
		{
			var temp1 = parseInt (JBMaxLen.value);
			if(temp1 > 500){
				alert("Max JitterBuffer cann't be greater than 500");
				return;
			}
		}
		if (natKeepAliveMessage.checked)
		{
			natKeepAliveMessageFlag.value = "1";
		}
		if (changeCharEnable.checked)
		{
			changeCharEnableFlag.value = "1";
		}
		if (changeReferCharEnable.checked)
		{
			changeReferCharEnableFlag.value = "1";
		}
		if (t38Enable.checked)
		{
			t38EnableFlag.value = "1";
		}
		if (echoCancelEnable.checked)
		{
			echoCancelEnableFlag.value = "Yes";
		}
		if (comfortnoiseEnable.checked)
		{
			comfortnoiseEnableFlag.value = "1";
		}
		if (vadEnable.checked)
		{
			vadEnableFlag.value = "Yes";
		}
		if (PrackEnable.checked)
		{
			PrackEnableFlag.value = "Yes";
		}
		if (sendUnregister.checked)
		{
			sendUnregisterFlag.value = "Yes";
		}
		if (autoSynPhoneDateSwitch.checked)
		{
			autoSynPhoneDateSwitchFlag.value = "1";
		}
		if (syncSystemDate.checked)
		{
			syncSystemDateFlag.value = "Yes";
		}
		if (AJBEnable.checked)
		{
			AJBEnableFlag.value = "1";
		}
		else
		{
			AJBEnableFlag.value = "2";
			JBMaxLen.value="400";
		}
		if (VoIP8021pEnable.checked)
		{
			VoIP8021pEnableFlag.value = "1";
		}
		if (sessiontimerEnable.checked)
		{
			sessiontimerEnableFlag.value = "1";
		}
		if (warmline0.checked)
		{
			warmline0Flag.value = "1";
		}
		
		if (warmline1.checked)
		{
			warmline1Flag.value = "1";
		}
	
		if (warmline2.checked)
		{
			warmline2Flag.value = "1";
		}
		
		if (warmline3.checked)
		{
			warmline3Flag.value = "1";
		}
			
		if (callwait0.checked)
		{
			callwait0Flag.value = "1";
		}
	
		if (callwait1.checked)
		{
			callwait1Flag.value = "1";
		}
	
		if (callwait2.checked)
		{
			callwait2Flag.value = "1";
		}
		
		if (callwait3.checked)
		{
			callwait3Flag.value = "1";
		}
	
		if (transfer0.checked)
		{
			transfer0Flag.value = "Yes";
		}
		
		if (transfer1.checked)
		{
			transfer1Flag.value = "Yes";
		}
	
		if (transfer2.checked)
		{
			transfer2Flag.value = "Yes";
		}
		
		if (transfer3.checked)
		{
			transfer3Flag.value = "Yes";
		}
	
		if (fwdAll0.checked)
		{
		fwdAll0Flag.value = "1";
		}
		
		if (fwdAll1.checked)
		{
			fwdAll1Flag.value = "1";
		}
	
		if (fwdAll2.checked)
		{
			fwdAll2Flag.value = "1";
		}
	
		if (fwdAll3.checked)
		{
			fwdAll3Flag.value = "1";
		}
	
		if (fwdBusy0.checked)
		{
			fwdBusy0Flag.value = "1";
		}
	
		if (fwdBusy1.checked)
		{
			fwdBusy1Flag.value = "1";
		}
		
		if (fwdBusy2.checked)
		{
			fwdBusy2Flag.value = "1";
		}
		
		if (fwdBusy3.checked)
		{
			fwdBusy3Flag.value = "1";
		}
	
		if (fwdNoAns0.checked)
		{
			fwdNoAns0Flag.value = "1";
		}
			
		if (fwdNoAns1.checked)
		{
			fwdNoAns1Flag.value = "1";
		}
	
		if (fwdNoAns2.checked)
		{
			fwdNoAns2Flag.value = "1";
		}
		
		if (fwdNoAns3.checked)
		{
			fwdNoAns3Flag.value = "1";
		}
	
		if (threepartyservice0.checked)
		{
			threepartyservice0Flag.value = "Yes";
		}
			
		if (threepartyservice1.checked)
		{
			threepartyservice1Flag.value = "Yes";
		}
		
		if (threepartyservice2.checked)
		{
			threepartyservice2Flag.value = "Yes";
		}
		
		if (threepartyservice3.checked)
		{
			threepartyservice3Flag.value = "Yes";
		}
	
		if (threepartyservermix0.checked)
		{
			threepartyservermix0Flag.value = "Yes";
		}
	
		if (threepartyservermix1.checked)
		{
			threepartyservermix1Flag.value = "Yes";
		}
		
		if (threepartyservermix2.checked)
		{
			threepartyservermix2Flag.value = "Yes";
		}
		if (threepartyservermix3.checked)
		{
			threepartyservermix3Flag.value = "Yes";
		}
	
		if (synphonedate0.checked)
		{
			synphonedate0Flag.value = "1";
		}
	
		if (synphonedate1.checked)
		{
			synphonedate1Flag.value = "1";
		}
	
		if (synphonedate2.checked)
		{
			synphonedate2Flag.value = "1";
		}
		
		if (synphonedate3.checked)
		{
			synphonedate3Flag.value = "1";
		}
		
		if (cb_DigitMapEnable.checked)
		{
			DigitMapEnableFlag.value = "1";
		}
	
		if (cb_DigitMapSpecialEnable.checked)
		{
			DigitMapSpecialEnableFlag.value = "1";
			ipt_DigitMapSpecial.value = txt_DigitMapSpecial.value;
		}
		if (cb_PBXPrefixEnable.checked)
		{
			PBXPrefixEnableFlag.value = "1";
		}
		if (cb_PBXDialSecondaryEnable.checked)
		{
			PBXDialSecondaryEnableFlag.value = "1";
		}
		if (siprtpredundancy.checked)
		{
			siprtpredundancyflag.value = "1";
		}
	
		VoIP_SIP_flag.value = "1";
		if( true == setEBooValueCookie(document.ConfigForm) )
			submit();
	}
}

</script>
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
									<P align=right><FONT face=���� color=#ffffff><B><FONT face=���� 
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
								<TD id=MenuArea_L2 bgColor=#427594 colSpan=2 height=24></TD></TR>
					</TBODY></TABLE>
					<SCRIPT language=javascript>
						MakeMenu(getElById ('Selected_Menu').value);
					</SCRIPT>

					<TABLE id=table3 height=15 cellSpacing=0 cellPadding=0 width=808 
					border=0><TBODY>
						<TR>
							<TD height=15><IMG height=15 src="/img/panel1.gif" width=164 border=0> </TD>
							<TD><IMG height=15 src="/img/panel2.gif" width=645 border=0> </TD>
						</TR>
			</TBODY></TABLE></TD></TR>
			<TR>
				<TD vAlign=top>
					<TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
						<TBODY>
							<TR>
								<TD width=157 bgColor=#ef8218 height=30>
									<P class=Item_L1><%tcWebApi_get("String_Entry","VoipStep1Text","s")%></P></TD>
									<TD width=7 bgColor=#ef8218></TD>
									<TD width=474></TD>
									<TD vAlign=top width=170 background=/img/panel4.gif 
										rowSpan=13><TABLE cellSpacing=0 cellPadding=20 width="100%" 
										border=0 height='100%'>
										<TBODY>
											<TR>
												<TD valign='top'><A 
													href="/cgi-bin/help_content.asp#�����绰����" 
													target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 border=0> </A></TD>
											</TR>
											<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
												<TR>
													<TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
											<% end if %>  
							</TBODY></TABLE></TD></TR>
							<TR>
								<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
								<TD width=7 background=/img/panel3.gif></TD>
							<TD></TD></TR>
							<TR>
								<TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
								<P class=Item_L2></P></TD>
								<TD width=7 background=/img/panel3.gif></TD>
								<TD>
									<TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
										<TBODY>
											<TR>
												<TD width=10>&nbsp;</TD>
												<TD>
													<FORM id=ConfigForm name="ConfigForm" action="/cgi-bin/app-VoIP_Adv.asp" method="post">
														<DIV id=pptp>
															<TABLE border=0 cellSpacing=0 cellPadding=0 width="100%">
																<TBODY>
																	<TR>
																		<TD>
																			<input type='button' id=applyClickBas name=applyClickBas style="font-size:100%;width:170;height:30" onClick="applyClickBasic()" value='<%tcWebApi_get("String_Entry","VoipStep85Text","s")%>'>
																		</TD>
																	</TR>
																	<TR>
																		<TD height="10">
																		</TD>
																	</TR>
																	<TR>
																		<TD class=table1_head>
																			<B>
																				<%tcWebApi_get("String_Entry","VoipStep84Text","s")%>
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
															<TABLE border=0 cellSpacing=1 cellPadding=0 width="98%">
																<TBODY>
																	<TR>
																		<TD width="35%" align=left>
																			<%tcWebApi_get("String_Entry","VoipStep164Text","s")%>
																		</TD>
																		<TD width="65%" colSpan=2 align=left>
																			<input type='text' id='regExpTmr' name='regExpTmr' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "RegistrationExpire", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "RegistrationExpire", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD width="35%" align=left>
																			<%tcWebApi_get("String_Entry","VoipStep165Text","s")%>
																		</TD>
																		<TD width="65%" colSpan=2 align=left>
																			<input type='text' id='maxStartDelay' name='maxStartDelay' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "MaxStartDelay", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "MaxStartDelay", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR>
																		<TD align=left>
																			<%tcWebApi_get("String_Entry","VoipStep166Text","s")%>
																		</TD>
																		<TD align=left>
																			<input type='text' id='regRetryInt' name='regRetryInt' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "RegisterRetryInterval", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "RegisterRetryInterval", "s") else asp_Write("") end if %>">
																		</TD>
																	</TR>
																	<TR style="display:none">
																		<TD align=left>
																			<%tcWebApi_get("String_Entry","VoipStep167Text","s")%>
																		</TD>
																		<TD align=left>
																			<% if tcWebApi_get("WebCustom_Entry","isMultiDigitMap","h") = "Yes" then %>
																				<input type='text' name='dialPlan' id='dialPlan' value="">
																			<% else %>
																			 	<input type='text' name='dialPlan' id='dialPlan2' value="<%if tcWebApi_get("VoIPAdvanced_Common", "DialPlan", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "DialPlan", "s") else asp_Write("") end if %>">
																			<%end if%>
																		</TD>
																	</TR>
																	<tr >
																		<td ><%tcWebApi_get("String_Entry","VoipStep168Text","s")%></td>
																		<td >
																			<input type='text' id='megacodmstarttime' name='megacodmstarttime' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "StartDigitTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "StartDigitTimer", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep169Text","s")%></td>
																		<td>
																			<input type='text' id='megacodmshorttime' name='megacodmshorttime' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "InterDigitTimerShort", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "InterDigitTimerShort", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep170Text","s")%></td>
																		<td>
																		 	<input type='text' id='megacodmlongtime' name='megacodmlongtime' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "InterDigitTimerLong", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "InterDigitTimerLong", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep171Text","s")%></td>
																		<td>
																			<input type='text' id='megacodmttime' name='megacodmttime' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "InterDigitTimerT", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "InterDigitTimerT", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep172Text","s")%></td>
																		<td>
																			<input type='text' id='busytonetimer' name='busytonetimer' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "BusyToneTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "BusyToneTimer", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep173Text","s")%></td>
																		<td>
																			<input type='text' id='hangingremindertimer' name='hangingremindertimer' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "HangingReminderToneTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "HangingReminderToneTimer", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep174Text","s")%></td>
																		<td>
																			<input type='text' id='noanswertimer' name='noanswertimer' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPDigitMap_Entry", "NoAnswerTimer", "h") <> "N/A" then tcWebAPI_get("VoIPDigitMap_Entry", "NoAnswerTimer", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep175Text","s")%></td>
																		<td>
																			<select id="dmtmode" name="dmtmode" size="1" >
																				<option value="0" <%if tcWebApi_get("VoIPDigitMap_Entry", "TMode", "h")  = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep176Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPDigitMap_Entry", "TMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep177Text","s")%></option>
																				<option value="2" <%if tcWebApi_get("VoIPDigitMap_Entry", "TMode", "h") = "2" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep178Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep179Text","s")%></td>
																		<td>
																			<select id="dtmfRelay" name="dtmfRelay" size="1"  onchange="dtmfRelayChange(this)">
																				<option value="SIPInfo" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "SIPInfo" then asp_Write("selected") end if %>>
																					SIPInfo
																				<option value="RFC2833" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "RFC2833" then asp_Write("selected") end if %>>
																					RFC2833
																				<option value="InBand" <%if tcWebApi_get("VoIPAdvanced_Common", "DTMFTransportMode", "h") = "InBand" then asp_Write("selected") end if %>>
																					InBand
																			</select>
																		</td>
																	</tr>
																	<tr id="trRFC2833PT">
																		<td>Telephone Event Payload Type:</td>
																		<td>
																			 <input type='text' size=20 maxlength=20 id='rfc2833pt' name='rfc2833pt' onchange='limitNum(this)' value="<%if tcWebApi_get("VoIPBasic_Common", "TelephoneEventPayloadType", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "TelephoneEventPayloadType", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep180Text","s")%></td>
																		<td>
																			<select id="hookFlashRelay" name="hookFlashRelay" size="1">
																				<option value="SIPInfo" <%if tcWebApi_get("VoIPAdvanced_Common", "FlashOffTransSetting", "h") = "SIPInfo" then asp_Write("selected") end if %>>
																					SIPInfo
																				<option value="None" <%if tcWebApi_get("VoIPAdvanced_Common", "FlashOffTransSetting", "h") = "None" then asp_Write("selected") end if %>>
																					None
																			</select>
																		</td>
																	</tr>
																	<tr style="display:none">
																		<td><%tcWebApi_get("String_Entry","VoipStep181Text","s")%></td>
																		<td>
																			<select id="transport" name="transport" size="1">
																				<option value="UDP" <%if tcWebApi_get("VoIPBasic_Common", "SIPTransportProtocol", "h") = "UDP" then asp_Write("selected") end if %>>
																					UDP
																				<option value="TCP" <%if tcWebApi_get("VoIPBasic_Common", "SIPTransportProtocol", "h") = "TCP" then asp_Write("selected") end if %>>
																					TCP
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep182Text","s")%></td>
																		<td>
																			<input type='checkbox' id='natKeepAliveMessage' name='natKeepAliveMessage' <%if tcWebApi_get("VoIPBasic_Common", "HeartbeatSwitch", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep183Text","s")%></td>
																		<td>
																			<input type='text' id='natKeepAliveTime' name='natKeepAliveTime' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPBasic_Common", "HeartbeatCycle", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "HeartbeatCycle", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<% if tcWebApi_get("WebCustom_Entry", "isCTPONC9Supported", "h") = "Yes"  then %>
																		<tr>
																			<td><%tcWebApi_get("String_Entry","VoipStep184Text","s")%></td>
																			<td>
																				<select id="fromtoheadurl" name="fromtoheadurl" size="1">
																					<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "FromToHeadURI", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep185Text","s")%></option>
																					<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "FromToHeadURI", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep186Text","s")%></option>
																					<option value="2" <%if tcWebApi_get("VoIPAdvanced_Common", "FromToHeadURI", "h") = "2" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep187Text","s")%></option>
																				</select>
																			</td>
																		</tr>
																	<% end if %>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep188Text","s")%></td>
																		<td>
																			<select id="optionssericemode" name="optionssericemode" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "OptionsSericeMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep189Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "OptionsSericeMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep190Text","s")%></option>
																				<option value="2" <%if tcWebApi_get("VoIPAdvanced_Common", "OptionsSericeMode", "h") = "2" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep191Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep192Text","s")%></td>
																		<td>
																			<select id="faxctrlmode" name="faxctrlmode" size="1">
																				<option value="other" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxCtrlMode", "h") = "other" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep193Text","s")%></option>
																				<option value="all" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxCtrlMode", "h") = "all" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep194Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep195Text","s")%></td>
																		<td>
																			<select id="faxpassthru" name="faxpassthru" size="1">
																				<option value="PCMU" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxPassThruCodec", "h") = "PCMU" then asp_Write("selected") end if %>>PCMU</option>
																				<option value="PCMA" <%if tcWebApi_get("VoIPAdvanced_Common", "FaxPassThruCodec", "h") = "PCMA" then asp_Write("selected") end if %>>PCMA</option>
																			</select>
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep196Text","s")%></td>
																		<td>
																			<input id="siprtpredundancy" name="siprtpredundancy" type="checkbox" <%if tcWebApi_get("VoIPAdvanced_Common", "rtpRedundantEnable", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																			<td><%tcWebApi_get("String_Entry","VoipStep197Text","s")%></td>
																			<td>
																			<input name="siprtpredundancypt" type="text" id="sip_rtp_redundancy_pt_val_1" onchange="limitNum(this)"  value="<%if tcWebApi_get("VoIPAdvanced_Common", "rtpRedundantPT", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "rtpRedundantPT", "s") else asp_Write("") end if %>">
																				&nbsp;(96-127)
																			</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep198Text","s")%></td>
																		<td>
																			<select id="DivHotLineType" name="DivHotLineType" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "HotLineMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep199Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "HotLineMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep200Text","s")%></option>
																				<option value="2" <%if tcWebApi_get("VoIPAdvanced_Common", "HotLineMode", "h") = "2" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep201Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep202Text","s")%></td>
																		<td>
																			<select id="DivIsRegisterWithPort" name="DivIsRegisterWithPort" size="1">
																				<option value="Yes" <%if tcWebApi_get("VoIPAdvanced_Common", "RegResponsePortEnable", "h") = "Yes" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep203Text","s")%></option>
																				<option value="No" <%if tcWebApi_get("VoIPAdvanced_Common", "RegResponsePortEnable", "h") = "No" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep204Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep205Text","s")%></td>
																		<td>
																			<select id="DivIsMsgWithUser" name="DivIsMsgWithUser" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "MsgWithUserEnable", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep206Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "MsgWithUserEnable", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep207Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep208Text","s")%></td>
																		<td>
																			<input type='text' id='DivHotLineActiveCode' name='DivHotLineActiveCode' value="<%if tcWebApi_get("VoIPAdvanced_Common", "HotLineActNum", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "HotLineActNum", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep209Text","s")%></td>
																		<td>
																			<select id="DivCWType" name="DivCWType" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "CallWaitType", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep210Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "CallWaitType", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep211Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep212Text","s")%></td>
																		<td>
																			<select id="DivDialtone" name="DivDialtone" size="1">
																				<option value="Yes" <%if tcWebApi_get("VoIPAdvanced_Common", "NoRegDialToneEnable", "h") = "Yes" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep213Text","s")%></option>
																				<option value="No" <%if tcWebApi_get("VoIPAdvanced_Common", "NoRegDialToneEnable", "h") = "No" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep214Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep215Text","s")%></td>
																		<td>
																			<select id="DivCodecSelectFlag" name="DivCodecSelectFlag" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "VoiceCodecPriorityCtrl", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep216Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "VoiceCodecPriorityCtrl", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep217Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep218Text","s")%></td>
																		<td>
																			<select id="DivDigitMatchMethod" name="DivDigitMatchMethod" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "NumberMatchMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep219Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "NumberMatchMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep220Text","s")%></option>
																				<option value="2" <%if tcWebApi_get("VoIPAdvanced_Common", "NumberMatchMode", "h") = "2" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep221Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep222Text","s")%></td>
																		<td>
																			<select id="DivSupplementServiceType" name="DivSupplementServiceType" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "ActLocalSupplementService", "h") = "0" then asp_Write("selected") end if %>>None</option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "ActLocalSupplementService", "h") = "1" then asp_Write("selected") end if %>>Local</option>
																				<option value="2" <%if tcWebApi_get("VoIPAdvanced_Common", "ActLocalSupplementService", "h") = "2" then asp_Write("selected") end if %>>INFO</option>
																				<option value="3" <%if tcWebApi_get("VoIPAdvanced_Common", "ActLocalSupplementService", "h") = "3" then asp_Write("selected") end if %>>CTC_IMS</option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep223Text","s")%></td>
																		<td>
																			<select id="EndCharacterHandleMode" name="EndCharacterHandleMode" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "EndCharHandleMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep224Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "EndCharHandleMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep225Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep226Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='changeCharEnable' ID="Checkbox6" value='YES' <%if tcWebApi_get("VoIPAdvanced_Common", "SupportSpecCharTransfer", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep227Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='changeReferCharEnable' ID="Checkbox6" value='YES' <%if tcWebApi_get("VoIPAdvanced_Common", "SupReferSpecCharTrans", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep228Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='t38Enable' ID="Checkbox7" value='1'  <%if tcWebApi_get("VoIPMedia_Common", "FaxCodec", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep229Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='comfortnoiseEnable' ID="Checkbox81" value='YES' <%if tcWebApi_get("VoIPMedia_Entry0", "VAD", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep230Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='vadEnable' ID="Checkbox82" value='YES' <%if tcWebApi_get("VoIPMedia_Entry0", "SilenceCompressionEnable", "h") = "Yes" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep231Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='PrackEnable' ID="Checkbox9" value='YES' <%if tcWebApi_get("VoIPBasic_Common", "PRACKEnable", "h") = "Yes" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep232Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='sendUnregister' ID="Checkbox10" value='YES' <%if tcWebApi_get("VoIPBasic_Common", "LogOffMsgSendEnable", "h") = "Yes" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep233Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='autoSynPhoneDateSwitch' ID="Checkbox12" value='YES' <%if tcWebApi_get("VoIPAdvanced_Common", "CallSynEnAutoSwitch", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep234Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='syncSystemDate' ID="Checkbox13" value='YES' <%if tcWebApi_get("VoIPAdvanced_Common", "SyncSysTimeEnable", "h") = "Yes" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep235Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='VoIP8021pEnable' onClick='Priority8021PClick(this)' ID="Checkbox11" value='0' <%if tcWebApi_get("VoIPBasic_Common", "VoIP8021PMarkEnable", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr id="8021psip">
																		<td>802.1p(SIP)[0-7]:</td>
																		<td>
																			<input type='text' id='VoIP8021PSIP' name='VoIP8021PSIP' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPBasic_Common", "SIP8021PMark", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SIP8021PMark", "s") else asp_Write("") end if %>">
																				&nbsp;
																		</td>
																	</tr>
																	<tr id="8021prtp">
																		<td>802.1p(RTP)[0-7]:</td>
																		<td>
																			<input type='text' id='VoIP8021PRTP' name='VoIP8021PRTP' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPBasic_Common", "RTP8021PMark", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "RTP8021PMark", "s") else asp_Write("") end if %>">
																				&nbsp;
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep236Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='sessiontimerEnable' onClick='sessionClick(this)' ID="Checkbox11" value='0' <%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_FLAG", "h") = "1" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr id="sesstimer">
																		<td><%tcWebApi_get("String_Entry","VoipStep237Text","s")%></td>
																		<td>
																			<input type='text' id='sessionTimer' name='sessionTimer' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_TIMER", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_TIMER", "s") else asp_Write("") end if %>">
																				&nbsp;<%tcWebApi_get("String_Entry","VoipStep238Text","s")%>
																		</td>
																	</tr>
																	<tr id="sessminse">
																		<td><%tcWebApi_get("String_Entry","VoipStep239Text","s")%></td>
																		<td>
																			<input type='text' id='sessionMinSE' name='sessionMinSE' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_MIN_EXP", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_MIN_EXP", "s") else asp_Write("") end if %>">
																				&nbsp;<%tcWebApi_get("String_Entry","VoipStep238Text","s")%>
																		</td>
																	</tr>
																	<tr id="sessrefresher" >
																		<td><%tcWebApi_get("String_Entry","VoipStep240Text","s")%></td>
																		<td>
																			<select id="sessionRefresher" name="sessionRefresher" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_REFRESHER", "h") = "0" then asp_Write("selected") end if %>>
																					none
																				<option value="1" <%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_REFRESHER", "h") = "1" then asp_Write("selected") end if %>>
																					uac
																				<option value="2" <%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_REFRESHER", "h") = "2" then asp_Write("selected") end if %>>
																					uas
																			</select>
																		</td>
																	</tr>
																	<tr id="sessmethod" >
																		<td><%tcWebApi_get("String_Entry","VoipStep241Text","s")%></td>
																		<td>
																			<select id="sessionMethod" name="sessionMethod" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_METHOD", "h") = "0" then asp_Write("selected") end if %>>
																					UPDATE
																				<option value="1" <%if tcWebApi_get("VoIPBasic_Common", "SC_ACCT_SIP_SESSION_METHOD", "h") = "1" then asp_Write("selected") end if %>>
																					INVITE
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep242Text","s")%></td>
																		<td>
  																			<input type='text' size=20 maxlength=20 id='dscpmark' name='dscpmark' onchange='limitNum(this)' value="<%if tcWebApi_get("VoIPBasic_Common", "SIPDSCPMark", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "SIPDSCPMark", "s") else asp_Write("") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep243Text","s")%></td>
																		<td><input name="mymgtos" type="text" id="in_h_tos_val" onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPBasic_Common", "RTPDSCPMark", "h") <> "N/A" then tcWebAPI_get("VoIPBasic_Common", "RTPDSCPMark", "s") else asp_Write("") end if %>"></td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep244Text","s")%></td>
																		<td>
																			<select id="DivRevPolarityType" name="DivRevPolarityType" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPAdvanced_Common", "PolarityReversalMode", "h") = "0" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep245Text","s")%></option>
																				<option value="1" <%if tcWebApi_get("VoIPAdvanced_Common", "PolarityReversalMode", "h") = "1" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep246Text","s")%></option>
																				<option value="2" <%if tcWebApi_get("VoIPAdvanced_Common", "PolarityReversalMode", "h") = "2" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep247Text","s")%></option>
																			</select>
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep248Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='echoCancelEnable' ID="Checkbox8" value='YES' <%if tcWebApi_get("VoIPMedia_Common", "EchoCancellationEnable", "h") = "Yes" then asp_Write("checked") end if %>>
																		</td>
																	</tr>
																	<tr >
																		<td><%tcWebApi_get("String_Entry","VoipStep249Text","s")%></td>
																		<td valign="middle">
																			<select id="EchoCancellationLowSpeedFax" name="EchoCancellationLowSpeedFax" size="1">
																				<option value="No" <%if tcWebApi_get("VoIPMedia_Commonn", "EchoCancellationLowSpeedFax", "h") = "No" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep245Text","s")%></option>
																				<option value="Yes" <%if tcWebApi_get("VoIPMedia_Common", "EchoCancellationLowSpeedFax", "h") = "Yes" then asp_Write("selected") end if %>><%tcWebApi_get("String_Entry","VoipStep250Text","s")%></option>
																				<option value="" <%if tcWebApi_get("VoIPMedia_Common", "EchoCancellationLowSpeedFax", "h") = "" then asp_Write("selected") end if %>>Auto</option>
																			</select>
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep251Text","s")%></td>
																		<td valign="middle">
																			<input type='checkbox' name='AJBEnable' onClick='AJBClick(this)' ID="Checkbox15" value='1' <%if tcWebApi_get("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_TYPE", "h") <> "2" then asp_Write("checked") end if%>>
																		</td>
																	</tr>
																	<tr id="JB_LEN">
																		<td><%tcWebApi_get("String_Entry","VoipStep252Text","s")%></td>
																		<td>
																			<input type='text' name='JBMaxLen' id='JBMaxLen' onchange="limitNum(this)" value="<%if tcWebApi_get("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_TYPE", "h") <> "2" then if tcWebApi_get("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_LEN", "h") <> "N/A" then tcWebAPI_get("VoIPAdvanced_Common", "SC_SYS_VOICE_JB_LEN", "s") else asp_Write("200") end if else asp_Write("200") end if %>">
																		</td>
																	</tr>
																	<tr>
																		<td><%tcWebApi_get("String_Entry","VoipStep253Text","s")%></td>
																		<td>
																			<select name="codecRate" size="1">
																				<option value="0" <%if tcWebApi_get("VoIPMedia_Common", "SC_ACCT_MEDIA_G723_RATE", "h") = "0" then asp_Write("selected") end if %>>5.3</option>
																			<option value="1" <%if tcWebApi_get("VoIPMedia_Common", "SC_ACCT_MEDIA_G723_RATE", "h") = "1" then asp_Write("selected") end if %>>6.3</option>
																		</td>
																	</tr>
															</TBODY>
														</TABLE>
														<div id="perLineTablePlaceholder1">
														</div>
														<TABLE border=0 cellSpacing=1 cellPadding=0 width="98%">
															<tr>
																<td height="10">
																</td>
															</tr>
															<tr>
																<td align="center">
																	<input type='hidden' value='0' id="VoIP_SIP_flag" name="VoIP_SIP_flag">
																	<input type='hidden' value='0' id="natKeepAliveMessageFlag" name="natKeepAliveMessageFlag">
																	<input type='hidden' value='0' id="changeCharEnableFlag" name="changeCharEnableFlag">
																	<input type='hidden' value='0' id="changeReferCharEnableFlag" name="changeReferCharEnableFlag">
																	<input type='hidden' value='0' id="t38EnableFlag" name="t38EnableFlag">
																	<input type='hidden' value='No' id="echoCancelEnableFlag" name="echoCancelEnableFlag">
																	<input type='hidden' value='No' id="comfortnoiseEnableFlag" name="comfortnoiseEnableFlag">
																	<input type='hidden' value='No' id="vadEnableFlag" name="vadEnableFlag">
																	<input type='hidden' value='No' id="PrackEnableFlag" name="PrackEnableFlag">
																	<input type='hidden' value='No' id="sendUnregisterFlag" name="sendUnregisterFlag">
																	<input type='hidden' value='0' id="autoSynPhoneDateSwitchFlag" name="autoSynPhoneDateSwitchFlag">
																	<input type='hidden' value='No' id="syncSystemDateFlag" name="syncSystemDateFlag">
																	<input type='hidden' value='2' id="AJBEnableFlag" name="AJBEnableFlag">
																	<input type='hidden' value='0' id="sessiontimerEnableFlag" name="sessiontimerEnableFlag">
																	<input type='hidden' value='0' id="VoIP8021pEnableFlag" name="VoIP8021pEnableFlag">
																	<input type='hidden' value='0' id="warmline0Flag" name="warmline0Flag">
																	<input type='hidden' value='0' id="warmline1Flag" name="warmline1Flag">
																	<input type='hidden' value='0' name="warmline2Flag">
																	<input type='hidden' value='0' name="warmline3Flag">
																	<input type='hidden' value='0' id="callwait0Flag" name="callwait0Flag">
																	<input type='hidden' value='0' id="callwait1Flag" name="callwait1Flag">
																	<input type='hidden' value='0' name="callwait2Flag">
																	<input type='hidden' value='0' name="callwait3Flag">
																	<input type='hidden' value='No' id="transfer0Flag" name="transfer0Flag">
																	<input type='hidden' value='No' id="transfer1Flag" name="transfer1Flag">
																	<input type='hidden' value='No' name="transfer2Flag">
																	<input type='hidden' value='No' name="transfer3Flag">
																	<input type='hidden' value='0' id="fwdAll0Flag" name="fwdAll0Flag">
																	<input type='hidden' value='0' id="fwdAll1Flag" name="fwdAll1Flag">
																	<input type='hidden' value='0' name="fwdAll2Flag">
																	<input type='hidden' value='0' name="fwdAll3Flag">
																	<input type='hidden' value='0' id="fwdBusy0Flag" name="fwdBusy0Flag">
																	<input type='hidden' value='0' id="fwdBusy1Flag" name="fwdBusy1Flag">
																	<input type='hidden' value='0' name="fwdBusy2Flag">
																	<input type='hidden' value='0' name="fwdBusy3Flag">
																	<input type='hidden' value='0' id="fwdNoAns0Flag" name="fwdNoAns0Flag">
																	<input type='hidden' value='0' id="fwdNoAns1Flag" name="fwdNoAns1Flag">
																	<input type='hidden' value='0' name="fwdNoAns2Flag">
																	<input type='hidden' value='0' name="fwdNoAns3Flag">
																	<input type='hidden' value='No' id="threepartyservice0Flag" name="threepartyservice0Flag">
																	<input type='hidden' value='No' id="threepartyservice1Flag" name="threepartyservice1Flag">
																	<input type='hidden' value='No' name="threepartyservice2Flag">
																	<input type='hidden' value='No' name="threepartyservice3Flag">
																	<input type='hidden' value='No' id="threepartyservermix0Flag" name="threepartyservermix0Flag">
																	<input type='hidden' value='No' id="threepartyservermix1Flag" name="threepartyservermix1Flag">
																	<input type='hidden' value='No' name="threepartyservermix2Flag">
																	<input type='hidden' value='No' name="threepartyservermix3Flag">
																	<input type='hidden' value='0' id="synphonedate0Flag" name="synphonedate0Flag">
																	<input type='hidden' value='0' id="synphonedate1Flag" name="synphonedate1Flag">
																	<input type='hidden' value='0' name="synphonedate2Flag">
																	<input type='hidden' value='0' name="synphonedate3Flag">
																	<input type="hidden" id="isMultiDigitMap" name="isMultiDigitMap" value="<% if tcWebApi_get("WebCustom_Entry","isMultiDigitMap","h") = "Yes" then asp_write("1") else asp_write("0") end if %>">
																	<% if tcWebApi_get("WebCustom_Entry","isMultiDigitMap","h") = "Yes" then %>
																		<input type='hidden' id='DigitMap1' name="DigitMap1" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap1", "s")%>">
																		<input type='hidden' id='DigitMap2' name="DigitMap2" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap2", "s")%>">
																		<input type='hidden' id='DigitMap3' name="DigitMap3" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap3", "s")%>">
																		<input type='hidden' id='DigitMap4' name="DigitMap4" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap4", "s")%>">
																		<input type='hidden' id='DigitMap5' name="DigitMap5" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap5", "s")%>">
																		<input type='hidden' id='DigitMap6' name="DigitMap6" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap6", "s")%>">
																		<input type='hidden' id='DigitMap7' name="DigitMap7" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap7", "s")%>">
																		<input type='hidden' id='DigitMap8' name="DigitMap8" value="<%tcWebAPI_get("VoIPDigitMap_Entry", "DigitMap8", "s")%>">
																	<% end if%>
																	<input type='hidden' value='0' id="DigitMapEnableFlag" name="DigitMapEnableFlag">
								
																	<input type='hidden' value='0' id="DigitMapSpecialEnableFlag" name="DigitMapSpecialEnableFlag">
																	<input type='hidden' value='0' id="PBXPrefixEnableFlag" name="PBXPrefixEnableFlag">
																	<input type='hidden' value='0' id="PBXDialSecondaryEnableFlag" name="PBXDialSecondaryEnableFlag">
																	<input type='hidden' value='0' id="siprtpredundancyflag" name="siprtpredundancyflag">
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
							<TD width=7 background=/img/panel3.gif></TD>
						<TD>&nbsp;</TD></TR>
						<TR>
							<TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
							<TD width=7 background=/img/panel3.gif></TD>
						<TD></TD></TR>
						<TR>
							<TD vAlign=top width=157 bgColor=#e7e7e7></TD>
							<TD width=7 background=/img/panel3.gif></TD>
						<TD></TD></TR>
				</TBODY></TABLE></TD></TR>
						<TR>
							<TD height=1>
							<TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
								border=0><TBODY>
								<TR>
								<TD width=162 bgColor=#ef8218></TD>
								<TD width=278 bgColor=#427594></TD>
								<TD width=196 bgColor=#427594><!-- InstanceEndEditable --></TD>
								<TD width=170 bgColor=#313031></TD>
								</TR>
							</TBODY></TABLE>
						</TD></TR>
</TBODY></TABLE></BODY></HTML>
