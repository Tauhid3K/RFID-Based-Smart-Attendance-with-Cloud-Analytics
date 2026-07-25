#include "sessions.h"
#include "storage.h"
#include "utils.h"
#include "rfid_reader.h"
#include "attendance.h"
#include <ArduinoJson.h>

namespace Sessions {
    static std::vector<Session> sessionList;

    bool load() {
        sessionList.clear();
        JsonDocument doc;
        if (!Storage::readJson(Storage::SESSIONS_PATH, doc)) {
            Serial.println("[Sessions] No sessions file found or invalid. Initializing empty DB...");
            save();
            return false;
        }

        JsonArray arr = doc.as<JsonArray>();
        for (JsonObject obj : arr) {
            Session s;
            s.id                 = obj["id"] | "";
            s.date               = obj["date"] | "";
            s.subject            = obj["subject"] | "";
            s.teacher            = obj["teacher"] | "";
            s.dept               = obj["dept"] | "";
            s.sem                = obj["sem"] | "";
            s.section            = obj["section"] | obj["sec"] | "";
            s.room               = obj["room"] | "";
            s.notes              = obj["notes"] | "";
            s.startEpoch         = obj["startEpoch"] | 0;
            s.startTime          = obj["startTime"] | "";
            s.endEpoch           = obj["endEpoch"] | 0;
            s.endTime            = obj["endTime"] | "";
            s.scheduledDate      = obj["scheduledDate"] | "";
            s.scheduledStartTime = obj["scheduledStartTime"] | "";
            s.scheduledEndTime   = obj["scheduledEndTime"] | "";
            s.duration           = obj["duration"] | "";
            s.presentsCount      = obj["presentsCount"] | 0;
            s.status             = obj["status"] | "Ended";
            s.autoStartDisabled  = obj["autoStartDisabled"] | false;

            if (s.id.length() > 0) {
                sessionList.push_back(s);
            }
        }

        Serial.printf("[Sessions] Loaded %d sessions from SD card.\n", (int)sessionList.size());
        
        // Restore RFID reader active subject if an active session exists
        Session active;
        if (getActive(active)) {
            RfidReader::setActiveSession(active.subject, active.id);
        }

        return true;
    }

    bool save() {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();

        for (const auto& s : sessionList) {
            JsonObject obj = arr.add<JsonObject>();
            obj["id"]                 = s.id;
            obj["date"]               = s.date;
            obj["subject"]            = s.subject;
            obj["teacher"]            = s.teacher;
            obj["dept"]               = s.dept;
            obj["sem"]                = s.sem;
            obj["section"]            = s.section;
            obj["room"]               = s.room;
            obj["notes"]              = s.notes;
            obj["startEpoch"]         = s.startEpoch;
            obj["startTime"]          = s.startTime;
            obj["endEpoch"]           = s.endEpoch;
            obj["endTime"]            = s.endTime;
            obj["scheduledDate"]      = s.scheduledDate;
            obj["scheduledStartTime"] = s.scheduledStartTime;
            obj["scheduledEndTime"]   = s.scheduledEndTime;
            obj["duration"]           = s.duration;
            obj["presentsCount"]      = s.presentsCount;
            obj["status"]             = s.status;
            obj["autoStartDisabled"]  = s.autoStartDisabled;
        }

        if (!Storage::writeJson(Storage::SESSIONS_PATH, doc)) {
            Serial.println("[Sessions] Failed to write sessions list to SD card!");
            return false;
        }

        return true;
    }

    const std::vector<Session>& getAll() {
        return sessionList;
    }

    bool getById(const String& id, Session& outSession) {
        for (const auto& s : sessionList) {
            if (s.id.equalsIgnoreCase(id)) {
                outSession = s;
                return true;
            }
        }
        return false;
    }

    bool getActive(Session& outSession) {
        for (const auto& s : sessionList) {
            if (s.status.equalsIgnoreCase("Active")) {
                outSession = s;
                // Live calculate present count from attendance records for this subject today
                std::vector<AttendanceRecord> records = Attendance::getRecords("", s.date, s.dept, s.sem, s.subject, s.id, 1000);
                outSession.presentsCount = (int)records.size();
                return true;
            }
        }
        return false;
    }

