#include "webserver.h"
#include "web_pages.h"
#include "config.h"
#include "storage.h"
#include "students.h"
#include "attendance.h"
#include "sessions.h"
#include "utils.h"
#include "wifi_manager.h"
#include "rfid_reader.h"

// Removed conflicting macro
// Using HTTP method enums from ESPAsyncWebServer
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <esp_system.h>

namespace WebServerManager {
    static AsyncWebServer server(80);
    static String sessionToken = "";

    // Helper to extract a bearer token and check authorization.
    static bool isAuthorized(AsyncWebServerRequest *request) {
        String token = "";

        // Try Authorization: Bearer <token> header
        if (request->hasHeader("Authorization")) {
            String header = request->getHeader("Authorization")->value();
            header.trim();
            if (header.startsWith("Bearer ")) {
                token = header.substring(7);
                token.trim();
            }
        }

        return token.length() > 0 && token.equals(sessionToken);
    }

    // Helper to buffer POST/PUT body chunks
    static void bufferBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (index == 0) {
            request->_tempObject = malloc(total + 1);
            if (request->_tempObject) {
                ((char*)request->_tempObject)[0] = '\0';
            }
        }
        if (request->_tempObject) {
            memcpy((char*)request->_tempObject + index, data, len);
            ((char*)request->_tempObject)[index + len] = '\0';
        }
    }

    // Custom handler for PUT /api/students/{uid}
    class StudentsPutHandler : public AsyncWebHandler {
    public:
        bool canHandle(AsyncWebServerRequest *request) override {
            if (request->method() == HTTP_OPTIONS && request->url().startsWith("/api/students/") && request->url().length() > 14) {
                return true;
            }
            return (request->method() == HTTP_PUT && request->url().startsWith("/api/students/") && request->url().length() > 14);
        }

        void handleRequest(AsyncWebServerRequest *request) override {
            if (request->method() == HTTP_OPTIONS) {
                request->send(204);
                return;
            }

            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            String rawPath = request->url().substring(14);
            String uid = Utils::urlDecode(rawPath);
            uid.trim();

            if (uid.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Missing student UID\"}");
                return;
            }

            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, bodyStr);
            if (err) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            Student s;
            s.uid = doc["uid"] | uid;
            s.name = doc["name"] | "";
            s.roll = doc["roll"] | "";
            s.dept = doc["dept"] | "";
            s.sem = doc["sem"] | "";
            s.session = doc["session"] | doc["sec"] | "";
            s.phone = doc["phone"] | "";
            s.email = doc["email"] | "";

            if (s.name.length() == 0 || s.roll.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Name and Roll are required\"}");
                return;
            }

            if (Students::update(uid, s)) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(404, "application/json", "{\"error\":\"Student not found or UID conflict\"}");
            }
        }

        void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) override {
            bufferBody(request, data, len, index, total);
        }
    };

    // Custom handler for DELETE /api/students/{uid}
    class StudentsDeleteHandler : public AsyncWebHandler {
    public:
        bool canHandle(AsyncWebServerRequest *request) override {
            if (request->method() == HTTP_OPTIONS && request->url().startsWith("/api/students/") && request->url().length() > 14) {
                return true;
            }
            return (request->method() == HTTP_DELETE && request->url().startsWith("/api/students/") && request->url().length() > 14);
        }

        void handleRequest(AsyncWebServerRequest *request) override {
            if (request->method() == HTTP_OPTIONS) {
                request->send(204);
                return;
            }

            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            String rawPath = request->url().substring(14);
            String uid = Utils::urlDecode(rawPath);
            uid.trim();

            if (uid.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Missing student UID\"}");
                return;
            }

            if (Students::remove(uid)) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(404, "application/json", "{\"error\":\"Student not found\"}");
            }
        }
    };
    
    // Server Init
    void init() {
        // Configure CORS Default Headers
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");

        // Use an unpredictable, boot-scoped token. It is deliberately never
        // logged or included in URLs, where browser history and proxies leak it.
        sessionToken = "sess_" + String(esp_random(), HEX) + String(esp_random(), HEX) +
                       String(esp_random(), HEX) + String(esp_random(), HEX);

        // Serve SPA dashboard in chunks to safely stream 66KB HTML from flash memory
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
                size_t totalLen = strlen_P(INDEX_HTML);
                if (index >= totalLen) {
                    return 0;
                }
                size_t len = totalLen - index;
                if (len > maxLen) {
                    len = maxLen;
                }
                memcpy_P(buffer, INDEX_HTML + index, len);
                return len;
            });
            // The dashboard is embedded in the firmware. Never let a browser
            // keep an old dashboard after a new firmware upload.
            response->addHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
            response->addHeader("Pragma", "no-cache");
            request->send(response);
        });

        // POST /api/login
        server.on("/api/login", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, bodyStr);
            if (err) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String user = doc["username"] | "";
            String pass = doc["password"] | "";

            if (user.equals(Config::adminUser) && pass.equals(Config::adminPass)) {
                JsonDocument resDoc;
                resDoc["status"] = "success";
                resDoc["token"] = sessionToken;
                String resStr;
                serializeJson(resDoc, resStr);
                request->send(200, "application/json", resStr);
            } else {
                request->send(401, "application/json", "{\"error\":\"Invalid credentials\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // GET /api/dashboard
        server.on("/api/dashboard", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            
            JsonDocument doc;
            doc["uptime"] = Utils::getUptimeString();
            doc["studentsCount"] = Students::getCount();
            doc["todayAttendance"] = Attendance::getTodayCount();
            
            JsonObject sd = doc.createNestedObject("sdStatus");
            bool sdOk = Storage::isAvailable();
            sd["available"] = sdOk;
            if (sdOk) {
                sd["totalBytes"] = Storage::getCardSize();
                sd["usedBytes"] = Storage::getUsedBytes();
            } else {
                sd["totalBytes"] = 0;
                sd["usedBytes"] = 0;
            }
            
            JsonObject wifi = doc.createNestedObject("wifi");
            bool isAP = WifiManager::isAPModeActive();
            wifi["mode"] = isAP ? "AP" : "STA";
            wifi["ssid"] = WifiManager::getSsidName();
            wifi["rssi"] = WifiManager::getSignalStrength();
            wifi["ip"] = WifiManager::getIpAddress();
            
            JsonObject lastScan = doc.createNestedObject("lastScan");
            lastScan["uid"] = RfidReader::lastScanUid;
            lastScan["name"] = RfidReader::lastScanName;
            lastScan["time"] = RfidReader::lastScanTime;
            lastScan["status"] = RfidReader::lastScanStatus;
            lastScan["epoch"] = RfidReader::lastScanEpoch;

            doc["systemTime"] = Utils::getFormattedDate() + " " + Utils::getFormattedTime();
            doc["epoch"] = Utils::getEpoch();

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });

        // GET /api/students
        server.on("/api/students", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            String query = "";
            if (request->hasParam("search")) {
                query = request->getParam("search")->value();
            }

            std::vector<Student> list = Students::search(query);
            JsonDocument doc;
            JsonArray arr = doc.to<JsonArray>();

            for (const auto& s : list) {
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

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });

        // POST /api/students
        server.on("/api/students", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, bodyStr);
            if (err) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            Student s;
            s.uid = doc["uid"] | "";
            s.name = doc["name"] | "";
            s.roll = doc["roll"] | "";
            s.dept = doc["dept"] | "";
            s.sem = doc["sem"] | "";
            s.session = doc["session"] | doc["sec"] | "";
            s.phone = doc["phone"] | "";
            s.email = doc["email"] | "";
            String originalUid = doc["originalUid"] | "";
            originalUid.trim();

            if (s.uid.length() == 0 || s.name.length() == 0 || s.roll.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"UID, Name, and Roll are required\"}");
                return;
            }

            // An edit uses the same reliable POST route as registration, but
            // includes the UID of the record it is replacing. This prevents
            // the unchanged card UID from being checked as a new registration.
            if (originalUid.length() > 0) {
                if (Students::update(originalUid, s)) {
                    request->send(200, "application/json", "{\"status\":\"success\"}");
                } else {
                    request->send(400, "application/json", "{\"error\":\"Student update failed. The card may belong to another student.\"}");
                }
                return;
            }

            if (Students::add(s)) {
                request->send(201, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(400, "application/json", "{\"error\":\"UID conflict. Card already registered.\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // POST /api/student-delete - reliable JSON-based delete endpoint.
        // This avoids depending on URL path decoding for RFID UIDs.
        server.on("/api/student-delete", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            if (deserializeJson(doc, bodyStr)) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String uid = doc["uid"] | "";
            uid.trim();
            if (uid.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Missing student UID\"}");
                return;
            }

            if (Students::remove(uid)) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(404, "application/json", "{\"error\":\"Student not found\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // POST /api/students/update - use the same standard JSON request
        // pattern as the other dashboard actions. It avoids custom PUT route
        // handling that can drop authentication on some library versions.
        server.on("/api/students/update", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            if (deserializeJson(doc, bodyStr)) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String originalUid = doc["originalUid"] | "";
            originalUid.trim();
            Student updated;
            updated.uid = doc["uid"] | originalUid;
            updated.name = doc["name"] | "";
            updated.roll = doc["roll"] | "";
            updated.dept = doc["dept"] | "";
            updated.sem = doc["sem"] | "";
            updated.session = doc["session"] | doc["sec"] | "";
            updated.phone = doc["phone"] | "";
            updated.email = doc["email"] | "";

            if (originalUid.length() == 0 || updated.uid.length() == 0 || updated.name.length() == 0 || updated.roll.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"UID, name, and roll are required\"}");
                return;
            }

            if (Students::update(originalUid, updated)) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(400, "application/json", "{\"error\":\"Student update failed\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // GET /api/attendance
        server.on("/api/attendance", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            String search  = request->hasParam("search")  ? request->getParam("search")->value()  : "";
            String date    = request->hasParam("date")    ? request->getParam("date")->value()    : "";
            String dept    = request->hasParam("dept")    ? request->getParam("dept")->value()    : "";
            String sem     = request->hasParam("sem")     ? request->getParam("sem")->value()     : "";
            String subject = request->hasParam("subject") ? request->getParam("subject")->value() : "";
            int limit = request->hasParam("limit") ? request->getParam("limit")->value().toInt() : 100;

            String sessionId = request->hasParam("sessionId") ? request->getParam("sessionId")->value() : "";
            std::vector<AttendanceRecord> list = Attendance::getRecords(search, date, dept, sem, subject, sessionId, limit);
            JsonDocument doc;
            JsonArray arr = doc.to<JsonArray>();

            for (const auto& r : list) {
                JsonObject obj = arr.add<JsonObject>();
                obj["date"]    = r.date;
                obj["time"]    = r.time;
                obj["uid"]     = r.uid;
                obj["name"]    = r.name;
                obj["roll"]    = r.roll;
                obj["dept"]    = r.dept;
                obj["sem"]     = r.sem;
                obj["subject"] = r.subject;
                obj["sessionId"] = r.sessionId;
                obj["status"]  = r.status;
            }

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });

        // GET /api/session - get active session details
        server.on("/api/session", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            Session active;
            JsonDocument doc;
            if (Sessions::getActive(active)) {
                doc["id"]                 = active.id;
                doc["subject"]            = active.subject;
                doc["teacher"]            = active.teacher;
                doc["dept"]               = active.dept;
                doc["sem"]                = active.sem;
                doc["section"]            = active.section;
                doc["room"]               = active.room;
                doc["notes"]              = active.notes;
                doc["date"]               = active.date;
                doc["startTime"]          = active.startTime;
                doc["startEpoch"]         = active.startEpoch;
                doc["presentsCount"]      = active.presentsCount;
                doc["status"]             = "Active";
            } else {
                doc["id"]                 = "";
                doc["subject"]            = RfidReader::activeSubject;
                doc["teacher"]            = "";
                doc["dept"]               = "";
                doc["sem"]                = "";
                doc["section"]            = "";
                doc["room"]               = "";
                doc["notes"]              = "";
                doc["date"]               = Utils::getFormattedDate();
                doc["startTime"]          = "";
                doc["startEpoch"]         = 0;
                doc["presentsCount"]      = 0;
                doc["status"]             = "None";
            }

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });

        // GET /api/sessions - list all sessions (quick, scheduled, history)
        server.on("/api/sessions", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            const std::vector<Session>& list = Sessions::getAll();

            JsonDocument doc;
            JsonArray arr = doc.to<JsonArray>();

            for (const auto& s : list) {
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
            }

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });

        // POST /api/session/start - start Quick Session or Schedule Session
        // Kept separate from /api/session/end so route matching can never
        // create an unintended General session when ending a class.
        server.on("/api/session/start", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            if (deserializeJson(doc, bodyStr)) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            Session s;
            s.subject            = doc["subject"] | "General";
            s.teacher            = doc["teacher"] | "";
            s.dept               = doc["dept"] | "";
            s.sem                = doc["sem"] | "";
            s.section            = doc["section"] | doc["sec"] | "";
            s.room               = doc["room"] | "";
            s.notes              = doc["notes"] | "";
            s.scheduledDate      = doc["scheduledDate"] | "";
            s.scheduledStartTime = doc["scheduledStartTime"] | "";
            s.scheduledEndTime   = doc["scheduledEndTime"] | "";

            String reqId = doc["id"] | "";
            Session resultSession;
            bool success = false;

            if (reqId.length() > 0) {
                success = Sessions::startScheduled(reqId, resultSession);
            } else if (s.scheduledDate.length() > 0 && s.scheduledStartTime.length() > 0) {
                success = Sessions::schedule(s, resultSession);
            } else {
                success = Sessions::startQuick(s, resultSession);
            }

            if (success) {
                JsonDocument resDoc;
                resDoc["status"]         = "success";
                resDoc["id"]             = resultSession.id;
                resDoc["subject"]        = resultSession.subject;
                resDoc["startTime"]      = resultSession.startTime;
                resDoc["sessionStatus"]  = resultSession.status;
                String resStr;
                serializeJson(resDoc, resStr);
                request->send(200, "application/json", resStr);
            } else {
                request->send(500, "application/json", "{\"error\":\"Failed to save session to SD Card\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // POST /api/session/end - end active session and return summary
        server.on("/api/session/end", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            Session ended;
            if (Sessions::endActive(ended)) {
                JsonDocument doc;
                doc["status"]        = "success";
                doc["id"]            = ended.id;
                doc["subject"]       = ended.subject;
                doc["startTime"]     = ended.startTime;
                doc["endTime"]       = ended.endTime;
                doc["duration"]      = ended.duration;
                doc["presentsCount"] = ended.presentsCount;
                doc["dept"]          = ended.dept;
                doc["sem"]           = ended.sem;
                doc["section"]       = ended.section;

                String resStr;
                serializeJson(doc, resStr);
                request->send(200, "application/json", resStr);
            } else {
                request->send(400, "application/json", "{\"error\":\"No active session to end\"}");
            }
        });

        // POST /api/session/delete - delete a session by ID
        server.on("/api/session/delete", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            if (deserializeJson(doc, bodyStr)) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String id = doc["id"] | "";
            if (id.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Missing session id\"}");
                return;
            }

            if (Sessions::deleteById(id)) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(400, "application/json", "{\"error\":\"Failed to delete session. Active sessions cannot be deleted.\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // POST /api/session/clear-history - remove ended sessions only
        server.on("/api/session/clear-history", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (Sessions::clearHistory()) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(500, "application/json", "{\"error\":\"Failed to clear session history\"}");
            }
        });

        // POST /api/session/update - update a scheduled session by ID
        server.on("/api/session/update", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            if (deserializeJson(doc, bodyStr)) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String id = doc["id"] | "";
            if (id.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Missing session id\"}");
                return;
            }

            Session updated;
            updated.subject            = doc["subject"] | "";
            updated.teacher            = doc["teacher"] | "";
            updated.dept               = doc["dept"] | "";
            updated.sem                = doc["sem"] | "";
            updated.section            = doc["section"] | "";
            updated.room               = doc["room"] | "";
            updated.notes              = doc["notes"] | "";
            updated.scheduledDate      = doc["scheduledDate"] | "";
            updated.scheduledStartTime = doc["scheduledStartTime"] | "";
            updated.scheduledEndTime   = doc["scheduledEndTime"] | "";

            if (updated.subject.length() == 0 || updated.scheduledDate.length() == 0 || updated.scheduledStartTime.length() == 0) {
                request->send(400, "application/json", "{\"error\":\"Subject, scheduled date, and start time are required\"}");
                return;
            }
            if (updated.scheduledEndTime.length() > 0 && updated.scheduledEndTime < updated.scheduledStartTime) {
                request->send(400, "application/json", "{\"error\":\"End time cannot be earlier than start time\"}");
                return;
            }

            if (Sessions::updateById(id, updated)) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(400, "application/json", "{\"error\":\"Failed to update. Only scheduled sessions can be edited.\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // GET /api/reports/daily
        server.on("/api/reports/daily", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            String date = "";
            if (request->hasParam("date")) {
                date = request->getParam("date")->value();
            } else {
                date = Utils::getFormattedDate();
            }

            String json = Attendance::getDailyReportJson(date);
            request->send(200, "application/json", json);
        });

        // GET /api/reports/monthly
        server.on("/api/reports/monthly", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            String month = "";
            if (request->hasParam("month")) {
                month = request->getParam("month")->value();
            } else {
                month = Utils::getFormattedDate().substring(0, 7);
            }

            String json = Attendance::getMonthlyReportJson(month);
            request->send(200, "application/json", json);
        });

        // GET /api/settings
        server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            JsonDocument doc;
            doc["deviceName"] = Config::deviceName;
            doc["cooldownS"] = Config::cooldownS;
            doc["adminUser"] = Config::adminUser;
            doc["wifiSSID"] = Config::wifiSSID;
            doc["adminPass"] = "";
            doc["wifiPass"] = "";

            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        });

        // POST /api/settings
        server.on("/api/settings", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, bodyStr);
            if (err) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            if (doc.containsKey("deviceName")) Config::deviceName = doc["deviceName"].as<String>();
            if (doc.containsKey("cooldownS")) Config::cooldownS = doc["cooldownS"].as<uint32_t>();
            if (doc.containsKey("adminUser")) Config::adminUser = doc["adminUser"].as<String>();
            if (doc.containsKey("adminPass")) {
                String newPass = doc["adminPass"].as<String>();
                if (newPass.length() > 0) {
                    Config::adminPass = newPass;
                }
            }
            if (doc.containsKey("wifiSSID")) Config::wifiSSID = doc["wifiSSID"].as<String>();
            if (doc.containsKey("wifiPass")) Config::wifiPass = doc["wifiPass"].as<String>();

            if (Config::save()) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(500, "application/json", "{\"error\":\"Failed to save settings to SD Card\"}");
            }
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // POST /api/time
        server.on("/api/time", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            if (!request->_tempObject) {
                request->send(400, "application/json", "{\"error\":\"Empty body\"}");
                return;
            }

            String bodyStr = (char*)request->_tempObject;
            free(request->_tempObject);
            request->_tempObject = NULL;

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, bodyStr);
            if (err || !doc.containsKey("epoch")) {
                request->send(400, "application/json", "{\"error\":\"Invalid epoch payload\"}");
                return;
            }

            uint32_t epoch = doc["epoch"].as<uint32_t>();
            Utils::setSystemTime(epoch);
            request->send(200, "application/json", "{\"status\":\"success\"}");
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            bufferBody(request, data, len, index, total);
        });

        // POST /api/backup
        server.on("/api/backup", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            bool success1 = Storage::backupFile(Storage::CONFIG_PATH);
            bool success2 = Storage::backupFile(Storage::STUDENTS_PATH);

            if (success1 && success2) {
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                request->send(500, "application/json", "{\"error\":\"Backup failed\"}");
            }
        });

        // POST /api/restart
        server.on("/api/restart", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }

            request->send(200, "application/json", "{\"status\":\"restarting\"}");
            
            // Defer restart using a FreeRTOS task
            xTaskCreate([](void*){
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                ESP.restart();
            }, "restart_task", 2048, NULL, 1, NULL);
        });

        // GET /attendance.csv - Stream CSV file directly from SD Card
        server.on("/attendance.csv", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!isAuthorized(request)) {
                request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
                return;
            }
            
            if (SD.exists(Storage::ATTENDANCE_PATH)) {
                request->send(SD, Storage::ATTENDANCE_PATH, "text/csv", true); // true = Attachment download
            } else {
                request->send(404, "text/plain", "Attendance file not found on SD card");
            }
        });

        // Register custom wildcard handlers
        server.addHandler(new StudentsPutHandler());
        server.addHandler(new StudentsDeleteHandler());

        // Catch-all (404 & OPTIONS)
        server.onNotFound([](AsyncWebServerRequest *request){
            if (request->method() == HTTP_OPTIONS) {
                request->send(204);
                return;
            }
            // If body chunk was allocated but not used, free it
            if (request->_tempObject) {
                free(request->_tempObject);
                request->_tempObject = NULL;
            }
            request->send(404, "text/plain", "Not Found");
        });

        server.begin();
        Serial.println("[WebServer] Async Web Server started on port 80.");
    }

    void stop() {
        server.end();
        Serial.println("[WebServer] Async Web Server stopped.");
    }
}
