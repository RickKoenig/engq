<?php
	session_start();
	// select host of database
	require '../common/php/dbinfo_local.php';
	// require '../common/php/dbinfo_webhost.php';
	$doreplace = true;
	
	function setupdatabase() {
		global $dbserver,$dbuser,$dbpassword,$dbdatabase;
		// connect to database
		mysql_connect($dbserver,$dbuser,$dbpassword)
		or die(mysql_error());
		
		// select the database
		mysql_select_db($dbdatabase)
		or die(mysql_error());
		
		// create table if needed
		mysql_query("CREATE TABLE users(id INT NOT NULL AUTO_INCREMENT,PRIMARY KEY(id),name VARCHAR(30),UNIQUE(name))")
		//or die(mysql_error());
		;
		mysql_query("CREATE TABLE tracks2 (id int(11) NOT NULL AUTO_INCREMENT, uid int(11) NOT NULL, name varchar(30),userdata int(11), JSONuserdata mediumtext, PRIMARY KEY (id))");
	}
	
	// return userid if user is in the database, 0 otherwise
	function login($username) {
		setupdatabase();
		
		// attempt to use existing name
		$good = false;
		$result = mysql_query("SELECT id FROM users WHERE name = '$username'")
		or die(mysql_error()); 
		$row = mysql_fetch_array( $result);
		if (!$row)
			return 0;
		return $row[0];
	}
	
	// return userid of new user if sucessfully registered, 0 otherwise
	function register($username) {
		setupdatabase();
		
		// attempt to use existing name
		$result = mysql_query("SELECT id FROM users WHERE name = '$username'")
		or die(mysql_error()); 
		$row = mysql_fetch_array( $result);
		if ($row)
			return 0;
			
		// attempt to insert new name
		mysql_query("INSERT INTO users(name) VALUES('$username') ")
		// or die(mysql_error());  
		;
		
		// attempt to use existing name again
		$result = mysql_query("SELECT id FROM users WHERE name = '$username'")
		or die(mysql_error()); 
		$row = mysql_fetch_array( $result);
		if (!$row)
			return 0;
		return $row[0];
	}
	

	function logout(&$lu,&$li) {
		if (isset($_SESSION['userid'])) {
			$lu = $_SESSION['username'];
			$li = $_SESSION['userid'];
			unset($_SESSION['username']);
			unset($_SESSION['userid']);
			return true;
		} else {
			return false;
		}
	}

	function disappear(&$lu,&$li) {
		if (isset($_SESSION['userid'])) {
			setupdatabase();
  			$lu = $_SESSION['username'];
			$li = $_SESSION['userid'];
			mysql_query("DELETE FROM users WHERE id=$li") 
			or die(mysql_error());
			unset($_SESSION['username']);
			unset($_SESSION['userid']);
			// TODO Maybe get rid of user's tracks too.
			return true;
		} else {
			return false;
		}
	}

	// echo JSON array of tracklist
	
	function gettracks($loginuserid) {
		setupdatabase();
		$result = mysql_query("SELECT * FROM tracks2 WHERE uid = $loginuserid ORDER BY name") or
//		$result = mysql_query("SELECT * FROM tracks2") or
		die(mysql_error());
		$firstrow = true;
		echo "[ ";
		while($row = mysql_fetch_array($result)){
			if (!$firstrow) {
				echo ',';
			}
			$firstcol = true;
			echo '{';
			for($i = 0; $i < mysql_num_fields($result); $i++){
				if (!$firstcol) {
					echo ',';
				}
				$fn = mysql_field_name($result, $i);
				$tp = mysql_field_type($result, $i);
				echo "\"$fn\":";
/*				if ($tp == 'string') {
						echo "\"string $row[$i]\"";
					//if (strlen($row[$i])>0)
					//	echo "$row[$i]";
					//else
					//	echo "\"zerolenstr\"";
				} else if ($tp == 'blob') {
					if (strlen($row[$i])>0)
						echo "\"blob $row[$i]\"";
					else
						echo "\"zerolenblob\"";
				} else if ($tp == 'int')
					echo "$row[$i]";
				else
					echo "\"huey\""; 
  */
  				if ($tp == 'int')
					echo "$row[$i]";
				else {
	  				$j =json_encode($row[$i]);
  					echo "$j";
				}
  				//echo "\"$tp\"";
				$firstcol = false;
			}
			echo '}';
			$firstrow = false;
		}
		echo " ]";
	}

	function getuserdata($loginuserid,$tname,$fieldname,&$udata) {
		$udata = "hey";
//		return true;
		setupdatabase();
		// attempt to use existing name
		$result = mysql_query("SELECT $fieldname FROM tracks2 WHERE uid = $loginuserid AND name = '$tname'");
		if (!$result) {
			return false;
    		//die('Invalid query: ' . mysql_error());
		}
		$tp = mysql_field_type($result,0);
		$row = mysql_fetch_array($result);
		if (!$row)
			return false;
		if (!sizeof($row))
			return false;
		if ($tp == 'int')
			$udata = $row[0];
		else 
			$udata = json_encode($row[0]);
		//if (true)
		//	$udata = "\"ho\"";
		return true;
	}
	
	function deletetrack($loginuserid,$tname) {
		setupdatabase();
		// attempt to use existing name
		$result = mysql_query("DELETE FROM tracks2 WHERE uid = $loginuserid AND name = '$tname'") or
		die(mysql_error());
		return true;
	}
	
	function setuserdata($loginuserid,$tname,$fieldname,$udata) {
		global $doreplace;
		//$udata = "hey";
//		return true;
		setupdatabase();
		// attempt to use existing name
		$result = mysql_query("SELECT $fieldname FROM tracks2 WHERE uid = $loginuserid AND name = '$tname'")
		or die(mysql_error());
		$row = mysql_fetch_array($result);
		if ($doreplace) {
			$result = mysql_query("DELETE FROM tracks2 WHERE uid = $loginuserid AND name = '$tname'") or
			die(mysql_error());
		} else {
			if ($row)
				return false;
		}
		$ud = $loginuserid * 10;
		$result = mysql_query("INSERT INTO tracks2 (uid, name, userdata, JSONuserdata) VALUES($loginuserid,'$tname',$ud,'$udata') ")
		or die(mysql_error());  
		return $result;
	}
	
