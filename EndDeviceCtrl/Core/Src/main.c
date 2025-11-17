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
	
#include <stdio.h>
#include <string.h>
#include "dht11.h"

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t cmd_pc;
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
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&cmd_pc, sizeof(cmd_pc));
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	char upload_data[100];
	float temp;
	uint8_t humi;
	
	HAL_Delay(1000);
	
  while (1)
  {
		// 周期性上报传感器数据和设备状态
		
		if(DHT11_Get(&temp, &humi) == 0)
		{
			// 采集温湿度成功
			sprintf(upload_data, "DHT11_1:%.1f_%u\n", temp, humi);
			
			HAL_UART_Transmit(&huart1, (uint8_t*)upload_data, strlen(upload_data), 1000);
		}

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET)
			sprintf(upload_data, "BEEP1:1\n");
		else
			sprintf(upload_data, "BEEP1:0\n");
		
		HAL_UART_Transmit(&huart1, (uint8_t*)upload_data, strlen(upload_data), 1000);
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
			sprintf(upload_data, "LED2:0\n");
		else
			sprintf(upload_data, "LED2:1\n");	
		
		HAL_UART_Transmit(&huart1, (uint8_t*)upload_data, strlen(upload_data), 1000);
		
		HAL_Delay(1500);
		
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void LED_Ctrl(uint16_t led_num, uint16_t cmd, uint16_t arg)
{
	if(led_num == 1)
	{
		if(arg > 1000) arg = 1000;
		
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, arg);
	}
	else if(led_num == 2)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, cmd == 1 ? GPIO_PIN_RESET : GPIO_PIN_SET);
	
	}
	else if(led_num == 3)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, cmd == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);			
	}
}


void Beep_Ctrl(uint16_t beep_num, uint16_t cmd, uint16_t arg)
{
	if(beep_num == 1)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, cmd == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}


void Servo_Ctrl(uint16_t servo_num, uint16_t cmd, uint16_t arg)
{
	if(servo_num == 1)
	{
		uint16_t pulse_width;
		
		if(arg > 180) arg = 180;
		
		pulse_width = (240 - 50) * arg / 180 + 50;
		
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, pulse_width);		
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
	if(huart == &huart1)
	{
		static uint8_t cmd_total[12] = "", recv_cnt = 0;
		uint16_t dev_type, dev_id, ctrl_cmd, cmd_arg;
		
		if(recv_cnt == 0)
		{
			if(cmd_pc == 0xA5) cmd_total[recv_cnt++] = 0xA5;
		}
		else if(recv_cnt == 1)
		{
			if(cmd_pc == 0x5A) cmd_total[recv_cnt++] = 0x5A;
			else recv_cnt = 0;
		}
		else if(2 <= recv_cnt && recv_cnt <= 9)
		{
			cmd_total[recv_cnt++] = cmd_pc;
		}
		else if(recv_cnt == 10)
		{
			if(cmd_pc == 0xC3) cmd_total[recv_cnt++] = 0xC3;
			else recv_cnt = 0;
		}
		else if(recv_cnt == 11)
		{
			if(cmd_pc == 0x3C) 
			{
				// 成功接收到一条完整的控制指令
				
				dev_type = ((uint16_t*)cmd_total)[1];
				dev_id = ((uint16_t*)cmd_total)[2];
				ctrl_cmd = ((uint16_t*)cmd_total)[3];
				cmd_arg = ((uint16_t*)cmd_total)[4];
				
				if(dev_type == 1)
				{
					// LED
					LED_Ctrl(dev_id, ctrl_cmd, cmd_arg);
				}
				else if(dev_type == 2)
				{
					// 蜂鸣器
					Beep_Ctrl(dev_id, ctrl_cmd, cmd_arg);
				}				
				else if(dev_type == 4)
				{
					// 舵机
					Servo_Ctrl(dev_id, ctrl_cmd, cmd_arg);
				}
			}
			
			recv_cnt = 0;
		}		
		
//		if(cmd_pc == 'a')
//		{
//			// 开灯
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
//		}
//		else if(cmd_pc == 'b')
//		{
//			// 关灯
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
//		}
//		else if(cmd_pc == 'c')
//		{
//			// 报警
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
//		}
//		else if(cmd_pc == 'd')
//		{
//			// 静音
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
//		}
		
		// 用中断的方式接收一个字节的数据
		HAL_UART_Receive_IT(&huart1, (uint8_t*)&cmd_pc, sizeof(cmd_pc));
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
