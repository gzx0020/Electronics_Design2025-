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
//    uint32_t freq;  // Ƶ�� (Hz)
//    float gain;     // ���� (���Ա��)
//} FreqResponse;
// �����ṹ
typedef struct {
    double real;
    double imag;
} Complex;


// ����������ö��
typedef enum {
    RECTANGULAR,
    HANNING,
    HAMMING,
    BLACKMAN
} WindowType;
extern    int point_num ;  // ���ݵ�����
extern    float Fs ;    // ������8kHz
extern    int N;         // �˲�������
 extern   int L ;       // Ƶ���������
extern   double h_coeffs[501]; 

extern Complex com;
double interpolate_gain(FreqResponse* resp, int point_num, double freq);
void generate_window(double* window, int N, WindowType type);
void complex_fft(Complex* x, int L, int inverse);
void compute_fir_coeffs(
    FreqResponse* resp,    // ��Ƶ��������
    int point_num,         // ���ݵ�����
    float Fs,              // ����Ƶ�� (Hz)
    int N,                 // �˲������� (ϵ������)
    WindowType window_type,// ����������
    int L,                 // Ƶ��������� (��Ϊ2����)
    double* h_final        // ����˲���ϵ������
);



#endif
