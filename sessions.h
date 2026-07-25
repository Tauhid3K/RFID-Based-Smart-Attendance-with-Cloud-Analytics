#ifndef SESSIONS_H
#define SESSIONS_H

#include <Arduino.h>
#include <vector>

struct Session {
    String id;
    String date;
    String subject;
    String teacher;
    String dept;
    String sem;
    String section;
    String room;
    String notes;
    uint32_t startEpoch = 0;
    String startTime;
    uint32_t endEpoch = 0;
    String endTime;
    String scheduledDate;
    String scheduledStartTime;
    String scheduledEndTime;
    String duration;
    int presentsCount = 0;
    String status; // "Active", "Ended", "Scheduled"
    bool autoStartDisabled = false;
};

namespace Sessions {
    // Load session list from SD card (/sessions.json)
    bool load();

    // Save session list to SD card (/sessions.json)
    bool save();

    // Get all sessions
    const std::vector<Session>& getAll();

    // Get session by ID
    bool getById(const String& id, Session& outSession);

    // Get currently active session (returns true if active session exists)
    bool getActive(Session& outSession);

    // Start a Quick Session immediately
    bool startQuick(const Session& sessionData, Session& outStartedSession);

    // Start an existing scheduled session immediately by ID
    bool startScheduled(const String& id, Session& outStartedSession);

    // Create a Scheduled Session for a specific date/time
    bool schedule(const Session& sessionData, Session& outScheduledSession);

    // End the active session
    bool endActive(Session& outEndedSession);

    // Delete a session by ID (cannot delete Active sessions)
    bool deleteById(const String& id);

    // Remove all ended sessions while preserving active and scheduled sessions.
    bool clearHistory();

    // Update a scheduled session by ID
    bool updateById(const String& id, const Session& updatedData);

    // Check scheduled sessions and auto-start if match current date/time
    void checkScheduledAutoStart();
}

#endif // SESSIONS_H
