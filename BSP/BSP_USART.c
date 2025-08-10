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
#include "main.h"
#include <stdint.h>
#include <string.h>
#include "BSP_CONTROL.h"

// 变量定义

 uint8_t txBuffer[11]; 
  uint8_t contlV[11];
 uint8_t rxBuffer[7]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF}; 
 uint8_t RX_pa = 0x00;
volatile uint8_t dmaComplete = 0;
DataPoint dataStorage[DATA_POINTS];
uint16_t storageIndex = 0;
uint32_t txValue=100;
float_t volt[DATA_POINTS]={0};
// uart7接收串口屏发送的数组
uint8_t H7Buffer[8];
uint8_t sweepend=0;
uint8_t rxindex=0;
// 重定向
int fputc(int ch,FILE *f)
{
//采用轮询方式发送1字节数据，超时时间设置为无限等待
HAL_UART_Transmit(&huart7,(uint8_t *)&ch,1,HAL_MAX_DELAY);
return ch;
}
int fgetc(FILE *f)
{
uint8_t ch;
// 采用轮询方式接收 1字节数据，超时时间设置为无限等待
HAL_UART_Receive( &huart7,(uint8_t*)&ch,1, HAL_MAX_DELAY );
return ch;
}

// DMA完成回调
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // 启动DMA接收
			if(Saopin==1){
					HAL_UART_Receive_IT(&huart1, &RX_pa,1);
				
    }
		


}}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
   if (huart->Instance == USART1) {
				//dmaComplete = 1;
//        // 接收完成一个字
        // 检查是否已经接收了7个字节
		 rxBuffer[rxindex] = RX_pa;
		 rxindex++;
		 HAL_UART_Receive_IT(&huart1, &RX_pa,1);
		 
        if (rxindex >= 7) {
            dmaComplete = 1;  // 置位完成标志
            rxindex = 0;      // 重置索引，为下一次接收做准备
        } 

//	 else {
//					rxindex = 0;
//	 
//	 }
	 
	 }
				    if(huart->Instance == UART7) {
							dataReady=1;
       
//        HAL_UART_Receive_DMA(&huart7, H7Buffer,sizeof(H7Buffer));// 重新启用接收
    }
}

//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
//if (huart->Instance == USART1) {
//__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
//}
//}

//// 40位转换函数
//void transfer40(uint16_t value, uint8_t buffer[5]) {
//	  buffer[0] = 0xAB
//    buffer[0] = 0x00;
//    buffer[1] = 0x00;
//    buffer[2] = 0x00; 
//    buffer[3] = (value >> 8) & 0xFF;
//    buffer[4] = value & 0xFF;
//}


   		/*包头：0xAB

字节1：类型 (0x00=幅值, 0x01=频率)

字节2：通道号 (0x01=通道1, 0x02=通道2...)

字节3：波形类型 (0x00=正弦波, 0x01=三角波)

字节4：开始标志 0xCC

字节5-7：幅度整数部分（24位大端序）

字节8-9：幅度小数部分（16位大端序）
				
若为频率
字节5-9：为40位整数位
包尾：0xBA
*/
void build_packet(uint8_t type, uint8_t channel, uint8_t wave_type, 
                  double value, uint32_t freq,uint8_t packet[11]) 
{
    // 包头
    packet[0] = 0xAB;
    // 类型
    packet[1] = type;
    // 通道号
    packet[2] = channel;
    // 波形类型
    packet[3] = wave_type;
    // 开始标志
    packet[4] = 0xCC;

    if (type == 0x00) {  // 幅值类型
        // 分离整数和小数部分
        uint32_t integer_part = (uint32_t)value;
        double fractional = value - integer_part;
        uint16_t fractional_part = (uint16_t)(fractional * 65536.0 + 0.5);  // 四舍五入
        
        // 确保不超过24位限制
        if (integer_part > 0xFFFFFF) integer_part = 0xFFFFFF;
        
        // 写入幅值整数部分（24位大端序）
        packet[5] = (integer_part >> 16) & 0xFF;
        packet[6] = (integer_part >> 8)  & 0xFF;
        packet[7] = integer_part & 0xFF;
        
        // 写入幅值小数部分（16位大端序）
        packet[8] = (fractional_part >> 8) & 0xFF;
        packet[9] = fractional_part & 0xFF;
    } 
    else if (type == 0x01) {  // 频率类型
        // 转换为40位整数
        uint64_t freq_value = (uint64_t)freq;
        
        // 写入频率值（40位大端序）
        packet[5] = (freq_value >> 32) & 0xFF;
        packet[6] = (freq_value >> 24) & 0xFF;
        packet[7] = (freq_value >> 16) & 0xFF;
        packet[8] = (freq_value >> 8)  & 0xFF;
        packet[9] = freq_value & 0xFF;
    }
    else if(type == 0x02){
			 uint64_t  value1 = (uint64_t)(value * 1024*1024*8);
				packet[5] = (value1 >> 32) & 0xFF;
        packet[6] = (value1 >> 24) & 0xFF;
        packet[7] = (value1 >> 16) & 0xFF;
        packet[8] = (value1 >> 8)  & 0xFF;
        packet[9] = value1 & 0xFF;
		
		
		}
    // 包尾
    packet[10] = 0xBA;
}



uint32_t hex_to_decimal(const uint8_t *bytes) {
    // 小端序：低地址存储低位字节    
    return (uint32_t)(
        (bytes[2] << 0)  |  // 最低位字节，左移0位
        (bytes[3] << 8)  |  // 次低位字节，左移8位
        (bytes[4] << 16) |  // 次高位字节，左移16位
        (bytes[5] << 24)    // 最高位字节，左移24位
    );
}
// 发送FIR系数
void send_fir_coefficients(const double fir_coeff[250], uint8_t wave_type) {
    uint8_t packet[11];

    for (uint8_t i = 0; i < 250; i++) {
        // 通道号作为系数编号（从 1 到 100，对应 0x01 到 0x64）
        uint8_t channel = i + 1;

        // 类型为 0x02 表示 FIR 系数
        build_packet(0x02, channel, wave_type, fir_coeff[i],0, packet);


       HAL_UART_Transmit_DMA(&huart1, packet, sizeof(packet));

    }
		build_packet(0x02, 250, 0, 0,0, packet);


       HAL_UART_Transmit_DMA(&huart1, packet, sizeof(packet));
}
/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/

