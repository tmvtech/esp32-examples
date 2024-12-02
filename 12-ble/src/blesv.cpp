#include "blesv.h"
#include "common.h"

#include <Arduino.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEService.h>

class BLE_ch_callbacks: public BLECharacteristicCallbacks
{
    /**
	 * @brief Callback function to support a read request.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 * @param [in] param The BLE GATTS param. Use param->read.
	 */
    virtual void onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) override
    {
        String msg = "Value of ";
        msg.concat(pCharacteristic->getUUID().toString().c_str());
        msg.concat(" read");
        Serial.println(msg);
    }
	
    /**
	 * @brief Callback function to support a write request.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 * @param [in] param The BLE GATTS param. Use param->write.
	 */
	virtual void onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) override
    {
        String msg = "Value of ";
        msg.concat(pCharacteristic->getUUID().toString().c_str());
        msg.concat(" written: ");
        msg.concat(pCharacteristic->getValue().c_str());
        Serial.println(msg);
    }
	
    /**
	 * @brief Callback function to support a Notify request.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 */
	virtual void onNotify(BLECharacteristic* pCharacteristic) override
    {
        String msg = "Notify from ";
        msg.concat(pCharacteristic->getUUID().toString().c_str());
        Serial.println(msg);
    }

	/**
	 * @brief Callback function to support a Notify/Indicate Status report.
	 * @param [in] pCharacteristic The characteristic that is the source of the event.
	 * @param [in] s Status of the notification/indication
	 * @param [in] code Additional code of underlying errors
	 */
	virtual void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) override
    {
        String msg = "Status from ";
        msg.concat(pCharacteristic->getUUID().toString().c_str());
        msg.concat(": ");
        msg.concat(std::to_string(s).c_str());
        Serial.println(msg);
    }
};

class BLE_sv_callbacks: public BLEServerCallbacks
{
    virtual void onConnect(BLEServer* pServer) override
    {
        BLEDevice::startAdvertising();
    }

    virtual void onDisconnect(BLEServer* pServer) override
    {
        BLEDevice::startAdvertising();
    }
};

BLEServer* ble_server;
BLEService* ble_service;
BLEDescriptor read_charect_descriptor("2902", 100);
BLECharacteristic* read_charect;
BLECharacteristic* write_charect;
BLE_ch_callbacks ble_ch_callbacks;
BLE_sv_callbacks ble_sv_callbacks;

void blesv_init(std::__cxx11::string device_name)
{
    // Create server and service
    BLEDevice::init(device_name);
    ble_server = BLEDevice::createServer();
    ble_server->setCallbacks(&ble_sv_callbacks);
    ble_service = ble_server->createService(BLE_SERVICE);

    // Setup read characteristic
    read_charect = ble_service->createCharacteristic(BLE_READ_CHARACTERISTIC, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    read_charect_descriptor.setValue("Notifications enabled");
    read_charect->addDescriptor(&read_charect_descriptor);
    read_charect->setCallbacks(&ble_ch_callbacks);
    read_charect->setValue(0);

    // Setup write charect
    write_charect = ble_service->createCharacteristic(BLE_WRITE_CHARACTERISTIC, BLECharacteristic::PROPERTY_WRITE);
    write_charect->setCallbacks(&ble_ch_callbacks);
    write_charect->setValue(0);

    // Start service
    ble_service->start();

    // Setup advertising
    auto advert = BLEDevice::getAdvertising();
    advert->addServiceUUID(BLE_SERVICE);
    advert->setScanResponse(true);
    advert->setMinPreferred(0x06); // iphone fix?
    advert->setMaxPreferred(0x12);

    // Start advertising
    BLEDevice::startAdvertising();
    Serial.println("Started BLE Server");
}

uint32_t blesv_counter = 0;
void blesv_tick()
{
    blesv_counter++;
    // change value and notify
    read_charect->setValue(blesv_counter);
    read_charect->notify(true);
    delay(100);
}

