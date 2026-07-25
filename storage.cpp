#include "storage.h"
#include <SPI.h>
#include <SD.h>

// Storage constant definitions
const int Storage::SD_CS_PIN = 4;
const int Storage::RFID_CS_PIN = 5;
const char* Storage::CONFIG_PATH = "/config.json";
const char* Storage::STUDENTS_PATH = "/students.json";
const char* Storage::ATTENDANCE_PATH = "/attendance.csv";
const char* Storage::SESSIONS_PATH = "/sessions.json";

namespace Storage {
    static bool sdInitialized = false;

    bool init() {
        // Configure Chip Select pins
        pinMode(SD_CS_PIN, OUTPUT);
        pinMode(RFID_CS_PIN, OUTPUT);
        
        // De-select both devices initially by pulling CS HIGH
        digitalWrite(SD_CS_PIN, HIGH);
        digitalWrite(RFID_CS_PIN, HIGH);

        // VSPI pins on ESP32 are SCK = 18, MISO = 19, MOSI = 23.
        // SD.begin on ESP32 automatically configures SPI pins correctly.
        if (!SD.begin(SD_CS_PIN)) {
            Serial.println("[Storage] SD Card initialization failed!");
            sdInitialized = false;
            return false;
        }

        // Verify we can access the root directory
        File root = SD.open("/");
        if (!root) {
            Serial.println("[Storage] SD Card: Failed to open root directory!");
            sdInitialized = false;
            return false;
        }
        root.close();

        Serial.println("[Storage] SD Card initialized successfully.");
        sdInitialized = true;
        return true;
    }

    bool isAvailable() {
        if (!sdInitialized) {
            return init();
        }
        // SD.cardType() returns CARD_NONE if card is disconnected
        return SD.cardType() != CARD_NONE;
    }

    bool readJson(const char* filepath, JsonDocument& doc) {
        if (!isAvailable()) {
            Serial.println("[Storage] readJson: SD Card not available");
            return false;
        }
        
        File file = SD.open(filepath, FILE_READ);
        if (!file) {
            Serial.printf("[Storage] Failed to open %s for reading\n", filepath);
            return false;
        }

        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.printf("[Storage] Failed to parse JSON from %s: %s\n", filepath, error.c_str());
            return false;
        }

        return true;
    }

    bool writeJson(const char* filepath, const JsonDocument& doc) {
        if (!isAvailable()) {
            Serial.println("[Storage] writeJson: SD Card not available");
            return false;
        }

        // Write to a temporary file first, then replace the original. This
        // prevents a failed clear/update from leaving stale or partial JSON.
        String tempPath = String(filepath) + ".tmp";
        SD.remove(tempPath.c_str());
        File file = SD.open(tempPath.c_str(), FILE_WRITE);
        if (!file) {
            Serial.printf("[Storage] Failed to open temporary file for %s\n", filepath);
            return false;
        }

        size_t bytesWritten = serializeJson(doc, file);
        file.close();

        if (bytesWritten == 0) {
            SD.remove(tempPath.c_str());
            Serial.printf("[Storage] Failed to write JSON to %s\n", filepath);
            return false;
        }

        // Remove the old file and atomically move the completed replacement.
        SD.remove(filepath);
        if (!SD.rename(tempPath.c_str(), filepath)) {
            SD.remove(tempPath.c_str());
            Serial.printf("[Storage] Failed to replace %s\n", filepath);
            return false;
        }

        return true;
    }

    bool appendCsv(const char* filepath, const String& line) {
        if (!isAvailable()) {
            Serial.println("[Storage] appendCsv: SD Card not available");
            return false;
        }

        // Open in append mode
        File file = SD.open(filepath, FILE_APPEND);
        if (!file) {
            Serial.printf("[Storage] Failed to open %s for appending\n", filepath);
            return false;
        }

        size_t bytesWritten = file.println(line);
        file.close();

        return bytesWritten > 0;
    }

    bool backupFile(const char* filepath) {
        if (!isAvailable()) {
            Serial.println("[Storage] backupFile: SD Card not available");
            return false;
        }

        File src = SD.open(filepath, FILE_READ);
        if (!src) {
            Serial.printf("[Storage] Backup: Source file %s not found\n", filepath);
            return false;
        }

        // Create backup file path in /backups/ directory if possible, or prefix backup_
        // Let's use /backup_<filename>
        String backupPath = "/backup_";
        String origName = String(filepath);
        if (origName.startsWith("/")) {
            backupPath += origName.substring(1);
        } else {
            backupPath += origName;
        }

        // Open destination file
        File dest = SD.open(backupPath.c_str(), FILE_WRITE);
        if (!dest) {
            Serial.printf("[Storage] Backup: Failed to create destination file %s\n", backupPath.c_str());
            src.close();
            return false;
        }

        // Buffer copy
        uint8_t buffer[512];
        while (src.available()) {
            int len = src.read(buffer, sizeof(buffer));
            dest.write(buffer, len);
        }

        dest.close();
        src.close();
        Serial.printf("[Storage] Backup successful: %s -> %s\n", filepath, backupPath.c_str());
        return true;
    }

    uint64_t getCardSize() {
        if (!isAvailable()) return 0;
        return SD.cardSize();
    }

    uint64_t getUsedBytes() {
        if (!isAvailable()) return 0;
        return SD.usedBytes();
    }
}
