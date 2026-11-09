# Fog service (Python)

Runs on a home PC. Planned modules:

- MQTT client (EMQX, TLS)
- AFAD verification (`apiv2/event/filter`, time window and radius around home)
- LLM call (Gemini) with JSON schema validation and template fallback
- SQLite event log
- Telegram bot (notifications, voice messages)
- Offline queue

Configuration comes from `../.env` (see `../.env.example`).
