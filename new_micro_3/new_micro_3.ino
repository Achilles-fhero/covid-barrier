#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SharpIR.h>
#define model 1080
#define sharpPin A0


SharpIR mySensor = SharpIR(sharpPin, model);
int distance_cm;

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00002";
  
int wiperPin = 4;
int alcoholPin = 5;
int alcoholIR = 6;
int belowIR = 7;
int upperIR = 8;
bool mainSwitch = false;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  pinMode(wiperPin, OUTPUT); // variant low/high
  pinMode(alcoholPin, OUTPUT);
  pinMode(upperIR, INPUT);
  pinMode(belowIR, INPUT);
  digitalWrite(wiperPin, HIGH);
  digitalWrite(alcoholPin, HIGH);

}

void loop() {
  int alcoholStatus= digitalRead (alcoholIR);
  //Enter
  if(alcoholStatus == 0) {
    mainSwitch = true;
  } 
  if(mainSwitch == true) {
    sendWarning();
    sprayAlcohol();
    openWiper();
    mainSwitch = false;
    alcoholStatus = 1;
    delay(2000);
  } else {
    sendPass();
    delay(50);
  }
}

void sprayAlcohol(){

  digitalWrite(alcoholPin, LOW);
  delay(1100);  
  digitalWrite(alcoholPin,HIGH);
} 

void openWiper () {
  bool wiperSwitch = true;
  while(wiperSwitch) {
    radio.begin();
    int belowIRStatus = digitalRead (belowIR);
    digitalWrite(wiperPin, LOW);
    delay(50);
    if(belowIRStatus == 0) {
      delay(400);
      digitalWrite(wiperPin, HIGH);
      delay(10000);
      while(wiperSwitch) {
        radio.begin();
        digitalWrite(wiperPin, LOW);
        int upperIRStatus = digitalRead (upperIR);
        delay(50);
        if(upperIRStatus == 0) {
          delay(50);
          digitalWrite(wiperPin, HIGH);
          belowIRStatus = digitalRead (belowIR);
          delay(500);
          wiperSwitch = false;
        }
      }
    }  
  }
  
}

void sendWarning (){
  bool ok = 0;
  while(ok != 1) {
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    const char text_barrier[] = "WARNING";
    ok = radio.write(&text_barrier, sizeof(text_barrier));
    delay(50);
  }
  
}

void sendPass (){
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  distance_cm = mySensor.distance();
  if(distance_cm >= 50) {
    radio.begin();
    const char text_barrier[] = "PASS";
    bool ok = radio.write(&text_barrier, sizeof(text_barrier)); // Send the data
    
  } else {
    radio.begin();
    const char text_barrier[] = "WARNING";
    bool ok = radio.write(&text_barrier, sizeof(text_barrier)); // Send the data
  }
}
