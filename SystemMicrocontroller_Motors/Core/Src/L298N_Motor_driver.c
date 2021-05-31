/*
 * L298N_Motor_driver.c
 *
 *  Created on: May 23, 2021
 *      Author: Olaf Huerta
 */
/* Includes ------------------------------------------------------------------*/
#include "L298N_Motor_driver.h"

/* Defines -------------------------------------------------------------------*/
#define isCurrentGPIOUsedL298N			(L298N->DirectionPin_1_Port == CurrentGPIO) || \
		(L298N->DirectionPin_2_Port == CurrentGPIO) || \
		(L298N->ChannelInfo.PWM_Pin_Port == CurrentGPIO)
#define Recommended_PWM_Frequency        (1000u) /**<Recommended Frequency for signal in a motor */
#define InitialPulseValue                (0u)
#define IDLE							 (0u)
#define FORWARD							 (1u)
#define REVERSE							 (2u)
/* Static Functions Prototypes ------------------------------------------------*/
static void L298N_SET_TIMER_Period(L298N_HandleTypeDef * L298N, uint32_t Frequency_In_Hz);
static void L298N_Enable_GPIO_RCC(L298N_HandleTypeDef * L298N);
static L298N_STD_TYPE ValidateTIMInstance(TIM_HandleTypeDef *TIMPWM, uint32_t Channel);
static void L298N_Enable_TIM_RCC(L298N_HandleTypeDef * L298N);

/**
 * function: L298N_driver_init
 *
 * This function initializates the Peripherals involved in the management of the
 * L298N. This needs the L298N HandleTypeDef with the configuration.
 *
 * @param L298N
 * @return L298_OK - Initialization Successful
 * 		   L298_ERROR - ERROR in the initialization
 */
L298N_STD_TYPE L298N_driver_init(L298N_HandleTypeDef * L298N){
	GPIO_InitTypeDef GPIO_InitStruct;
	if(L298_OK != ValidateTIMInstance(&L298N->PWM_TIMER, L298N->ChannelInfo.Channel)){
		return L298_ERROR;
	}
	L298N_Enable_GPIO_RCC(L298N);
	L298N_SET_TIMER_Period(L298N, L298N->Frequency_PWM);
	L298N_Enable_TIM_RCC(L298N);

	if (HAL_TIM_PWM_Init(&(L298N->PWM_TIMER)) != HAL_OK)
	{
		return L298_ERROR;
	}
	if (HAL_TIM_PWM_ConfigChannel(&(L298N->PWM_TIMER), &(L298N->ChannelInfo.PWM_Handle), L298N->ChannelInfo.Channel) != HAL_OK)
	{
		return L298_ERROR;
	}

	L298N->ChannelInfo.PWM_GPIO_Init.Pin = L298N->ChannelInfo.PWMPin;
	L298N->ChannelInfo.PWM_GPIO_Init.Mode = GPIO_MODE_AF_PP;
	L298N->ChannelInfo.PWM_GPIO_Init.Pull = GPIO_NOPULL;
	L298N->ChannelInfo.PWM_GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init((L298N->ChannelInfo.PWM_Pin_Port), &(L298N->ChannelInfo.PWM_GPIO_Init));

	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = L298N->DirectionPin_1;

	HAL_GPIO_Init(L298N->DirectionPin_1_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, RESET);

	GPIO_InitStruct.Pin = L298N->DirectionPin_2;

	HAL_GPIO_Init(L298N->DirectionPin_1_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, RESET);

	return L298_OK;
}

/**
 * function: L298N_start_pwm
 *
 * @param L298N
 * @param dutycycle
 * @return L298_ERROR
 * 		   L298_OK
 */
L298N_STD_TYPE L298N_start_pwm(L298N_HandleTypeDef * L298N,uint8_t dutycycle){
	L298N_Set_Duty_Cycle(L298N, dutycycle);
	if (HAL_TIM_PWM_Start(&(L298N->PWM_TIMER),L298N->ChannelInfo.Channel)){
		return L298_ERROR;
	}
	return L298_OK;
}

