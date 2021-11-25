export BUILD_ROOT = $(shell pwd)

export INCLUDE_PATH = $(BUILD_ROOT)/include

BIN = output/kernel
export ELF := $(BIN).elf
export IMG := $(BIN)8.img

BUILD_DIR = $(BUILD_ROOT)/kernel/ \
            $(BUILD_ROOT)/uart/ \
            $(BUILD_ROOT)/lib/ \
            $(BUILD_ROOT)/timer/ \
            $(BUILD_ROOT)/irq/ \
            $(BUILD_ROOT)/mm/ \
            $(BUILD_ROOT)/videocore/ \
            $(BUILD_ROOT)/user/ \
            $(BUILD_ROOT)/power/ \
            $(BUILD_ROOT)/boot/ \

export DEBUG = false
