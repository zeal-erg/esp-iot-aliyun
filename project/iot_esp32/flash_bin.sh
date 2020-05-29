#!/bin/bash
# Author: wym
# Last date: Jan 28, 2018
# Desc:This script is used to flash the newest compiled bin and echo log.
#       In
#       user1 ---- only flash user1 bin
#       all ----- flash all bin, will erase user data and authorization information.
#
echo ""
CHIP_NAME=esp32
echo "chip:$CHIP_NAME"

# 获取当前USB_PORT数目，若为1自动选择此端口进行烧录，若大于1则手动选择端口
USB_NUM=$(ls /dev/ttyUSB*|awk -vsum=0 'BEGIN{ i=0 } { i++ } END{ print i }')

if [ "$USB_NUM" -gt "1" ];then
    echo "There are $USB_NUM usb port in PC:"
    ls /dev/ttyUSB*|awk '{printf "%s.%s  ", NR, $0}'
    echo ""
    echo "Choose the number of usb port below:(1/2/3/...)"
    read portnum
    USB_PORT=$(ls /dev/ttyUSB*|awk -vport=$portnum 'NR==port{printf "%s", $0}')
else
    echo "There is only 1 usb port in PC, choose this port automatically."
    USB_PORT=$(ls /dev/ttyUSB*)
fi

echo "usb port:"$USB_PORT

# set flash parameters
if [ "esp32" = $CHIP_NAME ];then
    PRJ_SDK_PATH=../../esp-idf
    BOOTLOADER_ADDR=0x1000
    FLASH_MODE=qio
    ECHO_BAUD=115200
    echo "Ensure your flash size of WiFi module is larger than 4MB."
else
    PRJ_SDK_PATH=../../ESP8266_RTOS_SDK
    BOOTLOADER_ADDR=0x0
    FLASH_MODE=qio
    ECHO_BAUD=74880
    echo "Ensure your flash size of WiFi module is larger than 2MB."
fi
USER1_ADDR=0x10000
PARTITION_NAME=partitions
FLASH_BAUD=921600
FLASH_TOOL_PATH=$PRJ_SDK_PATH/components/esptool_py/esptool/esptool.py
PRJ_NAME=iot_esp32


if [ "all" = "$1" ];then
    echo "# Flash all bin.... #"
    python $FLASH_TOOL_PATH \
            --chip $CHIP_NAME \
            --port $USB_PORT \
            --baud $FLASH_BAUD \
            --before default_reset \
            --after hard_reset \
            write_flash -z \
            --flash_mode $FLASH_MODE \
            --flash_freq 40m \
            --flash_size detect \
            0x0  ~+/build/$PRJ_NAME_target.bin 
else 
    echo "# Flash user1($USER1_ADDR) bin only... #"
    python $FLASH_TOOL_PATH \
            --chip $CHIP_NAME \
            --port $USB_PORT \
            --baud $FLASH_BAUD \
            --before default_reset \
            --after hard_reset \
            write_flash -z \
            --flash_mode $FLASH_MODE \
            --flash_freq 40m \
            --flash_size detect \
            $USER1_ADDR ~+/build/$PRJ_NAME.bin 
fi
# 若为分开烧录,不覆盖用户区flash和授权信息
# $BOOTLOADER_ADDR ~+/build/bootloader/bootloader.bin \
# $USER1_ADDR ~+/build/$PRJ_NAME.bin \
# 0x8000 ~+/build/$PARTITION_NAME.bin

echo " "
echo "Flash end. Now, serial log port monitor will run automatically."
# echo "Please enter your linux account password:"
# echo "(or you can enter it in ./project/$PRJ_NAME/run_flash.sh like the example below.)"
# echo "******(your account password)" | sudo -S python ~+/../tools/log_print_tool/log_74880.py
# run serial port monitor.
python2 ../../tools/log_print_tool/echo_log.py $USB_PORT $ECHO_BAUD
