#include "BSP_JUDGE.h"


uint32_t freq[DATA_POINTS] = {0};




float amplitude[DATA_POINTS] = {0.0};

FreqResponse responses[DATA_POINTS];


void combine_to_struct(uint32_t freq[], float amplitude[], int n, FreqResponse responses[]) {

    for (uint16_t i = 0; i < DATA_POINTS; i++) {
        responses[i].freq = freq[i];
        responses[i].gain = amplitude[i];
    }
}

//const char* determine_filter_type(FreqResponse resp[], int n) {
//   
//    int idx_low = 0, idx_high = 0;
//    for (int i = 1; i < n; i++) {
//        if (resp[i].freq < resp[idx_low].freq) idx_low = i;
//        if (resp[i].freq > resp[idx_high].freq) idx_high = i;
//    }

//    
//    double low_gain = resp[idx_low].gain;
//    double high_gain = resp[idx_high].gain;
//    
//    // 计算增益阈值
//    double max_gain = resp[0].gain;
//    for (int i = 1; i < n; i++) {
//        if (resp[i].gain > max_gain) max_gain = resp[i].gain;
//    }
//    double threshold = max_gain * 0.707;

//    // 判断低通或高通
//    if (low_gain >= threshold && high_gain < threshold) {
//        return "Low-pass";
//    } else if (low_gain < threshold && high_gain >= threshold) {
//        return "High-pass";
//    }

//    // 判断带通或带阻
//    int band_pass_detected = 0;
//    int band_stop_detected = 0;
//    for (int i = 0; i < n; i++) {
//        if (resp[i].gain >= threshold) band_pass_detected = 1;
//        if (resp[i].gain < threshold) band_stop_detected = 1;
//    }
//    if (band_pass_detected && band_stop_detected) {
//        return "Band-pass";
//    } else if (!band_pass_detected && band_stop_detected) {
//        return "Band-stop";
//    }
//    
//    return "Unknown";
//}

//const char* determine_filter_type(FreqResponse resp[], int n) {
//    
//    int low_pass = 1;
//    int high_pass = 1;
//    int band_pass = 0;
//    int band_stop = 0;
//    float threshold = 2; 

//    for (int i = 1; i < n - 1; i++) {
//        if (resp[i].gain < resp[i-1].gain && resp[i].gain < resp[i+1].gain) {
//            band_stop = 1;  // 有带阻特征
//        }
//        if (resp[i].gain > resp[i-1].gain && resp[i].gain > resp[i+1].gain) {
//            band_pass = 1;  // 有带通特征
//        }
//        
//      
//        if (resp[i].freq < resp[n-1].freq / 2 && resp[i].gain < threshold) {
//            high_pass = 0; 
//        }
//        if (resp[i].freq > resp[n-1].freq / 2 && resp[i].gain > threshold) {
//            low_pass = 0;  
//        }
//    }

//   
//    if (band_pass) {
//        return "Band-pass";
//    } else if (band_stop) {
//        return "Band-stop";
//    } else if (low_pass) {
//        return "Low-pass";
//    } else if (high_pass) {
//        return "High-pass";
//    } else {
//        return "Unknown";
//    }
//}

//const char* determine_filter_type(FreqResponse resp[], int n) {
//    
//    int low_pass = 1;
//    int high_pass = 1;
//    int band_pass = 0;
//    int band_stop = 0;
//    float threshold = 2.0;
//    float gain_change_threshold = 5.0;

//   
//    smooth_curve(resp, n, 2);

//    for (int i = 1; i < n - 1; i++) {        
//        if (fabs(resp[i].gain - resp[i-1].gain) > gain_change_threshold) {
//            continue; // 如果增益变化过大，则跳过
//        }

//        // 带阻
//        if (resp[i].gain < resp[i-1].gain && resp[i].gain < resp[i+1].gain) {
//            band_stop = 1;  
//        }

