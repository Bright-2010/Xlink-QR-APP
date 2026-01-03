/**
 * @file qr_scanner.h
 * @brief 二维码扫描模块
 * @note 使用外部库（如QRCodeReader）进行二维码解码
 */

#ifndef QR_SCANNER_H
#define QR_SCANNER_H

#include <stdint.h>
#include <stdbool.h>

#define QR_URL_MAX_LEN   256  // 最大URL长度

/**
 * @brief 初始化二维码扫描器
 * @param uart_handle UART句柄（用于接收二维码数据）
 */
void qr_scanner_init(void *uart_handle);

/**
 * @brief 扫描二维码并提取URL
 * @param url_buffer 输出缓冲区，用于存储提取的URL
 * @param buffer_size 缓冲区大小
 * @param timeout_ms 超时时间（毫秒）
 * @return 0成功，-1失败
 */
int qr_scanner_scan(char *url_buffer, uint32_t buffer_size, uint32_t timeout_ms);

/**
 * @brief 验证URL格式
 * @param url URL字符串
 * @return true有效，false无效
 */
bool qr_validate_url(const char *url);

/**
 * @brief 从二维码数据中解析URL
 * @param qr_data 原始二维码数据
 * @param qr_data_len 数据长度
 * @param url_buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 0成功，-1失败
 */
int qr_parse_url(const uint8_t *qr_data, uint32_t qr_data_len,
                 char *url_buffer, uint32_t buffer_size);

#endif // QR_SCANNER_H

