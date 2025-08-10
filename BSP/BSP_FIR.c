#include "BSP_FIR.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "BSP_JUDGE.h"
# define M_PI 3.1415926
double fir_coeff[500] = {

-0.000000, -0.000000, -0.000001, -0.000001, -0.000001, -0.000001, -0.000000, 0.000001, 0.000004, 0.000007, 0.000011, 0.000015, 0.000018, 0.000022, 0.000024, 0.000024, 0.000023, 0.000018, 0.000011, 0.000000, -0.000014, -0.000031, -0.000052, -0.000075, -0.000100, -0.000127, -0.000153, -0.000180, -0.000204, -0.000226, -0.000243, -0.000256, -0.000262, -0.000263, -0.000256, -0.000242, -0.000222, -0.000194, -0.000162, -0.000125, -0.000084, -0.000043, -0.000001, 0.000038, 0.000073, 0.000103, 0.000124, 0.000137, 0.000139, 0.000129, 0.000108, 0.000074, 0.000029, -0.000027, -0.000093, -0.000166, -0.000245, -0.000328, -0.000411, -0.000493, -0.000569, -0.000639, -0.000698, -0.000745, -0.000778, -0.000796, -0.000797, -0.000782, -0.000750, -0.000703, -0.000642, -0.000569, -0.000486, -0.000396, -0.000302, -0.000208, -0.000117, -0.000031, 0.000045, 0.000109, 0.000159, 0.000192, 0.000208, 0.000205, 0.000184, 0.000145, 0.000090, 0.000020, -0.000063, -0.000155, -0.000253, -0.000354, -0.000455, -0.000552, -0.000641, -0.000720, -0.000785, -0.000835, -0.000868, -0.000882, -0.000878, -0.000856, -0.000816, -0.000760, -0.000690, -0.000609, -0.000519, -0.000424, -0.000327, -0.000231, -0.000140, -0.000057, 0.000016, 0.000076, 0.000122, 0.000151, 0.000164, 0.000160, 0.000140, 0.000105, 0.000056, -0.000005, -0.000075, -0.000152, -0.000233, -0.000314, -0.000394, -0.000469, -0.000537, -0.000596, -0.000643, -0.000679, -0.000701, -0.000709, -0.000704, -0.000686, -0.000656, -0.000615, -0.000566, -0.000509, -0.000447, -0.000383, -0.000318, -0.000254, -0.000193, -0.000137, -0.000087, -0.000045, -0.000011, 0.000015, 0.000032, 0.000040, 0.000041, 0.000035, 0.000022, 0.000005, -0.000016, -0.000039, -0.000064, -0.000088, -0.000111, -0.000131, -0.000149, -0.000163, -0.000173, -0.000178, -0.000180, -0.000177, -0.000170, -0.000161, -0.000148, -0.000134, -0.000118, -0.000102, -0.000085, -0.000069, -0.000054, -0.000041, -0.000029, -0.000019, -0.000011, -0.000004, -0.000000, 0.000003, 0.000004, 0.000005, 0.000004, 0.000003, 0.000002, 0.000001, -0.000000, -0.000001, -0.000001, -0.000001, -0.000001, -0.000001, -0.000000, -0.000000, -0.000000


											//FIR系数...
										};

Complex com;
										
    int point_num =DATA_POINTS;  // 数据点数量
    float Fs = 1000000;    
    int N =501;         // 滤波器长度
    int L = 1024;       // 频域采样点数
   double h_coeffs[501]; 

// 线性插值计算任意频率增益
double interpolate_gain(FreqResponse* resp, int point_num, double freq) {
    // 边界检查
    if (point_num <= 0 || !resp) return 0.0;
    
    // 低于最小频率
    if (freq <= resp[0].freq) {
        return (resp[0].gain > 0) ? resp[0].gain : 0.0;
    }
    
    // 高于最大频率
    if (freq >= resp[point_num-1].freq) {
        return (resp[point_num-1].gain > 0) ? resp[point_num-1].gain : 0.0;
    }
    
    // 查找区间
    int left = 0;
    while (left < point_num-1 && resp[left+1].freq < freq) {
        left++;
    }
    
    // 有效性检查
    if (left >= point_num-1) return 0.0;
    
    // 线性插值
    double f0 = resp[left].freq;
    double f1 = resp[left+1].freq;
    double g0 = resp[left].gain;
    double g1 = resp[left+1].gain;
    
    // 防止除零
    if (fabs(f1 - f0) < 1e-9) return (g0 + g1)/2.0;
    
    return g0 + (g1 - g0) * (freq - f0) / (f1 - f0);
}

