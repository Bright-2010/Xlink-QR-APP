/**
 * @file main.c
 * @brief 主应用程序入口
 */

#include "ota_manager.h"
#include "../common/ui_status.h"
#include "../common/flash_manager.h"
#include "../common/qr_scanner.h"
#include "../common/firmware_download.h"
#include "../drivers/system_init.h"
#include <stdint.h>
#include <stddef.h>

#ifdef USE_HAL_DRIVER
#include "stm32f1xx_hal.h"
#else
#include "stm32f10x.h"
#endif

/**
 * @brief 系统初始化
 */
static void system_init(void)
{
    // 系统初始化（时钟、GPIO、UART等）
    System_Init();
    
    // 初始化Flash管理器
    flash_manager_init();
    
    // 初始化二维码扫描器
    qr_scanner_init(NULL);  // UART句柄在qr_scanner.c中已配置
    
    // 初始化固件下载模块
    firmware_download_init(NULL);  // 网络句柄在http_client.c中已配置
    
    // 初始化UI
    ui_init();
    
    // 初始化OTA管理器
    ota_manager_init();
}

/**
 * @brief 主循环
 */
int main(void)
{
    // 系统初始化
    system_init();
    
    ui_show_message("系统启动完成");
    ui_update_status(UI_STATUS_IDLE);
    
    // 主循环
    while (1) {
        // 处理OTA升级流程
        ota_process();
        
        // 处理其他应用任务
        // application_task();
        
        // 延时
        delay_ms(10);
    }
    
    return 0;
}

