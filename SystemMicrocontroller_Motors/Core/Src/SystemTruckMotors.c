/*
 * SystemTruckMotors.c
 *
 *  Created on: Jun 4, 2021
 *      Author: Olaf
 */
#include "main.h"
#include "L298N_Motor_driver.h"

L298N_HandleTypeDef TestHandle;

void L298N_Test(void);

int SystemTruckMotors_main(void){

	L298N_Test();

	while(1u){
		__NOP();
	};
	return 0;


}

void L298N_Test(void){
	L298N_Get_Default_Config(&TestHandle);
	L298N_driver_init(&TestHandle);
	L298N_start_pwm(&TestHandle,75);
	HAL_Delay(1000);
	L298N_Set_Duty_Cycle(&TestHandle,50);
	HAL_Delay(1000);
	L298N_Set_Duty_Cycle(&TestHandle,30);
	HAL_Delay(1000);
	L298N_Set_Duty_Cycle(&TestHandle,80);
}




