<?php

/**
 * Created by PhpStorm.
 * User: jiao
 * Date: 6/1/16
 * Time: 3:55 PM
 */
include_once 'db_config.php';
require_once 'wifi_indoor.php';
class db_operate
{
    public $conn;
    public function __construct() {
        $this->conn = mysql_connect(HOST,USERNAME,PASSWORD);
        if (!$this->conn) {    
            die("could not connect database, error:" .mysql_error());
        }        
        mysql_select_db(DATABASE, $this->conn);
    }

    public function __destruct() {        
            $this->close_connection();        
	}

    public function query_max_user($id) {        
        $wifi = new wifi_indoor();
	$records = array();
        $result = mysql_query("select * from wifi_indoor_relative WHERE user_id = '" . $id ."' and id = (select max(id) from wifi_indoor_relative where user_id = '" . $id . "')");
	if (!$result) {
		return $wifi;
	}
        while ($row = mysql_fetch_object($result)) {
            $wifi->color = $row->color;
            $wifi->id = $row->id;
            $wifi->location_x = $row->location_x;
            $wifi->location_y = $row->location_y;
            $wifi->map_id = $row->map_id;
            $wifi->timestamp = $row->timestamp;
            $wifi->user_id = $row->user_id;
	    $wifi->orientation = $row->orientation;
	    $records[] = $wifi;
        }
        return $wifi;
    }

    public function query_by_id($id) {
        $wifi = new wifi_indoor();
	$records = array();
        $result = mysql_query("select * from wifi_indoor_relative WHERE id >= '$id'");
        while ($row = mysql_fetch_object($result)) {
            $wifi = new wifi_indoor();
	    $wifi->color = $row->color;
            $wifi->id = $row->id;
            $wifi->location_x = $row->location_x;
            $wifi->location_y = $row->location_y;
            $wifi->map_id = $row->map_id;
            $wifi->timestamp = $row->timestamp;
            $wifi->user_id = $row->user_id;
	    $wifi->orientation = $row->orientation;
	    $records[] = $wifi;
        }
        return $records;
    }
    
    public function query_color_by_id($id) {
        $color = "";
        $result = mysql_query("select color from wifi_indoor_relative WHERE user_id = '$id' limit 1");
        while ($row = mysql_fetch_array($result)) {
            $color = $row["color"];
        }
        return $color;
    }
    
    public function query_counts($start_time, $end_time) {
        $records = array();
        $result = mysql_query("select * from wifi_indoor_relative WHERE timestamp BETWEEN '$start_time' AND '$end_time'");
        $i=0;
        while ($row = mysql_fetch_object($result)) {
            $wifi = new wifi_indoor();
            $wifi->color = $row->color;
            $wifi->id = $row->id;
            $wifi->location_x = $row->location_x;
            $wifi->location_y = $row->location_y;
            $wifi->map_id = $row->map_id;
            $wifi->timestamp = $row->timestamp;
            $wifi->user_id = $row->user_id;
	    $wifi->orientation = $row->orientation;
            $records[] = $wifi;
            $i++;
        }
        return $records;
    }

    private function close_connection() {
        mysql_close($this->conn);
    }
}
