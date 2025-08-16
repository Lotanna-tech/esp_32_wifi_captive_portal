#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_handler.h"
#include "wifi.h"
#include "http_sever.h"
static const char *TAG = "app_main";

void app_main(void)
{
    nvs_init();
    char ssid[32] = {0};
    char pass[64] = {0};
    read_wifi_credentials(ssid, sizeof(ssid), pass, sizeof(pass));
    if (strlen(ssid) > 0) {
        ESP_LOGI(TAG, "Found SSID: %s, attempting STA mode", ssid);
        // Try connecting in STA mode
        sta_init(ssid, pass);
    } else {
        ESP_LOGW(TAG, "No Wi-Fi credentials found. Starting AP mode...");
        // Start Access Point mode and HTTP server for config
        ap_init();
        start_web_server();
    }

}
