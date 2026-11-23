// SismoEv test 08 — MPU6050 sensors (0x68 and 0x69)
// Wiring: 3V3 bus row 48, SDA bus row 49 (GPIO21), SCL bus row 50 (GPIO22).
// MPU #1: AD0 -> GND (0x68). MPU #2: AD0 -> 3V3 (0x69).
// Serial Monitor 115200 (or Serial Plotter): acceleration of each sensor, 10 times a second.
// Lying still: X ~ 0, Y ~ 0, Z ~ 9.8 m/s2 (gravity). Shake it and the numbers jump.

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu1, mpu2;
bool ok1 = false, ok2 = false;

void scanI2C() {
  Serial.println("I2C scan:");
  int n = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) { Serial.printf("  found 0x%02X\n", addr); n++; }
  }
  if (n == 0) Serial.println("  nothing found: check 3V3, GND, SDA, SCL wires");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22);
  scanI2C();

  ok1 = mpu1.begin(0x68, &Wire);
  ok2 = mpu2.begin(0x69, &Wire);
  Serial.println(ok1 ? "MPU #1 (0x68) OK" : "MPU #1 (0x68) NOT FOUND");
  Serial.println(ok2 ? "MPU #2 (0x69) OK" : "MPU #2 (0x69) not connected");
  if (ok1) { mpu1.setAccelerometerRange(MPU6050_RANGE_4_G); mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ); }
  if (ok2) { mpu2.setAccelerometerRange(MPU6050_RANGE_4_G); mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ); }
}

void printSensor(const char *name, Adafruit_MPU6050 &m) {
  sensors_event_t a, g, t;
  m.getEvent(&a, &g, &t);
  Serial.printf("%s_X:%.2f %s_Y:%.2f %s_Z:%.2f  ", name, a.acceleration.x, name,
                a.acceleration.y, name, a.acceleration.z);
}

void loop() {
  if (ok1) printSensor("M1", mpu1);
  if (ok2) printSensor("M2", mpu2);
  if (ok1 || ok2) Serial.println();
  delay(100);
}
