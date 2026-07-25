#ifndef STUDENTS_H
#define STUDENTS_H

#include <Arduino.h>
#include <vector>

struct Student {
    String uid;
    String name;
    String roll;
    String dept;
    String sem;
    String session;
    String phone;
    String email;
};

namespace Students {
    // Load students list from SD card
    bool load();

    // Save students list to SD card
    bool save();

    // Get count of registered students
    size_t getCount();

    // Get all students (read-only reference)
    const std::vector<Student>& getAll();

    // Get a specific student by UID
    bool getByUid(const String& uid, Student& outStudent);

    // Add a new student
    bool add(const Student& student);

    // Update an existing student's details
    bool update(const String& uid, const Student& updatedStudent);

    // Delete a student by UID
    bool remove(const String& uid);

    // Search students by query (name, roll, or uid)
    std::vector<Student> search(const String& query);
}

#endif // STUDENTS_H
