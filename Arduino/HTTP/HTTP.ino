#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include "HttpReq.h"
#include <aJSON.h>

#define DHTPIN 40
#define DHTTYPE DHT22 // (AM2301) See DHT lib documentation.

#define heatPin 42
#define fanPin 44

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,1,201);

EthernetServer server(8090);

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds


DHT dht(DHTPIN, DHTTYPE);

boolean heat;
boolean fan;

void setup() {
  // start serial port:
  Serial.begin(9600);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  // initialize listener
  server.begin();
  
  // init DHT
  dht.begin();
  
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  // init furnace stuff
  heat = false;
  fan = false;
  pinMode(heatPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  
}

#define REQ_PART_METHOD 1
#define REQ_PART_RESOURCE 2
#define REQ_PART_HEADER 5
#define REQ_PART_BODY 8

void loop() {
  ///////////////////////////////////
  // Inbound requests 
  ///////////////////////////////////
  
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean stillHeader = true;
    int nowParsing = REQ_PART_METHOD;
    
    String method="";
    String resource="";
    String reqBody="";
    
    while (client.connected()) {
      
      // PARSE THE REQUEST
      boolean lastCharWasNewline = false;
      while (client.available()) {
        char c = client.read();
        switch(nowParsing) {
          case REQ_PART_METHOD:
            if (c!=-1 && c!='\n' && c!=' ') {
              method+=c;
            } else {
              nowParsing = REQ_PART_RESOURCE;
            }
            break;
          case REQ_PART_RESOURCE:
            if (c!=-1 && c!='\n' && c!=' ') {
              resource+=c;
            } else {
              nowParsing = REQ_PART_HEADER;
            }
            break;
          case REQ_PART_HEADER:
            if (c=='\n') {
              if (lastCharWasNewline) {
                nowParsing = REQ_PART_BODY;
                break;
              }
              lastCharWasNewline = true; 
            } else {
              if (c!='\r') {
                lastCharWasNewline = false;
              }
            }
            break;
          case REQ_PART_BODY:
            reqBody += c;
            break;
        }
      }
#ifdef REQ_PART_DEBUG
      Serial.print("method:  ");
      Serial.println(method);
      Serial.print("resource:");
      Serial.println(resource);
      if (method == "POST") {
        Serial.println("Body:");
        Serial.println(reqBody);
      }
#endif
      
      
      
      // DETERMINE RESOURCE
      if (resource != "/therm") {
        client.println("HTTP/1.1 404 NOT FOUND");
        break; // while connected
      }
      
      
      // PARSE AND PROCESS POSTED DATA  TODO: move to before response (so changes are shown)
      if (method == "POST") {
        char jsonChars[reqBody.length()+1]; 
        reqBody.toCharArray(jsonChars, reqBody.length()+1);
        
        aJsonObject* root = aJson.parse(jsonChars);
      
        if (root != NULL) {
          //Serial.println("Parsed successfully 1 " );
          aJsonObject* jsonFurn = aJson.getObjectItem(root, "Furn"); 
    
          if (jsonFurn != NULL) {
            //Serial.println("Parsed successfully 2a " );
            aJsonObject* jsonHeat = aJson.getObjectItem(jsonFurn, "heat"); 
  
            heat = (int)jsonHeat->valuebool;
            
            //Serial.println("Parsed successfully 2b " );
            aJsonObject* jsonFan = aJson.getObjectItem(jsonFurn, "fan"); 
  
            fan = (int)jsonFan->valuebool;
          }
        }
      }
      
      
      // SEND RESPONSE
      // send a standard http response header
      client.println("HTTP/1.1 200 OK");
      client.println("Connection: close");  // the connection will be closed after completion of the response
      client.println();
      // output the value of each analog input pin
      client.println("{");
      client.println("\t\"DHT\" : {");
        client.print("\t\t\"H\" : ");
        client.print(dht.readHumidity());
        client.println(",");
        client.print("\t\t\"T\" : ");
        client.print(dht.readTemperature());
        client.println();
      client.println("\t},");
      client.println("\t\"Furn\" : {");
        client.print("\t\t\"heat\" : ");
        client.print(heat);
        client.println(",");
        client.print("\t\t\"fan\" : ");
        client.print(fan);
        client.println();
      client.println("\t}");
      client.println("}");
      break; // success, leave while connected. 
    } // while connected
    
    client.stop();

    Serial.println("client disconnected");
  }
  
  
  /////////////////////////////////
  // Digital Outs
  /////////////////////////////////
  digitalWrite(heatPin, !heat);
  digitalWrite(fanPin, !fan);
  
  
  /////////////////////////////////
  // Outbound Requests
  /////////////////////////////////
  if (millis() - lastConnectionTime > postingInterval) {  //scheduling for reqs.
    HttpReq r = HttpReq("192.168.1.107:8080/lights");
    //r.reqBody = body;
    r.execute();
    Serial.println(r.httpStatus);
    
    char charBuf[r.body.length()+1];
    r.body.toCharArray(charBuf, r.body.length()+1);
    aJsonObject* jsonObject = aJson.parse(charBuf);
    
    lastConnectionTime = millis();
  }
}
