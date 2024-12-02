#include "wifi.h"
#include <WiFi.h>

#include "sdcard.h"

#define WIFI_TIMEOUT_MS 5000

bool tWiFi::connect(const char* ssid, const char* pass)
{
    WiFi.mode(WIFI_STA); // set mode to station
    WiFi.begin(ssid, pass); // connect to the given network

    uint16_t timeout = 0;
    while (WiFi.status() != WL_CONNECTED) // while not connected
    {
        if (timeout > WIFI_TIMEOUT_MS) // check timeout
        {
            WiFi.disconnect(true); // disconnect
            Serial.println("WiFi: Connection failed");
            return false; // failed to connect
        }
        timeout++; // increase timeout counter
        vTaskDelay(1 / portTICK_PERIOD_MS); // wait 1ms
    }

    m_ssid = ssid; // save ssid
    Serial.println("WiFi: Connected to " + m_ssid + " (" + WiFi.localIP().toString() + ")");
    return true; // connected successfully
}

// Splits a string into two on the first appearence of the character in c. str1 = str, str2 = return
char* str_split(char* str, char c)
{
    uint16_t i = 0;
    char* out = NULL;
    while (str[i] != '\0') // until the string ends
    {
        if (str[i] == c) // if the curr char is c
        {
            if (out == NULL) // if out is still null
                out = (str + i + 1); // save the second string pointer
            str[i] = '\0'; // replace c with null char
        }
        i++; // increase index
    }
    return out != NULL ? out : str; // return pointer to second string
}

bool tWiFi::connect(tSDCard* sd)
{
    // 97 = 32 max ssid + new line char + 63 max password + '\0'
    char wtxt[97];
    // load wifi.txt
    uint16_t wtxtlen = sd->readAllText(wtxt, 97, "/wifi.txt");
    // min ssid = 0, new line = 1, min pass = 8
    if (wtxtlen < 9)
    {
        Serial.println("WiFi: non-existent or invalid wifi.txt");
        return false;
    }

    // Separate ssid from password
    char* ssid = wtxt;
    char* pass = str_split(wtxt, '\n');

    // Pass will be the same as ssid if there is only one line of text
    if (ssid == pass)
    {
        Serial.println("WiFi: invalid wifi.txt");
        return false;
    }

    // Connect
    return connect(ssid, pass);
}

bool tWiFi::connected() { return WiFi.status() == WL_CONNECTED; }

void tWiFi::disconnect() { WiFi.disconnect(true); }

IPAddress tWiFi::getIP() { return WiFi.localIP(); }

String tWiFi::getSSID() { return m_ssid; }

