

#include <SPI.h>
#include <Ethernet.h>
#include "HttpReq.h"


// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  
  
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,1,201);

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

void setup() {
  // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000);


  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  
}

void loop() {
  
  HttpReq r = HttpReq("www.arduino.cc");
  
  while (millis() - lastConnectionTime < postingInterval) {}
  
  
  r.execute();
  
  delay(10000);
}



