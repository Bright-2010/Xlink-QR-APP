#!/bin/bash
# HAL库集成脚本
# 用于从STM32CubeMX项目复制HAL库文件

set -e

echo "=========================================="
echo "  STM32 HAL库集成脚本"
echo "=========================================="

# 检查参数
if [ $# -lt 1 ]; then
    echo "用法: $0 <CubeMX项目路径>"
    echo "示例: $0 ~/STM32CubeMX/MyProject"
    exit 1
fi

CUBEMX_PROJECT=$1
HAL_DIR="HAL"

# 检查CubeMX项目路径
if [ ! -d "$CUBEMX_PROJECT" ]; then
    echo "错误: CubeMX项目路径不存在: $CUBEMX_PROJECT"
    exit 1
fi

# 检查Drivers目录
if [ ! -d "$CUBEMX_PROJECT/Drivers" ]; then
    echo "错误: 找不到Drivers目录，请确认这是STM32CubeMX生成的项目"
    exit 1
fi

echo "正在从 $CUBEMX_PROJECT 复制HAL库文件..."

# 创建HAL目录结构
mkdir -p $HAL_DIR/STM32F1xx_HAL_Driver/Inc
mkdir -p $HAL_DIR/STM32F1xx_HAL_Driver/Src
mkdir -p $HAL_DIR/CMSIS/Device/ST/STM32F1xx/Include
mkdir -p $HAL_DIR/CMSIS/Include

# 复制HAL驱动库
if [ -d "$CUBEMX_PROJECT/Drivers/STM32F1xx_HAL_Driver" ]; then
    echo "复制 STM32F1xx_HAL_Driver..."
    cp -r $CUBEMX_PROJECT/Drivers/STM32F1xx_HAL_Driver/Inc/* $HAL_DIR/STM32F1xx_HAL_Driver/Inc/ 2>/dev/null || true
    cp -r $CUBEMX_PROJECT/Drivers/STM32F1xx_HAL_Driver/Src/* $HAL_DIR/STM32F1xx_HAL_Driver/Src/ 2>/dev/null || true
    echo "✓ HAL驱动库复制完成"
else
    echo "警告: 找不到STM32F1xx_HAL_Driver目录"
fi

# 复制CMSIS文件
if [ -d "$CUBEMX_PROJECT/Drivers/CMSIS" ]; then
    echo "复制 CMSIS文件..."
    cp -r $CUBEMX_PROJECT/Drivers/CMSIS/Device/ST/STM32F1xx/Include/* $HAL_DIR/CMSIS/Device/ST/STM32F1xx/Include/ 2>/dev/null || true
    cp -r $CUBEMX_PROJECT/Drivers/CMSIS/Include/* $HAL_DIR/CMSIS/Include/ 2>/dev/null || true
    echo "✓ CMSIS文件复制完成"
else
    echo "警告: 找不到CMSIS目录"
fi

# 验证关键文件
echo ""
echo "验证关键文件..."
MISSING_FILES=0

# 检查HAL库关键文件
if [ ! -f "$HAL_DIR/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h" ]; then
    echo "✗ 缺少: stm32f1xx_hal.h"
    MISSING_FILES=$((MISSING_FILES + 1))
fi

if [ ! -f "$HAL_DIR/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_flash.h" ]; then
    echo "✗ 缺少: stm32f1xx_hal_flash.h"
    MISSING_FILES=$((MISSING_FILES + 1))
fi

if [ ! -f "$HAL_DIR/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal_uart.h" ]; then
    echo "✗ 缺少: stm32f1xx_hal_uart.h"
    MISSING_FILES=$((MISSING_FILES + 1))
fi

# 检查CMSIS关键文件
if [ ! -f "$HAL_DIR/CMSIS/Include/core_cm3.h" ]; then
    echo "✗ 缺少: core_cm3.h"
    MISSING_FILES=$((MISSING_FILES + 1))
fi

if [ ! -f "$HAL_DIR/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h" ]; then
    echo "✗ 缺少: stm32f1xx.h"
    MISSING_FILES=$((MISSING_FILES + 1))
fi

if [ $MISSING_FILES -eq 0 ]; then
    echo "✓ 所有关键文件已就绪"
    echo ""
    echo "=========================================="
    echo "  HAL库集成完成！"
    echo "=========================================="
    echo ""
    echo "下一步："
    echo "1. 编辑 config.h 配置硬件参数"
    echo "2. 运行 make clean && make all 编译项目"
else
    echo ""
    echo "警告: 缺少 $MISSING_FILES 个关键文件"
    echo "请检查CubeMX项目路径是否正确"
fi

