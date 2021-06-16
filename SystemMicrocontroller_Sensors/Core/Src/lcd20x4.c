/*
 * lcd20x4.c
 *
 *  Created on: May 15, 2021
 *      Author: jdrqu
 */

#include "lcd20x4.h"

const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54}; //address of the first position of each row for a 20x4 LCD

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

//Private functions
static void LCD_WriteCommand(LCD_HandleTypeDef * lcd, uint8_t command);
static void LCD_WriteData(LCD_HandleTypeDef * lcd, uint8_t data);
static void LCD_Write(LCD_HandleTypeDef *lcd, uint8_t data);


/**
  * @brief  Initializes the GPIOx peripherals in the LCD_Init structure, then sends start sequence to LCD.
  * @param  lcd - LCD_HandleTypeDef structure specifies GPIO pins and LCD mode
  */
void LCD_Init(LCD_HandleTypeDef * lcd){
	LCD_PinInit(lcd);
	DELAY(5);
	if(LCD_MODE_4BITS == lcd->Mode)
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

void LCD_GetDefaultConfig(LCD_HandleTypeDef* hlcd){

	hlcd->Mode = LCD_MODE_4BITS;
	hlcd->CursorMode = LCD_CURSOR_ON_BLINK_ON;

	hlcd->RS_port = GPIOE;
	hlcd->RS_pin = GPIO_PIN_7;

	hlcd->RW_port = GPIOE;
	hlcd->RW_pin = GPIO_PIN_9;

	hlcd->EN_port = GPIOE;
	hlcd->EN_pin = GPIO_PIN_11;

	/* Pins not used in 4 bit mode
	hlcd->DB0_port = GPIOE;
	hlcd->DB0_pin = GPIO_PIN_10;
	hlcd->DB1_port = GPIOE;
	hlcd->DB1_pin = GPIO_PIN_12;
	hlcd->DB2_port = GPIOE;
	hlcd->DB2_pin = GPIO_PIN_14;
	hlcd->DB3_port = GPIOD;
	hlcd->DB3_pin = GPIO_PIN_8;
	*/

	hlcd->DB4_port = GPIOE;
	hlcd->DB4_pin = GPIO_PIN_15;
	hlcd->DB5_port = GPIOB;
	hlcd->DB5_pin = GPIO_PIN_11;
	hlcd->DB6_port = GPIOB;
	hlcd->DB6_pin = GPIO_PIN_13;
	hlcd->DB7_port = GPIOB;
	hlcd->DB7_pin = GPIO_PIN_15;
}


/**
  * @brief  Initializes the GPIOx peripherals in the provided LCD_Init structure.
  * @param  lcd - LCD_HandleTypeDef structure specifies GPIO pins
  */
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

	if(LCD_MODE_4BITS == lcd->Mode) {
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

/**
  * @brief  Initializes the GPIOx RCC in the provided LCD_Init structure.
  * @param  lcd - LCD_HandleTypeDef structure specifies GPIO pins
  */
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


/**
  * @brief  Prints to current cursor position
  * @param  lcd - LCD_HandleTypeDef structure specifies GPIO pins
  * @param  str - compatible with printf style formatting
  */
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

/**
  * @brief  sets cursor position
  * @param  line - This parameter can be any value of LCD_LineSelect_TypeDef
  * @param  col - Between 0 and 19 for 20 character LCD
  */
void LCD_SetCursorPosition(LCD_HandleTypeDef * lcd, LCD_LineSelect_TypeDef line, uint8_t col)
{
	LCD_WriteCommand(lcd, COMMAND_SET_DDRAM_ADDR + ROW_20[line] + col);
}

/**
  * @brief  prints message to a specific line
  * @param  line - This parameter can be any value of LCD_LineSelect_TypeDef
  * @param  str - compatible with printf style formatting
  */
void LCD_PrintToLine(LCD_HandleTypeDef * lcd, LCD_LineSelect_TypeDef line, const char* str, ...)
{
	char stringArray[20];
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);

	LCD_SetCursorPosition(lcd, line, 0);
	for(uint8_t i=0;  i<strlen(stringArray) && i<20; i++)
	{
		LCD_WriteData(lcd, (uint8_t)stringArray[i]);
	}
}


/**
  * @brief  controls cursor mode
  * @param  CursorMode - check LCD_CursorMode_TypeDef for available modes
  */
void LCD_SetCursorMode(LCD_HandleTypeDef * lcd, LCD_CursorMode_TypeDef CursorMode)
{
  if(LCD_CURSOR_ON_BLINK_ON == CursorMode){
	  LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);
	  lcd->CursorMode = LCD_CURSOR_ON_BLINK_ON;
  }
  else if(LCD_CURSOR_ON_BLINK_OFF == CursorMode){
	  LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C);
	  lcd->CursorMode = LCD_CURSOR_ON_BLINK_OFF;
  }
  else if(LCD_CURSOR_OFF == CursorMode){
	  LCD_WriteCommand(lcd, COMMAND_DISPLAY_ON_OFF_CONTROL | OPT_D);
	  lcd->CursorMode = LCD_CURSOR_OFF;
  }
}



/**
  * @brief  clear current LCD message
  * @param  lcd - pointer to LCD handler
  */
void LCD_Clear(LCD_HandleTypeDef * lcd){
	LCD_WriteCommand(lcd, COMMAND_CLEAR_DISPLAY);
	DELAY(1);
}

/**
 * @brief Write a byte to the command register
 */
static void LCD_WriteCommand(LCD_HandleTypeDef * lcd, uint8_t command)
{
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, LCD_RS_COMMAND);			// Write to command register

	if(LCD_MODE_4BITS == lcd->Mode)
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
 * @brief Write a byte to the data register
 */
void LCD_WriteData(LCD_HandleTypeDef * lcd, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->RS_port, lcd->RS_pin, LCD_RS_DATA);			// Write to data register
	
	if(LCD_MODE_4BITS == lcd->Mode)
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
 * @brief Set len bits on the bus and toggle the enable line
 */
void LCD_Write(LCD_HandleTypeDef *lcd, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->RW_port, lcd->RW_pin, LCD_RW_WRITE);			// Write
	HAL_GPIO_WritePin(lcd->EN_port, lcd->EN_pin, 1);
	DELAY(1);

	if(LCD_MODE_4BITS == lcd->Mode) {
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
