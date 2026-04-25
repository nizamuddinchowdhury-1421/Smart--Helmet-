/*
  Smart Helmet Receiver Code - FULL FRESH & IMPROVED (April 2026)
  - Engine ON only if Helmet + No Alcohol + Not Drowsy
  - Buzzer ON for Alcohol OR Drowsiness
  - Fixed RF receiving
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RH_ASK.h>
#include <SPI.h>

// ====================== LCD ======================
LiquidCrystal_I2C lcd(0x27, 16, 2);    

// ====================== RF ======================
RH_ASK rf_driver(2000, 11, 0, 10);   // RX on Pin 11

// ====================== PINS ======================
const int ignitionRelay = 4;   // Relay for BO Motor / Ignition
const int buzzerPin     = 5;   // Buzzer (and optional shared LED)

// Data variables
int helmetStatus = 0;
int alcoholLevel = 0;
int sleepStatus  = 0;
bool engineState = false;

// Timing
unsigned long lastReceivedTime = 0;
const unsigned long timeout = 5000;        // 5 seconds no signal = offline

int currentPage = 0;
unsigned long lastPageSwitch = 0;
const unsigned long pageInterval = 3000;   // Switch page every 3 seconds

char buf[60];

void setup() {
  Serial.begin(115200);
  Serial.println("=== Smart Helmet Receiver Starting ===");

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); 
  lcd.print(" Smart Helmet ");
  lcd.setCursor(0, 1); 
  lcd.print("   System ON   ");
  delay(2500);
  lcd.clear();

  // Initialize RF
  if (!rf_driver.init()) {
    Serial.println("RF init FAILED!");
    lcd.setCursor(0, 0); lcd.print("RF Init Failed!");
    while(1);
  }
  Serial.println("RF initialized OK. Waiting for data...");

  // Pin setup
  pinMode(ignitionRelay, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  digitalWrite(ignitionRelay, LOW);
  digitalWrite(buzzerPin, LOW);

  lastReceivedTime = millis();
  lastPageSwitch = millis();

  Serial.println("System Ready.");
}

void loop() {
  unsigned long currentMillis = millis();

  // ====================== RECEIVE RF DATA ======================
  uint8_t buflen = sizeof(buf);                    // Reset buffer length every loop
  if (rf_driver.recv((uint8_t*)buf, &buflen)) {
    buf[buflen] = '\0';
    String str_out = String((char*)buf);
    
    lastReceivedTime = currentMillis;

    int comma1 = str_out.indexOf(',');
    int comma2 = str_out.lastIndexOf(',');

    if (comma1 > 0 && comma2 > comma1) {
      helmetStatus = str_out.substring(0, comma1).toInt();
      alcoholLevel = str_out.substring(comma1 + 1, comma2).toInt();
      sleepStatus  = str_out.substring(comma2 + 1).toInt();

      Serial.print("✅ Received: ");
      Serial.print(str_out);
      Serial.print(" | Helmet:");
      Serial.print(helmetStatus);
      Serial.print(" | Alcohol:");
      Serial.print(alcoholLevel);
      Serial.print(" | Drowsy:");
      Serial.println(sleepStatus);
    }
  }

  // ====================== TIMEOUT - No Signal ======================
  if (currentMillis - lastReceivedTime > timeout) {
    if (currentPage != 99) {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("No Data Received");
      lcd.setCursor(0, 1); lcd.print("System Offline");
      digitalWrite(buzzerPin, HIGH);        // Alert when offline
      currentPage = 99;
      Serial.println("*** SYSTEM OFFLINE - No signal ***");
    }
    return;
  } 
  else if (currentPage == 99) {
    digitalWrite(buzzerPin, LOW);
    currentPage = 0;
    lcd.clear();
  }

  // ====================== IMPROVED ENGINE CONTROL ======================
   // ====================== IMPROVED + FIXED ENGINE CONTROL (Active LOW) ======================
  // Engine should be ON only if: Helmet worn + No Alcohol + Not Drowsy
  engineState = (helmetStatus == 1 && alcoholLevel == 0 && sleepStatus == 0);
  
  // FIXED: Most relay modules are Active LOW → send LOW to turn relay ON
  digitalWrite(ignitionRelay, engineState ? LOW : HIGH);

  // ====================== BUZZER CONTROL ======================
  // Buzzer ON if Alcohol OR Drowsy
  bool alertCondition = (alcoholLevel == 1 || sleepStatus == 1);
  digitalWrite(buzzerPin, alertCondition ? HIGH : LOW);

  // ====================== PAGE SWITCHING ======================
  if (currentMillis - lastPageSwitch >= pageInterval) {
    currentPage = (currentPage == 0) ? 1 : 0;
    lastPageSwitch = currentMillis;
    lcd.clear();
  }

  // ====================== LCD DISPLAY ======================
  if (currentPage == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Helmet:  ");
    lcd.print(helmetStatus == 1 ? "Yes " : "No  ");

    lcd.setCursor(0, 1);
    lcd.print("Alcohol: ");
    lcd.print(alcoholLevel == 1 ? "Yes " : "No  ");
  } 
  else {
    lcd.setCursor(0, 0);
    lcd.print("Drowsy:  ");
    lcd.print(sleepStatus == 1 ? "Yes " : "No  ");

    lcd.setCursor(0, 1);
    lcd.print("Engine:  ");
    lcd.print(engineState ? "ON  " : "OFF ");
  }

  delay(50);   // Small delay for stability
}