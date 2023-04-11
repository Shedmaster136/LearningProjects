<?php
	require __DIR__ . '/functions/login.php';
	if(getUserLogin())
	{
		header('Location: /private/');
	}
	else
	{
		header('Location: /login/');
	}
?>
