
<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_GET['user_id'])){
        $user_id = $_GET['user_id'];
        $query = mysql_query("SELECT * FROM wifi_indoor_location where user_id='" . $user_id . "' limit 1;");
        if (mysql_num_rows($query)==0){
            echo "The user_id " . $user_id . " can not be found in the database!";
        } else {
            $query = mysql_query("INSERT INTO wifi_raw_data_convert (user_id, tem_wifi) values ('$user_id', -1)") or die(mysql_error());
            $query = mysql_query("INSERT INTO wifi_raw_data (user_id, tem_wifi) values ('$user_id', -1)") or die(mysql_error());
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
