
#include "indicators.h"

#include <driver/gpio.h>
#include <esp_log.h>

#include "hardware.h"

static bool indicatorLocateSet=0;


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