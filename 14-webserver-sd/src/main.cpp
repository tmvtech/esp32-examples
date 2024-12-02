#include <Arduino.h>
#include "sdcard.h"
#include "wifi.h"
#include "web.h"

tSDCard sd(GPIO_NUM_4);
tWiFi wifi;
tWeb web(&sd);

void setup() 
{
    Serial.begin(115200);
    // Print the contents of the web root dir
    sd.printFolderContents(WEB_ROOT);
    // Try to connect to WiFi
    while (!wifi.connect(&sd))
        vTaskDelay(5000 / portTICK_PERIOD_MS);

    // Init the web server
    web.init();
    Serial.println("Setup: Complete");
}

void loop() 
{
    // Check if wifi is connected
    if (!wifi.connected())
    {
        Serial.println("WiFi: Trying to reconnect");
        wifi.connect(&sd); // connect
    }
    // Delay
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}