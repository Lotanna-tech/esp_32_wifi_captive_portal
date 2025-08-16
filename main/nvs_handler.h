#pragma once
#include "esp_err.h"
void save_wifi_credentials(const char *ssid,const char *pass);
void read_wifi_credentials(char *ssid,size_t ssid_size,char *pass,size_t pass_size);
void nvs_init();