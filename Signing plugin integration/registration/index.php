<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	if(getUserLogin())
	{
		header('Location: /private/');
	}
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
					<li>Ознакомьтесь с договором об оказании услуг</li>
					<li>Если с огласны с условиями договора, дайте согласие</li>
					<li>Выберите </li>
					<li>Нажмите "Отправить запрос"</li>
				</ul>
			</div>
			<div>Реквизиты организации</div>
			<div class="data_inputs">
				<table id="table_inputs">
					<tr>
						<td><label for="inn_input">ИНН:</label></td>
						<td><input type="text" class="data_input" placeholder="ИНН" id="inn_input"></td>
					</tr>
					<tr>
						<td><label for="ogrn_input">ОГРН:</label></td>
						<td><input type="text" class="data_input" placeholder="ОГРН" id="ogrn_input"></td>
					</tr>
					<tr>
						<td><label for="name_input">Официальное наименование:</label></td>
						<td><input type="text" class="data_input" placeholder="Наименование" id="name_input"></td>
					</tr>
					<tr>
						<td><label for="index_input">Индекс:</label></td>
						<td><input type="text" class="data_input" placeholder="Индекс" id="index_input"></td>
					</tr>
					<tr>
						<td><label for="city_input">Город:</label></td>
						<td><input type="text" class="data_input" placeholder="Город" id="city_input"></td>
					</tr>
					<tr>
						<td><label for="phone_input">Телефон:</label></td>
						<td><input type="text" class="data_input" placeholder="Телефон" id="phone_input"></td>
					</tr>
					<tr>
						<td><label for="mail_input">Почта (Логин):</label></td>
						<td><input type="text" class="data_input" placeholder="Email" id="mail_input"></td>
					</tr>
					<tr>
						<td><label for="password_input">Пароль:</label></td>
						<td><input type="text" class="data_input" placeholder="Пароль" id="password_input"></td>
					</tr>
					<tr>
						<td><label for="password_2_input">Пароль (повторно):</label></td>
						<td><input type="text" class="data_input" placeholder="Пароль (повторно)" id="password_2_input"></td>
					</tr>
				</table>
			</div>
			<div id = "policy_agreement">
				<p align="center">
					<strong>Договор об оказании услуг</strong>
				</p>
				Текст договора
			</div>
			<div>
				<input type="checkbox" id="agreement_signed">
				В соответствии с ФЗ "О персональных данных" Я даю свое согласие на обработку персональных данных
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
