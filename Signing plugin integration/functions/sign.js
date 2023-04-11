document.getElementById("agreement_signed").addEventListener('click', show_hidden);
hiddenElements = document.getElementsByClassName("hidden_elem");
for(var i = 0; i < hiddenElements.length; i++)
{
	hiddenElements[i].style.display = "none";
}
//var dataToSign = "";
var detachedSign = false;
var storedCertificates = [];

function show_hidden(element)
{
	if(element.currentTarget.checked)
	{
		for(var i = 0; i < hiddenElements.length; i++)
		{
			hiddenElements[i].style.display = "initial";
		}
		if(storedCertificates.length == 0)
		{
			get_certificates();
		}
	}
	else
	{
		for(var i = 0; i < hiddenElements.length; i++)
		{
			hiddenElements[i].style.display = "none";
		}
	}
}


function get_certificates()
{
	cadesplugin.async_spawn(function *(){
		var storeExists = true;
		try
		{
			var storeObj = yield cadesplugin.CreateObjectAsync("CAdESCOM.Store");
			if(!storeObj){
				alert("Хранилище сертификатов не доступно");
				return;
			}
			yield storeObj.Open();
		}
			
		catch(ex){
			storeExists = false;
			alert("Ошибка создания подписи "+cadesplugin.getLastError(ex));
		}
		if(storeExists)
		{
			try
			{
				certs = yield storeObj.Certificates;
				certsCount = yield certs.Count;
			}
			catch(ex)
			{
				alert(cadesplugin.getLastError(ex));
				return;
			}
			var certInsert = document.getElementById("Certificates");
			for(var i = 1; i <= certsCount; i++)
			{
				var certificate;
				try{
					certificate = yield certs.Item(i);
				}
				catch(ex){
					alert(cadesplugin.getLastError(ex));
				}
				var newCertOption = document.createElement("OPTION");
				var validFrom;
				var certName;
				try{
					validFrom = new Date((yield certificate.ValidFromDate));
					certName = yield certificate.SubjectName;
				}
				catch(ex){
					alert(cadesplugin.getLastError(ex));
				}
				newCertOption.text = validFrom + certName;
				certInsert.options.add(newCertOption);
				storedCertificates.push(certificate);
				
			}
			yield storeObj.Close();
		}
	});
}

function create_signature(dataToSign)
{
	if(dataToSign == undefined || dataToSign == '')
	{
		alert("Данные для подписи отсутсвуют");
		return;
	}
	return cadesplugin.async_spawn(function*() {
		var certBox = document.getElementById("Certificates");
		var selectedCert = certBox.selectedIndex;
		if(selectedCert == -1)
		{
			alert("Укажите сертификат");
			return;
		}
		var signCert = storedCertificates[selectedCert];
		try 
		{
			var signerObj = yield cadesplugin.CreateObjectAsync("CAdESCOM.CPSigner");
		} 
		catch (err) 
		{
			alert(cadesplugin.getLastError(err));
			return;
		}
		if (signerObj) 
		{
			yield signerObj.propset_Certificate(signCert);
		}
		else {
			alert(cadesplugin.getLastError(err));
			return;
		}
		var signedDataObj = yield cadesplugin.CreateObjectAsync("CAdESCOM.CadesSignedData");
		var tspService = "http://testca.cryptopro.ru/tsp/tsp.srf";
		var signedData = "";
		if (dataToSign) 
		{
			try{
				yield signedDataObj.propset_ContentEncoding(cadesplugin.CADESCOM_STRING_TO_UCS2LE); //CADESCOM_BASE64_TO_BINARY
				yield signedDataObj.propset_Content(dataToSign);
				yield signerObj.propset_Options(1); //CAPICOM_CERTIFICATE_INCLUDE_WHOLE_CHAIN
				yield signerObj.propset_TSAAddress(tspService);
			}
			catch(err)
			{
				alert(cadesplugin.getLastError(err));
				return;
			}
			try 
			{
			    signedData = yield signedDataObj.SignCades(signerObj, cadesplugin.CADESCOM_CADES_T, detachedSign);
			}
			catch (err) {
				alert("Не удалось создать подпись из-за ошибки: " + cadesplugin.getLastError(err));
				return;
			}
			return signedData;
		}
	});
}


function verify_signature(signedData, dataToSign)
{
	if(signedData== ''){
		alert("Данные для проверки отсутствуют");
		return false;
	}
	return cadesplugin.async_spawn(function*() {
		var signedDataObj = yield cadesplugin.CreateObjectAsync("CAdESCOM.CadesSignedData");
		if(detachedSign)
		{
			try{
				yield signedDataObj.propset_ContentEncoding(cadesplugin.CADESCOM_BASE64_TO_BINARY);
				yield signedDataObj.propset_Content(dataToSign);
				yield signedDataObj.VerifyCades(signedData, cadesplugin.CADESCOM_CADES_T, true);
			}catch(err)
			{
				alert("Не получилось подтвердить подпись. Ошибка: " + cadesplugin.getLastError(err));
				return false;
			}
		}
		else
		{
			try{
				yield signedDataObj.VerifyCades(signedData, cadesplugin.CADESCOM_CADES_T);
			}catch(err)
			{
				alert("Не получилось подтвердить подпись. Ошибка: " + cadesplugin.getLastError(err));
				return false;
			}
		}
		return true
	});
}
