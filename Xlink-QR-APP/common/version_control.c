/**
 * @file version_control.c
 * @brief 固件版本控制实现
 */

#include "version_control.h"
#include "flash_manager.h"
#include <string.h>
#include <stdio.h>

// 固件版本信息在固件中的偏移地址（通常在固件头部）
#define FIRMWARE_VERSION_OFFSET  0x200  // 示例偏移地址

/**
 * @brief 比较两个版本号
 */
int version_compare(const firmware_version_t *v1, const firmware_version_t *v2)
{
    if (v1->major != v2->major) {
        return (int)v1->major - (int)v2->major;
    }
    if (v1->minor != v2->minor) {
        return (int)v1->minor - (int)v2->minor;
    }
    if (v1->revision != v2->revision) {
        return (int)v1->revision - (int)v2->revision;
    }
    return (int)v1->build - (int)v2->build;
}

/**
 * @brief 从字符串解析版本号
 */
int version_parse(const char *version_str, firmware_version_t *version)
{
    if (version_str == NULL || version == NULL) {
        return -1;
    }
    
    int major, minor, revision, build;
    int ret = sscanf(version_str, "%d.%d.%d.%d", &major, &minor, &revision, &build);
    
    if (ret != 4) {
        return -1;
    }
    
    version->major = (uint8_t)major;
    version->minor = (uint8_t)minor;
    version->revision = (uint8_t)revision;
    version->build = (uint8_t)build;
    
    return 0;
}

/**
 * @brief 版本号转字符串
 */
int version_to_string(const firmware_version_t *version, char *buffer, uint32_t buffer_size)
{
    if (version == NULL || buffer == NULL || buffer_size < 16) {
        return -1;
    }
    
    snprintf(buffer, buffer_size, "%d.%d.%d.%d",
             version->major, version->minor, version->revision, version->build);
    
    return 0;
}

/**
 * @brief 获取当前运行固件的版本号
 */
int version_get_current(firmware_version_t *version)
{
    if (version == NULL) {
        return -1;
    }
    
    partition_t current_partition = flash_get_current_partition();
    if (current_partition == PARTITION_NONE) {
        return -1;
    }
    
    uint32_t base_addr = flash_get_partition_base(current_partition);
    
    // 从Flash中读取版本信息
    memcpy(version, (const void *)(base_addr + FIRMWARE_VERSION_OFFSET), 
           sizeof(firmware_version_t));
    
    return 0;
}

/**
 * @brief 从固件数据中提取版本号
 */
int version_extract_from_firmware(const uint8_t *firmware_data, 
                                  uint32_t firmware_size,
                                  firmware_version_t *version)
{
    if (firmware_data == NULL || version == NULL) {
        return -1;
    }
    
    if (FIRMWARE_VERSION_OFFSET + sizeof(firmware_version_t) > firmware_size) {
        return -1;
    }
    
    memcpy(version, firmware_data + FIRMWARE_VERSION_OFFSET, 
           sizeof(firmware_version_t));
    
    return 0;
}

/**
 * @brief 检查是否需要更新
 * @note 允许降级，只要版本不同就允许更新
 */
bool version_need_update(const firmware_version_t *current_version,
                         const firmware_version_t *target_version)
{
    if (current_version == NULL || target_version == NULL) {
        return false;
    }
    
    // 如果版本不同，则允许更新（包括降级）
    return (version_compare(target_version, current_version) != 0);
}

