# SismoEv

**Home earthquake protection and notification system with AFAD verification and LLM assistance.**
IoT course project — Tunahan İnci

SismoEv measures earthquake shaking at building level, protects the home locally within half a second
(cuts a selected circuit, turns on emergency light, sounds a siren), verifies events against official
AFAD data, and uses an LLM to classify events and inform the family. The LLM never controls the hardware;
power can only be restored with a physical button.

## Earthquake detection

Three sensors vote on every event:

| Sensor | Principle | Role |
|---|---|---|
| MPU6050 | MEMS accelerometer + gyro | Primary: peak acceleration, tilt |
| ADXL345 | MEMS accelerometer (different maker) | Second opinion |
| SW-18020P | Mechanical spring vibration switch | Independent, noise-immune |

An event counts as an earthquake when at least two sensors agree **and** the shaking looks like an
earthquake: low frequency (1–10 Hz), sustained for seconds (STA/LTA trigger), not a single impulse.
A knock or a slammed door is short and high-frequency, so it is classified as fake.
AFAD data adds a final verification layer.

**Demo requirement:** three shaking trials; the system detects the earthquake-like ones and recognises
one as fake. See [docs/demo-plan.md](docs/demo-plan.md).

## Architecture

| Layer | Runs on | Does |
|---|---|---|
| Edge | ESP32 | Sampling, detection, relay / light / siren, OLED, buttons, UPS battery |
| Cloud | EMQX broker, AFAD API, Gemini API, Telegram | MQTT over TLS, verification data, LLM, notifications |
| Fog | Python service on a home PC | AFAD check, LLM call + JSON validation, SQLite log, Telegram bot, offline queue |

## Repository layout

```
docs/       wiring diagrams, pin plan, demo plan
firmware/   ESP32 code (Arduino)
  tests/    small test sketches for each part
  sismoev/  main firmware
fog/        Python fog service
app/        Flutter web app (live status and history)
```

## Hardware

See [docs/pin-plan.md](docs/pin-plan.md) and the wiring diagram ([EN](docs/wiring_en.png), [TR](docs/wiring_tr.png)).

## Secrets

API keys and passwords are never committed. Copy `.env.example` to `.env` (fog service) and
`firmware/sismoev/secrets.example.h` to `secrets.h` (firmware), then fill in your own values.
Both real files are listed in `.gitignore`.

---

### Türkçe özet

SismoEv, binadaki deprem sarsıntısını üç sensörle (MPU6050, ADXL345, yaylı titreşim sensörü) ölçen,
yarım saniye içinde yerel koruma yapan (hat kesme, acil ışık, siren), olayları AFAD verisiyle doğrulayan
ve LLM ile aileyi bilgilendiren bir IoT projesidir. LLM donanımı hiçbir zaman kontrol etmez; elektrik
yalnızca fiziksel butonla geri açılır.
