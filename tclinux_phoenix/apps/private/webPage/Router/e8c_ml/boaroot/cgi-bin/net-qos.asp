<%
if Request_Form("QoS_Flag") = "1" then 
	TCWebApi_set("WebCurSet_Entry","qos_id","Mode")
	TCWebApi_set("QoS_Common","Mode","TempleteString")
	TCWebApi_set("QoS_Common","SelectTag","SelectTag")
	TCWebApi_commit("QoS_Common")
	TCWebApi_save()	
elseif Request_Form("QoS_Flag") = "2" then
	TCWebApi_set("QoS_Common","Discipline","DiscplineFlag")
	TCWebApi_commit("QoS_Common")
	TCWebApi_save()	
end if

If Request_Form("SaveFlag") = "1" then	
	TCWebApi_set("WebCurSet_Entry","qos_id","Mode")
	TCWebApi_unset("QoS_Common")
	TCWebApi_set("QoS_Common","QoSOptType","QoSRuleFlag")
	TCWebApi_set("QoS_Common","Mode","TempleteString")
	TCWebApi_set("QoS_Common","Active","QosFlag")
	TCWebApi_set("QoS_Common","UplinkBandwidth","Bandwidth")
	TCWebApi_set("QoS_Common","Discipline","DiscplineFlag")
	TCWebApi_set("QoS_Common","EnableDSCPMark","EnDscpFlag")
	TCWebApi_set("QoS_Common","En8021PRemark","Enable8021P")

	if Request_Form("DiscplineFlag") = "CAR" then 
	if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then
		TCWebApi_set("QoSAttr_CarEntry","QueueBW1","Q1Car")
		TCWebApi_set("QoSAttr_CarEntry","QueueBW2","Q2Car")
		TCWebApi_set("QoSAttr_CarEntry","QueueBW3","Q3Car")
		TCWebApi_set("QoSAttr_CarEntry","QueueBW4","Q4Car")
		TCWebApi_set("QoSAttr_CarEntry","QueueBW5","Q5Car")
		TCWebApi_set("QoSAttr_CarEntry","QueueBW6","Q6Car")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoSAttr_CarEntry","QueueBW7","Q7Car")
		TCWebApi_set("QoSAttr_CarEntry","QueueBW8","Q8Car")
	end if				
	else
		TCWebApi_set("QoS_Common","QueueBW1","Q1Car")
		TCWebApi_set("QoS_Common","QueueBW2","Q2Car")
		TCWebApi_set("QoS_Common","QueueBW3","Q3Car")
		TCWebApi_set("QoS_Common","QueueBW4","Q4Car")
		TCWebApi_set("QoS_Common","QueueBW5","Q5Car")
		TCWebApi_set("QoS_Common","QueueBW6","Q6Car")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueBW7","Q7Car")
		TCWebApi_set("QoS_Common","QueueBW8","Q8Car")
	end if				
	end if	
		TCWebApi_set("QoS_Common","QueueSW1","Q1EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW2","Q2EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW3","Q3EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW4","Q4EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW5","Q5EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW6","Q6EnableFlag")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueSW7","Q7EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW8","Q8EnableFlag")
	end if				
		TCWebApi_set("QoS_Common","QueueP1","Q1PValue")
		TCWebApi_set("QoS_Common","QueueP2","Q2PValue")
		TCWebApi_set("QoS_Common","QueueP3","Q3PValue")
		TCWebApi_set("QoS_Common","QueueP4","Q4PValue")
		TCWebApi_set("QoS_Common","QueueP5","Q5PValue")
		TCWebApi_set("QoS_Common","QueueP6","Q6PValue")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueP7","Q7PValue")
		TCWebApi_set("QoS_Common","QueueP8","Q8PValue")
	end if				
	elseif Request_Form("DiscplineFlag") = "WRR" then
		TCWebApi_set("QoS_Common","EnableForceWeight","EnableForceFlag")
	if tcwebApi_get("WebCustom_Entry","isCTJOYME2Supported","h") = "Yes" then
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW1","Q1Weight")
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW2","Q2Weight")
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW3","Q3Weight")
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW4","Q4Weight")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW5","Q5Weight")
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW6","Q6Weight")
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW7","Q7Weight")
		TCWebApi_set("QoSAttr_WeightEntry","QueueBW8","Q8Weight")
	end if
	else
		TCWebApi_set("QoS_Common","QueueBW1","Q1Weight")
		TCWebApi_set("QoS_Common","QueueBW2","Q2Weight")
		TCWebApi_set("QoS_Common","QueueBW3","Q3Weight")
		TCWebApi_set("QoS_Common","QueueBW4","Q4Weight")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueBW5","Q5Weight")
		TCWebApi_set("QoS_Common","QueueBW6","Q6Weight")
		TCWebApi_set("QoS_Common","QueueBW7","Q7Weight")
		TCWebApi_set("QoS_Common","QueueBW8","Q8Weight")
	end if
	end if
		TCWebApi_set("QoS_Common","QueueSW1","Q1EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW2","Q2EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW3","Q3EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW4","Q4EnableFlag")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueSW5","Q5EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW6","Q6EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW7","Q7EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW8","Q8EnableFlag")
	end if
		TCWebApi_set("QoS_Common","QueueP1","Q1PValue")
		TCWebApi_set("QoS_Common","QueueP2","Q2PValue")
		TCWebApi_set("QoS_Common","QueueP3","Q3PValue")
		TCWebApi_set("QoS_Common","QueueP4","Q4PValue")		
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueP5","Q5PValue")
		TCWebApi_set("QoS_Common","QueueP6","Q6PValue")
		TCWebApi_set("QoS_Common","QueueP7","Q7PValue")
		TCWebApi_set("QoS_Common","QueueP8","Q8PValue")
	end if
	elseif Request_Form("DiscplineFlag") = "PQ" then 
		TCWebApi_set("QoS_Common","QueueSW1","Q1EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW2","Q2EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW3","Q3EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW4","Q4EnableFlag")
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueSW5","Q5EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW6","Q6EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW7","Q7EnableFlag")
		TCWebApi_set("QoS_Common","QueueSW8","Q8EnableFlag")
	end if
		TCWebApi_set("QoS_Common","QueueP1","Q1PValue")
		TCWebApi_set("QoS_Common","QueueP2","Q2PValue")
		TCWebApi_set("QoS_Common","QueueP3","Q3PValue")
		TCWebApi_set("QoS_Common","QueueP4","Q4PValue")		
	if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then
		TCWebApi_set("QoS_Common","QueueP5","Q5PValue")
		TCWebApi_set("QoS_Common","QueueP6","Q6PValue")
		TCWebApi_set("QoS_Common","QueueP7","Q7PValue")
		TCWebApi_set("QoS_Common","QueueP8","Q8PValue")
	end if
	end if
	
	TCWebApi_commit("QoS_Common")
	
	TCWebApi_save()	
