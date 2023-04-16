#include "BleServerCallbacks.h"

#define TAG "BleServerCallbacks"

BleServerCallbacks::BleServerCallbacks(std::function<void(const BluetoothStatus &status)> btStatusChangedCallback)
    : isConnected_(false), bluetoothStatusChangedCallback(btStatusChangedCallback)
{
}

void BleServerCallbacks::onConnect(BLEServer *pServer, ble_gap_conn_desc* desc)
{
    LOGI(TAG, "Connected to device: %s", pServer->getPeerInfo(0).getAddress().toString().c_str());

    // uint16_t handle = pServer->getPeerInfo(0).getConnHandle();
    // pServer->updateConnParams(handle, 1, 5, 10, 20);
    // LOGI(TAG, "Connection params reset");

    this->bluetoothStatusChangedCallback(BluetoothStatus::CONNECTED);

    this->isConnected_ = true;
};

void BleServerCallbacks::onDisconnect(BLEServer *pServer)
{
    LOGI(TAG, "Disconnected with device");
    this->bluetoothStatusChangedCallback(BluetoothStatus::DISCONNECTED);
    this->isConnected_ = false;
}
/***************** New - Security handled here ********************
****** Note: these are the same return values as defaults ********/
uint32_t BleServerCallbacks::onPassKeyRequest()
{
    LOGI(TAG, "Server PassKeyRequest");
    return 123456;
}

bool BleServerCallbacks::onConfirmPIN(uint32_t pass_key)
{
    LOGI(TAG, "The passkey YES/NO number: %d", pass_key);
    return true;
}

void BleServerCallbacks::onAuthenticationComplete(ble_gap_conn_desc desc)
{
    LOGI(TAG, "Starting BLE work!");
}
/*******************************************************************/