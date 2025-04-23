<?php
define ("FILEPATH","./map_info/");

class init_map{
    public function get_map_info($map_id) {
        if ($this->check_map_file($map_id)) {
            require_once FILEPATH . $map_id . "_map_info.php";
        } else {
            return "123";
        }
        
        $map_name = MAP_NAME;
	$map_min_x = MAP_MIN_X;
        $map_max_x = MAP_MAX_X;
        $map_min_y = MAP_MIN_Y;
        $map_max_y = MAP_MAX_Y;
        $map_resolution = MAP_RESOLUTION;
        $map_ap_info = AP_INFO;
        $length = $map_max_x - $map_min_x;
        $width = $map_max_y - $map_min_y;
        
        $map_info_str = $map_name . " " . $length . " " . $width . " " . $map_ap_info;
        return $map_info_str;
    }
    
    public function check_map_file($map_id) {
        $filename = FILEPATH . $map_id . "_map_info.php";
        if (file_exists($filename)) {
            return true;
        } else {
            return false;
        }
    }

    public function transform_map_info($map_info) {
        $map_info_arr = explode(" ",$map_info);
        $pro = "";
        $i = 1;
        while ($i < count($map_info_arr)) {
            $para = (int)($map_info_arr[$i] * 50);
            
            $pro = $pro . $para . "_";
            $i++;
        }
        $len = strlen($pro)-1;
        return substr($pro,0,$len);
    }   
}
?>