// 生成窗函数
void generate_window(double* window, int N, WindowType type) {
    switch (type) {
        case RECTANGULAR:  // 矩形窗
            for (int n = 0; n < N; n++) 
                window[n] = 1.0;
            break;
            
        case HANNING:  // 汉宁窗
            for (int n = 0; n < N; n++) 
                window[n] = 0.5 - 0.5 * cos(2 * M_PI * n / (N - 1));
            break;
            
        case HAMMING:  // 汉明窗
            for (int n = 0; n < N; n++) 
                window[n] = 0.54 - 0.46 * cos(2 * M_PI * n / (N - 1));
            break;
            
        case BLACKMAN:  // 布莱克曼窗
            for (int n = 0; n < N; n++) 
                window[n] = 0.42 - 0.5 * cos(2 * M_PI * n / (N - 1)) 
                         + 0.08 * cos(4 * M_PI * n / (N - 1));
            break;
    }
}

// Cooley-Tukey FFT算法(基2)
void complex_fft(Complex* x, int L, int inverse) {
    // 位反转重排
    for (int i = 0, j = 0; i < L; i++) {
        if (j > i) {
            Complex temp = x[i];
            x[i] = x[j];
            x[j] = temp;
        }
        int m = L >> 1;
        while (m >= 1 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    // 蝶形运算
    for (int s = 1; s <= log2(L); s++) {
        int m = 1 << s;
        int m2 = m >> 1;
        double theta = (inverse ? 1 : -1) * 2 * M_PI / m;
        
        for (int j = 0; j < m2; j++) {
            Complex w = {cos(j * theta), sin(j * theta)};
            for (int k = j; k < L; k += m) {
                Complex t = {w.real * x[k + m2].real - w.imag * x[k + m2].imag,
                            w.real * x[k + m2].imag + w.imag * x[k + m2].real};
                Complex u = x[k];
                x[k].real = u.real + t.real;
                x[k].imag = u.imag + t.imag;
                x[k + m2].real = u.real - t.real;
                x[k + m2].imag = u.imag - t.imag;
            }
        }
    }

    // 逆FFT需缩放
    if (inverse) {
        for (int i = 0; i < L; i++) {
            x[i].real /= L;
            x[i].imag /= L;
        }
    }
}

void compute_fir_coeffs(
    FreqResponse* resp, int point_num, float Fs, int N,
    WindowType window_type, int L, double* h_final) 
{
    // 1. 参数检查
    if (N < 1 || L < N || Fs <= 0) {
        
        return;
    }
    
    // 2. 计算合理的缩放因子
    double max_gain = 0.0;
    for (int i = 0; i < point_num; i++) {
        if (resp[i].gain > max_gain && resp[i].gain < 1e6) { // 忽略异常大值
            max_gain = resp[i].gain;
        }
    }
    
    // 使用对数缩放仅当增益变化范围很大时
    double scale_factor = 1.0;
    if (max_gain > 10.0) {
        scale_factor = log(max_gain);
       
    } else {
        scale_factor = 1.0;
    }
    
    // 3. 分配内存
    double* gain_full = (double*)malloc(L * sizeof(double));
    Complex* H = (Complex*)malloc(L * sizeof(Complex));
    double d = (N - 1) / 2.0; 
    double df = Fs / L;  // 频率分辨率
    
    // 4. 构建全频带增益(0~Fs)
    for (int k = 0; k <= L/2; k++) {
        double freq = k * df;
        double gain = interpolate_gain(resp, point_num, freq);
        gain_full[k] = gain;
        
        // 确保增益在合理范围内
        if (gain_full[k] > 1.0) gain_full[k] = 1.0;
        if (gain_full[k] < 0.0) gain_full[k] = 0.0;
    }
    
    // 5. 共轭对称部分
    for (int k = L/2 + 1; k < L; k++) {
        gain_full[k] = gain_full[L - k];  // 共轭对称
    }
    
    // 6. 构建频域响应
    for (int k = 0; k < L; k++) {
        double phase = -2 * M_PI * k * d / L;
        
        // 优化相位计算
        phase = fmod(phase, 2 * M_PI);
        if (phase > M_PI) phase -= 2 * M_PI;
        if (phase < -M_PI) phase += 2 * M_PI;
        
        // 使用CMSIS-DSP的精确三角函数
        H[k].real = gain_full[k] * arm_cos_f32((float32_t)phase);
        H[k].imag = gain_full[k] * (-arm_sin_f32((float32_t)phase));
    }
    
    // 7. 使用CMSIS-DSP进行逆FFT
    arm_cfft_instance_f32 fft_instance;
    arm_status status = arm_cfft_init_f32(&fft_instance, L);
    if (status != ARM_MATH_SUCCESS) {
        
        free(gain_full);
        free(H);
        return;
    }
    
    float32_t fft_buffer[2*L] ; 
		
//    if (!fft_buffer) {
//       
//        free(gain_full);
//        free(H);
//        return;
//    }
    
    // 8. 复制数据到FFT缓冲区
    for (int i = 0; i < L; i++) {
        fft_buffer[2*i] = (float32_t)H[i].real;
        fft_buffer[2*i+1] = (float32_t)H[i].imag;
    }
    
    // 9. 执行逆FFT
    arm_cfft_f32(&fft_instance, fft_buffer, 1, 1);
    
    // 10. 归一化
    float32_t scale = 1.0f / L;
    arm_scale_f32(fft_buffer, scale, fft_buffer, 2 * L);
    
    // 11. 提取实部
    double* h_d = (double*)malloc(N * sizeof(double));
    for (int i = 0; i < N; i++) {
        h_d[i] = (double)fft_buffer[2*i];
    }
//    // 12. 循环移位使主瓣居中（修正版）
//int center_index = (N - 1) / 2;
//double* h_shifted = (double*)malloc(N * sizeof(double));

//// 创建临时数组存储移位结果
//for (int i = 0; i < N; i++) {
//    // 计算目标位置：将中心点移到数组中间
//    int target_idx = (i + center_index) % N;
//    h_shifted[target_idx] = h_d[i];
//}

//// 复制回原数组
//memcpy(h_d, h_shifted, N * sizeof(double));
//free(h_shifted);
    // 12. 循环移位使主瓣居中
    double* h_shifted = (double*)malloc(N * sizeof(double));
    int center_index = (N - 1) / 2;
    for (int i = 0; i < N; i++) {
        int shift_idx = (i + center_index) % N;
        h_shifted[i] = h_d[shift_idx];
    }
    memcpy(h_d, h_shifted, N * sizeof(double));
    free(h_shifted);
    
    // 13. 强制对称
    int mid = (N - 1) / 2;
    for (int i = 0; i < mid; i++) {
        double avg = (h_d[i] + h_d[N - 1 - i]) / 2.0;
        h_d[i] = avg;
        h_d[N - 1 - i] = avg;
    }
    
    // 13.1 添加对称性验证和修正
    double max_asymmetry = 0.0;
    for (int i = 0; i < mid; i++) {
        double diff = fabs(h_d[i] - h_d[N - 1 - i]);
        if (diff > max_asymmetry) max_asymmetry = diff;
        
        if (diff > 1e-9) {
            double avg = (h_d[i] + h_d[N - 1 - i]) / 2.0;
            h_d[i] = avg;
            h_d[N - 1 - i] = avg;
        }
    }
   
    
    // 14. 生成窗函数
    double* window = (double*)malloc(N * sizeof(double));
    generate_window(window, N, window_type);
    
    // 15. 加窗得到最终系数
    for (int i = 0; i < N; i++) {
        h_final[i] = h_d[i] * window[i] / scale_factor;
    }
    
    // 16. 释放内存
    free(gain_full);
    free(H);
    free(fft_buffer);
    free(window);
    free(h_d);
    
    // 17. 验证结果
    double max_coeff = 0.0;
    for (int i = 0; i < N; i++) {
        if (fabs(h_final[i]) > max_coeff) 
            max_coeff = fabs(h_final[i]);
    }
    
    // 18. 验证系数对称性
    
    double max_diff = 0.0;
    for (int i = 0; i < mid; i++) {
        double diff = fabs(h_final[i] - h_final[N - 1 - i]);
        if (diff > max_diff) max_diff = diff;
    }
  
    
    // 19. 调整系数增益（避免过小）
    if (max_coeff < 1e-6) {
        
        double gain_factor = 1.0 / max_coeff * 1e-3;
        for (int i = 0; i < N; i++) {
            h_final[i] *= gain_factor;
        }
    }
}



										