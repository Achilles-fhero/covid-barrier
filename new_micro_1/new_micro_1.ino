#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

RF24 radio(10, 11);               // nRF24L01 (CE,CSN)
const byte address[6] = "00001";

int tempIR = 3;
int wiperPin = 5;
int buzzerTone = 6;
int upperIR = 8;
int belowIR = 7;
float temp = 0;
bool statusTrigger = false;
bool wiperTrigger = false;
unsigned long previousTime = 0;
const unsigned long firstSignal = 0;
const unsigned long signalInterval = 3000;
bool refreshSignal = true;


void setup() {
  Serial.begin(9600);
  // Start Wire library for I2C
  Wire.begin();
  mlx.begin();
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  pinMode(wiperPin, OUTPUT);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop(){
  unsigned long currentTime = millis();
  digitalWrite(wiperPin, HIGH);
  if(!statusTrigger) {
    if(displayTemp()) {
      statusTrigger = true;
      delay(1000);
      previousTime = currentTime;  
    }  
  } else {
  //Receivng
    radio.startListening();
    if (currentTime - previousTime >= firstSignal && refreshSignal) {
      Serial.println("FIRSSSST");
      if ( radio.available()) {
        char text_barrier1[32] = "";
        radio.read(&text_barrier1, sizeof(text_barrier1)); 
        String transData = String(text_barrier1);
      }
    } 
    if (currentTime - previousTime >= signalInterval) {
      Serial.println("SECOND");
      refreshSignal = false;
//      network.update();
//      radio.startListening();
//      radio.begin();
      if ( radio.available()) {
        char text_barrier1[32] = "";
        radio.read(&text_barrier1, sizeof(text_barrier1)); 
        String transData = String(text_barrier1);
        Serial.println(transData);
        if(transData == "PASS") { 
          wiperTrigger = true;
        } else {
          wiperTrigger = false;
        }
        
        while(wiperTrigger) {
          int belowIRStatus = digitalRead (belowIR);
          digitalWrite(wiperPin, LOW);
          delay(50);
          if(belowIRStatus == 0) {
            delay(400);
            digitalWrite(wiperPin, HIGH);
            delay(10000);
            while(wiperTrigger) {
              digitalWrite(wiperPin, LOW);
              int upperIRStatus = digitalRead (upperIR);
              delay(50);
              if(upperIRStatus == 0) {
                delay(150);
                digitalWrite(wiperPin, HIGH);
                belowIRStatus = digitalRead (belowIR);
                delay(1000);
                wiperTrigger = false;
                statusTrigger = false;
                temp = 0;
                refreshSignal = true;
              }
            }
          }
        } 
    }
        delay(50);
      }
    }
    
  delay(50);
  display.display();
}

bool displayTemp() {
  float tempC = 0;
  int tempIRStatus = digitalRead(tempIR);
  Serial.println(tempIRStatus);
  display.clearDisplay();
  if(tempIRStatus == 0) {
    delay(1000);
    
    for(int i=0; i<800; i++){
      tempC= mlx.readObjectTempC();
    }
    tempC += 1;
    if(tempC > 37) {
      
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(100);
      noTone(buzzerTone);
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(100);
      noTone(buzzerTone);
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(100);
      noTone(buzzerTone);
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(100);
      noTone(buzzerTone);
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(100);
      noTone(buzzerTone);
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(100);
      noTone(buzzerTone);
      display.clearDisplay();
      //Set the color - always use white despite actual display color
      display.setTextColor(WHITE);
      //Set the font size
      display.setTextSize(4);
      //Set the cursor coordinates
      display.setCursor(0,15);
      display.print(tempC);
      display.display();
      delay(2000);
      display.clearDisplay();
      return false;
   }
      tone(buzzerTone, 261);
      delay(100);
      noTone(buzzerTone);
      delay(10);
      noTone(buzzerTone);
      display.clearDisplay();
      //Set the color - always use white despite actual display color
      display.setTextColor(WHITE);
      //Set the font size
      display.setTextSize(4);
      //Set the cursor coordinates
      display.setCursor(0,15);
      display.print(tempC);
  
    return true;
  } else {
    noTone(buzzerTone);
  }
  delay(100);
  return false;
}
