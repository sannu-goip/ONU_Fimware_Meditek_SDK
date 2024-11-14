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
		<link href="css/all.min.css" rel="stylesheet" type="text/css">
		<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">	
		<link href="https://fonts.googleapis.com/css?family=Nunito:200,200i,300,300i,400,400i,600,600i,700,700i,800,800i,900,900i"
        rel="stylesheet"> 
		<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
		<link href="css/main.css" rel="stylesheet">

		<script language='javascript' src='/JS/jquery.js'></script>
		<script language='javascript' src='/JS/util.js'></script>
	
		<style type=text/css>
			

.form-control 
{
    display: block;
    width: 100%;
    padding: .375rem .75rem;
    font-size: 1.2rem;
    font-weight: 400;
    line-height: 1.5;
    color: var(--bs-body-color);
    background-color: var(--bs-body-bg);
    background-clip: padding-box;
    border: var(--bs-border-width) solid var(--bs-border-color);
    -webkit-appearance: none;
    -moz-appearance: none;
    appearance: none;
    border-radius: var(--bs-border-radius);
    transition: border-color .15s ease-in-out,box-shadow .15s ease-in-out;
}

#myVideo {
  position: fixed;
  right: 0;
  bottom: 0;
  min-width: 100%;
  min-height: 100%;
}

/* Add some content at the bottom of the video/page */
.content {
  position: fixed;
  bottom: 0;
  background: rgba(0, 0, 0, 0.5);
  color: #f1f1f1;
  width: 100%;
  padding: 20px;
}

/* Style the button used to pause/play the video */
#myBtn {
  width: 200px;
  font-size: 18px;
  padding: 10px;
  border: none;
  background: #000;
  color: #fff;
  cursor: pointer;
}

#myBtn:hover {
  background: #ddd;
  color: black;
}


		#captchaimg{
			padding-top:8px;
		}

		[type=button]:not(:disabled), [type=reset]:not(:disabled), [type=submit]:not(:disabled), button:not(:disabled) {
    cursor: pointer;
    width: 109px;
	float: left;
	padding: 4px;
    font-size: 15px;
}

		.container-md{
			width: 900px;
		}
		.back-image-box{
	        width: 100%;
	        height:500px;
	        background-repeat: no-repeat;
	        background: url(/img/imagewifi.jpg);
	      	}
			  
		.login_box {
    		border-radius: 4px;
			/* box-shadow: 0px 0px 6px rgba(0,0,0,0.1); */
			text-align: center;
			margin: 0 auto;
			top: 0px;
			position: relative;
			padding: 2px 3px;
		}
		.login_btn {
    border: solid 2px #fd6102;
    background: #fd6102;
    border-radius: 5px;
    color: #fff;
    padding: 10px;
    width: 100%;
    font-weight: bold;
    margin-top: 6px;
    margin-bottom: 6px;
}
	.login_body h1 {
    		color: #584848;
    		font-size: 28px;
    		font-weight: bold;
    		margin-bottom: 30px;
		}

.login_body {
    padding: 30px 30px 10px 30px;
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
	<body onload="LoadFrame()" >
<video autoplay muted loop id="myVideo">
  <source src="../img/background_AdobeExpress.mp4" type="video/mp4">
</video>
<div class="container-fluid">
<div class="row">
<div class="col-md-12 col-sm-12 col-xs-12 logo_box" style="position: relative;margin-left:-30px">
	
	<img src="../img/logo.png" class="img-fluid" alt="">
</div>
</div>
</div>
	  
<div class="login_box container" style="margin-top:30px;">
	
	 <div class="row" id="div_login">

	 <div class="col-sm-5 col-5 text-center offset-4 " style="margin-top: 40px;">		

        <form id="loginui" name="loginui" class="row" method="post">
		
		 	 
				<div class="login_body" style="background-color:#cacaca40; border-radius:10px;margin-left:-18%; margin-top:-50px !important;margin-right:40px;width:100%;height:60%; margin-top:20px">
					<h1 style="color: #fd6102;margin-left:-20px; font-size: 40px;">Login Now</h1>
					<div class="form-group input-group pb-2">
	                      <input id="username" name="username" size="20" autocomplete="off" class="form-control" placeholder="Username: "> 
					</div>
					 	<div class="form-group input-group pb-2">
	                  		<input id="password"  type="password" name="password" size="20" autocomplete="off" class="form-control" placeholder="Password: ">
					 	</div>
	                <div class="form-group input-group">
					 <input id="ValidateCode" class="form-control" type="validateCode" name="validateCode" placeholder="ValidateCode"/>
	                    <input id="Captcha_url" type="hidden" name="captcha_url" value="">
					</div>
				<div style="COLOR: #ff0000" align="center" colspan="3" id='errmsg' name='errmsg'></div>
					 <div class="form-group">
						<input id="loginbutton" style="float: left;" onclick="doLogin()" type="button" value="Login" name="loginbutton" class=" login_btn btn-sm">
									<span style="padding-top: 12px;" id="Valitag"></span>
						
					<INPUT style="float: right;" id="Refresh" onclick="refreshself()" type="button" value="Refresh" name="Refresh" class="login_btn" >
            <br>
                 <br>
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

									  <SCRIPT language=javascript>
										var Valivalue ="";
										var html = "";
										var url = "/captcha/";
										var ValitagValue = document.getElementById('Valitag');
										
										Valivalue ="<%tcWebApi_get("WebCurSet_Entry", "Captchaurl", "s")%>";
										html = "<img id = captchaimg  name=captchaimg src=" + url + Valivalue+ ".gif height=45px width=120px padding=5px>";
										ValitagValue.insertAdjacentHTML('afterbegin', html);
									</SCRIPT>	
	                 </div>
				</div>
            </form>
         </div>
	  </div> 
   </div>

    <script type="text/javascript" src="/JS/bootstrap.min.js"></script>
    <script type="text/javascript" src="/JS/popper.min.js"></script>
	<script type="text/javascript" src='/JS/util.js'></script>
							
</body>
</html>
