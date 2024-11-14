#include <Arduino.h>

void setup() 
{
    // PULLDOWN is low when not clicked
    pinMode(GPIO_NUM_22, INPUT_PULLDOWN);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
    // Read state of the button
    int state = digitalRead(GPIO_NUM_22);
    // Change state of the led
    digitalWrite(LED_BUILTIN, state);
    // Sleep 100ms (loop runs at 10Hz)
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

/*

// toggle

int lastbtnstate = LOW;
int ledstate = LOW;

void loop() 
{
    // Read state of the button
    int btnstate = digitalRead(GPIO_NUM_22);
    // When button is clicked
    if (lastbtnstate != btnstate && btnstate == HIGH)
    {
        // Change state of the led
        ledstate = !ledstate;
        digitalWrite(LED_BUILTIN, ledstate);
    }
    // Save last button state
    lastbtnstate = btnstate;
    // Sleep 10ms (loop runs at 100Hz)
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

*/