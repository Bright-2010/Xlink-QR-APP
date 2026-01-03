/**
 * @file firmware_download.h
 * @brief 固件下载与校验模块
 */

#ifndef FIRMWARE_DOWNLOAD_H
#define FIRMWARE_DOWNLOAD_H

#include <stdint.h>
#include <stdbool.h>

// 下载状态
typedef enum {
    DOWNLOAD_IDLE = 0,
    DOWNLOAD_CONNECTING,
    DOWNLOAD_DOWNLOADING,
    DOWNLOAD_COMPLETE,
    DOWNLOAD_FAILED,
    DOWNLOAD_VERIFYING
} download_status_t;

// 下载回调函数类型
typedef void (*download_progress_cb)(uint32_t downloaded, uint32_t total);
typedef void (*download_status_cb)(download_status_t status);

/**
 * @brief 初始化固件下载模块
 * @param network_handle 网络句柄（如WiFi、Ethernet等）
 */
void firmware_download_init(void *network_handle);

/**
 * @brief 从URL下载固件
 * @param url 固件下载URL
 * @param buffer 存储缓冲区
 * @param buffer_size 缓冲区大小
 * @param downloaded_size 实际下载大小（输出）
 * @param progress_cb 进度回调函数
 * @param status_cb 状态回调函数
 * @return 0成功，-1失败
 */
int firmware_download_from_url(const char *url,
                               uint8_t *buffer,
                               uint32_t buffer_size,
                               uint32_t *downloaded_size,
                               download_progress_cb progress_cb,
                               download_status_cb status_cb);

/**
 * @brief 计算CRC32校验值
 * @param data 数据指针
 * @param size 数据大小
 * @return CRC32值
 */
uint32_t calculate_crc32(const uint8_t *data, uint32_t size);

/**
 * @brief 验证固件CRC32
 * @param data 固件数据
 * @param size 数据大小
 * @param expected_crc 期望的CRC32值
 * @return true校验通过，false校验失败
 */
bool firmware_verify_crc32(const uint8_t *data, uint32_t size, uint32_t expected_crc);

/**
 * @brief 计算MD5哈希值（可选，用于更严格的校验）
 * @param data 数据指针
 * @param size 数据大小
 * @param hash_output 输出哈希值（16字节）
 * @return 0成功，-1失败
 */
int calculate_md5(const uint8_t *data, uint32_t size, uint8_t *hash_output);

/**
 * @brief 验证固件MD5哈希
 * @param data 固件数据
 * @param size 数据大小
 * @param expected_hash 期望的哈希值（16字节）
 * @return true校验通过，false校验失败
 */
bool firmware_verify_md5(const uint8_t *data, uint32_t size, const uint8_t *expected_hash);

#endif // FIRMWARE_DOWNLOAD_H

