
#include "ArduinoWebSocketClient.h"
#include <EthernetClient.h>
#include <WiFiClient.h>

ArduinoWebSocketClient::ArduinoWebSocketClient(bool wifiClient) {
    if (wifiClient) {
      client = new WiFiClient();
    }
    else {
       client = new EthernetClient();
    }
}

ArduinoWebSocketClient::~ArduinoWebSocketClient() {
	disconnect();
	delete client;
}

bool ArduinoWebSocketClient::connect(char* hostname, int port, char* websocketUri) {
	disconnect();
        
    this->hostname = hostname;
    this->port = port;
    this->websocketUri = websocketUri;

	if (client->connect(hostname, port) && sendHandshake()) {
		lastHeartbeat = millis();
		return true;
	}

	disconnect();
	return false;
}

bool ArduinoWebSocketClient::connected() {
	return client->connected();
}

void ArduinoWebSocketClient::disconnect() {
	client->stop();
}

void ArduinoWebSocketClient::monitor() {
	if (!client->connected())
	  	return;

	// read data if available
	if (client->available()) {
  		char buffer[MAX_PAYLOAD];
   		int n = read(buffer);

          if (handler != NULL) {
                  int size = n -2;
                  char data[size+1];
                  memcpy(data, &buffer[2], size);
                  data[size] = '\0';
        	  handler(this, data, size);
          }
   	}
    
    //send heartbeat (room for improvement)    
    unsigned long now = millis();
    if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
      send(NULL, 0);
      lastHeartbeat = now;
    }
}

bool ArduinoWebSocketClient::sendHandshake() {
	client->print("GET ");
	client->print(websocketUri);
	client->println(" HTTP/1.1");
	client->print("Host: ");
  	client->print(hostname);
   	client->print(":");
  	client->println(port);
	client->println("Connection: upgrade");
    client->println("Upgrade: websocket");
    client->println("Sec-WebSocket-Version:13");
    client->println("Sec-WebSocket-Key:key");
    client->println("\r\n");
    
    unsigned long now = millis();
	while (!client->available() && ((millis() - now) < HANDSHAKE_TIMEOUT));
    if (!client->available()) {
    	return false;
    }
    
    char buffer[MAX_PAYLOAD];
    read((char*)&buffer); 
    
    //check if HTTP response status equals 101
	return buffer[9] == '1' && buffer[10] == '0' && buffer[11] == '1'; 
}

int ArduinoWebSocketClient::read(char* buffer) {
	char* begin = buffer;
	
   	int i = 0;
	while (client->available() && i < MAX_PAYLOAD) {
		buffer[i++] = client->read();
	}
	
	//discard data if sent too much
	if (client->available()) {
		client->flush();
	}

	return i;
}

//
// Sends single text frame. For more information please visit http://tools.ietf.org/html/rfc6455#section-5
//
bool ArduinoWebSocketClient::send(char* payload, int size) {
	if (size > MAX_PAYLOAD){
		return false;
	}

    uint8_t buffer[6 + size];
    buffer[0] = (uint8_t) 0x81;
    buffer[1] = (uint8_t)(128 + size);
    for (int i = 0; i < 4; ++i) {
      buffer[2 +i] = (uint8_t) random(256);
    }
    
    for (int i = 0; i < size; ++i) {
      buffer[6+i] = payload[i] ^ buffer[2 + i%4];
    }
    
    int sent = client->write(buffer, 6 + size);
    return sent-6 == size;
}

void ArduinoWebSocketClient::setMessageHandler(OnMessageHandler handler) {
  this->handler = handler;
}
