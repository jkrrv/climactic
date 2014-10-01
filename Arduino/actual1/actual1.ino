#include <DHT.h>

#define DHTPIN 3     // signal in pin for DHT
#define HPIN 7      // signal out pin for heat relay
#define FPIN 6      // signal out pin for fan relay
#define FSTAT 10     // signal in pin for whether the furnace is on
#define FSTATa 0    // analog input pin for furnace power


// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dh1(DHTPIN, DHTTYPE);

byte incomingByte = 0;
String incomingString = "";
int fStats[]={0,0,0,0,0,0,0,0,0,0};

void setup() {
  Serial.begin(9600); 
  //Serial.println("DHTxx test!");

  dh1.begin();
  pinMode(HPIN, OUTPUT);
  pinMode(FPIN, OUTPUT);
  pinMode(FSTAT, INPUT);
  analogReference(DEFAULT);
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float inHum = dh1.readHumidity();
  float inTmp = dh1.readTemperature();
  int inFStat = digitalRead(FSTAT);
  
  fStats[9] = fStats[8];
  fStats[8] = fStats[7];
  fStats[7] = fStats[6];
  fStats[6] = fStats[5];
  fStats[5] = fStats[4];
  fStats[4] = fStats[3];
  fStats[3] = fStats[2];
  fStats[2] = fStats[1];
  fStats[1] = fStats[0];
  //fStats[0] = digitalRead(FSTAT); //analogRead(FSTATa);
  //inFStat = fStats[0] + fStats[1] + fStats[2] + fStats[3] + fStats[4] + fStats[5] + fStats[6] + fStats[7] + fStats[8] + fStats[9];
  
 // if (inFStat>6000) {
    //inFStat=0;
 // } else {
 //   inFStat=1;
 // }
  
  boolean outHeat = 0;
  boolean outFan = 0;

  String sOut = "";

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (Serial.available() > 0) {

    // READ THE SERIAL STREAM 
    incomingByte = Serial.read();
    incomingString = String(incomingByte, BIN);
    
    if (incomingByte != B1010) {

      // PARSE AND STORE VALUES FROM SERIAL STREAM
      outHeat=!!(incomingByte & B00000001);
      outFan =!!(incomingByte & B00000010);
  
      // OUTPUT HARWDWARE STUFF
      digitalWrite(HPIN,outHeat);
      digitalWrite(FPIN,outFan);

  
      // WRITE STUFF TO SERIAL
      //Serial.print("Humidity: "); 
      Serial.write(0);
      
      // write command
      Serial.print("\t");
      Serial.print(incomingString);
      Serial.print("\t\t");
      Serial.print(outHeat);
      Serial.print("\t");
      Serial.print(outFan);
  
      Serial.print("\t\t");
      Serial.print(inHum);
      Serial.print("\t");
      //Serial.print("Temperature: "); 
      Serial.print(inTmp);
      Serial.print("\t");
      Serial.print(inFStat);
      Serial.print("\t");
      
      Serial.println();
      Serial.flush();
    }
  }
  
  
}

