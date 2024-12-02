#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C oled(U8G2_R0);
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0);

// The x position of the square on screen
int pos = 0;

void setup() 
{
    // Start the display
    oled.begin();
}

void loop() 
{
    // calculate next position (between 0 and 108)
    pos = (pos + 1) % (128 - 20);

    // Clear buffer (NOT THE DISPLAY)
    oled.clearBuffer();
    // Draw 20 wide square
    oled.drawBox(pos, 6, 20, 20);
    // Send buffer to display
    oled.sendBuffer();

    // Optional delay (1ms)
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
