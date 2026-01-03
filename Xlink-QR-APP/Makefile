# STM32 OTA Project Makefile
# Target: STM32F108T6

# 工具链配置
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# 项目配置
PROJECT_NAME = stm32_ota
MCU = STM32F108T6
TARGET_CPU = cortex-m3

# 目录配置
BOOTLOADER_DIR = bootloader
APP_DIR = application
COMMON_DIR = common
DRIVERS_DIR = drivers
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# 源文件
BOOTLOADER_SOURCES = $(wildcard $(BOOTLOADER_DIR)/*.c)
APP_SOURCES = $(wildcard $(APP_DIR)/*.c)
COMMON_SOURCES = $(wildcard $(COMMON_DIR)/*.c)
DRIVER_SOURCES = $(wildcard $(DRIVERS_DIR)/*.c)

# 对象文件
BOOTLOADER_OBJECTS = $(BOOTLOADER_SOURCES:$(BOOTLOADER_DIR)/%.c=$(OBJ_DIR)/bootloader_%.o)
APP_OBJECTS = $(APP_SOURCES:$(APP_DIR)/%.c=$(OBJ_DIR)/app_%.o)
COMMON_OBJECTS = $(COMMON_SOURCES:$(COMMON_DIR)/%.c=$(OBJ_DIR)/common_%.o)
DRIVER_OBJECTS = $(DRIVER_SOURCES:$(DRIVERS_DIR)/%.c=$(OBJ_DIR)/driver_%.o)

# HAL库路径配置（需要根据实际路径修改）
HAL_DIR = HAL
STM32F1_DIR = $(HAL_DIR)/STM32F1xx_HAL_Driver
CMSIS_DIR = $(HAL_DIR)/CMSIS

# 包含目录
INCLUDES = -I$(BOOTLOADER_DIR) \
           -I$(APP_DIR) \
           -I$(COMMON_DIR) \
           -I$(DRIVERS_DIR) \
           -I. \
           -I$(CMSIS_DIR)/Device/ST/STM32F1xx/Include \
           -I$(CMSIS_DIR)/Include \
           -I$(STM32F1_DIR)/Inc

# HAL库源文件（如果使用HAL库）
HAL_SOURCES = $(wildcard $(STM32F1_DIR)/Src/*.c)
HAL_OBJECTS = $(HAL_SOURCES:$(STM32F1_DIR)/Src/%.c=$(OBJ_DIR)/hal_%.o)

# 编译选项
CFLAGS = -mcpu=$(TARGET_CPU) -mthumb \
         -Wall -Wextra -Wno-unused-parameter \
         -Os -ffunction-sections -fdata-sections \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER \
         $(INCLUDES)

# 如果使用HAL库，添加HAL定义
# CFLAGS += -DUSE_HAL_DRIVER

LDFLAGS = -mcpu=$(TARGET_CPU) -mthumb \
          -Wl,--gc-sections \
          -Wl,-Map=$(BUILD_DIR)/$(PROJECT_NAME).map \
          -T$(MCU).ld

# 默认目标
all: bootloader application

# Bootloader
bootloader: $(BUILD_DIR)/bootloader.bin

$(BUILD_DIR)/bootloader.bin: $(BUILD_DIR)/bootloader.elf
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

$(BUILD_DIR)/bootloader.elf: $(BOOTLOADER_OBJECTS) $(DRIVER_OBJECTS) $(HAL_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJDUMP) -h -S $@ > $(BUILD_DIR)/bootloader.lst

# Application
application: $(BUILD_DIR)/application.bin

$(BUILD_DIR)/application.bin: $(BUILD_DIR)/application.elf
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

$(BUILD_DIR)/application.elf: $(APP_OBJECTS) $(COMMON_OBJECTS) $(DRIVER_OBJECTS) $(HAL_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJDUMP) -h -S $@ > $(BUILD_DIR)/application.lst

# 编译规则
$(OBJ_DIR)/bootloader_%.o: $(BOOTLOADER_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/app_%.o: $(APP_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/common_%.o: $(COMMON_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/driver_%.o: $(DRIVERS_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# HAL库编译规则（如果使用HAL库）
$(OBJ_DIR)/hal_%.o: $(STM32F1_DIR)/Src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all bootloader application clean
