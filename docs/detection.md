# Earthquake detection logic

All three sensors are mounted on the same board and read every 10 ms (100 Hz).

| Sensor | Measures | Role |
|---|---|---|
| MPU6050 (0x68) | 3-axis acceleration | main accelerometer |
| ADXL345 (0x53) | 3-axis acceleration | second opinion |
| SW-18020P (GPIO18) | vibration yes/no | simple independent vote |

Each shaking event passes three checks:

1. **Trigger (STA/LTA):** 0.5 s average shaking vs 10 s average; ratio above ~3 starts an event.
2. **Duration and frequency:** an earthquake is a back-and-forth motion at roughly 1–10 Hz lasting
   at least ~1.5 s. A knock is a single sharp spike lasting 0.1–0.3 s and is labelled **FAKE**.
3. **2-of-3 vote:** at least two sensors must agree.

Passing all three → **EARTHQUAKE** (relay cuts power, siren, red LED). Severity (LIGHT / STRONG)
comes from the peak amplitude. AFAD is then queried; in the demo the expected label is
"local detection, no AFAD record".

The numbers above are starting values; they are tuned from recorded data of the real setup.

Demo: strong shake 3–4 s → EARTHQUAKE (STRONG); gentle shake → EARTHQUAKE (LIGHT);
single knock on the table → FAKE, no alarm.
