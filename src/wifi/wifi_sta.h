#include <stdio.h>
#include <string.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_log.h>

typedef enum WifiStatus {
    WIFI_CLIENT_START = 0,
    WIFI_CLIENT_WAITING_CONNECTION,
    WIFI_CLIENT_CONNECTED,
    WIFI_CLIENT_DISCONNECTED,
    WIFI_CLIENT_GOT_IP,
    WIFI_CLIENT_WORKING
} WifiStatus;

typedef struct WifiState {
    WifiStatus status;
    char *wifi_ssid;
    char *wifi_password;
    uint8_t ip[4];
    uint8_t mac[6];
} WifiState;


void wifi_sta_init(WifiState *state);
void wifi_status_print(WifiState *state);

