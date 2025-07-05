// Hz for HFP 15-40Hz
#define debug 1
#define INITIAL_FREQ 20

#include <LCD-I2C.h>
#include <Wire.h>

const int greenLedPin = 9;
const int orangeLedPin = 10;
const int redLedPin = 11; 
const int greenPotPin = A0;
const int orangePotPin = A1;
const int redPotPin = A2;

const int downPin = 2;
const int upPin = 3;
const int okPin = 4;

int greenIntensity = 0;
int orangeIntensity = 0;
int redIntensity = 0;
int flickerSpeed = 10;
unsigned long halfPeriod = 1000UL / INITIAL_FREQ / 2;

int downState = HIGH;
int upState = HIGH;
int okState = HIGH;

struct Menu {
  int id;
  char* name;
  int value;
  int step;
  int min;
  int max;
}; 

struct Menu menu[] = {
  {0, "FREQUENCY", INITIAL_FREQ, 1, 1, 180},
  {1, "MEASURE", 1, 1, 0, 1},
  {2, "RUNTEST", 0, 1, 0, 1}
};
int maxMenu = 2;

int currentMenu = 0;
int menuActive = 0;
int isTesting = 0;
int displayDataOnce = 0;

LCD_I2C lcd(0x27, 16, 2);
unsigned long lcdLastRefresh;

void setup() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  pinMode(downPin, INPUT_PULLUP);
  pinMode(upPin, INPUT_PULLUP);
  pinMode(okPin, INPUT_PULLUP);

  // Raise frequency on Timer1: pins 9 and 10 → ~31.25 kHz
  TCCR1B = TCCR1B & 0b11111000 | 0x01;

  // Raise frequency on Timer2: pin 11 → ~31.25 kHz
  TCCR2B = TCCR2B & 0b11111000 | 0x01;

  Serial.begin(9600);
  Serial.println("Tetrachromacy Test Device Initialized");

  Wire.begin();
  lcd.begin(&Wire);
  lcd.clear();
  lcd.display();
  lcd.backlight();  
  
  lcdLastRefresh = millis();
}

void loop() {
  if (digitalRead(downPin) == LOW && downState == HIGH) {
    if (menu[2].value) {
      displayDataOnce = 1;
    }
    else if (!menuActive) {
      if (currentMenu==0) {
        currentMenu = maxMenu;
      } else {
        currentMenu--;
      }
    } else {
      menu[currentMenu].value -= menu[currentMenu].step;
      if (menu[currentMenu].value<menu[currentMenu].min) {
        menu[currentMenu].value = menu[currentMenu].min;
      }
    }
    downState = LOW;
  }
  if (digitalRead(downPin) == HIGH && downState == LOW) {
    downState = HIGH;
  }

  if (digitalRead(upPin) == LOW && upState == HIGH) {
    if (menu[2].value) {
      displayDataOnce = 1;
    }
    else if (!menuActive) {
      if (currentMenu==maxMenu) {
        currentMenu = 0;
      } else {
        currentMenu++;
      }
    }  else {
      menu[currentMenu].value += menu[currentMenu].step;
      if (menu[currentMenu].value>menu[currentMenu].max) {
        menu[currentMenu].value = menu[currentMenu].max;
      }
    }
    upState = LOW;
  }
  if (digitalRead(upPin) == HIGH && upState == LOW) {
    upState = HIGH;
  } 

  if (digitalRead(okPin) == LOW && okState == HIGH) {
    if (menu[2].value) {
      menu[2].value = 0;
    } else {
      menuActive = !menuActive;
    }
    okState = LOW;
  }
  if (digitalRead(okPin) == HIGH && okState == LOW) {
    okState = HIGH;
  }   

  greenIntensity = map(analogRead(greenPotPin), 0, 1023, 0, 255);
  orangeIntensity = map(analogRead(orangePotPin), 0, 1023, 0, 255);
  redIntensity = map(analogRead(redPotPin), 0, 1023, 0, 255);
  halfPeriod = 1000UL / menu[0].value / 2;

  analogWrite(greenLedPin, greenIntensity);
  analogWrite(orangeLedPin, 0);
  analogWrite(redLedPin, redIntensity);
  delay(halfPeriod);

  analogWrite(greenLedPin, 0);
  analogWrite(orangeLedPin, orangeIntensity);
  analogWrite(redLedPin, 0);

  if (!menu[2].value || displayDataOnce) {
    if (millis()-lcdLastRefresh>1000 || displayDataOnce) {
      displayDataOnce = 0;

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(menu[currentMenu].name);
      lcd.print("=");
      lcd.print(menu[currentMenu].value);
      if (menuActive) {
        lcd.print(" *");
      }

      if (menu[1].value) {
        lcd.setCursor(0,1);      
        lcd.print("G");
        lcd.print(greenIntensity);
        lcd.setCursor(4,1);
        lcd.print(" O");
        lcd.print(orangeIntensity);
        lcd.setCursor(9,1);
        lcd.print(" R");
        lcd.print(redIntensity);
      }

      lcdLastRefresh = millis();
    }
  }

  delay(halfPeriod);

  #ifdef debug
  // Print current values for debugging
  Serial.print("GOR: ");
  Serial.print(greenIntensity);
  Serial.print(",");
  Serial.print(orangeIntensity);
  Serial.print(",");
  Serial.print(redIntensity);
  Serial.print(" hp ");
  Serial.print(halfPeriod);
  Serial.print("\t\t");

  Serial.print(menu[currentMenu].name);
  Serial.print("=");
  Serial.print(menu[currentMenu].value);

  if (menuActive) {
    Serial.print("*");
  }

  Serial.print("\n");
  #endif
}
