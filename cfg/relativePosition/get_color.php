<?php
require_once './db_operation/db_operate.php';
$db = new db_operate();
if(isset($_GET["data"])) {
    echo $db->query_color_by_id($_GET["data"]);
} else {
    echo "can't get color";
}
?>
