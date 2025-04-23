/**
 * Apply and receive estimate location of user from server and draw the user
 * path
 * 
 * @author jun_zhao
 */
var firstPointFlag_forTrack = false;
var firstPointFlag_forUsercolor = true;
var firstPointFlag_forMap = true;

var currentresultsarr = new Array();
var lastresultsarr = new Array();

var idNum;

var user = new Object();

//trace();

setTimeout('trace()',3000);

function trace() {
	console.log("send trace query!");
	traceAsynReq();
	setTimeout('trace()', 500);
}
/**
 * send asynchronous query to the servlet.for trace users location based on the
 * source signal data.
 * 
 */
function traceAsynReq() {
	var xmlhttp;
	xmlhttp = new XMLHttpRequest();

	xmlhttp.open("POST",
			"./point_trace_show.php?t="+Math.random(), true);
	xmlhttp.setRequestHeader("Content-Type",
			"application/x-www-form-urlencoded;charset=UTF-8");
	xmlhttp
			.send("FirstSelect=" + firstPointFlag_forTrack + "&idCount="
					+ idNum);

	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var results = xmlhttp.responseText;
			console.log(results);
			idNum = parseResponse(results);
			selectMap();
		        draw();
		}
	}
}

/**
 * parse response data from server init user color and map
 */
function parseResponse(results) {
	var responseArray = new Array();

	var crlf = ";";
	var resultarr = results.split(crlf);
	for (var i = 0; i < resultarr.length; i++) {
		responseArray = resultarr[i].split(" ");
		var result = new Object();// ////weird!!!
		result.username = responseArray[0];
		result.mapid = responseArray[1];
		result.pointx = responseArray[2];
		result.pointy = responseArray[3];
		result.mapinfo = responseArray[4];
		result.idCount = responseArray[5];
		result.color = responseArray[6];

		var mapinfoarr = result.mapinfo.split("_");

		if (firstPointFlag_forMap) {
			initMap(mapinfoarr);
			draw_module();
			firstPointFlag_forMap = false;
		}
                setmapurl(result.mapid);
		if (firstPointFlag_forUsercolor) {
			setmapurl(result.mapid);// init map
		}
		var alreadyexist = false;
		for (var j = 0; j < currentresultsarr.length; j++) {
			if (responseArray[0] == currentresultsarr[j].username) {
				currentresultsarr[j] = result;
				alreadyexist = true;
			}
		}
		if (alreadyexist == false) {// new user
			currentresultsarr.push(result);
		}
	}
	firstPointFlag_forUsercolor = false;
	return result.idCount;
}

function initMap(mapinfoarr) {
	length = mapinfoarr[0];
	width = mapinfoarr[1];
        console.log("width:"+width);
        console.log("height:"+length);

	var pointx, pointy;
	var ispointx = true;
	for (var i = 2; i < mapinfoarr.length; i++) {
		if (ispointx) {
			pointx = mapinfoarr[i];
			ispointx = false;
		} else {
			pointy = mapinfoarr[i];
			var sspoint = new Object();
			sspoint.pointx = parseInt(pointx);
			sspoint.pointy = parseInt(pointy);
			baseArray.push(sspoint);
			ispointx = true;
		}
	}
}

function selectMap() {
	var image = new Image();
	image.src = getmapurl();
        console.log("image-src:"+image.src);
	image.onload = function() {
		ctx.drawImage(image, rec_x, rec_y,1250,830);
	};
	ctx.drawImage(image, rec_x, rec_y,1250,830);
}

function draw() {
	if (!firstPointFlag_forTrack) {
		firstPointFlag_forTrack = true;
		lastresultsarr = currentresultsarr.concat();// deep copy
	}

	lastresultsarr = currentresultsarr.concat();

	for (var i = 0; i < currentresultsarr.length; i++) {
		var point = new Object();
		point.pointx = currentresultsarr[i].pointx;
		point.pointy = currentresultsarr[i].pointy;
		var color = currentresultsarr[i].color;
		console.log(color);
		if (point.pointx == 0 && point.pointy==0)
                {} else {
		    drawResultPoint(point, color);
                }
	}
}

function getRandomColor() {
	return "#"
			+ ("00000" + ((Math.random() * 16777215 + 0.5) >> 0).toString(16))
					.slice(-6);
}

/**
 * Get the module point data from the server here and draw the point on canvas
 */
function drawResultPoint(point, color) {
	console.log("color = " + color);
	var currentpointx = parseInt(point.pointx) + rec_x;
	var currentpointy = parseInt(point.pointy) + rec_y;
	ctx.beginPath();
	ctx.arc(currentpointx, currentpointy, resultPointRadius, 0, Math.PI * 2,
			false);
	ctx.fillStyle = color;
	ctx.globalAlpha = 1;
	ctx.fill();
}
