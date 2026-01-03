# HAL库集成说明

## 目录结构

```
HAL/
├── STM32F1xx_HAL_Driver/    # STM32F1 HAL驱动库
│   ├── Inc/                 # 头文件
│   └── Src/                 # 源文件
└── CMSIS/                   # CMSIS核心文件
    ├── Device/ST/STM32F1xx/Include/  # 设备特定头文件
    └── Include/             # CMSIS核心头文件
```

## 集成方法

### 方法1: 使用STM32CubeMX（推荐）

1. **打开STM32CubeMX**
   ```bash
   # 从ST官网下载并安装STM32CubeMX
   # https://www.st.com/en/development-tools/stm32cubemx.html
   ```

2. **创建新项目**
   - 选择MCU: STM32F108T6
   - 配置外设（UART1, UART2, UART3）
   - 生成代码

3. **复制HAL库文件**
   ```bash
   # 从CubeMX生成的项目中复制
   cp -r <CubeMX_Project>/Drivers/STM32F1xx_HAL_Driver/* HAL/STM32F1xx_HAL_Driver/
   cp -r <CubeMX_Project>/Drivers/CMSIS/* HAL/CMSIS/
   ```

### 方法2: 手动下载标准外设库

1. **下载标准外设库**
   - 从ST官网下载STM32F10x标准外设库
   - 解压后复制到HAL目录

2. **或使用Git克隆**
   ```bash
   # STM32 HAL库（如果可用）
   # 注意：需要从ST官网获取
   ```

## 必需的文件

### HAL库必需文件（如果使用HAL库）

- `stm32f1xx_hal.h`
- `stm32f1xx_hal_flash.h` / `stm32f1xx_hal_flash.c`
- `stm32f1xx_hal_uart.h` / `stm32f1xx_hal_uart.c`
- `stm32f1xx_hal_gpio.h` / `stm32f1xx_hal_gpio.c`
- `stm32f1xx_hal_rcc.h` / `stm32f1xx_hal_rcc.c`

### CMSIS必需文件

- `core_cm3.h`
- `stm32f1xx.h`
- `system_stm32f1xx.h`

## 验证集成

集成完成后，运行：
```bash
make clean
make all
```

如果编译成功，说明HAL库已正确集成。

