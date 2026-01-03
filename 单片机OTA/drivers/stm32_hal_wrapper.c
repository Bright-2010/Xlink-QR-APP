/**
 * @file stm32_hal_wrapper.c
 * @brief STM32 HAL库封装实现
 * @note 基于STM32F1 HAL库实现
 */

#include "stm32_hal_wrapper.h"
#include <string.h>

// STM32 HAL库头文件（需要根据实际项目路径调整）
// 如果使用STM32CubeMX生成的项目，这些头文件会自动包含
#ifdef USE_HAL_DRIVER
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_gpio.h"
#else
// 如果没有HAL库，使用寄存器直接操作（简化实现）
#include "stm32f10x.h"  // 标准外设库
#endif

// UART句柄（需要根据实际配置）
#ifdef USE_HAL_DRIVER
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
#else
// 标准外设库方式
#define USART1_BASE    0x40013800
#define USART2_BASE    0x40004400
#define USART3_BASE    0x40004800
#endif

// GPIO定义（需要根据实际配置）
#ifdef USE_HAL_DRIVER
extern GPIO_TypeDef* GPIOA;
extern GPIO_TypeDef* GPIOB;
extern GPIO_TypeDef* GPIOC;
#else
// 标准外设库已定义
#endif

// ==================== Flash操作实现 ====================

void flash_unlock(void)
{
#ifdef USE_HAL_DRIVER
    HAL_FLASH_Unlock();
#else
    // 标准外设库方式
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
#endif
}

void flash_lock(void)
{
#ifdef USE_HAL_DRIVER
    HAL_FLASH_Lock();
#else
    // 标准外设库方式
    FLASH->CR |= FLASH_CR_LOCK;
#endif
}

int flash_erase_page(uint32_t page_addr)
{
#ifdef USE_HAL_DRIVER
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = page_addr;
    EraseInitStruct.NbPages = 1;
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) == HAL_OK) {
        return 0;
    }
    return -1;
#else
    // 标准外设库方式
    // 等待Flash空闲
    while (FLASH->SR & FLASH_SR_BSY);
    
    // 检查是否已解锁
    if (FLASH->CR & FLASH_CR_LOCK) {
        flash_unlock();
    }
    
    // 设置页擦除
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = page_addr;
    FLASH->CR |= FLASH_CR_STRT;
    
    // 等待完成
    while (FLASH->SR & FLASH_SR_BSY);
    
    // 清除标志
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }
    
    // 清除PER位
    FLASH->CR &= ~FLASH_CR_PER;
    
    return 0;
#endif
}

int flash_program_word(uint32_t addr, uint32_t data)
{
#ifdef USE_HAL_DRIVER
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data) == HAL_OK) {
        return 0;
    }
    return -1;
#else
    // 标准外设库方式
    // 等待Flash空闲
    while (FLASH->SR & FLASH_SR_BSY);
    
    // 检查是否已解锁
    if (FLASH->CR & FLASH_CR_LOCK) {
        flash_unlock();
    }
    
    // 设置编程位
    FLASH->CR |= FLASH_CR_PG;
    
    // 写入数据
    *(volatile uint32_t *)addr = data;
    
    // 等待完成
    while (FLASH->SR & FLASH_SR_BSY);
    
    // 清除标志
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }
    
    // 清除PG位
    FLASH->CR &= ~FLASH_CR_PG;
    
    return 0;
#endif
}

// ==================== UART操作实现 ====================

// UART句柄数组
static void *g_uart_handles[3] = {NULL, NULL, NULL};

int uart_init(uint8_t uart_num, uint32_t baudrate)
{
    if (uart_num < 1 || uart_num > 3) {
        return -1;
    }
    
#ifdef USE_HAL_DRIVER
    UART_HandleTypeDef *huart = NULL;
    
    switch (uart_num) {
        case 1:
            huart = &huart1;
            break;
        case 2:
            huart = &huart2;
            break;
        case 3:
            huart = &huart3;
            break;
        default:
            return -1;
    }
    
    huart->Instance = (uart_num == 1) ? USART1 : 
                      (uart_num == 2) ? USART2 : USART3;
    huart->Init.BaudRate = baudrate;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(huart) == HAL_OK) {
        g_uart_handles[uart_num - 1] = huart;
        return 0;
    }
    return -1;
#else
    // 标准外设库方式
    USART_TypeDef *usart = (uart_num == 1) ? USART1 : 
                          (uart_num == 2) ? USART2 : USART3;
    
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(usart, &USART_InitStruct);
    USART_Cmd(usart, ENABLE);
    
    g_uart_handles[uart_num - 1] = (void *)usart;
    return 0;
#endif
}

