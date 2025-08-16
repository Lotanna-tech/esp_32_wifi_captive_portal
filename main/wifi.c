#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "wifi.h"
#include "nvs_handler.h"
static const char *TAG="wifi_handler";

void wifi_handler(void *args,esp_event_base_t event_base,int32_t event_id,void *event_data){
    if(event_base==WIFI_EVENT && event_id==WIFI_EVENT_STA_START){
        esp_wifi_connect();
        ESP_LOGI(TAG,"Wifi Connecting....please wait");
    }
    else if(event_base==WIFI_EVENT && event_id==WIFI_EVENT_STA_DISCONNECTED){
        esp_wifi_connect();
        ESP_LOGW(TAG,"Wifi Disconnnected ...retrying");
    }

    else if(event_base==IP_EVENT && event_id==IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t *event=(ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG,"Got Ip:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}


void ap_init(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t conf=WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&conf);

    wifi_config_t ap_conf={
        .ap={
            .ssid="ESP32 Admin",
            .ssid_len=0,
            .channel=1,
            .password="lotanna9090",
            .max_connection=4,
            .authmode=WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_conf));
    esp_err_t err=esp_wifi_start();
    if (err!=ESP_OK){
        ESP_LOGE(TAG,"AP mode not started successfully");
    }
    else{
        ESP_LOGI(TAG,"AP mode started successfully");
    }

}

void sta_init(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wificfg=WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificfg);
    char ssid[32];
    char pass[64];
    esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,wifi_handler,NULL,NULL);
    esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_handler,NULL,NULL);
    read_wifi_credentials(ssid,sizeof(ssid),pass,sizeof(pass));
    if (strlen(ssid)==0){
        ESP_LOGW(TAG,"No Credentials saved");
    }

    wifi_config_t sta_config={0};
    strlcpy((char *) sta_config.sta.ssid,ssid,sizeof(sta_config.sta.ssid));
    strlcpy((char *) sta_config.sta.password,pass,sizeof(sta_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    esp_wifi_set_config(WIFI_IF_STA,&sta_config);
    esp_err_t ret=esp_wifi_start();
    if(ret!=ESP_OK){
        ESP_LOGE(TAG,"STA Wifi not started ");
        
    }
    else{
        ESP_LOGI(TAG,"STA Wifi started");
    }

}