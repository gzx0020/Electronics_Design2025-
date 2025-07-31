#include "BSP_CONTROL.h"


#include <stdio.h>
#include <math.h>

uint8_t DMA_flag=1;


double calculate_input_pp(double freq) {
    // 计算角频率 (omega)
    double omega = 2 * M_PI * freq;
    
    // 计算 |H(jω)| 的幅值
    double real_part = 1 - REAL_PART_COEFFICIENT * omega * omega; // 实部：1 - 10^{-8}ω2
    double imag_part = IMAG_PART_COEFFICIENT * omega;             // 虚部：3×10^{-4}ω
    double magnitude_H = NUMERATOR / sqrt(real_part * real_part + imag_part * imag_part);
    
    // 输出峰峰值
    double output_pp = 2; // 输出峰峰值（V）
    
    // 计算输入峰峰值
    double input_pp = output_pp / magnitude_H;
    
    return input_pp;
}