int uart_send_byte(uint8_t uart_num, uint8_t byte)
{
    if (uart_num < 1 || uart_num > 3) {
        return -1;
    }
    
#ifdef USE_HAL_DRIVER
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)g_uart_handles[uart_num - 1];
    if (huart == NULL) {
        return -1;
    }
    
    if (HAL_UART_Transmit(huart, &byte, 1, 1000) == HAL_OK) {
        return 0;
    }
    return -1;
#else
    // 标准外设库方式
    USART_TypeDef *usart = (USART_TypeDef *)g_uart_handles[uart_num - 1];
    if (usart == NULL) {
        return -1;
    }
    
    // 等待发送缓冲区空
    while (!(usart->SR & USART_SR_TXE));
    usart->DR = byte;
    
    return 0;
#endif
}

int uart_send_string(uint8_t uart_num, const char *str)
{
    if (str == NULL) {
        return -1;
    }
    
    int len = 0;
    while (*str) {
        if (uart_send_byte(uart_num, *str) != 0) {
            break;
        }
        str++;
        len++;
    }
    return len;
}

int uart_receive_byte(uint8_t uart_num, uint8_t *byte)
{
    if (byte == NULL || uart_num < 1 || uart_num > 3) {
        return -1;
    }
    
#ifdef USE_HAL_DRIVER
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)g_uart_handles[uart_num - 1];
    if (huart == NULL) {
        return -1;
    }
    
    // 非阻塞接收
    if (HAL_UART_Receive(huart, byte, 1, 0) == HAL_OK) {
        return 0;
    }
    return -1;
#else
    // 标准外设库方式
    USART_TypeDef *usart = (USART_TypeDef *)g_uart_handles[uart_num - 1];
    if (usart == NULL) {
        return -1;
    }
    
    // 检查接收标志
    if (usart->SR & USART_SR_RXNE) {
        *byte = (uint8_t)(usart->DR & 0xFF);
        return 0;
    }
    return -1;
#endif
}

int uart_receive(uint8_t uart_num, uint8_t *buffer, uint32_t size, uint32_t timeout_ms)
{
    if (buffer == NULL || size == 0) {
        return -1;
    }
    
    uint32_t start_time = get_system_tick();
    uint32_t received = 0;
    
    while (received < size) {
        uint8_t byte;
        if (uart_receive_byte(uart_num, &byte) == 0) {
            buffer[received++] = byte;
        } else {
            if ((get_system_tick() - start_time) >= timeout_ms) {
                break;  // 超时
            }
            delay_ms(1);
        }
    }
    
    return received;
}

// ==================== 系统时钟实现 ====================

uint32_t get_system_tick(void)
{
#ifdef USE_HAL_DRIVER
    return HAL_GetTick();
#else
    // 标准外设库方式 - 使用SysTick
    extern volatile uint32_t uwTick;
    return uwTick;
#endif
}

void delay_ms(uint32_t ms)
{
#ifdef USE_HAL_DRIVER
    HAL_Delay(ms);
#else
    // 标准外设库方式
    uint32_t start = get_system_tick();
    while ((get_system_tick() - start) < ms) {
        // 等待
    }
#endif
}

// SysTick中断处理函数（需要在中断向量表中注册）
void SysTick_Handler(void)
{
#ifdef USE_HAL_DRIVER
    HAL_IncTick();
#else
    // 标准外设库方式
    extern volatile uint32_t uwTick;
    uwTick++;
#endif
}

// ==================== GPIO操作实现 ====================

bool gpio_read_pin(void *port, uint16_t pin)
{
    if (port == NULL) {
        return false;
    }
    
#ifdef USE_HAL_DRIVER
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    return (HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET);
#else
    // 标准外设库方式
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    return (GPIO_ReadInputDataBit(gpio, pin) == Bit_SET);
#endif
}

void gpio_write_pin(void *port, uint16_t pin, bool state)
{
    if (port == NULL) {
        return;
    }
    
#ifdef USE_HAL_DRIVER
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    HAL_GPIO_WritePin(gpio, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
#else
    // 标准外设库方式
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    GPIO_WriteBit(gpio, pin, state ? Bit_SET : Bit_RESET);
#endif
}

void gpio_toggle_pin(void *port, uint16_t pin)
{
    if (port == NULL) {
        return;
    }
    
#ifdef USE_HAL_DRIVER
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    HAL_GPIO_TogglePin(gpio, pin);
#else
    // 标准外设库方式
    bool current = gpio_read_pin(port, pin);
    gpio_write_pin(port, pin, !current);
#endif
}

// ==================== 系统复位实现 ====================

void system_reset(void)
{
#ifdef USE_HAL_DRIVER
    HAL_NVIC_SystemReset();
#else
    // 标准外设库方式
    NVIC_SystemReset();
#endif
}