End if

%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>Qos</TITLE>
<META http-equiv=Content-Language content=zh-cn>
<META http-equiv=Content-Type content="text/html; charset=gb2312"><LINK 
href="/JS/stylemain.css" type=text/css rel=stylesheet>
<STYLE type=text/css>
	
	TABLE {
    FONT-SIZE: 12px;
    FONT-FAMILY: "system-ui";
    FONT-SIZE: 12px;
}
</STYLE>
<SCRIPT language=javascript src="/JS/menu.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/jquery.js"></SCRIPT>
<SCRIPT language=javascript src="/JS/util.js"></SCRIPT>

<SCRIPT language=JavaScript type=text/javascript>
function btnSaveCheck()
{
	var  vForm = document.ConfigForm;
	GetTempleteString();
	checkboxstatus();
	convertNullValue();
	vForm.SaveFlag.value = "1";
	if( true == setEBooValueCookie(vForm) )
		vForm.submit();
}
function btnSave()
{
	var  vForm = document.ConfigForm;
	var QosSwitch ="<%tcWebApi_get("QoS_Common","Active","s") %>";
	GetTempleteString();
	checkboxstatus();
	convertNullValue();
<%if tcwebApi_get("Info_WLan", "isCUCSupport","h")="Yes" then%>
	if(vForm.DiscplineFlag.value == "WRR")
		if(checkIfInt(vForm.Q1Weight.value)==false  || checkIfInt(vForm.Q2Weight.value)==false  ||checkIfInt(vForm.Q3Weight.value)==false  ||checkIfInt(vForm.Q4Weight.value)==false
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
		||checkIfInt(vForm.Q5Weight.value)==false  ||checkIfInt(vForm.Q6Weight.value)==false  ||checkIfInt(vForm.Q7Weight.value)==false  ||checkIfInt(vForm.Q8Weight.value)==false
<% end if %>
		)
			return false;
<%end if%>
	if(vForm.DiscplineFlag.value == "WRR" && QosSwitch=="Yes")
	{
		var weightArr = new Array(vForm.Q1Weight.value, vForm.Q2Weight.value, vForm.Q3Weight.value, vForm.Q4Weight.value
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
								 , vForm.Q5Weight.value, vForm.Q6Weight.value, vForm.Q7Weight.value, vForm.Q8Weight.value
<% end if %>
		);
		var weightArrST = new Array(vForm.Q5Enable.checked, vForm.Q6Enable.checked, vForm.Q7Enable.checked, vForm.Q8Enable.checked
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
									, vForm.Q19Enable.checked, vForm.Q20Enable.checked, vForm.Q21Enable.checked, vForm.Q22Enable.checked
<% end if %>
		);
		var idx = 0;
		var weightTotal = 0;
		
		for ( idx = 0; idx < weightArr.length; idx ++ )
		{
			if ( false == isPlusInteger(weightArr[idx])
					|| ( weightArrST[idx] && parseInt(weightArr[idx]) <= 0 )
					)
			{
				alert('<%tcWebApi_get("String_Entry","InvalidWeightText","s")%>');
				return false;
			}
			
			//if ( weightArrST[idx] )
				weightTotal += parseInt(weightArr[idx]);
		}
		
		if ( weightTotal > 100 )
		{
				alert('<%tcWebApi_get("String_Entry","SumWRRWeightText","s")%>');
				return false;
		}
	}
	var bdw = vForm.Bandwidth.value;
	if (bdw!="0"){
	  if (parseInt(bdw) < 8192
	      || parseInt(bdw) > 1000000000
	      || !isInteger(bdw))
	  {
	      alert('<%tcWebApi_get("String_Entry","RangeUplinkBandwidthText","s")%>');
	      return false;
	  }
	}
	else
	{
		bdw = 1000000000;
	}
	
	if(vForm.DiscplineFlag.value == "CAR" && QosSwitch=="Yes")
	{
		var bandArr = new Array(vForm.Q1Car.value, vForm.Q2Car.value, vForm.Q3Car.value, vForm.Q4Car.value,vForm.Q5Car.value,vForm.Q6Car.value
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
								, vForm.Q7Car.value, vForm.Q8Car.value
<% end if %>
		);
		var bandArrST = new Array(vForm.Q9Enable.checked, vForm.Q10Enable.checked, vForm.Q11Enable.checked, vForm.Q12Enable.checked,vForm.Q13Enable.checked,vForm.Q14Enable.checked
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
								 , vForm.Q23Enable.checked, vForm.Q24Enable.checked
<% end if %>
		);
		var idx = 0;
		var bandTotal = 0;
		
		for ( idx = 0; idx < bandArr.length; idx ++ )
		{
			if ( false == isPlusInteger(bandArr[idx]))
			{
				alert('<%tcWebApi_get("String_Entry","InvalidBroadbandValueText","s")%>');
				return false;
			}
			
			if ( bandArrST[idx] )
				bandTotal += parseInt(bandArr[idx]);
		}
		
		if ( bandTotal*1000 > bdw )
		{
				alert('<%tcWebApi_get("String_Entry","SumBroadbandText","s")%>');
				return false;
		}
	}
	vForm.SaveFlag.value = "1";
	if( true == setEBooValueCookie(vForm) )
		vForm.submit();
}
function checkIfInt(weight)
{
	var len ;
	if(isNaN(weight) ){
		alert("please input an interger for weight!");
		return false;
	}
	len = weight.length;
	var ch;
	for(i=0;i<len;i++)
	{
		ch = weight.charAt(i);
		if(ch >'9' || ch<'0')
		{
			alert("please input interger for weight!");
			return false;
		}	
	}
	if(weight>15 || weight<1)
	{
		alert("weight must be 1-15!");
		return false;
	}
	return true;
	
}
function convertNullValue()
{
	var  vForm = document.ConfigForm;
	if(vForm.Q1Weight.value == "") vForm.Q1Weight.value = "0";
	if(vForm.Q2Weight.value == "") vForm.Q2Weight.value = "0";
	if(vForm.Q3Weight.value == "") vForm.Q3Weight.value = "0";
	if(vForm.Q4Weight.value == "") vForm.Q4Weight.value = "0";
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
	if(vForm.Q5Weight.value == "") vForm.Q5Weight.value = "0";
	if(vForm.Q6Weight.value == "") vForm.Q6Weight.value = "0";
	if(vForm.Q7Weight.value == "") vForm.Q7Weight.value = "0";
	if(vForm.Q8Weight.value == "") vForm.Q8Weight.value = "0";
<%end if%>
<%if tcwebApi_get("Info_WLan", "isCUCSupport","h")="Yes" then%>
	if(vForm.DiscplineFlag.value == "WRR")
		if(checkIfInt(vForm.Q1Weight.value)==false  || checkIfInt(vForm.Q2Weight.value)==false  ||checkIfInt(vForm.Q3Weight.value)==false  ||checkIfInt(vForm.Q4Weight.value)==false
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
		  checkIfInt(vForm.Q5Weight.value)==false  || checkIfInt(vForm.Q6Weight.value)==false  || checkIfInt(vForm.Q7Weight.value)==false  ||checkIfInt(vForm.Q8Weight.value)==false 
<%end if%>
		)
			return false;
<%end if%>
	if(vForm.Q1Car.value == "") vForm.Q1Car.value = "0";
	if(vForm.Q2Car.value == "") vForm.Q2Car.value = "0";
	if(vForm.Q3Car.value == "") vForm.Q3Car.value = "0";
	if(vForm.Q4Car.value == "") vForm.Q4Car.value = "0";
	if(vForm.Q5Car.value == "") vForm.Q5Car.value = "0";
	if(vForm.Q6Car.value == "") vForm.Q6Car.value = "0";	
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
	if(vForm.Q7Car.value == "") vForm.Q7Car.value = "0";
	if(vForm.Q8Car.value == "") vForm.Q8Car.value = "0";
<% end if %>
	return true;
}
function checkboxstatus()
{
	var  vForm = document.ConfigForm;
	if(vForm.Enable.checked)
		vForm.QosFlag.value = "Yes";
	else vForm.QosFlag.value = "No";
	
	if(vForm.EnableForceWeight.checked)
		vForm.EnableForceFlag.value = "Yes";
	else  vForm.EnableForceFlag.value = "No";
	if(vForm.EnableDSCPMark.checked)
		vForm.EnDscpFlag.value = "Yes";
	else vForm.EnDscpFlag.value = "No";
	
	if("priority" == getRadioVal("Plan"))
	{
		vForm.DiscplineFlag.value = "PQ";
		if(vForm.Q1Enable.checked) vForm.Q1EnableFlag.value = "Yes";
		else vForm.Q1EnableFlag.value = "No";
		
		if(vForm.Q2Enable.checked) vForm.Q2EnableFlag.value = "Yes";
		else vForm.Q2EnableFlag.value = "No";
		
		if(vForm.Q3Enable.checked) vForm.Q3EnableFlag.value = "Yes";
		else vForm.Q3EnableFlag.value = "No";
		
		if(vForm.Q4Enable.checked) vForm.Q4EnableFlag.value = "Yes";
		else vForm.Q4EnableFlag.value = "No";
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
		if(vForm.Q15Enable.checked) vForm.Q5EnableFlag.value = "Yes";
		else vForm.Q5EnableFlag.value = "No";

		if(vForm.Q16Enable.checked) vForm.Q6EnableFlag.value = "Yes";
		else vForm.Q6EnableFlag.value = "No";

		if(vForm.Q17Enable.checked) vForm.Q7EnableFlag.value = "Yes";
		else vForm.Q7EnableFlag.value = "No";

		if(vForm.Q18Enable.checked) vForm.Q8EnableFlag.value = "Yes";
		else vForm.Q8EnableFlag.value = "No";
<% end if %>
	}
	else if("weight" == getRadioVal("Plan"))
	{
		vForm.DiscplineFlag.value = "WRR";
		if(vForm.Q5Enable.checked) vForm.Q1EnableFlag.value = "Yes";
		else vForm.Q1EnableFlag.value = "No";
		
		if(vForm.Q6Enable.checked) vForm.Q2EnableFlag.value = "Yes";
		else vForm.Q2EnableFlag.value = "No";
		
		if(vForm.Q7Enable.checked) vForm.Q3EnableFlag.value = "Yes";
		else vForm.Q3EnableFlag.value = "No";
		
		if(vForm.Q8Enable.checked) vForm.Q4EnableFlag.value = "Yes";
		else vForm.Q4EnableFlag.value = "No";
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
		if(vForm.Q19Enable.checked) vForm.Q5EnableFlag.value = "Yes";
		else vForm.Q5EnableFlag.value = "No";
		
		if(vForm.Q20Enable.checked) vForm.Q6EnableFlag.value = "Yes";
		else vForm.Q6EnableFlag.value = "No";
		
		if(vForm.Q21Enable.checked) vForm.Q7EnableFlag.value = "Yes";
		else vForm.Q7EnableFlag.value = "No";
		
		if(vForm.Q22Enable.checked) vForm.Q8EnableFlag.value = "Yes";
		else vForm.Q8EnableFlag.value = "No";
<% end if %>
	}
	else if("car" == getRadioVal("Plan"))
	{
		vForm.DiscplineFlag.value = "CAR";
		if(vForm.Q9Enable.checked) vForm.Q1EnableFlag.value = "Yes";
		else vForm.Q1EnableFlag.value = "No";
		
		if(vForm.Q10Enable.checked) vForm.Q2EnableFlag.value = "Yes";
		else vForm.Q2EnableFlag.value = "No";
		
		if(vForm.Q11Enable.checked) vForm.Q3EnableFlag.value = "Yes";
		else vForm.Q3EnableFlag.value = "No";
		
		if(vForm.Q12Enable.checked) vForm.Q4EnableFlag.value = "Yes";
		else vForm.Q4EnableFlag.value = "No";
		
		if(vForm.Q13Enable.checked) vForm.Q5EnableFlag.value = "Yes";
		else vForm.Q5EnableFlag.value = "No";
		
		if(vForm.Q14Enable.checked) vForm.Q6EnableFlag.value = "Yes";
		else vForm.Q6EnableFlag.value = "No";
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
		if(vForm.Q23Enable.checked) vForm.Q7EnableFlag.value = "Yes";
		else vForm.Q7EnableFlag.value = "No";
		
		if(vForm.Q24Enable.checked) vForm.Q8EnableFlag.value = "Yes";
		else vForm.Q8EnableFlag.value = "No";
<% end if %>
	}
	if(vForm.oldDSCP.value != vForm.EnDscpFlag.value || vForm.old8021P.value != vForm.Enable8021P.value)
	{
		vForm.ReCommitFlg.value = "2";
	}
}

