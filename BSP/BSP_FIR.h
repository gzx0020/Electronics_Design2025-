#ifndef _BSP_FIR_H
#define _BSP_FIR_H

#include "main.h"
#include "BSP_USART.h"
#include "BSP_JUDGE.h"
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

# define M_PI 3.1415926
extern double fir_coeff[500];
extern int point_num;
extern float Fs;
extern int N;
extern int L ;



//typedef struct {
//    uint32_t freq;  // 频率 (Hz)
//    float gain;     // 增益 (线性标度)
//} FreqResponse;
// 复数结构
typedef struct {
    double real;
    double imag;
} Complex;


// 窗函数类型枚举
typedef enum {
    RECTANGULAR,
    HANNING,
    HAMMING,
    BLACKMAN
} WindowType;
extern    int point_num ;  // 数据点数量
extern    float Fs ;    // 采样率8kHz
extern    int N;         // 滤波器长度
 extern   int L ;       // 频域采样点数
extern   double h_coeffs[501]; 

extern Complex com;
double interpolate_gain(FreqResponse* resp, int point_num, double freq);
void generate_window(double* window, int N, WindowType type);
void complex_fft(Complex* x, int L, int inverse);
void compute_fir_coeffs(
    FreqResponse* resp,    // 幅频数据数组
    int point_num,         // 数据点数量
    float Fs,              // 采样频率 (Hz)
    int N,                 // 滤波器长度 (系数个数)
    WindowType window_type,// 窗函数类型
    int L,                 // 频域采样点数 (需为2的幂)
    double* h_final        // 输出滤波器系数数组
);



#endif
