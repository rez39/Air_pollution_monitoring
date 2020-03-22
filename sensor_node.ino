
#ifdef ESP32
#error ESP32 does not work with SoftSerial, use HardwareSerial example instead
#endif

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "secrets.h"
#include <SDS011.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "MutichannelGasSensor.h"

#define SENSOR_ADDR     0X04        // default to 0x04
#define PRE_HEAT_TIME   5

#define ONE_WIRE_BUS D4
#include "DHT.h"
#define DHTPIN D3

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

OneWire oneWire(ONE_WIRE_BUS);
#define DHTTYPE DHT11
DallasTemperature sensors(&oneWire);
float p10, p25;
int error;
double temp;
SDS011 my_sds;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  my_sds.begin(D1, D2); //RX, TX
  sensors.begin();
  Serial.begin(115200);  // Initialize serial
  WiFi.mode(WIFI_STA);
  dht.begin();
  Wire.begin(D5,D6);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  Serial.begin(115200);
  gas.begin(SENSOR_ADDR);   //
    for(int i=60*PRE_HEAT_TIME; i>=0; i--)
    {
        Serial.print(i/60);
        Serial.print(":");
        Serial.println(i%60);
        delay(1000);
    }
    Serial.println("off the for loop");
}
float h;
float co;
float no2;
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }

    Serial.println("\nConnected.");
  }
  
    Serial.println("\nConnected.");
  h = dht.readHumidity();
  co = gas.measure_CO();
  no2 = gas.measure_NO2();
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp = sensors.getTempCByIndex(0);
  error = my_sds.read(&p25, &p10);
  if (!error) {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
    Serial.println(temp);
    Serial.println(h);
    ThingSpeak.setField(1, (float)temp);
    ThingSpeak.setField(2, (float)h);
    ThingSpeak.setField(3, (float)p25);
    ThingSpeak.setField(4, (float)p10);
    ThingSpeak.setField(5, (float)co);
    ThingSpeak.setField(6, (float)no2);
    delay(1000);
    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
  else{ 
  Serial.println("error");
  }
  delay(30000);
}
