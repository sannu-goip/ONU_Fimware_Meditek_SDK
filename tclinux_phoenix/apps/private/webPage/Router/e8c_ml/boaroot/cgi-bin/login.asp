<%
if tcWebApi_get("WebCustom_Entry", "isMultiLanguageSupport", "h") = "Yes" then
	if Request_Form("Language_Flag")="1" then
		tcWebApi_constSet("LanguageSwitch_Entry","Type","1") 
		tcWebApi_constSet("LanguageSwitch_Entry","Language","Chinese") 
	elseif Request_Form("Language_Flag")="2" then
		tcWebApi_constSet("LanguageSwitch_Entry","Type","2") 
		tcWebApi_constSet("LanguageSwitch_Entry","Language","English") 
	end if

	TCWebApi_commit("LanguageSwitch_Entry")
end if
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3c.org/TR/1999/REC-html401-19991224/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>Login</title>
		<meta http-equiv=Content-Type content="text/html; charset=gb2312">
		<meta content="Microsoft FrontPage 5.0" name="GENERATOR">
		<script language='javascript' src='/JS/jquery.js'></script>
		<script language='javascript' src='/JS/util.js'></script>
		<style type=text/css>
			#div_login
			{
				FONT-SIZE: 12px; MARGIN: 100px 50px; COLOR: #333333; FONT-FAMILY: "    "
			}
			table {
				FONT-SIZE: 15px; FONT-FAMILY: "    "
			}
		</style>
		<script language="javascript" type=text/javascript>
		if ( self != top )
			top.location.href = '/cgi-bin/login.asp';
		function getsec(str)
		{
			var str1=str.substring(1,str.length)*1;    
			var str2=str.substring(0,1);  
			if (str2=="s")
			{  
				return str1*1000; 
			}
			else if (str2=="h")
			{  
				return str1*60*60*1000; 
		  }
			else if (str2=="d")
			{  
				return str1*24*60*60*1000; 
		  }  
		}
		<% if TCWebApi_get("WebCustom_Entry","isCGNXSupport","h" ) = "Yes" then %>
		function refreshself()
		{
			top.window.location.href ='/cgi-bin/login.asp';
		}
		<% end if%>	
		function GET_C(Name)
		{
			var search = Name + "=";
			if ( document.cookie.length > 0 )
			{
				offset = document.cookie.indexOf(search);
				if ( offset != -1 )
				{
					offset += search.length;
					end = document.cookie.indexOf(";", offset);
					if(end == -1) end = document.cookie.length;
					return unescape(document.cookie.substring(offset, end));
				}
				else
				return "";
			}
			else
			return "";
		}
		
		function SET_C_T(name, value, time)
		{
			var strsec = getsec(time); 
			var exp = new Date();  
			exp.setTime(exp.getTime() + strsec*1);  
			document.cookie = name + "="+ escape (value) + ";expires=" + exp.toGMTString()+";path=/;";
		} 

		function SET_C(name, value)
		{
			document.cookie = name + "="+ escape (value) + ";path=/;";
		}

		function DEL_C(name)
		{
			var exp = new Date();
			exp.setTime(exp.getTime() - 10000);
			document.cookie = name + "=del;expires=" + exp.toGMTString();
			document.cookie = name + "=del;expires=" + exp.toGMTString()+";path=/;";
		}

		function onHandleKeyDown(e)
		{
			var key = 0;
			if (window.event)
			{
				key = window.event.keyCode;
			}
			else if (e)
			{
				key = e.which;
			}
			if (key == 13 && document.activeElement.id != 'loginbutton')
			{
				doLogin();
			}
		}
		document.onkeypress = onHandleKeyDown;

		var loginTimes = 0;

		function doLogin()
		{
	<% if TCWebApi_get("WebCustom_Entry","isCGNXSupport","h" ) = "Yes" then %>
	var Captchastr = document.getElementById('ValidateCode');
	var Captchaimg = document.getElementById('captchaimg').src;
	var Captchaurl = document.getElementById('Captcha_url');	
	var captcha  ="";
	var captchaIndx ="";
	<% end if%>	
			/* check empty */
			if ( 0 == $('#username').val().length )
			{
				alert('<%tcWebApi_get("String_Entry","LoginAccountCantBeNullText","s")%>');
				return false;
			}

			if ( 0 == $('#password').val().length )
			{
				alert('<%tcWebApi_get("String_Entry","LoginPswCantbeNullText","s")%>');
				return false;
			}
	<% if TCWebApi_get("WebCustom_Entry","isCGNXSupport","h" ) = "Yes" then %>
		if (Captchastr.value == "")
		{
			alert('<%tcWebApi_get("String_Entry","LoginValidateCodeAlertText","s")%>');
			return false;
		}
	
		captcha = Captchaimg.split("_");
		captchaIndx = captcha[1].split(".");	
		Captchaurl.value = captchaIndx[0];
	<% end if%>	
			if( (GET_C("loginTimes") != "") && (typeof(GET_C("loginTimes")) != "undefined") )
				loginTimes = parseInt(GET_C("loginTimes"));
			if ( loginTimes >= 3 )
			{
				$('#errmsg').text('<%tcWebApi_get("String_Entry","LoginFailThreeTimesText","s")%>');
				return false;	
			}
			SET_C_T("loginTimes", loginTimes, "s60");
			var form = $('#loginui');
	    $.ajax({
	        url: login_check_addr,
	        type: form.attr("method"),
	        data: form.serialize(),
	        dataType: "json",
	        beforeSend: function () 
	        {
	        	$('#loginbutton').attr('disabled', true);
	        },
	        error: function ()
	        {
	        	$('#loginbutton').attr('disabled', false);
	 					alert('fetal error!');
	        },
	        complete:function ()
	        {
	        	$('#loginbutton').attr('disabled', false);
	        },
	        success: function (result)
	        {
	        		if ( '1' == result.Locked )
	        		{
	        				$('#errmsg').text('<%tcWebApi_get("String_Entry","LoginFailThreeTimesText","s")%>');
	        				loginTimes = 0;
	        				DEL_C('loginTimes');
	        		}
	        		else if ( '1' == result.Logged )
	        		{
	        				$('#errmsg').text('<%tcWebApi_get("String_Entry","AdminAccountIsLoggedText","s")%>');
	        				loginTimes = 0;
	        				DEL_C('loginTimes');		
	        		}
	        		else if ( '2' == result.Logged )
	        		{
	        				$('#errmsg').text('<%tcWebApi_get("String_Entry","UserAccountIsLoggedText","s")%>');
	        				loginTimes = 0;
	        				DEL_C('loginTimes');
	        		}
					<% if TCWebApi_get("WebCustom_Entry","isCGNXSupport","h" ) = "Yes" then %>
					else if( '2' == result.CaptchaOK )
					{
							$('#errmsg').text('<%tcWebApi_get("String_Entry","LoginValidateCodeErrorText","s")%>');
					}
					<% end if%>	
	        		else if ( '0' == result.Privilege )
	        		{
	        				loginTimes += 1;
									SET_C_T("loginTimes", loginTimes, "s60");
	        				$('#errmsg').text('<%tcWebApi_get("String_Entry","LoginYouHaveText","s")%>' +  loginTimes + '<%tcWebApi_get("String_Entry","LoginUserNamePswErrorText","s")%>');      			
	        		}
	        		else if ( '1' == result.Privilege || '2' == result.Privilege )
	        		{
	        				loginTimes = 0;
	        				DEL_C('loginTimes');
	        				SET_C('ecntToken', result.ecntToken);
	        				top.location.href = '/cgi-bin/content.asp';
	        		}
	        }
	    });
		}

		function LoadFrame()
		{
			$('#username').val('useradmin');
			$('#username').focus();
		}

