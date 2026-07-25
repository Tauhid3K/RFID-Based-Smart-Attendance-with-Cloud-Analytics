#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

namespace WifiManager {
    // Initialize Wi-Fi connection or AP fallback
    void init();

    // Maintain a station connection after startup.
    void update();

    // Check if system is currently in Access Point mode
    bool isAPModeActive();

    // Get current IP Address as string
    String getIpAddress();

    // Get current network SSID or AP SSID
    String getSsidName();

    // Get Wi-Fi signal strength in dBm (returns 0 if in AP mode)
    int getSignalStrength();
}

#endif // WIFI_MANAGER_H
