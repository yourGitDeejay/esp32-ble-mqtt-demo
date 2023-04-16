#include <stdio.h>

#include "BleManager.h"
#include "SharedManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "main"

extern "C"
{
    void app_main(void);
}

void demoCharWrite(const ActivityStatus& status) {
    LOGI(TAG, "Characteristic status changed, new status: %i", (int)status);
}

void btStatusChanged(const BluetoothStatus &status) {
    LOGI(TAG, "BT status changed, new status: %i", (int)status);
}

void app_main(void)
{
    esp_err_t status;
    BleManager bm("ESP32", &demoCharWrite, &btStatusChanged);

    while (1) {
        LOGI(TAG, "Advertising...");
        bm.Advertise();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}