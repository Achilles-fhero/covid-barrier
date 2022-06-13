#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);               // nRF24L01 (CE,CSN)
const byte address[6] = "00002";

int wiperPin = 6;
int belowIR = 7;
int upperIR = 8;
bool wiperTrigger = false;
bool warningTrigger = false;


void setup() {
  //  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  pinMode(wiperPin, OUTPUT); // variant low/high
  pinMode(upperIR, INPUT);
  pinMode(belowIR, INPUT);
  digitalWrite(wiperPin, HIGH);
}

void loop() {
  int belowIRStatus = digitalRead (belowIR);
  int upperIRStatus = digitalRead (upperIR);
  radio.startListening();
  if (radio.available()) {
    char text_barrier[32] = "";
    radio.read(&text_barrier, sizeof(text_barrier));
    String transData = String(text_barrier);
    if (transData == "WARNING") {
      wiperTrigger = true;
    } else {
      wiperTrigger = false;
    }
    while (wiperTrigger) {
      upperIRStatus = digitalRead(upperIR);
      digitalWrite(wiperPin, LOW);
      delay(50);
      if (upperIRStatus == 0) {
        delay(200);
        digitalWrite(wiperPin, HIGH);
        delay(3000);
        while (wiperTrigger) {
          //            Serial.println("GOOD");
          if (radio.available()) {
            char text_barrier[32] = "";
            radio.read(&text_barrier, sizeof(text_barrier));
            String transData = String(text_barrier);
            if (transData == "PASS") {
              wiperTrigger = false;
            } else {
              wiperTrigger = true;
            }
            //              Serial.println(transData);
          }
          delay(50);
        }
      }
    }
  }
  
  if (belowIRStatus != 0) {
    digitalWrite(wiperPin, LOW);
  } else {
    delay(200);
    digitalWrite(wiperPin, HIGH);
  }

  delay(100);
}
