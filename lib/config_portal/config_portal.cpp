#include "config_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

static WebServer server(80);
static ConfigPortal* instance = nullptr;

static const char* CSS = R"(
body{font-family:system-ui,sans-serif;max-width:480px;margin:40px auto;padding:0 16px;background:#f5f5f5}
h1{color:#333;border-bottom:2px solid #007bff;padding-bottom:8px}
h2{color:#555;margin-top:0}
form{background:#fff;padding:20px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,0.1)}
label{display:block;margin:12px 0 4px;font-weight:500}
input[type=text],input[type=password],input[type=number]{width:100%;padding:8px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box}
input[type=submit],button{background:#007bff;color:#fff;border:none;padding:12px 24px;border-radius:4px;cursor:pointer;margin-top:16px}
input[type=submit]:hover,button:hover{background:#0056b3}
.nav{display:flex;gap:8px;flex-wrap:wrap;margin-bottom:20px}
.nav a{background:#6c757d;color:#fff;padding:8px 16px;border-radius:4px;text-decoration:none}
.nav a:hover{background:#545b62}
.status{background:#fff;padding:16px;border-radius:8px;margin-bottom:20px;box-shadow:0 2px 4px rgba(0,0,0,0.1)}
.ok{color:#28a745}.err{color:#dc3545}
)";

ConfigPortal::ConfigPortal(ConfigStore& store) : store_(store) {
    instance = this;
}

void ConfigPortal::start(const char* apSsid, const char* apPassword) {
    Serial.println("[Portal] Starting configuration portal");

    store_.load(config_);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);
    Serial.printf("[Portal] AP: %s  IP: %s\n", apSsid, WiFi.softAPIP().toString().c_str());

    setupRoutes();
    server.begin();
    Serial.println("[Portal] Ready at http://192.168.4.1");

    while (true) {
        server.handleClient();
        delay(1);
    }
}

void ConfigPortal::setupRoutes() {
    server.on("/", HTTP_GET, []() { instance->handleRoot(); });
    server.on("/wifi", HTTP_GET, []() { instance->handleWiFiPage(); });
    server.on("/wifi", HTTP_POST, []() { instance->handleWiFiSave(); });
    server.on("/mqtt", HTTP_GET, []() { instance->handleMqttPage(); });
    server.on("/mqtt", HTTP_POST, []() { instance->handleMqttSave(); });
    server.on("/ota", HTTP_GET, []() { instance->handleOtaPage(); });
    server.on("/update", HTTP_POST,
        []() { instance->handleOtaResult(); },
        []() { instance->handleOtaUpload(); });
    server.on("/reboot", HTTP_POST, []() { instance->handleReboot(); });
}

void ConfigPortal::sendPage(const char* title, const String& body) {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>";
    html += title;
    html += "</title><style>";
    html += CSS;
    html += "</style></head><body><h1>";
    html += title;
    html += "</h1>";
    html += "<div class='nav'>";
    html += "<a href='/'>Status</a>";
    html += "<a href='/wifi'>WiFi</a>";
    html += "<a href='/mqtt'>MQTT</a>";
    html += "<a href='/ota'>OTA</a>";
    html += "</div>";
    html += body;
    html += "</body></html>";
    server.send(200, "text/html", html);
}

String ConfigPortal::makeInput(const char* label, const char* name, const char* value,
                                const char* type, int maxlen) {
    String html = "<label>";
    html += label;
    html += "</label><input type='";
    html += type;
    html += "' name='";
    html += name;
    html += "' value='";
    html += value;
    html += "' maxlength='";
    html += maxlen;
    html += "'>";
    return html;
}

void ConfigPortal::handleRoot() {
    String body = "<div class='status'><h2>Current Configuration</h2>";

    body += "<p><strong>WiFi SSID:</strong> ";
    body += config_.wifi_ssid[0] ? config_.wifi_ssid : "<em>Not configured</em>";
    body += "</p>";

    body += "<p><strong>MQTT Host:</strong> ";
    body += config_.mqtt_host[0] ? config_.mqtt_host : "<em>Not configured</em>";
    if (config_.mqtt_port > 0) {
        body += ":";
        body += config_.mqtt_port;
    }
    body += "</p>";

    body += "<p><strong>Tenant ID:</strong> ";
    body += config_.tenant_id[0] ? config_.tenant_id : "<em>Not configured</em>";
    body += "</p>";

    body += "<p><strong>Application ID:</strong> ";
    body += config_.application_id[0] ? config_.application_id : "<em>Not configured</em>";
    body += "</p>";

    body += "<p><strong>Device ID:</strong> ";
    body += config_.mqtt_client_id[0] ? config_.mqtt_client_id : "<em>Not configured</em>";
    body += "</p>";

    body += "<p><strong>Status:</strong> ";
    if (config_.isValid()) {
        body += "<span class='ok'>Ready to connect</span>";
    } else {
        body += "<span class='err'>Configuration incomplete</span>";
    }
    body += "</p></div>";

    body += "<form method='POST' action='/reboot'>";
    body += "<button type='submit'>Reboot Device</button>";
    body += "</form>";

    sendPage("ESP32 Configuration", body);
}

void ConfigPortal::handleWiFiPage() {
    String body = "<form method='POST'>";
    body += makeInput("WiFi SSID", "ssid", config_.wifi_ssid, "text", 32);
    body += makeInput("WiFi Password", "pass", config_.wifi_pass, "password", 64);
    body += "<input type='submit' value='Save WiFi Settings'>";
    body += "</form>";

    sendPage("WiFi Settings", body);
}

void ConfigPortal::handleWiFiSave() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    strncpy(config_.wifi_ssid, ssid.c_str(), sizeof(config_.wifi_ssid) - 1);
    strncpy(config_.wifi_pass, pass.c_str(), sizeof(config_.wifi_pass) - 1);

    if (store_.save(config_)) {
        String body = "<p class='ok'>WiFi settings saved.</p>";
        body += "<p><a href='/'>Back to status</a></p>";
        sendPage("WiFi Settings", body);
    } else {
        String body = "<p class='err'>Failed to save settings.</p>";
        body += "<p><a href='/wifi'>Try again</a></p>";
        sendPage("WiFi Settings", body);
    }
}

void ConfigPortal::handleMqttPage() {
    String body = "<form method='POST'>";
    body += makeInput("MQTT Host", "host", config_.mqtt_host, "text", 64);
    body += makeInput("MQTT Port", "port", String(config_.mqtt_port).c_str(), "number", 5);
    body += makeInput("Tenant ID", "tenant", config_.tenant_id, "text", 64);
    body += makeInput("Application ID", "app", config_.application_id, "text", 64);
    body += makeInput("Device ID (Client ID)", "client", config_.mqtt_client_id, "text", 64);
    body += makeInput("Device Token (Username)", "user", config_.mqtt_username, "text", 64);
    body += makeInput("Device Password", "pass", config_.mqtt_password, "password", 64);
    body += "<input type='submit' value='Save MQTT Settings'>";
    body += "</form>";

    sendPage("MQTT Settings", body);
}

void ConfigPortal::handleMqttSave() {
    strncpy(config_.mqtt_host, server.arg("host").c_str(), sizeof(config_.mqtt_host) - 1);
    config_.mqtt_port = server.arg("port").toInt();
    strncpy(config_.tenant_id, server.arg("tenant").c_str(), sizeof(config_.tenant_id) - 1);
    strncpy(config_.application_id, server.arg("app").c_str(), sizeof(config_.application_id) - 1);
    strncpy(config_.mqtt_client_id, server.arg("client").c_str(), sizeof(config_.mqtt_client_id) - 1);
    strncpy(config_.mqtt_username, server.arg("user").c_str(), sizeof(config_.mqtt_username) - 1);
    strncpy(config_.mqtt_password, server.arg("pass").c_str(), sizeof(config_.mqtt_password) - 1);

    if (config_.mqtt_port == 0) {
        config_.mqtt_port = 1883;
    }

    if (store_.save(config_)) {
        String body = "<p class='ok'>MQTT settings saved.</p>";
        body += "<p><a href='/'>Back to status</a></p>";
        sendPage("MQTT Settings", body);
    } else {
        String body = "<p class='err'>Failed to save settings.</p>";
        body += "<p><a href='/mqtt'>Try again</a></p>";
        sendPage("MQTT Settings", body);
    }
}

void ConfigPortal::handleOtaPage() {
    String body = "<form method='POST' action='/update' enctype='multipart/form-data'>";
    body += "<label>Firmware Binary (.bin)</label>";
    body += "<input type='file' name='firmware' accept='.bin'>";
    body += "<input type='submit' value='Upload Firmware'>";
    body += "</form>";

    sendPage("OTA Update", body);
}

void ConfigPortal::handleOtaUpload() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[OTA] Start: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("[OTA] Done: %u bytes\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
    }
}

void ConfigPortal::handleOtaResult() {
    bool ok = !Update.hasError();

    String body;
    if (ok) {
        body = "<p class='ok'>Update successful!</p>";
        body += "<form method='POST' action='/reboot'>";
        body += "<button type='submit'>Reboot Now</button>";
        body += "</form>";
    } else {
        body = "<p class='err'>Update failed.</p>";
        body += "<p><a href='/ota'>Try again</a></p>";
    }

    sendPage("OTA Update", body);
}

void ConfigPortal::handleReboot() {
    String body = "<p>Rebooting...</p>";
    body += "<p>Reconnect to your normal WiFi network.</p>";
    sendPage("Rebooting", body);

    delay(500);
    ESP.restart();
}

void startConfigPortal(ConfigStore& store, const char* apSsid, const char* apPassword) {
    ConfigPortal portal(store);
    portal.start(apSsid, apPassword);
}
