# 快速开始指南

## 5分钟快速集成

### 1. 使用STM32CubeMX生成项目（2分钟）

```bash
1. 打开STM32CubeMX
2. 选择 STM32F108T6
3. 配置UART1, UART2, UART3
4. 生成代码
5. 复制HAL库到项目HAL目录
```

### 2. 配置硬件接口（1分钟）

编辑 `config.h`:
```c
#define QR_UART_NUM      1
#define DEBUG_UART_NUM   2
#define WIFI_UART_NUM    3
```

### 3. 编译项目（1分钟）

```bash
make clean
make all
```

### 4. 烧录固件（1分钟）

```bash
# 使用ST-Link Utility或OpenOCD
# Bootloader: 0x08000000
# Application: 0x08002000
```

## 完成！

现在可以：
- 扫描二维码触发OTA升级
- 通过UART2查看调试信息
- 测试完整OTA流程

详细说明请参考 `BUILD_GUIDE.md`

