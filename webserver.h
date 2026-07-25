#ifndef ESP_ATTENDENCE_WEBSERVER_H
#define ESP_ATTENDENCE_WEBSERVER_H

#include <Arduino.h>

namespace WebServerManager {
    // Initialize the HTTP web server and API routes
    void init();

    // Stop the web server
    void stop();
}

#endif // WEBSERVER_H
