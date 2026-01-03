/**
 * @file ota_manager.h
 * @brief OTA升级管理器
 */

#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

// OTA状态
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_SCANNING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_WRITING,
    OTA_STATE_COMPLETE,
    OTA_STATE_FAILED
} ota_state_t;

// OTA错误码
typedef enum {
    OTA_ERROR_NONE = 0,
    OTA_ERROR_QR_SCAN_FAILED,
    OTA_ERROR_INVALID_URL,
    OTA_ERROR_DOWNLOAD_FAILED,
    OTA_ERROR_VERIFY_FAILED,
    OTA_ERROR_FLASH_ERASE_FAILED,
    OTA_ERROR_FLASH_WRITE_FAILED,
    OTA_ERROR_VERSION_CHECK_FAILED,
    OTA_ERROR_NETWORK_ERROR
} ota_error_t;

/**
 * @brief 初始化OTA管理器
 */
void ota_manager_init(void);

/**
 * @brief 启动OTA升级流程
 * @return 0成功，-1失败
 */
int ota_start_upgrade(void);

/**
 * @brief 处理OTA升级流程（需要在主循环中调用）
 */
void ota_process(void);

/**
 * @brief 获取当前OTA状态
 */
ota_state_t ota_get_state(void);

/**
 * @brief 获取OTA错误码
 */
ota_error_t ota_get_error(void);

/**
 * @brief 取消OTA升级
 */
void ota_cancel(void);

/**
 * @brief 检查是否有待处理的OTA升级
 */
bool ota_has_pending_upgrade(void);

#endif // OTA_MANAGER_H

