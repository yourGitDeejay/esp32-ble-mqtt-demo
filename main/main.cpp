#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "BleManager.h"
#include "WifiManager.h"
#include "MqttManager.h"
#include "SharedManager.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

#define TAG "main"

extern "C"
{
    void app_main(void);
}

MqttManager mqtt;

// demo callback method when the characteristic 
// from ble service has changed
void demoCharWrite(const ActivityStatus& status) {
    LOGI(TAG, "\n\nCharacteristic status changed, new status: %i\n\n", (int)status);

    mqtt.Publish(CONFIG_MQTT_TEST_TOPIC, "Characteristic changed!", 1, 0);
}

// callback method if the changed data is string
void demoCharWriteString(const char* data) {
    LOGI(TAG, "\n\nCharacteristic written, new data: %s\n\n", data);

    mqtt.Publish(CONFIG_MQTT_TEST_TOPIC, data, 1, 0);
}

// demo callback when BLE status (connected/disconnected/...) has changed
void btStatusChanged(const BluetoothStatus &status) {
    LOGI(TAG, "BT status changed, new status: %i", (int)status);
}

void app_main(void)
{
    esp_err_t status;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    EventGroupHandle_t wifiEventGroup = xEventGroupCreate();

    bool wifiConnected = false;
    BleManager ble(BLUETOOTH_NAME, &demoCharWrite, &btStatusChanged, &demoCharWriteString);
    WifiManager wifi(wifiEventGroup);

    // connect to desired hotspot
    wifi.Connect(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, WIFI_AUTH_WPA2_PSK);
    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(wifiEventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        wifiConnected = true;
        LOGI(TAG, "connected to ap SSID:%s password:%s", CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        LOGI(TAG, "Failed to connect to SSID:%s, password:%s", CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    } else {
        LOGE(TAG, "UNEXPECTED EVENT");
    }

    if (wifiConnected) {
        // connect to the broker after successful connection to the internet
        LOGI(TAG, "Connecting to MQTT broker, broker = %s, user = %s, password = %s", CONFIG_MQTT_SERVER_URI, CONFIG_MQTT_SERVER_USERNAME, CONFIG_MQTT_SERVER_PASSWORD);
        if ((status = mqtt.Connect(CONFIG_MQTT_SERVER_URI, CONFIG_MQTT_SERVER_USERNAME, CONFIG_MQTT_SERVER_PASSWORD)) != ESP_OK) {
            LOGE(TAG, "Error connecting to MQTT broker: %s", esp_err_to_name(status));
        } else {
            LOGI(TAG, "Wait for some time...");
            vTaskDelay(4000 / portTICK_PERIOD_MS);

            mqtt.Subscribe(CONFIG_MQTT_TEST_TOPIC, 1);
            while (1) {
                // advertise BLE, allow the clients to connect & publish via BLE -> MQTT and vice versa
                LOGI(TAG, "Advertising...");
                ble.Advertise();

                vTaskDelay(10000 / portTICK_PERIOD_MS);
            }
        }
    }

    esp_restart();    
}