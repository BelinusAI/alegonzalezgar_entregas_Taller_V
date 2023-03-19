/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"

//Definición de elementos
GPIO_Handler_t handlerLed2 = {0}; //PA5

int main(void){

	//Configurando el Led2 -> PA5
	handlerLed2.pGPIOx							= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerLed2.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;


	//Cargar configuración del pin
	GPIO_Config(&handlerLed2);

	GPIO_WritePin(&handlerLed2, SET);
    /* Loop forever */
	while(1){
		GPIOxTooglePin(&handlerLed2);

		for(int i = 0; i < 2000000; i++){
			__NOP();
		}
	}

}
