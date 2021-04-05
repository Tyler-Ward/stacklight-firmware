
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

ledc_channel_config_t ledc_channel[4] = {
    {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,
        .gpio_num   = (33),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
    {
        .channel    = LEDC_CHANNEL_1,
        .duty       = 0,
        .gpio_num   = (13),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
    {
        .channel    = LEDC_CHANNEL_2,
        .duty       = 0,
        .gpio_num   = (32),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    },
    {
        .channel    = LEDC_CHANNEL_3,
        .duty       = 0,
        .gpio_num   = (14),
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

void SetupOutputs()
{
    //light test
    int ch;

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
    /*
    while(1){

        for (ch = 0; ch < 4; ch++) {
            ledc_set_fade_with_time(ledc_channel[ch].speed_mode,ledc_channel[ch].channel, 8191, 3000);
            ledc_fade_start(ledc_channel[ch].speed_mode,ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
            vTaskDelay(3000 / portTICK_PERIOD_MS);

            ledc_set_fade_with_time(ledc_channel[ch].speed_mode,ledc_channel[ch].channel, 0, 3000);
            ledc_fade_start(ledc_channel[ch].speed_mode,ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);

            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
    }
    */
}