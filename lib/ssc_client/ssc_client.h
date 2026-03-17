#pragma once

#include <Client.h>
#include <PubSubClient.h>
#include <cstdint>

enum class SSCState {
    Disconnected,
    Connecting,
    Connected
};

struct SSCConfig {
    const char* host;
    uint16_t port;
    const char* clientId;
    const char* username;
    const char* password;
    const char* tenantId;
    const char* applicationId;
};

using CommandCallback = void (*)(const char* topic, uint8_t* payload, unsigned int length);

class SSCClient {
public:
    explicit SSCClient(Client& networkClient);

    void begin(const SSCConfig& config);
    void update();
    void disconnect();

    bool publishTelemetry(float temperature, float humidity);
    bool publishAck(const char* payload);
    void setCommandCallback(CommandCallback cb);

    bool isConnected() const { return state_ == SSCState::Connected; }
    SSCState state() const { return state_; }
    PubSubClient& getMqttClient() { return mqtt_; }
    const char* getAckTopic() const { return ackTopic_; }

private:
    void buildTopics();
    void startConnect();
    void checkConnection();
    void scheduleRetry();
    bool publishBirthMessage();

    PubSubClient mqtt_;
    SSCConfig config_{};
    CommandCallback commandCallback_ = nullptr;

    char telemetryTopic_[160];
    char statusTopic_[160];
    char cmdTopic_[160];
    char ackTopic_[160];
    char lwtPayload_[64];

    SSCState state_ = SSCState::Disconnected;
    uint32_t connectStartTime_ = 0;
    uint32_t retryTime_ = 0;
    uint32_t backoffMs_ = 5000;

    static constexpr uint32_t CONNECT_TIMEOUT_MS = 10000;
    static constexpr uint32_t MIN_BACKOFF_MS = 5000;
    static constexpr uint32_t MAX_BACKOFF_MS = 60000;
    static constexpr uint16_t KEEP_ALIVE_SEC = 60;
};