<%if TCWebApi_get("WebCustom_Entry","isMultiLanguageSupport","h" ) = "Yes" then%>		
		function LanguageSelect()
		{			
			var vForm = document.loginui;
			switch (vForm.selectLanguage.value)
			{
				case 'Chinese':
				vForm.Language_Flag.value = "1";	
				break;

				case 'English':
				vForm.Language_Flag.value = "2";
				break;
				
				default:
				break;
			}

			document.loginui.submit();
		}
<%end if%> 
	</script>
	<body onload="LoadFrame()">
		<div id=div_login >
			<form id="loginui" name="loginui" method="post">
			<% if TCWebApi_get("WebCustom_Entry","isCGNXSupport","h" ) = "Yes" then %>		
<TABLE style="POSITION: relative" borderColor="#e7e7e7" height="45%" cellSpacing="0" cellPadding="0" align="center" bgColor="#e7e7e7" border="0" width ="390">
<%else%>
				<table style="POSITION: relative" borderColor="#e7e7e7" height="45%" cellSpacing="0" cellPadding="0" align="center" bgColor="#e7e7e7" border="0">
<%end if%> 
					<tbody>
						<tr>
							<td align="middle" bgColor="#ffffff" colSpan="3" height="10">
								<label><img height="117" src="/img/logo.jpg" width="337"></label>
							</td>
						</tr>
							<tr>
								<td align=right width="30%" height=50>
									<%tcWebApi_get("String_Entry","UserPswText","s")%>
								</td>
								<td width=30>  </td>
								<td>
										<label>
											<input id="username" style="WIDTH: 140px; FONT-FAMILY: Arial" name="username" size="20" autocomplete="off"> 
										</label>
								</td>
							</tr>

							<tr>
								<td align=right height=30>
									<%tcWebApi_get("String_Entry","PasswordText","s")%>
								</td>
								<td>  </td>
								<td>
									<input id="password" style="WIDTH: 140px; FONT-FAMILY: Arial" type="password" name="password" size="20" autocomplete="off">
								</td>
							</tr>
