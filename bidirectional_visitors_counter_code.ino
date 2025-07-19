#define BLYNK_TEMPLATE_ID "TMPL3o05gVhCH"
#define BLYNK_TEMPLATE_NAME "Bidirectional12"
#define BLYNK_AUTH_TOKEN "Pg0HCcQmZvEj75ZKgM5OJigTtf-4wIpD"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
char ssid[] = "One Plus 7";       // Replace with your WiFi name
char pass[] = "Atharv@123";       // Replace with your WiFi password

// IR Sensor Pins
const int irEntry = D5;
const int irExit = D6;

// Buzzer & LED
const int buzzerPin = D7;
const int ledPin = D4;

// Counters
int entryCount = 0;
int exitCount = 0;

// LCD and NTP Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // IST offset

void setup() {
  Serial.begin(115200);

  pinMode(irEntry, INPUT_PULLUP);
  pinMode(irExit, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");
  }

  Serial.println("\nWiFi connected");
  lcd.clear();
  lcd.print("WiFi Connected");

  Blynk.config(BLYNK_AUTH_TOKEN);
  unsigned long start = millis();
  while (!Blynk.connect() && millis() - start < 10000) {
    Serial.print(".");
    delay(500);
  }

  lcd.setCursor(0, 1);
  if (Blynk.connected()) {
    Serial.println("Blynk connected");
    lcd.print("Blynk OK     ");
  } else {
    Serial.println("Blynk not connected");
    lcd.print("Blynk Fail   ");
  }

  timeClient.begin();
  delay(1000);
  lcd.clear();
  updateLCD();
}

void loop() {
  Blynk.run();
  timeClient.update();

  if (digitalRead(irEntry) == LOW) {
    entryCount++;
    Serial.println("Entry: " + String(entryCount));
    beep();
    digitalWrite(ledPin, HIGH);
    updateLCD();
    delay(500);
  }

  if (digitalRead(irExit) == LOW) {
    exitCount++;
    Serial.println("Exit: " + String(exitCount));
    beep();
    digitalWrite(ledPin, HIGH);
    updateLCD();
    delay(500);
  }

  digitalWrite(ledPin, LOW);
  delay(200);
}

void updateLCD() {
  int total = entryCount - exitCount;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IN:");
  lcd.print(entryCount);
  lcd.setCursor(8, 0);
  lcd.print("OUT:");
  lcd.print(exitCount);
  lcd.setCursor(0, 1);
  lcd.print("TOTAL:");
  lcd.print(total);

  Blynk.virtualWrite(V0, entryCount);
  Blynk.virtualWrite(V1, exitCount);
  Blynk.virtualWrite(V2, total);
}

void beep() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
}