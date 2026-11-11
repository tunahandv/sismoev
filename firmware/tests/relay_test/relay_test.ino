// SismoEv test 04 — relay + fan through an IRLZ44N MOSFET
// GPIO26 -> 330R -> MOSFET gate; drain -> relay IN; source -> GND; 10k gate->source.
// HIGH = relay on (fan runs), LOW = relay off. Serial Monitor: 115200 baud.

const int RELAY = 26;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  Serial.println("Relay test (MOSFET driver)");
}

void loop() {
  digitalWrite(RELAY, HIGH);
  Serial.println("Relay ON  -> fan should run");
  delay(3000);
  digitalWrite(RELAY, LOW);
  Serial.println("Relay OFF -> fan should stop");
  delay(3000);
}
