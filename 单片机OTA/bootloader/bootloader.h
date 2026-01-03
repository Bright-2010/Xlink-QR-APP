/**
 * @file bootloader.h
 * @brief Bootloader主头文件
 */

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "../common/flash_manager.h"

// Bootloader配置
#define BOOTLOADER_TIMEOUT_MS    5000  // 5秒超时，等待升级命令

/**
 * @brief Bootloader初始化
 */
void bootloader_init(void);

/**
 * @brief Bootloader主循环
 */
void bootloader_main(void);

/**
 * @brief 检查是否需要进入Bootloader模式
 * @return true进入Bootloader，false直接启动应用
 */
bool bootloader_should_enter(void);

/**
 * @brief 选择并启动应用分区
 */
void bootloader_start_application(void);

/**
 * @brief 验证分区并决定启动哪个分区
 * @return 有效的分区，如果都无效返回PARTITION_NONE
 */
partition_t bootloader_select_partition(void);

/**
 * @brief 跳转到应用程序
 * @param app_addr 应用程序起始地址
 */
void bootloader_jump_to_app(uint32_t app_addr);

/**
 * @brief 系统复位
 */
void bootloader_system_reset(void);

#endif // BOOTLOADER_H

