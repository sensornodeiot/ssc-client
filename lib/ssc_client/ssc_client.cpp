#include "ssc_client.h"
#include "config.h"
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
    mqtt_.setBufferSize(1024);

    if (commandCallback_) {
        mqtt_.setCallback(commandCallback_);
    }

    snprintf(lwtPayload_, sizeof(lwtPayload_),
             "{\"state\":\"offline\",\"reason\":\"lwt\"}");

    Serial.printf("[SSC] Broker: %s:%u\n", config_.host, config_.port);
    Serial.printf("[SSC] Telemetry: %s\n", telemetryTopic_);
    Serial.printf("[SSC] Status: %s\n", statusTopic_);
    Serial.printf("[SSC] Cmd: %s\n", cmdTopic_);
    Serial.printf("[SSC] Ack: %s\n", ackTopic_);
}

void SSCClient::setCommandCallback(CommandCallback cb) {
    commandCallback_ = cb;
}

bool SSCClient::publishAck(const char* payload) {
    if (state_ != SSCState::Connected) return false;
    bool ok = mqtt_.publish(ackTopic_, payload, false);
    if (ok) {
        Serial.printf("[SSC] ACK sent to %s\n", ackTopic_);
    } else {
        Serial.println("[SSC] ACK publish failed");
    }
    return ok;
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

    snprintf(cmdTopic_, sizeof(cmdTopic_),
             "tenant/%s/app/%s/device/%s/cmd",
             config_.tenantId, config_.applicationId, config_.username);

    snprintf(ackTopic_, sizeof(ackTopic_),
             "tenant/%s/app/%s/device/%s/ack",
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
        mqtt_.subscribe(cmdTopic_);
        Serial.printf("[SSC] Subscribed: %s\n", cmdTopic_);
    }
}

void SSCClient::checkConnection() {
    if (mqtt_.connected()) {
        Serial.println("[SSC] Connected");
        state_ = SSCState::Connected;
        backoffMs_ = MIN_BACKOFF_MS;
        publishBirthMessage();
        mqtt_.subscribe(cmdTopic_);
        Serial.printf("[SSC] Subscribed: %s\n", cmdTopic_);
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
    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"state\":\"online\",\"firmwareVersion\":\"%s\"}",
             FIRMWARE_VERSION);
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
