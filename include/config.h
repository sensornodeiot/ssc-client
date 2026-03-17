#pragma once

// Config Portal AP settings
#define OTA_AP_SSID "SSN32-Config"
#define OTA_AP_PASSWORD "ssn32config"

// Boot button to trigger config portal
#define OTA_TRIGGER_PIN 0 // GPIO0 = BOOT button

// Firmware version fallback (overridden by build flag)
#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "0.0.0-dev"
#endif
