# 编译和集成指南

## 概述

本指南详细说明如何集成STM32 HAL库、配置硬件接口、编译项目并进行测试。

## 步骤1: 集成STM32 HAL库

### 方式1: 使用STM32CubeMX（推荐）

1. **安装STM32CubeMX**
   - 从ST官网下载并安装STM32CubeMX
   - 网址: https://www.st.com/en/development-tools/stm32cubemx.html

2. **创建新项目**
   ```
   1. 打开STM32CubeMX
   2. 选择 "New Project"
   3. 选择 MCU: STM32F108T6
   4. 配置时钟、UART、GPIO等外设
   5. 生成代码
   ```

3. **配置外设**
   - **UART1**: 用于二维码扫描模块
     - TX: PA9
     - RX: PA10
     - 波特率: 9600
   
   - **UART2**: 用于调试输出
     - TX: PA2
     - RX: PA3
     - 波特率: 115200
   
   - **UART3**: 用于WiFi模块
     - TX: PB10
     - RX: PB11
     - 波特率: 115200

4. **生成代码**
   - 选择 "Project Manager" -> "Code Generator"
   - 选择 "Copy only necessary library files"
   - 点击 "Generate Code"

5. **复制HAL库文件**
   ```bash
   # 将生成的HAL库文件复制到项目目录
   cp -r <CubeMX_Project>/Drivers/STM32F1xx_HAL_Driver HAL/
   cp -r <CubeMX_Project>/Drivers/CMSIS HAL/
   ```

### 方式2: 手动集成标准外设库

1. **下载标准外设库**
   - 从ST官网下载STM32F10x标准外设库
   - 解压到项目目录

2. **配置库路径**
   ```bash
   # 创建HAL目录结构
   mkdir -p HAL/STM32F10x_StdPeriph_Driver
   mkdir -p HAL/CMSIS
   
   # 复制库文件
   cp -r <StdPeriph_Lib>/Libraries/STM32F10x_StdPeriph_Driver/* HAL/STM32F10x_StdPeriph_Driver/
   cp -r <StdPeriph_Lib>/Libraries/CMSIS/* HAL/CMSIS/
   ```

## 步骤2: 配置硬件接口

编辑 `config.h` 文件，根据实际硬件配置：

```c
// UART配置
#define QR_UART_NUM              1      // 二维码扫描模块UART
#define QR_UART_BAUDRATE         9600   // 根据扫描模块调整

#define DEBUG_UART_NUM           2      // 调试UART
#define DEBUG_UART_BAUDRATE     115200

#define WIFI_UART_NUM            3      // WiFi模块UART
#define WIFI_UART_BAUDRATE       115200

// GPIO配置（可选）
#define OTA_TRIGGER_PIN          GPIO_PIN_1   // OTA触发按键
#define OTA_TRIGGER_PORT         GPIOA

#define LED_STATUS_PIN           GPIO_PIN_13  // LED状态指示
#define LED_STATUS_PORT          GPIOC

// WiFi配置
#define WIFI_SSID                "YourWiFiSSID"
#define WIFI_PASSWORD            "YourWiFiPassword"
```

## 步骤3: 选择使用的库类型

在 `config.h` 中配置：

```c
// 如果使用STM32CubeMX生成的HAL库，取消注释：
// #define USE_HAL_DRIVER

// 如果使用标准外设库，保持注释状态（默认）
```

在 `drivers/stm32_hal_wrapper.c` 中，代码会自动根据 `USE_HAL_DRIVER` 宏选择使用HAL库或标准外设库。

## 步骤4: 编译项目

### 使用Makefile编译

1. **配置Makefile**
   ```bash
   # 编辑Makefile，设置HAL库路径
   HAL_DIR = HAL
   ```

2. **编译Bootloader**
   ```bash
   make bootloader
   ```
   生成文件: `build/bootloader.bin`

3. **编译Application**
   ```bash
   make application
   ```
   生成文件: `build/application.bin`

4. **清理编译文件**
   ```bash
   make clean
   ```

### 使用STM32CubeIDE编译

1. **导入项目**
   - 打开STM32CubeIDE
   - File -> Import -> Existing Projects into Workspace
   - 选择项目目录

