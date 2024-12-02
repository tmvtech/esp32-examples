#include <Arduino.h>

// keyboard characters
uint8_t btnChar[4][4] = 
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

// current keyboard state
uint8_t btnState[4][4] = {{0}};
// keyboard state in last update
uint8_t btnLastState[4][4] = {{0}};

// initialize the keyboard pins
void keyobard_init()
{
    // Row
    pinMode(GPIO_NUM_12, OUTPUT);
    pinMode(GPIO_NUM_13, OUTPUT);
    pinMode(GPIO_NUM_14, OUTPUT);
    pinMode(GPIO_NUM_15, OUTPUT);
    // Column
    pinMode(GPIO_NUM_16, INPUT_PULLUP);
    pinMode(GPIO_NUM_17, INPUT_PULLUP);
    pinMode(GPIO_NUM_18, INPUT_PULLUP);
    pinMode(GPIO_NUM_19, INPUT_PULLUP);
}

// read the current keyboard state and store the previous one
void keyboard_update()
{
    for (uint8_t y = 0; y < 4; y++)
    {
        // start row check
        digitalWrite(GPIO_NUM_12 + y, LOW);
        for (uint8_t x = 0; x < 4; x++)
        {
            btnLastState[y][x] = btnState[y][x];
            btnState[y][x] = !digitalRead(GPIO_NUM_16 + x);
        }
        digitalWrite(GPIO_NUM_12 + y, HIGH);
    }
}

// setup
void setup()
{
    Serial.begin(115200);

    keyobard_init();
}

// loop
void loop()
{
    keyboard_update();
    
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (btnState[x][y] && !btnLastState[x][y])
                Serial.printf("%c", btnChar[x][y]);
}
