#include "config.h"
#include "storage.h"
#include <ArduinoJson.h>

namespace Config {
    String wifiSSID = "SQL_SLAMMER☢️🔥";
    String wifiPass = "404notfound";
    uint32_t cooldownS = 30;
    String deviceName = "ESP32-Attendance";
    String adminUser = "admin";
    String adminPass = "admin123";

    void loadDefaults() {
        // Default network credentials; AP fallback is used if this network is unavailable.
        wifiSSID = "SQL_SLAMMER☢️🔥";
        wifiPass = "404notfound";
        cooldownS = 30;
        deviceName = "ESP32-Attendance";
        adminUser = "admin";
        adminPass = "admin123";
    }

    bool load() {
        JsonDocument doc;
        if (!Storage::readJson(Storage::CONFIG_PATH, doc)) {
            Serial.println("[Config] Settings file not found or invalid. Loading defaults...");
            loadDefaults();
            // Save defaults to create config.json on the card
            save();
            return false;
        }

        if (doc.containsKey("wifiSSID")) wifiSSID = doc["wifiSSID"].as<String>();
        if (doc.containsKey("wifiPass")) wifiPass = doc["wifiPass"].as<String>();
        if (doc.containsKey("cooldownS")) cooldownS = doc["cooldownS"].as<uint32_t>();
        if (doc.containsKey("deviceName")) deviceName = doc["deviceName"].as<String>();
        if (doc.containsKey("adminUser")) adminUser = doc["adminUser"].as<String>();
        if (doc.containsKey("adminPass")) adminPass = doc["adminPass"].as<String>();

        Serial.println("[Config] Settings loaded successfully.");
        return true;
    }

    bool save() {
        JsonDocument doc;
        doc["wifiSSID"] = wifiSSID;
        doc["wifiPass"] = wifiPass;
        doc["cooldownS"] = cooldownS;
        doc["deviceName"] = deviceName;
        doc["adminUser"] = adminUser;
        doc["adminPass"] = adminPass;

        if (!Storage::writeJson(Storage::CONFIG_PATH, doc)) {
            Serial.println("[Config] Failed to save settings file!");
            return false;
        }

        Serial.println("[Config] Settings saved to SD card.");
        return true;
    }
}
