#include <Arduino.h>
#if defined(ESP32)  
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Wire.h>
#include <ThingsBoard.h>
#include "device.h"
#include "wifi_id.h"
#include "thingsboard_id.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#define THINGSBOARD_SERVER       "demo.thingsboard.io"

WiFiClient espClient;
ThingsBoard tb(espClient);


void WifiConnect();
void onSendSensor();
void button();

int buzzOn = 0;
int sensorState1 = 0;
int sensorState2 = 0;
int count = 0;
int limit = 5; 
int lastcount = -1;

long lastmilis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ButtonUp, INPUT);
  pinMode(ButtonDown, INPUT);

  //Buzzer Setup
  pinMode(BUZZER_PIN, OUTPUT);
  //Buzzer Setup

  //People counter setup
  pinMode (sensorPin1,
INPUT_PULLUP);
  pinMode (sensorPin2, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  //People counter setup
  
  Serial.println(count);

  WifiConnect();
  if (tb.connect(THINGSBOARD_SERVER, THINGSBOARD_ACCESS_TOKEN))
    Serial.println("Connected to thingsboard");
  else
    Serial.println("Error connected to thingsboard");
  Serial.println("System ready.");
}

void loop() {
  //People counter loop
  sensorState1 = digitalRead(sensorPin1);
  sensorState2 = digitalRead(sensorPin2);

  if(sensorState1 == LOW){
    count++; 
    delay(1000);
  }

  if(sensorState2 == LOW){
    if(count>0)
      count--; 
      delay(1000);
  }
  //People counter loop
  
   if(count<=limit)
  { 
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(relay, HIGH);
    int buzzOn = 0;
  }
  else if (count>0 && count<10){
    digitalWrite(relay, LOW);
    int buzzOn = 1;
    if (buzzOn = 1){
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
    }
  }
  else {
    digitalWrite(relay, LOW);
    int buzzOn = 1;
    if (buzzOn = 1){
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500) ; 
    }
  }

  if ((millis() - lastmilis)>1000 && tb.connected())
  {
    lastmilis = millis();
    onSendSensor();
    button();
    //pot();
  }
    
  tb.loop();
}

void button()
{
  int ButtonStateUp = digitalRead(ButtonUp);
  int ButtonStateDown = digitalRead(ButtonDown);
  if(ButtonStateUp == HIGH)
  {
    limit++;
    Serial.println(limit);
  }
  else if(ButtonStateDown == HIGH)
  {
    limit--;
    Serial.println(limit);
  }
}

void onSendSensor()
{
  tb.sendTelemetryFloat("count", count);
  tb.sendTelemetryFloat("limit", limit);
}

void WifiConnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }  
  Serial.print("System connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
}