// entrypoint of tracksmain.php
	// pass in $name,$age,$obj
	extract($_REQUEST);
	if (isset($command)) {
		if ($command == 'login') { // $name
			$res = login($name);
			if ($res) {
				$_SESSION['username'] = $name;
				$_SESSION['userid'] = $res;
				echo "{";
				echo " \"result\":true,"; 	// the php will add the quotes to non quoted strings
				echo " \"username\":\"$name\","; 	// the php will add the quotes to non quoted strings
				echo " \"userid\":$res,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"login sucessful\","; 		// this is a number, no quotes are necessary
				echo " \"list\":";
					gettracks($res);
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"login failed\""; 		// this is a number, no quotes are necessary
				echo "}";
			}
		} else if ($command == 'register') { // $name
			$res = register($name);
			if ($res) {
				echo "{";
				echo " \"result\":true,";
				echo " \"userid\":$res,";
				echo " \"message\":\"registration sucessful\"";
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,";
				echo " \"message\":\"registration failed\"";
				echo "}";
			}
			
		} else if ($command == 'logout') {
			$louser = "";
			$loid = 0;
			if (logout($louser,$loid)) {
				echo "{";
				echo " \"result\":true,"; 	// the php will add the quotes to non quoted strings
				echo " \"username\":\"$louser\","; 	// the php will add the quotes to non quoted strings
				echo " \"userid\":$loid,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"logout sucessful\""; 		// this is a number, no quotes are necessary
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"logout failed\""; 		// this is a number, no quotes are necessary
				echo "}";
			}
		} else if ($command == 'disappear') {
			$louser = "";
			$loid = 0;
			if (disappear($louser,$loid)) {
				echo "{";
				echo " \"result\":true,"; 	// the php will add the quotes to non quoted strings
				echo " \"username\":\"$louser\","; 	// the php will add the quotes to non quoted strings
				echo " \"userid\":$loid,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"disappear sucessful\""; 		// this is a number, no quotes are necessary
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"disappear failed\""; 		// this is a number, no quotes are necessary
				echo "}";
			}
		} else if ($command == 'getdata') { // $name
			$fieldname = "JSONuserdata";
			//$fieldname = "userdata";
			if (getuserdata($_SESSION['userid'],$name,$fieldname,$userdata)) {
				echo "{";
				echo " \"result\":true,"; 	// the php will add the quotes to non quoted strings
				//if (strlen($userdata))
				echo " \"data\":$userdata,"; 	// the php will add the quotes to non quoted strings
				//echo " \"$fieldname\":$userdata,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"found track\""; 		// this is a number, no quotes are necessary
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"can't find track\""; 		// this is a number, no quotes are necessary
				echo "}";
			}
		} else if ($command == 'deletedata') { // $name
			$fieldname = "JSONuserdata";
			//$fieldname = "userdata";
			if (deletetrack($_SESSION['userid'],$name)) {
				echo "{";
				echo " \"result\":true,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"deleted track\","; 		// this is a number, no quotes are necessary
				echo " \"list\":";
					gettracks($_SESSION['userid']);
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"can't find track\""; 		// this is a number, no quotes are necessary
				echo "}";
			}
		} else if ($command == 'setdata') { // $name,$data
			$fieldname = "JSONuserdata";
			//$fieldname = "userdata";
			//$name = 'aname';
			//$userdata = 47;
			if (setuserdata($_SESSION['userid'],$name,$fieldname,$data)) {
				echo "{";
				echo " \"result\":true,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"track inserted\","; 		// this is a number, no quotes are necessary
				echo " \"list\":";
					gettracks($_SESSION['userid']);
				echo "}";
			} else {
				echo "{";
				echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
				echo " \"message\":\"track already exists\""; 		// this is a number, no quotes are necessary
				echo "}";
			}
		} else { // invalid command
			echo "{";
			echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
			echo " \"message\":\"bad command\""; 		// this is a number, no quotes are necessary
			echo "}";
		}
	} else {
		echo "{";
		echo " \"result\":false,"; 	// the php will add the quotes to non quoted strings
		echo " \"message\":\"no command\""; 		// this is a number, no quotes are necessary
		echo "}";
	}
		
?>
