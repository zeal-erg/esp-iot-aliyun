/**
 * @file iot_esp_wifi_config.h
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __IOT_ESP_WIFI_CONFIG_H__
#define __IOT_ESP_WIFI_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "iot_esp_kvs.h"
#include "iot_esp_type.h"

typedef enum {
    IOT_ESP_WIFI_MODE_NULL = 0, /**< null mode */
    IOT_ESP_WIFI_MODE_STA,      /**< WiFi station mode */
    IOT_ESP_WIFI_MODE_AP,       /**< WiFi soft-AP mode */
    IOT_ESP_WIFI_MODE_APSTA,    /**< WiFi station + soft-AP mode */
    IOT_ESP_WIFI_MODE_MAX
} iot_esp_wifi_mode_t;

struct iot_esp_cofig {
    iot_esp_uint8 ssid[32];     /**< SSID of target AP. Null terminated string. */
    iot_esp_uint8 password[64]; /**< Password of target AP. Null terminated string.*/
    iot_esp_uint8 ap_ssid[32];     /**< SSID of target AP. Null terminated string. */
    iot_esp_uint8 ap_password[64]; /**< Password of target AP. Null terminated string.*/
    iot_esp_wifi_mode_t wifi_mode;

    iot_esp_kvs_handle handle;
};

/* sys dubug */
#define CONFIG_ESP_WIFI_SSID        "ziroom_504"        /* test wifi ssid */
#define CONFIG_ESP_WIFI_PASSWORD    "4001001111"        /* test wifi password */

#define CONFIG_ESP_WIFI_AP_SSID     "geek_"             /* test wifi ap ssid */
#define CONFIG_ESP_WIFI_AP_PASSWORD "12345678"          /* test wifi ap password */

#define CONFIG_ESP_WIFI_AP_MAXCON   (2)

#define IOT_ESP_WIFI_INIT_MODE IOT_ESP_WIFI_MODE_STA

/**
 * @brief 
 * 
 */
#define IOT_ESP_WIFI_MQTT_PRIORITY      (10)



#ifdef __cplusplus
}
#endif

#endif
