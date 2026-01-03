/**
 * @file ui_status.h
 * @brief 用户界面和状态提示模块
 */

#ifndef UI_STATUS_H
#define UI_STATUS_H

#include <stdint.h>

// 状态枚举
typedef enum {
    UI_STATUS_IDLE = 0,
    UI_STATUS_SCANNING_QR,
    UI_STATUS_DOWNLOADING,
    UI_STATUS_VERIFYING,
    UI_STATUS_WRITING_FLASH,
    UI_STATUS_UPGRADE_COMPLETE,
    UI_STATUS_UPGRADE_FAILED,
    UI_STATUS_REBOOTING
} ui_status_t;

// 错误类型
typedef enum {
    UI_ERROR_NONE = 0,
    UI_ERROR_QR_SCAN_FAILED,
    UI_ERROR_DOWNLOAD_FAILED,
    UI_ERROR_VERIFY_FAILED,
    UI_ERROR_FLASH_WRITE_FAILED,
    UI_ERROR_VERSION_CHECK_FAILED,
    UI_ERROR_NETWORK_ERROR
} ui_error_t;

/**
 * @brief 初始化用户界面
 */
void ui_init(void);

/**
 * @brief 更新状态显示
 * @param status 当前状态
 */
void ui_update_status(ui_status_t status);

/**
 * @brief 显示错误信息
 * @param error 错误类型
 */
void ui_show_error(ui_error_t error);

/**
 * @brief 显示进度信息
 * @param progress 进度百分比（0-100）
 */
void ui_show_progress(uint8_t progress);

/**
 * @brief 显示文本消息
 * @param message 消息文本
 */
void ui_show_message(const char *message);

/**
 * @brief 清除显示
 */
void ui_clear(void);

#endif // UI_STATUS_H

