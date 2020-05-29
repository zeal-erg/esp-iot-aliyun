/**
 * @file iot_esp_nvs.c
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "iot_esp_kvs.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "iot_esp_type.h"

#include "nvs_flash.h"
#include "iot_esp_list.h"

#define IOT_ESP_KVS_TAG     ""
/* KVS_NAME */
#define IOT_ESP_KVS_NAME    "iot_esp"

struct kvs_status {
    nvs_handle *handle;
    bool is_init;
    iot_esp_uint8 err_code;
    iot_esp_uint16 max_len;

    struct iot_esp_list_head list;
};

/* can write and read size 4K */
#define IOT_ESP_KVS_MAX_LEN (0x1000)

static nvs_handle g_local_handle;
struct kvs_status iot_esp_kvs_status;

/**
 * @brief iot_esp_kvs_init get partition info
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_init(void)
{
    esp_err_t ret = ESP_OK;

    memset(&iot_esp_kvs_status, 0, sizeof(struct kvs_status));
    INIT_LIST_HEAD(&iot_esp_kvs_status.list);

    ret = nvs_open(IOT_ESP_KVS_NAME, NVS_READWRITE, &g_local_handle);
    if (ESP_OK != ret) {
        ESP_LOGI(IOT_ESP_KVS_TAG, "nvs_open fail %d", ret);
        return IOT_ESP_KVS_FLASH_ERROR;
    }
    ESP_LOGI(IOT_ESP_KVS_TAG, "nvs_open handle 0x%x", g_local_handle);

    iot_esp_kvs_status.handle = g_local_handle;
    return IOT_ESP_OK;
}

/**
 * @brief 
 * 
 * @param handle 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_create(iot_esp_kvs_handle *handle)
{
    struct kvs_status *tmp_list = NULL;
    tmp_list = (struct kvs_status *)malloc(sizeof(struct kvs_status));
    if (NULL == tmp_list) {
        ESP_LOGI(IOT_ESP_KVS_TAG, "malloc fail!");
        return IOT_ESP_MALLOC_FAILD;
    }
    tmp_list->handle = g_local_handle;
    tmp_list->is_init = true;
    tmp_list->max_len = IOT_ESP_KVS_MAX_LEN;
    INIT_LIST_HEAD(&tmp_list->list);

    iot_esp_list_add_tail(&tmp_list->list, &iot_esp_kvs_status.list);
    *handle = (iot_esp_kvs_handle *)tmp_list;
    ESP_LOGI(IOT_ESP_KVS_TAG, "%p", handle);
    return IOT_ESP_OK;
}

/**
 * @brief 
 * 
 * @param handle 
 * @param key 
 * @param buff 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_write(iot_esp_kvs_handle *handle,char *key,char *buff)
{
    esp_err_t ret = ESP_OK;
    if (NULL == handle) {
        ESP_LOGI(IOT_ESP_KVS_TAG, "error handle!");
        return IOT_ESP_INVALID_PARAM;
    }
    struct kvs_status *tmp_list = (struct kvs_status *)handle;
    ret = nvs_set_str(*tmp_list->handle, key, buff);
    if (ESP_OK != ret) {
        ESP_LOGI(IOT_ESP_KVS_TAG, "nvs_set_str fail 0x%x", ret);
        return IOT_ESP_KVS_FLASH_ERROR;
    }
    return IOT_ESP_OK;
}

/**
 * @brief 
 * 
 * @param handle 
 * @param key 
 * @param buff 
 * @param len 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_read(iot_esp_kvs_handle *handle,char *key,char *buff,iot_esp_uint32 *len)
{
    esp_err_t ret = ESP_OK;
    if (NULL == handle) {
        ESP_LOGI(IOT_ESP_KVS_TAG, "error handle!");
        return IOT_ESP_INVALID_PARAM;
    }
    struct kvs_status *tmp_list = (struct kvs_status *)handle;
    ret = nvs_get_str(*tmp_list->handle, key, buff, len);
    if (ESP_OK != ret) {
        ESP_LOGI(IOT_ESP_KVS_TAG, "nvs_get_str fail 0x%x", ret);
        return IOT_ESP_KVS_FLASH_ERROR;
    }
    return IOT_ESP_OK;
}

/**
 * @brief 
 * 
 * @param handle 
 * @param key 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_free(iot_esp_kvs_handle *handle,char *key)
{
    return IOT_ESP_OK;
}

/**
 * @brief 
 * 
 * @param handle 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_delete(iot_esp_kvs_handle *handle)
{
    return IOT_ESP_OK;
}

