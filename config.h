#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
    // Config values
    extern String wifiSSID;
    extern String wifiPass;
    extern uint32_t cooldownS;
    extern String deviceName;
    extern String adminUser;
    extern String adminPass;

    // Load and Save settings from storage
    bool load();
    bool save();
    void loadDefaults();
}

#endif // CONFIG_H
