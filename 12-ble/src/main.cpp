#include <Arduino.h>
#include "blesv.h"
// #include "blecl.h"

void setup() 
{
    Serial.begin(115200);
    blesv_init("TMVEsp32");
    // blecl_init();
}

void loop() 
{
    blesv_tick();
    // blecl_tick();
}
