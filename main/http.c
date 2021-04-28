#include <string.h>
#include <sys/param.h>

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_spiffs.h>

#include "http.h"
#include "output.h"
#include "settings.h"

static const char *TAG = "eth_example";

static esp_err_t file_get_handler(httpd_req_t *req)
{
    FILE* f = fopen(req->user_ctx, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file");
        return ESP_ERR_NOT_FOUND;
    }

    char buf[256];

    size_t read;

    httpd_resp_set_type(req,"text/css");

    do{
        read = fread((void*) buf, 1, sizeof(buf), f);
        httpd_resp_send_chunk(req,buf,read);
    } while(read == sizeof(buf));

    fclose(f);

    httpd_resp_send_chunk(req, NULL, 0);


    return ESP_OK;
}

static const httpd_uri_t page_main = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = file_get_handler,
    .user_ctx  = "/spiffs/template_test.html"
};

static const httpd_uri_t page_css = {
    .uri = "/css/styles.css",
    .method = HTTP_GET,
    .handler = file_get_handler,
    .user_ctx  = "/spiffs/css/styles.css"
};

static esp_err_t set_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    char mode[32];
    memset(mode,0,32);
    httpd_query_key_value(buf,"mode",mode,32);
    //remove colons at the end of short strings
    if(mode[strlen(mode)-1]==':' || mode[strlen(mode)-1]==';')
    {
        mode[strlen(mode)-1]='\0';
    }
    SetOutputsMode(mode);

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t page_set_mode = {
    .uri = "/set",
    .method = HTTP_POST,
    .handler = set_post_handler,
    .user_ctx = NULL
};

void setup_web_server()
{
    ESP_LOGI(TAG,"WEBSERVER THREAD STARTED");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    static httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    if(httpd_start(&server,&config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server,&page_main);
        httpd_register_uri_handler(server,&page_css);
        httpd_register_uri_handler(server,&page_set_mode);
    }
}
