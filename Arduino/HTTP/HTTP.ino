

#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  
  
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,1,201);

char server[] = "www.arduino.cc";

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
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
  while (millis() - lastConnectionTime < postingInterval) {}
  
  httpDo();
  
}


void httpDo() {
  // initialize the library instance:
  EthernetClient client;

  while (true) { // break'd when connection ends or fails.
  
    // if you're not connected, and ten seconds have passed since
    // your last connection, then connect again and send data:
    if(!client.connected()) {
      if(!httpConnect()) {
        Serial.println("Connection Failed.");
        break;
      }
    }
  
    // if there's incoming data from the net connection.
    // send it out the serial port.  This is for debugging
    // purposes only:
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    } 
  
    // if there's no net connection, but there was one last time
    // through the loop, then stop the client:
    if (!client.connected() && lastConnected) {
  //    Serial.println();
  //    Serial.println("disconnecting.");
      client.stop();
      break;
    }
  }
}

// this method makes a HTTP connection to the server:
int httpConnect() {
  
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /latest.txt HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    lastConnected = client.connected();
    // note the time that the connection was made:
    lastConnectionTime = millis();
    return 1; // success
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
    return 0; // error
  }
}




