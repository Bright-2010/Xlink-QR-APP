/**
 * @file system_init.h
 * @brief 系统初始化头文件
 */

#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

/**
 * @brief 系统时钟配置
 */
void SystemClock_Config(void);

/**
 * @brief GPIO初始化
 */
void GPIO_Init_Config(void);

/**
 * @brief UART GPIO配置
 */
void UART_GPIO_Init(uint8_t uart_num);

/**
 * @brief 系统初始化
 */
void System_Init(void);

#endif // SYSTEM_INIT_H

