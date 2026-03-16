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

class SSCClient {
public:
    explicit SSCClient(Client& networkClient);

    void begin(const SSCConfig& config);
    void update();
    void disconnect();

    bool publishTelemetry(float temperature, float humidity);
    bool isConnected() const { return state_ == SSCState::Connected; }
    SSCState state() const { return state_; }

private:
    void buildTopics();
    void startConnect();
    void checkConnection();
    void scheduleRetry();
    bool publishBirthMessage();

    PubSubClient mqtt_;
    SSCConfig config_{};

    char telemetryTopic_[160];
    char statusTopic_[160];
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
