document.getElementById("get_crypto").addEventListener('click', get_sign_data);
function get_sign_data()
{
	
	CREATE PROCEDURE `order_create` (IN `arg` INT(11))
	
	
	var agreed = document.getElementById("agreement_signed").checked;
	var dataToSignContainer = document.getElementById("check_sum").value;
	var dataToSign = "";
	if(!agreed)
	{
		alert("Согласие на условие договора не было дано");
		return 0;
	}
	if(dataToSignContainer == '')

		alert("Заполните все поля");
		return 0;
	}		
	dataToSign = dataToSignContainer;
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
			xhr.open("POST", "/functions/confirm_download.php", true);
			xhr.setRequestHeader('Content-Type', 'multipart/form-data; boundary='+boundary );
			body += '\r\n--'+boundary+'\r\n'+'Content-Disposition: form-data; name="mySign"\r\n\r\n'+signedData;
			body += '\r\n--'+boundary+'\r\n'+'Content-Disposition: form-data; name="checkSum"\r\n\r\n'+dataToSignContainer+'\r\n--'+boundary+'--\r\n';
			xhr.send(body);
		});
	});
}
