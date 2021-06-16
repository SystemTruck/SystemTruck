/*
 * SystemTruckSensors.c
 *
 *  Created on: Jun 15, 2021
 *      Author: jdrqu
 */

#include "stdio.h"
#include "HCSR04_Ultrasonic_Driver.h"
#include "lcd20x4.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;
LCD_HandleTypeDef LCD_handle;

void Test_LCD();
void Test_HCSR04();

int SystemTruckSensors_Main(){


	Test_LCD();
	Test_HCSR04();

	while(1){

	}
	return 0;
}

void Test_LCD(){
	LCD_GetDefaultConfig(&LCD_handle);
	LCD_Init(&LCD_handle);
	LCD_Clear(&LCD_handle);
	LCD_SetCursorMode(&LCD_handle, LCD_CURSOR_OFF);
	LCD_PrintToLine(&LCD_handle, LCD_LINE_1, "Testing HC-SR04");
}

void Test_HCSR04(){
	HCSR04_Handler HCSR04;
	char TEXT[20] = {0};
	float Distance = 0.0;

	HCSR04_GetDefaultConfig(&HCSR04);
	HCSR04_Init(&HCSR04, &htim2);

	while(1){
		HCSR04_Trigger(&HCSR04);
		Distance = HCSR04_GetDistance(&HCSR04);
		if(Distance != 0) {
			sprintf(TEXT, "Dist = %.2f cm", Distance);
			HAL_UART_Transmit(&huart2, TEXT, 20, 1000);
			LCD_PrintToLine(&LCD_handle, LCD_LINE_3, TEXT);
		}
		HAL_Delay(100);
	}
}


