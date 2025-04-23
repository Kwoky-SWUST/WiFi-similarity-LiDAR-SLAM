<?php
/**
 * Created by PhpStorm.
 * User: jiao
 * Date: 6/1/16
 * Time: 4:48 PM
 */
require_once './db_operation/db_operate.php';
require_once './util/enlarge_operate.php';
require_once './util/init_map_info.php';
$db = new db_operate();
$enlarge = new enlarge_operate();
$x = new init_map();
$rs = array();
if (isset($_POST["FirstSelect"])&&isset($_POST["idCount"])) { 
    if ($_POST["FirstSelect"] == "true") {
       $timestamp = time();
        $rs = $db->query_counts($timestamp-2,$timestamp);
    } else {
        $rs[0] = $db->query_by_id($_POST["idCount"]);
    }
    $i = 0;
    $text = "";
    while ($i < count($rs)) {
        //echo $rs[$i]->location_x ." " . $rs[$i]->location_y ."<br/>";
        $result_point_str = $enlarge->getEnlargePointsString($rs[$i]->location_x,$rs[$i]->location_y);
        //echo $result_point_str . "<br/>";
        $map_info = $x->get_map_info($rs[$i]->map_id);
        //echo $map_info. "<br/>";
        $pro_map_info = $x->transform_map_info($map_info);
        //echo $pro_map_info. "<br/>";       
        $text = $text . $rs[$i]->user_id ." " . $rs[$i]->map_id ." " . $result_point_str . " " . $pro_map_info . " " . $rs[$i]->id . " " . $rs[$i]->color . ";";
        $i++; 
    }
    $len = strlen($text)-1;
    if (count($rs) == 0) {
    } else {
        echo substr($text,0,$len);
    }
} else {
    echo "error:don't post data to server";
}

