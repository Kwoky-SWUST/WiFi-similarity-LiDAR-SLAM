<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_POST['user_id']) && isset($_POST['longitude']) && isset($_POST['latitude']) && isset($_POST['altitude']) && isset($_POST['accuracy'])){
	$user_id = $_POST['user_id'];
	$longitude=$_POST['longitude'];
	$latitude=$_POST['latitude'];
	$altitude=$_POST['altitude'];
	$accuracy=$_POST['accuracy'];


	 $query = mysql_query("INSERT INTO gps_mobile (user_id, longitude,latitude,altitude,accuracy) values ('$user_id', '$longitude', '$latitude', '$altitude', '$accuracy')") or die(mysql_error());
        if($query){
                echo "successfully inserted!";}
        else {
                echo "problem with insertion!";}

}
else{
		echo "Please input all fields!";
	}
?>

