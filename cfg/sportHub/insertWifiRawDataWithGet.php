<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_GET['user_id']) && isset($_GET['tem_wifi'])){
	$user_id = $_GET['user_id'];
	$tem_wifi=$_GET['tem_wifi'];


	 $query = mysql_query("INSERT INTO wifi_raw_data (user_id, tem_wifi) values ('$user_id', '$tem_wifi')") or die(mysql_error());
        if($query){
            $query = mysql_query("select * from parameters where parameter_name='sampling_time'") or die(mysql_error());
            $result=array();
            while($row = mysql_fetch_array($query))
                {  	
                $result[$row['parameter_name']]=$row['value'];
                }
            echo json_encode($result);
        }
        else {
                echo "problem with insertion!";}

}
else{
		echo "Please input all fields!";
	}
?>

