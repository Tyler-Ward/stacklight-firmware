#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>

#include "settings.h"

static const char *TAG = "settings";

nvs_handle_t nvsHandle;

//variables to store cached values
static uint16_t dmxAddr = 1;
static uint8_t artnetNet = 0;
static uint8_t artnetSubNet = 0;
static uint8_t artnetUniverse = 0;

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
    nvs_get_u8(nvsHandle,"artnetNet",&artnetNet);
    nvs_get_u8(nvsHandle,"artnetSubNet",&artnetSubNet);
    nvs_get_u8(nvsHandle,"artnetUniverse",&artnetUniverse);
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

uint8_t settingsGetArtnetNet()
{
    return artnetNet;
}

void settingsSetArtnetNet(uint8_t newArtnetNet)
{
    artnetNet=newArtnetNet;
    nvs_set_u8(nvsHandle,"artnetNet",artnetNet);
    nvs_commit(nvsHandle);
}

uint8_t settingsGetArtnetSubNet()
{
    return artnetSubNet;
}

void settingsSetArtnetSubNet(uint8_t newArtnetSubNet)
{
    artnetSubNet=newArtnetSubNet;
    nvs_set_u8(nvsHandle,"artnetSubNet",artnetSubNet);
    nvs_commit(nvsHandle);
}

uint8_t settingsGetArtnetUniverse()
{
    return artnetUniverse;
}

void settingsSetArtnetUniverse(uint8_t newArtnetUniverse)
{
    artnetUniverse=newArtnetUniverse;
    nvs_set_u8(nvsHandle,"artnetUniverse",artnetUniverse);
    nvs_commit(nvsHandle);
}
