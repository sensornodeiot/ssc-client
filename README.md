# End-node guide connecting to Sensorclouds

Sensorclouds-client bundle library for connecting end-node to the [Sensorclouds](https://ssc.sensornodeiot.com) IoT platform. Features a web config portal, WiFi management with auto reconnect, MQTT telemetry publishing, OTA firmware updates, and NVS-based persistent configuration.

## Project Structure

```
esp32/
├── src/main.cpp                  # Entry point: setup() and loop()
├── include/config.h              # AP credentials, boot button pin
├── lib/
│   ├── config_store/             # NVS-based persistent configuration
│   ├── config_portal/            # Captive portal web UI (4-step wizard)
│   ├── ssc_client/               # MQTT client for Sensorclouds platform
│   ├── wifi_manager/             # Non-blocking WiFi state machine
│   └── sensor/                   # Abstract sensor interface
├── test/
│   ├── test_config/              # Config struct validation (native)
│   ├── test_config_store/        # NVS persistence roundtrips (esp32dev)
│   ├── test_config_store_save/   # NVS save edge cases with mocks (native)
│   └── test_ssc_client/          # MQTT topic/payload construction (native)
├── docs/                         # MQTT spec, example sketches
└── platformio.ini                # Build & test configuration
```

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or IDE plugin)
- ESP32 Dev Module
- USB cable

### Build & Flash

```bash
pio run                                       # Build
pio run --target upload                       # Flash to device
pio run --target monitor                      # Serial monitor (115200 baud)
pio run --target upload --target monitor      # Flash and monitor
```

### External Dependencies

| Library                                                   | Version | Purpose           |
| --------------------------------------------------------- | ------- | ----------------- |
| [PubSubClient](https://github.com/knolleary/pubsubclient) | ^2.8    | MQTT 3.1.1 client |

All other libraries (WiFi, Preferences, WebServer, Update) are built into the ESP32 Arduino core.

## Configuration

### Boot Flow

```
Power on
  └─ Load config from NVS
       ├─ Config invalid → Start config portal
       ├─ BOOT button held 2s → Start config portal
       └─ Config valid → Connect WiFi → Connect MQTT → Publish telemetry
```

### Config Portal

| Setting     | Value                                             |
| ----------- | ------------------------------------------------- |
| AP SSID     | `SSN32-Config`                                    |
| AP Password | `ssn32config`                                     |
| Portal URL  | `http://192.168.4.1`                              |
| Trigger     | Hold BOOT button (GPIO0) for 2 seconds at startup |

The portal is a single-page web app with four steps: **Status** → **WiFi** → **MQTT** → **OTA**.

### Config Fields

```cpp
struct Config {
    char wifi_ssid[33];
    char wifi_pass[65];
    char mqtt_host[65];
    uint16_t mqtt_port;            // Default: 1883
    char mqtt_client_id[65];       // Must match SSC Device ID
    char mqtt_username[65];        // Device Token from SSC
    char mqtt_password[65];
    char tenant_id[65];
    char application_id[65];
    uint32_t publish_interval_ms;  // Default: 10000
};
```

Validation helpers: `hasWiFi()` (SSID non-empty), `hasMqtt()` (all MQTT fields non-empty), `isValid()` (both).

## MQTT Reference

### Topics

```
Telemetry:  tenant/{tenantId}/app/{applicationId}/device/{mqtt_username}/data
Status:     tenant/{tenantId}/app/{applicationId}/device/{mqtt_username}/status
```

### Payloads

**Telemetry** (QoS 0):

```json
{ "temperature": 25.5, "humidity": 60.25 }
```

**Birth message** (QoS 1, retained — published on connect):

```json
{ "state": "online" }
```

**Last Will & Testament** (QoS 1, retained — sent by broker on disconnect):

```json
{ "state": "offline", "reason": "lwt" }
```

### Connection Settings

- Keep-alive: 60s
- Buffer size: 512 bytes
- Reconnect: exponential backoff 5s → 60s max

## Testing

Uses the **Unity** framework via PlatformIO Test Runner.

```bash
pio test                  # Run all tests (native environment)
pio test -e native        # Host-only tests
pio test -e esp32dev      # On-device tests (requires connected board)
```

| Suite                    | Environment | Tests                                                                      |
| ------------------------ | ----------- | -------------------------------------------------------------------------- |
| `test_config`            | native      | Config struct validation: `clear()`, `hasWiFi()`, `hasMqtt()`, `isValid()` |
| `test_config_store`      | esp32dev    | NVS save/load roundtrips, empty config, custom intervals                   |
| `test_config_store_save` | native      | NVS save edge cases with mocked Preferences, failure injection             |
| `test_ssc_client`        | native      | MQTT topic format, telemetry/birth/LWT payload construction                |

## Extending the Project

### Adding a Sensor

Subclass `Sensor` from `lib/sensor/sensor.h`:

```cpp
#include "sensor.h"

class DHTSensor : public Sensor {
public:
    bool begin() override {
        // Initialize hardware
        return true;
    }

    SensorReading read() override {
        SensorReading r;
        r.temperature = /* read from hardware */;
        r.humidity    = /* read from hardware */;
        r.valid       = true;
        return r;
    }
};
```

Then instantiate it in `src/main.cpp` and pass readings to `sscClient.publishTelemetry(temp, humidity)`.
