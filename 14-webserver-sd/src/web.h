#ifndef _web_h_
#define _web_h_

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "sdcard.h"
#include "wifi.h"

// Root folder for the webserver assets
#define WEB_ROOT "/www"

/* @brief Class that manages the Web Server */
class tWeb
{
    private:
    // Webserver
    AsyncWebServer m_server;
    // SD card handle
    tSDCard* m_sd;

    /* @brief Sends a reply with code 500 and a plain text message
     * @param req The request
     * @param msg The message to send */
    void sendError(AsyncWebServerRequest* req, String msg);

    /* @brief Load an send a file
     * @param req The request
     * @param msg The message to send
     * @param code The HTTP code the reply will have
     * @returns Success */
    bool handleFile(AsyncWebServerRequest* req, String path, int code = 200);
    void handleReq(AsyncWebServerRequest* req);
    /* @brief Sends the 404 page
     * @param req The request */
    void handle404(AsyncWebServerRequest* req);
    /* @brief Sends the favicon
     * @param req The request */
    void handleFavicon(AsyncWebServerRequest* req);

    public:
    /* @brief Creates a tWeb class
     * @param sd The SD card handle */
    tWeb(tSDCard* sd) : m_server(80), m_sd(sd) {}
    /* @brief Initializes and starts the server */
    void init();
};

#endif
