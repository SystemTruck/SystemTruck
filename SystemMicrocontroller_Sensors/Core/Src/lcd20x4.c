/*
 * lcd20x4.c
 *
 *  Created on: May 15, 2021
 *      Author: jdrqu
 */

#include "lcd20x4.h"

const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54}; //address of the first position of each row for a 20x4 LCD


void LCD_Init(LCD_HandleTypeDef * lcd){

	LCD_PinInit(lcd);
	DELAY(5);
	if(lcd->Mode == LCD_MODE_4BITS)
	{
		HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, LCD_RS_COMMAND);		// Write to command register
		DELAY(50); //wait > 40 ms
		LCD_Write(lcd, 0x3);
		DELAY(5); //wait >4.1 ms
		LCD_Write(lcd, 0x3);
		DELAY(1); //wait >100 us
		LCD_Write(lcd, 0x3);
		DELAY(1);
		LCD_Write(lcd, 0x2);
		DELAY(1);
		LCD_WriteCommand(lcd, COMMAND_FUNCTION_SET | OPT_N); //2 lines, 4 bit data length
		LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL);
		LCD_WriteCommand(lcd, COMMAND_CLEAR_DISPLAY);
		LCD_WriteCommand(lcd, COMMAND_ENTRY_MODE_SET | OPT_INC); // Increment cursor
		LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B); //turn on display, cursor and cursor blink
	}
	else
	{
		DELAY(45); //wait > 40 ms
		LCD_WriteCommand(lcd, 0x30);
		DELAY(5); //wait >4.1 ms
		LCD_WriteCommand(lcd, 0x30);
		DELAY(1); //wait >100 us
		LCD_WriteCommand(lcd, 0x30);
		DELAY(1);
		LCD_WriteCommand(lcd, COMMAND_FUNCTION_SET | OPT_DL | OPT_N); //2 lines, 8 bit data length
		LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B); //turn on display, cursos and cursor blink
		LCD_WriteCommand(lcd, COMMAND_CLEAR_DISPLAY);
		LCD_WriteCommand(lcd, COMMAND_ENTRY_MODE_SET | OPT_INC); // Increment cursor
	}
}

void LCD_PinInit(LCD_HandleTypeDef * lcd) {
	LCD_Enable_GPIO_RCC(lcd);

	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = lcd->EN_pin;
	HAL_GPIO_Init(lcd->EN_port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(lcd->EN_port, lcd->EN_pin, RESET);

	GPIO_InitStruct.Pin = lcd->RS_pin;
	HAL_GPIO_Init(lcd->RS_port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, RESET);

	GPIO_InitStruct.Pin = lcd->RW_pin;
	HAL_GPIO_Init(lcd->RW_port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, RESET);

	if(lcd->Mode == LCD_MODE_4BITS) {
		GPIO_InitStruct.Pin = lcd->DB4_pin;
		HAL_GPIO_Init(lcd->DB4_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB5_pin;
		HAL_GPIO_Init(lcd->DB5_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB6_pin;
		HAL_GPIO_Init(lcd->DB6_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB7_pin;
		HAL_GPIO_Init(lcd->DB7_port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(lcd->DB4_port, lcd->DB4_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB5_port, lcd->DB5_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB6_port, lcd->DB6_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB7_port, lcd->DB7_pin, RESET);
	} else {
		GPIO_InitStruct.Pin = lcd->DB0_pin;
		HAL_GPIO_Init(lcd->DB0_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB1_pin;
		HAL_GPIO_Init(lcd->DB1_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB2_pin;
		HAL_GPIO_Init(lcd->DB2_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB3_pin;
		HAL_GPIO_Init(lcd->DB3_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB4_pin;
		HAL_GPIO_Init(lcd->DB4_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB5_pin;
		HAL_GPIO_Init(lcd->DB5_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB6_pin;
		HAL_GPIO_Init(lcd->DB6_port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = lcd->DB7_pin;
		HAL_GPIO_Init(lcd->DB7_port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(lcd->DB0_port, lcd->DB0_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB1_port, lcd->DB1_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB2_port, lcd->DB2_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB3_port, lcd->DB3_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB4_port, lcd->DB4_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB5_port, lcd->DB5_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB6_port, lcd->DB6_pin, RESET);
		HAL_GPIO_WritePin(lcd->DB7_port, lcd->DB7_pin, RESET);
	}

}


void LCD_Enable_GPIO_RCC(LCD_HandleTypeDef * lcd) {
	GPIO_TypeDef *CurrentGPIO;

	CurrentGPIO = GPIOA;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
	CurrentGPIO = GPIOB;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	CurrentGPIO = GPIOC;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}
	CurrentGPIO = GPIOD;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
	CurrentGPIO = GPIOE;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOE_CLK_ENABLE();
	}
	CurrentGPIO = GPIOF;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOF_CLK_ENABLE();
	}
	CurrentGPIO = GPIOG;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOG_CLK_ENABLE();
	}
	CurrentGPIO = GPIOH;
	if(isCurrentGPIOUsed){
		__HAL_RCC_GPIOH_CLK_ENABLE();
	}

}

//Prints to current cursor position
void LCD_Printf(LCD_HandleTypeDef * lcd, const char* str, ...)
{
	char stringArray[80];
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);
	for(uint8_t i=0;  i<strlen(stringArray) && i<80; i++)
	{
		LCD_WriteData(lcd, (uint8_t)stringArray[i]);
	}
}

void LCD_PrintTo1stLine(LCD_HandleTypeDef * lcd, const char* str, ...)
{
	char stringArray[20];
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);

	LCD_SetCursorPosition(lcd, 0, 0);
	for(uint8_t i=0;  i<strlen(stringArray) && i<20; i++)
	{
		LCD_WriteData(lcd, (uint8_t)stringArray[i]);
	}
}

void LCD_PrintTo2ndLine(LCD_HandleTypeDef * lcd, const char* str, ...)
{
	char stringArray[20];
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);

	LCD_SetCursorPosition(lcd, 1, 0);
	for(uint8_t i=0;  i<strlen(stringArray) && i<20; i++)
	{
		LCD_WriteData(lcd, (uint8_t)stringArray[i]);
	}
}

void LCD_PrintTo3rdLine(LCD_HandleTypeDef * lcd, const char* str, ...)
{
	char stringArray[20];
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);

	LCD_SetCursorPosition(lcd, 2, 0);
	for(uint8_t i=0;  i<strlen(stringArray) && i<20; i++)
	{
		LCD_WriteData(lcd, (uint8_t)stringArray[i]);
	}
}

void LCD_PrintTo4thLine(LCD_HandleTypeDef * lcd, const char* str, ...)
{
	char stringArray[20];
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);

	LCD_SetCursorPosition(lcd, 3, 0);
	for(uint8_t i=0;  i<strlen(stringArray) && i<20; i++)
	{
		LCD_WriteData(lcd, (uint8_t)stringArray[i]);
	}
}


/*
 * CursorMode may take any of the following values:
 * LCD_CURSOR_OFF
 * LCD_CURSOR_ON_BLINK_OFF
 * LCD_CURSOR_ON_BLINK_ON
 */
void LCD_SetCursorMode(LCD_HandleTypeDef * lcd, uint8_t CursorMode)
{
  if(CursorMode == LCD_CURSOR_ON_BLINK_ON){
	  LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);
	  lcd->CursorMode = LCD_CURSOR_ON_BLINK_ON;
  }
  else if(CursorMode == LCD_CURSOR_ON_BLINK_OFF){
	  LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C);
	  lcd->CursorMode = LCD_CURSOR_ON_BLINK_OFF;
  }
  else if(CursorMode == LCD_CURSOR_OFF){
	  LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D);
	  lcd->CursorMode = LCD_CURSOR_OFF;
  }
}

void LCD_SetCursorPosition(LCD_HandleTypeDef * lcd, uint8_t row, uint8_t col)
{
	LCD_WriteCommand(lcd, COMMAND_SET_DDRAM_ADDR + ROW_20[row] + col);
}


void LCD_Clear(LCD_HandleTypeDef * lcd){
	LCD_WriteCommand(lcd, COMMAND_CLEAR_DISPLAY);
	DELAY(1);
}

/**
 * Write a byte to the command register
 */
void LCD_WriteCommand(LCD_HandleTypeDef * lcd, uint8_t command)
{
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, LCD_RS_COMMAND);			// Write to command register

	if(lcd->Mode == LCD_MODE_4BITS)
	{
		LCD_Write(lcd, command >> 4);
		LCD_Write(lcd, command & 0x0F);
		DELAY(1);
	}
	else
	{
		LCD_Write(lcd, command);
	}

}

