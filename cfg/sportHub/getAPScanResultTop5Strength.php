<?php
require 'db_connect.php';
require 'macLocation_class.php';
function sortStrength($a,$b) {
    if ($a->ap_strength > $b->ap_strength) {
        return -1;
    } else {
        return  1;
    }
}
$query = mysql_query("SELECT * from wifi_raw_data where user_id='356319062089796' ORDER BY timestamp DESC limit 1") or die(mysql_error());
$wifi_info = "";
while($row = mysql_fetch_array($query)) { 
    $wifi_info = $row["tem_wifi"];
}

$macLocation_arr = array();
$wifi_info = str_replace("\r\n","",$wifi_info);
$wifi_array = explode(" ", $wifi_info);
for($i=1; $i<count($wifi_array)-1; $i++) {
    $mac = $wifi_array[$i];
    $rs = mysql_query("select * from ap_coordinates_combined where mac_address='" .$mac. "'");
    if($rs) {
        $macLocation = new mac_location_class();
	while ($row = mysql_fetch_array($rs)) {
            $macLocation->mac_address = $row["mac_address"];
            $macLocation->level = $row["map_id"];
            $macLocation->ap_id = "";
            $macLocation->ap_latitude = $row["latitude"];
            $macLocation->ap_longtitude = $row["longitude"];
            $macLocation->ap_strength = $wifi_array[$i+1];
            $macLocation_arr[] = $macLocation;
        }
    }
    $i++;
}
usort($macLocation_arr,'sortStrength');
echo json_encode(array("mac_locations"=>array_slice($macLocation_arr,0,5)));
?>
