#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R0, GPIO_NUM_2, GPIO_NUM_4, GPIO_NUM_5);

// The x position of the square on screen
int pos = 0;

void setup()
{
    oled.begin();
    oled.clearDisplay();
}

void loop()
{
    pos = (pos + 1) % 108;
    oled.clearBuffer();
    oled.drawBox(pos, 20, 20, 20);
    oled.sendBuffer();
    
    // Optional delay (10ms)
    vTaskDelay(10 / portTICK_PERIOD_MS);
}