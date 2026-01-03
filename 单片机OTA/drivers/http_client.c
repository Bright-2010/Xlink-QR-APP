/**
 * @file http_client.c
 * @brief HTTP客户端实现
 */

#include "http_client.h"
#include "stm32_hal_wrapper.h"
#include <string.h>
#include <stdio.h>

static http_mode_t g_http_mode = HTTP_MODE_AT_COMMAND;
static uint8_t g_uart_num = 1;

// AT命令缓冲区
#define AT_BUFFER_SIZE  512
static char g_at_buffer[AT_BUFFER_SIZE];

/**
 * @brief 发送AT命令并等待响应
 */
static int at_send_command(const char *cmd, const char *expected_response, uint32_t timeout_ms)
{
    // 清空接收缓冲区
    uint8_t dummy;
    while (uart_receive_byte(g_uart_num, &dummy) == 0);
    
    // 发送AT命令
    uart_send_string(g_uart_num, cmd);
    uart_send_string(g_uart_num, "\r\n");
    
    // 等待响应
    uint32_t start_time = get_system_tick();
    uint32_t buffer_pos = 0;
    
    while ((get_system_tick() - start_time) < timeout_ms) {
        uint8_t byte;
        if (uart_receive_byte(g_uart_num, &byte) == 0) {
            if (buffer_pos < AT_BUFFER_SIZE - 1) {
                g_at_buffer[buffer_pos++] = byte;
                g_at_buffer[buffer_pos] = '\0';
                
                // 检查是否包含期望的响应
                if (expected_response && strstr((char *)g_at_buffer, expected_response)) {
                    return 0;  // 成功
                }
                
                // 检查错误响应
                if (strstr((char *)g_at_buffer, "ERROR") || 
                    strstr((char *)g_at_buffer, "FAIL")) {
                    return -1;  // 失败
                }
            }
        }
        delay_ms(10);
    }
    
    return -1;  // 超时
}

/**
 * @brief 初始化HTTP客户端
 */
int http_client_init(http_mode_t mode, uint8_t uart_num)
{
    g_http_mode = mode;
    g_uart_num = uart_num;
    
    if (mode == HTTP_MODE_AT_COMMAND) {
        // 初始化WiFi模块（ESP8266示例）
        delay_ms(1000);  // 等待模块就绪
        
        // 测试AT命令
        if (at_send_command("AT", "OK", 2000) != 0) {
            return -1;
        }
        
        // 设置为透传模式（可选）
        // at_send_command("AT+CIPMODE=1", "OK", 2000);
        
        return 0;
    }
    
    // TCP直接模式需要初始化lwIP等
    return 0;
}

/**
 * @brief 解析URL
 */
int http_parse_url(const char *url, char *hostname, char *path, uint16_t *port)
{
    if (url == NULL || hostname == NULL || path == NULL || port == NULL) {
        return -1;
    }
    
    // 默认端口
    *port = 80;
    
    // 跳过协议
    const char *p = url;
    if (strncmp(p, "http://", 7) == 0) {
        p += 7;
        *port = 80;
    } else if (strncmp(p, "https://", 8) == 0) {
        p += 8;
        *port = 443;
    } else {
        return -1;  // 不支持的协议
    }
    
    // 提取主机名和路径
    const char *path_start = strchr(p, '/');
    if (path_start) {
        // 检查端口号
        const char *port_start = strchr(p, ':');
        if (port_start && port_start < path_start) {
            // 有端口号
            size_t hostname_len = port_start - p;
            if (hostname_len >= 64) hostname_len = 63;
            memcpy(hostname, p, hostname_len);
            hostname[hostname_len] = '\0';
            
            *port = (uint16_t)atoi(port_start + 1);
        } else {
            // 无端口号
            size_t hostname_len = path_start - p;
            if (hostname_len >= 64) hostname_len = 63;
            memcpy(hostname, p, hostname_len);
            hostname[hostname_len] = '\0';
        }
        
        strncpy(path, path_start, 255);
        path[255] = '\0';
    } else {
        // 无路径，使用根路径
        size_t hostname_len = strlen(p);
        if (hostname_len >= 64) hostname_len = 63;
        memcpy(hostname, p, hostname_len);
        hostname[hostname_len] = '\0';
        strcpy(path, "/");
    }
    
    return 0;
}

