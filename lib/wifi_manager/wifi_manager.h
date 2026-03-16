#pragma once

#include <cstdint>

enum class WiFiState {
    Disconnected,
    Connecting,
    Connected
};

class WiFiManager {
public:
    void begin(const char* ssid, const char* password);
    void update();
    void disconnect();

    WiFiState state() const { return state_; }
    bool isConnected() const { return state_ == WiFiState::Connected; }

private:
    void startConnect();
    void checkConnection();
    void scheduleRetry();

    const char* ssid_ = nullptr;
    const char* password_ = nullptr;

    WiFiState state_ = WiFiState::Disconnected;
    uint32_t connectStartTime_ = 0;
    uint32_t retryTime_ = 0;
    uint32_t backoffMs_ = 5000;

    static constexpr uint32_t CONNECT_TIMEOUT_MS = 15000;
    static constexpr uint32_t MIN_BACKOFF_MS = 5000;
    static constexpr uint32_t MAX_BACKOFF_MS = 60000;
};
