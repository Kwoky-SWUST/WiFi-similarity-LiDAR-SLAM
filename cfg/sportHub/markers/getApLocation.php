<?php
require_once __DIR__ . '/db_ap_connect.php';

$db= new DB_CONNECT();

$result = mysql_query("select * from ap_coordinates where map_id='SportsHub_outside'");


	$response = array();
	$i=0;
	while($row = mysql_fetch_array($result)){
		$reading = array();
		$reading['mac_address'] = $row['mac_address'];
		$reading['map_id'] = $row['map_id'];
		$reading['longitude'] = $row['longitude'];
		$reading['latitude'] = $row['latitude'];
	
		$response[$i]=$reading;
		$i++;
	}
//echo $response;
echo json_encode($response);
?>
