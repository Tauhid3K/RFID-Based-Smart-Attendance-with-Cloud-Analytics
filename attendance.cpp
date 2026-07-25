#include "attendance.h"
#include "storage.h"
#include "utils.h"
#include <SD.h>
#include <ArduinoJson.h>
#include <algorithm>

namespace Attendance {
    static const char* CSV_HEADER = "Date,Time,UID,Name,Roll,Department,Semester,Subject,SessionID,Status";


    // Helper function to split a string by comma, handling potential empty fields
    static std::vector<String> splitCsvLine(const String& line) {
        std::vector<String> fields;
        int start = 0;
        int end = line.indexOf(',');
        while (end != -1) {
            fields.push_back(line.substring(start, end));
            start = end + 1;
            end = line.indexOf(',', start);
        }
        fields.push_back(line.substring(start));
        return fields;
    }

    // Upgrade only the header when an SD card contains an older attendance
    // file. Existing 8- and 9-column rows are intentionally retained because
    // the reader supports both layouts.
    static bool ensureCurrentCsvHeader() {
        File source = SD.open(Storage::ATTENDANCE_PATH, FILE_READ);
        if (!source || source.size() == 0) {
            if (source) source.close();
            return Storage::appendCsv(Storage::ATTENDANCE_PATH, CSV_HEADER);
        }

        String header = source.readStringUntil('\n');
        header.trim();
        if (header == CSV_HEADER) {
            source.close();
            return true;
        }

        // Only upgrade known historical headers. Do not overwrite a damaged
        // or manually changed file whose format cannot be identified safely.
        const String oldHeader = "Date,Time,UID,Name,Roll,Department,Semester,Status";
        const String previousHeader = "Date,Time,UID,Name,Roll,Department,Semester,Subject,Status";
        if (header != oldHeader && header != previousHeader) {
            source.close();
            Serial.println("[Attendance] Unsupported CSV header; refusing to append.");
            return false;
        }

        const String tempPath = String(Storage::ATTENDANCE_PATH) + ".tmp";
        SD.remove(tempPath.c_str());
        File destination = SD.open(tempPath.c_str(), FILE_WRITE);
        if (!destination) {
            source.close();
            return false;
        }

        destination.println(CSV_HEADER);
        while (source.available()) {
            String row = source.readStringUntil('\n');
            destination.println(row);
        }
        destination.close();
        source.close();

        // Preserve the original file until the replacement is in place. This
        // is safer than deleting it before a rename that may fail.
        const String backupPath = String(Storage::ATTENDANCE_PATH) + ".header-backup";
        SD.remove(backupPath.c_str());
        if (!SD.rename(Storage::ATTENDANCE_PATH, backupPath.c_str())) {
            SD.remove(tempPath.c_str());
            Serial.println("[Attendance] Failed to prepare CSV header upgrade.");
            return false;
        }
        if (!SD.rename(tempPath.c_str(), Storage::ATTENDANCE_PATH)) {
            SD.rename(backupPath.c_str(), Storage::ATTENDANCE_PATH);
            SD.remove(tempPath.c_str());
            Serial.println("[Attendance] Failed to upgrade CSV header.");
            return false;
        }
        SD.remove(backupPath.c_str());

        Serial.println("[Attendance] CSV header upgraded for session IDs.");
        return true;
    }

