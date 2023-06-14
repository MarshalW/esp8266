#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#include "config.h"

#define DATAPIN 2 // DATA Pin
#define DHTTYPE DHT11 // SENSOR DHT11/DHT12
DHT dht(DATAPIN, DHTTYPE);
 
ESP8266WebServer server(PORT);

void metrics() {  
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

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
 
void loop(void) {
  server.handleClient();
}
