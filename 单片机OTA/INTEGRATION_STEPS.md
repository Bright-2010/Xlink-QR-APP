# HAL库集成步骤（详细版）

## 快速集成（推荐使用脚本）

### 步骤1: 使用STM32CubeMX生成项目

1. **下载并安装STM32CubeMX**
   - 访问: https://www.st.com/en/development-tools/stm32cubemx.html
   - 下载并安装（需要Java运行环境）

2. **创建新项目**
   ```
   1. 打开STM32CubeMX
   2. 点击 "New Project"
   3. 在搜索框输入 "STM32F108T6"
   4. 选择对应的MCU
   5. 点击 "Start Project"
   ```

3. **配置外设**
   
   **UART1配置（二维码扫描模块）**
   ```
   - 左侧选择 "Connectivity" -> "USART1"
   - Mode: Asynchronous
   - Baud Rate: 9600
   - GPIO Settings: 
     * PA9 -> USART1_TX
     * PA10 -> USART1_RX
   ```
   
   **UART2配置（调试输出）**
   ```
   - 选择 "USART2"
   - Mode: Asynchronous
   - Baud Rate: 115200
   - GPIO Settings:
     * PA2 -> USART2_TX
     * PA3 -> USART2_RX
   ```
   
   **UART3配置（WiFi模块）**
   ```
   - 选择 "USART3"
   - Mode: Asynchronous
   - Baud Rate: 115200
   - GPIO Settings:
     * PB10 -> USART3_TX
     * PB11 -> USART3_RX
   ```

4. **配置时钟**
   ```
   - 左侧选择 "Clock Configuration"
   - 配置HSE（如果使用外部晶振）
   - 设置系统时钟为72MHz
   ```

5. **生成代码**
   ```
   - 点击 "Project Manager"
   - Project Settings:
     * Project Name: STM32_OTA
     * Toolchain/IDE: Makefile
   - Code Generator:
     * 选择 "Copy only necessary library files"
   - 点击 "GENERATE CODE"
   ```

### 步骤2: 使用集成脚本复制HAL库

```bash
# 运行集成脚本
./scripts/integrate_hal.sh <CubeMX项目路径>

# 示例：
./scripts/integrate_hal.sh ~/STM32CubeMX/STM32_OTA
```

脚本会自动：
- 创建HAL目录结构
- 复制所有必需的HAL库文件
- 验证关键文件是否存在

### 步骤3: 配置硬件参数

编辑 `config.h`，根据实际硬件配置：

```c
// UART配置（已默认配置，可根据需要调整）
#define QR_UART_NUM              1
#define QR_UART_BAUDRATE         9600

#define DEBUG_UART_NUM           2
#define DEBUG_UART_BAUDRATE      115200

#define WIFI_UART_NUM            3
#define WIFI_UART_BAUDRATE       115200

// GPIO配置（可选，根据实际硬件）
#define OTA_TRIGGER_PIN          GPIO_PIN_1
#define OTA_TRIGGER_PORT         GPIOA

#define LED_STATUS_PIN           GPIO_PIN_13
#define LED_STATUS_PORT          GPIOC

// WiFi配置（根据实际WiFi网络）
#define WIFI_SSID                "YourWiFiSSID"
#define WIFI_PASSWORD            "YourWiFiPassword"
```

### 步骤4: 选择库类型

在 `config.h` 中：

```c
// 如果使用STM32CubeMX生成的HAL库，取消注释：
#define USE_HAL_DRIVER

// 如果使用标准外设库，保持注释状态
```

### 步骤5: 验证集成

```bash
# 运行项目设置脚本
./scripts/setup_project.sh

# 编译项目
make clean
make all
```

## 手动集成（不使用脚本）

如果不想使用脚本，可以手动复制文件：

```bash
# 1. 创建目录
mkdir -p HAL/STM32F1xx_HAL_Driver/Inc
mkdir -p HAL/STM32F1xx_HAL_Driver/Src
mkdir -p HAL/CMSIS/Device/ST/STM32F1xx/Include
mkdir -p HAL/CMSIS/Include

# 2. 复制HAL驱动库
cp -r <CubeMX项目>/Drivers/STM32F1xx_HAL_Driver/Inc/* HAL/STM32F1xx_HAL_Driver/Inc/
cp -r <CubeMX项目>/Drivers/STM32F1xx_HAL_Driver/Src/* HAL/STM32F1xx_HAL_Driver/Src/

# 3. 复制CMSIS文件
cp -r <CubeMX项目>/Drivers/CMSIS/Device/ST/STM32F1xx/Include/* HAL/CMSIS/Device/ST/STM32F1xx/Include/
cp -r <CubeMX项目>/Drivers/CMSIS/Include/* HAL/CMSIS/Include/
```

## 使用标准外设库

如果不使用HAL库，可以使用标准外设库：

1. **下载标准外设库**
   - 从ST官网下载STM32F10x标准外设库
   - 解压后复制到HAL目录

2. **配置项目**
   - 在 `config.h` 中保持 `USE_HAL_DRIVER` 注释状态
   - 代码会自动使用标准外设库实现

## 验证清单

集成完成后，检查以下文件是否存在：

- [ ] `HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h`
- [ ] `HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_flash.h`
- [ ] `HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_uart.h`
- [ ] `HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_gpio.h`
- [ ] `HAL/CMSIS/Include/core_cm3.h`
- [ ] `HAL/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h`

## 常见问题

### Q1: 脚本提示找不到文件

**A**: 检查CubeMX项目路径是否正确，确保路径包含`Drivers`目录

### Q2: 编译时找不到头文件

**A**: 
1. 检查HAL库文件是否已正确复制
2. 检查Makefile中的HAL_DIR路径是否正确
3. 运行 `make clean && make all` 重新编译

### Q3: 链接错误

**A**: 确保所有HAL库源文件都已复制到`HAL/STM32F1xx_HAL_Driver/Src/`目录

## 下一步

完成HAL库集成后：
1. 配置硬件参数（`config.h`）
2. 编译项目（`make all`）
3. 烧录固件
4. 测试功能

详细说明请参考 `BUILD_GUIDE.md`

