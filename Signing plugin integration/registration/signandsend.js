document.getElementById("get_crypto").addEventListener('click', get_sign_data);
function get_sign_data()
{
	var agreed = document.getElementById("agreement_signed").checked;
	var dataToSignContainer = document.getElementsByClassName("data_input");
	var dataToSign = "";
	var pass1 = document.getElementById("password_input").value;
	var pass2 = document.getElementById("password_2_input").value;
	if(pass1 != pass2)
	{
		alert("Пароли не совпадают");
		return 0;
	}
	if(!agreed)
	{
		alert("Согласие на условие договора не было дано");
		return 0;
	}
	for(var i = 0; i < dataToSignContainer.length-1; i++)
	{
		if(dataToSignContainer[i].value != '')
		{
			dataToSign += (dataToSignContainer[i].value) + '|';
		}
		else
		{
			alert("Заполните все поля");
			return 0;
		}
	}		
	var registrationData = dataToSign;
	dataToSign += document.getElementById('policy_agreement').textContent;
	//подписать данные
	create_signature(dataToSign).then(function(signedData)
	{
		if(signedData == undefined)
		{
			return 0;
		}
		verify_signature(signedData).then(function(verified)
		{
			if(!verified)
			{
				return 0;
			}
			alert(dataToSign);
			var xhr = new XMLHttpRequest();
			xhr.onload = function()
			{
				if (this.readyState == 4 && this.status == 200)
				{
					if(this.responseText.match(/Location: /))
					{
						
						window.location.href = this.responseText.replace(/Location: / , '');
					}
					else
					{
						alert(this.responseText);
					}
				}
			}
			var boundary = "randomDelim";
			var body = "";
			xhr.open("POST", "/functions/register.php", true);
			xhr.setRequestHeader('Content-Type', 'multipart/form-data; boundary='+boundary );
			body += '\r\n--'+boundary+'\r\n'+'Content-Disposition: form-data; name="mySign"\r\n\r\n'+signedData;
			body += '\r\n--'+boundary+'\r\n'+'Content-Disposition: form-data; name="registrationData"\r\n\r\n'+registrationData+'\r\n--'+boundary+'--\r\n';
			xhr.send(body);
		});
	});
}
