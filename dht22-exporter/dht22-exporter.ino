#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#include "config.h"

#define DATAPIN 4 // DATA Pin 注意，0和2是都碰到了兼容性问题
#define DHTTYPE DHT22 // SENSOR DHT11/DHT22
DHT dht(DATAPIN, DHTTYPE);

float temperature;
float humidity;

unsigned long lastSend;
 
ESP8266WebServer server(PORT);

void metrics() {  
  String message;
  message += F("dhtexp_temperature "); 
  message += String(temperature); 
  message += F("\ndhtexp_humidity ");
  message += String(humidity); 
  
  server.send(200, "text/plain", message);
}
 
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Get all data at /metrics"));
    });
    server.on(F("/metrics"), HTTP_GET, metrics);
}
 
 
void setup(void) {
  Serial.begin(115200);  
  Serial.setTimeout(2000);

  lastSend = 0;

  // Wait for serial to initialize.
  while(!Serial) { }
  
  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  Serial.println("");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  
  restServerRouting();
  server.begin();
  Serial.println("HTTP server started");

}

void getData(){
  temperature = dht.readTemperature(); // Gets the values of the temperature
  humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}
 
void loop(void) {
  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getData();
    lastSend = millis();
  }
  server.handleClient();
}
