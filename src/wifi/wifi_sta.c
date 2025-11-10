#include "wifi_sta.h"

static const char *TAG = "WIFI";

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                          void *event_data) 
{   

    WifiState *wifi_state = (WifiState*)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        wifi_state->status = WIFI_CLIENT_START;
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) 
    {
        wifi_state->status = WIFI_CLIENT_CONNECTED;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        wifi_state->status = WIFI_CLIENT_DISCONNECTED;
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        memcpy(wifi_state->ip,(uint8_t *)&event->ip_info.ip.addr, 4);
        esp_wifi_get_mac(ESP_IF_WIFI_STA, wifi_state->mac);
        wifi_state->status = WIFI_CLIENT_GOT_IP;
    }

}

void wifi_sta_init(WifiState *state) 
{   
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_cfg = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK
        }
    };

    memcpy(wifi_cfg.sta.ssid, state->wifi_ssid, strlen(state->wifi_ssid));
    memcpy(wifi_cfg.sta.password, state->wifi_password, strlen(state->wifi_password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &event_handler, state, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        &event_handler, state, &instance_got_ip));

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    state->status = WIFI_CLIENT_START;
}

void wifi_status_print(WifiState *state) 
{
    switch (state->status) 
    {
        case WIFI_CLIENT_START:
            state->status = WIFI_CLIENT_WAITING_CONNECTION;
            return;
        case WIFI_CLIENT_WAITING_CONNECTION:
            ESP_LOGI(TAG, "Trying connect to Wifi Point");
            return;
        case WIFI_CLIENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to Wifi Point");
            return;
        case WIFI_CLIENT_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from Wifi Point");
            break;
        case WIFI_CLIENT_GOT_IP:
            ESP_LOGI(TAG, "GOT IP!");
            ESP_LOGI(TAG, "IP-адрес: %d.%d.%d.%d\n", state->ip[0], state->ip[1], state->ip[2], state->ip[3]);
            state->status = WIFI_CLIENT_WORKING;
            return;
        case WIFI_CLIENT_WORKING:
            ESP_LOGI(TAG, "WIFI WORKING");
            ESP_LOGI(TAG, "IP-адрес: %d.%d.%d.%d\n", state->ip[0], state->ip[1], state->ip[2], state->ip[3]);
            return;
        default:
            break;
    }
}