<%if TCWebApi_get("WebCustom_Entry","isMultiLanguageSupport","h" ) = "Yes" then%>
							<tr>
								<td align=right height=30><%tcWebApi_get("String_Entry","LanguageSelectText","s")%></td>
									<td>  </td>
									<td>
										<input id="Language_Flag" type="hidden" name="Language_Flag" value="0">
										<select id=selectLanguage onChange=LanguageSelect() name="selectLanguage">
										<option value="English" <%if tcWebApi_get("LanguageSwitch_Entry","Language","h") = "English" then asp_Write("selected") end if%>>English
											<option value="Chinese" <%if tcWebApi_get("LanguageSwitch_Entry","Language","h") = "Chinese" then asp_Write("selected") end if%>>Chinese  
											
							            </select>
									</td>
							</tr>
<%end if%> 
<%if TCWebApi_get("WebCustom_Entry","isCGNXSupport","h" ) = "Yes" then%>
							<tr>
								<td align=right height=30><%tcWebApi_get("String_Entry","LoginValidateCodeText","s")%></td>
									<td>  </td>
								<TD>
								<INPUT id="ValidateCode" style="WIDTH: 140px; FONT-FAMILY: Arial" type="validateCode" name="validateCode" size="20">
								</TD>
								<td>
									<input id="Captcha_url" type="hidden" name="captcha_url" value="">
								</td>
							</tr>
							<TR>
							<TD align=right height=30>
							</TD>
								<TD>  </TD>	
								<TD><div id="Valitag">&nbsp &nbsp;
								<INPUT id="Refresh" onclick="refreshself()" type="button" value="Refresh" name="Refresh" style ="position:absolute;top:75%;margin-top:-button">
							 	 </div> </TD>	
							<SCRIPT language=javascript>
								var Valivalue ="";
								var html = "";
								var url = "/captcha/";
								var ValitagValue = document.getElementById('Valitag');
								
								Valivalue ="<% tcWebApi_get("WebCurSet_Entry", "Captchaurl", "s")%>";
								html = "<img id = captchaimg  name=captchaimg src=" + url + Valivalue+ ".gif height=65px width=144px>";
								ValitagValue.insertAdjacentHTML('afterbegin', html);
							</SCRIPT>
						</TR> 
<%end if%> 
							<tr>
								<td style="COLOR: #ff0000" align="center" colspan="3" id='errmsg' name='errmsg'>
								</td>
							</tr>
							<tr>
									<td align=center colSpan=3>
										<input id="loginbutton" onclick="doLogin()" type="button" value="<%tcWebApi_get("String_Entry","UserLoginText","s")%>" name="loginbutton">
										&nbsp;
									</td>
							</tr>
					</tbody>
			</table>
			</form>
		</div>
</body>
</html>
