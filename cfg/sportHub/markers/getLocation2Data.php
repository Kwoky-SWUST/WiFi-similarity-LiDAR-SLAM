<?php
require_once __DIR__ . '/db_ap_connect.php';
//date_default_timezone_set('prc');
$phoneName = $_GET["phoneName"];
$startTime = $_GET["startTime"];

//echo $phoneName;
//echo "<br>";
//echo $startTime;
//echo "<br>";
//echo $endTime;
//echo "<br>";

$db = new DB_CONNECT();
//echo $time;
$result = mysql_query("SELECT * FROM wifi_indoor_location WHERE timestamp >=\" ".$startTime."\" AND user_id=".$phoneName." limit 1");

//echo $result;
$response = array();
	$i =0;
	while($row = mysql_fetch_array($result))
	{  $reading = array();  
 		$reading['user_id'] = $row['user_id'] ;
		$reading['map_id'] = $row['map_id'];
		$reading['longitude'] = $row['longitude'];
		$reading['latitude'] = $row['latitude'];
		$reading['timestamp'] = $row['timestamp'];
	
	$response[$i] = $reading;
	$i++;	
		 
	}
//print_r(mysql_fetch_array($result));  
$json = json_encode($response);
echo $json;

?>
