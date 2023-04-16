#include "BleManager.h"
#include "BleWriteServiceCallbacks.h"

#define TAG "BleManager"

BleManager::BleManager(
    const std::string &deviceName, 
    std::function<void(const ActivityStatus &status)> statusChangedCallback, 
    std::function<void(const BluetoothStatus &status)> btStatusChangedCallback
) : isAdvertising(false), 
    callbacks(btStatusChangedCallback)
{
    // Create the BLE Device
    NimBLEDevice::init(deviceName);

    // Create the BLE Server
    this->server = NimBLEDevice::createServer();

    this->notifyService = this->server->createService(NOTIFY_SERVICE_UUID);
    this->writeService = this->server->createService(WRITE_SERVICE_UUID);

    this->requestCharacteristic = this->writeService->createCharacteristic(
        REQUEST_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );
    this->requestCharacteristic->setCallbacks(new BleWriteServiceCallbacks(statusChangedCallback));

    this->server->setCallbacks(&(this->callbacks), false);

    LOGI(TAG, "Init");
}

BleManager::~BleManager() {
    for (auto &s : this->notifyCharacteristics)
        this->notifyService->removeCharacteristic(s.characteristic, true);
    this->notifyCharacteristics.clear();

    this->writeService->removeCharacteristic(this->requestCharacteristic, true);

    this->server->removeService(this->notifyService);
    this->server->removeService(this->writeService);
    
    LOGI(TAG, "Destructor");
}

void BleManager::Advertise() {
    if (!this->isAdvertising && !this->callbacks.IsConnected()) {
        this->isAdvertising = true;

        this->notifyService->start();
        this->writeService->start();

        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        /** Add the services to the advertisment data **/
        pAdvertising->addServiceUUID(this->notifyService->getUUID());
        pAdvertising->addServiceUUID(this->writeService->getUUID());

        pAdvertising->setScanResponse(true);
        pAdvertising->start();

        LOGI(TAG, "Started advertising, waiting for the client to connect...");
    }
}

void BleManager::StopAdvertise() {
    if (this->isAdvertising) {
        this->isAdvertising = false;

        NimBLEDevice::stopAdvertising();
        
        LOGI(TAG, "Stopped advertising");
    }
}

void BleManager::NotifyAll() {
    if (!this->callbacks.IsConnected()) {
        LOGW(TAG, "No device is connected, could not notify the client.");
        return;
    }
    
    for (auto &c : this->notifyCharacteristics) {
        c.characteristic->setValue(*(c.value));
        c.characteristic->notify();

        LOGI(TAG, "Notified uuid: %s, value: %u", c.uuid.c_str(), *(c.value));
    }
}

void BleManager::Notify(const std::string& uuid) {
    if (!this->callbacks.IsConnected()) {
        LOGW(TAG, "No device is connected, could not notify the client.");
        return;
    }

    for (auto &c : this->notifyCharacteristics) {
        if (c.uuid == uuid) {
            c.characteristic->setValue(*(c.value));
            c.characteristic->notify();

            LOGI(TAG, "Notified uuid: %s, value: %u", c.uuid.c_str(), *(c.value));
            break;
        }
    }
}

void BleManager::AddNotifyCharacteristic(const std::vector<NotifyValue> &nValues) {
    for (const NotifyValue &nValue: nValues) {
        this->AddNotifyCharacteristic(nValue);
    }
}

void BleManager::AddNotifyCharacteristic(const NotifyValue &nValue) {
    NimBLECharacteristic* c;
    Value v;

    c = this->notifyService->createCharacteristic(
        nValue.uuid,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE);

    v.value = nValue.value;
    v.uuid = nValue.uuid;
    v.characteristic = c;

    notifyCharacteristics.push_back(v);

    LOGI(TAG, "Added notify characteristic with uuid: %s", v.uuid.c_str());
}