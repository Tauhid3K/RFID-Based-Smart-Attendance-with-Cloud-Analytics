#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

namespace Utils {
    // Initialize system timekeeping
    void initTime();

    // Set time manually using a Unix Epoch timestamp (seconds since 1970)
    void setSystemTime(uint32_t epoch);

    // Get current Unix Epoch timestamp
    uint32_t getEpoch();

    // Check if system time has been synchronized (is set to a valid year)
    bool isTimeSet();

    // Get formatted Date (YYYY-MM-DD)
    String getFormattedDate();

    // Get formatted Time (HH:MM:SS)
    String getFormattedTime();

    // Get system uptime as a string (e.g., "0d 04:12:30")
    String getUptimeString();
    
    // Helper to format specific epoch into Date/Time
    String formatEpochDate(uint32_t epoch);
    String formatEpochTime(uint32_t epoch);

    // URL decode a string (convert %XX hex codes and +)
    String urlDecode(const String& input);
}

#endif // UTILS_H
