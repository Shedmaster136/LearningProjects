<?php
	require $_SERVER['DOCUMENT_ROOT'] . '/functions/login.php';
	$login = getUserLogin();
	if(!$login)
	{
		header('Location: /login/');
		exit;
	}
	
	// 
	$query = "SELECT purchase_act.id,
	purchase_act.date,
	enc_tool.name,
	tool_unit.id AS tool_id,
	download_complete.date AS downloadDate
	FROM user
	JOIN purchase_act ON user.id = purchase_act.user
	JOIN tool_unit ON purchase_act.tool_unit = tool_unit.id
	JOIN enc_tool ON tool_unit.enc_tool = enc_tool.id
	LEFT JOIN download_complete ON purchase_act.id = download_complete.id
	WHERE email = ?";
	$args = array();
	array_push($args, $login);
	$mySqlQuery = new MySQLQueries();
	if(!$mySqlQuery->openConnection())
	{
		echo "Ошибка на сайте";
		exit;
	}
	if(!$mySqlQuery->sqlQuery($query, $args))
	{
		echo "Ошибка на сайте";
		exit;
	}
	//
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
		<a href='/crypto_tools'>Криптографические средства</a>
		<div id="main_content">
			<h1>Список СКЗИ</h1>
			<div class="database_list">
				<table id = "tool_list">
					<tr>
						<th>Название</th>
						<th>Идентификатор</th>
						<th>Дата заказа</th>
						<th>Приобретение</th>
						<th>Подтверждено</th>
					</tr>
					<?php while($resArr = $mySqlQuery->getNextArray(MYSQLI_ASSOC)): ?>
						<tr>
							<td><?= $resArr['name'] ?></td>
							<td><?= $resArr['tool_id'] ?></td>
							<td><?= $resArr['date'] ?></td>
							<td><?= $resArr['id'] ?></td>
							<td><?php 
								if(isset($resArr['downloadDate']))
								{
									echo $resArr['downloadDate'];
									continue;
								}
								if($resArr['downloaded'])
								{
									echo'<a href="confirm.php?purchaseId=' . $resArr['id'] . '"'.'>Подтвердить</a>';
									continue;
								}
								echo '<a href="/functions/download.php?toolId=' . $resArr['name'] . '"' .'>Загрузить</a>';
								?></td>
						</tr>
					<?php endwhile;?>
				</table>
			</div>
		</div>
	</body>
</html>
