/**
 * @file system_init.c
 * @brief 系统初始化代码
 */

#include "stm32_hal_wrapper.h"
#include "../config.h"
#include <stdint.h>

// STM32 HAL库头文件（需要根据实际项目路径调整）
#ifdef USE_HAL_DRIVER
// 如果使用STM32CubeMX生成的项目，这些头文件会自动包含
// #include "stm32f1xx_hal.h"
// #include "stm32f1xx_hal_rcc.h"
// #include "stm32f1xx_hal_gpio.h"
#else
// 标准外设库（需要集成到项目中）
// #include "stm32f10x.h"
// #include "stm32f10x_rcc.h"
// #include "stm32f10x_gpio.h"
#endif

/**
 * @brief 系统时钟配置
 */
void SystemClock_Config(void)
{
#ifdef USE_HAL_DRIVER
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    // 配置HSE
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 8MHz * 9 = 72MHz
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    // 配置系统时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
    
    // 配置SysTick
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
#else
    // 标准外设库方式
    ErrorStatus HSEStartUpStatus;
    
    // 使能HSE
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEReady();
    
    if (HSEStartUpStatus == SUCCESS) {
        // 使能预取指
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        
        // Flash等待周期
        FLASH_SetLatency(FLASH_Latency_2);
        
        // HCLK = SYSCLK
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        
        // PCLK2 = HCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);
        
        // PCLK1 = HCLK/2
        RCC_PCLK1Config(RCC_HCLK_Div2);
        
        // PLL配置: HSE * 9 = 72MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        
        // 使能PLL
        RCC_PLLCmd(ENABLE);
        
        // 等待PLL就绪
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        // 选择PLL作为系统时钟源
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        // 等待PLL成为系统时钟源
        while (RCC_GetSYSCLKSource() != 0x08);
    }
    
    // 配置SysTick
    SysTick_Config(SystemCoreClock / 1000);
#endif
}

/**
 * @brief GPIO初始化
 */
void GPIO_Init_Config(void)
{
#ifdef USE_HAL_DRIVER
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // 配置LED引脚（如果使用）
    #ifdef LED_STATUS_PIN
    GPIO_InitStruct.Pin = LED_STATUS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_STATUS_PORT, &GPIO_InitStruct);
    #endif
    
    // 配置按键引脚（如果使用）
    #ifdef OTA_TRIGGER_PIN
    GPIO_InitStruct.Pin = OTA_TRIGGER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(OTA_TRIGGER_PORT, &GPIO_InitStruct);
    #endif
#else
    // 标准外设库方式
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置LED引脚（如果使用）
    #ifdef LED_STATUS_PIN
    GPIO_InitStructure.GPIO_Pin = LED_STATUS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_STATUS_PORT, &GPIO_InitStructure);
    #endif
    
    // 配置按键引脚（如果使用）
    #ifdef OTA_TRIGGER_PIN
    GPIO_InitStructure.GPIO_Pin = OTA_TRIGGER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OTA_TRIGGER_PORT, &GPIO_InitStructure);
    #endif
#endif
}

/**
 * @brief UART GPIO配置
 */
void UART_GPIO_Init(uint8_t uart_num)
{
#ifdef USE_HAL_DRIVER
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if (uart_num == 1) {
        // UART1: PA9(TX), PA10(RX)
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    } else if (uart_num == 2) {
        // UART2: PA2(TX), PA3(RX)
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    } else if (uart_num == 3) {
        // UART3: PB10(TX), PB11(RX)
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART3_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
#else
    // 标准外设库方式
    GPIO_InitTypeDef GPIO_InitStructure;
    
    if (uart_num == 1) {
        // UART1: PA9(TX), PA10(RX)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else if (uart_num == 2) {
        // UART2: PA2(TX), PA3(RX)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else if (uart_num == 3) {
        // UART3: PB10(TX), PB11(RX)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
#endif
}

/**
 * @brief 系统初始化
 */
void System_Init(void)
{
    // 系统时钟配置
    SystemClock_Config();
    
    // GPIO初始化
    GPIO_Init_Config();
    
    // UART GPIO配置
    UART_GPIO_Init(1);  // UART1 - 二维码扫描
    UART_GPIO_Init(2);  // UART2 - 调试输出
    UART_GPIO_Init(3);  // UART3 - WiFi模块
    
    // UART初始化
    uart_init(1, QR_UART_BAUDRATE);
    uart_init(2, DEBUG_UART_BAUDRATE);
    uart_init(3, WIFI_UART_BAUDRATE);
}

