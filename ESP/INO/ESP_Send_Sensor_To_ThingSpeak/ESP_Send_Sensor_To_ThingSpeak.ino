#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire myWire(2); // Set GPIO2 on ESP8266 as the oneWire interface.
DallasTemperature sensors(&myWire);

float TempC_0; //One sensor is applied in this case.
 
// replace with your channel's thingspeak API key,
String apiKey = "your apiKey";                           // Replace this with your api key
const char* ssid = "your ssid";                          // Replace this with your ssid
const char* password = "your password";                  // Replace this with your password
 
const char* server = "api.thingspeak.com";

WiFiClient client;
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
}
 
void loop() {
  Serial.println();
  Serial.println("Requesting temperature from sensor...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  TempC_0=sensors.getTempCByIndex(0);  // Get data from the sensor
 
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }  
  
 
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(TempC_0);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
 
     Serial.print("Temperature: ");
     Serial.print(TempC_0);
     Serial.print(" degrees C");
     Serial.println(" % send to Thingspeak");
  }
  client.stop();
 
  Serial.println("Waiting...");
  // thingspeak needs minimum 15 sec delay between updates
  delay(20000);
}
