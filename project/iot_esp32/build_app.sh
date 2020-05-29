#!/bin/bash
# $1 : APP_NAME
# $2 : APP_VERSION
# $3 : APP_ARGVS
# 	all build and flash all bin
#	clean clean
#	none only build bin

if [ -z "$1" ];then
	echo "please input the app name!!!"
	exit 1
else
	APP_BIN_NAME=$1
fi

if [ -z "$2" ];then
	echo "please input the app version(eg. 1.0.0)!!!"
	exit 1
else
	USER_SW_VER=$2
fi	

CHIP_NAME=esp32
PRJ_NAME=iot_$CHIP_NAME
TOP_DIR=$PWD/../..

OUTPUT_PATH=$TOP_DIR/out/$APP_BIN_NAME

if [ "iot_esp32" = $PRJ_NAME ];then
	TOOLCHAIN=xtensa-esp32-elf
else
	TOOLCHAIN=xtensa-lx106-elf
fi

#set xtensa toolchain path
PATH_TMP=$PATH
export PATH=$PATH_TMP:$TOP_DIR/tools/$TOOLCHAIN/bin

# check app path
if [ ! -d $PWD/iot_app/$APP_BIN_NAME ]; then
	echo "please input correct app name!!!"
	exit 1
fi

echo "APP_PATH=/iot_app/$APP_BIN_NAME"
echo "APP_NAME=$APP_BIN_NAME"
echo "APP_VERSION=$USER_SW_VER"

echo ""
echo "iot_esp32 compile start..."
echo ""
set -e

if [ -e "./build/$PRJ_NAME.bin" ]; then
	rm ./build/$PRJ_NAME.bin
fi
if [ -e "./build/$PRJ_NAME.S" ]; then
	rm ./build/$PRJ_NAME.S
fi
if [ -d "./build/iot"  ]; then
	rm -r ./build/iot
fi
IOT_MFG=single_mfg_config
iot_esp32_combia_bin()
{
        PARTITION_BIN_NAME=partitions_singleapp
        python $TOP_DIR/tools/filepack/bin_combia.py \
                ./build/bootloader/bootloader.bin \
                ./build/$PARTITION_BIN_NAME.bin \
                ./build/$PRJ_NAME.bin \
                ./build/$PRJ_NAME"_targe".bin \
                ./build/$IOT_MFG.bin
}

iot_esp32_md5_bin()
{
        python $TOP_DIR/tools/filepack/build_package_info.py \
                $OUTPUT_PATH/$USER_SW_VER/ \
                $APP_BIN_NAME   \
                $USER_SW_VER \
                $OUTPUT_PATH/$USER_SW_VER/
}

iot_esp32_out_bin()
{
        echo ""
        echo "output file...."
        if [ -e "./build/$PRJ_NAME.bin" ]; then
                cp ./build/$PRJ_NAME.bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_UA_"$USER_SW_VER.bin
                echo "cp ./build/$PRJ_NAME.bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_UA_"$USER_SW_VER.bin"
                cp ./build/$PRJ_NAME.bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_UG_"$USER_SW_VER.bin
                echo "cp ./build/$PRJ_NAME.bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_UG_"$USER_SW_VER.bin"
                cp ./build/$IOT_MFG.bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_MFG_"$USER_SW_VER.bin
                echo "cp ./build/$IOT_MFG.bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_MFG_"$USER_SW_VER.bin"

        fi

        if [ -e "./build/$PRJ_NAME"_targe".bin" ]; then
                cp ./build/$PRJ_NAME"_targe".bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_target_QIO_"$USER_SW_VER.bin
                echo "cp ./build/$PRJ_NAME"_targe".bin $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_target_QIO_"$USER_SW_VER.bin"
        fi

}

iot_esp32_nvs()
{
        echo "output single_mfg_config.bin"
        $TOP_DIR/esp-idf/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py \
        --input single_mfg_config.csv \
        --output ./build/$IOT_MFG.bin \
        --size 0x4000
}

iot_esp32_generate_asm_file()
{
        echo ""
        echo "generating dump file....."
        ../../tools/$TOOLCHAIN/bin/$TOOLCHAIN-objdump -x -d -S ./build/$CHIP_NAME.elf >$CHIP_NAME.S
        if [ -e "./$PRJ_NAME.S" ]; then
                cp ./$PRJ_NAME.S $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_"$USER_SW_VER"_dump".S
                echo "cp ./$PRJ_NAME.S $OUTPUT_PATH/$USER_SW_VER/$APP_BIN_NAME"_"$USER_SW_VER"_dump".S"
        fi
}

# make project
iot_esp32_make_bin()
{
        make -j4 APP_BIN_NAME=$APP_BIN_NAME USER_SW_VER=$USER_SW_VER $1
        echo ""
        if [ "clean" != "$1" ];then
                if [ ! -d "$OUTPUT_PATH/$USER_SW_VER" ]; then
                        mkdir -p $OUTPUT_PATH/$USER_SW_VER
                fi
                iot_esp32_nvs
                iot_esp32_combia_bin
                iot_esp32_out_bin
                # md5 check info
                echo ""
                iot_esp32_md5_bin
                echo ""
        fi
}

if [ "all" = "$3" ]; then
        echo "****** iot_esp32 build and flash all bin ******"
        iot_esp32_make_bin
        ./flash_bin.sh all

elif [ "user1" = "$3" ]; then
        echo "****** iot_esp32 build and flash user1 bin ******"
        iot_esp32_make_bin
        ./flash_bin.sh user1
elif [ "defconfig" = "$3" ]; then
        make chip=esp32 defconfig
else
        echo "****** iot_esp32 only build project ******"
        iot_esp32_make_bin $3
fi


if [ $? -ne 0 ]; then
    echo "****** iot_esp32 build_app.sh failed,sorry ******"
    exit 1
else
    echo "****** iot_esp32 build_app.sh succeed,enjoy iot ******"
fi

echo '      ___           ___                        ___                                               ___                   '
echo '     /  /\         /  /\           ___        /  /\          __                      ___        /  /\          ___     '
echo '    /  /::\       /  /::|         /__/\      /  /::\        |  |\                   /__/\      /  /::\        /__/\    '
echo '   /  /:/\:\     /  /:|:|         \__\:\    /  /:/\:\       |  |:|                  \__\:\    /  /:/\:\       \  \:\   '
echo '  /  /::\ \:\   /  /:/|:|__   ___ /  /::\  /  /:/  \:\      |  |:|                  /  /::\  /  /:/  \:\       \__\:\  '
echo ' /__/:/\:\ \:\ /__/:/ |:| /\ /__/\  /:/\/ /__/:/ \__\:\     |__|:|__             __/  /:/\/ /__/:/ \__\:\      /  /::\ '
echo ' \  \:\ \:\_\/ \__\/  |:|/:/ \  \:\/:/~~  \  \:\ /  /:/     /  /::::\           /__/\/:/~~  \  \:\ /  /:/     /  /:/\:\'
echo '  \  \:\ \:\       |  |:/:/   \  \::/      \  \:\  /:/     /  /:/~~~~           \  \::/      \  \:\  /:/     /  /:/__\/'
echo '   \  \:\_\/       |__|::/     \__\/        \  \:\/:/     /__/:/                 \  \:\       \  \:\/:/     /__/:/     '
echo '    \  \:\         /__/:/                    \  \::/      \__\/                   \__\/        \  \::/      \__\/      '
echo '     \__\/         \__\/                      \__\/                                             \__\/                  '