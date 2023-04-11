<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	if(!isset($_GET['purchaseId']))
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
	
	$query = "SELECT EXISTS(SELECT * FROM purchase_act WHERE id = ?)";
	$args = array();
	array_push($args, $_GET['purchaseId']);
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
		echo "Заказа не существует";
		exit;
	}
	session_start();
	$_SESSION['toolId'] = $_GET['purchaseId'];
?>
<!DOCTYPE html>
<html>
    <head>
		<meta charset="utf-8">
		<link href="/stylesheets/registration.css" rel="stylesheet">
		<script language="javascript" src="/functions/cadesplugin_api.js" async></script>
		<script language="javascript" src="/functions/sign.js" defer></script>
		<script language="javascript" src="signandsend.js" defer></script>
        <title>Страница регистрации</title>
    </head>
    <body>
		<div id="main_content">
			<h1>Заполните форму регистрации</h1>
			<div>
				<ul>
					<li>Заполните все поля</li>
					<li>Нажмите "Отправить запрос"</li>
				</ul>
			</div>
			<div class="data_inputs">
				<table id="table_inputs">
					<tr>
						<td><label for="check_sum">Контрольная сумма:</label></td>
						<td><input type="text" class="data_input" placeholder="Полученный хеш" id="check_sum"></td>
					</tr>
				</table>
			</div>
			<div id = "policy_agreement">
				<p align="center">
					<strong>Акт о получении</strong>
				</p>
				Текст договора
			</div>
			<div>
				<input type="checkbox" id="agreement_signed">
				Получение подтверждаю
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
