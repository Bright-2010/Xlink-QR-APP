/**
 * @file ui_status.c
 * @brief 用户界面和状态提示实现
 */

#include "ui_status.h"
#include "../drivers/stm32_hal_wrapper.h"
#include <string.h>
#include <stdio.h>

// 状态文本映射
static const char *status_texts[] = {
    "就绪",
    "正在扫描二维码...",
    "正在下载固件...",
    "正在验证固件...",
    "正在写入Flash...",
    "升级完成",
    "升级失败",
    "正在重启..."
};

// 错误文本映射
static const char *error_texts[] = {
    "无错误",
    "二维码扫描失败",
    "固件下载失败",
    "固件验证失败",
    "Flash写入失败",
    "版本检查失败",
    "网络错误"
};

// 调试UART配置
#define DEBUG_UART_NUM  2

/**
 * @brief 初始化用户界面
 */
void ui_init(void)
{
    // 初始化调试UART
    uart_init(DEBUG_UART_NUM, 115200);
}

/**
 * @brief 更新状态显示
 */
void ui_update_status(ui_status_t status)
{
    if (status >= sizeof(status_texts) / sizeof(status_texts[0])) {
        return;
    }
    
    const char *text = status_texts[status];
    
    // 通过UART输出
    uart_send_string(DEBUG_UART_NUM, text);
    uart_send_string(DEBUG_UART_NUM, "\r\n");
}

/**
 * @brief 显示错误信息
 */
void ui_show_error(ui_error_t error)
{
    if (error >= sizeof(error_texts) / sizeof(error_texts[0])) {
        return;
    }
    
    const char *text = error_texts[error];
    
    // 显示错误信息
    uart_send_string(DEBUG_UART_NUM, "错误: ");
    uart_send_string(DEBUG_UART_NUM, text);
    uart_send_string(DEBUG_UART_NUM, "\r\n");
}

/**
 * @brief 显示进度信息
 */
void ui_show_progress(uint8_t progress)
{
    if (progress > 100) {
        progress = 100;
    }
    
    // 通过UART输出进度
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "进度: %d%%\r\n", progress);
    uart_send_string(DEBUG_UART_NUM, buffer);
}

/**
 * @brief 显示文本消息
 */
void ui_show_message(const char *message)
{
    if (message == NULL) {
        return;
    }
    
    uart_send_string(DEBUG_UART_NUM, message);
    uart_send_string(DEBUG_UART_NUM, "\r\n");
}

/**
 * @brief 清除显示
 */
void ui_clear(void)
{
    uart_send_string(DEBUG_UART_NUM, "\033[2J");  // ANSI清屏
}

