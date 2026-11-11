// SismoEv test 05 — system test: LEDs + buttons + relay (MOSFET) + buzzer
// NORMAL: green on, relay on (fan runs), buzzer off.
// B2 (test) -> ALARM: relay cut (fan stops), red blinks, siren.
// B3 (mute) -> siren off; alarm stays active (red keeps blinking) until power is restored.
// B1 (restore) -> back to NORMAL. Power can only be restored with this physical button.

const int LED_GREEN = 16, LED_YELLOW = 17, LED_RED = 4;
const int BTN_RESTORE = 32, BTN_TEST = 33, BTN_MUTE = 14;
const int RELAY = 26;   // HIGH = relay on (MOSFET driver)
const int BUZZER = 27;  // HIGH = sound

enum State { NORMAL, ALARM };
State state = NORMAL;
bool muted = false;
bool lastRestore = false, lastTest = false, lastMute = false;

void backToNormal() {
  state = NORMAL;
  muted = false;
  digitalWrite(RELAY, HIGH);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("NORMAL: power on, fan running");
}

void startAlarm() {
  state = ALARM;
  muted = false;
  digitalWrite(RELAY, LOW);
  digitalWrite(LED_GREEN, LOW);
  Serial.println("ALARM: shaking! circuit cut, siren on");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_GREEN, OUTPUT); pinMode(LED_YELLOW, OUTPUT); pinMode(LED_RED, OUTPUT);
  pinMode(RELAY, OUTPUT); pinMode(BUZZER, OUTPUT);
  pinMode(BTN_RESTORE, INPUT_PULLUP); pinMode(BTN_TEST, INPUT_PULLUP); pinMode(BTN_MUTE, INPUT_PULLUP);
  backToNormal();
}

void loop() {
  bool restore = digitalRead(BTN_RESTORE) == LOW;
  bool test    = digitalRead(BTN_TEST) == LOW;
  bool mute    = digitalRead(BTN_MUTE) == LOW;

  if (test && !lastTest && state == NORMAL) startAlarm();
  if (mute && !lastMute && state == ALARM) { muted = true; Serial.println("Siren muted"); }
  if (restore && !lastRestore && state == ALARM) backToNormal();
  lastRestore = restore; lastTest = test; lastMute = mute;

  if (state == ALARM) {
    bool on = (millis() / 300) % 2;
    digitalWrite(LED_RED, on);
    digitalWrite(BUZZER, (!muted && on) ? HIGH : LOW);
  }
  delay(20);
}
