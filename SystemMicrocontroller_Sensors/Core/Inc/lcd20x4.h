/*
 * lcd20x4.h
 *
 *  Created on: May 15, 2021
 *      Author: jdrqu
 */

#ifndef INC_LCD20X4_H_
#define INC_LCD20X4_H_

#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


typedef enum {
	LCD_OK = 0,
	LCD_ERROR = 1
} LCD_StatusTypeDef;

typedef enum {
	LCD_MODE_4BITS  = 0,
	LCD_MODE_8BITS = 1
} LCD_Mode_TypeDef;

typedef enum{
	LCD_LINE_1 = 0,
	LCD_LINE_2 = 1,
	LCD_LINE_3 = 2,
	LCD_LINE_4 = 3
} LCD_LineSelect_TypeDef;

typedef enum{
	LCD_CURSOR_OFF = 0,
	LCD_CURSOR_ON_BLINK_OFF = 1,
	LCD_CURSOR_ON_BLINK_ON = 2
} LCD_CursorMode_TypeDef;


typedef struct {
	LCD_Mode_TypeDef Mode;			//Specifies whether to use 4 bit or 8 bit mode.
	LCD_CursorMode_TypeDef CursorMode;     //Specifies cursor on/off and blink on/off.

	GPIO_TypeDef *DB0_port;
	uint16_t DB0_pin;
	GPIO_TypeDef *DB1_port;
	uint16_t DB1_pin;
	GPIO_TypeDef *DB2_port;
	uint16_t DB2_pin;
	GPIO_TypeDef *DB3_port;
	uint16_t DB3_pin;
	GPIO_TypeDef *DB4_port;
	uint16_t DB4_pin;
	GPIO_TypeDef *DB5_port;
	uint16_t DB5_pin;
	GPIO_TypeDef *DB6_port;
	uint16_t DB6_pin;
	GPIO_TypeDef *DB7_port;
	uint16_t DB7_pin;

	GPIO_TypeDef *RS_port;
	uint16_t RS_pin;

	GPIO_TypeDef *EN_port;
	uint16_t EN_pin;

	GPIO_TypeDef *RW_port;
	uint16_t RW_pin;

} LCD_HandleTypeDef;




/**
 * @brief  Initializes the GPIOx peripherals in the LCD_Init structure, then sends start sequence to LCD.
 * @param  lcd - LCD_HandleTypeDef structure specifies GPIO pins and LCD mode
 */
void LCD_Init(LCD_HandleTypeDef * lcd);
void LCD_PinInit(LCD_HandleTypeDef * lcd);
void LCD_Enable_GPIO_RCC(LCD_HandleTypeDef * lcd);

/**
 * @brief  //Prints to current cursor position
 * @param  lcd - LCD_HandleTypeDef structure specifies GPIO pins
 * @param  str - compatible with printf style formatting
 */
void LCD_Printf(LCD_HandleTypeDef * lcd, const char* str, ...);

/**
 * @brief  prints message to a specific line
 * @param  lcd - pointer to LCD handler
 * @param  line - This parameter can be any value of @ref LCD_Line_Define
 * @param  str - compatible with printf style formatting
 */
void LCD_PrintToLine(LCD_HandleTypeDef * lcd, LCD_LineSelect_TypeDef line, const char* str, ...);


void LCD_Clear(LCD_HandleTypeDef * lcd);
void LCD_SetCursorPosition(LCD_HandleTypeDef * lcd, uint8_t row, uint8_t col);
void LCD_SetCursorMode(LCD_HandleTypeDef * lcd, LCD_CursorMode_TypeDef CursorMode);





#endif /* INC_LCD20X4_H_ */
