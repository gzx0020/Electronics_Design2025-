#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "main.h"
#include "usart.h"

#include <string.h>
#include <stdlib.h>
#include "stdio.h"	

#define DATA_POINTS 599


typedef enum {
    SINE_WAVE = 0x00,  // 正弦波
    ADC_WAVE = 0x01    // ADC 波形
} waveform_type_t;

typedef enum {
    CHANNEL_ONE = 0x01, // 通道一
    CHANNEL_TWO = 0x02  // 通道二
} channel_t;



// 接收数据结构体
typedef struct {
    uint32_t integer : 24;  // 24位整数部分
    uint16_t fraction;      // 16位小数部分
} DataPoint;

extern uint8_t txBuffer[11]; 
extern uint8_t rxBuffer[7];
extern DataPoint dataStorage[DATA_POINTS];
extern uint16_t storageIndex ;
extern volatile uint8_t dmaComplete;
extern uint16_t txValue;
extern float volt[DATA_POINTS];
extern uint8_t contlV[11];

// 函数声明
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void transfer40(uint16_t value, uint8_t buffer[5]) ;
void processStoredData(void);
void build_packet(uint8_t type, uint8_t channel, uint8_t wave_type, 
                  double value, uint8_t packet[11]) ;
#endif
