/*
 * Bootloader.c
 *
 *  Created on: Aug 11, 2021
 *      Author: Olaf
 */
#include "main.h"
#include "can.h"
#include "crc.h"
#include "usart.h"
#include "gpio.h"
#include "SystemTruckBL_main.h"

#define SYSTEM_TRUCK_APP_SECTOR           FLASH_SECTOR2_BASE_ADDRESS

void SystemTruckBL_main(void){

  /**< Lets check whether button is pressed or not, if pressed jump to user application */
  if ( HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin) == GPIO_PIN_RESET )
  {
	  bootloader_jump_to_user_app();

  }

  printmsg("BL_DEBUG_MSG:Button is not pressed .. executing user app\n");
	while(1){
		printmsg("Hello From Bootloader\n");
		HAL_Delay(1000);



	}

}


/**<code to jump to user application
 *Here we are assuming FLASH_SECTOR2_BASE_ADDRESS
 *is where the user application is stored
 */
void bootloader_jump_to_user_app(void)
{
   /**<just a function pointer to hold the address of the reset handler of the user app.*/
    void (*app_reset_handler)(void);
    printmsg("BL_DEBUG_MSG:bootloader_jump_to_user_app\n");

    /**< 1. configure the MSP by reading the value from the base address of the sector 2*/
    uint32_t msp_value = *(volatile uint32_t *)SYSTEM_TRUCK_APP_SECTOR;
    printmsg("BL_DEBUG_MSG:MSP value : %#x\n",msp_value);

    //This function comes from CMSIS.
    __set_MSP(msp_value);

    //SCB->VTOR = FLASH_SECTOR1_BASE_ADDRESS;

    /* 2. Now fetch the reset handler address of the user application
     * from the location FLASH_SECTOR2_BASE_ADDRESS+4
     */
    uint32_t resethandler_address = *(volatile uint32_t *) (SYSTEM_TRUCK_APP_SECTOR + 4);

    app_reset_handler = (void*) resethandler_address;

    printmsg("BL_DEBUG_MSG: app reset handler addr : %#x\n",app_reset_handler);

    //3. jump to reset handler of the user application
    app_reset_handler();

}



