#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>

#include "settings.h"

static const char *TAG = "settings";

nvs_handle_t nvsHandle;

//variables to store cached values
static uint16_t dmxAddr = 1;



void settingsSetup()
{
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(TAG, "ERASING FLASH");
        nvs_flash_erase();
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);

    err = nvs_open("storage", NVS_READWRITE, &nvsHandle);

    nvs_get_u16(nvsHandle,"dmxAddr",&dmxAddr);
}

uint16_t settingsGetDmxAddr()
{
    return dmxAddr;
}

void settingsSetDmxAddr(uint16_t newdmxAddr)
{
    dmxAddr=newdmxAddr;
    nvs_set_u16(nvsHandle,"dmxAddr",dmxAddr);
    nvs_commit(nvsHandle);
}
