<?php
	require_once $_SERVER['DOCUMENT_ROOT'] . '/functions/dbconnection.php';
	function checkAuth(string $login, string $password): bool
	{
		$query = "SELECT password FROM user WHERE email = ?";
		$args = array();
		array_push($args, $login);
		$mySqlQuery = new MySQLQueries();
		if(!$mySqlQuery->openConnection())
		{
			return false;
		}
		if(!$mySqlQuery->sqlQuery($query, $args))
		{
			return false;
		}
		if(!$resArr = $mySqlQuery->getNextArray(MYSQLI_NUM))
		{
			return false;
		}
		if($password === $resArr[0])
		{
			//id generation and database setup
			setcookie('login', $login, time()+3600, '/');
			setcookie('password', $password, time()+3600, '/');
			return true;
		}
		return false;
	}
	//get login from generated cookie sequence. probably no reason for this fucntion at all
	function getUserLogin(): ?string
	{
		$loginFromCookie = $_COOKIE['login'] ?? '';
		$passwordFromCookie = $_COOKIE['password'] ?? '';
		if(checkAuth($loginFromCookie, $passwordFromCookie))
		{
			return $loginFromCookie;
		}
		return null;
	}
?>
