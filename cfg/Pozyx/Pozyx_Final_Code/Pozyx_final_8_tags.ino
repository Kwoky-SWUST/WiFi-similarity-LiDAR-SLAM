#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>

device_range_t range,testData[15];
uint32_t printData[15];
uint8_t error[8],data[5];
long int starttime = 0;
uint16_t My_ID;
char* My_Id_Name; 
int errorbit[8],tme[8];

int status,i,reset=0,error_no=0,tme_error;

  int No_of_Anchors = 8;
  uint16_t destination_id[8]={0x6045,0x6057,0x6029,0x607C,0x600F,0x606C,0x6069,0x6008};
  
struct Destination{
  char *ds;
}D[8];

void setup(){
  Serial.begin(115200);  
    
  if(Pozyx.begin() == POZYX_FAILURE){delay(100); Pozyx.resetSystem(); } 
 
  randomSeed(analogRead(0));  
  while ( Pozyx.getNetworkId(&My_ID)!=1 ) delay(random(5,10)*66);

  if( My_ID == 0x6054 )  My_Id_Name = "User01"; 
  if( My_ID == 0x6046 )  My_Id_Name = "User02";
  if( My_ID == 0x6032 )  My_Id_Name = "User03";
  if( My_ID == 0x605D )  My_Id_Name = "User04";
  if( My_ID == 0x6021 )  My_Id_Name = "User05";
  if( My_ID == 0x605E )  My_Id_Name = "User06";
  if( My_ID == 0x607F )  My_Id_Name = "User07";
  if( My_ID == 0x605F )  My_Id_Name = "User08";
  
  D[0].ds ="User01"; D[1].ds ="User02"; D[2].ds ="User03"; D[3].ds ="User04"; D[4].ds ="User05"; D[5].ds="User06"; 
  D[6].ds="User07"; D[7].ds="User08"; 

  uint8_t configuration_leds = 0x0;    
  Pozyx.regWrite(POZYX_CONFIG_LEDS, &configuration_leds, 1);
}

void loop(){

 tme_error=0;
   
 for(i=0; i<No_of_Anchors;i++){ 
       
    status = 1;
    data[4] = 0;
    
    starttime = millis();
    status &= Pozyx.doRanging(destination_id[i], &range);
    tme[i] = millis() - starttime;  
     
    delay(random(5,15)*20); 
    if( tme[i] >60 ) tme_error++; 
         
    Pozyx.regRead(POZYX_WHO_AM_I, data, 7); 
    error[i] = data[4];

    if( data[4]!=0 ) reset++; else if(reset<3) reset=0;
    
    errorbit[i]=0;    

         
    if (status == POZYX_SUCCESS  && range.distance < 35000 && testData[i].timestamp != range.timestamp) printData[i]=range.distance; 
    else  { error_no++;  errorbit[i]=1; }      
    testData[i].timestamp = range.timestamp;
    
 }
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
      Serial.print("GET /server_testing/insertIRPSPozyxRangeData.php?data=");  
    
      Serial.print(My_Id_Name); Serial.print(",");   
      Serial.print(My_ID,HEX);  Serial.print(","); 
      
      Serial.print(No_of_Anchors); Serial.print(";"); 
     
       for(i=0;i<No_of_Anchors;i++){
        Serial.print(D[i].ds); Serial.print(";");       
        if( errorbit[i]==0 ) Serial.print(printData[i]); else Serial.print("-1"); 
        Serial.print(";"); 
      } Serial.print(","); 

      Serial.print(millis());
      Serial.print(","); 
      
      Serial.println(" HTTP/1.1");
      //Serial.println("Host: 202.94.70.33");   // ip address of the public server
      Serial.println("Host: 10.42.0.1");   // ip address of the openmesh server
      Serial.println("Content-Type: application/x-www-form-urlencoded");
      Serial.println("Connection: close");
      Serial.println("");
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if ( reset>=3 || tme_error>=3 || error_no>=18)  { delay(1500); Pozyx.resetSystem(); }
      if( error_no%8!=0 ) error_no=0; 
} 