/**
 * @brief 从URL下载数据（AT命令模式）
 */
static int http_download_at_mode(const char *url,
                                 uint8_t *buffer,
                                 uint32_t buffer_size,
                                 uint32_t *downloaded_size,
                                 void (*progress_cb)(uint32_t downloaded, uint32_t total))
{
    char hostname[64];
    char path[256];
    uint16_t port;
    
    if (http_parse_url(url, hostname, path, &port) != 0) {
        return -1;
    }
    
    // 建立TCP连接
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", hostname, port);
    if (at_send_command(cmd, "OK", 10000) != 0) {
        return -1;
    }
    
    delay_ms(500);
    
    // 发送HTTP GET请求
    char http_request[512];
    snprintf(http_request, sizeof(http_request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n", path, hostname);
    
    // 设置发送长度
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", (int)strlen(http_request));
    if (at_send_command(cmd, ">", 2000) != 0) {
        return -1;
    }
    
    // 发送HTTP请求
    uart_send_string(g_uart_num, http_request);
    
    delay_ms(1000);
    
    // 接收HTTP响应
    uint32_t received = 0;
    bool header_received = false;
    uint32_t content_length = 0;
    
    uint32_t start_time = get_system_tick();
    uint32_t timeout = 60000;  // 60秒超时
    
    while (received < buffer_size && (get_system_tick() - start_time) < timeout) {
        uint8_t byte;
        if (uart_receive_byte(g_uart_num, &byte) == 0) {
            if (!header_received) {
                // 解析HTTP响应头
                g_at_buffer[received % AT_BUFFER_SIZE] = byte;
                
                // 查找Content-Length
                if (strstr((char *)g_at_buffer, "Content-Length:")) {
                    const char *cl = strstr((char *)g_at_buffer, "Content-Length:");
                    if (cl) {
                        content_length = (uint32_t)atoi(cl + 15);
                    }
                }
                
                // 查找HTTP头结束（\r\n\r\n）
                if (received >= 3) {
                    if (g_at_buffer[(received - 3) % AT_BUFFER_SIZE] == '\r' &&
                        g_at_buffer[(received - 2) % AT_BUFFER_SIZE] == '\n' &&
                        g_at_buffer[(received - 1) % AT_BUFFER_SIZE] == '\r' &&
                        byte == '\n') {
                        header_received = true;
                        received = 0;  // 重置，开始接收数据
                        continue;
                    }
                }
                received++;
            } else {
                // 接收数据部分
                buffer[received++] = byte;
                
                if (progress_cb && content_length > 0) {
                    progress_cb(received, content_length);
                }
                
                // 如果知道内容长度，检查是否接收完成
                if (content_length > 0 && received >= content_length) {
                    break;
                }
            }
        } else {
            delay_ms(10);
        }
    }
    
    *downloaded_size = received;
    
    // 关闭连接
    at_send_command("AT+CIPCLOSE", "OK", 2000);
    
    return (received > 0) ? 0 : -1;
}

/**
 * @brief 从URL下载数据
 */
int http_client_download(const char *url,
                        uint8_t *buffer,
                        uint32_t buffer_size,
                        uint32_t *downloaded_size,
                        void (*progress_cb)(uint32_t downloaded, uint32_t total))
{
    if (url == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    if (g_http_mode == HTTP_MODE_AT_COMMAND) {
        return http_download_at_mode(url, buffer, buffer_size, downloaded_size, progress_cb);
    }
    
    // TCP直接模式（需要lwIP实现）
    // TODO: 实现lwIP版本
    
    return -1;
}

