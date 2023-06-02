#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

#define led 2
#define garagedoor1 13
#define garagedoor2 14

const char* ssid = wifi_ssid;
const char* password = wifi_password;

WiFiServer server(1337);

void setup(){
    Serial.begin(9600); 
    pinMode(led, OUTPUT);
    pinMode(garagedoor1, OUTPUT);
    pinMode(garagedoor2, OUTPUT);
    // delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    // Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    // Serial.println("\nConnected to the WiFi network");
    // Serial.println(WiFi.localIP());

    server.begin(1337);
}

void loop(){
  digitalWrite(led, LOW);
  digitalWrite(garagedoor1, LOW);
  digitalWrite(garagedoor2, LOW);
  // Wait for a client to connect
  WiFiClient client = server.available(); 
  
  if (client) {
    Serial.println("Client connected!");

    // Read the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    int garagedoor = request.indexOf("garagedoor");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<link rel='shortcut icon' href='data:image/x-icon;,' type='image/x-icon'> ");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>Garage Controller</h1>");

    // Parse the request
    if (garagedoor != -1) {
      String value = request.substring(17, 18);
      // client.println(value);
      if (value == "1") {
        client.println("<h3>Garage Door 1</h3>");
        digitalWrite(garagedoor1, HIGH);
      } else if (value == "2") {
        client.println("<h3>Garage Door 2</h3>");
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
    client.println("</body>");
    client.println("</html>");
    // Close the connection
    client.stop();

    Serial.println("Client disconnected!");
  }
}