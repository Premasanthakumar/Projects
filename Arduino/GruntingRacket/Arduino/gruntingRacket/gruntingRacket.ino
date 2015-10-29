/*
Grunting Racket
Every time swinging a tennis racket, you can hear pro tennis players' grantings.
Components are an axcellerometer, a battery, a speaker, Arduino mini, Adafruit Soundboard, a LED, and some buttons.
When a value from axcellerometer excesses a particular threshold, the soundboard will play a tone.
You can switch to other players whose granting you want to hear by pressing a button on the grip.
*/

//============================== filenames =============================
//filename should have 8 charactors
String sharapova[] = {
"SHARA172WAV\n",
"shara18 WAV\n"
};

String serena[] = {
};

String federer[] = {
};

String djokovic[] = {
};

//============================== include libraries =============================
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 5
#define SFX_RX 6

// Connect to the RST pin on the Sound Board
#define SFX_RST 4

// You can also monitor the ACT pin for when audio is playing!

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

// can also try hardware serial with
// Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);

//============================== global variables =============================
//Pin assignments -----------------------------------------
const int xInput = A0;
const int yInput = A1;
const int zInput = A2;
const int playerSelectButtonPin = 2;
const int LEDPin = 13;

//Flags -----------------------------------------
boolean isSwung = false;
unsigned long lastSwing;
const int pauseDuration = 2000;

#define DEBOUNCE 100  // button debouncer

//============================== setup =============================
void setup() {
   //analogReference(EXTERNAL);
   Serial.begin(115200);
   Serial.println("Adafruit Sound Board!");
  
   // softwareserial at 9600 baud
   ss.begin(9600);
   // can also do Serial1.begin(9600);
  
   if (!sfx.reset()) {
     Serial.println("Not found");
     while (1);
   }
   Serial.println("SFX board found");
   
   pinMode(playerSelectButtonPin, INPUT);
   pinMode(LEDPin, OUTPUT);
}

//============================== loop() =============================
void loop() {
  //flush input on software serial
  flushInput(); 
  
  //try to detect swing of racket
  swingDetection();
  
  //play sounds if the racket is swung
  playSounds();
  
  Serial.println("=================");
  //delay(3000);
  
  if(digitalRead(playerSelectButtonPin) == HIGH){
    digitalWrite(LEDPin, HIGH);
  } else {
    digitalWrite(LEDPin, LOW);
  }
}

//============================== swingDetection() =============================
void swingDetection() {
  //threshold
  int thresholdHigh = 600;
  int thresholdLow = 50;
  
  
  //read analog inputs
  int xRaw = readAxis(xInput);
  int yRaw = readAxis(yInput);
  int zRaw = readAxis(zInput);
  
  String printThis = "xRaw: " + String(xRaw) + " yRaw: " + String(yRaw) + " zRaw: " + String(zRaw);
  //Serial.println(printThis);
  
  //if one of the input values excesses the threshold, turn the flag on
  if (xRaw > 600 || yRaw > 600 || zRaw > 600 || xRaw < 50 || yRaw < 50 || zRaw < 50){
    isSwung = true;
    lastSwing = millis();
    Serial.println("Racket was swung!");
    Serial.println("lastSwing: " + String(lastSwing));
    
  } else {
    isSwung = false;
    Serial.println("isSwung turned FALSE");
  }
}

//============================== playSounds() =============================
void playSounds() {
  if (isSwung && lastSwing - millis() > pauseDuration) {
    Serial.println("trying to play sound");
    String filename = sharapova[0]; //the file you want to play
    
    //convert string to char array
    char name[12];
    filename.toCharArray(name, 12);
    
    //play sound with the filename
    Serial.println("sent command to SFX with this string: " + filename);
    if (! sfx.playTrack(name) ) {
      Serial.println("Failed to play track?");
    }
  }

  //reset the flag
  isSwung = false;
  Serial.println("isSwung turned FALSE");
}

//============================== readAxis() =============================
// Read "sampleSize" samples and report the average
//
int readAxis(int axisPin) {
  int sampleSize = 10;
  
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++) {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}

//============================== flushInput() =============================
void flushInput() {
  // Read all available serial input to flush pending data.
  uint16_t timeoutloop = 0;
  while (timeoutloop++ < 40) {
    while(ss.available()) {
      ss.read();
      timeoutloop = 0;  // If char was received reset the timer
    }
    delay(1);
  }
}
