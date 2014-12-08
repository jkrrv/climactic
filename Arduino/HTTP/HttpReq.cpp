#include <Ethernet.h>
#include "HttpReq.h"; // include the magical request library

/* not pretty, but effective */
#define httpPortionStatus 2
#define httpPortionHeader 1
#define httpPortionBody 0


String reqServer;
String resource = "/";
int port;
EthernetClient client;
boolean lastConnected = false; 
String reqBody = "";
String method = "GET";
String responseBuffer = "";
int httpStatus = 0;
String body = "";
String headers = "";

HttpReq::HttpReq(String url, int p) { // Constructor  DO NOT INCLUDE PROTOCOL 
  __construct(url, p);
};

HttpReq::HttpReq(String url) { // Constructor  Port and resource will be extracted from url string.
  __construct(url, 80);
};

HttpReq::~HttpReq() { // Destructor
  //nothing to do...
};


void HttpReq::__construct(String url, int p) {
  method = "GET";
  int urlBreak = url.indexOf("/");
  int urlPort = url.indexOf(":");
  if (urlBreak > 0) {
    reqServer = url.substring(0, urlBreak);
    resource = url.substring(urlBreak);
  } else {
    reqServer = url;
    resource = "/";
  }
  if (urlPort > 0 && urlPort < urlBreak) {
    p = reqServer.substring(urlPort+1).toInt();
    reqServer = reqServer.substring(0, urlPort);
  }
  
#ifdef HTTP_REQ_DEBUG
  Serial.print("reqServer: ");
  Serial.println(reqServer);
  Serial.print("resource:  ");
  Serial.println(resource);
  Serial.print("port:      ");
  Serial.println(p);
#endif
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
  char *buf = (char*)reqServer.c_str(); //TODO: make sure this is correct.
  if (client.connect(buf, port)) {
#ifdef HTTP_REQ_DEBUG
    Serial.println("connecting...");
    Serial.println(method + " " + resource + " HTTP/1.1");
    Serial.println("Host: " + reqServer);
    Serial.println("User-Agent: arduino-ethernet");
    Serial.println("Connection: close");
    Serial.println();
    if (method == "POST") {
      Serial.println(reqBody);
    }
#endif
    // send the HTTP request:
    client.println(method + " " + resource + " HTTP/1.1");
    client.println("Host: " + reqServer);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    if (method == "POST") {
      client.println(reqBody);
    }
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
