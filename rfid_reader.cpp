#include "rfid_reader.h"
#include "config.h"
#include "storage.h"
#include "students.h"
#include "attendance.h"
#include "utils.h"
#include <SPI.h>
#include <MFRC522.h>

namespace RfidReader {
    const int BUZZER_PIN = 12;
    const int LED_PIN = 14;

    String lastScanUid = "";
    String lastScanName = "";
    String lastScanTime = "";
    String lastScanStatus = "None";
    uint32_t lastScanEpoch = 0;

    static MFRC522 mfrc522(Storage::RFID_CS_PIN, 22); // Reset PIN = 22

    // Active session subject set from web dashboard
    String activeSubject = "General";
    String activeSessionId = "";
    bool sessionActive = false;

    struct ScanCooldown {
        String uid;
        uint32_t epoch;
    };

    // Last accepted scan for each UID in the current session.
    static std::vector<ScanCooldown> sessionScannedUids;

    bool init() {
        pinMode(BUZZER_PIN, OUTPUT);
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);

        // Initialize RC522 reader
        mfrc522.PCD_Init();
        
        // Read version of MFRC522 to verify connection
        byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
        if (v == 0x00 || v == 0xFF) {
            Serial.println("[RFID] RC522 communication test failed! Check wiring.");
            return false;
        }

        Serial.printf("[RFID] RC522 Reader initialized. Firmware: 0x%02X\n", v);
        return true;
    }

    void triggerSuccessFeedback() {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(150);
        digitalWrite(BUZZER_PIN, LOW);
        delay(250);
        digitalWrite(LED_PIN, LOW);
    }

    void triggerFailureFeedback() {
        digitalWrite(BUZZER_PIN, HIGH);
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
        }
        digitalWrite(BUZZER_PIN, LOW);
    }

    void triggerDuplicateFeedback() {
        // Three short beeps and LED flashes for a repeated card scan.
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_PIN, HIGH);
            digitalWrite(BUZZER_PIN, HIGH);
            delay(80);
            digitalWrite(LED_PIN, LOW);
            digitalWrite(BUZZER_PIN, LOW);
            delay(80);
        }
    }

    void clearCooldowns() {
        sessionScannedUids.clear(); // kept for API compatibility
    }

    void setActiveSubject(const String& subject) {
        setActiveSession(subject, "");
    }

    void setActiveSession(const String& subject, const String& sessionId) {
        activeSubject = subject;
        activeSessionId = sessionId;
        sessionActive = true;
        sessionScannedUids.clear(); // new subject = fresh duplicate list
        Serial.printf("[RFID] Active session set to: %s\n", activeSubject.c_str());
    }

    void endSession() {
        activeSubject = "General";
        activeSessionId = "";
        sessionActive = false;
        sessionScannedUids.clear();
        Serial.println("[RFID] Session ended.");
    }

    static String getUidString(MFRC522::Uid* uid) {
        String uidStr = "";
        for (byte i = 0; i < uid->size; i++) {
            if (uid->uidByte[i] < 0x10) uidStr += "0";
            uidStr += String(uid->uidByte[i], HEX);
        }
        uidStr.toUpperCase();
        return uidStr;
    }

    void update() {
        // Check for card presence. SPI select pins are toggled by MFRC522 library
        if (!mfrc522.PICC_IsNewCardPresent()) {
            return;
        }

        // Select the card
        if (!mfrc522.PICC_ReadCardSerial()) {
            return;
        }

        String uid = getUidString(&(mfrc522.uid));
        uint32_t nowEpoch = Utils::getEpoch();

        if (!sessionActive) {
            Serial.printf("[RFID] Ignored UID %s because no session is active.\n", uid.c_str());
            lastScanUid = uid;
            lastScanName = "No Active Session";
            lastScanTime = Utils::getFormattedTime();
            lastScanStatus = "No Active Session";
            lastScanEpoch = nowEpoch;
            triggerFailureFeedback();
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }

        // Per-session duplicate check: same UID cannot scan twice in the same subject session
        for (auto& scanned : sessionScannedUids) {
            if (scanned.uid.equalsIgnoreCase(uid)) {
                uint32_t elapsed = nowEpoch >= scanned.epoch ? nowEpoch - scanned.epoch : 0;
                if (elapsed >= Config::cooldownS) {
                    scanned.epoch = nowEpoch;
                    triggerDuplicateFeedback();
                }
                Serial.printf("[RFID] UID %s already scanned in session '%s'. Ignoring.\n", uid.c_str(), activeSubject.c_str());
                mfrc522.PICC_HaltA();
                mfrc522.PCD_StopCrypto1();
                return;
            }
        }

        // 3. Match UID against registered database
        Student student;
        bool registered = Students::getByUid(uid, student);

        if (registered) {
            Serial.printf("[RFID] Scanned card: %s (Roll: %s) session='%s' - scan #%d this session\n",
                student.name.c_str(), student.roll.c_str(), activeSubject.c_str(), (int)sessionScannedUids.size());
            
            // Append CSV attendance record with subject
            if (!Attendance::log(student.uid, student.name, student.roll, student.dept, student.sem, activeSubject, activeSessionId)) {
                Serial.println("[RFID] Attendance was not recorded; card remains eligible for retry.");
                lastScanUid = student.uid;
                lastScanName = student.name;
                lastScanTime = Utils::getFormattedTime();
                lastScanStatus = "Storage Error";
                lastScanEpoch = nowEpoch;
                triggerFailureFeedback();
                mfrc522.PICC_HaltA();
                mfrc522.PCD_StopCrypto1();
                return;
            }
            sessionScannedUids.push_back({uid, nowEpoch});

            // Update live dashboard states
            lastScanUid = student.uid;
            lastScanName = student.name;
            lastScanTime = Utils::getFormattedTime();
            lastScanStatus = "Success";
            lastScanEpoch = nowEpoch;

            triggerSuccessFeedback();
        } else {
            Serial.printf("[RFID] Unknown card UID: %s\n", uid.c_str());

            lastScanUid = uid;
            lastScanName = "Unknown Card";
            lastScanTime = Utils::getFormattedTime();
            lastScanStatus = "Unknown";
            lastScanEpoch = nowEpoch;
            sessionScannedUids.push_back({uid, nowEpoch});

            triggerFailureFeedback();
        }

        // Halt card reading
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    }
}
