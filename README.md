# ESP32-GarageSystem
This is a web application running on an ESP-WROOM-32 board. It could run on many other boards. This is just the one I am using.
When you visit the IP address (or set up an internal domain) of your local ESP/Arduino board. Your browser will be presented with two buttons (or more) to send a signal to your garage door button.
In my case, I have attached the garage door boards themselves to a breadboard. I am then using a 2n2222 transistor to simulate board button presses.
This will allow for control of the garage door from anywhere in WiFi range.
In my case, this is paired with Security cameras, which I use to monitor the current status of the door.
I have also added a DHT11 sensor to this setup so that I can see the temperature in the garage itself.
The application responds to an http request to its specified port. Meaning this could work well with automations/apple shortcuts/node red implementations. Etc.

Feel free to use it as needed.
