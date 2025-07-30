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





// 构建数据包函数（使用40位整数部分，无小数部分）
// type: 00-ADC采样率, 01-DAC输出频率
// channel: 通道号 (1-通道1, 2-通道2...)
// wave_type: 波形类型 (0-正弦波, 1-三角波, 2-方波)
// freq: 频率值 (单位Hz，整数)
// packet: 输出缓冲区（至少11字节）
void buildpacket(uint8_t type, uint8_t channel, uint8_t wave_type, uint64_t freq, uint8_t* packet) {
    // 包头
    packet[0] = 0xAB;
    
    // 前两位类型 + 第三位通道号
    packet[1] = type;
    packet[2] = channel;  // 通道号
    
    // 第四位波形类型
    packet[3] = wave_type;
    
    // 开始标志
    packet[4] = 0xCC;
    
    // 40位整数部分（5字节大端序）
    // 注意：freq必须是小于2^40的值 (约1.1万亿Hz)
    packet[5] = (freq >> 32) & 0xFF;  // 最高字节
    packet[6] = (freq >> 24) & 0xFF;
    packet[7] = (freq >> 16) & 0xFF;
    packet[8] = (freq >> 8)  & 0xFF;
    packet[9] = freq & 0xFF;
    
    // 包尾
    packet[10] = 0xBA;
}

/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/

