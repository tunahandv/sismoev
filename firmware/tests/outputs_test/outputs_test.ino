// SismoEv test 03 — LEDs, buttons, buzzer, relay
// Serial Monitor: 115200 baud.
//
// Wiring (see docs/pin-plan.md):
//   Green LED  GPIO16 -> 330R -> LED(+, long leg) ; LED(-, short leg) -> GND
//   Yellow LED GPIO17 -> 330R -> LED(+)           ; LED(-) -> GND
//   Red LED    GPIO4  -> 330R -> LED(+)           ; LED(-) -> GND
//   Button B1  GPIO32 -> button -> GND  (restore power)
//   Button B2  GPIO33 -> button -> GND  (test)
//   Button B3  GPIO14 -> button -> GND  (mute)
//   Buzzer module: VCC -> 5V (VIN), GND -> GND, I/O -> GPIO27
//   Relay module : VCC -> 5V (VIN), GND -> GND, IN  -> GPIO26
//
// What it does:
//   On start the three LEDs light up one after another.
//   B1 -> green LED, B2 -> yellow LED, B3 -> red LED + short beep.
//   Serial commands: r = toggle relay, b = beep, a = all LEDs on/off.

const int LED_GREEN  = 16;
const int LED_YELLOW = 17;
const int LED_RED    = 4;
const int BTN_RESTORE = 32;
const int BTN_TEST    = 33;
const int BTN_MUTE    = 14;
const int BUZZER = 27;
const int RELAY  = 26;

bool relayOn = false;
bool allLeds = false;

// The relay module is low-level triggered and powered from 5 V.
// A 3.3 V HIGH may not switch it fully off, so "off" releases the pin
// (input mode) and the module's own pull-up keeps it off.
void setRelay(bool on) {
  if (on) {
    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, LOW);
  } else {
    pinMode(RELAY, INPUT);
  }
  relayOn = on;
  Serial.printf("Relay %s\n", on ? "ON (circuit closed)" : "OFF");
}

void beep(int ms) {
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nSismoEv output test");

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN_RESTORE, INPUT_PULLUP);
  pinMode(BTN_TEST, INPUT_PULLUP);
  pinMode(BTN_MUTE, INPUT_PULLUP);
  setRelay(false);

  int leds[] = {LED_GREEN, LED_YELLOW, LED_RED};
  for (int i = 0; i < 3; i++) {
    digitalWrite(leds[i], HIGH);
    delay(400);
    digitalWrite(leds[i], LOW);
  }
  Serial.println("Ready. Press buttons, or type r / b / a and Enter.");
}

void loop() {
  bool b1 = digitalRead(BTN_RESTORE) == LOW;
  bool b2 = digitalRead(BTN_TEST) == LOW;
  bool b3 = digitalRead(BTN_MUTE) == LOW;

  if (!allLeds) {
    digitalWrite(LED_GREEN, b1);
    digitalWrite(LED_YELLOW, b2);
    digitalWrite(LED_RED, b3);
  }

  static bool lastB1 = false, lastB2 = false, lastB3 = false;
  if (b1 && !lastB1) Serial.println("B1 (restore) pressed");
  if (b2 && !lastB2) Serial.println("B2 (test) pressed");
  if (b3 && !lastB3) { Serial.println("B3 (mute) pressed"); beep(80); }
  lastB1 = b1; lastB2 = b2; lastB3 = b3;

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r') setRelay(!relayOn);
    if (c == 'b') { Serial.println("Beep"); beep(300); }
    if (c == 'a') {
      allLeds = !allLeds;
      digitalWrite(LED_GREEN, allLeds);
      digitalWrite(LED_YELLOW, allLeds);
      digitalWrite(LED_RED, allLeds);
      Serial.printf("All LEDs %s\n", allLeds ? "ON" : "OFF");
    }
  }
  delay(20);
}
