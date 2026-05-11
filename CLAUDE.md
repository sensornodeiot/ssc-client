# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

PlatformIO + Arduino framework, targeting ESP32 Dev Module. Two environments:

- `esp32dev` — on-device build/flash (default)
- `native` — host build for unit tests that don't require hardware

```bash
pio run                                       # Build (esp32dev)
pio run --target upload                       # Flash to device
pio run --target upload --target monitor      # Flash + serial monitor (115200)
pio test                                      # Run all tests
pio test -e native                            # Host-only tests
pio test -e esp32dev                          # On-device tests (requires board)
pio test -e native -f test_ssc_client         # Run a single suite
```

The `native` env sets `build_src_filter = -<*>` and `lib_ignore = config_store` so it links only against pure-C++ code, not Arduino/ESP-IDF. When adding new test suites that touch hardware APIs, list them under `test_ignore` in the `[env:native]` section.

## Repository Intent

This codebase is a **library/template for downstream developers** to connect their own ESP32 devices to the Sensorclouds platform. The downstream user should touch only `src/main.cpp` (instantiate `SSCApp`, optionally wire a `Sensor*`) and add a `Sensor` subclass. **Avoid adding new wiring or app-level glue to `main.cpp`** — push it into `lib/ssc_app/` so it stays inside the library surface.

## Architecture

**`SSCApp` is the orchestrator** (`lib/ssc_app/`). It owns all global state (config, WiFi, MQTT, OTA, command handlers, sensor pointer) and exposes a 3-method public surface: `setSensor()`, `begin()`, `update()`. `src/main.cpp` is a ~16-line thin entry that instantiates it.

**Library composition** under `lib/`:

- `ssc_app` — Top-level orchestrator. Holds singletons of the rest.
- `config_store` — NVS-backed `Config` persistence (uses `Preferences`)
- `config_portal` — Captive-portal web UI; **blocks** in `startConfigPortal()` until user submits, then reboots
- `wifi_manager` — Non-blocking WiFi state machine with exponential backoff
- `ssc_client` — Wraps `PubSubClient`; owns MQTT topic construction (`tenant/{tenantId}/app/{applicationId}/device/{mqtt_username}/{data|status|cmd|ack}`), birth/LWT, reconnect
- `ota_handler` — Receives `ota:*` commands, runs state machine (download → SHA-256 verify → write to inactive OTA partition → reboot), persists progress to NVS so it can ACK after reboot
- `config_cmd_handler` — Receives `config:*` commands. Currently handles `config:set_interval` (bounds: `MIN_INTERVAL_MS`..`MAX_INTERVAL_MS` in the header), persists via `ConfigStore::save()`, ACKs back. Designed to grow more `config:*` subcommands via its internal switch.
- `sensor` — Abstract `Sensor` base class; downstream user subclasses it and passes the pointer to `SSCApp::setSensor()`

**Command routing** — `SSCClient` exposes a single MQTT command callback (C-style `void(*)(...)`, no captures). `SSCApp` registers a **static** member function and uses `SSCApp::instance_` (set in `begin()`) to forward into a non-static `dispatchCommand()`. The dispatcher parses JSON once and routes by `command` prefix: `ota:*` → `OtaHandler`, `config:*` → `ConfigCmdHandler`. **Do not add ad-hoc MQTT subscriptions elsewhere.** New command families should add a new handler library and another `else if` branch in `SSCApp::dispatchCommand()`.

**Periodic telemetry** — Driven by `SSCApp::update()` using `millis()` and `config.publish_interval_ms`. Publishes only when `userSensor_ != nullptr`, MQTT is connected, and `SensorReading::valid == true`. **Never default to publishing placeholder values** — a downstream user shipping unmodified code should not emit fake telemetry. The interval is live-mutable: `config:set_interval` updates the same `Config` object the scheduler reads, so changes take effect on the next tick.

**OTA persistence across reboot** — `OtaHandler` writes progress state to NVS before rebooting. On boot, after MQTT connects, `SSCApp::update()` calls `otaHandler.checkPendingAck()` exactly once (guarded by `otaAckChecked_`) to publish the final ACK with the new `FIRMWARE_VERSION`. The platform compares this string to determine update success, so bump the `-D FIRMWARE_VERSION=` build flag in `platformio.ini` on each release.

**Partition layout** — `board_build.partitions = min_spiffs.csv` provides dual ~1.9 MB OTA slots required for the OTA flow. Don't change this without updating `OtaHandler`.

**Config validation** — `Config::isValid()` = `hasWiFi() && hasMqtt()`. `SSCApp::begin()` enters the portal if invalid or if the BOOT button (GPIO0, `OTA_TRIGGER_PIN` from `include/config.h`) is held at reset.

## Reference Docs

- `README.md` — User-facing overview, MQTT topic/payload spec, OTA flow, `config:set_interval` usage, sensor wiring template
- `docs/SSC_CLIENT_REMOTE_OTA_SPEC.md` — Protocol contract for OTA commands and ACK states (authoritative wire format for **all** `/cmd` and `/ack` messages, not just OTA)
- `docs/OTA_IMPLEMENTATION_PLAN.md` — Design rationale for the OTA state machine
