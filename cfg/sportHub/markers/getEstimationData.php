<?php
require_once "location_class.php";
$myfile = fopen("estimation.txt", "r+") or die("unable to open estimation");
$location_arr = array();
$i=0;
while($i<79){
    $i++;
    $line = fgets($myfile);
    $line = str_replace("\r\n","",$line);
    $str_array = explode(" ",$line);
	$location = new location_class();
	$location->ap_name = $str_array[0];
	
	$location->estimation_level = $str_array[1];
	$location->estimation_latitude = $str_array[3];
	$location->estimation_longtitude = $str_array[2];
	
	$location->ap_level = $str_array[4];
	$location->ap_latitude = $str_array[6];
	$location->ap_longtitude = $str_array[5];
        $location->computeDistance();	

	$location_arr[] = $location; 
}
fclose($myfile);
echo json_encode(array("locations"=>$location_arr));
?>
