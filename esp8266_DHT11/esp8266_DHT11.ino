

#include <DHT.h>

// Robo India Tutorial
// Simple code upload the tempeature and humidity data using thingspeak.com
// Hardware: NodeMCU,DHT11
//DHT11 pin 1 (leftmost) in esp's Vin
//DHT11 pin 2 to esp's D3
//DHT11 pin 3 not connected
//DHT11 pin 4 to esp's GND

#include <ESP8266WiFi.h>

String apiKey = "I4LFBVYUOOIXT9IB";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "Allt bra";     // replace with your wifi ssid and wpa2 key
const char *pass =  "Sweden1!";
const char* server = "api.thingspeak.com";

#define DHTPIN 2       //On NodeMCU, this works with dht11 plugged into D4

DHT dht(DHTPIN, DHT11);

#include <FlowMeter.h>  // https://github.com/sekdiy/FlowMeter

// connect a flow meter to an interrupt pin (see notes on your Arduino model for pin numbers)
FlowMeter Meter = FlowMeter(16);// pin 16 (D0) 

// set the measurement update period to 1s (1000 ms)
const unsigned long period = 1000;


WiFiClient client;
void setup()
{
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Meter.reset();
}

void loop()
{
  delay(2000);
  float h = dht.readHumidity();
  delay(2000);
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println(h);
    Serial.println(t);
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
  }
  client.stop();

  Meter.tick(period);
   Serial.println("Currently " + String(Meter.getCurrentFlowrate()) + " l/min, " + String(Meter.getTotalVolume())+ " l total.");

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}
