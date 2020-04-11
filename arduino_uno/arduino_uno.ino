
// Hardware: NodeMCU,DHT11
//DHT11 pin 1 (leftmost) in esp's Vin
//DHT11 pin 2 to arduino
//DHT11 pin 3 not connected
//DHT11 pin 4 to esp's GND


//AT+CWMODE=2 , =1 for wifi mode # To setp to AP mode
//AT+CWLAP # lists all wifi available 
//AT+CWJAP="<wifi name>","<pw>" # returns WIFI CONNETED \n WIFI GOT IP OK

//!!!!!!  1!!!! Need to upload with tx rx disconnected, connect after 

#include <FlowMeter.h>  // https://github.com/sekdiy/FlowMeter
#include <DHT.h>
//#include <WiFi.h>
//#include <ESP8266WiFi.h>

#define SSID "Allt bra"      // "SSID-WiFiname" 
#define PASS "Sweden1!"       // "password"
String msg = "GET /update?api_key=I4LFBVYUOOIXT9IB"; //change it with your key...
String apiKey="I4LFBVYUOOIXT9IB";
#define host "api.thingspeak.com";

#define DHTPIN 2       //On NodeMCU, this works with dht11 plugged into D4

DHT dht(DHTPIN, DHT11);

// connect a flow meter to an interrupt pin (see notes on your Arduino model for pin numbers)
FlowMeter Meter = FlowMeter(4);// pin 16 (D0)  on nodeMCU

// set the measurement update period to 1s (1000 ms)
const unsigned long period = 1000;

float t;
int h;
float flow_rate;
float total_vol;
String tempC;
int error;
int is_connected=0;
void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(10);
  
  while (is_connected==0){
    Serial.println("AT");
    delay(5000);
    if(Serial.find("OK")){
        is_connected = connectWiFi();
        if(is_connected==0)
        {
          Serial.println("Couldnt connect, trying again...");
        }
        else{
          Serial.println("Connected.");
        }
      }
  }
  Meter.reset();
}

void loop(){
    start:
    error=0;
  delay(2000);
  h = dht.readHumidity();
  delay(2000);
  t = dht.readTemperature();
  
  if (isnan(h) || isnan(t))
  {
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Temp: ");
    Serial.println(t);
    Serial.println("Failed to read from DHT sensor!");
    goto start;
  }
  else{
    Serial.println("Dht Reading all good.");
     sendtemp();
      
  }

  //Resend if transmission is not completed 
  if (error==1){
    Serial.println("gotostart");
    goto start; //go to label "start"
  }

  Meter.tick(period);
  flow_rate = Meter.getCurrentFlowrate();
   total_vol = Meter.getTotalVolume();
  Serial.print("flow: ");
  Serial.println(flow_rate);
  Serial.print("total: ");
  Serial.println(total_vol);

  
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Waiting not to overload thingspeak...");
  Serial.println("-------------------------------------");
  Serial.println();

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}

void sendtemp(){
  String postStr = "AT+CIPSTART=\"TCP\",\"";
  postStr+= host;
  postStr += "\",80";
  Serial.println(postStr);
  delay(2000);
  if(Serial.find("Error")){
    Serial.println("ERROR ERROR ERROR");
    return;
  }
    postStr = msg;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(flow_rate);
    postStr += "&field4=";
    postStr += String(total_vol);
    postStr += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(postStr.length());
  if(Serial.find(">")){
    Serial.print(postStr);
  }
  else{
    Serial.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
}

boolean connectWiFi(){
  Serial.println("AT+CWMODE=1"); // sets to wifi search (2 is AP)
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd); // connects to wifi with pw
  delay(5000);
  if(Serial.find("OK")){
    return true;
  }else{
    return false;
  }
}

void MeterISR() { // useless for now, allows to use swtich to turn on and off the meter
    // let our flow meter count the pulses
    Meter.count();
}
