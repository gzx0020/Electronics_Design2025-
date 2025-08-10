#include "BSP_CONTROL.h"
#include "BSP_USART.h"

#include <stdio.h>
#include <math.h>
# include "main.h"

uint8_t DMA_flag=1;


//double calculate_input_pp(double freq) {
//    // �����Ƶ�� (omega)
//    double omega = 2 * M_PI * freq;
//    
//    // ���� |H(j��)| �ķ�ֵ
//    double real_part = 1 - REAL_PART_COEFFICIENT * omega * omega; // ʵ����1 - 10^{-8}��2
//    double imag_part = IMAG_PART_COEFFICIENT * omega;             // �鲿��3��10^{-4}��
//    double magnitude_H = NUMERATOR / sqrt(real_part * real_part + imag_part * imag_part);
//    
//    // ������ֵ
//    double output_pp = 2; // ������ֵ��V��
//    
//    // ����������ֵ
//    double input_pp = output_pp / magnitude_H;
//    
//    return input_pp;
//}

double calculate_input_pp(double freq, double output_pp) {
    // �����Ƶ�� (omega)
    double omega = 2 * M_PI * freq;
    
    // ���� |H(j��)| �ķ�ֵ
    double real_part = 1 - REAL_PART_COEFFICIENT * omega * omega; // ʵ����1 - 10^{-8}��2
    double imag_part = IMAG_PART_COEFFICIENT * omega;             // �鲿��3��10^{-4}��
    double magnitude_H = NUMERATOR / sqrt(real_part * real_part + imag_part * imag_part);
    
    // ����������ֵ
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

//// ����ʹ�������λ����ϵͳЧ�ʹ��ͣ�������
// void Stm32_SoftReset(void)
// {
//   __set_FAULTMASK(1);//��ֹ���еĿ������ж�
//   NVIC_SystemReset();//�����λ
// }
 
 


// ��������ָ��
void process_H7_command(uint8_t header_byte, uint8_t *H7Buffer, 
                       uint8_t *txBuffer, uint8_t *contlV,uint16_t *freq_update) 
{
	 
    if(header_byte == 0x55) {
        if(H7Buffer[1] == 0x00) {  // ����Ƶ��

            currentState = STATE_SET_FREQ;
						
        } 
        else if(H7Buffer[1] == 0x01) {  // ���÷�ֵ

            currentState = STATE_SET_AMPLITUDE;
        }
    } 
    else if(header_byte == 0xAA) {  // ����ɨƵ
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
    
    // �������ݰ�
    build_packet(0x01, 0x00, 0x00,0, *txValue, txBuffer);
    HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
    
    // �ȴ�DMA�������
    while(!(*dmaComplete))
        ;
    *dmaComplete = 0;
    
    // ������ǰ���ݵ�
    dataStorage[*storageIndex].integer = (rxBuffer[1] << 16) | 
                                        (rxBuffer[2] << 8) | 
                                        rxBuffer[3];
    dataStorage[*storageIndex].fraction = (rxBuffer[4] << 8) | rxBuffer[5];
    
    // ת��Ϊ������
    volt[*storageIndex] = dataStorage[*storageIndex].integer + 
                         (dataStorage[*storageIndex].fraction / 65536.0f);
    
    // ����������Ƶ��ֵ
    (*storageIndex)++;
    *txValue += 100;
    
    // ���һ��ɨ��
    if(*txValue >= 10 + 598*100 || *storageIndex >= DATA_POINTS) {
        *txValue = 100;
        *storageIndex = 0;
        *sweepend = 1;  // ������ݾ���
        // currentState = STATE_LEARNING;  // ���齫currentStateҲ��Ϊ��������
    }
    
    // ������һ��Ƶ��
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
//          // ������ǰ���ݵ�
//          dataStorage[storageIndex].integer = (rxBuffer[1] << 16) | 
//                                             (rxBuffer[2] << 8) | 
//                                             rxBuffer[3];
//          dataStorage[storageIndex].fraction = (rxBuffer[4] << 8) | rxBuffer[5];
//          
//          // ת��Ϊ������
//          volt[storageIndex] = dataStorage[storageIndex].integer + 
//                              (dataStorage[storageIndex].fraction / 65536.0f);
//          
//	   

//          // ����������Ƶ��ֵ
////				delay_us(10000);
//          storageIndex++;
//          txValue += step;
//        
//		
//          // ���һ��ɨ��
//          if(txValue >= 10+ 598*100 | storageIndex >= DATA_POINTS) {
//              txValue = 100;
//              storageIndex = 0;
////              dataReady = 1;  // ������ݾ���
//						currentState = STATE_LEARNING;
//          }
//          
//          // ������һ��Ƶ��
//          build_packet(0x01, 0x00, 0x00, txValue, txBuffer);
//          HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
//      
//}

