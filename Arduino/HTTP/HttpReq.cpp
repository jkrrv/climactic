#include <Ethernet.h>
#include "HttpReq.h"; // include the declaration

/* not pretty, but effective */
#define httpPortionStatus 2
#define httpPortionHeader 1
#define httpPortionBody 0


String server;
String resource = "/";
int port;
EthernetClient client;
boolean lastConnected = false; 
String responseBuffer = "";
int httpStatus = 0;
String body = "";
String headers = "";

HttpReq::HttpReq(String url, int p) { // Constructor
  __construct(url, p);
};

HttpReq::HttpReq(String url) { // Constructor
  __construct(url, 80);
};

HttpReq::~HttpReq() { // Destructor
  //nothing to do...
};


void HttpReq::__construct(String url, int p) {
  int urlBreak = url.indexOf("/");
  if (urlBreak > 0) {
    server = url.substring(0, urlBreak);
    resource = url.substring(urlBreak);
  } else {
    server = url;
    resource = "/";
  }
  port = p;
  parsePortion = httpPortionStatus;
}



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
      parsePortion = httpPortionStatus; // reset to Status for next request.
      return 1; //success
    }
  }
}

int HttpReq::httpConnect() {
  // if there's a successful connection:
  char *buf = (char*)server.c_str(); //TODO: make sure this is correct.
  if (client.connect(buf, port)) {
#ifdef HTTP_REQ_DEBUG
    Serial.println("connecting...");
#endif
    // send the HTTP PUT request:
    client.println("GET " + resource + " HTTP/1.1");
    client.println("Host: " + server);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    lastConnected = client.connected();
    // note the time that the connection was made:
    return 1; // success
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed. Disconnecting.");
    client.stop();
    return 0; // error
  }
}


void HttpReq::parseResponseLine(String line) {
  switch (parsePortion) {
    case httpPortionStatus:
      if (line.startsWith("HTTP")) {
        httpStatus = 0;
        int space1 = line.indexOf(' ');
        int space2 = line.indexOf(' ', space1+1);
        httpStatus = line.substring(space1+1, space2).toInt();
        if (httpStatus > 0) {
#ifdef HTTP_REQ_DEBUG
          Serial.println("Status: " + String(httpStatus));
#endif
          parsePortion = httpPortionHeader;
          headers = "";
          break;
        }
      }
      
      
    case httpPortionHeader:
      if (line != "\r") {
        headers += line;
        headers += "\n";
      } else {
        parsePortion = httpPortionBody;
        body = "";
      }
      break;
      
      
    case httpPortionBody:
      body += line;
      body += "\n";
  }
}
