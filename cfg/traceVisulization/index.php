<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>wifi-module</title>
    <meta name="description" content="indoor_location_module" />
    <meta name="keywords" content="indoor location" />
    <meta name="author" content="ZJ SUTD" />
    <link rel="shortcut icon" href="./images/tmp.png">
    <!-- 全局样式 CSS文件 -->
    <link rel="stylesheet" href="./css/normalize.css" />
    <!-- 用户样式 CSS文件 -->
    <link rel="stylesheet" href="./css/wifi-module.css" />
    <!-- jQuery文件 -->
    <script  type="text/javascript" src="./js/jquery-2.1.3.min.js"></script>
</head>
<body>
<div>
    <!--<div class="mouseMoveParaShow">
  Mouse PX:
  <input id="mouse_pointx">
  Mouse PY:
  <input id="mouse_pointy">
  <div>Wifi Signature:<span id="finger_printA1"></span></div>
</div>
<div>
    ApGroup<input type="text" name="apGroupIdAdjustModel" id="apGroupIdAdjustModel" value=""/>
    <input type="button" name="button" value="Adjust" onClick="adjustModel()" />
</div>
<div class="trace">
    <input type="button" name="button" value="TRACE" onClick="trace()"/>
</div> -->
</div>

<div id="my_painter">
    <canvas id="myCanvas" width="1845" height="1537"
            style="border:5px solid #c3c3c3"></canvas>
</div>
<!--canvas JavaScript文件-->
<script type="text/javascript" src="./js/wifi-module.js"></script>
<script type="text/javascript" src="./js/mapurl.js"></script>
<script type="text/javascript" src="./js/wifiTraceQuery.js"></script>
</body>
</html>
