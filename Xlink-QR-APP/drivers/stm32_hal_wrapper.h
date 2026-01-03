/**
 * @file stm32_hal_wrapper.h
 * @brief STM32 HAL库封装层
 * @note 提供Flash、UART、GPIO等基础功能的封装
 */

#ifndef STM32_HAL_WRAPPER_H
#define STM32_HAL_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ==================== Flash操作 ====================

/**
 * @brief 解锁Flash
 */
void flash_unlock(void);

/**
 * @brief 锁定Flash
 */
void flash_lock(void);

/**
 * @brief 擦除Flash页
 * @param page_addr 页地址
 * @return 0成功，-1失败
 */
int flash_erase_page(uint32_t page_addr);

/**
 * @brief 编程Flash字（32位）
 * @param addr 地址（必须4字节对齐）
 * @param data 数据
 * @return 0成功，-1失败
 */
int flash_program_word(uint32_t addr, uint32_t data);

// ==================== UART操作 ====================

/**
 * @brief 初始化UART
 * @param uart_num UART编号（1-3）
 * @param baudrate 波特率
 * @return 0成功，-1失败
 */
int uart_init(uint8_t uart_num, uint32_t baudrate);

/**
 * @brief 发送一个字节
 * @param uart_num UART编号
 * @param byte 字节数据
 * @return 0成功，-1失败
 */
int uart_send_byte(uint8_t uart_num, uint8_t byte);

/**
 * @brief 发送字符串
 * @param uart_num UART编号
 * @param str 字符串
 * @return 发送的字节数
 */
int uart_send_string(uint8_t uart_num, const char *str);

/**
 * @brief 接收一个字节（非阻塞）
 * @param uart_num UART编号
 * @param byte 输出字节
 * @return 0成功，-1无数据
 */
int uart_receive_byte(uint8_t uart_num, uint8_t *byte);

/**
 * @brief 接收数据（阻塞，带超时）
 * @param uart_num UART编号
 * @param buffer 缓冲区
 * @param size 缓冲区大小
 * @param timeout_ms 超时时间（毫秒）
 * @return 接收的字节数
 */
int uart_receive(uint8_t uart_num, uint8_t *buffer, uint32_t size, uint32_t timeout_ms);

// ==================== 系统时钟 ====================

/**
 * @brief 获取系统时钟计数（毫秒）
 * @return 时钟计数
 */
uint32_t get_system_tick(void);

/**
 * @brief 延时（毫秒）
 * @param ms 延时时间
 */
void delay_ms(uint32_t ms);

// ==================== GPIO操作 ====================

/**
 * @brief 读取GPIO引脚
 * @param port GPIO端口（GPIOA, GPIOB等）
 * @param pin 引脚号
 * @return true高电平，false低电平
 */
bool gpio_read_pin(void *port, uint16_t pin);

/**
 * @brief 设置GPIO引脚
 * @param port GPIO端口
 * @param pin 引脚号
 * @param state true高电平，false低电平
 */
void gpio_write_pin(void *port, uint16_t pin, bool state);

/**
 * @brief 切换GPIO引脚
 * @param port GPIO端口
 * @param pin 引脚号
 */
void gpio_toggle_pin(void *port, uint16_t pin);

// ==================== 系统复位 ====================

/**
 * @brief 系统复位
 */
void system_reset(void);

#ifdef __cplusplus
}
#endif

#endif // STM32_HAL_WRAPPER_H

