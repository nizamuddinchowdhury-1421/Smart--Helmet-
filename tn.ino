/*
  Smart Helmet Transmitter - FIXED & CORRECTED
  Alcohol sensor logic fixed + all variables properly declared
*/

#include <RH_ASK.h>
#include <SPI.h>

#define DEBUG_MODE true

// ====================== PINS ======================
const int alcoholSensorPin = A0;   // MQ-3 Digital Out
const int wearSensorPin    = A1;   // Wear Detection IR Sensor
const int sleepSensorPin   = A2;   // Sleep Detection IR Sensor
const int buzzerPin        = 7;    // Buzzer
const int ledPin           = 8;    // LED (visual alert)

// ====================== SETTINGS ======================
const unsigned long sendInterval = 200;           // Send data every 200ms

// Drowsiness Detection Settings
const int drowsinessThreshold = 5;                // 5 detections needed
const unsigned long drowsinessTimeWindow = 8000;  // within 8 seconds
const unsigned long resetTime = 6000;             // reset after 6s of no signal

RH_ASK rf_driver(2000);   // 2000 bps speed

// Drowsiness variables
unsigned long sleepTimestamps[5] = {0};
int sleepIndex = 0;
unsigned long lastSleepDetectionTime = 0;
bool isDrowsy = false;

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Smart Helmet Transmitter Starting ===");
  Serial.println("Buzzer on Pin 7 | LED on Pin 8");

  // Initialize RF module
  if (!rf_driver.init()) {
    Serial.println("RF Driver initialization FAILED!");
    while(1);  // stop if RF fails
  }
  Serial.println("RF Driver initialized successfully.");

  // Pin modes
  pinMode(alcoholSensorPin, INPUT);
  pinMode(wearSensorPin,    INPUT);
  pinMode(sleepSensorPin,   INPUT);
  pinMode(buzzerPin,        OUTPUT);
  pinMode(ledPin,           OUTPUT);

  // Ensure buzzer and LED are OFF initially
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);

  // Startup Test: Buzzer + LED for 1 second
  Serial.println("Testing Buzzer and LED for 1 second...");
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);
  Serial.println("Test complete. Transmitter is now ready!");
  Serial.println("=========================================");
}

void loop() {
  unsigned long currentMillis = millis();

  // Read all sensors
  int rawAlcohol   = digitalRead(alcoholSensorPin);   // Raw reading from MQ-3
  int helmetStatus = digitalRead(wearSensorPin);
  int sleepSignal  = digitalRead(sleepSensorPin);

  // ====================== ALCOHOL DETECTION (FIXED) ======================
  int alcoholLevel = (rawAlcohol == LOW) ? 1 : 0;     // LOW on sensor = Alcohol detected

  // ====================== DROWSINESS DETECTION ======================
  if (sleepSignal == LOW) {   // Change to HIGH if your IR sensor outputs HIGH when eye closed
    lastSleepDetectionTime = currentMillis;

    sleepTimestamps[sleepIndex] = currentMillis;
    sleepIndex = (sleepIndex + 1) % 5;

    // Count how many detections in the time window
    int count = 0;
    for (int i = 0; i < 5; i++) {
      if (currentMillis - sleepTimestamps[i] <= drowsinessTimeWindow) {
        count++;
      }
    }
    isDrowsy = (count >= drowsinessThreshold);
  }

  // Reset drowsiness if no signal for a while
  if (currentMillis - lastSleepDetectionTime > resetTime) {
    isDrowsy = false;
  }

  // ====================== BUZZER + LED CONTROL (FIXED) ======================
  if (isDrowsy) {
    digitalWrite(buzzerPin, LOW);   // ON when drowsy
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(buzzerPin, HIGH);    // OFF when not drowsy
    digitalWrite(ledPin, HIGH);
  }

  // ====================== RF TRANSMISSION ======================
  if (currentMillis - lastSendTime >= sendInterval) {
    lastSendTime = currentMillis;

    // Format: helmetStatus,alcoholLevel,drowsy(1/0)
    String data = String(helmetStatus) + "," +
                  String(alcoholLevel) + "," +
                  String(isDrowsy ? 0 : 1);

    rf_driver.send((uint8_t *)data.c_str(), data.length());
    rf_driver.waitPacketSent();

    #if DEBUG_MODE
      Serial.print("Sent: ");
      Serial.print(data);
      Serial.print(" | Helmet:");
      Serial.print(helmetStatus);
      Serial.print(" | Alcohol:");
      Serial.print(alcoholLevel);
      Serial.print(" | Drowsy:");
      Serial.println(isDrowsy ? "NO" : "YES");
    #endif
  }

  delay(10);  // Small delay for stability
}