/**
 * @file qrcode_decoder.h
 * @brief 二维码解码器（简化版，基于quirc算法）
 * @note 这是一个轻量级的二维码解码实现，适合嵌入式系统
 */

#ifndef QRCODE_DECODER_H
#define QRCODE_DECODER_H

#include <stdint.h>
#include <stdbool.h>

// 二维码解码结果
typedef struct {
    bool valid;              // 是否有效
    uint8_t *data;            // 解码数据
    uint32_t data_len;        // 数据长度
    uint8_t version;          // 二维码版本
    uint8_t ecc_level;        // 纠错级别
} qrcode_result_t;

/**
 * @brief 初始化二维码解码器
 */
void qrcode_decoder_init(void);

/**
 * @brief 解码二维码数据
 * @param image_data 图像数据（灰度图，8位）
 * @param width 图像宽度
 * @param height 图像高度
 * @param result 解码结果（输出）
 * @return 0成功，-1失败
 */
int qrcode_decode(const uint8_t *image_data, uint32_t width, uint32_t height, qrcode_result_t *result);

/**
 * @brief 从UART接收的字符串数据解码（简化版）
 * @param qr_string 二维码字符串数据
 * @param result_data 输出数据缓冲区
 * @param buffer_size 缓冲区大小
 * @return 数据长度，-1失败
 * @note 如果二维码扫描模块直接输出字符串，可以使用此函数
 */
int qrcode_decode_string(const char *qr_string, char *result_data, uint32_t buffer_size);

/**
 * @brief 释放解码结果
 */
void qrcode_result_free(qrcode_result_t *result);

#endif // QRCODE_DECODER_H

