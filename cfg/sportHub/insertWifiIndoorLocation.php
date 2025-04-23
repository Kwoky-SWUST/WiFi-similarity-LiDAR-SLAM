<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_POST['user_id']) && isset($_POST['map_id']) && isset($_POST['longitude']) && isset($_POST['latitude'])){
    
	$user_id = $_POST['user_id'];
	$map_id=$_POST['map_id'];
	$longitude=$_POST['longitude'];
	$latitude=$_POST['latitude'];


	 $query = mysql_query("INSERT INTO wifi_indoor_location (user_id, map_id,longitude,latitude) values ('$user_id', '$map_id', '$longitude', '$latitude')") or die(mysql_error());
        if($query){
                echo "successfully inserted!";}
        else {
                echo "problem with insertion!";}

}
else{
		echo "Please input all fields!";
	}
?>

