<?php

	function verifyData(string $signedText) bool
	{
		try
		{
			$sd = new CPSignedData();
			$sd->VerifyCades($signedText, CADES_T, false);
		}
		catch(Exception $e)
		{
			return false;
		}
		return true;
	}
	function signData(string $signText)?string
	{
		try
		{
			$store = new CPStore();//LOCAL_MACHINE_STORE CURRENT_USER_STORE
			$store->Open(CURRENT_USER_STORE, "my", STORE_OPEN_MAXIMUM_ALLOWED);//, "my", STORE_OPEN_READ_ONLY);
			
			$certificates = $store->get_Certificates;
			$certificates = $certificates->Find(CERTIFICATE_FIND_SUBJECT_NAME, "SKZI_Service", 0);
			if(!is_string($certificates))
			{
				$certificates = $certificates->Item(1);
			}
			if(!$certificates)
			{
				return null;
			}
			$signer = new CPSigner();
			$tsp_address = "http://testca.cryptopro.ru/tsp/tsp.srf";
			$signer->set_TSAAddress($tsp_addres);
			$signer->set_Certificate($certificates);
			$sd = new CPSignedData();
			$sd->set_ContentEncoding(STRING_TO_UCS2LE);
			$sd->set_Content($signText);
			$sm = $sd->SignCades($signer, CADES_T, false);
			return $sm;
		}
		catch(Exception $e)
		{
			return null;
		}
	}
?>
