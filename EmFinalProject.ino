#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "PinChangeInterrupt.h"
#include "SevSeg.h"
SevSeg sevseg;

//----------------------------------------------set Pin/Port--------------------------------------//
const int soundSensorPin = A0;
const int buzzerPin = 7;
#define pinButton 11

const int R2Pin = 22; // pin color red in RGB led
const int G2Pin = 23; // pin color green in RGB led
const int B2Pin = 24; // pin color blue in RGB led
const int R1Pin = 4; // pin color red in led
const int G1Pin = 5; // pin color green in led
//---------------------------------------------set paramiter--------------------------------------//
const int soundThreshold = 200;
int GcountBack = 20,RcountBack = 30,countBack = 0;
// GcountBack is time for green light
// RcountBack  is time for red light
int state;
/* 0:know it has emerg
   1:know len
   2:complete check len */
bool isEmerg = false;
// it has emergenct or not
bool isGreen = false;
// RGB light is green color or not
bool outTime;
char customKey;
char emerlen,countlen = '2';
LiquidCrystal_I2C lcd(0x27,20,4);

const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'Y', 'N', 'E'}
  };
//-----------set pin keypad--------//
byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {34, 35, 36};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup()
{
  pinMode(soundSensorPin, INPUT);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(R1Pin, OUTPUT);
  pinMode(G1Pin, OUTPUT);
  pinMode(R2Pin, OUTPUT);
  pinMode(G2Pin, OUTPUT);
  pinMode(B2Pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  attachPCINT(digitalPinToPCINT(pinButton), Emerg, FALLING); // Attach interrupt on pinButton
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on backlight
  lcd.noCursor(); // Hide cursor
  Serial.begin(9600); // Initialize serial communication
  //-----------------set sevseg-----------------------------//
  byte numDigits = 2;
  byte digitPins[] = {40,41};
  byte segmentPins[] = {42, 43, 44, 45, 46, 47, 48, 49};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);
}

void loop()
{
  if(isEmerg){
    lightControl("emerg");  // Handle emergency light control
    noTone(buzzerPin);     // Stop sound...
    }
  else {
    state = 0; //reset state
    lcd.clear(); // clear LCD
    lightControl("normal");  // Handle normal light control
    //---------------------siren---------------------------------------------//
    int soundLevel = analogRead(soundSensorPin); //get sound level form microphon sensor
    Serial.println(soundLevel);
    // has siren 
    if(soundLevel > soundThreshold){
      lcd.print("!!Emergency!!"); // show on LCD that "!!Emergency!!"
      tone(buzzerPin, 1000); // Activate the buzzer with a frequency of 1000Hz
    }
    //-----------------------------------------------------------------------//
    }
}

void handleEmerg()
{
  countingTime("mil"); // count time in mode mil
  if(outTime){
    outTime = false; // reset outTime
    return;
  }

  if(state == 0){
    lcd.clear();
    lcd.print("Emergency on:");
    lcd.setCursor(0,13);
    customKey = customKeypad.getKey(); // get value form keypad
    if(customKey){
      emerlen = customKey;
      state = 1;
    }
  }
  
  if(state == 1){
    lcd.clear();
    if(emerlen != countlen){
      if(countBack > 5) countBack = 5;
    }
    if(countBack <= 0){
      state = 2;
      sevseg.setNumber(0, 0); // show number 0 on 4 digit 7 segment
      if(emerlen != countlen ){
        if(isGreen && emerlen == '1'){
        twoLight("red");
        oneLight("green");
        isGreen = !isGreen;
        }
        if(!isGreen && emerlen == '2'){
        twoLight("green");
        oneLight("red");
        isGreen = !isGreen;
        }
      }
    }
  }
}

void Emerg(){
  isEmerg = !isEmerg;
}

void countingTime(String Mode){
  static unsigned long timer = millis();
  if(Mode == "mil" && state != 2){
    if (millis() - timer > 1000){
      timer += 1000; // set timer = now
      sevseg.setNumber(countBack, 0); // show number on 4 digit 7 segment
      countBack -= 1;
      outTime = true;
    } 
  }

  if(Mode == "De"){
    if (millis()-timer >= 1000) {
      timer += 1000; // set timer = now
      sevseg.setNumber(countBack, 0); // show number on 4 digit 7 segment
      countBack--;
    }
  }
}

void lightControl(String Mode){
  if(Mode == "emerg"){
    handleEmerg();
    if(state == 0){
      // change color and set time for count
      if(countBack == 0){
        if(isGreen){
          countBack = RcountBack;
          twoLight("red");
          oneLight("green");
          isGreen = !isGreen;
          countlen = '1';
        }
        else{
          countBack = GcountBack;
          oneLight("red");
          twoLight("green");
          isGreen = !isGreen;
          countlen = '2';
        }
      }
    }
  }
  
  if(Mode == "normal"){
    if(countBack == 0){
    // change color and set time for count
      if(isGreen){
        countBack = RcountBack;
        twoLight("red");
        oneLight("green");
        isGreen = !isGreen;
        countlen = '1';
      }
      else{
        countBack = GcountBack;
        oneLight("red");
        twoLight("green");
        isGreen = !isGreen;
        countlen = '2';
      }
    }
    countingTime("De");
  }
  sevseg.refreshDisplay();
}

void oneLight(String Mode){
  // use red LED 
  if(Mode == "red"){
    analogWrite(R1Pin, HIGH);
    analogWrite(G1Pin, LOW);
  }
  // use green LED
  if(Mode == "green"){
    analogWrite(R1Pin, LOW);
    analogWrite(G1Pin, HIGH);
  }
}

void twoLight(String Mode){
  // show red color with RGB LED
  if(Mode == "red"){
    analogWrite(R2Pin,255);
    analogWrite(G2Pin,255);
    analogWrite(B2Pin,0);
  }
  // show yellow color with RGB LED
  if(Mode == "yellow"){
    analogWrite(R2Pin,150);
    analogWrite(G2Pin,255);
    analogWrite(B2Pin,0);
  }
  // show green color with RGB LED
  if(Mode == "green"){
    analogWrite(R2Pin,255);
    analogWrite(G2Pin,0);
    analogWrite(B2Pin,255);
  }
}