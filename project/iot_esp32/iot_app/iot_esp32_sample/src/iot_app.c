/**
 * @file iot_app.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "iot_esp_gpio.h"
#include "iot_esp_type.h"

/**
 * @brief 
 * 
 */
void pre_app_init(void)
{
}

/**
 * @brief 
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret device_init(void)
{
    return IOT_ESP_OK;
}