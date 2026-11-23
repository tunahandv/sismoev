// SismoEv test 09 — event logger: measures each shake/knock with both MPU6050s
// and prints one summary line per event, plus a first-guess decision.
// The numbers are used to set the real thresholds (docs/detection.md).
// Serial Monitor 115200.
//
// Per event:
//   sure      : how long the shaking lasted (ms)
//   tepe M1/M2: strongest shaking of each sensor (m/s2, gravity removed)
//   ritim     : back-and-forth rate (Hz) of each sensor
//   anahtar   : did the spring switch fire

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---- starting thresholds (to be tuned from recorded data) ----
const float START_THR   = 0.8;   // m/s2: shaking above this starts an event
const float END_THR     = 0.5;   // m/s2: below this (both sensors) for QUIET_MS ends it
const unsigned long QUIET_MS = 400;
const unsigned long MIN_QUAKE_MS = 1000;  // shorter = knock
const float MIN_HZ = 0.8, MAX_HZ = 12.0;  // quake-like rhythm
const float SENSOR_AGREE = 1.0;           // both sensors must peak above this
const float STRONG_PEAK  = 6.0;           // m/s2: above = strong, below = light
const float CROSS_HYST   = 0.3;           // m/s2: minimum wiggle counted when measuring rhythm
const float CROSS_FRAC   = 0.3;           // ...and at least this share of the event's peak

const int SW = 18;
Adafruit_MPU6050 mpu[2];
bool ok[2] = {false, false};

volatile bool swFired = false;
void IRAM_ATTR onSwitch() { swFired = true; }

struct Track {
  float base[3] = {0, 0, 0};   // slow average = gravity + tilt
  bool primed = false;
  float peak = 0;
  int crossX = 0, crossY = 0;
  int signX = 0, signY = 0;
};
Track tr[2];

// Label typed in the Serial Monitor before a trial: g = guclu, h = hafif, v = vurus
char label = '-';
bool inEvent = false;
unsigned long startMs = 0, lastLoudMs = 0, eventNo = 0;

float readDyn(int i, float &dx, float &dy) {
  sensors_event_t a, g, t;
  mpu[i].getEvent(&a, &g, &t);
  float v[3] = {a.acceleration.x, a.acceleration.y, a.acceleration.z};
  Track &k = tr[i];
  if (!k.primed) { for (int j = 0; j < 3; j++) k.base[j] = v[j]; k.primed = true; }
  float d[3];
  for (int j = 0; j < 3; j++) {
    d[j] = v[j] - k.base[j];
    k.base[j] += 0.02f * d[j];   // about 0.5 s time constant at 100 Hz
  }
  dx = d[0]; dy = d[1];
  return sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
}

void countCross(int &sign, int &count, float val, float peak) {
  float h = max(CROSS_HYST, CROSS_FRAC * peak);
  int s = val > h ? 1 : (val < -h ? -1 : 0);
  if (s != 0 && s != sign) { if (sign != 0) count++; sign = s; }
}

void resetTracks() {
  for (int i = 0; i < 2; i++) {
    tr[i].peak = 0; tr[i].crossX = tr[i].crossY = 0; tr[i].signX = tr[i].signY = 0;
  }
  swFired = false;
}

void finishEvent(unsigned long now) {
  eventNo++;
  unsigned long dur = lastLoudMs - startMs;
  float secs = max(dur, 1UL) / 1000.0f;
  float hz[2];
  for (int i = 0; i < 2; i++) hz[i] = max(tr[i].crossX, tr[i].crossY) / 2.0f / secs;

  const char *verdict;
  bool bothAgree = (!ok[0] || tr[0].peak > SENSOR_AGREE) && (!ok[1] || tr[1].peak > SENSOR_AGREE);
  float hzMain = ok[0] ? hz[0] : hz[1];
  float peakAvg = (tr[0].peak + tr[1].peak) / ((ok[0] && ok[1]) ? 2 : 1);
  if (dur < MIN_QUAKE_MS)                       verdict = "SAHTE (kisa darbe)";
  else if (hzMain < MIN_HZ || hzMain > MAX_HZ)  verdict = "SAHTE (ritim uymuyor)";
  else if (!bothAgree)                          verdict = "SAHTE (sensorler uyusmuyor)";
  else if (peakAvg >= STRONG_PEAK)              verdict = "DEPREM - GUCLU";
  else                                          verdict = "DEPREM - HAFIF";

  Serial.printf("[%c] Olay %lu | sure %lu ms | tepe M1 %.1f M2 %.1f | ritim M1 %.1f Hz M2 %.1f Hz | anahtar %s | => %s\n",
                label, eventNo, dur, tr[0].peak, tr[1].peak, hz[0], hz[1], swFired ? "evet" : "hayir", verdict);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22);
  Wire.setClock(400000);
  ok[0] = mpu[0].begin(0x68, &Wire);
  ok[1] = mpu[1].begin(0x69, &Wire);
  Serial.printf("MPU #1 %s, MPU #2 %s\n", ok[0] ? "OK" : "YOK", ok[1] ? "OK" : "YOK");
  for (int i = 0; i < 2; i++) if (ok[i]) {
    mpu[i].setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu[i].setFilterBandwidth(MPU6050_BAND_44_HZ);
  }
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SW), onSwitch, CHANGE);
  Serial.println("Hazir. 3 sn dokunma (sensorler oturuyor).");
  Serial.println("Her denemeden once etiket yaz + Enter: g = guclu, h = hafif, v = vurus");
  delay(3000);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'g' || c == 'h' || c == 'v') {
      label = c;
      Serial.printf("Etiket: %s\n", c == 'g' ? "GUCLU sallama" : c == 'h' ? "HAFIF sallama" : "VURUS");
    }
  }
  static unsigned long next = millis();
  unsigned long now = millis();
  if ((long)(now - next) < 0) return;
  next += 10;  // 100 Hz

  float m[2] = {0, 0};
  for (int i = 0; i < 2; i++) if (ok[i]) {
    float dx, dy;
    m[i] = readDyn(i, dx, dy);
    if (inEvent) {
      tr[i].peak = max(tr[i].peak, m[i]);
      countCross(tr[i].signX, tr[i].crossX, dx, tr[i].peak);
      countCross(tr[i].signY, tr[i].crossY, dy, tr[i].peak);
    }
  }
  float loud = max(m[0], m[1]);

  if (!inEvent && loud > START_THR) {
    inEvent = true; startMs = lastLoudMs = now; resetTracks();
    for (int i = 0; i < 2; i++) tr[i].peak = m[i];
  } else if (inEvent) {
    if (loud > END_THR) lastLoudMs = now;
    else if (now - lastLoudMs > QUIET_MS) { inEvent = false; finishEvent(now); }
  }
}
