<?php
require_once __DIR__ . '/db_ap_connect.php';
$db = new DB_CONNECT();
//$time = strtotime(date('Y-m-d H:i:s', strtotime('-10 hour')));
//echo $time;
$result = mysql_query("SELECT * FROM (SELECT * FROM  `wifi_indoor_location` WHERE timestamp >= (current_timestamp()-interval 5 minute) ORDER by timestamp DESC) as a GROUP BY user_id");


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

echo  json_encode($response);
?>
