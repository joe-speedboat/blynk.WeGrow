//*************************************************************
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <DHT.h>
#include <BlynkSimpleEsp8266.h>
#include <math.h>

/*
#define LED_BUILTIN 2
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;
 */


// ---------- WEMOS D1 MINI PINS ----------
// A0 ->  moisture sensor -> analog read
// D0 GPIO16 ->  digital write   ->  pull up for moisture sensor
// D1 GPIO5  ->  I2C             ->  DHT sensor
// D2 GPIO4  ->  
// D3 GPIO0  ->
// D4 GPIO2  ->  
// D5 GPIO14 ->  digital write   ->  Relais 4 -> Watering
// D6 GPIO12 ->  digital write   ->  Relais 3  -> Fan
// D7 GPIO13 ->  digital write   ->  Relais 2  -> Lights
// D8 GPIO15 ->  digital write   ->  Relais 1  -> 

// -------------- VIRTUAL IO ---------
#define LIGHT_LED_PIN    V1
#define FAN_LED_PIN      V2
#define AIR_TEMP_VPIN    V4
#define AIR_HUM_PIN      V5
#define SOIL_PIN         V6
#define FAN_PIN          12      
#define LIGHT_PIN        13
#define WATER_PIN        14    
#define SPARE_PIN        15
#define SOIL_PULL_UP_PIN 16

WidgetLED light(LIGHT_LED_PIN);
WidgetLED fan(FAN_LED_PIN);
boolean isFirstConnect = true;
int FAN_PIN_STATE;
int LIGHT_PIN_STATE;

#define DHTPIN 5         // What digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Set Blynk auth token here
char auth[] = "-set-token-here";

// Your WiFi credentials. Set password to "" for open networks.
char ssid[] = "IOT";
char pass[] = "GetITNow";

// ************************************ CUSTOM FUNCTIONS ********************************

BLYNK_CONNECTED() {
 // if (isFirstConnect) {
    delay(1000); 
    Blynk.syncAll();
    Blynk.notify("DEVICE STARTING !!!");
    isFirstConnect = false;
//  }
}

void sendAirSensor()
{
  float h = round(dht.readHumidity());
  float t = round(dht.readTemperature()); // or dht.readTemperature(true) for Fahrenheit
  if (isnan(h) || isnan(t) || t > 80  || t < 2 || h > 99 || h < 2) {
    Blynk.notify("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(AIR_HUM_PIN, h);
  Blynk.virtualWrite(AIR_TEMP_VPIN, t);
}

void sendSoilSensor()
{
  // DO NOT FORGET to add a pull up for soil testing to avoid electrolytic reactions between electrodes
  digitalWrite(SOIL_PULL_UP_PIN, HIGH);
  delay(150);  
  int s = analogRead(A0) / 10;
  if (isnan(s)) {
    Blynk.notify("Failed to read from Analog sensor!");
    return;
  }
  Blynk.virtualWrite(SOIL_PIN, s);
}

void updateStatusLed(){
  if (digitalRead(FAN_PIN) == LOW) {  // FAN IS ON - PIN STATE HIGH
   if (FAN_PIN_STATE != LOW) {
     FAN_PIN_STATE = LOW;
     fan.on();
    }
  }else{ // FAN IS OFF - PIN STATE HIGH
   if (FAN_PIN_STATE != HIGH) {
     FAN_PIN_STATE = HIGH;
     fan.off();
    }
  }

 if (digitalRead(LIGHT_PIN) == LOW) {  // LIGHT IS ON - PIN STATE HIGH
   if (LIGHT_PIN_STATE != LOW) {
     LIGHT_PIN_STATE = LOW;
     light.on();
    }
  }else{ // LIGHT IS OFF - PIN STATE HIGH
   if (LIGHT_PIN_STATE != HIGH) {
     LIGHT_PIN_STATE = HIGH;
     light.off();
    }
  }
}


// ************************************ MAIN FUNCTIONS ********************************

void setup()
{
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // pull up all used IOs to prevent undefined state on relais
  pinMode(SOIL_PULL_UP_PIN, OUTPUT);    
  pinMode(FAN_PIN, OUTPUT);    
  pinMode(WATER_PIN, OUTPUT);    
  pinMode(LIGHT_PIN, OUTPUT);    
  pinMode(SPARE_PIN, OUTPUT);   
  digitalWrite(FAN_PIN, HIGH);
  digitalWrite(WATER_PIN, HIGH);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(SPARE_PIN, HIGH);

  dht.begin(); // initialize dht one wire bus
  
  timer.setInterval( 60000L, sendAirSensor);    // push dht sensor data to web
  timer.setInterval( 120000L, sendSoilSensor);  // push soil hum data to web
  timer.setInterval(  1000L, updateStatusLed);  // align blink widgets to physical state
}

void loop()
{
  Blynk.run();
  timer.run();
}

