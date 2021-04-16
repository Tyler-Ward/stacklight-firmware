#include <string.h>
#include <sys/param.h>

#include <esp_http_server.h>
#include <esp_log.h>

#include "http.h"
#include "output.h"

static const char *TAG = "eth_example";

static esp_err_t status_get_handler(httpd_req_t *req)
{
    char ret[] = "<h1>Light Control</h1><form action=\"/set\" method=\"post\"><select id=\"mode\" name=\"mode\"><option value=\"off\">Off</option><option value=\"red\">Red</option><option value=\"yellow\">Yellow</option><option value=\"green\">Green</option><option value=\"all\">All</option></select><br><button type=\"submit\">Submit</button></form>";

    httpd_resp_send(req,ret,strlen(ret));

    return ESP_OK;
}

static const httpd_uri_t page_main = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = status_get_handler,
    .user_ctx  = NULL
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
    if(mode[strlen(mode)-1]==':')
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
    

    static httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    if(httpd_start(&server,&config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server,&page_main);
        httpd_register_uri_handler(server,&page_set_mode);
    }
}