#include <avr/io.h>
#include <SPI.h>
#include <WiFi.h>
#include <util/delay.h>

#include "ArduinoWebSocketClient.h"

char* ssid = "ssid";     
char* pass = "pass";

char* host = "ip_or_hostname";
int port = 80;
char* websocketUri = "/websocket";

void handler(ArduinoWebSocketClient* client, char *data, int size) {
  Serial.print("Received data of size: ");
  Serial.println(size);
  Serial.print("Data: ");
  Serial.println(data);
}

ArduinoWebSocketClient client(true);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  while(WiFi.status() == WL_NO_SHIELD);

  // attempt to connect to Wifi network:
  int status = WL_IDLE_STATUS;
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    delay(5000);
  } 
 
  printWifiStatus();
 
  Serial.print("connecting to websocket... ");
  client.setMessageHandler(handler);
  while(!client.connect(host, port, websocketUri));
  Serial.println("OK");

  
}

void loop() {
  client.monitor();
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}


