/**
 * @file main.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 2020-04-03
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

/**
 * @brief iot_app_init_thread
 * 
 * @param param 
 */
static void iot_app_init_thread(void *param)
{
    extern void iot_esp_main(void);
    iot_esp_main();

    /* Kill init thread after all init tasks done */
    vTaskDelete(NULL);
}

/**
 * @brief iot_app_init_entry
 * 
 */
void iot_app_init_entry(void)
{
    if(xTaskCreate(iot_app_init_thread,"iot_app_init", 4096, NULL, tskIDLE_PRIORITY + 3, NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(iot_app_init_thread) failed", __FUNCTION__);
}



/**
 * @brief 
 * 
 */
void app_main(void) {
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("\r\nsystem reset type %d\r\n", esp_reset_reason());
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
           "esp32",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    printf("silicon revision %d, ", chip_info.revision);
    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    printf("Free heap: %d\n", esp_get_free_heap_size());

    iot_app_init_entry();
}
