/**
 * 
 */

// JavaScript Document
var length, width;

var rec_x = 300, rec_y = 100;//initial rectangle dot
/**
 * we maintenance a dynamic base set here
 */	
var base_radius = 7;

var baseArray = new Array();//Actual baseArray.

var resultPointRadius = 5;


var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");

/**
 * Point convert function
 */	
function recpx_convertto_docpx(pointx){
	return (pointx + rec_x);
}
function recpy_convertto_docpy(pointy){
	return (pointy + rec_y);
}

/**
 * Draw module
 */	
function draw_module(){
ctx.clearRect(0, 0, canvas.width, canvas.height);

/**
* --------------------------------------------------------------------
* This section is for drawing rectangle and the bases
* --------------------------------------------------------------------
*/
ctx.fillStyle="#0000ff";
ctx.strokeRect(rec_x, rec_y, length, width);

//draw bases
for(i = 0; i < baseArray.length; i ++){
	console.log("basepointx = "  + baseArray[i].pointx);
	console.log("basepointy = "  + baseArray[i].pointy);
	var basepointx = recpx_convertto_docpx(baseArray[i].pointx);
	var basepointy = recpy_convertto_docpy(baseArray[i].pointy);
	ctx.beginPath(); 
	ctx.arc(basepointx, basepointy, base_radius, 0, Math.PI*2, false); 
	ctx.fillStyle = "#000"; 
	ctx.fill();
}
}
