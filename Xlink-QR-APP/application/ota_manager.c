/**
 * @file ota_manager.c
 * @brief OTA升级管理器实现
 */

#include "ota_manager.h"
#include "../common/qr_scanner.h"
#include "../common/firmware_download.h"
#include "../common/flash_manager.h"
#include "../common/version_control.h"
#include "../common/ui_status.h"
#include "../drivers/stm32_hal_wrapper.h"
#include <string.h>
#include <stdlib.h>

// OTA状态机
static ota_state_t g_ota_state = OTA_STATE_IDLE;
static ota_error_t g_ota_error = OTA_ERROR_NONE;

// 固件缓冲区（需要根据实际RAM大小调整）
#define FIRMWARE_BUFFER_SIZE     (28 * 1024)  // 28KB，匹配分区大小
static uint8_t g_firmware_buffer[FIRMWARE_BUFFER_SIZE];

// 固件信息
static char g_firmware_url[QR_URL_MAX_LEN];
static uint32_t g_firmware_size = 0;
static firmware_version_t g_target_version;

/**
 * @brief 初始化OTA管理器
 */
void ota_manager_init(void)
{
    g_ota_state = OTA_STATE_IDLE;
    g_ota_error = OTA_ERROR_NONE;
    memset(g_firmware_url, 0, sizeof(g_firmware_url));
    g_firmware_size = 0;
}

/**
 * @brief 下载进度回调
 */
static void download_progress_callback(uint32_t downloaded, uint32_t total)
{
    if (total > 0) {
        uint8_t progress = (uint8_t)((downloaded * 100) / total);
        ui_show_progress(progress);
    }
}

/**
 * @brief 下载状态回调
 */
static void download_status_callback(download_status_t status)
{
    switch (status) {
        case DOWNLOAD_CONNECTING:
            ui_update_status(UI_STATUS_DOWNLOADING);
            break;
        case DOWNLOAD_DOWNLOADING:
            ui_update_status(UI_STATUS_DOWNLOADING);
            break;
        case DOWNLOAD_VERIFYING:
            ui_update_status(UI_STATUS_VERIFYING);
            break;
        case DOWNLOAD_COMPLETE:
            // 状态由主流程控制
            break;
        case DOWNLOAD_FAILED:
            g_ota_state = OTA_STATE_FAILED;
            g_ota_error = OTA_ERROR_DOWNLOAD_FAILED;
            ui_show_error(UI_ERROR_DOWNLOAD_FAILED);
            break;
        default:
            break;
    }
}

/**
 * @brief 步骤1：扫描二维码获取URL
 */
static int ota_step_scan_qr(void)
{
    ui_update_status(UI_STATUS_SCANNING_QR);
    g_ota_state = OTA_STATE_SCANNING;
    
    int ret = qr_scanner_scan(g_firmware_url, sizeof(g_firmware_url), 30000);  // 30秒超时
    
    if (ret != 0) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_QR_SCAN_FAILED;
        ui_show_error(UI_ERROR_QR_SCAN_FAILED);
        return -1;
    }
    
    if (!qr_validate_url(g_firmware_url)) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_INVALID_URL;
        ui_show_error(UI_ERROR_NETWORK_ERROR);
        return -1;
    }
    
    return 0;
}

/**
 * @brief 步骤2：下载固件
 */
static int ota_step_download(void)
{
    ui_update_status(UI_STATUS_DOWNLOADING);
    g_ota_state = OTA_STATE_DOWNLOADING;
    
    int ret = firmware_download_from_url(
        g_firmware_url,
        g_firmware_buffer,
        FIRMWARE_BUFFER_SIZE,
        &g_firmware_size,
        download_progress_callback,
        download_status_callback
    );
    
    if (ret != 0) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_DOWNLOAD_FAILED;
        ui_show_error(UI_ERROR_DOWNLOAD_FAILED);
        return -1;
    }
    
    if (g_firmware_size == 0 || g_firmware_size > FIRMWARE_BUFFER_SIZE) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_DOWNLOAD_FAILED;
        ui_show_error(UI_ERROR_DOWNLOAD_FAILED);
        return -1;
    }
    
    return 0;
}

/**
 * @brief 步骤3：验证固件
 */
static int ota_step_verify(void)
{
    ui_update_status(UI_STATUS_VERIFYING);
    g_ota_state = OTA_STATE_VERIFYING;
    
    // 提取目标版本
    if (version_extract_from_firmware(g_firmware_buffer, g_firmware_size, 
                                      &g_target_version) != 0) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_VERSION_CHECK_FAILED;
        ui_show_error(UI_ERROR_VERSION_CHECK_FAILED);
        return -1;
    }
    
    // 检查版本（允许降级）
    firmware_version_t current_version;
    if (version_get_current(&current_version) == 0) {
        if (!version_need_update(&current_version, &g_target_version)) {
            // 版本相同，不需要更新
            g_ota_state = OTA_STATE_FAILED;
            g_ota_error = OTA_ERROR_VERSION_CHECK_FAILED;
            ui_show_message("固件版本相同，无需更新");
            return -1;
        }
        // 版本不同，允许更新（包括降级）
    }
    
    // CRC32校验（从固件头部读取期望的CRC32值）
    // 这里需要根据实际固件格式解析CRC32
    // uint32_t expected_crc = extract_crc32_from_firmware(g_firmware_buffer);
    // if (!firmware_verify_crc32(g_firmware_buffer, g_firmware_size, expected_crc)) {
    //     g_ota_state = OTA_STATE_FAILED;
    //     g_ota_error = OTA_ERROR_VERIFY_FAILED;
    //     ui_show_error(UI_ERROR_VERIFY_FAILED);
    //     return -1;
    // }
    
    return 0;
}

