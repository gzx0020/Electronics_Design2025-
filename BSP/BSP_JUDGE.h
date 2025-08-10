#ifndef _BSP_JUDGE_H
#define _BSP_JUDGE_H

#include "main.h"
#include "BSP_USART.h"

#include <string.h>
#include <stdlib.h>
#include "stdio.h"

typedef struct {
    uint32_t freq;
    float gain;  
}FreqResponse;



extern uint32_t freq[DATA_POINTS];
extern float amplitude[DATA_POINTS];
extern FreqResponse responses[DATA_POINTS];
extern uint8_t n;


void combine_to_struct(uint32_t freq[], float amplitude[], int n, FreqResponse responses[]);
const char* determine_filter_type(FreqResponse resp[], int n) ;
void smooth_curve(FreqResponse resp[], int n, int window_size);












#endif



