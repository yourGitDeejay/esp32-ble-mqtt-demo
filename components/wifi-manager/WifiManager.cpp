#include "WifiManager.h"
#include "SharedManager.h"

#include <cstring>

#include "esp_err.h"
#include "esp_log.h"

#define MAX_RETRY           5

#define TAG     "WifiManager"

WifiManager::WifiManager(EventGroupHandle_t eventGroup)
    : wifiEventGroup(eventGroup), numRetry(0)
{
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    LOGI(TAG, "wifi_init_sta finished.");
}

void WifiManager::Connect(const std::string &ssid, const std::string &password, wifi_auth_mode_t mode) {
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &WifiManager::WifiEventHandler,
                                                        this,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &WifiManager::WifiEventHandler,
                                                        this,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = { 0 };
    strlcpy((char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid));
    strlcpy((char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password));
    // strcpy((char *)wifi_config.sta.ssid, ssid.c_str());
    // strcpy((char *)wifi_config.sta.password, password.c_str());
    // wifi_config.sta.threshold.authmode = mode;

    LOGI(TAG, "Connecting to %s, pass = %s, mode = %i", wifi_config.sta.ssid, wifi_config.sta.password, mode);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WifiManager::Reset() {
    ESP_ERROR_CHECK(esp_wifi_stop());
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiManager::WifiEventHandler);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiManager::WifiEventHandler);
    numRetry = 0;

    LOGI(TAG, "Wifi connect stopped.");
}

void WifiManager::WifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    WifiManager *wm = (WifiManager*) arg;
    
    LOGI(TAG, "event id = %i", event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* data = (wifi_event_sta_disconnected_t*) event_data;
        
        if (wm->numRetry < MAX_RETRY) {
            ESP_ERROR_CHECK(esp_wifi_connect());
            wm->numRetry++;
            LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(wm->wifiEventGroup, WIFI_FAIL_BIT);
        }
        LOGI(TAG,"connect to the AP fail, reason = %i", data->reason);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        wm->numRetry = 0;
        xEventGroupSetBits(wm->wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}