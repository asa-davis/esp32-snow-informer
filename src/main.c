#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>

#include "snow_api.h"
#include "display.h"

struct snowData snowDataCurr = {.depth = 0, .fresh = 0};

const int updateIntervalHours = 4;

// every hour, get the snow depth and fresh snow from the api
void get_depth_and_fresh(void *pvParams) 
{
    int i = updateIntervalHours;

    while (1)
    {
        if(i == updateIntervalHours) {
            struct snowData snowDataNew = get_snow_data();
            if (snowDataNew.depth > -1) snowDataCurr = snowDataNew;
            i = 0;
        }
        vTaskDelay(1000 * 60 * 60 / portTICK_PERIOD_MS);
        i++;
    }
}

// every 2 seconds, switch between displaying snow depth and fresh snow
void display_depth_and_fresh(void *pvParams) 
{
    while (1) 
    {
        printf("DEPTH: %din\n", snowDataCurr.depth);
        set_display(snowDataCurr.depth);
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        printf("FRESH: %din\n", snowDataCurr.fresh);
        set_display(snowDataCurr.fresh);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// every 10ms, tick the display to switch cycles
void tick(void *pvParams)
{
    while (1) {
        display_tick();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    nvs_flash_init();
    display_init();
    //set_display_test();
    snow_api_init();

    xTaskCreate(&display_depth_and_fresh, "DISPLAY_DEPTH_AND_FRESH", 2048, NULL, 5, NULL);
    xTaskCreate(&get_depth_and_fresh, "GET_DEPTH_AND_FRESH", 2048 * 2, NULL, 5, NULL);
    xTaskCreate(&tick, "TICK", 2048, NULL, 5, NULL);
}