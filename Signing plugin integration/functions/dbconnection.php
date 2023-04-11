<?php
	class MySQLQueries
	{
		private const HOST_NAME = "192.168.1.26";
		private const DB_NAME = "skzi_accounting";
		private const DB_USER = "SKZI_site";
		private const PASSWORD = "dc61m8e10al4";
		
		private $statementSet = false;
		private $queryStatement;
		
		private $resultSet = false;
		private $queryResult;
		
		private $dbLink = null;
		private $initialized = false;
		
		function __construct()
		{
			
		}
		function __destruct()
		{
			if($this->initialized)
			{
				$this->dbLink->close();
			}
		}
		
		public function openConnection(): bool
		{
			$this->dbLink = new mysqli(self::HOST_NAME, self::DB_USER, self::PASSWORD, self::DB_NAME);
			if(!$this->dbLink)
			{
				return false;
			}
			$this->initialized = true;
			return true;
		}
		
		
		public function sqlQuery(string $query, array $args = null):bool
		{
			if(!$this->initialized)
			{
				return false;
			}
			if(substr_count($query, '?') != count($args))
			{
				return false;
			}
			if($this->statementSet)
			{
				$this->queryStatement->close();
			}
			if(!$this->queryStatement = $this->dbLink->prepare($query))
			{
				return false;
			}
			$this->statementSet = true;
			if($args != null)
			{
				$queryArgTypes = "";
				foreach($args as $argument)
				{
					if (is_string($argument))
					{
						$queryArgTypes .= "s";
					}
					elseif(is_double($argument))
					{
						$queryArgTypes .= "d";
					}
					elseif(is_integer($argument))
					{
						$queryArgTypes .= "i";
					}
					else
					{
						return false;
					}
				}
				array_unshift($args, $queryArgTypes);
				call_user_func_array(array($this->queryStatement, "bind_param"), $args);
			}
			if(!$this->queryStatement->execute())
			{
				return false;
			}
			$this->queryResult = $this->queryStatement->get_result();
			if(!$this->queryResult)
			{
				$this->resultSet = false;
			}
			else
			{ 
				$this->resultSet = true;
			}
			return true;
		}
		public function getNextArray(int $resultType): ?array
		{
			if(!$this->resultSet)
			{
				return null;
			}
			return $this->queryResult->fetch_array($resultType);
		}
	}
?>
