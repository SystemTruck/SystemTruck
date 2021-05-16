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


/************************************** Command register **************************************/
#define COMMAND_CLEAR_DISPLAY 0x01

#define COMMAND_RETURN_HOME 0x02

#define COMMAND_ENTRY_MODE_SET 0x04
#define OPT_S	0x01					// Shift entire display to right
#define OPT_INC 0x02					// Cursor increment

#define COMMAND_DISPLAY_ON_OFF_CONTROL 0x08
#define OPT_D	0x04					// Turn on display
#define OPT_C	0x02					// Turn on cursor
#define OPT_B 	0x01					// Turn on cursor blink

#define COMMAND_CURSOR_DISPLAY_SHIFT 0x10		// Move and shift cursor
#define OPT_SC 0x08
#define OPT_RL 0x04

#define COMMAND_FUNCTION_SET 0x20
#define OPT_DL 0x10						// Set interface data length
#define OPT_N 0x08						// Set number of display lines
#define OPT_F 0x04						// Set alternate font

#define COMMAND_SETCGRAM_ADDR 0x040

#define COMMAND_SET_DDRAM_ADDR 0x80				// Set DDRAM address

#define isCurrentGPIOUsed	(lcd->DB0_port == CurrentGPIO) || (lcd->DB1_port == CurrentGPIO) || \
							(lcd->DB2_port == CurrentGPIO) || (lcd->DB3_port == CurrentGPIO) || (lcd->DB4_port == CurrentGPIO) || \
							(lcd->DB5_port == CurrentGPIO) || (lcd->DB6_port == CurrentGPIO) || (lcd->DB7_port == CurrentGPIO) || \
							(lcd->EN_port == CurrentGPIO) || (lcd->RS_port == CurrentGPIO) || (lcd->RW_port == CurrentGPIO)

#define DELAY(X) HAL_Delay(X)

#define LCD_RS_DATA 1
#define LCD_RS_COMMAND 0

#define LCD_RW_READ 1
#define LCD_RW_WRITE 0

typedef struct {
	uint8_t Mode;			/*!< Specifies whether to use 4 bit or 8 bit mode.
                           This parameter can be any value of @ref LCD_Mode_Define */
	uint8_t CursorMode;     /*!< Specifies cursor on/off and blink on/off.
                           This parameter can be any value of @ref LCD_CursorMode_Define */

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

typedef enum {
	LCD_OK = 0,
	LCD_ERROR = 1
} LCD_StatusTypeDef;

/** @defgroup LCD_Mode_Define
 * @{
 */
#define LCD_MODE_4BITS 0
#define LCD_MODE_8BITS 1

/** @defgroup LCD_CursorMode_Define
 * @{
 */
#define LCD_CURSOR_OFF 0
#define LCD_CURSOR_ON_BLINK_OFF 1
#define LCD_CURSOR_ON_BLINK_ON 2


void LCD_Init(LCD_HandleTypeDef * lcd);
void LCD_PinInit(LCD_HandleTypeDef * lcd);
void LCD_Enable_GPIO_RCC(LCD_HandleTypeDef * lcd);


void LCD_Printf(LCD_HandleTypeDef * lcd, const char* str, ...);
void LCD_PrintTo1stLine(LCD_HandleTypeDef * lcd, const char* str, ...);
void LCD_PrintTo2ndLine(LCD_HandleTypeDef * lcd, const char* str, ...);
void LCD_PrintTo3rdLine(LCD_HandleTypeDef * lcd, const char* str, ...);
void LCD_PrintTo4thLine(LCD_HandleTypeDef * lcd, const char* str, ...);

void LCD_Clear(LCD_HandleTypeDef * lcd);
void LCD_SetCursorPosition(LCD_HandleTypeDef * lcd, uint8_t row, uint8_t col);
void LCD_SetCursorMode(LCD_HandleTypeDef * lcd, uint8_t CursorMode);

void LCD_WriteCommand(LCD_HandleTypeDef * lcd, uint8_t command);
void LCD_WriteData(LCD_HandleTypeDef * lcd, uint8_t data);
void LCD_Write(LCD_HandleTypeDef *lcd, uint8_t data);



#endif /* INC_LCD20X4_H_ */
