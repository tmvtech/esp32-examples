#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, GPIO_NUM_2, GPIO_NUM_4);
    Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
}

void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();
        Serial1.print(c);
        Serial2.print(c);
    }

    if (Serial1.available())
    {
        char c = Serial1.read();
        Serial.print(c);
        Serial2.print(c);
    }

    if (Serial2.available())
    {
        char c = Serial2.read();
        Serial.print(c);
        Serial1.print(c);
    }
}