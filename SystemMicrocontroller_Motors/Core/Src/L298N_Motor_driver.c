/*
 * L298N_Motor_driver.c
 *
 *  Created on: May 23, 2021
 *      Author: Olaf Huerta
 */
/* Includes ------------------------------------------------------------------*/
#include "L298N_Motor_driver.h"

/* Defines -------------------------------------------------------------------*/

#define isCurrentGPIOUsedL298N		(L298N->DirectionPin_1_Port == CurrentGPIO) || (L298N->DirectionPin_2_Port == CurrentGPIO) || \
		(L298N->ChannelInfo.PWM_Pin_Port == CurrentGPIO)

#define Recommended_PWM_Frequency   (500u) /**<Recommended Frequency for signal in a motor */
#define InitialPulseValue           (0u)


/* Static Functions Prototypes ------------------------------------------------*/

static void L298N_SET_TIMER_Period(L298N_HandleTypeDef * L298N, uint32_t Frequency_In_Hz);
static void L298N_Enable_GPIO_RCC(L298N_HandleTypeDef * L298N);
static L298N_STD_TYPE Validate_L298N_Parameters(L298N_HandleTypeDef * L298N);
static void L298N_Enable_TIM_RCC(L298N_HandleTypeDef * L298N);

/**
 * Function: L298N_driver_init
 *
 * @brief This function handles the initialization of the Peripherals involved in the management of the
 * L298N.
 *
 * @param L298N The L298N_HandleTypeDef handle
 * @return L298_OK - Initialization Successful
 * 		   L298_ERROR - ERROR in the initialization
 */
L298N_STD_TYPE L298N_driver_init(L298N_HandleTypeDef * L298N){
	GPIO_InitTypeDef GPIO_InitStruct;
	if(L298_OK != Validate_L298N_Parameters(L298N)){
		return L298_ERROR;
	}
	L298N_Enable_GPIO_RCC(L298N);
	L298N_SET_TIMER_Period(L298N, L298N->Frequency_PWM);
	L298N_Enable_TIM_RCC(L298N);
	/**<TIM timer configuration */
	L298N->ChannelInfo.PWM_Handle.OCMode= TIM_OCMODE_PWM1;
	L298N->PWM_TIMER.Init.CounterMode = TIM_COUNTERMODE_UP;
	L298N->PWM_TIMER.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	L298N->ChannelInfo.PWM_Handle.OCFastMode = TIM_OCFAST_DISABLE;
	L298N->ChannelInfo.PWM_Handle.OCPolarity = TIM_OCPOLARITY_HIGH;
	L298N->PWM_TIMER.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	/**<TIM Initialization */
	if (HAL_TIM_PWM_Init(&(L298N->PWM_TIMER)) != HAL_OK)
	{
		return L298_ERROR;
	}
	/**<PWM Channel Initialization */
	if (HAL_TIM_PWM_ConfigChannel(&(L298N->PWM_TIMER), &(L298N->ChannelInfo.PWM_Handle), L298N->ChannelInfo.Channel) != HAL_OK)
	{
		return L298_ERROR;
	}
	/**<PWM Pin initialization */
	L298N->ChannelInfo.PWM_GPIO_Init.Pin = L298N->ChannelInfo.PWMPin;
	L298N->ChannelInfo.PWM_GPIO_Init.Mode = GPIO_MODE_AF_PP;
	L298N->ChannelInfo.PWM_GPIO_Init.Pull = GPIO_NOPULL;
	L298N->ChannelInfo.PWM_GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init((L298N->ChannelInfo.PWM_Pin_Port), &(L298N->ChannelInfo.PWM_GPIO_Init));

	/**< Direction Pin Initialization*/
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
 * Function: L298N_start_pwm
 *
 * @brief This function start the PWM in the L298N Pin selected and configured
 *	at the desired Dutycycle.
 *
 * @param L298N - L298N - The L298N_HandleTypeDef handle to be started
 * @param dutycycle
 * @return L298_ERROR - If there was a error at the initialization of the PWM
 * 		   L298_OK    - If PWM successfully started
 */
L298N_STD_TYPE L298N_start_pwm(L298N_HandleTypeDef * L298N,uint8_t dutycycle){
	L298N_Set_Duty_Cycle(L298N, dutycycle);
	if (HAL_TIM_PWM_Start(&(L298N->PWM_TIMER),L298N->ChannelInfo.Channel)){
		return L298_ERROR;
	}
	return L298_OK;
}

/**
 * Function: L298N_Set_Duty_Cycle
 *
 * @brief This function changes the PWM dutycycle value.
 *
 * @param L298N - The L298N_HandleTypeDef handle to be set the duty cycle
 * @return L298_OK - Initialization Successful
 * 		   L298_ERROR - ERROR in the initialization
 */
void L298N_Set_Duty_Cycle(L298N_HandleTypeDef * L298N,uint8_t dutycycle){
	uint32_t pulse;
	pulse = ((L298N->PWM_TIMER.Init.Period * dutycycle) /100u);
	__HAL_TIM_SET_COMPARE(&(L298N->PWM_TIMER),L298N->ChannelInfo.Channel,pulse);
}

/**
 * Function: L298N_Get_Default_Config
 *
 * @brief This function get default configuration to drive a L298N driver
 *
 * @param L298N - The L298N_HandleTypeDef handle to be set the duty cycle
 * @return L298_OK - Initialization Successful
 * 		   L298_ERROR - ERROR in the initialization
 */
void L298N_Get_Default_Config(L298N_HandleTypeDef * L298N){
	L298N->PWM_TIMER.Instance = TIM3; /**< TIM Instance */
	L298N->PWM_TIMER.Init.Prescaler = 99; /**< Usually a value x99 */
	L298N->Frequency_PWM = Recommended_PWM_Frequency; /**< Recommended frequency for DC motors*/
	L298N->ChannelInfo.PWM_Handle.Pulse = InitialPulseValue;
	L298N->ChannelInfo.Channel= TIM_CHANNEL_1;
	L298N->ChannelInfo.PWMPin = GPIO_PIN_6;
	L298N->ChannelInfo.PWM_Pin_Port = GPIOA;
	L298N->ChannelInfo.PWM_GPIO_Init.Alternate = GPIO_AF2_TIM3;
	L298N->DirectionPin_1 = GPIO_PIN_5;
	L298N->DirectionPin_2 = GPIO_PIN_4;
	L298N->DirectionPin_1_Port = GPIOA;
	L298N->DirectionPin_2_Port = GPIOA;
}

/**
 * Function: L298N_Set_Direction
 * @brief Set direction FORWARD or REVERSE in the L298N device
 *
 * @param L298N L298N - The L298N_HandleTypeDef handle
 * @param Direction IDLE    = 0;
 * 					FORWARD = 1;
 * 					REVERSE = 2;
 */
L298N_STD_TYPE L298N_Set_Direction(L298N_HandleTypeDef *L298N, L298N_Direction Direction){
	L298N_STD_TYPE retval = L298_OK;
	if ( L298N_FORWARD == Direction) {
		HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, RESET);
		HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, SET);
	}else if (L298N_REVERSE == Direction){
		HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, RESET);
		HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, SET);
	} else if (L298N_BRAKE == Direction) {
		HAL_GPIO_WritePin(L298N->DirectionPin_1_Port, L298N->DirectionPin_1, RESET);
		HAL_GPIO_WritePin(L298N->DirectionPin_2_Port, L298N->DirectionPin_2, RESET);
	} else {
		retval = L298_ERROR;
	}
	return retval;
}

