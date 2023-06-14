#include <Arduino.h>
#include <Math.h>
#include "DHT.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "config.h"

#define DHTTYPE DHT22

// DHT Sensor
int DHTPin = 13; // d7
DHT dht(DHTPin, DHTTYPE);

float Temperature = 0;
float Humidity = 0;

int count = 0;
int countForClient = 0;

// 数码管位引脚
int disp1 = 5;
int disp2 = 4;
int disp3 = 0;
int disp4 = 2;

// 595 连接引脚
int dataPin = 14;  // d5
int latchPin = 12; // d6
int clockPin = 15; //d8

int displayDelay = 2;

const char* ssid = SSID;
const char* password = PASS;

ESP8266WebServer server(PORT); //SEREVER ON PORT 8663

// Define the bit-patterns for the 7-segment displays
const byte SevenSeg[17] =
{
  // Hex digits
  B11111100, B01100000, B11011010, B11110010,   // 0123
  B01100110, B10110110, B10111110, B11100000,   // 4567
  B11111110, B11110110, B11101110, B00111110,   // 89AB
  B00011010, B01111010, B10011110, B10001110,   // CDEF
  B11000110 // 摄氏度
};

bool showTemperature = true;

void metrics() {  
//  float temperature = dht.readTemperature();
//  float humidity = dht.readHumidity();

  String message;
  message += F("dhtexp_temperature "); 
  message += String(Temperature); 
  message += F("\ndhtexp_humidity ");
  message += String(Humidity); 
  
  server.send(200, "text/plain", message);
}

void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Get all data at /metrics"));
    });
    server.on(F("/metrics"), HTTP_GET, metrics);
}

// 连接wifi
void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

  restServerRouting();
  server.begin();
  Serial.println("HTTP server started");
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(disp1, OUTPUT);
  pinMode(disp2, OUTPUT);
  pinMode(disp3, OUTPUT);
  pinMode(disp4, OUTPUT);

  // delay a little and then set
  delay(100);

  pinMode(DHTPin, INPUT);
  dht.begin();

  initWifi();
}


void AllDispOff() {
  // Turn all cathode drivers off
  digitalWrite(disp1, LOW);  // thousands off
  digitalWrite(disp2, LOW);  // hundreds off
  digitalWrite(disp3, LOW);  // tens off
  digitalWrite(disp4, LOW);  // units off
}

void DisplayADigit(int dispno, byte digit2disp, boolean decimalPoint) {

  // Turn off the shift register pins
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);

  AllDispOff();  // Turn off all cathode drivers.

  digit2disp = digit2disp ^ B11111111;

  // 增加小数点
  if (decimalPoint)
    digit2disp &= 0xFE;

  // shift the bits out:
  shiftOut(dataPin, clockPin, LSBFIRST, digit2disp);

  digitalWrite(latchPin, HIGH);  // Set latch high to set segments.
  digitalWrite(dispno, HIGH);  // Drive one cathode low to turn on display.

  delay(displayDelay);  // Wait a bit for POV
}

void SevenSegDisplay(float value)
{
  int d1, d2, d3, d4;
  if (value >= 100)
    value = 99.9;  // Do some bounds checks to avoid strangeness
  if (value < 0)
    value = 0;

  d1 = (int) (value / 10);    // 十位
  DisplayADigit(disp1, byte(SevenSeg[d1]), false); // 显示十位

  d2 = (int)(value - d1 * 10); // 个位
  DisplayADigit(disp2, byte(SevenSeg[d2]), true); // 显示个位

  d3 = (int)((value - d1 * 10 - d2) * 10); // 小数位
  DisplayADigit(disp3, byte(SevenSeg[d3]), false); // 显示小数位

  if (showTemperature) {
    d4 = 16; // 摄氏度
    DisplayADigit(disp4, byte(SevenSeg[d4]), false); // 显示摄氏度
  }

}

//http://digistump.com/wiki/oak/tutorials/particle-variable
void switchTest () {
  // 读取A0的输入值
  int adcvalue = 0;
  adcvalue = analogRead(A0);

  if (adcvalue < 1024 / 2) {
    showTemperature = !showTemperature;
  }
}

//获取温度湿度数据
void getData() {
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
}

// 显示数据
void showData(float t,float h)
{
  if (showTemperature) {
    SevenSegDisplay(t);
  } else {
    SevenSegDisplay(h);
  }
}

int getMsPerCount(int ms) {
  int msStep = displayDelay * 4;
  float v = ms / msStep;
  return ceil(v);
}

void clearCount() {
  if (count > 30000) {
    count = 0;
  }
}

void handleClient(){
  if(countForClient%200==0){
    server.handleClient();
  }
  countForClient++;
}

void loop()
{
  float t = Temperature, h = Humidity;

  if (count % getMsPerCount(500) == 0) {
    switchTest ();
  }

  if (count % getMsPerCount(1000 * 10) == 0) {
    getData();
    t = Temperature;
    h = Humidity;
  }

  showData(t,h);

  clearCount();
  count++;

  handleClient();
}