    bool startQuick(const Session& sessionData, Session& outStartedSession) {
        // End any active session first
        Session activeDummy;
        if (getActive(activeDummy)) {
            endActive(activeDummy);
        }

        uint32_t nowEpoch = Utils::getEpoch();
        String nowTime = Utils::getFormattedTime();
        String nowDate = Utils::getFormattedDate();

        Session s = sessionData;
        s.id = "SESS_" + String(nowEpoch) + "_" + String(random(100, 999));
        s.date = nowDate;
        s.startEpoch = nowEpoch;
        s.startTime = nowTime;
        s.status = "Active";
        s.presentsCount = 0;
        s.duration = "0 mins";
        s.autoStartDisabled = false;

        if (s.subject.length() == 0) s.subject = "General";

        sessionList.push_back(s);
        outStartedSession = s;

        // Set RFID reader active subject
        RfidReader::setActiveSession(s.subject, s.id);

        Serial.printf("[Sessions] Started Quick Session: %s (ID: %s) at %s\n", s.subject.c_str(), s.id.c_str(), s.startTime.c_str());
        return save();
    }

    bool startScheduled(const String& id, Session& outStartedSession) {
        // End any active session first
        Session activeDummy;
        if (getActive(activeDummy)) {
            endActive(activeDummy);
        }

        for (size_t i = 0; i < sessionList.size(); i++) {
            if (sessionList[i].id.equalsIgnoreCase(id)) {
                uint32_t nowEpoch = Utils::getEpoch();
                String nowTime = Utils::getFormattedTime();
                String nowDate = Utils::getFormattedDate();

                sessionList[i].startEpoch = nowEpoch;
                sessionList[i].startTime = nowTime;
                sessionList[i].date = nowDate;
                sessionList[i].status = "Active";
                sessionList[i].presentsCount = 0;
                sessionList[i].duration = "0 mins";
                sessionList[i].autoStartDisabled = false;

                outStartedSession = sessionList[i];

                // Set RFID reader active subject
                RfidReader::setActiveSession(sessionList[i].subject, sessionList[i].id);

                Serial.printf("[Sessions] Started Scheduled Session: %s (ID: %s) at %s\n", 
                    sessionList[i].subject.c_str(), sessionList[i].id.c_str(), sessionList[i].startTime.c_str());
                return save();
            }
        }
        return false;
    }

    bool schedule(const Session& sessionData, Session& outScheduledSession) {
        Session s = sessionData;
        uint32_t nowEpoch = Utils::getEpoch();
        s.id = "SCHED_" + String(nowEpoch) + "_" + String(random(100, 999));
        s.status = "Scheduled";
        s.presentsCount = 0;
        s.autoStartDisabled = false;
        if (s.subject.length() == 0) s.subject = "General";

        sessionList.push_back(s);
        outScheduledSession = s;

        Serial.printf("[Sessions] Scheduled Session created: %s for date %s %s-%s\n", 
            s.subject.c_str(), s.scheduledDate.c_str(), s.scheduledStartTime.c_str(), s.scheduledEndTime.c_str());
        return save();
    }

    bool endActive(Session& outEndedSession) {
        for (size_t i = 0; i < sessionList.size(); i++) {
            if (sessionList[i].status.equalsIgnoreCase("Active")) {
                uint32_t nowEpoch = Utils::getEpoch();
                sessionList[i].endEpoch = nowEpoch;
                sessionList[i].endTime = Utils::getFormattedTime();
                sessionList[i].status = "Ended";
                if (sessionList[i].scheduledDate.length() > 0) {
                    sessionList[i].autoStartDisabled = true;
                }

                uint32_t diffSec = (nowEpoch > sessionList[i].startEpoch) ? (nowEpoch - sessionList[i].startEpoch) : 0;
                uint32_t mins = diffSec / 60;
                uint32_t hrs = mins / 60;
                mins %= 60;
                if (hrs > 0) {
                    sessionList[i].duration = String(hrs) + "h " + String(mins) + "m";
                } else {
                    sessionList[i].duration = String(mins) + " mins";
                }

                // Count presents for this session
                std::vector<AttendanceRecord> records = Attendance::getRecords("", sessionList[i].date, sessionList[i].dept, sessionList[i].sem, sessionList[i].subject, sessionList[i].id, 1000);
                sessionList[i].presentsCount = (int)records.size();

                outEndedSession = sessionList[i];

                // Reset RFID reader
                RfidReader::endSession();

                Serial.printf("[Sessions] Ended Session: %s (Duration: %s, Presents: %d)\n", 
                    sessionList[i].subject.c_str(), sessionList[i].duration.c_str(), sessionList[i].presentsCount);
                return save();
            }
        }
        return false;
    }

