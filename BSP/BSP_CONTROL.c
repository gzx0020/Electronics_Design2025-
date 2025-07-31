#include "BSP_CONTROL.h"


#include <stdio.h>
#include <math.h>

uint8_t DMA_flag=1;


double calculate_input_pp(double freq) {
    // �����Ƶ�� (omega)
    double omega = 2 * M_PI * freq;
    
    // ���� |H(j��)| �ķ�ֵ
    double real_part = 1 - REAL_PART_COEFFICIENT * omega * omega; // ʵ����1 - 10^{-8}��2
    double imag_part = IMAG_PART_COEFFICIENT * omega;             // �鲿��3��10^{-4}��
    double magnitude_H = NUMERATOR / sqrt(real_part * real_part + imag_part * imag_part);
    
    // ������ֵ
    double output_pp = 2; // ������ֵ��V��
    
    // ����������ֵ
    double input_pp = output_pp / magnitude_H;
    
    return input_pp;
}







