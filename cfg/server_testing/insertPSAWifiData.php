<?php
 
/*
 * Following code will insert a new reading row
 */
 
// array for JSON response
$response = array();
 
// check for required fields
if (isset($_GET['data'])) {
 
    $data = $_GET['data'];
    $newData = explode(",", $data);
    $user_id = $newData[0];
    $tem_wifi = $newData[1];
    $color = $newData[2];

    echo "entered php <br> <br>";
 
    // include db connect class
    require_once __DIR__ . '/db_ap_connect.php';
 
    // connecting to db
    $db = new DB_CONNECT();
        echo "before inserting data<br><br>";
    // mysql inserting a new row
    $result = mysql_query("INSERT INTO psa_rawdata (user_id, tem_wifi, color) VALUES('$user_id','$tem_wifi','$color')");

    // check if row inserted or not
    if ($result) {
        echo "success<br><br>";
        // successfully inserted into database
        $response["success"] = 1;
        $response["message"] = "Data successfully created.<br><br>";
 
        // echoing JSON response
        echo json_encode($response);
    } else {
        echo "failed to enter data to database<br><br>";
        // failed to insert row
        $response["success"] = 0;
        $response["message"] = "Oops! An error occurred.<br><br>";
 
        // echoing JSON response
        echo json_encode($response);
    }
}

 else {
    echo "give correct values<br><br>";
    // required field is missing
    $response["success"] = 0;
    $response["message"] = "Required field(s) is missing";
 
    // echoing JSON response
    echo json_encode($response);
}
?>
