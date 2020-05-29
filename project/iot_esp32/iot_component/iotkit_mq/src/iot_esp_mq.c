/**
 * @file iot_esp_mq.c
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "iot_esp_mq.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "iot_esp_list.h"
#include "iot_esp_type.h"
#include "iot_esp_wifi_config.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "mqtt_client.h"
#include "nvs_flash.h"

static const char *TAG = "MQTT";

struct iot_esp_mq_msg_recv_attrib {
    iot_esp_uint32 topic_len;
    iot_esp_int8 *topic;
};

struct iot_esp_mq_config {
    EventGroupHandle_t mq_event_group;
    esp_mqtt_client_handle_t client;
};

struct iot_esp_mq_info {
    bool is_start;
    iot_esp_uint16 mqtt_seq;
    iot_esp_mq_protocol_handler_cb mqtt_cb;
    struct iot_esp_list_head list;
};

struct iot_esp_mq_status {
    bool mq_flag;
    bool wifi_connnected_flag;
    bool res;
    iot_esp_uint8 event_bits;
    TaskHandle_t mq_task_handle;
    QueueHandle_t mq_queue_handle;
    struct iot_esp_mq_config *config;

    struct iot_esp_mq_info info;
};

/**
 * @brief 
 * 
 */
enum iot_esp_mq_cmd {
    MQTT_CMD_SUBSCRIBED = 0,
    MQTT_CMD_UNSUBSCRIBED,
    MQTT_CMD_PUBLISHED,
    MQTT_CMD_DATA,
    MQTT_CMD_NETWORK_CONNECTED,
    MQTT_CMD_MAX,
};

/**
 * @brief 
 * 
 */
struct iot_esp_mq_msg {
    enum iot_esp_mq_cmd cmd;
    iot_esp_uint32 len;
    void *attrib;
    iot_esp_int8 *data;
};

#define MQ_ALIGN(n, align) ((n + align - 1) & (~(align - 1)))
#define MQ_MSG_LEN(len, attribu_len, attrib) (sizeof(struct iot_esp_mq_msg) + MQ_ALIGN(len, 4) + sizeof(attrib) + attribu_len)
#define MQ_ATTRIB_ADDR(msg, len) (iot_esp_uint32)(msg + sizeof(struct iot_esp_mq_msg) + MQ_ALIGN(len, 4))

struct iot_esp_mq_status g_iot_esp_mq_status;



//mqtt连上事件
const static int CONNECTED_BIT = BIT0;
const static int IOT_ESP_CONNECTED_BIT = BIT0;

#define mqtt_event_group (g_iot_esp_mq_status.config->mq_event_group)
#define mq_client (g_iot_esp_mq_status.config->client)
#define mq_queue (g_iot_esp_mq_status.mq_queue_handle)


static void iot_esp_mq_get(esp_mqtt_event_handle_t event);

/**
 * @brief mqtt_event_handler
 * 
 * @param event 
 * @return esp_err_t 
 */
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
    esp_mqtt_client_handle_t tmtp_client = event->client;
    int msg_id;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:  //MQTT连上事件
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xEventGroupSetBits(mqtt_event_group, CONNECTED_BIT);

            // msg_id = esp_mqtt_client_subscribe(tmtp_client, "/a1IuFjwsCNk/3FtQAP8e8ucpEoKRAQE9/user/get", 1);
            // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            break;

        case MQTT_EVENT_DISCONNECTED:  //MQTT断开连接事件
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:  //MQTT发送订阅事件
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(tmtp_client, "/topic/qos0", "订阅成功", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:  //MQTT取消订阅事件
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:  //MQTT发布事件
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:  //MQTT接受数据事件
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            iot_esp_mq_get(event);

            break;

        case MQTT_EVENT_ERROR:  //MQTT错误事件
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            xEventGroupClearBits(mqtt_event_group, CONNECTED_BIT);
            break;

        default:

            break;
    }
    return ESP_OK;
}

/**
 * @brief mqtt_app_start
 * 
 */
