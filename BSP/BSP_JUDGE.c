#include "BSP_JUDGE.h"


uint16_t freq[DATA_POINTS] = {0};


float amplitude[DATA_POINTS] = {0.0};

uint8_t n = sizeof(freq) / sizeof(freq[0]);


FreqResponse responses[DATA_POINTS];




// ???????????????????????????????
void combine_to_struct(const double freq[], const double amplitude[], int n, FreqResponse responses[]) {
    for (int i = 0; i < n; i++) {
        responses[i].freq = freq[i];
        responses[i].gain = amplitude[i];
    }
}


const char* determine_filter_type(FreqResponse resp[], int n) {
    // 1. 找到最大增益
    double max_gain = resp[0].gain;
    for (int i = 1; i < n; i++) {
        if (resp[i].gain > max_gain) {
            max_gain = resp[i].gain;
        }
    }
    double threshold = max_gain * 0.707;  // -3dB阈值

    // 2. 检查首尾增益特性
   uint8_t  low_start = (resp[0].gain >= threshold);
   uint8_t  high_end = (resp[n-1].gain >= threshold);

    // 3. 判断类型
    if (low_start && !high_end) {
        return "Low-pass";
    } else if (!low_start && high_end) {
        return "High-pass";
    } else if (!low_start && !high_end) {
        // 检查带通：是否存在连续频段增益超过阈值
        for (int i = 0; i < n; i++) {
            if (resp[i].gain >= threshold) {
                return "Band-pass";
            }
        }
        return "Unknown";  // 无通带
    } else {  // low_start && high_end
        // 检查带阻：是否存在连续频段增益低于阈值
        for (int i = 0; i < n; i++) {
            if (resp[i].gain < threshold) {
                return "Band-stop";
            }
        }
        return "All-pass";  // 无阻带
    }
}



