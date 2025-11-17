#include "main.h"
#include "tim.h"
#include "delay.h"



// 微秒级延时函数，参数 t 的范围为 [0, 65535]
void Delay_US(uint16_t t)
{
	__HAL_TIM_SetCounter(&htim1, 0);
	
	HAL_TIM_Base_Start(&htim1);
	
	while(__HAL_TIM_GetCounter(&htim1) < t);
	
	HAL_TIM_Base_Stop(&htim1);
}
