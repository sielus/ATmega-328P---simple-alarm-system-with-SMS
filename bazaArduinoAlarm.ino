
#include <Wire.h>   
#include <LiquidCrystal_I2C.h> 
#include "DHT.h"

DHT dht;
int pinFrontSensor = A0; 
int pinBackSensor = A1; 

int pinUPStemp = A2;
int pinMQsensor = A3;

// digitalPins
int pinDHTsensor = 9;
int pinSiren = 8;
int pinACswitch = 10;

// A5 A4 = LCD 
// A3 = MQ7
// A2 = UPS temp
// A0 A1 = (A0 front sensor | A1 back sensor)

// RX = Keyboard

// D6 D7 = GSM com
// D8 = Alarm sirene switch 
// D9 = DHT-11
// D10 = AC switch 

int valFrontSensor = 0;
int valBackSensor = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  
void setup() {

  pinMode(pinSiren, OUTPUT); 
  dht.setup(pinDHTsensor);

  Serial.begin(9600);
  lcd.begin(16,2);   
  lcd.backlight(); 
  lcd.setCursor(0,0);
  setCursorLCD("Alarm rozbrojony","");
  delay(5000);
  lcd.off();

}

int index = 0;
char password[4];
bool ifArmed = false;
char key;
char newKey;
boolean ifAlarmTriggered = false;
boolean startTiming = false;

unsigned long previosTime;
unsigned long currentTime;

int dhtTemp;
int dhtHumidity;

int oldSec;

void loop() {
  currentTime = millis();
  dhtHumidity = dht.getHumidity();
  dhtTemp = dht.getTemperature();

  if (ifArmed) { // jeżeli alarm jest uzbrojony, można wpisać tylko hasło do wyłączenia
    if (Serial.available() > 0) {
      lcd.on();
      newKey = Serial.read();
      if (newKey != '#') {
        if (index > 3) {
          index = 0;
          memset(password, 0, sizeof password);
          setCursorLCD("Alarm uzbrojony","Haslo:");
        }
        key = newKey;
        password[index] = key;
        lcd.print(key);
        lcd.setCursor(7 + index,1);    
        index++;

      }

      else if (newKey == '#') {
        if (checkPasswd(convertToString(password, 4))) {
          disArmAlarm();
        } else {
          setCursorLCD("Alarm uzbrojony","Zle haslo!");
          delay(1000);
          setCursorLCD("Alarm uzbrojony","Haslo:");
          memset(password, 0, sizeof password);
          index = 0;
        }
        index = 0;
        memset(password, 0, sizeof password);
      }
    }

    valFrontSensor = analogRead(pinFrontSensor);
    valBackSensor = analogRead(pinBackSensor);

    if(startTiming == false){
      previosTime = millis();
    }

    if(valFrontSensor < 1000){
      startTiming = true;
    }

    if(valBackSensor < 1000){
      ifAlarmTriggered = true;
      backSensorTriggered();
    }

    //triger ruchu

    if(startTiming){
      if(currentTime - previosTime >= 5000){ //Wait 10sec when front sensor trigger until alarm siren on 
        ifAlarmTriggered = true;
        frontSensorTriggered();
      }
    }

  } else if (!ifArmed) {
    if (Serial.available() > 0) {
      lcd.on();
      key = Serial.read();
      if (key == 'A') {
        //print temp on LCD
        Serial.println("A temp");
        setCursorLCD("Temperatura " + String(dhtTemp),"Wilgotność " + String(dhtHumidity));
        delay(3000);
        lcd.clear();
        lcd.off();

      } else if (key == 'B') {
        //print battery on lcd
        Serial.println("B battery");
      } else if (key == '#') {
        armAlarm();
      }
    }
  }
}



boolean checkPasswd(String passwordToCheck) {
  String okPass = "4444";


  if (passwordToCheck == okPass) {
    return true;
  } else {
    return false;
  }
}



void armAlarm() {  // Alarm włączony, funkcje zablokowane (jedynie opcja wpisania hasła)
    int x;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Trwa uzbrajanie");
    lcd.setCursor(0,1);
      for(x = 30; x >= 0; x--){
        lcd.print(String(x) + "s" + " ");
        delay(1000);
        lcd.setCursor(0,1);
        }

    
    setCursorLCD("Alarm uzbrojony","Haslo:");
    delay(4000);
    lcd.off();
    ifArmed = true; 
    startTiming = false;
    ifAlarmTriggered = false;
    
   
}

String convertToString(char* a, int size){
  int i;
  String s = "";
  for (i = 0; i < size; i++) {
    s = s + a[i];
  }
  return s;
}

void disArmAlarm() {
    ifArmed = false; 
    startTiming = false;
    ifAlarmTriggered = false;  
  
  setCursorLCD("Alarn rozbrojony",String(dhtTemp));
  digitalWrite(pinSiren, LOW); 

  delay(5000);
  lcd.off();

}

void setCursorLCD(String messageRowFirst, String messageRowSec){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(messageRowFirst);
  lcd.setCursor(0,1);
  lcd.print(messageRowSec);
  lcd.setCursor(messageRowSec.length(),1);
}

void frontSensorTriggered(){
   startAlarmSiren();
   Serial.println("frontSensorTriggered");
}

void backSensorTriggered(){
  Serial.println("backSensorTriggered");
  startAlarmSiren();
}

void startAlarmSiren(){
  digitalWrite(pinSiren, HIGH); 
  Serial.println("startAlarmSiren");

}