</SCRIPT>

<META content="MSHTML 6.00.6000.16809" name=GENERATOR></HEAD>
<BODY style="TEXT-ALIGN: center" vLink=#000000 aLink=#000000 link=#000000 
leftMargin=0 topMargin=0 
onload="DisplayLocation(getElement('Selected_Menu').value);FinishLoad();if(getElById('ConfigForm') != null)LoadFrame()" 
onunload=DoUnload() marginheight="0" marginwidth="0">
<TABLE height="100%" cellSpacing=0 cellPadding=0 width=808 align=center 
border=0>
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
                <TD class=welcom vAlign=bottom align=middle width=120><%tcWebApi_get("String_Entry","TitleWelcomeText","s")%></TD>
                <TD vAlign=bottom width=50><A onclick=DoLogout() 
                  href="/cgi-bin/login.asp" id=logOut name=logOut target=_top><SPAN 
                  class=logout><%tcWebApi_get("String_Entry","TitleLogOutText","s")%>
      </SPAN></A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
      <TABLE id=table2 height=100 cellSpacing=0 cellPadding=0 width=808 
border=0>
        <TBODY>
        <TR>
          <TD class=LocationDisplay id=LocationDisplay align=middle width=163 
          bgColor=#ef8218 rowSpan=3></TD>
          <TD width=434 bgColor=#427594 height=33>
            <P align=right><FONT face=<%tcWebApi_get("String_Entry","RomanText","s")%> color=#ffffff><B><FONT face=<%tcWebApi_get("String_Entry","RomanText","s")%>
            color=#ffffff size=6><INPUT id=Selected_Menu type=hidden 
            value="<%tcWebApi_get("String_Entry","ContentNetText","s")%>-><%tcWebApi_get("String_Entry","ContentQosText","s")%>" name=Net_Qos> </FONT></B><SPAN 
            class=GatewayName><%tcWebApi_get("String_Entry","TitleGateWayNameText","s")%>
            <SCRIPT language=javascript>
				document.write(top.gateWayName);
			</SCRIPT>
             </SPAN></FONT></P></TD>
          <TD width=211 bgColor=#ef8218 height=33>
            <P class=GatewayType align=center><%tcWebApi_get("String_Entry","TitleGateWayTypeText","s")%>
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
	<SCRIPT language=JavaScript type=text/javascript>
		function QosModeConstruction(domain,Mode,Enable,Bandwidth,Plan,EnableForceWeight,EnableDSCPMark,Enable8021P)
		{
			this.domain = domain;
			this.Mode = Mode;
			this.Enable = Enable;
			this.Bandwidth = Bandwidth;
			this.Plan = Plan;
			this.EnableForceWeight = EnableForceWeight;
			this.EnableDSCPMark = EnableDSCPMark;
			this.Enable8021P = Enable8021P;
		}
		var CurMode = new Array(new QosModeConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS","OTHER","0","0","priority","0","0","0"),null);
		
		function QosAppConstruction(domain,AppName,ClassQueue)
		{
			this.domain = domain;
			this.AppName = AppName;
			this.ClassQueue = ClassQueue;
		}
		var CurApp = new Array(new QosAppConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.App.1","","4"),new QosAppConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.App.2","","4"),null);
		
		function QosClassConstruction(domain,ClassQueue,Type,Max,Min,ProtocolList,DSCPMarkValue,Value8021P)
		{
			this.domain = domain;
			this.ClassQueue = ClassQueue;
			this.Type = Type;
			this.Max  = Max;
			this.Min  = Min;
			this.ProtocolList  = ProtocolList;
			this.DSCPMarkValue = DSCPMarkValue;
			this.Value8021P    = Value8021P;
		}
		var CurClassArray = new Array(null);
		
		function QosQueueConstruction(domain,Enable,Priority,Weight)
		{
			this.domain = domain;
			this.Enable = Enable;
			this.Priority = Priority;
			this.Weight = Weight;
		}
		var CurQueue = new Array(new QosQueueConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.PriorityQueue.1","0","1","0"),new QosQueueConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.PriorityQueue.2","0","2","0"),new QosQueueConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.PriorityQueue.3","0","3","0"),new QosQueueConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.PriorityQueue.4","0","4","0"),new QosQueueConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.PriorityQueue.5","0","5","0"),new QosQueueConstruction("InternetGatewayDevice.X_CT-COM_UplinkQoS.PriorityQueue.6","0","6","0"),null);
		
		function Qos(Mode,App,Class,Queue)
		{
		this.Mode  = Mode;
		this.App   = App;
		this.Class = Class;
		this.Queue = Queue;
		}
		var CurQoS = new Qos(CurMode,CurApp,CurClassArray,CurQueue);
		var AppCnt = 0;
		var ClsCnt = 0;
		var QueueCnt= 0;
		var ClsAddFlag  = false;
		
		function CurQoSShow()
		{
			with (getElById('ConfigForm'))
			{
				var	vForm = document.ConfigForm;
				if (!vForm.Enable.checked)
				{
					setDisplay('QOSGlobe', 0);
					setDisplay('COMVlanBtn', 0);
					setDisplay('QueueEdit',0);
					setDisplay('AddBtn',0);
					return;
				}
				else
				{
					setDisplay('QOSGlobe', 1);
					setDisplay('COMVlanBtn', 1);
					setDisplay('QueueEdit',1);
					setDisplay('AddBtn',1);
				}
				if(vForm.DiscplineFlag.value == "PQ")
				{
					setDisplay('PQEdit',1);
					setDisplay('WRREdit',0);
					setDisplay('CAREdit',0);
				}
				else if (vForm.DiscplineFlag.value == "WRR")
				{
					setDisplay('PQEdit',0);
					setDisplay('WRREdit',1);
					setDisplay('CAREdit',0);
				}
				else
				{
					setDisplay('PQEdit',0);
					setDisplay('WRREdit',0);
					setDisplay('CAREdit',1);
				}
				AppCnt = CurQoS.App.length - 1;
				ClsCnt = CurQoS.Class.length - 1;
				QueueCnt = CurQoS.Queue.length - 1;
			}
		}
		
		function QoSEnableChange()
		{
			with (getElById('ConfigForm'))
			{
				var vForm = document.ConfigForm;
				var url;
				var SubmitForm = new webSubmitForm();
			
				if(Enable.checked == false)
				{
					if(!confirm("<%tcWebApi_get("String_Entry","Whether1Text","s")%>"))
					{
					Enable.checked = true;
					vForm.SaveFlag.value = "0";
					return;
					}
					setDisplay('QueueEdit',0);
					setDisplay('AddBtn',0);
					setDisplay('QOSGlobe',0);
					setDisplay('COMVlanBtn',0);					
				}
				else
				{
					if(!confirm("<%tcWebApi_get("String_Entry","Whether2Text","s")%>"))
					{
						Enable.checked = false;
						vForm.SaveFlag.value = "0";
						return;
					}
					vForm.ReCommitFlg.value = "1";
				}
				btnSaveCheck();
			}
		}
		
		function GetTempleteString()
		{
			var Form = document.ConfigForm;
			var selectValue = getSelectVal('Mode');
			switch (selectValue)
			{
				case "20":
					Form.TempleteString.value = "TR069,INTERNET";
				break;
				case "21":
					Form.TempleteString.value = "TR069,VOIP,INTERNET";
				break;
				case "22":
					Form.TempleteString.value = "TR069,IPTV,INTERNET";
				break;
				case "23":
					Form.TempleteString.value = "TR069,VOIP,IPTV,INTERNET";
				break;
				case "24":
					Form.TempleteString.value = "OTHER";
				break;
				default:
					Form.TempleteString.value = selectValue;
				break;
			}
		}
		
		function QoSChangeMode()
		{
			document.ConfigForm.QoS_Flag.value = "1";
			GetTempleteString();
			if( true == setEBooValueCookie(document.ConfigForm) )
				document.ConfigForm.submit();
		}
		
		function PlanChange()
		{
			var Form = new webSubmitForm();
			var url ;
			with (getElById('ConfigForm'))
			{
				if (false == confirm("<%tcWebApi_get("String_Entry","Whether3Text","s")%>"))
				{
					if(DiscplineFlag.value == "PQ")
						Plan[0].checked = true;
					else if((DiscplineFlag.value == "WRR"))
						Plan[1].checked = true;
					else
						Plan[2].checked = true;
					return;
				}
				checkboxstatus();
				document.ConfigForm.QoS_Flag.value = "2";
				if( true == setEBooValueCookie(document.ConfigForm) )
					document.ConfigForm.submit();
			}
		}
		
		function LoadFrame()
		{
			if ((document.ConfigForm.DiscplineFlag.value == "PQ") || (document.ConfigForm.DiscplineFlag.value == "CAR"))//0  or 2
			{
				setDisplay('EnableForce', 0);
			}
			document.ConfigForm.QoS_Flag.value = "0";
			document.ConfigForm.SaveFlag.value = "0";
			document.ConfigForm.ReCommitFlg.value = "-1";
		}
		
		function VlanSubmit(filename)
		{
			location.replace(filename);
		}
		
		function initQoSTemplate()
	  {
	  	var templateName = '<% tcWebApi_get("QoS_Common","Mode","s") %>';
		var templateArray;
	  	var templateArray1 = new Array(
	  								<%if tcwebApi_get("WebCustom_Entry","isCTPONHNSupported","h") <> "Yes" then%>
	  								<%if tcwebApi_get("WebCustom_Entry","isCTPONCZNXSupported","h") <> "Yes" then%>
										<% if  tcwebApi_get("WebCustom_Entry","isCTPONNXSupported","h") = "Yes" then %>
										 'INTERNET,TR069'
										<% else %>
										 'TR069,INTERNET'
										<% end if %>
							
										<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") <> "Yes" then %>
										<% if tcwebApi_get("WebCustom_Entry","isCTPONCNJSSupported","h") = "Yes" then %>
										, 'VOIP,TR069,INTERNET'
										<% elseif  tcwebApi_get("WebCustom_Entry","isCTPONNXSupported","h") = "Yes" then %>
										, 'INTERNET,TR069,VOIP'
										<% else %>
										, 'TR069,VOIP,INTERNET'
										<% end if %>
										<% end if %>
							
										<% if tcwebApi_get("WebCustom_Entry","isCT1PORTSupported","h") <> "Yes" then %>
										<% if tcwebApi_get("WebCustom_Entry","isCTPONJSSupported","h") = "Yes" then %>
										, 'IPTV,TR069,INTERNET'
										<% elseif  tcwebApi_get("WebCustom_Entry","isCTPONNXSupported","h") = "Yes" then %>
										, 'INTERNET,TR069,IPTV'
										<% else %>
										, 'TR069,IPTV,INTERNET'
										<% end if %>
										<% end if %>
							
										<% if tcwebApi_get("WebCustom_Entry","isCTPONNOVOIP","h") <> "Yes" then %>
										<% if tcwebApi_get("WebCustom_Entry","isCTPONCNJSSupported","h") = "Yes" then %>
										, 'VOIP,IPTV,TR069,INTERNET'
										<% elseif  tcwebApi_get("WebCustom_Entry","isCTPONNXSupported","h") = "Yes" then %>
										, 'INTERNET,TR069,VOIP,IPTV'
										<% else %>
										, 'TR069,VOIP,IPTV,INTERNET'
										<% end if %>
										<% end if %>
										<% else %>
										'TR069,INTERNET'
										, 'VOIP,TR069,INTERNET'
										, 'IPTV,TR069,INTERNET'
										, 'VOIP,IPTV,TR069,INTERNET'
										<% end if %>
										, 'OTHER');
										<% else %>
										'TR069,VOIP,IPTV,INTERNET');
										<% end if %>

		var templateArray2 = new Array('IPTV,INTERNET','OTHER');

		<% if tcwebApi_get("WebCustom_Entry","isCwmpSupported","h") <> "Yes" then %>
			<% if tcwebApi_get("WebCustom_Entry","isVOIPSupported","h") <> "Yes" then %>
				templateArray = templateArray2;
			<% end if %>
		<% else %>
			templateArray = templateArray1;
		<% end if %>
					
			var i = 0;
	  	var cusname = templateName;
	  	var hascusopt = true;
	  	var isSel = 0;

  		with (getElById('Mode'))
			{
				for( i=0; i< templateArray.length; i++)
				{
					var opt = new Option(templateArray[i], templateArray[i]);
					if ( templateName == templateArray[i] )
					{
						opt.selected = true
						isSel = i;
						hascusopt = false;
					}
					options.add ( opt );
					options[isSel].setAttribute('selected', 'true');
				}

				if ( hascusopt && 0 != templateName.length )
				{
					var optcus = new Option(cusname, cusname);
					optcus.selected = true
					options.add ( optcus );
					isSel = i;
					options[isSel].setAttribute('selected', 'true');
				}
			}
	  }
	</SCRIPT>

      <TABLE height="100%" cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=157 bgColor=#ef8218 height=30>
            <P class=Item_L1>QoS</P></TD>
          <TD width=7 bgColor=#ef8218>　</TD>
          <TD width=474>　</TD>
          <TD vAlign=top width=170 background=/img/panel4.gif rowSpan=5>
            <TABLE cellSpacing=0 cellPadding=20 width="100%" border=0 height='100%'>
              <TBODY>
              <TR>
                <TD valign='top'><A 
                  href="/cgi-bin/help_content.asp#QoS" 
                  target=_blank><IMG height=34 src="<%tcWebApi_get("String_Entry","IMGHelpText","s")%>" width=40 
                  border=0></A></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isWebTYLOGOSupported","h") = "Yes" then %>
              <TR>
                <TD valign='bottom'><IMG src='/img/tybottom.jpg' height=76 width=112></TD></TR>
