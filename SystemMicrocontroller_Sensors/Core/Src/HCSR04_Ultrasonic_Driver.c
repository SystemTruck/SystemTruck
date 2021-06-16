/*
 * HC-SR04_Ultrasonic_Driver.c
 *
 *  Created on: Jun 14, 2021
 *      Author: jdrqu
 */


#include "HCSR04_Ultrasonic_Driver.h"
#include "DWT_Delay.h"

#define LOW 0
#define HIGH 1

// Speed of sound in meters per second
#define SPEED_OF_SOUND 347

/* Measurements above this value will be disregarded.
It is recommended to discard measurements above 4 meters */
#define SENSOR_LIMIT_METERS 4

typedef struct
{
	uint8_t  LOGIC_STATE;
	uint16_t TMR_OVC;				// Number of times the timer overflows between T1 and T2
	uint32_t TMR_PSC;				// Prescaler used for timer configuration
	uint32_t TMR_ARR;				// Autoreaload value
	uint16_t TMR_Frequency_MHZ;
	uint32_t T1;					// Contains timer count value on rising edge, the moment sensor emits ultrasound
	uint32_t T2;					// Contains timer count value on falling edge, the moment sensor receives ultrasound
	uint32_t DIFF;
	float    DISTANCE;				// Distance in CM.
	TIM_HandleTypeDef* TIM_Handler;
	TIM_TypeDef* TIM_Instance;
	HAL_TIM_ActiveChannel ACTIV_CH;
	uint32_t TIM_IC_CH;				/* This parameter can be one of the following values:
	              	  	  	  	  	   @arg TIM_CHANNEL_1
	              	  	  	  	  	   @arg TIM_CHANNEL_2
	              	  	  	  	  	   @arg TIM_CHANNEL_3
	              	  	  	  	  	   @arg TIM_CHANNEL_4 */
}HCSR04_Status;

HCSR04_Status HCSR04_Devices[NUMBER_OF_HCSR04_DEVICES];

/**
  * @brief  Initializes the TRIG GPIO and associated TIMER as well as the corresponding structure in @HCSR04_Devices
  * @param  HCSR04_Handle - Structure used to configure the sensor. Refer to HCSR04_GetDefaultConfig()
  * @param  TIM_Handler - Handle to timer to be used. Must have Input-Capture capabilities.
  */
void HCSR04_Init(HCSR04_Handler* HCSR04_Handle, TIM_HandleTypeDef* TIM_Handler){
	GPIO_InitTypeDef TRIG_GPIO_InitStruct = {0};
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_IC_InitTypeDef sConfigIC = {0};

	DWT_Delay_Init();

	/* ----------- Configure The HCSR04 TRIGGER GPIO Pin ----------- */

	if(GPIOA == HCSR04_Handle->TRIG_GPIO)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
	else if(GPIOB == HCSR04_Handle->TRIG_GPIO)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	else if(GPIOC == HCSR04_Handle->TRIG_GPIO)
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}
	else if(GPIOD == HCSR04_Handle->TRIG_GPIO)
	{
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
	else if(GPIOE == HCSR04_Handle->TRIG_GPIO)
	{
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}

	TRIG_GPIO_InitStruct.Pin = HCSR04_Handle->TRIG_PIN;
	TRIG_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	TRIG_GPIO_InitStruct.Pull = GPIO_NOPULL;
	TRIG_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(HCSR04_Handle->TRIG_GPIO, &TRIG_GPIO_InitStruct);

	/* ----------- Initialize the HCSR04_Status structure ----------- */
	HCSR04_Devices[HCSR04_Handle->ID].TMR_PSC = HCSR04_Handle->TIM_Prescaler;
	HCSR04_Devices[HCSR04_Handle->ID].TMR_ARR = HCSR04_Handle->TIM_Autoreload;
	HCSR04_Devices[HCSR04_Handle->ID].TMR_OVC = 0;
	HCSR04_Devices[HCSR04_Handle->ID].TMR_Frequency_MHZ = HCSR04_Handle->TIM_ClockFrequency_MHZ;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler = TIM_Handler;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Instance = HCSR04_Handle->TIM_Instance;
	HCSR04_Devices[HCSR04_Handle->ID].LOGIC_STATE = LOW;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_IC_CH = HCSR04_Handle->TIM_IC_CH;


	if(HCSR04_Handle->TIM_IC_CH == TIM_CHANNEL_1)
	{
		HCSR04_Devices[HCSR04_Handle->ID].ACTIV_CH = HAL_TIM_ACTIVE_CHANNEL_1;
	}
	else if(HCSR04_Handle->TIM_IC_CH == TIM_CHANNEL_2)
	{
		HCSR04_Devices[HCSR04_Handle->ID].ACTIV_CH = HAL_TIM_ACTIVE_CHANNEL_2;
	}
	else if(HCSR04_Handle->TIM_IC_CH == TIM_CHANNEL_3)
	{
		HCSR04_Devices[HCSR04_Handle->ID].ACTIV_CH = HAL_TIM_ACTIVE_CHANNEL_3;
	}
	else if(HCSR04_Handle->TIM_IC_CH == TIM_CHANNEL_4)
	{
		HCSR04_Devices[HCSR04_Handle->ID].ACTIV_CH = HAL_TIM_ACTIVE_CHANNEL_4;
	}

	/* ----------- Configure the Input-Capture timer ----------- */
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Instance = HCSR04_Handle->TIM_Instance;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Init.Prescaler = HCSR04_Devices[HCSR04_Handle->ID].TMR_PSC;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Init.Period = HCSR04_Devices[HCSR04_Handle->ID].TMR_ARR;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Init.CounterMode = TIM_COUNTERMODE_UP;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_Base_Init(HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_OK != HAL_TIM_ConfigClockSource(HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler, &sClockSourceConfig)){
		Error_Handler();
	}

	if (HAL_OK != HAL_TIM_IC_Init(HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler)){
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_OK != HAL_TIMEx_MasterConfigSynchronization(HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler, &sMasterConfig)){
		Error_Handler();
	}

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_OK != HAL_TIM_IC_ConfigChannel(HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler, &sConfigIC, HCSR04_Handle->TIM_IC_CH)){
		Error_Handler();
	}
	HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler->Instance->DIER |= 1;
	if (HAL_OK != HAL_TIM_IC_Start_IT(HCSR04_Devices[HCSR04_Handle->ID].TIM_Handler, HCSR04_Handle->TIM_IC_CH)){
		Error_Handler();
	}

}

