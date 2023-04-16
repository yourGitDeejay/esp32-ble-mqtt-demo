#ifndef BLE_WRITE_SERVICE_CALLBACKS_H
#define BLE_WRITE_SERVICE_CALLBACKS_H

#include "SharedManager.h"

#include <NimBLEDevice.h>
#include <stdint.h>
#include <functional>

#define BLE_WRITE_SERVICE_TAG "BleWriteServiceCallbacks"

/** Handler class for characteristic actions */
class BleWriteServiceCallbacks : public NimBLECharacteristicCallbacks {
private:
    std::function<void(const ActivityStatus &status)> mCallback;

public:
    BleWriteServiceCallbacks(std::function<void(const ActivityStatus &status)> activityStatusChanged) 
    : mCallback(activityStatusChanged) {}

    void onRead(NimBLECharacteristic* pCharacteristic) {

    }
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        LOGI(BLE_WRITE_SERVICE_TAG, "%s : onWrite(), value: %s", pCharacteristic->getUUID().toString().c_str(), pCharacteristic->getValue().c_str());

        int32_t request = pCharacteristic->getValue<int32_t>();

        mCallback((ActivityStatus)request);
    }
};

#endif