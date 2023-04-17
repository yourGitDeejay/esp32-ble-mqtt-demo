#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include "BleServerCallbacks.h"
#include "SharedManager.h"

#include <NimBLEDevice.h>
#include <vector>
#include <string>
#include <functional>

struct Value{
    BLECharacteristic *characteristic;
    std::string uuid;
    uint32_t *value;
};

class BleManager {
private:
    bool isAdvertising;

    BLEServer *server;
    BLEService *writeService;
    BLEService *notifyService;
    BLECharacteristic *requestCharacteristic;

    std::vector<Value> notifyCharacteristics;
    BleServerCallbacks callbacks;

public:
    BleManager(
        const std::string &deviceName, 
        std::function<void(const ActivityStatus &status)> statusChanged, 
        std::function<void(const BluetoothStatus &status)> btStatusChangedCallback,
        std::function<void(const char *data)> mCallbackString
    );
    ~BleManager();

    void Advertise();
    void StopAdvertise();
    void NotifyAll();
    void Notify(const std::string& uuid);

    void AddNotifyCharacteristic(const std::vector<NotifyValue> &nValues);
    void AddNotifyCharacteristic(const NotifyValue &nValue);
};

#endif