#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_wifi.h"

#include <string>

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

class WifiManager {
private:
    EventGroupHandle_t wifiEventGroup;
    int numRetry;

    static void WifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
public:
    WifiManager(EventGroupHandle_t eventGroup);

    void Connect(const std::string &ssid, const std::string &password, wifi_auth_mode_t mode);
    void Reset();
};

#endif