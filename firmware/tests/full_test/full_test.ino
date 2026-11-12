// SismoEv test 07 — everything wired so far, in one sketch
// Start: LEDs light one by one, short beep, relay on (fan runs), green LED on.
// Spring switch (GPIO18) hit -> yellow LED flashes for 0.5 s (no alarm, info only).
// B2 (test)    -> ALARM: relay cuts the fan, red blinks, siren.
// B3 (mute)    -> siren off, alarm stays (red keeps blinking).
// B1 (restore) -> back to NORMAL. Power is restored only by this button.
// Serial Monitor: 115200 baud.

const int LED_GREEN = 16, LED_YELLOW = 17, LED_RED = 4;
const int BTN_RESTORE = 32, BTN_TEST = 33, BTN_MUTE = 14;
const int RELAY = 26;   // HIGH = relay on (MOSFET driver)
const int BUZZER = 27;  // HIGH = sound
const int SW = 18;      // spring vibration switch, LOW when it closes

enum State { NORMAL, ALARM };
State state = NORMAL;
bool muted = false;
bool lastRestore = false, lastTest = false, lastMute = false;

volatile unsigned long swHits = 0;
volatile unsigned long swLastMs = 0;
void IRAM_ATTR onSwitch() { swHits++; swLastMs = millis(); }

void backToNormal() {
  state = NORMAL;
  muted = false;
  digitalWrite(RELAY, HIGH);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("NORMAL: elektrik acik, fan donuyor");
}

void startAlarm() {
  state = ALARM;
  muted = false;
  digitalWrite(RELAY, LOW);
  digitalWrite(LED_GREEN, LOW);
  Serial.println("ALARM: elektrik kesildi, siren caliyor");
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nSismoEv tam test");

  pinMode(LED_GREEN, OUTPUT); pinMode(LED_YELLOW, OUTPUT); pinMode(LED_RED, OUTPUT);
  pinMode(RELAY, OUTPUT); pinMode(BUZZER, OUTPUT);
  digitalWrite(RELAY, LOW);
  pinMode(BTN_RESTORE, INPUT_PULLUP); pinMode(BTN_TEST, INPUT_PULLUP); pinMode(BTN_MUTE, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SW), onSwitch, CHANGE);

  // self-test: LEDs one by one, then a short beep
  int leds[] = {LED_GREEN, LED_YELLOW, LED_RED};
  for (int i = 0; i < 3; i++) { digitalWrite(leds[i], HIGH); delay(300); digitalWrite(leds[i], LOW); }
  digitalWrite(BUZZER, HIGH); delay(100); digitalWrite(BUZZER, LOW);
  Serial.println("LED ve buzzer kendi testi bitti");

  backToNormal();
}

void loop() {
  bool restore = digitalRead(BTN_RESTORE) == LOW;
  bool test    = digitalRead(BTN_TEST) == LOW;
  bool mute    = digitalRead(BTN_MUTE) == LOW;

  if (test && !lastTest) {
    Serial.println("B2 (test) basildi");
    if (state == NORMAL) startAlarm();
  }
  if (mute && !lastMute) {
    Serial.println("B3 (sustur) basildi");
    if (state == ALARM) { muted = true; Serial.println("Siren susturuldu, alarm devam ediyor"); }
  }
  if (restore && !lastRestore) {
    Serial.println("B1 (geri ac) basildi");
    if (state == ALARM) backToNormal();
  }
  lastRestore = restore; lastTest = test; lastMute = mute;

  // spring switch: report once per burst (ignore contact bounce)
  static unsigned long reportedHits = 0;
  unsigned long hits = swHits, lastMs = swLastMs, now = millis();
  bool swActive = now - lastMs < 500;
  if (hits != reportedHits && now - lastMs > 300) {
    Serial.println("Yayli anahtar tetiklendi");
    reportedHits = hits;
  }
  digitalWrite(LED_YELLOW, swActive && hits > 0);

  if (state == ALARM) {
    bool on = (now / 300) % 2;
    digitalWrite(LED_RED, on);
    digitalWrite(BUZZER, (!muted && on) ? HIGH : LOW);
  }
  delay(20);
}
