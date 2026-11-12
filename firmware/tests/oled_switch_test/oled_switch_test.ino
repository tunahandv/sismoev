// SismoEv test 06 — OLED + SW-18020P vibration switch (no soldering needed)
// Wiring: 3V3 bus row 48, SDA bus row 49 (GPIO21), SCL bus row 50 (GPIO22).
// OLED VCC->3V3 bus, GND->GND, SDA->row 49, SCL->row 50.
// SW-18020P: one leg -> GPIO18, other leg -> GND (INPUT_PULLUP).
// Serial Monitor 115200: lists I2C devices, then counts switch hits.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int SW = 18;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
bool oledOk = false;
volatile unsigned long hits = 0;
volatile unsigned long lastHitMs = 0;

void IRAM_ATTR onHit() {
  hits++;
  lastHitMs = millis();
}

void scanI2C() {
  Serial.println("I2C scan:");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) Serial.printf("  found 0x%02X\n", addr);
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Wire.begin(21, 22);
  scanI2C();

  // begin() does not check that the screen answers, so ask the bus first.
  Wire.beginTransmission(0x3C);
  bool present = Wire.endTransmission() == 0;
  oledOk = present && oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println(oledOk ? "OLED OK" : "OLED NOT FOUND (check wires / address)");

  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SW), onHit, CHANGE);
}

void loop() {
  static unsigned long shown = 0;
  bool active = millis() - lastHitMs < 300;

  if (hits != shown) {
    shown = hits;
    Serial.printf("Switch hits: %lu\n", shown);
  }

  if (oledOk) {
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.println("SismoEv");
    oled.setTextSize(1);
    oled.setCursor(0, 24);
    oled.printf("Yay anahtar: %lu", shown);
    oled.setCursor(0, 40);
    oled.println(active ? ">> TITRESIM <<" : "sakin");
    oled.display();
  }
  delay(50);
}
