<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_POST['mac_address']) && isset($_POST['map_id']) && isset($_POST['longitude']) && isset($_POST['latitude'])){
	$mac_address = $_POST['mac_address'];
	$map_id = $_POST['map_id'];
	$tem_wifi=$_POST['tem_wifi'];
	$longitude=$_POST['longitude'];
	$latitude=$_POST['latitude'];

	 $query = mysql_query("INSERT INTO ap_coordinates (mac_address,map_id, longitude, latitude) values ('$mac_address','$map_id', '$longitude','$latitude')") or die(mysql_error());
        if($query){
             echo "successfully inserted!";
        }
        else {
                echo "problem with insertion!";}

}
else{
		echo "Please input all fields!";
	}
?>

