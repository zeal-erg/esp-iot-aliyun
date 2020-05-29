/**
 * @file iot_esp_mq.h
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __IOT_ESP_MQ_H__
#define __IOT_ESP_MQ_H__

#include <stdio.h>
#include <string.h>
#include "iot_esp_type.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef iot_esp_uint32 iot_esp_mq_handle;

/**
 * @brief 
 * 
 */
enum iot_esp_mq_seq {
    MQTT_DEFULT = 0,
    MQTT_MAX,
};

/* mqtt api */
typedef iot_esp_ret (*iot_esp_mq_protocol_handler_cb)(cJSON *root_json);

/**
 * @brief iot_esp_mq_register_cb
 * 
 * @param mq_pro 
 * @param handler 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_mq_register_cb(iot_esp_uint16 mq_pro, iot_esp_mq_protocol_handler_cb handler);

typedef void (*iot_mqc_publish_result_cb)(iot_esp_ret ret, void *prv_data);

iot_esp_ret iot_esp_mq_send_custom_msg(iot_esp_uint32 protocol, const iot_esp_uint8 *data, iot_esp_uint8 qos,
                                       iot_esp_uint32 to_lmt, const iot_mqc_publish_result_cb cb, void *prv_data);

/**
 * @brief iot_esp_mq_init get partition info
 * 
 * @return iot_esp_ret 
 */
void iot_esp_mq_init(void);

#ifdef __cplusplus
}
#endif

#endif

