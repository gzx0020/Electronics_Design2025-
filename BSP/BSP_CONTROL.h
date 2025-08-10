#ifndef _BSP_CONTROL_H
#define _BSP_CONTROL_H

#include "main.h"
#include "BSP_USART.h"


#define M_PI 3.1415926
#define NUMERATOR 5
#define REAL_PART_COEFFICIENT 1e-8
#define IMAG_PART_COEFFICIENT 3e-4
#define step 500
extern uint8_t DMA_flag;

double calculate_input_pp(double freq, double output_pp);
void process_H7_command(uint8_t header_byte, uint8_t *H7Buffer, 
                       uint8_t *txBuffer, uint8_t *contlV,uint16_t *freq_update);

void handle_sweep_mode(uint8_t* dmaComplete, uint8_t*sweepend, 
                      DataPoint dataStorage[DATA_POINTS], float volt[DATA_POINTS],
                      uint32_t* txValue,  uint8_t* storageIndex,
                      uint8_t txBuffer[], uint8_t rxBuffer[], UART_HandleTypeDef* huart);
#endif


