<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_POST['user_id']) && isset($_POST['orientation_data']) && isset($_POST['accelerate_data']) && isset($_POST['magnetic_data']) && isset($_POST['barometer_data'])){
	$user_id = $_POST['user_id'];
	$orientation_data=$_POST['orientation_data'];
	$accelerate_data=$_POST['accelerate_data'];
	$magnetic_data=$_POST['magnetic_data'];
	$barometer_data=$_POST['barometer_data'];


    	$query = mysql_query("INSERT INTO imu_mobile (user_id, orientation_data, accelerate_data, magnetic_data, barometer_data) VALUES('$user_id','$orientation_data', '$accelerate_data','$magnetic_data','$barometer_data')");

        if($query){
        echo "successfully inserted!"; 
        $response["success"] = 1;
        $response["message"] = "Data successfully created.<br><br>";
 
        // echoing JSON response
        echo json_encode($response);
        }
        else {
        echo "problem with insertion!";
        $response["success"] = 0;
        $response["message"] = "Oops! An error occurred.<br><br>"; 
        // echoing JSON response
        echo json_encode($response);
        }
        }
        else{
		echo "Please input all fields!";
		$response["success"] = 0;
		$response["message"] = "Required field(s) is missing";
		// echoing JSON response
		echo json_encode($response);
	}
mysql_close($con);
?>


