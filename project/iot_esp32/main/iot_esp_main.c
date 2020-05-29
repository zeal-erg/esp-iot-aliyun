/**
 * @file iot_esp_main.c
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "iot_esp_gpio.h"
#include "iot_esp_type.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "iot_esp_wifi_config.h"
#include "iot_esp_mq.h"

#include "esp_log.h"
#include "iot_esp_cli.h"
#include "iot_esp_kvs.h"
#include "cJSON.h"

struct iot_esp_cofig g_iot_esp_config;
wifi_interface_t g_tmp_wifi_t = ESP_IF_MAX;
wifi_mode_t g_tmp_wifi_interface_t = WIFI_MODE_NULL;

#define CONFIG_ESP_MAXIMUM_RETRY (10)
/* The examples use WiFi configuration that you can set via project configuration menu
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

static const char* TAG = "wifi station";
static int s_retry_num = 0;

wifi_config_t wifi_config;

/**
 * @brief elog_user_assert_hook
 * 
 * @param ex 
 * @param func 
 * @param line 
 */
static void elog_user_assert_hook(const char* ex, const char* func, size_t line) {
#if  0
#ifdef ELOG_ASYNC_OUTPUT_ENABLE
    /* disable async output */
    elog_async_enabled(false);
#endif

    /* disable logger output lock */
    elog_output_lock_enabled(false);
    /* disable flash plugin lock */
    // elog_flash_lock_enabled(false);
    /* output logger assert information */
    elog_a("elog", "(%s) has assert failed at %s:%ld.\n", ex, func, line);
    /* write all buffered log to flash */
    // elog_flash_flush();

    /* never run here */
    while (1) {
        ;
    }
#endif
}

/**
 * @brief iot_esp_log_init
 * 
 */
void iot_esp_log_init(void)
{
#if 0
    /* easylogger init */
    if (elog_init() == ELOG_NO_ERR) {
        /* set enabled format */
        elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
        elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
        elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
        elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
        elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
        elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
        /* set EasyLogger assert hook */
        elog_assert_set_hook(elog_user_assert_hook);
        /* initialize EasyLogger Flash plugin */
        // elog_flash_init();
        /* start EasyLogger */
        elog_start();
    }
#endif
}

/**
 * @brief iot_esp_nvs_flash_init
 * 
 */
void iot_esp_nvs_flash_init(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_LOGI(TAG, "nvs_flash_init %d", ret);
}

/**
 * @brief 
 * 
 * @param ssid 
 * @param password 
 * @return iot_esp_ret 
 */
static iot_esp_ret iot_esp_set_wifi_info(struct iot_esp_cofig *wifi_info)
{
    iot_esp_ret ret = IOT_ESP_OK;
    cJSON* iot_esp_config_root = NULL;
    /* add user ssid and password */
    iot_esp_config_root = cJSON_CreateObject();
    if (NULL == iot_esp_config_root) {
        ESP_LOGI(TAG, "iot_esp_config_root is NULL");
        goto ERR_CJSON;
    }
    char* ssid = (char*)&wifi_info->ssid;
    char* password = (char*)&wifi_info->password;
    char* ap_ssid = (char*)&wifi_info->ap_ssid;
    char* ap_password = (char*)&wifi_info->ap_password;

    cJSON_AddStringToObject(iot_esp_config_root, "ssid", ssid);
    cJSON_AddStringToObject(iot_esp_config_root, "password", password);
    cJSON_AddStringToObject(iot_esp_config_root, "ap_ssid", ap_ssid);
    cJSON_AddStringToObject(iot_esp_config_root, "ap_password", ap_password);
    cJSON_AddNumberToObject(iot_esp_config_root, "wifi_mode", wifi_info->wifi_mode);

    char* print_root = cJSON_PrintUnformatted(iot_esp_config_root);
    if (NULL == print_root) {
        cJSON_Delete(iot_esp_config_root);
        ESP_LOGI(TAG, "print_root is NULL");
        goto ERR_CJSON;
    }
    cJSON_Delete(iot_esp_config_root);
    iot_esp_config_root = NULL;
    ESP_LOGI("iot_esp_main", "%s", print_root);
    /* save wifi info */
    ret = iot_esp_kvs_write(&g_iot_esp_config.handle, "wifi_info", print_root);
    if (IOT_ESP_OK != ret) {
        ESP_LOGI(TAG, "iot_esp_kvs_write fail!");
        return ret;
    }
    free(print_root);
    return IOT_ESP_OK;
ERR_CJSON:
    return IOT_ESP_CJSON_ERROR;
}

