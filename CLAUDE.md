# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses **PlatformIO** with the Arduino framework targeting the ESP32 Dev Module.

```bash
pio run                          # Build
pio run --target upload          # Build and flash to device
pio run --target monitor         # Open serial monitor (115200 baud)
pio run --target upload --target monitor  # Flash and monitor
pio test                         # Run unit tests (PlatformIO Test Runner)
```

## Architecture

Minimal Arduino-style ESP32 project:

- `src/main.cpp` — Single entry point with `setup()` and `loop()` (Arduino API)
- `include/` — Project-wide header files
- `lib/` — Private libraries (self-contained components go here, each in its own subdirectory)
- `test/` — Unit tests for PlatformIO Test Runner
- `platformio.ini` — Board, framework, and monitor configuration

## Key Configuration

**`platformio.ini`:**
- Board: `esp32dev` (Espressif ESP32 Dev Module, 240 MHz)
- Framework: `arduino`
- Serial monitor: 115200 baud with `time` and `default` filters

**Available IDF components** (usable via Arduino abstraction or directly):
- WiFi, BLE, Bluetooth
- SPIFFS, NVS flash storage
- HTTP client/server, MQTT
- GPIO, SPI, I2C, UART drivers
- FreeRTOS
