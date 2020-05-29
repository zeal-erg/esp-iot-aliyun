/**
 * @file iot_esp_gpio.h
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __IOT_ESP_GPIO_H__
#define __IOT_ESP_GPIO_H__

#include "driver/gpio.h"
#include "iot_esp_type.h"

extern void iot_esp_assert(const char *_file, int _line, const char *_func);
#ifdef IOT_ESP_ASSERT
// #define IOT_ESP_GPIO_ASSERT(n) {if (!(n)) iot_esp_assert(__FILE__, __LINE__, __func__);}
#define IOT_ESP_GPIO_ASSERT(cond)                                   \
    do {                                                            \
        if (!(cond)) {                                              \
            printf("[ASSERT] [ERR] %s:%d\r\n", __FILENAME__, __LINE__); \
            while (1) {                                             \
            }                                                       \
        }                                                           \
    } while (0)
#else
#define IOT_ESP_GPIO_ASSERT(n)
#endif

typedef enum {
    IOT_ESP_GPIO_0 = 0,
    IOT_ESP_GPIO_1,
    IOT_ESP_GPIO_2,
    IOT_ESP_GPIO_3,
    IOT_ESP_GPIO_4,
    IOT_ESP_GPIO_5,
    IOT_ESP_GPIO_6,
    IOT_ESP_GPIO_7,
    IOT_ESP_GPIO_8,
    IOT_ESP_GPIO_9,
    IOT_ESP_GPIO_10,
    IOT_ESP_GPIO_11,
    IOT_ESP_GPIO_12,
    IOT_ESP_GPIO_13,
    IOT_ESP_GPIO_14,
    IOT_ESP_GPIO_15,
    IOT_ESP_GPIO_16,
    IOT_ESP_GPIO_17,
    IOT_ESP_GPIO_18,
    IOT_ESP_GPIO_19,
    IOT_ESP_GPIO_20,
    IOT_ESP_GPIO_21,
    IOT_ESP_GPIO_22,
    IOT_ESP_GPIO_23,
    IOT_ESP_GPIO_24,
    IOT_ESP_GPIO_25,
    IOT_ESP_GPIO_26,
    IOT_ESP_GPIO_27,
    IOT_ESP_GPIO_28,
    IOT_ESP_GPIO_29,
    IOT_ESP_GPIO_30,
    IOT_ESP_GPIO_31,
    IOT_ESP_GPIO_32,
    IOT_ESP_GPIO_33,
    //follow only support input mode
    IOT_ESP_GPIO_34 = 34,
    IOT_ESP_GPIO_35,
    IOT_ESP_GPIO_36,
    IOT_ESP_GPIO_37,
    IOT_ESP_GPIO_38,
    IOT_ESP_GPIO_39,
    IOT_ESP_GPIO_MAX,
} iot_esp_gpio_port;

typedef enum {
    IOT_ESP_GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                                         /*!< GPIO mode : disable input and output             */
    IOT_ESP_GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                             /*!< GPIO mode : input only                           */
    IOT_ESP_GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                           /*!< GPIO mode : output only mode                     */
    IOT_ESP_GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)),                               /*!< GPIO mode : output only with open-drain mode     */
    IOT_ESP_GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)), /*!< GPIO mode : output and input with open-drain mode*/
    IOT_ESP_GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),                         /*!< GPIO mode : output and input mode                */
} iot_esp_gpio_mode_t;

typedef enum {
    IOT_ESP_GPIO_PULLUP_ONLY,     /*!< Pad pull up            */
    IOT_ESP_GPIO_PULLDOWN_ONLY,   /*!< Pad pull down          */
    IOT_ESP_GPIO_PULLUP_PULLDOWN, /*!< Pad pull up + pull down*/
    IOT_ESP_GPIO_FLOATING,        /*!< Pad floating           */
} iot_esp_gpio_pull_mode_t;

typedef enum {
    IOT_ESP_GPIO_DRIVE_CAP_0 = 0,       /*!< Pad drive capability: weak          */
    IOT_ESP_GPIO_DRIVE_CAP_1 = 1,       /*!< Pad drive capability: stronger      */
    IOT_ESP_GPIO_DRIVE_CAP_2 = 2,       /*!< Pad drive capability: medium */
    IOT_ESP_GPIO_DRIVE_CAP_DEFAULT = 2, /*!< Pad drive capability: medium */
    IOT_ESP_GPIO_DRIVE_CAP_3 = 3,       /*!< Pad drive capability: strongest     */
    IOT_ESP_GPIO_DRIVE_CAP_MAX,
} iot_esp_gpio_drive_cap_t;

typedef enum {
    IOT_ESP_GPIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    IOT_ESP_GPIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    IOT_ESP_GPIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    IOT_ESP_GPIO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
    IOT_ESP_GPIO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
    IOT_ESP_GPIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
    IOT_ESP_GPIO_INTR_MAX,
} iot_esp_gpio_int_type_t;

typedef void (*gpio_isr_callback_t)(void *);

/**
 * @brief gpio init
 * 
 * @param port 
 * @param gpoi_mode 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_inout_set(iot_esp_gpio_port port, iot_esp_gpio_mode_t gpoi_mode, iot_esp_gpio_pull_mode_t pull_mode);

/**
 * @brief 
 * 
 * @param port 
 * @param value 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_write(iot_esp_gpio_port port, bool value);

/**
 * @brief 
 * 
 * @param port 
 * @return true 
 * @return false 
 */
bool iot_esp_gpio_read(iot_esp_gpio_port port);

/**
 * @brief 
 * 
 * @param port 
 * @param callback 
 * @param intr_type 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_intr_set(iot_esp_gpio_port port, gpio_isr_callback_t callback, iot_esp_gpio_int_type_t intr_type);

/**
 * @brief 
 * 
 * @param port 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_intr_enable(iot_esp_gpio_port port);

/**
 * @brief 
 * 
 * @param port 
 * 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_intr_disable(iot_esp_gpio_port port);

/**
 * @brief Enable GPIO wake-up function.
 *
 * @param gpio_num GPIO number.
 *
 * @param intr_type GPIO wake-up type. Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */

/**
 * @brief 
 * 
 * @param port 
 * @param intr_type 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_wakeup_enable(iot_esp_gpio_port port, iot_esp_gpio_int_type_t intr_type);

/**
 * @brief 
 * 
 * @param gpio_num 
 * @return esp_err_t 
 */
iot_esp_ret iot_esp_gpio_wakeup_disable(iot_esp_gpio_port port);

#endif
