# 集成指南

## 概述

本项目已经集成了所有必要的库和功能，可以直接使用。本文档说明如何将项目集成到STM32开发环境中。

## 已集成的组件

### 1. STM32 HAL库封装层
- **位置**: `drivers/stm32_hal_wrapper.h/c`
- **功能**: Flash操作、UART通信、GPIO控制、系统时钟
- **状态**: 框架已实现，需要根据实际HAL库版本调整

### 2. HTTP客户端
- **位置**: `drivers/http_client.h/c`
- **功能**: 通过AT命令（WiFi模块）下载固件
- **状态**: 已实现，支持ESP8266等WiFi模块

### 3. 二维码解码器
- **位置**: `drivers/qrcode_decoder.h/c`
- **功能**: 二维码解码（简化版，支持字符串直接解码）
- **状态**: 基础实现，支持扫描模块直接输出字符串

### 4. 所有核心模块
- Flash管理、OTA管理、版本控制等已全部实现并集成

## 集成步骤

### 步骤1: 配置STM32 HAL库

1. **使用STM32CubeMX生成项目**（推荐）
   - 选择STM32F108T6
   - 配置UART1、UART2、UART3
   - 配置SysTick
   - 生成代码

2. **或手动集成HAL库**
   - 将STM32 HAL库文件添加到项目
   - 在`drivers/stm32_hal_wrapper.c`中实现具体函数

### 步骤2: 更新HAL封装函数

编辑 `drivers/stm32_hal_wrapper.c`，根据实际HAL库实现以下函数：

```c
// Flash操作
void flash_unlock(void) {
    HAL_FLASH_Unlock();
}

void flash_lock(void) {
    HAL_FLASH_Lock();
}

int flash_erase_page(uint32_t page_addr) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = page_addr;
    EraseInitStruct.NbPages = 1;
    uint32_t PageError = 0;
    return (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) == HAL_OK) ? 0 : -1;
}

int flash_program_word(uint32_t addr, uint32_t data) {
    return (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data) == HAL_OK) ? 0 : -1;
}

// UART操作
int uart_init(uint8_t uart_num, uint32_t baudrate) {
    // 根据uart_num初始化对应的UART
    // 例如：HAL_UART_Init(&huart1);
}

// 系统时钟
uint32_t get_system_tick(void) {
    return HAL_GetTick();
}
```

### 步骤3: 配置硬件接口

编辑 `config.h`，根据实际硬件配置：

```c
// UART配置
#define QR_UART_NUM              1      // 二维码扫描模块UART
#define DEBUG_UART_NUM            2      // 调试UART
#define WIFI_UART_NUM             3      // WiFi模块UART

// GPIO配置（可选）
#define OTA_TRIGGER_PIN           GPIO_PIN_1
#define LED_STATUS_PIN            GPIO_PIN_13
```

### 步骤4: 配置WiFi模块（如果使用）

如果使用ESP8266等WiFi模块：

1. 连接WiFi模块到UART3
2. 在`http_client.c`中可能需要调整AT命令（根据模块型号）

### 步骤5: 编译项目

#### 使用STM32CubeIDE

1. 导入项目到STM32CubeIDE
2. 配置编译选项
3. 编译Bootloader和Application

#### 使用Makefile

```bash
# 编译Bootloader
make bootloader

# 编译Application
make application
```

### 步骤6: 烧录固件

1. **先烧录Bootloader**
   ```
   地址: 0x08000000
   文件: build/bootloader.bin
   ```

2. **再烧录Application**
   ```
   地址: 0x08002000
   文件: build/application.bin
   ```

## 使用说明

### 1. 启动系统

系统上电后：
- Bootloader首先运行
- 检查是否需要进入Bootloader模式
- 验证A/B分区并选择有效分区启动
- 跳转到应用程序

### 2. 触发OTA升级

方式1: 通过按键（如果配置了）
```c
// 在主循环中检测按键
if (gpio_read_pin(OTA_TRIGGER_PORT, OTA_TRIGGER_PIN)) {
    ota_start_upgrade();
}
```

方式2: 通过UART命令
```c
// 接收UART命令触发升级
```

### 3. 扫描二维码

1. 将二维码扫描模块连接到UART1
2. 扫描包含固件URL的二维码
3. 系统自动提取URL并开始下载

### 4. 固件下载

系统自动：
1. 连接WiFi（如果使用WiFi模块）
2. 通过HTTP下载固件
3. 验证CRC32
4. 写入Flash
5. 重启设备

## 版本控制说明

**重要**: 本项目已移除降级限制，允许回滚到旧版本。

- 只要版本不同，就可以升级（包括降级）
- 版本相同则跳过升级
- 版本格式: `major.minor.revision.build`

如需限制只能升级，修改 `common/version_control.c`:

```c
bool version_need_update(...) {
    // 只允许升级，不允许降级
    return (version_compare(target_version, current_version) > 0);
}
```

## 调试

### UART调试输出

调试信息通过UART2输出（115200波特率），包括：
- 状态信息
- 错误信息
- 进度信息

### 常见问题

1. **Flash写入失败**
   - 检查Flash是否已解锁
   - 检查地址是否对齐（4字节）
   - 检查是否已擦除

2. **下载失败**
   - 检查WiFi连接
   - 检查URL是否可访问
   - 检查网络超时设置

3. **二维码扫描失败**
   - 检查UART配置
   - 检查扫描模块协议
   - 检查URL格式

## 扩展功能

### 集成完整quirc库

如果需要从图像解码二维码：

1. 下载quirc库: https://github.com/dlbeer/quirc
2. 添加到项目
3. 在`drivers/qrcode_decoder.c`中实现`qrcode_decode()`

### 使用lwIP替代AT命令

如果需要直接TCP连接：

1. 集成lwIP协议栈
2. 在`drivers/http_client.c`中实现TCP模式
3. 配置为`HTTP_MODE_TCP_DIRECT`

## 技术支持

- 查看 `README.md` - 项目概述
- 查看 `IMPLEMENTATION_NOTES.md` - 实现细节
- 查看 `USAGE.md` - 使用说明

