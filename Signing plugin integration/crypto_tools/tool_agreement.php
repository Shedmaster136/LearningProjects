<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	if(!isset($_GET['toolId']))
	{
		header('Location: /private/');
		exit;
	}
	$login = getUserLogin();
	if(!$login)
	{
		header('Location: /login/');
		exit;
	}
	
	$query = "SELECT EXISTS(SELECT * FROM enc_tool WHERE name = ?)";
	$args = array();
	array_push($args, $_GET['toolId']);
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
	if(!$toolExists = $mySqlQuery->getNextArray(MYSQLI_NUM))
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	if(!$toolExists[0])
	{
		echo "Средство не существует";
		exit;
	}
	session_start();
	$_SESSION['toolId'] = $_GET['toolId'];
	//get blank with conditions
?>
<!DOCTYPE html>
<html>
    <head>
		<meta charset="utf-8">
		<link href="/stylesheets/registration.css" rel="stylesheet">
		<title><?='Получить' . $_GET['toolId'] ?></title>
		<script language="javascript" src="/functions/cadesplugin_api.js" async></script>
		<script language="javascript" src="/functions/sign.js" defer></script>
		<script language="javascript" src="signandsend.js" defer></script>
    </head>
    <body>
		<div id="main_content">
			<h1>Заявление на получение СКЗИ</h1>
			<div>
				<ul>
					<li>Ознакомьтесь с договором о порядке получения СКЗИ <?=$_GET['toolId'] ?></li>
					<li>Если с огласны с условиями договора, дайте согласие</li>
					<li>Выберите сертификат подписи</li>
					<li>Нажмите "Отправить запрос"</li>
				</ul>
			</div>
			<div id = "policy_agreement">
				<p align="center">
					<strong>Договор о порядке предоставления СКЗИ</strong>
				</p>
				Текст договора
			</div>
			<div>
				<input type="checkbox" id="agreement_signed">
				Я согласен с условиями договора
			</div>
			<div>
				<select size="4" name="Certificates" id="Certificates" class="hidden_elem" style="width:100%; resize:none"></select>
			</div>
			<div>
				<button id="get_crypto" class="hidden_elem">Отправить запрос</button>
			</div>
		</div>
    </body>
</html>
