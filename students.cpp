#include "students.h"
#include "storage.h"
#include <ArduinoJson.h>

namespace Students {
    static std::vector<Student> studentList;

    bool load() {
        studentList.clear();
        
        JsonDocument doc;
        if (!Storage::readJson(Storage::STUDENTS_PATH, doc)) {
            Serial.println("[Students] No students file found or invalid. Initializing empty DB...");
            save(); // Create an empty file
            return false;
        }

        JsonArray arr = doc.as<JsonArray>();
        for (JsonObject obj : arr) {
            Student s;
            s.uid = obj["uid"] | "";
            s.name = obj["name"] | "";
            s.roll = obj["roll"] | "";
            s.dept = obj["dept"] | "";
            s.sem = obj["sem"] | "";
            s.session = obj["session"] | obj["sec"] | "";
            s.phone = obj["phone"] | "";
            s.email = obj["email"] | "";
            
            if (s.uid.length() > 0) {
                studentList.push_back(s);
            }
        }

        Serial.printf("[Students] Loaded %d students from SD card.\n", studentList.size());
        return true;
    }

    bool save() {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();

        for (const auto& s : studentList) {
            JsonObject obj = arr.add<JsonObject>();
            obj["uid"] = s.uid;
            obj["name"] = s.name;
            obj["roll"] = s.roll;
            obj["dept"] = s.dept;
            obj["sem"] = s.sem;
            obj["session"] = s.session;
            obj["phone"] = s.phone;
            obj["email"] = s.email;
        }

        if (!Storage::writeJson(Storage::STUDENTS_PATH, doc)) {
            Serial.println("[Students] Failed to write students list to SD card!");
            return false;
        }

        Serial.println("[Students] Students list saved to SD card.");
        return true;
    }

    size_t getCount() {
        return studentList.size();
    }

    const std::vector<Student>& getAll() {
        return studentList;
    }

    bool getByUid(const String& uid, Student& outStudent) {
        String targetUid = uid;
        targetUid.trim();
        for (const auto& s : studentList) {
            String sUid = s.uid;
            sUid.trim();
            if (sUid.equalsIgnoreCase(targetUid)) {
                outStudent = s;
                return true;
            }
        }
        return false;
    }

    bool add(const Student& student) {
        Student newStudent = student;
        newStudent.uid.trim();
        if (newStudent.uid.length() == 0) return false;

        // Check for duplicates
        for (const auto& s : studentList) {
            String sUid = s.uid;
            sUid.trim();
            if (sUid.equalsIgnoreCase(newStudent.uid)) {
                Serial.printf("[Students] Add failed: Student with UID %s already exists\n", newStudent.uid.c_str());
                return false;
            }
        }

        studentList.push_back(newStudent);
        return save(); // Auto-save on change
    }

    bool update(const String& uid, const Student& updatedStudent) {
        String targetUid = uid;
        targetUid.trim();
        for (size_t i = 0; i < studentList.size(); i++) {
            String existingUid = studentList[i].uid;
            existingUid.trim();
            if (existingUid.equalsIgnoreCase(targetUid)) {
                // If UID is being changed, make sure new UID is unique
                String newUid = updatedStudent.uid;
                newUid.trim();
                if (!targetUid.equalsIgnoreCase(newUid)) {
                    for (size_t j = 0; j < studentList.size(); j++) {
                        String otherUid = studentList[j].uid;
                        otherUid.trim();
                        if (j != i && otherUid.equalsIgnoreCase(newUid)) {
                            Serial.printf("[Students] Update failed: Target UID %s already exists\n", updatedStudent.uid.c_str());
                            return false;
                        }
                    }
                }
                const Student previousStudent = studentList[i];
                studentList[i] = updatedStudent;
                studentList[i].uid.trim();
                if (!save()) {
                    studentList[i] = previousStudent;
                    Serial.println("[Students] Update failed; restored previous student data.");
                    return false;
                }
                return true;
            }
        }
        return false;
    }

    bool remove(const String& uid) {
        String targetUid = uid;
        targetUid.trim();
        if (targetUid.length() == 0) return false;

        for (auto it = studentList.begin(); it != studentList.end(); ++it) {
            String itemUid = it->uid;
            itemUid.trim();
            if (itemUid.equalsIgnoreCase(targetUid)) {
                Serial.printf("[Students] Removing student: %s (UID: %s)\n", it->name.c_str(), itemUid.c_str());
                const size_t index = it - studentList.begin();
                const Student removedStudent = *it;
                studentList.erase(it);
                if (!save()) {
                    // Do not report a successful deletion if it was not saved
                    // to the SD card. Restore the exact original position.
                    studentList.insert(studentList.begin() + index, removedStudent);
                    Serial.println("[Students] Remove failed; restored student in memory.");
                    return false;
                }
                return true;
            }
        }
        Serial.printf("[Students] Remove failed: UID %s not found in database of %d students\n", targetUid.c_str(), (int)studentList.size());
        return false;
    }

    std::vector<Student> search(const String& query) {
        std::vector<Student> results;
        if (query.length() == 0) return studentList;

        String queryLower = query;
        queryLower.toLowerCase();

        for (const auto& s : studentList) {
            String nameLower = s.name; nameLower.toLowerCase();
            String rollLower = s.roll; rollLower.toLowerCase();
            String uidLower = s.uid; uidLower.toLowerCase();
            String deptLower = s.dept; deptLower.toLowerCase();

            if (nameLower.indexOf(queryLower) >= 0 ||
                rollLower.indexOf(queryLower) >= 0 ||
                uidLower.indexOf(queryLower) >= 0 ||
                deptLower.indexOf(queryLower) >= 0) {
                results.push_back(s);
            }
        }
        return results;
    }
}
