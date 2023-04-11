<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/sign.php';
	if(!isset($_POST['mySign']))
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
	if(!isset($_SESSION['toolId']))
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
	$query = "SELECT id FROM user WHERE email = ? ";
	$args = array();
	array_push($args, $login);
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
	if(!$resArr = $mySqlQuery->getNextArray(MYSQLI_NUM))
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	if(!$userId = $resArr[0])
	{
		header('Location: /private/');
		exit;
	}
	// get toolId from its name
	$query = "SELECT id from enc_tool WHERE name = ? ";
	if(!$mySqlQuery->sqlQuery($query, array($_SESSION['toolId'])))
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	if(!$myToolId = $mySqlQuery->getNextArray(MYSQLI_NUM)[0]);
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	//set new value
	$query ="CALL purchase_tool(?, ?, ?);";
	$args = array();
	array_push($args, $userId, $myToolId, $_POST['mySign']);
	if(!$mySqlQuery->sqlQuery($query, $args))
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	//end session
	unset($_SESSION['toolId']);
	//redirect to homepage
	echo 'Location: /private/';
?>
