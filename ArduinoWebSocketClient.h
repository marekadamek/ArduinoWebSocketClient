
#ifndef _ARDUINO_WEBSOCKET_CLIENT_HEADER_
#define _ARDUINO_WEBSOCKET_CLIENT_HEADER_

#include "Arduino.h"
#include <Client.h>

class ArduinoWebSocketClient {
	public:
	static const int HEARTBEAT_INTERVAL = 5000;
	static const unsigned long HANDSHAKE_TIMEOUT = 30000;
	static const int MAX_PAYLOAD = 125;

	typedef void (*OnMessageHandler)(ArduinoWebSocketClient* client, char* message, int size);
	
	ArduinoWebSocketClient(bool wifi = false);
	~ArduinoWebSocketClient();

	bool connect(char* hostname, int port, char* websocketUri);
    bool connected();
    void disconnect();
	void monitor();
	bool send(char *payload, int size);
	void setMessageHandler(OnMessageHandler handler);
	
	private:
	Client* client;

    OnMessageHandler handler;
    unsigned long lastHeartbeat;
    char* hostname;
    int port;
    char* websocketUri;
    
    bool sendHandshake();
	int read(char* buffer);
};

#endif