//        // 带通
//        if (resp[i].gain > resp[i-1].gain && resp[i].gain > resp[i+1].gain) {
//            band_pass = 1;  
//        }

//        // 低/高
//        if (resp[i].freq < resp[n-1].freq / 2 && resp[i].gain < threshold) {
//            high_pass = 0; 
//        }
//        if (resp[i].freq > resp[n-1].freq / 2 && resp[i].gain > threshold) {
//            low_pass = 0;  
//        }
//    }
//   
//    if (band_pass) {
//        return "Band-pass";
//    } else if (band_stop) {
//        return "Band-stop";
//    } else if (low_pass) {
//        return "Low-pass";
//    } else if (high_pass) {
//        return "High-pass";
//    } else {
//        return "Unknown";
//    }
//}


// 平滑处理：简单移动平均
void smooth_curve(FreqResponse resp[], int n, int window_size) {
    float smoothed_gain;
    for (int i = window_size; i < n - window_size; i++) {
        smoothed_gain = 0;
        for (int j = -window_size; j <= window_size; j++) {
            smoothed_gain += resp[i + j].gain;
        }
        resp[i].gain = smoothed_gain / (2 * window_size + 1); 
    }
}



const char* determine_filter_type(FreqResponse resp[], int n) {
    

    // 1. 找到最低频和最高频的索引
    int idx_low = 0, idx_high = 0;
    for (int i = 1; i < n; i++) {
        if (resp[i].freq < resp[idx_low].freq) idx_low = i;
        if (resp[i].freq > resp[idx_high].freq) idx_high = i;
    }
    double low_gain = resp[idx_low].gain;
    double high_gain = resp[idx_high].gain;

    // 2. 计算最大增益和阈值（-3dB点）
    double max_gain = resp[0].gain;
    for (int i = 1; i < n; i++) {
        if (resp[i].gain > max_gain) max_gain = resp[i].gain;
    }
    double threshold = max_gain * 0.9; // 线性增益阈值

    // 3. 判断滤波器类型
    // 低通: 低频通过 & 高频衰减
    if (low_gain >= threshold && high_gain < threshold) {
        return "Low-pass";
    }
    // 高通: 低频衰减 & 高频通过
    if (low_gain < threshold && high_gain >= threshold) {
        return "High-pass";
    }
    // 带通: 两端均衰减 → 中间必有通过（因max_gain≥threshold）
    if (low_gain < threshold && high_gain < threshold) {
        return "Band-pass";
    }
    // 带阻: 两端均通过 & 中间存在衰减点
    if (low_gain >= threshold && high_gain >= threshold) {
        for (int i = 0; i < n; i++) {
            if (resp[i].gain < threshold) {
                return "Band-stop"; // 发现衰减点 → 带阻
            }
        }
        // 无衰减点 → 全通（非预期，按默认处理）
    }
    return "low-stop";
}






//const char* determine_filter_type(FreqResponse resp[], int n) {
//    int low_pass = 1;
//    int high_pass = 1;
//    int band_pass = 0;
//    int band_stop = 0;
//    
//    int zero_crossings = 0;  

//    smooth_curve(resp, n, 2);  

//    for (int i = 1; i < n; i++) {      
//        if ((resp[i-1].gain > 0 && resp[i].gain < 0) || (resp[i-1].gain < 0 && resp[i].gain > 0)) {
//            zero_crossings++;
//        }
//    }

// 
//    if (zero_crossings == 0) {     
//        if (resp[n-1].gain > 0) {
//            return "Low-pass";  
//        } else {
//            return "High-pass"; 
//        }
//    } else if (zero_crossings == 1) {      
//        if (resp[n-1].gain > 0) {
//            return "Low-pass";  
//        } else {
//            return "High-pass";  
//        }
//    } else if (zero_crossings == 2) {     
//        return "Band-pass";
//    } else if (zero_crossings > 2) {
//        return "Band-stop";
//    }

//    return "Unknown";  
//}


