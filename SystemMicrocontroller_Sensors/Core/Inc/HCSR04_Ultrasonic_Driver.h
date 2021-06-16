/*
 * HC-SR04_Ultrasonic_Driver.h
 *
 *  Created on: Jun 14, 2021
 *      Author: jdrqu
 */

#ifndef INC_HCSR04_ULTRASONIC_DRIVER_H_
#define INC_HCSR04_ULTRASONIC_DRIVER_H_

#include "main.h"

// The Number of HC-SR04 Ultrasonic sensors used in the application
#define NUMBER_OF_HCSR04_DEVICES 1




typedef struct
{
	uint8_t						ID;					/* Must be a unique integer for each HCSR04 device.
													   First HCSR04 device must have ID = 0,
													   second one must have ID = 1, and so on */

	GPIO_TypeDef * 				TRIG_GPIO;
	uint16_t       				TRIG_PIN;
	TIM_TypeDef*   				TIM_Instance;		// Use a 32-bit timer with Input-Capture capabilities
	uint32_t       				TIM_IC_CH;
	uint32_t 					TIM_Autoreload; 	// TIM Autoreload
	uint16_t					TIM_Prescaler;		// TIM Prescaler
	uint16_t       				TIM_ClockFrequency_MHZ;
}HCSR04_Handler;

/**
  * @brief  Initializes the TRIG GPIO and associated TIMER as well as the corresponding structure in @HCSR04_Devices
  * @param  HCSR04_Handle - Structure used to configure the sensor. Refer to HCSR04_GetDefaultConfig()
  * @param  TIM_Handler - Handle to timer to be used. Must have Input-Capture capabilities.
  */
void HCSR04_Init(HCSR04_Handler* HCSR04_Handle, TIM_HandleTypeDef* TIM_Handler);
void HCSR04_GetDefaultConfig(HCSR04_Handler* HCSR04_Handle);

void HCSR04_TMR_OVERFLOW_ISR(TIM_HandleTypeDef* htim);
void HCSR04_TMR_IC_ISR(TIM_HandleTypeDef* htim);

float HCSR04_GetDistance(HCSR04_Handler* HCSR04_Handle);
void HCSR04_Trigger(HCSR04_Handler* HCSR04_Handle);

#endif /* INC_HCSR04_ULTRASONIC_DRIVER_H_ */
