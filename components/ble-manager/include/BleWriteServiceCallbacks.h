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
    std::function<void(const char *data)> mCallbackString;

public:
    BleWriteServiceCallbacks(std::function<void(const ActivityStatus &status)> activityStatusChanged, std::function<void(const char *data)> cbString) 
    : mCallback(activityStatusChanged), mCallbackString(cbString) {}

    void onRead(NimBLECharacteristic* pCharacteristic) {

    }
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        LOGI(BLE_WRITE_SERVICE_TAG, "%s : onWrite(), value: %s", pCharacteristic->getUUID().toString().c_str(), pCharacteristic->getValue().c_str());

        mCallbackString(pCharacteristic->getValue().c_str());

        // demo
        int32_t request = pCharacteristic->getValue<int32_t>();
        mCallback((ActivityStatus)request);
    }
};

#endif