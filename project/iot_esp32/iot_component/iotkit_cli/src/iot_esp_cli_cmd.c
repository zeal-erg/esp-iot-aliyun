/**
 * @file iot_esp_cli_cmd.c
 * @author zenggong@tuya.com 
 * @brief 
 *          cmd list file
 * @version 0.1
 * @date 2020-04-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"

/**
 * @brief iot_esp_cli_demo
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_cli_demo(int argc, char **argv) {
    printf("iot-esp hello world\r\n");
    return 0;
}

/**
 * @brief iot_esp_restart
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_restart(int argc, char **argv) {
    printf("iot-esp restarting\r\n");
    esp_restart();
    return 0;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_sysver(int argc, char **argv) {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("iot-esp %s chip with %d CPU cores, WiFi%s%s ver %d\r\n",
             "esp32",
             chip_info.cores,
             (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
             chip_info.revision);
    return 0;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_sysfree_heap_size(int argc, char **argv) {
    printf("iot-esp free heap %d \r\n",esp_get_free_heap_size());
    return 0;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_pmem_cmd(int argc, char **argv) {
    int i;
    char *pos = NULL;
    char *addr = NULL;
    int nunits = 16;
    int width = 4;

    switch (argc) {
        case 4:
            width = strtol(argv[3], NULL, 0);
        case 3:
            nunits = strtol(argv[2], NULL, 0);
            nunits = nunits > 0x400 ? 0x400 : nunits;
        case 2:
            addr = (char *)strtol(argv[1], &pos, 0);
            break;
        default:
            break;
    }

    if (pos == NULL || pos == argv[1]) {
        printf(
            "p <addr> <nunits> <width>\r\n"
            "addr  : address to display\r\n"
            "nunits: number of units to display (default is 16)\r\n"
            "width : width of unit, 1/2/4 (default is 4)\r\n");
        return -1;
    }

    switch (width) {
        case 1:
            for (i = 0; i < nunits; i++) {
                if (i % 16 == 0) {
                    printf("0x%08x:", (unsigned int)addr);
                }
                printf(" %02x", *(unsigned char *)addr);
                addr += 1;
                if (i % 16 == 15) {
                    printf("\r\n");
                }
            }
            break;
        case 2:
            for (i = 0; i < nunits; i++) {
                if (i % 8 == 0) {
                    printf("0x%08x:", (unsigned int)addr);
                }
                printf(" %04x", *(unsigned short *)addr);
                addr += 2;
                if (i % 8 == 7) {
                    printf("\r\n");
                }
            }
            break;
        default:
            for (i = 0; i < nunits; i++) {
                if (i % 4 == 0) {
                    printf("0x%08x:", (unsigned int)addr);
                }
                printf(" %08x", *(unsigned int *)addr);
                addr += 4;
                if (i % 4 == 3) {
                    printf("\r\n");
                }
            }
            break;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_ps_cmd(int argc, char **argv) {
    char *pcWriteBuffer, *info;
    const char *const pcHeader = "State   Priority  Stack    #          Base\r\n********************************************************\r\n";
    BaseType_t xSpacePadding;

    info = pvPortMalloc(1536);
    if (NULL == info) {
        return -1;
    }
    pcWriteBuffer = info;

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Minus three for the null terminator and half the number of characters in
    "Task" so the column lines up with the centre of the heading. */
    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }
    strcpy(pcWriteBuffer, pcHeader);
    // vTaskList(pcWriteBuffer + strlen(pcHeader));
    printf("%s\r\n",info);

    vPortFree(info);
    return 0;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_get_mac_cmd(int argc, char **argv) {
    extern wifi_interface_t g_tmp_wifi_t;
    uint8_t get_mac[6];

    esp_err_t ret = esp_wifi_get_mac(g_tmp_wifi_t, get_mac);
    if (ret != ESP_OK) {
        printf("get mac err %d\r\n", ret);
        return -1;
    }
    printf("mac" MACSTR "\r\n", MAC2STR(get_mac));
    return 0;
}

/**
 * @brief 
 * 
 * @param authmode 
 */
static void print_auth_mode(int authmode) {
    switch (authmode) {
        case WIFI_AUTH_OPEN:
            printf("Authmode \tOPEN\t");
            break;
        case WIFI_AUTH_WEP:
            printf("Authmode \tWEP\t");
            break;
        case WIFI_AUTH_WPA_PSK:
            printf("Authmode \tWPA_PSK\t");
            break;
        case WIFI_AUTH_WPA2_PSK:
            printf("Authmode \tWPA2_PSK\t");
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            printf("Authmode \tWPA_WPA2_PSK\t");
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            printf("Authmode \tWPA2_ENTERPRISE\t");
            break;
        default:
            printf("Authmode \tUNKNOWN\t");
            break;
    }
}

/**
 * @brief 
 * 
 * @param pairwise_cipher 
 * @param group_cipher 
 */
static void print_cipher_type(int pairwise_cipher, int group_cipher) {
    switch (pairwise_cipher) {
        case WIFI_CIPHER_TYPE_NONE:
            printf("Pairwise Cipher \tNONE\t");
            break;
        case WIFI_CIPHER_TYPE_WEP40:
            printf("Pairwise Cipher \tWEP40\t");
            break;
        case WIFI_CIPHER_TYPE_WEP104:
            printf("Pairwise Cipher \tWEP104\t");
            break;
        case WIFI_CIPHER_TYPE_TKIP:
            printf("Pairwise Cipher \tTKIP\t");
            break;
        case WIFI_CIPHER_TYPE_CCMP:
            printf("Pairwise Cipher \tCCMP\t");
            break;
        case WIFI_CIPHER_TYPE_TKIP_CCMP:
            printf("Pairwise Cipher \tCCMP\t");
            break;
        default:
            printf("Pairwise Cipher \tUNKNOWN\t");
            break;
    }

    switch (group_cipher) {
        case WIFI_CIPHER_TYPE_NONE:
            printf("Group Cipher \tNONE\t");
            break;
        case WIFI_CIPHER_TYPE_WEP40:
            printf("Group Cipher \tWEP40\t");
            break;
        case WIFI_CIPHER_TYPE_WEP104:
            printf("Group Cipher \tP104\t");
            break;
        case WIFI_CIPHER_TYPE_TKIP:
            printf("Group Cipher \tTKIP\t");
            break;
        case WIFI_CIPHER_TYPE_CCMP:
            printf("Group Cipher \tCCMP\t");
            break;
        case WIFI_CIPHER_TYPE_TKIP_CCMP:
            printf("Group Cipher \tCCMP\t");
            break;
        default:
            printf("Group Cipher \tUNKNOWN\t");
            break;
    }
}

/**
 * @brief 
 * 
 */
/* Initialize Wi-Fi as sta and set scan method */
static void wifi_scan(void) {
    uint16_t number = 10;
    wifi_ap_record_t ap_info[10];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    printf("Total APs scanned = %u\r\n", ap_count);
    for (int i = 0; (i < 10) && (i < ap_count); i++) {
        printf("SSID \t%s\t", ap_info[i].ssid);
        printf("RSSI \t%d\t", ap_info[i].rssi);
        printf("Channel \t%d\t", ap_info[i].primary);
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        printf("\n");
    }
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int iot_esp_wifi_scan_cmd(int argc, char **argv) {
    wifi_scan();
    return 0;
}