#include "ssc_client.h"
#include <Arduino.h>

SSCClient::SSCClient(Client& networkClient)
    : mqtt_(networkClient) {
}

void SSCClient::begin(const SSCConfig& config) {
    config_ = config;
    backoffMs_ = MIN_BACKOFF_MS;

    buildTopics();

    mqtt_.setServer(config_.host, config_.port);
    mqtt_.setKeepAlive(KEEP_ALIVE_SEC);
    mqtt_.setBufferSize(512);

    snprintf(lwtPayload_, sizeof(lwtPayload_),
             "{\"state\":\"offline\",\"reason\":\"lwt\"}");

    Serial.printf("[SSC] Broker: %s:%u\n", config_.host, config_.port);
    Serial.printf("[SSC] Telemetry: %s\n", telemetryTopic_);
    Serial.printf("[SSC] Status: %s\n", statusTopic_);
}

void SSCClient::update() {
    switch (state_) {
        case SSCState::Disconnected:
            if (millis() >= retryTime_) {
                startConnect();
            }
            break;

        case SSCState::Connecting:
            checkConnection();
            break;

        case SSCState::Connected:
            if (!mqtt_.connected()) {
                Serial.println("[SSC] Connection lost");
                state_ = SSCState::Disconnected;
                scheduleRetry();
            } else {
                mqtt_.loop();
            }
            break;
    }
}

void SSCClient::disconnect() {
    mqtt_.disconnect();
    state_ = SSCState::Disconnected;
}

void SSCClient::buildTopics() {
    snprintf(telemetryTopic_, sizeof(telemetryTopic_),
             "tenant/%s/app/%s/device/%s/data",
             config_.tenantId, config_.applicationId, config_.username);

    snprintf(statusTopic_, sizeof(statusTopic_),
             "tenant/%s/app/%s/device/%s/status",
             config_.tenantId, config_.applicationId, config_.username);
}

void SSCClient::startConnect() {
    Serial.println("[SSC] Connecting...");
    state_ = SSCState::Connecting;
    connectStartTime_ = millis();

    bool ok = mqtt_.connect(
        config_.clientId,
        config_.username,
        config_.password,
        statusTopic_,
        1,
        true,
        lwtPayload_
    );

    if (ok) {
        Serial.println("[SSC] Connected");
        state_ = SSCState::Connected;
        backoffMs_ = MIN_BACKOFF_MS;
        publishBirthMessage();
    }
}

void SSCClient::checkConnection() {
    if (mqtt_.connected()) {
        Serial.println("[SSC] Connected");
        state_ = SSCState::Connected;
        backoffMs_ = MIN_BACKOFF_MS;
        publishBirthMessage();
        return;
    }

    if (millis() - connectStartTime_ > CONNECT_TIMEOUT_MS) {
        Serial.printf("[SSC] Connection timeout (state=%d)\n", mqtt_.state());
        state_ = SSCState::Disconnected;
        scheduleRetry();
    }
}

void SSCClient::scheduleRetry() {
    retryTime_ = millis() + backoffMs_;
    Serial.printf("[SSC] Retry in %lu ms\n", backoffMs_);

    backoffMs_ = backoffMs_ * 2;
    if (backoffMs_ > MAX_BACKOFF_MS) {
        backoffMs_ = MAX_BACKOFF_MS;
    }
}

bool SSCClient::publishBirthMessage() {
    const char* payload = "{\"state\":\"online\"}";
    bool ok = mqtt_.publish(statusTopic_, payload, true);
    if (ok) {
        Serial.println("[SSC] Birth message sent");
    } else {
        Serial.println("[SSC] Birth message failed");
    }
    return ok;
}

bool SSCClient::publishTelemetry(float temperature, float humidity) {
    if (state_ != SSCState::Connected) {
        return false;
    }

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"temperature\":%.2f,\"humidity\":%.2f}",
             temperature, humidity);

    bool ok = mqtt_.publish(telemetryTopic_, payload);
    if (ok) {
        Serial.printf("[SSC] Telemetry: %s\n", payload);
    } else {
        Serial.println("[SSC] Telemetry publish failed");
    }
    return ok;
}
