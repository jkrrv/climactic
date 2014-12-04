#include <Ethernet.h>
#include "HttpReq.h"; // include the declaration

#define HTTP_REQ_STATUS 2
#define HTTP_REQ_HEADER 1
#define HTTP_REQ_BODY 0

#define HTTP_REQ_DEBUG

String server;
EthernetClient client;
boolean lastConnected = false; 
String responseBuffer = "";
int parsePortion = HTTP_REQ_STATUS;
int httpStatus = 0;

HttpReq::HttpReq(String s) { // Constructor
  server = s;
};

HttpReq::~HttpReq() { // Destructor
  //nothing to do...
};



int HttpReq::execute() { // process request.

  while (true) { // break'd when connection ends or fails.
  
    // if you're not connected, and ten seconds have passed since
    // your last connection, then connect again and send data:
    if(!client.connected()) {
      if(!httpConnect()) {
        Serial.println("Connection Failed.");
        //break;
        return 0; // error
      }
    }
  
    // if there's incoming data from the net connection.
    // send it out the serial port.  This is for debugging
    // purposes only:
    if (client.available()) {
      char c = client.read();
      if (c != '\n') { // waits for newlines
        responseBuffer += c;
      } else {
#ifdef HTTP_REQ_DEBUG
        Serial.println(responseBuffer);
#endif
        parseResponseLine(responseBuffer);
        responseBuffer = "";
      }
    } 
  
    // if there's no net connection, but there was one last time
    // through the loop, then stop the client:
    if (!client.connected() && lastConnected) {
#ifdef HTTP_REQ_DEBUG
      Serial.println("disconnecting.");
#endif
      lastConnected = false;
      client.stop();
      //break;
      parsePortion = HTTP_REQ_STATUS; // reset to Status for next request.
      return 1; //success
    }
  }
}

int HttpReq::httpConnect() {
  // if there's a successful connection:
  char *buf = (char*)server.c_str(); //TODO: make sure this is correct.
  if (client.connect(buf, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /latest.txt HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    lastConnected = client.connected();
    // note the time that the connection was made:
    return 1; // success
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
    return 0; // error
  }
}


void HttpReq::parseResponseLine(String line) {
  Serial.println(parsePortion);
  if (parsePortion == HTTP_REQ_STATUS) {
    Serial.println("here");
    if (line.startsWith("HTTP")) {
      int space1 = line.indexOf(' ');
      int space2 = line.indexOf(' ', space1+1);
      httpStatus = line.substring(space1+1, space2).toInt();
      Serial.println("Status: " + httpStatus);
    }
  }
}
