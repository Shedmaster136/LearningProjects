<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	if(getUserLogin())
	{
		header('Location: /private/');
	}
	if(!empty($_POST))
	{
		$login = $_POST['login'] ?? '';
		$password = $_POST['password'] ?? '';
		if(checkAuth($login, $password))
		{
			header('Location: /private/');
		}
		else
		{
			$error = 'Неверный логин или пароль';
		}
	}
?>
<!DOCTYPE html>
<html>
    <head>
		<meta charset="utf-8">
		<link href="/stylesheets/registration.css" rel="stylesheet">
        <title>Страница аутентификации</title>
    </head>
    <body>
		<div id = "main_content">
			<h1>Сервис распространения СКЗИ</h1>
			<div class="data_inputs">
				<?php if(isset($error)): ?>
				<span style="color:red;">
					<?= $error ?>
				</span>
				<?php endif; ?>
				<form method="post" action="/login/index.php">
					<table id="table_inputs">
						<tr>
							<td><label for="login_input">Логин:</label></td>
							<td><input type="text" class="data_input" placeholder="Email" id="login_input" name="login"></td>
						</tr>
						<tr>
							<td><label for="password_input">Пароль:</label></td>
							<td><input type="password" class="data_input" placeholder="Пароль" id="password_input" name="password"></td>
						</tr>
						<tr>
							<td colspan="2" style="text-align:center"><input type="submit" value="Вход"></td>
						</tr>
					</table>
				</form>
			</div>
			<div>
				<p><a href="/registration/">Регистрация</a></p>
			</div>
		</div>
    </body>
</html>
