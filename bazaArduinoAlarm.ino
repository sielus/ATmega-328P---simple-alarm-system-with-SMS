
#include <Wire.h>   
#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);   
  lcd.backlight(); 
  lcd.setCursor(0,0);
  setCursorLCD("Alarm rozbrojony","temp");
  delay(5000);
  lcd.off();

}
int index = 0;
char password[4];
bool ifArmed = false;
char key;
char newKey;
void loop() {
  if (ifArmed) { // jeżeli alarm jest uzbrojony, można wpisać tylko hasło do wyłączenia
    if (Serial.available() > 0) { // Wyłączenie alarmu po wypisaniu hasła
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

    //triger ruchu

  } else if (!ifArmed) {
    if (Serial.available() > 0) {
      lcd.on();
      key = Serial.read();
      if (key == 'A') {
        //print temp on LCD
        Serial.println("A temp");
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
   ifArmed = true; 
   setCursorLCD("Alarm uzbrojony","Haslo:");
   delay(4000);
   lcd.off();

}

String convertToString(char* a, int size)
{
  int i;
  String s = "";
  for (i = 0; i < size; i++) {
    s = s + a[i];
  }
  return s;
}

void disArmAlarm() {
  ifArmed = false;
  setCursorLCD("Alarn rozbrojony","Temp ");
  
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