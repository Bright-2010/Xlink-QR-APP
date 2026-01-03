/**
 * @file config_example.h
 * @brief 配置示例文件
 * @note 复制此文件为 config.h 并根据实际硬件配置修改
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== 硬件配置 ====================

// Flash配置
#define FLASH_BASE_ADDR          0x08000000
#define FLASH_SIZE               (64 * 1024)   // 64KB
#define FLASH_PAGE_SIZE          1024          // 1KB per page

// Bootloader配置
#define BOOTLOADER_SIZE          (8 * 1024)    // 8KB

// 应用分区配置
#define PARTITION_SIZE           (28 * 1024)   // 每个分区28KB

// RAM配置
#define FIRMWARE_BUFFER_SIZE     (28 * 1024)   // 固件缓冲区大小（需根据实际RAM调整）

// ==================== UART配置 ====================

// 二维码扫描UART
#define QR_UART                 USART1
#define QR_UART_BAUDRATE        9600
#define QR_UART_TX_PIN          GPIO_PIN_9
#define QR_UART_RX_PIN          GPIO_PIN_10

// 调试UART
#define DEBUG_UART              USART2
#define DEBUG_UART_BAUDRATE     115200
#define DEBUG_UART_TX_PIN       GPIO_PIN_2
#define DEBUG_UART_RX_PIN       GPIO_PIN_3

// WiFi模块UART（如果使用）
#define WIFI_UART               USART3
#define WIFI_UART_BAUDRATE      115200
#define WIFI_UART_TX_PIN        GPIO_PIN_10
#define WIFI_UART_RX_PIN        GPIO_PIN_11

// ==================== GPIO配置 ====================

// Bootloader模式检测引脚
#define BOOT_PIN                GPIO_PIN_0
#define BOOT_PORT               GPIOA

// OTA升级触发按键
#define OTA_TRIGGER_PIN         GPIO_PIN_1
#define OTA_TRIGGER_PORT        GPIOA

// LED状态指示
#define LED_STATUS_PIN          GPIO_PIN_13
#define LED_STATUS_PORT         GPIOC

// ==================== 网络配置 ====================

// WiFi配置（如果使用ESP8266等模块）
#define WIFI_SSID               "YourWiFiSSID"
#define WIFI_PASSWORD           "YourWiFiPassword"

// HTTP服务器配置
#define HTTP_SERVER_PORT        80
#define HTTP_TIMEOUT_MS         30000

// ==================== OTA配置 ====================

// 二维码扫描超时时间（毫秒）
#define QR_SCAN_TIMEOUT_MS      30000

// 固件下载超时时间（毫秒）
#define DOWNLOAD_TIMEOUT_MS     60000

// 版本信息在固件中的偏移地址
#define FIRMWARE_VERSION_OFFSET  0x200

// ==================== 调试配置 ====================

// 启用调试输出
#define DEBUG_ENABLED           1

// 调试级别
#define DEBUG_LEVEL_ERROR       1
#define DEBUG_LEVEL_WARNING     2
#define DEBUG_LEVEL_INFO        3
#define DEBUG_LEVEL_DEBUG       4

#define CURRENT_DEBUG_LEVEL     DEBUG_LEVEL_INFO

// ==================== 功能开关 ====================

// 启用MD5校验（需要MD5库支持）
#define ENABLE_MD5_CHECK        0

// 启用HTTPS支持（需要mbedTLS库）
#define ENABLE_HTTPS            0

// 启用LCD/OLED显示
#define ENABLE_DISPLAY          0

// ==================== 安全配置 ====================

// 最大固件大小（字节）
#define MAX_FIRMWARE_SIZE       (28 * 1024)

// 允许的最大下载重试次数
#define MAX_DOWNLOAD_RETRIES    3

// Bootloader超时时间（毫秒）
#define BOOTLOADER_TIMEOUT_MS   5000

#endif // CONFIG_H