<% end if %>
              </TBODY></TABLE>　 </TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=30>
            <P class=Item_L2></P></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD>
            <FORM name="ConfigForm" action="/cgi-bin/net-qos.asp" method="post">
            <TABLE cellSpacing=0 cellPadding=0 width="100%" border=0>
              <TBODY>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=108><B><%tcWebApi_get("String_Entry","RuleTemplateText","s")%></B></TD>
                                <TD colSpan=6>
																<select id=select onChange="QoSChangeMode()" size=1 name="Mode" id='Mode'>
                                </select> 
			                        <SCRIPT language=JavaScript type=text/javascript>
															initQoSTemplate();
															</SCRIPT>
                                  <input type="hidden" name="QoS_Flag" value="0"> </TD>
                              </TR>
                    <TR>
                      <TD height=30><B><%tcWebApi_get("String_Entry","EnableText","s")%>QoS??</B></TD>
                      <TD width=392><INPUT id="Enable" onclick="QoSEnableChange()" type=checkbox value=0 name="Enable" <%if TCWebApi_get("QoS_Common", "Active","h") = "Yes" then asp_Write("checked") end if%>>
                                  <input type="hidden" id="QosFlag" name="QosFlag" value="Yes">
                                  <input type="hidden" id="UpBandValue" name="UpBandValue" value="<% tcWebApi_get("QoS_Common","UplinkBandwidth","s") %>">
                                  <input type="hidden" id="DiscplineFlag" name="DiscplineFlag" value="<% tcWebApi_get("QoS_Common","Discipline","s") %>">
                                  <input type="hidden" id="QoSRuleFlag" name="QoSRuleFlag" value="discRule">
                                  <input type="hidden" id="TempleteString" name="TempleteString" value="TR069,INTERNET">
                                  <input type="hidden" id="SelectTag" name="SelectTag" value="1"></TD>
                              </TR></TBODY></TABLE>
                  <DIV id=QOSGlobe>
                  <TABLE width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=108><B><%tcWebApi_get("String_Entry","UplinkBandwidthText","s")%></B></TD>
                                  <TD width=122><input id=Bandwidth maxlength=10 size=16 value="<%if tcWebApi_get("QoS_Common","UplinkBandwidth","h") <> "N/A" then tcWebApi_get("QoS_Common","UplinkBandwidth","s") else asp_Write("0") end if%>" name="Bandwidth">
                                    <script language="JavaScript" type="text/JavaScript">
									</script> 
                                  </TD>
                      <TD width=280>(0,8192-1000000000)bps</TD></TR></TBODY></TABLE>
                  <TABLE width="100%" border=0 <% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported","h") = "Yes" then if tcWebApi_get("DataSpeedLimit_Entry","SpeedLimitModeUP","h") <> "0" then %>style='display:none' <% end if end if %>>
                    <TBODY>
                    <TR>
                      <TD width=310 height=30><B><%tcWebApi_get("String_Entry","SchedulingPolicyText","s")%></B></TD>
                      <TD width=30><INPUT id="Plan" onclick="PlanChange()" type=radio value="priority" name="Plan" <%if tcWebApi_get("QoS_Common","Discipline","h") = "PQ" then asp_Write("checked") end if%>></TD>
                      <TD width=180>PQ</TD>
                      <TD width=30><INPUT id="Plan" onclick="PlanChange()" type=radio value="weight" name="Plan" <%if tcWebApi_get("QoS_Common","Discipline","h") = "WRR" then asp_Write("checked") end if%>></TD>
                      <TD width=180>WRR</TD>
                      <TD width=30><INPUT id="Plan" onclick="PlanChange()"  type=radio value="car" name="Plan" <%if tcWebApi_get("QoS_Common","Discipline","h") = "CAR" then asp_Write("checked") elseif tcWebApi_get("QoS_Common","Discipline","h") = "N/A" then asp_Write("checked") end if%>></TD>
                                  <TD width=553>CAR </TD>
                                </TR></TBODY></TABLE>
                  <TABLE width="100%" border=0>
                    <TBODY>
                    <TR id=EnableForce>
                      <TD width=108 height=30><B><%tcWebApi_get("String_Entry","ForcedBandwidthText","s")%></B></TD>
                      <TD colSpan=3><INPUT id=EnableForceWeight type=checkbox value=0 name="EnableForceWeight" <%if tcWebApi_get("QoS_Common","EnableForceWeight","h") = "Yes" then asp_Write("checked") end if%>>
                                    <input type="hidden" id="EnableForceFlag" name="EnableForceFlag" value="No"></TD></TR>
                    <TR>
                      <TD width=130 height=30><B><%tcWebApi_get("String_Entry","EnableText","s")%>DSCP/TC<%tcWebApi_get("String_Entry","FlagText","s")%></B></TD>
                      <TD width=90><INPUT id=EnableDSCPMark type=checkbox 
                         value=0  name="EnableDSCPMark" <%if tcWebApi_get("QoS_Common","EnableDSCPMark","h") = "Yes" then asp_Write("checked") end if%>>
                                    <input type="hidden" id="EnDscpFlag" name="EnDscpFlag" value="No"></TD>
                                  <TD width=132><B><%tcWebApi_get("String_Entry","EnableText","s")%>802.1P<%tcWebApi_get("String_Entry","FlagText","s")%></B></TD>
                      <TD width=130><SELECT id=Enable8021P size=1 name="Enable8021P"> 
                          <OPTION value="0" <%if tcWebApi_get("QoS_Common","En8021PRemark","h") = "0" then asp_Write("selected") end if%>>0<%tcWebApi_get("String_Entry","FlagText","s")%></OPTION> 
						  <OPTION value="1" <%if tcWebApi_get("QoS_Common","En8021PRemark","h") = "1" then asp_Write("selected") end if%>><%tcWebApi_get("String_Entry","TransparentTransmissionText","s")%></OPTION> 
						  <OPTION value="2" <%if tcWebApi_get("QoS_Common","En8021PRemark","h") = "2" then asp_Write("selected") end if%>><%tcWebApi_get("String_Entry","RepeatedMarkText","s")%></OPTION>
						  </SELECT></TD></TR></TBODY></TABLE></DIV>
                  <DIV id=COMVlanBtn>
                  <TABLE width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=92><INPUT id=COMVlanCls onclick="VlanSubmit('/cgi-bin/qos-comvlan.asp')" type=button value="<%tcWebApi_get("String_Entry","SetVLANText","s")%>" name=COMVlanCls></TD></TR></TBODY></TABLE></DIV>
                  <DIV id=QueueEdit>
                  <DIV id=PQEdit>
                  <TABLE width=360 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title align=middle width=100><%tcWebApi_get("String_Entry","QueueText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","PriorityText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","EnableText","s")%></TD></TR>
                    <TR>
                      <TD align=middle>Q1</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","HighestText","s")%></TD>
                      <TD align=middle><INPUT id=Q1Enable name=Q1Enable type=checkbox value="0" <%if tcWebApi_get("QoS_Common","QueueSW1","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q2</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","HighText","s")%></TD>
                      <TD align=middle><INPUT id=Q2Enable name=Q2Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW2","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q3</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","MiddleText","s")%></TD>
                      <TD align=middle><INPUT id=Q3Enable name=Q3Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW3","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q4</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","LowText","s")%></TD>
                      <TD align=middle><INPUT id=Q4Enable name=Q4Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW4","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
					<!-- PQ  Enable 15-18-->
                    <TR>
                      <TD align=middle>Q5</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","LowText","s")%></TD>
                      <TD align=middle><INPUT id=Q15Enable name=Q15Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW5","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q6</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","LowText","s")%></TD>
                      <TD align=middle><INPUT id=Q16Enable name=Q16Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW6","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q7</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","LowText","s")%></TD>
                      <TD align=middle><INPUT id=Q17Enable name=Q17Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW7","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q8</TD>
                      <TD align=middle><%tcWebApi_get("String_Entry","LowText","s")%></TD>
                      <TD align=middle><INPUT id=Q18Enable name=Q18Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW8","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
