#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <Arduino.h>
#include <vector>

struct AttendanceRecord {
    String date;
    String time;
    String uid;
    String name;
    String roll;
    String dept;
    String sem;
    String subject; // Class/subject name for this session
    String sessionId; // Unique ID of the session that created this record
    String status;  // Typically "Present"
};

namespace Attendance {
    // Write an attendance entry to the CSV
    bool log(const String& uid, const String& name, const String& roll, 
             const String& dept, const String& sem, const String& subject,
             const String& sessionId);

    // Read records directly from CSV with filtering and search, sorted newest first
    std::vector<AttendanceRecord> getRecords(
        const String& searchVal, 
        const String& dateFilter, 
        const String& deptFilter, 
        const String& semFilter, 
        const String& subjectFilter,
        const String& sessionIdFilter = "",
        int limit = 100
    );

    // Count today's attendance logs
    size_t getTodayCount();

    // Get report data
    // Daily report for a given date: returns map of dept -> count
    String getDailyReportJson(const String& date);

    // Monthly report for a given month (YYYY-MM): returns roll -> present days count
    String getMonthlyReportJson(const String& yearMonth);

    // Student history: returns all records for a specific UID
    std::vector<AttendanceRecord> getStudentHistory(const String& uid);
}

#endif // ATTENDANCE_H
