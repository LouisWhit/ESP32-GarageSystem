#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include "secrets.h"

#define led 2
#define garagedoor1 13
#define garagedoor2 14
// #define testpin 13
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
    // pinMode(testpin, OUTPUT);
    // These pinMode OUTPUTs send voltage to a 2n2222 transistor which then sends voltage to the garage door button. See example explainers: https://www.youtube.com/watch?v=IAyGYiBQExk and https://www.youtube.com/watch?v=6bSFkKDxA18
    // delay(1000);
    
    dht.begin();

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    // Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        // Serial.print(".");
    }

    // Serial.println("\nConnected to the WiFi network");
    // Serial.println(WiFi.localIP());

    server.begin(port);
}

void loop(){
  digitalWrite(led, LOW);
  digitalWrite(garagedoor1, LOW);
  digitalWrite(garagedoor2, LOW);
  // digitalWrite(testpin, HIGH);
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
    webpage += "<!DOCTYPE HTML><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta http-equiv='X-UA-Compatible' content='ie=edge'><title>Garage Controller</title><link rel='shortcut icon' href='data:image/x-icon;,' type='image/x-icon'><style>body{font-family: Arial, sans-serif;color:#fff;background-color: #330000;background-image: url(\"data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='100%25' height='100%25' viewBox='0 0 800 400'%3E%3Cdefs%3E%3CradialGradient id='a' cx='396' cy='281' r='514' gradientUnits='userSpaceOnUse'%3E%3Cstop offset='0' stop-color='%23D18'/%3E%3Cstop offset='1' stop-color='%23330000'/%3E%3C/radialGradient%3E%3ClinearGradient id='b' gradientUnits='userSpaceOnUse' x1='400' y1='148' x2='400' y2='333'%3E%3Cstop offset='0' stop-color='%23FA3' stop-opacity='0'/%3E%3Cstop offset='1' stop-color='%23FA3' stop-opacity='0.5'/%3E%3C/linearGradient%3E%3C/defs%3E%3Crect fill='url(%23a)' width='800' height='400'/%3E%3Cg fill-opacity='0.4'%3E%3Ccircle fill='url(%23b)' cx='267.5' cy='61' r='300'/%3E%3Ccircle fill='url(%23b)' cx='532.5' cy='61' r='300'/%3E%3Ccircle fill='url(%23b)' cx='400' cy='30' r='300'/%3E%3C/g%3E%3C/svg%3E\");background-attachment: fixed;background-size: cover;}h1,h2,h3,h4,h5,h6{color:#fff;}#termometer{width:25px;background:#38383f;height:240px;position:relative;border:9px solid #2a2a2e;border-radius:20px;z-index:1;margin-bottom:50px}#termometer:after,#termometer:before{position:absolute;content:"";border-radius:50%}#termometer:before{width:100%;height:34px;bottom:9px;background:#38383f;z-index:-1}#termometer:after{transform:translateX(-50%);width:50px;height:50px;background-color:#3dcadf;bottom:-41px;border:9px solid #2a2a2e;z-index:-3;left:50%}#termometer #graduations{height:59%;top:20%;width:50%}#termometer #graduations,#termometer #graduations:before{position:absolute;border-top:2px solid rgba(0,0,0,.5);border-bottom:2px solid rgba(0,0,0,.5)}#termometer #graduations:before{content:"";height:34%;width:100%;top:32%}#termometer #temperature{bottom:0;background:linear-gradient(#f17a65,#3dcadf) bottom no-repeat;width:100%;border-radius:20px;background-size:100% 240px;transition:.2s ease-in-out}#termometer #temperature,#termometer #temperature:after,#termometer #temperature:before{position:absolute}#termometer #temperature:before{content:attr(data-value);background:rgba(0,0,0,.7);color:#fff;z-index:2;padding:5px 10px;border-radius:5px;font-size:1em;line-height:1;transform:translateY(50%);left:calc(-100% + 1em / 1.5);top:calc(-1em + 5px - 5px * 2)}#termometer #temperature:after{content:"";border-top:.4545454545em solid transparent;border-bottom:.4545454545em solid transparent;border-right:.6666666667em solid rgba(0,0,0,.7);left:100%;top:calc(-1em / 2.2 + 5px)}</style></head><body><div style='display:flex; height:100px;justify-content: space-between;'><div class='flex-item'><h1>Garage Controller</h1></div><div class='flex-item' style='justify-content: flex-end;'>";
    webpage += "<div id='termometer'><div id='temperature' style='height:";
    webpage += String(f);
    webpage += "%' data-value='";
    webpage += String(f);
    webpage += "'></div><div id='graduations'></div></div>";
    webpage += "<p style='width:75px; text-align:center;'>Humidity: ";
    webpage += String(h);
    webpage +="</p></div></div><div style='display:flex; height:60px; gap:10px 20px;'><div class='flex-item' style='background-color:white;border-radius:4px;'><a href='/?garagedoor=1' style='display:block;color:black;text-align:center;padding:20px 40px;'>Door 1</a></div><div class='flex-item' style='background-color:white;border-radius:4px;padding:20px 40px;'><a href='/?garagedoor=2' style='color:black;text-align:center;display:block;'>Door 2</a></div>";

    // Parse the request
    if (garagedoor != -1) {
      String value = request.substring(17, 18);
      if (value == "1") {
        webpage += "<div style='display:flex; height:100px;justify-content: space-between;'><div class='flex-item'><h3>Success: Garage Door 1</h3></div>";
        digitalWrite(garagedoor1, HIGH);
      } else if (value == "2") {
        webpage += "<div style='display:flex; height:100px;justify-content: space-between;'><div class='flex-item'><h3>Success: Garage Door 2</h3></div>";
        digitalWrite(garagedoor2, HIGH);
      }
        // Flash the LED
        for (int i = 0; i < 5; i++) {
          digitalWrite(led, HIGH);
          delay(100);
          digitalWrite(led, LOW);
          delay(100);
        }
    }
    webpage += "</body></html>";
    client.println(webpage);
    // Close the connection
    client.stop();

    Serial.println("Client disconnected!");
  }
}