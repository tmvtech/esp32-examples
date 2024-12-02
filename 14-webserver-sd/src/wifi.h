#ifndef _wifi_h_
#define _wifi_h_

#include <Arduino.h>
#include "sdcard.h"

/* @brief Class that manages the connection to WiFi
 * @todo Add Task to reconnect */
class tWiFi
{
    private:
    // Network name
    String m_ssid;

    public:
    /* @brief Creates a new tWiFi */
    tWiFi() : m_ssid("") {}

    /* @brief Connect to a network using the given credentials
     * @param ssid Network name
     * @param pass Network password
     * @returns Success */
    bool connect(const char* ssid, const char* pass);
    /* @brief Connect to a network using the credentials stored in 'wifi.txt'
     * @param sd SD Card handler
     * @returns Success */
    bool connect(tSDCard* sd);
    /* @brief Disconnect and turn off wifi */
    void disconnect();
    /* @brief Check if connected to a network
     * @returns True if connected */
    bool connected();

    /* @brief Get local ip
     * @returns Local IP address */
    IPAddress getIP();
    /* @brief Get ssid 
     * @returns Network name */
    String getSSID();
};

#endif
