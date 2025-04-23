	 	var time = [];
	 	var v1 = [];
	 	var v2 = [];
	 	var v3 = [];
	 	var l1 = [];
	 	var l2 = [];
	 	var l3 = [];
	 	var i1 = [];
	 	var i2 = [];
	 	var i3 = [];
	 	
	 	function get_readings_with_date(account, meter,fdate){
	 		
				$.ajax({
			        type: 'GET',
			        url: 'get_phase_voltage_with_Date.php',
			        data:{'account':account, 'meter':meter, 'fdate':fdate, 'ldate':ldate},
			        dataType: 'json',
			        success: function(data) {
			        	alert(data.length);
			            time = data.time;
			            
			            v1 = data.v1;
			            v2 = data.v2;
			            v3 = data.v3;
			            l1 = data.l1;
			            l2 = data.l2;
			            l3 = data.l3;
			            i1 = data.i1;
			            i2 = data.i2;
			            i3 = data.i3;
			            
			        },
			        async: false
			    });
	 	};

	 	function format_date(fdate){
	 		 var yyyy = fdate.getFullYear().toString();
   			var mm = (fdate.getMonth()+1).toString(); // getMonth() is zero-based
   			var dd  = fdate.getDate().toString();
   			return yyyy +"-"+ (mm[1]?mm:"0"+mm[0]) + "-"+(dd[1]?dd:"0"+dd[0]) + " 00:00:00";
	 	}
	 	function format_ldate(fdate){
	 		 var yyyy = fdate.getFullYear().toString();
   			var mm = (fdate.getMonth()+1).toString(); // getMonth() is zero-based
   			var dd  = (fdate.getDate()).toString();
   			return yyyy +"-"+ (mm[1]?mm:"0"+mm[0]) + "-"+(dd[1]?dd:"0"+dd[0]) +" 23:59:59";
	 	}

	 	function get_readings(account, meter,fdate,status){
	 		var fdate = new Date(fdate);
	 		var ldate = new Date(fdate);
	 		fdate = format_date(fdate);
	 		ldate = format_ldate(ldate);
	 		if(status==1){
	 			fdate=1;
	 			ldate=1;
	 		}
	 		
	 		$.ajax({
			        type: 'GET',
			        url: 'get_phase_voltage.php',
			        data:{'account':account, 'meter':meter,'fdate':fdate,'ldate':ldate},
			        dataType: 'json',
			        success: function(data) {
			            time = data.time;
			            v1 = data.v1;
			            v2 = data.v2;
			            v3 = data.v3;
			            l1 = data.l1;
			            l2 = data.l2;
			            l3 = data.l3;
			            i1 = data.i1;
			            i2 = data.i2;
			            i3 = data.i3;
			            
			        },
			        async: false
			    });
	 	};

		
	 	function get_meter_readings(account, meter,fdate) {
	 			var status =0;
	 			if(fdate==1){
	 				status =1;
	 			}
	 		
	 			get_readings(account, meter, fdate,status);
	 		
			    	
			
		
		

	var lineChartData1 = {

		labels : time,//["8:00","9:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00","17:00","18:00","19:00","20:00","21:00","22:00","23:00","24:00","1:00","2:00","3:00","4:00","5:00","6:00","7:00"],
		datasets : [
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "red",
			pointHighlightFill : "red",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : v1//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "green",
			pointHighlightFill : "green",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : v2//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "blue",
			pointHighlightFill : "blue",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : v3//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		}
		]

	}

	var lineChartData2 = {

		labels : time,//["8:00","9:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00","17:00","18:00","19:00","20:00","21:00","22:00","23:00","24:00","1:00","2:00","3:00","4:00","5:00","6:00","7:00"],
		datasets : [
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "red",
			pointHighlightFill : "red",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : l1//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "green",
			pointHighlightFill : "green",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : l2//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "blue",
			pointHighlightFill : "blue",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : l3//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		}
		]

	}

	var lineChartData3 = {

		labels : time,//["8:00","9:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00","17:00","18:00","19:00","20:00","21:00","22:00","23:00","24:00","1:00","2:00","3:00","4:00","5:00","6:00","7:00"],
		datasets : [
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "red",
			pointHighlightFill : "red",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : i1//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "green",
			pointHighlightFill : "green",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : i2//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		{
			label: "My First dataset",
			fillColor : "rgba(220,220,220,0.2)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(1220,220,220,1)",
			pointStrokeColor : "blue",
			pointHighlightFill : "blue",
			pointHighlightStroke : "rgba(220,220,220,1)",
			data : i3//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		}
		]

	}
	

	
		var ctx = document.getElementById("canvas").getContext("2d");
		window.myLine = new Chart(ctx).Line(lineChartData1, {
			responsive: true
		});
		var ctx = document.getElementById("canvas1").getContext("2d");
		window.myBar = new Chart(ctx).Line(lineChartData2, {
			responsive : true
		});
		var ctx = document.getElementById("canvas2").getContext("2d");
		window.myLine = new Chart(ctx).Line(lineChartData3, {
			responsive: true
		});
		
		

};	



	






	/*window.onload = function(){

		moneyChoose();
		var ctx = document.getElementById("canvas").getContext("2d");
		window.myLine = new Chart(ctx).Line(lineChartData, {
			responsive: true
		});
		var ctx = document.getElementById("canvas1").getContext("2d");
		window.myBar = new Chart(ctx).Bar(barChartData1, {
			responsive : true
		});
		var ctx = document.getElementById("canvas2").getContext("2d");
		window.myLine = new Chart(ctx).Bar(barChartData2, {
			responsive: true
		});
		var ctx = document.getElementById("canvas3").getContext("2d");
		window.myLine = new Chart(ctx).Bar(barChartData3, {
			responsive: true
		});
		document.getElementById("currpower").innerHTML = Math.round(power*100)/100+" W";
		document.getElementById("totimport").innerHTML = Math.round(totexport*100)/100+" kWh";
		document.getElementById("totcon").innerHTML = Math.round(totimport*100)/100+" kWh";
		document.getElementById("totexport").innerHTML = Math.round(totimport*100)/100+" kWh";
	}*/


