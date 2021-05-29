/*
 * L298N_Motor_driver.h
 *
 *  Created on: May 23, 2021
 *      Author: Olaf
 */

#ifndef INC_L298N_MOTOR_DRIVER_H_
#define INC_L298N_MOTOR_DRIVER_H_

#include "main.h"

typedef enum
{
  L298_OK       = 0x00U,
  L298_ERROR    = 0x01U,
} L298N_STD_TYPE;

typedef TIM_HandleTypeDef PWM_TIM_HandleTypeDef;

typedef struct {
	uint32_t Channel;
	uint32_t PWMPin;
	GPIO_TypeDef *PWM_Pin_Port;
	TIM_OC_InitTypeDef PWM_Handle;
} L298N_Channel_Typedef;

typedef struct {
	PWM_TIM_HandleTypeDef PWM_TIMER;
	L298N_Channel_Typedef ChannelInfo;
	uint16_t DirectionPin_1;
	GPIO_TypeDef *DirectionPin_1_Port;
	uint16_t DirectionPin_2;
	GPIO_TypeDef *DirectionPin_2_Port;
	uint16_t Frequency_PWM;
} L298N_HandleTypeDef;


void L298N_SET_TIMER_Period(L298N_HandleTypeDef * L298N, uint32_t Frequency_In_Hz);
void L298N_Get_Default_Config(L298N_HandleTypeDef * L298N);
L298N_STD_TYPE L298N_driver_init(L298N_HandleTypeDef * L298N);



#endif /* INC_L298N_MOTOR_DRIVER_H_ */
