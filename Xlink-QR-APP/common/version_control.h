/**
 * @file version_control.h
 * @brief 固件版本控制模块
 */

#ifndef VERSION_CONTROL_H
#define VERSION_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

// 版本号结构（主版本.次版本.修订版本.构建号）
typedef struct {
    uint8_t major;      // 主版本号
    uint8_t minor;      // 次版本号
    uint8_t revision;   // 修订版本号
    uint8_t build;      // 构建号
} firmware_version_t;

/**
 * @brief 比较两个版本号
 * @param v1 版本1
 * @param v2 版本2
 * @return >0 v1 > v2, =0 v1 == v2, <0 v1 < v2
 */
int version_compare(const firmware_version_t *v1, const firmware_version_t *v2);

/**
 * @brief 从字符串解析版本号
 * @param version_str 版本字符串（格式：major.minor.revision.build）
 * @param version 输出版本结构
 * @return 0成功，-1失败
 */
int version_parse(const char *version_str, firmware_version_t *version);

/**
 * @brief 版本号转字符串
 * @param version 版本结构
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 0成功，-1失败
 */
int version_to_string(const firmware_version_t *version, char *buffer, uint32_t buffer_size);

/**
 * @brief 获取当前运行固件的版本号
 * @param version 输出版本结构
 * @return 0成功，-1失败
 */
int version_get_current(firmware_version_t *version);

/**
 * @brief 从固件数据中提取版本号
 * @param firmware_data 固件数据
 * @param firmware_size 固件大小
 * @param version 输出版本结构
 * @return 0成功，-1失败
 */
int version_extract_from_firmware(const uint8_t *firmware_data, 
                                  uint32_t firmware_size,
                                  firmware_version_t *version);

/**
 * @brief 检查是否需要更新
 * @param current_version 当前版本
 * @param target_version 目标版本
 * @return true需要更新，false不需要更新
 */
bool version_need_update(const firmware_version_t *current_version,
                         const firmware_version_t *target_version);

#endif // VERSION_CONTROL_H

