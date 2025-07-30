#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "main.h"
#include "usart.h"

#include <string.h>
#include <stdlib.h>
#include "stdio.h"	

void buildpacket(uint8_t type, uint8_t channel, uint8_t wave_type, uint64_t freq, uint8_t* packet) ;


#endif
