/**
 * @file bootloader.c
 * @brief Bootloader主实现
 */

#include "bootloader.h"
#include "../common/flash_manager.h"
#include "../drivers/stm32_hal_wrapper.h"
#include <string.h>

// 向量表结构
typedef struct {
    uint32_t stack_pointer;
    void (*reset_handler)(void);
} vector_table_t;

/**
 * @brief Bootloader初始化
 */
void bootloader_init(void)
{
    // 初始化系统时钟
    // SystemInit();
    
    // 初始化Flash管理器
    flash_manager_init();
    
    // 初始化UART（用于调试）
    // uart_init();
}

/**
 * @brief 检查是否需要进入Bootloader模式
 */
bool bootloader_should_enter(void)
{
    // 方法1：检查特定GPIO引脚（如BOOT0引脚）
    // if (gpio_read_boot_pin() == HIGH) {
    //     return true;
    // }
    
    // 方法2：检查UART命令
    // if (uart_receive_boot_command()) {
    //     return true;
    // }
    
    // 方法3：检查Flash标志位（由应用程序设置）
    // uint32_t flag_addr = 0x08000000 + 0x1000;  // 标志位地址
    // if (*(volatile uint32_t *)flag_addr == 0xDEADBEEF) {
    //     // 清除标志位
    //     flash_erase_page(flag_addr);
    //     return true;
    // }
    
    // 默认：检查超时或直接启动应用
    return false;
}

/**
 * @brief 验证分区并决定启动哪个分区
 */
partition_t bootloader_select_partition(void)
{
    partition_info_t info_a, info_b;
    bool valid_a = false, valid_b = false;
    
    // 检查A分区
    if (flash_read_partition_info(PARTITION_A, &info_a) == 0) {
        if (info_a.magic == PARTITION_MAGIC && 
            info_a.status == PARTITION_VALID) {
            valid_a = flash_verify_partition(PARTITION_A);
        }
    }
    
    // 检查B分区
    if (flash_read_partition_info(PARTITION_B, &info_b) == 0) {
        if (info_b.magic == PARTITION_MAGIC && 
            info_b.status == PARTITION_VALID) {
            valid_b = flash_verify_partition(PARTITION_B);
        }
    }
    
    // 选择策略：
    // 1. 如果只有一个有效，选择它
    // 2. 如果两个都有效，选择版本更高的
    // 3. 如果都无效，返回NONE
    
    if (valid_a && valid_b) {
        // 两个都有效，选择版本更高的
        if (info_a.version > info_b.version) {
            return PARTITION_A;
        } else {
            return PARTITION_B;
        }
    } else if (valid_a) {
        return PARTITION_A;
    } else if (valid_b) {
        return PARTITION_B;
    }
    
    return PARTITION_NONE;
}

/**
 * @brief 选择并启动应用分区
 */
void bootloader_start_application(void)
{
    partition_t selected_partition = bootloader_select_partition();
    
    if (selected_partition == PARTITION_NONE) {
        // 没有有效分区，进入错误处理
        // 可以闪烁LED或通过UART输出错误信息
        while (1) {
            // 错误处理循环
            // led_error_blink();
        }
    }
    
    uint32_t app_addr = flash_get_partition_base(selected_partition);
    
    // 跳转到应用程序
    bootloader_jump_to_app(app_addr);
}

/**
 * @brief 跳转到应用程序
 */
void bootloader_jump_to_app(uint32_t app_addr)
{
    // 禁用所有中断
    __disable_irq();
    
    // 关闭SysTick
    // SysTick->CTRL = 0;
    
    // 重置所有外设（可选）
    // RCC_DeInit();
    
    // 设置向量表偏移
    // SCB->VTOR = app_addr;
    
    // 获取应用程序的向量表
    vector_table_t *app_vector_table = (vector_table_t *)app_addr;
    
    // 设置堆栈指针
    __set_MSP(app_vector_table->stack_pointer);
    
    // 跳转到应用程序的复位处理函数
    app_vector_table->reset_handler();
}

/**
 * @brief 系统复位
 */
void bootloader_system_reset(void)
{
    system_reset();
}

/**
 * @brief Bootloader主循环
 */
void bootloader_main(void)
{
    bootloader_init();
    
    // 检查是否需要进入Bootloader模式
    if (bootloader_should_enter()) {
        // 进入Bootloader模式，等待升级命令
        // 这里可以实现固件下载功能（通过UART/USB等）
        // bootloader_upgrade_mode();
        
        // 超时后启动应用
        uint32_t start_time = get_system_tick();
        while ((get_system_tick() - start_time) < BOOTLOADER_TIMEOUT_MS) {
            // 检查是否有升级命令
            // if (check_upgrade_command()) {
            //     perform_upgrade();
            //     break;
            // }
        }
    }
    
    // 启动应用程序
    bootloader_start_application();
}


