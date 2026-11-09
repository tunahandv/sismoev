// SismoEv test 02 — I2C scanner
// Lists every device on the I2C bus (SDA = GPIO21, SCL = GPIO22).
// Expected: 0x3C (OLED), 0x53 (ADXL345), 0x68 (MPU6050).
// Open Tools > Serial Monitor at 115200 baud.

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22);
  Serial.println("\nSismoEv I2C scanner");
}

void loop() {
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Found device at 0x%02X", addr);
      if (addr == 0x3C) Serial.print("  (OLED)");
      if (addr == 0x53) Serial.print("  (ADXL345)");
      if (addr == 0x68) Serial.print("  (MPU6050)");
      Serial.println();
      found++;
    }
  }
  Serial.printf("%d device(s) found\n\n", found);
  delay(3000);
}
