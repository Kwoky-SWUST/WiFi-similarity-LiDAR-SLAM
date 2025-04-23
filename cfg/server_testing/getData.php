<?php
$link=mysql_connect("127.0.0.1","root","yuenchau");
mysql_query("SET NAMES utf8");
mysql_select_db("wifitrack",$link);
$sql=mysql_query("select location_x,location_y,color from wifi_indoor order by id desc limit 0,1",$link);
while($row=mysql_fetch_assoc($sql))
	$output[]=$row;
print(json_encode($output));
mysql_close();
?>