static void mqtt_app_start(void) {
    // ESP_ERROR_CHECK(esp_netif_init());

    esp_mqtt_client_config_t mqtt_cfg = {
        .host = "a1IuFjwsCNk.iot-as-mqtt.cn-shanghai.aliyuncs.com",  //MQTT服务器IP
        .client_id = "12345|securemode=3,signmethod=hmacsha1|",
        .event_handle = mqtt_event_handler,                         //MQTT事件
        .port = 1883,                                               //端口
        .username = "3FtQAP8e8ucpEoKRAQE9&a1IuFjwsCNk",             //用户名
        .password = "C77F1B71F22D76E3320B09D97DB2214BE3385CA8",     //密码
    };

    mq_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mq_client);

    ESP_LOGI(TAG, "client %p", mq_client);

    //等mqtt连上
    xEventGroupWaitBits(mqtt_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

/**
 * @brief 
 * 
 */
void iot_esp_mq_wifi_is_conneted(void)
{
    xEventGroupWaitBits(mqtt_event_group, IOT_ESP_CONNECTED_BIT, false, true, portMAX_DELAY);
}

/**
 * @brief iot_esp_mq_notify
 * 
 */
void iot_esp_mq_notify(void)
{
    struct iot_esp_mq_msg *mq_msg = NULL;
    mq_msg = (struct iot_esp_mq_msg *)malloc(sizeof(struct iot_esp_mq_msg));
    if (NULL == mq_msg) {
        ESP_LOGE(TAG, "mq_msg malloc fail");
        return;
    }
    mq_msg->cmd = MQTT_CMD_NETWORK_CONNECTED;
    mq_msg->data = NULL;
    xQueueSend(mq_queue, mq_msg,0);
}

/**
 * @brief iot_esp_mq_notify
 * 
 */
static void iot_esp_mq_get(esp_mqtt_event_handle_t event)
{
    struct iot_esp_mq_msg *mq_msg = NULL;
    struct iot_esp_mq_msg_recv_attrib *attrib = NULL;
    iot_esp_uint32 msg_len = MQ_MSG_LEN(event->data_len, event->topic_len, struct iot_esp_mq_msg_recv_attrib);

    mq_msg = (struct iot_esp_mq_msg *)malloc(msg_len);
    if (NULL == mq_msg) {
        ESP_LOGE(TAG, "mq_msg malloc fail");
        return;
    }
    memset(mq_msg, 0, msg_len);

    mq_msg->cmd = MQTT_CMD_DATA;
    mq_msg->len = event->data_len;
    memcpy(mq_msg->data, event->data, event->data_len);

    mq_msg->attrib = (void *)MQ_ATTRIB_ADDR(mq_msg, mq_msg->len);
    attrib = (struct iot_esp_mq_msg_recv_attrib *)mq_msg->attrib;
    attrib->topic_len = event->topic_len;
    memcpy(attrib->topic, event->topic, event->topic_len);

    xQueueSend(mq_queue, mq_msg, 0);
}

iot_esp_ret iot_esp_client_start()
{
    return IOT_ESP_OK;
}

iot_esp_ret iot_esp_client_subscribe(void)
{
    return IOT_ESP_OK;
}

iot_esp_ret iot_esp_mq_register_cb(iot_esp_uint16 mq_pro, iot_esp_mq_protocol_handler_cb handler)
{
    struct iot_esp_mq_info *mq_info = NULL;
    if (mq_pro >= MQTT_MAX) {
        ESP_LOGE(TAG, "mq_pro invalid param!");
        return IOT_ESP_INVALID_PARAM;
    }

    mq_info = (struct iot_esp_mq_info *)malloc(sizeof(struct iot_esp_mq_info));
    if (NULL == mq_info) {
        ESP_LOGE(TAG, "mq_info malloc fail!");
        return IOT_ESP_MALLOC_FAILD;
    }
    memset(mq_info, 0, sizeof(struct iot_esp_mq_info));

    mq_info->mqtt_seq = mq_pro;
    mq_info->is_start = false;
    mq_info->mqtt_cb = handler;
    INIT_LIST_HEAD(&mq_info->list);

    iot_esp_list_add_tail(&mq_info->list, &g_iot_esp_mq_status.info.list);
    /* create subscribe */

    return IOT_ESP_OK;
}

iot_esp_ret iot_esp_mq_parse(struct iot_esp_mq_msg *mq_msg)
{
    return IOT_ESP_OK;
}

/**
 * @brief 
 * 
 * @param argc 
 */
void iot_esp_mq_entry(void *argc) {
    iot_esp_ret ret = IOT_ESP_OK;
    int msg_id;
    struct iot_esp_mq_msg *mq_msg = NULL;
    mqtt_event_group = xEventGroupCreate();
    mqtt_app_start();

    while(1) {
        /* 等待消息队列及消息分发 */
        xQueueReceive(mq_queue,mq_msg, 0xFFFFFFFF);
        if (NULL == mq_msg) {
            continue;
        }
        switch (mq_msg->cmd) {
            case MQTT_CMD_SUBSCRIBED: {
                /* 判断是否存在需要 */
                msg_id = esp_mqtt_client_subscribe(mq_client, "/a1IuFjwsCNk/3FtQAP8e8ucpEoKRAQE9/user/get", 1);
                ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            }
            break;

            case MQTT_CMD_UNSUBSCRIBED: {

            }
            break;

            case MQTT_CMD_PUBLISHED: {

            }
            break;

            case MQTT_CMD_DATA: {
                iot_esp_mq_parse(mq_msg);
            }
            break;

            case MQTT_CMD_NETWORK_CONNECTED: {
                /*  */

            }
            break;

            default:

            break;
        }
        /* 释放消息 */
        free(mq_msg);
    }
}

/**
 * @brief 
 * 
 */
void iot_esp_mq_init(void) {
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
    ESP_LOGI(TAG, "start...");

    struct iot_esp_mq_config *mq_config = NULL;
    mq_config = (struct iot_esp_mq_config *)malloc(sizeof(struct iot_esp_mq_config));
    if (NULL == mq_config) {
        ESP_LOGE(TAG, "mq_config malloc fail!");
        return;
    }
    memset(&g_iot_esp_mq_status,0,sizeof(struct iot_esp_mq_status));
    g_iot_esp_mq_status.config = mq_config;

    mq_queue = xQueueCreate(128, sizeof(struct iot_esp_mq_msg));
    if (NULL == mq_queue) {
        ESP_LOGE(TAG, "mq_handle crt fail!");
        return;
    }
    INIT_LIST_HEAD(&g_iot_esp_mq_status.info.list);
    xTaskCreate(iot_esp_mq_entry, "iot_mq", 2048, NULL, IOT_ESP_WIFI_MQTT_PRIORITY, &g_iot_esp_mq_status.mq_task_handle);
}
