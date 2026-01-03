/**
 * @file main.c
 * @brief Bootloader入口
 */

#include "bootloader.h"

/**
 * @brief Bootloader主函数
 */
int main(void)
{
    bootloader_main();
    
    // 不应该到达这里
    while (1) {
        // 错误处理
    }
    
    return 0;
}

