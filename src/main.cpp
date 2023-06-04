#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include "secrets.h"

#define led 2
#define garagedoor1 13
#define garagedoor2 14
#define port 1337
#define serial_baud 9600

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = wifi_ssid;
const char* password = wifi_password;

WiFiServer server(port);

void setup(){
    Serial.begin(serial_baud); 
    pinMode(led, OUTPUT);
    pinMode(garagedoor1, OUTPUT);
    pinMode(garagedoor2, OUTPUT);
    // These pinMode OUTPUTs send voltage to a 2n2222 transistor which then sends voltage to the garage door button. See example explainers: https://www.youtube.com/watch?v=IAyGYiBQExk and https://www.youtube.com/watch?v=6bSFkKDxA18
    // delay(1000);
    
    dht.begin();

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    // Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    // Serial.println("\nConnected to the WiFi network");
    // Serial.println(WiFi.localIP());

    server.begin(port);
}

void loop(){
  digitalWrite(led, LOW);
  digitalWrite(garagedoor1, LOW);
  digitalWrite(garagedoor2, LOW);
  // Wait for a client to connect
  WiFiClient client = server.available(); 
  
  if (client) {
    Serial.println("Client connected!");

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = t * 1.8 + 32;

    // Read the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    int garagedoor = request.indexOf("garagedoor");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println();

    String webpage = "";
    webpage += "<!DOCTYPE HTML><html><head><link rel='shortcut icon' href='data:image/x-icon;,' type='image/x-icon'></head><body><div style='display:flex; height:100px;justify-content: space-between;'><div class='flex-item'><h1>Garage Controller</h1></div><div class='flex-item' style='justify-content: flex-end;'><p>Temp: ";
    webpage += String(f);
    webpage += "</p><p>Humidity: ";
    webpage += String(h);
    webpage +="</p></div></div><div style='display:flex; height:60px; gap:10px 20px;'><div class='flex-item' style='background-color:black;border-radius:4px;'><a href='/garagedoor=1' style='display:block;color:white;text-align:center;padding:20px 40px;'>Door 1</a></div><div class='flex-item' style='background-color:black;border-radius:4px;padding:20px 40px;'><a href='/garagedoor=2' style='color:white;text-align:center;display:block;'>Door 2</a></div>";

    // Parse the request
    if (garagedoor != -1) {
      String value = request.substring(17, 18);
      // client.println(value);
      if (value == "1") {
        webpage += "<h3>Garage Door 1</h3>";
        digitalWrite(garagedoor1, HIGH);
      } else if (value == "2") {
        webpage += "<h3>Garage Door 2</h3>";
        digitalWrite(garagedoor2, HIGH);
      }
        // Flash the LED
        for (int i = 0; i < 5; i++) {
          digitalWrite(led, HIGH);
          delay(500);
          digitalWrite(led, LOW);
          delay(500);
        }
    }
    webpage += "</body></html>";
    client.println(webpage);
    // Close the connection
    client.stop();

    Serial.println("Client disconnected!");
  }
}