/**
 * @file http_client.h
 * @brief HTTP客户端实现（支持AT命令和直接TCP）
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

// HTTP客户端模式
typedef enum {
    HTTP_MODE_AT_COMMAND = 0,  // 使用AT命令（WiFi模块）
    HTTP_MODE_TCP_DIRECT = 1   // 直接TCP（lwIP等）
} http_mode_t;

/**
 * @brief 初始化HTTP客户端
 * @param mode 工作模式
 * @param uart_num UART编号（AT命令模式）
 * @return 0成功，-1失败
 */
int http_client_init(http_mode_t mode, uint8_t uart_num);

/**
 * @brief 从URL下载数据
 * @param url URL地址
 * @param buffer 数据缓冲区
 * @param buffer_size 缓冲区大小
 * @param downloaded_size 实际下载大小（输出）
 * @param progress_cb 进度回调（可选）
 * @return 0成功，-1失败
 */
int http_client_download(const char *url,
                        uint8_t *buffer,
                        uint32_t buffer_size,
                        uint32_t *downloaded_size,
                        void (*progress_cb)(uint32_t downloaded, uint32_t total));

/**
 * @brief 解析URL
 * @param url 完整URL
 * @param hostname 主机名（输出，至少64字节）
 * @param path 路径（输出，至少256字节）
 * @param port 端口号（输出）
 * @return 0成功，-1失败
 */
int http_parse_url(const char *url, char *hostname, char *path, uint16_t *port);

#endif // HTTP_CLIENT_H

