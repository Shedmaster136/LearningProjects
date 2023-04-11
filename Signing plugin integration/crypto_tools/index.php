<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	$login = getUserLogin();
	if(!$login)
	{
		header('Location: /login/');
		exit;
	}
	
	$query = "SELECT id, name FROM enc_tool";
	$mySqlQuery = new MySQLQueries();
	if(!$mySqlQuery->openConnection())
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
	if(!$mySqlQuery->sqlQuery($query))
	{
		echo "Ошибка сайта. Попробуйте позже";
		exit;
	}
?>
<!DOCTYPE html>
<html>
    <head>
		<meta charset="utf-8">
		<link href="/stylesheets/registration.css" rel="stylesheet">
        <title>Личный кабинет</title>
    </head>
    <body>
		
		<a href='/functions/exit.php'>Выход</a>
		<div id="main_content">
			<h1>Список СКЗИ, доступных для приобретения</h1>
			<div class="database_list">
				<table id = "tool_list">
					<tr>
						<th>Название</th>
						<th>Идентификатор</th>
					</tr>
					<?php while($resArr = $mySqlQuery->getNextArray(MYSQLI_ASSOC)): ?>
						<tr>
							<td><a href=<?='"tool_order.php?toolId=' . $resArr['name'] . '"'?>><?= $resArr['name'] ?></a></td>
							<td><?= $resArr['id'] ?></td>
						</tr>
					<?php endwhile;?>
				</table>
			</div>
		</div>
	</body>
</html>