void HCSR04_GetDefaultConfig(HCSR04_Handler* HCSR04_Handle){
	HCSR04_Handle->ID = 0;
	HCSR04_Handle->TIM_Autoreload = 5000000;
	HCSR04_Handle->TIM_Prescaler = 9;
	HCSR04_Handle->TIM_ClockFrequency_MHZ = 32;
	HCSR04_Handle->TIM_IC_CH = TIM_CHANNEL_1;
	HCSR04_Handle->TIM_Instance = TIM2;
	HCSR04_Handle->TRIG_GPIO = GPIOC;
	HCSR04_Handle->TRIG_PIN = GPIO_PIN_11;
}


void HCSR04_TMR_OVERFLOW_ISR(TIM_HandleTypeDef* htim){
	for(int i = 0; i < NUMBER_OF_HCSR04_DEVICES; i++)
	{
		if(htim->Instance == HCSR04_Devices[i].TIM_Instance)
		{
			HCSR04_Devices[i].TMR_OVC++;
		}
	}
}


void HCSR04_TMR_IC_ISR(TIM_HandleTypeDef* htim){
	uint32_t PS = 0;
	for(int i = 0; i < NUMBER_OF_HCSR04_DEVICES; i++)
	{
		if((htim->Instance == HCSR04_Devices[i].TIM_Instance) && (htim->Channel == HCSR04_Devices[i].ACTIV_CH))
		{
			if (HCSR04_Devices[i].LOGIC_STATE == LOW)
			{
				// Capture T1 & Reverse The ICU Edge Polarity
				HCSR04_Devices[i].T1 = HAL_TIM_ReadCapturedValue(htim, HCSR04_Devices[i].TIM_IC_CH);
				HCSR04_Devices[i].LOGIC_STATE = HIGH;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, HCSR04_Devices[i].TIM_IC_CH, TIM_INPUTCHANNELPOLARITY_FALLING);
				HCSR04_Devices[i].TMR_OVC = 0;
			}
			else if (HCSR04_Devices[i].LOGIC_STATE == HIGH)
			{
				// Read The Current Prescaler Value For The Timer
				PS = HCSR04_Devices[i].TIM_Instance->PSC;
				// Capture T2 & Calculate The Distance
				HCSR04_Devices[i].T2 = HAL_TIM_ReadCapturedValue(htim, HCSR04_Devices[i].TIM_IC_CH);
				HCSR04_Devices[i].T2 += (HCSR04_Devices[i].TMR_OVC * (HCSR04_Devices[i].TMR_ARR+1));
				if(HCSR04_Devices[i].T2 > HCSR04_Devices[i].T1){
					HCSR04_Devices[i].DIFF = HCSR04_Devices[i].T2 - HCSR04_Devices[i].T1;
				} else {
					HCSR04_Devices[i].DIFF = HCSR04_Devices[i].T1 - HCSR04_Devices[i].T2;
				}

				// Write The Distance Value To The Global Struct & Reverse The ICU Edge

				/* Regarding the following if statement:
				 * Left side:  Time in us it would take for sound to travel SENSOR_LIMIT_METERS
				 * Right side: Time in us detected by sensor
				 */

				if((SENSOR_LIMIT_METERS*1000000)/SPEED_OF_SOUND < HCSR04_Devices[i].DIFF/(HCSR04_Devices[i].TMR_Frequency_MHZ/(PS+1))){
					HCSR04_Devices[i].DISTANCE = 0; //reading is actually due to timeout
				} else {
					HCSR04_Devices[i].DISTANCE = (HCSR04_Devices[i].DIFF * 0.017)/(HCSR04_Devices[i].TMR_Frequency_MHZ/(PS+1));
				}
				HCSR04_Devices[i].LOGIC_STATE = LOW;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, HCSR04_Devices[i].TIM_IC_CH, TIM_INPUTCHANNELPOLARITY_RISING);
			}
		}
	}
}

//This function will return 0 if reading is invalid
float HCSR04_GetDistance(HCSR04_Handler* HCSR04_Handle){
	return HCSR04_Devices[HCSR04_Handle->ID].DISTANCE;
}

void HCSR04_Trigger(HCSR04_Handler* HCSR04_Handle){
	HAL_GPIO_WritePin(HCSR04_Handle->TRIG_GPIO, HCSR04_Handle->TRIG_PIN, HIGH);
	DWT_Delay_us(2);
	HAL_GPIO_WritePin(HCSR04_Handle->TRIG_GPIO, HCSR04_Handle->TRIG_PIN, LOW);
}

