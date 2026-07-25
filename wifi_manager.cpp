#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>

namespace WifiManager {
    static bool apMode = false;
    static String activeSsid = "";
    static uint32_t lastReconnectAttempt = 0;

    static void startAP() {
        apMode = true;
        WiFi.mode(WIFI_AP);
        
        const String apName = "ESP Attendece";
        const String apPassword = "12345678";
        
        // softAP takes (ssid, password, channel, ssid_hidden, max_connection)
        WiFi.softAP(apName.c_str(), apPassword.c_str());
        
        activeSsid = apName;
        Serial.println("[WiFi] Started in Access Point (AP) fallback mode.");
        Serial.printf("[WiFi] SSID: %s\n", apName.c_str());
        Serial.printf("[WiFi] Password: %s\n", apPassword.c_str());
        Serial.printf("[WiFi] IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    }

    void init() {
        // Stop any active connections
        WiFi.disconnect(true);
        WiFi.softAPdisconnect(true);
        delay(200);

        String ssid = Config::wifiSSID;
        String pass = Config::wifiPass;

        if (ssid.length() == 0) {
            startAP();
            return;
        }

        Serial.printf("[WiFi] Attempting to connect to network: %s\n", ssid.c_str());
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.persistent(false);

        // Fixed address requested for the local attendance network.
        IPAddress localIp(192, 168, 110, 200);
        IPAddress gateway(192, 168, 110, 1);
        IPAddress subnet(255, 255, 255, 0);
        IPAddress primaryDns(192, 168, 110, 1);
        IPAddress secondaryDns(8, 8, 8, 8);
        if (!WiFi.config(localIp, gateway, subnet, primaryDns, secondaryDns)) {
            Serial.println("[WiFi] Static IP configuration failed; continuing with DHCP.");
        }
        
        WiFi.begin(ssid.c_str(), pass.c_str());

        // Wait for connection with a 15 second timeout (30 * 500ms)
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 30) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            apMode = false;
            activeSsid = ssid;
            Serial.println("[WiFi] Successfully connected to local network.");
            Serial.printf("[WiFi] IP Address: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("[WiFi] RSSI: %d dBm\n", WiFi.RSSI());
        } else {
            Serial.println("[WiFi] Network connection failed. Falling back to Access Point mode...");
            startAP();
        }
    }

    void update() {
        if (apMode || WiFi.status() == WL_CONNECTED) return;

        uint32_t now = millis();
        if (now - lastReconnectAttempt >= 15000 || lastReconnectAttempt == 0) {
            lastReconnectAttempt = now;
            Serial.println("[WiFi] Connection lost; attempting to reconnect.");
            WiFi.reconnect();
        }
    }

    bool isAPModeActive() {
        return apMode;
    }

    String getIpAddress() {
        if (apMode) {
            return WiFi.softAPIP().toString();
        } else {
            return WiFi.localIP().toString();
        }
    }

    String getSsidName() {
        return activeSsid;
    }

    int getSignalStrength() {
        if (apMode) {
            return 0;
        } else {
            return WiFi.RSSI();
        }
    }
}
