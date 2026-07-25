#include "utils.h"
#include <sys/time.h>
#include <time.h>

namespace Utils {
    void initTime() {
        // Set up NTP server configuration (it runs in background when connected to Wi-Fi)
        // Bangladesh Standard Time (UTC+6, no daylight-saving adjustment).
        configTzTime("BDT-6", "pool.ntp.org", "time.nist.gov");
    }

    void setSystemTime(uint32_t epoch) {
        struct timeval tv;
        tv.tv_sec = epoch;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        Serial.printf("[Utils] System time set to epoch: %u (%s %s)\n", 
                      epoch, getFormattedDate().c_str(), getFormattedTime().c_str());
    }

    uint32_t getEpoch() {
        return (uint32_t)time(NULL);
    }

    bool isTimeSet() {
        // Threshold: Jan 1st 2020 (1577836800)
        return time(NULL) > 1577836800;
    }

    String getFormattedDate() {
        time_t now = time(NULL);
        struct tm timeinfo;
        if (now < 1577836800) {
            return "2020-01-01"; // Default date if unset
        }
        localtime_r(&now, &timeinfo);
        char buffer[12];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
        return String(buffer);
    }

    String getFormattedTime() {
        time_t now = time(NULL);
        struct tm timeinfo;
        if (now < 1577836800) {
            return "00:00:00"; // Default time if unset
        }
        localtime_r(&now, &timeinfo);
        char buffer[10];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
        return String(buffer);
    }

    String getUptimeString() {
        uint32_t sec = millis() / 1000;
        uint32_t min = sec / 60;
        uint32_t hr = min / 60;
        uint32_t day = hr / 24;

        sec %= 60;
        min %= 60;
        hr %= 24;

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%ud %02u:%02u:%02u", day, hr, min, sec);
        return String(buffer);
    }

    String formatEpochDate(uint32_t epoch) {
        time_t temp = (time_t)epoch;
        struct tm timeinfo;
        localtime_r(&temp, &timeinfo);
        char buffer[12];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
        return String(buffer);
    }

    String formatEpochTime(uint32_t epoch) {
        time_t temp = (time_t)epoch;
        struct tm timeinfo;
        localtime_r(&temp, &timeinfo);
        char buffer[10];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
        return String(buffer);
    }

    String urlDecode(const String& input) {
        String decoded = "";
        char c;
        for (size_t i = 0; i < input.length(); i++) {
            c = input.charAt(i);
            if (c == '+') {
                decoded += ' ';
            } else if (c == '%' && i + 2 < input.length()) {
                char hexStr[3] = { input.charAt(i + 1), input.charAt(i + 2), '\0' };
                char decodedChar = (char) strtol(hexStr, NULL, 16);
                decoded += decodedChar;
                i += 2;
            } else {
                decoded += c;
            }
        }
        return decoded;
    }
}
