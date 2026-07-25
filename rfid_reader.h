#ifndef RFID_READER_H
#define RFID_READER_H

#include <Arduino.h>

namespace RfidReader {
    // GPIO Pins for Feedback
    extern const int BUZZER_PIN;
    extern const int LED_PIN;

    // Last scan state for dashboard live notification
    extern String lastScanUid;
    extern String lastScanName;
    extern String lastScanTime;
    extern String lastScanStatus; // "Success", "Unknown"
    extern uint32_t lastScanEpoch;

    // Active session subject name
    extern String activeSubject;
    extern String activeSessionId;

    // True only while an attendance session is active.
    extern bool sessionActive;

    // Initialize the MFRC522 reader and feedback pins
    bool init();

    // Call regularly in loop() to scan cards
    void update();

    // Set the active subject/class for this session (resets per-session duplicate list)
    void setActiveSubject(const String& subject);

    // Set the active class and its unique session ID.
    void setActiveSession(const String& subject, const String& sessionId);

    // End the current session (resets subject to General and clears scanned list)
    void endSession();

    // Clear the scan session cache (alias kept for compatibility)
    void clearCooldowns();

    // Feedback triggers
    void triggerSuccessFeedback();
    void triggerFailureFeedback();
    void triggerDuplicateFeedback();
}

#endif // RFID_READER_H
