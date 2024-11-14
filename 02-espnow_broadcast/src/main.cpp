#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

typedef struct 
{
    int a;
    float b;
    char c;
} mymsg;

// FF:FF:FF:FF:FF:FF is the broadcast address
uint8_t broadcastAddr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
esp_now_peer_info_t broadcastPeer;

void OnDataRecv(const uint8_t* mac, const uint8_t* data, int len)
{
    // Check message length
    if (len != sizeof(mymsg))
    {
        Serial.println("Received invalid msg");
        return; // return if invalid
    }

    // Get mac string from uint array
    char macstr[32];
    snprintf(macstr, 31, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Cast data to mymsg* and print it's values
    mymsg* msg = (mymsg*)data;
    Serial.printf("[%s] Received: %d, %f, %c\n", macstr, msg->a, msg->b, msg->c);
}

void OnDataSent(const uint8_t* mac, esp_now_send_status_t stat)
{
    // Get mac string from uint array (destination)
    char macstr[32];
    snprintf(macstr, 31, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Log status
    Serial.printf("[%s] Last Packet Sent: %s\n", macstr, stat == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() 
{
    // init serial
    Serial.begin(115200);

    // init wifi
    WiFi.mode(WIFI_STA);

    // init espnow
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Can't start espnow, rebooting");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP.restart();
    }

    // populate peer info
    broadcastPeer.channel = 0; // 0 = any
    broadcastPeer.encrypt = false; // can't encrypt when broadcasting
    memcpy(broadcastPeer.peer_addr, broadcastAddr, 6); // copy broadcast address

    // register peer
    if (esp_now_add_peer(&broadcastPeer) != ESP_OK)
    {
        Serial.println("Can't register espnow broadcast peer, rebooting");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP.restart();
    }

    // Register callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() 
{
    // wait 5secs
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    // populate message with random number
    mymsg msg;
    msg.a = random(10, 100);
    msg.b = msg.a / 10.0;
    msg.c = (msg.a % 26) + 'a';
    
    // send message
    esp_now_send(broadcastAddr, (uint8_t*)&msg, sizeof(msg));
}
