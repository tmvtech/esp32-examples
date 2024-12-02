#include <Arduino.h>

// Ledc channel (0 - 15)
#define LEDC_CHANNEL_0 0
// Accuracy of the timer (1 - 16)
#define LEDC_TIMER_13_BIT 13
// Buzzer pin
#define BUZZER_PIN GPIO_NUM_4

// Melody Note, contains the frequency and the duration
struct MelodyNote
{
    int freq;
    int duration;
};

// Melody
const struct MelodyNote Melody[] = PROGMEM {  
    {330, 8},{330, 4},{330, 4},{262, 8},{330, 4},{392, 2},{196, 2},{262, 3},
    {196, 3},{165, 3},{220, 4},{247, 4},{233, 8},{220, 4},{196, 8},{330, 8},
    {392, 8},{440, 4},{349, 8},{392, 4},{330, 3},{262, 8},{294, 8},{247, 3},
    {262, 3},{196, 3},{165, 3},{220, 4},{247, 4},{233, 8},{220, 4},{196, 8},
    {330, 8},{392, 8},{440, 4},{349, 8},{392, 4},{330, 3},{262, 8},{294, 8},
    {247, 2},{392, 8},{370, 8},{330, 8},{311, 4},{330, 4},{208, 8},{220, 8},
    {262, 4},{220, 8},{262, 8},{294, 3},{392, 8},{370, 8},{330, 8},{311, 4},
    {330, 4},{523, 4},{523, 8},{523, 2},{392, 8},{370, 8},{330, 8},{311, 4},
    {330, 4},{208, 8},{220, 8},{262, 4},{220, 8},{262, 8},{294, 3},{311, 3},
    {294, 3},{262, 1},{262, 8},{262, 4},{262, 4},{262, 8},{294, 4},{330, 8},
    {262, 4},{220, 8},{196, 2},{262, 8},{262, 4},{262, 4},{262, 8},{294, 4},
    {330, 1},{262, 8},{262, 4},{262, 4},{262, 8},{294, 4},{330, 8},{262, 4},
    {220, 8},{196, 2}
};
// Melody Size (array size in bytes divided by struct size in bytes)
#define MelodySize sizeof(Melody) / sizeof(MelodyNote)

// Task that will play the melody
void playMelodyTask(void* params) // added (void* params)
{
    // Variable that will hold the real note duration
    int noteDuration; 
    // Play melody
    for (int i = 0; i < MelodySize; i++)
    {
        // Calculate duration
        noteDuration = 800 / Melody[i].duration;
        // Setup note
        ledcSetup(LEDC_CHANNEL_0, Melody[i].freq * 2, LEDC_TIMER_13_BIT);
        // Set pin
        ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL_0);
        // Play note
        ledcWrite(LEDC_CHANNEL_0, 50);
        // Delay
        vTaskDelay((noteDuration * 1.30) / portTICK_PERIOD_MS); // changed delay to vTaskDelay
    }
    ledcDetachPin(BUZZER_PIN);
    // End task, NEEDS TO BE HERE, WILL CAUSE CRASHES IF MISSING!
    vTaskDelete(NULL); 
}

void setup()
{
    pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin to output mode
    xTaskCreate(playMelodyTask, "Melody Task", 1024, NULL, 5, NULL);
}

// LED Blink to test multithreading
void loop()
{
    digitalWrite(GPIO_NUM_2, HIGH);
    vTaskDelay(250 / portTICK_PERIOD_MS);
    digitalWrite(GPIO_NUM_2, LOW);
    vTaskDelay(250 / portTICK_PERIOD_MS);
}
