// Arduino Library by James Kurtz for dealing with HTTP requests in a more OOP sort of way. 

#ifndef HTTPREQ_H
#define HTTPREQ_H

#include <Arduino.h>
#include <Ethernet.h>

class HttpReq {
  public:
    HttpReq(String, int);
    HttpReq(String);
    ~HttpReq();
    int execute();
    String body;
    String headers;
  private:
    String reqHost;
    String resource;
    int port;
    int httpConnect();
    EthernetClient client;
    boolean lastConnected; 
    String responseBuffer;
    void parseResponseLine(String);
    int parsePortion;
};

#endif
