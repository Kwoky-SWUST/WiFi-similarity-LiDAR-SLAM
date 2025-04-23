<?php

require_once __DIR__ .'/db_connect.php';
if(isset($_POST['time_start']) && isset($_POST['time_end']) ){
	$time_start = $_POST['time_start'];
	$time_end = $_POST['time_end'];


	 $query = mysql_query("SELECT * FROM wifi_indoor_location WHERE timestamp>='$time_start' and timestamp<='$time_end';") or die(mysql_error());

        if($query){
            $responce=array();
            $i =0;
            $result=array();
            while($row = mysql_fetch_array($query))
                { 
                $result['user_id']=$row['user_id'];
                $result['map_id']=$row['map_id'];
                $result['timestamp']=$row['timestamp'];
                $result['longitude']=$row['longitude'];
                $result['latitude']=$row['latitude'];
                $responce[$i]=$result;
                $i++;
                }
            echo json_encode($responce);
        }
        else {
                echo "problem with insertion!";
        }

}
else{
		echo "Please input all fields!";
	}
?>

