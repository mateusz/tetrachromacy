// Hz for Heterochromatic Flicker Photometry 15-40Hz
#define INITIAL_FREQ 20
#define DISPLAY_PERIOD_MS 1000
#define BUTTONS_PERIOD_MS 10
#define DITHER_PERIOD_MICROS 100

#include <LCD-I2C.h>
#include <Wire.h>

const int greenLedPin = 11;
const int orangeLedPin = 10;
const int redLedPin = 9; 
const int greenPotPin = A2;
const int orangePotPin = A1;
const int redPotPin = A0;

const int downPin = 2;
const int upPin = 3;
const int okPin = 4;

int greenIntensity = 0;
int orangeIntensity = 0;
int redIntensity = 0;
unsigned long halfPeriod = 1000UL / INITIAL_FREQ / 2;

// Software 10-bit PWM variables for pin 11 (green)
unsigned long pwmCounter = 0;
const unsigned long PWM_PERIOD = 4;  // Number of cycles for dithering pattern

// Timing variables for flicker control
unsigned long hfpLastRefresh = 0;
bool hfpState = false;  // false = first half, true = second half

unsigned long buttonsLastRefresh = 0;
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

// Function to implement 10-bit software PWM using temporal dithering
void setSoftwarePWM10bit(int value10bit) {
  // Convert 10-bit value (0-1023) to 8-bit base + fractional part
  int base8bit = value10bit >> 2;  // Divide by 4 to get 8-bit base (0-255)
  int fraction = value10bit & 0x03;  // Get the 2 least significant bits (0-3)
  
  // Create dithering pattern based on fraction
  int currentPWM = base8bit;
  
  // Add temporal dithering based on PWM counter and fraction
  switch (fraction) {
    case 0:
      // No dithering needed
      break;
    case 1:
      // Add 1 every 4th cycle
      if ((pwmCounter & 0x03) == 0) currentPWM++;
      break;
    case 2:
      // Add 1 every 2nd cycle
      if ((pwmCounter & 0x01) == 0) currentPWM++;
      break;
    case 3:
      // Add 1 three out of every 4 cycles
      if ((pwmCounter & 0x03) != 3) currentPWM++;
      break;
  }
  
  // Clamp to 8-bit range
  if (currentPWM > 255) currentPWM = 255;
  
  // Set the PWM value
  OCR2A = currentPWM;
}

void setup() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  pinMode(downPin, INPUT_PULLUP);
  pinMode(upPin, INPUT_PULLUP);
  pinMode(okPin, INPUT_PULLUP);

  TCCR2A = _BV(WGM20) | _BV(WGM21) | _BV(COM2A1);  // Fast PWM, non-inverting
  TCCR2B = _BV(CS20);  // Prescaler = 1 (fastest frequency)

  // Set 10-bit Fast PWM mode (WGM13:0 = 0111)
  TCCR1A = _BV(WGM11) | _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1);  // WGM11=1, WGM10=1, COM1A1=1, COM1B1=1 (non-inverting)
  TCCR1B = _BV(WGM12) | _BV(CS10);                  // WGM12=1, CS10=1 (no prescaling)

  // Set initial duty cycle (10-bit range: 0–1023)
  OCR1A = 0;  // 0% on pin 9, red - 10-bit range: 0-1023
  OCR1B = 0;  // 0% on pin 10, orange - 10-bit range: 0-1023
  OCR2A = 0;  // 0% on pin 11, green - 8-bit range: 0-255 (but uses software dithering)

  Serial.begin(9600);
  Serial.println("Heterochromatic Flicker Photometry Tetrachromacy Test Device Initialized");

  Wire.begin();
  lcd.begin(&Wire);
  lcd.clear();
  lcd.display();
  lcd.backlight();  
  
  lcdLastRefresh = millis();
}

void loop() {
  // Refresh buttons less frequently than the main dithering loop.
  if (millis()-buttonsLastRefresh>BUTTONS_PERIOD_MS) {
    buttonsLastRefresh = millis();

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
  }

  // Refresh menu - takes time and power, so produces a small flicker, so reduced frequency.
  if (!menu[2].value || displayDataOnce) {
    if (millis()-lcdLastRefresh>DISPLAY_PERIOD_MS || displayDataOnce) {
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
        lcd.print("R");
        lcd.print(redIntensity);
        lcd.setCursor(5,1);
        lcd.print("O");
        lcd.print(orangeIntensity);
        lcd.setCursor(10,1);
        lcd.print("G");
        lcd.print(greenIntensity);  // Display 10-bit value
      }

      lcdLastRefresh = millis();
    }
  }

  // Check if it's time to switch flicker state
  if (millis() - hfpLastRefresh >= halfPeriod) {
    hfpState = !hfpState;
    hfpLastRefresh = millis();
  }

  // ---------- Main dithering loop ----------

  // Read potentiometer values
  //greenIntensity = map(analogRead(greenPotPin), 0, 1023, 0, 1023);
  greenIntensity = map(analogRead(greenPotPin), 0, 1023, 0, 255);
  orangeIntensity = map(analogRead(orangePotPin), 0, 1023, 0, 1023);
  redIntensity = map(analogRead(redPotPin), 0, 1023, 0, 1023);
  halfPeriod = 1000UL / menu[0].value / 2;

  // Set LED states based on flicker state
  if (!hfpState) {
    // First half period - Green LED with 10-bit software PWM
    //setSoftwarePWM10bit(greenIntensity);
    OCR2A = greenIntensity;
    OCR1B = 0;  // Orange off
    OCR1A = redIntensity;  // Red on
  } else {
    // Second half period - Orange LED
    //setSoftwarePWM10bit(0);  // Green off
    OCR2A = 0;
    OCR1B = orangeIntensity;  // Orange on
    OCR1A = 0;  // Red off
  }

  // Increment PWM counter for dithering (runs at full speed)
  pwmCounter++;

  // Small delay to prevent overwhelming the system while allowing fast PWM updates
  //delayMicroseconds(DITHER_PERIOD_MICROS);
  delay(halfPeriod/10);
}