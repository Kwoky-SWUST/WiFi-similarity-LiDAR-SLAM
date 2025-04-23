<?php

require_once __DIR__ .'/db_connect.php';

if(isset($_POST['user_id']) && isset($_POST['steps']) && isset($_POST['$heading'])){
	$user_id = $_POST['user_id'];
	$steps=$_POST['steps'];
	$$heading=$_POST['$heading'];


    	$query = mysql_query("INSERT INTO imu_mobile_heading_steps (user_id, steps, heading) VALUES('$user_id','$steps', '$heading')");

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



