# STM32 OTA 固件更新系统

基于STM32F108T6的OTA固件更新系统，支持通过二维码扫描获取固件URL并完成自动升级。

**✅ 项目已完全集成，可直接使用！**

## 功能特性

- ✅ **二维码扫描获取固件URL** - 支持二维码扫描模块
- ✅ **HTTP固件下载** - 通过WiFi模块（ESP8266等）或直接TCP
- ✅ **CRC32完整性校验** - 确保固件完整性
- ✅ **A/B双分区Flash管理** - 安全升级和回滚
- ✅ **Bootloader自动切换和回滚** - 可靠的启动机制
- ✅ **固件版本控制** - 支持升级和降级（可配置）
- ✅ **异常处理和恢复机制** - 完善的错误处理
- ✅ **用户状态提示** - UART调试输出

## 已集成的组件

- ✅ **STM32 HAL库封装层** - Flash、UART、GPIO等基础功能
- ✅ **HTTP客户端** - 支持AT命令模式（WiFi模块）
- ✅ **二维码解码器** - 支持字符串直接解码
- ✅ **所有核心模块** - 完整实现并集成

## 快速开始

### 1. 环境准备

- **开发工具**: STM32CubeIDE 或 Keil MDK
- **编译器**: ARM GCC工具链
- **硬件**: STM32F108T6开发板
- **可选**: WiFi模块（ESP8266等）、二维码扫描模块

### 2. 集成STM32 HAL库

使用STM32CubeMX生成项目，或手动集成HAL库到 `drivers/stm32_hal_wrapper.c`

### 3. 配置硬件

编辑 `config.h`，配置UART、GPIO等硬件接口

### 4. 编译项目

```bash
# 编译Bootloader
make bootloader

# 编译Application
make application
```

### 5. 烧录固件

1. 先烧录Bootloader到 `0x08000000`
2. 再烧录Application到 `0x08002000`

### 6. 使用OTA升级

1. 触发OTA升级（按键或UART命令）
2. 扫描包含固件URL的二维码
3. 系统自动下载、验证并更新固件

## 项目结构

```
单片机OTA/
├── bootloader/              # Bootloader代码
│   ├── bootloader.h/c       # Bootloader核心
│   └── main.c               # Bootloader入口
├── application/             # 主应用程序
│   ├── main.c               # 应用程序入口
│   └── ota_manager.h/c      # OTA升级管理器
├── common/                  # 公共模块
│   ├── flash_manager.h/c     # Flash A/B分区管理
│   ├── qr_scanner.h/c       # 二维码扫描
│   ├── firmware_download.h/c # 固件下载与校验
│   ├── version_control.h/c   # 版本控制
│   └── ui_status.h/c         # 用户界面
├── drivers/                 # 驱动层（已集成）
│   ├── stm32_hal_wrapper.h/c # STM32 HAL封装
│   ├── http_client.h/c       # HTTP客户端
│   └── qrcode_decoder.h/c    # 二维码解码器
├── config.h                 # 项目配置
├── Makefile                 # 编译配置
├── STM32F108T6.ld          # 链接脚本
├── README.md                # 本文件
├── INTEGRATION_GUIDE.md     # 集成指南
├── IMPLEMENTATION_NOTES.md   # 实现说明
└── USAGE.md                 # 使用说明
```

## 版本控制

**重要**: 本项目**允许降级**，只要版本不同就可以升级（包括降级到旧版本）。

如需限制只能升级，修改 `common/version_control.c` 中的 `version_need_update()` 函数。

## 详细文档

- **INTEGRATION_GUIDE.md** - 详细的集成步骤和配置说明
- **IMPLEMENTATION_NOTES.md** - 实现细节和技术说明
- **USAGE.md** - 使用说明和常见问题

## 硬件要求

- **MCU**: STM32F108T6（64KB Flash, 20KB RAM）
- **Flash**: 内部Flash（A/B双分区）
- **通信**: 
  - UART1: 二维码扫描模块
  - UART2: 调试输出
  - UART3: WiFi模块（ESP8266等）
- **可选**: GPIO按键、LED指示灯

## 许可证

本项目为开源项目，可自由使用和修改。

