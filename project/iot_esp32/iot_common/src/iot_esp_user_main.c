/**
 * @file iot_esp_main.c
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdio.h>

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "esp_log.h"
#include "iot_esp_type.h"

#define IOT_ESP_MAIN_TAG "iot_esp_main"

extern void pre_app_init(void);
extern iot_esp_ret device_init(void);

/**
 * @brief 
 * 
 */
void iot_esp_user_main(void) {
    iot_esp_ret ret = IOT_ESP_OK;
    printf("%s_V%s\r\n", APP_BIN_NAME, USER_SW_VER);
    printf("iot_esp firmware compiled at %s %s\r\n", __DATE__, __TIME__);

    /*  */
    pre_app_init();

    /*  */
    ret = device_init();
    if (IOT_ESP_OK != ret) {
        ESP_LOGE(IOT_ESP_MAIN_TAG, "device_init fail %d", ret);
        return;
    }

}