    bool deleteById(const String& id) {
        for (auto it = sessionList.begin(); it != sessionList.end(); ++it) {
            if (it->id.equalsIgnoreCase(id)) {
                if (it->status.equalsIgnoreCase("Active")) {
                    Serial.println("[Sessions] Cannot delete an active session.");
                    return false;
                }
                Serial.printf("[Sessions] Deleted session: %s (%s)\n", it->subject.c_str(), id.c_str());
                sessionList.erase(it);
                return save();
            }
        }
        Serial.printf("[Sessions] Session not found for delete: %s\n", id.c_str());
        return false;
    }

    bool clearHistory() {
        std::vector<Session> previous = sessionList;
        size_t removed = 0;
        for (auto it = sessionList.begin(); it != sessionList.end();) {
            if (it->status.equalsIgnoreCase("Ended")) {
                it = sessionList.erase(it);
                removed++;
            } else {
                ++it;
            }
        }

        if (!save()) {
            sessionList = previous;
            Serial.println("[Sessions] Clear history failed; restored previous session list.");
            return false;
        }

        Serial.printf("[Sessions] Cleared %u ended session(s).\n", (unsigned)removed);
        return true;
    }

    bool updateById(const String& id, const Session& updatedData) {
        if (updatedData.subject.length() == 0 || updatedData.scheduledDate.length() == 0 || updatedData.scheduledStartTime.length() == 0) {
            Serial.println("[Sessions] Scheduled session needs subject, date, and start time.");
            return false;
        }
        if (updatedData.scheduledEndTime.length() > 0 && updatedData.scheduledEndTime < updatedData.scheduledStartTime) {
            Serial.println("[Sessions] Scheduled session end time is before start time.");
            return false;
        }
        for (size_t i = 0; i < sessionList.size(); i++) {
            if (sessionList[i].id.equalsIgnoreCase(id)) {
                if (!sessionList[i].status.equalsIgnoreCase("Scheduled")) {
                    Serial.println("[Sessions] Can only edit Scheduled sessions.");
                    return false;
                }
                Session previous = sessionList[i];
                sessionList[i].subject            = updatedData.subject;
                sessionList[i].teacher             = updatedData.teacher;
                sessionList[i].dept                = updatedData.dept;
                sessionList[i].sem                 = updatedData.sem;
                sessionList[i].section             = updatedData.section;
                sessionList[i].room                = updatedData.room;
                sessionList[i].notes               = updatedData.notes;
                sessionList[i].scheduledDate       = updatedData.scheduledDate;
                sessionList[i].scheduledStartTime  = updatedData.scheduledStartTime;
                sessionList[i].scheduledEndTime    = updatedData.scheduledEndTime;
                sessionList[i].autoStartDisabled   = false;
                if (!save()) {
                    sessionList[i] = previous;
                    Serial.println("[Sessions] Update failed; restored previous session data.");
                    return false;
                }
                Serial.printf("[Sessions] Updated session: %s (%s)\n", updatedData.subject.c_str(), id.c_str());
                return true;
            }
        }
        Serial.printf("[Sessions] Session not found for update: %s\n", id.c_str());
        return false;
    }

    void checkScheduledAutoStart() {
        Session active;
        if (getActive(active)) return; // Don't interrupt an ongoing active session

        String today = Utils::getFormattedDate();
        String nowTime = Utils::getFormattedTime();

        for (size_t i = 0; i < sessionList.size(); i++) {
            if (sessionList[i].status.equalsIgnoreCase("Scheduled") && !sessionList[i].autoStartDisabled) {
                if (sessionList[i].scheduledDate == today && 
                    sessionList[i].scheduledStartTime <= nowTime && 
                    (sessionList[i].scheduledEndTime.length() == 0 || sessionList[i].scheduledEndTime >= nowTime)) {
                    
                    Serial.printf("[Sessions] Auto-starting Scheduled Session: %s\n", sessionList[i].subject.c_str());
                    sessionList[i].startEpoch = Utils::getEpoch();
                    sessionList[i].startTime = nowTime;
                    sessionList[i].date = today;
                    sessionList[i].status = "Active";

                    RfidReader::setActiveSession(sessionList[i].subject, sessionList[i].id);
                    save();
                    break;
                }
            }
        }
    }
}
