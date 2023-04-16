#ifndef BLE_SERVER_CALLBACKS_H
#define BLE_SERVER_CALLBACKS_H

#include "SharedManager.h"

#include <NimBLEDevice.h>
#include <functional>

class BleServerCallbacks : public BLEServerCallbacks
{
private:
    bool isConnected_;
    std::function<void(const BluetoothStatus &status)> bluetoothStatusChangedCallback;

public:
    const bool & IsConnected() const { return this->isConnected_; }

    BleServerCallbacks(std::function<void(const BluetoothStatus &status)> btStatusChangedCallback);

    void onConnect(BLEServer *pServer, ble_gap_conn_desc* desc);
    void onDisconnect(BLEServer *pServer);
    
    /***************** New - Security handled here ********************
****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest();
    bool onConfirmPIN(uint32_t pass_key);

    void onAuthenticationComplete(ble_gap_conn_desc desc);
    /*******************************************************************/
};

#endif