#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "main.h"
#include "usart.h"

#include <string.h>
#include <stdlib.h>
#include "stdio.h"	
typedef enum {
    SINE_WAVE = 0x00,  // 正弦波
    ADC_WAVE = 0x01    // ADC 波形
} waveform_type_t;

typedef enum {
    CHANNEL_ONE = 0x01, // 通道一
    CHANNEL_TWO = 0x02  // 通道二
} channel_t;



void build_data_packet(uint8_t *packet, double value, waveform_type_t waveform, channel_t channel, uint8_t *data_packet) ;


#endif
