#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

IDF_PATH := $(abspath $(shell pwd)/../../esp-idf)
PROJECT_NAME := iot_esp32

# add user comonent dirs
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/../iot_frame/lib

EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_app)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_user_common)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_common)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_component)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_component/iotkit_alios)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_component/iotkit_cli)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_component/iotkit_kvs)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_component/iotkit_mq)
EXTRA_COMPONENT_DIRS += $(abspath $(shell pwd)/iot_component/iotkit_light)


SDKCONFIG_DEFAULTS := sdkconfig_$(chip).defaults
# add easylogger path
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/../iot_frame/EasyLogger

include $(IDF_PATH)/make/project.mk
