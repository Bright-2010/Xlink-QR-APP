/**
 * @file flash_manager.c
 * @brief Flash A/B双分区管理实现
 */

#include "flash_manager.h"
#include "../drivers/stm32_hal_wrapper.h"
#include <string.h>

/**
 * @brief 初始化Flash管理器
 */
void flash_manager_init(void)
{
    // 初始化Flash接口
    flash_unlock();
}

/**
 * @brief 获取当前运行的分区
 * 通过检查PC地址判断当前运行在哪个分区
 */
partition_t flash_get_current_partition(void)
{
    uint32_t pc = (uint32_t)flash_get_current_partition;
    
    if (pc >= APP_A_BASE_ADDR && pc < APP_A_END_ADDR) {
        return PARTITION_A;
    } else if (pc >= APP_B_BASE_ADDR && pc < APP_B_END_ADDR) {
        return PARTITION_B;
    }
    
    return PARTITION_NONE;
}

/**
 * @brief 获取目标分区（用于写入新固件）
 */
partition_t flash_get_target_partition(void)
{
    partition_t current = flash_get_current_partition();
    
    // 如果当前在A分区，则写入B分区；反之亦然
    if (current == PARTITION_A) {
        return PARTITION_B;
    } else if (current == PARTITION_B) {
        return PARTITION_A;
    }
    
    // 默认写入A分区
    return PARTITION_A;
}

/**
 * @brief 擦除目标分区
 */
int flash_erase_partition(partition_t partition)
{
    uint32_t base_addr = flash_get_partition_base(partition);
    uint32_t end_addr = base_addr + PARTITION_SIZE;
    
    flash_unlock();
    
    // 擦除分区内的所有页
    for (uint32_t addr = base_addr; addr < end_addr; addr += FLASH_PAGE_SIZE) {
        int ret = flash_erase_page(addr);
        if (ret != 0) {
            flash_lock();
            return -1;
        }
    }
    
    flash_lock();
    return 0;
}

/**
 * @brief 写入数据到指定分区
 */
int flash_write_partition(partition_t partition, uint32_t offset,
                         const uint8_t *data, uint32_t size)
{
    if (offset + size > PARTITION_SIZE) {
        return -1;  // 超出分区大小
    }
    
    uint32_t base_addr = flash_get_partition_base(partition);
    uint32_t write_addr = base_addr + offset;
    
    flash_unlock();
    
    // 按字（4字节）写入
    uint32_t words = size / 4;
    uint32_t remainder = size % 4;
    const uint32_t *word_data = (const uint32_t *)data;
    
    for (uint32_t i = 0; i < words; i++) {
        int ret = flash_program_word(write_addr + i * 4, word_data[i]);
        if (ret != 0) {
            flash_lock();
            return -1;
        }
    }
    
    // 处理剩余字节
    if (remainder > 0) {
        uint32_t last_word = 0;
        memcpy(&last_word, data + words * 4, remainder);
        int ret = flash_program_word(write_addr + words * 4, last_word);
        if (ret != 0) {
            flash_lock();
            return -1;
        }
    }
    
    flash_lock();
    return 0;
}

/**
 * @brief 读取分区数据
 */
int flash_read_partition(partition_t partition, uint32_t offset,
                        uint8_t *data, uint32_t size)
{
    if (offset + size > PARTITION_SIZE) {
        return -1;
    }
    
    uint32_t base_addr = flash_get_partition_base(partition);
    uint32_t read_addr = base_addr + offset;
    
    memcpy(data, (const void *)read_addr, size);
    return 0;
}

/**
 * @brief 写入分区信息
 */
int flash_write_partition_info(partition_t partition,
                               const partition_info_t *info)
{
    uint32_t base_addr = flash_get_partition_base(partition);
    uint32_t info_addr = base_addr + PARTITION_SIZE - sizeof(partition_info_t);
    
    flash_unlock();
    
    const uint32_t *info_words = (const uint32_t *)info;
    uint32_t word_count = sizeof(partition_info_t) / 4;
    
    for (uint32_t i = 0; i < word_count; i++) {
        int ret = flash_program_word(info_addr + i * 4, info_words[i]);
        if (ret != 0) {
            flash_lock();
            return -1;
        }
    }
    
    flash_lock();
    return 0;
}

/**
 * @brief 读取分区信息
 */
int flash_read_partition_info(partition_t partition,
                              partition_info_t *info)
{
    uint32_t base_addr = flash_get_partition_base(partition);
    uint32_t info_addr = base_addr + PARTITION_SIZE - sizeof(partition_info_t);
    
    memcpy(info, (const void *)info_addr, sizeof(partition_info_t));
    
    // 验证魔数
    if (info->magic != PARTITION_MAGIC) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief 标记分区为有效
 */
int flash_mark_partition_valid(partition_t partition)
{
    partition_info_t info;
    
    if (flash_read_partition_info(partition, &info) != 0) {
        return -1;
    }
    
    info.status = PARTITION_VALID;
    return flash_write_partition_info(partition, &info);
}

/**
 * @brief 标记分区为无效
 */
int flash_mark_partition_invalid(partition_t partition)
{
    partition_info_t info;
    
    if (flash_read_partition_info(partition, &info) != 0) {
        return -1;
    }
    
    info.status = PARTITION_INVALID;
    return flash_write_partition_info(partition, &info);
}

/**
 * @brief 验证分区完整性（CRC32校验）
 */
bool flash_verify_partition(partition_t partition)
{
    partition_info_t info;
    
    if (flash_read_partition_info(partition, &info) != 0) {
        return false;
    }
    
    if (info.magic != PARTITION_MAGIC) {
        return false;
    }
    
    if (info.status != PARTITION_VALID) {
        return false;
    }
    
    // 计算实际CRC32
    // calculate_crc32在firmware_download.c中定义
    extern uint32_t calculate_crc32(const uint8_t *data, uint32_t size);
    
    uint32_t base_addr = flash_get_partition_base(partition);
    uint32_t data_size = PARTITION_SIZE - sizeof(partition_info_t);
    uint32_t calculated_crc = calculate_crc32((const uint8_t *)base_addr, data_size);
    
    return (calculated_crc == info.crc32);
}

/**
 * @brief 获取分区基地址
 */
uint32_t flash_get_partition_base(partition_t partition)
{
    switch (partition) {
        case PARTITION_A:
            return APP_A_BASE_ADDR;
        case PARTITION_B:
            return APP_B_BASE_ADDR;
        default:
            return 0;
    }
}

