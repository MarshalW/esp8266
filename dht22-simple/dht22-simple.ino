#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <DHT.h>

#define DATAPIN 4 // DATA Pin
#define DHTTYPE DHT22 // SENSOR DHT11/DHT22
DHT dht(DATAPIN, DHTTYPE);

float temperature;
float humidity;

unsigned long lastSend;
  
 
void setup(void) {
  Serial.begin(115200);  
  Serial.setTimeout(2000);

  lastSend = 0;

  // Wait for serial to initialize.
  while(!Serial) { }
  
  dht.begin();
}

void getData(){
  temperature = dht.readTemperature(); // Gets the values of the temperature
  humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }else{
    Serial.println(temperature);
    Serial.println(humidity);
  }
}
 
void loop(void) {
  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getData();
    lastSend = millis();
  }
}
