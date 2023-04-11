<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/sign.php';
	if(!(isset($_POST['mySign']) && isset($_POST['checkSum'])))
	{
		echo "Ошибка, отправьте подписанный договор.";
		exit;
	}
	$login = getUserLogin();
	if(!$login)
	{
		header('Location: /login/');
		exit;
	}
	session_start();
	if(!isset($_SESSION['purchaseId']))
	{
		echo "Ошибка сессии";
		exit;
	}
	if(!verifyData($_POST['mySign']))
	{
		echo "Подпись недействительна";
		exit;
	}
	//get user id from his email value
	$query = "INSERT INTO download_complete (id, checksum, verification)
	VALUES(?,?,?)";
	$args = array();
	array_push($args, $_SESSION['purchaseId'], $_POST['checkSum'], $_POST['mySign']);
	$mySqlQuery = new MySQLQueries();
	if(!$mySqlQuery->openConnection())
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	if(!$mySqlQuery->sqlQuery($query, $args))
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	//end session
	unset($_SESSION['purchaseId']);
	//redirect to homepage
	echo 'Location: /private/';
?>