/**
 * @brief 步骤4：写入Flash
 */
static int ota_step_write_flash(void)
{
    ui_update_status(UI_STATUS_WRITING_FLASH);
    g_ota_state = OTA_STATE_WRITING;
    
    // 获取目标分区
    partition_t target_partition = flash_get_target_partition();
    
    // 标记当前分区为无效（防止启动失败时回滚到旧版本）
    partition_t current_partition = flash_get_current_partition();
    if (current_partition != PARTITION_NONE) {
        flash_mark_partition_invalid(current_partition);
    }
    
    // 擦除目标分区
    if (flash_erase_partition(target_partition) != 0) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_FLASH_ERASE_FAILED;
        ui_show_error(UI_ERROR_FLASH_WRITE_FAILED);
        return -1;
    }
    
    // 写入固件数据
    if (flash_write_partition(target_partition, 0, g_firmware_buffer, 
                              g_firmware_size) != 0) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_FLASH_WRITE_FAILED;
        ui_show_error(UI_ERROR_FLASH_WRITE_FAILED);
        return -1;
    }
    
    // 写入分区信息
    partition_info_t partition_info;
    partition_info.magic = PARTITION_MAGIC;
    partition_info.version = g_target_version.major << 24 | 
                           g_target_version.minor << 16 |
                           g_target_version.revision << 8 |
                           g_target_version.build;
    partition_info.crc32 = calculate_crc32(g_firmware_buffer, g_firmware_size);
    partition_info.size = g_firmware_size;
    partition_info.status = PARTITION_VALID;
    memset(partition_info.reserved, 0, sizeof(partition_info.reserved));
    
    if (flash_write_partition_info(target_partition, &partition_info) != 0) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_FLASH_WRITE_FAILED;
        ui_show_error(UI_ERROR_FLASH_WRITE_FAILED);
        return -1;
    }
    
    // 验证写入的数据
    if (!flash_verify_partition(target_partition)) {
        g_ota_state = OTA_STATE_FAILED;
        g_ota_error = OTA_ERROR_VERIFY_FAILED;
        ui_show_error(UI_ERROR_VERIFY_FAILED);
        return -1;
    }
    
    return 0;
}

/**
 * @brief 启动OTA升级流程
 */
int ota_start_upgrade(void)
{
    if (g_ota_state != OTA_STATE_IDLE) {
        return -1;  // 已有升级在进行
    }
    
    g_ota_error = OTA_ERROR_NONE;
    return 0;
}

/**
 * @brief 处理OTA升级流程
 */
void ota_process(void)
{
    switch (g_ota_state) {
        case OTA_STATE_IDLE:
            if (ota_has_pending_upgrade()) {
                g_ota_state = OTA_STATE_SCANNING;
            }
            break;
            
        case OTA_STATE_SCANNING:
            if (ota_step_scan_qr() == 0) {
                // 扫描成功，进入下载状态
                g_ota_state = OTA_STATE_DOWNLOADING;
            }
            break;
            
        case OTA_STATE_DOWNLOADING:
            if (ota_step_download() == 0) {
                // 下载成功，进入验证状态
                g_ota_state = OTA_STATE_VERIFYING;
            }
            break;
            
        case OTA_STATE_VERIFYING:
            if (ota_step_verify() == 0) {
                // 验证成功，进入写入状态
                g_ota_state = OTA_STATE_WRITING;
            }
            break;
            
        case OTA_STATE_WRITING:
            if (ota_step_write_flash() == 0) {
                // 写入成功，升级完成
                g_ota_state = OTA_STATE_COMPLETE;
                ui_update_status(UI_STATUS_UPGRADE_COMPLETE);
                
                // 延迟后重启
                ui_update_status(UI_STATUS_REBOOTING);
                delay_ms(2000);
                system_reset();
            }
            break;
            
        case OTA_STATE_COMPLETE:
            // 升级完成，等待重启
            break;
            
        case OTA_STATE_FAILED:
            // 升级失败，等待用户重试
            break;
            
        default:
            break;
    }
}

/**
 * @brief 获取当前OTA状态
 */
ota_state_t ota_get_state(void)
{
    return g_ota_state;
}

/**
 * @brief 获取OTA错误码
 */
ota_error_t ota_get_error(void)
{
    return g_ota_error;
}

/**
 * @brief 取消OTA升级
 */
void ota_cancel(void)
{
    if (g_ota_state != OTA_STATE_IDLE && 
        g_ota_state != OTA_STATE_COMPLETE &&
        g_ota_state != OTA_STATE_FAILED) {
        g_ota_state = OTA_STATE_IDLE;
        g_ota_error = OTA_ERROR_NONE;
    }
}

/**
 * @brief 检查是否有待处理的OTA升级
 */
bool ota_has_pending_upgrade(void)
{
    // 可以通过GPIO、UART命令等方式触发
    // 示例：检查按键或UART命令
    // return (gpio_read_upgrade_button() == PRESSED);
    return false;
}

