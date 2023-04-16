#ifndef SHARED_MANAGER_H
#define SHARED_MANAGER_H

#include <stdint.h>
#include <string>

#include "driver/gpio.h"
#include "esp_log.h"

#define BLUETOOTH_NAME          "ESP32"

#define HALL_EFFECT_SENSOR_PORT GPIO_NUM_23
#define BME280_SDA_PORT         GPIO_NUM_27
#define BME280_SCL_PORT         GPIO_NUM_26

#define NOTIFY_SERVICE_UUID     "9ccb9675-a230-4aca-b42f-48f5ac9f4cf4"
#define WRITE_SERVICE_UUID      "c2a0ced5-58f6-4084-a598-f4de8500467c"

#define SPEED_UUID              "5914eec0-5ae0-402c-8557-637c48389647"
#define TICKS_UUID              "ad4dfcbd-b0ad-49c2-ae56-9e7805ec29b2"

#define TEMPERATURE_UUID        "30d985fb-c671-4015-9b9c-95812a54effd"
#define HUMIDITY_UUID           "fb86b390-cf9f-48c8-b83d-47882dc3cdb3"
#define PRESSURE_UUID           "c06aac76-87bb-492c-b5a3-ce8f8ad563d7"
#define ALTITUDE_UUID           "52436f9b-726f-4d93-a7b8-605b8a510c88"

#define REQUEST_UUID            "d8b8d475-2ad1-4272-af5a-9262075f3b5f"
#define RESPONSE_STATUS_UUID    "e2849789-313e-4f4c-a7a7-8656ba518127"
#define ACTIVITY_STATUS_UUID    "c327f525-1fd1-4add-a69b-afa6a41d9cf9"

#ifdef CONFIG_SHOW_DEBUG_MESSAGES
    #define LOGE( TAG, MESSAGE, ... ) ESP_LOGE( TAG, MESSAGE, ##__VA_ARGS__ )
    #define LOGW( TAG, MESSAGE, ... ) ESP_LOGW( TAG, MESSAGE, ##__VA_ARGS__ )
    #define LOGI( TAG, MESSAGE, ... ) ESP_LOGI( TAG, MESSAGE, ##__VA_ARGS__ )

    #define LOGE_FULL( TAG, STATUS, METHOD_NAME ) ESP_LOGE( TAG, "%s failed. Error code: %i. Error meaning: %s", METHOD_NAME, STATUS, esp_err_to_name(STATUS) )
    #define LOGW_FULL( TAG, STATUS, METHOD_NAME ) ESP_LOGW( TAG, "%s failed. Error code: %i. Error meaning: %s", METHOD_NAME, STATUS, esp_err_to_name(STATUS) )
    #define LOGI_FULL( TAG, STATUS, METHOD_NAME ) ESP_LOGI( TAG, "%s failed. Error code: %i. Error meaning: %s", METHOD_NAME, STATUS, esp_err_to_name(STATUS) )
#else
    #define LOGE( TAG, MESSAGE, ... )
    #define LOGW( TAG, MESSAGE, ... )
    #define LOGI( TAG, MESSAGE, ... )

    #define LOGE_FULL( TAG, STATUS, METHOD_NAME )
    #define LOGW_FULL( TAG, STATUS, METHOD_NAME )
    #define LOGI_FULL( TAG, STATUS, METHOD_NAME )
#endif

enum class ActivityStatus {
    START,
    RESUME,
    PAUSE,
    STOP,
    IDLE,
    CALIBRATE,
    NONE
};

enum class BluetoothStatus {
    CONNECTED,
    DISCONNECTED,
    NONE
};

struct NotifyValue{
    std::string uuid;
    uint32_t *value;
};

#endif