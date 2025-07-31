#include "BSP_JUDGE.h"


uint16_t freq[DATA_POINTS] = {0};




float amplitude[DATA_POINTS] = {0.0};

FreqResponse responses[DATA_POINTS];


void combine_to_struct(double freq[],  double amplitude[], int n, FreqResponse responses[]) {
	for (uint16_t i=0;i<DATA_POINTS;i++){
	freq[i]=100+i*100;
}
    for (int i = 0; i < DATA_POINTS; i++) {
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
    int low_pass = 1;
    int high_pass = 1;
    int band_pass = 0;
    int band_stop = 0;
    
    int zero_crossings = 0;  
    int min_gain_idx = -1, max_gain_idx = -1;
    float max_gain = resp[0].gain; 
    float min_gain = resp[0].gain; 
//    smooth_curve(resp, n, 2);  // 平滑滤波
if (n > 0) {
     // 初始化最大值为第一个元素
    for (uint16_t i = 1; i < DATA_POINTS; i++) {
        if (resp[i].gain > max_gain) {  // 如果当前元素更大
            max_gain = resp[i].gain;    // 更新最大值
        }
    }
}
printf("%.2f\n", max_gain);
// 寻找最小值
if (n > 0) {
     // 初始化最小值为第一个元素
    for (uint16_t i = 1; i < DATA_POINTS; i++) {
        if (resp[i].gain < min_gain) {  // 如果当前元素更小
            min_gain = resp[i].gain;    // 更新最小值
        }
    }
}
printf("%.2f\n", min_gain);
     float mid=(max_gain-min_gain)*0.77;
    for (int i = 1; i < DATA_POINTS; i++) {
        
        if ((resp[i-1].gain > mid && resp[i].gain < mid) || (resp[i-1].gain < mid && resp[i].gain > mid)) {
            zero_crossings++;  
        }

        
        if (resp[i].gain < resp[min_gain_idx].gain || min_gain_idx == -1) {
            min_gain_idx = i;
        }
        if (resp[i].gain > resp[max_gain_idx].gain || max_gain_idx == -1) {
            max_gain_idx = i;
        }
    }

    
    if (zero_crossings == 2) {
        
        if (resp[max_gain_idx].gain > resp[min_gain_idx].gain) {
            band_pass = 1; 
        } else {
            band_stop = 1;  
        }
    }


    if (zero_crossings == 0) {
        
        if (resp[n-1].gain > mid) {
            return "Low-pass";  
        } else {
            return "High-pass";  
        }
    } else if (zero_crossings == 1) {
        
        if (resp[n-1].gain > mid) {
            return "Low-pass"; 
        } else {
            return "High-pass";  
        }
    } else if (band_pass) {
        return "Band-pass";  
    } else if (band_stop) {
        return "Band-stop";  
    }

    return "Unknown";
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


