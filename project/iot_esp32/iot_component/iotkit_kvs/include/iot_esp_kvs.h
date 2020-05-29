/**
 * @file iot_esp_kvs.h
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __IOT_ESP_KVS_H__
#define __IOT_ESP_KVS_H__

#include <stdio.h>
#include <string.h>
#include "iot_esp_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef iot_esp_uint32 iot_esp_kvs_handle;

/**
 * @brief iot_esp_kvs_init get partition info
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_init(void);
/**
 * @brief 
 * 
 * @param handle 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_create(iot_esp_kvs_handle *handle);

/**
 * @brief 
 * 
 * @param handle 
 * @param key 
 * @param buff 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_write(iot_esp_kvs_handle *handle, char *key, char *buff);
/**
 * @brief 
 * 
 * @param handle 
 * @param key 
 * @param buff 
 * @param len 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_kvs_read(iot_esp_kvs_handle *handle, char *key, char *buff, iot_esp_uint32 *len);
#ifdef __cplusplus
}
#endif

#endif

