
<?php

if(isset($_GET['user_id'])){
        $user_id = $_GET['user_id'];

	require_once __DIR__ .'/db_irps_wifi_connect.php';

	// connecting to db
	$db = new DB_CONNECT();
   
        $query = mysql_query("SELECT * FROM wifi_indoor_relative where user_id='" . $user_id . "' limit 1;");
        if (mysql_num_rows($query)==0){
            echo "The user_id " . $user_id . " can not be found in the database!";
        } else {
            $query = mysql_query("INSERT INTO wifi_rawdata_relative (user_id, tem_wifi) values ('$user_id', 1)") or die(mysql_error());
            if($query){
                echo "reset user " . $user_id . " successfully!";
            } else {
                echo "reset user " . $user_id . " failed!";
            }
        }
}else{
        echo "Please input the user_id!";
}
mysql_close($con);
?>
