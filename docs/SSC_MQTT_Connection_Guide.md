# SSC (SensorClouds) MQTT Connection Guide

> **Scope:** Development only — Plain TCP, Port **1883** (unencrypted).  
> This guide is intended as the reference specification for building an Arduino library that connects to SSC over Wi-Fi and supports web-portal-based configuration.

---

## Table of Contents

1. [Overview](#1-overview)
2. [MQTT Broker Details (Development)](#2-mqtt-broker-details-development)
3. [Device Credentials](#3-device-credentials)
4. [Connection Parameters](#4-connection-parameters)
5. [Topic Structure](#5-topic-structure)
6. [Payload Format](#6-payload-format)
7. [Last Will and Testament (LWT) — Device Status](#7-last-will-and-testament-lwt--device-status)
8. [Connection Sequence](#8-connection-sequence)
9. [Arduino Library Design Guide](#9-arduino-library-design-guide)
10. [Web Portal Configuration Parameters](#10-web-portal-configuration-parameters)
11. [Code Example — Arduino / ESP8266 / ESP32 (Plain TCP)](#11-code-example--arduino--esp8266--esp32-plain-tcp)
12. [QoS Reference](#12-qos-reference)
13. [Troubleshooting](#13-troubleshooting)
14. [Security Notice](#14-security-notice)

---

## 1. Overview

**SensorClouds (SSC)** is an IoT platform reachable via MQTT. Devices authenticate with a per-device token and password, publish sensor telemetry to a structured topic, and use Last Will and Testament (LWT) messages to signal online/offline status.

This guide covers the **Development / Plain TCP** connection mode only:

| Mode | Protocol | Port | Use Case |
|------|----------|------|----------|
| Plain TCP *(this guide)* | MQTT | **1883** | Local development & testing |
| Direct TLS | MQTTS | 8883 | Production IoT devices |
| WebSocket TLS | WSS | 443 | Web apps / firewalled networks |

> ⚠️ **Never use plain TCP (port 1883) in production.** Credentials and data are transmitted unencrypted.

---

## 2. MQTT Broker Details (Development)

| Parameter | Value |
|-----------|-------|
| **Host** | `mqtt.sensornodeiot.com` |
| **Port** | `1883` |
| **Protocol** | MQTT (plain TCP, unencrypted) |
| **TLS** | Disabled |
| **URL** | `mqtt://mqtt.sensornodeiot.com:1883` |

---

## 3. Device Credentials

All credentials are **per-device** and are found under the device's **Settings** tab in the SSC portal.

| Credential | Where to Find It | Notes |
|------------|-----------------|-------|
| **Username** | Device Settings → Device Token | Auto-generated; unique per device |
| **Password** | Device Settings → "Generate Password" | Must be explicitly generated once |
| **Client ID** | Device Settings → Device ID | Must exactly match the Device ID |

> 💡 The **Client ID must exactly match** the Device ID registered in SSC. Mismatches will cause connection refusal.

---

## 4. Connection Parameters

These are the recommended MQTT connection parameters for development use:

```
Host:             mqtt.sensornodeiot.com
Port:             1883
Client ID:        <DEVICE_ID>             // From Device Settings
Username:         <DEVICE_TOKEN>          // From Device Settings
Password:         <GENERATED_PASSWORD>    // Generated in Device Settings
Keep-Alive:       60 seconds
Clean Session:    true
QoS (telemetry): 0 or 1
QoS (status/LWT): 1
```

---

## 5. Topic Structure

Topics are **device-specific** and must be copied from the device's **Settings → MQTT Topics** section in the SSC portal.

### 5.1 Telemetry Topic (Publish)

```
tenant/{tenantId}/app/{applicationId}/device/{deviceToken}/data
```

| Segment | Description |
|---------|-------------|
| `{tenantId}` | Your tenant identifier (e.g., `sensornode-uat`) |
| `{applicationId}` | UUID of your SSC application |
| `{deviceToken}` | The device's unique token (same as MQTT username) |
| `data` | Fixed suffix for telemetry payloads |

**Example:**
```
tenant/sensornode-uat/app/aaf00bbb-3b09-4616-b483-410ccdb81c51/device/abc123token/data
```

### 5.2 Status Topic (Publish / Subscribe — LWT)

The status topic follows the same pattern but ends in `/status` (retrieve the exact value from Device Settings → MQTT Topics):

```
tenant/{tenantId}/app/{applicationId}/device/{deviceToken}/status
```

> Always copy the **exact topics** from Device Settings. Do not construct them manually to avoid errors.

---

## 6. Payload Format

All payloads use **JSON**.

### 6.1 Telemetry Payload

Send key-value pairs of sensor readings:

```json
{
  "temperature": 25.5,
  "humidity": 60.2,
  "pressure": 1013.25
}
```

- Keys are arbitrary sensor field names defined in your SSC application.
- Values can be numbers, strings, or booleans.
- No envelope/wrapper is required — flat JSON is preferred.

### 6.2 Status Payload — Online (Birth Message)

Published **immediately after** a successful MQTT connection:

```json
{
  "state": "online",
  "timestamp": "2026-03-16T13:00:00Z"
}
```

### 6.3 Status Payload — Offline (LWT Will Message)

Configured **before** connecting as the LWT will payload:

```json
{
  "state": "offline",
  "timestamp": "2026-03-16T13:00:00Z",
  "reason": "lwt"
}
```

> The `timestamp` field is **optional** — SSC will use server time if omitted.  
> The `reason` field is **optional** and used for audit logs.

---

## 7. Last Will and Testament (LWT) — Device Status

LWT allows SSC to detect when a device disconnects unexpectedly.

### 7.1 How it Works

1. The device configures a **Will Message** before connecting.
2. If the device disconnects unexpectedly (power loss, crash, network drop), the MQTT broker automatically publishes the Will Message on the device's behalf.
3. SSC receives the offline message and marks the device as **offline** within ~90 seconds (1.5 × keep-alive interval).

### 7.2 LWT Configuration

Configure these parameters on the MQTT client **before calling `connect()`**:

| LWT Parameter | Value |
|---------------|-------|
| **Will Topic** | `<your status topic>` (from Device Settings) |
| **Will Payload** | `{"state":"offline","reason":"lwt"}` |
| **Will QoS** | `1` |
| **Will Retain** | `true` |

### 7.3 Birth Message

After a successful connection, immediately publish the online status:

| Parameter | Value |
|-----------|-------|
| **Topic** | `<your status topic>` |
| **Payload** | `{"state":"online"}` |
| **QoS** | `1` |
| **Retain** | `true` |

### 7.4 Keep-Alive & Detection Timing

| Parameter | Recommended Value |
|-----------|------------------|
| Keep-Alive interval | 60 seconds |
| Offline detection time | ~90 seconds (1.5 × keep-alive) |

---

## 8. Connection Sequence

The full recommended sequence for establishing an SSC MQTT connection:

```
1. Load credentials from non-volatile storage (NVS / EEPROM / SPIFFS)
   - Wi-Fi SSID & password
   - MQTT host, port, client ID, username, password
   - Telemetry topic, status topic

2. Connect to Wi-Fi

3. Configure MQTT client:
   a. Set broker host & port (1883 for dev)
   b. Set keep-alive = 60 s
   c. Configure LWT:
      - Topic:   <status_topic>
      - Payload: {"state":"offline","reason":"lwt"}
      - QoS:     1
      - Retain:  true

4. Call CONNECT with:
   - Client ID = <DEVICE_ID>
   - Username  = <DEVICE_TOKEN>
   - Password  = <GENERATED_PASSWORD>
   - Clean Session = true

5. On successful connection:
   a. Publish birth message to <status_topic>:
      Payload: {"state":"online"}  QoS:1  Retain:true

6. Start publish loop:
   - Read sensor data
   - Build JSON payload
   - Publish to <telemetry_topic>  QoS:0 or 1
   - Call client.loop() / yield frequently

7. On disconnect / connection lost:
   a. Wait / back-off
   b. Attempt reconnect (go to step 3)
```

---

## 9. Arduino Library Design Guide

This section describes the expected interface and behaviour for the Arduino SSC library (`SSCClient`).

### 9.1 Class Interface (Proposed API)

```cpp
class SSCClient {
public:
  // Initialise with underlying network client (WiFiClient for dev, WiFiClientSecure for prod)
  SSCClient(Client& networkClient);

  // Configuration — called before begin()
  void setBroker(const char* host, uint16_t port = 1883);
  void setCredentials(const char* clientId, const char* username, const char* password);
  void setTelemetryTopic(const char* topic);
  void setStatusTopic(const char* topic);
  void setKeepAlive(uint16_t seconds = 60);

  // Optional callback for incoming messages (commands / config)
  void setMessageCallback(void (*callback)(const char* topic, const uint8_t* payload, unsigned int len));

  // Lifecycle
  bool begin();          // Connect to MQTT broker + publish birth message
  void loop();           // Must be called frequently in Arduino loop()
  void disconnect();     // Graceful disconnect

  // Publishing
  bool publishTelemetry(const char* jsonPayload);
  bool publishTelemetry(const char* jsonPayload, bool retain = false, uint8_t qos = 0);
  bool publishStatus(const char* state);   // "online" or "offline"
  bool publish(const char* topic, const char* payload, bool retain = false, uint8_t qos = 0);

  // Status
  bool isConnected();
  int  lastError();      // Returns PubSubClient error code
};
```

### 9.2 Internal Behaviour

- **Auto-reconnect**: `loop()` should detect disconnection and attempt reconnect with exponential back-off (start: 5 s, max: 60 s).
- **LWT setup**: Always configure LWT before calling `connect()`.
- **Birth message**: Always publish `{"state":"online"}` immediately after successful connect.
- **JSON builder**: Provide a simple helper to build flat JSON from key-value pairs without heap fragmentation.
- **Non-blocking**: All operations should be non-blocking; use state machines rather than `while` loops with `delay()`.

### 9.3 Dependencies

| Library | Purpose | Source |
|---------|---------|--------|
| `PubSubClient` | MQTT client | Arduino Library Manager |
| `WiFi` / `ESP8266WiFi` | Wi-Fi connectivity | Built-in (ESP32 / ESP8266) |
| `ArduinoJson` *(optional)* | JSON payload building | Arduino Library Manager |
| `Preferences` / `EEPROM` | Credential persistence | Built-in |

### 9.4 Platform Targets

| Platform | Network Client | Notes |
|----------|---------------|-------|
| ESP32 | `WiFiClient` (dev) | Arduino core 2.x+ |
| ESP8266 | `WiFiClient` (dev) | Arduino core 3.x+ |
| Arduino MKR Wi-Fi 1010 | `WiFiClient` | WiFiNINA library |

---

## 10. Web Portal Configuration Parameters

The web portal (captive portal or hosted config page) should allow users to configure and persist the following parameters. These values are stored in non-volatile storage on the device and loaded at boot.

### 10.1 Wi-Fi Settings

| Parameter | Field Label | Type | Notes |
|-----------|-------------|------|-------|
| `wifi_ssid` | Wi-Fi Network Name | Text | 32 chars max |
| `wifi_password` | Wi-Fi Password | Password | 64 chars max |

### 10.2 MQTT / SSC Settings

| Parameter | Field Label | Type | Default | Notes |
|-----------|-------------|------|---------|-------|
| `mqtt_host` | MQTT Broker Host | Text | `mqtt.sensornodeiot.com` | Editable for custom brokers |
| `mqtt_port` | MQTT Port | Number | `1883` | Dev: 1883 |
| `mqtt_client_id` | Device ID | Text | — | Must match SSC Device ID exactly |
| `mqtt_username` | Device Token | Text | — | From SSC Device Settings |
| `mqtt_password` | Device Password | Password | — | Generated in SSC Device Settings |
| `mqtt_telemetry_topic` | Telemetry Topic | Text | — | Full topic path from SSC Device Settings |
| `mqtt_status_topic` | Status Topic | Text | — | Full topic path from SSC Device Settings |

### 10.3 Optional / Advanced Settings

| Parameter | Field Label | Type | Default | Notes |
|-----------|-------------|------|---------|-------|
| `mqtt_keepalive` | Keep-Alive (s) | Number | `60` | Recommended: 60 |
| `publish_interval` | Publish Interval (ms) | Number | `10000` | Telemetry send rate |
| `device_name` | Device Friendly Name | Text | — | Display label only |

### 10.4 Portal UX Recommendations

- Display a **"Test Connection"** button that attempts a single MQTT connect and reports success/failure.
- Show the **current connection status** (Connected / Disconnected / Connecting) on the portal home page.
- Provide a **"Copy from clipboard"** hint next to topic fields, as SSC topics are long.
- **Mask passwords** by default with a show/hide toggle.
- Store all values in `Preferences` (ESP32) or `EEPROM` (ESP8266) with a magic byte/version check.

---

## 11. Code Example — Arduino / ESP8266 / ESP32 (Plain TCP)

> Development only — no TLS. Replace `WiFiClient` with `WiFiClientSecure` for production.

```cpp
#include <WiFi.h>             // ESP32 — use <ESP8266WiFi.h> for ESP8266
#include <PubSubClient.h>

// ── Wi-Fi ──────────────────────────────────────────────────
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ── SSC MQTT (Development — Port 1883) ────────────────────
const char* MQTT_HOST     = "mqtt.sensornodeiot.com";
const int   MQTT_PORT     = 1883;              // Plain TCP, dev only
const char* MQTT_CLIENT   = "<DEVICE_ID>";    // From SSC Device Settings
const char* MQTT_USER     = "<DEVICE_TOKEN>"; // From SSC Device Settings
const char* MQTT_PASS     = "<GENERATED_PASSWORD>"; // From SSC Device Settings

// ── Topics — copy exact values from SSC Device Settings ───
const char* TOPIC_TELEMETRY = "tenant/<tenantId>/app/<appId>/device/<deviceToken>/data";
const char* TOPIC_STATUS    = "tenant/<tenantId>/app/<appId>/device/<deviceToken>/status";

// ── LWT payload ───────────────────────────────────────────
const char* LWT_PAYLOAD  = "{\"state\":\"offline\",\"reason\":\"lwt\"}";
const char* BORN_PAYLOAD = "{\"state\":\"online\"}";

WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);

unsigned long lastPublish = 0;
const long    PUBLISH_INTERVAL = 10000; // ms

// ── Wi-Fi ──────────────────────────────────────────────────
void connectWifi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Wi-Fi connected, IP: ");
  Serial.println(WiFi.localIP());
}

// ── MQTT Reconnect ─────────────────────────────────────────
void reconnectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to SSC MQTT...");

    bool ok = mqtt.connect(
      MQTT_CLIENT,   // Client ID  — must match Device ID
      MQTT_USER,     // Username   — Device Token
      MQTT_PASS,     // Password
      TOPIC_STATUS,  // LWT topic
      1,             // LWT QoS = 1
      true,          // LWT retain = true
      LWT_PAYLOAD    // LWT payload
    );

    if (ok) {
      Serial.println(" connected.");
      // Publish birth / online message
      mqtt.publish(TOPIC_STATUS, BORN_PAYLOAD, true); // retain = true, QoS 0
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(". Retrying in 5 s...");
      delay(5000);
    }
  }
}

// ── Incoming message handler ──────────────────────────────
void onMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message on [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();
}

// ── Setup ──────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  connectWifi();

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setKeepAlive(60);
  mqtt.setCallback(onMessage);
}

// ── Loop ───────────────────────────────────────────────────
void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWifi();
  if (!mqtt.connected()) reconnectMQTT();
  mqtt.loop();

  // Publish telemetry every PUBLISH_INTERVAL ms
  if (millis() - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = millis();

    // Build JSON payload (replace with real sensor readings)
    String payload = "{\"temperature\":25.5,\"humidity\":60.2}";

    if (mqtt.publish(TOPIC_TELEMETRY, payload.c_str())) {
      Serial.println("Telemetry published: " + payload);
    } else {
      Serial.println("Telemetry publish failed.");
    }
  }
}
```

### Required Libraries (Arduino Library Manager)

- **PubSubClient** by Nick O'Leary
- **WiFi** (built-in for ESP32) or **ESP8266WiFi** (built-in for ESP8266)

---

## 12. QoS Reference

| QoS | Name | Guarantee | Recommended For |
|-----|------|-----------|----------------|
| 0 | At most once | Fire-and-forget | High-frequency telemetry |
| 1 | At least once | Delivered at least once (may duplicate) | Status / LWT messages |
| 2 | Exactly once | Exactly once (high overhead) | Critical commands |

**SSC Recommendations:**

| Message Type | QoS | Retain |
|-------------|-----|--------|
| Telemetry | 0 or 1 | false |
| Birth (online) | 1 | true |
| LWT (offline) | 1 | true |

---

## 13. Troubleshooting

### Connection Refused (rc=-2 / rc=5)

- Verify **Client ID** matches the SSC Device ID exactly (case-sensitive).
- Verify **Username** is the Device Token (not an email or name).
- Ensure a **Password has been generated** in Device Settings — tokens without a generated password will be rejected.
- Confirm the broker host resolves correctly: `ping mqtt.sensornodeiot.com`.
- Confirm port 1883 is not blocked by your router/firewall.

### Device Shows "Unknown" / Never "Online"

- Ensure the **birth message** is published after every successful connect.
- Ensure the LWT **status topic** exactly matches the one in Device Settings.
- Ensure LWT `retain = true` and `QoS = 1`.

### Messages Not Appearing in SSC

- Verify the **telemetry topic** is copied exactly from Device Settings.
- Validate your JSON payload is well-formed (no trailing commas, proper quoting).
- Check `mqtt.publish()` return value — `false` indicates the publish buffer is full (increase `PubSubClient` buffer size with `setBufferSize()`).

### Frequent Disconnects

- Increase keep-alive interval if the network is unstable.
- Call `mqtt.loop()` as frequently as possible — avoid long `delay()` calls.
- Ensure `reconnectMQTT()` uses a back-off delay to avoid hammering the broker.

---

## 14. Security Notice

> ⚠️ **Port 1883 is for development use only.**

Plain TCP MQTT transmits all data — including device credentials — in cleartext. Any actor on the same network can capture credentials and impersonate your device.

Before deploying to production:

- Switch to **MQTTS (port 8883)** with `WiFiClientSecure` and the Let's Encrypt ISRG Root X1 CA.
- Or use **WSS (port 443)** for firewall-friendly TLS.
- Never commit real device tokens or passwords to source control.
- Rotate the device password periodically via SSC Device Settings.

---

*Guide version: 1.0 — Generated for SSC tenant `sensornode-uat`, Application `aaf00bbb-3b09-4616-b483-410ccdb81c51`*  
*Last updated: 2026-03-16*