    bool log(const String& uid, const String& name, const String& roll, 
             const String& dept, const String& sem, const String& subject,
             const String& sessionId) {
        if (!Storage::isAvailable()) {
            Serial.println("[Attendance] SD Card not available for logging");
            return false;
        }
        
        if (!ensureCurrentCsvHeader()) {
            Serial.println("[Attendance] Could not prepare the attendance CSV.");
            return false;
        }

        String date = Utils::getFormattedDate();
        String time = Utils::getFormattedTime();
        String status = "Present";

        // Sanitize commas to prevent broken CSV structures
        String cName = name;    cName.replace(",", " ");
        String cRoll = roll;    cRoll.replace(",", " ");
        String cDept = dept;    cDept.replace(",", " ");
        String cSem  = sem;     cSem.replace(",", " ");
        String cSubj = subject; cSubj.replace(",", " ");
        String cSessionId = sessionId; cSessionId.replace(",", " ");
        if (cSubj.length() == 0) cSubj = "General";

        String line = date + "," + time + "," + uid + "," + cName + "," + cRoll + "," + cDept + "," + cSem + "," + cSubj + "," + cSessionId + "," + status;
        bool success = Storage::appendCsv(Storage::ATTENDANCE_PATH, line);
        if (success) {
            Serial.printf("[Attendance] Logged present for %s (%s) subject=%s at %s\n", cName.c_str(), uid.c_str(), cSubj.c_str(), time.c_str());
        } else {
            Serial.println("[Attendance] Failed to append CSV row!");
        }
        return success;
    }

    std::vector<AttendanceRecord> getRecords(
        const String& searchVal, 
        const String& dateFilter, 
        const String& deptFilter, 
        const String& semFilter,
        const String& subjectFilter,
        const String& sessionIdFilter,
        int limit
    ) {
        std::vector<AttendanceRecord> records;
        if (!Storage::isAvailable()) return records;

        File file = SD.open(Storage::ATTENDANCE_PATH, FILE_READ);
        if (!file) return records;

        // Skip CSV Header
        if (file.available()) {
            file.readStringUntil('\n');
        }

        String searchLower = searchVal;
        searchLower.toLowerCase();
        
        String deptFilterLower = deptFilter;
        deptFilterLower.toLowerCase();
        
        String semFilterLower = semFilter;
        semFilterLower.toLowerCase();

        String subjectFilterLower = subjectFilter;
        subjectFilterLower.toLowerCase();

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;

            std::vector<String> fields = splitCsvLine(line);

            AttendanceRecord r;
            r.date = fields.size() > 0 ? fields[0] : "";
            r.time = fields.size() > 1 ? fields[1] : "";
            r.uid  = fields.size() > 2 ? fields[2] : "";
            r.name = fields.size() > 3 ? fields[3] : "";
            r.roll = fields.size() > 4 ? fields[4] : "";
            r.dept = fields.size() > 5 ? fields[5] : "";
            r.sem  = fields.size() > 6 ? fields[6] : "";
            // Support legacy 8-column rows, prior 9-column rows, and current
            // 10-column rows which include the unique session ID.
            if (fields.size() >= 10) {
                r.subject = fields[7];
                r.sessionId = fields[8];
                r.status  = fields[9];
            } else if (fields.size() >= 9) {
                r.subject = fields[7];
                r.status  = fields[8];
            } else if (fields.size() >= 8) {
                r.subject = ""; // old record, no subject
                r.status  = fields[7];
            } else {
                continue; // malformed row
            }

            // Apply Date Filter
            if (dateFilter.length() > 0 && r.date != dateFilter) continue;
            
            // Apply Department Filter
            if (deptFilter.length() > 0) {
                String rDeptLower = r.dept;
                rDeptLower.toLowerCase();
                if (rDeptLower != deptFilterLower) continue;
            }

            // Apply Semester Filter
            if (semFilter.length() > 0) {
                String rSemLower = r.sem;
                rSemLower.toLowerCase();
                if (rSemLower != semFilterLower) continue;
            }

            // Apply Subject Filter
            if (subjectFilter.length() > 0) {
                String rSubjLower = r.subject;
                rSubjLower.toLowerCase();
                if (rSubjLower != subjectFilterLower) continue;
            }

            if (sessionIdFilter.length() > 0 && !r.sessionId.equalsIgnoreCase(sessionIdFilter)) continue;

            // Apply Text Search Filter (matches name, roll, or uid)
            if (searchVal.length() > 0) {
                String nameLower = r.name; nameLower.toLowerCase();
                String rollLower = r.roll; rollLower.toLowerCase();
                String uidLower  = r.uid;  uidLower.toLowerCase();

                if (nameLower.indexOf(searchLower) == -1 &&
                    rollLower.indexOf(searchLower) == -1 &&
                    uidLower.indexOf(searchLower)  == -1) {
                    continue;
                }
            }

            records.push_back(r);
        }
        file.close();

