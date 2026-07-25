#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace Storage {
    // Pin Definitions
    extern const int SD_CS_PIN;
    extern const int RFID_CS_PIN;

    // File Paths
    extern const char* CONFIG_PATH;
    extern const char* STUDENTS_PATH;
    extern const char* ATTENDANCE_PATH;
    extern const char* SESSIONS_PATH;

    // Core functions
    bool init();
    bool isAvailable();

    // JSON file helpers
    bool readJson(const char* filepath, JsonDocument& doc);
    bool writeJson(const char* filepath, const JsonDocument& doc);

    // CSV file helpers
    bool appendCsv(const char* filepath, const String& line);
    
    // Backup helper
    bool backupFile(const char* filepath);
    
    // Get SD card details
    uint64_t getCardSize();
    uint64_t getUsedBytes();
}

#endif // STORAGE_H
