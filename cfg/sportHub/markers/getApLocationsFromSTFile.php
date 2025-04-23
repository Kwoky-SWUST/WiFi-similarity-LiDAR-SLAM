<?php
require_once "location_class.php";
$myfile = fopen("ap_locations_st.txt", "a+") or die("unable to open estimation");
$location_arr = array();
$i=0;
while($i<1500){
    $i++;
    $line = fgets($myfile);
    $line = str_replace("\r\n","",$line);
    $str_array = explode("\t",$line);
	$location = new location_class();
	$location->ap_name = "";

	$location->ap_level = $str_array[0];
	$location->ap_latitude = $str_array[2];
	$location->ap_longtitude = $str_array[1];
	$location->ap_mac=$str_array[3];

	$location_arr[] = $location; 
}
fclose($myfile);
echo json_encode(array("locations"=>$location_arr));
?>
