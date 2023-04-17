#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "esp_err.h"
#include "mqtt_client.h"

class MqttManager{
private:
    esp_mqtt_client_config_t cfg;
    esp_mqtt_client_handle_t client;

protected:

    static void MqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
    static void LogIfNonZero(const char *message, int error_code);
public:
    bool IsConnected;

    MqttManager();

    esp_err_t Connect(const char *uri, const char *username, const char* password);

    void Publish(const char *topic, const char *data, const int &qos, const int &retain) const;
    void Subscribe(const char *topic, const int &qos) const;
    void UnSubscribe(const char *topic) const;
};

#endif