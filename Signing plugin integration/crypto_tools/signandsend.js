document.getElementById("get_crypto").addEventListener('click', get_sign_data);
function get_sign_data()
{
	var agreed = document.getElementById("agreement_signed").checked;
	var dataToSign = "";	
	var registrationData = dataToSign;
	dataToSign += document.getElementById('policy_agreement').textContent;
	if(agreed)
	{
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
				xhr.open("POST", "/functions/purchase.php", true);
				xhr.setRequestHeader('Content-Type', 'multipart/form-data; boundary='+boundary );
				body += '\r\n--'+boundary+'\r\n'+'Content-Disposition: form-data; name="mySign"\r\n\r\n'+signedData+'\r\n--'+boundary+'--\r\n';
				xhr.send(body);
			});
		});
	}
	else
	{
		alert("Согласие на условие договора не было дано");
		return 0;
	}
}
