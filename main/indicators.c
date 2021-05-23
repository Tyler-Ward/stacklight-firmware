
#include "indicators.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>

#include "hardware.h"

static bool indicatorLocateSet=0;

static bool inidcatorArtnetTimed=0;
esp_timer_handle_t artnetIndicatorTimer;
static int64_t artnetIndicatorTimeout;

static void indicatorsArtnetLEDTimeout(void* arg);

void indicatorsSetup()
{
    gpio_config_t output_conf;

    output_conf.intr_type = GPIO_INTR_DISABLE;
    output_conf.mode = GPIO_MODE_OUTPUT;
    output_conf.pin_bit_mask = 0;
    output_conf.pin_bit_mask |= ((1ULL<<GPIO_LED_ENABLE));
    output_conf.pin_bit_mask |= ((1ULL<<GPIO_LED_ARTNET));
    output_conf.pin_bit_mask |= ((1ULL<<GPIO_LED_LOCATE));
    output_conf.pin_bit_mask |= ((1ULL<<GPIO_LED_NETWORK));
    output_conf.pin_bit_mask |= ((1ULL<<GPIO_LED_STATUS));
    output_conf.pull_down_en = 0;
    output_conf.pull_up_en = 0;
    gpio_config(&output_conf);

    indicatorLocateSet=0;

    gpio_set_level(GPIO_LED_ENABLE,1);

    const esp_timer_create_args_t artnetIndicatorTimerArgs = {
        .callback = &indicatorsArtnetLEDTimeout,
        /* name is optional, but may help identify the timer when debugging */
        .name = "artnetLED"
    };
    ESP_ERROR_CHECK(esp_timer_create(&artnetIndicatorTimerArgs, &artnetIndicatorTimer));
    esp_timer_start_periodic(artnetIndicatorTimer, 500000);
}

static void indicatorsArtnetLEDTimeout(void* arg)
{
    if (inidcatorArtnetTimed)
    {
        int64_t time_since_boot = esp_timer_get_time();
        if(time_since_boot>artnetIndicatorTimeout)
        {
            indicatorsSetArtnet(0);
            inidcatorArtnetTimed=0;
        }
    }
}

void indicatorsSetArtnetTimed(uint32_t timeus)
{
    inidcatorArtnetTimed=1;
    artnetIndicatorTimeout=esp_timer_get_time()+timeus;
    gpio_set_level(GPIO_LED_ARTNET,1);
}

void indicatorsSetArtnet(bool iluminated)
{
    gpio_set_level(GPIO_LED_ARTNET,iluminated);
}

void indicatorsSetLocate(bool iluminated)
{
    gpio_set_level(GPIO_LED_LOCATE,iluminated);
    indicatorLocateSet=iluminated;
}

bool indicatorsGetLocate()
{
    return indicatorLocateSet;
}

void indicatorsSetNetwork(bool iluminated)
{
    gpio_set_level(GPIO_LED_NETWORK,iluminated);
}

void indicatorsSetStatus(bool iluminated)
{
    gpio_set_level(GPIO_LED_STATUS,iluminated);
}