<?php
class location_class{
	public $ap_name;
	public $ap_level;
	public $ap_latitude;
	public $ap_longtitude;
	public $ap_mac;
	
	public $estimation_level;
	public $estimation_latitude;
	public $estimation_longtitude;

	public $error;

	function computeDistance() {
            $lat_dis = abs($this->ap_latitude - $this->estimation_latitude);
            $lng_dis = abs($this->ap_longtitude - $this->estimation_longtitude);
            $distance = sqrt($lat_dis*$lat_dis + $lng_dis*$lng_dis);
            $this->error = $this->getdistance();
          //  $this->error = $distance*100000;

	}

	function getdistance(){
    	    //将角度转为狐度
    	    $radLat1=deg2rad($this->ap_latitude);//deg2rad()函数将角度转换为弧度
    	    $radLat2=deg2rad($this->estimation_latitude);
    	    $radLng1=deg2rad($this->ap_longtitude);
     	    $radLng2=deg2rad($this->estimation_longtitude);
    	    $a=$radLat1-$radLat2;
    	    $b=$radLng1-$radLng2;
    	    $s=2*asin(sqrt(pow(sin($a/2),2)+cos($radLat1)*cos($radLat2)*pow(sin($b/2),2)))*6378.137*1000;
    	    return $s;
	}
}
?>
