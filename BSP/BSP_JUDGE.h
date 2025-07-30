#ifndef _BSP_JUDGE_H
#define _BSP_JUDGE_H

#include "main.h"
#include "BSP_USART.h"

#include <string.h>
#include <stdlib.h>
#include "stdio.h"

typedef struct {
    uint16_t freq;
    double gain;  
}FreqResponse;



extern uint16_t freq[DATA_POINTS];
extern float amplitude[DATA_POINTS];
extern FreqResponse responses[DATA_POINTS];
extern uint8_t n;



const char* determine_filter_type(FreqResponse resp[], int n) ;













#endif