void L298N_Set_Duty_Cycle(L298N_HandleTypeDef * L298N,uint8_t dutycycle){
	uint32_t pulse;
	pulse = L298N->PWM_TIMER.Init.Period - ( (L298N->PWM_TIMER.Init.Period * dutycycle) /100u);
	__HAL_TIM_SET_COMPARE(&(L298N->PWM_TIMER),L298N->ChannelInfo.Channel,pulse);
}

void L298N_Get_Default_Config(L298N_HandleTypeDef * L298N){
	L298N->PWM_TIMER.Instance = TIM3;
	L298N->PWM_TIMER.Init.Prescaler = 99; /**< Usually a value x99 */
	L298N->PWM_TIMER.Init.CounterMode = TIM_COUNTERMODE_UP;
	L298N->PWM_TIMER.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	L298N->PWM_TIMER.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	L298N->Frequency_PWM = Recommended_PWM_Frequency;
	L298N->ChannelInfo.PWM_Handle.OCMode= TIM_OCMODE_PWM2;
	L298N->ChannelInfo.PWM_Handle.Pulse = InitialPulseValue;
	L298N->ChannelInfo.PWM_Handle.OCPolarity = TIM_OCPOLARITY_HIGH;
	L298N->ChannelInfo.PWM_Handle.OCFastMode = TIM_OCFAST_ENABLE;
	L298N->ChannelInfo.Channel= TIM_CHANNEL_1;
	L298N->ChannelInfo.PWMPin = GPIO_PIN_6;
	L298N->ChannelInfo.PWM_Pin_Port = GPIOA;
	L298N->ChannelInfo.PWM_GPIO_Init.Alternate = GPIO_AF2_TIM3;
	L298N->DirectionPin_1 = GPIO_PIN_5;
	L298N->DirectionPin_2 = GPIO_PIN_4;
	L298N->DirectionPin_1_Port = GPIOA;
	L298N->DirectionPin_2_Port = GPIOA;
}

void L298N_Set_Direction(L298N_HandleTypeDef *L298N, uint32_t Direction){
	HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, RESET);
	HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, RESET);

	if ( FORWARD == Direction) {
		HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, SET);
		HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, RESET);
	}else if (REVERSE == Direction){
		HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, RESET);
		HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, SET);
	} else {
		/**<Do nothing */
	}
}


static void L298N_SET_TIMER_Period(L298N_HandleTypeDef * L298N, uint32_t Frequency_In_Hz){
	uint32_t TimerClock;
	uint32_t Period;
	TimerClock = SystemCoreClock/((L298N->PWM_TIMER.Init.Prescaler)+1);
	Period = (TimerClock/Frequency_In_Hz)-1;
	L298N->PWM_TIMER.Init.Period = Period;
}

static void L298N_Enable_TIM_RCC(L298N_HandleTypeDef * L298N) {
	if  (TIM1 == L298N->PWM_TIMER.Instance ) {
		__HAL_RCC_TIM1_CLK_ENABLE();
	}else if(TIM2 == (L298N->PWM_TIMER.Instance) ){
		__HAL_RCC_TIM2_CLK_ENABLE();
	}else if(TIM3 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM3_CLK_ENABLE();
	}else if(TIM4 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM4_CLK_ENABLE();
	}else if(TIM5 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM5_CLK_ENABLE();
	}else if(TIM6 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM6_CLK_ENABLE();
	}else if(TIM7 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM7_CLK_ENABLE();
	}else if(TIM8 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM8_CLK_ENABLE();
	}else if(TIM9 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM9_CLK_ENABLE();
	}else if(TIM10 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM10_CLK_ENABLE();
	}else if(TIM11 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM11_CLK_ENABLE();
	}else if(TIM12 == L298N->PWM_TIMER.Instance ){
		__HAL_RCC_TIM12_CLK_ENABLE();
	}

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
