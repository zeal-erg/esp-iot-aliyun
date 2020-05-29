/**
 * @file iot_esp32_type.h
 * @author
 * @brief 
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __IOT_ESP_TYPE_H__
#define __IOT_ESP_TYPE_H__

#include "stdint.h"
/* assert enable */
#define IOT_ESP_ASSERT

#define iot_esp_uint8 unsigned char
#define iot_esp_int8 signed char

#define iot_esp_uint16 unsigned short
#define iot_esp_int16 signed short

#define iot_esp_uint32 unsigned int
#define iot_esp_int32 signed int

/* iot - esp error code */
typedef iot_esp_uint16 iot_esp_ret;

#define IOT_ESP_OK                          (0)
#define IOT_ESP_COM_ERROR                   (-1)
#define IOT_ESP_INVALID_PARAM               (-2)
#define IOT_ESP_MALLOC_FAILD                (-3)
#define IOT_ESP_NOT_SUPPORTED               (-4)
#define IOT_ESP_NETWORK_ERROR               (-5)
#define IOT_ESP_KVS_FLASH_ERROR             (-6)
#define IOT_ESP_KVS_FLASH_KEY_ERROR         (-7)
#define IOT_ESP_CJSON_ERROR                 (-20)

#endif
