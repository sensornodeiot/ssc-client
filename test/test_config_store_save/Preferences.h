#pragma once

#include <cstddef>
#include <cstring>
#include <cstdint>

// Stub Preferences class for native testing of ConfigStore::save()
class Preferences {
public:
    // --- Static control flags (set from tests) ---
    static bool begin_result;
    static size_t putString_result;
    static size_t putUShort_result;
    static size_t putULong_result;

    // Fail putString on the Nth call (0 = never fail)
    static int putString_fail_on_call;

    // --- Call counters ---
    static int putString_calls;
    static int putUShort_calls;
    static int putULong_calls;
    static int begin_calls;
    static int end_calls;

    static void reset() {
        begin_result = true;
        putString_result = 4;   // non-zero = success
        putUShort_result = 2;
        putULong_result = 4;
        putString_fail_on_call = 0;
        putString_calls = 0;
        putUShort_calls = 0;
        putULong_calls = 0;
        begin_calls = 0;
        end_calls = 0;
    }

    bool begin(const char* /*name*/, bool /*readOnly*/ = false) {
        begin_calls++;
        return begin_result;
    }

    void end() {
        end_calls++;
    }

    size_t putString(const char* /*key*/, const char* /*value*/) {
        putString_calls++;
        if (putString_fail_on_call > 0 && putString_calls == putString_fail_on_call) {
            return 0;
        }
        return putString_result;
    }

    size_t getString(const char* /*key*/, char* buf, size_t len) {
        if (buf && len > 0) buf[0] = '\0';
        return 0;
    }

    size_t putUShort(const char* /*key*/, uint16_t /*value*/) {
        putUShort_calls++;
        return putUShort_result;
    }

    uint16_t getUShort(const char* /*key*/, uint16_t defaultValue = 0) {
        return defaultValue;
    }

    size_t putULong(const char* /*key*/, uint32_t /*value*/) {
        putULong_calls++;
        return putULong_result;
    }

    uint32_t getULong(const char* /*key*/, uint32_t defaultValue = 0) {
        return defaultValue;
    }

    bool clear() {
        return true;
    }
};

// Static member definitions
bool   Preferences::begin_result = true;
size_t Preferences::putString_result = 4;
size_t Preferences::putUShort_result = 2;
size_t Preferences::putULong_result = 4;
int    Preferences::putString_fail_on_call = 0;
int    Preferences::putString_calls = 0;
int    Preferences::putUShort_calls = 0;
int    Preferences::putULong_calls = 0;
int    Preferences::begin_calls = 0;
int    Preferences::end_calls = 0;
