/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : main
 ******************************************************************************
 * HAL -> Hadware Abstraction Layer
 *
 * Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo entrada
 * y salida de datos digitales
 *
 * Caracteristicas Configurables de cada PIN
 *  - Velocidad
 *  - Tipo de entrada (pull up, pull down, open drain)
 *  - Tipo de salida (push-pull, opena drain ...)
 *  - Entrada análoga
 ******************************************************************************
 */

#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

//Función principal
int main(void){

	//Definir Handler para el PIN
	GPIO_Handler_t handlerUserLedPin = {0};

	//Puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Caragar configuración
	GPIO_Config(&handlerUserLedPin);

	//Encender PIN_A5
	GPIO_WritePin(&handlerUserLedPin, SET);

	while(1){
		NOP();
	}




}