/**
 * Write a byte to the data register
 */
void LCD_WriteData(LCD_HandleTypeDef * lcd, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, LCD_RS_DATA);			// Write to data register

	if(lcd->Mode == LCD_MODE_4BITS)
	{
		LCD_Write(lcd, data >> 4);
		LCD_Write(lcd, data & 0x0F);
	}
	else
	{
		LCD_Write(lcd, data);
	}

}


/**
 * Set len bits on the bus and toggle the enable line
 */
void LCD_Write(LCD_HandleTypeDef *lcd, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->RW_port, lcd->RW_pin, LCD_RW_WRITE);			// Write
	HAL_GPIO_WritePin(lcd->EN_port, lcd->EN_pin, 1);
	DELAY(1);

	if(lcd->Mode == LCD_MODE_4BITS) {
		HAL_GPIO_WritePin(lcd->DB4_port, lcd->DB4_pin, (data >> 0) & 0x01);
		HAL_GPIO_WritePin(lcd->DB5_port, lcd->DB5_pin, (data >> 1) & 0x01);
		HAL_GPIO_WritePin(lcd->DB6_port, lcd->DB6_pin, (data >> 2) & 0x01);
		HAL_GPIO_WritePin(lcd->DB7_port, lcd->DB7_pin, (data >> 3) & 0x01);
	}else {
		HAL_GPIO_WritePin(lcd->DB0_port, lcd->DB0_pin, (data >> 0) & 0x01);
		HAL_GPIO_WritePin(lcd->DB1_port, lcd->DB1_pin, (data >> 1) & 0x01);
		HAL_GPIO_WritePin(lcd->DB2_port, lcd->DB2_pin, (data >> 2) & 0x01);
		HAL_GPIO_WritePin(lcd->DB3_port, lcd->DB3_pin, (data >> 3) & 0x01);
		HAL_GPIO_WritePin(lcd->DB4_port, lcd->DB4_pin, (data >> 4) & 0x01);
		HAL_GPIO_WritePin(lcd->DB5_port, lcd->DB5_pin, (data >> 5) & 0x01);
		HAL_GPIO_WritePin(lcd->DB6_port, lcd->DB6_pin, (data >> 6) & 0x01);
		HAL_GPIO_WritePin(lcd->DB7_port, lcd->DB7_pin, (data >> 7) & 0x01);
	}


	HAL_GPIO_WritePin(lcd->EN_port, lcd->EN_pin, 0);
	DELAY(1);
}
