/**
 * @file iot_esp_cli.h
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-14
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __IOT_ESP_CLI_H__
#define __IOT_ESP_CLI_H__
#include "esp_console.h"
#include "iot_esp_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOT_ESP_CLI_MAX (1)
#define STATIC_CLI_CMD_ATTRIBUTE __attribute__((used, section(".static_cli_cmds")))

struct iot_esp_cli_status {
    int num_static_cmds;
    int num_commands;
    esp_console_cmd_t *static_cli_cmds;
};

/**IOT_ESP_CLI_MANG
 * @brief iot_esp_cli_init
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_cli_init(void);

/**
 * @brief iot_esp_cli_register_cmd
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_cli_register_cmd(void);

#ifdef __cplusplus
}
#endif

#endif