        // Sort newest first by reversing the vector
        std::reverse(records.begin(), records.end());

        // Trim records to limit if necessary
        if (limit > 0 && records.size() > (size_t)limit) {
            records.resize(limit);
        }

        return records;
    }

    size_t getTodayCount() {
        if (!Storage::isAvailable()) return 0;

        File file = SD.open(Storage::ATTENDANCE_PATH, FILE_READ);
        if (!file) return 0;

        String today = Utils::getFormattedDate();
        size_t count = 0;

        if (file.available()) {
            file.readStringUntil('\n'); // skip header
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;

            int firstComma = line.indexOf(',');
            if (firstComma != -1) {
                String date = line.substring(0, firstComma);
                if (date == today) {
                    count++;
                }
            }
        }
        file.close();
        return count;
    }

    String getDailyReportJson(const String& date) {
        if (!Storage::isAvailable()) return "{}";

        File file = SD.open(Storage::ATTENDANCE_PATH, FILE_READ);
        if (!file) return "{}";

        if (file.available()) {
            file.readStringUntil('\n');
        }

        JsonDocument doc;
        JsonObject depts = doc.to<JsonObject>();

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;

            std::vector<String> fields = splitCsvLine(line);
            if (fields.size() < 8) continue;

            String rDate = fields[0];
            String rDept = fields[5];

            if (rDate == date) {
                int count = depts[rDept] | 0;
                depts[rDept] = count + 1;
            }
        }
        file.close();

        String result;
        serializeJson(doc, result);
        return result;
    }

    String getMonthlyReportJson(const String& yearMonth) {
        if (!Storage::isAvailable()) return "{}";

        File file = SD.open(Storage::ATTENDANCE_PATH, FILE_READ);
        if (!file) return "{}";

        if (file.available()) {
            file.readStringUntil('\n');
        }

        JsonDocument doc;
        JsonObject students = doc.to<JsonObject>();

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;

            std::vector<String> fields = splitCsvLine(line);
            if (fields.size() < 8) continue;

            String rDate = fields[0]; // YYYY-MM-DD
            String rRoll = fields[4];
            String rName = fields[3];

            if (rDate.startsWith(yearMonth)) {
                if (!students.containsKey(rRoll)) {
                    JsonObject sObj = students[rRoll].to<JsonObject>();
                    sObj["name"] = rName;
                    sObj["days"] = 1;
                } else {
                    int count = students[rRoll]["days"] | 0;
                    students[rRoll]["days"] = count + 1;
                }
            }
        }
        file.close();

        String result;
        serializeJson(doc, result);
        return result;
    }

    std::vector<AttendanceRecord> getStudentHistory(const String& uid) {
        std::vector<AttendanceRecord> records;
        if (!Storage::isAvailable()) return records;

        File file = SD.open(Storage::ATTENDANCE_PATH, FILE_READ);
        if (!file) return records;

        if (file.available()) {
            file.readStringUntil('\n');
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;

            std::vector<String> fields = splitCsvLine(line);
            if (fields.size() < 8) continue;

            if (fields[2].equalsIgnoreCase(uid)) {
                AttendanceRecord r;
                r.date = fields[0];
                r.time = fields[1];
                r.uid = fields[2];
                r.name = fields[3];
                r.roll = fields[4];
                r.dept = fields[5];
                r.sem = fields[6];
                // Support legacy 8-column rows, prior 9-column rows, and
                // current 10-column rows that include the session ID.
                if (fields.size() >= 10) {
                    r.subject = fields[7];
                    r.sessionId = fields[8];
                    r.status = fields[9];
                } else if (fields.size() >= 9) {
                    r.subject = fields[7];
                    r.status = fields[8];
                } else {
                    r.status = fields[7];
                }
                records.push_back(r);
            }
        }
        file.close();

        std::reverse(records.begin(), records.end());
        return records;
    }
}
