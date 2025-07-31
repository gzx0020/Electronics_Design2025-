#ifndef _BSP_CONTROL_H
#define _BSP_CONTROL_H

#include "main.h"
#include "BSP_USART.h"


#define M_PI 3.1415926
#define NUMERATOR 5
#define REAL_PART_COEFFICIENT 1e-8
#define IMAG_PART_COEFFICIENT 3e-4

extern uint8_t DMA_flag;


double calculate_input_pp(double freq);


#endif