/**
 * @brief 
 * 
 * @param wifi_info_get 
 * @return iot_esp_ret 
 */
static iot_esp_ret iot_esp_get_wifi_info(struct iot_esp_cofig *wifi_info_get)
{
    cJSON* iot_esp_config_root = NULL;
    cJSON* json = NULL;
    iot_esp_ret ret = IOT_ESP_OK;

    char* wifi_info = NULL;
    iot_esp_uint32 wifi_info_len = 0xFF;
    wifi_info = (char*)malloc(128);
    if (NULL == wifi_info) {
        ESP_LOGI(TAG, "wifi_info malloc fail!");
        return IOT_ESP_MALLOC_FAILD;
    }

    ret = iot_esp_kvs_read(&wifi_info_get->handle, "wifi_info", wifi_info, &wifi_info_len);
    if (IOT_ESP_OK == ret) {
        ESP_LOGI("wifi_info", "%s", wifi_info);
        iot_esp_config_root = cJSON_Parse(wifi_info);
        if (NULL == iot_esp_config_root) {
            ESP_LOGI(TAG, "cJSON_Parse fail!");
            goto wifi_config_default;
        }
        json = cJSON_GetObjectItem(iot_esp_config_root, "ssid");
        if (NULL != json) {
            memcpy(wifi_info_get->ssid, json->valuestring, strlen(json->valuestring));
        } else {
            ESP_LOGE("wifi_info", "get ssid err!");
            goto wifi_config_default;
        }
        json = cJSON_GetObjectItem(iot_esp_config_root, "password");
        if (NULL != json) {
            memcpy(wifi_info_get->password, json->valuestring, strlen(json->valuestring));
        } else {
            ESP_LOGE("wifi_info", "get password err!");
            goto wifi_config_default;
        }
        json = cJSON_GetObjectItem(iot_esp_config_root, "wifi_mode");
        if (NULL != json) {
            wifi_info_get->wifi_mode = json->valueint;
        } else {
            ESP_LOGE("wifi_info", "get password err!");
            goto wifi_config_default;
        }
        json = cJSON_GetObjectItem(iot_esp_config_root, "ap_ssid");
        if (NULL != json) {
            memcpy(wifi_info_get->ap_ssid, json->valuestring, strlen(json->valuestring));
        } else {
            ESP_LOGE("wifi_info", "get ssid err!");
            goto wifi_config_default;
        }
        json = cJSON_GetObjectItem(iot_esp_config_root, "ap_password");
        if (NULL != json) {
            memcpy(wifi_info_get->ap_password, json->valuestring, strlen(json->valuestring));
        } else {
            ESP_LOGE("wifi_info", "get password err!");
            goto wifi_config_default;
        }
        free(wifi_info);
        cJSON_Delete(iot_esp_config_root);

        ESP_LOGI("iot_esp_main", "ssid:%s password:%s", g_iot_esp_config.ssid, g_iot_esp_config.password);
        return ret;
    }
wifi_config_default:
    if (iot_esp_config_root) {
        cJSON_Delete(iot_esp_config_root);
    }
    if (wifi_info) {
        free(wifi_info);
    }

    return IOT_ESP_CJSON_ERROR;
}

/**
 * @brief iot_esp_nvs_flash_init
 * 
 */
void iot_esp_get_sysinfo(void)
{

    iot_esp_ret ret = IOT_ESP_OK;
    iot_esp_kvs_init();
    iot_esp_kvs_create(&g_iot_esp_config.handle);

    ret = iot_esp_get_wifi_info(&g_iot_esp_config);
    if (IOT_ESP_OK != ret) {
        memcpy(&g_iot_esp_config.ssid, CONFIG_ESP_WIFI_SSID, strlen(CONFIG_ESP_WIFI_SSID));
        memcpy(&g_iot_esp_config.password, CONFIG_ESP_WIFI_PASSWORD, strlen(CONFIG_ESP_WIFI_PASSWORD));
        memcpy(&g_iot_esp_config.ap_ssid, CONFIG_ESP_WIFI_AP_SSID, strlen(CONFIG_ESP_WIFI_AP_SSID));
        memcpy(&g_iot_esp_config.ap_password, CONFIG_ESP_WIFI_AP_PASSWORD, strlen(CONFIG_ESP_WIFI_AP_PASSWORD));

        /* default mode station */
        g_iot_esp_config.wifi_mode = IOT_ESP_WIFI_MODE_STA;
        ret = iot_esp_set_wifi_info(&g_iot_esp_config);
        if (IOT_ESP_OK != ret) {
            ESP_LOGE("set err", "iot_esp_set_wifi_info fail!");
        }
    }
}

