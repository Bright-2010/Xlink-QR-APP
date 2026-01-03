/**
 * @file flash_manager.h
 * @brief Flash A/B双分区管理模块
 */

#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

// Flash分区配置（STM32F108T6: 64KB Flash）
#define FLASH_BASE_ADDR          0x08000000
#define FLASH_SIZE               (64 * 1024)  // 64KB
#define FLASH_PAGE_SIZE          1024         // 1KB per page

// Bootloader占用前8KB
#define BOOTLOADER_SIZE          (8 * 1024)
#define BOOTLOADER_END_ADDR      (FLASH_BASE_ADDR + BOOTLOADER_SIZE)

// A/B分区配置（每个分区28KB）
#define PARTITION_SIZE           (28 * 1024)
#define APP_A_BASE_ADDR          BOOTLOADER_END_ADDR
#define APP_A_END_ADDR           (APP_A_BASE_ADDR + PARTITION_SIZE)
#define APP_B_BASE_ADDR          APP_A_END_ADDR
#define APP_B_END_ADDR           (APP_B_BASE_ADDR + PARTITION_SIZE)

// 分区状态标志
#define PARTITION_MAGIC          0xABCD1234
#define PARTITION_VALID          0x00000001
#define PARTITION_INVALID        0x00000000

// 分区信息结构（位于每个分区末尾）
typedef struct {
    uint32_t magic;              // 魔数
    uint32_t version;            // 固件版本
    uint32_t crc32;              // CRC32校验值
    uint32_t size;               // 固件大小
    uint32_t status;             // 分区状态（VALID/INVALID）
    uint32_t reserved[3];        // 保留字段
} partition_info_t;

// 分区枚举
typedef enum {
    PARTITION_A = 0,
    PARTITION_B = 1,
    PARTITION_NONE = 2
} partition_t;

/**
 * @brief 初始化Flash管理器
 */
void flash_manager_init(void);

/**
 * @brief 获取当前运行的分区
 */
partition_t flash_get_current_partition(void);

/**
 * @brief 获取目标分区（用于写入新固件）
 */
partition_t flash_get_target_partition(void);

/**
 * @brief 擦除目标分区
 */
int flash_erase_partition(partition_t partition);

/**
 * @brief 写入数据到指定分区
 * @param partition 目标分区
 * @param offset 分区内偏移地址
 * @param data 数据指针
 * @param size 数据大小
 */
int flash_write_partition(partition_t partition, uint32_t offset, 
                         const uint8_t *data, uint32_t size);

/**
 * @brief 读取分区数据
 */
int flash_read_partition(partition_t partition, uint32_t offset,
                        uint8_t *data, uint32_t size);

/**
 * @brief 写入分区信息
 */
int flash_write_partition_info(partition_t partition, 
                               const partition_info_t *info);

/**
 * @brief 读取分区信息
 */
int flash_read_partition_info(partition_t partition, 
                              partition_info_t *info);

/**
 * @brief 标记分区为有效
 */
int flash_mark_partition_valid(partition_t partition);

/**
 * @brief 标记分区为无效
 */
int flash_mark_partition_invalid(partition_t partition);

/**
 * @brief 验证分区完整性（CRC32校验）
 */
bool flash_verify_partition(partition_t partition);

/**
 * @brief 获取分区基地址
 */
uint32_t flash_get_partition_base(partition_t partition);

#endif // FLASH_MANAGER_H

