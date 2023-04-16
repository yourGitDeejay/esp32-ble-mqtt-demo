#include <stdio.h>

#include "BleManager.h"
#include "MqttManager.h"
#include "SharedManager.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

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
    BleManager bm(BLUETOOTH_NAME, &demoCharWrite, &btStatusChanged);
    MqttManager mm(CONFIG_MQTT_SERVER_URI, CONFIG_MQTT_SERVER_USERNAME, CONFIG_MQTT_SERVER_PASSWORD);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if ((status = mm.Connect()) != ESP_OK) {
        LOGE(TAG, "Error connecting to MQTT broker: %s", esp_err_to_name(status));
    }

    while (1) {
        LOGI(TAG, "Advertising...");
        bm.Advertise();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}