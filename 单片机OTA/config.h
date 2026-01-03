/**
 * @file config.h
 * @brief 项目配置文件
 * @note 根据实际硬件配置修改此文件
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== 硬件配置 ====================

// Flash配置（STM32F108T6: 64KB Flash）
#define FLASH_BASE_ADDR          0x08000000
#define FLASH_SIZE               (64 * 1024)   // 64KB
#define FLASH_PAGE_SIZE          1024          // 1KB per page

// Bootloader配置
#define BOOTLOADER_SIZE          (8 * 1024)    // 8KB

// 应用分区配置
#define PARTITION_SIZE           (28 * 1024)   // 每个分区28KB

// RAM配置（STM32F108T6: 20KB RAM）
#define FIRMWARE_BUFFER_SIZE     (28 * 1024)   // 固件缓冲区大小
// 注意：如果RAM不足，可以减小此值，但需要支持分块下载

// ==================== UART配置 ====================

// 二维码扫描UART（连接二维码扫描模块）
#define QR_UART_NUM              1
#define QR_UART_BAUDRATE         9600

// 调试UART（用于状态输出）
#define DEBUG_UART_NUM           2
#define DEBUG_UART_BAUDRATE      115200

// WiFi模块UART（ESP8266等，用于固件下载）
#define WIFI_UART_NUM            3
#define WIFI_UART_BAUDRATE       115200

// ==================== GPIO配置 ====================

// Bootloader模式检测引脚（可选）
#define BOOT_PIN                 GPIO_PIN_0
#define BOOT_PORT                GPIOA

// OTA升级触发按键（可选）
#define OTA_TRIGGER_PIN          GPIO_PIN_1
#define OTA_TRIGGER_PORT         GPIOA

// LED状态指示（可选）
#define LED_STATUS_PIN           GPIO_PIN_13
#define LED_STATUS_PORT          GPIOC

// ==================== HAL库配置 ====================

// 选择使用的库类型
// 如果使用STM32CubeMX生成的项目，取消下面的注释：
// #define USE_HAL_DRIVER

// 如果使用标准外设库，保持注释状态（默认）
// 注意：当前配置为标准外设库模式

// ==================== 网络配置 ====================

// WiFi配置（如果使用ESP8266等模块）
#define WIFI_SSID                "YourWiFiSSID"
#define WIFI_PASSWORD             "YourWiFiPassword"

// HTTP配置
#define HTTP_TIMEOUT_MS          60000          // 60秒超时

// ==================== OTA配置 ====================

// 二维码扫描超时时间（毫秒）
#define QR_SCAN_TIMEOUT_MS       30000         // 30秒

// 固件下载超时时间（毫秒）
#define DOWNLOAD_TIMEOUT_MS      60000         // 60秒

// 版本信息在固件中的偏移地址
#define FIRMWARE_VERSION_OFFSET  0x200

// ==================== 功能开关 ====================

// 启用调试输出
#define DEBUG_ENABLED            1

// 调试级别
#define DEBUG_LEVEL_ERROR        1
#define DEBUG_LEVEL_WARNING      2
#define DEBUG_LEVEL_INFO         3
#define DEBUG_LEVEL_DEBUG        4

#define CURRENT_DEBUG_LEVEL      DEBUG_LEVEL_INFO

// 启用MD5校验（需要MD5库支持）
#define ENABLE_MD5_CHECK         0

// 启用HTTPS支持（需要mbedTLS库）
#define ENABLE_HTTPS             0

// ==================== 安全配置 ====================

// 最大固件大小（字节）
#define MAX_FIRMWARE_SIZE        (28 * 1024)

// 允许的最大下载重试次数
#define MAX_DOWNLOAD_RETRIES     3

// Bootloader超时时间（毫秒）
#define BOOTLOADER_TIMEOUT_MS    5000

// ==================== 版本控制 ====================

// 允许降级（已默认启用，版本不同即可升级）
// 如需限制只能升级，修改 version_control.c 中的 version_need_update 函数

#endif // CONFIG_H

