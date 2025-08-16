#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_handler.h"
static const char *TAG="nvs_storage";

void save_wifi_credentials(const char *ssid,const char *pass){
    nvs_handle_t handle;
    esp_err_t ret=nvs_open("wifi_store",NVS_READWRITE,&handle);
    if (ret!=ESP_OK){
        ESP_LOGE(TAG,"NVS Storage could not be opened");
    }
    else{
        ESP_LOGI(TAG,"NVS Storage Opened Successfully");
    }

    ESP_ERROR_CHECK(nvs_set_str(handle,"ssid",ssid));
    ESP_ERROR_CHECK(nvs_set_str(handle,"pass",pass));

    esp_err_t ret1=nvs_commit(handle);

    if(ret1!=ESP_OK){
        ESP_LOGE(TAG,"Wifi Credentials couldnt be saved ");
    }
    else{
        ESP_LOGI(TAG,"Wifi Credentials saved");

    }
    nvs_close(handle);
}

void read_wifi_credentials(char *ssid,size_t ssid_size,char *pass,size_t pass_size){
    nvs_handle_t handle;
    esp_err_t ret=nvs_open("wifi_store",NVS_READONLY,&handle);
    if(ret!=ESP_OK){
        ESP_LOGE(TAG,"NVS couldnt be opened");
    }
    else{
        ESP_LOGI(TAG,"NVS opened successfully");
    }

    esp_err_t ret1=nvs_get_str(handle,"ssid",ssid,&ssid_size);
    if(ret1==ESP_ERR_NVS_NOT_FOUND){
        ssid[0]='\0';
    }
    esp_err_t ret2=nvs_get_str(handle,"pass",pass,&pass_size);
    if (ret2==ESP_ERR_NVS_NOT_FOUND){
        pass[0]='\0';
    }

    nvs_close(handle);
}



void nvs_init(){
    esp_err_t ret=nvs_flash_init();
    if(ret==ESP_ERR_NVS_NO_FREE_PAGES || ret==ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_LOGE(TAG,"Erasing NVS due to lack of free pages or a new version found");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    else{
        ESP_LOGI(TAG,"NVS initialized");
    }
}