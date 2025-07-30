/**
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  * @file       BSP_USART.c/h
  * @brief      将HAL库串口函数进行二次封装，并在串口中断中接收数据
  * @note      	
  * @history
  *  Version    Date            Author          Modification
  *  V3.0.0     2020.7.14     	              	1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  */

#include <stdio.h>// 包含标准输入输出头文件
#include "BSP_USART.h"

#include <stdint.h>
#include <string.h>
int fputc(int ch,FILE *f)
{
//采用轮询方式发送1字节数据，超时时间设置为无限等待
HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,HAL_MAX_DELAY);
return ch;
}
int fgetc(FILE *f)
{
uint8_t ch;
// 采用轮询方式接收 1字节数据，超时时间设置为无限等待
HAL_UART_Receive( &huart1,(uint8_t*)&ch,1, HAL_MAX_DELAY );
return ch;
}

/**
	* @brief          串口接收中断回调函数
  * @param[in]     	huart 串口序号
  * @retval         none
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance==USART1)
	{


	}
	

}











// 数据包构建函数
void build_data_packet(uint8_t *packet, double value, waveform_type_t waveform, channel_t channel, uint8_t *data_packet) {
    // 初始化数据包的起始部分
    packet[0] = 0xAB;  // 包头
    packet[1] = (waveform == SINE_WAVE) ? 0x00 : 0x01;  // 类型（幅值或频率）
    packet[2] = channel;  // 通道号
    packet[3] = waveform;  // 波形类型
    packet[4] = 0xCC;  // 开始标志

    // 处理幅值或频率部分
    if (waveform == SINE_WAVE) {
        // 幅值，分成整数部分和小数部分
        int32_t integer_part = (int32_t)value;  // 整数部分
        int16_t decimal_part = (int16_t)round((value - integer_part) * 65536);  // 小数部分，使用round()进行四舍五入

        // 写入整数部分（24位大端序）
        packet[5] = (integer_part >> 16) & 0xFF;
        packet[6] = (integer_part >> 8) & 0xFF;
        packet[7] = integer_part & 0xFF;

        // 写入小数部分（16位大端序）
        packet[8] = (decimal_part >> 8) & 0xFF;
        packet[9] = decimal_part & 0xFF;
    } else if (waveform == ADC_WAVE) {
        // 频率部分，40位整数部分
        uint64_t frequency = (uint64_t)value;

        // 写入频率（40位大端序）
        packet[5] = (frequency >> 32) & 0xFF;  // 高 8 位
        packet[6] = (frequency >> 24) & 0xFF;  // 中高 8 位
        packet[7] = (frequency >> 16) & 0xFF;  // 中低 8 位
        packet[8] = (frequency >> 8) & 0xFF;   // 低 8 位
        packet[9] = frequency & 0xFF;          // 低 8 位
    }

    // 包尾
    packet[10] = 0xBA;
}




/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/

