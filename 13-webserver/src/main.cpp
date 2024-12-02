#include <Arduino.h>
#include <WiFi.h>

// WiFi network name
const char *wifi_ssid = "";
// WiFi network password
const char *wifi_pass = "";

// Http webserver on port 80
WiFiServer sv(80);

// Led pin (onboard is D2)
#define LED_PIN GPIO_NUM_2
// Led state (1 on or 0 off)
uint8_t led_state = 1;

// Send HTML page
void sendHtml(WiFiClient cli)
{
    cli.println(
        "HTTP/1.1 200 OK\n"
        "Content-Type: text/html\n"
        "Connection: close\n"
        "\n"
        "<!DOCTYPE HTML>\n"
        "<html>\n"
        "    <head>\n"
        "        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"\n"
        "        <link rel=\"icon\" href=\"data:,\">\n"
        "        <style>\n"
        "            html { font-family: Roboto; display: inline-block; margin: 0px auto; text-align: center;}\n"
        "            .button {background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block;\n"
        "                      font-size: 16px;margin: 4px 2px; cursor: pointer;text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}\n"
        "            .button_ON {background-color: white; color: black; border: 2px solid #4CAF50;}\n"
        "            .button_OFF {background-color: white; color: black; border: 2px solid #f44336;}\n"
        "        </style>\n"
        "    </head>\n"
        "    <body>\n"
        "        <h2>Esp32 LED</h2>\n"
        "        <p>Click to toggle LED</p>"
    );
    cli.println(led_state ? // Only send relevant button
        "<p><a href=\"/off\n\"><button class=\"button button_ON\">ON</button></a></p>"
        :
        "<p><a href=\"/on\n\"><button class=\"button button_OFF\">OFF</button></a></p>"
    );
    cli.println(
        "    </body>\n"
        "</html>"
    );
}


// Initialize and start the Server
void initWiFiServer()
{
    // Set as Wifi Station (client, needs an access point to connect to)
    WiFi.mode(WIFI_STA);
    // Connect to your WiFi network
    WiFi.begin(wifi_ssid, wifi_pass);
    // Wait for connection (Be carefull as the Esp can be stuck here forever)
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    // Print local IP
    Serial.println();
    Serial.println(WiFi.localIP());
    // Start server
    sv.begin();
}

// Runs once at power on
void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    Serial.println("Hello");

    // Set led pin as output and turn it on
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, led_state);
    // Connect to WiFi and init server
    initWiFiServer();
}

// String to store the received data
String request = "";
// Loop forever
void loop()
{
    // Accept connections
    WiFiClient cli = sv.available();

    // Return if client is invalid
    if (!cli)
        return;

    // Repeat while client is connected
    while (cli.connected())
    {   // if client is available
        if (cli.available())
        {
            // Read and store char received
            char c = cli.read();
            request += c;

            // When finished receiving header
            if (c == '\n')
            {
                // Handle path /off
                if (request.indexOf("GET /off") != -1)
                {
                    Serial.println("GET /off");
                    led_state = 0; // turn led off
                }
                // Handle path /on
                else if (request.indexOf("GET /on") != -1)
                {
                    Serial.println("GET /on");
                    led_state = 1; // turn led on
                }
                else // invalid path or no path
                    Serial.println("GET /");
                // Switch led state
                digitalWrite(LED_PIN, led_state);
                // send html page
                sendHtml(cli);
                // exit loop
                break;
            }
        }
    }
    // small delay to avoid problems
    delay(5);
    // clear receive "buffer"
    request = "";
    // close client connection
    cli.stop();
}
