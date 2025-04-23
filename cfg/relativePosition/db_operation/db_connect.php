<?php
/**
 * Created by PhpStorm.
 * User: jiao
 * Date: 6/1/16
 * Time: 3:39 PM
 */
include_once("db_config.php");
$conn = mysql_connect(HOST,USERNAME,PASSWORD);
if (!$conn) {
    die("Could not connect to database, error: " .mysql_errno());
}

mysql_select_db(DATABASE,$conn);
?>