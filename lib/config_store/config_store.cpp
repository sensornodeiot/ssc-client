#include "config_store.h"
#include <Preferences.h>

namespace {
    constexpr const char* KEY_WIFI_SSID       = "wifi_ssid";
    constexpr const char* KEY_WIFI_PASS       = "wifi_pass";
    constexpr const char* KEY_MQTT_HOST       = "mqtt_host";
    constexpr const char* KEY_MQTT_PORT       = "mqtt_port";
    constexpr const char* KEY_MQTT_CLIENT_ID  = "mqtt_cid";
    constexpr const char* KEY_MQTT_USERNAME   = "mqtt_user";
    constexpr const char* KEY_MQTT_PASSWORD   = "mqtt_pass";
    constexpr const char* KEY_TENANT_ID       = "tenant_id";
    constexpr const char* KEY_APP_ID          = "app_id";
    constexpr const char* KEY_PUBLISH_INTERVAL = "pub_int";
    constexpr const char* KEY_OTA_CMD_ID      = "ota_cmd_id";
    constexpr const char* KEY_OTA_TGT_VER     = "ota_tgt_ver";
    constexpr const char* KEY_OTA_STATE       = "ota_state";
}

bool ConfigStore::begin() {
    Preferences prefs;
    bool ok = prefs.begin(NAMESPACE, true);
    prefs.end();
    return ok;
}

bool ConfigStore::load(Config& config) {
    config.clear();

    Preferences prefs;
    if (!prefs.begin(NAMESPACE, true)) {
        return false;
    }

    prefs.getString(KEY_WIFI_SSID, config.wifi_ssid, sizeof(config.wifi_ssid));
    prefs.getString(KEY_WIFI_PASS, config.wifi_pass, sizeof(config.wifi_pass));
    prefs.getString(KEY_MQTT_HOST, config.mqtt_host, sizeof(config.mqtt_host));
    config.mqtt_port = prefs.getUShort(KEY_MQTT_PORT, 1883);
    prefs.getString(KEY_MQTT_CLIENT_ID, config.mqtt_client_id, sizeof(config.mqtt_client_id));
    prefs.getString(KEY_MQTT_USERNAME, config.mqtt_username, sizeof(config.mqtt_username));
    prefs.getString(KEY_MQTT_PASSWORD, config.mqtt_password, sizeof(config.mqtt_password));
    prefs.getString(KEY_TENANT_ID, config.tenant_id, sizeof(config.tenant_id));
    prefs.getString(KEY_APP_ID, config.application_id, sizeof(config.application_id));
    config.publish_interval_ms = prefs.getULong(KEY_PUBLISH_INTERVAL, 10000);

    prefs.end();
    return true;
}

bool ConfigStore::save(const Config& config) {
    Preferences prefs;
    if (!prefs.begin(NAMESPACE, false)) {
        return false;
    }

    bool ok = true;
    ok &= prefs.putString(KEY_WIFI_SSID, config.wifi_ssid) > 0 || config.wifi_ssid[0] == '\0';
    ok &= prefs.putString(KEY_WIFI_PASS, config.wifi_pass) > 0 || config.wifi_pass[0] == '\0';
    ok &= prefs.putString(KEY_MQTT_HOST, config.mqtt_host) > 0 || config.mqtt_host[0] == '\0';
    ok &= prefs.putUShort(KEY_MQTT_PORT, config.mqtt_port) > 0;
    ok &= prefs.putString(KEY_MQTT_CLIENT_ID, config.mqtt_client_id) > 0 || config.mqtt_client_id[0] == '\0';
    ok &= prefs.putString(KEY_MQTT_USERNAME, config.mqtt_username) > 0 || config.mqtt_username[0] == '\0';
    ok &= prefs.putString(KEY_MQTT_PASSWORD, config.mqtt_password) > 0 || config.mqtt_password[0] == '\0';
    ok &= prefs.putString(KEY_TENANT_ID, config.tenant_id) > 0 || config.tenant_id[0] == '\0';
    ok &= prefs.putString(KEY_APP_ID, config.application_id) > 0 || config.application_id[0] == '\0';
    ok &= prefs.putULong(KEY_PUBLISH_INTERVAL, config.publish_interval_ms) > 0;

    prefs.end();
    return ok;
}

bool ConfigStore::clear() {
    Preferences prefs;
    if (!prefs.begin(NAMESPACE, false)) {
        return false;
    }
    bool ok = prefs.clear();
    prefs.end();
    return ok;
}

bool ConfigStore::saveOtaState(const OtaState& otaState) {
    Preferences prefs;
    if (!prefs.begin(NAMESPACE, false)) return false;

    bool ok = true;
    ok &= prefs.putString(KEY_OTA_CMD_ID, otaState.commandId) > 0;
    ok &= prefs.putString(KEY_OTA_TGT_VER, otaState.targetVersion) > 0;
    ok &= prefs.putUChar(KEY_OTA_STATE, otaState.state) > 0;

    prefs.end();
    return ok;
}

bool ConfigStore::loadOtaState(OtaState& otaState) {
    otaState.clear();
    Preferences prefs;
    if (!prefs.begin(NAMESPACE, true)) return false;

    prefs.getString(KEY_OTA_CMD_ID, otaState.commandId, sizeof(otaState.commandId));
    prefs.getString(KEY_OTA_TGT_VER, otaState.targetVersion, sizeof(otaState.targetVersion));
    otaState.state = prefs.getUChar(KEY_OTA_STATE, 0);

    prefs.end();
    return true;
}

bool ConfigStore::clearOtaState() {
    Preferences prefs;
    if (!prefs.begin(NAMESPACE, false)) return false;

    prefs.remove(KEY_OTA_CMD_ID);
    prefs.remove(KEY_OTA_TGT_VER);
    prefs.remove(KEY_OTA_STATE);

    prefs.end();
    return true;
}