<% end if %>
					  </TBODY></TABLE></DIV>
                  <DIV id=WRREdit>
                  <TABLE width=420 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title align=middle width=100><%tcWebApi_get("String_Entry","QueueText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","WeightText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","EnableText","s")%></TD></TR>
                    <TR>
                      <TD align=middle>Q1</TD>
                      <TD align=middle><INPUT id=Q1Weight name="Q1Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW1","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW1","s") end if%>"></TD>
                      <TD align=middle><INPUT id=Q5Enable name=Q5Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW1","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q2</TD>
                      <TD align=middle><INPUT id=Q2Weight name="Q2Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW2","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW2","s") end if%>"> 
                      <TD align=middle><INPUT id=Q6Enable name=Q6Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW2","h") = "Yes" then asp_Write("checked") end if %>></TD></TR>
                    <TR>
                      <TD align=middle>Q3</TD>
                      <TD align=middle><INPUT id=Q3Weight  name="Q3Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW3","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW3","s") end if%>"> 
                      <TD align=middle><INPUT id=Q7Enable  name=Q7Enable type=checkbox value=0  <%if tcWebApi_get("QoS_Common","QueueSW3","h") = "Yes" then asp_Write("checked") end if %>></TD></TR>
                    <TR>
                      <TD align=middle>Q4</TD>
                      <TD align=middle><INPUT id=Q4Weight name="Q4Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW4","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW4","s") end if%>"> 
                      <TD align=middle><INPUT id=Q8Enable name=Q8Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW4","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
					<!--19-22-->
					<TR>
                      <TD align=middle>Q5</TD>
                      <TD align=middle><INPUT id=Q5Weight name="Q5Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW5","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW5","s") end if%>"></TD>
                      <TD align=middle><INPUT id=Q19Enable name=Q19Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW5","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q6</TD>
                      <TD align=middle><INPUT id=Q6Weight name="Q6Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW6","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW6","s") end if%>"> 
                      <TD align=middle><INPUT id=Q20Enable name=Q20Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW6","h") = "Yes" then asp_Write("checked") end if %>></TD></TR>
                    <TR>
                      <TD align=middle>Q7</TD>
                      <TD align=middle><INPUT id=Q7Weight  name="Q7Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW7","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW7","s") end if%>"> 
                      <TD align=middle><INPUT id=Q21Enable  name=Q21Enable type=checkbox value=0  <%if tcWebApi_get("QoS_Common","QueueSW7","h") = "Yes" then asp_Write("checked") end if %>></TD></TR>
                    <TR>
                      <TD align=middle>Q8</TD>
                      <TD align=middle><INPUT id=Q8Weight name="Q8Weight" size=3 value="<%if tcWebApi_get("QoS_Common","QueueBW8","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW8","s") end if%>"> 
                      <TD align=middle><INPUT id=Q22Enable name=Q22Enable type=checkbox value=0 <%if tcWebApi_get("QoS_Common","QueueSW8","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
<% end if %>
                      </TBODY></TABLE></DIV>
                  <% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported","h") = "Yes" then if tcWebApi_get("DataSpeedLimit_Entry","SpeedLimitModeUP","h") <> "0" then %><div style="display:none"> <% end if end if %>
                  <DIV id=CAREdit>
                  <TABLE width=420 border=1>
                    <TBODY>
                    <TR>
                      <TD class=table_title align=middle width=100><%tcWebApi_get("String_Entry","QueueText","s")%></TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","BandwidthText","s")%>(kbps)</TD>
                      <TD class=table_title align=middle><%tcWebApi_get("String_Entry","EnableText","s")%></TD></TR>
                    <TR>
                      <TD align=middle>Q1</TD>
                      <TD align=middle><INPUT id=Q1Car size=3 name="Q1Car" value="<%if tcWebApi_get("QoS_Common","QueueBW1","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW1","s") end if%>"></TD>
                      <TD align=middle><INPUT id=Q9Enable type=checkbox name="Q9Enable" <%if tcWebApi_get("QoS_Common","QueueSW1","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q2</TD>
                      <TD align=middle><INPUT id=Q2Car size=3 name="Q2Car" value="<%if tcWebApi_get("QoS_Common","QueueBW2","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW2","s") end if%>"> 
                      <TD align=middle><INPUT id=Q10Enable type=checkbox name="Q10Enable" <%if tcWebApi_get("QoS_Common","QueueSW2","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q3</TD>
                      <TD align=middle><INPUT id=Q3Car size=3 name="Q3Car" value="<%if tcWebApi_get("QoS_Common","QueueBW3","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW3","s") end if%>"> 
                      <TD align=middle><INPUT id=Q11Enable type=checkbox name="Q11Enable" <%if tcWebApi_get("QoS_Common","QueueSW3","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q4</TD>
                      <TD align=middle><INPUT id=Q4Car size=3 name="Q4Car" value="<%if tcWebApi_get("QoS_Common","QueueBW4","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW4","s") end if%>"> 
                      <TD align=middle><INPUT id=Q12Enable type=checkbox name="Q12Enable" <%if tcWebApi_get("QoS_Common","QueueSW4","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q5</TD>
                      <TD align=middle><INPUT id=Q5Car size=3 name="Q5Car" value="<%if tcWebApi_get("QoS_Common","QueueBW5","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW5","s") end if%>"> 
                      <TD align=middle><INPUT id=Q13Enable type=checkbox name="Q13Enable" <%if tcWebApi_get("QoS_Common","QueueSW5","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q6</TD>
                      <TD align=middle><INPUT id=Q6Car size=3 name="Q6Car" value="<%if tcWebApi_get("QoS_Common","QueueBW6","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW6","s") end if%>"> 
                      <TD align=middle><INPUT id=Q14Enable type=checkbox name="Q14Enable" <%if tcWebApi_get("QoS_Common","QueueSW6","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
					<!--23-24 -->
					<TR>
                      <TD align=middle>Q7</TD>
                      <TD align=middle><INPUT id=Q7Car size=3 name="Q7Car" value="<%if tcWebApi_get("QoS_Common","QueueBW7","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW7","s") end if%>"> 
                      <TD align=middle><INPUT id=Q23Enable type=checkbox name="Q23Enable" <%if tcWebApi_get("QoS_Common","QueueSW7","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
                    <TR>
                      <TD align=middle>Q8</TD>
                      <TD align=middle><INPUT id=Q8Car size=3 name="Q8Car" value="<%if tcWebApi_get("QoS_Common","QueueBW8","h") <> "N/A" then tcWebApi_get("QoS_Common","QueueBW8","s") end if%>"> 
                      <TD align=middle><INPUT id=Q24Enable type=checkbox name="Q24Enable" <%if tcWebApi_get("QoS_Common","QueueSW8","h") = "Yes" then asp_Write("checked") end if%>></TD></TR>
<% end if %>
                      </TBODY></TABLE></DIV><% if tcWebApi_get("WebCustom_Entry", "isCTPONCYJXSupported","h") = "Yes" then if tcWebApi_get("DataSpeedLimit_Entry","SpeedLimitModeUP","h") <> "0" then %></div><% end if end if %></DIV>
                          <P> 
                            <input type="hidden" id="Q1EnableFlag" name="Q1EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW1","s") %>">
                            <input type="hidden" id="Q2EnableFlag" name="Q2EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW2","s") %>">
                            <input type="hidden" id="Q3EnableFlag" name="Q3EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW3","s") %>">
                            <input type="hidden" id="Q4EnableFlag" name="Q4EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW4","s") %>">
                            <input type="hidden" id="Q5EnableFlag" name="Q5EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW5","s") %>">
                            <input type="hidden" id="Q6EnableFlag" name="Q6EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW6","s") %>">
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
	                        <input type="hidden" id="Q7EnableFlag" name="Q7EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW7","s") %>">
                            <input type="hidden" id="Q8EnableFlag" name="Q8EnableFlag" value="<% tcWebApi_get("QoS_Common","QueueSW8","s") %>">
<% end if %>					
                            <input type="hidden" id="Q1PValue" name="Q1PValue" value="1">
                            <input type="hidden" id="Q2PValue" name="Q2PValue" value="2">
                            <input type="hidden" id="Q3PValue" name="Q3PValue" value="3">
                            <input type="hidden" id="Q4PValue" name="Q4PValue" value="4">
                            <input type="hidden" id="Q5PValue" name="Q5PValue" value="5">
                            <input type="hidden" id="Q6PValue" name="Q6PValue" value="6">
<% if tcwebApi_get("WebCustom_Entry","isQoSEightQueueSupported","h") = "Yes" then %>
                            <input type="hidden" id="Q7PValue" name="Q7PValue" value="7">
                            <input type="hidden" id="Q8PValue" name="Q8PValue" value="8">
<% end if %>					
                            <input type="hidden" id="Entry_Const_0" name="Entry_Const_0" value="0">
                            <input type="hidden" id="Entry_Const_1" name="Entry_Const_1" value="1">
                            <input type="hidden" id="Entry_Const_2" name="Entry_Const_2" value="2">
                            <input type="hidden" id="Entry_Const_3" name="Entry_Const_3" value="3">
                            <input type="hidden" id="Entry_Const_4" name="Entry_Const_4" value="4">
                            <input type="hidden" id="Entry_Const_5" name="Entry_Const_5" value="5">
                            <input type="hidden" id="Entry_Const_6" name="Entry_Const_6" value="6">
                            <input type="hidden" id="Entry_Const_7" name="Entry_Const_7" value="7">
                            <input type="hidden" id="Entry_Const_8" name="Entry_Const_8" value="8">
                            <input type="hidden" id="Entry_Const_9" name="Entry_Const_9" value="9">

                            <input type="hidden" id="Entry_Const_10" name="Entry_Const_10" value="10">
                            <input type="hidden" id="Entry_Const_11" name="Entry_Const_11" value="11">
                            <input type="hidden" id="Entry_Const_12" name="Entry_Const_12" value="12">
                            <input type="hidden" id="Entry_Const_13" name="Entry_Const_13" value="13">
                            <input type="hidden" id="ReCommitFlg" name="ReCommitFlg" value="-1">
						 	<input type="hidden" id="TypeRuleFlag" name="TypeRuleFlag" value="typeRule">
	  						<input type="hidden" id="AppRuleFlag" name="AppRuleFlag" value="appRule">
	  						<input type="hidden" id="oldDSCP" name="oldDSCP" value="<% tcWebApi_get("QoS_Common","EnableDSCPMark","s") %>">
	  						<input type="hidden" id="old8021P" name="old8021P" value="<% tcWebApi_get("QoS_Common","En8021PRemark","s") %>">
	  						<input type="hidden" id="oldActive" name="oldActive" value="<% tcWebApi_get("QoS_Common","Active","s") %>">
	  						<input type="hidden" id="NAValue" name="NAValue" value="N/A">
							
                          </P>
                  <DIV id=AddBtn>
                  <TABLE width="100%" border=0>
                    <TBODY>
                    <TR>
                      <TD width=92><INPUT id=AddCls onclick="VlanSubmit('/cgi-bin/qos-clsedit.asp')" type=button value="<%tcWebApi_get("String_Entry","EditPageText","s")%>" name=AddCls></TD></TR></TBODY></TABLE></DIV>
                <TD width=10>&nbsp;</TD></TR>
              <TR>
                <TD width=10>&nbsp;</TD>
                <TD>
                  <TABLE cellSpacing=0 cellPadding=3 width=462 border=1>
                    <TBODY></TBODY></TABLE>
                  <SCRIPT language=JavaScript type=text/javascript>
CurQoSShow();
</SCRIPT>
                          <input type="hidden" id="SaveFlag" name="SaveFlag" value="0"> 
                        <TD width=10>&nbsp;</TD></TR></TBODY></TABLE></FORM></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7 height=10></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR>
        <TR>
          <TD vAlign=top width=157 bgColor=#e7e7e7></TD>
          <TD width=7 background=/img/panel3.gif>　</TD>
          <TD></TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD height=1>
      <TABLE id=table7 height=35 cellSpacing=0 cellPadding=0 width=808 
        border=0><TBODY>
        <TR>
          <TD width=162 bgColor=#ef8218>　</TD>
          <TD width=278 bgColor=#427594>　</TD>

          <TD width=196 bgColor=#427594>

            <P align=center><IMG id=btnOK 

            onclick="btnSave()" height=23 

            src="<%tcWebApi_get("String_Entry","IMGOKText","s")%>" width=80 border=0>&nbsp;&nbsp;<IMG id=btnCancel 

            height=23 src="<%tcWebApi_get("String_Entry","IMGCancelText","s")%>" onclick="RefreshPage()" width=80 border=0></P></TD>

          <TD width=170 bgColor=#313031></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>

</BODY></HTML>
