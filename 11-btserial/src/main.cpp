#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() 
{
    SerialBT.begin("TMV Esp32");
}

void loop() 
{
    if (SerialBT.available())
        SerialBT.write(SerialBT.read());
}