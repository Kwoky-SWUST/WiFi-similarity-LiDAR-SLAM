<?php

/**
 * Created by PhpStorm.
 * User: jiao
 * Date: 6/1/16
 * Time: 5:12 PM
 */
class enlarge_operate
{
    public $enlarge = 50;

    public function deacidize_int_float($pointxy) {
        $pointxy_real = ($pointxy);
        return $pointxy_real;
    }

    public function getEnlargePointsString($pointx,$pointy) {
//	echo $pointx . " " . $pointy;
        $point_x = $pointx*50;
        $point_y = $pointy*50;
        $result = $point_x . " " .$point_y;
//	echo "result" . $result;
        return $result;
    }
}