2. **配置项目**
   - 右键项目 -> Properties
   - C/C++ Build -> Settings
   - 配置包含路径和库路径

3. **编译**
   - Project -> Build All (Ctrl+B)

## 步骤5: 烧录固件

### 使用ST-Link Utility

1. **连接ST-Link**
   - 将ST-Link连接到开发板
   - 打开ST-Link Utility

2. **烧录Bootloader**
   ```
   1. File -> Open File
   2. 选择 build/bootloader.bin
   3. Target -> Program & Verify
   4. Start Address: 0x08000000
   5. 点击 Start
   ```

3. **烧录Application**
   ```
   1. File -> Open File
   2. 选择 build/application.bin
   3. Target -> Program & Verify
   4. Start Address: 0x08002000
   5. 点击 Start
   ```

### 使用OpenOCD

```bash
# 烧录Bootloader
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "program build/bootloader.bin verify reset exit 0x08000000"

# 烧录Application
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "program build/application.bin verify reset exit 0x08002000"
```

## 步骤6: 测试

### 1. 基础功能测试

**测试UART通信**
```c
// 在main.c的main函数中添加测试代码
uart_send_string(2, "UART Test\r\n");
```

**测试Flash操作**
```c
// 测试Flash读写
flash_unlock();
uint32_t test_addr = 0x08002000;
uint32_t test_data = 0x12345678;
flash_program_word(test_addr, test_data);
uint32_t read_data = *(volatile uint32_t *)test_addr;
// 验证 read_data == test_data
```

### 2. OTA功能测试

**测试二维码扫描**
1. 连接二维码扫描模块到UART1
2. 扫描包含固件URL的二维码
3. 检查UART2输出，应该看到"正在扫描二维码..."

**测试固件下载**
1. 准备一个测试固件文件
2. 将固件上传到HTTP服务器
3. 生成包含固件URL的二维码
4. 扫描二维码，系统应自动下载固件

**测试Flash写入**
1. 触发OTA升级
2. 观察UART2输出，应该看到：
   - "正在下载固件..."
   - "正在验证固件..."
   - "正在写入Flash..."
   - "升级完成"

### 3. Bootloader测试

1. **测试分区选择**
   - 烧录两个不同版本的固件到A/B分区
   - 重启设备，Bootloader应选择有效分区启动

2. **测试回滚机制**
   - 写入一个无效的固件到B分区
   - 重启设备，Bootloader应回滚到A分区

## 常见问题

### Q1: 编译错误 - 找不到HAL库文件

**A**: 检查HAL库路径配置
```bash
# 确保HAL目录存在
ls HAL/STM32F1xx_HAL_Driver/Inc/
```

### Q2: 链接错误 - 未定义的符号

**A**: 确保所有必要的HAL库源文件都被编译
```bash
# 检查Makefile中的HAL_SOURCES
make clean
make all
```

### Q3: Flash写入失败

**A**: 检查以下几点：
1. Flash是否已解锁
2. 写入地址是否对齐（4字节）
3. Flash是否已擦除
4. 地址是否在有效范围内

### Q4: UART无输出

**A**: 检查：
1. UART GPIO配置是否正确
2. 波特率是否匹配
3. 硬件连接是否正确
4. 时钟是否已配置

### Q5: WiFi模块无响应

**A**: 检查：
1. WiFi模块电源是否正常
2. UART连接是否正确
3. AT命令格式是否正确
4. 模块是否已初始化

## 调试技巧

1. **使用UART调试输出**
   - 所有状态信息通过UART2输出
   - 使用串口工具（如PuTTY）查看

2. **使用LED指示**
   - 配置LED引脚，用于状态指示
   - 在关键位置添加LED闪烁

3. **使用调试器**
   - 使用ST-Link或J-Link进行在线调试
   - 设置断点，单步执行

## 下一步

完成基础测试后，可以：
1. 优化代码性能
2. 添加更多功能
3. 进行压力测试
4. 优化Flash使用
5. 添加加密和签名验证

## 技术支持

如有问题，请参考：
- `README.md` - 项目概述
- `INTEGRATION_GUIDE.md` - 集成指南
- `USAGE.md` - 使用说明

