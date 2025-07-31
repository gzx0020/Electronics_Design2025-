/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "memorymap.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "BSP_USART.h"
#include "BSP_JUDGE.h"
#include "BSP_CONTROL.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define step 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t dataReady=0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_UART8_Init();
  /* USER CODE BEGIN 2 */
//	printf("HELLOWORLD! \r\n");
    txValue = 100;
    uint8_t type=0x01;
    uint8_t channel=0x00;
    uint8_t wave_type=0x00;
//    transfer40(txValue, txBuffer);
    build_packet(type,  channel,  wave_type,
                 txValue,  txBuffer) ;
				build_packet(0x00,  channel,  wave_type,
                 	0.5,  contlV);						 
//    HAL_UART_Transmit_DMA(&huart1, contlV, sizeof(contlV));
//		HAL_UART_DMAStop(&huart1);
HAL_UART_Transmit(&huart1, contlV, sizeof(contlV), 100);
		build_packet(0x00,  channel,  wave_type,
                 	calculate_input_pp(txValue+step),  contlV);
HAL_UART_Transmit(&huart1, contlV, sizeof(contlV), 100);	
    HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
//				build_packet(0x00,  channel,  wave_type,
//                 0.3959,  contlV) ;
//		HAL_UART_Transmit_DMA(&huart1, contlV, sizeof(contlV));							 
//    uint8_t packet[11];
//
//    // 构建幅值包：通道1，正弦波，幅度123.456
//    build_packet(0x00, 0x01, 0x00, 123.456, packet);
//
//    // 构建频率包：通道2，三角波，频率100000
//    build_packet(0x01, 0x02, 0x01, 100000.0, packet);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
//1.发送频率实现扫频
      if (dmaComplete) {
          dmaComplete = 0;
			
          // 解析当前数据点
          dataStorage[storageIndex].integer = (rxBuffer[1] << 16) | 
                                             (rxBuffer[2] << 8) | 
                                             rxBuffer[3];
          dataStorage[storageIndex].fraction = (rxBuffer[4] << 8) | rxBuffer[5];
          
          // 转换为浮点数
          volt[storageIndex] = dataStorage[storageIndex].integer + 
                              (dataStorage[storageIndex].fraction / 65536.0f);
          
	   

          // 更新索引和频率值
//				delay_us(10000);
          storageIndex++;
          txValue += step;
          HAL_UART_Transmit_DMA(&huart1, contlV, sizeof(contlV));
			 HAL_UART_DMAStop(&huart1);
          // 完成一轮扫描
          if(txValue >= 10+ 598*100 | storageIndex >= DATA_POINTS) {
              txValue = 100;
              storageIndex = 0;
              dataReady = 1;  // 标记数据就绪
          }
          
          // 发送下一个频率
          build_packet(type, channel, wave_type, txValue, txBuffer);
          HAL_UART_Transmit_DMA(&huart1, txBuffer, sizeof(txBuffer));
      }
      
      // 2. 频谱分析
      if(dataReady) {
          dataReady = 0;
          				//以下发送用于波形显示调试
				for(uint16_t i=0;i<DATA_POINTS;i++){
					printf("%.2f\n",volt[i]);
				}          
          combine_to_struct(freq, volt, DATA_POINTS, responses);
          const char* filterType = determine_filter_type(responses, DATA_POINTS);
          
             printf("Filter type: %s\n", filterType);
      }



				
	  



//已有幅频特性曲线如何判断滤波器类型
//并计算FIR系数

////		char test1[]="hello world!";
////		HAL_UART_Transmit(&huart1,(uint8_t*)test1,sizeof(test1),HAL_MAX_DELAY);
//		printf("HELLOWORLD! \r\n");
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void delay_us(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
	reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* 刚进入时的计数器值 */
 
    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICK是一个递减的计数器 */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;
 
            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }
} 
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
