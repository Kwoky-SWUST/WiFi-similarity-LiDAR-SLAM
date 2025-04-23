<?php

require_once __DIR__ .'/db_connect.php';
     
        $query = mysql_query("SELECT * FROM (SELECT * from wifi_indoor_location ORDER BY timestamp DESC) AS a GROUP BY user_id;") or die(mysql_error());

        if($query){
            $responce=array();
            $i =0;
            $result=array();
            while($row = mysql_fetch_array($query))
                { 
                $result['user_id']=$row['user_id'];
                $result['map_id']=$row['map_id'];
                $result['timestamp']=strtotime($row['timestamp'])*1000;
                $result['longitude']=$row['longitude'];
                $result['latitude']=$row['latitude'];
                $responce[$i]=$result;
                $i++;
                }
            echo json_encode($responce);
        }
        else {
                echo "problem with access!";
        }
?>

