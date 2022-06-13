#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SharpIR.h>
#define model 1080
#define sharpPin A0

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

SharpIR mySensor = SharpIR(sharpPin, model);

int distance_cm;

//const int trigPin = 4;
//const int echoPin = 5;
// defines variables
//long duration;
//int distance;

void setup() {
  // put your setup code here, to run once:
//  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
//  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  Serial.begin(9600);
} 

void loop() {
//  // Clears the trigPin
//  digitalWrite(trigPin, LOW);
//  // Sets the trigPin on HIGH state for 10 micro seconds
//  digitalWrite(trigPin, HIGH);
//  digitalWrite(trigPin, LOW);
//  // Reads the echoPin, returns the sound wave travel time in microseconds
//  duration = pulseIn(echoPin, HIGH);
//  // Calculating the distance
//  distance = duration * 0.034 / 2;
  distance_cm = mySensor.distance();

  // Print the measured distance to the serial monitor:
  Serial.print("Mean distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  if(distance_cm > 50) {
    radio.begin();
    const char text_barrier1[] = "PASS";
    bool ok = radio.write(&text_barrier1, sizeof(text_barrier1)); // Send the data
    Serial.println(ok);
  } else {
    radio.begin();
    const char text_barrier1[] = "WARNING";
    bool ok = radio.write(&text_barrier1, sizeof(text_barrier1)); // Send the data
    Serial.println(ok);
  }
  
  delay(50);
} 
