#include "blecl.h"
#include "common.h"

#include <Arduino.h>
#include <BLEDevice.h>

static uint8_t blecl_hexchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
String blecl_byteToHex(uint8_t val)
{
    // split into 2 x 4 bits
    uint8_t a = (val & (0xF0)) >> 4;
    uint8_t b =  val & (0x0F);
    
    // Get characters and concat them together
    String ret = String((char)blecl_hexchars[a]);
    ret.concat((char)blecl_hexchars[b]);
    return ret;
}
String blecl_int32ToHex(uint32_t val)
{
    // Split into 4 x byte
    uint8_t a = (val & 0xFF000000) >> 24;
    uint8_t b = (val & 0x00FF0000) >> 16;
    uint8_t c = (val & 0x0000FF00) >> 8;
    uint8_t d = (val & 0x000000FF);
    
    // Get each byte to Hex and concat
    String ret = blecl_byteToHex(d);
    ret.concat('-');
    ret.concat(blecl_byteToHex(c));
    ret.concat('-');
    ret.concat(blecl_byteToHex(b));
    ret.concat('-');
    ret.concat(blecl_byteToHex(a));
    return ret;
}

static bool blecl_doScan = false;
static bool blecl_doConnect = false;
static bool blecl_connected = false;
static BLERemoteCharacteristic* blecl_remoteReadChar;
static BLERemoteCharacteristic* blecl_remoteWriteChar;
static BLEAdvertisedDevice* blecl_device;

static void blecl_onnotify(BLERemoteCharacteristic* remoteChar, uint8_t* data, size_t len, bool isNotify)
{
    Serial.print("Notify callback for characteristic ");
    Serial.print(remoteChar->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(len);
    Serial.print("data: ");
    if (len >= 4)
        Serial.println(blecl_int32ToHex(*((uint32_t*)data)));
    else 
        Serial.println((char*)data);
}

class BLE_cliCallbacks : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient) {}

    void onDisconnect(BLEClient *pclient)
    {
        blecl_connected = false;
    }
};
static BLE_cliCallbacks blecl_clicallbacks;

bool blecl_connect()
{
    BLEClient *client = BLEDevice::createClient();

    client->setClientCallbacks(&blecl_clicallbacks);
    client->connect(blecl_device);

    // Check if service exists
    BLERemoteService *service = service = client->getService(BLE_SERVICE_UUID);
    if (!service)
    {
        client->disconnect();
        return false;
    }
    
    // Check if characteristics exit 
    blecl_remoteReadChar = service->getCharacteristic(BLE_READ_CHARACTERISTIC_UUID);
    blecl_remoteWriteChar = service->getCharacteristic(BLE_WRITE_CHARACTERISTIC_UUID);
    if (!blecl_remoteReadChar || !blecl_remoteWriteChar)
    {
        client->disconnect();
        return false;
    }

    // Try read readChar 
    if (blecl_remoteReadChar->canRead())
    {
        uint32_t val = blecl_remoteReadChar->readUInt32();

        String log = String("Read Charact value: ");
        log.concat(String(val));
        Serial.println(log);
    }
    
    // Try to register for notify 
    if (blecl_remoteReadChar->canNotify())
    {
        blecl_remoteReadChar->registerForNotify(blecl_onnotify);
    }

    // Set as connected
    blecl_connected = true;
    return true;
}

class BLE_advCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertDevice) override
    {
        Serial.println(String("[BLE] Device found: ").concat(advertDevice.toString().c_str()));

        // If has our service, it is the right device
        if (advertDevice.haveServiceUUID() && advertDevice.isAdvertisingService(BLE_SERVICE_UUID))
        {
            // Stop scan and save device to connect to 
            BLEDevice::getScan()->stop();
            blecl_device = new BLEAdvertisedDevice(advertDevice);
            blecl_doConnect = true;
            blecl_doScan = true;
        }
    }
};
static BLE_advCallbacks blecl_advcallbacks;

void blecl_init()
{
    BLEDevice::init("");
    BLEScan *scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(&blecl_advcallbacks);
    scan->setWindow(10);
    scan->setActiveScan(true);
    scan->start(5, false);
}

void blecl_tick()
{
    if (blecl_doConnect)
    {
        if (blecl_connect())
            Serial.println("[BLE] Connected");
        else
            Serial.println("[BLE] Failed to connect");
        blecl_doConnect = false;
    }

    if (blecl_connected)
    {
        if (blecl_remoteWriteChar->canWrite())
        {
            String v = "Time since boot: " + String(millis() / 1000);
            blecl_remoteWriteChar->writeValue(v.c_str(), v.length());
        }
    }
    else
    {
        BLEDevice::getScan()->start(5);
    }

    delay(1000);
}
