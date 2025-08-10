#include "BSP_CONTROL.h"
#include "BSP_USART.h"

#include <stdio.h>
#include <math.h>
# include "main.h"

uint8_t DMA_flag=1;


//double calculate_input_pp(double freq) {
//    // 计算角频率 (omega)
//    double omega = 2 * M_PI * freq;
//    
//    // 计算 |H(jω)| 的幅值
//    double real_part = 1 - REAL_PART_COEFFICIENT * omega * omega; // 实部：1 - 10^{-8}ω2
//    double imag_part = IMAG_PART_COEFFICIENT * omega;             // 虚部：3×10^{-4}ω
//    double magnitude_H = NUMERATOR / sqrt(real_part * real_part + imag_part * imag_part);
//    
//    // 输出峰峰值
//    double output_pp = 2; // 输出峰峰值（V）
//    
//    // 计算输入峰峰值
//    double input_pp = output_pp / magnitude_H;
//    
//    return input_pp;
//}

double calculate_input_pp(double freq, double output_pp) {
    // 计算角频率 (omega)
    double omega = 2 * M_PI * freq;
    
    // 计算 |H(jω)| 的幅值
    double real_part = 1 - REAL_PART_COEFFICIENT * omega * omega; // 实部：1 - 10^{-8}ω2
    double imag_part = IMAG_PART_COEFFICIENT * omega;             // 虚部：3×10^{-4}ω
    double magnitude_H = NUMERATOR / sqrt(real_part * real_part + imag_part * imag_part);
    
    // 计算输入峰峰值
    double input_pp = (output_pp-0.1) / magnitude_H;
    
    return input_pp;
}

//void process_H7_command(uint8_t header_byte, uint8_t *H7Buffer, uint8_t *contlV, uint8_t *txBuffer, float txValue) {
//    if (header_byte == 0x55) {
//        if (H7Buffer[1] == 0x00) {
//            build_packet(0x01, 0x00, 0x00, hex_to_decimal(H7Buffer), contlV);
//            HAL_UART_Transmit(&huart1, contlV, sizeof(contlV), 100);
////					HAL_UART_Receive(&huart7, H7Buffer, 8, HAL_MAX_DELAY);
//					Stm32_SoftReset();
//        } else if (H7Buffer[1] == 0x01) {
//            build_packet(0x00, 0x00, 0x00, 0.001f * hex_to_decimal(H7Buffer), txBuffer);
//            HAL_UART_Transmit(&huart1, txBuffer, sizeof(txBuffer), 100);
////					HAL_UART_Receive(&huart7, H7Buffer, 8, HAL_MAX_DELAY);
//					Stm32_SoftReset();
//        }
//    } else if (header_byte == 0xAA) {
//        build_packet(0x00, 0x00, 0x00, calculate_input_pp(txValue + step), contlV);
//        HAL_UART_Transmit(&huart1, contlV, sizeof(contlV), 100);
//        HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
//    }
//}

//// 考虑使用软件复位，但系统效率过低，故舍弃
// void Stm32_SoftReset(void)
// {
//   __set_FAULTMASK(1);//禁止所有的可屏蔽中断
//   NVIC_SystemReset();//软件复位
// }
 
 


// 处理串口屏指令
void process_H7_command(uint8_t header_byte, uint8_t *H7Buffer, 
                       uint8_t *txBuffer, uint8_t *contlV,uint16_t *freq_update) 
{
	 
    if(header_byte == 0x55) {
        if(H7Buffer[1] == 0x00) {  // 设置频率

            currentState = STATE_SET_FREQ;
						
        } 
        else if(H7Buffer[1] == 0x01) {  // 设置幅值

            currentState = STATE_SET_AMPLITUDE;
        }
    } 
    else if(header_byte == 0xAA) {  // 启动扫频
        currentState = STATE_LEARNING;        
    }
		else if(header_byte == 0xBB) {  
        currentState = TRANS_FPGA;        
    }
		
}
void handle_sweep_mode(uint8_t* dmaComplete, uint8_t*sweepend, 
                      DataPoint dataStorage[DATA_POINTS], float volt[DATA_POINTS],
                      uint32_t* txValue,  uint8_t* storageIndex,
                      uint8_t txBuffer[], uint8_t rxBuffer[], UART_HandleTypeDef* huart) {
    
    // 发送数据包
    build_packet(0x01, 0x00, 0x00,0, *txValue, txBuffer);
    HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
    
    // 等待DMA传输完成
    while(!(*dmaComplete))
        ;
    *dmaComplete = 0;
    
    // 解析当前数据点
    dataStorage[*storageIndex].integer = (rxBuffer[1] << 16) | 
                                        (rxBuffer[2] << 8) | 
                                        rxBuffer[3];
    dataStorage[*storageIndex].fraction = (rxBuffer[4] << 8) | rxBuffer[5];
    
    // 转换为浮点数
    volt[*storageIndex] = dataStorage[*storageIndex].integer + 
                         (dataStorage[*storageIndex].fraction / 65536.0f);
    
    // 更新索引和频率值
    (*storageIndex)++;
    *txValue += 100;
    
    // 完成一轮扫描
    if(*txValue >= 10 + 598*100 || *storageIndex >= DATA_POINTS) {
        *txValue = 100;
        *storageIndex = 0;
        *sweepend = 1;  // 标记数据就绪
        // currentState = STATE_LEARNING;  // 建议将currentState也作为参数传入
    }
    
    // 发送下一个频率
    build_packet(0x01, 0x00, 0x00, 0,*txValue, txBuffer);
    HAL_UART_Transmit_DMA(huart, txBuffer, sizeof(txBuffer));
}

//void handle_sweep_mode(uint8_t dmaComplete,) {
//	   build_packet(0x01,  0x00,  0x00,
//                 txValue,  txBuffer) ;
//	 HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
//    while(!dmaComplete)
//			;
//          dmaComplete = 0;
//			
//          // 解析当前数据点
//          dataStorage[storageIndex].integer = (rxBuffer[1] << 16) | 
//                                             (rxBuffer[2] << 8) | 
//                                             rxBuffer[3];
//          dataStorage[storageIndex].fraction = (rxBuffer[4] << 8) | rxBuffer[5];
//          
//          // 转换为浮点数
//          volt[storageIndex] = dataStorage[storageIndex].integer + 
//                              (dataStorage[storageIndex].fraction / 65536.0f);
//          
//	   

//          // 更新索引和频率值
////				delay_us(10000);
//          storageIndex++;
//          txValue += step;
//        
//		
//          // 完成一轮扫描
//          if(txValue >= 10+ 598*100 | storageIndex >= DATA_POINTS) {
//              txValue = 100;
//              storageIndex = 0;
////              dataReady = 1;  // 标记数据就绪
//						currentState = STATE_LEARNING;
//          }
//          
//          // 发送下一个频率
//          build_packet(0x01, 0x00, 0x00, txValue, txBuffer);
//          HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
//      
//}