/**
 * @brief 
 * 
 */
void iot_esp_wifi_init(struct iot_esp_cofig *wifi_info_get)
{
    // /* common init */
    // // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    // ESP_LOGE(TAG, "mode %d", wifi_info_get->wifi_mode);

    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // if (wifi_info_get->wifi_mode == IOT_ESP_WIFI_MODE_STA) {
    //     esp_netif_create_default_wifi_sta();
    //     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    //     esp_event_handler_instance_t instance_any_id;
    //     esp_event_handler_instance_t instance_got_ip;
    //     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    //                                                         ESP_EVENT_ANY_ID,
    //                                                         &event_handler,
    //                                                         NULL,
    //                                                         &instance_any_id));
    //     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
    //                                                         IP_EVENT_STA_GOT_IP,
    //                                                         &event_handler,
    //                                                         NULL,
    //                                                         &instance_got_ip));

    //     memcpy(&wifi_config.sta.ssid, g_iot_esp_config.ssid, sizeof(g_iot_esp_config.ssid));
    //     memcpy(&wifi_config.sta.password, g_iot_esp_config.password, sizeof(g_iot_esp_config.password));
    //     g_tmp_wifi_interface_t = WIFI_MODE_STA;
    //     g_tmp_wifi_t = ESP_IF_WIFI_STA;
    // } else if (wifi_info_get->wifi_mode == IOT_ESP_WIFI_MODE_AP) {
    //     esp_netif_create_default_wifi_ap();
    //     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    //     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    //                                                         ESP_EVENT_ANY_ID,
    //                                                         &event_handler,
    //                                                         NULL,
    //                                                         NULL));

    //     memcpy(&wifi_config.ap.ssid, g_iot_esp_config.ap_ssid, sizeof(g_iot_esp_config.ap_ssid));
    //     memcpy(&wifi_config.ap.password, g_iot_esp_config.ap_password, sizeof(g_iot_esp_config.ap_password));
    //     wifi_config.ap.ssid_len = strlen((char *)&g_iot_esp_config.ap_ssid);
    //     wifi_config.ap.max_connection = CONFIG_ESP_WIFI_AP_MAXCON;
    //     wifi_config.ap.authmode = (strlen((char *)&g_iot_esp_config.ap_password) == 0)?WIFI_AUTH_OPEN:WIFI_AUTH_WPA_WPA2_PSK;

    //     g_tmp_wifi_interface_t = WIFI_MODE_AP;
    //     g_tmp_wifi_t = ESP_IF_WIFI_AP;
    // }

    // ESP_ERROR_CHECK(esp_wifi_set_mode(g_tmp_wifi_interface_t));
    // ESP_ERROR_CHECK(esp_wifi_set_config(g_tmp_wifi_t, &wifi_config));
    // ESP_ERROR_CHECK(esp_wifi_start());

    // ESP_LOGI(TAG, "wifi_init_%s finished.", (g_tmp_wifi_interface_t == WIFI_MODE_AP) ? "AP" : "STA");
}

/**
 * @brief 
 * 
 * @param argc 
 */
void vTask_iot_esp_entry(void *argc)
{
    ESP_LOGI(TAG, "vTask_iot_esp_entry Start");
    extern void iot_esp_user_main(void);
    iot_esp_user_main();
    vTaskDelete(NULL);
}

/**
 * @brief iot_esp_main
 * 
 */
void iot_esp_main(void) {
    memset(&g_iot_esp_config, 0, sizeof(struct iot_esp_cofig));

    // iot_esp_log_init();
    // iot_esp_nvs_flash_init();
    // iot_esp_get_sysinfo();

    // /* cli task */
    // iot_esp_cli_init();
    // iot_esp_cli_register_cmd();

    // /* wifi event init */
    // iot_esp_wifi_init(&g_iot_esp_config);

    xTaskCreate(vTask_iot_esp_entry, (char*)"vTask_Entry", 2048, NULL, 5, NULL);
    // iot_esp_mq_init();
}
