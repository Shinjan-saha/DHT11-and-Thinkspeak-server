#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"


#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define DHTPIN 0     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


String apiWritekey = "your write key"; 
const char* ssid = "Airtel_9883015426";
const char* password = "RSSR@110323";
const char* server = "api.thingspeak.com";
WiFiClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();

  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("NodeMCU connected to wifi: ");
  Serial.println(ssid);
  Serial.println();
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hif = dht.computeHeatIndex(t, h, false);

  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C\t");
  Serial.print("Heat index: ");
  Serial.print(hif);
  Serial.println(" °C");

  // Display data on OLED
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);     
  display.println("DHT11 Sensor");
  
  display.setCursor(0, 10);
  display.print("Humidity: ");
  display.print(h);
  display.println(" %");

  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(t);
  display.println(" C");

  display.setCursor(0, 30);
  display.print("Heat Index: ");
  display.print(hif);
  display.println(" C");

  display.display();

  
  if (client.connect(server,80)) {
    String tsData = "api_key=" + apiWritekey;
    tsData +="&field1=" + String(t);  // Temperature to Field 1
    tsData +="&field2=" + String(h);  // Humidity to Field 2
    tsData += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiWritekey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" and Humidity: ");
    Serial.print(h);
    Serial.println(" uploaded to ThingSpeak server...");
  }
  client.stop();

  Serial.println("Waiting to upload next reading...");
  delay(15000); // Wait 15 seconds to upload next reading
}
