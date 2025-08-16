#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "nvs_handler.h"
#include <string.h>
#include "http_sever.h"
#include <ctype.h>

static const char *TAG="http_handler";
const char *html_page="<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"  <meta charset=\"UTF-8\">"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"  <title>Wi-Fi Setup</title>"
"  <style>"
"    body {"
"      font-family: Arial, sans-serif;"
"      background: #f4f6f8;"
"      margin: 0;"
"      display: flex;"
"      justify-content: center;"
"      align-items: center;"
"      height: 100vh;"
"    }"
"    .container {"
"      background: #fff;"
"      border-radius: 12px;"
"      box-shadow: 0 4px 10px rgba(0,0,0,0.1);"
"      padding: 20px 25px;"
"      width: 95%;"
"      max-width: 400px;"
"      text-align: center;"
"    }"
"    h2 {"
"      margin-bottom: 20px;"
"      color: #333;"
"    }"
"    .input-group {"
"      position: relative;"
"      margin: 10px 0;"
"    }"
"    input[type=\"text\"], input[type=\"password\"] {"
"      width: 100%;"
"      padding: 12px;"
"      border: 1px solid #ccc;"
"      border-radius: 8px;"
"      font-size: 16px;"
"    }"
"    .toggle-eye {"
"      position: absolute;"
"      right: 12px;"
"      top: 50%;"
"      transform: translateY(-50%);"
"      cursor: pointer;"
"      font-size: 18px;"
"      color: #888;"
"    }"
"    button {"
"      background: #0078d7;"
"      color: #fff;"
"      border: none;"
"      padding: 12px;"
"      width: 100%;"
"      border-radius: 8px;"
"      font-size: 16px;"
"      cursor: pointer;"
"      margin-top: 15px;"
"    }"
"    button:hover {"
"      background: #005fa3;"
"    }"
"    .footer {"
"      margin-top: 15px;"
"      font-size: 12px;"
"      color: #777;"
"    }"
"  </style>"
"</head>"
"<body>"
"  <div class=\"container\">"
"    <h2>Connect to Wi-Fi</h2>"
"    <form method=\"POST\" action=\"/connect\">"
"      <div class=\"input-group\">"
"        <input type=\"text\" name=\"ssid\" placeholder=\"Wi-Fi Name (SSID)\" required>"
"      </div>"
"      <div class=\"input-group\">"
"        <input type=\"password\" name=\"pass\" id=\"password\" placeholder=\"Password\" required>"
"        <span class=\"toggle-eye\" onclick=\"togglePassword()\">&#128065;</span>"
"      </div>"
"      <button type=\"submit\">Save & Connect</button>"
"    </form>"
"    <div class=\"footer\">"
"      <p>ESP32 Setup Portal</p>"
"    </div>"
"  </div>"
"  <script>"
"    function togglePassword(){"
"      var passField = document.getElementById('password');"
"      if(passField.type === 'password'){"
"        passField.type = 'text';"
"      } else {"
"        passField.type = 'password';"
"      }"
"    }"
"  </script>"
"</body>"
"</html>";

static esp_err_t get_root_handler(httpd_req_t *req){
    httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req,html_page,strlen(html_page));
    return ESP_OK;
}

static void url_decode(char *dst, const char *src, size_t dst_size) {
    char a, b;
    while (*src && dst_size > 1) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
        dst_size--;
    }
    *dst = '\0';
}


static esp_err_t get_post_handler(httpd_req_t *req){
    char buf[256];
    int total_len=req->content_len;
    if(total_len>=sizeof(buf)){
        ESP_LOGE(TAG,"Post Content too long");
        httpd_resp_send_err(req,HTTPD_400_BAD_REQUEST,"Content too long");
        return ESP_FAIL;

    }

    int received=httpd_req_recv(req,buf,total_len);
    if(received<=0){
        ESP_LOGE(TAG,"Failed to receive Post Data");
        return ESP_FAIL;
    }
    buf[received]='\0';
    ESP_LOGI(TAG,"Raw Post Data %s",buf);

    char ssid_encoded[64]={0};
    char pass_encoded[64]={0};

    httpd_query_key_value(buf,"ssid",ssid_encoded,sizeof(ssid_encoded));
    httpd_query_key_value(buf,"pass",pass_encoded,sizeof(pass_encoded));

    //Decode the data
    char ssid[64]={0};
    char pass[64]={0};
    size_t ssid_size=sizeof(ssid);
    size_t pass_size=sizeof(pass);

    url_decode(ssid,ssid_encoded,ssid_size);
    url_decode(pass,pass_encoded,pass_size);
    ESP_LOGI(TAG,"Decoded SSID %s",ssid);
    ESP_LOGI(TAG,"Decoded Password %s",pass);

    save_wifi_credentials(ssid,pass);
    const char *resp="<h2>Wi-Fi Credentials Saved! Rebooting...</h2>";
    httpd_resp_send(req,resp,strlen(resp));

    return ESP_OK;

}

httpd_handle_t start_web_server(){
    httpd_config_t config=HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;  // Increase stack for large pages
    config.max_uri_handlers = 16;       
    httpd_handle_t server=NULL;

    if(httpd_start(&server,&config)==ESP_OK){
        httpd_uri_t get_uri={
            .uri="/",
            .method=HTTP_GET,
            .handler=get_root_handler,
            .user_ctx=NULL
        };
        httpd_register_uri_handler(server,&get_uri);

        httpd_uri_t post_uri={
            .uri="/connect",
            .method=HTTP_POST,
            .handler=get_post_handler,
            .user_ctx=NULL
        };
        httpd_register_uri_handler(server,&post_uri);
    }
    return server;

}

