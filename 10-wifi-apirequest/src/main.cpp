#define OPENWEATHERMAP_API_KEY "inputyourapikeyhere"
#define OPENWEATHERMAP_CURRENT_WEATHER "https://api.openweathermap.org/data/2.5/weather?lat=%lf&lon=%lf&appid=%s"

#include <Arduino.h>
#include <stdio.h>

#include <WiFi.h>
#include <HTTPClient.h>

const char* wifi_ssid = "";
const char* wifi_pass = "";

void initWiFi()
{
    WiFi.begin(wifi_ssid, wifi_pass);
    Serial.println("Connecting...");
    while (WiFi.status()!= WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected with address: ");
    Serial.println(WiFi.localIP());
}

String getCurrentWeather(double lat, double lon)
{
    char url[150];
    sprintf(url, OPENWEATHERMAP_CURRENT_WEATHER, lat, lon, OPENWEATHERMAP_API_KEY);
    if (WiFi.status() != WL_CONNECTED)
        return "{}";
    HTTPClient http;
    http.begin(url);
    int resp = http.GET();
    if (resp <= 0)
        return "{}";
    String data = http.getString();
    http.end();
    return data;
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    initWiFi(); // connect to wifi

    String data = getCurrentWeather(51.5072, 0.1276); // London
    Serial.println(data);
}

void loop() { }