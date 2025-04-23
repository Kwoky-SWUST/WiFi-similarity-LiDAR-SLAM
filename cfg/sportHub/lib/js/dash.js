	 	var daily_time = [];
	 	var daily_imports = [];
	 	
	 	
		
		

	 	function daily(account) {
			    	$.ajax({
			        type: 'GET',
			        url: 'get_total_export_daily.php',
			        data:{'account':account},
			        dataType: 'json',
			        success: function(data) {
			            daily_time = data.device_time;
			            daily_imports = data.value;
			         
			            
			        },
			        async: false
			    });
			};
		


		
	
	var barChartData1 = {
		labels : daily_time,//["Monday","Tuesday","Thursday","Wednsday","Friday","Saturday","Sunday"],
		datasets : [
		{
			fillColor : "rgba(220,220,220,0.5)",
			strokeColor : "rgba(220,220,220,0.8)",
			highlightFill: "rgba(220,220,220,0.75)",
			highlightStroke: "rgba(220,220,220,1)",
			data : daily_imports//[randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor(),randomScalingFactor()]
		},
		
		]

	}
		

