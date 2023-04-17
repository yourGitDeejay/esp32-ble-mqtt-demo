#include "MqttManager.h"
#include "SharedManager.h"

#include "esp_system.h"

#define TAG "MqttManager"

MqttManager::MqttManager(): IsConnected(false) { }


void MqttManager::LogIfNonZero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
void MqttManager::MqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    MqttManager *mm = (MqttManager*) handler_args;

    LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        mm->IsConnected = true;
        LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        mm->IsConnected = false;
        LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        // LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            MqttManager::LogIfNonZero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            MqttManager::LogIfNonZero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            MqttManager::LogIfNonZero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_err_t MqttManager::Connect(const char *uri, const char *username, const char* password) {
    LOGI(TAG, "Generating MQTT config");
    esp_mqtt_client_config_t cfg = { 0 };
    cfg.uri = uri;
    cfg.username = username;
    cfg.password = password;
    this->client = esp_mqtt_client_init(&cfg);
    LOGI(TAG, "MQTT init success");
    
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_err_t e = esp_mqtt_client_register_event(this->client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, &MqttManager::MqttEventHandler, this);
    LOGI(TAG, "MQTT register event");
    if (e != ESP_OK) return e;
    return esp_mqtt_client_start(this->client);
}

void MqttManager::Publish(const char *topic, const char *data, const int &qos, const int &retain) const {
    if (!IsConnected) {
        LOGE(TAG, "Connect to broker before publishing! Publish aborted.");
        return;
    }
    int msg_id = esp_mqtt_client_publish(client, topic, data, 0, qos, retain);
    LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}

void MqttManager::Subscribe(const char *topic, const int &qos) const {
    if (!IsConnected) {
        LOGE(TAG, "Connect to broker before subscribing! Subscribe aborted.");
        return;
    }
    int msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
    LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}

void MqttManager::UnSubscribe(const char *topic) const {
    if (!IsConnected) {
        LOGE(TAG, "Connect to broker before unsubscribing! Unsubscribe aborted.");
        return;
    }
    int msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
    LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
}