/**
 * Function: L298N_Set_Direction
 * @brief Set direction FORWARD or REVERSE in the L298N device
 *
 * @param L298N -  The L298N_HandleTypeDef handle
 * @param Frequency_In_Hz - Frequency desired
 */
static void L298N_SET_TIMER_Period(L298N_HandleTypeDef * L298N, uint32_t Frequency_In_Hz){
	uint32_t TimerClock;
	uint32_t Period;
	TimerClock = SystemCoreClock/((L298N->PWM_TIMER.Init.Prescaler)+1);
	Period = (TimerClock/Frequency_In_Hz)-1;
	L298N->PWM_TIMER.Init.Period = Period;
}

/**
 * Function:  L298N_Enable_TIM_RCC
 *
 * @brief This function enable TIM CLK in L298N initialization.
 *
 * @param L298N The L298N_HandleTypeDef that is going to be initialized
 */
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

/**
 * Function:  L298N_Enable_GPIO_RCC
 *
 * @brief This function enable GPIO CLK in L298N initialization.
 *
 * @param L298N The L298N_HandleTypeDef that is going to be initialized
 */
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

/**
 * Function:  Validate_L298N_Parameters
 *
 * @brief This function validates parameters in L298N initialization.
 *
 * @param L298N The L298N_HandleTypeDef structure to be validated
 * @return L298_OK - Validation Successful
 * 		   L298_ERROR - ERROR in the validation
 */
static L298N_STD_TYPE Validate_L298N_Parameters(L298N_HandleTypeDef *L298N){
	TIM_TypeDef* Instance;
	uint32_t Channel;
	Instance = (L298N->PWM_TIMER.Instance);
	Channel = L298N->ChannelInfo.Channel;
	if (0u == IS_TIM_INSTANCE(Instance)){
		return L298_ERROR;
	}
	if (0u == IS_TIM_CC1_INSTANCE(Instance)){
		return L298_ERROR;
	}
	if (0u == IS_TIM_CCX_INSTANCE(Instance,Channel)){
		return L298_ERROR;
	}
	return L298_OK;
}
