<?php
require_once __DIR__ . '/db_ap_connect.php';
$db = new DB_CONNECT();
//$time = strtotime(date('Y-m-d H:i:s', strtotime('-10 hour')));
//echo $time;
//$startTime = '2016-08-11 00:42:29';
//$endTime = '2016-08-11 13:45:54';

$startTime = '2016-08-11 18:30:00';
$endTime = '2016-08-11 19:20:00';

$data = array();
$result = mysql_query("SELECT user_id, timestamp, longitude, latitude FROM wifi_indoor_location WHERE timestamp BETWEEN '$startTime' AND '$endTime' ORDER BY timestamp ASC");

    while($row = mysql_fetch_assoc($result))
        $rows[] = $row;

    foreach($rows as $row)
    {
        {
                array_push($data, $row);
        }
    }
echo  json_encode($data);
?>
