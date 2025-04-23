<?php
/**
 * Created by PhpStorm.
 * User: jiao
 * Date: 6/1/16
 * Time: 4:48 PM
 */
require_once './db_operation/db_operate.php';
require_once './util/enlarge_operate.php';
require_once './db_operation/wifi_indoor.php';
$db = new db_operate();
$enlarge = new enlarge_operate();
$id = $_GET["id"];
$central = $_GET["central"];
$rs = array();
if ($id == "-1") {
	$timestamp = time();
	$rs = $db->query_counts($timestamp-1,$timestamp+2);
} else {
	$rs = $db->query_by_id($id);
}

$i = 0;
$centralUser = new wifi_indoor();
$text = "";
$centralUser = $db->query_max_user($central);
$i=0;
while ($i < count($rs)) {
	$rs[$i]->location_y = -$rs[$i]->location_y;
	if ($rs[$i]->user_id == $centralUser->user_id) {
		$rs[$i]->location_x = 0;
		$rs[$i]->location_y = 0;
	} else {
		$rs[$i]->location_x = $rs[$i]->location_x - $centralUser->location_x;
		$rs[$i]->location_y = $rs[$i]->location_y - $centralUser->location_y;
	}
	$i++;
}
echo json_encode(array("locations"=>$rs));

