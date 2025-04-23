<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_POST['user_id']) && isset($_POST['tem_wifi'])){
	$user_id = $_POST['user_id'];
	$tem_wifi=$_POST['tem_wifi'];

        $wifi_array = explode(" ",$tem_wifi);
        $wifi_lng_lat = $wifi_array[0];
        for ($i=1; $i<(count($wifi_array)-1); $i++) {
            $lng_lat = mysql_query("select * from ap_coordinates_combined where mac_address='" . $wifi_array[$i] . "'");
            while ($row = mysql_fetch_array($lng_lat)) {
                $wifi_lng_lat = $wifi_lng_lat . " " .$row["longitude"] ." " . $row["latitude"] ." " . $row["map_id"]  . " " . $wifi_array[$i+1];
            } 
            $i++;
        }
        $query = mysql_query("INSERT INTO wifi_raw_data_convert (user_id, tem_wifi) values ('$user_id', '$wifi_lng_lat')") or die(mysql_error());
	 $query = mysql_query("INSERT INTO wifi_raw_data (user_id, tem_wifi) values ('$user_id', '$tem_wifi')") or die(mysql_error());
        if($query){
            $query = mysql_query("select * from parameters where parameter_name='sampling_time'") or die(mysql_error());
            $result=array();
            while($row = mysql_fetch_array($query))
                {  	
                $result[$row['parameter_name']]=$row['value'];
                }
            echo json_encode($result);
        }
        else {
                echo "problem with insertion!";}

}
else{
		echo "Please input all fields!";
	}
mysql_close($con);
?>

