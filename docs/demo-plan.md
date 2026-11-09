# Demo plan — three shaking trials

Instructor's requirement: three separate trials. The system must detect the earthquake-like ones
and recognise one of them as fake.

| Trial | How | Expected result |
|---|---|---|
| 1. Strong earthquake | Board on a tray; move the tray back and forth 2–3 times per second for 5–8 s | EARTHQUAKE: relay cuts, light on, siren |
| 2. Light earthquake | Slower, gentler back-and-forth motion for 5–8 s | EARTHQUAKE (light) |
| 3. Fake | One hard knock on the table, or a slammed door | FAKE: impulse / local vibration; no protection |

## How the system tells them apart

| Feature | Earthquake | Knock / door |
|---|---|---|
| Duration | seconds | a fraction of a second |
| Frequency | low, about 1–10 Hz | high, sharp spike |
| Sensor agreement | 2 or 3 of 3 sensors see sustained shaking | usually one sharp spike, no sustained motion |

Detection steps on the ESP32: band-pass filter (1–10 Hz) → STA/LTA trigger → minimum duration →
2-of-3 sensor vote. AFAD verification is an extra layer on top.

Each trial is logged (time, peak g per sensor, duration, dominant frequency, decision) so the
results can go straight into the final report's test table.
