#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

const char* ssid = wifi_ssid;
const char* password = wifi_password;

WiFiServer server(1337);

void setup(){
    Serial.begin(9600);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    server.begin(1337);
}

void loop(){
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
    client.println("<body>");
    client.println("<h1>Garage Controller</h1>");

    // Parse the request
    if (garagedoor != -1) {
      String value = request.substring(17, 18);
      // client.println(value);
      if (value == "1") {
        client.println("<h3>Garage Door 1</h3>");
        // Set D13 low
        digitalWrite(13, LOW);
        // Flash the LED
        for (int i = 0; i < 5; i++) {
          digitalWrite(2, HIGH);
          delay(500);
          digitalWrite(2, LOW);
          delay(500);
        }
        digitalWrite(13, HIGH);
      } else if (value == "2") {
        client.println("<h3>Garage Door 2</h3>");
        // Set D14 low
        digitalWrite(14, LOW);
        // Flash the LED
        for (int i = 0; i < 5; i++) {
          digitalWrite(2, HIGH);
          delay(500);
          digitalWrite(2, LOW);
          delay(500);
        }
        digitalWrite(14, HIGH);
      }
    }
    client.println("</body>");
    // Close the connection
    client.stop();

    Serial.println("Client disconnected!");
  }
}