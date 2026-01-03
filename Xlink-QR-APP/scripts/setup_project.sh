#!/bin/bash
# 项目设置脚本
# 自动配置项目环境

set -e

echo "=========================================="
echo "  STM32 OTA项目设置脚本"
echo "=========================================="

# 检查工具链
echo "检查ARM工具链..."
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "✗ 未找到ARM工具链"
    echo "请安装ARM GCC工具链："
    echo "  macOS: brew install arm-none-eabi-gcc"
    echo "  Linux: sudo apt-get install gcc-arm-none-eabi"
    exit 1
fi
echo "✓ ARM工具链已安装"

# 检查OpenOCD（可选）
if command -v openocd &> /dev/null; then
    echo "✓ OpenOCD已安装"
else
    echo "⚠ OpenOCD未安装（可选，用于烧录）"
fi

# 创建必要的目录
echo ""
echo "创建项目目录结构..."
mkdir -p HAL/STM32F1xx_HAL_Driver/Inc
mkdir -p HAL/STM32F1xx_HAL_Driver/Src
mkdir -p HAL/CMSIS/Device/ST/STM32F1xx/Include
mkdir -p HAL/CMSIS/Include
mkdir -p build/obj
echo "✓ 目录结构已创建"

# 检查HAL库
echo ""
echo "检查HAL库文件..."
HAL_FILES=0

if [ -f "HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h" ]; then
    echo "✓ 找到HAL库文件"
    HAL_FILES=1
else
    echo "⚠ HAL库文件未找到"
    echo ""
    echo "请执行以下步骤之一："
    echo "1. 使用STM32CubeMX生成项目，然后运行："
    echo "   ./scripts/integrate_hal.sh <CubeMX项目路径>"
    echo ""
    echo "2. 手动下载并复制HAL库文件到HAL目录"
fi

# 显示配置信息
echo ""
echo "=========================================="
echo "  当前配置"
echo "=========================================="
echo "MCU: STM32F108T6"
echo "Flash: 64KB"
echo "RAM: 20KB"
echo ""
echo "UART配置:"
echo "  UART1: 二维码扫描 (9600)"
echo "  UART2: 调试输出 (115200)"
echo "  UART3: WiFi模块 (115200)"
echo ""

if [ $HAL_FILES -eq 0 ]; then
    echo "⚠ 需要集成HAL库才能编译"
    echo "请参考 BUILD_GUIDE.md 完成HAL库集成"
else
    echo "✓ 项目已就绪，可以编译"
    echo ""
    echo "编译命令:"
    echo "  make clean"
    echo "  make all"
fi

echo "=========================================="

