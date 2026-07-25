#include <Arduino.h>
#include <SPI.h>
#include "storage.h"
#include "config.h"
#include "students.h"
#include "attendance.h"
#include "utils.h"
#include "wifi_manager.h"
#include "webserver.h"
#include "rfid_reader.h"
#include "sessions.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=======================================================");
    Serial.println("   ESP32 RFID Attendance Management System Initializing");
    Serial.println("=======================================================");

    // Initialize the shared hardware SPI bus explicitly
    // SCK = GPIO 18, MISO = GPIO 19, MOSI = GPIO 23
    SPI.begin(18, 19, 23, -1);

    // 1. Initialize Storage (SD Card Module)
    if (!Storage::init()) {
        Serial.println("[Main] Warning: SD Card initialization failed! System running degraded.");
    }

    // 2. Load settings from SD Card config.json (or save defaults)
    Config::load();

    // 3. Load students list from SD Card students.json (or create empty)
    Students::load();

    // 4. Load sessions from SD Card sessions.json (or create empty)
    Sessions::load();

    // 5. Initialize built-in NTP/time keeping
    Utils::initTime();

    // 5. Connect to local Wi-Fi or fall back to Access Point
    WifiManager::init();

    // 6. Start the administrative REST API & Web Dashboard
    WebServerManager::init();

    // 7. Initialize MFRC522 RFID card scanner module
    if (!RfidReader::init()) {
        Serial.println("[Main] Warning: RC522 RFID reader initialization failed!");
    }

    Serial.println("=======================================================");
    Serial.println("   Initialization Successful! System is Online.");
    Serial.printf("   Device Hostname: %s\n", Config::deviceName.c_str());
    Serial.printf("   Dashboard URL:   http://%s/\n", WifiManager::getIpAddress().c_str());
    Serial.println("=======================================================\n");
}

void loop() {
    WifiManager::update();

    // Start scheduled sessions even when nobody has the dashboard open.
    static uint32_t lastSchedulerCheck = 0;
    uint32_t now = millis();
    if (now - lastSchedulerCheck >= 1000 || lastSchedulerCheck == 0) {
        Sessions::checkScheduledAutoStart();
        lastSchedulerCheck = now;
    }

    // Scan for cards and log attendance
    RfidReader::update();

    // Minor delay to yield CPU slice and avoid excessive polling
    delay(5);
}
