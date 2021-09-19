
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include <string.h>

#include "esp_log.h"

#include "hardware.h"

static const char *TAG = "output";

ledc_channel_config_t ledc_channel[4] = {
    {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,
        .gpio_num   = (GPIO_OUTPUT_RED),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
    {
        .channel    = LEDC_CHANNEL_1,
        .duty       = 0,
        .gpio_num   = (GPIO_OUTPUT_YELLOW),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
    {
        .channel    = LEDC_CHANNEL_2,
        .duty       = 0,
        .gpio_num   = (GPIO_OUTPUT_GREEN),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
    {
        .channel    = LEDC_CHANNEL_3,
        .duty       = 0,
        .gpio_num   = (GPIO_OUTPUT_BUZZER),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
};

void SetOutputsDMX(uint16_t offset, uint8_t* data)
{
    ledc_set_duty_and_update(ledc_channel[0].speed_mode,ledc_channel[0].channel,data[offset]*32,0);
    ledc_set_duty_and_update(ledc_channel[1].speed_mode,ledc_channel[1].channel,data[offset+1]*32,0);
    ledc_set_duty_and_update(ledc_channel[2].speed_mode,ledc_channel[2].channel,data[offset+2]*32,0);
    ledc_set_duty_and_update(ledc_channel[3].speed_mode,ledc_channel[3].channel,data[offset+3]*32,0);
}

typedef struct
{
    const char *modeName;
    int red;
    int yellow;
    int green;
    int buzzer;
} out_mode_t;

static out_mode_t modes[] = {
    {"off", 0, 0, 0, 0},
    {"buzzer", 0, 0, 0, 255*32},
    {"all", 255*32, 255*32, 255*32, 0},
    {"red", 255*32, 0, 0, 0},
    {"yellow",0, 255*32, 0, 0},
    {"green", 0, 0, 255*32, 0},
    {"red_buzzer", 255*32, 0, 0, 255*32},
    {"yellow_buzzer",0, 255*32, 0, 255*32},
    {"green_buzzer", 0, 0, 255*32, 255*32},
    {0x00,0,0,0}
};

void SetOutputsMode(char* mode)
{
    int i = 0;
    while(modes[i].modeName)
    {
        if(strcmp(modes[i].modeName,mode)==0)
        {
            ledc_set_duty_and_update(ledc_channel[0].speed_mode,ledc_channel[0].channel,modes[i].red,0);
            ledc_set_duty_and_update(ledc_channel[1].speed_mode,ledc_channel[1].channel,modes[i].yellow,0);
            ledc_set_duty_and_update(ledc_channel[2].speed_mode,ledc_channel[2].channel,modes[i].green,0);
            ledc_set_duty_and_update(ledc_channel[3].speed_mode,ledc_channel[3].channel,modes[i].buzzer,0);
            return;
        }
        i++;
    }
    ESP_LOGW(TAG, "No supported mode: %s", mode);
}

void SetupOutputs()
{

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ledc_timer_config(&ledc_timer);



    ledc_channel_config(&ledc_channel[0]);
    ledc_channel_config(&ledc_channel[1]);
    ledc_channel_config(&ledc_channel[2]);
    ledc_channel_config(&ledc_channel[3]);

    ledc_fade_func_install(0);
}