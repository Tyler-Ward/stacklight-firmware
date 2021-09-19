#include <string.h>
#include <sys/param.h>

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_spiffs.h>

#include "http.h"
#include "output.h"
#include "indicators.h"
#include "settings.h"
#include "rdm.h"
#include "version.h"

static const char *TAG = "stacklight_http";

static esp_err_t file_get_handler(httpd_req_t *req)
{
    FILE* f = fopen(req->user_ctx, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file");
        return ESP_FAIL;
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

/* Get variable 
 *
 * param buffer buffer containing tag, tag is replaced with value.
 */
static uint16_t getVariable(char* buffer)
{
    if(strcmp(buffer,"ArtnetNet")==0)
    {
        return sprintf(buffer,"%d",settingsGetArtnetNet());
    }
    if(strcmp(buffer,"ArtnetSubNet")==0)
    {
        return sprintf(buffer,"%d",settingsGetArtnetSubNet());
    }
    if(strcmp(buffer,"ArtnetUniverse")==0)
    {
        return sprintf(buffer,"%d",settingsGetArtnetUniverse());
    }
    if(strcmp(buffer,"DMXAddr")==0)
    {
        return sprintf(buffer,"%d",settingsGetDmxAddr());
    }
    if(strcmp(buffer,"RDMUID")==0)
    {
        uint8_t rdmUID[6];
        rdmGetRDMUID(rdmUID);
        return sprintf(buffer,"%02X%02X:%02X%02X%02X%02X",
            rdmUID[0],rdmUID[1],rdmUID[2],rdmUID[3],rdmUID[4],rdmUID[5]);
    }
    if(strcmp(buffer,"LocateSelectOff")==0)
    {
        if(indicatorsGetLocate())
        {
            buffer[0]='\0';
            return 0;
        }
        else
            return sprintf(buffer," selected");
    }
    if(strcmp(buffer,"LocateSelectOn")==0)
    {
        if(indicatorsGetLocate())
            return sprintf(buffer," selected");
        else
        {
            buffer[0]='\0';
            return 0;
        }
    }
    if(strcmp(buffer,"version")==0)
    {
        return sprintf(buffer,SOFTWARE_VERSION_STRING);
    }

    buffer[0]='\0';
    return(0);
}

static esp_err_t template_get_handler(httpd_req_t *req)
{
    FILE* f = fopen(req->user_ctx, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open template");
        return ESP_FAIL;
    }

    char buf[256];
    uint16_t index = 0;
    char ch;

    while(fread((void*)&ch,1,1,f))
    {
        if(ch =='%')
        {
            //enter tag processing
            if(index!=0)
            {
                //clear buffer
                httpd_resp_send_chunk(req,buf,index);
                index=0;
            }

            //copy tag into buffer
            while(fread((void*)&ch,1,1,f))
            {
                if(ch == '%')
                {
                    //end of tag
                    buf[index]='\0';
                    index = getVariable(buf);
                    break;
                }
                else
                {
                    buf[index] = ch;
                    index++;

                    //check for overrunning tags
                    if(index==255) //need one space for the end of string character
                    {
                        ESP_LOGE(TAG, "template tag too long");
                        fclose(f);
                        return ESP_FAIL;
                    }
                }
            }
        }
        else
        {
            buf[index] = ch;
            index++;
            //send data if buffer is full
            if(index==256)
            {
                httpd_resp_send_chunk(req,buf,index);
                index=0;
            }
        }
    }

    if(index!=0)
    {
        httpd_resp_send_chunk(req,buf,index);
        index=0;
    }

    httpd_resp_send_chunk(req, NULL, 0);

    fclose(f);

    return ESP_OK;
}

static const httpd_uri_t page_main = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = template_get_handler,
    .user_ctx  = "/spiffs/index.html"
};

static const httpd_uri_t page_index = {
    .uri = "/index.html",
    .method = HTTP_GET,
    .handler = template_get_handler,
    .user_ctx  = "/spiffs/index.html"
};

static const httpd_uri_t page_about = {
    .uri = "/about.html",
    .method = HTTP_GET,
    .handler = template_get_handler,
    .user_ctx  = "/spiffs/about.html"
};

static const httpd_uri_t page_css = {
    .uri = "/css/styles.css",
    .method = HTTP_GET,
    .handler = file_get_handler,
    .user_ctx  = "/spiffs/css/styles.css"
};

static const httpd_uri_t page_js = {
    .uri = "/js/scripts.js",
    .method = HTTP_GET,
    .handler = file_get_handler,
    .user_ctx  = "/spiffs/js/scripts.js"
};

static esp_err_t set_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    if(remaining>100)
    {
        httpd_resp_set_status(req,"413 Payload Too Large");
        httpd_resp_send(req,"Post Payload too large",HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    /* Read the data for the request */
    if ((ret = httpd_req_recv(req, buf,MIN(remaining, sizeof(buf)))) <= 0)
    {
        return ESP_FAIL;
    }

    //null terminate string for processing
    buf[ret]='\0';

    //get values
    char mode[32];

    memset(mode,0,32);
    if(httpd_query_key_value(buf,"mode",mode,32)!=ESP_OK)
    {
        httpd_resp_send_err(req,HTTPD_400_BAD_REQUEST,NULL);
        return ESP_FAIL;
    }

    SetOutputsMode(mode);

    // send success
    httpd_resp_send(req, "Success", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t page_set_mode = {
    .uri = "/set",
    .method = HTTP_POST,
    .handler = set_post_handler,
    .user_ctx = NULL
};

static esp_err_t artnet_post_handler(httpd_req_t *req)
{
    char buf[101];
    int ret, remaining = req->content_len;

    if(remaining>100)
    {
        httpd_resp_set_status(req,"413 Payload Too Large");
        httpd_resp_send(req,"Post Payload too large",HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    /* Read the data for the request */
    if ((ret = httpd_req_recv(req, buf,MIN(remaining, sizeof(buf)))) <= 0)
    {
        return ESP_FAIL;
    }

    //null terminate string for processing
    buf[ret]='\0';

    uint16_t newDMXAddr = 0;
    uint8_t newArtnetNet = 0;
    uint8_t newArtnetSubNet = 0;
    uint8_t newArtnetUniverse = 0;

    //get values
    char value[32];
    char* end;
    uint8_t missingValues = 0;

    memset(value,0,32);
    if(httpd_query_key_value(buf,"DMXAddr",value,32)==ESP_OK)
    {
        newDMXAddr = strtol(value,&end,10);
    }
    else
    {
        missingValues+=1;
    }

    memset(value,0,32);
    if(httpd_query_key_value(buf,"Net",value,32)==ESP_OK)
    {
        newArtnetNet = strtol(value,&end,10);
    }
    else
    {
        missingValues+=1;
    }

    memset(value,0,32);
    if(httpd_query_key_value(buf,"Subnet",value,32)==ESP_OK)
    {
        newArtnetSubNet = strtol(value,&end,10);
    }
    else
    {
        missingValues+=1;
    }

    memset(value,0,32);
    if(httpd_query_key_value(buf,"Universe",value,32)==ESP_OK)
    {
        newArtnetUniverse = strtol(value,&end,10);
    }
    else
    {
        missingValues+=1;
    }

    if(missingValues>0)
    {
        httpd_resp_send_err(req,HTTPD_400_BAD_REQUEST,NULL);
        return ESP_FAIL;
    }

    /* Log data received */
    ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
    ESP_LOGI(TAG, "%d-%d-%d  %d", newArtnetNet, newArtnetSubNet, newArtnetUniverse, newDMXAddr);
    ESP_LOGI(TAG, "====================================");

    //check values
    if(newArtnetNet<=127 && newArtnetNet>=0
        && newArtnetSubNet<=15 && newArtnetSubNet>=0
        && newArtnetUniverse<=15 && newArtnetUniverse>=0
        && newDMXAddr<=418 && newDMXAddr>=1)
    {
        settingsSetArtnetNet(newArtnetNet);
        settingsSetArtnetSubNet(newArtnetSubNet);
        settingsSetArtnetUniverse(newArtnetUniverse);
        settingsSetDmxAddr(newDMXAddr);
    }
    else
    {
        httpd_resp_send_err(req,HTTPD_400_BAD_REQUEST,NULL);
        return ESP_FAIL;
    }

    // send success
    httpd_resp_send(req, "Success", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t artnet_set_mode = {
    .uri = "/artnetConfig",
    .method = HTTP_POST,
    .handler = artnet_post_handler,
    .user_ctx = NULL
};

static esp_err_t locate_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret=0, remaining = req->content_len;

    if(remaining>100)
    {
        httpd_resp_set_status(req,"413 Payload Too Large");
        httpd_resp_send(req,"Post Payload too large",HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    /* Read the data for the request */
    if ((ret = httpd_req_recv(req, buf,MIN(remaining, sizeof(buf)))) <= 0)
    {
        return ESP_FAIL;
    }

    //null terminate string for processing
    buf[ret]='\0';

    char mode[32];
    memset(mode,0,32);
    if(httpd_query_key_value(buf,"locate",mode,32)!=ESP_OK)
    {
        httpd_resp_send_err(req,HTTPD_400_BAD_REQUEST,NULL);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
    ESP_LOGI(TAG, "%s", mode);
    ESP_LOGI(TAG, "====================================");

    if(strcmp(mode,"true")==0)
    {
        indicatorsSetLocate(1);
    }
    else if(strcmp(mode,"false")==0)
    {
        indicatorsSetLocate(0);
    }
    else
    {
        httpd_resp_send_err(req,HTTPD_400_BAD_REQUEST,NULL);
        return ESP_FAIL;
    }

    // send success
    httpd_resp_send(req, "Success", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t locate_set_mode = {
    .uri = "/locate",
    .method = HTTP_POST,
    .handler = locate_post_handler,
    .user_ctx = NULL
};

void httpSetup()
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
        httpd_register_uri_handler(server,&page_index);
        httpd_register_uri_handler(server,&page_about);
        httpd_register_uri_handler(server,&page_css);
        httpd_register_uri_handler(server,&page_js);
        httpd_register_uri_handler(server,&page_set_mode);
        httpd_register_uri_handler(server,&artnet_set_mode);
        httpd_register_uri_handler(server,&locate_set_mode);
    }
}
