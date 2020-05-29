/**
 * @file iot_esp_gpio.c
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "iot_esp_gpio.h"

/**
 * @brief 
 * 
 * @param num 
 * @return gpio_config_t 
 */
static gpio_config_t iot_esp_gpio_init_io(gpio_num_t num) 
{
    IOT_ESP_GPIO_ASSERT(num < IOT_ESP_GPIO_MAX);
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    return io_conf;
}

/**
 * @brief gpio init
 * 
 * @param port 
 * @param gpoi_mode 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_inout_set(iot_esp_gpio_port port, iot_esp_gpio_mode_t gpoi_mode, iot_esp_gpio_pull_mode_t pull_mode)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }
    /* gpio_config default */
    gpio_config_t io_conf = iot_esp_gpio_init_io(port);
    io_conf.mode = (gpio_mode_t)gpoi_mode;

    if (pull_mode == IOT_ESP_GPIO_PULLUP_ONLY) {
        io_conf.pull_up_en = true;
    } else
    if (pull_mode == IOT_ESP_GPIO_PULLDOWN_ONLY) {
        io_conf.pull_down_en = true;
    } else
    if (pull_mode == IOT_ESP_GPIO_PULLUP_PULLDOWN) {
        io_conf.pull_up_en = true;
        io_conf.pull_down_en = true;
    }

    gpio_config(&io_conf);
    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_gpio_write
 * 
 * @param port gpio_port
 *
 * @param value true/false
 *
 * @return iot_esp_ret
 *          - IOT_ESP_OK
 *          - IOT_ESP_INVALID_PARAM
 */
iot_esp_ret iot_esp_gpio_write(iot_esp_gpio_port port, bool value)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }
    gpio_set_level(port, value);
    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_gpio_read
 * 
 * @param port gpio_port
 * @return true 
 * @return false 
 */
bool iot_esp_gpio_read(iot_esp_gpio_port port)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }
    int level = gpio_get_level(port);
    return (level == 1)?true:false;
}

/**
 * @brief iot_esp_gpio_intr_set
 * 
 * @param port 
 * @param callback 
 * @param intr_type 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_intr_set(iot_esp_gpio_port port, gpio_isr_callback_t callback, iot_esp_gpio_int_type_t intr_type)
{
    static bool flag_isr_service = false;
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }

    /* gpio_config default */
    gpio_config_t io_conf = iot_esp_gpio_init_io(port);
    io_conf.intr_type = (gpio_int_type_t)intr_type;
    gpio_config(&io_conf);

    if (!flag_isr_service) {
        flag_isr_service = true;
        gpio_install_isr_service(0);
    }

    gpio_isr_handler_add(port, callback, (void*)port);

    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_gpio_intr_enable
 * 
 * @param port gpio_port
 * @return iot_esp_ret 
 *          - IOT_ESP_INVALID_PARAM
 *          - IOT_ESP_OK
 */
iot_esp_ret iot_esp_gpio_intr_enable(iot_esp_gpio_port port)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }

    gpio_intr_enable(port);
    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_gpio_intr_disable
 * 
 * @param port gpio_port
 * @return iot_esp_ret
 *          - IOT_ESP_INVALID_PARAM
 *          - IOT_ESP_OK
 */
iot_esp_ret iot_esp_gpio_intr_disable(iot_esp_gpio_port port)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }

    gpio_intr_disable(port);
    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_gpio_wakeup_enable
 * 
 * @param port gpio_port
 * @param intr_type 
 * @return iot_esp_ret 
 */
iot_esp_ret iot_esp_gpio_wakeup_enable(iot_esp_gpio_port port, iot_esp_gpio_int_type_t intr_type)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }
    /* gpio_config default */
    int ret = gpio_wakeup_enable(port, (gpio_int_type_t)intr_type);
    if (ret != 0) {
        return IOT_ESP_COM_ERROR;
    }
    return IOT_ESP_OK;
}

/**
 * @brief iot_esp_gpio_wakeup_disable
 * 
 * @param gpio_num gpio_port
 * @return esp_err_t 
 */
iot_esp_ret iot_esp_gpio_wakeup_disable(iot_esp_gpio_port port)
{
    if (port >= IOT_ESP_GPIO_MAX) {
        return IOT_ESP_INVALID_PARAM;
    }
    /* gpio_config default */
    int ret = gpio_wakeup_disable(port);
    if (ret != 0) {
        return IOT_ESP_COM_ERROR;
    }
    return IOT_ESP_OK;
}