<?php
	require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/dbconnection.php';
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/sign.php';
	if(!(isset($_POST['mySign']) && isset($_POST['registrationData'])))
	{
		echo "Ошибка в отправленных данных";
		exit;
	}
	//check sign
	if(!verifyData($_POST['mySign']))
	{
		echo "Подпись недействительна";
		exit;
	}
	$query = "INSERT INTO user (inn, ogrn, name, post_index, city, phone_num, email, password, signed_agreement)
	VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
	$registrationData = explode('|', $_POST['registrationData']);
	array_push($registrationData, $_POST['mySign']);
	$mySqlQuery = new MySQLQueries();
	if(!$mySqlQuery->openConnection())
	{
		echo "Ошибка на сайте";
		exit;
	}
	if(!$mySqlQuery->sqlQuery($query, $registrationData))
	{
		echo "Ошибка на сайте";
		exit;
	}
	setcookie('login', $registrationData[6], time()+3600, '/');
	setcookie('password', $registrationData[7], time()+3600, '/');
	echo('Location: /private/');
?>
