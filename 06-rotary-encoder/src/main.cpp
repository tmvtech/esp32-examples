#include <Arduino.h>
// The encoder itself
#include <AiEsp32RotaryEncoder.h>
// Helper, great for making menus and sliders
#include <AiEsp32RotaryEncoderNumberSelector.h>

// If you want the encoder to increase in the 
// oposite direction, switch pin A with pin B
// Clock or Data pin
#define ROTARY_ENCODER_A_PIN 19
// Clock or Data pin
#define ROTARY_ENCODER_B_PIN 18
// Change to the one that fits your encoder the 
// best, recommended values are 1, 2 and 4
#define ROTARY_ENCODER_STEPS 2

AiEsp32RotaryEncoder* enc = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, 
                     ROTARY_ENCODER_B_PIN, -1, -1, ROTARY_ENCODER_STEPS);
AiEsp32RotaryEncoderNumberSelector nenc = AiEsp32RotaryEncoderNumberSelector();

void IRAM_ATTR readEncoderISR() { enc->readEncoder_ISR(); }

void setup() 
{
    // Open serial and wait 2.5 seconds
    Serial.begin(115200);
    vTaskDelay(2500 / portTICK_PERIOD_MS);
    
    // Start encoder and set interrupt callback
    enc->begin();
    enc->setup(readEncoderISR);
    // Setup Number Selector
    nenc.attachEncoder(enc);
    // Set min, max, increment and whether it should loop
    nenc.setRange(0, 1, 0.01, true);
    // Set initial value to 0
    nenc.setValue(0);
}

void loop() 
{
    if (enc->encoderChanged())
        Serial.printf("%.2f\n", nenc.getValue());
}