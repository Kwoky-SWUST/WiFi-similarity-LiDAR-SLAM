<?php

require_once __DIR__ .'/db_connect.php';
if(isset($_GET['user_id']) ){
	$user_id = $_GET['user_id'];


	 $query = mysql_query("SELECT * FROM `wifi_indoor_location` where user_id = '$user_id' ORDER BY timestamp DESC limit 1") or die(mysql_error());
        if($query){
           //echo "SELECT * FROM `wifi_indoor_location` where user_id = '$user_id' ORDER BY timestamp DESC limit 1";
            $result=array();
            while($row = mysql_fetch_array($query))
                {  	
    		 $result['user_id']=$row['user_id'];
                $result['map_id']=$row['map_id'];
                $result['timestamp']=$row['timestamp'];
                $result['longitude']=$row['longitude'];
                $result['latitude']=$row['latitude'];
                }
            echo json_encode($result);
        }
        else {
                echo "problem with access!";
        }

}
else{
		echo "Please input all fields!";
	}
?>

