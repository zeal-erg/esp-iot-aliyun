/**
 * @file iot_esp_cli.c
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-14
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <string.h>
#include "iot_esp_cli.h"

#include "esp_log.h"
#include "esp_system.h"

#define IOT_ESP_CLI_TAG "IOT_ESP_CLI"

#include "iot_esp_cli_cmd.c"

static struct iot_esp_cli_status cli_info;
const esp_console_cmd_t built_ins[] STATIC_CLI_CMD_ATTRIBUTE;

/**
 * @brief iot_esp_cli_init
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_cli_init(void)
{
#ifdef IDF_V4_2
    esp_err_t ret = ESP_OK;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    ret = esp_console_repl_init(&repl_config);
    if (ESP_OK != ret) {
        ESP_LOGE(IOT_ESP_CLI_TAG, "esp_console_repl_init fail 0x%x", ret);
        return IOT_ESP_INVALID_PARAM;
    }
    ret = esp_console_repl_start();
    if (ESP_OK != ret) {
        ESP_LOGE(IOT_ESP_CLI_TAG, "esp_console_repl_start fail 0x%x", ret);
        return IOT_ESP_INVALID_PARAM;
    }
#if 0
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    ret = esp_console_init(&console_config);
    if (ESP_OK != ret) {
        ESP_LOGE(IOT_ESP_CLI_TAG, "esp_console_init fail 0x%x", ret);
    }
#endif
#endif
    memset(&cli_info,0,sizeof(struct iot_esp_cli_status));

    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_cli_register_default
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_cli_register_default(void)
{
    esp_err_t ret = ESP_OK;
    const esp_console_cmd_t cmd = {
        .command = "hello",
        .help = "Print Hello World",
        .hint = NULL,
        .func = iot_esp_cli_demo,
    };
    ret = esp_console_cmd_register(&cmd);
    if (ESP_OK != ret) {
        ESP_LOGE(IOT_ESP_CLI_TAG, "esp_console_cmd_register fail 0x%x", ret);
        return IOT_ESP_INVALID_PARAM;
    }
    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_cli_register_cmd
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_cli_register_cmd(void)
{
    esp_err_t ret = ESP_OK;
    int num_static_cmds,num_commands = 0x00;

    esp_console_cmd_t *static_cmds = NULL;
    extern char _iot_esp_static_cli_cmds_start, _iot_esp_static_cli_cmds_end;

    static_cmds = (esp_console_cmd_t *)&_iot_esp_static_cli_cmds_start;
    num_static_cmds = (esp_console_cmd_t *)&_iot_esp_static_cli_cmds_end -
                           (esp_console_cmd_t *)&_iot_esp_static_cli_cmds_start;
    num_commands = num_static_cmds;
    /* register cmd */
    int i = 0x00;
    for (; i < num_commands; static_cmds++,i++) {
        ret = esp_console_cmd_register(static_cmds);
        if (ESP_OK != ret) {
            ESP_LOGE(IOT_ESP_CLI_TAG, "esp_console_cmd_register fail 0x%x", ret);
            return IOT_ESP_INVALID_PARAM;
        }
    }
    return IOT_ESP_OK;
}

const esp_console_cmd_t built_ins[] STATIC_CLI_CMD_ATTRIBUTE = {
    /*cli self*/
    {"hello", "hello word", NULL, iot_esp_cli_demo, NULL},
    {"reboot", "reboot system", NULL, iot_esp_restart, NULL},
    {"sysver", "system version", NULL, iot_esp_sysver, NULL},
    {"p", "print memory", NULL, iot_esp_pmem_cmd, NULL},
    {"free", "print memory", NULL, iot_esp_sysfree_heap_size, NULL},
    {"ps", "task list", NULL, iot_esp_ps_cmd,NULL},
    {"getmac", "device mac", NULL, iot_esp_get_mac_cmd, NULL},
    {"scan", "wifi scan", NULL, iot_esp_wifi_scan_cmd, NULL}

};