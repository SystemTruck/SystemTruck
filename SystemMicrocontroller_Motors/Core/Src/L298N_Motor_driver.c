/*
 * L298N_Motor_driver.c
 *
 *  Created on: May 23, 2021
 *      Author: Olaf
 */

#include "L298N_Motor_driver.h"

#define isCurrentGPIOUsedL298N			(L298N->DirectionPin_1_Port == CurrentGPIO) || \
										(L298N->DirectionPin_2_Port == CurrentGPIO) || \
										(L298N->ChannelInfo.PWM_Pin_Port == CurrentGPIO)
#define Recommended_PWM_Frequency        (500u)
#define InitialPulseValue                (800u)

static void L298N_Enable_GPIO_RCC(L298N_HandleTypeDef * L298N);
static L298N_STD_TYPE Validate_PWM_Structure(L298N_HandleTypeDef * L298N);
static L298N_STD_TYPE ValidateTIMInstance(TIM_HandleTypeDef *TIMPWM, uint32_t Channel);

L298N_STD_TYPE L298N_driver_init(L298N_HandleTypeDef * L298N){
	GPIO_InitTypeDef GPIO_InitStruct;
	if(L298_OK != Validate_PWM_Structure(L298N)){
		return L298_ERROR;
	}
	L298N_Enable_GPIO_RCC(L298N);
	L298N_SET_TIMER_Period(L298N, L298N->Frequency_PWM);
	__HAL_RCC_TIM3_CLK_ENABLE(); /** Checar reloj del TIM */



	  if (HAL_TIM_PWM_Init(&(L298N->PWM_TIMER)) != HAL_OK)
	  {
		  return L298_ERROR;
	  }
	  if (HAL_TIM_PWM_ConfigChannel(&(L298N->PWM_TIMER), &(L298N->ChannelInfo.PWM_Handle), L298N->ChannelInfo.Channel) != HAL_OK)
	  {
		  return L298_ERROR;
	  }

	    GPIO_InitStruct.Pin = L298N->ChannelInfo.PWMPin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3; /** Checar AF del TIM */
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = L298N->DirectionPin_1;
	//HAL_GPIO_Init(L298N->DirectionPin_1_Port, &GPIO_InitStruct);
	//HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, RESET);
	GPIO_InitStruct.Pin = L298N->DirectionPin_2;
	//HAL_GPIO_Init(L298N->DirectionPin_1_Port, &GPIO_InitStruct);
	//HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, RESET);

	  HAL_TIM_PWM_Start(&(L298N->PWM_TIMER),L298N->ChannelInfo.Channel);

	return L298_OK;



}

void L298N_Get_Default_Config(L298N_HandleTypeDef * L298N){
	L298N->PWM_TIMER.Instance = TIM3;
	L298N->PWM_TIMER.Init.Prescaler = 99; /**< Usually a value x99 */
	L298N->PWM_TIMER.Init.CounterMode = TIM_COUNTERMODE_UP;
	L298N->PWM_TIMER.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	L298N->PWM_TIMER.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	L298N->Frequency_PWM = Recommended_PWM_Frequency;
	L298N->ChannelInfo.PWM_Handle.OCMode= TIM_OCMODE_PWM2;
	L298N->ChannelInfo.PWM_Handle.Pulse = InitialPulseValue; /** HAcer fx para duty cycle*/
	L298N->ChannelInfo.PWM_Handle.OCPolarity = TIM_OCPOLARITY_HIGH;
	L298N->ChannelInfo.PWM_Handle.OCFastMode = TIM_OCFAST_ENABLE;
	L298N->ChannelInfo.Channel= TIM_CHANNEL_1;
	L298N->ChannelInfo.PWMPin = GPIO_PIN_6;
	L298N->ChannelInfo.PWM_Pin_Port = GPIOA;
}


void L298N_SET_TIMER_Period(L298N_HandleTypeDef * L298N, uint32_t Frequency_In_Hz){
	uint32_t TimerClock;
	uint32_t Period;
	TimerClock = SystemCoreClock/((L298N->PWM_TIMER.Init.Prescaler)+1);
	Period = (TimerClock/Frequency_In_Hz)-1;
	L298N->PWM_TIMER.Init.Period = Period;
}

static void L298N_Enable_GPIO_RCC(L298N_HandleTypeDef * L298N) {
	GPIO_TypeDef *CurrentGPIO;
	CurrentGPIO = GPIOA;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
	CurrentGPIO = GPIOB;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	CurrentGPIO = GPIOC;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}
	CurrentGPIO = GPIOD;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
	CurrentGPIO = GPIOE;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}
	CurrentGPIO = GPIOF;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOF_CLK_ENABLE();
	}
	CurrentGPIO = GPIOG;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOG_CLK_ENABLE();
	}
	CurrentGPIO = GPIOH;
	if(isCurrentGPIOUsedL298N){
		__HAL_RCC_GPIOH_CLK_ENABLE();
	}

}

static L298N_STD_TYPE Validate_PWM_Structure(L298N_HandleTypeDef * L298N){
uint32_t return_val;
	return_val = ValidateTIMInstance(&L298N->PWM_TIMER, L298N->ChannelInfo.Channel);
	return return_val;
}


static L298N_STD_TYPE ValidateTIMInstance(PWM_TIM_HandleTypeDef *TIMPWM, uint32_t Channel){

		if (0u == IS_TIM_INSTANCE(TIMPWM->Instance)){
			return L298_ERROR;
		}
		if (0u == IS_TIM_CC1_INSTANCE(TIMPWM->Instance)){
				return L298_ERROR;
			}
		if (0u == IS_TIM_CCX_INSTANCE(TIMPWM->Instance,Channel)){
				return L298_ERROR;
			}
	return L298_OK;
}
